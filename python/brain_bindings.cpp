#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Error.hpp"
#include "../src/core/Validation/Validation.hpp"

namespace py = pybind11;

// Helper function to translate NLM exceptions to Python exceptions
void translate_nlm_exceptions() {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm::ConfigurationException& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const nlm::MemoryManagementException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm::SystemStateException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm::ResourceAccessException& e) {
            PyErr_SetString(PyExc_IOError, e.what());
        } catch (const nlm::PythonBindingException& e) {
            PyErr_SetString(PyExc_TypeError, e.what());
        } catch (const nlm::NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::bad_variant_access& e) {
            PyErr_SetString(PyExc_ValueError, "Invalid configuration value type");
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

// Helper function for safe Python type conversions
size_t safe_convert_to_size_t(const py::object& obj, const std::string& paramName) {
    try {
        if (py::isinstance<py::int_>(obj)) {
            long val = obj.cast<long>();
            if (val < 0) {
                throw nlm::InvalidArgumentException(
                    "Parameter '" + paramName + "': value cannot be negative: " + std::to_string(val),
                    paramName
                ).withSuggestion("Convert to positive integer");
            }
            if (val > std::numeric_limits<size_t>::max()) {
                throw nlm::ConfigurationException(
                    "Value too large for size_t: " + std::to_string(val),
                    paramName
                ).withSuggestion("Use a smaller value");
            }
            return static_cast<size_t>(val);
        } else {
            throw nlm::PythonBindingException(
                "Expected integer for parameter '" + paramName + "', got " + std::string(py::repr(obj)),
                "type"
            ).withSuggestion("Convert parameter to integer type");
        }
    } catch (const nlm::NLMException& e) {
        throw;
    } catch (const std::exception& e) {
        throw nlm::PythonBindingException(
            "Error converting parameter '" + paramName + "': " + std::string(e.what()),
            std::string(py::repr(obj))
        ).withSuggestion("Check parameter type and value");
    }
}

// Helper function for safe Python string conversions
std::string safe_convert_to_string(const py::object& obj, const std::string& paramName) {
    try {
        return obj.cast<std::string>();
    } catch (...) {
        throw nlm::PythonBindingException(
            "Expected string for parameter '" + paramName + "'",
            "type"
        ).withSuggestion("Convert parameter to string type");
    }
}

// Helper function for safe Python float conversions
float safe_convert_to_float(const py::object& obj, const std::string& paramName) {
    try {
        double val = obj.cast<double>();
        if (val < 0.0 || val > 1.0) {
            throw nlm::InvalidArgumentException(
                "Parameter '" + paramName + "': value " + std::to_string(val) +
                " is out of expected range [0.0, 1.0]",
                paramName
            ).withSuggestion("Check parameter value is within expected range");
        }
        return static_cast<float>(val);
    } catch (...) {
        throw nlm::PythonBindingException(
            "Expected float/double for parameter '" + paramName + "'",
            "type"
        ).withSuggestion("Convert parameter to float/double type");
    }
}

void bind_brain(py::module_& m) {
    // Register exception translator
    translate_nlm_exceptions();
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        try {
            auto config = std::make_shared<Config>();
            NLM_LOG_INFO("Python: Created default configuration");
            return config;
        } catch (...) {
            throw nlm::PythonBindingException(
                "Failed to create default configuration in Python",
                "createDefaultConfig"
            ).withSuggestion("Check system resources and try again");
        }
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        // Validate config parameter
        if (!config) {
            throw nlm::InvalidArgumentException(
                "Config parameter cannot be null",
                "config"
            ).withSuggestion("Create a valid Config object using createDefaultConfig()");
        }
        
        try {
            auto brain = std::make_shared<Brain>(config);
            NLM_LOG_INFO("Python: Created brain with configuration");
            return brain;
        } catch (const nlm::NLMException& e) {
            throw;
        } catch (const std::exception& e) {
            throw nlm::PythonBindingException(
                "Failed to create brain: " + std::string(e.what()),
                "createBrain"
            ).withSuggestion("Check configuration and try again");
        }
    }, py::arg("config"), "Create a new brain with configuration");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration. Throws ValueError on invalid config.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True on success, False on error.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. Throws ValueError if step parameters are invalid.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. Throws ValueError if parameters are invalid.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Throws TypeError if input type is invalid.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Throws ValueError if parameters are invalid.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type. Throws ValueError if parameters are invalid.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset,
             "Reset brain state. Throws RuntimeError if reset fails.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Throws IOError on save failure.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Throws IOError on load failure.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Throws ValueError if region name is invalid.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns None if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage. Throws ValueError if stage is invalid.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status.");
    
    // Additional Pythonic features for better user experience
    m.def("createBrainWithParams", [](py::kwargs kwargs) {
        auto config = std::make_shared<Config>();
        
        // Pythonic parameter setting with validation
        if (kwargs.contains("neuron_count")) {
            try {
                size_t count = kwargs["neuron_count"].cast<size_t>();
                config->set("neuron_count", static_cast<int64_t>(count), ConfigSource::Default);
            } catch (...) {
                throw nlm::PythonBindingException(
                    "Invalid neuron_count parameter",
                    "createBrainWithParams"
                ).withSuggestion("neuron_count must be a positive integer");
            }
        }
        
        if (kwargs.contains("region_count")) {
            try {
                size_t count = kwargs["region_count"].cast<size_t>();
                config->set("region_count", static_cast<int64_t>(count), ConfigSource::Default);
            } catch (...) {
                throw nlm::PythonBindingException(
                    "Invalid region_count parameter",
                    "createBrainWithParams"
                ).withSuggestion("region_count must be a positive integer");
            }
        }
        
        if (kwargs.contains("simulation_timestep")) {
            try {
                double timestep = kwargs["simulation_timestep"].cast<double>();
                config->set("simulation_timestep", timestep, ConfigSource::Default);
            } catch (...) {
                throw nlm::PythonBindingException(
                    "Invalid simulation_timestep parameter",
                    "createBrainWithParams"
                ).withSuggestion("simulation_timestep must be a positive number");
            }
        }
        
        return std::make_shared<Brain>(config);
    }, "Create a brain with Pythonic keyword arguments for convenience.");
    
    // Python-friendly configuration helpers
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        try {
            if (!config) {
                throw nlm::InvalidArgumentException("Config cannot be null", "config");
            }
            
            // Basic validation
            bool valid = true;
            std::string errors;
            
            if (config->has("neuron_count")) {
                if (auto countOpt = config->get<int64_t>("neuron_count")) {
                    if (*countOpt <= 0) {
                        errors += "neuron_count must be positive; ";
                        valid = false;
                    }
                }
            }
            
            if (config->has("region_count")) {
                if (auto countOpt = config->get<int64_t>("region_count")) {
                    if (*countOpt <= 0) {
                        errors += "region_count must be positive; ";
                        valid = false;
                    }
                }
            }
            
            if (config->has("simulation_timestep")) {
                if (auto timestepOpt = config->get<double>("simulation_timestep")) {
                    if (*timestepOpt <= 0.0) {
                        errors += "simulation_timestep must be positive; ";
                        valid = false;
                    }
                }
            }
            
            if (!valid) {
                throw nlm::ConfigurationException(
                    "Configuration validation failed: " + errors,
                    "validateConfig"
                ).withSuggestion("Fix configuration errors and try again");
            }
            
            return true;
            
        } catch (const nlm::NLMException& e) {
            throw;
        } catch (const std::exception& e) {
            throw nlm::PythonBindingException(
                "Error validating configuration: " + std::string(e.what()),
                "validateConfig"
            );
        }
    }, py::arg("config"), "Validate configuration and return True if valid. Throws ValueError on invalid config.");
    
    // Error handling utilities for Python users
    m.def("getNLMVersion", []() {
        return "Phase 6.2 - Enhanced Error Handling";
    }, "Get NLM version string.");
    
    m.def("getErrorInfo", [](const py::object& exception) {
        py::dict info;
        
        try {
            std::string msg;
            if (exception.is_none()) {
                return info;
            }
            
            // Extract exception information for debugging
            info["type"] = std::string(py::str(py::type(exception).attr("__name__")));
            info["message"] = std::string(py::str(exception));
            info["timestamp"] = std::chrono::system_clock::now();
            
            return info;
            
        } catch (...) {
            return info;
        }
    }, py::arg("exception"), "Get error information for debugging.")
    .doc() = "Extract error information for debugging purposes.";
    
    // Python context manager for error handling
    py::class_<py::error_already_set>(m, "ErrorContext")
        .def(py::init<>(), "Context manager for handling Python exceptions")
        .def("__enter__", []() { return py::none(); })
        .def("__exit__", [](py::object, py::object, py::object, py::object) {
            // Clear any existing Python exceptions
            PyErr_Clear();
            return true;
        });
}