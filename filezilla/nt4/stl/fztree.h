/* fztree.h: the AVL tree under map and set, for the NT 4.0 build of FileZilla (see
   fzstl.h). Nodes are never moved or copied, so erasing one leaves iterators to the
   others valid. KX gives a value's key: KX::key(v). Keys are ordered by operator<.
   Written for the NT 4.0 port. Licence: GPL v2 or later, as FileZilla. */
#ifndef FZSTL_TREE_H
#define FZSTL_TREE_H
#include "fzstl.h"

namespace std {

struct _TreeLink {
	_TreeLink *l, *r, *p;
	int h;
};

/* The shape of the tree: everything that doesn't depend on the type of the values. */
struct _TreeBase {
	_TreeLink *root;
	size_t n;

	static int _h(const _TreeLink *x) { return x ? x->h : 0; }
	static void _fix(_TreeLink *x)
	{
		int a = _h(x->l), b = _h(x->r);
		x->h = (a > b ? a : b) + 1;
	}
	static _TreeLink *_min(_TreeLink *x) { while (x->l) x = x->l; return x; }
	static _TreeLink *_max(_TreeLink *x) { while (x->r) x = x->r; return x; }
	static _TreeLink *_next(_TreeLink *x)
	{
		if (x->r)
			return _min(x->r);
		_TreeLink *p = x->p;
		while (p && x == p->r) {
			x = p;
			p = p->p;
		}
		return p;
	}
	_TreeLink *_prev(_TreeLink *x) const
	{
		if (!x)
			return root ? _max(root) : 0; // from end()
		if (x->l)
			return _max(x->l);
		_TreeLink *p = x->p;
		while (p && x == p->l) {
			x = p;
			p = p->p;
		}
		return p;
	}
	void _replace(_TreeLink *u, _TreeLink *v)
	{
		if (!u->p)
			root = v;
		else if (u == u->p->l)
			u->p->l = v;
		else
			u->p->r = v;
		if (v)
			v->p = u->p;
	}
	_TreeLink *_rotl(_TreeLink *x)
	{
		_TreeLink *y = x->r;
		x->r = y->l;
		if (y->l)
			y->l->p = x;
		_replace(x, y);
		y->l = x;
		x->p = y;
		_fix(x);
		_fix(y);
		return y;
	}
	_TreeLink *_rotr(_TreeLink *x)
	{
		_TreeLink *y = x->l;
		x->l = y->r;
		if (y->r)
			y->r->p = x;
		_replace(x, y);
		y->r = x;
		x->p = y;
		_fix(x);
		_fix(y);
		return y;
	}
	void _rebalance(_TreeLink *x)
	{
		while (x) {
			_fix(x);
			int bf = _h(x->l) - _h(x->r);
			if (bf > 1) {
				if (_h(x->l->l) < _h(x->l->r))
					_rotl(x->l);
				x = _rotr(x);
			} else if (bf < -1) {
				if (_h(x->r->r) < _h(x->r->l))
					_rotr(x->r);
				x = _rotl(x);
			}
			x = x->p;
		}
	}
	/* Link x in as the left (left != 0) or right child of parent (0: the root). */
	void _link(_TreeLink *x, _TreeLink *parent, int left)
	{
		x->l = x->r = 0;
		x->h = 1;
		x->p = parent;
		if (!parent)
			root = x;
		else if (left)
			parent->l = x;
		else
			parent->r = x;
		++n;
		_rebalance(parent);
	}
	/* Unlink z; the caller frees it. */
	void _unlink(_TreeLink *z)
	{
		_TreeLink *start;
		if (!z->l) {
			start = z->p;
			_replace(z, z->r);
		} else if (!z->r) {
			start = z->p;
			_replace(z, z->l);
		} else {
			_TreeLink *y = _min(z->r);
			if (y->p != z) {
				start = y->p;
				_replace(y, y->r);
				y->r = z->r;
				y->r->p = y;
			} else
				start = y;
			_replace(z, y);
			y->l = z->l;
			y->l->p = y;
			y->h = z->h;
		}
		--n;
		_rebalance(start);
	}
};

template <class K, class V, class KX> class _Tree : public _TreeBase {
public:
	struct Node : _TreeLink {
		V val;
	};
	static V &_val(_TreeLink *x) { return ((Node *)x)->val; }
	static const K &_key(_TreeLink *x) { return KX::key(((Node *)x)->val); }

	_Tree() { root = 0; n = 0; }
	_Tree(const _Tree &o) { root = 0; n = 0; _copy(o); }
	~_Tree() { _clear(); }
	_Tree &operator=(const _Tree &o)
	{
		if (this != &o) {
			_clear();
			_copy(o);
		}
		return *this;
	}

	_TreeLink *_first() const { return root ? _min(root) : 0; }
	/* The node with key k, or 0. */
	_TreeLink *_find(const K &k) const
	{
		_TreeLink *x = root;
		while (x) {
			if (k < _key(x))
				x = x->l;
			else if (_key(x) < k)
				x = x->r;
			else
				return x;
		}
		return 0;
	}
	/* The first node whose key isn't less than k (strict: greater than k), or 0. */
	_TreeLink *_bound(const K &k, int strict) const
	{
		_TreeLink *x = root, *best = 0;
		while (x) {
			if (strict ? (k < _key(x)) : !(_key(x) < k)) {
				best = x;
				x = x->l;
			} else
				x = x->r;
		}
		return best;
	}
	/* Insert v unless its key is there; *added says which. Returns the node. */
	_TreeLink *_insert(const V &v, int *added)
	{
		const K &k = KX::key(v);
		_TreeLink *x = root, *parent = 0;
		int left = 0;
		while (x) {
			parent = x;
			if (k < _key(x)) {
				x = x->l;
				left = 1;
			} else if (_key(x) < k) {
				x = x->r;
				left = 0;
			} else {
				*added = 0;
				return x;
			}
		}
		Node *nn = (Node *)::operator new(sizeof(Node));
		_Construct(&nn->val, v);
		_link(nn, parent, left);
		*added = 1;
		return nn;
	}
	void _erase(_TreeLink *x)
	{
		_unlink(x);
		_Destroy(&((Node *)x)->val);
		::operator delete((void *)x);
	}
	void _clear()
	{
		_free(root);
		root = 0;
		n = 0;
	}
	void _swap(_Tree &o)
	{
		std::swap(root, o.root);
		std::swap(n, o.n);
	}

private:
	static void _free(_TreeLink *x)
	{
		while (x) {
			_free(x->r);
			_TreeLink *l = x->l;
			_Destroy(&((Node *)x)->val);
			::operator delete((void *)x);
			x = l;
		}
	}
	void _copy(const _Tree &o)
	{
		int added;
		for (_TreeLink *x = o._first(); x; x = _next(x))
			_insert(_val(x), &added);
	}
};

}

#endif
