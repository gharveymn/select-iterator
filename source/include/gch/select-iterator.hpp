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

#ifndef GCH_NODISCARD
#  if __has_cpp_attribute(nodiscard) >= 201603L
#    define GCH_NODISCARD [[nodiscard]]
#  else
#    define GCH_NODISCARD
#  endif
#endif

#if __cpp_impl_three_way_comparison && __has_include(<compare>)
#  include <compare>
#  if __cpp_lib_three_way_comparison >= 201907L && ! defined(GCH_THREE_WAY_COMPARISON)
#    define GCH_THREE_WAY_COMPARISON
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

  template <typename T, template <typename...> class TupleLike, typename ...Ts>
  struct tuple_index<T, TupleLike<Ts...>> : detail::tuple_index_helper<0, T, Ts...>
  { };
  
  namespace adl
  {
    
    using std::get;
    namespace resolve
    {
      template <std::size_t Index, typename T>
      auto get (T&& t) noexcept (noexcept (get<Index> (std::forward<T> (t))))
        -> decltype (get<Index> (std::forward<T> (t)));
    }
    
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  class select_iterator
  {
  public:
    
    using tuple_ptr  = typename std::iterator_traits<TupleIter>::pointer;
    
    static constexpr bool is_const = std::is_const<
      typename std::remove_pointer<tuple_ptr>::type>::value;
  
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
    
    constexpr /* implicit */ select_iterator (TupleIter it)
      noexcept (std::is_nothrow_copy_constructible<TupleIter>::value)
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
  
    GCH_NODISCARD
    constexpr select_iterator operator+ (difference_type n) const
      noexcept (noexcept (std::declval<TupleIter> () + n))
    {
      return select_iterator (m_iter + n);
    }
  
    GCH_CPP14_CONSTEXPR select_iterator& operator-= (difference_type n)
      noexcept (noexcept (std::declval<TupleIter> () -= n))
    {
      m_iter -= n;
      return *this;
    }
  
    GCH_NODISCARD
    constexpr select_iterator operator- (difference_type n) const
      noexcept (noexcept (std::declval<TupleIter> () - n))
    {
      return select_iterator (m_iter - n);
    }
  
    GCH_NODISCARD
    constexpr reference operator[] (difference_type n) const
      noexcept (noexcept (std::declval<TupleIter> ()[n]))
    {
      return operator+ (n).operator* ();
    }
  
    GCH_NODISCARD
    constexpr auto operator* (void) const
      noexcept (noexcept (adl::resolve::get<Index> (std::declval<TupleIter> ().operator* ())))
      -> decltype (adl::resolve::get<Index> (std::declval<TupleIter> ().operator* ()))
    {
      using std::get;
      return get<Index> (m_iter.operator* ());
    }
  
    GCH_NODISCARD
    constexpr auto operator-> (void) const
      noexcept (noexcept (&std::declval<select_iterator<Index, Value, TupleIter>> ().operator* ()))
      -> decltype (&std::declval<select_iterator<Index, Value, TupleIter>> ().operator* ())
    {
      return &operator* ();
    }
    
    GCH_NODISCARD
    constexpr const TupleIter& base (void) const noexcept
    {
      return m_iter;
    }

  private:
    TupleIter m_iter;
  };

#ifdef GCH_THREE_WAY_COMPARISON
  
  // SAME ITER
  
  template <std::size_t Index, typename Value, typename TupleIter>
  requires requires (TupleIter lhs, TupleIter rhs) { { lhs == rhs } -> std::convertible_to<bool>; }
  GCH_NODISCARD constexpr
  bool operator== (const select_iterator<Index, Value, TupleIter>& lhs,
                   const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () == rhs.base ()))
  {
    return lhs.base () == rhs.base ();
  }

  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator<=> (const select_iterator<Index, Value, TupleIter>& lhs,
                    const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () <=> rhs.base ()))
  {
    return lhs.base () <=> rhs.base ();
  }
  
  // BASE ITER LEFT
  
  template <std::size_t Index, typename Value, typename TupleIter>
  requires requires (TupleIter lhs, TupleIter rhs) { { lhs == rhs } -> std::convertible_to<bool>; }
  GCH_NODISCARD constexpr
  bool operator== (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs == rhs.base ()))
  {
    return lhs == rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator<=> (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs <=> rhs.base ()))
  {
    return lhs <=> rhs.base ();
  }
  
  // BASE ITER RIGHT
  
  template <std::size_t Index, typename Value, typename TupleIter>
  requires requires (TupleIter lhs, TupleIter rhs) { { lhs == rhs } -> std::convertible_to<bool>; }
  GCH_NODISCARD constexpr
  bool operator== (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () == rhs))
  {
    return lhs.base () == rhs;
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator<=> (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () <=> rhs))
  {
    return lhs.base () <=> rhs;
  }
  
#else
  
  // SAME ITER
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator< (const select_iterator<Index, Value, TupleIter>& lhs,
                  const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () < rhs.base ()))
    -> decltype (lhs.base () < rhs.base ())
  {
    return lhs.base () < rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator> (const select_iterator<Index, Value, TupleIter>& lhs,
                  const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () > rhs.base ()))
    -> decltype (lhs.base () > rhs.base ())
  {
    return lhs.base () > rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator<= (const select_iterator<Index, Value, TupleIter>& lhs,
                   const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () <= rhs.base ()))
    -> decltype (lhs.base () <= rhs.base ())
  {
    return lhs.base () <= rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator>= (const select_iterator<Index, Value, TupleIter>& lhs,
                   const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () >= rhs.base ()))
    -> decltype (lhs.base () >= rhs.base ())
  {
    return lhs.base () >= rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator== (const select_iterator<Index, Value, TupleIter>& lhs,
                   const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () == rhs.base ()))
    -> decltype (lhs.base () == rhs.base ())
  {
    return lhs.base () == rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator!= (const select_iterator<Index, Value, TupleIter>& lhs,
                   const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () != rhs.base ()))
    -> decltype (lhs.base () != rhs.base ())
  {
    return lhs.base () != rhs.base ();
  }
  
  // BASE ITER LEFT
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator< (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs < rhs.base ()))
    -> decltype (lhs < rhs.base ())
  {
    return lhs < rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator> (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs > rhs.base ()))
    -> decltype (lhs > rhs.base ())
  {
    return lhs > rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator<= (const TupleIter& lhs,const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs <= rhs.base ()))
    -> decltype (lhs <= rhs.base ())
  {
    return lhs <= rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator>= (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs >= rhs.base ()))
    -> decltype (lhs >= rhs.base ())
  {
    return lhs >= rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator== (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs == rhs.base ()))
    -> decltype (lhs == rhs.base ())
  {
    return lhs == rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator!= (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs != rhs.base ()))
    -> decltype (lhs != rhs.base ())
  {
    return lhs != rhs.base ();
  }
  
  // BASE ITER RIGHT
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator< (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () < rhs))
    -> decltype (lhs.base () < rhs)
  {
    return lhs.base () < rhs;
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator> (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () > rhs))
    -> decltype (lhs.base () > rhs)
  {
    return lhs.base () > rhs;
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator<= (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () <= rhs))
    -> decltype (lhs.base () <= rhs)
  {
    return lhs.base () <= rhs;
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator>= (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () >= rhs))
    -> decltype (lhs.base () >= rhs)
  {
    return lhs.base () >= rhs;
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator== (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () == rhs))
    -> decltype (lhs.base () == rhs)
  {
    return lhs.base () == rhs;
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator!= (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () != rhs))
    -> decltype (lhs.base () != rhs)
  {
    return lhs.base () != rhs;
  }
  
#endif
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator- (const select_iterator<Index, Value, TupleIter>& lhs,
                  const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs.base () - rhs.base ()))
    -> decltype (lhs.base () - rhs.base ())
  {
    return lhs.base () - rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator- (const TupleIter& lhs, const select_iterator<Index, Value, TupleIter>& rhs)
    noexcept (noexcept (lhs - rhs.base ()))
    -> decltype (lhs - rhs.base ())
  {
    return lhs - rhs.base ();
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  auto operator- (const select_iterator<Index, Value, TupleIter>& lhs, const TupleIter& rhs)
    noexcept (noexcept (lhs.base () - rhs))
    -> decltype (lhs.base () - rhs)
  {
    return lhs.base () - rhs;
  }
  
  template <std::size_t Index, typename Value, typename TupleIter>
  GCH_NODISCARD constexpr
  select_iterator<Index, Value, TupleIter>
  operator+ (typename select_iterator<Index, Value, TupleIter>::difference_type n,
             const select_iterator<Index, Value, TupleIter>& it)
    noexcept (noexcept (select_iterator<Index, Value, TupleIter> (n + it.base ())))
  {
    return select_iterator<Index, Value, TupleIter> (n + it.base ());
  }
  
  template <std::size_t Index, typename TupleIter>
  constexpr
  select_iterator<Index,
                  typename std::tuple_element<Index, typename TupleIter::value_type>::type,
                  TupleIter>
  make_select_iterator (TupleIter&& it)
  {
    return { std::forward<TupleIter> (it) };
  }
  
  template <typename T, typename TupleIter>
  constexpr
  select_iterator<tuple_index<T, typename TupleIter::value_type>::value, T, TupleIter>
  make_select_iterator (TupleIter&& it)
  {
    return { std::forward<TupleIter> (it) };
  }
  
  template <std::size_t Index, typename TupleIter>
  constexpr
  auto selecting (TupleIter&& it)
    -> decltype (make_select_iterator<Index> (std::forward<TupleIter> (it)))
  {
    return make_select_iterator<Index> (std::forward<TupleIter> (it));
  }
  
  template <typename T, typename TupleIter>
  constexpr
  auto selecting (TupleIter&& it)
    -> decltype (make_select_iterator<T> (std::forward<TupleIter> (it)))
  {
    return make_select_iterator<T> (std::forward<TupleIter> (it));
  }
  
}

#endif // SELECT_ITERATOR_HPP
