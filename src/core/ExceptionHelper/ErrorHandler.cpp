#include "ErrorHandler.hpp"
#include <stdexcept>

namespace nlm {

// ErrorHandler implementation
void ErrorHandler::registerError(const std::string& component, const std::string& operation, 
                                 const std::string& details, const std::string& file, int line) {
    ErrorContext context(component, operation);
    context.setFile(file).setLine(line);
    
    if (!details.empty()) {
        context.addDetail("details", details);
    }
    
    if (errorCallback) {
        errorCallback(context, NLMException(details.empty() ? "Error occurred" : details));
    }
}

} // namespace nlm
