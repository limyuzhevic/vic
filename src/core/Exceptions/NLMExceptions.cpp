// NLM Exception Implementation
// Implementation of custom exception classes and error handling

#include "core/Exceptions/NLMExceptions.hpp"
#include "core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

// Exception registry implementation
ExceptionRegistry& ExceptionRegistry::getInstance() {
    static ExceptionRegistry instance;
    return instance;
}

void ExceptionRegistry::registerException(const std::string& category, std::exception_ptr exception) {
    exceptionHistory_.push_back({category, exception});
}

std::vector<std::pair<std::string, std::exception_ptr>> ExceptionRegistry::getExceptionHistory() const {
    return exceptionHistory_;
}

void ExceptionRegistry::clearHistory() {
    exceptionHistory_.clear();
}

std::string ExceptionRegistry::getLastErrorSummary() const {
    if (exceptionHistory_.empty()) {
        return "No errors recorded.";
    }
    
    std::string summary = "Last error: " + exceptionHistory_.back().first + "\n";
    try {
        std::rethrow_exception(exceptionHistory_.back().second);
    } catch (const std::exception& e) {
        summary += std::string("  Message: ") + e.what();
    }
    return summary;
}

} // namespace nlm
