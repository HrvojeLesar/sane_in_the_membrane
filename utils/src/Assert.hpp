#ifndef SANE_IN_THE_MEMBRANE_UTILS_ASSERT
#define SANE_IN_THE_MEMBRANE_UTILS_ASSERT

#include <format>
#include <iostream>
#include <string_view>

#ifndef NDEBUG
#include <source_location>
[[noreturn]] inline bool assert_fail(std::string_view fmt, std::format_args args, std::source_location loc = std::source_location::current()) {
    std::cerr << "ASSERTION FAILED: " << std::vformat(fmt, args) << '\n' << "File: " << loc.line() << ":" << loc.column() << " " << loc.file_name() << '\n';

    std::abort();
}
#else
[[noreturn]] inline bool assert_fail(std::string_view fmt, std::format_args args) {
    std::cerr << "ASSERTION FAILED\n" << std::vformat(fmt, args) << '\n';

    std::abort();
}
#endif

#define SITM_ASSERT(expr, fmt, ...) ((expr) ? true : assert_fail(fmt, std::make_format_args(__VA_ARGS__)))

#endif // !SANE_IN_THE_MEMBRANE_UTILS_ASSERT
