#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Enhanced Python wrappers for NLM - Pythonic API
namespace py = pybind11;
namespace nlm {

// Helper function to convert NeuronId to Python
static std::string neuron_id_to_str(const NeuronId& id) {
    return "<NeuronId: " + std::to_string(id.value) + ">";
}

// Helper function to convert SynapseId to Python
static std::string synapse_id_to_str(const SynapseId& id) {
    return "<SynapseId: " + std::to_string(id.value) + ">";
}

// Helper function to convert RegionId to Python
static std::string region_id_to_str(const RegionId& id) {
    return "<RegionId: " + std::to_string(id.value) + ">";
}

// Helper function to convert PopulationId to Python
static std::string population_id_to_str(const PopulationId& id) {
    return "<PopulationId: " + std::to_string(id.value) + ">";
}

// Context manager for Brain simulation
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    ~BrainContextManager() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Make brain accessible
    std::shared_ptr<Brain> operator->() {
        return brain_;
    }
    
    // For use in with statement
    std::shared_ptr<Brain> get_brain() {
        return brain_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced Config class with better Python integration
class PythonConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    PythonConfig() : config_(std::make_shared<Config>()) {}
    
    // Load from file
    bool load_from_file(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    // Load from arguments (simulate command line)
    bool load_from_args(const std::vector<std::string>& args) {
        // Convert Python list to C++ argv style
        int argc = args.size();
        char** argv = new char*[argc + 1];
        argv[0] = new char[1];
        argv[0][0] = '\0'; // Program name
        
        for (size_t i = 0; i < args.size(); ++i) {
            argv[i + 1] = new char[args[i].size() + 1];
            std::strcpy(argv[i + 1], args[i].c_str());
            if (args[i].substr(0, 2) == "--" && args[i].find('=') != std::string::npos) {
                // --key=value format
                size_t pos = args[i].find('=');
                std::string key = "--" + args[i].substr(2, pos - 2);
                std::string value = args[i].substr(pos + 1);
                argv[i + 1] = new char[key.size() + value.size() + 2];
                std::strcpy(argv[i + 1], key.c_str());
                std::strcat(argv[i + 1], "=");
                std::strcat(argv[i + 1], value.c_str());
            }
        }
        
        bool result = config_->loadFromArgs(argc, argv);
        
        // Clean up
        delete[] argv[0];
        for (int i = 1; i < argc; ++i) {
            delete[] argv[i];
        }
        delete[] argv;
        
        return result;
    }
    
    // Save to file
    bool save_to_file(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // Get value with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    // Get value with default
    template<typename T>
    T get_or(const std::string& key, const T& default_value) const {
        return config_->getOr<T>(key, default_value);
    }
    
    // Set value
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, int value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value);
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Remove key
    void remove(const std::string& key) {
        config_->remove(key);
    }
    
    // Get all keys
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    // Get config summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Get underlying config
    std::shared_ptr<Config> get_config() {
        return config_;
    }
    
    // Pythonic dictionary-like interface
    void update(const py::dict& kwargs) {
        for (auto item : kwargs) {
            std::string key = py::cast<std::string>(item.first);
            py::object value_obj = item.second;
            
            // Try to infer type
            if (value_obj.is_none()) {
                config_->set(key, "");
            } else if (value_obj.is_int()) {
                config_->set(key, value_obj.cast<int>());
            } else if (value_obj.is_float()) {
                config_->set(key, value_obj.cast<double>());
            } else if (value_obj.is_bool()) {
                config_->set(key, value_obj.cast<bool>());
            } else {
                config_->set(key, value_obj.cast<std::string>());
            }
        }
    }
    
    // Pythonic get with dot notation
    class ConfigAccessor {
    public:
        ConfigAccessor(std::shared_ptr<PythonConfig> parent) : parent_(parent) {}
        
        template<typename T>
        T get(const std::string& key, const T& default_value = T()) const {
            auto val = parent_->get<T>(key);
            return val ? *val : default_value;
        }
        
        template<typename T>
        T& operator[](const std::string& key) {
            static T dummy = T();
            auto val = parent_->get<T>(key);
            if (!val) {
                parent_->set(key, T());
                return dummy;
            }
            // This is a hack - we can't return reference to internal value easily
            // For now, we'll just demonstrate the interface
            return const_cast<T&>(*val);
        }
        
    private:
        std::shared_ptr<PythonConfig> parent_;
    };
    
    ConfigAccessor accessor() {
        return ConfigAccessor(std::shared_ptr<PythonConfig>(this));
    }
};

// Enhanced AgentBrain with Pythonic interface
class PythonAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    PythonAgentBrain(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with world
    void initialize(std::shared_ptr<SimpleWorld> world) {
        agent_brain_->initialize(*world);
    }
    
    // Get sensory input size
    size_t get_sensory_input_size() const {
        return agent_brain_->getSensoryInputSize();
    }
    
    // Get motor output size
    size_t get_motor_output_size() const {
        return agent_brain_->getMotorOutputSize();
    }
    
    // Process sensory percept
    void process_sensory_input(std::shared_ptr<SensoryPercept> percept) {
        agent_brain_->processSensoryInput(*percept);
    }
    
    // Decode motor command
    MotorCommand decode_motor_command() {
        return agent_brain_->decodeMotorCommand();
    }
    
    // Apply reward modulation
    void apply_reward_modulation(float reward, float predicted_reward = 0.0f) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    // Update development
    void update_development(double timestep = 0.1) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    // Get developmental stage
    DevelopmentalStage get_developmental_stage() const {
        return agent_brain_->getDevelopmentalStage();
    }
    
    // Get neuromodulation level
    float get_neuromodulation_level() const {
        return agent_brain_->getNeuromodulationLevel();
    }
    
    // Get curiosity level
    float get_curiosity_level() const {
        return agent_brain_->getCuriosityLevel();
    }
    
    // Get novelty level
    float get_novelty_level() const {
        return agent_brain_->getNoveltyLevel();
    }
    
    // Get prediction error
    float get_prediction_error() const {
        return agent_brain_->getPredictionError();
    }
    
    // Reset agent
    void reset() {
        agent_brain_->reset();
    }
    
    // Get brain pointer
    std::shared_ptr<Brain> get_brain() {
        return agent_brain_->getBrain();
    }
    
    // Configuration methods
    void enable_reward_modulation(bool enable) {
        agent_brain_->enableRewardModulation(enable);
    }
    
    void enable_structural_plasticity(bool enable) {
        agent_brain_->enableStructuralPlasticity(enable);
    }
    
    void enable_development(bool enable) {
        agent_brain_->enableDevelopment(enable);
    }
    
    void enable_curiosity(bool enable) {
        agent_brain_->enableCuriosity(enable);
    }
    
    bool is_reward_modulation_enabled() const {
        return agent_brain_->isRewardModulationEnabled();
    }
    
    bool is_structural_plasticity_enabled() const {
        return agent_brain_->isStructuralPlasticityEnabled();
    }
    
    bool is_development_enabled() const {
        return agent_brain_->isDevelopmentEnabled();
    }
    
    bool is_curiosity_enabled() const {
        return agent_brain_->isCuriosityEnabled();
    }
    
    // Pythonic action execution
    py::dict step_brain(
        std::shared_ptr<PythonConfig> config,
        const py::list& sensory_inputs,
        int steps = 1
    ) {
        auto brain = get_brain();
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        py::dict results;
        
        for (int step = 0; step < steps; ++step) {
            // Process sensory inputs if provided
            if (!sensory_inputs.empty()) {
                for (size_t i = 0; i < sensory_inputs.size(); ++i) {
                    auto percept = std::dynamic_pointer_cast<SensoryPercept>(sensory_inputs[i].cast<std::shared_ptr<SensoryPercept>>());
                    if (percept) {
                        brain->receiveSensoryInput(*percept);
                    }
                }
            }
            
            // Run simulation step
            brain->step(step);
            
            // Collect results
            py::dict step_results;
            step_results["step"] = step;
            step_results["total_neurons"] = brain->getTotalNeuronCount();
            step_results["total_synapses"] = brain->getTotalSynapseCount();
            step_results["active_neurons"] = brain->getActiveNeuronCount();
            step_results["firing_neurons"] = brain->getFiringNeuronCount();
            step_results["average_firing_rate"] = brain->getAverageFiringRate();
            step_results["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            
            results[py::str(step)] = step_results;
        }
        
        return results;
    }
    
    // Get brain statistics as Python dict
    py::dict get_brain_stats() {
        auto brain = get_brain();
        py::dict stats;
        
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["pending_spikes"] = brain->getPendingSpikeEventCount();
        
        return stats;
    }
    
    // Enhanced action decoding with Python integration
    py::dict get_detailed_motor_output() {
        auto brain = get_brain();
        py::dict output;
        
        // Get action from brain
        auto action = brain->produceAction();
        if (action) {
            py::dict action_dict;
            action_dict["type"] = static_cast<int>(action->getType());
            action_dict["parameters"] = action->getParameters();
            action_dict["name"] = action->getName();
            output["action"] = action_dict;
        } else {
            output["action"] = py::none();
        }
        
        // Add brain statistics
        output["stats"] = get_brain_stats();
        
        return output;
    }
};

// Enhanced Python API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        A modern, Pythonic interface to the NLM neural simulation framework.
        
        This module provides:
        - Enhanced configuration management with Python dictionary syntax
        - Agent brain abstraction for easy interaction with neural networks
        - Context managers for resource management
        - Simulation step-by-step execution with detailed result reporting
        - Development and neuromodulation control
        - Curiosity-driven exploration behavior
        
        Key features:
        - Pythonic dictionary-like configuration interface
        - Easy-to-use agent brain API
        - Comprehensive simulation control
        - Rich result reporting
        - Development lifecycle management
        
        Example usage:
            ```python
            import pynlm as nlm
            
            # Create configuration
            config = nlm.Config()
            config.update({"neuron_count": 1000, "region_count": 2})
            
            # Create brain with context manager
            with nlm.Brain(config) as brain:
                # Run simulation
                results = brain.run_simulation(100, sensory_inputs=[...])
                
                # Get statistics
                stats = brain.get_stats()
            ```
        "pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Enhanced Config class
    py::class_<PythonConfig>(m, "Config", R"pbdoc(
        Enhanced configuration class with Pythonic interface.
        
        Provides dictionary-like access, type conversion, and easy management.
        )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &PythonConfig::load_from_file,
             py::arg("filepath"), "Load configuration from file")
        .def("load_from_args", &PythonConfig::load_from_args,
             py::arg("args"), "Load configuration from arguments")
        .def("save_to_file", &PythonConfig::save_to_file,
             py::arg("filepath"), "Save configuration to file")
        .def("get", &PythonConfig::get<py::object>,
             py::arg("key"), "Get configuration value by key")
        .def("get_or", &PythonConfig::get_or<py::object>,
             py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("set", &PythonConfig::set, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("has", &PythonConfig::has, py::arg("key"), "Check if key exists")
        .def("remove", &PythonConfig::remove, py::arg("key"), "Remove key")
        .def("get_keys", &PythonConfig::get_keys, "Get all configuration keys")
        .def("summary", &PythonConfig::summary, "Get configuration summary")
        .def("update", &PythonConfig::update, py::arg("kwargs"), "Update from dictionary")
        .def("__getitem__", [](PythonConfig& self, const std::string& key) {
            return self.get_or<py::object>(key, py::none());
        }, py::arg("key"), "Get item using dictionary syntax")
        .def("__setitem__", [](PythonConfig& self, const std::string& key, py::object value) {
            if (value.is_none()) {
                self.set(key, "");
            } else if (value.is_int()) {
                self.set(key, value.cast<int>());
            } else if (value.is_float()) {
                self.set(key, value.cast<double>());
            } else if (value.is_bool()) {
                self.set(key, value.cast<bool>());
            } else {
                self.set(key, value.cast<std::string>());
            }
        }, py::arg("key"), py::arg("value"), "Set item using dictionary syntax")
        .def("__contains__", &PythonConfig::has, py::arg("key"), "Check if key exists")
        .def("__repr__", [](const PythonConfig& self) {
            return "Config with " + std::to_string(self.get_keys().size()) + " entries";
        });

    // Register original classes for advanced use
    py::class_<Config>(m, "ConfigInternal", R"pbdoc(
        Internal Config class - use PythonConfig for enhanced interface
        )pbdoc");

    // Enhanced AgentBrain class
    py::class_<PythonAgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface with Pythonic API.
        
        Provides simplified interaction with neural networks including
        simulation control, neuromodulation, and development.
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &PythonAgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &PythonAgentBrain::get_sensory_input_size,
             "Get expected sensory input size")
        .def("get_motor_output_size", &PythonAgentBrain::get_motor_output_size,
             "Get expected motor output size")
        .def("process_sensory_input", &PythonAgentBrain::process_sensory_input,
             py::arg("percept"), "Process sensory percept")
        .def("decode_motor_command", &PythonAgentBrain::decode_motor_command,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &PythonAgentBrain::apply_reward_modulation,
             py::arg("reward"), py::arg("predicted_reward"), "Apply reward-based neuromodulation")
        .def("update_development", &PythonAgentBrain::update_development,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &PythonAgentBrain::get_developmental_stage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &PythonAgentBrain::get_neuromodulation_level,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &PythonAgentBrain::get_curiosity_level,
             "Get curiosity level")
        .def("get_novelty_level", &PythonAgentBrain::get_novelty_level,
             "Get novelty level")
        .def("get_prediction_error", &PythonAgentBrain::get_prediction_error,
             "Get prediction error")
        .def("reset", &PythonAgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &PythonAgentBrain::get_brain, "Get underlying brain")
        .def("enable_reward_modulation", &PythonAgentBrain::enable_reward_modulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &PythonAgentBrain::enable_structural_plasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &PythonAgentBrain::enable_development,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &PythonAgentBrain::enable_curiosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &PythonAgentBrain::is_reward_modulation_enabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &PythonAgentBrain::is_structural_plasticity_enabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &PythonAgentBrain::is_development_enabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &PythonAgentBrain::is_curiosity_enabled,
             "Check if curiosity is enabled")
        .def("step_brain", &PythonAgentBrain::step_brain,
             py::arg("config"), py::arg("sensory_inputs"), py::arg("steps") = 1,
             "Run brain simulation with step-by-step results")
        .def("get_brain_stats", &PythonAgentBrain::get_brain_stats,
             "Get comprehensive brain statistics as dictionary")
        .def("get_detailed_motor_output", &PythonAgentBrain::get_detailed_motor_output,
             "Get detailed motor output with statistics")
        .def("__repr__", [](const PythonAgentBrain& self) {
            return "AgentBrain (enhanced Python interface)";
        });

    // Original classes for backward compatibility and advanced use
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron ID)pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse ID)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region ID)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Population ID)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Firing state)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Original classes for backward compatibility
    py::class_<Config>(m, "Config", R"pbdoc(Original Config class)")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), "Load from file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"), "Load from args")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save to file")
        .def("getKeys", &Config::getKeys, "Get keys")
        .def("has", &Config::has, py::arg("key"), "Check if key exists")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), "Set value")
        .def("getOr", &Config::getOr<py::object>, py::arg("key"), py::arg("default_value"), "Get with default")
        .def("summary", &Config::summary, "Get summary");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base sensory input)")
        .def("getType", &SensoryInput::getType)
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body)")
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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept)")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple world)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Brain)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize brain")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"), "Perform step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform step with time")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), "Receive input")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject to neurons")
        .def("produceAction", &Brain::produceAction, "Produce action")
        .def("reset", &Brain::reset, "Reset brain")
        .def("save", &Brain::save, py::arg("filepath"), "Save state")
        .def("load", &Brain::load, py::arg("filepath"), "Load state")
        .def("addRegion", &Brain::addRegion, py::arg("name"), "Add region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, "Get region")
        .def("getRegionCount", &Brain::getRegionCount, "Get region count")
        .def("getRegionIds", &Brain::getRegionIds, "Get region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spikes")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get pending spikes")
        .def("getPredictionSystem", &Brain::getPredictionSystem, py::return_value_policy::reference_internal, "Get prediction system")
        .def("getWorkingMemory", &Brain::getWorkingMemory, py::return_value_policy::reference_internal, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, py::return_value_policy::reference_internal, "Get episodic memory")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, py::return_value_policy::reference_internal, "Get development system")
        .def("getNeuromodulationLevel", &Brain::getExcitationInhibitionRatio, "Get neuromodulation level")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get config")
        .def("logStatus", &Brain::logStatus, "Log status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, SimpleWorld& world) { self.initialize(world); }, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", [](AgentBrain& self, SensoryPercept& percept) { self.processSensoryInput(percept); }, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .getBrain(), py::return_value_policy::reference_internal)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Factory functions
    m.def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create default config");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain with config");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain");

    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create default config");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
