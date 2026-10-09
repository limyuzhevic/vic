#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)

        The Config class manages brain configuration parameters for NLM simulations.
        Configuration values can be loaded from JSON files, command line arguments,
        or set at runtime. Values support various types including scalars and arrays.

        Configuration keys use dot notation for nested parameters, e.g., "brain.neuron_count".
        
        Configuration sources are prioritized: Command Line > File > Runtime > Default.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to JSON configuration file
                 
             Returns:
                 True if configuration was loaded successfully, False otherwise
                 
             Raises:
                 RuntimeError: If the file cannot be read or parsed
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments.
            
            Parses command line arguments in the format --key=value and loads
            them into the configuration. Command line arguments have the highest
            priority and override file and runtime settings.
            
            Args:
                argc: Number of command line arguments
                argv: Array of command line argument strings
                
            Returns:
                True if arguments were parsed successfully, False otherwise
                
            Raises:
                RuntimeError: If argument parsing fails
            )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path where to save the configuration JSON
                 
             Returns:
                 True if configuration was saved successfully, False otherwise
                 
             Raises:
                 RuntimeError: If the file cannot be written
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key (supports dot notation for nested values)
                 
             Returns:
                 True if the key exists in the configuration, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 List of all configuration keys (supports dot notation for nested values)
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             Removes all configuration entries, resetting to empty state.
             Command line and file source values are also cleared.
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 Formatted string containing all configuration entries
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)

        Abstract base class representing sensory input data from the environment.
        This includes vision, audio, touch, internal signals, and proprioception.
        
        Derived classes provide specific implementations for different sensory modalities.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)

        Visual sensory input representing camera images or visual observations.
        Provides pixel-based visual data for brain processing.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the vision data as a flat array of RGB/RGBA values")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels (1, 3, or 4)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)

        Audio sensory input representing sound data.
        Provides time-series audio samples for processing.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data as a time-series of samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)

        Internal neural signals representing brain state and activity.
        Provides access to internal neuron states, modulatory signals, and
        physiological measurements from within the brain.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)

        Represents a motor command or action that the brain can produce.
        Actions include movements, interactions, and other behaviors.
        
        Each action has a type and optional parameters that define specific
        characteristics of the action (e.g., direction, speed, power).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters as list of floats")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters from list of floats")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)

        Represents an object in the simulated world.
        World objects include resources, hazards, walls, and markers.
        
        Objects have position, type, and associated value (e.g., energy, health).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object X coordinate")
        .def_readwrite("y", &WorldObject::y, "Object Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type from WorldObjectType enum")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., energy, score)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active in simulation");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)

        Represents the physical state of the agent in the world.
        Includes position, orientation, movement, energy, and health metrics.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Current health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body state to default values");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)

        Result of applying a motor command in the world.
        Contains reward value, success status, and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value (positive = good, negative = bad)")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Human-readable result description");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)

        Container for sensory information processed by the agent.
        Holds vision, audio, touch, proprioception, and internal signals
        from the environment.
        
        This is the primary interface for agents to receive sensory input
        from the world for brain processing.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a dict")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)

        A 2D simulation environment where the NLM brain operates.
        Provides methods for agent interaction, object management, and world updates.
        
        The world simulates physics, agent movement, and environmental interactions.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensing range")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position in the world")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world physics for given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply agent's motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get agent's current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent's current body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by wall or out of bounds)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy capacity")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy capacity")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)

        The main Brain class for NLM neural network simulation.
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        The brain processes sensory input, maintains internal state, and produces
        motor actions through complex neural dynamics and learning mechanisms.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)

        AgentBrain provides a high-level interface for connecting an NLM brain to a world
        environment, handling sensory input, motor output, and neuromodulation.
        
        This class implements the brain-world interaction loop, making it easier to
        use NLM in agent-based simulations and robotics applications.
        
        Key features:
        - Sensory input processing and injection into brain
        - Motor command decoding from brain activity
        - Reward-based neuromodulation
        - Development system updates
        - Subsystem control (reward modulation, plasticity, development, curiosity)
        - Access to neuromodulation levels and prediction error
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface from existing brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent with world environment")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected size of sensory input vector")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected size of motor output vector")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept from world and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into actionable motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation using dopamine system")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system based on time elapsed")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get overall neuromodulation level (affects learning and plasticity)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (drives exploration behavior)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty detection level (responds to unexpected stimuli)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal (guides learning)")
        .def("reset", &AgentBrain::reset,
             "Reset agent brain for new episode or episode restart")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get reference to the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward-based modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity (connection growth)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable developmental changes over time")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // High-level convenience functions for common NLM usage patterns
    
    m.def("runCompleteSimulation", [](std::shared_ptr<Brain> brain, 
                                     std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent,
                                     int numSteps) {
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from internal signals
            auto internal = percept.getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1f);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
        R"pbdoc(Run a complete simulation loop with brain-world interaction.
        
        This is a convenience function that handles the complete simulation loop:
        world update → sensory input → brain processing → motor output → world action.
        
        Args:
            brain: The neural brain instance
            world: The simulation world
            agent: The agent brain interface
            numSteps: Number of simulation steps to run
            
        Note:
            This function assumes world.getSensoryPercept().getInternal()[0] 
            contains reward information.
        )pbdoc");

    m.def("createBrainWithConfig", [](const std::string& configPath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configPath)) {
            throw py::value_error("Failed to load configuration from file: " + configPath);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("configPath"),
        R"pbdoc(Create and configure a brain from a JSON configuration file.
        
        Args:
            configPath: Path to JSON configuration file
            
        Returns:
            Configured brain instance
            
        Raises:
            ValueError: If configuration file cannot be loaded
        )pbdoc");

    m.def("createStandardConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Set standard neural parameters
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.initial_weight_mean", 0.5);
        config->set("brain.initial_weight_std", 0.1);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        
        // Enable key plasticity mechanisms
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        // Enable neuromodulation
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        return config;
    },
        R"pbdoc(Create a standard configuration with commonly used parameters.
        
        Returns:
            Configured Config object with sensible defaults for most use cases
        )pbdoc");

    m.def("runAgentEpisode", [](std::shared_ptr<AgentBrain> agent,
                              std::shared_ptr<SimpleWorld> world,
                              int maxSteps,
                              bool verbose = false) {
        world->reset();
        agent->reset();
        
        for (int step = 0; step < maxSteps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Get brain to step it
            auto brain = agent->getBrain();
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward if available
            auto internal = percept.getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1f);
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->getFiringNeuronCount() << " firing, "
                         << agent->getCuriosityLevel() << " curiosity, "
                         << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
            }
        }
    }, py::arg("agent"), py::arg("world"), py::arg("maxSteps"), py::arg("verbose") = false,
        R"pbdoc(Run a complete agent episode in a world.
        
        This handles the complete agent-environment interaction loop.
        
        Args:
            agent: The agent brain interface
            world: The simulation world
            maxSteps: Maximum number of simulation steps
            verbose: If True, print progress information every 100 steps
        )pbdoc");

    m.def("benchmarkBrainPerformance", [](std::shared_ptr<Brain> brain,
                                         int numSteps,
                                         int warmupSteps = 100) {
        // Warm-up
        for (int step = 0; step < warmupSteps; ++step) {
            brain->step(step, step * 0.1f);
        }
        
        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step, step * 0.1f);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double avgTimePerStep = static_cast<double>(duration.count()) / numSteps;
        
        return py::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            avgTimePerStep,
            numSteps / (duration.count() / 1000.0)  // steps per second
        );
    }, py::arg("brain"), py::arg("numSteps"), py::arg("warmupSteps") = 100,
        R"pbdoc(Benchmark brain performance metrics.
        
        Args:
            brain: The brain to benchmark
            numSteps: Number of steps to run for benchmarking
            warmupSteps: Number of warm-up steps to ignore
            
        Returns:
            Tuple of (total_spikes, avg_firing_rate, avg_ms_per_step, steps_per_second)
        )pbdoc");

} // namespace nlm
