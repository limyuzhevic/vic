#include "Error.hpp"

namespace nlm {

// Global error handler setup
void initializeErrorHandling() {
    // Set up default error handler
    ErrorHandler::setErrorHandler([](const NLMException& error) {
        // Default implementation - could be extended for different logging backends
    });
}

// Helper function to check and throw on error
inline bool checkError(const NLMException& error, const std::string& context = "") {
    if (error.getSeverity() >= ErrorSeverity::Error) {
        if (context.empty()) {
            std::cerr << "[" << errorSeverityToString(error.getSeverity()) << "] " << error.what() << std::endl;
        } else {
            std::cerr << "[" << errorSeverityToString(error.getSeverity()) << "] " << context << ": " << error.what() << std::endl;
        }
        ErrorHandler::addError(error);
        return false;
    }
    return true;
}

} // namespace nlm
