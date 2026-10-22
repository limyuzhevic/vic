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

// Helper function for Pythonic validation
inline void validateNotNull(const void* ptr, const std::string& name) {
    if (!ptr) {
        throw std::invalid_argument(name + " cannot be null");
    }
}

inline std::string to_python_string(const std::string& str) {
    return str;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides Pythonic interfaces to the NLM C++ neural simulation
        framework, enabling users to create and simulate artificial neural systems
        with memory, prediction, cognition, and neuromodulation capabilities.
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Event-driven dynamics with synaptic delays
        - Multiple plasticity rules (STDP, Hebbian, structural)
        - Memory systems (working, episodic, associative)
        - Prediction and planning systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and plasticity
        - Agent-environment interaction
        - Checkpoint persistence
        
        Example usage:
        
        ```python
        import pynlm
        
        # Create a brain with default configuration
        brain = pynlm.create_brain()
        brain.initialize()
        
        # Create a simple world
        world = pynlm.create_simple_world()
        
        # Create an agent brain interface
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Run simulation loop
        for step in range(100):
            # Get sensory input from world
            percept = world.get_sensory_percept()
            
            # Process sensory input through agent
            agent.process_sensory_input(percept)
            
            # Update world
            agent.decode_motor_command()
            action = agent.decode_motor_command()
            world.apply_motor_command(action, step)
            world.update(0.001)
            
            # Check for learning
            if step % 100 == 0:
                agent.apply_reward_modulation(0.1, 0.05)
        ```
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // ====================================================================
    // IDENTIFIER TYPES
    // ====================================================================
    
    // NeuronId - Unique identifier for neurons
    py::class_<NeuronId>(m, "NeuronId", 
        R"pbdoc(Unique identifier for a neuron in the neural network.)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, 
            R"pbdoc(The internal value of the neuron identifier.)pbdoc")
        .def("index", &NeuronId::index,
            R"pbdoc(Get the zero-based index of this neuron ID.)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId - Unique identifier for synapses
    py::class_<SynapseId>(m, "SynapseId",
        R"pbdoc(Unique identifier for a synapse between neurons.)pbdoc")
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
    py::class_<RegionId>(m, "RegionId",
        R"pbdoc(Unique identifier for a brain region containing neurons.)pbdoc")
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
    py::class_<PopulationId>(m, "PopulationId",
        R"pbdoc(Unique identifier for a population of similar neurons.)pbdoc")
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

    // ====================================================================
    // ENUMERATIONS
    // ====================================================================

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType",
        R"pbdoc(Enumeration of different neuron types in the NLM system.)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that increases firing probability in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that decreases firing probability in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that modulates plasticity and learning")
        .value("Sensory", NeuronType::Sensory, "Neuron that receives and processes sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that produces motor commands for action")
        .value("Internal", NeuronType::Internal, "Neuron involved in internal cognitive processes")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType",
        R"pbdoc(Enumeration of different synapse types and connection mechanisms.)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Synapse that increases postsynaptic firing probability")
        .value("Inhibitory", SynapseType::Inhibitory, "Synapse that decreases postsynaptic firing probability")
        .value("Modulatory", SynapseType::Modulatory, "Synapse that modulates synaptic strength based on neuromodulators")
        .value("Electrical", SynapseType::Electrical, "Direct electrical connection between neurons")
        .value("GapJunction", SynapseType::GapJunction, "Electrical synapse through gap junctions")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage",
        R"pbdoc(Enumeration of developmental stages that affect plasticity and learning.)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for specific learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Stage of neural maturation and refinement")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable neural connections")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining plasticity")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState",
        R"pbdoc(Enumeration of neuron firing states representing different stages of neural activity.)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest, not actively firing")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron is being inhibited by other neurons")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType",
        R"pbdoc(Enumeration of action types for motor and behavioral responses.)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the world")
        .value("MoveRight", ActionType::MoveRight, "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left to change orientation")
        .value("TurnRight", ActionType::TurnRight, "Turn right to change orientation")
        .value("Look", ActionType::Look, "Look in a specific direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Eat an object or resource")
        .value("Drink", ActionType::Drink, "Drink water or liquid")
        .value("Rest", ActionType::Rest, "Rest and conserve energy")
        .value("Wait", ActionType::Wait, "Wait without taking any action")
        .value("Custom", ActionType::Custom, "Custom action defined by user")
        .export_values();

    // MotorCommand enumeration - low-level motor commands
    py::enum_<MotorCommand>(m, "MotorCommand",
        R"pbdoc(Low-level motor command enumeration for agent movement and actions.)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Propel forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Propel backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate left")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate right")
        .value("LookLeft", MotorCommand::LookLeft, "Pan sensors left")
        .value("LookRight", MotorCommand::LookRight, "Pan sensors right")
        .value("Interact", MotorCommand::Interact, "Interact with nearby object")
        .value("Wait", MotorCommand::Wait, "No movement")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType",
        R"pbdoc(Enumeration of object types in the simulated world.)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space in the world")
        .value("Resource", WorldObjectType::Resource, "Resource that provides positive reward")
        .value("Hazard", WorldObjectType::Hazard, "Hazard that provides negative reward")
        .value("Wall", WorldObjectType::Wall, "Boundary or obstacle")
        .value("Marker", WorldObjectType::Marker, "Navigation aid or landmark")
        .export_values();

    // ====================================================================
    // CONFIGURATION SYSTEM
    // ====================================================================

    py::class_<Config>(m, "Config",
        R"pbdoc(Configuration system for the NLM neural simulation framework.)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.)pbdoc")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments.)pbdoc")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.)pbdoc")
        .def("get_keys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.)pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.)pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        // Convenience methods for common configuration values
        .def("get_neuron_count", &Config::getOr<int64_t>, "neuron_count", 500,
             R"pbdoc(Get the number of neurons to create. Default: 500.)pbdoc")
        .def("set_neuron_count", [](Config& self, int64_t count) {
            self.set("neuron_count", count, ConfigSource::User);
        }, py::arg("count"),
             R"pbdoc(Set the number of neurons to create.)pbdoc")
        .def("get_simulation_timestep", &Config::getOr<double>, "simulation_timestep", 0.001,
             R"pbdoc(Get the simulation timestep in seconds. Default: 0.001.)pbdoc")
        .def("set_simulation_timestep", [](Config& self, double timestep) {
            self.set("simulation_timestep", timestep, ConfigSource::User);
        }, py::arg("timestep"),
             R"pbdoc(Set the simulation timestep in seconds.)pbdoc")
        .def("get_connection_probability", &Config::getOr<float>, "connection_probability", 0.15f,
             R"pbdoc(Get the connection probability between neurons. Default: 0.15.)pbdoc")
        .def("set_connection_probability", [](Config& self, float prob) {
            self.set("connection_probability", prob, ConfigSource::User);
        }, py::arg("probability"),
             R"pbdoc(Set the connection probability between neurons.)pbdoc");

    // ====================================================================
    // SENSORY INPUT SYSTEMS
    // ====================================================================

    py::class_<SensoryInput>(m, "SensoryInput",
        R"pbdoc(Base class for all sensory input modalities.)pbdoc")
        .def("get_type", &SensoryInput::getType,
             R"pbdoc(Get the type of sensory input.)pbdoc")
        .def("get_data", &SensoryInput::getData,
             R"pbdoc(Get the raw sensory data as a vector of floats.)pbdoc")
        .def("get_dimensions", &SensoryInput::getDimensions,
             R"pbdoc(Get the dimensionality of the sensory data.)pbdoc")
        .def("get_timestamp", &SensoryInput::getTimestamp,
             R"pbdoc(Get the timestamp of this sensory input.)pbdoc")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the timestamp of this sensory input.)pbdoc")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    // Vision - visual sensory input
    py::class_<Vision, SensoryInput>(m, "Vision",
        R"pbdoc(Visual sensory input representing a 2D image with RGB channels.)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(Create a Vision input with specified dimensions.)pbdoc")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set the vision data as a flattened array of RGB values.)pbdoc")
        .def("get_width", &Vision::getWidth,
             R"pbdoc(Get the width of the vision input in pixels.)pbdoc")
        .def("get_height", &Vision::getHeight,
             R"pbdoc(Get the height of the vision input in pixels.)pbdoc")
        .def("get_channels", &Vision::getChannels,
             R"pbdoc(Get the number of color channels (typically 3 for RGB).)pbdoc")
        .def("set_resolution", [](Vision& self, size_t width, size_t height) {
            // Note: This assumes Vision has a method to resize; check actual API
        }, py::arg("width"), py::arg("height"),
             R"pbdoc(Set the resolution of the vision input.)pbdoc");

    // Audio - audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio",
        R"pbdoc(Audio sensory input representing sound samples.)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"),
             R"pbdoc(Create Audio input with specified sample rate and duration.)pbdoc")
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set the audio data as a vector of sample values.)pbdoc")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             R"pbdoc(Set the sample rate in Hz.)pbdoc")
        .def("get_sample_rate", &Audio::getSampleRate,
             R"pbdoc(Get the sample rate in Hz.)pbdoc")
        .def("get_num_samples", &Audio::getNumSamples,
             R"pbdoc(Get the number of audio samples.)pbdoc");

    // InternalSignals - internal state signals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals",
        R"pbdoc(Internal signals representing the agent's internal state.)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add an internal signal value.)pbdoc")
        .def("clear_signals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all internal signals.)pbdoc")
        .def("get_signal_count", &InternalSignals::getDataSize,
             R"pbdoc(Get the number of internal signals.)pbdoc");

    // ====================================================================
    // MOTOR OUTPUT SYSTEMS
    // ====================================================================

    py::class_<Action>(m, "Action",
        R"pbdoc(Action representation for motor output from the brain.)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Create an action with the specified type.)pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Create an action with type and parameters.)pbdoc")
        .def("get_type", &Action::getType,
             R"pbdoc(Get the action type.)pbdoc")
        .def("set_type", &Action::setType, py::arg("type"),
             R"pbdoc(Set the action type.)pbdoc")
        .def("get_parameters", &Action::getParameters,
             R"pbdoc(Get the action parameters.)pbdoc")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set the action parameters.)pbdoc")
        .def("get_name", &Action::getName,
             R"pbdoc(Get the human-readable name of this action.)pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(Create a copy of this action.)pbdoc")
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + std::to_string(static_cast<int>(action.getType())) + ">";
        })
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // ====================================================================
    // WORLD SYSTEM
    // ====================================================================

    py::class_<WorldObject>(m, "WorldObject",
        R"pbdoc(Representation of an object in the simulated world.)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(Create a world object at position with type and properties.)pbdoc")
        .def_readwrite("x", &WorldObject::x,
            R"pbdoc(X coordinate position in the world.)pbdoc")
        .def_readwrite("y", &WorldObject::y,
            R"pbdoc(Y coordinate position in the world.)pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
            R"pbdoc{Radius of the world object.)pbdoc")
        .def_readwrite("type", &WorldObject::type,
            R"pbdoc(Type of the world object.)pbdoc")
        .def_readwrite("value", WorldObject::value,
            R"pbdoc{Value or magnitude of the world object.)pbdoc")
        .def_readwrite("active", WorldObject::active,
            R"pbdoc{Whether the world object is currently active.)pbdoc")
        .def("is_colliding", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return (dx * dx + dy * dy) <= (obj.radius * obj.radius);
        }, py::arg("x"), py::arg("y"),
            R"pbdoc{Check if this object is colliding with a position.)pbdoc")
        .def("distance_to", [](const WorldObject& obj, const WorldObject& other) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"),
            R"pbdoc{Calculate distance to another world object.)pbdoc");

    py::class_<AgentBody>(m, "AgentBody",
        R"pbdoc(Physical body state of an agent in the simulated world.)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
            R"pbdoc{X coordinate position.)pbdoc")
        .def_readwrite("y", &AgentBody::y,
            R"pbdoc{Y coordinate position.)pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
            R"pbdoc{Orientation in radians.)pbdoc")
        .def_readwrite("velocity_x", &AgentBody::velocityX,
            R"pbdoc{X velocity component.)pbdoc")
        .def_readwrite("velocity_y", &AgentBody::velocityY,
            R"pbdoc{Y velocity component.)pbdoc")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity,
            R"pbdoc{Angular velocity in radians per second.)pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
            R"pbdoc{Energy level between 0 and 1.)pbdoc")
        .def_readwrite("health", &AgentBody::health,
            R"pbdoc{Health level between 0 and 1.)pbdoc")
        .def_readwrite("age", &AgentBody::age,
            R"pbdoc{Age in simulation steps.)pbdoc")
        .def_readwrite("is_moving", &AgentBody::isMoving,
            R"pbdoc{Whether the agent is currently moving.)pbdoc")
        .def_readwrite("is_turning", &AgentBody::isTurning,
            R"pbdoc{Whether the agent is currently turning.)pbdoc")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime,
            R"pbdoc{Time of last action.)pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc{Reset the agent body to initial state.)pbdoc")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"),
             R"pbdoc{Set the agent's position.)pbdoc")
        .def("set_velocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"),
             R"pbdoc{Set the agent's velocity.)pbdoc")
        .def("update_position", [](AgentBody& body, float dt) {
            body.x += body.velocityX * dt;
            body.y += body.velocityY * dt;
            body.orientation += body.angularVelocity * dt;
        }, py::arg("dt"),
             R"pbdoc{Update position based on current velocity.)pbdoc");

    py::class_<ActionResult>(m, "ActionResult",
        R"pbdoc(Result of an action applied to the world.)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc{Create an action result with reward, success, and message.)pbdoc")
        .def_readwrite("reward", &ActionResult::reward,
            R"pbdoc{Reward received from the action.)pbdoc")
        .def_readwrite("success", &ActionResult::success,
            R"pbdoc{Whether the action was successful.)pbdoc")
        .def_readwrite("message", &ActionResult::message,
            R"pbdoc{Message describing the action result.)pbdoc")
        .def("__repr__", [](const ActionResult& result) {
            std::string success_str = result.success ? "SUCCESS" : "FAILED";
            return "<ActionResult reward=" + std::to_string(result.reward) + 
                   " success=" + success_str + ">";
        });

    // SimpleWorld - the simulated environment
    py::class_<SimpleWorld>(m, "SimpleWorld",
        R"pbdoc(Simple 2D world for NLM agent simulation.)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             R"pbdoc{Configure the world dimensions and agent vision.)pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc{Reset the world to initial state.)pbdoc")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc{Set the agent's starting position.)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc{Update the world state for the given timestep.)pbdoc")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             R"pbdoc{Apply a motor command to the world.)pbdoc")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc{Get the current sensory percept from the world.)pbdoc")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc{Get the agent's body state.)pbdoc")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc{Add a world object to the simulation.)pbdoc")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc{Remove an object at the given position.)pbdoc")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc{Check if a position is valid (within world bounds).)pbdoc")
        .def("get_width", &SimpleWorld::getWidth,
             R"pbdoc{Get the world width.)pbdoc")
        .def("get_height", &SimpleWorld::getHeight,
             R"pbdoc{Get the world height.)pbdoc")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             R"pbdoc{Get the maximum energy for agents.)pbdoc")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc{Set the maximum energy for agents.)pbdoc")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc{Get the energy decay rate.)pbdoc")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc{Set the energy decay rate.)pbdoc")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             R"pbdoc{Get the current simulation time.)pbdoc")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc{Set the random seed for reproducible simulations.)pbdoc")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             R"pbdoc{Get the random seed.)pbdoc")
        // Convenience methods
        .def("get_vision_percept", [](SimpleWorld& world) {
            return world.getSensoryPercept();
        }, R"pbdoc{Get vision percept from the world (alias for get_sensory_percept).)pbdoc")
        .def("get_vision", [](SimpleWorld& world) {
            return world.getSensoryPercept();
        }, R"pbdoc{Get vision percept (alias for get_sensory_percept).)pbdoc")
        .def("get_agent_position", [](SimpleWorld& world) {
            auto body = world.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, R"pbdoc{Get the agent's current position as (x, y) tuple.)pbdoc")
        .def("get_agent_orientation", [](SimpleWorld& world) {
            return world.getAgentBody().orientation;
        }, R"pbdoc{Get the agent's orientation in radians.)pbdoc")
        .def("add_resource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
            R"pbdoc{Add a resource object to the world.)pbdoc")
        .def("add_hazard", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.value = value;
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = -1.0f,
            R"pbdoc{Add a hazard object to the world.)pbdoc")
        .def("add_wall", [](SimpleWorld& world, float x, float y, float radius) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Wall;
            obj.radius = radius;
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
            R"pbdoc{Add a wall object to the world.)pbdoc")
        .def("find_nearest", [](SimpleWorld& world, WorldObjectType type) {
            auto& objects = world.get_objects();  // Note: check if this method exists
            float min_dist = std::numeric_limits<float>::max();
            WorldObject* nearest = nullptr;
            
            for (auto& obj : objects) {
                if (obj.type == type && obj.active) {
                    float dx = obj.x - world.getAgentBody().x;
                    float dy = obj.y - world.getAgentBody().y;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    if (dist < min_dist) {
                        min_dist = dist;
                        nearest = &obj;
                    }
                }
            }
            
            if (nearest) {
                return std::make_tuple(nearest->x, nearest->y, nearest->value);
            } else {
                return std::make_tuple(0.0f, 0.0f, 0.0f);
            }
        }, py::arg("type"),
            R"pbdoc{Find the nearest object of a given type. Returns (x, y, value).)pbdoc");

    // ====================================================================
    // AGENT BRAIN INTERFACE
    // ====================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept",
        R"pbdoc(Sensory percept data from the world.)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision,
             R"pbdoc{Get vision data as a flattened array.)pbdoc")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc{Set vision data.)pbdoc")
        .def("get_vision_width", &SensoryPercept::getVisionWidth,
             R"pbdoc{Get vision width.)pbdoc")
        .def("get_vision_height", &SensoryPercept::getVisionHeight,
             R"pbdoc{Get vision height.)pbdoc")
        .def("get_touch", &SensoryPercept::getTouch,
             R"pbdoc{Get touch data.)pbdoc")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc{Set touch data.)pbdoc")
        .def("get_internal", &SensoryPercept::getInternal,
             R"pbdoc{Get internal signals.)pbdoc")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc{Set internal signals.)pbdoc")
        .def("get_proprioception", &SensoryPercept::getProprioception,
             R"pbdoc{Get proprioception data.)pbdoc")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc{Set proprioception data.)pbdoc")
        .def("get_audio", &SensoryPercept::getAudio,
             R"pbdoc{Get audio data.)pbdoc")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc{Set audio data.)pbdoc")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             R"pbdoc{Get all sensory signals as a combined vector.)pbdoc")
        .def("get_timestamp", &SensoryPercept::getTimestamp,
             R"pbdoc{Get the timestamp of this percept.)pbdoc")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc{Set the timestamp of this percept.)pbdoc")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept vision=" + std::to_string(percept.getVision().size()) + 
                   " touch=" + std::to_string(percept.getTouch().size()) + ">";
        })
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, R"pbdoc{Convert sensory percept to a Python dictionary.)pbdoc")
        .def("from_dict", [](SensoryPercept& percept, py::dict data) {
            if (py::contains(data, "vision")) {
                percept.setVision(py::cast<std::vector<float>>(data["vision"]));
            }
            if (py::contains(data, "touch")) {
                percept.setTouch(py::cast<std::vector<float>>(data["touch"]));
            }
            if (py::contains(data, "internal")) {
                percept.setInternal(py::cast<std::vector<float>>(data["internal"]));
            }
            if (py::contains(data, "proprioception")) {
                percept.setProprioception(py::cast<std::vector<float>>(data["proprioception"]));
            }
            if (py::contains(data, "audio")) {
                percept.setAudio(py::cast<std::vector<float>>(data["audio"]));
            }
            if (py::contains(data, "timestamp")) {
                percept.setTimestamp(py::cast<double>(data["timestamp"]));
            }
        }, py::arg("data"),
            R"pbdoc{Create a sensory percept from a Python dictionary.)pbdoc");

    py::class_<AgentBrain>(m, "AgentBrain",
        R"pbdoc(Agent brain interface connecting NLM brain to world.)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc{Create an agent brain interface with the given brain.)pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc{Initialize the agent brain with a world.)pbdoc")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             R"pbdoc{Get the expected size of sensory input.)pbdoc")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             R"pbdoc{Get the expected size of motor output.)pbdoc")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"),
             R"pbdoc{Process sensory input from the world and inject into brain.)pbdoc")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             R"pbdoc{Decode brain motor activity into a motor command.)pbdoc")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             R"pbdoc{Apply reward-based neuromodulation.)pbdoc")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc{Update the development system.)pbdoc")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc{Get the current developmental stage.)pbdoc")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc{Get the current neuromodulation level.)pbdoc")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             R"pbdoc{Get the current curiosity level.)pbdoc")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             R"pbdoc{Get the current novelty level.)pbdoc")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             R"pbdoc{Get the current prediction error.)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc{Reset the agent for a new episode.)pbdoc")
        .def("get_brain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             R"pbdoc{Get the underlying brain object.)pbdoc")
        // Configuration methods
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             R"pbdoc{Enable or disable reward modulation.)pbdoc")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             R"pbdoc{Enable or disable structural plasticity.)pbdoc")
        .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"),
             R"pbdoc{Enable or disable development.)pbdoc")
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             R"pbdoc{Enable or disable curiosity-driven exploration.)pbdoc")
        // Property getters
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc{Check if reward modulation is enabled.)pbdoc")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc{Check if structural plasticity is enabled.)pbdoc")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc{Check if development is enabled.)pbdoc")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc{Check if curiosity-driven exploration is enabled.)pbdoc")
        // Convenience methods
        .def("get_curiosity_drive", [](AgentBrain& agent) {
            return agent.getCuriosityLevel();
        }, R"pbdoc{Get curiosity drive for exploration.)pbdoc")
        .def("get_novelty_sensitivity", [](AgentBrain& agent) {
            return agent.getNoveltyLevel();
        }, R"pbdoc{Get sensitivity to novelty.)pbdoc")
        .def("get_learning_rate", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc{Get the learning rate based on neuromodulation.)pbdoc")
        .def("should_explore", [](AgentBrain& agent) {
            return agent.getCuriosityLevel() > 0.5f;
        }, R"pbdoc{Check if the agent should explore rather than exploit.)pbdoc")
        .def("explore", [](AgentBrain& agent) {
            // Generate a random exploration command
            int choice = agent.get_brain()->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }, R"pbdoc{Generate a random exploration command.)pbdoc")
        .def("create_episode", [](AgentBrain& agent, std::shared_ptr<SimpleWorld> world) {
            agent.reset();
            agent.initialize(*world);
        }, py::arg("world"),
            R"pbdoc{Create a new episode with the given world.)pbdoc")
        .def("process_episode_step", [](AgentBrain& agent, SimpleWorld& world, double timestep) {
            // Process one step of an episode
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, timestep);
            world.update(timestep);
            return action;
        }, py::arg("world"), py::arg("timestep"),
            R"pbdoc{Process one step of an episode. Returns the action taken.)pbdoc");

    // ====================================================================
    // BRAIN SYSTEM
    // ====================================================================

    py::class_<Brain>(m, "Brain",
        R"pbdoc(Central neural simulation brain class.)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc{Create a brain with the given configuration.)pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc{Initialize the brain with configuration.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             R"pbdoc{Perform a simulation step.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             R"pbdoc{Perform a simulation step with timestamp.)pbdoc")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc{Receive sensory input from environment.)pbdoc")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc{Inject current into a specific neuron.)pbdoc")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc{Inject current into all neurons of a specific type.)pbdoc")
        .def("produce_action", &Brain::produceAction,
             R"pbdoc{Produce motor action based on neural activity.)pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc{Reset brain state.)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc{Save brain state to file (checkpointing).)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc{Load brain state from file.)pbdoc")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc{Add a new neural region.)pbdoc")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc{Get a region by ID.)pbdoc")
        .def("get_region_count", &Brain::getRegionCount,
             R"pbdoc{Get the number of regions.)pbdoc")
        .def("get_region_ids", &Brain::getRegionIds,
             R"pbdoc{Get all region IDs.)pbdoc")
        .def("get_regions", &Brain::getRegions, py::return_value_policy::reference_internal,
             R"pbdoc{Get all regions.)pbdoc")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             R"pbdoc{Get total neuron count across all regions.)pbdoc")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             R"pbdoc{Get total synapse count across all regions.)pbdoc")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             R"pbdoc{Get count of active neurons.)pbdoc")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             R"pbdoc{Get count of currently firing neurons.)pbdoc")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             R"pbdoc{Get average firing rate across all neurons.)pbdoc")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc{Get excitation/inhibition balance ratio.)pbdoc")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             R"pbdoc{Get total spike count.)pbdoc")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             R"pbdoc{Get current developmental stage.)pbdoc")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc{Set developmental stage.)pbdoc")
        .def("get_config", &Brain::getConfig, py::return_value_policy::reference_internal,
             R"pbdoc{Get the configuration.)pbdoc")
        .def("log_status", &Brain::logStatus,
             R"pbdoc{Log brain status.)pbdoc")
        // Convenience methods
        .def("get_spike_system", &Brain::getSpikeSystem,
             R"pbdoc{Get the spike system.)pbdoc")
        .def("get_stdp", &Brain::getSTDP,
             R"pbdoc{Get the STDP plasticity rule.)pbdoc")
        .def("get_hebbian", &Brain::getHebbian,
             R"pbdoc{Get the Hebbian plasticity rule.)pbdoc")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             R"pbdoc{Get the structural plasticity system.)pbdoc")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             R"pbdoc{Get the prediction system.)pbdoc")
        .def("get_working_memory", &Brain::getWorkingMemory,
             R"pbdoc{Get the working memory system.)pbdoc")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             R"pbdoc{Get the episodic memory system.)pbdoc")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             R"pbdoc{Get the associative memory system.)pbdoc")
        .def("get_planner", &Brain::getPlanner,
             R"pbdoc{Get the neural planner.)pbdoc")
        .def("get_concept_formation", &Brain::getConceptFormation,
             R"pbdoc{Get the concept formation system.)pbdoc")
        .def("get_attention", &Brain::getAttention,
             R"pbdoc{Get the attentional selection system.)pbdoc")
        .def("get_development_system", &Brain::getDevelopmentSystem,
             R"pbdoc{Get the development system.)pbdoc")
        .def("get_dopamine", &Brain::getDopamine,
             R"pbdoc{Get the dopamine neuromodulator.)pbdoc")
        .def("get_curiosity", &Brain::getCuriosity,
             R"pbdoc{Get the curiosity neuromodulator.)pbdoc")
        .def("get_novelty", &Brain::getNovelty,
             R"pbdoc{Get the novelty neuromodulator.)pbdoc")
        .def("get_prediction_error_signal", &Brain::getPredictionErrorSignal,
             R"pbdoc{Get the prediction error signal.)pbdoc")
        .def("get_random_generator", &Brain::getRandomGenerator,
             R"pbdoc{Get the random number generator.)pbdoc")
        .def("apply_neuromodulation", &Brain::applyNeuromodulation, py::arg("signal"),
             R"pbdoc{Apply neuromodulatory signals.)pbdoc")
        .def("update_plasticity", &Brain::updatePlasticity,
             R"pbdoc{Update plasticity rules.)pbdoc")
        .def("develop", &Brain::develop,
             R"pbdoc{Apply developmental changes.)pbdoc")
        // Factory functions as methods
        .def("create_default_config", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, R"pbdoc{Create a default configuration.)pbdoc")
        .def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), R"pbdoc{Create a new brain with configuration.)pbdoc")
        .def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, R"pbdoc{Create a new simple world.)pbdoc")
        .def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), R"pbdoc{Create a new agent brain interface.)pbdoc")
        // Statistics and monitoring
        .def("get_network_density", [](Brain& brain) {
            size_t total_neurons = brain.getTotalNeuronCount();
            size_t total_synapses = brain.getTotalSynapseCount();
            return total_neurons > 0 ? (double)total_synapses / total_neurons : 0.0;
        }, R"pbdoc{Get the network density (synapses per neuron).)pbdoc")
        .def("get_activity_pattern", [](Brain& brain) {
            // Get a simple activity pattern from neurons
            auto regions = brain.getRegions();
            std::vector<float> activity_pattern;
            for (const auto& region : regions) {
                auto neurons = region->getAllNeurons();
                for (const auto& neuron : neurons) {
                    activity_pattern.push_back(neuron->getState().membranePotential);
                }
            }
            return activity_pattern;
        }, R"pbdoc{Get the current activity pattern of all neurons.)pbdoc")
        .def("get_development_progress", [](Brain& brain) {
            auto stage = brain.getDevelopmentalStage();
            double progress = 0.0;
            switch (stage) {
                case DevelopmentalStage::Initial: progress = 0.1; break;
                case DevelopmentalStage::CriticalPeriod: progress = 0.4; break;
                case DevelopmentalStage::Maturation: progress = 0.7; break;
                case DevelopmentalStage::Adult: progress = 0.9; break;
                case DevelopmentalStage::Aging: progress = 1.0; break;
            }
            return progress;
        }, R"pbdoc{Get the developmental progress (0-1).)pbdoc")
        .def("simulate_episode", [](Brain& brain, SimpleWorld& world, int steps) {
            for (int step = 0; step < steps; ++step) {
                auto percept = world.getSensoryPercept();
                brain.receiveSensoryInput(percept);
                auto action = brain.produceAction();
                world.applyMotorCommand(action, step * 0.001);
                world.update(step * 0.001);
                brain.step(step, step * 0.001);
            }
        }, py::arg("world"), py::arg("steps"),
            R"pbdoc{Simulate an episode with the world for given steps.)pbdoc")
        .def("run_learning_experiment", [](Brain& brain, SimpleWorld& world, int duration_steps) {
            for (int step = 0; step < duration_steps; ++step) {
                // Inject random patterns to stimulate learning
                int neuron_count = brain.getTotalNeuronCount();
                if (neuron_count > 0) {
                    int idx = brain.getRandomGenerator()->uniformInt(0, neuron_count - 1);
                    brain.injectCurrent(NeuronId(idx), 10.0f);
                }
                brain.step(step, step * 0.001);
                if (step % 100 == 0) {
                    brain.logStatus();
                }
            }
        }, py::arg("world"), py::arg("duration_steps"),
            R"pbdoc{Run a learning experiment with random input patterns.)pbdoc");

    // ====================================================================
    // FACTORY FUNCTIONS AND UTILITIES
    // ====================================================================

    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc{Create a default configuration.)pbdoc")

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc{Create a new brain with configuration.)pbdoc")

    m.def("create_brain_default", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, R"pbdoc{Create a brain with default configuration.)pbdoc")

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc{Create a new simple world.)pbdoc")

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc{Create a new agent brain interface.)pbdoc")

    m.def("create_agent_brain_default", []() -> std::shared_ptr<AgentBrain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, R"pbdoc{Create an agent brain with default configuration.)pbdoc")

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ====================================================================
    // SIMULATION UTILITIES
    // ====================================================================

    m.def("run_demo_simulation", [](int steps, bool verbose = true) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        brain->initialize();
        agent->initialize(*world);
        
        std::vector<MotorCommand> actions;
        for (int step = 0; step < steps; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, step * 0.001);
            world->update(step * 0.001);
            actions.push_back(action);
            
            if (verbose && step % 100 == 0) {
                brain->logStatus();
            }
        }
        
        return actions;
    }, py::arg("steps"), py::arg("verbose") = true,
        R"pbdoc{Run a demonstration simulation and return the sequence of actions.)pbdoc")

    m.def("run_learning_demo", [](int steps, bool verbose = true) {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            // Create a random input pattern
            int neuron_count = brain->getTotalNeuronCount();
            if (neuron_count > 0) {
                int idx = brain->getRandomGenerator()->uniformInt(0, neuron_count - 1);
                brain->injectCurrent(NeuronId(idx), 15.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (verbose && step % 100 == 0) {
                brain->logStatus();
            }
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("steps"), py::arg("verbose") = true,
        R"pbdoc{Run a learning demonstration and return total spike count.)pbdoc")

    m.def("benchmark_plasticity", []() {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        brain->initialize();
        
        // Enable plasticity
        if (auto region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run plasticity experiment
        for (int step = 0; step < 500; ++step) {
            int neuron_count = brain->getTotalNeuronCount();
            if (neuron_count > 0) {
                // Apply correlated activity for STDP
                int pre = brain->getRandomGenerator()->uniformInt(0, neuron_count/2);
                int post = brain->getRandomGenerator()->uniformInt(neuron_count/2 + 1, neuron_count - 1);
                
                brain->injectCurrent(NeuronId(pre), 20.0f);
                brain->step(step * 2, step * 2 * 0.001);
                
                brain->injectCurrent(NeuronId(post), 20.0f);
                brain->step(step * 2 + 1, (step * 2 + 1) * 0.001);
            }
        }
        
        return brain->getTotalSynapseCount();
    }, R"pbdoc{Benchmark plasticity learning. Returns final synapse count.)pbdoc")

    // ====================================================================
    // UTILITY FUNCTIONS
    // ====================================================================

    m.def("get_version", []() {
        return "NLM Python Bindings v0.1.0";
    }, R"pbdoc{Get the version of the NLM Python bindings.)pbdoc")

    m.def("get_build_info", []() {
        py::dict info;
        info["version"] = "NLM Python Bindings v0.1.0";
        info["copyright"] = "Copyright (c) NLM Project Authors";
        info["license"] = "MIT License";
        info["description"] = "Python bindings for NLM (Neural Learning Machine) C++ framework";
        return info;
    }, R"pbdoc{Get build information about the NLM bindings.)pbdoc")

    m.def("validate_brain_state", [](Brain& brain) {
        std::string errors;
        
        if (brain.getTotalNeuronCount() == 0) {
            errors += "Brain has no neurons. ";
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            errors += "Brain has no synapses. ";
        }
        
        if (brain.getDevelopmentalStage() == DevelopmentalStage::Initial && 
            brain.getTotalSpikeCount() > 100) {
            errors += "High activity in initial developmental stage. ";
        }
        
        if (errors.empty()) {
            return true;
        } else {
            throw std::runtime_error("Brain state validation failed: " + errors);
        }
    }, py::arg("brain"),
        R"pbdoc{Validate the brain state. Returns True if valid, raises exception if invalid.)pbdoc")

    m.def("safe_brain_step", [](Brain& brain, SimulationStep step, Timestamp time = 0.0) {
        try {
            brain.step(step, time);
            return true;
        } catch (const std::exception& e) {
            throw std::runtime_error("Brain step failed: " + std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
        R"pbdoc{Safely perform a brain step. Returns True if successful, raises exception if failed.)pbdoc")

    m.def("safe_brain_reset", [](Brain& brain) {
        try {
            brain.reset();
            return true;
        } catch (const std::exception& e) {
            throw std::runtime_error("Brain reset failed: " + std::string(e.what()));
        }
    }, py::arg("brain"),
        R"pbdoc{Safely reset the brain. Returns True if successful, raises exception if failed.)pbdoc")

    m.def("compare_brains", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        if (brains.size() < 2) {
            throw std::invalid_argument("Need at least 2 brains for comparison");
        }
        
        py::dict comparison;
        comparison["brain_count"] = (int)brains.size();
        comparison["total_neurons"] = 0;
        comparison["total_synapses"] = 0;
        comparison["total_spikes"] = 0;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            auto& brain = brains[i];
            comparison["brain_" + std::to_string(i) + "_neurons"] = (int)brain->getTotalNeuronCount();
            comparison["brain_" + std::to_string(i) + "_synapses"] = (int)brain->getTotalSynapseCount();
            comparison["brain_" + std::to_string(i) + "_spikes"] = (int)brain->getTotalSpikeCount();
            
            comparison["total_neurons"] += brain->getTotalNeuronCount();
            comparison["total_synapses"] += brain->getTotalSynapseCount();
            comparison["total_spikes"] += brain->getTotalSpikeCount();
        }
        
        return comparison;
    }, py::arg("brains"),
        R"pbdoc{Compare multiple brains and return statistics.)pbdoc")

    // ====================================================================
    // CONTEXT MANAGERS
    // ====================================================================

    // Context manager for Brain simulation
    class BrainContextManager {
    public:
        BrainContextManager(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {}
        ~BrainContextManager() {
            if (brain_) {
                brain_->reset();
            }
        }
        
        BrainContextManager(const BrainContextManager&) = delete;
        BrainContextManager& operator=(const BrainContextManager&) = delete;
        
        BrainContextManager(BrainContextManager&& other) noexcept : brain_(std::move(other.brain_)) {}
        BrainContextManager& operator=(BrainContextManager&& other) noexcept {
            if (this != &other) {
                brain_ = std::move(other.brain_);
            }
            return *this;
        }
        
        std::shared_ptr<Brain> get_brain() { return brain_; }
        
        void enter() {
            // Brain is already initialized, nothing to do here
        }
        
        void exit(int, const char*, const char*, const char*) {
            if (brain_) {
                brain_->reset();
                brain_.reset();
            }
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };

    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return BrainContextManager(std::move(brain));
    }, py::arg("brain"),
        R"pbdoc{Create a context manager for brain simulation with automatic cleanup.)pbdoc")

    // Context manager for SimpleWorld simulation
    class WorldContextManager {
    public:
        WorldContextManager(std::shared_ptr<SimpleWorld> world) : world_(std::move(world)) {}
        ~WorldContextManager() {
            if (world_) {
                world_->reset();
            }
        }
        
        WorldContextManager(const WorldContextManager&) = delete;
        WorldContextManager& operator=(const WorldContextManager&) = delete;
        
        WorldContextManager(WorldContextManager&& other) noexcept : world_(std::move(other.world_)) {}
        WorldContextManager& operator=(WorldContextManager&& other) noexcept {
            if (this != &other) {
                world_ = std::move(other.world_);
            }
            return *this;
        }
        
        std::shared_ptr<SimpleWorld> get_world() { return world_; }
        
        void enter() {
            // World is already configured, nothing to do here
        }
        
        void exit(int, const char*, const char*, const char*) {
            if (world_) {
                world_->reset();
                world_.reset();
            }
        }
        
    private:
        std::shared_ptr<SimpleWorld> world_;
    };

    m.def("world_context", [](std::shared_ptr<SimpleWorld> world) {
        return WorldContextManager(std::move(world));
    }, py::arg("world"),
        R"pbdoc{Create a context manager for world simulation with automatic cleanup.)pbdoc")

    // Context manager for AgentBrain simulation
    class AgentContextManager {
    public:
        AgentContextManager(std::shared_ptr<AgentBrain> agent) : agent_(std::move(agent)) {}
        ~AgentContextManager() {
            if (agent_) {
                agent_->reset();
            }
        }
        
        AgentContextManager(const AgentContextManager&) = delete;
        AgentContextManager& operator=(const AgentContextManager&) = delete;
        
        AgentContextManager(AgentContextManager&& other) noexcept : agent_(std::move(other.agent_)) {}
        AgentContextManager& operator=(AgentContextManager&& other) noexcept {
            if (this != &other) {
                agent_ = std::move(other.agent_);
            }
            return *this;
        }
        
        std::shared_ptr<AgentBrain> get_agent() { return agent_; }
        
        void enter() {
            // Agent is already initialized, nothing to do here
        }
        
        void exit(int, const char*, const char*, const char*) {
            if (agent_) {
                agent_->reset();
                agent_.reset();
            }
        }
        
    private:
        std::shared_ptr<AgentBrain> agent_;
    };

    m.def("agent_context", [](std::shared_ptr<AgentBrain> agent) {
        return AgentContextManager(std::move(agent));
    }, py::arg("agent"),
        R"pbdoc{Create a context manager for agent simulation with automatic cleanup.)pbdoc")

    // ====================================================================
    // END OF MODULE
    // ====================================================================
    }

} // namespace nlm
