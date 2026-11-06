#define NPP_MINI_STL_DEFINE_NPOS
#include <stdio.h>
#include "string"
#include "vector"
#include "algorithm"
using namespace std;
#define T(x) do { if (!(x)) { printf("FAIL %d: %s\n", __LINE__, #x); bad++; } } while (0)
static int bad;
struct R { string s; int v; bool operator==(const R &o) const { return v == o.v; } };
static bool lessr(const R &a, const R &b) { return a.v < b.v; }
int main() {
	string a("hello"), b = a + " world", e;
	T(b == "hello world"); T(b.size() == 11); T(e.empty()); T(strlen(e.c_str()) == 0);
	T(b.find("world") == 6); T(b.find('z') == string::npos); T(b.rfind('o') == 7); T(b.find_last_of("lo") == 9); T(b.find_first_of("ow") == 4);
	T(b.substr(6) == "world"); T(b.substr(0, 5) == a); T(b.substr(99) == "");
	string c = b; c.erase(0, 6); T(c == "world"); c.insert(0, "my "); T(c == "my world"); c.replace(0, 2, "our"); T(c == "our world");
	c += c; T(c == "our worldour world"); c.assign(c.c_str() + 4, 5); T(c == "world");
	T(a < b); T(string("b") > "a"); T(a.compare("hello") == 0); T(string(3, 'x') == "xxx");
	c.erase(c.begin()); T(c == "orld"); c.clear(); T(c.size() == 0 && c.c_str()[0] == 0);
	string big; for (int i = 0; i < 5000; i++) big += 'a'; T(big.size() == 5000); T(string(big).size() == 5000);
	vector<string> v; for (int i = 0; i < 100; i++) { char t[8]; sprintf(t, "%d", i); v.push_back(t); }
	T(v.size() == 100); T(v[42] == "42"); v.push_back(v[0]); T(v.back() == "0");
	v.erase(v.begin()); T(v[0] == "1" && v.size() == 100); v.erase(v.begin(), v.begin() + 9); T(v[0] == "10"); v.insert(v.begin(), "x"); T(v[0] == "x" && v[1] == "10");
	vector<string> w = v; w[0] = "y"; T(v[0] == "x"); T(find(w.begin(), w.end(), string("42")) != w.end()); T(find(w.begin(), w.end(), string("no")) == w.end());
	vector<R> r; int n[] = {5, 1, 4, 2, 3}; for (int i = 0; i < 5; i++) { R x; x.v = n[i]; x.s = "s"; r.push_back(x); }
	sort(r.begin(), r.end(), lessr); for (int i = 0; i < 5; i++) T(r[i].v == i + 1);
	vector<int> q; q.resize(3, 7); T(q.size() == 3 && q[2] == 7); q.resize(1, 0); T(q.size() == 1); q.clear(); T(q.empty()); sort(q.begin(), q.end());
	printf("mini-STL: %d failures\n", bad); return bad;
}
