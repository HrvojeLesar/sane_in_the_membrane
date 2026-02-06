#include <memory>
#include <Logger.hpp>

inline std::unique_ptr<sane_in_the_membrane::utils::CLogger>  g_logger = std::make_unique<sane_in_the_membrane::utils::CLogger>();

inline constexpr const sane_in_the_membrane::utils::ELogLevel TRACE    = sane_in_the_membrane::utils::ELogLevel::LOG_TRACE;
inline constexpr const sane_in_the_membrane::utils::ELogLevel DEBUG    = sane_in_the_membrane::utils::ELogLevel::LOG_DEBUG;
inline constexpr const sane_in_the_membrane::utils::ELogLevel INFO     = sane_in_the_membrane::utils::ELogLevel::LOG_INFO;
inline constexpr const sane_in_the_membrane::utils::ELogLevel WARN     = sane_in_the_membrane::utils::ELogLevel::LOG_WARN;
inline constexpr const sane_in_the_membrane::utils::ELogLevel ERR      = sane_in_the_membrane::utils::ELogLevel::LOG_TRACE;
inline constexpr const sane_in_the_membrane::utils::ELogLevel CRITICAL = sane_in_the_membrane::utils::ELogLevel::LOG_CRITICAL;
