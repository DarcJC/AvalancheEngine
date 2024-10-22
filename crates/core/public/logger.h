#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <source_location>
#include <format>
#include <string_view>
#include "avalanche_core_export.h"
#include "polyfill.h"


namespace avalanche::core {

    enum class LogLevel : uint8_t {
        Trace = 0,
        Debug,
        Info,
        Warn,
        Error,
        Critical,
        Off,
        NumLevels,
    };

    struct SourceLoc {
        AVALANCHE_CONSTEXPR SourceLoc() = default;
        AVALANCHE_CONSTEXPR SourceLoc(const char* filename, int line, const char* function_name)
            : filename(filename)
            , line(line)
            , function_name(function_name)
        {}
        AVALANCHE_CONSTEXPR bool is_empty() const AVALANCHE_NOEXCEPT {
            return 0 != line;
        }

        const char* filename = nullptr;
        int line = 0;
        const char* function_name = nullptr;
    };

    class AVALANCHE_CORE_API ILogManager {
    public:
        virtual ~ILogManager();

        static ILogManager& get();

        virtual void log(LogLevel level, std::string_view msg) = 0;

        virtual void log(LogLevel level, std::string_view msg, const SourceLoc& source_loc) = 0;

        virtual void log(LogLevel level, std::string_view msg, const std::source_location& source_loc) = 0;

        virtual void trigger_breakpoint() = 0;

    public:
        template <typename... Args>
        void log(LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
            auto msg = std::format(fmt, std::forward<Args>(args)...);
            log(level, msg);
        }

        template <typename... Args>
        void log(const SourceLoc& source_loc, LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
            auto msg = std::format(fmt, std::forward<Args>(args)...);
            log(level, msg, source_loc);
        }

        template <typename... Args>
        void log(const std::source_location& source_loc, LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
            auto msg = std::format(fmt, std::forward<Args>(args)...);
            log(level, msg, source_loc);
        }

        template <typename... Args>
        inline void trace(std::format_string<Args...> fmt, Args &&... args) {
            log(LogLevel::Trace, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void debug(std::format_string<Args...> fmt, Args &&... args) {
            log(LogLevel::Debug, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void info(std::format_string<Args...> fmt, Args &&... args) {
            log(LogLevel::Info, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void warn(std::format_string<Args...> fmt, Args &&... args) {
            log(LogLevel::Warn, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void error(std::format_string<Args...> fmt, Args &&... args) {
            log(LogLevel::Error, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        inline void critical(std::format_string<Args...> fmt, Args &&... args) {
            log(LogLevel::Critical, fmt, std::forward<Args>(args)...);
        }
    };

}

#if !defined(AVALANCHE_DISABLE_BREAKPOINT_TRIGGER)
#   if defined(_MSC_VER) // MSVC
#       define AVALANCHE_TRIGGER_BREAKPOINT() __debugbreak();
#   elif defined(__clang__) // Clang
#       if defined(__APPLE__)
#           define AVALANCHE_TRIGGER_BREAKPOINT() __builtin_debugtrap();
#       else
#           define AVALANCHE_TRIGGER_BREAKPOINT() __builtin_trap();
#       endif
#   elif defined(__GNUC__) || defined(__GNUG__) // GCC
#           define AVALANCHE_TRIGGER_BREAKPOINT() __builtin_trap();
#   else
#           include <signal.h>
#           define AVALANCHE_TRIGGER_BREAKPOINT() raise(SIGTRAP); // POSIX signal for breakpoint
#   endif // defined(_MSC_VER)
#else
#   define AVALANCHE_TRIGGER_BREAKPOINT()
#endif // !defined(AVALANCHE_DISABLE_BREAKPOINT_TRIGGER)

#define AVALANCHE_LOGGER avalanche::core::ILogManager::get()
#define AVALANCHE_LOG_WITH_SOURCE_LOC(level, ...) AVALANCHE_LOGGER.log(std::source_location::current(), level, __VA_ARGS__)
#define AVALANCHE_CHECK_RUNTIME(expr, ...) do { if (!!(!(expr))) AVALANCHE_UNLIKELY_BRANCH { AVALANCHE_LOG_WITH_SOURCE_LOC(avalanche::core::LogLevel::Critical, __VA_ARGS__); AVALANCHE_TRIGGER_BREAKPOINT(); exit(233); } } while (false);
#define AVALANCHE_ENSURE(expr, ...) do { if (!!(!(expr))) AVALANCHE_UNLIKELY_BRANCH { AVALANCHE_LOG_WITH_SOURCE_LOC(avalanche::core::LogLevel::Error, __VA_ARGS__); AVALANCHE_TRIGGER_BREAKPOINT(); } } while (false);
#define AVALANCHE_CHECK(expr, ...) AVALANCHE_CHECK_RUNTIME(expr, __VA_ARGS__)
#define AVALANCHE_TODO(...) AVALANCHE_ENSURE(false, "🚧 Stepped into codes that working in progress 🚧")
