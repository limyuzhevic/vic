#include "EnhancedLogger.hpp"
#include <sstream>
#include <iomanip>
#include <csignal>
#include <execinfo.h>  // For stack traces

namespace nlm {

// Global variable for stack trace size
const int STACK_TRACE_SIZE = 50;

EnhancedLogger::EnhancedLogger() {
    consoleLogger_ = std::make_shared<EnhancedConsoleLogger>(EnhancedLogLevel::Info);
    performanceProfiler_ = std::make_unique<PerformanceProfiler>();
    
    // Start performance profiler
    performanceProfiler_->start();
    
    NLM_LOG_INFO("EnhancedLogger initialized");
}

EnhancedLogger::~EnhancedLogger() {
    if (performanceProfiler_) {
        performanceProfiler_->stop();
    }
    
    NLM_LOG_INFO("EnhancedLogger destroyed");
}

std::string EnhancedLogger::getStackTrace() {
    std::vector<std::string> stackFrames;
    
    // Get current stack trace
    void* callstack[STACK_TRACE_SIZE];
    int frames = backtrace(callstack, STACK_TRACE_SIZE);
    
    // Convert to symbols
    char** symbols = backtrace_symbols(callstack, frames);
    
    for (int i = 1; i < frames; ++i) {  // Skip the getStackTrace frame
        std::string symbol(symbols[i]);
        
        // Try to demangle C++ symbols
        size_t pos = symbol.find("_Z");
        if (pos != std::string::npos) {
            // Simplified demangling (full demangling is complex)
            // Just show the function name in brackets
            size_t start = symbol.find('(');
            size_t end = symbol.find('+', start);
            if (start != std::string::npos && end != std::string::npos) {
                std::string function = symbol.substr(start + 1, end - start - 1);
                stackFrames.push_back(function);
            } else {
                stackFrames.push_back(symbol);
            }
        } else {
            stackFrames.push_back(symbol);
        }
    }
    
    // Free symbols
    free(symbols);
    
    // Format stack trace
    std::stringstream ss;
    ss << "Stack trace:" << std::endl;
    for (size_t i = 0; i < stackFrames.size(); ++i) {
        ss << "  #" << i << " " << stackFrames[i] << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
