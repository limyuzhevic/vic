// Error handling framework implementation for NLM
// This file implements the comprehensive error handling utilities

#include "core/ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {
    
    // NLMException implementation
    std::string NLMException::getTypeString() const {
        switch (type_) {
            case Type::ConfigurationError: return "Configuration Error";
            case Type::InitializationError: return "Initialization Error";
            case Type::ValidationError: return "Validation Error";
            case Type::MemoryError: return "Memory Error";
            case Type::SimulationError: return "Simulation Error";
            case Type::PlasticityError: return "Plasticity Error";
            case Type::NeuromodulationError: return "Neuromodulation Error";
            case Type::MemorySystemError: return "Memory System Error";
            case Type::PredictionError: return "Prediction Error";
            case Type::CognitionError: return "Cognition Error";
            case Type::DevelopmentError: return "Development Error";
            case Type::EnvironmentError: return "Environment Error";
            case Type::SerializationError: return "Serialization Error";
            case Type::UnknownError: return "Unknown Error";
            default: return "Unknown Error";
        }
    }
    
    std::string NLMException::getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    // ErrorHandling namespace implementation
    namespace ErrorHandling {
        
        bool validateConfig(const std::map<std::string, std::string>& config,
                           const std::map<std::string, std::function<bool(const std::string&)>>& validators) {
            bool allValid = true;
            for (const auto& pair : config) {
                const std::string& key = pair.first;
                const std::string& value = pair.second;
                
                auto it = validators.find(key);
                if (it != validators.end()) {
                    if (!it->second(value)) {
                        Logger::getGlobal().error("Configuration validation failed for key: " + key + 
                                                 " with value: " + value,
                                                 "", 0, "", "config_validation");
                        allValid = false;
                    }
                }
            }
            return allValid;
        }
        
        // Analysis namespace implementation
        namespace Analysis {
            ErrorAnalysis analyzeLogStatistics(const LogStatistics& stats) {
                ErrorAnalysis analysis;
                analysis.totalErrors = stats.totalLogs.load();
                
                // Extract error information from statistics
                for (size_t i = (size_t)LogLevel::Warning; i <= (size_t)LogLevel::Fatal; ++i) {
                    if (stats.logsByLevel[i] > 0) {
                        std::string levelStr = Logger::logLevelToString(static_cast<LogLevel>(i));
                        // For now, we'll assume all warnings and above are errors
                        if (i >= (size_t)LogLevel::Warning) {
                            analysis.totalErrors += stats.logsByLevel[i];
                            analysis.errorsByComponent["general"] += stats.logsByLevel[i];
                        }
                    }
                }
                
                // In a real implementation, we would extract more detailed information from log entries
                return analysis;
            }
        }
        
    } // namespace ErrorHandling
    
    // Helper function implementations
    std::string Logger::getCurrentThreadId() {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
    std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string Logger::logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Verbose: return "VERBOSE";
            case LogLevel::Info: return "INFO";
            case LogLevel::Notice: return "NOTICE";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Minor: return "MINOR";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            case LogLevel::Fatal: return "FATAL";
            case LogLevel::Audit: return "AUDIT";
            case LogLevel::Performance: return "PERFORMANCE";
            default: return "UNKNOWN";
        }
    }
    
    void Logger::configureDefaultLogger(const std::string& name) {
        static std::map<std::string, std::shared_ptr<Logger>> namedLoggers;
        if (namedLoggers.find(name) == namedLoggers.end()) {
            namedLoggers[name] = std::make_shared<Logger>();
        }
    }
    
    std::shared_ptr<Logger> Logger::getConfiguredLogger(const std::string& name) {
        static std::map<std::string, std::shared_ptr<Logger>> namedLoggers;
        auto it = namedLoggers.find(name);
        if (it != namedLoggers.end()) {
            return it->second;
        }
        // Create default logger if not found
        configureDefaultLogger(name);
        return namedLoggers[name];
    }
    
} // namespace nlm