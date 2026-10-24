#pragma once

#include <iostream>
#include <sstream>
#include <string>

// Global flag to control DCOUT output. Set via LIGHTUSD_ENABLE_DCOUT environment variable.
namespace lightusd {
extern bool g_enable_dcout_output;

// Default limit for recursive/iterative traversals to prevent infinite loops.
// Used as max recursion depth or max iteration count in tree/graph traversals.
constexpr size_t kMaxDefaultTraversalLimit = 1024 * 1024;
}

#if !defined(LIGHTUSD_PRODUCTION_BUILD) && !defined(LIGHTUSD_FUZZER_BUILD)
#if defined(LIGHTUSD_DEBUG_PRINT)
#define LIGHTUSD_LOCAL_DEBUG_PRINT
#endif
#endif


#if defined(_MSC_VER) && !defined(__clang__)
#define LIGHTUSD_NOINLINE __declspec(noinline)
#else
#define LIGHTUSD_NOINLINE __attribute__((noinline))
#endif

namespace lightusd {
namespace diag {

// Builds one PUSH_* diagnostic. Every member is defined out of line in
// str-util.cc, so a PUSH_* expansion costs a few calls instead of an inlined
// std::ostringstream; with thousands of expansions that inlined stream was a
// large share of the library's code size. The output matches the stream's
// default formatting: exact overloads cover the common types, and anything
// else (enums, small integers, types with their own operator<<) goes through
// one out-of-line stream per type.
class Message {
 public:
  Message(const char *file, const char *func, int line);
  Message(const char *file, const char *tag, const char *func, int line);
  Message(const char *file, const std::string &tag, const char *func, int line);
  Message(const Message &) = delete;
  Message &operator=(const Message &) = delete;

  Message &operator<<(const std::string &v);
  Message &operator<<(const char *v);
  Message &operator<<(char v);
  Message &operator<<(int v);
  Message &operator<<(unsigned v);
  Message &operator<<(long v);
  Message &operator<<(unsigned long v);
  Message &operator<<(long long v);
  Message &operator<<(unsigned long long v);
  Message &operator<<(double v);

  template <typename T>
  Message &operator<<(const T &v) {
    return AppendStreamed(v);
  }

  std::string Take();

 private:
  template <typename T>
  LIGHTUSD_NOINLINE Message &AppendStreamed(const T &v) {
    std::ostringstream ss;
    ss << v;
    buf_ += ss.str();
    return *this;
  }

  std::string buf_;
};

}  // namespace diag
}  // namespace lightusd

#if defined(LIGHTUSD_PRODUCTION_BUILD)
// Do not include full filepath for privacy.
#define LIGHTUSD_DIAG_FILE nullptr
#else
#define LIGHTUSD_DIAG_FILE __FILE__
#endif

#define PUSH_ERROR_AND_RETURN(s)                                       \
  do {                                                                 \
    ::lightusd::diag::Message ss_e(LIGHTUSD_DIAG_FILE, __func__, __LINE__); \
    ss_e << s;                                                         \
    PushError(ss_e.Take());                                            \
    return false;                                                      \
  } while (0)

#define PUSH_ERROR_AND_RETURN_TAG(tag, s)                                   \
  do {                                                                      \
    ::lightusd::diag::Message ss_e(LIGHTUSD_DIAG_FILE, tag, __func__, __LINE__); \
    ss_e << s;                                                              \
    PushError(ss_e.Take());                                                 \
    return false;                                                           \
  } while (0)

#define PUSH_ERROR(s)                                                  \
  do {                                                                 \
    ::lightusd::diag::Message ss_e(LIGHTUSD_DIAG_FILE, __func__, __LINE__); \
    ss_e << s;                                                         \
    PushError(ss_e.Take());                                            \
  } while (0)

#define PUSH_WARN(s)                                                   \
  do {                                                                 \
    ::lightusd::diag::Message ss_w(LIGHTUSD_DIAG_FILE, __func__, __LINE__); \
    ss_w << s;                                                         \
    PushWarn(ss_w.Take());                                             \
  } while (0)

#if defined(LIGHTUSD_LOCAL_DEBUG_PRINT)
// DCOUT macro - accepts iostream expressions with << operator
// Example usage: DCOUT("ptr = " << std::hex << ptr)
#define DCOUT(x)                                               \
  do {                                                         \
    if (lightusd::g_enable_dcout_output) {                     \
      std::ostringstream dcout_ss;                             \
      dcout_ss << x;                                           \
      std::cout << __FILE__ << ":" << __func__ << ":"          \
                << std::to_string(__LINE__) << " "             \
                << dcout_ss.str() << "\n";                     \
    }                                                          \
  } while (false)
#else
#define DCOUT(x)
#endif


// Simple auto-free class
// Use this class when saving stack size is required(e.g. recursive function call).
// T must have default constructor
template<typename T>
class AutoFree {
 private:
  T *_v{nullptr};

 public:

  AutoFree() : _v(new T()) {
  }

  ~AutoFree() {
    delete _v;
  }

  T &value() { return *_v; }
};
