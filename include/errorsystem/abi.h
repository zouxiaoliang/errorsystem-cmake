/**
 * @author zouxiaoliang
 * @date 2023/03/27
 */

#ifndef ABI_H
#define ABI_H

#include <string>

#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may
// have issues and returns 1 for 'defined( __has_include )', while
// '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if defined(__has_include) && (!defined(BOOST_GCC) || (__GNUC__ + 0) >= 5)
#if __has_include(<cxxabi.h>)
#define BOOST_CORE_HAS_CXXABI_H
#endif
#elif defined(__GLIBCXX__) || defined(__GLIBCPP__)
#define BOOST_CORE_HAS_CXXABI_H
#endif

#if defined(BOOST_CORE_HAS_CXXABI_H)
#include <cxxabi.h>
// For some archtectures (mips, mips64, x86, x86_64) cxxabi.h in Android NDK is
// implemented by gabi++ library
// (https://android.googlesource.com/platform/ndk/+/master/sources/cxx-stl/gabi++/),
// which does not implement abi::__cxa_demangle(). We detect this implementation
// by checking the include guard here.
#if defined(__GABIXX_CXXABI_H__)
#undef BOOST_CORE_HAS_CXXABI_H
#else
#include <cstddef>
#include <cstdlib>
#endif
#endif

namespace boost {

namespace core {

inline char const* demangle_alloc(char const* name) noexcept;
inline void demangle_free(char const* name) noexcept;

class scoped_demangled_name {
private:
    char const* m_p;

public:
    explicit scoped_demangled_name(char const* name) noexcept
        : m_p(demangle_alloc(name)) {}

    ~scoped_demangled_name() noexcept {
        demangle_free(m_p);
    }

    char const* get() const noexcept {
        return m_p;
    }

    scoped_demangled_name(scoped_demangled_name const&) = delete;
    scoped_demangled_name& operator=(scoped_demangled_name const&) = delete;
};

#if defined(BOOST_CORE_HAS_CXXABI_H)

inline char const* demangle_alloc(char const* name) noexcept {
    int status = 0;
    std::size_t size = 0;
    return abi::__cxa_demangle(name, NULL, &size, &status);
}

inline void demangle_free(char const* name) noexcept {
    std::free(const_cast<char*>(name));
}

inline std::string demangle(char const* name) {
    scoped_demangled_name demangled_name(name);
    char const* p = demangled_name.get();
    if (!p)
        p = name;
    return p;
}

#else

inline char const* demangle_alloc(char const* name) noexcept {
    return name;
}

inline void demangle_free(char const*) noexcept {}

inline std::string demangle(char const* name) {
    return name;
}

#endif

} // namespace core

} // namespace boost

#undef BOOST_CORE_HAS_CXXABI_H

#endif // ABI_H
