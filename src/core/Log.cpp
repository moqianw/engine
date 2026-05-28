#include "core/Log.hpp"

#include <QCoreApplication>
#include <QDir>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
namespace EG {

    std::shared_ptr<spdlog::logger> Log::s_core;
    std::shared_ptr<spdlog::logger> Log::s_gfx;

    void Log::init()
    {
        if (s_core && s_gfx) {
            return;
        }

        const QString logDirectory = QDir(QCoreApplication::applicationDirPath())
            .filePath(QStringLiteral("logs"));
        QDir().mkpath(logDirectory);
        const std::string engineLogPath =
            QDir(logDirectory).filePath(QStringLiteral("engine.log")).toStdString();

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(engineLogPath, true);
        auto msvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

        consoleSink->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%n] %v");
        fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] [%n] %v");

        s_core = std::make_shared<spdlog::logger>("CORE",
            spdlog::sinks_init_list{ consoleSink, fileSink, msvcSink });

        s_gfx = std::make_shared<spdlog::logger>("GFX",
            spdlog::sinks_init_list{ consoleSink, fileSink, msvcSink });

        spdlog::register_logger(s_core);
        spdlog::register_logger(s_gfx);

#if defined(_DEBUG) || !defined(NDEBUG)
        s_core->set_level(spdlog::level::trace);
        s_gfx->set_level(spdlog::level::trace);
#else
        s_core->set_level(spdlog::level::info);
        s_gfx->set_level(spdlog::level::info);
#endif

        spdlog::flush_on(spdlog::level::warn);
    }

    std::shared_ptr<spdlog::logger>& Log::core() { return s_core; }
    std::shared_ptr<spdlog::logger>& Log::gfx() { return s_gfx; }

} // namespace EG
