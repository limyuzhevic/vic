#include "ConfigError.hpp"
#include <sstream>

namespace nlm {

std::string ConfigError::formatDetailedMessage(const std::string& message,
                                             const std::string& file,
                                             size_t line,
                                             const std::string& context) {
    std::ostringstream oss;
    oss << "[" << file << ":" << line << "] " << message;
    if (!context.empty()) {
        oss << " (" << context << ")";
    }
    return oss.str();
}

} // namespace nlm