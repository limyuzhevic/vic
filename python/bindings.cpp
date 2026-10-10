#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

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

// Python-specific exception types
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException("Memory Error: " + msg) {}
};

// Template helper for ID class bindings to reduce code duplication
template<typename IDClass>
void bindIdClass(py::module& m, const std::string& name) {
    py::class_<IDClass>(m, name.c_str(), "Unique identifier for " + name)
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDClass::value)
        .def("index", &IDClass::index)
        .def("__eq__", &IDClass::operator==)
        .def("__ne__", &IDClass::operator!=)
        .def("__hash__", [](const IDClass& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const IDClass& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const IDClass& id) {
            return std::to_string(id.value);
        });
}

// Pythonic factory functions and convenience functions
namespace factory {
    inline std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        // Set some reasonable defaults
        config->set("neuron_count", 100, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }
    
    inline std::shared_ptr<Brain> createBrain(const std::string& configPath = "") {
        auto config = createDefaultConfig();
        if (!configPath.empty()) {
            config->loadFromFile(configPath);
        }
        return std::make_shared<Brain>(config);
    }
    
    inline std::shared_ptr<SimpleWorld> createWorld(int width = 16, int height = 16, 
                                                    size_t visionWidth = 5, size_t visionHeight = 5) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }
    
    inline std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }
}

// Validation and utility functions
namespace utils {
    inline void validateConfig(const Config& config) {
        auto keys = config.getKeys();
        if (keys.empty()) {
            throw ConfigurationError("Configuration is empty");
        }
    }
    
    inline bool isPositiveNumber(float value) { return value >= 0.0f; }
    inline bool isPositiveInt(size_t value) { return value > 0; }
    
    inline std::vector<float> normalizeVector(const std::vector<float>& data, float min = 0.0f, float max = 1.0f) {
        if (data.empty()) return {};
        
        float currentMin = *std::min_element(data.begin(), data.end());
        float currentMax = *std::max_element(data.begin(), data.end());
        
        if (currentMax == currentMin) {
            return std::vector<float>(data.size(), (min + max) / 2.0f);
        }
        
        std::vector<float> result;
        result.reserve(data.size());
        for (float val : data) {
            float normalized = (val - currentMin) / (currentMax - currentMin);
            result.push_back(min + normalized * (max - min));
        }
        return result;
    }
    
    inline std::string vectorToString(const std::vector<float>& vec, const std::string& delimiter = ", ") {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << delimiter;
            oss << vec[i];
        }
        return oss.str();
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (熙然) - Neural Learning Machine Python Bindings
        -----------------------------------------------------
        
        A Python interface to the experimental artificial developmental brain project NLM.
        This framework provides a biologically-inspired neural architecture that learns
        and develops through experience.
        
        Key Features:
        - LIF neuron dynamics with event-driven spike propagation
        - Multiple memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages affecting plasticity
        - Integrated prediction and planning systems
        - Embodied agent in a simulated world
        
        Usage Example:
        ```python
        import pynlm
        
        # Create a brain
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Create a world and agent
        world = pynlm.createWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation loop
        for step in range(100):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Step brain
            brain.step(step, step * 0.001)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.001)
        ```
        
        See the documentation for comprehensive API reference.
    )pbdoc";

    // Register Python exception types
    py::register_exception<NLMException>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError.ptr());
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError.ptr());
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError.ptr());

    // Register version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for the NLM neural simulation framework";

    // ID class bindings using template helper
    bindIdClass<NeuronId>(m, "NeuronId");
    bindIdClass<SynapseId>(m, "SynapseId");
    bindIdClass<RegionId>(m, "RegionId");
    bindIdClass<PopulationId>(m, "PopulationId");

    // Enum bindings
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .doc() = "Type of neuron in the neural network";

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .doc() = "Type of synaptic connection";

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .doc() = "Developmental stage of the neural system";

    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .doc() = "Current firing state of a neuron";

    py::enum_<ActionType>(m, "ActionType")
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
        .export_values()
        .doc() = "High-level action type for agent behavior";

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .doc() = "Low-level motor command for world interaction";

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .doc() = "Type of object in the world";

    // Config class with Pythonic enhancements
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
        
        Configuration management for the NLM neural simulation framework.
        Provides access to simulation parameters and settings.
        
        Common parameters include:
        - neuron_count: Number of neurons in the network
        - region_count: Number of brain regions
        - connection_probability: Probability of synaptic connections
        - random_seed: Random number generator seed
        - simulation_timestep: Time step for simulation
        
        Example:
        ```python
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("random_seed", 123)
        ```
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("set", &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source"),
             "Set a configuration value with source")
        .def("getOr", py::overload_cast<const std::string&, int64_t>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default for int64_t")
        .def("getOr", py::overload_cast<const std::string&, double>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default for double")
        .def("getOr", py::overload_cast<const std::string&, float>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default for float")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // ... (rest of the bindings would continue here)
}