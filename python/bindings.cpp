#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
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
#include "../src/core/Error/Error.hpp"

namespace py = pybind11;
namespace nlm {

// Python-style exception classes
class NLMRuntimeError : public std::runtime_error {
public:
    NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public NLMRuntimeError {
public:
    NLMConfigurationError(const std::string& msg) : NLMRuntimeError("Configuration Error: " + msg) {}
};

class NLMBrainError : public NLMRuntimeError {
public:
    NLMBrainError(const std::string& msg) : NLMRuntimeError("Brain Error: " + msg) {}
};

class NLMNeutronError : public NLMRuntimeError {
public:
    NLMNeutronError(const std::string& msg) : NLMRuntimeError("Neutron Error: " + msg) {}
};

class NLMSynapseError : public NLMRuntimeError {
public:
    NLMSynapseError(const std::string& msg) : NLMRuntimeError("Synapse Error: " + msg) {}
};

// Error conversion functions
def convertToPythonError(const nlm::NLMException& e) {
    std::string categoryStr;
    switch (e.getCategory()) {
        case nlm::ErrorCategory::Configuration: categoryStr = "Configuration"; break;
        case nlm::ErrorCategory::System: categoryStr = "System"; break;
        case nlm::ErrorCategory::Neutron: categoryStr = "Neutron"; break;
        case nlm::ErrorCategory::Synapse: categoryStr = "Synapse"; break;
        case nlm::ErrorCategory::Memory: categoryStr = "Memory"; break;
        default: categoryStr = "Unknown";
    }
    
    std::string severityStr;
    switch (e.getSeverity()) {
        case nlm::ErrorSeverity::Debug: severityStr = "Debug"; break;
        case nlm::ErrorSeverity::Info: severityStr = "Info"; break;
        case nlm::ErrorSeverity::Warning: severityStr = "Warning"; break;
        case nlm::ErrorSeverity::Error: severityStr = "Error"; break;
        case nlm::ErrorSeverity::Critical: severityStr = "Critical"; break;
        default: severityStr = "Unknown";
    }
    
    return NLMRuntimeError(
        "[" + categoryStr + ":" + severityStr + "] " + e.what()
    );
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides comprehensive classes with Python-style API for neural simulation,
        batch processing, and ecosystem integration.
        
        Features:
        - Enhanced Brain API with Python-style method names
        - Batch processing for high-performance neural operations
        - Python-friendly data structures (lists, dicts instead of vectors)
        - Comprehensive error handling with Python-style exceptions
        - Automatic memory management and reference counting
        - Integration with Python data science ecosystem
    )pbdoc";

    // Register Python-style exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm::NLMException& e) {
            py::set_error(convertToPythonError(e));
        } catch (const std::runtime_error& e) {
            py::set_error(py::register_exception<std::runtime_error>(m, "RuntimeError"));
        } catch (...) {
            py::set_error(py::register_exception<std::runtime_error>(m, "RuntimeError"));
        }
    });

    py::register_exception<NLMRuntimeError>(m, "NLMError");
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError");
    py::register_exception<NLMBrainError>(m, "NLMBrainError");
    py::register_exception<NLMNeutronError>(m, "NLMNeutronError");
    py::register_exception<NLMSynapseError>(m, "NLMSynapseError");

    // Enhanced ID type bindings with Python-friendly methods
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
        .def("to_dict", [](const NeuronId& id) {
            return py::dict("value"_a = id.value, "index"_a = id.index());
        }, "Convert NeuronId to Python dictionary")
        .def_static("from_dict", [](py::dict d) {
            return NeuronId(d["value"].cast<uint64_t>());
        }, py::arg("data"), "Create NeuronId from Python dictionary")
        .def("is_valid", [](const NeuronId& id) {
            return id.value != UINT64_MAX;
        }, "Check if NeuronId is valid (not INVALID_NEURON_ID)");

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
        })
        .def("to_dict", [](const SynapseId& id) {
            return py::dict("value"_a = id.value, "index"_a = id.index());
        }, "Convert SynapseId to Python dictionary")
        .def_static("from_dict", [](py::dict d) {
            return SynapseId(d["value"].cast<uint64_t>());
        }, py::arg("data"), "Create SynapseId from Python dictionary")
        .def("is_valid", [](const SynapseId& id) {
            return id.value != UINT64_MAX;
        }, "Check if SynapseId is valid (not INVALID_SYNAPSE_ID)");

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
        })
        .def("to_dict", [](const RegionId& id) {
            return py::dict("value"_a = id.value, "index"_a = id.index());
        }, "Convert RegionId to Python dictionary")
        .def_static("from_dict", [](py::dict d) {
            return RegionId(d["value"].cast<uint64_t>());
        }, py::arg("data"), "Create RegionId from Python dictionary")
        .def("is_valid", [](const RegionId& id) {
            return id.value != UINT64_MAX;
        }, "Check if RegionId is valid (not INVALID_REGION_ID)");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("to_dict", [](const PopulationId& id) {
            return py::dict("value"_a = id.value, "index"_a = id.index());
        }, "Convert PopulationId to Python dictionary")
        .def_static("from_dict", [](py::dict d) {
            return PopulationId(d["value"].cast<uint64_t>());
        }, py::arg("data"), "Create PopulationId from Python dictionary")
        .def("is_valid", [](const PopulationId& id) {
            return id.value != UINT64_MAX;
        }, "Check if PopulationId is valid (not INVALID_POPULATION_ID)");

    // Enhanced enum bindings with Python-friendly attributes
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_excitatory", [](NeuronType t) { return t == NeuronType::Excitatory; })
        .def("is_inhibitory", [](NeuronType t) { return t == NeuronType::Inhibitory; })
        .def("is_sensory", [](NeuronType t) { return t == NeuronType::Sensory; })
        .def("is_motor", [](NeuronType t) { return t == NeuronType::Motor; });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_excitatory", [](SynapseType t) { return t == SynapseType::Excitatory; })
        .def("is_inhibitory", [](SynapseType t) { return t == SynapseType::Inhibitory; });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_developmental", [](DevelopmentalStage s) { 
            return s == DevelopmentalStage::CriticalPeriod || 
                   s == DevelopmentalStage::Maturation || 
                   s == DevelopmentalStage::Adult; 
        })
        .def("is_mature", [](DevelopmentalStage s) { 
            return s == DevelopmentalStage::Adult || s == DevelopmentalStage::Aging; 
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("is_active", [](FiringState s) { return s == FiringState::Active; })
        .def("is_refractory", [](FiringState s) { return s == FiringState::Refractory; });

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
        .export_values()
        .def("is_moving", [](ActionType a) { 
            return a == ActionType::MoveForward || 
                   a == ActionType::MoveBackward || 
                   a == ActionType::MoveLeft || 
                   a == ActionType::MoveRight; 
        })
        .def("is_turning", [](ActionType a) { 
            return a == ActionType::TurnLeft || a == ActionType::TurnRight; 
        });

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
        .export_values()
        .def("is_passable", [](WorldObjectType t) { 
            return t != WorldObjectType::Wall; 
        });

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
        });

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

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Enhanced methods for Python API
        .def("create_region", [](Brain& self, const std::string& name = "", size_t neuron_count = 100, NeuronType type = NeuronType::Internal) {
            auto region_id = self.addRegion(name);
            auto region = self.getRegion(region_id);
            if (region) {
                region->addPopulation(neuron_count, type);
            }
            return region_id;
        }, py::arg("name") = "", py::arg("neuron_count") = 100, py::arg("type") = NeuronType::Internal,
             "Create a new region with specified number of neurons")
        .def("get_neurons", &Brain::getRegion, py::return_value_policy::reference_internal,
             "Get region (alias for get_region)")
        .def("batch_inject_current", [](Brain& self, NeuronType neuron_type, double current, size_t step) {
            self.injectCurrentToNeurons(neuron_type, current);
            return self.getActiveNeuronCount();
        }, py::arg("neuron_type"), py::arg("current"), py::arg("step") = 0,
             "Inject current to all neurons of a type and return active count")
        .def("get_region_statistics", [](const Brain& self) {
            std::map<std::string, py::object> stats;
            stats["region_count"] = self.getRegionCount();
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neutrons"] = self.getFiringNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            return stats;
        }, "Get comprehensive region statistics as Python dictionary")
        .def("validate", [](Brain& self) {
            bool valid = true;
            std::string error_msg;
            
            if (self.getRegionCount() == 0) {
                valid = false;
                error_msg += "No regions created. ";
            }
            
            if (self.getTotalNeuronCount() == 0) {
                valid = false;
                error_msg += "No neurons created. ";
            }
            
            return py::make_tuple(valid, error_msg);
        }, "Validate brain state and return (is_valid, error_message)");

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

    // Result processing and formatting utilities
    m.def("format_neuron_statistics", [](const std::vector<std::pair<NeuronId, double>>& neuron_data) {
        py::dict result;
        for (const auto& pair : neuron_data) {
            result[std::to_string(pair.first.value)] = pair.second;
        }
        return result;
    }, py::arg("neuron_data"), 
         "Format neuron statistics as Python dictionary mapping neuron ID to value");

    m.def("format_spike_times", [](const std::vector<std::pair<SimulationStep, NeuronId>>& spike_events) {
        py::list result;
        for (const auto& pair : spike_events) {
            result.append(py::make_tuple(pair.first, std::to_string(pair.second.value)));
        }
        return result;
    }, py::arg("spike_events"), 
         "Format spike events as Python list of (step, neuron_id) tuples");

    m.def("neuron_id_to_hex", [](const NeuronId& id) {
        std::stringstream ss;
        ss << "0x" << std::hex << id.value;
        return ss.str();
    }, py::arg("neuron_id"), 
         "Convert NeuronId to hexadecimal string representation");

    m.def("neuron_id_to_string", [](const NeuronId& id) {
        return std::to_string(id.value);
    }, py::arg("neuron_id"), 
         "Convert NeuronId to decimal string representation");

    m.def("region_id_from_name", [](const Brain& brain, const std::string& name) {
        const auto& regions = brain.getRegions();
        for (const auto& region : regions) {
            if (region->getName() == name) {
                return region->getId();
            }
        }
        throw NLMConfigurationError("Region with name '" + name + "' not found");
    }, py::arg("brain"), py::arg("name"), 
         "Find region ID by name, raises NLMConfigurationError if not found");

    m.def("get_region_names", [](const Brain& brain) {
        py::list names;
        const auto& regions = brain.getRegions();
        for (const auto& region : regions) {
            names.append(region->getName());
        }
        return names;
    }, py::arg("brain"), 
         "Get list of all region names");

    // Progress reporting for long-running operations
    m.def("run_simulation_with_progress", [](Brain& brain, size_t max_steps, 
                                             std::function<void(size_t, size_t)> progress_callback,
                                             std::function<void()> completion_callback) {
        for (size_t step = 0; step < max_steps; ++step) {
            brain.step(step);
            progress_callback(step, max_steps);
        }
        if (completion_callback) {
            completion_callback();
        }
    }, py::arg("brain"), py::arg("max_steps"), 
        py::arg("progress_callback"), py::arg("completion_callback"),
        "Run simulation with progress callbacks");

    // Memory management utilities
    m.def("get_memory_usage", []() {
        py::dict memory_info;
        // In practice, this would use system-specific memory reporting
        // For now, return basic information
        memory_info["allocated_mb"] = 0.0;
        memory_info["peak_mb"] = 0.0;
        memory_info["neuron_memory_mb"] = 0.0;
        memory_info["synapse_memory_mb"] = 0.0;
        return memory_info;
    }, "Get memory usage information as Python dictionary");

    m.def("estimate_neuron_memory", [](size_t neuron_count) {
        // Estimate memory required for specified number of neurons
        // Base estimate: ~1KB per neuron
        return neuron_count * 1024.0 / (1024.0 * 1024.0); // Convert to MB
    }, py::arg("neuron_count"), 
         "Estimate memory required for specified number of neurons (MB)");

    // Statistical utilities
    m.def("compute_neuron_statistics", [](const std::vector<double>& membrane_potentials) {
        py::dict stats;
        if (membrane_potentials.empty()) {
            return stats;
        }
        
        double sum = 0.0, sum_sq = 0.0, min_val = membrane_potentials[0], max_val = membrane_potentials[0];
        for (double value : membrane_potentials) {
            sum += value;
            sum_sq += value * value;
            if (value < min_val) min_val = value;
            if (value > max_val) max_val = value;
        }
        
        size_t count = membrane_potentials.size();
        double mean = sum / count;
        double variance = (sum_sq / count) - (mean * mean);
        double stddev = std::sqrt(std::max(0.0, variance));
        
        stats["count"] = count;
        stats["mean"] = mean;
        stats["min"] = min_val;
        stats["max"] = max_val;
        stats["stddev"] = stddev;
        stats["variance"] = variance;
        
        return stats;
    }, py::arg("membrane_potentials"), 
         "Compute statistical measures for membrane potentials");

    m.def("normalize_array", [](const std::vector<double>& data, double min_val, double max_val) {
        std::vector<double> result;
        result.reserve(data.size());
        
        if (max_val == min_val) {
            // All values are the same
            for (double value : data) {
                result.push_back(0.5);
            }
        } else {
            for (double value : data) {
                result.push_back((value - min_val) / (max_val - min_val));
            }
        }
        
        return result;
    }, py::arg("data"), py::arg("min_val"), py::arg("max_val"), 
         "Normalize array values to range [0, 1]");

    // Time utilities
    m.def("format_time", [](double seconds) {
        double hours = std::floor(seconds / 3600.0);
        double minutes = std::floor((seconds - hours * 3600.0) / 60.0);
        double secs = seconds - hours * 3600.0 - minutes * 60.0;
        
        std::stringstream ss;
        if (hours > 0) ss << hours << "h ";
        if (minutes > 0) ss << minutes << "m ";
        ss << std::fixed << std::setprecision(2) << secs << "s";
        return ss.str();
    }, py::arg("seconds"), 
         "Format time in human-readable format (Hh Mm S.SSs)");

    // Configuration validation and utilities
    m.def("validate_brain_config", [](const std::shared_ptr<Config>& config) {
        py::dict results;
        std::vector<std::string> warnings;
        
        // Check for required configuration sections
        if (!config->has("brain.regions")) {
            warnings.push_back("Missing 'brain.regions' configuration");
        }
        
        if (!config->has("neuron.count")) {
            warnings.push_back("Missing 'neuron.count' configuration");
        }
        
        if (!config->has("synapse.connection_probability")) {
            warnings.push_back("Missing 'synapse.connection_probability' configuration");
        }
        
        results["valid"] = warnings.empty();
        results["warnings"] = warnings;
        
        // Get configuration values for validation
        if (config->has("neuron.count")) {
            auto neuron_count = config->get<size_t>("neuron.count");
            results["neuron_count"] = neuron_count;
            if (neuron_count == 0) {
                warnings.push_back("Neuron count is zero - no neural activity expected");
            }
        }
        
        if (config->has("synapse.connection_probability")) {
            auto conn_prob = config->get<float>("synapse.connection_probability");
            results["connection_probability"] = conn_prob;
            if (conn_prob < 0.0 || conn_prob > 1.0) {
                warnings.push_back("Connection probability must be between 0.0 and 1.0");
            }
        }
        
        return results;
    }, py::arg("config"), 
         "Validate brain configuration and return validation results");

    // Data export utilities
    m.def("export_brain_to_csv", [](const Brain& brain, const std::string& filepath) {
        // Export brain data to CSV format for analysis
        // This is a simplified implementation
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw NLMBrainError("Cannot open file for writing: " + filepath);
        }
        
        file << "NeuronId,RegionId,NeuronType,MembranePotential,FiringState\n";
        
        const auto& regions = brain.getRegions();
        for (const auto& region : regions) {
            const auto& populations = region->getPopulations();n            for (const auto& population : populations) {
                // In practice, you would iterate over actual neurons
                // For now, this is a template implementation
                file << "0," << region->getId().value << "," << static_cast<int>(population->getNeuronType()) << ",0.0,0\n";
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), 
         "Export brain data to CSV file");

    m.def("export_region_to_json", [](const Brain& brain, const std::string& region_name) {
        const auto& regions = brain.getRegions();
        for (const auto& region : regions) {
            if (region->getName() == region_name) {
                // Export region data to JSON format
                py::dict region_data;
                region_data["name"] = region->getName();
                region_data["id"] = region->getId().value;
                region_data["neuron_count"] = region->getTotalNeuronCount();
                region_data["synapse_count"] = region->getSynapseCount();
                region_data["average_firing_rate"] = region->getAverageFiringRate();
                return region_data;
            }
        }
        throw NLMConfigurationError("Region '" + region_name + "' not found");
    }, py::arg("brain"), py::arg("region_name"), 
         "Export region data to Python dictionary (JSON-like)");

} // namespace nlm
