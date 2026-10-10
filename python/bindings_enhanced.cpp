// Additional utility functions for enhanced Python bindings
// Provides convenience functions, error handling, and utility classes

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced error handling classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError("World Error: " + message) {}
};

// Type conversion utilities
struct PyTypeConverter {
    static std::string actionTypeToString(ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    static std::string neuronTypeToString(NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    static std::string synapseTypeToString(SynapseType type) {
        switch (type) {
            case SynapseType::Excitatory: return "Excitatory";
            case SynapseType::Inhibitory: return "Inhibitory";
            case SynapseType::Modulatory: return "Modulatory";
            case SynapseType::Electrical: return "Electrical";
            case SynapseType::GapJunction: return "GapJunction";
            default: return "Unknown";
        }
    }
};

// Convenience functions for common operations
inline std::shared_ptr<Config> createConfigWithDefaults() {
    auto config = std::make_shared<Config>();
    
    // Set some sensible defaults
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config->set("random_seed", 42, nlm::ConfigSource::Runtime);
    
    return config;
}

inline std::shared_ptr<Brain> createBrainWithSize(size_t neuronCount, size_t regionCount = 2) {
    auto config = createConfigWithDefaults();
    config->set("neuron_count", neuronCount, nlm::ConfigSource::Runtime);
    config->set("region_count", regionCount, nlm::ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

inline std::shared_ptr<SimpleWorld> createStandardWorld() {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(100, 100, 20, 20);
    world->setAgentStart(50, 50);
    return world;
}

inline std::shared_ptr<AgentBrain> createStandardAgentBrain(size_t neuronCount = 500) {
    auto brain = createBrainWithSize(neuronCount);
    auto agent_brain = std::make_shared<AgentBrain>(brain);
    
    auto world = createStandardWorld();
    agent_brain->initialize(world);
    
    return agent_brain;
}

// Utility classes for Python users
class PyBrainSimulator {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent_brain;
    size_t stepCount;
    
public:
    PyBrainSimulator() : stepCount(0) {}
    
    void initialize(size_t neuronCount = 500, size_t worldWidth = 100, 
                    size_t worldHeight = 100, size_t visionWidth = 20, 
                    size_t visionHeight = 20) {
        // Create components
        auto config = createConfigWithDefaults();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Runtime);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        
        agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(world);
        
        // Reset world to set agent position
        world->reset();
        auto agent_body = world->getAgentBody();
        world->setAgentStart(agent_body.x, agent_body.y);
        
        stepCount = 0;
    }
    
    float step() {
        if (!agent_brain || !world) {
            throw BrainError("Simulator not initialized");
        }
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        agent_brain->processSensoryInput(percept);
        
        // Get action from brain
        auto action = agent_brain->decodeMotorCommand();
        
        // Apply action in world
        world->applyMotorCommand(action.getType(), static_cast<Timestamp>(stepCount));
        
        // Update world
        world->update(static_cast<float>(stepCount) * 0.1f);
        
        stepCount++;
        return agent_body.energy; // Return current energy level
    }
    
    size_t getStepCount() const { return stepCount; }
    
    std::shared_ptr<Brain> getBrain() { return brain; }
    std::shared_ptr<SimpleWorld> getWorld() { return world; }
    std::shared_ptr<AgentBrain> getAgentBrain() { return agent_brain; }
    
    // Get current statistics
    py::dict getStatistics() {
        py::dict stats;
        
        if (brain) {
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        }
        
        if (world) {
            auto agent_body = world->getAgentBody();
            stats["agent_x"] = agent_body.x;
            stats["agent_y"] = agent_body.y;
            stats["agent_orientation"] = agent_body.orientation;
            stats["agent_energy"] = agent_body.energy;
            stats["agent_health"] = agent_body.health;
            stats["agent_age"] = agent_body.age;
            stats["world_width"] = world->getWidth();
            stats["world_height"] = world->getHeight();
            stats["world_time"] = world->getSimulationTime();
        }
        
        if (agent_brain) {
            stats["neuromodulation_level"] = agent_brain->getNeuromodulationLevel();
            stats["curiosity_level"] = agent_brain->getCuriosityLevel();
            stats["novelty_level"] = agent_brain->getNoveltyLevel();
            stats["prediction_error"] = agent_brain->getPredictionError();
        }
        
        return stats;
    }
};

class PyConfigManager {
private:
    std::shared_ptr<Config> config;
    std::string filename;
    
public:
    PyConfigManager() {
        config = std::make_shared<Config>();
    }
    
    explicit PyConfigManager(const std::string& filepath) {
        config = std::make_shared<Config>();
        filename = filepath;
        if (!filepath.empty()) {
            config->loadFromFile(filepath);
        }
    }
    
    void set(const std::string& key, const py::object& value) {
        if (py::isinstance<int>(value)) {
            config->set(key, value.cast<int>(), nlm::ConfigSource::Runtime);
        } else if (py::isinstance<double>(value)) {
            config->set(key, value.cast<double>(), nlm::ConfigSource::Runtime);
        } else if (py::isinstance<std::string>(value)) {
            config->set(key, value.cast<std::string>(), nlm::ConfigSource::Runtime);
        } else if (py::isinstance<bool>(value)) {
            config->set(key, value.cast<bool>(), nlm::ConfigSource::Runtime);
        } else {
            throw ConfigurationError("Unsupported value type for key: " + key);
        }
    }
    
    py::object get(const std::string& key) {
        if (!config->has(key)) {
            throw ConfigurationError("Key not found: " + key);
        }
        
        // Try to get value as appropriate type
        auto intVal = config->get<int>(key);
        if (intVal.has_value()) return *intVal;
        
        auto doubleVal = config->get<double>(key);
        if (doubleVal.has_value()) return *doubleVal;
        
        auto stringVal = config->get<std::string>(key);
        if (stringVal.has_value()) return *stringVal;
        
        auto boolVal = config->get<bool>(key);
        if (boolVal.has_value()) return *boolVal;
        
        throw ConfigurationError("Unsupported type for key: " + key);
    }
    
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    void save(const std::string& filepath) {
        config->saveToFile(filepath);
        filename = filepath;
    }
    
    std::string getSummary() const {
        return config->summary();
    }
    
    std::vector<std::string> getKeys() const {
        return config->getKeys();
    }
    
    void loadFromFile(const std::string& filepath) {
        config->loadFromFile(filepath);
        filename = filepath;
    }
    
    std::shared_ptr<Config> getInternalConfig() { return config; }
};

PYBIND11_MODULE(pynlm, m) {
    // Import the enhanced bindings from the main module first
    py::module::import("pynlm").attr("__all__");
    
    // Add error classes
    py::class_<NLMError, std::runtime_error>(m, "NLMError")
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>());
    
    py::class_<BrainError, NLMError>(m, "BrainError")
        .def(py::init<const std::string&>());
    
    py::class_<WorldError, NLMError>(m, "WorldError")
        .def(py::init<const std::string&>());
    
    // Add utility functions
    m.def("create_config_with_defaults", &createConfigWithDefaults,
          "Create configuration with sensible defaults");
    
    m.def("create_brain_with_size", &createBrainWithSize,
          py::arg("neuronCount"), py::arg("regionCount") = 2,
          "Create brain with specified neuron count");
    
    m.def("create_standard_world", &createStandardWorld,
          "Create a standard world for testing");
    
    m.def("create_standard_agent_brain", &createStandardAgentBrain,
          py::arg("neuronCount") = 500,
          "Create standard agent brain with world");
    
    // Add PyBrainSimulator class
    py::class_<PyBrainSimulator>(m, "BrainSimulator", 
        R"pbdoc(Complete brain simulation environment with world integration)pbdoc")
        .def(py::init<>())
        .def("initialize", &PyBrainSimulator::initialize,
             py::arg("neuronCount") = 500, py::arg("worldWidth") = 100,
             py::arg("worldHeight") = 100, py::arg("visionWidth") = 20,
             py::arg("visionHeight") = 20,
             "Initialize the simulator with world")
        .def("step", &PyBrainSimulator::step,
             "Run one simulation step, return agent energy")
        .def("getStepCount", &PyBrainSimulator::getStepCount,
             "Get current step count")
        .def("getStatistics", &PyBrainSimulator::getStatistics,
             "Get current simulation statistics as dictionary")
        .def("reset", [](PyBrainSimulator& self) {
            self.initialize();
        }, "Reset the simulator to initial state");
    
    // Add PyConfigManager class
    py::class_<PyConfigManager>(m, "ConfigManager",
        R"pbdoc(High-level configuration manager with Python-friendly interface)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"),
             "Create config manager from file or empty")
        .def("set", &PyConfigManager::set,
             py::arg("key"), py::arg("value"),
             "Set configuration value with Python type conversion")
        .def("get", &PyConfigManager::get,
             py::arg("key"),
             "Get configuration value with automatic type conversion")
        .def("has", &PyConfigManager::has,
             py::arg("key"),
             "Check if configuration key exists")
        .def("save", &PyConfigManager::save,
             py::arg("filepath"),
             "Save configuration to file")
        .def("getSummary", &PyConfigManager::getSummary,
             "Get configuration summary")
        .def("getKeys", &PyConfigManager::getKeys,
             "Get all configuration keys")
        .def("loadFromFile", &PyConfigManager::loadFromFile,
             py::arg("filepath"),
             "Load configuration from file");
    
    // Add type conversion helper functions
    m.def("get_neuron_type_name", [](NeuronType type) {
        return PyTypeConverter::neuronTypeToString(type);
    }, py::arg("type"), "Get string name for neuron type");
    
    m.def("get_synapse_type_name", [](SynapseType type) {
        return PyTypeConverter::synapseTypeToString(type);
    }, py::arg("type"), "Get string name for synapse type");
    
    m.def("get_action_type_name", [](ActionType type) {
        return PyTypeConverter::actionTypeToString(type);
    }, py::arg("type"), "Get string name for action type");
    
    // Add simulation convenience functions
    m.def("run_quick_test", [](size_t neuronCount = 100, size_t steps = 100) {
        auto config = createConfigWithDefaults();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i * 0.1f));
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("neuronCount") = 100, py::arg("steps") = 100,
       "Run a quick test simulation and return spike count");
    
    m.def("create_demo_brain", []() {
        auto brain = createBrainWithSize(200);
        
        // Add some random input to make it interesting
        for (size_t i = 0; i < 50; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 1.0f);
        }
        
        return brain;
    }, "Create a demo brain with initial stimulation");
    
    m.def("create_demo_world", []() {
        auto world = createStandardWorld();
        
        // Add some obstacles and resources
        nlm::WorldObject wall1;
        wall1.x = 30; wall1.y = 30; wall1.type = nlm::WorldObjectType::Wall;
        world->addObject(wall1);
        
        nlm::WorldObject resource1;
        resource1.x = 70; resource1.y = 70; resource1.type = nlm::WorldObjectType::Resource;
        resource1.value = 10.0f;
        world->addObject(resource1);
        
        nlm::WorldObject hazard1;
        hazard1.x = 85; hazard1.y = 85; hazard1.type = nlm::WorldObjectType::Hazard;
        hazard1.value = -5.0f;
        world->addObject(hazard1);
        
        return world;
    }, "Create a demo world with objects");
    
    // Add batch operations utilities
    m.def("batch_neuron_injection", [](std::shared_ptr<Brain> brain, 
                                     float current, size_t count, 
                                     NeuronType type = NeuronType::Excitatory) {
        for (size_t i = 0; i < count; ++i) {
            brain->injectCurrentToNeurons(type, current);
        }
    }, py::arg("brain"), py::arg("current"), py::arg("count"), 
       py::arg("type") = NeuronType::Excitatory,
       "Inject current to multiple neurons");
    
    m.def("create_neuron_group", [](size_t count, NeuronType type = NeuronType::Internal) {
        // This would require modifying the brain to support group creation
        // For now, just return a helpful message
        throw BrainError("Neuron group creation not yet implemented - requires brain modification");
    }, py::arg("count"), py::arg("type") = NeuronType::Internal,
       "Create a group of neurons (feature requires brain modifications)");
}
