// Phase 6 Enhanced Logger Configuration
// Advanced logging utilities for Phase 6 integration debugging

#include "core/Logger/Logger.hpp"
#include <string>
#include <memory>
#include <fstream>

namespace nlm {
namespace Phase6Logger {

// Enhanced logging levels for integration debugging
enum class Phase6LogLevel {
    Debug = 0,
    Info = 1,
    Integration = 2,    // Integration-specific debug info
    Memory = 3,         // Memory system events
    Neuromod = 4,      // Neuromodulation events
    Cognition = 5,      // Cognition system events
    Development = 6,    // Development system events
    Warning = 7,
    Error = 8,
    Critical = 9
};

// Integration-specific log events
struct IntegrationLogEvent {
    Phase6LogLevel level;
    std::string message;
    std::string system;    // "memory", "prediction", "cognition", "neuromodulation", "development"
    std::string component; // Specific component (e.g., "working_memory", "dopamine", "planner")
    uint64_t timestamp;
    std::string details;
    
    IntegrationLogEvent(Phase6LogLevel lvl, const std::string& msg, 
                       const std::string& sys, const std::string& comp,
                       uint64_t ts, const std::string& det = "")
        : level(lvl), message(msg), system(sys), component(comp), 
          timestamp(ts), details(det) {}
};

// Enhanced logger for Phase 6 integration
class Phase6Logger : public ILogger {
public:
    Phase6Logger() : currentLevel(Phase6LogLevel::Info) {}
    ~Phase6Logger() override = default;
    
    void log(const IntegrationLogEvent& event) {
        // Format the log entry with system and component info
        std::string levelStr = levelToString(event.level);
        std::string systemStr = systemToString(event.system);
        std::string componentStr = event.component.empty() ? "" : "[" + event.component + "]";
        
        std::string formattedMessage = "[" + levelStr + "] " + systemStr + " " + 
                                       componentStr + " " + event.message;
        
        if (!event.details.empty()) {
            formattedMessage += " | " + event.details;
        }
        
        formattedMessage += " (step: " + std::to_string(event.timestamp) + ")";
        
        // Output based on level
        if (event.level >= currentLevel) {
            // Output to console with color coding
            outputToConsole(formattedMessage, event.level);
            
            // Output to log file if enabled
            if (logToFile) {
                outputToFile(formattedMessage);
            }
        }
    }
    
    void log(const LogEntry& entry) override {
        // Convert standard LogEntry to Phase6 format for integration events
        IntegrationLogEvent integrationEvent(
            static_cast<Phase6LogLevel>(entry.level),
            entry.message,
            "general",  // System
            "",        // Component
            static_cast<uint64_t>(entry.timestamp.time_since_epoch().count()),
            entry.file + ":" + std::to_string(entry.line) + " " + entry.function
        );
        
        log(integrationEvent);
    }
    
    void setLevel(Phase6LogLevel level) {
        currentLevel = level;
    }
    
    Phase6LogLevel getLevel() const {
        return currentLevel;
    }
    
    void setLevel(LogLevel level) {
        currentLevel = static_cast<Phase6LogLevel>(static_cast<int>(level) + 2);
    }
    
    LogLevel getLevel() const override {
        return static_cast<LogLevel>(static_cast<int>(currentLevel) - 2);
    }
    
    // Specialized logging methods for integration systems
    void logMemoryEvent(const std::string& component, const std::string& message, 
                       Phase6LogLevel level = Phase6LogLevel::Memory) {
        log(IntegrationLogEvent(level, message, "memory", component,
                               std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   std::chrono::system_clock::now().time_since_epoch()).count()));
    }
    
    void logNeuromodulationEvent(const std::string& component, const std::string& message,
                                Phase6LogLevel level = Phase6LogLevel::Neuromod) {
        log(IntegrationLogEvent(level, message, "neuromodulation", component,
                               std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   std::chrono::system_clock::now().time_since_epoch()).count()));
    }
    
    void logCognitionEvent(const std::string& component, const std::string& message,
                          Phase6LogLevel level = Phase6LogLevel::Cognition) {
        log(IntegrationLogEvent(level, message, "cognition", component,
                               std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   std::chrono::system_clock::now().time_since_epoch()).count()));
    }
    
    void logDevelopmentEvent(const std::string& component, const std::string& message,
                           Phase6LogLevel level = Phase6LogLevel::Development) {
        log(IntegrationLogEvent(level, message, "development", component,
                               std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   std::chrono::system_clock::now().time_since_epoch()).count()));
    }
    
    void logIntegrationEvent(const std::string& component, const std::string& message,
                           Phase6LogLevel level = Phase6LogLevel::Integration) {
        log(IntegrationLogEvent(level, message, "integration", component,
                               std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   std::chrono::system_clock::now().time_since_epoch()).count()));
    }
    
    // Enable/disable file logging
    void setLogToFile(bool enable) {
        logToFile = enable;
        if (enable) {
            initializeFileLogger();
        } else {
            closeFileLogger();
        }
    }
    
    bool getLogToFile() const {
        return logToFile;
    }
    
    // Set log file path
    void setLogFile(const std::string& filepath) {
        logFilepath = filepath;
        if (logToFile) {
            initializeFileLogger();
        }
    }
    
    // Integration-specific log level helpers
    static std::string levelToString(Phase6LogLevel level) {
        switch (level) {
            case Phase6LogLevel::Debug: return "DEBUG";
            case Phase6LogLevel::Info: return "INFO";
            case Phase6LogLevel::Integration: return "INTEG";
            case Phase6LogLevel::Memory: return "MEMORY";
            case Phase6LogLevel::Neuromod: return "NEUROMOD";
            case Phase6LogLevel::Cognition: return "COG";
            case Phase6LogLevel::Development: return "DEV";
            case Phase6LogLevel::Warning: return "WARN";
            case Phase6LogLevel::Error: return "ERROR";
            case Phase6LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    static std::string systemToString(const std::string& system) {
        if (system == "memory") return "MEM";
        if (system == "neuromodulation") return "NEURO";
        if (system == "cognition") return "COG";
        if (system == "development") return "DEV";
        if (system == "integration") return "INTEG";
        return system;
    }
    
private:
    void outputToConsole(const std::string& message, Phase6LogLevel level) {
        // Color coding based on level
        std::string colorCode;
        switch (level) {
            case Phase6LogLevel::Debug: colorCode = "\033[36m"; break;      // Cyan
            case Phase6LogLevel::Info: colorCode = "\033[32m"; break;      // Green
            case Phase6LogLevel::Integration: colorCode = "\033[34m"; break; // Blue
            case Phase6LogLevel::Memory: colorCode = "\033[35m"; break;   // Magenta
            case Phase6LogLevel::Neuromod: colorCode = "\033[33m"; break; // Yellow
            case Phase6LogLevel::Cognition: colorCode = "\033[31m"; break; // Red
            case Phase6LogLevel::Development: colorCode = "\033[90m"; break; // Gray
            case Phase6LogLevel::Warning: colorCode = "\033[33m"; break;  // Yellow
            case Phase6LogLevel::Error: colorCode = "\033[31m"; break;    // Red
            case Phase6LogLevel::Critical: colorCode = "\033[35m"; break; // Magenta
            default: colorCode = "\033[0m";
        }
        
        std::cout << colorCode << "[" << static_cast<uint64_t>(level) << "]" 
                  << " " << message << "\033[0m" << std::endl;
    }
    
    void initializeFileLogger() {
        if (logToFile) {
            fileOut.open(logFilepath, std::ios::app);
            if (!fileOut.is_open()) {
                std::cerr << "Failed to open log file: " << logFilepath << std::endl;
            }
        }
    }
    
    void outputToFile(const std::string& message) {
        if (fileOut.is_open()) {
            fileOut << message << std::endl;
            fileOut.flush();
        }
    }
    
    void closeFileLogger() {
        if (fileOut.is_open()) {
            fileOut.close();
        }
    }
    
    Phase6LogLevel currentLevel;
    bool logToFile = false;
    std::string logFilepath = "nlm_phase6.log";
    std::ofstream fileOut;
};

// Global Phase 6 logger instance
Phase6Logger& getPhase6Logger() {
    static Phase6Logger logger;
    return logger;
}

// Convenience functions for common integration logging scenarios
void logBrainInitialization() {
    getPhase6Logger().logIntegrationEvent("brain", "Phase 6 brain initialization started");
}

void logMemorySystemStatus(const std::string& system, size_t capacity, size_t used) {
    getPhase6Logger().logMemoryEvent(system, 
        "Memory system initialized: capacity " + std::to_string(capacity) + 
        ", used " + std::to_string(used));
}

void logNeuromodulationStatus(const std::string& neuromodulator, float level) {
    getPhase6Logger().logNeuromodulationEvent(neuromulator, 
        "Neuromodulator active: level " + std::to_string(level));
}

void logCognitionStatus(const std::string& component, const std::string& status) {
    getPhase6Logger().logCognitionEvent(component, "Cognition system " + status);
}

void logDevelopmentProgress(const std::string& stage, float progress) {
    getPhase6Logger().logDevelopmentEvent("development", 
        "Development stage: " + stage + ", progress: " + std::to_string(progress));
}

void logIntegrationSuccess(const std::string& system) {
    getPhase6Logger().logIntegrationEvent(system, "Integration successful: all components connected");
}

void logIntegrationWarning(const std::string& system, const std::string& warning) {
    getPhase6Logger().logIntegrationEvent(system, "Warning: " + warning, Phase6LogLevel::Warning);
}

void logIntegrationError(const std::string& system, const std::string& error) {
    getPhase6Logger().logIntegrationEvent(system, "Error: " + error, Phase6LogLevel::Error);
}

} // namespace Phase6Logger

} // namespace nlm
