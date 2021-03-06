#ifdef _ITERATOR_DEBUG_LEVEL
#undef _ITERATOR_DEBUG_LEVEL
#endif
#define _ITERATOR_DEBUG_LEVEL 0

#include "gch/select-iterator.hpp"
#include <tuple>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace gch;

template <std::size_t Index, typename Container>
void print_v (const Container& c)
{
  std::for_each (make_select_iterator<Index> (c.begin ()), make_select_iterator<Index> (c.end ()),
                 [](const typename std::tuple_element<Index, typename Container::value_type>::type& e)
                 {
                   std::cout << e << std::endl;
                 });
  std::cout << std::endl;
}

template <typename T, typename Container>
void print_v (const Container& c)
{
  std::for_each (make_select_iterator<T> (c.begin ()), make_select_iterator<T> (c.end ()),
                 [](const T& e) { std::cout << e << std::endl; });
  std::cout << std::endl;
}

struct myclass
{
    int get_value (void) { return x; }
    int x;
};

int main()
{
  myclass x { 1 };
  myclass y { 2 };
  myclass z { 3 };
  
  std::vector<std::tuple<int, std::string, bool, myclass>> v {{ 5, "hi0", true, x }, { 6, "hi1", false, y }, { 7, "hi2", true, z }};
  using iter = decltype (v)::iterator;
  
  print_v<0> (v);
  print_v<1> (v);
  print_v<2> (v);
  
  print_v<int> (v);
  print_v<std::string> (v);
  print_v<bool> (v);
  
  std::for_each (make_select_iterator<0> (v.begin ()), make_select_iterator<0> (v.end ()),
                 [](int& e) { e = e + 1; });
  
  print_v<0> (v);
  
  for (auto it = make_select_iterator<1> (v.begin ()); it != v.end (); ++it)
    std::cout << *it << std::endl;
  
  std::cout << std::endl;
  
  for (auto it = make_select_iterator<3> (v.begin ()); it != v.end (); ++it)
    std::cout << it->get_value () << std::endl;
  
  std::cout << std::endl;
  
  std::vector<std::pair<std::string, std::size_t>> vp {{ "hi11", 15}, { "hi12", 16}, { "hi13", 17}};
  print_v<0> (vp);
  print_v<1> (vp);
  
  print_v<std::string> (vp);
  print_v<std::size_t> (vp);
  
  const auto sit1 = make_select_iterator<0> (v.begin ());
  const auto sit2 = make_select_iterator<0> (v.end ());  
  
  assert (  (sit1 <  sit2));
  assert (! (sit1 >  sit2));
  assert (  (sit1 <= sit2));
  assert (! (sit1 >= sit2));
  assert (! (sit1 == sit2));
  assert (  (sit1 != sit2));
  
  assert (  (sit1 <  v.end ()));
  assert (! (sit1 >  v.end ()));
  assert (  (sit1 <= v.end ()));
  assert (! (sit1 >= v.end ()));
  assert (! (sit1 == v.end ()));
  assert (  (sit1 != v.end ()));
  
  assert (  (v.begin () <  sit2));
  assert (! (v.begin () >  sit2));
  assert (  (v.begin () <= sit2));
  assert (! (v.begin () >= sit2));
  assert (! (v.begin () == sit2));
  assert (  (v.begin () != sit2));
  
  assert (sit1 + 3 == sit2);
  assert (3 + sit1 == sit2);
  assert (sit2 - 3 == sit1);
  assert (sit2 - sit1 == 3);
  assert (sit1[2] == *(sit2 - 1));
  
  auto mit1 = sit1 + 1;
  assert (--mit1 == sit1);
  assert (mit1++ == sit1);
  assert (mit1 != sit1);
  assert ((mit1 -= 1) == sit1);
  
  auto mit2 = sit2 - 1;
  assert (++mit2 == sit2);
  assert (mit2-- == sit2);
  assert (mit2 != sit2);
  assert ((mit2 += 1) == sit2);
  
  return 0;
}
