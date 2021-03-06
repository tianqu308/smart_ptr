#ifndef BOOST_SMART_PTR_LOCAL_SHARED_PTR_HPP_INCLUDED
#define BOOST_SMART_PTR_LOCAL_SHARED_PTR_HPP_INCLUDED

//  local_shared_ptr.hpp
//
//  http://www.boost.org/libs/smart_ptr/
//
//  Copyright 2017 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/smart_ptr/detail/local_counted_base.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace boost
{

namespace detail
{

} // namespace detail

//
// local_shared_ptr
//
// as shared_ptr, but local to a thread.
// reference count manipulations are non-atomic.
//

template<class T> class local_shared_ptr
{
private:

    typedef local_shared_ptr this_type;

public:

    typedef typename boost::detail::sp_element<T>::type element_type;

private:

    element_type * px;
    boost::detail::local_counted_base * pn;

    template<class Y> friend class local_shared_ptr;

public:

    // destructor

    ~local_shared_ptr() BOOST_SP_NOEXCEPT
    {
        if( pn )
        {
            pn->release();
        }
    }

    // constructors

    BOOST_CONSTEXPR local_shared_ptr() BOOST_SP_NOEXCEPT : px( 0 ), pn( 0 )
    {
    }

#if !defined( BOOST_NO_CXX11_NULLPTR )

    BOOST_CONSTEXPR local_shared_ptr( boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT : px( 0 ), pn( 0 )
    {
    }

#endif

    template<class Y>
    explicit local_shared_ptr( Y * p ): px( p ),
        pn( new boost::detail::local_counted_impl( shared_ptr<T>( p ) ) )
    {
    }

    template<class Y, class D> local_shared_ptr( Y * p, D d ): px( p ),
        pn( new boost::detail::local_counted_impl( shared_ptr<T>( p, d ) ) )
    {
    }

#if !defined( BOOST_NO_CXX11_NULLPTR )

    template<class D> local_shared_ptr( boost::detail::sp_nullptr_t p, D d ): px( p ),
        pn( new boost::detail::local_counted_impl( shared_ptr<T>( p, d ) ) )
    {
    }

#endif

    template<class Y, class D, class A> local_shared_ptr( Y * p, D d, A a ): px( p ),
        pn( new boost::detail::local_counted_impl( shared_ptr<T>( p, d, a ) ) )
    {
    }

#if !defined( BOOST_NO_CXX11_NULLPTR )

    template<class D, class A> local_shared_ptr( boost::detail::sp_nullptr_t p, D d, A a ): px( p ),
        pn( new boost::detail::local_counted_impl( shared_ptr<T>( p, d, a ) ) )
    {
    }

#endif

    // construction from shared_ptr

    template<class Y> local_shared_ptr( shared_ptr<Y> const & r,
        typename boost::detail::sp_enable_if_convertible<Y, T>::type = boost::detail::sp_empty() )
        : px( r.get() ), pn( 0 )
    {
        boost::detail::sp_assert_convertible< Y, T >();

        if( r.use_count() != 0 )
        {
            pn = new boost::detail::local_counted_impl( r );
        }
    }

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    template<class Y> local_shared_ptr( shared_ptr<Y> && r,
        typename boost::detail::sp_enable_if_convertible<Y, T>::type = boost::detail::sp_empty() )
        : px( r.get() ), pn( 0 )
    {
        boost::detail::sp_assert_convertible< Y, T >();

        if( r.use_count() != 0 )
        {
            pn = new boost::detail::local_counted_impl( std::move(r) );
        }
    }

#endif

    // construction from unique_ptr

#if !defined( BOOST_NO_CXX11_SMART_PTR ) && !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    template< class Y, class D >
    local_shared_ptr( std::unique_ptr< Y, D > && r,
        typename boost::detail::sp_enable_if_convertible<Y, T>::type = boost::detail::sp_empty() )
        : px( r.get() ), pn( 0 )
    {
        boost::detail::sp_assert_convertible< Y, T >();

        if( px )
        {
            pn = new boost::detail::local_counted_impl( shared_ptr<T>( std::move(r) ) );
        }
    }

#endif

    template< class Y, class D >
    local_shared_ptr( boost::movelib::unique_ptr< Y, D > r ); // !
    //	: px( r.get() ), pn( new boost::detail::local_counted_impl( shared_ptr<T>( std::move(r) ) ) )
    //{
    //	boost::detail::sp_assert_convertible< Y, T >();
    //}

    // copy constructor

    local_shared_ptr( local_shared_ptr const & r ) BOOST_SP_NOEXCEPT : px( r.px ), pn( r.pn )
    {
        if( pn )
        {
            pn->add_ref();
        }
    }

    // move constructor

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    local_shared_ptr( local_shared_ptr && r ) BOOST_SP_NOEXCEPT : px( r.px ), pn( r.pn )
    {
        r.px = 0;
        r.pn = 0;
    }

#endif

    // converting copy constructor

    template<class Y> local_shared_ptr( local_shared_ptr<Y> const & r,
        typename boost::detail::sp_enable_if_convertible<Y, T>::type = boost::detail::sp_empty() ) BOOST_SP_NOEXCEPT
        : px( r.px ), pn( r.pn )
    {
        boost::detail::sp_assert_convertible< Y, T >();

        if( pn )
        {
            pn->add_ref();
        }
    }

    // converting move constructor

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    template<class Y> local_shared_ptr( local_shared_ptr<Y> && r,
        typename boost::detail::sp_enable_if_convertible<Y, T>::type = boost::detail::sp_empty() ) BOOST_SP_NOEXCEPT
        : px( r.px ), pn( r.pn )
    {
        boost::detail::sp_assert_convertible< Y, T >();

        r.px = 0;
        r.pn = 0;
    }

#endif

    // aliasing

    template<class Y>
    local_shared_ptr( local_shared_ptr<Y> const & r, element_type * p ) BOOST_SP_NOEXCEPT : px( p ), pn( r.pn )
    {
        if( pn )
        {
            pn->add_ref();
        }
    }

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    template<class Y>
    local_shared_ptr( local_shared_ptr<Y> && r, element_type * p ) BOOST_SP_NOEXCEPT : px( p ), pn( r.pn )
    {
        r.px = 0;
        r.pn = 0;
    }

#endif

    // assignment

    local_shared_ptr & operator=( local_shared_ptr const & r ) BOOST_SP_NOEXCEPT
    {
        local_shared_ptr( r ).swap( *this );
        return *this;
    }

    template<class Y> local_shared_ptr & operator=( local_shared_ptr<Y> const & r ) BOOST_SP_NOEXCEPT
    {
        local_shared_ptr( r ).swap( *this );
        return *this;
    }

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    local_shared_ptr & operator=( local_shared_ptr && r ) BOOST_SP_NOEXCEPT
    {
        local_shared_ptr( std::move( r ) ).swap( *this );
        return *this;
    }

    template<class Y>
    local_shared_ptr & operator=( local_shared_ptr<Y> && r ) BOOST_SP_NOEXCEPT
    {
        local_shared_ptr( std::move( r ) ).swap( *this );
        return *this;
    }

#endif

#if !defined( BOOST_NO_CXX11_NULLPTR )

    local_shared_ptr & operator=( boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT
    {
        local_shared_ptr().swap(*this);
        return *this;
    }

#endif

#if !defined( BOOST_NO_CXX11_SMART_PTR ) && !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    template<class Y, class D>
    local_shared_ptr & operator=( std::unique_ptr<Y, D> && r )
    {
        local_shared_ptr( std::move(r) ).swap( *this );
        return *this;
    }

#endif

    template<class Y, class D>
    local_shared_ptr & operator=( boost::movelib::unique_ptr<Y, D> r ); // !

    // reset

    void reset() BOOST_SP_NOEXCEPT
    {
        local_shared_ptr().swap( *this );
    }

    template<class Y> void reset( Y * p ) // Y must be complete
    {
        local_shared_ptr( p ).swap( *this );
    }

    template<class Y, class D> void reset( Y * p, D d )
    {
        local_shared_ptr( p, d ).swap( *this );
    }

    template<class Y, class D, class A> void reset( Y * p, D d, A a )
    {
        local_shared_ptr( p, d, a ).swap( *this );
    }

    template<class Y> void reset( local_shared_ptr<Y> const & r, element_type * p ) BOOST_SP_NOEXCEPT
    {
        local_shared_ptr( r, p ).swap( *this );
    }

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

    template<class Y> void reset( local_shared_ptr<Y> && r, element_type * p ) BOOST_SP_NOEXCEPT
    {
        local_shared_ptr( std::move( r ), p ).swap( *this );
    }

#endif

    // accessors

    typename boost::detail::sp_dereference< T >::type operator* () const BOOST_SP_NOEXCEPT
    {
        return *px;
    }

    typename boost::detail::sp_member_access< T >::type operator-> () const BOOST_SP_NOEXCEPT
    {
        return px;
    }

    typename boost::detail::sp_array_access< T >::type operator[] ( std::ptrdiff_t i ) const BOOST_SP_NOEXCEPT_WITH_ASSERT
    {
        BOOST_ASSERT( px != 0 );
        BOOST_ASSERT( i >= 0 && ( i < boost::detail::sp_extent< T >::value || boost::detail::sp_extent< T >::value == 0 ) );

        return static_cast< typename boost::detail::sp_array_access< T >::type >( px[ i ] );
    }

    element_type * get() const BOOST_SP_NOEXCEPT
    {
        return px;
    }

    // implicit conversion to "bool"
#include <boost/smart_ptr/detail/operator_bool.hpp>

    long local_use_count() const BOOST_SP_NOEXCEPT
    {
        return pn? pn->local_use_count(): 0;
    }

    // swap

    void swap( local_shared_ptr & r ) BOOST_SP_NOEXCEPT
    {
        std::swap( px, r.px );
        std::swap( pn, r.pn );
    }

    // owner_before

    template<class Y> bool owner_before( local_shared_ptr<Y> const & r ) const BOOST_SP_NOEXCEPT
    {
        return std::less< boost::detail::local_counted_base* >()( pn, r.pn );
    }
};

template<class T, class U> inline bool operator==( local_shared_ptr<T> const & a, local_shared_ptr<U> const & b ) BOOST_SP_NOEXCEPT
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=( local_shared_ptr<T> const & a, local_shared_ptr<U> const & b ) BOOST_SP_NOEXCEPT
{
    return a.get() != b.get();
}

#if !defined( BOOST_NO_CXX11_NULLPTR )

template<class T> inline bool operator==( local_shared_ptr<T> const & p, boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator==( boost::detail::sp_nullptr_t, local_shared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator!=( local_shared_ptr<T> const & p, boost::detail::sp_nullptr_t ) BOOST_SP_NOEXCEPT
{
    return p.get() != 0;
}

template<class T> inline bool operator!=( boost::detail::sp_nullptr_t, local_shared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return p.get() != 0;
}

#endif

template<class T> inline void swap( local_shared_ptr<T> & a, local_shared_ptr<T> & b ) BOOST_SP_NOEXCEPT
{
    a.swap( b );
}

template<class T, class U> local_shared_ptr<T> static_pointer_cast( local_shared_ptr<U> const & r ) BOOST_SP_NOEXCEPT
{
    (void) static_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = static_cast< E* >( r.get() );
    return local_shared_ptr<T>( r, p );
}

template<class T, class U> local_shared_ptr<T> const_pointer_cast( local_shared_ptr<U> const & r ) BOOST_SP_NOEXCEPT
{
    (void) const_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = const_cast< E* >( r.get() );
    return local_shared_ptr<T>( r, p );
}

template<class T, class U> local_shared_ptr<T> dynamic_pointer_cast( local_shared_ptr<U> const & r ) BOOST_SP_NOEXCEPT
{
    (void) dynamic_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = dynamic_cast< E* >( r.get() );
    return p? local_shared_ptr<T>( r, p ): local_shared_ptr<T>();
}

template<class T, class U> local_shared_ptr<T> reinterpret_pointer_cast( local_shared_ptr<U> const & r ) BOOST_SP_NOEXCEPT
{
    (void) reinterpret_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = reinterpret_cast< E* >( r.get() );
    return local_shared_ptr<T>( r, p );
}

#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

template<class T, class U> local_shared_ptr<T> static_pointer_cast( local_shared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) static_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = static_cast< E* >( r.get() );
    return local_shared_ptr<T>( std::move(r), p );
}

template<class T, class U> local_shared_ptr<T> const_pointer_cast( local_shared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) const_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = const_cast< E* >( r.get() );
    return local_shared_ptr<T>( std::move(r), p );
}

template<class T, class U> local_shared_ptr<T> dynamic_pointer_cast( local_shared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) dynamic_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = dynamic_cast< E* >( r.get() );
    return p? local_shared_ptr<T>( std::move(r), p ): local_shared_ptr<T>();
}

template<class T, class U> local_shared_ptr<T> reinterpret_pointer_cast( local_shared_ptr<U> && r ) BOOST_SP_NOEXCEPT
{
    (void) reinterpret_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename local_shared_ptr<T>::element_type E;

    E * p = reinterpret_cast< E* >( r.get() );
    return local_shared_ptr<T>( std::move(r), p );
}

#endif // !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )

// get_pointer() enables boost::mem_fn to recognize local_shared_ptr

template<class T> inline typename local_shared_ptr<T>::element_type * get_pointer( local_shared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return p.get();
}

// operator<<

#if !defined(BOOST_NO_IOSTREAM)

template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< ( std::basic_ostream<E, T> & os, local_shared_ptr<Y> const & p )
{
    os << p.get();
    return os;
}

#endif // !defined(BOOST_NO_IOSTREAM)

// hash_value

template< class T > struct hash;

template< class T > std::size_t hash_value( local_shared_ptr<T> const & p ) BOOST_SP_NOEXCEPT
{
    return boost::hash< typename local_shared_ptr<T>::element_type* >()( p.get() );
}

} // namespace boost

#endif  // #ifndef BOOST_SMART_PTR_LOCAL_SHARED_PTR_HPP_INCLUDED
