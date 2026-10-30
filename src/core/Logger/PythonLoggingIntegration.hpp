// Python logging integration module for NLM

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Logger.hpp"
#include "EnhancedLogger.hpp"

namespace py = pybind11;
namespace nlm {

namespace python_logging {
    
    // Python-friendly logging configuration
    struct PythonLogConfig {
        std::string module;
        LogLevel level = LogLevel::Info;
        bool useColors = true;
        std::string outputFile;
        bool enableRotation = false;
        size_t maxFileSizeMB = 100;
        size_t maxFiles = 5;
        bool rotateDaily = false;
        bool compressLogs = true;
        
        // Performance and monitoring
        bool enablePerformanceLogging = true;
        bool enableMemoryLogging = true;
        bool enableNetworkLogging = false;
        std::string networkEndpoint;
        
        // Debug and development
        bool enableDebugLogging = false;
        bool enableTraceLogging = false;
        std::map<std::string, std::string> customMetadata;
    };
    
    // Helper functions for Python logging
    class PythonLogger {
    public:
        PythonLogger() {
            // Initialize with default configuration
            auto& logger = getGlobalAdvancedLogger();
            
            // Create and configure console logger
            auto consoleLogger = std::make_shared<EnhancedConsoleLogger>();
            logger.addLogger(consoleLogger);
            
            // Set up performance monitoring
            if (enablePerformanceLogging) {
                // Performance monitoring would be set up here
            }
        }
        
        // Configure logging from Python dictionary
        static void configure(const py::dict& config) {
            PythonLogConfig pyConfig;
            
            // Parse configuration from Python dict
            if (config.contains("module")) {
                pyConfig.module = py::cast<std::string>(config["module"]);
            }
            
            if (config.contains("level")) {
                std::string levelStr = py::cast<std::string>(config["level"]);
                if (levelStr == "DEBUG") pyConfig.level = LogLevel::Debug;
                else if (levelStr == "INFO") pyConfig.level = LogLevel::Info;
                else if (levelStr == "WARNING") pyConfig.level = LogLevel::Warning;
                else if (levelStr == "ERROR") pyConfig.level = LogLevel::Error;
                else if (levelStr == "CRITICAL") pyConfig.level = LogLevel::Critical;
            }
            
            if (config.contains("use_colors")) {
                pyConfig.useColors = py::cast<bool>(config["use_colors"]);
            }
            
            if (config.contains("output_file")) {
                pyConfig.outputFile = py::cast<std::string>(config["output_file"]);
            }
            
            if (config.contains("enable_rotation")) {
                pyConfig.enableRotation = py::cast<bool>(config["enable_rotation"]);
            }
            
            if (config.contains("max_file_size_mb")) {
                pyConfig.maxFileSizeMB = py::cast<size_t>(config["max_file_size_mb"]);
            }
            
            if (config.contains("max_files")) {
                pyConfig.maxFiles = py::cast<size_t>(config["max_files"]);
            }
            
            if (config.contains("rotate_daily")) {
                pyConfig.rotateDaily = py::cast<bool>(config["rotate_daily"]);
            }
            
            if (config.contains("compress_logs")) {
                pyConfig.compressLogs = py::cast<bool>(config["compress_logs"]);
            }
            
            // Enable/disable various logging features
            if (config.contains("enable_performance_logging")) {
                pyConfig.enablePerformanceLogging = py::cast<bool>(config["enable_performance_logging"]);
            }
            
            if (config.contains("enable_memory_logging")) {
                pyConfig.enableMemoryLogging = py::cast<bool>(config["enable_memory_logging"]);
            }
            
            if (config.contains("enable_debug_logging")) {
                pyConfig.enableDebugLogging = py::cast<bool>(config["enable_debug_logging"]);
            }
            
            if (config.contains("enable_trace_logging")) {
                pyConfig.enableTraceLogging = py::cast<bool>(config["enable_trace_logging"]);
            }
            
            // Apply configuration
            applyConfiguration(pyConfig);
        }
        
        // Get current log statistics
        static py::dict getLogStatistics() {
            auto& logger = getGlobalAdvancedLogger();
            auto stats = logger.getLogStats();
            
            py::dict result;
            
            // Basic statistics
            result["total_logs"] = static_cast<long>(stats.totalLogs);
            
            // Statistics by level
            py::dict byLevel;
            byLevel["debug"] = static_cast<long>(stats.logsByLevel[0]);
            byLevel["info"] = static_cast<long>(stats.logsByLevel[1]);
            byLevel["warning"] = static_cast<long>(stats.logsByLevel[2]);
            byLevel["error"] = static_cast<long>(stats.logsByLevel[3]);
            byLevel["critical"] = static_cast<long>(stats.logsByLevel[4]);
            
            result["by_level"] = byLevel;
            
            // Calculate logs per second
            if (stats.totalLogs > 0 && stats.firstLog != stats.lastLog) {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - stats.firstLog).count();
                if (duration > 0) {
                    result["logs_per_second"] = static_cast<double>(stats.totalLogs) / duration;
                }
            }
            
            // Calculate uptime
            auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - stats.firstLog).count();
            result["uptime_seconds"] = uptime;
            
            // Memory statistics
            if (stats.totalLogs > 0) {
                // Estimate log size
                result["estimated_size_kb"] = static_cast<size_t>(stats.totalLogs * 100); // ~100 bytes per log
            }
            
            return result;
        }
        
        // Set log level for specific module
        static void setModuleLevel(const std::string& module, LogLevel level) {
            auto& logger = getGlobalAdvancedLogger();
            logger.pImpl->componentLogger->setModuleLevel(module, level);
        }
        
        // Enable/disable module
        static void setModuleEnabled(const std::string& module, bool enabled) {
            auto& logger = getGlobalAdvancedLogger();
            logger.pImpl->componentLogger->setModuleEnabled(module, enabled);
        }
        
        // Get module status
        static py::dict getModuleStatus(const std::string& module) {
            auto& logger = getGlobalAdvancedLogger();
            
            py::dict result;
            result["module"] = module;
            result["enabled"] = logger.pImpl->componentLogger->isModuleEnabled(module);
            result["level"] = static_cast<int>(logger.pImpl->componentLogger->getModuleLevel(module));
            
            return result;
        }
        
        // Log performance event
        static void logPerformance(const std::string& operation, double duration,
                                  const py::dict& metadata = py::dict()) {
            std::map<std::string, std::string> metaMap;
            
            for (auto item : metadata) {
                metaMap[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
            }
            
            auto& logger = getGlobalAdvancedLogger();
            logger.logPerformance(operation, duration, metaMap);
        }
        
        // Log memory usage
        static void logMemoryUsage(size_t allocated, size_t available) {
            auto& logger = getGlobalAdvancedLogger();
            logger.logMemoryUsage(allocated, available);
        }
        
        // Set network logging endpoint
        static void setNetworkLogEndpoint(const std::string& endpoint) {
            auto& logger = getGlobalAdvancedLogger();
            logger.setNetworkLogEndpoint(endpoint);
        }
        
        // Get all module statuses
        static py::list getAllModuleStatuses() {
            auto& logger = getGlobalAdvancedLogger();
            py::list result;
            
            // This would iterate through all registered modules
            // For now, return common modules
            std::vector<std::string> commonModules = {"CORE", "BRAIN", "WORLD", "AGENT", "PERFORMANCE", "MEMORY", "NETWORK"};
            
            for (const auto& module : commonModules) {
                result.append(getModuleStatus(module));
            }
            
            return result;
        }
        
        // Clear logs
        static void clearLogs() {
            auto& logger = getGlobalAdvancedLogger();
            logger.pImpl->componentLogger->clearLoggers();
        }
        
        // Reset log statistics
        static void resetStatistics() {
            auto& logger = getGlobalAdvancedLogger();
            logger.resetLogStats();
        }
        
        // Get current log level threshold
        static LogLevel getLogLevelThreshold() {
            auto& logger = getGlobalAdvancedLogger();
            return logger.pImpl->levelThreshold;
        }
        
        // Set log level threshold
        static void setLogLevelThreshold(LogLevel level) {
            auto& logger = getGlobalAdvancedLogger();
            logger.setLogLevelThreshold(level);
        }
        
        // Check if log level is enabled
        static bool isLogLevelEnabled(LogLevel level) {
            auto& logger = getGlobalAdvancedLogger();
            return logger.isLogLevelEnabled(level);
        }
        
    private:
        static void applyConfiguration(const PythonLogConfig& config) {
            auto& logger = getGlobalAdvancedLogger();
            auto consoleLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(logger.pImpl->componentLogger);
            
            if (consoleLogger) {
                consoleLogger->setLevel(config.level);
                consoleLogger->setUseColors(config.useColors);
                
                if (!config.outputFile.empty()) {
                    LogRotationConfig rotationConfig;
                    rotationConfig.maxSizeMB = config.maxFileSizeMB;
                    rotationConfig.maxFiles = config.maxFiles;
                    rotationConfig.rotateDaily = config.rotateDaily;
                    rotationConfig.compressOldLogs = config.compressLogs;
                    
                    consoleLogger->setOutputFile(config.outputFile, rotationConfig);
                }
                
                // Set module filters
                if (!config.module.empty()) {
                    consoleLogger->addModuleFilter(config.module);
                }
                
                // Set module levels
                if (!config.module.empty()) {
                    consoleLogger->setModuleLevel(config.module, config.level);
                }
            }
        }
    };
    
    // Register Python logging module
    void registerPythonLogging(py::module& m) {
        py::class_<PythonLogger>(m, "PythonLogger")
            .def(py::init<>())
            .def_static("configure", &PythonLogger::configure)
            .def_static("getLogStatistics", &PythonLogger::getLogStatistics)
            .def_static("setModuleLevel", &PythonLogger::setModuleLevel)
            .def_static("setModuleEnabled", &PythonLogger::setModuleEnabled)
            .def_static("getModuleStatus", &PythonLogger::getModuleStatus)
            .def_static("logPerformance", &PythonLogger::logPerformance)
            .def_static("logMemoryUsage", &PythonLogger::logMemoryUsage)
            .def_static("setNetworkLogEndpoint", &PythonLogger::setNetworkLogEndpoint)
            .def_static("getAllModuleStatuses", &PythonLogger::getAllModuleStatuses)
            .def_static("clearLogs", &PythonLogger::clearLogs)
            .def_static("resetStatistics", &PythonLogger::resetStatistics)
            .def_static("getLogLevelThreshold", &PythonLogger::getLogLevelThreshold)
            .def_static("setLogLevelThreshold", &PythonLogger::setLogLevelThreshold)
            .def_static("isLogLevelEnabled", &PythonLogger::isLogLevelEnabled);
        
        // Python-friendly log level enum
        py::enum_<LogLevel>(m, "LogLevel")
            .value("DEBUG", LogLevel::Debug)
            .value("INFO", LogLevel::Info)
            .value("WARNING", LogLevel::Warning)
            .value("ERROR", LogLevel::Error)
            .value("CRITICAL", LogLevel::Critical);
        
        // Register global logger function
        m.def("get_global_logger", &getGlobalAdvancedLogger, "Get the global advanced logger instance");
        
        // Convenience functions
        m.def("configure_logging", &PythonLogger::configure, "Configure logging from Python dictionary");
        m.def("get_logging_stats", &PythonLogger::getLogStatistics, "Get current logging statistics");
        m.def("log_event", [](const std::string& event, const py::dict& metadata) {
            // Convert py::dict to std::map<std::string, std::string>
            std::map<std::string, std::string> metaMap;
            for (auto item : metadata) {
                metaMap[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
            }
            
            auto& logger = getGlobalAdvancedLogger();
            logger.logStructured(LogLevel::Info, event, metaMap, "APPLICATION");
        }, "Log an event with metadata");
        
        m.def("log_performance", &PythonLogger::logPerformance, "Log performance metrics");
        m.def("log_memory", &PythonLogger::logMemoryUsage, "Log memory usage statistics");
    }
    
} // namespace python_logging

} // namespace nlm
