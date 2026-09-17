// Error System Implementation

#include "Error.hpp"
#include "../Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

// Global error handler
class GlobalErrorHandler {
public:
    using ErrorHandlerCallback = std::function<void(const NLMException&)>;
    
    void setErrorHandler(ErrorHandlerCallback handler) {
        errorHandler_ = handler;
    }
    
    void handleError(const NLMException& e) {
        if (errorHandler_) {
            errorHandler_(e);
        }
        // Always log the error
        NLM_LOG_STREAM(LogLevel::Error) << "Unhandled exception: " << e.what()
                                       .errorCode(e.getErrorCode())
                                       .stackTrace(getStackTrace());
    }
    
    void handleError(const std::exception& e, const std::string& context = "") {
        NLM_LOG_STREAM(LogLevel::Error) << "Exception in " << context << ": " << e.what()
                                       .errorCode("UNHANDLED")
                                       .stackTrace(getStackTrace());
    }
    
private:
    ErrorHandlerCallback errorHandler_;
};

GlobalErrorHandler globalErrorHandler;

// Exception registration for Python bindings
void registerPythonExceptions(py::module_& m) {
    m.def("register_exception", [](const std::string& name) {
        // This would register NLM exceptions with Python
        // Implementation depends on pybind11 version
    });
}

} // namespace nlm
