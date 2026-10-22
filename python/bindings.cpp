#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Core headers
#include "../src/brain/Brain.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Memory systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"

// Neuromodulators
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

// Prediction system
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

// Cognition systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

// Note: AttentionalSelection is defined in NeuralWorkingMemory.hpp, not as separate file

// Development system
#include "../src/development/DevelopmentSystem.hpp"

// Performance systems
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/EventQueue.hpp"

// Environment
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

// Pythonic error types
class PyConfigError : public std::runtime_error {
public:
    explicit PyConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyBrainError : public std::runtime_error {
public:
    explicit PyBrainError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper function for parameter validation
void validateNeuronId(const NeuronId& id, const std::string& paramName = "neuron") {
    if (id.value == static_cast<uint64_t>(-1)) {
        throw PyBrainError(paramName + " is an invalid neuron ID");
    }
}

void validateRegionId(const RegionId& id, const std::string& paramName = "region") {
    if (id.value == static_cast<uint64_t>(-1)) {
        throw PyConfigError(paramName + " is an invalid region ID");
    }
}

void validatePositive(const float& value, const std::string& paramName, float min = 0.0f) {
    if (value < min) {
        throw PyConfigError(paramName + " must be >= " + std::to_string(min) +
                           ", got " + std::to_string(value));
    }
}

void validateNonNegative(const float& value, const std::string& paramName, float min = 0.0f) {
    if (value < min) {
        throw PyConfigError(paramName + " must be >= " + std::to_string(min) +
                           ", got " + std::to_string(value));
    }
}

void validateRange(const float& value, const std::string& paramName,
                  float min, float max, bool inclusive = true) {
    float actualMin = inclusive ? min : min + 0.0001f;
    float actualMax = inclusive ? max : max - 0.0001f;
    if (value < actualMin || value > actualMax) {
        throw PyConfigError(paramName + " must be in range [" + std::to_string(actualMin) +
                           ", " + std::to_string(actualMax) + "], got " + std::to_string(value));
    }
}

// Python wrapper class for Brain with context manager support
class PyBrain {
public:
    PyBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    PyBrain(const PyBrain&) = delete;
    PyBrain& operator=(const PyBrain&) = delete;
    PyBrain(PyBrain&&) = default;
    PyBrain& operator=(PyBrain&&) = default;
    ~PyBrain() = default;

    // Context manager support
    void __enter__() { initialize(); }
    void __exit__(int /*exc_type*/, const std::string& /*exc_value*/, int /*exc_traceback*/) {
        reset();
    }

    // Core brain operations
    bool initialize() {
        if (!brain_) {
            throw PyBrainError("Brain is not properly initialized");
        }
        return brain_->initialize();
    }

    void step(SimulationStep step) {
        brain_->step(step);
    }

    void step(SimulationStep step, Timestamp time) {
        brain_->step(step, time);
    }

    void receiveSensoryInput(const SensoryInput& input) {
        brain_->receiveSensoryInput(input);
    }

    void injectCurrent(NeuronId neuron, MembranePotential current) {
        validateNeuronId(neuron);
        brain_->injectCurrent(neuron, current);
    }

    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        validateNonNegative(current, "current");
        brain_->injectCurrentToNeurons(type, current);
    }

    std::unique_ptr<Action> produceAction() {
        auto action = brain_->produceAction();
        if (!action) {
            throw PyBrainError("Brain failed to produce an action");
        }
        return action;
    }

    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }

    bool save(const std::string& filepath) const {
        validateNonNegative(static_cast<float>(filepath.length()), "filepath length");
        return brain_->save(filepath);
    }

    bool load(const std::string& filepath) {
        if (filepath.empty()) {
            throw PyConfigError("Filepath cannot be empty");
        }
        return brain_->load(filepath);
    }

    // Statistics and monitoring
    float getExcitationInhibitionRatio() const {
        return brain_->getExcitationInhibitionRatio();
    }

    size_t getTotalSpikeCount() const {
        return brain_->getTotalSpikeCount();
    }

    size_t getPendingSpikeEventCount() const {
        return brain_->getPendingSpikeEventCount();
    }

    size_t getTotalNeuronCount() const {
        return brain_->getTotalNeuronCount();
    }

    size_t getTotalSynapseCount() const {
        return brain_->getTotalSynapseCount();
    }

    size_t getActiveNeuronCount() const {
        return brain_->getActiveNeuronCount();
    }

    size_t getFiringNeuronCount() const {
        return brain_->getFiringNeuronCount();
    }

    float getAverageFiringRate() const {
        return brain_->getAverageFiringRate();
    }

    // Region management
    RegionId addRegion(const std::string& name = "") {
        return brain_->addRegion(name);
    }

    NeuralRegion* getRegion(RegionId id) {
        validateRegionId(id);
        auto* region = brain_->getRegion(id);
        if (!region) {
            throw PyBrainError("Region not found");
        }
        return region;
    }

    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() {
        return brain_->getWorkingMemory();
    }

    NeuralEpisodicMemory* getEpisodicMemory() {
        return brain_->getEpisodicMemory();
    }

    NeuralAssociativeMemory* getAssociativeMemory() {
        return brain_->getAssociativeMemory();
    }

    // Prediction system access
    PredictionSystem* getPredictionSystem() {
        return brain_->getPredictionSystem();
    }

    // Cognition system access
    NeuralPlanner* getPlanner() {
        return brain_->getPlanner();
    }

    ConceptFormation* getConceptFormation() {
        return brain_->getConceptFormation();
    }

    AttentionalSelection* getAttention() {
        return brain_->getAttention();
    }

    // Development system access
    DevelopmentSystem* getDevelopmentSystem() {
        return brain_->getDevelopmentSystem();
    }

    // Neuromodulation system access
    Dopamine* getDopamine() {
        return brain_->getDopamine();
    }

    Curiosity* getCuriosity() {
        return brain_->getCuriosity();
    }

    Novelty* getNovelty() {
        return brain_->getNovelty();
    }

    PredictionError* getPredictionErrorSignal() {
        return brain_->getPredictionErrorSignal();
    }

    // Configuration access
    std::shared_ptr<const Config> getConfig() const {
        return brain_->getConfig();
    }

    // Raw brain access (for advanced users)
    std::shared_ptr<Brain> getBrain() {
        return brain_;
    }

    const std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }

    // Simulation control
    void advanceSimulation(SimulationStep steps, Timestamp timestepDuration = 1.0f) {
        for (SimulationStep i = 0; i < steps; ++i) {
            brain_->step(i, timestepDuration * (i + 1));
        }
    }

    // Convenience method for running simulation loop
    void runSimulation(SimulationStep totalSteps, 
                      const std::function<void(SimulationStep, float)>& callback = nullptr) {
        for (SimulationStep step = 0; step < totalSteps; ++step) {
            brain_->step(step, step * 1.0f);
            if (callback) {
                callback(step, step * 1.0f);
            }
        }
    }

private:
    std::shared_ptr<Brain> brain_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced binding provides access to:
        - Core neural computation (Brain, AgentBrain, SimpleWorld)
        - Memory systems (NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory)
        - Neuromodulation (Dopamine, Curiosity, Novelty, PredictionError)
        - Prediction systems (PredictionSystem, NeuralPrediction)
        - Cognition systems (NeuralPlanner, ConceptFormation, AttentionalSelection)
        - Development system (DevelopmentSystem)
        - Performance systems (MemoryPool, EventQueue)
        - Environment (Environment, Observation)
        
        Features:
        - Pythonic API with context managers and property-based access
        - Comprehensive parameter validation with clear error messages
        - Type safety for critical parameters
        - Example usage and detailed documentation
        - Integration utilities for common workflows
        
        Usage example:
        ```python
        import pynlm
        
        # Create brain with default configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Use as context manager for automatic cleanup
        with pynlm.PyBrain(brain) as b:
            b.initialize()
            
            # Run simulation loop
            for step in range(100):
                b.step(step)
                action = b.produceAction()
                if callback:
                    callback(step, action)
        
        # Save brain state
        brain.save("brain_checkpoint.pkl")
        ```
    )pbdoc";

    // Register custom exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<PyConfigError>(m, "ConfigError");
    py::register_exception<PyBrainError>(m, "BrainError");

    // NeuronId - Unique identifier for neurons
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
        .def("is_valid", [](const NeuronId& id) {
            return id.value != static_cast<uint64_t>(-1);
        }, "Check if neuron ID is valid");

    // SynapseId - Unique identifier for synapses
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

    // RegionId - Unique identifier for brain regions
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

    // PopulationId - Unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
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

    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
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

    // Configuration
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Helper methods for common types
        .def("getInt", [](const Config& self, const std::string& key) {
            auto result = self.get<int>(key);
            return result ? *result : 0;
        }, py::arg("key"), "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto result = self.get<double>(key);
            return result ? *result : 0.0;
        }, py::arg("key"), "Get float configuration value")
        .def("getString", [](const Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            return result ? *result : "";
        }, py::arg("key"), "Get string configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value");

    // Sensory Input classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // World Object
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
        .def_readwrite("active", &WorldObject::active);

    // Agent Body
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
        .def("reset", &AgentBody::reset);

    // Action Result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory Percept
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Simple World
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // PyBrain - Enhanced Python wrapper with context manager support
    py::class_<PyBrain>(m, "PyBrain", R"pbdoc(Enhanced Python wrapper for Brain with context manager support)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("__enter__", &PyBrain::__enter__)
        .def("__exit__", [](PyBrain& self, int exc_type, const std::string& exc_value, int exc_traceback) {
            self.__exit__(exc_type, exc_value, exc_traceback);
        })
        .def("initialize", &PyBrain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (PyBrain::*)(SimulationStep)>(&PyBrain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (PyBrain::*)(SimulationStep, Timestamp)>(&PyBrain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &PyBrain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &PyBrain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &PyBrain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &PyBrain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &PyBrain::reset,
             "Reset brain state")
        .def("save", &PyBrain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &PyBrain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &PyBrain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &PyBrain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &PyBrain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &PyBrain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &PyBrain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &PyBrain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &PyBrain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &PyBrain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &PyBrain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &PyBrain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &PyBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &PyBrain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &PyBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &PyBrain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &PyBrain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &PyBrain::logStatus,
             "Log brain status")
        .def("getWorkingMemory", &PyBrain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &PyBrain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &PyBrain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &PyBrain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &PyBrain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &PyBrain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &PyBrain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &PyBrain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &PyBrain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &PyBrain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &PyBrain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &PyBrain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getBrain", &PyBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("advanceSimulation", &PyBrain::advanceSimulation,
             py::arg("steps"), py::arg("timestepDuration") = 1.0f,
             "Advance simulation by specified number of steps")
        .def("runSimulation", &PyBrain::runSimulation,
             py::arg("totalSteps"), py::arg("callback") = nullptr,
             "Run simulation loop with optional callback");

    // Original Brain class for compatibility
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
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
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal");

    // AgentBrain
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

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<PyBrain> {
        return std::make_shared<PyBrain>(config);
    }, py::arg("config"), "Create a new enhanced brain with configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions for common operations
    m.def("simulateEpisode", [](std::shared_ptr<PyBrain> brain, 
                              const std::vector<SensoryInput>& observations,
                              size_t maxSteps = 1000) {
        if (!brain->initialize()) {
            throw PyBrainError("Failed to initialize brain for episode simulation");
        }
        
        for (size_t step = 0; step < maxSteps; ++step) {
            if (step < observations.size()) {
                brain->receiveSensoryInput(observations[step]);
            }
            brain->step(step);
        }
        
        return brain->produceAction();
    }, py::arg("brain"), py::arg("observations"), py::arg("maxSteps") = 1000,
       "Simulate an episode with a series of sensory observations");

    m.def("runTrial", [](std::shared_ptr<PyBrain> brain,
                       const std::vector<SensoryInput>& observations,
                       const std::function<std::unique_ptr<Action>(SimulationStep)>& actionCallback = nullptr) {
        if (!brain->initialize()) {
            throw PyBrainError("Failed to initialize brain for trial");
        }
        
        std::unique_ptr<Action> finalAction;
        brain->runSimulation(observations.size(), 
                            [callback = actionCallback, &finalAction](SimulationStep step, float time) {
                                if (callback) {
                                    finalAction = callback(step);
                                }
                            });
        
        return finalAction;
    }, py::arg("brain"), py::arg("observations"), py::arg("actionCallback") = nullptr,
       "Run a trial with sensory observations and optional action callback");

    m.def("saveBrainWithMetadata", [](const std::string& filepath,
                                    std::shared_ptr<Brain> brain,
                                    const std::string& metadata = "") {
        if (filepath.empty()) {
            throw PyConfigError("Filepath cannot be empty");
        }
        if (!brain->save(filepath)) {
            throw PyBrainError("Failed to save brain state to " + filepath);
        }
        return true;
    }, py::arg("filepath"), py::arg("brain"), py::arg("metadata") = "",
       "Save brain state with optional metadata");

    m.def("loadBrainWithValidation", [](const std::string& filepath,
                                       std::shared_ptr<Brain> brain) {
        if (filepath.empty()) {
            throw PyConfigError("Filepath cannot be empty");
        }
        if (!brain->load(filepath)) {
            throw PyBrainError("Failed to load brain state from " + filepath);
        }
        return true;
    }, py::arg("filepath"), py::arg("brain"),
       "Load brain state with validation");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic helper functions
    m.def("validate_and_sanitize", [](const std::vector<float>& data, 
                                    size_t expected_size = 0,
                                    float min_value = -1000.0f,
                                    float max_value = 1000.0f) {
        std::vector<float> result = data;
        
        if (expected_size > 0 && result.size() != expected_size) {
            result.resize(expected_size);
        }
        
        for (auto& value : result) {
            if (value < min_value) value = min_value;
            if (value > max_value) value = max_value;
        }
        
        return result;
    }, py::arg("data"), py::arg("expected_size") = 0, 
       py::arg("min_value") = -1000.0f, py::arg("max_value") = 1000.0f,
       "Validate and sanitize a data vector");

    m.def("safe_softmax", [](const std::vector<float>& logits) {
        float max_logit = *std::max_element(logits.begin(), logits.end());
        float sum = 0.0f;
        std::vector<float> exp_logits;
        exp_logits.reserve(logits.size());
        
        for (float logit : logits) {
            exp_logits.push_back(std::exp(logit - max_logit));
            sum += exp_logits.back();
        }
        
        std::vector<float> probs;
        probs.reserve(exp_logits.size());
        
        for (float exp_logit : exp_logits) {
            probs.push_back(exp_logit / sum);
        }
        
        return probs;
    }, py::arg("logits"), "Compute numerically stable softmax probabilities");

    // Registration helper for memory systems
    m.def("registerMemorySystems", [](std::shared_ptr<Brain> brain) {
        auto working_memory = brain->getWorkingMemory();
        auto episodic_memory = brain->getEpisodicMemory();
        auto associative_memory = brain->getAssociativeMemory();
        
        // Enable replay for episodic memory if available
        if (episodic_memory) {
            episodic_memory->enableReplay(true);
        }
        
        return std::make_tuple(working_memory, episodic_memory, associative_memory);
    }, py::arg("brain"), "Register and configure memory systems");

    // Simulation monitoring utilities
    m.def("monitorSimulation", [](std::shared_ptr<PyBrain> brain,
                                 size_t report_interval = 100) {
        struct SimulationStats {
            SimulationStep steps_completed = 0;
            SimulationStep total_steps = 0;
            float avg_firing_rate = 0.0f;
            size_t total_spikes = 0;
            float last_reward = 0.0f;
            
            std::string to_string() const {
                return "Steps: " + std::to_string(steps_completed) +
                       ", Avg Firing Rate: " + std::to_string(avg_firing_rate) +
                       ", Total Spikes: " + std::to_string(total_spikes);
            }
        };
        
        SimulationStats stats;
        
        return [brain, report_interval, &stats](SimulationStep step) {
            stats.steps_completed = step + 1;
            stats.avg_firing_rate = brain->getAverageFiringRate();
            stats.total_spikes = brain->getTotalSpikeCount();
            
            if (step % report_interval == 0) {
                std::cout << "Step " << step << ": " << stats.to_string() << std::endl;
            }
            
            return stats;
        };
    }, py::arg("brain"), py::arg("report_interval") = 100,
       "Create a simulation monitor that reports statistics");

    // Error handling utilities
    m.def("safe_execute", [](const std::function<void()>& func,
                           const std::string& error_context = "Operation") {
        try {
            func();
            return true;
        } catch (const std::exception& e) {
            std::cerr << error_context << " failed: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("func"), py::arg("error_context") = "Operation",
       "Safely execute a function with error handling");
}

} // namespace nlm