#include "ErrorHandling.hpp"
#include <iostream>
#include <mutex>

namespace nlm {

// Global error handler management
namespace {
    IErrorHandler* globalErrorHandler = nullptr;
    std::mutex errorHandlerMutex;
}

// Error reporting and handling functions
void reportError(const std::string& context, const std::exception& e, bool throwException) {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    
    IErrorHandler* handler = getGlobalErrorHandler();
    if (handler) {
        try {
            auto& nlmException = dynamic_cast<const NLMException&>(e);
            handler->handleError(nlmException);
        } catch (const std::bad_cast&) {
            // Not an NLMException, create one
            NLMException nlmError(e.what(), NLMErrors::UnknownError);
            handler->handleError(nlmError);
        }
    } else {
        // Default handler
        DefaultErrorHandler defaultHandler;
        try {
            auto& nlmException = dynamic_cast<const NLMException&>(e);
            defaultHandler.handleError(nlmException);
        } catch (const std::bad_cast&) {
            NLMException nlmError(e.what(), NLMErrors::UnknownError);
            defaultHandler.handleError(nlmError);
        }
    }
    
    if (throwException) {
        throw;
    }
}

void reportError(NLMErrors errorCode, const std::string& context, const std::string& message, bool throwException) {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    
    std::string fullMessage = "[" + context + "] " + message;
    NLMException nlmError(fullMessage, errorCode);
    
    IErrorHandler* handler = getGlobalErrorHandler();
    if (handler) {
        handler->handleError(nlmError);
    } else {
        // Default handler
        DefaultErrorHandler defaultHandler;
        defaultHandler.handleError(nlmError);
    }
    
    if (throwException) {
        throw nlmError;
    }
}

// Global error handler management
IErrorHandler* getGlobalErrorHandler() {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    return globalErrorHandler;
}

void setGlobalErrorHandler(IErrorHandler* handler) {
    std::lock_guard<std::mutex> lock(errorHandlerMutex);
    globalErrorHandler = handler;
}

} // namespace nlm
