// Phase 1: Python Binding Template for NLM
// This file demonstrates the structure for Python bindings
// Phase 2: Complete implementation with pybind11

#pragma once
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <string>

namespace py = pybind11;

// Forward declarations
class PyBrain;
class PyConfig;
class PyLogger;

// Python bindings for NLM
PYBIND11_MODULE(pynlm, m) {
    m.doc() = "Python bindings for NLM (Neural Learning Machine)";
    
    // Version
    m.attr("__version__") = "0.1.0";
    
    // Enum bindings
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();
    
    py::enum_<nlm::SynapseType>(m, "SynapseType")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();
    
    py::enum_<nlm::LogLevel>(m, "LogLevel")
        .value("Debug", nlm::LogLevel::Debug)
        .value("Info", nlm::LogLevel::Info)
        .value("Warning", nlm::LogLevel::Warning)
        .value("Error", nlm::LogLevel::Error)
        .value("Critical", nlm::LogLevel::Critical)
        .export_values();
    
    // Configuration class
    py::class_<nlm::Config, std::shared_ptr<nlm::Config>>(m, "Config")
        .def(py::init<>())
        .def("load_from_file", &nlm::Config::loadFromFile)
        .def("load_from_args", &nlm::Config::loadFromArgs)
        .def("save_to_file", &nlm::Config::saveToFile)
        .def("set", static_cast<void(nlm::Config::*)(const std::string&, const std::string&, nlm::ConfigSource)>(&nlm::Config::set))
        .def("set", static_cast<void(nlm::Config::*)(const std::string&, int64_t, nlm::ConfigSource)>(&nlm::Config::set))
        .def("set", static_cast<void(nlm::Config::*)(const std::string&, double, nlm::ConfigSource)>(&nlm::Config::set))
        .def("set", static_cast<void(nlm::Config::*)(const std::string&, bool, nlm::ConfigSource)>(&nlm::Config::set))
        .def("get", static_cast<std::optional<std::string>(nlm::Config::*)(const std::string&) const>(&nlm::Config::get<std::string>))
        .def("get_or", &nlm::Config::template getOr<std::string>)
        .def("has", &nlm::Config::has)
        .def("clear", &nlm::Config::clear)
        .def("get_keys", &nlm::Config::getKeys)
        .def("summary", &nlm::Config::summary);
    
    // Logger class
    py::class_<nlm::Logger, std::shared_ptr<nlm::Logger>>(m, "Logger")
        .def(py::init<>())
        .def("add_logger", &nlm::Logger::addLogger)
        .def("clear_loggers", &nlm::Logger::clearLoggers)
        .def("set_level", &nlm::Logger::setLevel)
        .def("info", &nlm::Logger::info)
        .def("debug", &nlm::Logger::debug)
        .def("warning", &nlm::Logger::warning)
        .def("error", &nlm::Logger::error)
        .def("critical", &nlm::Logger::critical)
        .def("is_enabled", &nlm::Logger::isEnabled)
        .def_static("get_global", &nlm::Logger::getGlobal)
        .def_static("set_global", &nlm::Logger::setGlobal);
    
    // LogEntry structure
    py::class_<nlm::LogEntry>(m, "LogEntry")
        .def_readonly("level", &nlm::LogEntry::level)
        .def_readonly("message", &nlm::LogEntry::message)
        .def_readonly("file", &nlm::LogEntry::file)
        .def_readonly("line", &nlm::LogEntry::line)
        .def_readonly("function", &nlm::LogEntry::function);
    
    // ConsoleLogger class
    py::class_<nlm::ConsoleLogger, std::shared_ptr<nlm::ConsoleLogger>>(m, "ConsoleLogger")
        .def(py::init<>())
        .def(py::init<nlm::LogLevel>())
        .def("set_level", &nlm::ConsoleLogger::setLevel)
        .def("get_level", &nlm::ConsoleLogger::getLevel)
        .def("set_use_colors", &nlm::ConsoleLogger::setUseColors)
        .def("get_use_colors", &nlm::ConsoleLogger::getUseColors)
        .def("set_output_file", &nlm::ConsoleLogger::setOutputFile)
        .def("close_output_file", &nlm::ConsoleLogger::closeOutputFile);
    
    // Helper function for creating brain with configuration
    m.def("create_brain", [](const std::string& config_file) {
        auto config = std::make_shared<nlm::Config>();
        if (config->loadFromFile(config_file)) {
            return std::make_shared<nlm::Brain>(config);
        }
        return std::shared_ptr<nlm::Brain>(nullptr);
    }, "Create NLM brain with configuration file");
    
    // Simulation utility functions
    m.def("run_simple_simulation", [](std::shared_ptr<nlm::Brain> brain, int steps) {
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        return brain->getTotalSpikeCount();
    }, "Run simple simulation for specified number of steps");
    
    // Configuration helper functions
    m.def("get_default_config", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", int64_t(42), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", int64_t(500), nlm::ConfigSource::Default);
        config->set("region_count", int64_t(1), nlm::ConfigSource::Default);
        return config;
    }, "Create default configuration");
    
    // Error handling
    py::register_exception<std::runtime_error>(m, "NLMError");
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}
