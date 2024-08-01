#include "Logger.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <memory>

namespace avalanche::core {
    FORCEINLINE spdlog::level convet_log_level_to_spdlog(LogLevel level) {
        switch (level) {
            case LogLevel::Trace:
                return spdlog::level::trace;
            case LogLevel::Debug:
                return spdlog::level::debug;
            case LogLevel::Info:
                return spdlog::level::info;
            case LogLevel::Warn:
                return spdlog::level::warn;
            case LogLevel::Error:
                return spdlog::level::err;
            case LogLevel::Critical:
                return spdlog::level::critical;
            case LogLevel::Off:
            default:
                return spdlog::level::off;
        }
    }

    class LogManager final : public ILogManager {
    public:
        LogManager() {
            m_console_log_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            m_console_log_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [thread %t] [%^%L%$] %s:%# %v");

            m_file_log_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/default.log", true);
            m_file_log_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [thread %t] [%^%L%$] %v");

            m_composited_logger = std::make_shared<spdlog::logger>("AvalancheLogger", spdlog::sinks_init_list { m_console_log_sink, m_file_log_sink });
        }

        void log(LogLevel level, eastl::string_view msg) override {
            log(level, std::string_view(msg.begin(), msg.end()));
        }

        void log(LogLevel level, std::string_view msg) override {
            m_composited_logger->log(convet_log_level_to_spdlog(level), msg);
        }

        void log(LogLevel level, std::string_view msg, const SourceLoc &source_loc) override {
            m_composited_logger->log(spdlog::source_loc(source_loc.filename, source_loc.line, source_loc.function_name), convet_log_level_to_spdlog(level), msg);
        }

        void log(LogLevel level, eastl::string_view msg, const SourceLoc &source_loc) override {
            log(level, std::string_view(msg.begin(), msg.end()), source_loc);
        }

        void trigger_breakpoint() override {
#           if !defined(AVALANCHE_DISABLE_BREAKPOINT_TRIGGER)
#               if defined(_MSC_VER) // MSVC
                    __debugbreak();
#               elif defined(__clang__) // Clang
#                   if defined(__APPLE__)
                        __builtin_debugtrap();
#                   else
                        __builtin_trap();
#                   endif
#               elif defined(__GNUC__) || defined(__GNUG__) // GCC
                    __builtin_trap();
#               else
#                   include <signal.h>
                    raise(SIGTRAP); // POSIX signal for breakpoint
#               endif // defined(_MSC_VER)
#           endif // !defined(AVALANCHE_DISABLE_BREAKPOINT_TRIGGER)
        }

    private:
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> m_console_log_sink;
        std::shared_ptr<spdlog::sinks::basic_file_sink_mt> m_file_log_sink;
        std::shared_ptr<spdlog::logger> m_composited_logger;
    };

    ILogManager::~ILogManager() = default;

    ILogManager & ILogManager::get() {
        static LogManager static_logger{};
        return static_logger;
    }
}

