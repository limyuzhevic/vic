#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>

namespace py = pybind11;
namespace nlm {

// Helper function to convert exceptions to Python exceptions
py::exception<std::runtime_error> registerRuntimeError(py::module& m) {
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    return py::exception<std::runtime_error>(m, "RuntimeError");
}

// Template for ID class bindings - reduces code duplication
template<typename T, typename Tag>
void bindIdClass(py::module& m, const std::string& name, 
                const std::string& doc = "Unique identifier class") {
    using ValueType = uint64_t;
    
    py::class_<T>(m, name.c_str(), doc.c_str())
        .def(py::init<>())
        .def(py::init<ValueType>(), py::arg("value"), "Create ID from uint64_t value")
        // Pythonic property access
        .def_property("value", 
                     [](const T& id) -> ValueType { return id.value; },
                     [](T& id, ValueType v) { id.value = v; },
                     "The internal uint64_t value of this ID")
        .def_property_readonly("index", 
                              [](const T& id) -> ValueType { return id.index(); },
                              "Return the index (equivalent to value) of this ID")
        .def("is_valid", &T::value, 
             "Check if this is not the invalid ID sentinel")
        .def("__eq__", static_cast<bool (*)(const T&, const T&)>(&T::operator==))
        .def("__ne__", static_cast<bool (*)(const T&, const T&)>(&T::operator!=))
        .def("__lt__", static_cast<bool (*)(const T&, const T&)>(&T::operator<))
        .def("__le__", [](const T& a, const T& b) { return a.value <= b.value; })
        .def("__gt__", [](const T& a, const T& b) { return a.value > b.value; })
        .def("__ge__", [](const T& a, const T& b) { return a.value >= b.value; })
        .def("__hash__", [](const T& id) { return std::hash<ValueType>{}(id.value); })
        .def("__repr__", [](const T& id) {
            std::string tag_str;
            if (std::is_same_v<T, NeuronId>) tag_str = "NeuronId";
            else if (std::is_same_v<T, SynapseId>) tag_str = "SynapseId";
            else if (std::is_same_v<T, RegionId>) tag_str = "RegionId";
            else if (std::is_same_v<T, PopulationId>) tag_str = "PopulationId";
            else tag_str = "ID";
            return "<" + tag_str + ": " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const T& id) {
            std::string tag_str;
            if (std::is_same_v<T, NeuronId>) tag_str = "NeuronId";
            else if (std::is_same_v<T, SynapseId>) tag_str = "SynapseId";
            else if (std::is_same_v<T, RegionId>) tag_str = "RegionId";
            else if (std::is_same_v<T, PopulationId>) tag_str = "PopulationId";
            else tag_str = "ID";
            return tag_str + "(" + std::to_string(id.value) + ")";
        });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Automated class bindings with consistent Pythonic API
        - Comprehensive error handling with exception translation
        - Safe memory management with proper ownership policies
        - Rich documentation and type hints
        - Utility functions for common operations
        - Consistent behavior across all ID classes
    )pbdoc";

    // Register exception types
    auto RuntimeError = registerRuntimeError(m);

    // Bind ID classes using template - DRY approach
    bindIdClass<NeuronId>(m, "NeuronId", "Unique identifier for a neuron");
    bindIdClass<SynapseId>(m, "SynapseId", "Unique identifier for a synapse");
    bindIdClass<RegionId>(m, "RegionId", "Unique identifier for a brain region");
    bindIdClass<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population");

    // Bind invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Bind enum types with better documentation
    py::enum_<NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", NeuronType::Excitatory, "Excites postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibits postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Modulates neural excitability")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron detecting external stimuli")
        .value("Motor", NeuronType::Motor, "Motor neuron producing actions")
        .value("Internal", NeuronType::Internal, "Internal regulatory neuron")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synaptic transmission")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synaptic transmission")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory transmission")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse (gap junction)")
        .value("GapJunction", SynapseType::GapJunction, "Electrical gap junction")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for plasticity")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", "Neuron firing state enumeration")
        .value("Resting", FiringState::Resting, "Not firing, resting state")
        .value("Active", FiringState::Active, "Currently firing")
        .value("Refractory", FiringState::Refractory, "Cannot fire due to refractory period")
        .value("Inhibited", FiringState::Inhibited, "Temporarily inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", "Action type enumeration")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look (default direction)")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat object")
        .value("Drink", ActionType::Drink, "Drink liquid")
        .value("Rest", ActionType::Rest, "Rest (no action)")
        .value("Wait", ActionType::Wait, "Wait for next step")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate left")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate right")
        .value("LookLeft", MotorCommand::LookLeft, "Pan sensor left")
        .value("LookRight", MotorCommand::LookRight, "Pan sensor right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "No movement")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Positive reward item")
        .value("Hazard", WorldObjectType::Hazard, "Negative reward/harm")
        .value("Wall", WorldObjectType::Wall, "Barrier/blocker")
        .value("Marker", WorldObjectType::Marker, "Navigation marker")
        .export_values();

    // Config class with better error handling
    py::class_<Config>(m, "Config", 
                      R"pbdoc(Configuration class for NLM system
                      
                      Manages simulation configuration from files, command line, or runtime.
                      Supports JSON files, command line arguments, and runtime settings.
                      Provides safe value access with type conversion and defaults.
                      )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, False on error.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments. Returns True on success, False on error.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, False on error.")
        // Get methods with error checking
        .def("get", [](const Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            if (result) {
                return *result;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get a string value from config. Raises KeyError if not found.")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto result = self.get<int>(key);
            if (result) {
                return *result;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get an integer value from config. Raises KeyError if not found.")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto result = self.get<double>(key);
            if (result) {
                return *result;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get a float value from config. Raises KeyError if not found.")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto result = self.get<bool>(key);
            if (result) {
                return *result;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"), "Get a boolean value from config. Raises KeyError if not found.")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get value with string default")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class with better error handling
    py::class_<SensoryInput>(m, "SensoryInput",
                            "Base class for sensory input from the environment")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input as string")
        .def("getData", &SensoryInput::getData, "Get the raw data as a numpy array")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of this input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input (seconds since epoch)");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input (grayscale camera)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create Vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set image data (flattened row-major)")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of channels (1=grayscale, 3=RGB)");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input (sound signals)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio input with sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set audio sample rate (Hz)")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate (Hz)")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    // InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal signals sensory input (from brain state)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Action class with better Python API
    py::class_<Action>(m, "Action",
                      R"pbdoc(Action representation for motor output
                      
                      Represents an action that the agent can take in the environment.
                      Actions can be predefined (MoveForward, Look) or custom with parameters.
                      )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with type only")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def_property("type", &Action::getType, &Action::setType, "Action type (ActionType enum)")
        .def_property("parameters", &Action::getParameters, &Action::setParameters,
                     "Action parameters as list of floats")
        .def_property_readonly("name", &Action::getName, "Human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " (type=" + std::to_string(static_cast<int>(action.getType())) + ")>";
        });

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject",
                            "World object representation (position, type, properties)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object with position and properties")
        .def_property("x", &WorldObject::x, nullptr, "X coordinate")
        .def_property("y", &WorldObject::y, nullptr, "Y coordinate")
        .def_property("radius", &WorldObject::radius, nullptr, "Radius of circular object")
        .def_property("type", &WorldObject::type, nullptr, "Type of object (WorldObjectType)")
        .def_property("value", &WorldObject::value, nullptr, "Value/reward associated with object")
        .def_property("active", &WorldObject::active, nullptr, "Whether object is active in simulation");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", "Agent body state (physical properties)")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", "Result from world after applying an action")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received (can be negative)")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Human-readable message about result");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data (what agent perceives)")
        .def(py::init<>())
        .def_property("vision", &SensoryPercept::getVision, &SensoryPercept::setVision,
                     "Vision data as list of floats (row-major)")
        .def_property("touch", &SensoryPercept::getTouch, &SensoryPercept::setTouch,
                     "Touch/proximity data as list of floats")
        .def_property("internal", &SensoryPercept::getInternal, &SensoryPercept::setInternal,
                     "Internal state signals as list of floats")
        .def_property("proprioception", &SensoryPercept::getProprioception, &SensoryPercept::setProprioception,
                     "Proprioception (body position/velocity) as list of floats")
        .def_property("audio", &SensoryPercept::getAudio, &SensoryPercept::setAudio,
                     "Audio signals as list of floats (if audio enabled)")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals concatenated (for brain input)")
        .def_property("timestamp", &SensoryPercept::getTimestamp, &SensoryPercept::setTimestamp,
                     "Timestamp of this percept (seconds since epoch)");

    // SimpleWorld class with improved error handling
    py::class_<SimpleWorld>(m, "SimpleWorld",
                            "Simple 2D world for NLM simulation (environmental interface)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision field of view")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent. Returns ActionResult with reward.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::copy,
             "Get current sensory percept for the agent (copy for safety)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy,
             "Get agent body state (copy for safety)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall or obstacle)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::copy,
             "Get object at position (if any)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Brain class with improved error handling and documentation
    py::class_<Brain>(m, "Brain",
                     R"pbdoc(Central neural simulation brain class
                     
                     The brain implements real spiking neural computation with event-driven dynamics
                     and integrated memory, prediction, cognition, and neuromodulation systems.
                     
                     Key features:
                     - Event-driven LIF neuron dynamics
                     - Spike timing dependent plasticity (STDP)
                     - Reward prediction error learning
                     - Working, episodic, and associative memory
                     - Neuromodulation (dopamine, curiosity, novelty)
                     - Developmental stages and plasticity
                     - Prediction and planning systems
                     )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
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
             "Inject current into a specific neuron (mA)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (mA)")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset, "Reset brain state to initial")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True on success, False on error.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True on success, False on error.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns RegionId.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::copy,
             "Get a region by ID (copy for safety)")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::copy,
             "Get all regions (copy for safety)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate (Hz)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::copy,
             "Get the configuration (copy for safety)")
        .def("logStatus", &Brain::logStatus, "Log brain status to output");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain",
                           "Agent brain interface connecting NLM brain to world")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
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
             py::return_value_policy::copy,
             "Get the underlying brain (copy for safety)")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development system")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity system")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Utility functions
    m.def("generateRandomId", []() -> uint64_t {
        return static_cast<uint64_t>(rand()) << 32 | static_cast<uint64_t>(rand());
    }, "Generate a random ID (not a valid ID)");

    m.def("createIdFromString", [](const std::string& str) -> uint64_t {
        std::hash<std::string> hasher;
        return hasher(str);
    }, py::arg("str"), "Create a deterministic ID from string hash");

    m.def("isValidId", [](uint64_t value) -> bool {
        return value != UINT64_MAX;
    }, py::arg("value"), "Check if a raw ID value is valid (not INVALID_ID)");

    m.def("serializeId", [](uint64_t id) {
        return py::bytes(reinterpret_cast<const char*>(&id), sizeof(id));
    }, py::arg("id"), "Serialize ID to bytes for storage/transfer");

    m.def("deserializeId", [](py::bytes data) -> uint64_t {
        if (data.size() != sizeof(uint64_t)) {
            throw std::runtime_error("Invalid serialized ID size");
        }
        uint64_t id;
        std::memcpy(&id, data.data(), sizeof(uint64_t));
        return id;
    }, py::arg("data"), "Deserialize ID from bytes");

    // Convenience functions for common operations
    m.def("normalizeActionParameters", [](const std::vector<float>& params, float min = -1.0f, float max = 1.0f) {
        std::vector<float> normalized;
        normalized.reserve(params.size());
        for (float p : params) {
            float norm = (p - min) / (max - min);
            normalized.push_back(norm);
        }
        return normalized;
    }, py::arg("params"), py::arg("min") = -1.0f, py::arg("max") = 1.0f,
        "Normalize action parameters to [0, 1] range");

    m.def("denormalizeActionParameters", [](const std::vector<float>& normalized, float min = -1.0f, float max = 1.0f) {
        std::vector<float> denormalized;
        denormalized.reserve(normalized.size());
        for (float n : normalized) {
            float denorm = n * (max - min) + min;
            denormalized.push_back(denorm);
        }
        return denormalized;
    }, py::arg("normalized"), py::arg("min") = -1.0f, py::arg("max") = 1.0f,
        "Denormalize action parameters back to original range");

    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__license__") = "MIT";

    // Exception types for Python
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
            PyErr_SetString(PyExc_RuntimeError, "Unknown exception occurred");
        }
    });

    // Add type hints and module metadata
    py::class_<py::none>(m, "NoneType")
        .def(py::init<>())
        .def("__repr__", [](py::none) { return "None"; });

    // Add common Python types as aliases for convenience
    py::class_<std::vector<float>>(m, "FloatArray")
        .def(py::init<>())
        .def(py::init<size_t>())
        .def(py::init<std::vector<float>>())
        .def("__len__", [](const std::vector<float>& v) { return v.size(); })
        .def("__getitem__", [](const std::vector<float>& v, size_t i) {
            return v.at(i);
        })
        .def("__setitem__", [](std::vector<float>& v, size_t i, float val) {
            v.at(i) = val;
        })
        .def("append", [](std::vector<float>& v, float val) {
            v.push_back(val);
        })
        .def("clear", [](std::vector<float>& v) {
            v.clear();
        })
        .def("copy", [](const std::vector<float>& v) {
            return v;
        })
        .def("__repr__", [](const std::vector<float>& v) {
            std::stringstream ss;
            ss << "["; 
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << v[i];
            }
            ss << "]";
            return ss.str();
        });

    // Add common numeric types
    py::class_<float>(m, "Float")
        .def(py::init<>())
        .def(py::init<double>());

    py::class_<int>(m, "Int")
        .def(py::init<>())
        .def(py::init<long long>());

    // Add dictionary type for config access
    py::class_<std::map<std::string, std::string>>(m, "StringDict")
        .def(py::init<>())
        .def("__getitem__", [](const std::map<std::string, std::string>& d, const std::string& key) {
            auto it = d.find(key);
            if (it == d.end()) {
                throw py::key_error("Key not found: " + key);
            }
            return it->second;
        })
        .def("__setitem__", [](std::map<std::string, std::string>& d, const std::string& key, const std::string& value) {
            d[key] = value;
        })
        .def("__contains__", [](const std::map<std::string, std::string>& d, const std::string& key) {
            return d.find(key) != d.end();
        })
        .def("keys", [](const std::map<std::string, std::string>& d) {
            py::list keys;
            for (const auto& kv : d) {
                keys.append(kv.first);
            }
            return keys;
        })
        .def("values", [](const std::map<std::string, std::string>& d) {
            py::list values;
            for (const auto& kv : d) {
                values.append(kv.second);
            }
            return values;
        })
        .def("items", [](const std::map<std::string, std::string>& d) {
            py::list items;
            for (const auto& kv : d) {
                py::tuple item = py::make_tuple(kv.first, kv.second);
                items.append(item);
            }
            return items;
        });

    // Add tuple type
    py::class_<std::pair<std::string, std::string>>(m, "Pair")
        .def(py::init<>())
        .def(py::init<const std::string&, const std::string&>())
        .def("first", &std::pair<std::string, std::string>::first)
        .def("second", &std::pair<std::string, std::string>::second);
}

} // namespace nlm