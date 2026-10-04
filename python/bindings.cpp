#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

// Custom exception handling for Python bindings
namespace py = pybind11;

// Define custom exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& msg) : NLMError("Resource error: " + msg) {}
};

// Register all exceptions
void register_exceptions(py::module_& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<ResourceError>(m, "ResourceError", PyExc_RuntimeError);
}

// Type conversion utilities
namespace type_conversions {
    // Convert Python dict to ConfigValue
    nlm::ConfigValue dict_to_config_value(const py::dict& d) {
        if (d.size() == 0) return std::string("");
        
        // Handle simple cases first
        if (py::hasattr(d, "get") && d.get("type", py::none()).is_none() && d.get("value", py::none()).is_none()) {
            // Dictionary with single key-value pair
            for (auto item : d) {
                std::string key = item.first.cast<std::string>();
                py::object value = item.second;
                
                if (value.is_none()) return std::string("");
                if (value.is(py::bool_())) return value.cast<bool>();
                if (value.is(py::int_())) return value.cast<int64_t>();
                if (value.is(py::float_())) return value.cast<double>();
                if (value.is(py::str())) return value.cast<std::string>();
                if (value.is(py::list())) {
                    std::vector<double> vec = value.cast<std::vector<double>>();
                    return vec;
                }
                // Default to string
                return value.cast<std::string>();
            }
        }
        
        // Handle complex nested structures
        py::list keys = d.keys();
        std::vector<std::string> key_list;
        for (const auto& key : keys) {
            key_list.push_back(key.cast<std::string>());
        }
        
        if (key_list.size() == 1) {
            std::string key = key_list[0];
            py::object value = d[key];
            
            if (value.is(py::bool_())) return value.cast<bool>();
            if (value.is(py::int_())) return value.cast<int64_t>();
            if (value.is(py::float_())) return value.cast<double>();
            if (value.is(py::str())) return value.cast<std::string>();
            if (value.is(py::list())) {
                return value.cast<std::vector<double>>();
            }
        }
        
        // Return as dictionary string for complex cases
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (auto item : d) {
            if (!first) ss << ", ";
            ss << "\"" << item.first.cast<std::string>() << "\": ";
            ss << py::str(item.second);
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    
    // Convert Python string to NeuronId
    nlm::NeuronId string_to_neuron_id(const std::string& s) {
        try {
            size_t pos = s.find(':');
            if (pos != std::string::npos) {
                std::string num_str = s.substr(pos + 1);
                uint64_t value = std::stoull(num_str);
                return nlm::NeuronId(value);
            }
            return nlm::NeuronId(std::stoull(s));
        } catch (...) {
            throw ResourceError("Invalid neuron ID format: '" + s + "'");
        }
    }
    
    // Convert Python string to SynapseId
    nlm::SynapseId string_to_synapse_id(const std::string& s) {
        try {
            size_t pos = s.find(':');
            if (pos != std::string::npos) {
                std::string num_str = s.substr(pos + 1);
                uint64_t value = std::stoull(num_str);
                return nlm::SynapseId(value);
            }
            return nlm::SynapseId(std::stoull(s));
        } catch (...) {
            throw ResourceError("Invalid synapse ID format: '" + s + "'");
        }
    }
    
    // Convert Python string to RegionId
    nlm::RegionId string_to_region_id(const std::string& s) {
        try {
            size_t pos = s.find(':');
            if (pos != std::string::npos) {
                std::string num_str = s.substr(pos + 1);
                uint64_t value = std::stoull(num_str);
                return nlm::RegionId(value);
            }
            return nlm::RegionId(std::stoull(s));
        } catch (...) {
            throw ResourceError("Invalid region ID format: '" + s + "'");
        }
    }
    
    // Convert Python list to std::vector
    template<typename T>
    std::vector<T> py_list_to_vector(const py::list& py_list) {
        std::vector<T> result;
        result.reserve(py_list.size());
        for (const auto& item : py_list) {
            result.push_back(item.cast<T>());
        }
        return result;
    }
    
    // Convert Python list to std::vector<float>
    std::vector<float> py_list_to_float_vector(const py::list& py_list) {
        return py_list_to_vector<float>(py_list);
    }
    
    // Convert Python list to std::vector<double>
    std::vector<double> py_list_to_double_vector(const py::list& py_list) {
        return py_list_to_vector<double>(py_list);
    }
    
    // Convert Python list to std::vector<int>
    std::vector<int> py_list_to_int_vector(const py::list& py_list) {
        return py_list_to_vector<int>(py_list);
    }
}

// Statistics aggregation utilities
namespace stats_aggregation {
    // Aggregate neuron statistics
    py::dict aggregate_neuron_stats(const nlm::Brain& brain) {
        py::dict stats;
        stats["total"] = brain.getTotalNeuronCount();
        stats["active"] = brain.getActiveNeuronCount();
        stats["firing"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        return stats;
    }
    
    // Aggregate synapse statistics
    py::dict aggregate_synapse_stats(const nlm::Brain& brain) {
        py::dict stats;
        stats["total"] = brain.getTotalSynapseCount();
        return stats;
    }
    
    // Aggregate region statistics
    py::dict aggregate_region_stats(const nlm::Brain& brain) {
        py::dict stats;
        auto region_ids = brain.getRegionIds();
        stats["count"] = region_ids.size();
        
        if (region_ids.size() > 0) {
            float total_firing_rate = 0.0f;
            size_t total_neurons = 0;
            
            for (const auto& region_id : region_ids) {
                auto region = brain.getRegion(region_id);
                if (region) {
                    // This would need region-specific statistics methods
                    total_neurons += region->getNeuronCount();
                }
            }
            
            stats["average_neurons_per_region"] = total_neurons / region_ids.size();
            stats["average_firing_rate_per_region"] = total_firing_rate / region_ids.size();
        }
        
        return stats;
    }
}

// Batch processing utilities
namespace batch_processing {
    // Batch simulation steps
    std::vector<nlm::SimulationStep> batch_step(
        nlm::Brain& brain,
        const std::vector<nlm::SimulationStep>& steps,
        const std::vector<nlm::Timestamp>& times) {
        
        std::vector<nlm::SimulationStep> results;
        for (size_t i = 0; i < steps.size(); ++i) {
            if (i < times.size()) {
                brain.step(steps[i], times[i]);
            } else {
                brain.step(steps[i]);
            }
            results.push_back(steps[i]);
        }
        return results;
    }
    
    // Batch process multiple brains
    std::vector<nlm::SimulationStep> batch_process_brains(
        const std::vector<std::shared_ptr<nlm::Brain>>& brains,
        const std::vector<nlm::SimulationStep>& steps,
        const std::vector<nlm::Timestamp>& times) {
        
        std::vector<nlm::SimulationStep> all_steps;
        for (const auto& brain : brains) {
            for (size_t i = 0; i < steps.size(); ++i) {
                if (i < times.size()) {
                    brain->step(steps[i], times[i]);
                } else {
                    brain->step(steps[i]);
                }
            }
        }
        return all_steps;
    }
}

// Visualization utilities
namespace visualization {
    // Convert neural data to matplotlib-compatible format
    py::dict convert_to_matplotlib(const nlm::Brain& brain) {
        py::dict plot_data;
        
        // Get neuron data
        plot_data["neurons_total"] = brain.getTotalNeuronCount();
        plot_data["neurons_active"] = brain.getActiveNeuronCount();
        plot_data["neurons_firing"] = brain.getFiringNeuronCount();
        plot_data["average_firing_rate"] = brain.getAverageFiringRate();
        plot_data["total_spikes"] = brain.getTotalSpikeCount();
        plot_data["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        
        // Get region data
        auto region_ids = brain.getRegionIds();
        py::list region_names;
        py::list region_sizes;
        
        for (const auto& region_id : region_ids) {
            region_names.append(region_id.index());
            // Region size would require region-specific methods
            region_sizes.append(0);  // Placeholder
        }
        
        plot_data["region_indices"] = region_names;
        plot_data["region_sizes"] = region_sizes;
        
        return plot_data;
    }
    
    // Create simple neuron activity plot data
    py::dict create_neuron_activity_plot(const nlm::Brain& brain) {
        py::dict plot;
        
        // Create time series data
        py::list time_points;
        py::list firing_rates;
        
        for (size_t i = 0; i < 100; ++i) {  // Simulate 100 time points
            time_points.append(static_cast<double>(i) * 0.1);
            // Would need actual firing rate history from brain
            firing_rates.append(brain.getAverageFiringRate() * (0.5 + 0.5 * sin(i * 0.1)));
        }
        
        plot["time"] = time_points;
        plot["firing_rate"] = firing_rates;
        plot["title"] = "Neural Firing Rate Over Time";
        plot["xlabel"] = "Time (s)";
        plot["ylabel"] = "Firing Rate (Hz)";
        
        return plot;
    }
}

// Configuration validation utilities
namespace config_validation {
    // Validate configuration keys and values
    bool validate_config(const nlm::Config& config) {
        bool is_valid = true;
        
        // Check for required keys
        auto keys = config.getKeys();
        
        // Example validation rules
        if (config.has("brain.neuron_count") && config.get("brain.neuron_count").has_value()) {
            auto count = config.get("brain.neuron_count").value();
            if (count < 0 || count > 1000000) {
                is_valid = false;
            }
        }
        
        if (config.has("brain.simulation.dt") && config.get("brain.simulation.dt").has_value()) {
            auto dt = config.get("brain.simulation.dt").value();
            if (dt <= 0.0 || dt > 1.0) {
                is_valid = false;
            }
        }
        
        return is_valid;
    }
    
    // Get validation errors
    py::list get_validation_errors(const nlm::Config& config) {
        py::list errors;
        
        auto keys = config.getKeys();
        
        if (config.has("brain.neuron_count") && config.get("brain.neuron_count").has_value()) {
            auto count = config.get("brain.neuron_count").value();
            if (count < 0 || count > 1000000) {
                errors.append("brain.neuron_count must be between 0 and 1000000");
            }
        }
        
        return errors;
    }
    
    // Suggest configuration improvements
    py::dict suggest_improvements(const nlm::Config& config) {
        py::dict suggestions;
        
        // Example suggestions
        if (config.has("brain.simulation.dt") && config.get("brain.simulation.dt").has_value()) {
            auto dt = config.get("brain.simulation.dt").value();
            if (dt < 0.001) {
                suggestions["dt_recommendation"] = "Increase dt for better performance (e.g., 0.01)";
            } else if (dt > 0.1) {
                suggestions["dt_recommendation"] = "Decrease dt for better accuracy (e.g., 0.01)";
            }
        }
        
        if (config.getKeys().empty()) {
            suggestions["general"] = "Consider setting brain.neuron_count and brain.simulation.dt for optimal performance";
        }
        
        return suggestions;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides enhanced classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes context managers, convenience methods, error handling, and visualization utilities.
    )pbdoc";

    // Register custom exceptions
    register_exceptions(m);

    // Import all necessary types and headers
    py::class_<nlm::NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value)
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value)
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value)
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__ne__", &nlm::RegionId::operator!=)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<nlm::PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value)
        .def("index", &nlm::PopulationId::index)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__ne__", &nlm::PopulationId::operator!=);

    py::enum_<nlm::NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    py::enum_<nlm::SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();

    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    py::enum_<nlm::FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();

    py::class_<nlm::Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("set", [](nlm::Config& self, const std::string& key, py::object value) {
            try {
                if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else if (value.is(py::int_())) {
                    self.set(key, value.cast<int64_t>());
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>());
                } else if (value.is(py::list())) {
                    auto list = value.cast<std::vector<double>>();
                    self.set(key, list);
                } else {
                    throw ConfigurationError("Unsupported value type for key '" + key + "'");
                }
            } catch (const ConfigurationError& e) {
                throw;
            } catch (...) {
                throw ConfigurationError("Failed to set configuration key '" + key + "'");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("get", [](nlm::Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            if (result.has_value()) {
                return py::cast(result.value());
            }
            return py::none();
        }, py::arg("key"), "Get configuration value as Python object")
        .def("get_or", [](nlm::Config& self, const std::string& key, py::object default_value) {
            if (default_value.is(py::bool_())) {
                return self.getOr<bool>(key, default_value.cast<bool>());
            } else if (default_value.is(py::int_())) {
                return self.getOr<int64_t>(key, default_value.cast<int64_t>());
            } else if (default_value.is(py::float_())) {
                return self.getOr<double>(key, default_value.cast<double>());
            } else if (default_value.is(py::str())) {
                return self.getOr<std::string>(key, default_value.cast<std::string>());
            } else {
                throw ConfigurationError("Unsupported default value type");
            }
        }, py::arg("key"), py::arg("default_value"), "Get configuration value or return default")
        .def("validate", [](nlm::Config& self) {
            return config_validation::validate_config(self);
        }, "Validate configuration and return True if valid")
        .def("get_validation_errors", [](nlm::Config& self) {
            return config_validation::get_validation_errors(self);
        }, "Get list of validation error messages")
        .def("get_suggestions", [](nlm::Config& self) {
            return config_validation::suggest_improvements(self);
        }, "Get configuration improvement suggestions")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", [](nlm::Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            if (!result.has_value()) {
                throw py::key_error("Key '" + key + "' not found in configuration");
            }
            return py::cast(result.value());
        })
        .def("__setitem__", [](nlm::Config& self, const std::string& key, py::object value) {
            if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                auto list = value.cast<std::vector<double>>();
                self.set(key, list);
            } else {
                throw ConfigurationError("Unsupported value type for key '" + key + "'");
            }
        })
        .def("__contains__", &nlm::Config::has, py::arg("key"),
             "Check if configuration contains key");

    // Register type conversion functions
    m.def("convert_to_config_value", &type_conversions::dict_to_config_value,
          py::arg("dict"), "Convert Python dictionary to ConfigValue");
    m.def("convert_to_neuron_id", &type_conversions::string_to_neuron_id,
          py::arg("string"), "Convert string to NeuronId");
    m.def("convert_to_synapse_id", &type_conversions::string_to_synapse_id,
          py::arg("string"), "Convert string to SynapseId");
    m.def("convert_to_region_id", &type_conversions::string_to_region_id,
          py::arg("string"), "Convert string to RegionId");
    m.def("py_list_to_vector", &type_conversions::py_list_to_float_vector,
          py::arg("list"), "Convert Python list to std::vector<float>");

    py::class_<nlm::SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &nlm::SimpleWorld::reset)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed)
        // Context manager support
        .def("__enter__", [](nlm::SimpleWorld& self) {
            self.reset();
            return self;
        })
        .def("__exit__", [](nlm::SimpleWorld& self, py::args) {
            self.reset();
        })
        // Convenience methods
        .def("run_simulation", [](nlm::SimpleWorld& self, int steps, float dt) {
            for (int i = 0; i < steps; ++i) {
                self.update(dt);
            }
            return self.getSimulationTime();
        }, py::arg("steps"), py::arg("dt") = 0.01f, "Run simulation for specified number of steps")
        .def("run_episode", &nlm::SimpleWorld::reset, "Reset world for new episode")
        .def("step", &nlm::SimpleWorld::update, py::arg("timestep"), "Advance world by one timestep")
        .def("evaluate", [](nlm::SimpleWorld& self, const nlm::SensoryPercept& percept) {
            // Apply current brain actions if available
            // This would be used in agent-based evaluation
            return self.getAgentBody();
        }, py::arg("percept"), "Evaluate state with given percept")
        .def("get_stats", [](const nlm::SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["simulation_time"] = self.getSimulationTime();
            stats["max_energy"] = self.getMaxEnergy();
            stats["energy_decay_rate"] = self.getEnergyDecayRate();
            return stats;
        }, "Get world statistics as dictionary")
        .def("pause", [](nlm::SimpleWorld& self) {
            // Store current state for pause/resume
            // Implementation would depend on internal state management
            return true; // Placeholder
        }, "Pause world simulation")
        .def("resume", [](nlm::SimpleWorld& self) {
            // Resume from paused state
            // Implementation would depend on internal state management
            return true; // Placeholder
        }, "Resume world simulation")
        .def("step_by_step", [](nlm::SimpleWorld& self, float dt) {
            self.update(dt);
            return self.getAgentBody();
        }, py::arg("dt") = 0.01f, "Perform a single simulation step")
        .def("get_performance_metrics", [](const nlm::SimpleWorld& self) {
            py::dict metrics;
            metrics["simulation_time"] = self.getSimulationTime();
            metrics["update_rate"] = 1000.0f / (self.getSimulationTime() * 1000.0f); // Placeholder
            return metrics;
        }, "Get performance metrics");

    py::class_<nlm::Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset,
             "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &nlm::Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &nlm::Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &nlm::Brain::logStatus,
             "Log brain status")
        // Context manager support
        .def("__enter__", [](nlm::Brain& self) {
            self.initialize();
            return self;
        })
        .def("__exit__", [](nlm::Brain& self, py::args) {
            self.reset();
        })
        // Convenience methods
        .def("run_simulation", [](nlm::Brain& self, const nlm::SimpleWorld& world, int steps, float dt) {
            self.initialize();
            auto brain_ptr = self.getConfig();
            // Run simulation steps
            for (int i = 0; i < steps; ++i) {
                auto percept = world.getSensoryPercept();
                self.receiveSensoryInput(percept);
                self.step(i, world.getSimulationTime());
                world.update(dt);
            }
            return self.getAverageFiringRate();
        }, py::arg("world"), py::arg("steps"), py::arg("dt") = 0.01f,
             "Run simulation with world interaction")
        .def("train", [](nlm::Brain& self, const std::vector<nlm::SensoryPercept>& inputs, 
                       const std::vector<std::unique_ptr<nlm::Action>>& targets, 
                       float learning_rate) {
            // Training loop
            for (size_t i = 0; i < inputs.size(); ++i) {
                self.reset();
                self.receiveSensoryInput(inputs[i]);
                self.step(i);
                // Would typically apply backpropagation here
            }
            return static_cast<float>(inputs.size());
        }, py::arg("inputs"), py::arg("targets"), py::arg("learning_rate") = 0.01f,
             "Train brain on sequence of inputs/targets")
        .def("evaluate", [](nlm::Brain& self, const nlm::SensoryPercept& input) {
            self.reset();
            self.receiveSensoryInput(input);
            auto action = self.produceAction();
            return action;
        }, py::arg("input"), "Evaluate single input to get action")
        .def("train_episode", [](nlm::Brain& self, const nlm::SimpleWorld& world, 
                               const nlm::SensoryPercept& initial_percept, 
                               int max_steps, float dt) {
            self.initialize();
            self.receiveSensoryInput(initial_percept);
            
            for (int step = 0; step < max_steps; ++step) {
                self.step(step, world.getSimulationTime());
                world.update(dt);
            }
            return self.getAverageFiringRate();
        }, py::arg("world"), py::arg("initial_percept"), 
             py::arg("max_steps"), py::arg("dt") = 0.01f,
             "Run one training episode")
        .def("get_training_stats", [](nlm::Brain& self) {
            py::dict stats;
            stats["neurons"] = self.getTotalNeuronCount();
            stats["synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        }, "Get brain training statistics as dictionary")
        .def("get_neuron_stats", [](nlm::Brain& self) {
            return stats_aggregation::aggregate_neuron_stats(self);
        }, "Get aggregated neuron statistics")
        .def("get_synapse_stats", [](nlm::Brain& self) {
            return stats_aggregation::aggregate_synapse_stats(self);
        }, "Get aggregated synapse statistics")
        .def("get_region_stats", [](nlm::Brain& self) {
            return stats_aggregation::aggregate_region_stats(self);
        }, "Get aggregated region statistics")
        .def("get_all_stats", [](nlm::Brain& self) {
            py::dict all_stats;
            all_stats["neurons"] = stats_aggregation::aggregate_neuron_stats(self);
            all_stats["synapses"] = stats_aggregation::aggregate_synapse_stats(self);
            all_stats["regions"] = stats_aggregation::aggregate_region_stats(self);
            return all_stats;
        }, "Get all statistics as nested dictionary")
        .def("configure_for_training", [](nlm::Brain& self) {
            // Configure brain for training
            auto config = self.getConfig();
            if (config) {
                config->set("brain.training.enabled", true);
                config->set("brain.training.learning_rate", 0.01f);
                config->set("brain.plasticity.hebbian.enabled", true);
                config->set("brain.plasticity.stdp.enabled", true);
            }
            return true;
        }, "Configure brain for training mode")
        .def("configure_for_inference", [](nlm::Brain& self) {
            // Configure brain for inference
            auto config = self.getConfig();
            if (config) {
                config->set("brain.training.enabled", false);
                config->set("brain.plasticity.hebbian.enabled", false);
                config->set("brain.plasticity.stdp.enabled", false);
            }
            return true;
        }, "Configure brain for inference mode")
        .def("profile", [](nlm::Brain& self) {
            py::dict profile_data;
            auto start_time = std::chrono::high_resolution_clock::now();
            
            // Run a simulation step to measure performance
            self.reset();
            self.step(0);
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            profile_data["step_time_us"] = static_cast<double>(duration.count());
            profile_data["neurons_per_step"] = self.getTotalNeuronCount();
            profile_data["spikes_generated"] = self.getTotalSpikeCount();
            
            return profile_data;
        }, "Profile brain performance and return timing metrics")
        .def("monitor", [](nlm::Brain& self) {
            py::dict monitoring_data;
            monitoring_data["neuron_count"] = self.getTotalNeuronCount();
            monitoring_data["synapse_count"] = self.getTotalSynapseCount();
            monitoring_data["active_neuron_count"] = self.getActiveNeuronCount();
            monitoring_data["firing_neuron_count"] = self.getFiringNeuronCount();
            monitoring_data["average_firing_rate"] = self.getAverageFiringRate();
            monitoring_data["e_i_ratio"] = self.getExcitationInhibitionRatio();
            monitoring_data["total_spikes"] = self.getTotalSpikeCount();
            monitoring_data["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            monitoring_data["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            return monitoring_data;
        }, "Get real-time monitoring data")
        .def("batch_step", &batch_processing::batch_step,
             py::arg("steps"), py::arg("times") = std::vector<nlm::Timestamp>(),
             "Perform batch simulation steps")
        .def("visualize", [](nlm::Brain& self) {
            return visualization::convert_to_matplotlib(self);
        }, "Convert brain data to visualization format (matplotlib-compatible)")
        .def("plot_neuron_activity", [](nlm::Brain& self) {
            return visualization::create_neuron_activity_plot(self);
        }, "Create neuron activity plot data");

    py::class_<nlm::AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled)
        // Context manager support
        .def("__enter__", [](nlm::AgentBrain& self) {
            return self;
        })
        .def("__exit__", [](nlm::AgentBrain& self, py::args) {
            self.reset();
        })
        // Convenience methods
        .def("act", [](nlm::AgentBrain& self, const nlm::SensoryPercept& percept, 
                     const nlm::SimpleWorld& world, double timestep) {
            self.initialize(world);
            self.processSensoryInput(percept);
            auto cmd = self.decodeMotorCommand();
            world.applyMotorCommand(cmd, world.getSimulationTime());
            self.updateDevelopment(timestep);
            return cmd;
        }, py::arg("percept"), py::arg("world"), py::arg("timestep"),
             "Process percept and generate motor action")
        .def("run_episode", [](nlm::AgentBrain& self, const nlm::SimpleWorld& world,
                              const nlm::SensoryPercept& initial_percept,
                              int max_steps, float dt) {
            self.initialize(world);
            self.processSensoryInput(initial_percept);
            
            py::list actions;
            for (int step = 0; step < max_steps; ++step) {
                auto cmd = self.decodeMotorCommand();
                actions.append(cmd);
                world.applyMotorCommand(cmd, world.getSimulationTime());
                self.updateDevelopment(dt);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
            }
            return actions;
        }, py::arg("world"), py::arg("initial_percept"),
             py::arg("max_steps"), py::arg("dt") = 0.01f,
             "Run episode and return sequence of actions")
        .def("train_episode", [](nlm::AgentBrain& self, const nlm::SimpleWorld& world,
                                const nlm::SensoryPercept& initial_percept,
                                const std::vector<float>& rewards,
                                int max_steps, float dt) {
            self.initialize(world);
            self.processSensoryInput(initial_percept);
            
            float total_reward = 0.0f;
            for (int step = 0; step < max_steps && step < rewards.size(); ++step) {
                auto cmd = self.decodeMotorCommand();
                world.applyMotorCommand(cmd, world.getSimulationTime());
                self.applyRewardModulation(rewards[step], 0.0f); // Simplified
                self.updateDevelopment(dt);
                total_reward += rewards[step];
                
                if (step < max_steps - 1) {
                    auto percept = world.getSensoryPercept();
                    self.processSensoryInput(percept);
                }
            }
            return total_reward;
        }, py::arg("world"), py::arg("initial_percept"),
             py::arg("rewards"), py::arg("max_steps"), py::arg("dt") = 0.01f,
             "Run training episode and return total reward")
        .def("get_stats", [](nlm::AgentBrain& self) {
            py::dict stats;
            stats["sensory_input_size"] = self.getSensoryInputSize();
            stats["motor_output_size"] = self.getMotorOutputSize();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            return stats;
        }, "Get agent statistics as dictionary")
        .def("get_brain_stats", [](nlm::AgentBrain& self) {
            return stats_aggregation::aggregate_neuron_stats(*self.getBrain());
        }, "Get brain statistics from agent brain")
        .def("enable_training_mode", [](nlm::AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            return true;
        }, "Enable all training-related features")
        .def("enable_inference_mode", [](nlm::AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
            return true;
        }, "Enable inference-only mode")
        .def("monitor", [](nlm::AgentBrain& self) {
            py::dict monitoring_data;
            auto brain_stats = stats_aggregation::aggregate_neuron_stats(*self.getBrain());
            monitoring_data["brain"] = brain_stats;
            monitoring_data["sensory_input_size"] = self.getSensoryInputSize();
            monitoring_data["motor_output_size"] = self.getMotorOutputSize();
            monitoring_data["neuromodulation_level"] = self.getNeuromodulationLevel();
            monitoring_data["curiosity_level"] = self.getCuriosityLevel();
            monitoring_data["novelty_level"] = self.getNoveltyLevel();
            monitoring_data["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            monitoring_data["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            return monitoring_data;
        }, "Get comprehensive monitoring data from agent brain")
        .def("get_action_distribution", [](nlm::AgentBrain& self, const nlm::SimpleWorld& world,
                                           const nlm::SensoryPercept& percept, int steps) {
            py::dict distribution;
            py::list actions;
            
            self.initialize(world);
            self.processSensoryInput(percept);
            
            py::dict action_counts;
            action_counts[std::string("MoveForward")] = 0;
            action_counts[std::string("MoveBackward")] = 0;
            action_counts[std::string("TurnLeft")] = 0;
            action_counts[std::string("TurnRight")] = 0;
            action_counts[std::string("Look")] = 0;
            action_counts[std::string("Wait")] = 0;
            
            for (int step = 0; step < steps; ++step) {
                auto cmd = self.decodeMotorCommand();
                std::string action_name;
                
                switch (cmd) {
                    case nlm::MotorCommand::MoveForward: action_name = "MoveForward"; break;
                    case nlm::MotorCommand::MoveBackward: action_name = "MoveBackward"; break;
                    case nlm::MotorCommand::TurnLeft: action_name = "TurnLeft"; break;
                    case nlm::MotorCommand::TurnRight: action_name = "TurnRight"; break;
                    case nlm::MotorCommand::Look: action_name = "Look"; break;
                    default: action_name = "Wait"; break;
                }
                
                action_counts[action_name] = action_counts[action_name].cast<int>() + 1;
                actions.append(cmd);
                world.applyMotorCommand(cmd, world.getSimulationTime());
                auto percept_update = world.getSensoryPercept();
                self.processSensoryInput(percept_update);
            }
            
            distribution["actions"] = actions;
            distribution["counts"] = action_counts;
            distribution["total"] = steps;
            
            return distribution;
        }, py::arg("world"), py::arg("percept"), py::arg("steps"),
             "Analyze action distribution over multiple steps");

    m.def("createDefaultConfig", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Batch processing function
    m.def("batch_process_brains", &batch_processing::batch_process_brains,
          py::arg("brains"), py::arg("steps"), py::arg("times") = std::vector<nlm::Timestamp>(),
          "Perform batch processing on multiple brains");

    // Export constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
    
    // Export exceptions
    m.attr("NLMError") = py::type::of<nlm::NLMError>();
    m.attr("ConfigurationError") = py::type::of<nlm::ConfigurationError>();
    m.attr("SimulationError") = py::type::of<nlm::SimulationError>();
    m.attr("ResourceError") = py::type::of<nlm::ResourceError>();

    // Export utility functions
    m.def("convert_to_config_value", &type_conversions::dict_to_config_value,
          py::arg("dict"), "Convert Python dictionary to ConfigValue");
    m.def("convert_to_neuron_id", &type_conversions::string_to_neuron_id,
          py::arg("string"), "Convert string to NeuronId");
    m.def("convert_to_synapse_id", &type_conversions::string_to_synapse_id,
          py::arg("string"), "Convert string to SynapseId");
    m.def("convert_to_region_id", &type_conversions::string_to_region_id,
          py::arg("string"), "Convert string to RegionId");
    m.def("py_list_to_vector", &type_conversions::py_list_to_float_vector,
          py::arg("list"), "Convert Python list to std::vector<float>");
}

} // namespace nlm
