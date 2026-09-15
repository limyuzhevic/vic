#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "NLMError.hpp"

namespace nlm {

struct ErrorHandlerImpl {
    std::mutex mutex;
    std::vector<std::shared_ptr<IErrorHandler>> handlers;
    std::shared_ptr<IErrorHandler> defaultHandler;
    ErrorLevel errorLevelThreshold = ErrorLevel::Error;
    bool enableStackTraces = true;
    size_t maxStackTraceDepth = 20;
    std::string logFile;
    std::ofstream fileStream;
    std::map<ErrorCategory, std::string> categoryNames;
};

class DefaultErrorHandler : public IErrorHandler {
public:
    DefaultErrorHandler() {
        initializeCategoryNames();
        setLevel(LogLevel::Error);
    }
    
    void initializeCategoryNames() {
        categoryNames[ErrorCategory::System] = "System";
        categoryNames[ErrorCategory::Memory] = "Memory";
        categoryNames[ErrorCategory::Network] = "Network";
        categoryNames[ErrorCategory::Neural] = "Neural";
        categoryNames[ErrorCategory::Plasticity] = "Plasticity";
        categoryNames[ErrorCategory::MemorySystem] = "MemorySystem";
        categoryNames[ErrorCategory::Neuromodulation] = "Neuromodulation";
        categoryNames[ErrorCategory::Config] = "Config";
        categoryNames[ErrorCategory::World] = "World";
        categoryNames[ErrorCategory::Development] = "Development";
        categoryNames[ErrorCategory::Prediction] = "Prediction";
        categoryNames[ErrorCategory::Cognition] = "Cognition";
        categoryNames[ErrorCategory::Motor] = "Motor";
        categoryNames[ErrorCategory::Sensory] = "Sensory";
        categoryNames[ErrorCategory::Visualization] = "Visualization";
        categoryNames[ErrorCategory::Performance] = "Performance";
    }
    
    std::string getCategoryName(ErrorCategory category) const {
        auto it = categoryNames.find(category);
        return (it != categoryNames.end()) ? it->second : "Unknown";
    }
    
    void logToFile(const NLMError& error) {
        if (fileStream.is_open()) {
            std::lock_guard<std::mutex> lock(mutex);
            fileStream << "[NLM Error] " << error.getTimestamp() << ": " << error.what() << "\n";
            fileStream << "  Category: " << static_cast<int>(error.getCategory()) << " (" 
                       << getCategoryName(error.getCategory()) << ")\n";
            fileStream << "  Code: " << static_cast<int>(error.getCode()) << "\n";
            fileStream << "  Level: " << static_cast<int>(error.getLevel()) << "\n";
            
            const auto& context = error.getContext();
            if (!context.file.empty()) fileStream << "  File: " << context.file;
            if (context.line > 0) fileStream << ":" << context.line;
            if (!context.function.empty()) fileStream << " in " << context.function;
            fileStream << "\n";
            
            if (!context.details.empty()) {
                fileStream << "  Details: ";
                bool first = true;
                for (const auto& [key, value] : context.details) {
                    if (!first) fileStream << ", ";
                    fileStream << key << "=" << value;
                    first = false;
                }
                fileStream << "\n";
            }
            
            if (!context.stackTrace.empty()) {
                fileStream << "  Stack trace:\n";
                for (const auto& frame : context.stackTrace) {
                    fileStream << "    " << frame << "\n";
                }
            }
            
            fileStream << "\n";
        }
    }
    
    void handleError(const NLMError& error) override {
        // Check error level threshold
        if (error.getLevel() < errorLevelThreshold) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex);
        
        // Log to console with color coding
        std::cout << "\n";
        if (error.getLevel() <= ErrorLevel::Warning) {
            std::cout << "\033[1m" << "[" << getCategoryName(error.getCategory()) << "] " 
                      << error.what() << "\033[0m\n";
        } else {
            std::cout << "\033[1;31m" << "ERROR: " << error.what() << "\033[0m\n";
            std::cout << "\033[1;33m" << "  Category: " << getCategoryName(error.getCategory()) 
                      << " (Code: " << static_cast<int>(error.getCode()) << ")\033[0m\n";
        }
        
        const auto& context = error.getContext();
        if (!context.file.empty()) {
            std::cout << "\033[1;36m" << "  Location: " << context.file;
            if (context.line > 0) std::cout << ":" << context.line;
            if (!context.function.empty()) std::cout << " in " << context.function;
            std::cout << "\033[0m\n";
        }
        
        if (!context.details.empty()) {
            std::cout << "\033[1;32m" << "  Details: " << "\033[0m";
            for (const auto& [key, value] : context.details) {
                std::cout << "\033[1;33m" << key << "=" << value << "\033[0m";
                if (&key != &std::prev(context.details.end())->first) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
        
        // Log stack trace if enabled
        if (enableStackTraces && !context.stackTrace.empty()) {
            std::cout << "\033[1;35m" << "  Stack trace:" << "\033[0m\n";
            for (const auto& frame : context.stackTrace) {
                std::cout << "    " << frame << "\n";
            }
        }
        
        std::cout << "\033[1m" << "--- End of error ---\033[0m\n" << std::endl;
        
        // Log to file
        logToFile(error);
        
        // Flush output
        std::cout.flush();
        if (fileStream.is_open()) {
            fileStream.flush();
        }
    }
    
    bool shouldHandle(ErrorCategory category, ErrorCode code) const override {
        // Handle all errors by default
        return true;
    }
    
    void setLevel(LogLevel level) {
        errorLevelThreshold = static_cast<ErrorLevel>(level);
    }
    
    LogLevel getLevel() const {
        return static_cast<LogLevel>(errorLevelThreshold);
    }
    
    void setLogFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex);
        fileStream.close();
        fileStream.open(filepath, std::ios::app);
        if (!fileStream.is_open()) {
            std::cerr << "Failed to open error log file: " << filepath << std::endl;
        }
    }
    
    void closeLogFile() {
        std::lock_guard<std::mutex> lock(mutex);
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }
    
    bool isLogFileOpen() const {
        std::lock_guard<std::mutex> lock(mutex);
        return fileStream.is_open();
    }
    
    void setStackTracesEnabled(bool enabled) {
        enableStackTraces = enabled;
    }
    
    bool getStackTracesEnabled() const {
        return enableStackTraces;
    }
    
    void setMaxStackTraceDepth(size_t depth) {
        maxStackTraceDepth = depth;
    }
    
    size_t getMaxStackTraceDepth() const {
        return maxStackTraceDepth;
    }
    
    // Generate stack trace for current thread
    static std::vector<std::string> generateStackTrace() {
        std::vector<std::string> trace;
        
        if (!isStackTraceAvailable()) return trace;
        
        // Get the addresses for the backtrace
        void** addrs = new void*[maxStackDepth];
        int nptrs = backtrace(addrs, maxStackDepth);
        
        // Get the function names
        char** symbols = backtrace_symbols(addrs, nptrs);
        if (symbols) {
            for (int i = 1; i < nptrs; ++i) {  // Skip the first frame (this function)
                std::string symbol(symbols[i]);
                
                // Try to demangle C++ function names
                size_t pos = symbol.find("(+0x");
                if (pos != std::string::npos) {
                    // Extract function name
                    size_t start = 0;
                    if (symbol.find("__libc_start_main") != std::string::npos) {
                        start = symbol.find(" ");
                    } else {
                        start = symbol.find_last_of(" ") + 1;
                    }
                    size_t end = symbol.find("(", start);
                    if (end != std::string::npos) {
                        std::string func = symbol.substr(start, end - start);
                        // Try to demangle
                        trace.push_back(demangle(func) + " (" + symbol.substr(end) + ")");
                    } else {
                        trace.push_back(symbol);
                    }
                } else {
                    trace.push_back(symbol);
                }
            }
        }
        
        delete[] addrs;
        if (symbols) {
            free(symbols);
        }
        
        return trace;
    }
    
private:
    static bool isStackTraceAvailable() {
        // Check if we're in a reasonable environment
        return true;  // Always try, but handle failures gracefully
    }
    
    static constexpr size_t maxStackDepth = 100;
    
    std::string demangle(const std::string& mangled) {
        // Try to demangle C++ function names
        // This is a simplified implementation - in production, use cxxabi.h
        if (mangled.find("std::") != std::string::npos) {
            return mangled;
        }
        return mangled;
    }
    
    mutable std::mutex mutex;
    ErrorLevel errorLevelThreshold;
    bool enableStackTraces;
    std::map<ErrorCategory, std::string> categoryNames;
};

// Global error manager instance
static ErrorManager& getGlobalErrorManager() {
    static ErrorManager instance;
    return instance;
}

// ErrorManager implementation
void ErrorManager::registerHandler(std::shared_ptr<IErrorHandler> handler) {
    std::lock_guard<std::mutex> lock(mutex);
    handlers.push_back(handler);
}

void ErrorManager::unregisterHandler(std::shared_ptr<IErrorHandler> handler) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::find(handlers.begin(), handlers.end(), handler);
    if (it != handlers.end()) {
        handlers.erase(it);
    }
}

void ErrorManager::handleError(const NLMError& error) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check error level threshold
    if (error.getLevel() < errorLevelThreshold) {
        return;
    }
    
    // First try registered handlers
    for (auto& handler : handlers) {
        if (handler->shouldHandle(error.getCategory(), error.getCode())) {
            handler->handleError(error);
            return;
        }
    }
    
    // Use default handler if available
    if (defaultHandler) {
        defaultHandler->handleError(error);
        return;
    }
    
    // Final fallback: print to stderr
    std::cerr << "Unhandled NLM error: " << error.what() << std::endl;
}

void ErrorManager::setDefaultHandler(std::shared_ptr<IErrorHandler> handler) {
    std::lock_guard<std::mutex> lock(mutex);
    defaultHandler = handler;
}

void ErrorManager::setErrorLevel(ErrorLevel level) {
    std::lock_guard<std::mutex> lock(mutex);
    errorLevelThreshold = level;
}

// Global error handler initialization
static std::unique_ptr<DefaultErrorHandler> globalErrorHandler;

void initializeErrorSystem() {
    globalErrorHandler = std::make_unique<DefaultErrorHandler>();
    getGlobalErrorManager().setDefaultHandler(globalErrorHandler);
}

void shutdownErrorSystem() {
    if (globalErrorHandler) {
        globalErrorHandler->closeLogFile();
        globalErrorHandler.reset();
    }
}

void setErrorLogFile(const std::string& filepath) {
    if (globalErrorHandler) {
        globalErrorHandler->setLogFile(filepath);
    }
}

void setErrorLevel(ErrorLevel level) {
    if (globalErrorHandler) {
        globalErrorHandler->setLevel(static_cast<LogLevel>(level));
    }
    getGlobalErrorManager().setErrorLevel(level);
}

void setStackTracesEnabled(bool enabled) {
    if (globalErrorHandler) {
        globalErrorHandler->setStackTracesEnabled(enabled);
    }
}

// Error context utilities
namespace ErrorUtils {
    ErrorContext createContext(const std::string& message, 
                              const std::string& file,
                              const std::string& function,
                              int line) {
        return ErrorContext(message, file, function, line);
    }
    
    ErrorContext addContext(const ErrorContext& ctx, 
                          const std::string& key, 
                          const std::string& value) {
        ErrorContext result = ctx;
        result.addDetail(key, value);
        return result;
    }
    
    std::string formatErrorDetails(const std::map<std::string, std::string>& details) {
        if (details.empty()) return "";
        
        std::ostringstream oss;
        bool first = true;
        for (const auto& [key, value] : details) {
            if (!first) oss << ", ";
            oss << key << "=" << value;
            first = false;
        }
        return oss.str();
    }
}

// Python binding helpers
namespace PythonBindings {
    void translateNLMError(const NLMError& e) {
        // Convert NLMError to appropriate Python exception
        std::string msg = e.what();
        
        // Map error categories to Python exception types
        switch (e.getCategory()) {
            case ErrorCategory::Config:
                throw py::value_error("Config error: " + msg);
            case ErrorCategory::Neural:
                throw py::runtime_error("Neural error: " + msg);
            case ErrorCategory::MemorySystem:
                throw py::index_error("Memory error: " + msg);
            case ErrorCategory::Plasticity:
                throw py::runtime_error("Plasticity error: " + msg);
            case ErrorCategory::World:
                throw py::runtime_error("World error: " + msg);
            case ErrorCategory::Development:
                throw py::runtime_error("Development error: " + msg);
            case ErrorCategory::Prediction:
                throw py::runtime_error("Prediction error: " + msg);
            case ErrorCategory::Cognition:
                throw py::runtime_error("Cognition error: " + msg);
            case ErrorCategory::Motor:
                throw py::runtime_error("Motor error: " + msg);
            case ErrorCategory::Sensory:
                throw py::runtime_error("Sensory error: " + msg);
            case ErrorCategory::System:
            default:
                throw py::runtime_error("System error: " + msg);
        }
    }
    
    void registerExceptionTranslator() {
        py::register_exception_translator([](std::exception_ptr p) {
            try {
                if (p) std::rethrow_exception(p);
            } catch (const NLMError& e) {
                translateNLMError(e);
            } catch (const std::exception& e) {
                throw py::runtime_error("C++ exception: " + std::string(e.what()));
            }
        });
    }
}

} // namespace nlm
