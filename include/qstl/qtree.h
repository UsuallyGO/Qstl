
#ifndef _QSTL_TREE_H_
#define _QSTL_TREE_H_

#include <string.h>
#include "qstl/qiterator.h"
#include "qstl/qalloc.h"
#include "qstl/qstl_alloc.h"

typedef bool _rb_tree_node_color_type;
const _rb_tree_node_color_type _rb_tree_red_color = true;
const _rb_tree_node_color_type _rb_tree_black_color = false;

struct _rb_tree_node_base{
	typedef _rb_tree_node_base* base_ptr;
	typedef _rb_tree_node_color_type color_type;

	base_ptr left;
	base_ptr right;
	base_ptr parent;
	color_type color;

	base_ptr minimum(base_ptr x){
		while(x->left != 0)
			x = x->left;
		return x;
	}

	base_ptr maximum(base_ptr x){
		while(x->right != 0)
			x = x->right;
		return x;
	}
};

template<class Value>
struct _rb_tree_node : public _rb_tree_node_base{
	typedef _rb_tree_node<Value>* link_type;
	Value value_field;
};

struct _rb_tree_iterator_base{
	typedef _rb_tree_node_base::base_ptr base_ptr;

	base_ptr node;

	void increment(){
		if(node->right == 0){
			if(node == node->parent->left)
				node = node->parent;
			else{
				base_ptr p = node->parent;
				while(p->right == node)
					node = p, p = p->parent;
				if(p->right != node)
					node = p;
			}
		}
		else{
			node = node->right;
			while(node->left) node = node->left;
		}
	}

	void decrement(){
		if(node->left == 0){
			if(node == node->parent->right)
				node = node->parent;
			else{
				base_ptr p = node->parent;
				while(p->left == node)
					node = p, p = p->parent;
				if(p->left != node)
					node = p;
			}
		}
		else{
			node = node->left;
			while(node->right) node = node->right;
		}
	}
};

template<class Value>
struct _rb_tree_iterator : public _rb_tree_iterator_base{
	typedef Value& reference;
	typedef const Value& const_reference;
	typedef _rb_tree_node<Value>* link_type;

	_rb_tree_iterator(){}
	_rb_tree_iterator(link_type x){
		node = x;
	}
	reference operator*() const {
		return link_type(node)->value_field;
	}
	_rb_tree_iterator operator++(){
		_rb_tree_iterator tmp = *this;
		this->increment();
		return tmp;
	}
	_rb_tree_iterator operator++(int){
		this->increment(); return *this;
	}
	_rb_tree_iterator operator--(){
		_rb_tree_iterator tmp = *this;
		this->decrement();
		return tmp;
	}
	_rb_tree_iterator operator--(int){
		this->derement(); return *this;
	}
};

template<class Value>
struct _rb_tree_const_iterator : public _rb_tree_iterator_base{
	typedef Value& reference;
	typedef const Value& const_reference;
	typedef _rb_tree_node<Value>* link_type;

	_rb_tree_const_iterator(){}
	_rb_tree_const_iterator(link_type x){
		node = x;
	}
	_rb_tree_const_iterator(const _rb_tree_iterator<Value>& i){
		node = i.node;
	}

	const_reference operator*() const {
		return link_type(node)->value_field;
	}
	_rb_tree_const_iterator operator++(){
		_rb_tree_const_iterator tmp = *this;
		this->increment();
		return tmp;
	}
	_rb_tree_const_iterator operator++(int){
		this->increment();
		return *this;
	}
	_rb_tree_const_iterator operator--(){
		_rb_tree_const_iterator tmp = *this;
		this->decrement();
		return tmp;
	}
	_rb_tree_const_iterator operator--(int){
		this->derement(); return *this;
	}

};

inline bool operator==(const _rb_tree_iterator_base& x, 
						const _rb_tree_iterator_base& y){
	return x.node == y.node;
}

inline bool operator!=(const _rb_tree_iterator_base& x,
						const _rb_tree_iterator_base& y){
	return x.node != y.node;
}

bidirectional_iterator_tag
Iterator_Category(const _rb_tree_iterator_base& i){
	return bidirectional_iterator_tag();
}

class _int_cmp{
public:
	int operator()(int x, int y) const {
		return x - y;
	}
};

template<class Key, class Value, /*class KeyOfValue,*/
	class Compare = _int_cmp, class Alloc = alloc>
class _rb_tree{
private:
	typedef Key key_type;
	typedef Value value_type;
	typedef _rb_tree_node<Value>  rb_tree_node;
	typedef _rb_tree_node<Value>* link_type;
	typedef _rb_tree_node_base* base_ptr;
	typedef QSimple_alloc<rb_tree_node, 0> allocator;
public:
	typedef size_t size_type;
	typedef Value& reference;
	typedef const Value& const_reference;
	typedef _rb_tree_iterator<Value> iterator;
	typedef _rb_tree_const_iterator<Value> const_iterator;

private:
	link_type get_node(){
		return (link_type)allocator::allocate();
	}
	void put_node(link_type x){
		allocator::deallocate(x);
	}
	link_type& root() const {
		return (link_type&)header->parent;
	}
	link_type _find(const Key& k)const;
	void _rb_insert_rebalance(base_ptr n);
	void _rb_erase_fixup(const Key& k);
	void _left_rotation(base_ptr node);
	void _right_rotation(base_ptr node);
	link_type _left_most() const {
		if(root() == 0) return header;

		base_ptr node = header->parent;
		while(node->left) node = node->left;
		return (link_type)node;
	}
	link_type _right_most() const {
		if(root() == 0) return header;

		base_ptr node = header->parent;
		while(node->right) node = node->right;
		return (link_type)node;
	}
	bool _rb_verify() const;
	int _black_nodes_to_root(base_ptr n) const;

public:
	_rb_tree(){
		header = get_node();
		header->left  = 0;
		header->right = 0;
		root() = 0;//header->parent = 0

		node_num = 0;
		key_cmp = Compare();
	}
	iterator find(const Key& k);//can't return reference directly, because may be 0
	const_iterator find(const Key& k) const;
	void insert(const Key& k);
	void erase(const Key& k);//delete is sensitive word in C++, so use erase
	iterator begin(){ return _left_most(); }
	const_iterator begin() const { return _left_most(); }
	iterator end(){ return (link_type)(header); }
	const_iterator end() const { return (link_type)(header); }
	int node_count() const { return node_num; }
	bool rb_verify()const { return _rb_verify(); }

private:
	link_type header;
	size_type node_num;
	Compare key_cmp;
};

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
typename _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::link_type
_rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::_find(const Key& k) const {
	link_type n = root();
	while(n != 0){
		int cmp = key_cmp(k, n->value_field);
		if(cmp == 0) break;
		n = cmp > 0 ? (link_type)n->right : (link_type)n->left;
	}
	return n;
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
typename _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::iterator
_rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::find(const Key& k){
	link_type n = _find(k);
	if(n == 0) return end();
	else return iterator(n);
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
typename _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::const_iterator
_rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::find(const Key& k) const {
	link_type n = _find(k);
	if(n == 0) return end();
	else return const_iterator(n);
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
void _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::erase(const Key& k){
	_rb_erase_fixup(k);
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
void _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::_rb_erase_fixup(const Key& k){
	link_type z;
	int cmp;

	z = root();
	while(z){
		cmp = key_cmp(k, z->value_field);
		if(cmp == 0) break;
		z = cmp > 0 ? (link_type)z->right : (link_type)z->left;
	}
	if(!z) return;

	node_num--;
	base_ptr y = z;
	base_ptr x = 0, x_p = 0;
	if(z->left == 0)
		x = z->right;//when z->left is 0, z->right has at most one child
	else if(z->right == 0)
		x = z->left;
	else{
		y = z->right;
		while(y->left) y = y->left;
		x = y->right;
	}
	if(z == y){
		if(z == root()){
			header->parent = x;
			if(x) x->parent = header;
		}
		else{
			if(z->parent->left == z) z->parent->left = x;
			else z->parent->right = x;
			if(x) x->parent = z->parent;
			x_p = z->parent;
		}
	}
	else if(z->right == y){
		y->left = z->left; //y can't be null, because y initialized by z
		if(z->left) z->left->parent = y;
		if(z->parent->left == z) z->parent->left = y;
		else if(z->parent->right == z) z->parent->right = y;
		else z->parent->parent = y;//z->parent is header
		y->parent = z->parent;
		x_p = y;
	}
	else{
		y->left = z->left;
		if(z->left) z->left->parent = y;
		y->right = z->right;
		if(z->right) z->right->parent = y;
		base_ptr y_p = y->parent;
		y_p->left = x;
		if(x) x->parent = y_p;
		x_p = y_p;
		if(z->parent->left == z) z->parent->left = y;
		else if(z->parent->right == z) z->parent->right = y;
		else z->parent->parent = y;//z->parent is header
		y->parent = z->parent;
	}
	cmp = z->color;//cmp just reused temporarily
	z->color = y->color;
	y->color = cmp;
	y = z;

	if(y->color != _rb_tree_red_color){
		while(x != root() && (!x || x->color == _rb_tree_black_color)){
			base_ptr w;
			if(x_p->left == x) w = x_p->right;
			else w = x_p->left;
			/*For erase fixup, x's brother w is the key, understand why it can't be 0 then
			 * you understand everything about erase fixup.
			 * Why w can't be 0 here? First of all, y can't be 0, details see above.
			 * If z == y, z->left and z->right both are 0. If originally z is root(), 
			 * the current root() must be x. However, only x != root() can come here. If
			 * z is not root(), then z must has a parent, because z is black, z's parent
			 * must has another non-0 black child. That child is the w here.
			 * If z->right == y, because y->color is black, z must has another black non-0
			 * black child. That child is the w here.
			 * If z->right != y, y must has a parent. Because y is black, y's parent must
			 * has another non-0 black child. That child is the w here.
			 * So, w can't be 0 here.
			 */
			if(w->color == _rb_tree_red_color){//case a
				x_p->color = _rb_tree_red_color;
				w->color = _rb_tree_black_color;
				if(w == x_p->right){
					_left_rotation((base_ptr)x_p);
					w = x_p->right;
				}
				else{
					_right_rotation((base_ptr)x_p);
					w = x_p->left;
				}
			}
			if((!w->left || w->left->color == _rb_tree_black_color) //case b
				&& (!w->right || w->right->color == _rb_tree_black_color)){
				w->color = _rb_tree_red_color;
				x = x_p, x_p = x->parent;
				continue;
			}
			if(w == x_p->right){
				if((!w->right || w->color == _rb_tree_black_color)//case c
					&& (w->left && w->left->color == _rb_tree_red_color)){
					w->left->color = _rb_tree_black_color;
					w->color = _rb_tree_red_color;
					_right_rotation(w);
					w = w->parent;
				}

				w->color = x_p->color;//case d
				x_p->color = _rb_tree_black_color;
				if(w->right) w->right->color = _rb_tree_black_color;
				_left_rotation(x_p);
				break;
			}
			else{
				if((!w->left || w->left->color == _rb_tree_black_color)//case c'
					&& (w->right && w->right->color == _rb_tree_red_color)){
					w->right->color = _rb_tree_black_color;
					w->color = _rb_tree_red_color;
					_left_rotation(w);
					w = w->parent;
				}

				w->color = x_p->color;//case d'
				x_p->color = _rb_tree_black_color;
				if(w->left) w->left->color = _rb_tree_black_color;
				_right_rotation(x_p);
				break;
			}
		}
		if(x) x->color = _rb_tree_black_color;
	}
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
void _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::insert(const Key& k){
	link_type n = root();
	link_type node = get_node();
	node->left = 0, node->right =0;
	node->color = _rb_tree_red_color;
	node->parent = n, node->value_field = k;

	if(n == 0){
		root() = node;
		node->parent = header;
		node->color = _rb_tree_black_color;
		node_num++;
		return;
	}

	int cmp;
	for(;;){
		cmp = key_cmp(k, n->value_field);
		if(cmp == 0) return;//duplicate values
		if(cmp < 0 && n->left) n = (link_type)n->left;
		else if(cmp > 0 && n->right) n = (link_type)n->right;
		else break;//n's left or right is NULL
	}

	if(cmp < 0) n->left = node;
	else n->right = node;
	node->parent = n;
	node_num++;

    base_ptr  base = (base_ptr)node;
	_rb_insert_rebalance(base);
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
void
_rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::_rb_insert_rebalance(base_ptr node){
	base_ptr p, pp, u;//parent, pparent, uncle

	p = node;
	for(;;){
		if(node->parent->color == _rb_tree_black_color) break;
		p = node->parent, pp = p->parent;
		if(p == pp->left) u = pp->right;
		else u = pp->left;

		/*To insert, uncle's color is the key*/
		if(u && u->color == _rb_tree_red_color){
			p->color = _rb_tree_black_color;
			u->color = _rb_tree_black_color;
			pp->color = _rb_tree_red_color;
			node = pp;
		}
		else{
			if(p == pp->left && node == p->right)
				node = p, _left_rotation(node);
			else if(p == pp->right && node == p->left)
				node = p, _right_rotation(node);
			else{
				p->color = _rb_tree_black_color;
				pp->color = _rb_tree_red_color;
				if(p == pp->left) _right_rotation(pp);
				else _left_rotation(pp);
			}
		}
	}

	root()->color = _rb_tree_black_color;
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
void
_rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::_left_rotation(base_ptr node){
	base_ptr s = node->right;
	base_ptr p = node->parent;

	if(p->left == node) p->left = s;
	else if(p->right == node) p->right = s;
	else p->parent = s;
	s->parent = p;//when s is null, left_rotation will never be triggered
	node->right = s->left;
	if(s->left) s->left->parent = node;
	s->left = node;
	node->parent = s;
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
void
_rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::_right_rotation(base_ptr node){
	base_ptr s = node->left;
	base_ptr p = node->parent;

	if(p->right == node) p->right = s;
	else if(p->left == node) p->left = s;
	else p->parent = s;
	s->parent = p;//when s is null, right_rotation will never be triggered
	node->left = s->right;
	if(s->right) s->right->parent = node;
	s->right = node;
	node->parent = s;
}

template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
int _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::
_black_nodes_to_root(base_ptr n) const{
	int num = 0;

	while(n && n != header){
		if(n->color == _rb_tree_black_color) num++;
		n = n->parent;
	}
	return num;
}

/* This was once a century problem to me: how to know the tree is legal or illegal 
 * reb-black tree? To be naive, I choose to print out the pre-visit, in-visit to 
 * check out the tree. But as you know, it is time waisted and very difficult to 
 * know whether the print out is right or not. You have to build the legal tree 
 * manually first! This time, the strategy has been changed. I learn the way how to
 * check out the red-black tree from STL.
 *
 * We need to check out these points to make sure the tree is legal or not:
 *  1) Root should be black
 *  2) Red node should be no red children
 *  3) The number of black nodes from root to each leaf(null node) are all the same
 *  4) The value of parent is greater than left child and less than right child.
 *  5) The minimum node should be leftmost, the maximum node should be rightmost.
 *
 * We can apply to check the first 4 points right now. Because there is no minimum
 * and maximum in current version of rb tree(2016-11-10).
 *
 * Return value: true means legal tree, false means illegal.
 */
template<class Key, class Value, /*class KeyOfValue,*/ class Compare, class Alloc>
bool _rb_tree<Key, Value, /*KeyOfValue,*/ Compare, Alloc>::_rb_verify() const {
	if(header->parent == 0 || node_num == 0)
		return !header->parent && !node_num;
	if(header->parent->color != _rb_tree_black_color)
		return false;

	link_type n = _left_most();
	int num = _black_nodes_to_root(n);
	for(const_iterator iter = begin(); iter != end(); iter++){
		link_type x = (link_type)iter.node;
		link_type l = (link_type)x->left;
		link_type r = (link_type)x->right;
		if(x->color == _rb_tree_red_color){
			if((l && l->color == _rb_tree_red_color)
				||(r && r->color == _rb_tree_red_color))
			return false;
		}

		if(l && key_cmp(x->value_field, l->value_field) <= 0) return 0;
		if(r && key_cmp(x->value_field, r->value_field) >= 0) return 0;

		if(!l && !r){
			int tmp = _black_nodes_to_root(x);
			if(tmp != num) return false;
		}
	}

	return true;
}

#endif //!_QSTL_TREE_H_
