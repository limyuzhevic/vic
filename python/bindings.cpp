py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log level enumeration)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
        .export_values();

    py::class_<LogEntry>(m, "LogEntry", R"pbdoc(Log entry structure)pbdoc")
        .def_readonly("level", &LogEntry::level)
        .def_readonly("message", &LogEntry::message)
        .def_readonly("file", &LogEntry::file)
        .def_readonly("line", &LogEntry::line)
        .def_readonly("function", &LogEntry::function)
        .def_readonly("timestamp", &LogEntry::timestamp)
        .def("__repr__", [](const LogEntry& entry) {
            std::ostringstream oss;
            oss << "<LogEntry " << levelToString(entry.level) << ": " << entry.message << " at " << entry.file << ":" << entry.line << ">";
            return oss.str();
        });

    py::class_<ILogger, std::shared_ptr<ILogger>>(m, "ILogger", R"pbdoc(Base logger interface)pbdoc")
        .def("log", &ILogger::log, py::arg("entry"), "Log an entry")
        .def("setLevel", &ILogger::setLevel, py::arg("level"), "Set log level")
        .def("getLevel", &ILogger::getLevel, "Get current log level");

    py::class_<ConsoleLogger, std::shared_ptr<ConsoleLogger>>(m, "ConsoleLogger", R"pbdoc(Console logger with optional file output)pbdoc")
        .def(py::init<>())
        .def(py::init<LogLevel>(), py::arg("level"))
        .def("log", &ConsoleLogger::log, py::arg("entry"), "Log an entry")
        .def("setLevel", &ConsoleLogger::setLevel, py::arg("level"), "Set log level")
        .def("getLevel", &ConsoleLogger::getLevel, "Get current log level")
        .def("setUseColors", &ConsoleLogger::setUseColors, py::arg("useColors"), "Enable/disable colors")
        .def("getUseColors", &ConsoleLogger::getUseColors, "Get whether colors are enabled")
        .def("setOutputFile", &ConsoleLogger::setOutputFile, py::arg("filepath"), "Set output file")
        .def("closeOutputFile", &ConsoleLogger::closeOutputFile, "Close output file")
        .def("getLogLevel", [](const ConsoleLogger& logger) { return logger.getLevel(); }, "Get log level (alias for getLevel)")
        .def("__repr__", [](const ConsoleLogger& logger) {
            return "<ConsoleLogger level=" + levelToString(logger.getLevel()) + ">";
        });

    py::class_<Logger, std::shared_ptr<Logger>>(m, "Logger", R"pbdoc(Central logging system for NLM)pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"), "Add a logger sink")
        .def("clearLoggers", &Logger::clearLoggers, "Remove all loggers")
        .def("setLevel", &Logger::setLevel, py::arg("level"), "Set minimum log level for all loggers")
        
        // Convenience logging methods
        .def("log", [](Logger& self, LogLevel level, const std::string& message,
                      const std::string& file, int line, const std::string& function) {
            self.log(level, message, file, line, function);
        }, py::arg("level"), py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log a message with level")
        
        .def("debug", [](Logger& self, const std::string& message,
                        const std::string& file, int line, const std::string& function) {
            self.debug(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log a debug message")
        
        .def("info", [](Logger& self, const std::string& message,
                       const std::string& file, int line, const std::string& function) {
            self.info(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log an info message")
        
        .def("warning", [](Logger& self, const std::string& message,
                          const std::string& file, int line, const std::string& function) {
            self.warning(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log a warning message")
        
        .def("error", [](Logger& self, const std::string& message,
                        const std::string& file, int line, const std::string& function) {
            self.error(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log an error message")
        
        .def("critical", [](Logger& self, const std::string& message,
                           const std::string& file, int line, const std::string& function) {
            self.critical(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log a critical message")
        
        // Stream-style logging helper
        .def("logStream", [](Logger& self, LogLevel level) {
            return LogStream(level, "", 0, "");
        }, py::arg("level"), "Create a log stream for detailed formatting")
        
        // Log level checking
        .def("isEnabled", &Logger::isEnabled, py::arg("level"), "Check if a log level is enabled")
        
        // Global logger functions
        .def("getGlobal", &Logger::getGlobal, py::return_value_policy::reference_internal,
             "Get the global logger instance")
        .def("setGlobal", &Logger::setGlobal, py::arg("logger"),
             "Set the global logger instance")
        
        // Log level constants for convenience
        .attr("DEBUG") = LogLevel::Debug
        .attr("INFO") = LogLevel::Info
        .attr("WARNING") = LogLevel::Warning
        .attr("ERROR") = LogLevel::Error
        .attr("CRITICAL") = LogLevel::Critical
        
        // Convenience macros (these are already defined in the header, but we can expose them)
        .def("addLogMacros", [](py::module& m) {
            // Expose the macros as module-level constants if needed
        }, "Add logging macros to module")
        
        .def("__repr__", [](const Logger& logger) {
            return "<Logger>";
        });

    m.def("createLogger", []() -> std::shared_ptr<Logger> {
        return std::make_shared<Logger>();
    }, "Create a logger instance");

    m.def("getLogger", &Logger::getGlobal, py::return_value_policy::reference_internal,
          "Get the global logger instance (convenience function)");

    m.def("setLogger", &Logger::setGlobal, py::arg("logger"),
          "Set the global logger instance (convenience function)");

    // Helper functions for easy logging setup
    m.def("setupConsoleLogging", [](LogLevel level, bool useColors, const std::string& filepath = "") {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        consoleLogger->setUseColors(useColors);
        if (!filepath.empty()) {
            consoleLogger->setOutputFile(filepath);
        }
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        return logger;
    }, py::arg("level") = LogLevel::Info, py::arg("useColors") = true, py::arg("filepath") = "",
          "Set up console logging with optional file output");

    m.def("setupFileLogging", [](LogLevel level, const std::string& filepath) {
        auto logger = std::make_shared<Logger>();
        auto fileLogger = std::make_shared<ConsoleLogger>(level);
        fileLogger->setOutputFile(filepath);
        fileLogger->setUseColors(false); // Usually don't want colors in log files
        logger->addLogger(fileLogger);
        Logger::setGlobal(logger);
        return logger;
    }, py::arg("level") = LogLevel::Info, py::arg("filepath"),
          "Set up file-only logging");

    // Predefined log configuration presets
    m.def("createDebugConfig", []() -> std::shared_ptr<Logger> {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        return logger;
    }, "Create debug-level logging configuration");

    m.def("createInfoConfig", []() -> std::shared_ptr<Logger> {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        return logger;
    }, "Create info-level logging configuration");

    m.def("createProductionConfig", []() -> std::shared_ptr<Logger> {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        consoleLogger->setUseColors(false);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        return logger;
    }, "Create production-level logging configuration (warnings and errors only)");

    m.def("createSilentConfig", []() -> std::shared_ptr<Logger> {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        return logger;
    }, "Create silent logging configuration (errors only)");

    // Add logging macros as module-level convenience functions
    m.def("logDebug", [](const std::string& message) {
        NLM_LOG_DEBUG(message);
    }, py::arg("message"), "Debug logging macro");

    m.def("logInfo", [](const std::string& message) {
        NLM_LOG_INFO(message);
    }, py::arg("message"), "Info logging macro");

    m.def("logWarning", [](const std::string& message) {
        NLM_LOG_WARNING(message);
    }, py::arg("message"), "Warning logging macro");

    m.def("logError", [](const std::string& message) {
        NLM_LOG_ERROR(message);
    }, py::arg("message"), "Error logging macro");

    m.def("logCritical", [](const std::string& message) {
        NLM_LOG_CRITICAL(message);
    }, py::arg("message"), "Critical logging macro");

    // Add streaming log macro
    m.def("logDebugStream", []() {
        return NLM_LOG_STREAM(LogLevel::Debug);
    }, "Debug log stream macro");

    m.def("logInfoStream", []() {
        return NLM_LOG_STREAM(LogLevel::Info);
    }, "Info log stream macro");

    m.def("logWarningStream", []() {
        return NLM_LOG_STREAM(LogLevel::Warning);
    }, "Warning log stream macro");

    m.def("logErrorStream", []() {
        return NLM_LOG_STREAM(LogLevel::Error);
    }, "Error log stream macro");

    m.def("logCriticalStream", []() {
        return NLM_LOG_STREAM(LogLevel::Critical);
    }, "Critical log stream macro");