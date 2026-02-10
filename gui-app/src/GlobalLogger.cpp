#ifndef GLOGGER
#define GLOGGER

#include <Logger.hpp>

static inline sane_in_the_membrane::utils::CLogger            g_logger{};

inline constexpr const sane_in_the_membrane::utils::ELogLevel TRACE    = sane_in_the_membrane::utils::ELogLevel::LOG_TRACE;
inline constexpr const sane_in_the_membrane::utils::ELogLevel DEBUG    = sane_in_the_membrane::utils::ELogLevel::LOG_DEBUG;
inline constexpr const sane_in_the_membrane::utils::ELogLevel INFO     = sane_in_the_membrane::utils::ELogLevel::LOG_INFO;
inline constexpr const sane_in_the_membrane::utils::ELogLevel WARN     = sane_in_the_membrane::utils::ELogLevel::LOG_WARN;
inline constexpr const sane_in_the_membrane::utils::ELogLevel ERR      = sane_in_the_membrane::utils::ELogLevel::LOG_ERROR;
inline constexpr const sane_in_the_membrane::utils::ELogLevel CRITICAL = sane_in_the_membrane::utils::ELogLevel::LOG_CRITICAL;

#endif // !GLOGGER
