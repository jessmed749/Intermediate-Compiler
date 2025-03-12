#ifndef DEBUG_ASSERT_H
#define DEBUG_ASSERT_H

extern "C" void __assert_fail(char const *assertion,
                               char const *file, unsigned int line,
                               char const *function) noexcept(true);

extern "C" void
__assert_fail_break(char const *assertion) noexcept(true);

#define __ASSERT_STRING_1(expr) #expr
#define __ASSERT_STRINGIFY(expr) __ASSERT_STRING_1(expr)

/// Special version of assert that breaks in the debugger
#undef assert
#define assert(expr)                                                           \
    (static_cast<bool>(expr)                                                   \
             ? (void) 0                                                        \
             : __assert_fail_break(__FILE__ ":" __ASSERT_STRINGIFY(__LINE__)   \
                ": assertion " __ASSERT_STRINGIFY(expr) " failed\n"))

#endif // DEBUG_ASSERT_H
