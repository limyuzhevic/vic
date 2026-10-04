// Python bindings for logging functionality
// Provides logging, debugging, and monitoring capabilities for NLM agents

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <string>

namespace py = pybind11;

PYBIND11_MODULE(pynlm_logging, m) {
    m.doc() = "NLM Logging module - Python bindings for logging and monitoring";

    // Log levels
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("TRACE", nlm::LogLevel::TRACE)
        .value("DEBUG", nlm::LogLevel::DEBUG)
        .value("INFO", nlm::LogLevel::INFO)
        .value("WARN", nlm::LogLevel::WARN)
        .value("ERROR", nlm::LogLevel::ERROR)
        .value("FATAL", nlm::LogLevel::FATAL)
        .export_values();

    // Logger class
    py::class_<nlm::Logger>(m, "Logger")
        .def(py::init<>(), "Create a new logger instance")
        .def("setLevel", &nlm::Logger::setLevel, py::arg("level"),
             "Set the minimum log level for this logger")
        .def("getLevel", &nlm::Logger::getLevel, "Get the current log level")
        .def("setPattern", &nlm::Logger::setPattern, py::arg("pattern"),
             "Set the log message pattern (e.g., '%Y-%m-%d %H:%M:%S %L %M')")
        .def("log", &nlm::Logger::log, py::arg("level"), py::arg("message"),
             "Log a message at the specified level")
        .def("trace", &nlm::Logger::trace, py::arg("message"),
             "Log a trace message")
        .def("debug", &nlm::Logger::debug, py::arg("message"),
             "Log a debug message")
        .def("info", &nlm::Logger::info, py::arg("message"),
             "Log an info message")
        .def("warn", &nlm::Logger::warn, py::arg("message"),
             "Log a warning message")
        .def("error", &nlm::Logger::error, py::arg("message"),
             "Log an error message")
        .def("fatal", &nlm::Logger::fatal, py::arg("message"),
             "Log a fatal message")
        .def("logWithContext", &nlm::Logger::logWithContext,
             py::arg("level"), py::arg("message"), py::arg("context"),
             "Log a message with additional context information")
        .def("isLevelEnabled", &nlm::Logger::isLevelEnabled, py::arg("level"),
             "Check if a log level is enabled")
        .def("flush", &nlm::Logger::flush, "Flush all pending log messages")
        .def("getLogCount", &nlm::Logger::getLogCount,
             "Get the total number of log messages")
        .def("getLogCountByLevel", &nlm::Logger::getLogCountByLevel,
             py::arg("level"), "Get log count by level")
        .def("clearLogHistory", &nlm::Logger::clearLogHistory,
             "Clear the log history")
        .def("getRecentLogs", &nlm::Logger::getRecentLogs, py::arg("limit") = 10,
             "Get recent log messages (default 10)")
        .def("enableConsoleOutput", &nlm::Logger::enableConsoleOutput,
             py::arg("enable") = true, "Enable or disable console output")
        .def("enableFileOutput", &nlm::Logger::enableFileOutput,
             py::arg("filepath"), "Enable file output with specified path")
        .def("disableFileOutput", &nlm::Logger::disableFileOutput,
             "Disable file output")
        .def("getLogFilePath", &nlm::Logger::getLogFilePath,
             "Get the current log file path");

    // Global logger functions
    m.def("getGlobalLogger", &nlm::Logger::getGlobalLogger,
          "Get the global logger instance");
    m.def("setGlobalLogLevel", &nlm::Logger::setLevel, py::arg("level"),
          "Set the global log level");
    m.def("getGlobalLogLevel", &nlm::Logger::getLevel,
          "Get the global log level");
    m.def("logInfo", &nlm::Logger::info, py::arg("message"),
          "Log info message globally");
    m.def("logError", &nlm::Logger::error, py::arg("message"),
          "Log error message globally");
    m.def("logWarning", &nlm::Logger::warn, py::arg("message"),
          "Log warning message globally");

    // Agent monitoring utilities
    m.def("monitorAgentActivity", &nlm::Logger::logWithContext,
          py::arg("agent_name"), py::arg("activity"), py::arg("level") = nlm::LogLevel::INFO,
          "Monitor agent activity with context");
    m.def("logSensoryInput", &nlm::Logger::logWithContext,
          py::arg("vision_data"), py::arg("touch_data"), py::arg("internal_data"),
          "Log sensory input data");
    m.def("logMotorOutput", &nlm::Logger::logWithContext,
          py::arg("action"), py::arg("dopamine_level"), py::arg("curiosity_level"),
          "Log motor output and neuromodulation levels");
    m.def("logDevelopmentStage", &nlm::Logger::logWithContext,
          py::arg("age"), py::arg("stage"), py::arg("plasticity"),
          "Log developmental stage information");

    // Performance monitoring
    m.def("profilePerformance", &nlm::Logger::logWithContext,
          py::arg("operation"), py::arg("duration"), py::arg("memory_usage"),
          "Profile performance metrics");
    m.def("logMemoryUsage", &nlm::Logger::logWithContext,
          py::arg("neuron_count"), py::arg("synapse_count"), py::arg("memory_mb"),
          "Log memory usage statistics");
}
