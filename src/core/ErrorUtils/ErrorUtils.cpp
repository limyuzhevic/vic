#include "ErrorUtils.hpp"
#include "NLMErrors.hpp"
#include "Logger.hpp"
#include <sstream>

namespace nlm {

namespace ErrorFormatter {
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
}

} // namespace nlm
