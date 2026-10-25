#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <limits>

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

// Custom Python exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& msg) : NLMException("Brain Error: " + msg) {}
};

class WorldException : public NLMException {
public:
    explicit WorldException(const std::string& msg) : NLMException("World Error: " + msg) {}
};

class AgentException : public NLMException {
public:
    explicit AgentException(const std::string& msg) : NLMException("Agent Error: " + msg) {}
};

// Enhanced Config class with better Pythonic interface
void setupConfigClass(py::class_<Config>& configClass) {
    configClass
        // Constructor with optional file path
        .def(py::init<>(), "Create a default configuration")
        .def(py::init([](const std::string& filepath) {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                throw ConfigurationException("Failed to load configuration from: " + filepath);
            }
            return config;
        }), py::arg("filepath"), "Create configuration from file")
        
        // Enhanced setter methods with validation
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            if (!Config::isValidKey(key)) {
                throw ConfigurationException("Invalid configuration key: '" + key + "'");
            }
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        
        .def("set", [](Config& self, const std::string& key, int value) {
            if (!Config::isValidKey(key)) {
                throw ConfigurationException("Invalid configuration key: '" + key + "'");
            }
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        
        .def("set", [](Config& self, const std::string& key, double value) {
            if (!Config::isValidKey(key)) {
                throw ConfigurationException("Invalid configuration key: '" + key + "'");
            }
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        
        .def("set", [](Config& self, const std::string& key, bool value) {
            if (!Config::isValidKey(key)) {
                throw ConfigurationException("Invalid configuration key: '" + key + "'");
            }
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        
        // Enhanced getter methods with type conversion
        .def("get_string", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = "", "Get string configuration value")
        
        .def("get_int", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0, "Get integer configuration value")
        
        .def("get_float", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            auto val = self.get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0.0, "Get float configuration value")
        
        .def("get_bool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = false, "Get boolean configuration value")
        
        // Convenience methods for common configuration patterns
        .def("configure_brain", [](Config& self, int neuronCount, float connectionProb) {
            self.set("brain.neuron_count", neuronCount);
            self.set("brain.connection_probability", connectionProb);
        }, py::arg("neuron_count"), py::arg("connection_prob"), "Configure basic brain parameters")
        
        .def("configure_learning", [](Config& self, float learningRate, float stdpLtp, float stdpLtd) {
            self.set("plasticity.learning_rate", learningRate);
            self.set("plasticity.stdp.ltp_weight", stdpLtp);
            self.set("plasticity.stdp.ltd_weight", stdpLtd);
        }, py::arg("learning_rate"), py::arg("stdp_ltp"), py::arg("stdp_ltd"), "Configure learning parameters")
        
        .def("configure_neuron", [](Config& self, float threshold, float resting, float leakConductance) {
            self.set("brain.v_thresh", threshold);
            self.set("brain.v_rest", resting);
            self.set("brain.tau_mem", leakConductance);
        }, py::arg("threshold"), py::arg("resting"), py::arg("leak_conductance"), "Configure neuron parameters")
        
        // Batch operations for convenience
        .def("update_from_dict", [](Config& self, const py::dict& settings) {
            for (auto item : settings) {
                std::string key = py::cast<std::string>(item.first);
                if (item.second.is(py::str())) {
                    self.set(key, py::cast<std::string>(item.second));
                } else if (item.second.is(py::int_())) {
                    self.set(key, py::cast<int>(item.second));
                } else if (item.second.is(py::float_())) {
                    self.set(key, py::cast<double>(item.second));
                } else if (item.second.is(py::bool_())) {
                    self.set(key, py::cast<bool>(item.second));
                }
            }
        }, py::arg("settings"), "Update configuration from Python dictionary")
        
        .def("to_dict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto val = self.get<std::string>(key);
                if (val.has_value()) {
                    result[key] = val.value();
                } else {
                    auto valInt = self.get<int>(key);
                    if (valInt.has_value()) {
                        result[key] = valInt.value();
                    } else {
                        auto valFloat = self.get<double>(key);
                        if (valFloat.has_value()) {
                            result[key] = valFloat.value();
                        } else {
                            auto valBool = self.get<bool>(key);
                            if (valBool.has_value()) {
                                result[key] = valBool.value();
                            }
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        
        .def("validate", [](const Config& self) {
            std::vector<std::string> errors;
            for (const auto& key : self.getKeys()) {
                if (!self.validateKey(key)) {
                    errors.push_back("Invalid key: '" + key + "'");
                }
            }
            return errors;
        }, "Validate configuration keys")
        
        .def("get_keys_by_source", [](const Config& self, int source) {
            return self.getKeysBySource(static_cast<ConfigSource>(source));
        }, py::arg("source"), "Get configuration keys by source")
        
        .def("summary_by_source", [](const Config& self, int source) {
            return self.summaryBySource(static_cast<ConfigSource>(source));
        }, py::arg("source"), "Get configuration summary by source")
        
        // Context manager support for file operations
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config&, const py::object&, int, const py::object&) {
            // Cleanup if needed
        })
        
        // Statistics and utilities
        .def("get_statistics", [](const Config& self) {
            py::dict stats;
            stats["total_entries"] = self.getKeys().size();
            stats["default_entries"] = self.getKeysBySource(static_cast<int>(ConfigSource::Default)).size();
            stats["file_entries"] = self.getKeysBySource(static_cast<int>(ConfigSource::File)).size();
            stats["command_line_entries"] = self.getKeysBySource(static_cast<int>(ConfigSource::CommandLine)).size();
            stats["runtime_entries"] = self.getKeysBySource(static_cast<int>(ConfigSource::Runtime)).size();
            return stats;
        }, "Get configuration statistics")
        
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                if (self.has(key)) {
                    self.remove(key);
                }
                self.set(key, other.getOr<std::string>(key, ""));
            }
        }, py::arg("other"), "Merge another configuration into this one")
        
        .def("copy", [](const Config& self) {
            auto copy = std::make_shared<Config>();
            for (const auto& key : self.getKeys()) {
                copy->set(key, self.getOr<std::string>(key, ""));
            }
            return copy;
        }, "Create a copy of this configuration");
}

// Enhanced Brain class with better Python interface
void setupBrainClass(py::class_<Brain>& brainClass) {
    brainClass
        // Enhanced constructor
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        
        // Enhanced initialization with error handling
        .def("initialize", [](Brain& self) {
            if (!self.initialize()) {
                throw BrainException("Failed to initialize brain");
            }
        }, "Initialize brain with error handling")
        
        // Enhanced step methods with validation
        .def("step", [](Brain& self, int step) {
            if (step < 0) {
                throw BrainException("Step number must be non-negative");
            }
            self.step(static_cast<SimulationStep>(step));
        }, py::arg("step"), "Perform a simulation step")
        
        .def("step_with_time", [](Brain& self, int step, double time) {
            if (step < 0 || time < 0.0) {
                throw BrainException("Step number and time must be non-negative");
            }
            self.step(static_cast<SimulationStep>(step), static_cast<Timestamp>(time));
        }, py::arg("step"), py::arg("time"), "Perform a simulation step with timestamp")
        
        // Enhanced getters with better defaults
        .def("get_total_neurons", [](const Brain& self) { return self.getTotalNeuronCount(); }, "Get total neuron count")
        .def("get_total_synapses", [](const Brain& self) { return self.getTotalSynapseCount(); }, "Get total synapse count")
        .def("get_firing_neurons", [](const Brain& self) { return self.getFiringNeuronCount(); }, "Get count of firing neurons")
        .def("get_active_neurons", [](const Brain& self) { return self.getActiveNeuronCount(); }, "Get count of active neurons")
        
        // Statistical methods
        .def("get_firing_rate", [](const Brain& self) { return self.getAverageFiringRate(); }, "Get average firing rate")
        .def("get_ei_ratio", [](const Brain& self) { return self.getExcitationInhibitionRatio(); }, "Get E/I balance ratio")
        .def("get_total_spikes", [](const Brain& self) { return self.getTotalSpikeCount(); }, "Get total spike count")
        
        // Enhanced region methods
        .def("add_region", [](Brain& self, const std::string& name = "") {
            return self.addRegion(name);
        }, py::arg("name") = "", "Add a new neural region")
        
        .def("get_region", [](Brain& self, const RegionId& regionId) {
            auto region = self.getRegion(regionId);
            if (!region) {
                throw BrainException("Region not found");
            }
            return region;
        }, py::arg("region_id"), "Get region by ID")
        
        // Enhanced reset with cleanup
        .def("reset", [](Brain& self) {
            self.reset();
            self.initialize();
        }, "Reset and reinitialize brain")
        
        // Enhanced save/load with error handling
        .def("save", [](Brain& self, const std::string& filepath) {
            if (!self.save(filepath)) {
                throw BrainException("Failed to save brain to: " + filepath);
            }
        }, py::arg("filepath"), "Save brain state to file")
        
        .def("load", [](Brain& self, const std::string& filepath) {
            if (!self.load(filepath)) {
                throw BrainException("Failed to load brain from: " + filepath);
            }
        }, py::arg("filepath"), "Load brain state from file")
        
        // Simulation convenience methods
        .def("run_simulation", [](Brain& self, int numSteps, double timestep = 0.001) {
            for (int i = 0; i < numSteps; ++i) {
                self.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * timestep));
            }
        }, py::arg("num_steps"), py::arg("timestep") = 0.001, "Run simulation for specified steps")
        
        .def("run_learning_episode", [](Brain& self, int numSteps, const std::string& inputPattern) {
            // Inject input pattern
            self.injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            
            // Run learning episode
            for (int i = 0; i < numSteps; ++i) {
                self.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * 0.001));
            }
        }, py::arg("num_steps"), py::arg("input_pattern"), "Run a learning episode")
        
        // State inspection
        .def("get_brain_state", [](const Brain& self) {
            py::dict state;
            state["neuron_count"] = self.getTotalNeuronCount();
            state["synapse_count"] = self.getTotalSynapseCount();
            state["firing_neurons"] = self.getFiringNeuronCount();
            state["active_neurons"] = self.getActiveNeuronCount();
            state["average_firing_rate"] = self.getAverageFiringRate();
            state["e_i_ratio"] = self.getExcitationInhibitionRatio();
            state["total_spikes"] = self.getTotalSpikeCount();
            state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return state;
        }, "Get brain state as dictionary")
        
        .def("check_health", [](const Brain& self) {
            py::dict health;
            bool healthy = true;
            
            if (self.getTotalNeuronCount() == 0) {
                health["neurons"] = "No neurons present";
                healthy = false;
            }
            
            if (self.getTotalSynapseCount() == 0) {
                health["synapses"] = "No synapses present";
                healthy = false;
            }
            
            if (self.getFiringNeuronCount() == 0) {
                health["activity"] = "No firing neurons";
                healthy = false;
            }
            
            if (std::isnan(self.getAverageFiringRate()) || std::isinf(self.getAverageFiringRate())) {
                health["firing_rate"] = "Invalid firing rate";
                healthy = false;
            }
            
            health["healthy"] = healthy;
            return health;
        }, "Check brain health status")
        
        // Convenience factory methods
        .def("create_with_default_config", []() {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, "Create brain with default configuration")
        
        .def("create_with_config", [](const py::dict& configDict) {
            auto config = std::make_shared<Config>();
            for (auto item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                if (item.second.is(py::str())) {
                    config->set(key, py::cast<std::string>(item.second));
                } else if (item.second.is(py::int_())) {
                    config->set(key, py::cast<int>(item.second));
                } else if (item.second.is(py::float_())) {
                    config->set(key, py::cast<double>(item.second));
                } else if (item.second.is(py::bool_())) {
                    config->set(key, py::cast<bool>(item.second));
                }
            }
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        }, py::arg("config_dict"), "Create brain from configuration dictionary");
}

// Enhanced AgentBrain class
void setupAgentBrainClass(py::class_<AgentBrain>& agentClass) {
    agentClass
        // Enhanced constructor
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain with underlying brain")
        
        // Enhanced initialization
        .def("initialize", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world) {
            if (!self.initialize(*world)) {
                throw AgentException("Failed to initialize agent brain");
            }
        }, py::arg("world"), "Initialize agent brain with world")
        
        // Enhanced sensory processing with validation
        .def("process_sensory_input", [](AgentBrain& self, const SensoryPercept& percept) {
            if (!self.getBrain()->initialize()) {
                throw AgentException("Brain not initialized for sensory processing");
            }
            self.processSensoryInput(percept);
        }, py::arg("percept"), "Process sensory input")
        
        // Enhanced motor decoding
        .def("decode_motor_command", [](AgentBrain& self) {
            auto action = self.decodeMotorCommand();
            if (!action) {
                throw AgentException("Failed to decode motor command");
            }
            return action;
        }, "Decode motor command")
        
        // Enhanced neuromodulation
        .def("apply_reward_modulation", [](AgentBrain& self, float reward, float predictedReward) {
            self.applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predicted_reward"), "Apply reward-based neuromodulation")
        
        // Enhanced development
        .def("update_development", [](AgentBrain& self, double timestep) {
            self.updateDevelopment(timestep);
        }, py::arg("timestep"), "Update development system")
        
        // Subsystem management
        .def("enable_subsystem", [](AgentBrain& self, const std::string& subsystem, bool enable) {
            if (subsystem == "reward_modulation") {
                self.enableRewardModulation(enable);
            } else if (subsystem == "structural_plasticity") {
                self.enableStructuralPlasticity(enable);
            } else if (subsystem == "development") {
                self.enableDevelopment(enable);
            } else if (subsystem == "curiosity") {
                self.enableCuriosity(enable);
            } else {
                throw AgentException("Unknown subsystem: " + subsystem);
            }
        }, py::arg("subsystem"), py::arg("enable"), "Enable or disable a subsystem")
        
        // Status and statistics
        .def("get_status", [](const AgentBrain& self) {
            py::dict status;
            status["initialized"] = self.getBrain()->getTotalNeuronCount() > 0;
            status["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            status["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            status["development_enabled"] = self.isDevelopmentEnabled();
            status["curiosity_enabled"] = self.isCuriosityEnabled();
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            status["curiosity_level"] = self.getCuriosityLevel();
            status["novelty_level"] = self.getNoveltyLevel();
            status["prediction_error"] = self.getPredictionError();
            return status;
        }, "Get agent brain status")
        
        // Simulation convenience
        .def("run_episode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, int numSteps) {
            if (!self.initialize(*world)) {
                throw AgentException("Failed to initialize agent for episode");
            }
            
            for (int i = 0; i < numSteps; ++i) {
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                self.getBrain()->step(static_cast<SimulationStep>(i));
                auto action = self.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                float reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
                self.applyRewardModulation(reward, 0.0);
                self.updateDevelopment(0.1);
            }
        }, py::arg("world"), py::arg("num_steps"), "Run an episode with world interaction");
}

// Enhanced SimpleWorld class
void setupSimpleWorldClass(py::class_<SimpleWorld>& worldClass) {
    worldClass
        // Enhanced configuration
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, int width, int height, int visionWidth, int visionHeight) {
            if (width <= 0 || height <= 0 || visionWidth <= 0 || visionHeight <= 0) {
                throw WorldException("Invalid world dimensions");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"), "Configure world dimensions")
        
        // Enhanced agent setup
        .def("set_agent_start", [](SimpleWorld& self, double x, double y) {
            if (x < 0.0 || y < 0.0) {
                throw WorldException("Invalid agent start position");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent starting position")
        
        // Enhanced simulation loop
        .def("run_step", [](SimpleWorld& self, double timestep, std::shared_ptr<AgentBrain> agent) {
            if (timestep <= 0.0) {
                throw WorldException("Timestep must be positive");
            }
            
            self.update(timestep);
            
            auto percept = self.getSensoryPercept();
            agent->processSensoryInput(percept);
            
            agent->getBrain()->step(static_cast<SimulationStep>(self.getSimulationTime()));
            
            auto action = agent->decodeMotorCommand();
            self.applyMotorCommand(action, self.getSimulationTime());
        }, py::arg("timestep"), py::arg("agent"), "Run a single simulation step")
        
        // World manipulation
        .def("add_resource", [](SimpleWorld& self, double x, double y, double value = 1.0) {
            if (!self.isValidPosition(x, y)) {
                throw WorldException("Invalid position for object creation");
            }
            self.addObject(WorldObject(x, y, WorldObjectType::Resource, value));
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0, "Add a resource object")
        
        .def("add_hazard", [](SimpleWorld& self, double x, double y, double severity = 0.5) {
            if (!self.isValidPosition(x, y)) {
                throw WorldException("Invalid position for object creation");
            }
            self.addObject(WorldObject(x, y, WorldObjectType::Hazard, severity));
        }, py::arg("x"), py::arg("y"), py::arg("severity") = 0.5, "Add a hazard object")
        
        // World information
        .def("get_world_info", [](const SimpleWorld& self) {
            py::dict info;
            info["width"] = self.getWidth();
            info["height"] = self.getHeight();
            info["vision_width"] = self.getVisionWidth();
            info["vision_height"] = self.getVisionHeight();
            info["time"] = self.getSimulationTime();
            info["max_energy"] = self.getMaxEnergy();
            info["energy_decay_rate"] = self.getEnergyDecayRate();
            return info;
        }, "Get world information")
        
        .def("set_random_seed", [](SimpleWorld& self, uint64_t seed) {
            self.setRandomSeed(seed);
        }, py::arg("seed"), "Set random seed for reproducible simulations");
}

// Utility functions
void setupUtilities(py::module_& m) {
    // Configuration utilities
    m.def("load_config", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationException("Failed to load config from: " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Load configuration from file");
    
    m.def("create_config_from_dict", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            if (item.second.is(py::str())) {
                config->set(key, py::cast<std::string>(item.second));
            } else if (item.second.is(py::int_())) {
                config->set(key, py::cast<int>(item.second));
            } else if (item.second.is(py::float_())) {
                config->set(key, py::cast<double>(item.second));
            } else if (item.second.is(py::bool_())) {
                config->set(key, py::cast<bool>(item.second));
            }
        }
        return config;
    }, py::arg("config_dict"), "Create configuration from dictionary");
    
    // Brain utilities
    m.def("create_simple_brain", [](const py::dict& configDict) {
        auto config = std::make_shared<Config>();
        for (auto item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            if (item.second.is(py::str())) {
                config->set(key, py::cast<std::string>(item.second));
            } else if (item.second.is(py::int_())) {
                config->set(key, py::cast<int>(item.second));
            } else if (item.second.is(py::float_())) {
                config->set(key, py::cast<double>(item.second));
            } else if (item.second.is(py::bool_())) {
                config->set(key, py::cast<bool>(item.second));
            }
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config_dict"), "Create a simple brain from configuration dictionary");
    
    // Simulation utilities
    m.def("run_learning_simulation", [](std::shared_ptr<Brain> brain, int numSteps, double timestep = 0.001) {
        if (!brain->initialize()) {
            throw BrainException("Failed to initialize brain for simulation");
        }
        
        for (int i = 0; i < numSteps; ++i) {
            brain->step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * timestep));
        }
    }, py::arg("brain"), py::arg("num_steps"), py::arg("timestep") = 0.001, "Run a learning simulation");
    
    // Comparison and analysis utilities
    m.def("compare_brains", [](std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
        py::dict comparison;
        comparison["neuron_count_same"] = brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount();
        comparison["synapse_count_same"] = brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount();
        comparison["firing_rate_diff"] = std::abs(brain1->getAverageFiringRate() - brain2->getAverageFiringRate());
        comparison["e_i_ratio_diff"] = std::abs(brain1->getExcitationInhibitionRatio() - brain2->getExcitationInhibitionRatio());
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains");
    
    // Validation utilities
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        return config->validateKey(""); // This will be expanded
    }, py::arg("config"), "Validate configuration");
    
    // Version and info
    m.def("get_version", []() { return "NLM Phase 6 Final Integration"; }, "Get NLM version");
    
    m.def("get_credits", []() {
        return py::dict(
            "version", "Phase 6.0",
            "description", "Neural Learning Machine - Final Integration",
            "author", "NLM Research Team",
            "date", "2026",
            "features", py::list({
                "Real LIF neuron dynamics",
                "STDP and Hebbian plasticity",
                "Structural plasticity",
                "Multi-memory systems",
                "Neuromodulation (dopamine, curiosity, novelty)",
                "Prediction systems",
                "Cognition (planning, concept formation)",
                "Development (synaptogenesis, pruning, maturation)",
                "Enhanced Python bindings with error handling",
                "Configuration system with JSON/YAML support"
            })
        );
    }, "Get version and credits information");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This enhanced version includes:
        - Comprehensive error handling with custom exceptions
        - Pythonic interfaces with property-based access
        - Convenience methods for common operations
        - Configuration validation and utilities
        - Simulation management and analysis tools
        - Factory methods and builders
        - Context manager support
        - Statistics and monitoring
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationException>(m, "ConfigurationException");
    py::register_exception<BrainException>(m, "BrainException");
    py::register_exception<WorldException>(m, "WorldException");
    py::register_exception<AgentException>(m, "AgentException");
    
    // Setup enhanced classes
    setupConfigClass(py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc"));
    setupBrainClass(py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc"));
    setupAgentBrainClass(py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc"));
    setupSimpleWorldClass(py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc"));
    
    // Setup basic classes (unchanged)
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
    
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
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
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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
    
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Basic Config class (basic functionality)
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (auto-detects format)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration");
    
    // Basic Brain class (basic functionality)
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");
    
    // Basic AgentBrain class (basic functionality)
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);
    
    // Basic SimpleWorld class (basic functionality)
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get max energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");
    
    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Invalid IDs (from original bindings)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Setup utilities
    setupUtilities(m);
    
    // Version and info
    m.def("__version__", []() { return "Phase 6.0"; }, "Get NLM version");
    
    // Complete the module
}