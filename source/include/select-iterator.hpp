/** select-iterator.hpp
 * Short description here. 
 * 
 * Copyright © 2020 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SELECT_ITERATOR_HPP
#define SELECT_ITERATOR_HPP

#ifndef GCH_CPP14_CONSTEXPR
#  if __cpp_constexpr >= 201304L
#    define GCH_CPP14_CONSTEXPR constexpr
#  else
#    define GCH_CPP14_CONSTEXPR
#  endif
#endif

#include <cstddef>
#include <type_traits>
#include <utility>
#include <tuple>
#include <iterator>

namespace gch
{
  namespace detail
  {
    template <std::size_t Idx, typename T, typename Head, typename ...Tail>
    struct tuple_index_helper
      : tuple_index_helper<Idx + 1, T, Tail...>
    { };

    template <std::size_t Idx, typename T, typename ...Tail>
    struct tuple_index_helper<Idx, T, T, Tail...>
      : std::integral_constant<std::size_t, Idx>
    { };

    template <std::size_t Idx, typename T>
    struct tuple_index_helper<Idx, T, T>
      : std::integral_constant<std::size_t, Idx>
    { };

    template <std::size_t Idx, typename T, typename NotT>
    struct tuple_index_helper<Idx, T, NotT>
    {
      static_assert (std::is_same<T, NotT>::value, "type not found");
    };
  }

  template <typename T, typename Tuple>
  struct tuple_index;

  template <typename T, template <typename...> typename TupleLike, typename ...Ts>
  struct tuple_index<T, TupleLike<Ts...>> : detail::tuple_index_helper<0, T, Ts...>
  { };
  
  namespace adl_resolve
  {
    using std::get;
    
    template <std::size_t Index, typename Tuple>
    typename std::tuple_element<Index, Tuple>::type&
    get (Tuple& t) noexcept (noexcept (get<Index> (t)));
  
    template <std::size_t Index, typename Tuple>
    typename std::tuple_element<Index, Tuple>::type&&
    get (Tuple&& t) noexcept (noexcept (get<Index> (t)));
  
    template <std::size_t Index, typename Tuple>
    typename std::tuple_element<Index, Tuple>::type const&
    get (const Tuple& t) noexcept (noexcept (get<Index> (t)));
  
    template <std::size_t Index, typename Tuple>
    typename std::tuple_element<Index, Tuple>::type const&&
    get (const Tuple&& t) noexcept (noexcept (get<Index> (t)));
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  class select_iterator
  {
  public:
    
    using tuple_ptr  = typename std::iterator_traits<TupleIter>::pointer;
    
    static constexpr bool
    is_const = std::is_const<typename std::remove_pointer<tuple_ptr>::type>::value;
  
    using difference_type = typename std::iterator_traits<TupleIter>::difference_type;
    using value_type = Value;
    using pointer = typename std::conditional<is_const, const value_type *, value_type *>::type;
    using const_pointer = const value_type *;
    using reference = typename std::conditional<is_const, const value_type&, value_type&>::type;
    using const_reference = const value_type&;
    using iterator_category = typename std::iterator_traits<TupleIter>::iterator_category;
  
    select_iterator            (void)                       = default;
    select_iterator            (const select_iterator&)     = default;
    select_iterator            (select_iterator&&) noexcept = default;
    select_iterator& operator= (const select_iterator&)     = default;
    select_iterator& operator= (select_iterator&&) noexcept = default;
    ~select_iterator           (void)                       = default;
    
    constexpr /* implicit */ select_iterator (TupleIter it) noexcept
      : m_iter (it)
    { }
  
    GCH_CPP14_CONSTEXPR select_iterator& operator++ (void)
      noexcept (noexcept (++std::declval<TupleIter> ()))
    {
      ++m_iter;
      return *this;
    }
  
    GCH_CPP14_CONSTEXPR select_iterator operator++ (int)
      noexcept (noexcept (std::declval<TupleIter> ()++))
    {
      return select_iterator (m_iter++);
    }
  
    GCH_CPP14_CONSTEXPR select_iterator& operator-- (void)
      noexcept (noexcept (--std::declval<TupleIter> ()))
    {
      --m_iter;
      return *this;
    }
  
    GCH_CPP14_CONSTEXPR select_iterator operator-- (int)
      noexcept (noexcept (std::declval<TupleIter> ()--))
    {
      return select_iterator (m_iter--);
    }
  
    GCH_CPP14_CONSTEXPR select_iterator& operator+= (difference_type n)
      noexcept (noexcept (std::declval<TupleIter> () += n))
    {
      m_iter += n;
      return *this;
    }
  
    [[nodiscard]]
    constexpr select_iterator operator+ (difference_type n) const
      noexcept (noexcept (std::declval<TupleIter> () + n))
    {
      return select_iterator (m_iter + n);
    }
  
    [[nodiscard]]
    friend constexpr select_iterator
    operator+ (difference_type n, const select_iterator& it)
      noexcept (noexcept (n + std::declval<TupleIter> ()))
    {
      return select_iterator (n + it.m_iter);
    }
  
    GCH_CPP14_CONSTEXPR select_iterator& operator-= (difference_type n)
      noexcept (noexcept (std::declval<TupleIter> () -= n))
    {
      m_iter -= n;
      return *this;
    }
  
    [[nodiscard]]
    constexpr select_iterator operator- (difference_type n) const
      noexcept (noexcept (std::declval<TupleIter> () - n))
    {
      return select_iterator (m_iter - n);
    }
  
    [[nodiscard]]
    constexpr difference_type operator- (const select_iterator& other) const
      noexcept (noexcept (std::declval<TupleIter> () - std::declval<TupleIter> ()))
    {
      return m_iter - other.m_iter;
    }
  
    [[nodiscard]]
    constexpr reference operator[] (difference_type n) const
      noexcept (noexcept (std::declval<TupleIter> ()[n]))
    {
      return operator+ (n).operator* ();
    }
  
    [[nodiscard]]
    constexpr bool operator< (const select_iterator& other) const
      noexcept (noexcept (std::declval<TupleIter> () < std::declval<TupleIter> ()))
    {
      return m_iter < other.m_iter;
    }
  
    [[nodiscard]]
    constexpr bool operator> (const select_iterator& other) const
      noexcept (noexcept (std::declval<TupleIter> () > std::declval<TupleIter> ()))
    {
      return m_iter > other.m_iter;
    }
  
    [[nodiscard]]
    constexpr bool operator<= (const select_iterator& other) const
      noexcept (noexcept (std::declval<TupleIter> () <= std::declval<TupleIter> ()))
    {
      return m_iter <= other.m_iter;
    }
  
    [[nodiscard]]
    constexpr bool operator>= (const select_iterator& other) const
      noexcept (noexcept (std::declval<TupleIter> () >= std::declval<TupleIter> ()))
    {
      return m_iter >= other.m_iter;
    }
  
    [[nodiscard]]
    constexpr bool operator== (const select_iterator& other) const
      noexcept (noexcept (std::declval<TupleIter> () == std::declval<TupleIter> ()))
    {
      return m_iter == other.m_iter;
    }
  
    [[nodiscard]]
    constexpr bool operator!= (const select_iterator& other) const
      noexcept (noexcept (std::declval<TupleIter> () != std::declval<TupleIter> ()))
    {
      return m_iter != other.m_iter;
    }
  
    [[nodiscard]]
    constexpr reference operator* (void) const
      noexcept (noexcept (adl_resolve::get<Index> (std::declval<TupleIter> ().operator* ())))
    {
      return adl_resolve::get<Index> (m_iter.operator* ());
    }
  
    [[nodiscard]]
    constexpr pointer operator-> (void) const
      noexcept (noexcept (&operator* ()))
    {
      return &operator* ();
    }

  private:
    TupleIter m_iter;
  };
  
  template <std::size_t Index, typename TupleIter>
  constexpr
  select_iterator<Index,
                  typename std::tuple_element<Index, typename TupleIter::value_type>::type,
                  TupleIter>
  make_select_iterator (TupleIter it)
  {
    return it;
  }
  
  template <typename T, typename TupleIter>
  constexpr
  select_iterator<tuple_index<T, typename TupleIter::value_type>::value, T, TupleIter>
  make_select_iterator (TupleIter it)
  {
    return it;
  }
}

#endif // SELECT_ITERATOR_HPP
