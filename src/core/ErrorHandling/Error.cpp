// nlm - Neural Learning Machine
// Error Handling Implementation
// Phase 2: Robust Error Management Implementation
//
// This file implements the ErrorHandling system.

#include "Error.hpp"
#include <iostream>

namespace nlm {

// Initialize global error handler
namespace {
    ErrorHandler& globalErrorHandler = ErrorHandler::getInstance();
}

// Global error handling setup
namespace ErrorHandling {
    void initialize() {
        ErrorHandler::Config config;
        globalErrorHandler.configure(config);
    }
    
    void configure(const ErrorHandler::Config& config) {
        globalErrorHandler.configure(config);
    }
    
    bool handle(const std::exception& error) {
        return globalErrorHandler.handle(error);
    }
    
    void shutdown() {
        globalErrorHandler.shutdown();
    }
    
    void abort() {
        globalErrorHandler.abort();
    }
}

} // namespace nlm