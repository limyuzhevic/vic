# NLM Error Handling and User Feedback System

## Overview

This section introduces a comprehensive error handling and user feedback system for NLM (Neural Learning Machine). The system provides:

- Structured error handling with different severity levels
- User-friendly error messages
- Progress feedback and status updates
- Interactive console feedback
- Graceful error recovery
- Performance monitoring and metrics

## Error Handling Architecture

### Error Types

1. **Configuration Errors** - Invalid configuration files or parameters
2. **Initialization Errors** - Failure to initialize components
3. **Runtime Errors** - Errors during simulation execution
4. **Resource Errors** - Memory, file, or other resource issues
5. **Validation Errors** - Input validation failures

### Error Severity Levels

- **Critical** - System cannot function, requires immediate attention
- **Error** - Operation failed, partial functionality
- **Warning** - Potential issues, may affect performance
- **Info** - Informational messages, normal operation
- **Debug** - Detailed debugging information

## Error Handling Components

### 1. Error Classes

```cpp
// Base error class
class NLMError {
public:
    enum class Severity { Critical, Error, Warning, Info, Debug };
    
    NLMError(Severity severity, const std::string& message, 
             const std::string& component = "NLM");
    
    Severity getSeverity() const { return severity_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getComponent() const { return component_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    virtual std::string getDetails() const { return ""; }
    
protected:
    Severity severity_;
    std::string message_;
    std::string component_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific error types
class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message, const std::string& key = "")
        : NLMError(Severity::Error, message, "Config"), key_(key) {}
    
    std::string getDetails() const override {
        return key_.empty() ? "" : " (key: " + key_ + ")";
    }
    
private:
    std::string key_;
};

class BrainError : public NLMError {
public:
    BrainError(Severity severity, const std::string& message, const std::string& operation)
        : NLMError(severity, message, "Brain"), operation_(operation) {}
    
    std::string getDetails() const override {
        return " (operation: " + operation_ + ")";
    }
    
private:
    std::string operation_;
};
```

### 2. Error Handler Interface

```cpp
// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const NLMError& error) = 0;
    
    virtual void setErrorLevel(NLMError::Severity level) = 0;
    virtual NLMError::Severity getErrorLevel() const = 0;
    
    virtual void setReportingEnabled(bool enabled) = 0;
    virtual bool isReportingEnabled() const = 0;
    
    virtual void addErrorHandler(std::shared_ptr<IErrorHandler> handler) = 0;
    virtual void removeErrorHandler(std::shared_ptr<IErrorHandler> handler) = 0;
};
```

### 3. Console Error Handler

```cpp
// Console-based error handler with user-friendly messages
class ConsoleErrorHandler : public IErrorHandler {
public:
    ConsoleErrorHandler();
    ~ConsoleErrorHandler() override;
    
    void handleError(const NLMError& error) override;
    
    void setErrorLevel(NLMError::Severity level) override;
    NLMError::Severity getErrorLevel() const override;
    
    void setReportingEnabled(bool enabled) override;
    bool isReportingEnabled() const override;
    
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler) override;
    void removeErrorHandler(std::shared_ptr<IErrorHandler> handler) override;
    
    // User feedback methods
    void showProgress(const std::string& message, int percentage = -1);
    void showStatus(const std::string& status);
    void showHelp(const std::string& helpText);
    
    void enableRealTimeUpdates(bool enabled);
    bool areRealTimeUpdatesEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### 4. Error Reporter

```cpp
// Central error reporter with filtering and aggregation
class ErrorReporter {
public:
    static ErrorReporter& getInstance();
    
    void reportError(const NLMError& error);
    void reportWarning(const std::string& message, const std::string& component = "");
    void reportInfo(const std::string& message, const std::string& component = "");
    void reportDebug(const std::string& message, const std::string& component = "");
    
    void setErrorLevel(NLMError::Severity level);
    NLMError::Severity getErrorLevel() const;
    
    void setErrorFilter(const std::string& filter);
    std::string getErrorFilter() const;
    
    std::vector<NLMError> getRecentErrors(size_t count = 10) const;
    size_t getErrorCount(NLMError::Severity severity = NLMError::Severity::Error) const;
    
    void clearErrors();
    
    // Error aggregation and analysis
    std::string getErrorSummary() const;
    std::map<std::string, size_t> getErrorByComponent() const;
    
private:
    ErrorReporter();
    ~ErrorReporter();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Error Handling Implementation

### 1. Error Macros

```cpp
// Error handling macros for easy usage
#define NLM_THROW_ERROR(type, message) \
    throw type(message, #type)

#define NLM_REPORT_ERROR(type, message) \
    ErrorReporter::getInstance().reportError(type(message))

#define NLM_REPORT_WARNING(message) \
    ErrorReporter::getInstance().reportWarning(message)

#define NLM_REPORT_INFO(message) \
    ErrorReporter::getInstance().reportInfo(message)

#define NLM_REPORT_DEBUG(message) \
    ErrorReporter::getInstance().reportDebug(message)

// Component-specific error macros
#define BRAIN_ERROR(severity, message, operation) \
    NLM_REPORT_ERROR(BrainError(severity, message, operation))

#define CONFIG_ERROR(message, key) \
    NLM_REPORT_ERROR(ConfigError(message, key))

#define VALIDATION_ERROR(message) \
    NLM_REPORT_ERROR(NLMError(NLMError::Severity::Error, message, "Validation"))
```

### 2. Configuration Error Handling

```cpp
// Enhanced configuration loading with error handling
bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        ErrorReporter::getInstance().reportError(
            NLMError(NLMError::Severity::Error, "Configuration file path cannot be empty", "Config"));
        return false;
    }
    
    // Check if file exists
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ErrorReporter::getInstance().reportWarning(
            "Configuration file not found, using defaults: " + filepath, "Config");
        return false;
    }
    
    try {
        // Parse configuration with error handling
        // ... implementation ...
        
    } catch (const std::exception& e) {
        ErrorReporter::getInstance().reportError(
            NLMError(NLMError::Severity::Error, 
                    "Failed to parse configuration file: " + std::string(e.what()), "Config"));
        return false;
    }
    
    return true;
}
```

### 3. Brain Error Handling

```cpp
// Enhanced brain initialization with comprehensive error handling
bool Brain::initialize() {
    ErrorReporter::getInstance().reportInfo("Initializing brain...", "Brain");
    
    // Validate configuration
    if (!config_ || !config_->has("neuron_count") || !config_->has("region_count")) {
        ErrorReporter::getInstance().reportError(
            BrainError(NLMError::Severity::Critical, 
                      "Invalid configuration: missing required parameters", "initialize"));
        return false;
    }
    
    // Validate parameters
    uint64_t neuronCount = config_->getOr<uint64_t>("neuron_count", 100);
    uint64_t regionCount = config_->getOr<uint64_t>("region_count", 1);
    
    if (neuronCount < 1 || neuronCount > 1000000) {
        ErrorReporter::getInstance().reportError(
            BrainError(NLMError::Severity::Error, 
                      "Invalid neuron count: " + std::to_string(neuronCount), "initialize"));
        return false;
    }
    
    try {
        // Initialize components with error handling
        if (!initializeNeurons()) {
            return false;
        }
        
        if (!initializeRegions()) {
            return false;
        }
        
        if (!initializeConnections()) {
            return false;
        }
        
        ErrorReporter::getInstance().reportInfo("Brain initialization complete", "Brain");
        
    } catch (const std::exception& e) {
        ErrorReporter::getInstance().reportError(
            BrainError(NLMError::Severity::Critical, 
                      "Brain initialization failed: " + std::string(e.what()), "initialize"));
        return false;
    }
    
    return true;
}
```

### 4. Simulation Error Handling

```cpp
// Enhanced simulation with error handling and progress reporting
void Brain::simulate(SimulationStep maxSteps) {
    ErrorReporter::getInstance().reportInfo("Starting simulation...
", "Brain");
    
    // Setup progress tracking
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t lastProgressReport = 0;
    
    try {
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            // Check for interruption
            if (checkForInterruption()) {
                ErrorReporter::getInstance().reportInfo("Simulation interrupted by user", "Brain");
                break;
            }
            
            // Run simulation step
            step(step);
            
            // Report progress periodically
            if (step - lastProgressReport >= PROGRESS_REPORT_INTERVAL) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime).count();
                
                double progress = static_cast<double>(step) / maxSteps;
                double estimatedTotalTime = duration / progress;
                double remainingTime = estimatedTotalTime - duration;
                
                ErrorReporter::getInstance().reportInfo(
                    format("Step {}/{}. Progress: {:.1f}%. Remaining: {:.1f}s.
",
                    step, maxSteps, progress * 100, remainingTime), "Brain");
                
                lastProgressReport = step;
            }
            
            // Check for errors
            if (checkForErrors()) {
                ErrorReporter::getInstance().reportError(
                    BrainError(NLMError::Severity::Error, 
                              "Simulation error detected at step " + std::to_string(step), "step"));
                break;
            }
        }
        
        // Report completion
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
        
        ErrorReporter::getInstance().reportInfo(
            format("Simulation completed successfully. Total time: {:.2f}s.
", totalDuration), "Brain");
        
    } catch (const std::exception& e) {
        ErrorReporter::getInstance().reportError(
            BrainError(NLMError::Severity::Error, 
                      "Simulation failed: " + std::string(e.what()), "simulate"));
        throw;
    }
}
```

## User Feedback System

### 1. Progress Reporting

```cpp
class ProgressReporter {
public:
    ProgressReporter();
    ~ProgressReporter();
    
    void startProgress(const std::string& description);
    void updateProgress(int percentage, const std::string& message = "");
    void updateProgress(const std::string& message);
    void completeProgress(const std::string& message = "Completed");
    
    void setIndeterminate(bool indeterminate);
    bool isIndeterminate() const;
    
    void showTimeRemaining(bool show);
    void setSpeedEstimation(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### 2. Interactive Console

```cpp
class InteractiveConsole {
public:
    InteractiveConsole();
    ~InteractiveConsole();
    
    void start(const std::string& title = "NLM Console");
    void stop();
    
    bool isRunning() const { return running_; }
    
    // Command processing
    bool processCommand(const std::string& command);
    void displayHelp() const;
    void displayStatus() const;
    void displayErrors() const;
    
    // User interaction
    std::string getInput(const std::string& prompt = "> ");
    char getChar() const;
    bool waitForKey(char expectedKey) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool running_;
};
```

## Error Handling Integration

### 1. Global Error Handler Setup

```cpp
class NLMSystem {
public:
    static void initialize() {
        // Setup error handling
        auto errorHandler = std::make_shared<ConsoleErrorHandler>();
        errorHandler->setErrorLevel(NLMError::Severity::Warning);
        errorHandler->enableRealTimeUpdates(true);
        
        // Set as default error handler
        ErrorReporter::getInstance().addErrorHandler(errorHandler);
        
        // Setup global error reporter
        ErrorReporter::getInstance().setErrorLevel(NLMError::Severity::Warning);
        
        // Setup signal handlers for graceful shutdown
        setupSignalHandlers();
    }
    
    static void shutdown() {
        // Cleanup error handling
        ErrorReporter::getInstance().clearErrors();
        
        // Stop monitoring
        stopPerformanceMonitoring();
        
        // Save error log
        saveErrorLog();
    }
    
private:
    static void setupSignalHandlers() {
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);
    }
    
    static void handleSignal(int signum) {
        ErrorReporter::getInstance().reportInfo(
            "Received signal " + std::to_string(signum) + ", shutting down gracefully...", "System");
        
        // Cleanup and exit
        shutdown();
        exit(signum);
    }
};
```

### 2. Enhanced Error Reporting in Brain Demo

```cpp
int main(int argc, char* argv[]) {
    // Initialize NLM system
    NLMSystem::initialize();
    
    try {
        // Create error handler for user feedback
        auto errorHandler = std::dynamic_pointer_cast<ConsoleErrorHandler>(
            ErrorReporter::getInstance().getErrorHandlers()[0]);
        
        if (errorHandler) {
            errorHandler->setErrorLevel(NLMError::Severity::Warning);
            errorHandler->enableRealTimeUpdates(true);
            errorHandler->showProgress("Initializing NLM system...", 0);
        }
        
        // Parse command line arguments with error handling
        // ... implementation ...
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Show welcome message
        printWelcomeMessage();
        
        if (errorHandler) {
            errorHandler->updateProgress(25, "Creating configuration...");
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        if (errorHandler) {
            errorHandler->updateProgress(50, "Creating brain...");
        }
        
        // Create brain with error handling
        auto brain = std::make_shared<Brain>(config);
        
        if (errorHandler) {
            errorHandler->updateProgress(75, "Initializing components...");
        }
        
        if (!brain->initialize()) {
            throw BrainError(NLMError::Severity::Critical, "Brain initialization failed", "main");
        }
        
        if (errorHandler) {
            errorHandler->completeProgress("NLM system ready!");
        }
        
        // Display help if requested
        if (showHelp) {
            displayHelp();
            return 0;
        }
        
        // Run simulation with error handling
        runSimulation(brain, errorHandler);
        
        // Display final statistics
        displayFinalStatistics(brain, errorHandler);
        
    } catch (const NLMError& e) {
        ErrorReporter::getInstance().reportError(e);
        
        if (errorHandler) {
            errorHandler->completeProgress("Error occurred!");
        }
        
        std::cerr << "ERROR: " << e.getMessage() << std::endl;
        if (!e.getDetails().empty()) {
            std::cerr << "Details: " << e.getDetails() << std::endl;
        }
        
        ErrorReporter::getInstance().reportError(
            NLMError(NLMError::Severity::Critical, "NLM system shutdown due to error", "main"));
        
        return 1;
    } catch (const std::exception& e) {
        ErrorReporter::getInstance().reportError(
            NLMError(NLMError::Severity::Error, std::string("Unexpected error: ") + e.what(), "main"));
        
        std::cerr << "UNEXPECTED ERROR: " << e.what() << std::endl;
        
        ErrorReporter::getInstance().reportError(
            NLMError(NLMError::Severity::Critical, "NLM system shutdown due to unexpected error", "main"));
        
        return 1;
    }
    
    // Cleanup
    NLMSystem::shutdown();
    
    return 0;
}
```

## Error Handling Files

### Core Error Handling Implementation

```cpp
// src/core/Error/ErrorHandling.hpp
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <mutex>

namespace nlm {

// Error class definitions...
// Error handler interface...
// Error reporter class...
// Progress reporter class...
// Interactive console class...

} // namespace nlm
```

```cpp
// src/core/Error/ErrorHandling.cpp
#include "ErrorHandling.hpp"

namespace nlm {

// Implementation of error handling classes...
// Error handler implementations...
// Progress reporter implementations...
// Interactive console implementations...

} // namespace nlm
```

### Enhanced Brain with Error Handling

```cpp
// src/brain/BrainErrorHandling.hpp
#pragma once

#include "Brain.hpp"
#include "core/Error/ErrorHandling.hpp"

namespace nlm {

class BrainWithErrorHandling : public Brain {
public:
    BrainWithErrorHandling(std::shared_ptr<Config> config);
    ~BrainWithErrorHandling() override;
    
    bool initialize() override;
    void step(SimulationStep step) override;
    void step(SimulationStep step, Timestamp currentTime) override;
    
    // Error handling methods
    void setErrorHandler(std::shared_ptr<IErrorHandler> handler);
    std::shared_ptr<IErrorHandler> getErrorHandler() const;
    
    void enableErrorChecking(bool enabled);
    bool isErrorCheckingEnabled() const;
    
    std::vector<NLMError> getErrors() const;
    void clearErrors();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

## Usage Examples

### 1. Basic Error Handling

```cpp
#include "core/Error/ErrorHandling.hpp"

int main() {
    // Initialize NLM error system
    NLMSystem::initialize();
    
    try {
        // Your code here
        NLM_REPORT_ERROR(ConfigError("Invalid configuration value", "neuron_count"));
        
    } catch (const NLMError& e) {
        std::cerr << "Error: " << e.getMessage() << std::endl;
    }
    
    return 0;
}
```

### 2. Progress Reporting

```cpp
#include "core/Error/ErrorHandling.hpp"

int main() {
    auto progress = std::make_shared<ProgressReporter>();
    
    progress->startProgress("Running simulation");
    progress->updateProgress(25, "Initializing components");
    progress->updateProgress(50, "Running simulation steps");
    progress->updateProgress(75, "Finalizing results");
    progress->completeProgress("Simulation completed");
    
    return 0;
}
```

### 3. Interactive Console

```cpp
#include "core/Error/ErrorHandling.hpp"

int main() {
    auto console = std::make_shared<InteractiveConsole>();
    console->start("NLM Interactive Console");
    
    while (console->isRunning()) {
        std::string command = console->getInput("> ");
        console->processCommand(command);
    }
    
    return 0;
}
```

## Error Handling Best Practices

### 1. Error Classification
- Classify errors by severity and category
- Provide detailed error messages with context
- Include timestamps and component information

### 2. Error Recovery
- Implement graceful error recovery mechanisms
- Allow users to continue working after non-critical errors
- Provide clear guidance for resolving critical errors

### 3. Progress Feedback
- Show progress bars for long-running operations
- Report estimated completion times
- Provide status updates regularly

### 4. User Guidance
- Provide helpful error messages with suggested solutions
- Show examples of correct usage
- Display relevant documentation

### 5. Error Reporting
- Aggregate and analyze error patterns
- Provide error summaries and statistics
- Log errors for later analysis

## Benefits of Enhanced Error Handling

1. **Improved User Experience**
   - Clear error messages
   - Progress feedback
   - Interactive help system

2. **Better Debugging**
   - Detailed error information
   - Stack traces
   - Component context

3. **Enhanced Reliability**
   - Graceful error recovery
   - Comprehensive error checking
   - Input validation

4. **Advanced Features**
   - Real-time progress reporting
   - Interactive console
   - Performance monitoring

5. **Documentation and Support**
   - Built-in help system
   - Error examples
   - Troubleshooting guides

## Files Modified/Created

### Core Error Handling
- `src/core/Error/ErrorHandling.hpp` - Error handling interface
- `src/core/Error/ErrorHandling.cpp` - Error handling implementation

### Enhanced Brain
- `src/brain/BrainErrorHandling.hpp` - Brain with error handling
- `src/brain/BrainErrorHandling.cpp` - Brain error handling implementation

### Main Demo
- `src/main.cpp` - Enhanced with error handling
- `src/experiments/Phase7Demo.hpp` - Phase 7 demo with error handling
- `src/experiments/Phase7Demo.cpp` - Phase 7 demo implementation

### Error Handling Utilities
- `Phase7_ERROR_HANDLING.md` - Error handling documentation
- Additional error handling utilities and helper functions

## Integration with Existing System

The enhanced error handling system integrates seamlessly with:

1. **Existing Brain Operations**
   - All brain operations include error handling
   - Progress reporting for long-running tasks
   - Comprehensive error checking

2. **Phase 6 and Phase 7 Demos**
   - Interactive console for Phase 7
   - Progress reporting for all phases
   - Enhanced user feedback

3. **Python Bindings**
   - Error handling in Python API
   - Progress reporting for Python scripts
   - Graceful error recovery

4. **Configuration System**
   - Configuration validation
   - Error recovery for invalid configurations
   - Help and documentation integration

## Performance Considerations

### Error Handling Overhead
- Error checking is performed efficiently
- Progress reporting only when enabled
- Minimal overhead for production use

### Memory Usage
- Error handlers are lightweight
- Progress tracking uses minimal memory
- Error aggregation reduces memory footprint

### Performance Monitoring
- Built-in performance monitoring
- Progress tracking with time estimation
- Resource usage monitoring

## Future Enhancements

### Planned Features
1. **Distributed Error Reporting**
   - Remote error logging
   - Cloud-based error analysis
   - Team collaboration tools

2. **Advanced Error Recovery**
   - Automatic retry mechanisms
   - State restoration
   - Fallback strategies

3. **Enhanced User Interface**
   - Web-based error dashboard
   - Mobile app for monitoring
   - Customizable user interface

4. **Machine Learning Integration**
   - Error pattern recognition
   - Predictive error detection
   - Automated error recovery

### Research Directions
- Error taxonomy and classification
- User experience optimization
- Error recovery algorithms
- Performance error analysis

## Support and Community

### Documentation
- [Error Handling Guide](Phase7_ERROR_HANDLING.md)
- [Error Handling API Documentation](docs/ERROR_HANDLING.md)
- [Error Examples and Best Practices](docs/ERROR_EXAMPLES.md)

### Examples and Tutorials
- Error handling in Brain operations
- Progress reporting examples
- Interactive console usage
- Error recovery patterns

### Getting Help
1. **GitHub Repository:** https://github.com/nlm-project/nlm
2. **Documentation:** Comprehensive error handling guides
3. **Examples:** Working error handling examples
4. **Community:** Error handling discussion forums

## License

MIT License - See LICENSE file for details.

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.