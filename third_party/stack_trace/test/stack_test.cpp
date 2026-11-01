// Smoke test for the stack walker, built and run by .github/workflows/test-stack_trace.yml.
// Functions have external linkage so that the symbol can be resolved on every platform.

#include "stack_trace/stack.hpp"

#include <iostream>
#include <string>

#if defined(_MSC_VER)
    #define STACK_TEST_NOINLINE __declspec(noinline)
#else
    #define STACK_TEST_NOINLINE __attribute__((noinline))
#endif

// Without debug info, DbgHelp can only name exported symbols on Windows and attributes any
// unexported address to the closest preceding export, so main must be exported too
#if defined(_WIN32)
    #define STACK_TEST_EXPORT __declspec(dllexport)
#else
    #define STACK_TEST_EXPORT
#endif

STACK_TEST_EXPORT STACK_TEST_NOINLINE int stack_test_leaf()
{
    dbg::stack full;
    bool found = false;
    int mid_count = 0;
    for (dbg::stack::const_iterator it = full.begin(); it != full.end(); ++it) {
        std::cout << *it << "\n";
        if (it->function().find("stack_test_leaf") != std::string::npos)
            found = true;
        if (it->function().find("stack_test_mid") != std::string::npos)
            ++mid_count;
    }

    if (!found) {
        std::cerr << "stack_test_leaf not found in the captured stack\n";
        return 1;
    }

    // main calls stack_test_mid only once, so a second match means a frame was mislabelled
    if (mid_count != 1) {
        std::cerr << "stack_test_mid found " << mid_count << " times in the captured stack, expected 1\n";
        return 1;
    }

    dbg::stack limited(2);
    if (limited.depth() != 2) {
        std::cerr << "depth limit not honored, got " << limited.depth() << "\n";
        return 1;
    }

    return 0;
}

STACK_TEST_EXPORT STACK_TEST_NOINLINE int stack_test_mid()
{
    // Reading the volatile result after the call keeps stack_test_mid's frame from being tail-call optimised away
    volatile int result = stack_test_leaf();
    return result;
}

STACK_TEST_EXPORT int main()
{
    return stack_test_mid();
}
