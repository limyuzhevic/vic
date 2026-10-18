#pragma once

// Python bindings for NLM (Neural Learning Machine)
// Provides Python API for easy integration and scripting

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "core/Profiler.hpp"

namespace py = pybind11;

// Python bindings for Config class
void bindConfig(py::module& m) {
    py::class_<nlm::Config> config_class(m, "Config");
    
    config_class
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, "Load configuration from file")
        .def("loadFromArgs", &nlm::Config::loadFromArgs, "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, "Save configuration to file")
        .def("get", &nlm::Config::get<std::string>, "Get string value by key")
        .def("getOr", &nlm::Config::getOr<std::string>, "Get string value with default")
        .def("set", py::overload_cast<const std::string&, const std::string&, nlm::ConfigSource>(
                &nlm::Config::set), "Set string value")
        .def("set", py::overload_cast<const std::string&, int, nlm::ConfigSource>(
                &nlm::Config::set), "Set int value")
        .def("set", py::overload_cast<const std::string&, double, nlm::ConfigSource>(
                &nlm::Config::set), "Set double value")
        .def("set", py::overload_cast<const std::string&, bool, nlm::ConfigSource>(
                &nlm::Config::set), "Set bool value")
        .def("has", &nlm::Config::has, "Check if key exists")
        .def("remove", &nlm::Config::remove, "Remove key")
        .def("getKeys", &nlm::Config::getKeys, "Get all keys")
        .def("summary", &nlm::Config::summary, "Get configuration summary")
        .def("validate", &nlm::Config::validate, "Validate configuration")
        .def("equals", &nlm::Config::equals, "Check if equal to another config");
    
    // Enum for ConfigSource
    py::enum_<nlm::ConfigSource>(m, "ConfigSource")
        .value("Default", nlm::ConfigSource::Default)
        .value("File", nlm::ConfigSource::File)
        .value("CommandLine", nlm::ConfigSource::CommandLine)
        .value("Runtime", nlm::ConfigSource::Runtime);
}

// Python bindings for Logger class
void bindLogger(py::module& m) {
    py::class_<nlm::Logger> logger_class(m, "Logger");
    
    logger_class
        .def_static("getGlobal", &nlm::Logger::getGlobal, "Get global logger instance")
        .def("info", &nlm::Logger::info, "Log info message")
        .def("error", &nlm::Logger::error, "Log error message")
        .def("debug", &nlm::Logger::debug, "Log debug message")
        .def("warning", &nlm::Logger::warning, "Log warning message")
        .def("critical", &nlm::Logger::critical, "Log critical message")
        .def("setLevel", &nlm::Logger::setLevel, "Set log level")
        .def("isEnabled", &nlm::Logger::isEnabled, "Check if level is enabled");
    
    // Enum for LogLevel
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("Debug", nlm::LogLevel::Debug)
        .value("Info", nlm::LogLevel::Info)
        .value("Warning", nlm::LogLevel::Warning)
        .value("Error", nlm::LogLevel::Error)
        .value("Critical", nlm::LogLevel::Critical);
}

// Python bindings for Brain class
void bindBrain(py::module& m) {
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize, "Initialize brain")
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step), "Step simulation")
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step), "Step simulation with time")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount, "Get total spike count")
        .def("reset", &nlm::Brain::reset, "Reset brain")
        .def("logStatus", &nlm::Brain::logStatus, "Log brain status");
}

// Python bindings for Profiler
void bindProfiler(py::module& m) {
    py::class_<nlm::Profiler> profiler_class(m, "Profiler");
    
    profiler_class
        .def("setEnabled", &nlm::Profiler::setEnabled, "Enable/disable profiler")
        .def("setMemoryEnabled", &nlm::Profiler::setMemoryEnabled, "Enable/disable memory profiling")
        .def("setPerformanceEnabled", &nlm::Profiler::setPerformanceEnabled, "Enable/disable performance profiling")
        .def("setLeakDetectionEnabled", &nlm::Profiler::setLeakDetectionEnabled, "Enable/disable leak detection")
        .def("startTimer", &nlm::Profiler::startTimer, "Start timer")
        .def("stopTimer", &nlm::Profiler::stopTimer, "Stop timer")
        .def("getProfileReport", &nlm::Profiler::getProfileReport, "Get profiling report")
        .def("reset", &nlm::Profiler::reset, "Reset profiler")
        .def("hasLeaks", &nlm::Profiler::hasLeaks, "Check for memory leaks");
}

// Python bindings for SimulationPresets
void bindSimulationPresets(py::module& m) {
    py::class_<nlm::SimulationPresets> presets_class(m, "SimulationPresets");
    
    presets_class
        .def(py::init<>())
        .def("getPresetNames", &nlm::SimulationPresets::getPresetNames, "Get all preset names")
        .def("getPresetDescription", &nlm::SimulationPresets::getPresetDescription, "Get preset description")
        .def("getPresetConfig", &nlm::SimulationPresets::getPresetConfig, "Get preset configuration")
        .def("applyPreset", &nlm::SimulationPresets::applyPreset, "Apply preset to configuration")
        .def("getPresetParameter", &nlm::SimulationPresets::getPresetParameter, "Get preset parameter");
}

// Python bindings for BatchProcessor
void bindBatchProcessor(py::module& m) {
    py::class_<nlm::BatchJob>(m, "BatchJob")
        .def(py::init<>())
        .def_readwrite("name", &nlm::BatchJob::name)
        .def_readwrite("description", &nlm::BatchJob::description)
        .def_readwrite("parameters", &nlm::BatchJob::parameters)
        .def_readwrite("outputFile", &nlm::BatchJob::outputFile)
        .def_readwrite("enabled", &nlm::BatchJob::enabled);
    
    py::class_<nlm::BatchResult>(m, "BatchResult")
        .def(py::init<>())
        .def_readwrite("jobName", &nlm::BatchResult::jobName)
        .def_readwrite("success", &nlm::BatchResult::success)
        .def_readwrite("output", &nlm::BatchResult::output)
        .def_readwrite("executionTime", &nlm::BatchResult::executionTime)
        .def_readwrite("neuronCount", &nlm::BatchResult::neuronCount)
        .def_readwrite("totalSteps", &nlm::BatchResult::totalSteps)
        .def_readwrite("errorMessage", &nlm::BatchResult::errorMessage);
    
    py::class_<nlm::BatchProcessor> batch_processor_class(m, "BatchProcessor");
    
    batch_processor_class
        .def(py::init<>())
        .def("processJob", &nlm::BatchProcessor::processJob, "Process a single batch job")
        .def("processJobs", &nlm::BatchProcessor::processJobs, "Process multiple batch jobs")
        .def("generateReport", &nlm::BatchProcessor::generateReport, "Generate batch report")
        .def("saveResults", &nlm::BatchProcessor::saveResults, "Save results to file")
        .def("getProgress", &nlm::BatchProcessor::getProgress, "Get processing progress");
}

// Main module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine)";
    
    // Bind core classes
    bindConfig(m);
    bindLogger(m);
    bindBrain(m);
    bindProfiler(m);
    bindSimulationPresets(m);
    bindBatchProcessor(m);
    
    // Expose helper functions
    m.def("getProfilingReport", &nlm::getProfilingReport, "Get profiling report");
    m.def("createDefaultConfig", []() { return nlm::Config(); }, "Create default configuration");
    m.def("createLogger", []() { return std::make_shared<nlm::Logger>(); }, "Create logger instance");
    
    // Expose version information
    m.attr("__version__") = "2.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Neural Learning Machine - Advanced Neural Simulation Framework";
}

// Macro for Python extension
PYBIND11_PLUGIN_IMPL