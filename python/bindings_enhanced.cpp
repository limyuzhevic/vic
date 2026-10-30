// Enhanced Python Bindings for NLM
// Provides improved API with type aliases, property access, and better documentation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

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

// Type Aliases for Common Python Usage
typedef NeuronId NeuronID;
typedef SynapseId SynapseID;
typedef RegionId RegionID;
typedef PopulationId PopulationID;
typedef SimulationStep Step;
typedef Timestamp Time;
typedef MembranePotential Potential;
typedef FiringRate Rate;

// Property access helper functions

// NeuronId property access
static NeuronID neuron_id_from_value(uint64_t val) { return NeuronID(val); }
static std::string neuron_id_repr(const NeuronID& id) {
    return "<NeuronID: " + std::to_string(id.value) + ">";
}

// SynapseId property access
static SynapseID synapse_id_from_value(uint64_t val) { return SynapseID(val); }
static std::string synapse_id_repr(const SynapseID& id) {
    return "<SynapseID: " + std::to_string(id.value) + ">";
}

// RegionId property access
static RegionID region_id_from_value(uint64_t val) { return RegionID(val); }
static std::string region_id_repr(const RegionID& id) {
    return "<RegionID: " + std::to_string(id.value) + ">";
}

// PopulationId property access
static PopulationID population_id_from_value(uint64_t val) { return PopulationID(val); }

// Property access for Neuron class
typical neuron_properties(py::class_<Neuron>& cls) {
    cls
        .def_property_readonly("id", &Neuron::getId)
        .def_property_readonly("type", &Neuron::getType)
        .def_property("membrane_potential", &Neuron::getMembranePotential, &Neuron::setMembranePotential)
        .def_property("threshold", &Neuron::getThreshold, &Neuron::setThreshold)
        .def_property("firing_rate", &Neuron::getFiringRate, &Neuron::setFiringRate)
        .def_property_readonly("is_firing", &Neuron::isFiring)
        .def_property_readonly("is_refractory", &Neuron::isRefractory)
        .def_property("refractory_period", &Neuron::getRefractoryPeriod, &Neuron::setRefractoryPeriod)
        .def_property_readonly("last_spike_time", &Neuron::getLastSpikeTime)
        .def_property_readonly("total_current", &Neuron::getTotalCurrent);
}

// Property access for Config class
typical config_properties(py::class_<Config>& cls) {
    cls
        .def("get_value", &Config::get<std::string>)
        .def("set_value", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        })
        .def("get_int", &Config::getOr<int>)
        .def("get_double", &Config::getOr<double>)
        .def("get_bool", &Config::getOr<bool>)
        .def("get_string", &Config::getOr<std::string>)
        .def("has_key", &Config::has)
        .def("remove_key", &Config::remove)
        .def_property_readonly("keys", &Config::getKeys)
        .def("summary", &Config::summary);
}

// Property access for Action class
typical action_properties(py::class_<Action>& cls) {
    cls
        .def_property_readonly("type", &Action::getType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone);
}

// Property access for WorldObject class
typical world_object_properties(py::class_<WorldObject>& cls) {
    cls
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);
}

// Property access for AgentBody class
typical agent_body_properties(py::class_<AgentBody>& cls) {
    cls
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);
}

// Property access for SensoryPercept class
typical sensory_percept_properties(py::class_<SensoryPercept>& cls) {
    cls
        .def("get_vision", &SensoryPercept::getVision)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("get_internal", &SensoryPercept::getInternal)
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("get_audio", &SensoryPercept::getAudio)
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp);
}

PYBIND11_MODULE(pynlm, m) {
    // Enhanced module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        Enhanced Python API for the NLM C++ neural simulation framework.
        
        Key Features:
        - Type aliases for common identifiers (NeuronID, SynapseID, etc.)
        - Property-based API for better Python integration
        - Comprehensive error handling and validation
        - Enhanced documentation and examples
        - Context managers for resource management
        
        Classes exposed:
        - Brain: Central neural system with all integrated components
        - Config: Configuration management with multiple value types
        - AgentBrain: Bridge between brain and world
        - SimpleWorld: 2D simulation environment
        - SensoryInput: Vision, Audio, and Internal signals
        - Action: Motor outputs and behaviors
        - WorldObject: Objects in the simulation world
        - AgentBody: Agent state and properties
        - SensoryPercept: Integrated sensory observations
        
        Enumerations:
        - NeuronType: Cell types (excitatory, inhibitory, modulatory, etc.)
        - SynapseType: Connection types (excitatory, inhibitory, electrical, etc.)
        - DevelopmentalStage: Brain development stages
        - FiringState: Neuron activation states
        - ActionType: Possible actions (move, turn, interact, etc.)
        - WorldObjectType: Object categories (resource, hazard, wall, etc.)
        
        Core Functions:
        - createDefaultConfig(): Create default configuration
        - createBrain(config): Create a new neural brain
        - createSimpleWorld(): Create a simple 2D world
        - createAgentBrain(brain): Create agent interface
        
        Constants:
        - INVALID_* constants for invalid identifiers
        
        Usage Examples:
        ```python
        import pynlm
        
        # Create a brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(1000):
            brain.step(i)
            
        # Check statistics
        print(f"Neurons: {brain.getTotalNeuronCount()}")
        print(f"Firing: {brain.getFiringNeuronCount()}")
        ```
        
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register type aliases
    m.attr("NeuronID") = py::type<&NeuronID>();
    m.attr("SynapseID") = py::type<&SynapseID>();
    m.attr("RegionID") = py::type<&RegionID>();
    m.attr("PopulationID") = py::type<&PopulationID>();
    m.attr("Step") = py::type<&Step>();
    m.attr("Time") = py::type<&Time>();
    m.attr("Potential") = py::type<&Potential>();
    m.attr("Rate") = py::type<&Rate>();

    // NeuronId class with property access
    py::class_<NeuronID>(m, "NeuronID", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronID::value)
        .def("index", &NeuronID::index)
        .def("__eq__", &NeuronID::operator==)
        .def("__ne__", &NeuronID::operator!=)
        .def("__hash__", [](const NeuronID& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", neuron_id_repr);

    // SynapseId class with property access
    py::class_<SynapseID>(m, "SynapseID", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseID::value)
        .def("index", &SynapseID::index)
        .def("__eq__", &SynapseID::operator==)
        .def("__ne__", &SynapseID::operator!=)
        .def("__hash__", [](const SynapseID& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", synapse_id_repr);

    // RegionId class with property access
    py::class_<RegionID>(m, "RegionID", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionID::value)
        .def("index", &RegionID::index)
        .def("__eq__", &RegionID::operator==)
        .def("__ne__", &RegionID::operator!=)
        .def("__hash__", [](const RegionID& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", region_id_repr);

    // PopulationId class with property access
    py::class_<PopulationID>(m, "PopulationID", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationID::value)
        .def("index", &PopulationID::index)
        .def("__eq__", &PopulationID::operator==)
        .def("__ne__", &PopulationID::operator!=);

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

    // Config class with enhanced property access
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced property access for Config
    config_properties(py::class_<Config>(m, "Config"));

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with enhanced properties
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def_property_readonly("width", &Vision::getWidth)
        .def_property_readonly("height", &Vision::getHeight)
        .def_property_readonly("channels", &Vision::getChannels);

    // Audio class with enhanced properties
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    // InternalSignals class with enhanced properties
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);

    // Action class with enhanced properties
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_property_readonly("type", &Action::getType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone);

    // Enhanced property access for Action
    action_properties(py::class_<Action>(m, "Action"));

    // WorldObject class with enhanced properties
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f);

    // Enhanced property access for WorldObject
    world_object_properties(py::class_<WorldObject>(m, "WorldObject"));

    // AgentBody class with enhanced properties
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>());

    // Enhanced property access for AgentBody
    agent_body_properties(py::class_<AgentBody>(m, "AgentBody"));

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class with enhanced properties
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Enhanced property access for SensoryPercept
    sensory_percept_properties(py::class_<SensoryPercept>(m, "SensoryPercept"));

    // SimpleWorld class with enhanced properties
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed);

    // Brain class with enhanced property access
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
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
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
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status");

    // AgentBrain class with enhanced properties
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled);

    // Core functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced Python utilities
    m.def("create_neuron_id", &neuron_id_from_value,
          "Create a NeuronID from value");
    m.def("create_synapse_id", &synapse_id_from_value,
          "Create a SynapseID from value");
    m.def("create_region_id", &region_id_from_value,
          "Create a RegionID from value");
    m.def("create_population_id", &population_id_from_value,
          "Create a PopulationID from value");
}

} // namespace nlm
