#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

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

namespace {
// Helper function to create enum bindings with consistent documentation
template<typename T>
py::enum_<T> createEnum(py::module_ &m, const char* name, const char* doc) {
    return py::enum_<T>(m, name, doc)
        .export_values();
}

// Helper function to create ID class bindings with consistent pattern
template<typename IDClass>
py::class_<IDClass> createIdClass(py::module_ &m, const char* name, const char* doc) {
    return py::class_<IDClass>(m, name, doc)
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDClass::value)
        .def("index", &IDClass::index)
        .def("__eq__", &IDClass::operator==)
        .def("__ne__", &IDClass::operator!=)
        .def("__hash__", [](const IDClass& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const IDClass& id) {
            return "<" + std::string(name) + ": " + std::to_string(id.value) + ">";
        });
}

// Helper function to create simple class with property support
template<typename T>
py::class_<T> createSimpleClass(py::module_ &m, const char* name, const char* doc) {
    return py::class_<T>(m, name, doc);
}

// Helper function for returning values with better error handling
template<typename T>
py::return_value_policy safe_return() {
    return py::return_value_policy::reference_internal;
}

// Common initialization function for Config class
void configureConfig(py::class_<Config> &configClass) {
    configClass
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
        });
}

// Common initialization function for Action class
void configureAction(py::class_<Action> &actionClass) {
    actionClass
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
}

// Common initialization function for WorldObject class
void configureWorldObject(py::class_<WorldObject> &worldObjectClass) {
    worldObjectClass
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
}

// Context manager for resource cleanup
template<typename T>
class_<T> add_context_manager(py::class_<T>& cls) {
    return cls.def("__enter__", [](T& self) -> T& { return self; })
               .def("__exit__", [](T& /*self*/, py::args) {
                   // Add any cleanup logic here if needed
               });
}

// Convenience function type for factory functions
template<typename T, typename... Args>
std::shared_ptr<T> create_instance(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// Helper functions for statistical calculations
double calculate_std(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    double sum = 0.0;
    for (double v : values) {
        sum += (v - mean) * (v - mean);
    }
    return std::sqrt(sum / values.size());
}

double calculate_variance(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sum = 0.0;
    for (double v : values) {
        sum += (v - mean) * (v - mean);
    }
    return sum / (values.size() - 1);
}

py::list calculate_moving_average(const std::vector<double>& values, int windowSize) {
    py::list result;
    if (values.empty() || windowSize <= 0) return result;
    
    for (size_t i = 0; i < values.size(); ++i) {
        size_t start = std::max<size_t>(0, i - windowSize + 1);
        size_t end = i + 1;
        
        double sum = 0.0;
        for (size_t j = start; j < end; ++j) {
            sum += values[j];
        }
        
        double avg = sum / (end - start);
        result.append(avg);
    }
    
    return result;
}

// Helper function for CSV export
std::string escape_csv_field(const std::string& field) {
    if (field.find(',') != std::string::npos || field.find('"') != std::string::npos || field.find('\n') != std::string::npos) {
        std::string escaped = "\"" + field + "\"";
        std::replace(escaped.begin(), escaped.end(), '"', '"');
        return escaped;
    }
    return field;
}
} // namespace

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------

        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes for brain simulation, agents, and world interactions.

        Key Features:
        - Pythonic interface with properties and context managers
        - Rich documentation for all classes and methods
        - Resource management utilities
        - Convenience functions for common operations
        - Advanced visualization and monitoring tools
        - Comprehensive error handling and validation
        - Statistical analysis and data export capabilities

        Usage Example:
        ```python
        import pynlm

        # Create a brain with configuration
        config = pynlm.Config()
        config.set("brain.num_neurons", 1000)
        brain = pynlm.Brain(config)

        # Initialize the brain
        brain.initialize()

        # Run simulation steps
        for i in range(100):
            brain.step(i)

        # Create agent and world
        world = pynlm.SimpleWorld()
        agent_brain = pynlm.AgentBrain(brain)

        # Clean up resources
        brain.reset()
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    //====================================================================
    // CORE TYPES AND IDENTIFIERS
    //====================================================================

    // ID Types
    auto neuron_id_class = createIdClass<NeuronId>(m, "NeuronId",
        "Unique identifier for a neuron. Supports arithmetic operations and hashing for use as dictionary keys.");
    neuron_id_class.def("to_string", [](const NeuronId& id) { return std::to_string(id.value); });

    auto synapse_id_class = createIdClass<SynapseId>(m, "SynapseId",
        "Unique identifier for a synapse. Supports comparison and hashing for collections.");
    synapse_id_class.def("to_string", [](const SynapseId& id) { return std::to_string(id.value); });

    auto region_id_class = createIdClass<RegionId>(m, "RegionId",
        "Unique identifier for a brain region. Provides region lookup utilities.");
    region_id_class.def("to_string", [](const RegionId& id) { return std::to_string(id.value); });

    auto population_id_class = createIdClass<PopulationId>(m, "PopulationId",
        "Unique identifier for a neuron population. Manages neuron groups in the brain.");
    population_id_class.def("to_string", [](const PopulationId& id) { return std::to_string(id.value); });

    // Enum Types
    createEnum<NeuronType>(m, "NeuronType", "Enumeration of neuron types for neural modeling")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal);

    createEnum<SynapseType>(m, "SynapseType", "Enumeration of synapse types in neural circuits")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction);

    createEnum<DevelopmentalStage>(m, "DevelopmentalStage", "Neural development stages")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging);

    createEnum<FiringState>(m, "FiringState", "Neuron firing state enumeration")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited);

    createEnum<ActionType>(m, "ActionType", "Enumeration of motor action types")
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
        .value("Custom", ActionType::Custom);

    createEnum<MotorCommand>(m, "MotorCommand", "Low-level motor commands for agent control")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait);

    createEnum<WorldObjectType>(m, "WorldObjectType", "Types of objects in the simulation world")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker);

    //====================================================================
    // CONFIGURATION
    //====================================================================

    configureConfig(py::class_<Config>(m, "Config", "Configuration class for NLM system")));

    //====================================================================
    // SENSORY INPUTS
    //====================================================================

    py::class_<SensoryInput>(m, "SensoryInput", "Base class for all sensory inputs")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with property support
    auto vision_class = py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input for visual perception");
    vision_class
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");
    // Add property-like access
    vision_class.def_property_readonly("shape", [](const Vision& v) {
        return std::make_tuple(v.getWidth(), v.getHeight(), v.getChannels());
    }, "Get vision dimensions as (width, height, channels) tuple");

    // Audio class
    auto audio_class = py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input for sound perception");
    audio_class
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def_property_readonly("duration", [](const Audio& a) {
            return a.getNumSamples() / static_cast<float>(a.getSampleRate());
        }, "Get audio duration in seconds");

    // InternalSignals class
    auto internal_signals_class = py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal neural signals for brain state");
    internal_signals_class
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of internal signals")
        .def("__len__", &InternalSignals::getSignalCount)
        .def("__iter__", [](const InternalSignals& signals) {
            return py::make_iterator(signals.getSignals().begin(),
                                    signals.getSignals().end());
        }, py::return_value_policy::reference_internal);

    //====================================================================
    // MOTOR ACTIONS
    //====================================================================

    configureAction(py::class_<Action>(m, "Action", "Action representation for motor output"));

    //====================================================================
    // WORLD REPRESENTATION
    //====================================================================

    configureWorldObject(py::class_<WorldObject>(m, "WorldObject", "World object representation"));

    // WorldObject convenience methods
    py::class_<WorldObject>(m, "WorldObject")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
         "Calculate Euclidean distance to position (x, y)")
        .def("is_at", [](const WorldObject& self, float x, float y, float tolerance = 0.1f) {
            float dx = std::abs(self.x - x);
            float dy = std::abs(self.y - y);
            return (dx <= tolerance) && (dy <= tolerance);
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.1f,
         "Check if object is at position (x, y) within tolerance")
        .def("move_to", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
         "Move object to position (x, y)");

    // AgentBody class with properties
    auto agent_body_class = py::class_<AgentBody>(m, "AgentBody", "Agent body state and physical properties");
    agent_body_class
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset)
        .def("speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get current speed")
        .def("is_idle", [](const AgentBody& self) {
            return !self.isMoving && !self.isTurning;
        }, "Check if agent is idle");

    // ActionResult class
    auto action_result_class = py::class_<ActionResult>(m, "ActionResult", "Result of an action performed in the world");
    action_result_class
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about action result")
        .def("is_positive", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is positive (success with reward)")
        .def("is_negative", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result is negative (failure or penalty)");

    //====================================================================
    // SENSORY PERCEPTION
    //====================================================================

    auto sensory_percept_class = py::class_<SensoryPercept>(m, "SensoryPercept", "Combined sensory perception from all modalities");
    sensory_percept_class
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
        .def("is_sensory_available", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 || self.getTouch().size() > 0;
        }, "Check if any sensory input is available");

    //====================================================================
    // WORLD SIMULATION
    //====================================================================

    auto simple_world_class = py::class_<SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
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
        .def("getObjectCount", &SimpleWorld::getObjectCount)
        .def("__len__", &SimpleWorld::getObjectCount);

    // Context manager for world simulation
    simple_world_class.def("__enter__", [](SimpleWorld& self) -> SimpleWorld& { return self; })
                      .def("__exit__", [](SimpleWorld& /*self*/, py::args) {
                          // Clean up world state if needed
                      });

    // World convenience functions
    m.def("createWorld", [](int width, int height, int visionWidth, int visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
       "Create and configure a SimpleWorld instance");

    //====================================================================
    // BRAIN SIMULATION
    //====================================================================

    auto brain_class = py::class_<Brain>(m, "Brain", "Central neural simulation brain class with full neural network capabilities")
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
        .def("getNeuronCountByType", [](Brain& self, NeuronType type) {
            auto regions = self.getRegions();
            int count = 0;
            for (const auto& region : regions) {
                auto neurons = region.getNeurons();
                for (const auto& neuron : neurons) {
                    if (neuron.getType() == type) {
                        count++;
                    }
                }
            }
            return count;
        }, py::arg("type"), "Get neuron count by type")
        .def("getRegionNames", &Brain::getRegionIds,
             "Get region names/IDs")
        .def("clear", [](Brain& self) {
            self.reset();
        }, "Clear brain state and reset");

    // Context manager for brain operations
    brain_class.def("__enter__", [](Brain& self) -> Brain& { return self; })
               .def("__exit__", [](Brain& /*self*/, py::args) {
                   // Clean up brain state if needed
               });

    //====================================================================
    // AGENT INTERFACE
    //====================================================================

    auto agent_brain_class = py::class_<AgentBrain>(m, "AgentBrain", "Agent brain interface connecting NLM brain to world")
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

    // Context manager for agent operations
    agent_brain_class.def("__enter__", [](AgentBrain& self) -> AgentBrain& { return self; })
                     .def("__exit__", [](AgentBrain& /*self*/, py::args) {
                         // Clean up agent state if needed
                     });

    //====================================================================
    // FACTORY FUNCTIONS AND UTILITIES
    //====================================================================

    // Factory functions with error handling
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

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Convenience utility functions
    m.def("simulate_step", [](Brain& brain, SimulationStep step, Timestamp time = 0.0f) {
        brain.step(step, time);
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0f,
       "Convenience function to perform a single simulation step");

    m.def("create_agent_episode", [](AgentBrain& agent, float duration = 1.0f) {
        agent.reset();
        // Simple episode simulation (placeholder for actual episode logic)
        return "Episode completed";
    }, py::arg("agent"), py::arg("duration") = 1.0f,
       "Create a simple agent episode");

    // Example usage function
    m.def("example_usage", []() {
        py::print("NLM Python Bindings Example Usage:");
        py::print("==================================");
        py::print("");
        py::print("1. Create configuration:");
        py::print("   config = pynlm.Config()"); py::print("   config.set('brain.num_neurons', 1000)");
        py::print("");
        py::print("2. Create brain:");
        py::print("   brain = pynlm.Brain(config)");
        py::print("   brain.initialize()");
        py::print("");
        py::print("3. Run simulation:");
        py::print("   for i in range(100):");
        py::print("       brain.step(i)");
        py::print("");
        py::print("4. Create world and agent:");
        py::print("   world = pynlm.SimpleWorld()");
        py::print("   agent = pynlm.AgentBrain(brain)");
        py::print("   agent.initialize(world)");
        py::print("");
        py::print("5. Clean up:");
        py::print("   brain.reset()");
        py::print("   world.reset()");
        py::print("");
        py::print("For more examples, visit: https://github.com/your-repo/nlm-python");
        return "Example usage printed to console";
    }, "Print example usage to console");

    //====================================================================
    // ERROR HANDLING
    //====================================================================

    // Advanced exception handling for parameters and operations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    // Pythonic features for better error handling
    m.def("validate_parameters", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Validate required parameters
        if (config.has("brain.neuron_count") && config.getOr("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        if (config.has("brain.synapse_density") && (config.getOr("brain.synapse_density", 0.0) < 0.0 || config.getOr("brain.synapse_density", 0.0) > 1.0)) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
        
        if (config.has("brain.connection_probability") && (config.getOr("brain.connection_probability", 0.0) < 0.0 || config.getOr("brain.connection_probability", 0.0) > 1.0)) {
            errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
        
        if (!errors.empty()) {
            throw std::invalid_argument("Configuration validation failed:\n" + 
                                        std::accumulate(errors.begin(), errors.end(), std::string(),
                                                       [](const std::string& a, const std::string& b) {
                                                           return a + "\n" + b;
                                                       }));
        }
        
        return "Configuration is valid";
    }, py::arg("config"), "Validate configuration parameters and return success message or throw ValueError if invalid");

    m.def("get_validation_errors", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Validate required parameters
        if (config.has("brain.neuron_count") && config.getOr("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        if (config.has("brain.synapse_density") && (config.getOr("brain.synapse_density", 0.0) < 0.0 || config.getOr("brain.synapse_density", 0.0) > 1.0)) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
        
        if (config.has("brain.connection_probability") && (config.getOr("brain.connection_probability", 0.0) < 0.0 || config.getOr("brain.connection_probability", 0.0) > 1.0)) {
            errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
        
        return errors;
    }, py::arg("config"), "Get validation errors for configuration parameters");

    m.def("safe_simulation_step", [](Brain& brain, SimulationStep step, Timestamp time = 0.0f) {
        try {
            brain.step(step, time);
            return "Simulation step completed successfully";
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Simulation step failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0f,
       "Safely perform a simulation step with error handling and return success message");

    m.def("safe_brain_operation", [](std::shared_ptr<Brain> brain, std::function<void(std::shared_ptr<Brain>)> operation) {
        try {
            if (!brain) {
                throw std::invalid_argument("Brain pointer is null");
            }
            operation(brain);
            return "Operation completed successfully";
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Operation failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("operation"), "Safely execute a brain operation with error handling");

    // Advanced Pythonic features
    m.def("create_smart_brain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        
        class SmartBrain {
        public:
            std::shared_ptr<Brain> brain_;
            
            SmartBrain(std::shared_ptr<Brain> brain) : brain_(brain) {}
            
            // Context manager support
            SmartBrain& __enter__() {
                if (!brain_->initialize()) {
                    throw std::runtime_error("Failed to initialize brain");
                }
                return *this;
            }
            
            void __exit__(int /*exception_code*/, const char* /*exception_message*/, const char* /*traceback_message*/) {
                if (brain_) {
                    brain_->reset();
                }
            }
            
            // Property-like access for convenience
            int get_neuron_count() const {
                return brain_->getTotalNeuronCount();
            }
            
            int get_synapse_count() const {
                return brain_->getTotalSynapseCount();
            }
            
            float get_firing_rate() const {
                return brain_->getAverageFiringRate();
            }
            
            // Enhanced error handling for operations
            std::string run_simulation(int steps) {
                if (steps < 0) {
                    throw std::invalid_argument("Number of steps cannot be negative");
                }
                
                try {
                    for (int i = 0; i < steps; ++i) {
                        brain_->step(i);
                    }
                    return std::to_string(steps) + " simulation steps completed";
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string("Simulation failed: ") + e.what());
                }
            }
            
            // Dictionary-like access to brain state
            std::map<std::string, py::object> get_state_dict() {
                std::map<std::string, py::object> state;
                
                state["neuron_count"] = py::cast(brain_->getTotalNeuronCount());
                state["synapse_count"] = py::cast(brain_->getTotalSynapseCount());
                state["firing_count"] = py::cast(brain_->getFiringNeuronCount());
                state["average_firing_rate"] = py::cast(brain_->getAverageFiringRate());
                state["total_spikes"] = py::cast(brain_->getTotalSpikeCount());
                
                return state;
            }
            
            // Enhanced string representation
            std::string to_string() const {
                std::stringstream ss;
                ss << "SmartBrain:" << std::endl;
                ss << "  Neuron count: " << brain_->getTotalNeuronCount() << std::endl;
                ss << "  Synapse count: " << brain_->getTotalSynapseCount() << std::endl;
                ss << "  Firing count: " << brain_->getFiringNeuronCount() << std::endl;
                ss << "  Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
                ss << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
                return ss.str();
            }
            
            // Comparison operators
            bool operator==(const SmartBrain& other) const {
                return this->brain_.get() == other.brain_.get();
            }
            
            bool operator!=(const SmartBrain& other) const {
                return !(*this == other);
            }
        };
        
        return std::make_shared<SmartBrain>(brain);
    }, py::arg("config"), "Create a smart brain with enhanced Python features");

    // Additional utility functions
    m.def("safe_config_set", [](Config& config, const std::string& key, const std::string& value) {
        try {
            config.set(key, value);
            return "Successfully set " + key + " = " + value;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to set config value: ") + e.what());
        }
    }, py::arg("config"), py::arg("key"), py::arg("value"), "Safely set a configuration value with error handling");

    m.def("safe_config_get", [](const Config& config, const std::string& key) {
        try {
            if (!config.has(key)) {
                throw std::invalid_argument("Key '" + key + "' does not exist");
            }
            return config.getOr(key, "");
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to get config value: ") + e.what());
        }
    }, py::arg("config"), py::arg("key"), "Safely get a configuration value with error handling");

    // Validation and type checking
    m.def("validate_brain_config", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Check for required parameters
        if (!config.has("brain.neuron_count")) {
            errors.push_back("Missing required parameter: brain.neuron_count");
        }
        
        if (!config.has("brain.region_count")) {
            errors.push_back("Missing required parameter: brain.region_count");
        }
        
        // Validate parameter types and values
        if (config.has("brain.neuron_count")) {
            int neuron_count = config.getOr("brain.neuron_count", 0);
            if (neuron_count <= 0) {
                errors.push_back("brain.neuron_count must be a positive integer");
            }
            if (neuron_count > 100000) {
                errors.push_back("brain.neuron_count is too large (max 100000)");
            }
        }
        
        if (config.has("brain.region_count")) {
            int region_count = config.getOr("brain.region_count", 0);
            if (region_count <= 0) {
                errors.push_back("brain.region_count must be a positive integer");
            }
        }
        
        if (!errors.empty()) {
            throw std::invalid_argument("Configuration validation failed:\n" + 
                                        std::accumulate(errors.begin(), errors.end(), std::string(),
                                                       [](const std::string& a, const std::string& b) {
                                                           return a + "\n" + b;
                                                       }));
        }
        
        return "Configuration is valid";
    }, py::arg("config"), "Validate brain configuration and throw ValueError if invalid");

    // Pythonic iteration helpers
    m.def("iter_regions", [](std::shared_ptr<Brain> brain) {
        auto region_ids = brain->getRegionIds();
        return py::make_iterator(region_ids.begin(), region_ids.end());
    }, py::arg("brain"), "Get iterator for brain regions");

    m.def("get_region_stats", [](std::shared_ptr<Brain> brain, const RegionId& region_id) {
        auto region = brain->getRegion(region_id);
        if (!region) {
            throw std::invalid_argument("Region not found: " + std::to_string(region_id.value));
        }
        
        auto neurons = region->getAllNeurons();
        auto synapses = region->getSynapses();
        
        std::map<std::string, double> stats;
        stats["neuron_count"] = neurons.size();
        stats["synapse_count"] = synapses.size();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        return stats;
    }, py::arg("brain"), py::arg("region_id"), "Get statistics for a specific region");

    // Enhanced error reporting
    m.def("report_error", [](const std::string& error_type, const std::string& message, const std::string& context = "") {
        std::string full_message = "[" + error_type + "]";
        if (!context.empty()) {
            full_message += " Context: " + context;
        }
        full_message += " - " + message;
        
        throw std::runtime_error(full_message);
    }, py::arg("error_type"), py::arg("message"), py::arg("context") = "",
       "Report an error with detailed context and error type");

    // WorldObject factory function
    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
       "Create a new world object with specified properties");

    // Memory manager factory function
    m.def("createMemoryManager", [](std::shared_ptr<Brain> brain) {
        class MemoryManager {
        public:
            std::shared_ptr<Brain> brain_;
            
            MemoryManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
            
            // Save brain state
            bool save_state(const std::string& filename) {
                return brain_->save(filename);
            }
            
            // Load brain state
            bool load_state(const std::string& filename) {
                return brain_->load(filename);
            }
            
            // Get current memory statistics
            std::map<std::string, float> get_memory_stats() {
                std::map<std::string, float> stats;
                stats["neuron_count"] = brain_->getTotalNeuronCount();
                stats["synapse_count"] = brain_->getTotalSynapseCount();
                stats["firing_rate"] = brain_->getAverageFiringRate();
                stats["total_spikes"] = brain_->getTotalSpikeCount();
                return stats;
            }
            
            // Compact memory by removing inactive neurons (placeholder)
            void compact_memory() {
                // TODO: Implement actual memory compaction
                // This would involve identifying and removing inactive neurons/synapses
            }
            
            // Check if memory is approaching limits
            bool is_memory_warning() {
                auto stats = get_memory_stats();
                return stats["neuron_count"] > 10000 || stats["synapse_count"] > 50000;
            }
        };
        
        return std::make_shared<MemoryManager>(brain);
    }, py::arg("brain"), "Create a memory manager for brain state and statistics");

    // Visualizer factory function
    m.def("createVisualizer", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, float updateInterval = 1.0f) {
        class Visualizer {
        public:
            std::shared_ptr<Brain> brain_;
            std::shared_ptr<SimpleWorld> world_;
            float updateInterval_;
            
            Visualizer(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                      float updateInterval = 1.0f)
                : brain_(brain), world_(world), updateInterval_(updateInterval) {}
            
            // Get simple text visualization
            std::string get_brain_text_view() {
                std::stringstream ss;
                ss << "Brain State:" << std::endl;
                ss << "  Neurons: " << brain_->getTotalNeuronCount() << std::endl;
                ss << "  Synapses: " << brain_->getTotalSynapseCount() << std::endl;
                ss << "  Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
                ss << "  Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
                ss << "  Developmental Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
                return ss.str();
            }
            
            // Get world visualization
            std::string get_world_text_view() {
                std::stringstream ss;
                ss << "World State:" << std::endl;
                ss << "  Size: " << world_->getWidth() << "x" << world_->getHeight() << std::endl;
                ss << "  Objects: " << world_->getObjectCount() << std::endl;
                ss << "  Time: " << world_->getSimulationTime() << std::endl;
                ss << "  Energy Decay Rate: " << world_->getEnergyDecayRate() << std::endl;
                return ss.str();
            }
            
            // Get combined visualization
            std::string get_combined_view() {
                std::stringstream ss;
                ss << get_brain_text_view() << std::endl;
                ss << "---" << std::endl;
                ss << get_world_text_view() << std::endl;
                return ss.str();
            }
            
            // Check if visualization needs update
            bool needs_update(float timeSinceLastUpdate) {
                return timeSinceLastUpdate >= updateInterval_;
            }
        };
        
        return std::make_shared<Visualizer>(brain, world, updateInterval);
    }, py::arg("brain"), py::arg("world"), py::arg("updateInterval") = 1.0f,
       "Create a visualizer for displaying brain and world state");

    // Agent lifecycle factory function
    m.def("createAgentLifecycle", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   float maxEpisodes, float maxStepsPerEpisode) {
        class AgentLifecycle {
        public:
            std::shared_ptr<Brain> brain_;
            std::shared_ptr<SimpleWorld> world_;
            std::shared_ptr<AgentBrain> agent_;
            float maxEpisodes_;
            float maxStepsPerEpisode_;
            float currentEpisode_;
            float currentStep_;
            
            AgentLifecycle(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                          float maxEpisodes, float maxStepsPerEpisode)
                : brain_(brain), world_(world), maxEpisodes_(maxEpisodes),
                  maxStepsPerEpisode_(maxStepsPerEpisode), currentEpisode_(0.0f),
                  currentStep_(0.0f) {
                agent_ = std::make_shared<AgentBrain>(brain);
                agent_->initialize(*world_);
            }
            
            bool has_next_episode() const {
                return currentEpisode_ < maxEpisodes_;
            }
            
            void run_next_episode() {
                if (!has_next_episode()) throw std::runtime_error("No more episodes available");
                
                agent_->reset();
                currentStep_ = 0.0f;
                
                for (int step = 0; step < static_cast<int>(maxStepsPerEpisode_); ++step) {
                    // Get current percept
                    auto percept = world_->getSensoryPercept();
                    
                    // Process it
                    agent_->processSensoryInput(*percept);
                    
                    // Get action
                    auto action = agent_->decodeMotorCommand();
                    
                    // Apply action in world
                    world_->applyMotorCommand(action, world_->getSimulationTime());
                    
                    // Update world
                    world_->update(0.001f); // Use default timestep
                    
                    // Simulate brain
                    brain_->step(step, step * 0.001f);
                    
                    currentStep_ = step + 1.0f;
                }
                
                currentEpisode_ += 1.0f;
            }
            
            void reset() {
                agent_->reset();
                currentEpisode_ = 0.0f;
                currentStep_ = 0.0f;
            }
            
            float getEpisodeProgress() const {
                return currentEpisode_ / maxEpisodes_;
            }
            
            float getStepProgress() const {
                return currentStep_ / maxStepsPerEpisode_;
            }
        };
        
        return std::make_shared<AgentLifecycle>(brain, world, maxEpisodes, maxStepsPerEpisode);
    }, py::arg("brain"), py::arg("world"), py::arg("maxEpisodes"), py::arg("maxStepsPerEpisode"),
       "Create an agent lifecycle manager for running multiple episodes of simulation");

    // Training utility factory function
    m.def("createTrainingUtility", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                                     int numEpisodes, float explorationRate, float learningRate) {
        class TrainingUtility {
        public:
            std::shared_ptr<AgentBrain> agent_;
            std::shared_ptr<SimpleWorld> world_;
            int numEpisodes_;
            float explorationRate_;
            float learningRate_;
            
            TrainingUtility(std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                           int numEpisodes, float explorationRate, float learningRate)
                : agent_(agent), world_(world), numEpisodes_(numEpisodes),
                  explorationRate_(explorationRate), learningRate_(learningRate) {}
            
            std::tuple<std::vector<float>, std::vector<float>> train() {
                std::vector<float> episodeRewards;
                std::vector<float> episodeLengths;
                
                for (int episode = 0; episode < numEpisodes_; ++episode) {
                    float episodeReward = 0.0f;
                    float episodeLength = 0.0f;
                    
                    agent_->reset();
                    
                    for (int step = 0; step < 100; ++step) { // Fixed episode length
                        // Get sensory input
                        auto percept = world_->getSensoryPercept();
                        
                        // Process sensory input
                        agent_->processSensoryInput(*percept);
                        
                        // Get action
                        auto action = agent_->decodeMotorCommand();
                        
                        // Add exploration based on exploration rate
                        if (rand() / (float)RAND_MAX < explorationRate_) {
                            // Random action (simplified)
                            action = MotorCommand::MoveForward;
                        }
                        
                        // Apply action and get result
                        auto result = world_->applyMotorCommand(action, world_->getSimulationTime());
                        world_->update(0.001f);
                        
                        // Apply reward modulation
                        agent_->applyRewardModulation(result.reward, 0.0f);
                        
                        episodeReward += result.reward;
                        episodeLength += 1.0f;
                    }
                    
                    episodeRewards.push_back(episodeReward);
                    episodeLengths.push_back(episodeLength);
                }
                
                return std::make_tuple(episodeRewards, episodeLengths);
            }
            
            // Get average reward from last training session
            float get_average_reward(const std::vector<float>& rewards) {
                if (rewards.empty()) return 0.0f;
                float sum = 0.0f;
                for (float r : rewards) sum += r;
                return sum / rewards.size();
            }
        };
        
        return std::make_shared<TrainingUtility>(agent, world, numEpisodes, explorationRate, learningRate);
    }, py::arg("agent"), py::arg("world"), py::arg("numEpisodes"), py::arg("explorationRate") = 0.1f,
       py::arg("learningRate") = 0.01f,
       "Create a training utility for agent training with exploration and learning");

    //====================================================================
    // ADVANCED VISUALIZATION AND MONITORING
    //====================================================================

    // Advanced visualization with history tracking and data export
    m.def("create_advanced_visualizer", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         float updateInterval = 1.0f, bool enableAutoSave = false,
                                         const std::string& outputFile = "visualization_output.csv") {
        class AdvancedVisualizer {
        public:
            std::shared_ptr<Brain> brain_;
            std::shared_ptr<SimpleWorld> world_;
            float updateInterval_;
            bool enableAutoSave_;
            std::string outputFile_;
            
            std::vector<std::map<std::string, double>> history_;
            std::chrono::steady_clock::time_point lastUpdate_;
            
            AdvancedVisualizer(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              float updateInterval, bool enableAutoSave,
                              const std::string& outputFile)
                : brain_(brain), world_(world), updateInterval_(updateInterval),
                  enableAutoSave_(enableAutoSave), outputFile_(outputFile) {
                lastUpdate_ = std::chrono::steady_clock::now();
                history_.reserve(10000); // Pre-allocate for performance
            }
            
            // Update visualization with automatic history tracking
            void update() {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate_).count();
                
                if (elapsed >= updateInterval_) {
                    // Collect current state
                    std::map<std::string, double> current_state;
                    
                    // Brain statistics
                    current_state["time"] = world_->getSimulationTime();
                    current_state["neuron_count"] = brain_->getTotalNeuronCount();
                    current_state["synapse_count"] = brain_->getTotalSynapseCount();
                    current_state["active_neuron_count"] = brain_->getActiveNeuronCount();
                    current_state["firing_neuron_count"] = brain_->getFiringNeuronCount();
                    current_state["average_firing_rate"] = brain_->getAverageFiringRate();
                    current_state["total_spikes"] = brain_->getTotalSpikeCount();
                    current_state["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
                    current_state["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
                    
                    // Neuromodulation levels
                    auto agent = std::dynamic_pointer_cast<AgentBrain>(brain_);
                    if (agent) {
                        current_state["curiosity_level"] = agent->getCuriosityLevel();
                        current_state["novelty_level"] = agent->getNoveltyLevel();
                        current_state["prediction_error"] = agent->getPredictionError();
                        current_state["neuromodulation_level"] = agent->getNeuromodulationLevel();
                    }
                    
                    // World statistics
                    current_state["world_width"] = world_->getWidth();
                    current_state["world_height"] = world_->getHeight();
                    current_state["world_objects"] = world_->getObjectCount();
                    current_state["agent_x"] = world_->getAgentBody().x;
                    current_state["agent_y"] = world_->getAgentBody().y;
                    current_state["agent_energy"] = world_->getAgentBody().energy;
                    current_state["agent_health"] = world_->getAgentBody().health;
                    current_state["simulation_time"] = world_->getSimulationTime();
                    
                    // Store in history
                    history_.push_back(current_state);
                    
                    // Auto-save if enabled
                    if (enableAutoSave_) {
                        save_to_file();
                    }
                    
                    lastUpdate_ = now;
                }
            }
            
            // Get historical data as Python list of dictionaries
            py::list get_history() const {
                py::list result;
                for (const auto& state : history_) {
                    py::dict py_state;
                    for (const auto& pair : state) {
                        py_state[pair.first] = pair.second;
                    }
                    result.append(py_state);
                }
                return result;
            }
            
            // Get statistics for historical data
            std::map<std::string, double> get_history_stats() const {
                if (history_.empty()) {
                    return {};
                }
                
                std::map<std::string, double> stats;
                
                // Time series statistics
                std::vector<double> times;
                for (const auto& state : history_) {
                    if (state.find("time") != state.end()) {
                        times.push_back(state.at("time"));
                    }
                }
                
                if (!times.empty()) {
                    double sum = 0.0, min_val = times[0], max_val = times[0];
                    for (double t : times) {
                        sum += t;
                        min_val = std::min(min_val, t);
                        max_val = std::max(max_val, t);
                    }
                    
                    stats["time_span_seconds"] = max_val - min_val;
                    stats["average_time_interval"] = (max_val - min_val) / (times.size() - 1);
                    stats["total_samples"] = times.size();
                }
                
                // Neuron count statistics
                std::vector<double> neuron_counts;
                for (const auto& state : history_) {
                    if (state.find("neuron_count") != state.end()) {
                        neuron_counts.push_back(state.at("neuron_count"));
                    }
                }
                
                if (!neuron_counts.empty()) {
                    double sum = 0.0, min_val = neuron_counts[0], max_val = neuron_counts[0];
                    for (double n : neuron_counts) {
                        sum += n;
                        min_val = std::min(min_val, n);
                        max_val = std::max(max_val, n);
                    }
                    
                    stats["neuron_count_avg"] = sum / neuron_counts.size();
                    stats["neuron_count_min"] = min_val;
                    stats["neuron_count_max"] = max_val;
                    stats["neuron_count_std"] = calculate_std(neuron_counts, stats["neuron_count_avg"]);
                }
                
                // Firing rate statistics
                std::vector<double> firing_rates;
                for (const auto& state : history_) {
                    if (state.find("average_firing_rate") != state.end()) {
                        firing_rates.push_back(state.at("average_firing_rate"));
                    }
                }
                
                if (!firing_rates.empty()) {
                    double sum = 0.0, min_val = firing_rates[0], max_val = firing_rates[0];
                    for (double fr : firing_rates) {
                        sum += fr;
                        min_val = std::min(min_val, fr);
                        max_val = std::max(max_val, fr);
                    }
                    
                    stats["firing_rate_avg"] = sum / firing_rates.size();
                    stats["firing_rate_min"] = min_val;
                    stats["firing_rate_max"] = max_val;
                    stats["firing_rate_std"] = calculate_std(firing_rates, stats["firing_rate_avg"]);
                }
                
                return stats;
            }
            
            // Save historical data to CSV file
            bool save_to_file() {
                if (history_.empty()) {
                    return false;
                }
                
                std::ofstream file(outputFile_, std::ios::app);
                if (!file.is_open()) {
                    return false;
                }
                
                // Write header if file is empty
                file.seekp(0, std::ios::end);
                if (file.tellp() == 0) {
                    file << "timestamp,neuron_count,synapse_count,active_neuron_count,firing_neuron_count,average_firing_rate,total_spikes,e_i_ratio,developmental_stage,curiosity_level,novelty_level,prediction_error,neuromodulation_level,world_width,world_height,world_objects,agent_x,agent_y,agent_energy,agent_health,simulation_time" << std::endl;
                }
                
                // Write data
                for (const auto& state : history_) {
                    file << state["time"] << ",";
                    file << state["neuron_count"] << ",";
                    file << state["synapse_count"] << ";";
                    file << state["active_neuron_count"] << ";";
                    file << state["firing_neuron_count"] << ";";
                    file << state["average_firing_rate"] << ";";
                    file << state["total_spikes"] << ";";
                    file << state["e_i_ratio"] << ";";
                    file << state["developmental_stage"] << ";";
                    
                    // Optional fields
                    if (state.find("curiosity_level") != state.end()) {
                        file << state.at("curiosity_level") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (state.find("novelty_level") != state.end()) {
                        file << state.at("novelty_level") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (state.find("prediction_error") != state.end()) {
                        file << state.at("prediction_error") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (state.find("neuromodulation_level") != state.end()) {
                        file << state.at("neuromodulation_level") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    file << state["world_width"] << ";";
                    file << state["world_height"] << ";";
                    file << state["world_objects"] << ";";
                    file << state["agent_x"] << ";";
                    file << state["agent_y"] << ";";
                    file << state["agent_energy"] << ";";
                    file << state["agent_health"] << ";";
                    file << state["simulation_time"] << std::endl;
                }
                
                file.close();
                return true;
            }
            
            // Clear history
            void clear_history() {
                history_.clear();
            }
            
            // Get current state as dictionary
            py::dict get_current_state() const {
                py::dict state;
                
                if (brain_ && world_) {
                    // Brain state
                    state["neuron_count"] = brain_->getTotalNeuronCount();
                    state["synapse_count"] = brain_->getTotalSynapseCount();
                    state["active_neuron_count"] = brain_->getActiveNeuronCount();
                    state["firing_neuron_count"] = brain_->getFiringNeuronCount();
                    state["average_firing_rate"] = brain_->getAverageFiringRate();
                    state["total_spikes"] = brain_->getTotalSpikeCount();
                    state["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
                    state["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
                    
                    // Neuromodulation
                    auto agent = std::dynamic_pointer_cast<AgentBrain>(brain_);
                    if (agent) {
                        state["curiosity_level"] = agent->getCuriosityLevel();
                        state["novelty_level"] = agent->getNoveltyLevel();
                        state["prediction_error"] = agent->getPredictionError();
                        state["neuromodulation_level"] = agent->getNeuromodulationLevel();
                    }
                    
                    // World state
                    state["world_width"] = world_->getWidth();
                    state["world_height"] = world_->getHeight();
                    state["world_objects"] = world_->getObjectCount();
                    state["agent_x"] = world_->getAgentBody().x;
                    state["agent_y"] = world_->getAgentBody().y;
                    state["agent_energy"] = world_->getAgentBody().energy;
                    state["agent_health"] = world_->getAgentBody().health;
                    state["simulation_time"] = world_->getSimulationTime();
                    
                    // Timestamp
                    auto now = std::chrono::steady_clock::now();
                    auto epoch = now.time_since_epoch();
                    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                    state["timestamp"] = seconds.count();
                }
                
                return state;
            }
            
            // Generate real-time plot data
            py::dict get_realtime_plot_data(int max_points = 1000) {
                py::dict result;
                
                // Get recent data (last max_points or all if fewer)
                size_t start_index = std::max<size_t>(0, history_.size() - max_points);
                std::vector<double> times, firing_rates, neuron_counts;
                
                for (size_t i = start_index; i < history_.size(); ++i) {
                    const auto& state = history_[i];
                    if (state.find("time") != state.end()) {
                        times.push_back(state.at("time"));
                    }
                    if (state.find("average_firing_rate") != state.end()) {
                        firing_rates.push_back(state.at("average_firing_rate"));
                    }
                    if (state.find("neuron_count") != state.end()) {
                        neuron_counts.push_back(state.at("neuron_count"));
                    }
                }
                
                // Convert to Python lists
                py::list py_times = py::cast(times);
                py::list py_firing_rates = py::cast(firing_rates);
                py::list py_neuron_counts = py::cast(neuron_counts);
                
                result["time"] = py_times;
                result["firing_rate"] = py_firing_rates;
                result["neuron_count"] = py_neuron_counts;
                
                // Calculate moving averages
                py::list moving_avg_firing = calculate_moving_average(firing_rates, 10);
                py::list moving_avg_neurons = calculate_moving_average(neuron_counts, 10);
                
                result["moving_avg_firing_rate"] = moving_avg_firing;
                result["moving_avg_neuron_count"] = moving_avg_neurons;
                
                return result;
            }
            
            // Performance monitoring
            std::map<std::string, double> get_performance_metrics() const {
                std::map<std::string, double> metrics;
                
                if (history_.size() < 2) {
                    return metrics;
                }
                
                // Calculate performance metrics
                double total_time = 0.0;
                double total_neurons = 0.0;
                double total_spikes = 0.0;
                double total_firing = 0.0;
                
                for (const auto& state : history_) {
                    if (state.find("time") != state.end()) {
                        total_time += state.at("time");
                    }
                    if (state.find("neuron_count") != state.end()) {
                        total_neurons += state.at("neuron_count");
                    }
                    if (state.find("total_spikes") != state.end()) {
                        total_spikes += state.at("total_spikes");
                    }
                    if (state.find("average_firing_rate") != state.end()) {
                        total_firing += state.at("average_firing_rate");
                    }
                }
                
                size_t count = history_.size();
                
                metrics["avg_simulation_time"] = total_time / count;
                metrics["avg_neuron_count"] = total_neurons / count;
                metrics["avg_total_spikes"] = total_spikes / count;
                metrics["avg_firing_rate"] = total_firing / count;
                
                // Calculate efficiency metrics
                if (total_time > 0) {
                    metrics["neurons_per_second"] = (total_neurons / count) / (total_time / count);
                    metrics["spikes_per_second"] = (total_spikes / count) / (total_time / count);
                }
                
                return metrics;
            }
            
            // Export analysis results
            py::dict export_analysis_results() const {
                py::dict results;
                
                // Get history statistics
                auto stats = get_history_stats();
                results["history_statistics"] = py::cast(stats);
                
                // Get performance metrics
                auto metrics = get_performance_metrics();
                results["performance_metrics"] = py::cast(metrics);
                
                // Generate summary
                py::dict summary;
                summary["total_simulated_time"] = stats.find("time_span_seconds") != stats.end() ? stats.at("time_span_seconds") : 0.0;
                summary["total_simulation_steps"] = stats.find("total_samples") != stats.end() ? stats.at("total_samples") : 0;
                summary["peak_neuron_count"] = stats.find("neuron_count_max") != stats.end() ? stats.at("neuron_count_max") : 0.0;
                summary["peak_firing_rate"] = stats.find("firing_rate_max") != stats.end() ? stats.at("firing_rate_max") : 0.0;
                summary["final_neuron_count"] = history_.empty() ? 0.0 : history_.back().at("neuron_count");
                summary["final_firing_rate"] = history_.empty() ? 0.0 : history_.back().at("average_firing_rate");
                
                results["summary"] = summary;
                
                // Export recommendations
                py::list recommendations;
                
                if (stats.find("neuron_count_max") != stats.end() && stats.at("neuron_count_max") > 10000) {
                    recommendations.append("Consider reducing neuron count for better performance");
                }
                
                if (metrics.find("neurons_per_second") != metrics.end() && metrics.at("neurons_per_second") < 100) {
                    recommendations.append("Neuron processing rate is low, consider optimization");
                }
                
                results["recommendations"] = recommendations;
                
                return results;
            }
        };
        
        return std::make_shared<AdvancedVisualizer>(brain, world, updateInterval, enableAutoSave, outputFile);
    }, py::arg("brain"), py::arg("world"), py::arg("updateInterval") = 1.0f,
       py::arg("enableAutoSave") = false, py::arg("outputFile") = "visualization_output.csv",
       "Create an advanced visualizer with real-time monitoring, history tracking, and data export capabilities");

    // Real-time monitoring utilities
    m.def("create_realtime_monitor", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       float monitorInterval = 0.5f, int maxHistorySize = 1000) {
        class RealtimeMonitor {
        public:
            std::shared_ptr<Brain> brain_;
            std::shared_ptr<SimpleWorld> world_;
            float monitorInterval_;
            int maxHistorySize_;
            
            std::vector<std::map<std::string, double>> monitorHistory_;
            std::chrono::steady_clock::time_point lastMonitor_;
            bool monitoring_;
            
            RealtimeMonitor(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                           float monitorInterval, int maxHistorySize)
                : brain_(brain), world_(world), monitorInterval_(monitorInterval),
                  maxHistorySize_(maxHistorySize), monitoring_(false) {
                lastMonitor_ = std::chrono::steady_clock::now();
                monitorHistory_.reserve(maxHistorySize);
            }
            
            // Start monitoring
            void start_monitoring() {
                monitoring_ = true;
                lastMonitor_ = std::chrono::steady_clock::now();
            }
            
            // Stop monitoring
            void stop_monitoring() {
                monitoring_ = false;
            }
            
            // Update monitoring data
            void update_monitor() {
                if (!monitoring_) {
                    return;
                }
                
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastMonitor_).count();
                
                if (elapsed >= monitorInterval_) {
                    // Collect monitoring data
                    std::map<std::string, double> monitor_data;
                    
                    // Time
                    monitor_data["timestamp"] = world_->getSimulationTime();
                    
                    // Brain metrics
                    monitor_data["neuron_count"] = brain_->getTotalNeuronCount();
                    monitor_data["synapse_count"] = brain_->getTotalSynapseCount();
                    monitor_data["active_neuron_count"] = brain_->getActiveNeuronCount();
                    monitor_data["firing_neuron_count"] = brain_->getFiringNeuronCount();
                    monitor_data["average_firing_rate"] = brain_->getAverageFiringRate();
                    monitor_data["total_spikes"] = brain_->getTotalSpikeCount();
                    monitor_data["excitatory_synapses"] = brain_->getTotalSynapseCount() * 0.7; // Estimate
                    monitor_data["inhibitory_synapses"] = brain_->getTotalSynapseCount() * 0.3; // Estimate
                    
                    // Neuromodulation
                    auto agent = std::dynamic_pointer_cast<AgentBrain>(brain_);
                    if (agent) {
                        monitor_data["dopamine_level"] = agent->getNeuromodulationLevel();
                        monitor_data["curiosity_level"] = agent->getCuriosityLevel();
                        monitor_data["novelty_level"] = agent->getNoveltyLevel();
                        monitor_data["prediction_error"] = agent->getPredictionError();
                        monitor_data["reward_modulation"] = agent->getNeuromodulationLevel();
                    }
                    
                    // Development
                    monitor_data["developmental_stage"] = static_cast<int>(brain_->getDevelopmentalStage());
                    
                    // World metrics
                    monitor_data["world_width"] = world_->getWidth();
                    monitor_data["world_height"] = world_->getHeight();
                    monitor_data["world_objects"] = world_->getObjectCount();
                    monitor_data["agent_x"] = world_->getAgentBody().x;
                    monitor_data["agent_y"] = world_->getAgentBody().y;
                    monitor_data["agent_energy"] = world_->getAgentBody().energy;
                    monitor_data["agent_health"] = world_->getAgentBody().health;
                    monitor_data["agent_age"] = world_->getAgentBody().age;
                    
                    // Store in history (with limit)
                    monitorHistory_.push_back(monitor_data);
                    
                    if (monitorHistory_.size() > maxHistorySize_) {
                        monitorHistory_.erase(monitorHistory_.begin());
                    }
                    
                    lastMonitor_ = now;
                }
            }
            
            // Get monitoring history
            py::list get_monitor_history() const {
                py::list result;
                for (const auto& data : monitorHistory_) {
                    py::dict py_data;
                    for (const auto& pair : data) {
                        py_data[pair.first] = pair.second;
                    }
                    result.append(py_data);
                }
                return result;
            }
            
            // Get current monitoring data
            py::dict get_current_monitor_data() const {
                py::dict data;
                
                if (brain_ && world_) {
                    // Basic metrics
                    data["timestamp"] = world_->getSimulationTime();
                    data["neuron_count"] = brain_->getTotalNeuronCount();
                    data["synapse_count"] = brain_->getTotalSynapseCount();
                    data["active_neuron_count"] = brain_->getActiveNeuronCount();
                    data["firing_neuron_count"] = brain_->getFiringNeuronCount();
                    data["average_firing_rate"] = brain_->getAverageFiringRate();
                    data["total_spikes"] = brain_->getTotalSpikeCount();
                    
                    // Health indicators
                    data["brain_health"] = calculate_brain_health();
                    data["stability_score"] = calculate_stability_score();
                    data["activity_level"] = calculate_activity_level();
                    
                    // Alerts
                    data["alerts"] = get_active_alerts();
                }
                
                return data;
            }
            
            // Calculate brain health score (0-100)
            double calculate_brain_health() const {
                double health = 100.0;
                
                if (!brain_) return health;
                
                // Penalize low neuron count
                double neuron_ratio = brain_->getTotalNeuronCount() / 1000.0; // Expected around 1000
                if (neuron_ratio < 0.1) health *= 0.5;
                else if (neuron_ratio > 2.0) health *= 0.9;
                
                // Penalize high spike rate variability
                // (simplified calculation)
                double firing_rate = brain_->getAverageFiringRate();
                if (firing_rate > 100.0) health *= 0.8;
                
                // Check developmental stage
                auto stage = brain_->getDevelopmentalStage();
                if (stage == nlm::DevelopmentalStage::Initial || stage == nlm::DevelopmentalStage::Aging) {
                    health *= 0.7;
                }
                
                return std::max(0.0, health);
            }
            
            // Calculate stability score (0-100)
            double calculate_stability_score() const {
                if (monitorHistory_.size() < 2) return 50.0;
                
                // Calculate variance in key metrics
                std::vector<double> neuron_counts, firing_rates;
                for (const auto& data : monitorHistory_) {
                    if (data.find("neuron_count") != data.end()) {
                        neuron_counts.push_back(data.at("neuron_count"));
                    }
                    if (data.find("average_firing_rate") != data.end()) {
                        firing_rates.push_back(data.at("average_firing_rate"));
                    }
                }
                
                double neuron_variance = calculate_variance(neuron_counts);
                double firing_variance = calculate_variance(firing_rates);
                
                // Lower variance = higher stability
                double stability = 100.0 - (neuron_variance + firing_variance) / 2.0;
                return std::max(0.0, stability);
            }
            
            // Calculate activity level (normalized)
            double calculate_activity_level() const {
                if (!brain_) return 0.0;
                
                double firing_rate = brain_->getAverageFiringRate();
                double spike_count = brain_->getTotalSpikeCount();
                
                // Normalize to 0-100
                double activity = std::min(100.0, (firing_rate * 0.1) + (spike_count * 0.001));
                return activity;
            }
            
            // Get active alerts based on thresholds
            py::list get_active_alerts() const {
                py::list alerts;
                
                if (!brain_) return alerts;
                
                // Check for high firing rate
                double firing_rate = brain_->getAverageFiringRate();
                if (firing_rate > 50.0) {
                    py::dict alert;
                    alert["type"] = "high_firing_rate";
                    alert["value"] = firing_rate;
                    alert["severity"] = firing_rate > 100.0 ? "high" : "medium";
                    alert["message"] = "Firing rate is high, consider reducing neuron count or simulation speed";
                    alerts.append(alert);
                }
                
                // Check for low neuron count
                double neuron_count = brain_->getTotalNeuronCount();
                if (neuron_count < 100) {
                    py::dict alert;
                    alert["type"] = "low_neuron_count";
                    alert["value"] = neuron_count;
                    alert["severity"] = "medium";
                    alert["message"] = "Neuron count is low, brain may not function optimally";
                    alerts.append(alert);
                }
                
                // Check for high spike count variability (simplified)
                if (monitorHistory_.size() >= 3) {
                    double total_spikes = brain_->getTotalSpikeCount();
                    if (total_spikes > 1000) {
                        py::dict alert;
                        alert["type"] = "high_activity";
                        alert["value"] = total_spikes;
                        alert["severity"] = "low";
                        alert["message"] = "High activity detected, consider reducing simulation duration";
                        alerts.append(alert);
                    }
                }
                
                return alerts;
            }
            
            // Start/stop monitoring
            bool is_monitoring() const { return monitoring_; }
            
            // Export monitoring data
            bool export_monitoring_data(const std::string& filename) {
                std::ofstream file(filename);
                if (!file.is_open()) {
                    return false;
                }
                
                // Write CSV header
                file << "timestamp,neuron_count,synapse_count,active_neuron_count,firing_neuron_count,average_firing_rate,total_spikes,dopamine_level,curiosity_level,novelty_level,prediction_error,developmental_stage,world_width,world_height,world_objects,agent_x,agent_y,agent_energy,agent_health,agent_age,brain_health,stability_score,activity_level" << std::endl;
                
                // Write data
                for (const auto& data : monitorHistory_) {
                    file << data["timestamp"] << ",";
                    file << data["neuron_count"] << ";";
                    file << data["synapse_count"] << ";";
                    file << data["active_neuron_count"] << ";";
                    file << data["firing_neuron_count"] << ";";
                    file << data["average_firing_rate"] << ";";
                    file << data["total_spikes"] << ";";
                    
                    // Optional fields
                    if (data.find("dopamine_level") != data.end()) {
                        file << data.at("dopamine_level") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (data.find("curiosity_level") != data.end()) {
                        file << data.at("curiosity_level") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (data.find("novelty_level") != data.end()) {
                        file << data.at("novelty_level") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (data.find("prediction_error") != data.end()) {
                        file << data.at("prediction_error") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (data.find("developmental_stage") != data.end()) {
                        file << data.at("developmental_stage") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    file << data["world_width"] << ";";
                    file << data["world_height"] << ";";
                    file << data["world_objects"] << ";";
                    file << data["agent_x"] << ";";
                    file << data["agent_y"] << ";";
                    file << data["agent_energy"] << ";";
                    file << data["agent_health"] << ";";
                    file << data["agent_age"] << ";";
                    
                    if (data.find("brain_health") != data.end()) {
                        file << data.at("brain_health") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (data.find("stability_score") != data.end()) {
                        file << data.at("stability_score") << ";";
                    } else {
                        file << "NaN" << ";";
                    }
                    
                    if (data.find("activity_level") != data.end()) {
                        file << data.at("activity_level");
                    } else {
                        file << "NaN";
                    }
                    
                    file << std::endl;
                }
                
                file.close();
                return true;
            }
        };
        
        return std::make_shared<RealtimeMonitor>(brain, world, monitorInterval, maxHistorySize);
    }, py::arg("brain"), py::arg("world"), py::arg("monitorInterval") = 0.5f, py::arg("maxHistorySize") = 1000,
       "Create a real-time monitoring system for tracking brain and world metrics");

    // Data analysis and export utilities
    m.def("analyze_simulation_data", [](const std::string& dataFile) {
        std::ifstream file(dataFile);
        if (!file.is_open()) {
            throw std::invalid_argument("Cannot open data file: " + dataFile);
        }
        
        std::vector<std::map<std::string, double>> data;
        std::string line;
        std::getline(file, line); // Skip header
        
        while (std::getline(file, line)) {
            std::map<std::string, double> row;
            std::stringstream line_stream(line);
            std::string cell;
            int col = 0;
            
            while (std::getline(line_stream, cell, ',')) {
                if (col == 0) {
                    row["timestamp"] = std::stod(cell);
                } else if (col == 1) {
                    row["neuron_count"] = std::stod(cell);
                } else if (col == 2) {
                    row["synapse_count"] = std::stod(cell);
                } else if (col == 3) {
                    row["active_neuron_count"] = std::stod(cell);
                } else if (col == 4) {
                    row["firing_neuron_count"] = std::stod(cell);
                } else if (col == 5) {
                    row["average_firing_rate"] = std::stod(cell);
                } else if (col == 6) {
                    row["total_spikes"] = std::stod(cell);
                } else if (col == 7) {
                    row["dopamine_level"] = std::stod(cell);
                } else if (col == 8) {
                    row["curiosity_level"] = std::stod(cell);
                } else if (col == 9) {
                    row["novelty_level"] = std::stod(cell);
                } else if (col == 10) {
                    row["prediction_error"] = std::stod(cell);
                } else if (col == 11) {
                    row["developmental_stage"] = std::stod(cell);
                } else if (col == 12) {
                    row["world_width"] = std::stod(cell);
                } else if (col == 13) {
                    row["world_height"] = std::stod(cell);
                } else if (col == 14) {
                    row["world_objects"] = std::stod(cell);
                } else if (col == 15) {
                    row["agent_x"] = std::stod(cell);
                } else if (col == 16) {
                    row["agent_y"] = std::stod(cell);
                } else if (col == 17) {
                    row["agent_energy"] = std::stod(cell);
                } else if (col == 18) {
                    row["agent_health"] = std::stod(cell);
                } else if (col == 19) {
                    row["agent_age"] = std::stod(cell);
                } else if (col == 20) {
                    row["brain_health"] = std::stod(cell);
                } else if (col == 21) {
                    row["stability_score"] = std::stod(cell);
                } else if (col == 22) {
                    row["activity_level"] = std::stod(cell);
                }
                
                col++;
            }
            
            data.push_back(row);
        }
        
        file.close();
        
        py::dict results;
        
        // Calculate statistics
        if (!data.empty()) {
            // Time statistics
            double min_time = data[0]["timestamp"];
            double max_time = data[0]["timestamp"];
            double sum_time = 0.0;
            
            for (const auto& row : data) {
                min_time = std::min(min_time, row.at("timestamp"));
                max_time = std::max(max_time, row.at("timestamp"));
                sum_time += row.at("timestamp");
            }
            
            results["time_span"] = max_time - min_time;
            results["average_timestamp"] = sum_time / data.size();
            
            // Neuron count statistics
            double sum_neurons = 0.0, min_neurons = data[0]["neuron_count"], max_neurons = data[0]["neuron_count"];
            for (const auto& row : data) {
                sum_neurons += row.at("neuron_count");
                min_neurons = std::min(min_neurons, row.at("neuron_count"));
                max_neurons = std::max(max_neurons, row.at("neuron_count"));
            }
            
            results["neuron_count_avg"] = sum_neurons / data.size();
            results["neuron_count_min"] = min_neurons;
            results["neuron_count_max"] = max_neurons;
            
            // Firing rate statistics
            double sum_firing = 0.0, min_firing = data[0]["average_firing_rate"], max_firing = data[0]["average_firing_rate"];
            for (const auto& row : data) {
                sum_firing += row.at("average_firing_rate");
                min_firing = std::min(min_firing, row.at("average_firing_rate"));
                max_firing = std::max(max_firing, row.at("average_firing_rate"));
            }
            
            results["firing_rate_avg"] = sum_firing / data.size();
            results["firing_rate_min"] = min_firing;
            results["firing_rate_max"] = max_firing;
            
            // Spike statistics
            double sum_spikes = 0.0, min_spikes = data[0]["total_spikes"], max_spikes = data[0]["total_spikes"];
            for (const auto& row : data) {
                sum_spikes += row.at("total_spikes");
                min_spikes = std::min(min_spikes, row.at("total_spikes"));
                max_spikes = std::max(max_spikes, row.at("total_spikes"));
            }
            
            results["total_spikes_avg"] = sum_spikes / data.size();
            results["total_spikes_min"] = min_spikes;
            results["total_spikes_max"] = max_spikes;
            
            // Activity trends
            results["activity_trend"] = (results["firing_rate_max"] - results["firing_rate_min"]) / results["time_span"];
            
            // Stability indicators
            double neuron_variance = calculate_variance_from_data(data, "neuron_count");
            double firing_variance = calculate_variance_from_data(data, "average_firing_rate");
            
            results["neuron_stability"] = 100.0 - neuron_variance;
            results["firing_stability"] = 100.0 - firing_variance;
            
            // Health indicators
            results["data_quality_score"] = std::min(100.0, data.size() * 10.0); // Simple quality metric
        }
        
        return results;
    }, py::arg("dataFile"), "Analyze simulation data from CSV file and return statistical results");

    //====================================================================
    // EXAMPLE USAGE FUNCTIONS
    //====================================================================

    // Basic example usage
    m.def("example_usage", []() {
        py::print("NLM Python Bindings Example Usage:");
        py::print("==================================");
        py::print("");
        py::print("1. Create configuration:");
        py::print("   config = pynlm.Config()"); py::print("   config.set('brain.num_neurons', 1000)");
        py::print("");
        py::print("2. Create brain:");
        py::print("   brain = pynlm.Brain(config)");
        py::print("   brain.initialize()");
        py::print("");
        py::print("3. Run simulation:");
        py::print("   for i in range(100):");
        py::print("       brain.step(i)");
        py::print("");
        py::print("4. Create world and agent:");
        py::print("   world = pynlm.SimpleWorld()");
        py::print("   agent = pynlm.AgentBrain(brain)");
        py::print("   agent.initialize(world)");
        py::print("");
        py::print("5. Clean up:");
        py::print("   brain.reset()");
        py::print("   world.reset()");
        py::print("");
        py::print("For more examples, visit: https://github.com/your-repo/nlm-python");
        return "Example usage printed to console";
    }, "Print example usage to console");

    // Advanced example: Complete agent training
    m.def("example_advanced_training", []() {
        py::print("=== Advanced Training Example ===");
        py::print("");
        py::print("This example demonstrates a complete agent training workflow:");
        py::print("");
        
        // Create configuration
        py::print("1. Creating configuration...");
        py::auto config = pynlm.createDefaultConfig();
        config->set("brain.num_neurons", 500);
        config->set("brain.region_count", 1);
        
        // Create brain
        py::print("2. Creating brain...");
        py::auto brain = pynlm.createBrain(config);
        brain->initialize();
        
        // Create world and agent
        py::print("3. Creating world and agent...");
        py::auto world = pynlm.createSimpleWorld();
        world->configure(20, 20, 16, 16); // 20x20 world, 16x16 vision
        py::auto agent = pynlm.createAgentBrain(brain);
        agent->initialize(*world);
        
        // Training loop
        py::print("4. Running training episodes...");
        std::vector<float> rewards;
        
        for (int episode = 0; episode < 5; ++episode) {
            py::print("   Episode " + std::to_string(episode) + ":");
            agent->reset();
            
            float episode_reward = 0.0f;
            for (int step = 0; step < 50; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(*percept);
                
                // Decode action
                auto action = agent->decodeMotorCommand();
                
                // Apply action and get result
                auto result = world->applyMotorCommand(action, world->getSimulationTime());
                world->update(0.001f);
                
                // Apply reward modulation
                agent->applyRewardModulation(result.reward, 0.0f);
                
                episode_reward += result.reward;
                
                if (step % 10 == 0) {
                    py::print("     Step " + std::to_string(step) + ": Reward = " + std::to_string(result.reward));
                }
            }
            
            rewards.push_back(episode_reward);
            py::print("   Total reward: " + std::to_string(episode_reward));
        }
        
        // Show results
        py::print("");
        py::print("5. Training Results:");
        float avg_reward = 0.0f;
        for (size_t i = 0; i < rewards.size(); ++i) {
            py::print("   Episode " + std::to_string(i) + ": " + std::to_string(rewards[i]));
            avg_reward += rewards[i];
        }
        avg_reward /= rewards.size();
        py::print("   Average reward: " + std::to_string(avg_reward));
        
        // Clean up
        py::print("6. Cleaning up...");
        brain->reset();
        world->reset();
        
        py::print("");
        py::print("=== Advanced Training Complete ===");
        return "Advanced training example completed";
    }, "Run advanced training example with complete agent lifecycle");

    // Example: Visualization utilities
    m.def("example_visualization", []() {
        py::print("=== Visualization Example ===");
        py::print("");
        
        // Create brain
        py::auto config = pynlm.createDefaultConfig();
        config->set("brain.num_neurons", 200);
        py::auto brain = pynlm.createBrain(config);
        brain->initialize();
        
        // Create world
        py::auto world = pynlm.createSimpleWorld();
        world->configure(10, 10, 8, 8);
        
        // Create visualizer
        py::auto visualizer = pynlm.createVisualizer(brain, world);
        
        py::print("1. Brain state visualization:");
        py::print(visualizer->get_brain_text_view());
        
        py::print("2. World state visualization:");
        py::print(visualizer->get_world_text_view());
        
        py::print("3. Combined visualization:");
        py::print(visualizer->get_combined_view());
        
        py::print("");
        py::print("=== Visualization Example Complete ===");
        return "Visualization example completed";
    }, "Run visualization example showing brain and world state monitoring");

    // Example: Memory management
    m.def("example_memory_management", []() {
        py::print("=== Memory Management Example ===");
        py::print("");
        
        // Create brain
        py::auto config = pynlm.createDefaultConfig();
        config->set("brain.num_neurons", 300);
        py::auto brain = pynlm.createBrain(config);
        brain->initialize();
        
        // Create memory manager
        py::auto memory_manager = pynlm.createMemoryManager(brain);
        
        py::print("1. Initial memory stats:");
        auto initial_stats = memory_manager->get_memory_stats();
        for (const auto& pair : initial_stats) {
            py::print("   " + pair.first + ": " + std::to_string(pair.second));
        }
        
        // Run some simulation
        py::print("2. Running simulation...");
        for (int i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        // Check memory stats again
        py::print("3. Memory stats after simulation:");
        auto final_stats = memory_manager->get_memory_stats();
        for (const auto& pair : final_stats) {
            py::print("   " + pair.first + ": " + std::to_string(pair.second));
        }
        
        // Check if memory warning
        if (memory_manager->is_memory_warning()) {
            py::print("   WARNING: Memory usage is high!");
        }
        
        // Try save/load
        py::print("4. Testing save/load...");
        const std::string test_file = "test_brain_state.bin";
        if (memory_manager->save_state(test_file)) {
            py::print("   Successfully saved brain state to " + test_file);
            
            // Reset and load
            brain->reset();
            if (memory_manager->load_state(test_file)) {
                py::print("   Successfully loaded brain state from " + test_file);
            } else {
                py::print("   Failed to load brain state");
            }
            
            // Clean up
            std::remove(test_file.c_str());
        } else {
            py::print("   Failed to save brain state");
        }
        
        py::print("");
        py::print("=== Memory Management Example Complete ===");
        return "Memory management example completed";
    }, "Run memory management example showing save/load and statistics");

    //====================================================================
    // ERROR HANDLING
    //====================================================================

    // Custom exception for NLM errors
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

// End of namespace
} // namespace nlm