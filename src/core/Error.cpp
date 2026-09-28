#include "Error.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

NLMError::NLMError(const std::string& message, ErrorCode code)
    : std::runtime_error(message), code_(code) {
    NLM_LOG_ERROR("NLM Error: " + message + " (Code: " + std::to_string(static_cast<int>(code)) + ")");
}

NLMError::~NLMError() = default;

ErrorCode NLMError::getCode() const {
    return code_;
}

// Template error creation functions

template<typename... Args>
std::string formatError(const char* format, Args... args) {
    std::stringstream ss;
    ss << std::vformat(format, std::make_format_args(args...));
    return ss.str();
}

} // namespace nlm
