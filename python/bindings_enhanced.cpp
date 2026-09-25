#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/overloads.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Python exception types
class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class SimulationError : public std::runtime_error {
public:
    SimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public std::runtime_error {
public:
    InvalidArgumentError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for Python-Python conversions
inline std::vector<float> to_fvector(const py::array_t<float>& arr) {
    return py::cast<std::vector<float>>(arr);
}

inline py::array_t<float> to_farray(const std::vector<float>& vec) {
    return py::array_t<float>(vec);
}

inline std::vector<double> to_dvector(const py::array_t<double>& arr) {
    return py::cast<std::vector<double>>(arr);
}

inline py::array_t<double> to_darray(const std::vector<double>& vec) {
    return py::array_t<double>(vec);
}

inline std::vector<int> to_ivec(const py::array_t<int>& arr) {
    return py::cast<std::vector<int>>(arr);
}

inline py::array_t<int> to_iarray(const std::vector<int>& vec) {
    return py::array_t<int>(vec);
}

inline bool all_finite(const std::vector<float>& vec) {
    return std::all_of(vec.begin(), vec.end(), [](float x) { return std::isfinite(x); });
}

inline bool all_finite(const std::vector<double>& vec) {
    return std::all_of(vec.begin(), vec.end(), [](double x) { return std::isfinite(x); });
}

inline std::string format_vector(const std::vector<float>& vec, int precision = 3) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << std::fixed << std::setprecision(precision) << vec[i];
    }
    oss << "]";
    return oss.str();
}

inline std::string format_vector(const std::vector<double>& vec, int precision = 3) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << std::fixed << std::setprecision(precision) << vec[i];
    }
    oss << "]";
    return oss.str();
}

// Convenience functions for configuration
inline std::shared_ptr<Config> create_config_from_dict(const py::dict& config_dict) {
    auto config = std::make_shared<Config>();
    
    for (auto item : config_dict) {
        std::string key = py::cast<std::string>(item.first);
        
        if (item.second.is(py::int_())) {
            config->set(key, py::cast<int>(item.second));
        } else if (item.second.is(py::int64_t())) {
            config->set(key, py::cast<int64_t>(item.second));
        } else if (item.second.is(py::float_())) {
            config->set(key, py::cast<double>(item.second));
        } else if (item.second.is(py::bool_())) {
            config->set(key, py::cast<bool>(item.second));
        } else if (item.second.is(py::str())) {
            config->set(key, py::cast<std::string>(item.second));
        } else if (item.second.is(py::list())) {
            // Try to convert list to vector
            py::list list_val = py::cast<py::list>(item.second);
            bool is_int = true;
            bool is_float = true;
            
            for (auto elem : list_val) {
                if (!elem.is(py::int_())) {
                    is_int = false;
                }
                if (!elem.is(py::float_()) && !elem.is(py::int_())) {
                    is_float = false;
                }
            }
            
            if (is_int) {
                std::vector<int> int_list;
                for (auto elem : list_val) {
                    int_list.push_back(py::cast<int>(elem));
                }
                config->set(key, int_list);
            } else if (is_float) {
                std::vector<double> float_list;
                for (auto elem : list_val) {
                    float_list.push_back(py::cast<double>(elem));
                }
                config->set(key, float_list);
            } else {
                std::vector<std::string> str_list;
                for (auto elem : list_val) {
                    str_list.push_back(py::cast<std::string>(elem));
                }
                config->set(key, str_list);
            }
        }
    }
    
    return config;
}

inline py::dict config_to_dict(const Config& config) {
    py::dict result;
    
    for (const auto& key : config.getKeys()) {
        // Try to get value and add to dict
        try {
            if (config.has("neuron_count")) {
                auto val_opt = config.get<int64_t>(key);
                if (val_opt) {
                    result[key] = *val_opt;
                }
            } else if (config.has("connection_probability")) {
                auto val_opt = config.get<double>(key);
                if (val_opt) {
                    result[key] = *val_opt;
                }
            } else if (config.has("vision_enabled")) {
                auto val_opt = config.get<bool>(key);
                if (val_opt) {
                    result[key] = *val_opt;
                }
            } else {
                auto val_opt = config.get<std::string>(key);
                if (val_opt) {
                    result[key] = *val_opt;
                }
            }
        } catch (const std::exception&) {
            // Skip if conversion fails
        }
    }
    
    return result;
}

// Helper classes for simulation management
class SimulationRunner {
public:
    SimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : brain_(brain), config_(config), current_step_(0) {}
    
    // Run simulation for specified number of steps
    void run_steps(size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            brain_->step(i, static_cast<Timestamp>(i) * config_->getOr<double>("simulation_timestep", 0.001));
            current_step_ = i + 1;
        }
    }
    
    // Run simulation until stopping condition
    void run_until(std::function<bool(const Brain&)> condition) {
        while (!condition(*brain_)) {
            brain_->step(current_step_, static_cast<Timestamp>(current_step_) * config_->getOr<double>("simulation_timestep", 0.001));
            ++current_step_;
        }
    }
    
    // Get current simulation step
    SimulationStep getCurrentStep() const { return current_step_; }
    
    // Get current spike count
    size_t getTotalSpikes() const { return brain_->getTotalSpikeCount(); }
    
    // Get current brain reference
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
    // Get configuration reference
    std::shared_ptr<Config> getConfig() { return config_; }
    
    // Reset simulation
    void reset() {
        brain_->reset();
        current_step_ = 0;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    SimulationStep current_step_;
};

class AgentSimulator {
public:
    AgentSimulator(std::shared_ptr<SimpleWorld> world, std::shared_ptr<Config> config)
        : world_(world), config_(config), steps_since_reset_(0) {
        
        // Initialize agent brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        
        agentBrain_ = std::make_shared<AgentBrain>(brain);
        agentBrain_->initialize(*world_);
    }
    
    // Run agent simulation for specified steps
    std::vector<ActionResult> run_steps(size_t steps) {
        std::vector<ActionResult> results;
        
        for (size_t i = 0; i < steps; ++i) {
            // Get sensory input
            auto percept = world_->getSensoryPercept();
            
            // Process sensory input
            agentBrain_->processSensoryInput(percept);
            
            // Get action from brain
            auto action = agentBrain_->decodeMotorCommand();
            
            // Apply action to world
            ActionResult result = world_->applyMotorCommand(action, i * config_->getOr<double>("simulation_timestep", 0.001));
            
            results.push_back(result);
            
            // Apply reward modulation
            float reward = result.success ? result.reward : -result.reward * 0.5f;
            agentBrain_->applyRewardModulation(reward, agentBrain_->getPredictionError());
            
            // Update development
            agentBrain_->updateDevelopment(i * config_->getOr<double>("simulation_timestep", 0.001));
        }
        
        steps_since_reset_ += steps;
        return results;
    }
    
    // Get current agent brain
    std::shared_ptr<AgentBrain> getAgentBrain() { return agentBrain_; }
    
    // Get current world
    std::shared_ptr<SimpleWorld> getWorld() { return world_; }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() { return config_; }
    
    // Get agent body
    AgentBody getAgentBody() const { return world_->getAgentBody(); }
    
    // Reset simulation
    void reset() {
        world_->reset();
        agentBrain_->reset();
        agentBrain_->initialize(*world_);
        steps_since_reset_ = 0;
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<AgentBrain> agentBrain_;
    size_t steps_since_reset_;
};

// Initialize the Python module with enhanced API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        
        This module provides enhanced Python bindings for the NLM C++ neural
        simulation framework. It includes:
        
        **Core Classes**: Enhanced versions of all NLM classes with Pythonic APIs
        **Helper Functions**: Convenience functions for common operations
        **Simulation Management**: High-level simulation controllers
        **Configuration Tools**: Easy configuration loading and validation
        **Error Handling**: Comprehensive error types and validation
        **Performance Tools**: Utilities for data manipulation and validation
        
        Key Features:
        - Pythonic API design with intuitive method names
        - Automatic type conversion between C++ and Python
        - Enhanced error handling with descriptive error messages
        - Convenience functions for common simulation tasks
        - Configuration loading from Python dictionaries
        - Simulation runner for batch processing
        - Agent simulator for complete agent-environment interaction
        
        Usage Examples:
            import pynlm as nlm
            
            # Create a brain with configuration
            config = nlm.createDefaultConfig()
            config.set("neuron_count", 500)
            config.set("simulation_timestep", 0.001)
            brain = nlm.createBrain(config)
            
            # Run simulation
            brain.initialize()
            for step in range(1000):
                brain.step(step)
            
            # Create agent simulation
            world = nlm.createSimpleWorld()
            simulator = nlm.createAgentSimulator(world, config)
            
            # Run agent for 100 steps
            results = simulator.run_steps(100)
            
            # Get agent state
            body = simulator.getAgentBody()
            print(f"Agent position: ({body.x}, {body.y})")
            print(f"Agent energy: {body.energy}")
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    
    // Register identifier types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });
    
    // Register enum types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // Register configuration class with enhanced API
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", py::overload_cast<const std::string&>(&Config::loadFromFile),
             py::arg("filepath"), "Load configuration from JSON/YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save configuration to JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        // Pythonic getters with default values
        .def("getInt", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get integer value with default")
        .def("getFloat", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get float value with default")
        .def("getString", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get string value with default")
        .def("getBool", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get boolean value with default")
        // Pythonic setters
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, static_cast<double>(value));
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("setDict", &create_config_from_dict, py::arg("config_dict"),
             "Create configuration from Python dictionary")
        .def("toDict", &config_to_dict, "Convert configuration to Python dictionary")
        .def("validate", [](const Config& self, const std::string& key) {
            if (self.has(key)) {
                // Try to get value and validate (simplified)
                return true;
            }
            return false;
        }, py::arg("key"), "Validate configuration key exists")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // Register enhanced sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("toArray", [](const SensoryInput& self) {
            return to_farray(self.getData());
        }, "Convert sensory input to Python array");
    
    // Register vision class with enhanced API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (!all_finite(data)) {
                throw InvalidArgumentError("Vision data contains non-finite values");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("setArray", [](Vision& self, const py::array_t<float>& arr) {
            auto data = to_fvector(arr);
            if (!all_finite(data)) {
                throw InvalidArgumentError("Vision data contains non-finite values");
            }
            self.setData(data);
        }, py::arg("array"), "Set vision data from Python array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getPixelData", [](const Vision& self) {
            // Get flattened pixel data
            const auto& vision_data = self.getData();
            py::array_t<float> result = to_farray(vision_data);
            return result.reshape({static_cast<ssize_t>(self.getHeight()), 
                                 static_cast<ssize_t>(self.getWidth()), 
                                 static_cast<ssize_t>(self.getChannels())});
        }, "Get vision data as 2D/3D array");
    
    // Register audio class with enhanced API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (!all_finite(data)) {
                throw InvalidArgumentError("Audio data contains non-finite values");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setArray", [](Audio& self, const py::array_t<float>& arr) {
            auto data = to_fvector(arr);
            if (!all_finite(data)) {
                throw InvalidArgumentError("Audio data contains non-finite values");
            }
            self.setData(data);
        }, py::arg("array"), "Set audio data from Python array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getWaveform", [](const Audio& self) {
            const auto& audio_data = self.getData();
            return to_farray(audio_data);
        }, "Get audio waveform as array");
    
    // Register other enhanced classes
    py::class_<Action>(m, "Action", R"pbdoc(Action with enhanced API)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getParameter", [](Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw InvalidArgumentError("Parameter index out of range");
        }, py::arg("index"), "Get individual parameter value")
        .def("addParameter", &Action::addParameter, py::arg("value"), "Add parameter to action")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary");
    
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("isNear", [](const AgentBody& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= radius * radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
         "Check if agent is within radius of point");
    
    // Register simulation controllers
    py::class_<SimulationRunner>(m, "SimulationRunner", R"pbdoc(Simulation controller for batch processing)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<Config>>(),
             py::arg("brain"), py::arg("config"))
        .def("runSteps", &SimulationRunner::run_steps, py::arg("steps"),
             "Run simulation for specified number of steps")
        .def("runUntil", &SimulationRunner::runUntil, py::arg("condition"),
             "Run simulation until condition is met")
        .def("getCurrentStep", &SimulationRunner::getCurrentStep,
             "Get current simulation step")
        .def("getTotalSpikes", &SimulationRunner::getTotalSpikes,
             "Get total spike count")
        .def("getBrain", &SimulationRunner::getBrain,
             py::return_value_policy::reference_internal,
             "Get brain reference")
        .def("getConfig", &SimulationRunner::getConfig,
             py::return_value_policy::reference_internal,
             "Get configuration reference")
        .def("reset", &SimulationRunner::reset, "Reset simulation");
    
    py::class_<AgentSimulator>(m, "AgentSimulator", R"pbdoc(Agent-environment simulator)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>, std::shared_ptr<Config>>(),
             py::arg("world"), py::arg("config"))
        .def("runSteps", &AgentSimulator::run_steps, py::arg("steps"),
             "Run agent simulation for specified steps")
        .def("getAgentBrain", &AgentSimulator::getAgentBrain,
             py::return_value_policy::reference_internal,
             "Get agent brain reference")
        .def("getWorld", &AgentSimulator::getWorld,
             py::return_value_policy::reference_internal,
             "Get world reference")
        .def("getConfig", &AgentSimulator::getConfig,
             py::return_value_policy::reference_internal,
             "Get configuration reference")
        .def("getAgentBody", &AgentSimulator::getAgentBody,
             "Get agent body state")
        .def("reset", &AgentSimulator::reset, "Reset simulation");
    
    // Register convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createConfigFromDict", &create_config_from_dict, py::arg("config_dict"),
          "Create configuration from Python dictionary");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    m.def("createSimulationRunner", 
          [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
              return std::make_shared<SimulationRunner>(brain, config);
          }, py::arg("brain"), py::arg("config"), "Create a simulation runner");
    
    m.def("createAgentSimulator", 
          [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<Config> config) {
              return std::make_shared<AgentSimulator>(world, config);
          }, py::arg("world"), py::arg("config"), "Create an agent simulator");
    
    // Register convenience constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register helper functions
    m.def("validateConfig", [](const Config& config, const std::string& key) {
        if (!config.has(key)) {
            throw ConfigError("Configuration key not found: " + key);
        }
        return true;
    }, py::arg("config"), py::arg("key"), "Validate configuration key");
    
    m.def("getConfigValue", [](Config& config, const std::string& key, const py::object& default_value) {
        if (config.has(key)) {
            auto val_opt = config.get<std::string>(key);
            if (val_opt) {
                return *val_opt;
            }
        }
        return py::cast<std::string>(default_value);
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = "",
         "Get configuration value with default");
    
    m.def("setConfigValue", [](Config& config, const std::string& key, const py::object& value) {
        if (value.is(py::int_())) {
            config.set(key, py::cast<int>(value));
        } else if (value.is(py::float_())) {
            config.set(key, py::cast<double>(value));
        } else if (value.is(py::bool_())) {
            config.set(key, py::cast<bool>(value));
        } else if (value.is(py::str())) {
            config.set(key, py::cast<std::string>(value));
        } else {
            throw InvalidArgumentError("Unsupported configuration value type");
        }
    }, py::arg("config"), py::arg("key"), py::arg("value"), "Set configuration value");
    
    m.def("runSimpleSimulation", 
          [](std::shared_ptr<Brain> brain, size_t steps) {
              brain->step(0, 0.0f);  // Initialize
              for (size_t i = 1; i < steps; ++i) {
                  brain->step(i, i * 0.001f);
              }
              return brain->getTotalSpikeCount();
          }, py::arg("brain"), py::arg("steps"), "Run simple simulation and return spike count");
}

} // namespace nlm
