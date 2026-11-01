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

STACK_TEST_NOINLINE int stack_test_leaf()
{
    dbg::stack full;
    bool found = false;
    for (dbg::stack::const_iterator it = full.begin(); it != full.end(); ++it) {
        std::cout << *it << "\n";
        if (it->function().find("stack_test_leaf") != std::string::npos)
            found = true;
    }

    if (!found) {
        std::cerr << "stack_test_leaf not found in the captured stack\n";
        return 1;
    }

    dbg::stack limited(2);
    if (limited.depth() != 2) {
        std::cerr << "depth limit not honored, got " << limited.depth() << "\n";
        return 1;
    }

    return 0;
}

STACK_TEST_NOINLINE int stack_test_mid()
{
    return stack_test_leaf();
}

int main()
{
    return stack_test_mid();
}
