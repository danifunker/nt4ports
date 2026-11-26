// stltest.cpp: checks the small STL of nt4/stl. Prints "stltest: ok" or the first
// failure. Builds with any of the port's compilers and runs under ntrun, or on the
// host:  g++ -I filezilla/nt4/stl -nostdinc++ filezilla/nt4/stl/stltest.cpp
#include <stdio.h>
#include <stdlib.h>
#include "list"
#include "vector"
#include "deque"
#include "map"
#include "set"
#include "memory"
#include "algorithm"

static int failures;
#define CHECK(c) do { if (!(c)) { printf("stltest: line %d: %s\n", __LINE__, #c); failures++; } } while (0)

static int live; // objects of Obj alive
struct Obj {
	int v;
	Obj(int x = 0) : v(x) { live++; }
	Obj(const Obj &o) : v(o.v) { live++; }
	~Obj() { live--; }
	Obj &operator=(const Obj &o) { v = o.v; return *this; }
	int operator==(const Obj &o) const { return v == o.v; }
	int operator<(const Obj &o) const { return v < o.v; }
};

static unsigned long seed = 12345;
static int rnd(int n) { seed = seed * 1103515245 + 12345; return (int)((seed >> 16) % n); }

// The AVL shape: ordered, heights right, balanced, parents right. Returns the height.
static int check_tree(const std::_TreeLink *x, const std::_TreeLink *parent)
{
	if (!x)
		return 0;
	CHECK(x->p == parent);
	int a = check_tree(x->l, x), b = check_tree(x->r, x);
	CHECK(a - b <= 1 && b - a <= 1);
	int h = (a > b ? a : b) + 1;
	CHECK(x->h == h);
	return h;
}

static void test_vector()
{
	{
		std::vector<Obj> v;
		CHECK(v.empty());
		int i;
		for (i = 0; i < 100; i++)
			v.push_back(Obj(i));
		CHECK(v.size() == 100 && v[57].v == 57 && v.back().v == 99);
		v.insert(v.begin() + 10, Obj(-1));
		CHECK(v.size() == 101 && v[10].v == -1 && v[11].v == 10);
		v.erase(v.begin());
		CHECK(v[0].v == 1 && v[9].v == -1);
		v.erase(v.begin() + 5, v.begin() + 15);
		CHECK(v.size() == 90 && v[5].v == 15);
		v.resize(120);
		CHECK(v.size() == 120 && v[119].v == 0);
		v.resize(3);
		CHECK(v.size() == 3);
		std::vector<Obj> w(v);
		CHECK(w.size() == 3 && w[2].v == v[2].v);
		w.push_back(w[0]); // an element of itself, while it grows
		CHECK(w.back().v == w[0].v);
		v = w;
		CHECK(v.size() == 4);
		std::vector<Obj>::iterator it = std::find(v.begin(), v.end(), Obj(2));
		CHECK(it == v.begin() + 1); // v is 1 2 3 1
		v.clear();
		CHECK(v.empty());
	}
	CHECK(live == 0);
	std::vector<int> iv(5, 7);
	CHECK(iv.size() == 5 && iv[4] == 7);
	iv.pop_back();
	CHECK(iv.size() == 4);
}

static void test_deque()
{
	{
		std::deque<Obj> d;
		d.push_back(Obj(1));
		d.push_front(Obj(0));
		d.push_back(Obj(2));
		CHECK(d.size() == 3 && d.front().v == 0 && d.back().v == 2);
		d.insert(d.begin() + 1, Obj(9));
		CHECK(d[1].v == 9);
		d.pop_front();
		CHECK(d.front().v == 9 && d.size() == 3);
	}
	CHECK(live == 0);
}

static void test_list()
{
	{
		std::list<Obj> l;
		int i;
		for (i = 0; i < 10; i++)
			l.push_back(Obj(i));
		l.push_front(Obj(-1));
		CHECK(l.size() == 11 && l.front().v == -1 && l.back().v == 9);
		std::list<Obj>::iterator it = l.begin();
		++it;
		++it;
		CHECK(it->v == 1);
		std::list<Obj>::iterator keep = it;
		++keep;
		it = l.erase(it);
		CHECK(it == keep && it->v == 2 && l.size() == 10);
		l.insert(it, Obj(100));
		l.erase(std::find(l.begin(), l.end(), Obj(5)));
		int expect[] = { -1, 0, 100, 2, 3, 4, 6, 7, 8, 9 };
		i = 0;
		for (std::list<Obj>::const_iterator ci = l.begin(); ci != l.end(); ci++, i++)
			CHECK(ci->v == expect[i]);
		CHECK(i == 10);
		i = 9;
		for (std::list<Obj>::reverse_iterator ri = l.rbegin(); ri != l.rend(); ri++, i--)
			CHECK(ri->v == expect[i]);
		std::list<Obj> m;
		m.push_back(Obj(42));
		m.swap(l);
		CHECK(l.size() == 1 && l.front().v == 42 && m.size() == 10 && m.back().v == 9);
		std::list<Obj> e;
		e.swap(l);
		CHECK(l.empty() && e.size() == 1 && e.front().v == 42);
		std::list<Obj> c(m);
		CHECK(c.size() == 10);
		c.pop_front();
		c.pop_back();
		CHECK(c.front().v == 0 && c.back().v == 8);
		std::list<Obj>::iterator last = c.end();
		--last;
		CHECK(last->v == 8);
		c.clear();
		CHECK(c.empty() && c.begin() == c.end());
	}
	CHECK(live == 0);
}

static void test_map()
{
	enum { N = 2000 };
	static int ref[N];
	int i;
	{
		std::map<int, Obj> m;
		for (i = 0; i < N; i++)
			ref[i] = -1;
		for (i = 0; i < 20000; i++) {
			int k = rnd(N);
			if (rnd(3)) {
				m[k] = Obj(i);
				ref[k] = i;
			} else {
				size_t n = m.erase(k);
				CHECK(n == (size_t)(ref[k] >= 0 ? 1 : 0));
				ref[k] = -1;
			}
		}
		size_t count = 0;
		for (i = 0; i < N; i++)
			if (ref[i] >= 0) {
				count++;
				std::map<int, Obj>::iterator it = m.find(i);
				CHECK(it != m.end() && it->first == i && it->second.v == ref[i]);
			} else
				CHECK(m.find(i) == m.end());
		CHECK(m.size() == count);
		int prev = -1;
		count = 0;
		for (std::map<int, Obj>::const_iterator ci = m.begin(); ci != m.end(); ++ci, count++) {
			CHECK(ci->first > prev);
			prev = ci->first;
		}
		CHECK(count == m.size());
		check_tree(m.begin()._t->root, 0);
		// Erase every other element through iterators, the others staying valid.
		std::map<int, Obj>::iterator it = m.begin();
		while (it != m.end()) {
			std::map<int, Obj>::iterator next = it;
			++next;
			m.erase(it);
			it = next;
			if (it != m.end())
				++it;
		}
		check_tree(m.begin()._t->root, 0);
		std::map<int, Obj>::iterator e = m.end();
		--e;
		CHECK(e->first == prev || e->first < prev);
		std::pair<std::map<int, Obj>::iterator, bool> r = m.insert(std::map<int, Obj>::value_type(-5, Obj(1)));
		CHECK(r.second && r.first->first == -5 && m.begin()->first == -5);
		r = m.insert(std::map<int, Obj>::value_type(-5, Obj(2)));
		CHECK(!r.second && r.first->second.v == 1);
		std::map<int, Obj> copy(m);
		CHECK(copy.size() == m.size());
		m.clear();
		CHECK(m.empty() && m.begin() == m.end());
	}
	CHECK(live == 0);
	std::map<int, int> mi;
	mi[3]++;
	mi[3]++;
	CHECK(mi[3] == 2 && mi.count(3) == 1 && mi.count(4) == 0);
}

static void test_set()
{
	std::set<int> s;
	int i;
	for (i = 0; i < 500; i++)
		s.insert(rnd(300));
	int prev = -1;
	size_t n = 0;
	for (std::set<int>::iterator it = s.begin(); it != s.end(); it++, n++) {
		CHECK(*it > prev);
		prev = *it;
	}
	CHECK(n == s.size());
	CHECK(!s.insert(prev).second);
	s.erase(prev);
	CHECK(s.find(prev) == s.end());
	check_tree(s.begin()._t->root, 0);
}

// MFC's CArray moves its elements byte for byte: a list must survive that.
static void test_list_moved()
{
	{
		char a[sizeof(std::list<Obj>)], b[sizeof(std::list<Obj>)];
		std::list<Obj> *l = new ((void *)a) std::list<Obj>;
		l->push_back(Obj(1));
		l->push_back(Obj(2));
		memcpy(b, a, sizeof(a));
		memset(a, 0xcc, sizeof(a));
		l = (std::list<Obj> *)b;
		l->push_back(Obj(3));
		int i = 1;
		for (std::list<Obj>::iterator it = l->begin(); it != l->end(); ++it, i++)
			CHECK(it->v == i);
		CHECK(i == 4 && l->size() == 3);
		std::_Destroy(l);
	}
	CHECK(live == 0);
}

static int odd(const Obj &o) { return o.v & 1; }

static void test_remove_if()
{
	{
		std::list<Obj> l;
		int i;
		for (i = 0; i < 10; i++)
			l.push_back(Obj(i));
		l.erase(std::remove_if(l.begin(), l.end(), odd), l.end());
		CHECK(l.size() == 5);
		i = 0;
		for (std::list<Obj>::iterator it = l.begin(); it != l.end(); ++it, i += 2)
			CHECK(it->v == i);
	}
	CHECK(live == 0);
}

static void test_auto_ptr()
{
	{
		std::auto_ptr<Obj> a(new Obj(3));
		std::auto_ptr<Obj> b(a);
		CHECK(a.get() == 0 && b->v == 3);
		b.reset(new Obj(4));
		CHECK((*b).v == 4 && live == 1);
	}
	CHECK(live == 0);
}

int main()
{
	test_vector();
	test_deque();
	test_list();
	test_map();
	test_set();
	test_list_moved();
	test_remove_if();
	test_auto_ptr();
	std::pair<int, int> pr = std::make_pair(1, 2);
	CHECK(pr.first == 1 && pr.second == 2);
	if (!failures)
		printf("stltest: ok\n");
	return failures != 0;
}
