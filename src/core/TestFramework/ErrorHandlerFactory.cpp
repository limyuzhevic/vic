#include "ErrorHandler.hpp"
#include <stdexcept>

namespace nlm {

std::unique_ptr<ErrorHandler> ErrorHandlerFactory::createErrorHandler(const std::string& type, 
                                                           const std::map<std::string, std::string>& options) {
    if (type == "default" || type == "DefaultErrorHandler") {
        return std::make_unique<DefaultErrorHandler>();
    } else if (type == "silent" || type == "SilentErrorHandler") {
        return std::make_unique<SilentErrorHandler>();
    } else if (type == "exception" || type == "ExceptionErrorHandler") {
        return std::make_unique<ExceptionErrorHandler>();
    } else {
        throw std::runtime_error("Unknown error handler type: " + type);
    }
}

std::vector<std::string> ErrorHandlerFactory::getAvailableErrorHandlers() {
    return {"DefaultErrorHandler", "SilentErrorHandler", "ExceptionErrorHandler"};
}

} // namespace nlm
