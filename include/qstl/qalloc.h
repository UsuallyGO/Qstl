
#ifndef _QSTL_ALLOC_
#define _QSTL_ALLOC_

#include <stddef.h> //for size_t
#include <stdlib.h>

#ifdef _USE_MALLOC
#undef _USE_MALLOC
#endif

typedef void (*Malloc_Handler)();

template<int inst>
class _qalloc{
public:
    static void* allocate(size_t size);
    static void* reallocate(void* p, size_t size);
    static void* handler_allocate(size_t size);
    static void* handler_reallocate(void *p, size_t size);
    static void  deallocate(void* p, size_t size);
    static void  set_handler(Malloc_Handler handler)
    {  mem_handler = handler;  }

private:
    static Malloc_Handler mem_handler;
};

template<int inst>
Malloc_Handler _qalloc<inst>::mem_handler = NULL;

template<int inst>
void* _qalloc<inst>::allocate(size_t size)
{
    void *p = malloc(size);
    if(NULL == p) p = handler_allocate(size);
    return p;
}

template<int inst>
void* _qalloc<inst>::reallocate(void *p, size_t new_size)
{
    void *result = realloc(p, new_size);
    if(NULL == result) result = handler_reallocate(p, new_size);
    return result;
}

template<int inst>
void* _qalloc<inst>::handler_allocate(size_t size)
{
    void *p = NULL;

    if(NULL == mem_handler) return NULL;
    for(;;){
       mem_handler();
       p = malloc(size);
       if(p) return p;
    }
}

template<int inst>
void* _qalloc<inst>::handler_reallocate(void *p, size_t new_size)
{
    void *result;

    if(NULL == mem_handler) return NULL;
    for(;;){
        mem_handler();
        result = realloc(p, new_size);
        if(result) return result;
    }
}

template<int inst>
void _qalloc<inst>::deallocate(void *p, size_t /*size*/)
{
    free(p);
}

template<class T, int inst>
class QSimple_alloc{
    typedef _qalloc<inst> allocator;
public:
    static T* allocate();
    static T* allocate(size_t num);
    static T* realloc(T* p, size_t new_num);
    static void deallocate(T* p, size_t size);

private:
    static const size_t value_size;
};

template<class T, int inst>
const size_t QSimple_alloc<T, inst>::value_size = sizeof(T);

template<class T, int inst>
T* QSimple_alloc<T, inst>::allocate()
{
    T *p = (T*)allocator::allocate(value_size);
    return p;
}

template<class T, int inst>
T* QSimple_alloc<T, inst>::allocate(size_t num)
{
    T *p = (T*)allocator::allocate(value_size*num);
    return p;
}

template<class T, int inst>
T* QSimple_alloc<T, inst>::realloc(T *p, size_t new_num)
{
    T *result = (T*)allocator::reallocate(p, value_size*new_num);
    return result;
}

template<class T, int inst>
void QSimple_alloc<T, inst>::deallocate(T *p, size_t size)
{
    allocator::deallocate(p, size);
}

#define MEMORY_ALLOC_BOUND 128
#define MEMORY_ALLOC_BASE  8

template<bool parallel, int inst>
class QAdvanced_alloc{
    typedef _qalloc<inst> allocator;
public:
    static void* allocate(size_t size);
    static void* allocate(size_t size, size_t num);
    static void* reallocate(void* p, size_t new_size);
    static void  deallocate(void *p, size_t size);
    static void* refill(size_t size);
    static void* chunk_alloc(size_t size, int& num);
private:
    static size_t ROUND_UP(size_t n){
        if(n == 0) n = 1;
        n = (n + MEMORY_ALLOC_BASE - 1) & (~(MEMORY_ALLOC_BASE - 1));
        return n;
    }
    static size_t FREE_INDEX(size_t size){
        size_t index = ROUND_UP(size);
        index = (index / MEMORY_ALLOC_BASE) - 1;
        return index;
    }
    union freeObject{
        union freeObject *next;
        char buf[1];
    };

    static char* start_address;
    static char* end_address;
    static size_t  history_size;
    static freeObject* free_list[MEMORY_ALLOC_BOUND/MEMORY_ALLOC_BASE];//not array of pointer
};

template<bool parallel, int inst>
char* QAdvanced_alloc<parallel, inst>::start_address = 0;

template<bool parallel, int inst>
char* QAdvanced_alloc<parallel, inst>::end_address = 0;

template<bool parallel, int inst>
size_t QAdvanced_alloc<parallel, inst>::history_size = 0;

template<bool parallel, int inst>
typename QAdvanced_alloc<parallel, inst>::freeObject*
QAdvanced_alloc<parallel, inst>::free_list[] = {0};

template<bool parallel, int inst>
void* QAdvanced_alloc<parallel, inst>::allocate(size_t size)
{
    if(size > MEMORY_ALLOC_BOUND){
        return allocator::allocate(size);
    }else{
        freeObject* freeobj = free_list[FREE_INDEX(size)];
        if(NULL == freeobj){
            void* result = refill(ROUND_UP(size));
            return result;
        }
        char* result = freeobj->buf;
        free_list[FREE_INDEX(size)] = freeobj->next;
        return result;
    }
}

template<bool parallel, int inst>
void* QAdvanced_alloc<parallel, inst>::chunk_alloc(size_t size, int& nobjs)
{
    size_t bytes_left = (size_t)end_address - (size_t)start_address;
    size_t bytes_alloc = size * nobjs;

    if(bytes_left > bytes_alloc){
        char *result = start_address;
        start_address += bytes_alloc;
        return result;
    }else if(bytes_left > size){
        nobjs = bytes_left / size;//by reference
        char *result = start_address;
        start_address += size*nobjs;
        return result;
    }else{//bytes_left less than size, set bytes_left to free_list, alloc new buffer
        if(NULL != start_address){
            ((freeObject*)start_address)->next = free_list[FREE_INDEX(bytes_left)];
            free_list[FREE_INDEX(bytes_left)] = (freeObject*)start_address;
        }

        size_t bytes_new = bytes_alloc*2 + ROUND_UP(history_size>>4);
        start_address = (char*)malloc(bytes_new);
        if(NULL == start_address){
            for(size_t i_size = size; i_size < MEMORY_ALLOC_BOUND; i_size += MEMORY_ALLOC_BASE){
                freeObject *result = free_list[FREE_INDEX(i_size)];
                if(NULL != result){
                    free_list[FREE_INDEX(i_size)] = result->next;
                    start_address = (char*)result;
                    end_address = start_address + i_size;
                    return chunk_alloc(size, nobjs);
                }
            }
            start_address = (char*)allocator::allocate(bytes_new);
        }
        history_size += bytes_new;
        end_address = start_address + bytes_new;
        return chunk_alloc(size, nobjs);
    }
}

template<bool parallel, int inst>
void* QAdvanced_alloc<parallel, inst>::refill(size_t size)
{
    int nobjs = 20; //default to allocate 20 objs
    char* result = (char*)chunk_alloc(size, nobjs);//nobjs is passed by reference

    if(1 == nobjs) return result;

    freeObject *prev_obj, *current;
    prev_obj = free_list[FREE_INDEX(size)];
    for(int i = 1; i < nobjs; i++){
        current = (freeObject*)((size_t)result + i*size);
        current->next = prev_obj;
        prev_obj = current;
    }
    free_list[FREE_INDEX(size)] = prev_obj;
    return result;
}

template<bool parallel, int inst>
void QAdvanced_alloc<parallel, inst>::deallocate(void *p, size_t size)
{
    if(size > MEMORY_ALLOC_BOUND){
        allocator::deallocate(p, size);
    }
    else{
        ((freeObject*)p)->next = free_list[FREE_INDEX(size)];
        free_list[FREE_INDEX(size)] = (freeObject*)p;
    }
}

//typedef QAdvanced_alloc<false, 0> QAdvanced_alloc;

#endif//!_QSTL_ALLOC_
