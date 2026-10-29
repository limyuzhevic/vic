#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <map>
#include <variant>

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

// Forward declarations for additional classes we'll define
class SimulationManager;
class ExperimentRunner;
class Visualization;

// Custom exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

// Helper functions for type conversions
inline py::object variantToPython(const nlm::ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return py::cast(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return py::cast(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return py::cast(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return py::cast(std::get<bool>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return py::cast(std::get<std::string>(value));
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return py::cast(std::get<std::vector<int>>(value));
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return py::cast(std::get<std::vector<double>>(value));
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return py::cast(std::get<std::vector<std::string>>(value));
    } else if (std::holds_alternative<nlm::NestedConfig>(value)) {
        return py::cast(std::get<nlm::NestedConfig>(value));
    }
    return py::none();
}

// Configuration helper functions
inline std::shared_ptr<Config> createConfigFromJSON(const std::string& json_str) {
    auto config = std::make_shared<Config>();
    
    try {
        nlohmann::json j = nlohmann::json::parse(json_str);
        
        // Handle nested JSON structure
        if (j.contains("config") && j["config"].is_object()) {
            const auto& config_obj = j["config"];
            for (const auto& item : config_obj.items()) {
                if (item.value().is_object() && item.value().contains("value")) {
                    const auto& value = item.value()["value"];
                    const auto& source = item.value().value_or<std::string>("source", "Default");
                    
                    if (value.is_number_integer()) {
                        config->set(item.key(), value.get<int64_t>(), ConfigSource::Runtime);
                    } else if (value.is_number_float()) {
                        config->set(item.key(), value.get<double>(), ConfigSource::Runtime);
                    } else if (value.is_boolean()) {
                        config->set(item.key(), value.get<bool>(), ConfigSource::Runtime);
                    } else if (value.is_string()) {
                        config->set(item.key(), value.get<std::string>(), ConfigSource::Runtime);
                    } else if (value.is_array()) {
                        if (value.front().is_number_integer()) {
                            config->set(item.key(), value.get<std::vector<int64_t>>(), ConfigSource::Runtime);
                        } else if (value.front().is_number_float()) {
                            config->set(item.key(), value.get<std::vector<double>>(), ConfigSource::Runtime);
                        } else if (value.front().is_string()) {
                            config->set(item.key(), value.get<std::vector<std::string>>(), ConfigSource::Runtime);
                        }
                    }
                } else {
                    // Direct value
                    if (item.value().is_number_integer()) {
                        config->set(item.key(), item.value().get<int64_t>(), ConfigSource::Runtime);
                    } else if (item.value().is_number_float()) {
                        config->set(item.key(), item.value().get<double>(), ConfigSource::Runtime);
                    } else if (item.value().is_boolean()) {
                        config->set(item.key(), item.value().get<bool>(), ConfigSource::Runtime);
                    } else if (item.value().is_string()) {
                        config->set(item.key(), item.value().get<std::string>(), ConfigSource::Runtime);
                    }
                }
            }
        } else {
            // Handle flat structure
            for (const auto& item : j.items()) {
                if (item.value().is_number_integer()) {
                    config->set(item.key(), item.value().get<int64_t>(), ConfigSource::Runtime);
                } else if (item.value().is_number_float()) {
                    config->set(item.key(), item.value().get<double>(), ConfigSource::Runtime);
                } else if (item.value().is_boolean()) {
                    config->set(item.key(), item.value().get<bool>(), ConfigSource::Runtime);
                } else if (item.value().is_string()) {
                    config->set(item.key(), item.value().get<std::string>(), ConfigSource::Runtime);
                } else if (item.value().is_array()) {
                    // Array values - try to determine type
                    if (item.value().empty()) continue;
                    if (item.value()[0].is_number_integer()) {
                        config->set(item.key(), item.value().get<std::vector<int64_t>>(), ConfigSource::Runtime);
                    } else if (item.value()[0].is_number_float()) {
                        config->set(item.key(), item.value().get<std::vector<double>>(), ConfigSource::Runtime);
                    } else if (item.value()[0].is_string()) {
                        config->set(item.key(), item.value().get<std::vector<std::string>>(), ConfigSource::Runtime);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to parse JSON: ") + e.what());
    }
    
    return config;
}

inline std::string configToJSON(const Config& config) {
    nlohmann::json j;
    
    try {
        const auto& keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get<nlm::ConfigValue>(key);
            if (value) {
                j[key] = json_utils::toJson(*value);
            }
        }
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to serialize config to JSON: ") + e.what());
    }
    
    return j.dump(2);
}

// Create a more Pythonic config class
class PyConfig {
public:
    PyConfig() : config_(std::make_shared<Config>()) {}
    PyConfig(const std::string& json_str) : config_(createConfigFromJSON(json_str)) {}
    
    // Load from file
    bool loadFromFile(const std::string& filepath) {
        try {
            return config_->loadFromFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Failed to load config from file ") + filepath + ": " + e.what());
        }
    }
    
    // Load from args
    bool loadFromArgs(int argc, char** argv) {
        try {
            return config_->loadFromArgs(argc, argv);
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Failed to load config from args: ") + e.what());
        }
    }
    
    // Save to file
    bool saveToFile(const std::string& filepath) const {
        try {
            return config_->saveToFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Failed to save config to file ") + filepath + ": " + e.what());
        }
    }
    
    // Get values with type checking
    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        try {
            auto value = config_->get<T>(key);
            if (value) {
                return *value;
            }
            return default_value;
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Failed to get config value for key ") + key + ": " + e.what());
        }
    }
    
    // Set values
    void set(const std::string& key, const py::object& value) {
        try {
            if (py::isinstance<int>(value)) {
                config_->set(key, value.cast<int>());
            } else if (py::isinstance<double>(value)) {
                config_->set(key, value.cast<double>());
            } else if (py::isinstance<bool>(value)) {
                config_->set(key, value.cast<bool>());
            } else if (py::isinstance<std::string>(value)) {
                config_->set(key, value.cast<std::string>());
            } else if (py::isinstance<std::vector<int>>(value)) {
                config_->set(key, value.cast<std::vector<int>>());
            } else if (py::isinstance<std::vector<double>>(value)) {
                config_->set(key, value.cast<std::vector<double>>());
            } else if (py::isinstance<std::vector<std::string>>(value)) {
                config_->set(key, value.cast<std::vector<std::string>>());
            } else {
                throw ConfigError("Unsupported config value type");
            }
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Failed to set config value for key ") + key + ": " + e.what());
        }
    }
    
    // Check existence
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get keys
    std::vector<std::string> keys() const {
        return config_->getKeys();
    }
    
    // Get summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Convert to JSON string
    std::string toJSON() const {
        return configToJSON(*config_);
    }
    
    // Access underlying config
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
    // Static convenience methods
    static std::shared_ptr<PyConfig> createDefault() {
        return std::make_shared<PyConfig>();
    }
    
    static std::shared_ptr<PyConfig> createFromJSON(const std::string& json_str) {
        return std::make_shared<PyConfig>(json_str);
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Enhanced Brain class with Pythonic patterns
class PyBrain {
public:
    PyBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    PyBrain(const std::string& config_json) {
        auto config = createConfigFromJSON(config_json);
        brain_ = std::make_shared<Brain>(config);
    }
    
    // Initialize
    bool initialize() {
        try {
            return brain_->initialize();
        } catch (const std::exception& e) {
            throw BrainError(std::string("Failed to initialize brain: ") + e.what());
        }
    }
    
    // Step methods
    void step(SimulationStep currentStep) {
        brain_->step(currentStep);
    }
    
    void stepWithTime(SimulationStep currentStep, Timestamp currentTime) {
        brain_->step(currentStep, currentTime);
    }
    
    // Run episode
    void runEpisode(size_t steps) {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain_->step(step);
        }
    }
    
    // Convenience methods
    void injectSensory(const SensoryInput& input) {
        brain_->receiveSensoryInput(input);
    }
    
    void injectCurrentToAll(NeuronType type, MembranePotential current) {
        brain_->injectCurrentToNeurons(type, current);
    }
    
    // Statistics
    float getEIH() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalSpikes() const {
        return brain_->getTotalSpikeCount();
    }
    
    // Context manager support
    void __enter__() {
        initialize();
    }
    
    void __exit__(int, const std::string&, int, const std::string&) {
        brain_->reset();
    }
    
    // Get brain
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
    // Static convenience methods
    static std::shared_ptr<PyBrain> createDefault(const std::string& config_json = "") {
        if (config_json.empty()) {
            auto config = std::make_shared<Config>();
            return std::make_shared<PyBrain>(config);
        }
        return std::make_shared<PyBrain>(config_json);
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Enhanced SimpleWorld class
class PySimpleWorld {
public:
    PySimpleWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    void configure(float width, float height, size_t visionWidth, size_t visionHeight) {
        world_->configure(width, height, visionWidth, visionHeight);
    }
    
    void reset() {
        world_->reset();
    }
    
    void setAgentStart(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    void update(double timestep) {
        world_->update(timestep);
    }
    
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime) {
        return world_->applyMotorCommand(cmd, currentTime);
    }
    
    // Context manager
    void __enter__() {
        reset();
    }
    
    void __exit__(int, const std::string&, int, const std::string&) {
        // Cleanup if needed
    }
    
    // Batch operations
    void runSteps(size_t steps, double timestep = 0.01) {
        for (size_t i = 0; i < steps; ++i) {
            update(timestep);
        }
    }
    
    // Get world
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world_;
    }
    
    // Static convenience method
    static std::shared_ptr<PySimpleWorld> create(float width = 100.0f, float height = 100.0f,
                                                   size_t visionWidth = 20, size_t visionHeight = 20) {
        auto py_world = std::make_shared<PySimpleWorld>();
        py_world->configure(width, height, visionWidth, visionHeight);
        return py_world;
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

// Enhanced AgentBrain class
class PyAgentBrain {
public:
    PyAgentBrain(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    
    // Initialize with world
    void initialize(PySimpleWorld& world) {
        agent_brain_->initialize(*world.getWorld());
    }
    
    // Process input and decode action
    MotorCommand processAndDecode(const SensoryPercept& percept) {
        agent_brain_->processSensoryInput(percept);
        return agent_brain_->decodeMotorCommand();
    }
    
    // Complete agent cycle
    MotorCommand processEpisode(PySimpleWorld& world, const SensoryPercept& percept, double timestep) {
        initialize(world);
        processAndDecode(percept);
        updateDevelopment(timestep);
        return decodeMotorCommand();
    }
    
    // Development update
    void updateDevelopment(double timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    // Get brain
    std::shared_ptr<Brain> getBrain() const {
        return agent_brain_->getBrain();
    }
    
    // Static convenience method
    static std::shared_ptr<PyAgentBrain> createWithBrain(std::shared_ptr<Brain> brain) {
        return std::make_shared<PyAgentBrain>(brain);
    }
    
private:
    std::shared_ptr<AgentBrain> agent_brain_;
};

// SimulationManager class for managing simulation state
class SimulationManager {
public:
    SimulationManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world), step_count_(0) {}
    
    // Run simulation with checkpointing
    void runEpisode(size_t steps, double timestep = 0.01) {
        for (size_t i = 0; i < steps; ++i) {
            world_->update(timestep);
            
            // Get sensory input
            auto percept = world_->getSensoryPercept();
            
            // Process in brain
            brain_->step(step_count_, world_->getSimulationTime());
            brain_->receiveSensoryInput(percept);
            
            step_count_++;
            
            // Save checkpoint every 100 steps
            if (step_count_ % 100 == 0) {
                saveCheckpoint("checkpoint_" + std::to_string(step_count_) + ".bin");
            }
        }
    }
    
    // Save checkpoint
    bool saveCheckpoint(const std::string& filepath) {
        try {
            brain_->save(filepath);
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Failed to save checkpoint ") + filepath + ": " + e.what());
            return false;
        }
    }
    
    // Load checkpoint
    bool loadCheckpoint(const std::string& filepath) {
        try {
            brain_->load(filepath);
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Failed to load checkpoint ") + filepath + ": " + e.what());
            return false;
        }
    }
    
    // Get current state
    std::map<std::string, py::object> getState() const {
        std::map<std::string, py::object> state;
        
        // Add brain statistics
        state["total_spikes"] = py::cast(brain_->getTotalSpikeCount());
        state["active_neurons"] = py::cast(brain_->getActiveNeuronCount());
        state["firing_neurons"] = py::cast(brain_->getFiringNeuronCount());
        state["average_firing_rate"] = py::cast(brain_->getAverageFiringRate());
        state["eih_ratio"] = py::cast(brain_->getExcitationInhibitionRatio());
        state["step_count"] = py::cast(step_count_);
        
        return state;
    }
    
    // Reset manager
    void reset() {
        brain_->reset();
        step_count_ = 0;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    size_t step_count_;
};

// ExperimentRunner for predefined experiments
class ExperimentRunner {
public:
    ExperimentRunner() {}
    
    // Run simple agent experiment
    void runAgentExperiment(PySimpleWorld& world, PyAgentBrain& agent, size_t episode_steps, 
                          size_t num_episodes) {
        for (size_t episode = 0; episode < num_episodes; ++episode) {
            // Reset for new episode
            world.reset();
            agent.reset();
            
            // Run episode
            for (size_t step = 0; step < episode_steps; ++step) {
                double timestep = 0.01;
                
                // Get current percept
                auto percept = world.getWorld()->getSensoryPercept();
                
                // Agent processes input
                auto action = agent.processAndDecode(percept);
                
                // Apply action
                world.update(timestep);
                
                // Advance brain
                agent.getBrain()->step(step, world.getWorld()->getSimulationTime());
            }
        }
    }
    
    // Run brain stimulation experiment
    void runBrainStimulationExperiment(PyBrain& brain, size_t steps, NeuronType neuron_type, 
                                      MembranePotential current) {
        brain.initialize();
        brain.injectCurrentToAll(neuron_type, current);
        brain.runEpisode(steps);
    }
    
    // Run reward modulation experiment
    void runRewardExperiment(PySimpleWorld& world, PyAgentBrain& agent, 
                           std::vector<float> rewards, size_t steps_per_episode) {
        for (size_t episode = 0; episode < rewards.size(); ++episode) {
            world.reset();
            agent.reset();
            
            for (size_t step = 0; step < steps_per_episode; ++step) {
                auto percept = world.getWorld()->getSensoryPercept();
                auto action = agent.processAndDecode(percept);
                
                // Apply reward modulation
                agent.getBrain()->applyRewardModulation(rewards[episode], 0.0f);
                
                world.update(0.01);
                agent.getBrain()->step(step, world.getWorld()->getSimulationTime());
            }
        }
    }
};

// Visualization utilities
class Visualization {
public:
    Visualization() {}
    
    // Generate simple ASCII visualization of neural activity
    std::string visualizeBrain(const PyBrain& brain, size_t width = 50, size_t height = 30) {
        std::stringstream ss;
        
        // Get brain statistics
        size_t active = brain.getBrain()->getActiveNeuronCount();
        size_t total = brain.getBrain()->getTotalNeuronCount();
        
        if (total == 0) {
            return "No neurons in brain";
        }
        
        float activity_ratio = static_cast<float>(active) / total;
        
        // Simple bar chart
        ss << "Neural Activity Visualization\n";
        ss << "==============================\n";
        ss << "Active Neurons: " << active << "/" << total << " (" 
           << std::fixed << std::setprecision(1) << (activity_ratio * 100.0) << "%)\n\n";
        
        // Create activity bar
        size_t bar_length = width * activity_ratio;
        ss << "Activity: [";
        for (size_t i = 0; i < bar_length; ++i) {
            ss << "#";
        }
        for (size_t i = bar_length; i < width; ++i) {
            ss << " ";
        }
        ss << "]\n";
        
        // Add statistics
        ss << "\nStatistics:\n";
        ss << "  Total Spikes: " << brain.getBrain()->getTotalSpikeCount() << "\n";
        ss << "  Firing Rate: " << std::fixed << std::setprecision(2) 
           << brain.getBrain()->getAverageFiringRate() << " Hz\n";
        ss << "  E/I Ratio: " << std::fixed << std::setprecision(2) 
           << brain.getBrain()->getExcitationInhibitionRatio() << "\n";
        
        return ss.str();
    }
    
    // Create world map visualization
    std::string visualizeWorld(const PySimpleWorld& world) {
        std::stringstream ss;
        auto w = world.getWorld();
        
        ss << "World Visualization\n";
        ss << "===================\n";
        ss << "Dimensions: " << w->getWidth() << " x " << w->getHeight() << "\n";
        ss << "Simulation Time: " << w->getSimulationTime() << "\n";
        ss << "Agent Position: (" << w->getAgentBody().x << ", " << w->getAgentBody().y << ")\n";
        
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides enhanced, Pythonic APIs for Brain, Config, SimpleWorld,
        AgentBrain, and advanced simulation management.
        
        Key features:
        - Pythonic patterns (__iter__, __len__, context managers)
        - Convenience methods for common operations
        - Enhanced error handling with descriptive messages
        - SimulationManager for checkpointing and state management
        - ExperimentRunner for predefined experiments
        - Visualization utilities
        - Type hints and comprehensive documentation
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);

    // Register core types
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    // Enhanced Config class with Pythonic API
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("json_str"),
             "Create configuration from JSON string")
        .def("loadFromFile", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", &PyConfig::loadFromArgs, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        .def("saveToFile", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &PyConfig::keys, "Get all configuration keys")
        .def("clear", &PyConfig::clear, "Clear all configuration entries")
        .def("summary", &PyConfig::summary, "Get a summary string of the configuration")
        // Pythonic property-like access
        .def("__getitem__", [](PyConfig& self, const std::string& key) {
            auto value = self.get<double>(key, 0.0);
            return py::cast(value);
        })
        .def("__setitem__", &PyConfig::set)
        .def("__contains__", &PyConfig::has)
        .def("__repr__", [](const PyConfig& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", &PyConfig::keys)
        .def("__iter__", [](PyConfig& self) {
            return py::make_iterator(self.keys().begin(), self.keys().end());
        }, py::return_value_policy::reference)
        // Convenience methods
        .def("createDefault", &PyConfig::createDefault, "Create default configuration")
        .def("createFromJSON", &PyConfig::createFromJSON, py::arg("json_str"),
             "Create configuration from JSON string (class method)")
        .def("toJSON", &PyConfig::toJSON, "Convert configuration to JSON string")
        .def("getOr", &PyConfig::get, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default");

    // Enhanced Brain class with Pythonic API
    py::class_<PyBrain>(m, "Brain", R"pbdoc(Enhanced neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration object")
        .def(py::init<const std::string&>(), py::arg("config_json"),
             "Create brain with JSON configuration string")
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", &PyBrain::step, py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", &PyBrain::stepWithTime, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("runEpisode", &PyBrain::runEpisode, py::arg("steps"),
             "Run a complete simulation episode with specified number of steps")
        .def("receiveSensoryInput", &PyBrain::injectSensory, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &PyBrain::injectSensory, py::arg("input"),
             "Inject current into a specific neuron (convenience method)")
        .def("injectCurrentToAll", &PyBrain::injectCurrentToAll, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &PyBrain::getBrain,
             "Produce motor action based on neural activity")
        .def("reset", &PyBrain::getBrain,
             "Reset brain state")
        .def("save", &PyBrain::getBrain, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PyBrain::getBrain, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &PyBrain::getBrain, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &PyBrain::getBrain, py::arg("id"),
             "Get a region by ID")
        .def("getRegionCount", &PyBrain::getBrain,
             "Get the number of regions")
        .def("getRegionIds", &PyBrain::getBrain,
             "Get all region IDs")
        .def("getRegions", &PyBrain::getBrain,
             "Get all regions")
        .def("getTotalNeuronCount", &PyBrain::getBrain,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &PyBrain::getBrain,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &PyBrain::getBrain,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &PyBrain::getBrain,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &PyBrain::getBrain,
             "Get average firing rate across all neurons")
        .def("getEIH", &PyBrain::getEIH,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &PyBrain::getTotalSpikes,
             "Get total spike count")
        .def("getDevelopmentalStage", &PyBrain::getBrain,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &PyBrain::getBrain, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &PyBrain::getBrain,
             "Get the configuration")
        .def("logStatus", &PyBrain::getBrain,
             "Log brain status")
        // Pythonic patterns
        .def("__enter__", &PyBrain::__enter__)
        .def("__exit__", &PyBrain::__exit__, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"))
        // Convenience class methods
        .def("createDefault", &PyBrain::createDefault, py::arg("config_json") = "",
             "Create default brain with optional JSON configuration")
        // Rich comparison methods for IDs
        .def("__eq__", [](const PyBrain& a, const PyBrain& b) {
            return a.getBrain() == b.getBrain();
        })
        .def("__ne__", [](const PyBrain& a, const PyBrain& b) {
            return a.getBrain() != b.getBrain();
        });

    // Enhanced SimpleWorld class with Pythonic API
    py::class_<PySimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 20, py::arg("visionHeight") = 20,
             "Configure world dimensions and vision")
        .def("reset", &PySimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &PySimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &PySimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &PySimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &PySimpleWorld::getWorld,
             "Get sensory percept for the agent")
        .def("getAgentBody", &PySimpleWorld::getWorld,
             "Get agent body state")
        .def("addObject", &PySimpleWorld::getWorld, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &PySimpleWorld::getWorld, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &PySimpleWorld::getWorld, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getWidth", &PySimpleWorld::getWorld,
             "Get world width")
        .def("getHeight", &PySimpleWorld::getWorld,
             "Get world height")
        .def("getMaxEnergy", &PySimpleWorld::getWorld,
             "Get maximum energy")
        .def("setMaxEnergy", &PySimpleWorld::getWorld, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &PySimpleWorld::getWorld,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &PySimpleWorld::getWorld, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &PySimpleWorld::getWorld,
             "Get simulation time")
        .def("setRandomSeed", &PySimpleWorld::getWorld, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &PySimpleWorld::getWorld,
             "Get random seed")
        // Pythonic patterns
        .def("__enter__", &PySimpleWorld::__enter__)
        .def("__exit__", &PySimpleWorld::__exit__, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"))
        // Batch operations
        .def("runSteps", &PySimpleWorld::runSteps, py::arg("steps"), py::arg("timestep") = 0.01,
             "Run multiple simulation steps efficiently")
        // Convenience class method
        .def("create", &PySimpleWorld::create, py::arg("width") = 100.0f, py::arg("height") = 100.0f,
             py::arg("visionWidth") = 20, py::arg("visionHeight") = 20,
             "Create and configure world in one call");

    // Enhanced AgentBrain class with Pythonic API
    py::class_<PyAgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with brain object")
        .def("initialize", &PyAgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &PyAgentBrain::getBrain,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &PyAgentBrain::getBrain,
             "Get expected motor output size")
        .def("processSensoryInput", &PyAgentBrain::processAndDecode, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &PyAgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &PyAgentBrain::getBrain, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &PyAgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &PyAgentBrain::getBrain,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &PyAgentBrain::getBrain,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &PyAgentBrain::getBrain,
             "Get curiosity level")
        .def("getNoveltyLevel", &PyAgentBrain::getBrain,
             "Get novelty level")
        .def("getPredictionError", &PyAgentBrain::getBrain,
             "Get prediction error")
        .def("reset", &PyAgentBrain::getBrain,
             "Reset agent for new episode")
        .def("getBrain", &PyAgentBrain::getBrain,
             "Get the underlying brain")
        // Configuration methods
        .def("enableRewardModulation", &PyAgentBrain::getBrain, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &PyAgentBrain::getBrain, py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &PyAgentBrain::getBrain, py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &PyAgentBrain::getBrain, py::arg("enable"),
             "Enable/disable curiosity")
        // Convenience methods
        .def("processEpisode", &PyAgentBrain::processEpisode, py::arg("world"), py::arg("percept"),
             py::arg("timestep"), "Process complete agent episode")
        // Convenience class method
        .def("createWithBrain", &PyAgentBrain::createWithBrain, py::arg("brain"),
             "Create agent brain with existing brain object");

    // SimulationManager - manage simulation state and checkpoints
    py::class_<SimulationManager>(m, "SimulationManager", R"pbdoc(Manage simulation state and checkpoints)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>(),
             py::arg("brain"), py::arg("world"),
             "Create simulation manager with brain and world")
        .def("runEpisode", &SimulationManager::runEpisode, py::arg("steps"), py::arg("timestep") = 0.01,
             "Run simulation episode with automatic checkpointing")
        .def("saveCheckpoint", &SimulationManager::saveCheckpoint, py::arg("filepath"),
             "Save current simulation state to checkpoint")
        .def("loadCheckpoint", &SimulationManager::loadCheckpoint, py::arg("filepath"),
             "Load simulation state from checkpoint")
        .def("getState", &SimulationManager::getState,
             "Get current simulation state as dictionary")
        .def("reset", &SimulationManager::reset,
             "Reset simulation manager");

    // ExperimentRunner - run predefined experiments
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Run predefined experiments)pbdoc")
        .def(py::init<>())
        .def("runAgentExperiment", &ExperimentRunner::runAgentExperiment, py::arg("world"), py::arg("agent"),
             py::arg("episode_steps"), py::arg("num_episodes"),
             "Run agent-based experiment")
        .def("runBrainStimulationExperiment", &ExperimentRunner::runBrainStimulationExperiment,
             py::arg("brain"), py::arg("steps"), py::arg("neuron_type"), py::arg("current"),
             "Run brain stimulation experiment")
        .def("runRewardExperiment", &ExperimentRunner::runRewardExperiment,
             py::arg("world"), py::arg("agent"), py::arg("rewards"), py::arg("steps_per_episode"),
             "Run reward modulation experiment");

    // Visualization - basic visualization utilities
    py::class_<Visualization>(m, "Visualization", R"pbdoc(Basic visualization utilities)pbdoc")
        .def(py::init<>())
        .def("visualizeBrain", &Visualization::visualizeBrain, py::arg("brain"), py::arg("width") = 50, py::arg("height") = 30,
             "Create ASCII visualization of brain activity")
        .def("visualizeWorld", &Visualization::visualizeWorld, py::arg("world"),
             "Create ASCII visualization of world state");

    // Convenience function creators (maintain backward compatibility)
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

    // Helper functions
    m.def("createConfigFromJSON", &createConfigFromJSON, py::arg("json_str"),
         "Create configuration from JSON string");

    m.def("configToJSON", &configToJSON, py::arg("config"),
         "Convert configuration to JSON string");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced type conversions for Pythonic API
    py::implicitly_convertible<std::shared_ptr<Config>, std::shared_ptr<PyConfig>>();
    py::implicitly_convertible<std::shared_ptr<Brain>, std::shared_ptr<PyBrain>>();
    py::implicitly_convertible<std::shared_ptr<SimpleWorld>, std::shared_ptr<PySimpleWorld>>();
    py::implicitly_convertible<std::shared_ptr<AgentBrain>, std::shared_ptr<PyAgentBrain>>();

    // Documentation examples
    m.def("exampleSimpleSimulation", R"pbdoc(
        Example: Simple simulation
        ----------------------------
        ```python
        import pynlm as nlm
        
        # Create world and brain
        world = nlm.SimpleWorld.create(100, 100)
        brain = nlm.Brain.createDefault()
        
        # Initialize
        brain.initialize()
        
        # Run episode
        brain.runEpisode(100)
        
        # Create agent
        agent = nlm.AgentBrain.createWithBrain(brain)
        world.initialize(agent)
        
        # Visualizations
        viz = nlm.Visualization()
        print(viz.visualizeBrain(brain))
        print(viz.visualizeWorld(world))
        ```
    )pbdoc");

    m.def("exampleConfigManagement", R"pbdoc(
        Example: Configuration management
        -----------------------------------
        ```python
        import pynlm as nlm
        
        # Create config from JSON
        config_json = '''{
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1
        }'''
        config = nlm.Config.createFromJSON(config_json)
        
        # Modify config
        config["learning_rate"] = 0.01
        config["batch_size"] = 32
        
        # Save config
        config.saveToFile("my_config.json")
        
        # Create brain with config
        brain = nlm.Brain.createDefault(config.toJSON())
        ```
    )pbdoc");

    m.def("exampleExperimentRunner", R"pbdoc(
        Example: Experiment running
        -----------------------------
        ```python
        import pynlm as nlm
        
        # Setup
        world = nlm.SimpleWorld.create(200, 200)
        brain = nlm.Brain.createDefault()
        agent = nlm.AgentBrain.createWithBrain(brain)
        
        # Run experiment
        runner = nlm.ExperimentRunner()
        runner.runAgentExperiment(world, agent, 50, 10)
        
        # Get results
        manager = nlm.SimulationManager(brain, world)
        state = manager.getState()
        
        print(f"Total spikes: {state['total_spikes']}")
        print(f"Activity ratio: {state['active_neurons']/state['total_neurons']:.2%}")
        ```
    )pbdoc");

    // Backward compatibility constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced docstring with type hints
    m.def("__version__", []() { return "1.0.0"; });
    m.def("__author__", []() { return "NLM Development Team"; });
    m.def("__description__", []() { return "Enhanced Python bindings for NLM neural simulation framework"; });
}

} // namespace nlm
