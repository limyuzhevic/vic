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

// Helper function for safe parameter validation
template<typename T>
T validate_positive(const T& value, const std::string& param_name, T min_value = 0) {
    if (value < min_value) {
        throw std::invalid_argument("Parameter '" + param_name + "' must be >= " + std::to_string(min_value) + ". Got: " + std::to_string(value));
    }
    return value;
}

// Helper for creating consistent error messages
inline std::string make_error(const std::string& operation, const std::string& details) {
    return "NLM " + operation + ": " + details;
}

// === CONFIGURATION SYSTEM ===

void registerConfig(py::module_ &m) {
    py::class_<Config>(m, "Config", R"pbdoc(Configuration system for NLM)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (INI format)")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"), "Check if configuration key exists")
        .def("get_keys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set string configuration")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set integer configuration")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set float configuration")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration")
        .def("get_int", [](Config& self, const std::string& key, int default_value = 0) {
            return self.getOr<int>(key, default_value);
        }, py::arg("key"), py::arg("default_value") = 0, "Get integer configuration")
        .def("get_float", [](Config& self, const std::string& key, float default_value = 0.0f) {
            return self.getOr<float>(key, default_value);
        }, py::arg("key"), py::arg("default_value") = 0.0f, "Get float configuration")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value = "") {
            return self.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default_value") = "", "Get string configuration")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value = false) {
            return self.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default_value") = false, "Get boolean configuration")
        .def("validate", &Config::has, "Validate configuration key")
        .def("__repr__", [](const Config& cfg) {
            return "<NLM Config>";
        });
}

// === NEURAL IDENTIFIERS ===

void registerIdentifiers(py::module_ &m) {
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
        .def("__ne__", &PopulationId::operator!=);
}

// === SENSORY INPUT ===

void registerSensoryInput(py::module_ &m) {
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get sensory input type")
        .def("get_data", &SensoryInput::getData, "Get raw sensor data")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get data dimensions")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input for visual data)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 0, py::arg("height") = 0, py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("get_width", &Vision::getWidth, "Get vision width")
        .def("get_height", &Vision::getHeight, "Get vision height")
        .def("get_channels", &Vision::getChannels, "Get number of channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate") = 0, py::arg("num_samples") = 0)
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"), "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal neural signals)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals, "Clear all signals");
}

// === MOTOR ACTIONS ===

void registerActions(py::module_ &m) {
    py::class_<Action>(m, "Action", R"pbdoc(Motor action representation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType, "Get action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("get_name", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Clone action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("type") = WorldObjectType::Empty,
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object X position")
        .def_readwrite("y", &WorldObject::y, "Object Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state");
}

// === WORLD SIMULATION ===

void registerWorld(py::module_ &m) {
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (degrees)")
        .def_readwrite("velocity_x", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocity_y", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("is_moving", &AgentBody::isMoving, "Is moving flag")
        .def_readwrite("is_turning", &AgentBody::isTurning, "Is turning flag")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Result of an action)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward") = 0.0f, py::arg("success") = false, py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success flag")
        .def_readwrite("message", &ActionResult::message, "Result message");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision data")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch data")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals, "Get all signals")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("current_time"), "Apply motor command")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get max energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed");
}

// === BRAIN INTERFACE ===

void registerBrain(py::module_ &m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"), "Perform simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"), "Perform simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"), "Inject sensory input")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current to neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject current to neuron type")
        .def("produce_action", &Brain::produceAction, "Produce motor action")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state")
        .def("add_region", &Brain::addRegion, py::arg("name") = "", "Add neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"), "Get region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get region count")
        .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
        .def("get_regions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount, "Get active neuron count")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_pending_spike_event_count", &Brain::getPendingSpikeEventCount, "Get pending spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig, py::return_value_policy::reference_internal, "Get configuration")
        .def("log_status", &Brain::logStatus, "Log brain status")
        // Advanced brain operations
        .def("batch_step", [](Brain& self, size_t steps, float timestep = 0.001f) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001f, "Perform multiple steps efficiently")
        .def("inject_current_range", [](Brain& self, size_t start_idx, size_t count, float current) {
            validate_positive(count, "count", 0);
            for (size_t i = 0; i < count; ++i) {
                if (i < self.getTotalNeuronCount()) {
                    self.injectCurrent(NeuronId(i), current);
                }
            }
        }, py::arg("start_idx"), py::arg("count"), py::arg("current"), "Inject current to neuron range")
        .def("get_spike_statistics", [](Brain& self) {
            py::dict stats;
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["pending_spikes"] = self.getPendingSpikeEventCount();
            return stats;
        }, "Get brain spike statistics as dictionary");
}

// === AGENT INTERFACE ===

void registerAgentBrain(py::module_ &m) {
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory input")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward"), "Apply reward modulation")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get underlying brain")
        // Subsystem controls
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled, "Check if development enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity enabled");
}

// === ENUMERATIONS ===

void registerEnums(py::module_ &m) {
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type)pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::enum_<ActionResultType>(m, "ActionResultType", R"pbdoc(Action result type)pbdoc")
        .value("Success", ActionResultType::Success)
        .value("Failure", ActionResultType::Failure)
        .value("Partial", ActionResultType::Partial)
        .value("Error", ActionResultType::Error)
        .export_values();
}

// === FACTORY FUNCTIONS ===

void registerFactories(py::module_ &m) {
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface");
}

// === UTILITY FUNCTIONS ===

void registerUtilities(py::module_ &m) {
    m.def("create_neuron_id", [](uint64_t value) { return NeuronId(value); }, py::arg("value"), "Create neuron ID");
    m.def("create_synapse_id", [](uint64_t value) { return SynapseId(value); }, py::arg("value"), "Create synapse ID");
    m.def("create_region_id", [](uint64_t value) { return RegionId(value); }, py::arg("value"), "Create region ID");
    m.def("create_population_id", [](uint64_t value) { return PopulationId(value); }, py::arg("value"), "Create population ID");

    // Pythonic helper functions
    m.def("run_simulation_loop", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                  std::shared_ptr<AgentBrain> agent, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), 
        "Run complete simulation loop");

    m.def("batch_inject_current", [](std::shared_ptr<Brain> brain, float current, 
                                    size_t count = 100, NeuronType type = NeuronType::Internal) {
        brain->injectCurrentToNeurons(type, current);
    }, py::arg("brain"), py::arg("current"), py::arg("count") = 100, py::arg("type") = NeuronType::Internal, "Inject current to neuron group");

    m.def("get_network_statistics", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["neurons"] = brain->getTotalNeuronCount();
        stats["synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        return stats;
    }, py::arg("brain"), "Get network statistics dictionary");

    m.def("create_config_from_dict", [](py::dict config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::str(item.first);
            std::string value;
            
            if (py::isinstance(item.second, py::int_())) {
                int int_val = item.second.cast<int>();
                config->set(key, int_val, ConfigSource::Default);
            } else if (py::isinstance(item.second, py::float_())) {
                float float_val = item.second.cast<float>();
                config->set(key, float_val, ConfigSource::Default);
            } else if (py::isinstance(item.second, py::bool_())) {
                bool bool_val = item.second.cast<bool>();
                config->set(key, bool_val, ConfigSource::Default);
            } else {
                value = item.second.cast<std::string>();
                config->set(key, value, ConfigSource::Default);
            }
        }
        return config;
    }, py::arg("config_dict"), "Create config from Python dictionary");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        =============================================

        A powerful Python interface to the NLM neural simulation framework.

        This module provides comprehensive access to advanced neural computation including:
        - Real spiking neural networks with LIF dynamics
        - Multiple plasticity rules (STDP, Hebbian, structural)
        - Neuromodulation (dopamine, curiosity, novelty detection)
        - Working and episodic memory systems
        - Prediction and cognition modules
        - Developmental brain maturation
        - Complete world simulation environment

        Key Features:
        - Optimized batch operations for performance
        - Advanced error handling and validation
        - Pythonic API with type hints and comprehensive documentation
        - Integration with NumPy arrays for efficient data handling
        - Factory functions and builder patterns
        - Statistical analysis tools
        - Checkpoint and state management

        Quick Start Example:
            >>> import pynlm
            >>> config = pynlm.create_default_config()
            >>> brain = pynlm.create_brain(config)
            >>> brain.initialize()
            >>> for i in range(100):
            ...     brain.step(i)
            >>> print(f"Neurons: {brain.get_total_neuron_count()}")
            >>> print(f"Spikes: {brain.get_total_spike_count()}")

        Advanced Usage:
            >>> brain = pynlm.create_brain(config)
            >>> world = pynlm.create_simple_world()
            >>> agent = pynlm.create_agent_brain(brain)
            >>> agent.enable_reward_modulation(True)
            >>> agent.enable_curiosity(True)
            >>> # Run full simulation
            >>> pynlm.run_simulation_loop(brain, world, agent, 1000)
    )pbdoc";

    // Register all components
    registerConfig(m);
    registerIdentifiers(m);
    registerSensoryInput(m);
    registerActions(m);
    registerWorld(m);
    registerBrain(m);
    registerAgentBrain(m);
    registerEnums(m);
    registerFactories(m);
    registerUtilities(m);

    // Constants for easy access
    m.attr("VERSION") = "0.1.0";
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001f;
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;
}
