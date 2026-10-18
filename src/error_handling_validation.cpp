// Error handling and validation implementation for NLM

#include "error_handling_validation.hpp"

namespace nlm {

// Implementation of NLMError::getFormattedTimestamp
std::string NLMError::getFormattedTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// InputValidator implementations

} // namespace nlm
