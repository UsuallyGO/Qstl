
#ifndef _QSTL_STL_LIST_
#define _QSTL_STL_LIST_

struct _List_node_base{
	_List_node_base _M_next;
	_List_node_base _M_prev;
};

template<typename _Tp>
struct _List_node : public _List_node_base{
	_Tp _M_data;
};

struct _List_iterator_base{
	_List_node_base *_Node;

	_List_iterator_base():_Node(0){}
	_List_iterator_base(_List_node_base* __x):_Node(__x){}

	void _M_incr(){ _Node = _Node->_M_next; }
	void _M_decr(){ _Node = _Node->_M_prev; }

	bool operator==(const _List_iterator_base& __i){
		return __i._Node == _Node;
	}
	bool operator!=(const _List_iterator_base& __i){
		return __i._Node != _Node;
	}
};

template<typename _Tp, typename _Ref, typename _Ptr>
struct _List_iterator : public _List_iterator_base{
	typedef _List_iterator<_Tp, _Tp&, _Tp*> iterator;
	typedef _List_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;
	typedef _List_iterator<_Tp, _Ref, _Ptr> _Self;

	_List_iterator():_List_iterator_base(0){}
	_List_iterator(_List_node *__x):_List_iterator_base(__x){}
	_List_iterator(_List_iterator *__i):_List_iterator_base(__i->_Node){}

	typedef _Tp value_type;
	typedef _Ref reference;
	typedef _Ptr pointer;
	typedef _List_node<_Tp> _Node_type;

	_Self& operator++(){
		this->_M_incr();
		return *this;
	}

	_Self operator++(int){
		_Self __tmp = *this;
		this->_M_incr();
		return __tmp;
	}

	_Self& operator--(){
		this->_M_decr();
		return *this;
	}

	_Self operator--(int){
		_Self __tmp = *this;
		this->_M_decr();
		return __tmp;
	}

	reference operator*(){
		return ((_Node_type*)_Node)->_M_data;
	}
	pointer operator->(){
		return &(operator*());
	}
};

template<typename _Tp, typename _Alloc, bool _IsStatic>
class _List_alloc_base{
public:
	typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;
	allocator_type get_allocator()const { return _Node_allocator; }
	_List_alloc_base(const allocator_type& __a) : _Node_allocator(__a){}

protected:
	_List_node<_Tp>* _M_get_node(){
		return _Node_allocator.allocate(1);
	}
	void _M_put_node(_List_node<_Tp>* node){
		_Node_allocator.deallocate(node);
	}
private:
	allocator_type _Node_allocator;
	_List_node<_Tp>* _M_node;
};

template<typename _Tp, typename _Allocator>
class _List_alloc_base<_Tp, _Allocator, true>{
public:
	typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type allocator_type;
	allocator_type get_allocator() const {return _Node_allocator; }
	_List_alloc_base(const allocator_type&){}
protected:
	_List_node<_Tp>* _M_get_node(){
		return _Allocator::allocate(1);
	}
	void _M_put_node(_List_node<_Tp>* node){
		_Allocator::deallocate(node);
	}
protected:
	_List_node<_Tp>* _M_node;
};

template<typename _Tp, class _Alloc>
class _List_base : public _List_alloc_base<_Tp, _Alloc,
								_Alloc_traits<_Tp, _Alloc::_S_instanceless>>{
	~_List_bae(){
		clear();
		_M_put_node(_M_node);
	}
	void clear();
};

template<class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp)>
class list : protected _List_base<_Tp, _Allc>{
};

#endif //!_QSTL_STL_LIST_
