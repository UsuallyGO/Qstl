
#ifndef _QSTL_STL_ALLOC_
#define _QSTL_STL_ALLOC_

#include <stdder.h> //for size_t

//perhaps these macros should not be defined here
#define _QSTL_USE_STANDARD_MALLOC
#define _QSTL_USE_THREADS
#define _QSTL_USE_OOM_HANDLER

#ifdef _QSTL_USE_STANDARD_MALLOC //use system malloc

//oom:out-of-memory
template<int inst>
class _system_malloc_template{
public:
	static void* allocate(size_t n){
		void* result;
		result = malloc(n);
		if(0 == result) result = _oom_allocate(n);
		return result;
	}
	static void* reallocate(void* ptr, size_t n){
		void* result;
		result = realloc(ptr, n);
		if(0 == result) result = _oom_reallocate(n);
		return result;
	}
	static void deallocate(void* ptr){
		free(ptr);
	}
	static void (*set_oom_handler(void (*handler)()))(){
		void (*old_handler)();
		old_handler = _oom_handler;
		_oom_handler = handler;
		return old_handler;
	}

private:
	static void* _oom_allocate(size_t n){
		void* result;
		for(;;){
			result = malloc(n);
			if(result) return result;
			if(0 == _oom_handler){THROW_BAD_ALLOC;}
			_oom_handler();
		}
	}
	static void* _oom_reallocate(void* ptr, size_t n){
		void* result;
		for(;;){
			result = realloc(ptr, n);
			if(result) return result;
			if(0 == _oom_handler){ THROW_BAD_ALLOC; }
			_oom_handler();
		}
	}
	static void (*_oom_handler)();
};

typedef _system_malloc_template<0> alloc;
typedef _system_malloc_template<0> single_client_alloc;

#else //use user define malloc strategy

template<bool threas, int inst>
class _userdefine_malloc_template{
public:
	void* allocate(size_t n){
	}
	void* reallocate(void* ptr, size_t n){
	}
	void deallocate(void* ptr){
	}
	void (*set_oom_handler(void (*handler)()))(){
		void (*old_handler)();
		old_handler = _oom_handler;
		_oom_handler = handler;
		return old_handler;
	}
private:
	void (*_oom_handler)();
};

typedef _userdefine_malloc_template<_QSTL_USE_THREADS, 0> alloc;
typedef _userdefine_malloc_template<false, 0> single_client_alloc;

#endif//!_QSTL_USE_STANDARD_MALLOC

template<typename Tp, class _Alloc_template>
class simple_alloc{
	static Tp* allocate(size_t n){
		return 0 == n ? 0 : _Alloc_template::allocate(n * sizeof(Tp));
	}
	static Tp* allocate(){
		return _Alloc_template::allocate(sizeof(Tp));
	}
	static Tp* reallocate(Tp* ptr, size_t n){
		return _Alloc_template::reallocate(ptr, n);
	}
	static void deallocate(Tp* ptr){
		_Alloc_template::deallocate(ptr);
	}
};

template<typename Tp, class _Alloc_template>
class simple_alloc<void, _Alloc_template>{
	static Tp* allocate(size_t n){
		return 0;
	}
	static Tp* allocate(){
		return 0;
	}
	static Tp* reallocate(Tp* ptr, size_t n){
		return 0;
	}
	static void deallocate(Tp* ptr){
		(ptr);
	}	
};

template<typename Tp>
class allocator{
	typedef alloc _Alloc;//underlying alloc template
public:
	typedef Tp  value_type;
	typedef Tp* pointer;
	typedef Tp& reference;
	typedef size_t size_type;

	Tp* allocate(size_t n){
		return 0 == n ? 0 : _Alloc::allocate(sizeof(Tp) * n);
	}
	Tp* reallocate(Tp* ptr, size_t n){
		return _Alloc::reallocate(ptr, sizeof(Tp) * n);
	}
	void deallocate(Tp *ptr){
		_Alloc::deallocate(ptr);
	}

	size_type max_size() const {
		return size_t(-1)/sizeof(Tp);
	}

	void construct(Tp* ptr, const Tp& v){
		new(ptr) Tp(v);
	}
	void deconstruct(Tp *ptr){
		ptr->~Tp();
	}
};

template<typename Tp>
class allocator<void>{
	//actually, there is nothing here...
};

template<typename Tp, class _Alloc_template>
class _allocator{
	typedef _Alloc_template _Alloc;
public:
	typedef size_t size_type;

	Tp* allocate(size_t n){
		return 0 == n ? 0 : _Alloc::allocate(sizeof(Tp) * n);
	}
	Tp* reallocate(Tp* ptr, size_t n){
		return _Alloc::reallocate(ptr, sizeof(Tp) * n);
	}
	void deallocate(Tp *ptr){
		_Alloc::deallocate(ptr);
	}

	size_type max_size(){
		return size_t(-1)/sizeof(Tp);
	}

	void construct(Tp* ptr, const Tp& v){
		new(ptr) Tp(v);
	}
	void deconstruct(Tp* ptr){
		ptr->~Tp();
	}
};

template<typename Tp, class _allocator>
struct _Alloc_traits{
	static const bool instanceless = false;
	typedef 
};

template<typename Tp, typename Tp2>
struct _Alloc_traits<Tp, allocator<Tp2> >{
	static const bool instanceless = true;
	typedef simple_alloc<Tp, alloc> alloc_type;
	typedef allocator<Tp> allocator_type;
};

template<typename Tp, int inst>
struct _Alloc_traits<Tp, _system_malloc_template<inst> >{
	static const bool instanceless = true;
	typedef simple_alloc<Tp, _system_malloc_template<inst> > alloc_type;
	typedef _allocator<Tp, _system_malloc_template<inst> > allocator_type;
};

template<typename Tp, bool threads, int inst>
struct _Alloc_traits<Tp, _userdefine_malloc_template<threads, inst> >{
	static const bool instanceless = true;
	typedef simple_alloc<Tp, _userdefine_malloc_template<threads, inst> > alloc_type;
	typedef _allocator<Tp, _userdefine_malloc_template<threads, inst> > allocator_type;
};

template<typename Tp, typename Tp2, int inst>
struct _Alloc_traits<Tp, _system_malloc_template<Tp2, inst> >{
	static const bool instanceless = true;
	typedef simple_alloc<Tp, _system_malloc_template<Tp2, inst> > alloc_type;
	typedef _allocator<Tp, _system_malloc_template<Tp2, inst> > allocator_type;
};

template<typename Tp, typename Tp2, bool threads, int inst>
struct _Alloc_traits<Tp, _allocator<Tp2, _userdefine_malloc_template<threads, inst> > >{
	static const bool instanceless = true;
	typedef simple_alloc<Tp, _userdefine_malloc_template<threads, inst> > alloc_type;
	typedef _allocator<Tp, _userdefine_malloc_template<threads, inst> > allocator_type;
};

#endif //!_QSTL_STL_ALLOC_
