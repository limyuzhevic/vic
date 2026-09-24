#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Include all core headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

// Include brain components
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Synapse.hpp"

// Include agent components
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Include world components
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/Observation.hpp"

// Include sensory components
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Include motor components
#include "../src/motor/Action.hpp"
#include "../src/motor/MotorSystem.hpp"

// Include neuromodulation components
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

// Include plasticity components
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/plasticity/PlasticityRule.hpp"

// Include memory components
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"

// Include prediction components
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

// Include development components
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"

// Include cognition components
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"

// Include environment components
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for serialization
inline std::string serializeNeurons(const std::vector<Neuron*>& neurons) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (i > 0) ss << ",";
        const auto* neuron = neurons[i];
        if (neuron) {
            ss << "{\"id\":" << neuron->getId().value 
               << ",\"type\":" << static_cast<int>(neuron->getType())
               << ",\"v\":" << neuron->getMembranePotential()
               << ",\"firing\":" << neuron->isFiring() << "}";
        }
    }
    ss << "]";
    return ss.str();
}

inline std::string serializeSynapses(const std::vector<Synapse*>& synapses) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (i > 0) ss << ",";
        const auto* synapse = synapses[i];
        if (synapse) {
            ss << "{\"id\":" << synapse->getId().value
               << "\"source\":" << synapse->getSourceNeuron().value
               << "\"dest\":" << synapse->getDestinationNeuron().value
               << "\"weight\":" << synapse->getWeight() << "}";
        }
    }
    ss << "]";
    return ss.str();
}

inline std::string serializeRegions(const std::vector<NeuralRegion*>& regions) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < regions.size(); ++i) {
        if (i > 0) ss << ",";
        const auto* region = regions[i];
        if (region) {
            ss << "{\"id\":" << region->getId().value
               << "\"name\":" << region->getName() << "\"populations\":" << region->getPopulationCount()
               << "\"neurons\":" << region->getTotalNeuronCount() << "\"synapses\":" << region->getSynapseCount() << "}";
        }
    }
    ss << "]";
    return ss.str();
}

inline std::string serializePopulations(const std::vector<NeuralPopulation*>& populations) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < populations.size(); ++i) {
        if (i > 0) ss << ",";
        const auto* pop = populations[i];
        if (pop) {
            ss << "{\"id\":" << pop->getId().value
               << "\"size\":" << pop->getSize()
               << "\"type\":" << static_cast<int>(pop->getType())
               << "\"neurons\":" << pop->getNeuronCount() << "}";
        }
    }
    ss << "]";
    return ss.str();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python interface to the NLM C++ neural simulation framework.
        
        This module provides access to:
        - Core types (NeuronId, SynapseId, etc.)
        - Brain simulation with neurons, synapses, and regions
        - Agent interfaces for world interaction
        - Sensory processing (Vision, Audio, InternalSignals)
        - Motor control (Actions, MotorCommands)
        - Neuromodulation (Dopamine, Curiosity, Novelty)
        - Memory systems (WorkingMemory, EpisodicMemory)
        - Prediction and cognition (Planner, ConceptFormation)
        - Development and plasticity
        - World simulation (SimpleWorld)
        - Configuration management
        
        Example usage:
        
        ```python
        import pynlm
        
        # Create a brain with default configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create a simple world
        world = pynlm.createSimpleWorld()
        
        # Create an agent brain
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run a simulation step
        step = 0
        while step < 1000:
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Produce action
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, step)
            
            step += 1
        ```
    )pbdoc";

    // Register all exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Core type definitions
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // Enums
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

    // Core type aliases
    m.attr("SimulationStep") = py::int_(0);  // Type alias placeholder
    m.attr("Timestamp") = py::float_(0.0);   // Type alias placeholder
    m.attr("TimestepDuration") = py::float_(0.0);  // Type alias placeholder
    m.attr("SynapticWeight") = py::float_(0.0f);   // Type alias placeholder
    m.attr("MembranePotential") = py::float_(0.0f); // Type alias placeholder
    m.attr("FiringRate") = py::float_(0.0f);        // Type alias placeholder

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Config class
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
        .def("get", &Config::has, py::arg("key"),
             "Get a configuration value (returns None if not exists)")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", [](const SensoryInput& self) {
            return self.clone();
        }, "Create a copy of this sensory input");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input representing visual input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Vision::getData, "Get the raw pixel data as a vector")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("setWidth", &Vision::setWidth, py::arg("width"), "Set image width")
        .def("setHeight", &Vision::setHeight, py::arg("height"), "Set image height")
        .def("setChannels", &Vision::setChannels, py::arg("channels"), "Set number of channels")
        .def("to_array", [](const Vision& self) {
            return self.getData();
        }, "Convert vision data to numpy array");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input representing sound data)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Audio::getData, "Get the raw audio data as a vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("setNumSamples", &Audio::setNumSamples, py::arg("numSamples"), "Set number of samples")
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input for neural state)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"), "Get signal at index")
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get number of signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignals", [](const InternalSignals& self) {
            return self.getSignals();
        }, "Get all signals as a vector");

    // Action class
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
        .def("to_dict", [](const Action& self) {
            return std::make_pair(self.getType(), self.getParameters());
        }, "Convert action to dictionary (type, parameters)")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " with " + std::to_string(action.getParameters().size()) + " params>";
        });

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("is_within", [](const WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= radius * radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if point is within object");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state and kinematics)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Health (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is healthy")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f && self.energy > 0.0f;
        }, "Check if agent is alive");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world interaction)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value (-1 to 1)")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("is_positive_reward", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("is_negative_reward", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if reward is negative")
        .def("to_dict", [](const ActionResult& self) {
            return std::make_tuple(self.reward, self.success, self.message);
        }, "Convert to tuple (reward, success, message)");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data combining all input modalities)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as dict")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("has_vision", [](const SensoryPercept& self) {
            return self.getVision() != nullptr;
        }, "Check if vision data is available")
        .def("has_audio", [](const SensoryPercept& self) {
            return self.getAudio() != nullptr;
        }, "Check if audio data is available")
        .def("has_internal", [](const SensoryPercept& self) {
            return self.getInternal() != nullptr;
        }, "Check if internal signals are available")
        .def("has_proprioception", [](const SensoryPercept& self) {
            return self.getProprioception() != nullptr;
        }, "Check if proprioception data is available");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for one timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not in wall)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getWorldObjects", [](SimpleWorld& self) {
            return self.getWorldObjects();
        }, "Get all world objects")
        .def("find_nearby_objects", [](SimpleWorld& self, float x, float y, float radius) {
            auto objects = self.getWorldObjects();
            std::vector<WorldObject*> nearby;
            for (auto* obj : objects) {
                if (obj && obj->active) {
                    float dx = obj->x - x;
                    float dy = obj->y - y;
                    if (dx * dx + dy * dy <= radius * radius) {
                        nearby.push_back(obj);
                    }
                }
            }
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Find nearby objects")
        .def("get_agent_position", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_pair(body->x, body->y);
        }, "Get agent position as (x, y) tuple")
        .def("get_agent_velocity", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_pair(body->velocityX, body->velocityY);
        }, "Get agent velocity as (vx, vy) tuple");

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
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
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal, "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal, "Get STDP plasticity")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal, "Get Hebbian plasticity")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal, "Get structural plasticity")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal, "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal, "Get associative memory")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal, "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal, "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal, "Get concept formation")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal, "Get attentional selection")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal, "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal, "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal, "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal, "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal, "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal, "Get random generator")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signals")
        .def("getNeuronStatistics", [](Brain& self) {
            std::stringstream ss;
            ss << "Neurons: " << self.getTotalNeuronCount()
               << ", Active: " << self.getActiveNeuronCount()
               << ", Firing: " << self.getFiringNeuronCount()
               << ", Average rate: " << self.getAverageFiringRate();
            return ss.str();
        }, "Get formatted neuron statistics")
        .def("getSynapseStatistics", [](Brain& self) {
            std::stringstream ss;
            ss << "Synapses: " << self.getTotalSynapseCount()
               << ", E/I ratio: " << self.getExcitationInhibitionRatio();
            return ss.str();
        }, "Get formatted synapse statistics");

    // NeuralRegion class
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region containing multiple populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"),
             "Create region with ID")
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"),
             "Create region with ID and name")
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal,
             "Add a neuron population")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"),
             py::return_value_policy::reference_internal, "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get population count")
        .def("getPopulations", &NeuralRegion::getPopulations,
             py::return_value_policy::reference_internal, "Get all populations")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations,
             "Get all populations as vector")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add a synapse within this region")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"),
             py::return_value_policy::reference_internal, "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get synapse count")
        .def("getSynapses", &NeuralRegion::getSynapses,
             py::return_value_policy::reference_internal, "Get all synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"),
             "Get all synapses originating from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"),
             "Get all synapses targeting neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step all populations and synapses")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons in region")
        .def("getAllNeuronsConst", &NeuralRegion::getAllNeurons,
             "Get all neurons in region (const version)")
        .def("getNeuronCount", [](const NeuralRegion& self) {
            return self.getTotalNeuronCount();
        }, "Get neuron count")
        .def("getTotalConnections", [](const NeuralRegion& self) {
            return self.getSynapseCount();
        }, "Get total number of connections");

    // NeuralPopulation class
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Population of homogeneous neurons)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"),
             "Create population with ID and size")
        .def(py::init<PopulationId, size_t, NeuronType>(), py::arg("id"), py::arg("size"), py::arg("type"),
             "Create population with ID, size, and type")
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("getSize", &NeuralPopulation::getSize, "Get population size")
        .def("getType", &NeuralPopulation::getType, "Get neuron type")
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount, "Get neuron count")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"),
             py::return_value_policy::reference_internal, "Get neuron by index")
        .def("getNeurons", &NeuralPopulation::getNeurons,
             py::return_value_policy::reference_internal, "Get all neurons")
        .def("getNeuronIds", [](const NeuralPopulation& self) {
            std::vector<NeuronId> ids;
            ids.reserve(self.getNeuronCount());
            for (size_t i = 0; i < self.getNeuronCount(); ++i) {
                ids.push_back(self.getNeuron(i)->getId());
            }
            return ids;
        }, "Get all neuron IDs")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuron"), "Remove neuron from population")
        .def("clear", &NeuralPopulation::clear, "Clear all neurons")
        .def("getAverageMembranePotential", [](const NeuralPopulation& self) {
            float sum = 0.0f;
            size_t count = 0;
            for (size_t i = 0; i < self.getNeuronCount(); ++i) {
                sum += self.getNeuron(i)->getMembranePotential();
                count++;
            }
            return count > 0 ? sum / count : 0.0f;
        }, "Get average membrane potential")
        .def("getFiringRate", [](const NeuralPopulation& self) {
            float sum = 0.0f;
            size_t count = 0;
            for (size_t i = 0; i < self.getNeuronCount(); ++i) {
                sum += self.getNeuron(i)->getFiringRate();
                count++;
            }
            return count > 0 ? sum / count : 0.0f;
        }, "Get average firing rate")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Step all neurons in population")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("getNeuronTypes", [](const NeuralPopulation& self) {
            if (self.getNeuronCount() > 0) {
                return self.getNeuron(0)->getType();
            }
            return NeuronType::Internal;
        }, "Get neuron type (assumes homogeneous population)");

    // Neuron class
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron implementing LIF dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"), "Create neuron with ID")
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getState", &Neuron::getState, py::return_value_policy::reference_internal, "Get neuron state")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is refractory")
        .def("getFiringState", &Neuron::getFiringState, "Get firing state")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), "Set refractory period")
        .def("decrementRefractory", &Neuron::decrementRefractory, "Decrement refractory counter")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate")
        .def("getMembranePotentialResting", &Neuron::getRestingPotential, "Get resting potential")
        .def("setMembranePotentialResting", &Neuron::setRestingPotential, py::arg("potential"), "Set resting potential")\n        .def("getMembranePotentialReset", &Neuron::getResetPotential, "Get reset potential")
        .def("setMembranePotentialReset", &Neuron::setResetPotential, py::arg("potential"), "Set reset potential")
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period")
        .def("checkThreshold", &Neuron::checkThreshold, "Check if neuron has reached threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get last spike time")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "Perform LIF integration")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject external current")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear injected current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record a spike event")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, "Get incoming synapse handles")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, "Get outgoing synapse handles")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, py::return_value_policy::reference_internal, "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable plasticity rules")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"), "Update neuron for one simulation step")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("getAge", &Neuron::getAge, "Get neuron age (placeholder)")
        .def("to_dict", [](const Neuron& self) {
            std::stringstream ss;
            ss << "{\"id\":" << self.getId().value
               << "\"type\":" << static_cast<int>(self.getType())
               << "\"v\":" << self.getMembranePotential()
               << "\"firing\":" << self.isFiring() << "}";
            return ss.str();
        }, "Convert neuron to dictionary");

    // Synapse class
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons with plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"),
             "Create synapse with IDs")
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record pre-synaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record post-synaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get pre-synaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get post-synaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, py::return_value_policy::reference_internal, "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable plasticity rules")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace for reward modulation")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Update synapse for one simulation step")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("getDelaySteps", &Synapse::getDelay, "Get delay in steps")
        .def("getPreSpikeCount", [](const Synapse& self) {
            return self.getPreSpikeHistory().size();
        }, "Get number of pre-synaptic spikes")
        .def("getPostSpikeCount", [](const Synapse& self) {
            return self.getPostSpikeHistory().size();
        }, "Get number of post-synaptic spikes");

    // ActionResult class (already existing, adding Pythonic methods)
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world interaction)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("is_positive_reward", [](const ActionResult& self) { return self.reward > 0.0f; })
        .def("is_negative_reward", [](const ActionResult& self) { return self.reward < 0.0f; })
        .def("to_dict", [](const ActionResult& self) { return std::make_tuple(self.reward, self.success, self.message); });

    // Convenience factory functions
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

    m.def("createDefaultBrainConfig", []() {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for simulation
        config->set("simulation.step_count", "1000");
        config->set("simulation.dt", "0.01");
        config->set("neuron.lif.threshold", "-55.0");
        config->set("neuron.lif.reset_potential", "-70.0");
        config->set("neuron.lif.resting_potential", "-70.0");
        config->set("neuron.lif.leak_conductance", "10.0");
        config->set("neuron.lif.refractory_period", "5");
        config->set("plasticity.stdp.enabled", "true");
        config->set("plasticity.hebbian.enabled", "true");
        config->set("memory.working.capacity", "1000");
        config->set("memory.episodic.capacity", "100");
        return config;
    }, "Create a brain configuration with sensible defaults");

    m.def("createSimpleAgentConfig", []() {
        auto config = std::make_shared<Config>();
        // Simple agent configuration
        config->set("agent.max_memory", "100");
        config->set("agent.curiosity.sensitivity", "0.1");
        config->set("agent.planner.horizon", "10");
        config->set("agent.reward.discount", "0.95");
        config->set("agent.reward.noise", "0.01");
        return config;
    }, "Create a simple agent configuration");

    m.def("createWorldConfig", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        auto config = std::make_shared<Config>();
        config->set("world.width", std::to_string(width));
        config->set("world.height", std::to_string(height));
        config->set("world.vision_width", std::to_string(visionWidth));
        config->set("world.vision_height", std::to_string(visionHeight));
        return config;
    }, py::arg("width") = 100, py::arg("height") = 100, py::arg("visionWidth") = 20, py::arg("visionHeight") = 20,
       "Create a world configuration");

    // High-level convenience functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                              size_t steps, bool verbose = false) {
        brain->initialize();
        world->configure(100, 100, 20, 20);
        
        for (size_t i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(*percept);
            
            auto action = brain->produceAction();
            world->applyMotorCommand(*action, i);
            
            if (verbose && i % 100 == 0) {
                brain->logStatus();
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), 
                              brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 100, py::arg("verbose") = false,
       "Run a complete simulation loop");

    m.def("create_agent_system", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(100, 100, 20, 20);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), "Create a complete agent system (brain, world, agent)");

    // Error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    // Module metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";

    // System type constants
    m.attr("SIMULATION_STEP") = "SimulationStep";
    m.attr("TIMESTAMP") = "Timestamp";
    m.attr("SYNAPTIC_WEIGHT") = "SynapticWeight";
    m.attr("MEMBRANE_POTENTIAL") = "MembranePotential";
    m.attr("FIRING_RATE") = "FiringRate";
    m.attr("TIMESTEP_DURATION") = "TimestepDuration";

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;

    // Memory system type constants
    m.attr("MEMORY_TYPE_WORKING") = "working";
    m.attr("MEMORY_TYPE_EPISODIC") = "episodic";
    m.attr("MEMORY_TYPE_ASSOCIATIVE") = "associative";

    // Development stage constants
    m.attr("DEVELOPMENT_INITIAL") = "initial";
    m.attr("DEVELOPMENT_CRITICAL_PERIOD") = "critical_period";
    m.attr("DEVELOPMENT_MATURATION") = "maturation";
    m.attr("DEVELOPMENT_ADULT") = "adult";
    m.attr("DEVELOPMENT_AGING") = "aging";

    // Neuron type constants
    m.attr("NEURON_TYPE_EXCITATORY") = "excitatory";
    m.attr("NEURON_TYPE_INHIBITORY") = "inhibitory";
    m.attr("NEURON_TYPE_MODULATORY") = "modulatory";
    m.attr("NEURON_TYPE_SENSORY") = "sensory";
    m.attr("NEURON_TYPE_MOTOR") = "motor";
    m.attr("NEURON_TYPE_INTERNAL") = "internal";
}

} // namespace nlm
