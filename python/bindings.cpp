#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

// Custom exception types for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message) : NLMException("Configuration error: " + message) {}
};

class BrainInitializationError : public NLMException {
public:
    explicit BrainInitializationError(const std::string& message) : NLMException("Brain initialization error: " + message) {}
};

// Helper functions for Pythonic usage
namespace utils {
    // Create a default configuration with common Phase 6 settings
    std::shared_ptr<Config> createPhase6Config() {
        auto config = std::make_shared<Config>();
        
        // Core brain settings
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Memory systems
        config->set("working_memory_capacity", 100, ConfigSource::Default);
        config->set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
        config->set("associative_memory_max_associations", 5000, ConfigSource::Default);
        
        // Neuromodulation
        config->set("dopamine_initial_level", 0.5f, ConfigSource::Default);
        config->set("novelty_sensitivity", 0.1f, ConfigSource::Default);
        config->set("curiosity_rate", 0.01f, ConfigSource::Default);
        
        // Development
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Prediction
        config->set("prediction_error_threshold", 0.5f, ConfigSource::Default);
        
        return config;
    }
    
    // Create a simple demo configuration
    std::shared_ptr<Config> createDemoConfig() {
        auto config = std::make_shared<Config>();
        
        // Smaller brain for quick demo
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Enhanced learning
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        
        return config;
    }
    
    // Create a fast configuration for testing
    std::shared_ptr<Config> createFastConfig() {
        auto config = std::make_shared<Config>();
        
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.01, ConfigSource::Default);  // Larger timestep
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);  // Small brain
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        return config;
    }
    
    // Builder-style configuration setup
    class ConfigBuilder {
    public:
        explicit ConfigBuilder() : config(std::make_shared<Config>()) {}
        
        ConfigBuilder& withSeed(int64_t seed) {
            config->set("random_seed", seed, ConfigSource::User);
            return *this;
        }
        
        ConfigBuilder& withNeuronCount(size_t count) {
            config->set("neuron_count", static_cast<int64_t>(count), ConfigSource::User);
            return *this;
        }
        
        ConfigBuilder& withRegions(size_t count) {
            config->set("region_count", static_cast<int64_t>(count), ConfigSource::User);
            return *this;
        }
        
        ConfigBuilder& withConnectionProbability(float prob) {
            config->set("connection_probability", prob, ConfigSource::User);
            return *this;
        }
        
        ConfigBuilder& withLearning(float ltp, float ltd, float tau) {
            config->set("stdp_ltp_weight", ltp, ConfigSource::User);
            config->set("stdp_ltd_weight", ltd, ConfigSource::User);
            config->set("stdp_tau", tau, ConfigSource::User);
            return *this;
        }
        
        ConfigBuilder& enableMemory(size_t workingMem, size_t episodicMem, size_t assocMem) {
            config->set("working_memory_capacity", static_cast<int64_t>(workingMem), ConfigSource::User);
            config->set("episodic_memory_max_episodes", static_cast<int64_t>(episodicMem), ConfigSource::User);
            config->set("associative_memory_max_associations", static_cast<int64_t>(assocMem), ConfigSource::User);
            return *this;
        }
        
        ConfigBuilder& enableNeuromodulation(float dopamine, float novelty, float curiosity) {
            config->set("dopamine_initial_level", dopamine, ConfigSource::User);
            config->set("novelty_sensitivity", novelty, ConfigSource::User);
            config->set("curiosity_rate", curiosity, ConfigSource::User);
            return *this;
        }
        
        std::shared_ptr<Config> build() {
            return config;
        }
        
    private:
        std::shared_ptr<Config> config;
    };
    
    // Create brain with integrated systems
    std::shared_ptr<Brain> createIntegratedBrain(const std::shared_ptr<Config>& config) {
        if (!config) {
            throw ConfigurationError("Configuration cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        
        return brain;
    }
    
    // Create complete agent system
    std::pair<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
    createAgentSystem() {
        auto config = createPhase6Config();
        auto brain = createIntegratedBrain(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        return {brain, world, agentBrain};
    }
    
    // Brain convenience methods
    struct BrainSimulationStep {
        SimulationStep step;
        Timestamp time;
        size_t spikesGenerated;
        size_t neuronsFiring;
        float avgFiringRate;
        
        std::string toString() const {
            return "Step: " + std::to_string(step) + ", Time: " + std::to_string(time) +
                   ", Spikes: " + std::to_string(spikesGenerated) + ", Firing: " + std::to_string(neuronsFiring) +
                   ", Avg Rate: " + std::to_string(avgFiringRate);
        }
    };
    
    // Create a simple sensory input from Python data
    std::shared_ptr<Vision> createVisionFromArray(const std::vector<std::vector<std::vector<float>>>& data) {
        if (data.empty() || data[0].empty() || data[0][0].empty()) {
            throw ConfigurationError("Vision data cannot be empty");
        }
        
        size_t height = data.size();
        size_t width = data[0].size();
        size_t channels = data[0][0].size();
        
        auto vision = std::make_shared<Vision>(width, height, channels);
        
        // Flatten and set data
        std::vector<float> flatData;
        flatData.reserve(height * width * channels);
        
        for (const auto& row : data) {
            for (const auto& pixel : row) {
                flatData.insert(flatData.end(), pixel.begin(), pixel.end());
            }
        }
        
        vision->setData(flatData);
        return vision;
    }
    
    // Create action from parameters
    std::shared_ptr<Action> createAction(ActionType type, const std::vector<float>& parameters = {}) {
        return std::make_shared<Action>(type, parameters);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides a complete interface for creating and simulating
        artificial developmental brains with integrated memory, prediction,
        cognition, and neuromodulation systems.
        
        Key Features:
        - Integrated artificial brain with multiple memory systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Predictive cognition and concept formation
        - Developmental plasticity
        - Event-driven spiking neural computation
        - Python-friendly API with builder patterns
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const BrainInitializationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register basic type IDs
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Register all other types (same as before but with more convenience methods)
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

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with factory methods and builder pattern
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            return *value;
        }, py::arg("key"), "Get a configuration value as string")
        .def("getInt", [](Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            return *value;
        }, py::arg("key"), "Get a configuration value as int")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            return *value;
        }, py::arg("key"), "Get a configuration value as float")
        .def("getDouble", [](Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            return *value;
        }, py::arg("key"), "Get a configuration value as double")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value as int")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value as float")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Factory methods for common configurations
        .def_static("createPhase6Config", &utils::createPhase6Config,
                   "Create a Phase 6 configuration with all integrated systems")
        .def_static("createDemoConfig", &utils::createDemoConfig,
                   "Create a demo configuration suitable for demonstrations")
        .def_static("createFastConfig", &utils::createFastConfig,
                   "Create a fast configuration for quick testing")
        // Builder pattern
        .def_static("builder", []() { return utils::ConfigBuilder(); },
                   "Create a configuration builder for fluent API");

    // Vision class with factory methods
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def_static("fromArray", &utils::createVisionFromArray,
                   py::arg("data"), "Create Vision from nested array of pixel values")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getData", &Vision::getData)
        .def("toArray", &Vision::getData, "Get vision data as array");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getData", &Audio::getData);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData)
        .def("size", &InternalSignals::getDimensions);

    // Action class with factory methods
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_static("create", &utils::createAction, py::arg("type"), py::arg("parameters") = std::vector<float>(),
                   "Create an action with type and optional parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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
        .def("distanceTo", [](const AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isAt", [](const AgentBody& body, float x, float y, float threshold) {
            return body.distanceTo(x, y) < threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold") = 0.5f, "Check if at position");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& result) { return result.success && result.reward > 0; },
             "Check if result was successful and positive")
        .def("__repr__", [](const ActionResult& result) {
            std::string successStr = result.success ? "SUCCESS" : "FAILED";
            return "<ActionResult: " + successStr + " (reward: " + std::to_string(result.reward) + ")>";
        });

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("hasVision", [](const SensoryPercept& percept) { return !percept.getVision().getData().empty(); })
        .def("hasAudio", [](const SensoryPercept& percept) { return !percept.getAudio().getData().empty(); })
        .def("hasTouch", [](const SensoryPercept& percept) { return !percept.getTouch().getData().empty(); })
        .def("hasInternal", [](const SensoryPercept& percept) { return !percept.getInternal().getData().empty(); });

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("toString", [](const SimpleWorld& world) {
            return "World(width=" + std::to_string(world.getWidth()) +
                    ", height=" + std::to_string(world.getHeight()) +
                    ", agent=(" + std::to_string(world.getAgentBody().x) + ", " +
                    std::to_string(world.getAgentBody().y) + "))";
        }, "Get string representation");

    // Brain class - the main class with comprehensive functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
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
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
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
             "Log brain status")
        // Factory methods for brain creation
        .def_static("createIntegratedBrain", &utils::createIntegratedBrain,
                   py::arg("config"), "Create a brain with integrated systems")
        // Convenience methods
        .def("isInitialized", [](const Brain& brain) {
            // Try to access config - if it fails, brain is not initialized
            try {
                brain.getConfig();
                return true;
            } catch (...) {
                return false;
            }
        }, "Check if brain is initialized")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
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

    // Complete system factory method
    m.def("createAgentSystem", &utils::createAgentSystem,
          "Create a complete agent system with brain, world, and agent interface");

    // Invalid IDs constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Convenience constants
    m.attr("NEURON_TYPE_EXCITATORY") = py::cast(NeuronType::Excitatory);
    m.attr("NEURON_TYPE_INHIBITORY") = py::cast(NeuronType::Inhibitory);
    m.attr("NEURON_TYPE_MODULAR") = py::cast(NeuronType::Modulatory);
    m.attr("NEURON_TYPE_SENSORY") = py::cast(NeuronType::Sensory);
    m.attr("NEURON_TYPE_MOTOR") = py::cast(NeuronType::Motor);
    m.attr("NEURON_TYPE_INTERNAL") = py::cast(NeuronType::Internal);

    m.attr("ACTION_TYPE_MOVE_FORWARD") = py::cast(ActionType::MoveForward);
    m.attr("ACTION_TYPE_WAIT") = py::cast(ActionType::Wait);
    m.attr("ACTION_TYPE_REST") = py::cast(ActionType::Rest);

    m.attr("DEVELOPMENTAL_STAGE_INITIAL") = py::cast(DevelopmentalStage::Initial);
    m.attr("DEVELOPMENTAL_STAGE_ADULT") = py::cast(DevelopmentalStage::Adult);

    // Module version
    m.attr("__version__", "0.1.0");

    // Add submodules for advanced usage
    py::module_ utils_module = m.def_submodule("utils", "Utility functions for NLM");
    
    // Register helper functions
    utils_module.def("create_default_config", &utils::createPhase6Config,
                     "Create default configuration for Phase 6");
    
    utils_module.def("create_fast_config", &utils::createFastConfig,
                     "Create fast configuration for testing");
    
    utils_module.def("create_demo_config", &utils::createDemoConfig,
                     "Create demo configuration for demonstrations");
    
    // Brain statistics helper
    utils_module.def("analyze_brain_state", [](const Brain& brain) {
        std::string result = "Brain State Analysis:\n";
        result += "  Total Neurons: " + std::to_string(brain.getTotalNeuronCount()) + "\n";
        result += "  Total Synapses: " + std::to_string(brain.getTotalSynapseCount()) + "\n";
        result += "  Active Neurons: " + std::to_string(brain.getActiveNeuronCount()) + "\n";
        result += "  Firing Neurons: " + std::to_string(brain.getFiringNeuronCount()) + "\n";
        result += "  Total Spikes: " + std::to_string(brain.getTotalSpikeCount()) + "\n";
        result += "  E/I Ratio: " + std::to_string(brain.getExcitationInhibitionRatio()) + "\n";
        result += "  Average Firing Rate: " + std::to_string(brain.getAverageFiringRate()) + "\n";
        result += "  Developmental Stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + "\n";
        return result;
    }, "Analyze and print brain state information");

    // Error handling
    py::class_<NLMException, std::runtime_error>(m, "NLMException", "Base NLM exception")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<ConfigurationError, NLMException>(m, "ConfigurationError", "Configuration error")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<BrainInitializationError, NLMException>(m, "BrainInitializationError", "Brain initialization error")
        .def(py::init<const std::string&>(), py::arg("message"));
}

} // namespace nlm
