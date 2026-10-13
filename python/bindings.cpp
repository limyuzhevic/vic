#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Helper Function 1: Template ID Type Binder
// Module: Core Types / Integration Utilities
template<typename IDType>
void bindIDType(py::module& m, const std::string& name, const std::string& doc) {
    py::class_<IDType>(m, name.c_str(), doc.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [name](const IDType& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

// Helper Function 2: Template Enum Binder
// Module: Core Types / Integration Utilities
template<typename EnumType>
void bindEnum(py::module& m, const std::string& name, const std::string& doc,
             const std::map<std::string, EnumType>& enumValues) {
    py::enum_<EnumType> enumClass(m, name.c_str(), doc.c_str());
    
    for (const auto& pair : enumValues) {
        enumClass.value(pair.first.c_str(), pair.second);
    }
    enumClass.export_values();
}

// Helper Function 3: Template Sensory Input Binder
// Module: Sensory Systems / Integration Utilities
template<typename SensoryType>
void bindSensoryInputType(py::module& m, const std::string& name, const std::string& doc) {
    py::class_<SensoryType, SensoryInput>(m, name.c_str(), doc.c_str())
        .def(py::init<>());
}

// Helper Function 4: Property Binding Helper
// Module: Integration Utilities
template<typename T>
void addPropertyBinding(py::class_<T>& cls, const std::string& pythonName, const std::string& cppName,
                       std::function<void(T&)>, std::function<void(const T&)>) {
    // This is a placeholder for property binding
    // In practice, you'd need more sophisticated reflection or explicit binding
}

// Module 1: Core Types
void bindCoreTypes(py::module& m) {
    m.doc() = R"pbdoc(
        Core Types Module
        -----------------
        This module contains fundamental types used throughout the NLM system,
        including neuron identifiers, population identifiers, and typed enumerations.
        These types provide strong typing for simulation entities.
    )pbdoc";

    // ID Types
    bindIDType<NeuronId>(m, "NeuronId", "Unique identifier for a neuron");
    bindIDType<SynapseId>(m, "SynapseId", "Unique identifier for a synapse");
    bindIDType<RegionId>(m, "RegionId", "Unique identifier for a brain region");
    bindIDType<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population");

    // Enums with value mappings
    std::map<std::string, NeuronType> neuronTypeValues = {
        {"Excitatory", NeuronType::Excitatory},
        {"Inhibitory", NeuronType::Inhibitory},
        {"Modulatory", NeuronType::Modulatory},
        {"Sensory", NeuronType::Sensory},
        {"Motor", NeuronType::Motor},
        {"Internal", NeuronType::Internal}
    };
    bindEnum<NeuronType>(m, "NeuronType", "Neuron type enumeration", neuronTypeValues);

    std::map<std::string, SynapseType> synapseTypeValues = {
        {"Excitatory", SynapseType::Excitatory},
        {"Inhibitory", SynapseType::Inhibitory},
        {"Modulatory", SynapseType::Modulatory},
        {"Electrical", SynapseType::Electrical},
        {"GapJunction", SynapseType::GapJunction}
    };
    bindEnum<SynapseType>(m, "SynapseType", "Synapse type enumeration", synapseTypeValues);

    std::map<std::string, DevelopmentalStage> developmentalStageValues = {
        {"Initial", DevelopmentalStage::Initial},
        {"CriticalPeriod", DevelopmentalStage::CriticalPeriod},
        {"Maturation", DevelopmentalStage::Maturation},
        {"Adult", DevelopmentalStage::Adult},
        {"Aging", DevelopmentalStage::Aging}
    };
    bindEnum<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration", developmentalStageValues);

    std::map<std::string, FiringState> firingStateValues = {
        {"Resting", FiringState::Resting},
        {"Active", FiringState::Active},
        {"Refractory", FiringState::Refractory},
        {"Inhibited", FiringState::Inhibited}
    };
    bindEnum<FiringState>(m, "FiringState", "Neuron firing state enumeration", firingStateValues);

    std::map<std::string, ActionType> actionTypeValues = {
        {"MoveForward", ActionType::MoveForward},
        {"MoveBackward", ActionType::MoveBackward},
        {"MoveLeft", ActionType::MoveLeft},
        {"MoveRight", ActionType::MoveRight},
        {"TurnLeft", ActionType::TurnLeft},
        {"TurnRight", ActionType::TurnRight},
        {"Look", ActionType::Look},
        {"LookUp", ActionType::LookUp},
        {"LookDown", ActionType::LookDown},
        {"Interact", ActionType::Interact},
        {"Eat", ActionType::Eat},
        {"Drink", ActionType::Drink},
        {"Rest", ActionType::Rest},
        {"Wait", ActionType::Wait},
        {"Custom", ActionType::Custom}
    };
    bindEnum<ActionType>(m, "ActionType", "Action type enumeration", actionTypeValues);

    std::map<std::string, MotorCommand> motorCommandValues = {
        {"MoveForward", MotorCommand::MoveForward},
        {"MoveBackward", MotorCommand::MoveBackward},
        {"TurnLeft", MotorCommand::TurnLeft},
        {"TurnRight", MotorCommand::TurnRight},
        {"LookLeft", MotorCommand::LookLeft},
        {"LookRight", MotorCommand::LookRight},
        {"Interact", MotorCommand::Interact},
        {"Wait", MotorCommand::Wait}
    };
    bindEnum<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration", motorCommandValues);

    std::map<std::string, WorldObjectType> worldObjectTypeValues = {
        {"Empty", WorldObjectType::Empty},
        {"Resource", WorldObjectType::Resource},
        {"Hazard", WorldObjectType::Hazard},
        {"Wall", WorldObjectType::Wall},
        {"Marker", WorldObjectType::Marker}
    };
    bindEnum<WorldObjectType>(m, "WorldObjectType", "World object type enumeration", worldObjectTypeValues);
}

// Module 2: Configuration
void bindConfiguration(py::module& m) {
    m.doc() = R"pbdoc(
        Configuration Module
        ------------------
        This module provides configuration functionality for the NLM system.
        The Configuration class manages system settings, parameters, and
        simulation control options through a key-value store.
    )pbdoc";

    py::class_<Config>(m, "Config", "Configuration class for NLM system")
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
        // Python properties
        .def_property_readonly("path", &Config::getPath, "Current config file path (read-only)")
        .def_property_readonly("loaded", &Config::hasLoadedData, "Whether config has loaded data (read-only)")
        .def_property_readonly("entries", &Config::getEntryCount, "Number of loaded entries (read-only)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
}

// Module 3: Sensory Systems
void bindSensorySystems(py::module& m) {
    m.doc() = R"pbdoc(
        Sensory Systems Module
        --------------------
        This module provides sensory input functionality for the NLM system.
        Sensory inputs allow the brain to perceive and interact with the simulated environment.
    )pbdoc";

    // Base class
    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Specific sensory types
    bindSensoryInputType<Vision>(m, "Vision", "Vision sensory input");
    bindSensoryInputType<Audio>(m, "Audio", "Audio sensory input");
    bindSensoryInputType<InternalSignals>(m, "InternalSignals", "Internal signals sensory input");
}

// Module 4: Motor Systems
void bindMotorSystems(py::module& m) {
    m.doc() = R"pbdoc(
        Motor Systems Module
        ------------------
        This module provides action and motor command functionality for the NLM system.
        Motor systems translate brain activity into actions and commands.
    )pbdoc";

    py::class_<Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", "World object representation")
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

    // Action result
    py::class_<ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
}

// Module 5: Agent Systems
void bindAgentSystems(py::module& m) {
    m.doc() = R"pbdoc(
        Agent Systems Module
        ------------------
        This module provides agent-related functionality for the NLM system.
        Agent systems connect the neural brain to the simulated world,
        processing sensory inputs and producing motor outputs.
    )pbdoc";

    py::class_<AgentBody>(m, "AgentBody", "Agent body state")
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

    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
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
}

// Module 6: World Systems
void bindWorldSystems(py::module& m) {
    m.doc() = R"pbdoc(
        World Systems Module
        ------------------
        This module provides world simulation functionality for the NLM system.
        The world represents the simulated environment where agents exist and interact.
    )pbdoc";

    py::class_<SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
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
        // Python properties
        .def_property_readonly("is_paused", &SimpleWorld::isPaused, "World simulation state (read-only)")
        .def_property_readonly("tick_rate", &SimpleWorld::getTickRate, "Simulation timestep (read-only)")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + ", paused=" + std::to_string(world.isPaused()) + ">";
        });
}

// Module 7: Brain Systems
void bindBrainSystems(py::module& m) {
    m.doc() = R"pbdoc(
        Brain Systems Module
        ------------------
        This module provides the central neural simulation functionality for the NLM system.
        The Brain class implements the core neural network simulation with learning,
        plasticity, and development mechanisms.
    )pbdoc";

    py::class_<Brain>(m, "Brain", "Central neural simulation brain class")
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
              "Log brain status");
        
        // Python properties
        .def_property_readonly("is_running", &Brain::isInitialized, "Whether brain is initialized (read-only)")
        .def_property_readonly("step_count", &Brain::getStepCount, "Current step counter (read-only)")
        .def_property_readonly("is_active", &Brain::hasActiveNeurons, "Whether brain has active neurons (read-only)")
        .def_property_readonly("version", &Brain::getVersion, "Library version (read-only)")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + brain.getVersion() + ", step=" + std::to_string(brain.getStepCount()) + ", active=" + std::to_string(brain.hasActiveNeurons()) + ">";
        });
}

// Module 8: Integration Utilities
void bindIntegrationUtilities(py::module& m) {
    m.doc() = R"pbdoc(
        Integration Utilities Module
        ---------------------------
        This module contains factory functions and utility functions for the NLM system.
        These functions provide integration points between different components
        and handle common operations across the system.
    )pbdoc";

    // Error handling registration
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Error handling utilities
    m.def("registerErrorHandler", [](std::function<void(std::string)> handler) {
        // Custom error handler registration
    }, py::arg("handler"), "Register custom error handler");

    // System utilities
    m.def("getVersion", []() {
        return std::string("NLM v1.0.0");
    }, "Get NLM version string");

    // Constants and sentinel values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

PYBIND11_MODULE(pynlm, m) {
    // Call module binding functions
    bindCoreTypes(m);
    bindConfiguration(m);
    bindSensorySystems(m);
    bindMotorSystems(m);
    bindAgentSystems(m);
    bindWorldSystems(m);
    bindBrainSystems(m);
    bindIntegrationUtilities(m);

    // Module-level documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides modular bindings for core types, configuration, sensory systems,
        motor systems, agent systems, world systems, brain systems, and integration utilities.

        This binding has been refactored into logical modules for better maintainability,
        testability, and documentation. All existing functionality remains available.
    )pbdoc";

    // Error handling registration is now in bindIntegrationUtilities
}