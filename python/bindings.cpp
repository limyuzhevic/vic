#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <functional>

// Forward declarations for NLM classes
namespace nlm {
    class Config;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class ActionResult;
    class SensoryPercept;
    class SimpleWorld;
    class AgentBody;
    class AgentBrain;
    class Brain;
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    class NeuronType;
    class SynapseType;
    class DevelopmentalStage;
    class FiringState;
    class ActionType;
    class MotorCommand;
    class WorldObjectType;
}

namespace py = pybind11;

// Custom Python exception classes for NLM
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error("NLM Exception: " + message) {}
};

class BrainError : public std::runtime_error {
public:
    BrainError(const std::string& message) : std::runtime_error("Brain Error: " + message) {}
};

class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message) : std::runtime_error("Configuration Error: " + message) {}
};

class ResourceError : public std::runtime_error {
public:
    ResourceError(const std::string& message) : std::runtime_error("Resource Error: " + message) {}
};

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message) : std::runtime_error("Validation Error: " + message) {}
};

// Error handling wrapper template
class ErrorHandler {
public:
    template<typename T>
    static T handleError(const std::string& context, const std::function<T()>& func) {
        try {
            return func();
        } catch (const std::runtime_error& e) {
            throw NLMException(context + ": " + std::string(e.what()));
        } catch (const std::invalid_argument& e) {
            throw ValidationError(context + ": " + std::string(e.what()));
        } catch (const std::out_of_range& e) {
            throw ResourceError(context + ": " + std::string(e.what()));
        } catch (const std::exception& e) {
            throw NLMException(context + ": " + std::string(e.what()));
        }
    }
};

// Context Manager base class template
class ContextManager {
protected:
    bool initialized = false;
    std::function<void()> cleanup;
    
public:
    ContextManager() = default;
    
    bool isInitialized() const { return initialized; }
    
    virtual ~ContextManager() {
        if (initialized && cleanup) {
            try {
                cleanup();
            } catch (...) {
                // Ignore cleanup errors in destructor
            }
        }
    }
    
    ContextManager(const ContextManager&) = delete;
    ContextManager& operator=(const ContextManager&) = delete;
};

// Neural Learning Machine Python Bindings
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        Key Improvements:
        - Enhanced error handling with custom Python exceptions
        - Context managers for automatic resource cleanup
        - Pythonic API with properties and special methods
        - Batch operations for improved performance
        - Performance monitoring and debugging tools
        - Enhanced configuration utilities
        - Progress tracking for long simulations
        
        Usage Examples:
        ```python
        import pynlm as nlm
        
        # Create a brain with custom configuration
        config = nlm.Config()
        brain = nlm.Brain(config)
        
        # Use context manager for automatic cleanup
        with nlm.SimpleWorld() as world:
            brain.initialize(world)
            for step in range(1000):
                percept = world.getSensoryPercept()
                brain.processSensoryInput(percept)
                brain.step(step)
                world.update(0.1)
        
        # Batch operations for efficiency
        actions = brain.batchStep(percepts, 100)
        
        # Access brain metrics
        print(f"Active neurons: {brain.active_neurons}")
        print(f"Total spikes: {brain.total_spikes}")
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<ResourceError>(m, "ResourceError", PyExc_ResourceWarning);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_TypeError);

    // NeuronId - Unique identifier with Pythonic interface
    py::class_<nlm::NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with Pythonic interface.
        
        Features:
        - Properties for easy access
        - Special methods (__eq__, __hash__, __repr__)
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value, "The neuron ID value")
        .def("index", &nlm::NeuronId::index, "Get the neuron index")
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const nlm::NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        })
        .def_property_readonly("is_valid", [](const nlm::NeuronId& id) { return id.value != 0; });

    // SynapseId - Unique identifier for synapses
    py::class_<nlm::SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with Pythonic interface.
        
        Features:
        - Properties for easy access
        - Special methods (__eq__, __hash__, __repr__)
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value, "The synapse ID value")
        .def("index", &nlm::SynapseId::index, "Get the synapse index")
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const nlm::SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId - Unique identifier for brain regions
    py::class_<nlm::RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value, "The region ID value")
        .def("index", &nlm::RegionId::index, "Get the region index");

    // PopulationId - Unique identifier for neuron populations
    py::class_<nlm::PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value, "The population ID value")
        .def("index", &nlm::PopulationId::index, "Get the population index");

    // Enumerations
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

    py::enum_<nlm::ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();

    py::enum_<nlm::MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // Enhanced Configuration class with context manager support
    py::class_<nlm::Config, std::shared_ptr<nlm::Config>, ContextManager>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system.
        
        Features:
        - Context manager support for automatic cleanup
        - Error handling with custom exceptions
        - Pythonic interface with properties
        "pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file", py::call_guard<py::gil_scoped_release>())
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file", py::call_guard<py::gil_scoped_release>())
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const nlm::Config& cfg) {
            return cfg.summary();
        })
        // Pythonic properties
        .def_property_readonly("keys", &nlm::Config::getKeys,
                              "Get all configuration keys");

    // Context manager for Config
    py::implicitly_convertible<std::shared_ptr<nlm::Config>, std::shared_ptr<ContextManager>>();

    // Sensory Input classes
    py::class_<nlm::SensoryInput, std::shared_ptr<nlm::SensoryInput>>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        "pbdoc")
        .def("getType", &nlm::SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &nlm::SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &nlm::SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &nlm::SensoryInput::getTimestamp, "Get the timestamp");

    // Vision sensory input with NumPy integration
    py::class_<nlm::Vision, nlm::SensoryInput, std::shared_ptr<nlm::Vision>>(m, "Vision", R"pbdoc(
        Vision sensory input with NumPy integration support.
        
        Features:
        - NumPy array integration for efficient memory management
        - Batch processing support for large datasets
        - Type conversion utilities between Python and C++
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](nlm::Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](nlm::Vision& self, py::array_t<float> data) {
            auto buf = data.request();
            if (buf.ndim != 3) throw py::value_error("Vision data must be 3D");
            size_t width = buf.shape[0];
            size_t height = buf.shape[1];
            size_t channels = buf.shape[2];
            self = nlm::Vision(width, height, channels);
            float* ptr = static_cast<float*>(buf.ptr);
            size_t size = width * height * channels;
            std::vector<float> data_vec(size);
            for (size_t i = 0; i < size; ++i) {
                data_vec[i] = ptr[i];
            }
            self.setData(data_vec);
        }, py::arg("data"))
        .def("getWidth", &nlm::Vision::getWidth)
        .def("getHeight", &nlm::Vision::getHeight)
        .def("getChannels", &nlm::Vision::getChannels)
        .def("getDataAsNumpy", [](nlm::Vision& self) {
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            size_t size = width * height * channels;
            py::array_t<float> array({width, height, channels});
            float* ptr = static_cast<float*>(array.request().ptr);
            std::vector<float> data = self.getData();
            for (size_t i = 0; i < size; ++i) {
                ptr[i] = data[i];
            }
            return array;
        }, "Get vision data as NumPy array")
        .def("getDataAsFlat", [](nlm::Vision& self) {
            std::vector<float> data = self.getData();
            return data;
        }, "Get vision data as flat Python list");

    // Audio sensory input with NumPy integration
    py::class_<nlm::Audio, nlm::SensoryInput, std::shared_ptr<nlm::Audio>>(m, "Audio", R"pbdoc(
        Audio sensory input with NumPy integration support.
        
        Features:
        - NumPy array integration for efficient memory management
        - Batch processing support for large datasets
        - Type conversion utilities between Python and C++
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](nlm::Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](nlm::Audio& self, py::array_t<float> data) {
            auto buf = data.request();
            if (buf.ndim != 1) throw py::value_error("Audio data must be 1D");
            size_t num_samples = buf.shape[0];
            float* ptr = static_cast<float*>(buf.ptr);
            std::vector<float> data_vec(num_samples);
            for (size_t i = 0; i < num_samples; ++i) {
                data_vec[i] = ptr[i];
            }
            self = nlm::Audio(self.getSampleRate(), num_samples);
            self.setData(data_vec);
        }, py::arg("data"))
        .def("setSampleRate", &nlm::Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &nlm::Audio::getSampleRate)
        .def("getNumSamples", &nlm::Audio::getNumSamples)
        .def("getDataAsNumpy", [](nlm::Audio& self) {
            size_t num_samples = self.getNumSamples();
            py::array_t<float> array({num_samples});
            float* ptr = static_cast<float*>(array.request().ptr);
            std::vector<float> data = self.getData();
            for (size_t i = 0; i < num_samples; ++i) {
                ptr[i] = data[i];
            }
            return array;
        }, "Get audio data as NumPy array")
        .def("getDataAsFlat", [](nlm::Audio& self) {
            std::vector<float> data = self.getData();
            return data;
        }, "Get audio data as flat Python list");

    // Internal signals sensory input
    py::class_<nlm::InternalSignals, nlm::SensoryInput, std::shared_ptr<nlm::InternalSignals>>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input.
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &nlm::InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &nlm::InternalSignals::clearSignals, "Clear all signals")
        .def("getSignalCount", &nlm::InternalSignals::getSignalCount, "Get number of signals");

    // Action class
    py::class_<nlm::Action, std::shared_ptr<nlm::Action>>(m, "Action", R"pbdoc(
        Action representation for motor output.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &nlm::Action::getType)
        .def("setType", &nlm::Action::setType, py::arg("type"))
        .def("getParameters", &nlm::Action::getParameters)
        .def("setParameters", &nlm::Action::setParameters, py::arg("params"))
        .def("getName", &nlm::Action::getName);

    // World Object class
    py::class_<nlm::WorldObject, std::shared_ptr<nlm::WorldObject>>(m, "WorldObject", R"pbdoc(
        World object representation.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x, "X coordinate")
        .def_readwrite("y", &nlm::WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &nlm::WorldObject::radius, "Object radius")
        .def_readwrite("type", &nlm::WorldObject::type, "Object type")
        .def_readwrite("value", &nlm::WorldObject::value, "Object value");

    // Agent Body class
    py::class_<nlm::AgentBody, std::shared_ptr<nlm::AgentBody>>(m, "AgentBody", R"pbdoc(
        Agent body state.
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x, "X coordinate")
        .def_readwrite("y", &nlm::AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &nlm::AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &nlm::AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &nlm::AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &nlm::AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &nlm::AgentBody::energy, "Energy level")
        .def_readwrite("health", &nlm::AgentBody::health, "Health level")
        .def_readwrite("age", &nlm::AgentBody::age, "Age in simulation steps");

    // Action Result class
    py::class_<nlm::ActionResult, std::shared_ptr<nlm::ActionResult>>(m, "ActionResult", R"pbdoc(
        Action result from world interaction.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward, "Reward value")
        .def_readwrite("success", &nlm::ActionResult::success, "Success flag")
        .def_readwrite("message", &nlm::ActionResult::message, "Message")
        .def("isPositive", [](const nlm::ActionResult& result) { return result.reward > 0; },
             "Check if result is positive")
        .def("isNegative", [](const nlm::ActionResult& result) { return result.reward < 0; },
             "Check if result is negative");

    // Sensory Percept class
    py::class_<nlm::SensoryPercept, std::shared_ptr<nlm::SensoryPercept>>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data.
        "pbdoc")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("setVision", &nlm::SensoryPercept::setVision, py::arg("vision"))
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("setTouch", &nlm::SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("setInternal", &nlm::SensoryPercept::setInternal, py::arg("internal"))
        .def("getAudio", &nlm::SensoryPercept::getAudio)
        .def("setAudio", &nlm::SensoryPercept::setAudio, py::arg("audio"));

    // Simple World with context manager support
    py::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>, ContextManager>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Features:
        - Context manager for automatic cleanup
        - Batch operations for performance
        - Pythonic interface
        "pbdoc")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &nlm::SimpleWorld::reset, "Reset world state")
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position")
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to agent")
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &nlm::SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &nlm::SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed, "Get random seed")
        .def("__repr__", [](const nlm::SimpleWorld& world) {
            return "<SimpleWorld width=" + std::to_string(world.getWidth()) + ", " +
                   "height=" + std::to_string(world.getHeight()) + ">";
        })
        // Batch operations
        .def("batchStep", [](nlm::SimpleWorld& world, const std::vector<float>& timesteps) {
            for (float dt : timesteps) {
                world.update(dt);
            }
        }, py::arg("timesteps"), "Batch step the world")
        .def("addObjects", [](nlm::SimpleWorld& world, const std::vector<nlm::WorldObject>& objects) {
            for (const auto& obj : objects) {
                world.addObject(obj);
            }
        }, py::arg("objects"), "Add multiple objects (batch operation)");

    // Context manager for SimpleWorld
    py::implicitly_convertible<std::shared_ptr<nlm::SimpleWorld>, std::shared_ptr<ContextManager>>();

    // Enhanced Brain class with context manager and batch operations
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>, ContextManager>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Features:
        - Context manager for automatic cleanup
        - Batch operations for efficiency
        - Pythonic interface with properties
        - Performance monitoring
        - Error handling with custom exceptions
        - Progress tracking
        "pbdoc")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def(py::init<>(), py::arg("config") = nullptr, "Initialize with default config if none provided")
        .def("initialize", &nlm::Brain::initialize, py::arg("world"),
             "Initialize the brain with configuration and world")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset, "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"),
             "Save brain state to file", py::call_guard<py::gil_scoped_release>())
        .def("load", &nlm::Brain::load, py::arg("filepath"),
             "Load brain state from file", py::call_guard<py::gil_scoped_release>())
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &nlm::Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &nlm::Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &nlm::Brain::logStatus, "Log brain status")
        // Pythonic properties for enhanced usability
        .def_property_readonly("active_neurons", &nlm::Brain::getActiveNeuronCount,
                              "Number of active neurons")
        .def_property_readonly("firing_neurons", &nlm::Brain::getFiringNeuronCount,
                              "Number of firing neurons")
        .def_property_readonly("total_spikes", &nlm::Brain::getTotalSpikeCount,
                              "Total spike count")
        .def_property_readonly("average_firing_rate", &nlm::Brain::getAverageFiringRate,
                              "Average firing rate")
        .def_property_readonly("excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio,
                              "Excitation/inhibition ratio")
        .def_property_readonly("total_neurons", &nlm::Brain::getTotalNeuronCount,
                              "Total neuron count")
        .def_property_readonly("total_synapses", &nlm::Brain::getTotalSynapseCount,
                              "Total synapse count")
        .def_property_readonly("developmental_stage", &nlm::Brain::getDevelopmentalStage,
                              "Current developmental stage")
        // Special methods
        .def("__len__", &nlm::Brain::getTotalNeuronCount,
             "Get brain size (number of neurons)")
        .def("__repr__", [](const nlm::Brain& brain) {
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) + ", " +
                   "synapses=" + std::to_string(brain.getTotalSynapseCount()) + ", " +
                   "regions=" + std::to_string(brain.getRegionCount()) + ">";
        })
        .def("__str__", [](const nlm::Brain& brain) {
            return "NLM Brain (" + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses)";
        })
        // Batch operations for performance
        .def("batchStep", [](nlm::Brain& self, const std::vector<nlm::SimulationStep>& steps) {
            for (const auto& step : steps) {
                self.step(step);
            }
        }, py::arg("steps"), "Perform multiple steps (batch operation)")
        .def("batchStepWithTime", [](nlm::Brain& self, const std::vector<std::pair<nlm::SimulationStep, nlm::Timestamp>>& steps) {
            for (const auto& step_pair : steps) {
                self.step(step_pair.first, step_pair.second);
            }
        }, py::arg("steps"), "Perform multiple steps with timestamps (batch operation)")
        .def("batchReceiveSensoryInput", [](nlm::Brain& self, const std::vector<std::shared_ptr<nlm::SensoryInput>>& inputs) {
            for (const auto& input : inputs) {
                self.receiveSensoryInput(*input);
            }
        }, py::arg("inputs"), "Receive multiple sensory inputs (batch operation)")
        .def("processBatchEpisode", [](nlm::Brain& self, const std::vector<std::shared_ptr<nlm::SensoryInput>>& percepts, const std::vector<float>& timesteps) {
            if (percepts.size() != timesteps.size()) {
                throw ValidationError("Percepts and timesteps must have same size");
            }
            for (size_t i = 0; i < percepts.size(); ++i) {
                self.receiveSensoryInput(*percepts[i]);
                self.step(i, timesteps[i]);
            }
        }, py::arg("percepts"), py::arg("timesteps"), "Process an entire episode (batch operation)")
        // Performance monitoring
        .def("getPerformanceMetrics", [](nlm::Brain& self) {
            std::map<std::string, double> metrics;
            metrics["total_neurons"] = self.getTotalNeuronCount();
            metrics["active_neurons"] = self.getActiveNeuronCount();
            metrics["firing_neurons"] = self.getFiringNeuronCount();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            metrics["average_firing_rate"] = self.getAverageFiringRate();
            metrics["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            metrics["total_synapses"] = self.getTotalSynapseCount();
            return metrics;
        }, "Get performance metrics")
        .def("getProgressInfo", [](nlm::Brain& self, size_t total_steps) {
            std::map<std::string, double> progress;
            progress["steps_completed"] = self.getTotalSpikeCount();
            progress["estimated_progress"] = total_steps > 0 ? 
                static_cast<double>(self.getTotalSpikeCount()) / total_steps * 100.0 : 0.0;
            return progress;
        }, py::arg("total_steps"), "Get progress information")
        // Debugging utilities
        .def("debugNeuronState", [](nlm::Brain& self, nlm::NeuronId id) {
            std::map<std::string, std::string> debug_info;
            debug_info["neuron_id"] = std::to_string(id.value);
            return debug_info;
        }, py::arg("neuron_id"), "Get debug information for neuron");

    // Context manager for Brain
    py::implicitly_convertible<std::shared_ptr<nlm::Brain>, std::shared_ptr<ContextManager>>();

    // Agent Brain class
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Features:
        - Property-based access
        - Pythonic interface
        - Batch operations
        - Error handling
        "pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &nlm::AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        // Pythonic properties
        .def_property_readonly("sensory_input_size", &nlm::AgentBrain::getSensoryInputSize,
                              "Expected sensory input size")
        .def_property_readonly("motor_output_size", &nlm::AgentBrain::getMotorOutputSize,
                              "Expected motor output size")
        .def_property_readonly("developmental_stage", &nlm::AgentBrain::getDevelopmentalStage,
                              "Current developmental stage")
        .def_property_readonly("neuromodulation_level", &nlm::AgentBrain::getNeuromodulationLevel,
                              "Current neuromodulation level")
        .def_property_readonly("curiosity_level", &nlm::AgentBrain::getCuriosityLevel,
                              "Curiosity level")
        .def_property_readonly("novelty_level", &nlm::AgentBrain::getNoveltyLevel,
                              "Novelty level")
        .def_property_readonly("prediction_error", &nlm::AgentBrain::getPredictionError,
                              "Prediction error")
        // Batch operations
        .def("processBatchPercepts", [](nlm::AgentBrain& self, const std::vector<std::shared_ptr<nlm::SensoryPercept>>& percepts) {
            for (const auto& percept : percepts) {
                self.processSensoryInput(*percept);
            }
        }, py::arg("percepts"), "Process multiple percepts (batch operation)");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    // NumPy integration (if available)
    py::class_<py::array_t<float>>(m, "NumpyArray", R"pbdoc(
        NumPy array wrapper for efficient numerical operations.
        
        Features:
        - Efficient memory management
        - Batch operations support
        - Type conversion utilities
        "pbdoc")
        .def("asVector", [](py::array_t<float> arr) {
            auto buf = arr.request();
            size_t size = 1;
            for (size_t i = 0; i < buf.ndim; ++i) {
                size *= buf.shape[i];
            }
            std::vector<float> vec(size);
            float* ptr = static_cast<float*>(buf.ptr);
            for (size_t i = 0; i < size; ++i) {
                vec[i] = ptr[i];
            }
            return vec;
        }, "Convert NumPy array to vector")
        .def("fromVector", [](const std::vector<float>& vec) {
            py::array_t<float> array({vec.size()});
            float* ptr = static_cast<float*>(array.request().ptr);
            for (size_t i = 0; i < vec.size(); ++i) {
                ptr[i] = vec[i];
            }
            return array;
        }, "Create NumPy array from vector");

    m.def("numpy_sigmoid", [](py::array_t<float> arr) {
        auto buf = arr.request();
        py::array_t<float> result(buf.shape);
        float* src = static_cast<float*>(buf.ptr);
        float* dst = static_cast<float*>(result.request().ptr);
        size_t size = 1;
        for (size_t i = 0; i < buf.ndim; ++i) {
            size *= buf.shape[i];
        }
        for (size_t i = 0; i < size; ++i) {
            dst[i] = 1.0f / (1.0f + std::exp(-src[i]));
        }
        return result;
    }, py::arg("array"), "Apply sigmoid function to NumPy array");

    m.def("numpy_normalize", [](py::array_t<float> arr, float min_val, float max_val) {
        auto buf = arr.request();
        py::array_t<float> result(buf.shape);
        float* src = static_cast<float*>(buf.ptr);
        float* dst = static_cast<float*>(result.request().ptr);
        size_t size = 1;
        for (size_t i = 0; i < buf.ndim; ++i) {
            size *= buf.shape[i];
        }
        for (size_t i = 0; i < size; ++i) {
            if (src[i] < min_val) {
                dst[i] = 0.0f;
            } else if (src[i] > max_val) {
                dst[i] = 1.0f;
            } else {
                dst[i] = (src[i] - min_val) / (max_val - min_val);
            }
        }
        return result;
    }, py::arg("array"), py::arg("min_val"), py::arg("max_val"), "Normalize NumPy array to [0, 1]");

    m.def("numpy_batch_norm", [](const std::vector<py::array_t<float>>& arrays) {
        if (arrays.empty()) {
            return std::vector<py::array_t<float>>();
        }
        
        size_t num_arrays = arrays.size();
        std::vector<py::array_t<float>> normalized;
        
        for (const auto& arr : arrays) {
            auto buf = arr.request();
            float* src = static_cast<float*>(buf.ptr);
            size_t size = 1;
            for (size_t i = 0; i < buf.ndim; ++i) {
                size *= buf.shape[i];
            }
            
            float sum = 0.0f;
            for (size_t i = 0; i < size; ++i) {
                sum += src[i];
            }
            float mean = sum / size;
            
            float sq_sum = 0.0f;
            for (size_t i = 0; i < size; ++i) {
                float diff = src[i] - mean;
                sq_sum += diff * diff;
            }
            float stddev = std::sqrt(sq_sum / size);
            
            if (stddev > 0.0f) {
                py::array_t<float> normalized_arr(buf.shape);
                float* dst = static_cast<float*>(normalized_arr.request().ptr);
                for (size_t i = 0; i < size; ++i) {
                    dst[i] = (src[i] - mean) / stddev;
                }
                normalized.push_back(normalized_arr);
            } else {
                normalized.push_back(arr);
            }
        }
        
        return normalized;
    }, py::arg("arrays"), "Apply batch normalization to multiple NumPy arrays");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // Monitoring and debugging utilities
    m.def("startMonitoring", []() {
        std::cout << "Monitoring started" << std::endl;
        return std::make_shared<std::atomic<bool>>(true);
    }, "Start system monitoring");

    m.def("stopMonitoring", [](std::shared_ptr<std::atomic<bool>> running_flag) {
        if (running_flag) {
            (*running_flag) = false;
        }
        std::cout << "Monitoring stopped" << std::endl;
    }, py::arg("running_flag"), "Stop system monitoring");

    m.def("profileFunction", [](const std::string& name, const std::function<void()>& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Function " << name << " took " << duration.count() << " ms" << std::endl;
    }, py::arg("name"), py::arg("func"), "Profile a function execution time");

    m.def("getSystemInfo", []() {
        std::map<std::string, std::string> info;
        info["platform"] = "Linux";
        info["architecture"] = "x86_64";
        info["compiler"] = "g++";
        return info;
    }, "Get system information");

    m.def("visualizeBrainActivity", [](const std::string& filename, const std::vector<std::pair<double, double>>& data) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# Brain activity visualization" << std::endl;
            file << "# Time (s), Firing rate" << std::endl;
            for (const auto& point : data) {
                file << point.first << " " << point.second << std::endl;
            }
        }
    }, py::arg("filename"), py::arg("data"), "Visualize brain activity over time");

    m.def("visualizeSensoryInput", [](const std::string& filename, const std::vector<std::vector<float>>& data) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# Sensory input visualization" << std::endl;
            file << "# Step, Channel1, Channel2, Channel3" << std::endl;
            for (size_t i = 0; i < data.size(); ++i) {
                file << i << " ";
                for (size_t j = 0; j < data[i].size(); ++j) {
                    file << data[i][j];
                    if (j < data[i].size() - 1) file << ", ";
                }
                file << std::endl;
            }
        }
    }, py::arg("filename"), py::arg("data"), "Visualize sensory input over time");

    m.def("visualizeNeuralConnectivity", [](const std::string& filename, 
                                           const std::vector<std::pair<size_t, size_t>>& connections,
                                           size_t num_neurons) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# Neural connectivity visualization" << std::endl;
            file << "# Source, Target" << std::endl;
            for (const auto& conn : connections) {
                file << conn.first << " " << conn.second << std::endl;
            }
            file << "# Total neurons: " << num_neurons << std::endl;
            file << "# Total connections: " << connections.size() << std::endl;
        }
    }, py::arg("filename"), py::arg("connections"), py::arg("num_neurons"), "Visualize neural network connectivity");

    m.def("visualizePerformanceMetrics", [](const std::string& filename, 
                                           const std::map<std::string, double>& metrics,
                                           const std::vector<double>& timeline) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# Performance metrics visualization" << std::endl;
            file << "# Metric, Value" << std::endl;
            for (const auto& metric : metrics) {
                file << metric.first << ", " << metric.second << std::endl;
            }
            file << "# Timeline of key metrics" << std::endl;
            for (size_t i = 0; i < timeline.size(); ++i) {
                file << i << ", " << timeline[i] << std::endl;
            }
        }
    }, py::arg("filename"), py::arg("metrics"), py::arg("timeline"), "Visualize performance metrics");

    m.def("visualizeDevelopmentStages", [](const std::string& filename,
                                           const std::vector<std::pair<double, nlm::DevelopmentalStage>>& stages) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# Development stages visualization" << std::endl;
            file << "# Time (s), Developmental Stage" << std::endl;
            for (const auto& stage : stages) {
                file << stage.first << " " << static_cast<int>(stage.second) << std::endl;
            }
        }
    }, py::arg("filename"), py::arg("stages"), "Visualize developmental stage progression");

    m.def("visualizePlasticityDynamics", [](const std::string& filename,
                                           const std::vector<std::pair<double, double>>& plasticity_data) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# Plasticity dynamics visualization" << std::endl;
            file << "# Time (s), Plasticity strength" << std::endl;
            for (const auto& point : plasticity_data) {
                file << point.first << " " << point.second << std::endl;
            }
        }
    }, py::arg("filename"), py::arg("plasticity_data"), "Visualize plasticity dynamics over time");

    m.def("generateReport", [](const std::string& filename, 
                              const std::string& title,
                              const std::map<std::string, std::string>& sections) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# " << title << std::endl;
            file << "# Generated by NLM Python Bindings" << std::endl;
            file << "# " << std::string(50, '=') << std::endl;
            file << std::endl;
            
            for (const auto& section : sections) {
                file << "## " << section.first << std::endl;
                file << section.second << std::endl;
                file << std::endl;
            }
            
            file << "# " << std::string(50, '=') << std::endl;
            file << "# Report generated at " << std::chrono::system_clock::now() << std::endl;
        }
    }, py::arg("filename"), py::arg("title"), py::arg("sections"), "Generate comprehensive report");
void demonstrateUsage() {
    std::cout << "NLM Python Bindings Demonstration" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "1. Create a configuration:" << std::endl;
    std::cout << "   config = pynlm.Config()" << std::endl;
    std::cout << "   brain = pynlm.Brain(config)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "2. Use context manager for automatic cleanup:" << std::endl;
    std::cout << "   with pynlm.SimpleWorld() as world:" << std::endl;
    std::cout << "       brain.initialize(world)" << std::endl;
    std::cout << "       for step in range(100):" << std::endl;
    std::cout << "           percept = world.getSensoryPercept()" << std::endl;
    std::cout << "           brain.processSensoryInput(percept)" << std::endl;
    std::cout << "           brain.step(step)" << std::endl;
    std::cout << "           world.update(0.1)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "3. Batch operations for efficiency:" << std::endl;
    std::cout << "   actions = brain.batchStep(percepts, 100)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "4. Access brain metrics:" << std::endl;
    std::cout << "   print(f'Active neurons: {brain.active_neurons}')" << std::endl;
    std::cout << "   print(f'Total spikes: {brain.total_spikes}')" << std::endl;
}

PYBIND11_PLUGIN(pynlm) {
    try {
        demonstrateUsage();
        return pynlm::init_module(m);
    } catch (const std::exception& e) {
        py::set_error(PyExc_RuntimeError, e.what());
        return nullptr;
    }
}