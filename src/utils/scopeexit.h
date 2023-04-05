#ifndef UTILS_SCOPEEXIT_H
#define UTILS_SCOPEEXIT_H

#include <functional>


/**
 * A general purpose scope guard.
 *
 * This simple class mimics the behavior of std::experimental::scope_exit.
 */
class ScopeExit {
public:
   explicit ScopeExit(std::function<void ()>&& f) : m_f(std::move(f)) {}
   ~ScopeExit() { if (m_f) { m_f(); } }

   ScopeExit(const ScopeExit&) = delete;
   ScopeExit& operator=(const ScopeExit&) = delete;

   void release() { m_f = std::function<void ()>(); }

private:
   std::function<void ()> m_f;
};

#endif // UTILS_SCOPEEXIT_H
