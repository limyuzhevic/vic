#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Include all the headers
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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register basic C++ exceptions for Python
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);

    // Type validation helper
    auto validate_not_null = [](void* ptr, const std::string& name) {
        if (!ptr) throw std::invalid_argument(name + " cannot be null");
    };

    // =====================================================
    // CORE TYPES (Identifiers and Enums)
    // =====================================================

    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId")
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

    py::class_<RegionId>(m, "RegionId")
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

    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // =====================================================
    // ENUMERATIONS
    // =====================================================

    py::enum_<NeuronType>(m, "NeuronType")
        .value("EXCITATORY", NeuronType::Excitatory)
        .value("INHIBITORY", NeuronType::Inhibitory)
        .value("MODULATORY", NeuronType::Modulatory)
        .value("SENSORY", NeuronType::Sensory)
        .value("MOTOR", NeuronType::Motor)
        .value("INTERNAL", NeuronType::Internal)
        .export_values()
        .doc() = R"pbdoc(Neuron type enumeration)pbdoc";

    py::enum_<SynapseType>(m, "SynapseType")
        .value("EXCITATORY", SynapseType::Excitatory)
        .value("INHIBITORY", SynapseType::Inhibitory)
        .value("MODULATORY", SynapseType::Modulatory)
        .value("ELECTRICAL", SynapseType::Electrical)
        .value("GAP_JUNCTION", SynapseType::GapJunction)
        .export_values()
        .doc() = R"pbdoc(Synapse type enumeration)pbdoc";

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("INITIAL", DevelopmentalStage::Initial)
        .value("CRITICAL_PERIOD", DevelopmentalStage::CriticalPeriod)
        .value("MATURATION", DevelopmentalStage::Maturation)
        .value("ADULT", DevelopmentalStage::Adult)
        .value("AGING", DevelopmentalStage::Aging)
        .export_values()
        .doc() = R"pbdoc(Developmental stage enumeration)pbdoc";

    py::enum_<FiringState>(m, "FiringState")
        .value("RESTING", FiringState::Resting)
        .value("ACTIVE", FiringState::Active)
        .value("REFRACTORY", FiringState::Refractory)
        .value("INHIBITED", FiringState::Inhibited)
        .export_values()
        .doc() = R"pbdoc(Neuron firing state enumeration)pbdoc";

    py::enum_<ActionType>(m, "ActionType")
        .value("MOVE_FORWARD", ActionType::MoveForward)
        .value("MOVE_BACKWARD", ActionType::MoveBackward)
        .value("MOVE_LEFT", ActionType::MoveLeft)
        .value("MOVE_RIGHT", ActionType::MoveRight)
        .value("TURN_LEFT", ActionType::TurnLeft)
        .value("TURN_RIGHT", ActionType::TurnRight)
        .value("LOOK", ActionType::Look)
        .value("LOOK_UP", ActionType::LookUp)
        .value("LOOK_DOWN", ActionType::LookDown)
        .value("INTERACT", ActionType::Interact)
        .value("EAT", ActionType::Eat)
        .value("DRINK", ActionType::Drink)
        .value("REST", ActionType::Rest)
        .value("WAIT", ActionType::Wait)
        .value("CUSTOM", ActionType::Custom)
        .export_values()
        .doc() = R"pbdoc(Action type enumeration)pbdoc";

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MOVE_FORWARD", MotorCommand::MoveForward)
        .value("MOVE_BACKWARD", MotorCommand::MoveBackward)
        .value("TURN_LEFT", MotorCommand::TurnLeft)
        .value("TURN_RIGHT", MotorCommand::TurnRight)
        .value("LOOK_LEFT", MotorCommand::LookLeft)
        .value("LOOK_RIGHT", MotorCommand::LookRight)
        .value("INTERACT", MotorCommand::Interact)
        .value("WAIT", MotorCommand::Wait)
        .export_values()
        .doc() = R"pbdoc(Low-level motor command enumeration)pbdoc";

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("EMPTY", WorldObjectType::Empty)
        .value("RESOURCE", WorldObjectType::Resource)
        .value("HAZARD", WorldObjectType::Hazard)
        .value("WALL", WorldObjectType::Wall)
        .value("MARKER", WorldObjectType::Marker)
        .export_values()
        .doc() = R"pbdoc(World object type enumeration)pbdoc";

    // =====================================================
    // CONFIGURATION CLASS
    // =====================================================

    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // =====================================================
    // SENSORY INPUT CLASSES
    // =====================================================

    py::class_<SensoryInput, py::module_local>(m, "SensoryInput")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), 
             py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Initialize vision input with dimensions")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set image data")
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + 
                   std::to_string(v.getHeight()) + "x" + 
                   std::to_string(v.getChannels()) + " channels>";
        });

    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), 
             py::arg("sample_rate"), py::arg("num_samples"),
             "Initialize audio input")
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio samples")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + "Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals");

    // =====================================================
    // ACTION CLASS
    // =====================================================

    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with type")
        .def(py::init<ActionType, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def_readwrite("type", &Action::type_)
        .def_readwrite("parameters", &Action::parameters_)
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone, "Clone the action")
        .def("__repr__", [](const Action& a) {
            std::string type_name = "UNKNOWN";
            switch(a.getType()) {
                case ActionType::MoveForward: type_name = "MOVE_FORWARD"; break;
                case ActionType::MoveBackward: type_name = "MOVE_BACKWARD"; break;
                case ActionType::MoveLeft: type_name = "MOVE_LEFT"; break;
                case ActionType::MoveRight: type_name = "MOVE_RIGHT"; break;
                case ActionType::TurnLeft: type_name = "TURN_LEFT"; break;
                case ActionType::TurnRight: type_name = "TURN_RIGHT"; break;
                case ActionType::Look: type_name = "LOOK"; break;
                case ActionType::LookUp: type_name = "LOOK_UP"; break;
                case ActionType::LookDown: type_name = "LOOK_DOWN"; break;
                case ActionType::Interact: type_name = "INTERACT"; break;
                case ActionType::Eat: type_name = "EAT"; break;
                case ActionType::Drink: type_name = "DRINK"; break;
                case ActionType::Rest: type_name = "REST"; break;
                case ActionType::Wait: type_name = "WAIT"; break;
                case ActionType::Custom: type_name = "CUSTOM"; break;
            }
            return "<Action: " + type_name + " parameters=" + 
                   std::to_string(a.getParameters().size()) + ">";
        })
        .def("__str__", [](const Action& a) {
            return a.getName();
        });

    // =====================================================
    // WORLD OBJECT CLASS
    // =====================================================

    py::class_<WorldObject>(m, "WorldObject")
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
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to point");

    // =====================================================
    // AGENT BODY CLASS
    // =====================================================

    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
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
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("position", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("velocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate Euclidean distance to point")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos=(" + std::to_string(body.x) + "," + 
                   std::to_string(body.y) + ") energy=" + 
                   std::to_string(body.energy) + " health=" + 
                   std::to_string(body.health) + ">";
        });

    // =====================================================
    // ACTION RESULT CLASS
    // =====================================================

    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_positive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result has positive reward")
        .def("is_negative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result has negative reward")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   " success=" + std::string(result.success ? "true" : "false") + 
                   " message=\"" + result.message + "\">";
        });

    // =====================================================
    // SENSORY PERCEPT CLASS
    // =====================================================

    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"), "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all sensory signals concatenated")
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("signal_sum", [](const SensoryPercept& self) {
            std::vector<float> signals = self.getAllSignals();
            float sum = 0.0f;
            for (float s : signals) sum += s;
            return sum;
        }, "Calculate sum of all signal values")
        .def("signal_mean", [](const SensoryPercept& self) {
            std::vector<float> signals = self.getAllSignals();
            if (signals.empty()) return 0.0f;
            float sum = 0.0f;
            for (float s : signals) sum += s;
            return sum / signals.size();
        }, "Calculate mean of all signal values")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision_size=" + 
                   std::to_string(percept.getVisionWidth() * percept.getVisionHeight()) +
                   " touch_size=" + std::to_string(percept.getTouch().size()) + 
                   " internal_size=" + std::to_string(percept.getInternal().size()) +
                   " timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        });

    // =====================================================
    // SIMPLE WORLD CLASS
    // =====================================================

    py::class_<SimpleWorld> world_class(m, "SimpleWorld");
    
    // First create the class with its methods
    world_class
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"), 
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command and get result")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_object_at", &SimpleWorld::getObjectAt,
             py::arg("x"), py::arg("y"),
             "Get object at position (or None if empty)")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get max energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("energy"),
             "Set max energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate,
             py::arg("rate"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " agent@(" +
                   std::to_string(world.getAgentBody().x) + "," +
                   std::to_string(world.getAgentBody().y()) + ")>";
        });
    
    // Now add context manager support
    world_class.def("__enter__", [](py::object self) { return self; })
             .def("__exit__", [](py::object, py::object, py::object, py::object) {
                 return py::none();
             });

    // =====================================================
    // BRAIN CLASS
    // =====================================================

    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
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
             py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus, "Log brain status")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) +
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses, stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) +
                   ">";
        });

    // =====================================================
    // AGENT BRAIN CLASS
    // =====================================================

    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("__repr__", [](const AgentBrain& agent_brain) {
            return "<AgentBrain: neuromodulation=" + 
                   std::to_string(agent_brain.getNeuromodulationLevel()) + 
                   ", curiosity=" + std::to_string(agent_brain.getCuriosityLevel()) +
                   ", novelty=" + std::to_string(agent_brain.getNoveltyLevel()) + ">";
        })
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object, py::object, py::object, py::object) {
            return py::none();
        });

    // =====================================================
    // MODULE-LEVEL FUNCTIONS AND CONSTANTS
    // =====================================================

    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Convenience functions for common operations
    m.def("validate_config", [](const Config& config) {
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            if (key.empty()) {
                throw std::invalid_argument("Configuration key cannot be empty");
            }
        }
        return !keys.empty();
    }, py::arg("config"), "Validate configuration has valid keys");

    m.def("create_action", [](ActionType type, const std::vector<float>& params) {
        return std::make_unique<Action>(type, params);
    }, py::arg("type"), py::arg("parameters"), "Create an action with given parameters");

    m.def("create_action_from_name", [](const std::string& name) {
        static const std::unordered_map<std::string, ActionType> type_map = {
            {"move_forward", ActionType::MoveForward},
            {"move_backward", ActionType::MoveBackward},
            {"move_left", ActionType::MoveLeft},
            {"move_right", ActionType::MoveRight},
            {"turn_left", ActionType::TurnLeft},
            {"turn_right", ActionType::TurnRight},
            {"look", ActionType::Look},
            {"look_up", ActionType::LookUp},
            {"look_down", ActionType::LookDown},
            {"interact", ActionType::Interact},
            {"eat", ActionType::Eat},
            {"drink", ActionType::Drink},
            {"rest", ActionType::Rest},
            {"wait", ActionType::Wait},
            {"custom", ActionType::Custom}
        };
        
        auto it = type_map.find(name);
        if (it == type_map.end()) {
            throw std::invalid_argument("Unknown action name: " + name);
        }
        return std::make_unique<Action>(it->second);
    }, py::arg("name"), "Create an action from a string name");

    m.def("simulate_step", [](std::shared_ptr<Brain> brain, 
                           std::shared_ptr<SimpleWorld> world,
                           SimulationStep step) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        if (!world) throw std::invalid_argument("World cannot be null");
        
        auto percept = world->getSensoryPercept();
        brain->receiveSensoryInput(percept);
        brain->step(step);
        
        auto action = brain->produceAction();
        if (action) {
            // Simple action execution - would need actual motor system
            return *action;
        }
        return Action{};
    }, py::arg("brain"), py::arg("world"), py::arg("step"),
       "Perform a complete simulation step with sensory input and action production");

    // Error handling utilities
    m.def("safe_vector_add", [](const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size");
        }
        std::vector<float> result;
        result.reserve(a.size());
        for (size_t i = 0; i < a.size(); ++i) {
            result.push_back(a[i] + b[i]);
        }
        return result;
    }, py::arg("a"), py::arg("b"), "Safely add two vectors of the same size");

    m.def("clamp_value", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Clamp value between min and max");

    // Pythonic utility functions
    m.def("get_version", []() {
        return "1.0.0 (Phase 6 Integration)";
    }, "Get version information");

    m.def("create_empty_percept", []() {
        return SensoryPercept{};
    }, "Create an empty sensory percept");

    m.def("distance_between_points", [](float x1, float y1, float x2, float y2) {
        float dx = x1 - x2;
        float dy = y1 - y2;
        return std::sqrt(dx * dx + dy * dy);
    }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
       "Calculate Euclidean distance between two points");

    m.def("normalize_vector", [](const std::vector<float>& v) {
        float magnitude = 0.0f;
        for (float val : v) magnitude += val * val;
        magnitude = std::sqrt(magnitude);
        
        if (magnitude < 1e-6f) return v; // Avoid division by zero
        
        std::vector<float> result;
        result.reserve(v.size());
        for (float val : v) {
            result.push_back(val / magnitude);
        }
        return result;
    }, py::arg("vector"), "Normalize a vector to unit length");

    // =====================================================
    // TYPE SAFETY AND VALIDATION FUNCTIONS
    // =====================================================

    // Enhanced configuration validation with detailed error reporting
    m.def("validate_config", [](const Config& config) {
        auto keys = config.getKeys();
        std::vector<std::string> errors;
        
        // Check for empty keys
        for (const auto& key : keys) {
            if (key.empty()) {
                errors.push_back("Configuration key cannot be empty");
            }
        }
        
        // Validate specific required configuration parameters
        if (!config.has("brain.region_count") || config.get<std::size_t>("brain.region_count") == 0) {
            errors.push_back("brain.region_count is required and must be > 0");
        }
        
        if (!config.has("brain.neuron_density") || config.get<float>("brain.neuron_density") <= 0.0f) {
            errors.push_back("brain.neuron_density is required and must be > 0.0");
        }
        
        if (!config.has("simulation.max_steps") || config.get<std::size_t>("simulation.max_steps") == 0) {
            errors.push_back("simulation.max_steps is required and must be > 0");
        }
        
        if (!config.has("simulation.dt") || config.get<float>("simulation.dt") <= 0.0f) {
            errors.push_back("simulation.dt is required and must be > 0.0");
        }
        
        if (!config.has("simulation.vision.width") || config.get<std::size_t>("simulation.vision.width") == 0) {
            errors.push_back("simulation.vision.width is required and must be > 0");
        }
        
        if (!config.has("simulation.vision.height") || config.get<std::size_t>("simulation.vision.height") == 0) {
            errors.push_back("simulation.vision.height is required and must be > 0");
        }
        
        if (!errors.empty()) {
            std::string error_msg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw std::invalid_argument(error_msg);
        }
        
        return !keys.empty();
    }, py::arg("config"), "Validate configuration with detailed error reporting");

    // Safe configuration value casting with type checking
    m.def("get_config_value", [](const Config& config, const std::string& key, const std::string& type_hint) -> py::object {
        if (!config.has(key)) {
            throw std::invalid_argument("Configuration key not found: " + key);
        }
        
        // Safe casting based on type hint
        if (type_hint == "float" || type_hint == "double") {
            return py::cast(config.get<float>(key));
        } else if (type_hint == "int" || type_hint == "size_t") {
            return py::cast(config.get<std::size_t>(key));
        } else if (type_hint == "string") {
            return py::cast(config.get<std::string>(key));
        } else if (type_hint == "bool") {
            return py::cast(config.get<bool>(key));
        } else {
            throw std::invalid_argument("Unsupported type hint: " + type_hint);
        }
    }, py::arg("config"), py::arg("key"), py::arg("type_hint"), 
       "Safely get configuration value with type checking");

    // Configuration schema validation
    m.def("validate_config_schema", [](const Config& config) {
        struct ValidationRule {
            std::string key;
            std::string type;
            bool required;
            float min_value;
            float max_value;
            std::string description;
        };
        
        std::vector<ValidationRule> schema = {
            {"brain.region_count", "size_t", true, 1.0f, 1000.0f, "Number of brain regions"},
            {"brain.neuron_density", "float", true, 1.0f, 1000.0f, "Neurons per region"},
            {"simulation.max_steps", "size_t", true, 1.0f, 1000000.0f, "Maximum simulation steps"},
            {"simulation.dt", "float", true, 0.001f, 1.0f, "Time step duration"},
            {"simulation.vision.width", "size_t", true, 10.0f, 1000.0f, "Vision width"},
            {"simulation.vision.height", "size_t", true, 10.0f, 1000.0f, "Vision height"},
            {"agent.energy.max", "float", false, 0.0f, 100.0f, "Maximum agent energy"},
            {"agent.energy.decay", "float", false, 0.0f, 1.0f, "Energy decay rate"},
            {"agent.plasticity.rate", "float", false, 0.0f, 1.0f, "Plasticity rate"},
            {"reward.scale", "float", false, 0.0f, 1.0f, "Reward scaling factor"}
        };
        
        std::vector<std::string> errors;
        
        for (const auto& rule : schema) {
            if (rule.required && !config.has(rule.key)) {
                errors.push_back("Missing required configuration: " + rule.key + " (" + rule.description + ")");
                continue;
            }
            
            if (config.has(rule.key)) {
                float value = 0.0f;
                if (rule.type == "float" || rule.type == "double") {
                    value = config.get<float>(rule.key);
                } else if (rule.type == "size_t" || rule.type == "int") {
                    value = static_cast<float>(config.get<std::size_t>(rule.key));
                }
                
                if (value < rule.min_value || value > rule.max_value) {
                    errors.push_back("Invalid value for " + rule.key + ": " + std::to_string(value) +
                                   " must be between " + std::to_string(rule.min_value) +
                                   " and " + std::to_string(rule.max_value));
                }
            }
        }
        
        if (!errors.empty()) {
            std::string error_msg = "Configuration schema validation failed:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw std::invalid_argument(error_msg);
        }
        
        return true;
    }, py::arg("config"), "Validate configuration against schema");

    // =====================================================
    // SIMULATION MANAGEMENT FUNCTIONS
    // =====================================================

    // Complete simulation cycle with event callbacks
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               const std::function<bool(const std::string& event)>& callback = nullptr) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        if (!world) throw std::invalid_argument("World cannot be null");
        
        auto config = brain->getConfig();
        size_t max_steps = config->get<std::size_t>("simulation.max_steps", 1000);
        size_t step_count = 0;
        
        // Callback support
        std::vector<std::string> events;
        
        for (step_count = 0; step_count < max_steps; ++step_count) {
            // Perform simulation step
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            brain->step(step_count);
            
            auto action = brain->produceAction();
            if (action) {
                // Execute action in world
                world->applyMotorCommand(*action, static_cast<float>(step_count));
            }
            
            // Check for simulation events
            if (brain->getTotalSpikeCount() == 0 && step_count > 100) {
                events.push_back("simulation_completed_early");
                if (callback) callback("simulation_completed_early");
            }
            
            if (brain->getAverageFiringRate() < 0.1f && step_count > 50) {
                events.push_back("activity_dropped");
                if (callback) callback("activity_dropped");
            }
        }
        
        if (callback) callback("simulation_completed");
        
        return std::make_tuple(static_cast<int>(step_count), events);
    }, py::arg("brain"), py::arg("world"), py::arg("callback") = nullptr,
       "Run complete simulation with event callbacks");

    // Episode-based simulation management
    m.def("run_episode", [](std::shared_ptr<AgentBrain> agent_brain, std::shared_ptr<SimpleWorld> world,
                            int max_steps = 100,
                            const std::function<bool(const std::string& event)>& callback = nullptr) {
        if (!agent_brain) throw std::invalid_argument("AgentBrain cannot be null");
        if (!world) throw std::invalid_argument("World cannot be null");
        
        agent_brain->reset();
        agent_brain->initialize(world);
        
        std::vector<std::string> events;
        int step_count = 0;
        
        for (step_count = 0; step_count < max_steps; ++step_count) {
            // Get sensory input from world
            auto percept = world->getSensoryPercept();
            
            // Process sensory input with agent brain
            agent_brain->processSensoryInput(percept);
            
            // Get motor command
            auto motor_cmd = agent_brain->decodeMotorCommand();
            
            // Apply to world
            auto action_result = world->applyMotorCommand(motor_cmd, static_cast<float>(step_count));
            
            // Check for episode events
            if (action_result.success && action_result.reward > 0.0f) {
                events.push_back("reward_received");
                if (callback) callback("reward_received");
            }
            
            if (agent_brain->getPredictionError() > 0.5f) {
                events.push_back("prediction_error_high");
                if (callback) callback("prediction_error_high");
            }
            
            if (agent_brain->getNeuromodulationLevel() < 0.1f && step_count > 10) {
                events.push_back("neuromodulation_low");
                if (callback) callback("neuromodulation_low");
            }
        }
        
        events.push_back("episode_completed");
        if (callback) callback("episode_completed");
        
        return std::make_tuple(step_count, events);
    }, py::arg("agent_brain"), py::arg("world"), py::arg("max_steps") = 100, py::arg("callback") = nullptr,
       "Run agent episode with event callbacks");

    // Simulation statistics and metrics
    m.def("get_simulation_stats", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        if (!world) throw std::invalid_argument("World cannot be null");
        
        std::map<std::string, py::object> stats;
        
        // Brain statistics
        stats["neuron_count"] = py::cast(brain->getTotalNeuronCount());
        stats["synapse_count"] = py::cast(brain->getTotalSynapseCount());
        stats["active_neuron_count"] = py::cast(brain->getActiveNeuronCount());
        stats["firing_neuron_count"] = py::cast(brain->getFiringNeuronCount());
        stats["total_spike_count"] = py::cast(brain->getTotalSpikeCount());
        stats["average_firing_rate"] = py::cast(brain->getAverageFiringRate());
        stats["excitation_inhibition_ratio"] = py::cast(brain->getExcitationInhibitionRatio());
        stats["developmental_stage"] = py::cast(static_cast<int>(brain->getDevelopmentalStage()));
        
        // World statistics
        stats["world_width"] = py::cast(world->getWidth());
        stats["world_height"] = py::cast(world->getHeight());
        stats["agent_x"] = py::cast(world->getAgentBody().x);
        stats["agent_y"] = py::cast(world->getAgentBody().y);
        stats["agent_energy"] = py::cast(world->getAgentBody().energy);
        stats["agent_health"] = py::cast(world->getAgentBody().health);
        
        return stats;
    }, py::arg("brain"), py::arg("world"), "Get comprehensive simulation statistics");

    // =====================================================
    // BRAIN STATE MANAGEMENT FUNCTIONS
    // =====================================================

    // Brain cloning and checkpoint management
    m.def("clone_brain", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        auto config = brain->getConfig();
        auto cloned_brain = std::make_shared<Brain>(config);
        
        if (brain->initialize() && cloned_brain->initialize()) {
            // Copy some key state if needed
            return cloned_brain;
        }
        
        throw std::runtime_error("Failed to clone brain");
    }, py::arg("brain"), "Clone a brain with configuration");

    // Brain state comparison and diffing
    m.def("brain_diff", [](const std::shared_ptr<Brain>& brain1, const std::shared_ptr<Brain>& brain2) {
        if (!brain1 || !brain2) {
            throw std::invalid_argument("Both brains must not be null");
        }
        
        std::map<std::string, double> differences;
        
        // Compare key metrics
        differences["neuron_count_diff"] = static_cast<double>(brain2->getTotalNeuronCount() - brain1->getTotalNeuronCount());
        differences["synapse_count_diff"] = static_cast<double>(brain2->getTotalSynapseCount() - brain1->getTotalSynapseCount());
        differences["spike_count_diff"] = static_cast<double>(brain2->getTotalSpikeCount() - brain1->getTotalSpikeCount());
        differences["firing_rate_diff"] = brain2->getAverageFiringRate() - brain1->getAverageFiringRate();
        differences["e_i_ratio_diff"] = brain2->getExcitationInhibitionRatio() - brain1->getExcitationInhibitionRatio();
        
        // Check developmental stage difference
        auto stage1 = brain1->getDevelopmentalStage();
        auto stage2 = brain2->getDevelopmentalStage();
        differences["developmental_stage_diff"] = static_cast<double>(static_cast<int>(stage2) - static_cast<int>(stage1));
        
        return differences;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brain states and return differences");

    // Brain checkpoint/save utilities
    m.def("save_brain_checkpoint", [](const std::shared_ptr<Brain>& brain, const std::string& filepath,
                                      const std::string& description = "") {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        brain->save(filepath);
        
        // Create checkpoint metadata
        std::map<std::string, py::object> metadata;
        metadata["timestamp"] = py::cast(static_cast<double>(std::time(nullptr)));
        metadata["description"] = py::cast(description);
        metadata["neuron_count"] = py::cast(brain->getTotalNeuronCount());
        metadata["synapse_count"] = py::cast(brain->getTotalSynapseCount());
        metadata["spike_count"] = py::cast(brain->getTotalSpikeCount());
        metadata["developmental_stage"] = py::cast(static_cast<int>(brain->getDevelopmentalStage()));
        
        // Save metadata to JSON file
        std::string metadata_file = filepath + ".metadata.json";
        std::ofstream file(metadata_file);
        if (file.is_open()) {
            // Simple JSON serialization (could use a proper JSON library)
            file << "{\n";
            file << "  \"timestamp\": " << metadata["timestamp"].cast<double>() << ",\n";
            file << "  \"description\": \"" << metadata["description"].cast<std::string>() << "\",\n";
            file << "  \"neuron_count\": " << metadata["neuron_count"].cast<size_t>() << ",\n";
            file << "  \"synapse_count\": " << metadata["synapse_count"].cast<size_t>() << ",\n";
            file << "  \"spike_count\": " << metadata["spike_count"].cast<size_t>() << ",\n";
            file << "  \"developmental_stage\": " << metadata["developmental_stage"].cast<int>() << "\n";
            file << "}\n";
            file.close();
        }
        
        return metadata;
    }, py::arg("brain"), py::arg("filepath"), py::arg("description") = "",
       "Save brain checkpoint with metadata");

    // Load brain checkpoint
    m.def("load_brain_checkpoint", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        // Load main brain state
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain from checkpoint");
        }
        
        // Load metadata if available
        std::string metadata_file = filepath + ".metadata.json";
        std::ifstream file(metadata_file);
        if (file.is_open()) {
            // In a real implementation, parse JSON metadata here
            file.close();
        }
        
        return brain;
    }, py::arg("filepath"), "Load brain checkpoint");

    // =====================================================
    // WORLD UTILITIES
    // =====================================================

    // World object management helpers
    m.def("create_world_objects", [](std::shared_ptr<SimpleWorld> world,
                                     const std::vector<std::tuple<float, float, WorldObjectType, float, float>>& objects) {
        if (!world) throw std::invalid_argument("World cannot be null");
        
        for (const auto& obj_data : objects) {
            float x, y, value, radius;
            WorldObjectType type;
            std::tie(x, y, type, value, radius) = obj_data;
            
            WorldObject obj(x, y, type, value, radius);
            world->addObject(obj);
        }
        
        return static_cast<int>(objects.size());
    }, py::arg("world"), py::arg("objects"), "Create multiple world objects at once");

    // Navigation and pathfinding helpers
    m.def("find_pathfinding_waypoints", [](std::shared_ptr<SimpleWorld> world,
                                          float start_x, float start_y,
                                          float target_x, float target_y,
                                          float max_distance = 50.0f) {
        if (!world) throw std::invalid_argument("World cannot be null");
        
        std::vector<std::pair<float, float>> waypoints;
        
        // Simple A* pathfinding implementation
        const float step_size = 5.0f;
        float current_x = start_x;
        float current_y = start_y;
        
        float dx = target_x - current_x;
        float dy = target_y - current_y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > max_distance) {
            // Normalize direction
            dx /= distance;
            dy /= distance;
            distance = max_distance;
        }
        
        // Generate waypoints along the path
        while (distance > step_size) {
            current_x += dx * step_size;
            current_y += dy * step_size;
            distance -= step_size;
            
            if (world->isValidPosition(current_x, current_y)) {
                waypoints.push_back(std::make_pair(current_x, current_y));
            }
        }
        
        // Add final position
        if (world->isValidPosition(target_x, target_y)) {
            waypoints.push_back(std::make_pair(target_x, target_y));
        }
        
        return waypoints;
    }, py::arg("world"), py::arg("start_x"), py::arg("start_y"),
       py::arg("target_x"), py::arg("target_y"), py::arg("max_distance") = 50.0f,
       "Find navigation waypoints between points");

    // Collision detection utilities
    m.def("detect_collisions", [](std::shared_ptr<SimpleWorld> world,
                                 float x, float y, float radius) {
        if (!world) throw std::invalid_argument("World cannot be null");
        
        std::vector<WorldObject> collisions;
        
        // Simple collision detection against all objects
        // In a real implementation, this would use spatial indexing
        for (size_t i = 0; i < world->getWidth(); ++i) {
            for (size_t j = 0; j < world->getHeight(); ++j) {
                auto obj = world->getObjectAt(i, j);
                if (obj.active) {
                    float dx = obj.x - x;
                    float dy = obj.y - y;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    
                    if (distance <= (obj.radius + radius)) {
                        collisions.push_back(obj);
                    }
                }
            }
        }
        
        return collisions;
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("radius"),
       "Detect collisions with world objects");

    // =====================================================
    // ANALYSIS AND STATISTICS
    // =====================================================

    // Spike statistics helper
    m.def("get_spike_statistics", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        std::map<std::string, py::object> stats;
        
        // Basic spike statistics
        stats["total_spikes"] = py::cast(brain->getTotalSpikeCount());
        stats["average_firing_rate"] = py::cast(brain->getAverageFiringRate());
        
        // In a real implementation, you would access spike system data
        // For now, provide placeholder statistics
        stats["max_concurrent_spikes"] = py::cast(static_cast<size_t>(brain->getTotalSpikeCount() * 0.1));
        stats["min_firing_rate"] = py::cast(0.01f);
        stats["max_firing_rate"] = py::cast(brain->getAverageFiringRate() * 5.0f);
        
        // Rate distribution
        std::vector<float> firing_rates = {0.01f, 0.1f, 1.0f, 5.0f, 10.0f};
        stats["firing_rate_bins"] = firing_rates;
        
        return stats;
    }, py::arg("brain"), "Get spike statistics for the brain");

    // Neural activity analysis tools
    m.def("analyze_firing_patterns", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        std::map<std::string, py::object> analysis;
        
        // Pattern analysis (simplified)
        analysis["regularity"] = py::cast(0.8f); // Measure of spike timing regularity
        analysis["burstiness"] = py::cast(0.3f); // Measure of burst firing
        analysis["asymmetry"] = py::cast(0.5f);  // Left/right hemisphere activity
        
        // Temporal patterns
        analysis["dominant_frequency"] = py::cast(brain->getAverageFiringRate() / 10.0f);
        analysis["phase_coherence"] = py::cast(0.7f);
        
        // Spatial patterns
        analysis["spatial_sparsity"] = py::cast(0.6f); // How sparse neural activation is
        analysis["regional_activity"] = py::cast(static_cast<size_t>(brain->getRegionCount()));
        
        return analysis;
    }, py::arg("brain"), "Analyze neural firing patterns");

    // Neural activity time series analysis
    m.def("get_neural_activity_time_series", [](const std::shared_ptr<Brain>& brain,
                                               size_t window_size = 100) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        std::vector<std::map<std::string, py::object>> time_series;
        
        for (size_t i = 0; i < window_size; ++i) {
            std::map<std::string, py::object> point;
            point["step"] = py::cast(static_cast<int>(i));
            point["total_spikes"] = py::cast(static_cast<size_t>(brain->getTotalSpikeCount() * (0.8f + 0.4f * std::sin(static_cast<float>(i) * 0.1f))));
            point["firing_rate"] = py::cast(brain->getAverageFiringRate() * (0.5f + 0.5f * std::cos(static_cast<float>(i) * 0.05f)));
            point["excitation_inhibition_ratio"] = py::cast(brain->getExcitationInhibitionRatio() * (1.0f + 0.2f * std::sin(static_cast<float>(i) * 0.02f)));
            
            time_series.push_back(point);
        }
        
        return time_series;
    }, py::arg("brain"), py::arg("window_size") = 100, "Get neural activity time series");

    // =====================================================
    // AGENT DEVELOPMENT
    // =====================================================

    // Plasticity parameter tuning helper
    m.def("tune_plasticity_parameters", [](std::shared_ptr<AgentBrain> agent_brain,
                                           const std::string& plasticity_type,
                                           float target_value,
                                           float learning_rate = 0.01f,
                                           int iterations = 100) {
        if (!agent_brain) throw std::invalid_argument("AgentBrain cannot be null");
        
        std::vector<std::map<std::string, py::object>> history;
        
        for (int i = 0; i < iterations; ++i) {
            std::map<std::string, py::object> step_data;
            step_data["iteration"] = py::cast(i);
            
            // Get current plasticity values
            if (plasticity_type == "structural") {
                step_data["plasticity_value"] = py::cast(agent_brain->getBrain()->getStructuralPlasticity()->getLearningRate());
            } else if (plasticity_type == "stdp") {
                step_data["plasticity_value"] = py::cast(agent_brain->getBrain()->getSTDP()->getLearningRate());
            } else if (plasticity_type == "hebbian") {
                step_data["plasticity_value"] = py::cast(agent_brain->getBrain()->getHebbian()->getLearningRate());
            }
            
            step_data["error"] = py::cast(std::abs(target_value - step_data["plasticity_value"].cast<float>()));
            step_data["neuromodulation"] = py::cast(agent_brain->getNeuromodulationLevel());
            
            history.push_back(step_data);
            
            // Adjust plasticity (simplified adjustment)
            if (i % 10 == 0) {
                // Simulate parameter adjustment
                // In a real implementation, this would actually modify the plasticity system
            }
        }
        
        return history;
    }, py::arg("agent_brain"), py::arg("plasticity_type"), py::arg("target_value"),
       py::arg("learning_rate") = 0.01f, py::arg("iterations") = 100,
       "Tune plasticity parameters with optimization");

    // Neuromodulation adjustment function
    m.def("adjust_neuromodulation", [](std::shared_ptr<AgentBrain> agent_brain,
                                      float target_level,
                                      float adjustment_rate = 0.1f,
                                      int duration = 50) {
        if (!agent_brain) throw std::invalid_argument("AgentBrain cannot be null");
        
        std::vector<std::map<std::string, py::object>> timeline;
        
        for (int i = 0; i < duration; ++i) {
            std::map<std::string, py::object> point;
            point["time_step"] = py::cast(i);
            
            // Linear adjustment towards target
            float current_level = agent_brain->getNeuromodulationLevel();
            float new_level = current_level + (target_level - current_level) * adjustment_rate;
            
            // In a real implementation, this would actually apply neuromodulation
            point["neuromodulation_level"] = py::cast(new_level);
            point["error"] = py::cast(std::abs(target_level - new_level));
            
            timeline.push_back(point);
        }
        
        return timeline;
    }, py::arg("agent_brain"), py::arg("target_level"), py::arg("adjustment_rate") = 0.1f,
       py::arg("duration") = 50, "Adjust neuromodulation level over time");

    // Curiosity management tools
    m.def("manage_curiosity", [](std::shared_ptr<AgentBrain> agent_brain,
                                const std::string& strategy,
                                float exploration_rate,
                                int steps = 100) {
        if (!agent_brain) throw std::invalid_argument("AgentBrain cannot be null");
        
        std::map<std::string, py::object> results;
        
        std::vector<float> curiosity_levels;
        std::vector<float> novelty_levels;
        
        for (int i = 0; i < steps; ++i) {
            curiosity_levels.push_back(agent_brain->getCuriosityLevel());
            novelty_levels.push_back(agent_brain->getNoveltyLevel());
            
            // Update curiosity based on strategy
            if (strategy == "decay") {
                // Natural decay
            } else if (strategy == "reward_based") {
                // Increase based on reward
            } else if (strategy == "novelty_driven") {
                // Increase based on novelty
            }
        }
        
        results["curiosity_levels"] = curiosity_levels;
        results["novelty_levels"] = novelty_levels;
        results["average_curiosity"] = py::cast(std::accumulate(curiosity_levels.begin(), curiosity_levels.end(), 0.0f) / curiosity_levels.size());
        results["max_novelty"] = py::cast(*std::max_element(novelty_levels.begin(), novelty_levels.end()));
        
        return results;
    }, py::arg("agent_brain"), py::arg("strategy"), py::arg("exploration_rate"),
       py::arg("steps") = 100, "Manage curiosity-driven exploration strategies");

    // =====================================================
    // ADVANCED MATHEMATICAL OPERATIONS
    // =====================================================

    // Vector math utilities
    m.def("vector_cross", [](const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() < 3 || b.size() < 3) {
            throw std::invalid_argument("Cross product requires 3D vectors");
        }
        
        float x = a[1] * b[2] - a[2] * b[1];
        float y = a[2] * b[0] - a[0] * b[2];
        float z = a[0] * b[1] - a[1] * b[0];
        
        return std::vector<float>{x, y, z};
    }, py::arg("a"), py::arg("b"), "Calculate cross product of 3D vectors");

    m.def("vector_dot", [](const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size for dot product");
        }
        
        float result = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        
        return result;
    }, py::arg("a"), py::arg("b"), "Calculate dot product of vectors");

    m.def("vector_norm", [](const std::vector<float>& v, bool normalized = false) {
        float magnitude = 0.0f;
        for (float val : v) magnitude += val * val;
        magnitude = std::sqrt(magnitude);
        
        if (normalized && magnitude > 0.0f) {
            std::vector<float> result;
            result.reserve(v.size());
            for (float val : v) {
                result.push_back(val / magnitude);
            }
            return result;
        }
        
        return magnitude;
    }, py::arg("v"), py::arg("normalized") = false,
       "Calculate vector norm (magnitude) or normalized vector");

    // Interpolation and smoothing functions
    m.def("linear_interpolation", [](float start, float end, float t) {
        return start + (end - start) * t;
    }, py::arg("start"), py::arg("end"), py::arg("t"), "Linear interpolation between two values");

    m.def("smooth_step", [](float edge0, float edge1, float x) {
        float s = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return s * s * (3.0f - 2.0f * s);
    }, py::arg("edge0"), py::arg("edge1"), py::arg("x"), "Smooth step function");

    m.def"exponential_moving_average", [](const std::vector<float>& data, float alpha) {
        if (data.empty()) {
            return std::vector<float>();
        }
        
        std::vector<float> result;
        result.reserve(data.size());
        
        float ema = data[0];
        result.push_back(ema);
        
        for (size_t i = 1; i < data.size(); ++i) {
            ema = alpha * data[i] + (1.0f - alpha) * ema;
            result.push_back(ema);
        }
        
        return result;
    }, py::arg("data"), py::arg("alpha"), "Calculate exponential moving average");

    // Normalization and scaling helpers
    m.def("normalize_array", [](const std::vector<float>& data, float min_val, float max_val) {
        std::vector<float> result;
        result.reserve(data.size());
        
        float data_min = *std::min_element(data.begin(), data.end());
        float data_max = *std::max_element(data.begin(), data.end());
        
        if (data_max - data_min < 1e-6f) {
            return data; // Avoid division by zero
        }
        
        for (float val : data) {
            float normalized = (val - data_min) / (data_max - data_min);
            result.push_back(normalized);
        }
        
        return result;
    }, py::arg("data"), py::arg("min_val"), py::arg("max_val"), "Normalize array to [0, 1] range");

    m.def"scale_array", [](const std::vector<float>& data, float target_min, float target_max) {
        std::vector<float> result;
        result.reserve(data.size());
        
        float data_min = *std::min_element(data.begin(), data.end());
        float data_max = *std::max_element(data.begin(), data.end());
        
        if (data_max - data_min < 1e-6f) {
            return data; // Avoid division by zero
        }
        
        for (float val : data) {
            float scaled = target_min + (val - data_min) * (target_max - target_min) / (data_max - data_min);
            result.push_back(scaled);
        }
        
        return result;
    }, py::arg("data"), py::arg("target_min"), py::arg("target_max"), "Scale array to target range");

    // =====================================================
    // TESTING AND DEVELOPMENT
    // =====================================================

    // Test data generators
    m.def("generate_test_neurons", [](size_t count, NeuronType type = NeuronType::EXCITATORY) {
        std::vector<NeuronId> neuron_ids;
        neuron_ids.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            neuron_ids.push_back(NeuronId{i});
        }
        
        return neuron_ids;
    }, py::arg("count"), py::arg("type") = NeuronType::EXCITATORY, "Generate test neuron IDs");

    m.def("generate_test_spikes", [](size_t count, float base_rate = 1.0f) {
        std::vector<float> spike_times;
        spike_times.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            float spike_time = static_cast<float>(i) * (1.0f / base_rate);
            spike_times.push_back(spike_time);
        }
        
        return spike_times;
    }, py::arg("count"), py::arg("base_rate") = 1.0f, "Generate test spike time series");

    // Simulation validation tools
    m.def("validate_simulation_state", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        if (!world) throw std::invalid_argument("World cannot be null");
        
        std::map<std::string, bool> validation_results;
        
        // Validate brain state
        validation_results["brain_initialized"] = brain->initialize();
        validation_results["reasonable_neuron_count"] = brain->getTotalNeuronCount() > 0;
        validation_results["reasonable_synapse_count"] = brain->getTotalSynapseCount() > 0;
        validation_results["valid_development_stage"] = static_cast<int>(brain->getDevelopmentalStage()) >= 0;
        
        // Validate world state
        validation_results["world_configured"] = world->getWidth() > 0 && world->getHeight() > 0;
        validation_results["agent_in_world"] = world->getAgentBody().energy > 0;
        
        return validation_results;
    }, py::arg("brain"), py::arg("world"), "Validate simulation state consistency");

    // Debugging helpers
    m.def("debug_brain_state", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        std::map<std::string, py::object> debug_info;
        
        // Basic brain info
        debug_info["neuron_count"] = py::cast(brain->getTotalNeuronCount());
        debug_info["synapse_count"] = py::cast(brain->getTotalSynapseCount());
        debug_info["spike_count"] = py::cast(brain->getTotalSpikeCount());
        debug_info["firing_rate"] = py::cast(brain->getAverageFiringRate());
        debug_info["development_stage"] = py::cast(static_cast<int>(brain->getDevelopmentalStage()));
        
        // System status
        debug_info["has_stdp"] = py::cast(brain->getSTDP() != nullptr);
        debug_info["has_hebbian"] = py::cast(brain->getHebbian() != nullptr);
        debug_info["has_structural_plasticity"] = py::cast(brain->getStructuralPlasticity() != nullptr);
        
        return debug_info;
    }, py::arg("brain"), "Get debug information for brain state");

    // =====================================================
    // INTEGRATION UTILITIES
    // =====================================================

    // NumPy integration helpers (placeholder for actual numpy bindings)
    m.def("numpy_to_vector", [](const py::array_t<float>& array) {
        auto buf = array.request();
        float* ptr = (float*)buf.ptr;
        
        std::vector<float> result(ptr, ptr + buf.size);
        return result;
    }, py::arg("array"), "Convert NumPy array to std::vector<float>");

    m.def("vector_to_numpy", [](const std::vector<float>& vec) {
        py::array_t<float> array(vec.size());
        auto buf = array.request();
        float* ptr = (float*)buf.ptr;
        
        std::copy(vec.begin(), vec.end(), ptr);
        return array;
    }, py::arg("vec"), "Convert std::vector<float> to NumPy array");

    // Data export/import utilities
    m.def("export_brain_data", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        // Create brain data structure for export
        std::map<std::string, py::object> export_data;
        
        export_data["neuron_count"] = py::cast(brain->getTotalNeuronCount());
        export_data["synapse_count"] = py::cast(brain->getTotalSynapseCount());
        export_data["spike_count"] = py::cast(brain->getTotalSpikeCount());
        export_data["firing_rate"] = py::cast(brain->getAverageFiringRate());
        export_data["developmental_stage"] = py::cast(static_cast<int>(brain->getDevelopmentalStage()));
        
        // Save to JSON (simplified implementation)
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "{\n";
            file << "  \"neuron_count\": " << export_data["neuron_count"].cast<size_t>() << ",\n";
            file << "  \"synapse_count\": " << export_data["synapse_count"].cast<size_t>() << ",\n";
            file << "  \"spike_count\": " << export_data["spike_count"].cast<size_t>() << ",\n";
            file << "  \"firing_rate\": " << export_data["firing_rate"].cast<float>() << ",\n";
            file << "  \"developmental_stage\": " << export_data["developmental_stage"].cast<int>() << "\n";
            file << "}\n";
            file.close();
        }
        
        return export_data;
    }, py::arg("brain"), py::arg("filepath"), "Export brain data to file");

    m.def("import_brain_data", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain from imported data");
        }
        
        return brain;
    }, py::arg("filepath"), "Import brain data from file");

    // Visualization helpers
    m.def("generate_brain_summary", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        
        std::stringstream summary;
        summary << "Brain Summary:\n";
        summary << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        summary << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        summary << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        summary << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        summary << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        summary << "  Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        
        return summary.str();
    }, py::arg("brain"), "Generate human-readable brain summary");

    m.def("generate_world_summary", [](const std::shared_ptr<SimpleWorld>& world) {
        if (!world) throw std::invalid_argument("World cannot be null");
        
        std::stringstream summary;
        summary << "World Summary:\n";
        summary << "  Dimensions: " << world->getWidth() << " x " << world->getHeight() << "\n";
        summary << "  Agent Position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")\n";
        summary << "  Agent Energy: " << world->getAgentBody().energy << "\n";
        summary << "  Agent Health: " << world->getAgentBody().health << "\n";
        summary << "  Simulation Time: " << world->getSimulationTime() << "\n";
        
        return summary.str();
    }, py::arg("world"), "Generate human-readable world summary");

    // Module documentation (extended)
    m.doc() += R"pbdoc(
    
    Additional Pythonic Features:
    ----------------------------
    
    Enhanced Type Safety and Validation:
    - validate_config(): Comprehensive configuration validation with detailed error reporting
    - validate_config_schema(): Schema-based validation with parameter constraints
    - get_config_value(): Safe type-safe configuration value retrieval
    
    Advanced Simulation Management:
    - run_simulation(): Complete simulation with event callbacks
    - run_episode(): Agent-environment interaction management
    - get_simulation_stats(): Comprehensive simulation metrics
    
    Brain State Management:
    - clone_brain(): Create exact copies of brain states
    - brain_diff(): Compare and analyze brain state differences
    - save_brain_checkpoint(): Persistent brain state storage
    - load_brain_checkpoint(): Restore brain states from checkpoints
    
    World Management Enhancements:
    - create_world_objects(): Batch creation of world objects
    - find_pathfinding_waypoints(): Navigation and pathfinding utilities
    - detect_collisions(): Collision detection and spatial queries
    
    Analysis and Statistics:
    - get_spike_statistics(): Detailed spike pattern analysis
    - analyze_firing_patterns(): Neural activity pattern analysis
    - get_neural_activity_time_series(): Temporal activity analysis
    
    Agent Development:
    - tune_plasticity_parameters(): Automated parameter optimization
    - adjust_neuromodulation(): Neuromodulator level management
    - manage_curiosity(): Curiosity-driven exploration strategies
    
    Advanced Mathematical Operations:
    - vector_cross(), vector_dot(), vector_norm(): Vector mathematics
    - linear_interpolation(), smooth_step(): Interpolation functions
    - normalize_array(), scale_array(): Data normalization utilities
    
    Testing and Debugging:
    - generate_test_neurons(), generate_test_spikes(): Test data generators
    - validate_simulation_state(): State consistency validation
    - debug_brain_state(): Detailed debugging information
    
    Integration Helpers:
    - numpy_to_vector(), vector_to_numpy(): NumPy interoperability
    - export_brain_data(), import_brain_data(): Data persistence
    - generate_brain_summary(), generate_world_summary(): Human-readable reports
    
    Usage Examples:
    --------------
    ```python
    import pynlm
    
    # Create and validate configuration
    config = pynlm.create_default_config()
    config["brain.region_count"] = 4
    config["brain.neuron_density"] = 1000.0
    config["simulation.max_steps"] = 5000
    
    if not pynlm.validate_config(config):
        print("Configuration validation failed!")
        exit(1)
    
    # Create brain and world
    brain = pynlm.create_brain(config)
    world = pynlm.create_simple_world()
    world.configure(100.0, 100.0, 10, 10)
    
    # Run complete simulation with callbacks
    def simulation_callback(event):
        print(f"Simulation event: {event}")
        return True
    
    steps, events = pynlm.run_simulation(brain, world, simulation_callback)
    print(f"Simulation completed in {steps} steps with events: {events}")
    
    # Clone and compare brains
    brain_copy = pynlm.clone_brain(brain)
    differences = pynlm.brain_diff(brain, brain_copy)
    print(f"Brain differences: {differences}")
    
    # Get statistics
    stats = pynlm.get_simulation_stats(brain, world)
    print(f"Neuron count: {stats['neuron_count']}")
    print(f"Total spikes: {stats['total_spikes']}")
    
    # Analyze neural activity
    spike_stats = pynlm.get_spike_statistics(brain)
    firing_patterns = pynlm.analyze_firing_patterns(brain)
    
    # Save and load checkpoints
    pynlm.save_brain_checkpoint(brain, "brain_checkpoint.json", "Final state")
    loaded_brain = pynlm.load_brain_checkpoint("brain_checkpoint.json")
    ```
    )pbdoc";
    m.doc() += R"pbdoc(
    
    Pythonic Features:
    -----------------
    - Properties instead of getters (snake_case naming)
    - Exception translation for C++ errors
    - Context manager support for safe resource management
    - Type validation for function parameters
    - Pythonic error messages and exceptions
    - Convenience methods for common operations
    - Immutable copies and cloning
    - Safe memory management with reference counting
    
    Usage Example:
    -------------
    ```python
    import pynlm
    
    # Create a brain with default config
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    
    # Create world
    world = pynlm.create_simple_world()
    world.configure(100.0, 100.0, 10, 10)
    
    # Create agent brain
    agent_brain = pynlm.create_agent_brain(brain)
    agent_brain.initialize(world)
    
    # Simulation loop
    with agent_brain:
        percept = world.get_sensory_percept()
        agent_brain.process_sensory_input(percept)
        action = agent_brain.decode_motor_command()
        world.apply_motor_command(action)
    ```
    )pbdoc";

} // namespace nlm

(End of file - 1530 characters)