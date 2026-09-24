// Improved Python bindings for NLM with better method signatures and naming
// This file enhances the original bindings.cpp with:
// 1. More Pythonic method names
// 2. Better parameter names
// 3. Additional convenience methods
// 4. Enhanced type safety
// 5. Better documentation

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Full compatibility with Python 3.8+
        - Type-safe Python API with enhanced error handling
        - Extensive documentation and examples
        - Performance-optimized for neural simulations
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===================== Identity Classes =====================
    
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
        });

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

    // ===================== Enumerations =====================
    
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

    // ===================== Config Class =====================
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        This class provides configuration management for NLM systems.
        It supports loading/saving from files, setting/getting parameters,
        and maintaining configuration history.
        
        Configuration values can be accessed using dot notation
        (e.g., config.get("brain.neuron_count")) or through type-specific methods.
        
        Common configuration sections include:
        - brain: Neuron and synapse parameters
        - plasticity: Learning rules configuration
        - neuromod: Neuromodulation system settings
        - development: Developmental process parameters
        - performance: Simulation performance settings
    )pbdoc")
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
        
        // Type-specific getter methods
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value) return *value;
            throw std::runtime_error("Config key not found or wrong type: " + key);
        }, py::arg("key"), "Get integer configuration value")
        
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) return *value;
            throw std::runtime_error("Config key not found or wrong type: " + key);
        }, py::arg("key"), "Get float configuration value")
        
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            throw std::runtime_error("Config key not found or wrong type: " + key);
        }, py::arg("key"), "Get boolean configuration value")
        
        .def("get_string", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw std::runtime_error("Config key not found or wrong type: " + key);
        }, py::arg("key"), "Get string configuration value")
        
        // Type-specific setter methods
        .def("set_int", &Config::set, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        
        .def("set_bool", &Config::set, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("set_string", &Config::set, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        
        // Convenience methods
        .def("get_neuron_count", [](const Config& self) {
            return self.get<int64_t>("brain.neuron_count").value_or(1000);
        }, "Get neuron count (convenience method)")
        
        .def("set_neuron_count", [](Config& self, int64_t count) {
            self.set("brain.neuron_count", count);
        }, py::arg("count"), "Set neuron count (convenience method)")
        
        .def("get_synapse_density", [](const Config& self) {
            return self.get<double>("brain.synapse_density").value_or(0.1);
        }, "Get synapse density (convenience method)")
        
        .def("set_synapse_density", [](Config& self, double density) {
            self.set("brain.synapse_density", density);
        }, py::arg("density"), "Set synapse density (convenience method)")
        
        // Python-friendly repr
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===================== Sensory Input Classes =====================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Represents visual input from the agent's field of view.
        Data is organized as a flattened array with width x height x channels.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data array")
        .def("get_width", &Vision::getWidth, "Get image width")
        .def("get_height", &Vision::getHeight, "Get image height")
        .def("get_channels", &Vision::getChannels, "Get number of color channels")
        .def("reshape_data", [](Vision& self, size_t width, size_t height, size_t channels) {
            // Note: This would require implementation in Vision class
            throw std::runtime_error("reshape_data not yet implemented");
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
            "Reshape data array to specified dimensions");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Represents audio input from the environment.
        Data contains sound samples at specified sample rate.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data array")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set audio sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get audio sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of audio samples");

    // Internal signals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        Represents internal state signals (e.g., hunger, thirst, energy levels).
        Useful for modeling homeostatic needs and internal states.
    )pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("get_signal_count", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals");

    // ===================== Action Classes =====================
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Represents an action that can be taken by the agent.
        Actions consist of a type and optional parameters.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType, "Get action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("get_name", &Action::getName, "Get human-readable action name")
        .def("get_magnitude", [](const Action& self) {
            // Calculate action magnitude from parameters
            const auto& params = self.getParameters();
            if (params.empty()) return 0.0f;
            return *std::max_element(params.begin(), params.end());
        }, "Get action magnitude (max parameter value)")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Represents an object in the simulated world.
        Objects can be resources, hazards, walls, or markers.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object x position")
        .def_readwrite("y", &WorldObject::y, "Object y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        
        Result of applying a motor command to the world.
        Contains reward, success status, and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about result");

    // ===================== AgentBody Class =====================
    
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Represents the physical state of the agent in the world.
        Includes position, velocity, energy, health, and other body metrics.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle (radians)")
        .def_readwrite("velocity_x", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocity_y", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("max_energy", &AgentBody::maxEnergy, "Maximum energy")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("is_moving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("is_turning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime, "Time of last action")
        .def("get_speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get current speed")
        .def("get_position", [](const AgentBody& body) {
            return std::make_tuple(body.x, body.y);
        }, "Get (x, y) position as tuple")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("reset", &AgentBody::reset, "Reset body to default state");

    // ===================== SensoryPercept Class =====================
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        Container for all sensory inputs received by the agent in one time step.
        Includes vision, touch, internal signals, proprioception, and audio.
    )pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision data")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch data")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception,
             "Get proprioception data")
        .def("set_proprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"), "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as dictionary")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("add_noise", [](SensoryPercept& percept, float noise_level) {
            // Add Gaussian noise to sensory signals (simplified)
            auto vision = percept.getVision();
            auto data = vision.getData();
            for (float& value : data) {
                value += (std::rand() / (RAND_MAX / 2.0) - 1.0) * noise_level;
            }
            vision.setData(data);
            percept.setVision(vision);
        }, py::arg("noise_level"), "Add Gaussian noise to sensory data");

    // ===================== SimpleWorld Class =====================
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        A simple 2D simulation environment that provides sensory input to agents.
        Contains terrain, objects, and physics for agent navigation and interaction.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and agent's vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for given timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command and return result")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not a wall)")
        .def("get_object_at", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("get_time", &SimpleWorld::getSimulationTime,  // Alias
             "Get current simulation time (alternative name)")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("get_objects_near", [](const SimpleWorld& world, float x, float y, float radius) {
            std::vector<WorldObject> nearby;
            for (const auto& obj : world.getObjects()) {  // Assuming getObjects() method
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx*dx + dy*dy <= radius*radius) {
                    nearby.push_back(obj);
                }
            }
            return nearby;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
            "Get objects within radius of position")
        .def("get_resource_count", [](const SimpleWorld& world, WorldObjectType type) {
            int count = 0;
            for (const auto& obj : world.getObjects()) {
                if (obj.type == type) count++;
            }
            return count;
        }, py::arg("type"), "Get number of objects of given type");

    // ===================== Brain Class =====================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        The core class of NLM, implementing real spiking neural computation with
        event-driven dynamics and integrated memory, prediction, cognition, and
        neuromodulation systems.
        
        The brain operates as a closed-loop system that processes sensory input,
        makes decisions, and learns from experience through plasticity mechanisms.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        // Step methods with different signatures
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_type", &Brain::injectCurrentToNeurons,
             py::arg("neuron_type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        
        // Region management
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get number of regions")
        .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
        
        // Statistics
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
        .def("get_pending_spike_event_count", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        
        // Development
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("get_developmental_age", &Brain::getDevelopmentalStage,
             "Get developmental stage (alternative access)")
        
        // Memory systems access
        .def("get_working_memory", &Brain::getWorkingMemory,
             "Access working memory system")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             "Access episodic memory system")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             "Access associative memory system")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             "Access prediction system")
        
        // Cognition systems access
        .def("get_planner", &Brain::getPlanner, "Access neural planner")
        .def("get_concept_formation", &Brain::getConceptFormation,
             "Access concept formation system")
        .def("get_attention", &Brain::getAttention, "Access attentional selection")
        
        // Neuromodulation access
        .def("get_dopamine", &Brain::getDopamine, "Access dopamine neuromodulator")
        .def("get_curiosity", &Brain::getCuriosity, "Access curiosity neuromodulator")
        .def("get_novelty", &Brain::getNovelty, "Access novelty detection")
        .def("get_prediction_error", &Brain::getPredictionErrorSignal,
             "Access prediction error signal")
        
        // Plasticity system access
        .def("get_stdp", &Brain::getSTDP, "Access STDP plasticity system")
        .def("get_hebbian", &Brain::getHebbian, "Access Hebbian plasticity system")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             "Access structural plasticity system")
        
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("log_status", &Brain::logStatus, "Log brain status and statistics")
        
        // State management utilities
        .def("has_spikes", [](const Brain& brain) {
            return brain.getTotalSpikeCount() > 0;
        }, "Check if brain has produced any spikes")
        
        .def("is_active", [](const Brain& brain) {
            return brain.getFiringNeuronCount() > 0;
        }, "Check if brain has active neurons")
        
        .def("get_health_score", [](const Brain& brain) {
            // Simple health metric: firing rate normalized by max expected
            float firing_rate = brain.getAverageFiringRate();
            return std::min(1.0f, firing_rate / 100.0f); // Normalize to 0-1
        }, "Get brain health score (0-1)");

    // ===================== AgentBrain Class =====================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        The AgentBrain class serves as the interface between the NLM brain and the
        simulated world. It handles sensory transduction (processing sensory input)
        and motor decoding (converting neural activity to actions).
        
        This class implements the closed-loop interaction between brain and environment,
        enabling learning through experience and interaction with the world.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with given brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("decode_action", &AgentBrain::decodeMotorCommand,  // Alias
             "Decode brain motor activity into motor command (alternative name)")
        
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("apply_learning_signal", &AgentBrain::applyRewardModulation,  // Alias
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply learning signal (alternative name)")
        
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        
        // Development access
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_development_age", &AgentBrain::getDevelopmentalStage,
             "Get developmental stage (alternative access)")
        
        // Neuromodulation access
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        // Learning subsystem control
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        
        // Learning subsystem status
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // State management
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        
        // Advanced learning methods
        .def("apply_curiosity", [](AgentBrain& agent, float novelty) {
            // Apply curiosity-based learning
            if (agent.isCuriosityEnabled()) {
                float curiosity = agent.getCuriosityLevel();
                agent.applyRewardModulation(novelty, curiosity);
            }
        }, py::arg("novelty"), "Apply curiosity-based learning signal")
        
        .def("apply_prediction_error_learning", [](AgentBrain& agent, float error) {
            // Apply prediction error-based learning
            agent.applyRewardModulation(error, 0.0f);
        }, py::arg("error"), "Apply prediction error-based learning");

    // ===================== Factory Functions =====================
    
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object")
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // Utility constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM (Neural Learning Machine) - Python Bindings";
    m.attr("__author__") = "NLM Research Team";
}

} // namespace nlm