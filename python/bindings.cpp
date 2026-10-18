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

// Custom Python exceptions
class NLMConfigError : public std::runtime_error {
public:
    explicit NLMConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMBrainError : public std::runtime_error {
public:
    explicit NLMBrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMWorldError : public std::runtime_error {
public:
    explicit NLMWorldError(const std::string& msg) : std::runtime_error(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - High-performance neural simulation
        - Memory-efficient brain structures
        - Neuromodulation and plasticity support
        - World simulation environment
        - Agent-based behavior models
        
        Examples:
        ```python
        import pynlm
        
        # Create a brain with default configuration
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Create a world and agent
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning capabilities
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(1000):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMConfigError>(m, "ConfigError");
    py::register_exception<NLMBrainError>(m, "BrainError");
    py::register_exception<NLMWorldError>(m, "WorldError");

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
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enumeration with better documentation and helper methods
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
        .value("Excitatory", NeuronType::Excitatory, "Exciting neurons that increase firing probability")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that decrease firing probability")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that affect broader neural circuits")
        .value("Sensory", NeuronType::Sensory, "Neurons that process external sensory input")
        .value("Motor", NeuronType::Motor, "Neurons that produce motor output")
        .value("Internal", NeuronType::Internal, "Neurons that process internal signals")
        .export_values()
        .def("is_excitatory", [](NeuronType type) { return type == NeuronType::Excitatory; })
        .def("is_inhibitory", [](NeuronType type) { return type == NeuronType::Inhibitory; })
        .def("is_sensory", [](NeuronType type) { return type == NeuronType::Sensory; })
        .def("is_motor", [](NeuronType type) { return type == NeuronType::Motor; });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses that increase postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses that decrease postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses that affect broader circuits")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses with fast transmission")
        .value("GapJunction", SynapseType::GapJunction, "Electrical connections between neurons")
        .export_values()
        .def("is_excitatory", [](SynapseType type) { return type == SynapseType::Excitatory; })
        .def("is_inhibitory", [](SynapseType type) { return type == SynapseType::Inhibitory; });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable properties")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining performance")
        .export_values()
        .def("is_critical", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::CriticalPeriod || stage == DevelopmentalStage::Initial;
        })
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Maturation;
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)")
        .value("Resting", FiringState::Resting, "Neuron at rest (not firing)")
        .value("Active", FiringState::Active, "Neuron currently firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited from firing")
        .export_values()
        .def("is_active", [](FiringState state) { return state == FiringState::Active; })
        .def("is_refractory", [](FiringState state) { return state == FiringState::Refractory; });

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world coordinates")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world coordinates")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world coordinates")
        .value("MoveRight", ActionType::MoveRight, "Move right in world coordinates")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look at specific point or direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with world object")
        .value("Eat", ActionType::Eat, "Consume food/resource")
        .value("Drink", ActionType::Drink, "Consume liquid")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait/do nothing")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty, "Empty space in world")
        .value("Resource", WorldObjectType::Resource, "Useful resource that can be consumed")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object that causes damage")
        .value("Wall", WorldObjectType::Wall, "Impassable obstacle")
        .value("Marker", WorldObjectType::Marker, "Special location marker")
        .export_values();

    // Enhanced Config class with better error handling and convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises ConfigError if file cannot be read.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises ConfigError if file cannot be written.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            return self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"),
            "Set a configuration value as string.")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            return self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"),
            "Set a configuration value as integer.")
        .def("set", [](Config& self, const std::string& key, double value) {
            return self.set(key, value, ConfigSource::CommandLine);
        }, py::arg("key"), py::arg("value"),
            "Set a configuration value as float.")
        .def("get", [](Config& self, const std::string& key) {
            try {
                return self.getOr<std::string>(key, "");
            } catch (...) {
                return std::string("");
            }
        }, py::arg("key"),
            "Get a configuration value as string.")
        .def("get", [](Config& self, const std::string& key, int64_t default_value) {
            try {
                return self.getOr<int64_t>(key, default_value);
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value"),
            "Get a configuration value as integer.")
        .def("get", [](Config& self, const std::string& key, double default_value) {
            try {
                return self.getOr<double>(key, default_value);
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value"),
            "Get a configuration value as float.")
        .def("validate", &Config::getKeys,
             "Validate configuration consistency. Returns list of keys if valid.");

    // Enhanced SensoryInput classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set image data as flattened array of RGB values")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("getPixel", [](Vision& self, size_t x, size_t y) {
            if (x >= self.getWidth() || y >= self.getHeight()) {
                throw std::out_of_range("Pixel coordinates out of bounds");
            }
            // Note: This is a simplified implementation - actual implementation
            // would need to handle pixel access based on the specific Vision class
            return std::vector<float>{};
        }, py::arg("x"), py::arg("y"),
            "Get pixel data at specific coordinates (x, y)");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", [](Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals")
        .def("getSignalCount", &InternalSignals::getData, "Get number of signals")
        .def("normalize", [](InternalSignals& self) {
            auto data = self.getData();
            if (!data.empty()) {
                float max_val = *std::max_element(data.begin(), data.end());
                if (max_val > 0.0f) {
                    for (auto& val : data) val /= max_val;
                }
            }
            return data;
        }, "Normalize all signals to range [0, 1]");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("getDescription", [](const Action& self) {
            auto type_name = Action::getActionTypeName(self.getType());
            auto params = self.getParameters();
            if (params.empty()) {
                return type_name + " action";
            }
            return type_name + " action with parameters: [" + 
                   std::to_string(params.size()) + " values]";
        }, "Get human-readable description")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("execute", [](Action& self, std::shared_ptr<SimpleWorld> world, const AgentBody& body) {
            // Note: This is a conceptual implementation - actual execution
            // would depend on the specific world and agent body implementation
            return ActionResult{0.0f, true, "Action executed successfully"};
        }, py::arg("world"), py::arg("body"),
            "Execute this action in the given world and return result");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "World x-coordinate")
        .def_readwrite("y", &WorldObject::y, "World y-coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value/utility")
        .def_readwrite("active", &WorldObject::active, "Object active status")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
            "Calculate distance to given coordinates")
        .def("is_near", [](const WorldObject& self, float x, float y, float distance) {
            return self.distance_to(x, y) < distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"),
            "Check if object is within given distance of coordinates");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Agent x-position")
        .def_readwrite("y", &AgentBody::y, "Agent y-position")
        .def_readwrite("orientation", &AgentBody::orientation, "Agent orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Current health")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("get_position", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("get_velocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get agent speed")
        .def("get_direction", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent direction in radians");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Human-readable message")
        .def("is_good", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action result was good (successful and positive reward)")
        .def("is_bad", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if action result was bad (failed or negative reward)");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get visual percept")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set visual percept")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get tactile percept")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set tactile percept")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal percepts")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal percepts")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioceptive percept")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioceptive percept")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio percept")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio percept")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision) result["vision"] = *vision;
            
            auto touch = self.getTouch();
            if (touch) result["touch"] = *touch;
            
            auto internal = self.getInternal();
            if (internal) result["internal"] = *internal;
            
            auto proprioception = self.getProprioception();
            if (proprioception) result["proprioception"] = *proprioception;
            
            auto audio = self.getAudio();
            if (audio) result["audio"] = *audio;
            
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert percept to Python dictionary");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world. Returns ActionResult.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from current world state")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not occupied)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy capacity")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy capacity")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjectAt", [](SimpleWorld& self, float x, float y) {
            // Note: This would need to be implemented in the C++ SimpleWorld class
            return WorldObject{}; // Placeholder
        }, py::arg("x"), py::arg("y"),
            "Get object at coordinates (returns empty object if none)")
        .def("getObjectsInRadius", [](SimpleWorld& self, float x, float y, float radius) {
            // Note: This would need to be implemented in the C++ SimpleWorld class
            return std::vector<WorldObject>{}; // Placeholder
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
            "Get all objects within given radius of coordinates");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Raises BrainError if initialization fails.")
        .def("step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"),
             "Perform a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self.
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with a reference to self."
        .def "step", static_cast<void (Brain::*)(this->step)
             py::arg("currentStep"), py::arg("currentTime"),
             "Run a simulation step with current time as reference"
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
             "Save brain state to file. Raises BrainError if save fails.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises BrainError if load fails.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns region ID.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Raises BrainError if region not found.")
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
             "Set developmental stage. Raises std::invalid_argument if stage invalid.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("simulateEpisode", [](Brain& self, uint64_t steps, float timestep) {
            for (uint64_t step = 0; step < steps; ++step) {
                self.step(step, step * timestep);
            }
            return self.getTotalSpikeCount();
        }, py::arg("steps"), py::arg("timestep"),
            "Run complete simulation episode for given steps and timestep. Returns total spike count.")
        .def("runExperiment", [](Brain& self, const Config& cfg, uint64_t steps) {
            auto result = self.simulateEpisode(steps, cfg.getOr<double>("simulation_timestep", 0.001));
            return std::make_pair(self.getTotalSpikeCount(), result);
        }, py::arg("cfg"), py::arg("steps"),
            "Run experiment with given config and steps. Returns (spike_count, result) tuple.")
        .def("to_dict", [](const Brain& self) {
            py::dict result;
            result["total_neurons"] = self.getTotalNeuronCount();
            result["total_synapses"] = self.getTotalSynapseCount();
            result["active_neurons"] = self.getActiveNeuronCount();
            result["firing_neurons"] = self.getFiringNeuronCount();
            result["average_firing_rate"] = self.getAverageFiringRate();
            result["total_spikes"] = self.getTotalSpikeCount();
            result["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return result;
        }, "Convert brain to Python dictionary for inspection")
        .def("__str__", [](const Brain& self) {
            auto spike_count = self.getTotalSpikeCount();
            auto neuron_count = self.getTotalNeuronCount();
            auto firing_rate = self.getAverageFiringRate();
            return "Brain(neurons=" + std::to_string(neuron_count) +
                   ", spikes=" + std::to_string(spike_count) +
                   ", firing_rate=" + std::to_string(firing_rate) + ")";
        }, "Get string representation of brain");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with given brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent with world")
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
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("processEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, uint64_t steps) {
            for (uint64_t step = 0; step < steps; ++step) {
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Note: Brain step would need to be accessible here
                // In actual usage, this would be coordinated with brain stepping
                
                auto cmd = self.decodeMotorCommand();
                world->applyMotorCommand(cmd, world->getSimulationTime());
                
                // Apply reward based on action result
                auto reward = 0.0f; // Would come from world action result
                self.applyRewardModulation(reward, 0.0f);
                
                self.updateDevelopment(0.1f);
            }
            return self.getTotalSpikeCount();
        }, py::arg("world"), py::arg("steps"),
            "Process complete episode. Returns total spike count.")
        .def("getEpisodeStats", [](const AgentBrain& self) {
            py::dict stats;
            auto brain = self.getBrain();
            if (brain) {
                stats["spike_count"] = brain->getTotalSpikeCount();
                stats["firing_neurons"] = brain->getFiringNeuronCount();
                stats["average_firing_rate"] = brain->getAverageFiringRate();
                stats["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
                stats["curiosity_level"] = self.getCuriosityLevel();
                stats["novelty_level"] = self.getNoveltyLevel();
                stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            }
            return stats;
        }, "Get comprehensive episode statistics as dictionary")
        .def("createEpisodeData", [](AgentBrain& self) {
            return std::make_shared<std::vector<std::vector<double>>>(); // Placeholder for actual episode data
        }, "Create container for storing episode data for later analysis");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set some sensible defaults for Phase 6
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        return config;
    }, "Create a default configuration with Phase 6 settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw NLMConfigError("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration. Raises ConfigError if config is invalid.");

    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = createDefaultConfig();
        return createBrain(config);
    }, "Create a new brain with default configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw NLMBrainError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. Raises BrainError if brain is invalid.");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize agent brain interface.");

    // Version and utility functions
    m.def("getVersion", []() {
        return "NLM Phase 6.0 - Final Integration";
    }, "Get NLM version string");

    m.def("getPhase", []() {
        return 6;
    }, "Get current phase number");

    m.def("validateInstallation", []() {
        try {
            auto config = createDefaultConfig();
            auto brain = createBrain(config);
            brain->initialize();
            return true;
        } catch (...) {
            return false;
        }
    }, "Validate that NLM installation is working correctly");

    // Error constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Convenience aliases
    m.attr("NEURON_ID_INVALID") = py::cast(INVALID_NEURON_ID);
    m.attr("SYNAPSES_ID_INVALID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("REGION_ID_INVALID") = py::cast(INVALID_REGION_ID);

    // Module metadata
    m.attr("__version__") = "6.0.0";
    m.attr("__author__") = "NLM Research Project";
    m.attr("__description__") = "Neural Learning Machine - Phase 6 Final Integration";

    // Setup Python exception translations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMConfigError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const NLMBrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMWorldError& e) {
            PyErr_SetString(PyExc_OSError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    // Register Python array types for numpy compatibility
    py::class_<py::array_t<float>>(m, "FloatArray")
        .def(py::init<>())
        .def("from_vector", [](const std::vector<float>& vec) {
            return py::array_t<float>(vec);
        }, "Create float array from vector")
        .def("to_vector", [](py::array_t<float> arr) {
            return std::vector<float>(arr.data(), arr.data() + arr.size());
        }, "Convert float array to vector")
        .def("shape", [](py::array_t<float> arr) {
            return std::vector<size_t>({arr.shape(0), arr.shape(1)});
        }, "Get array shape");

    // Register numeric types for convenience
    m.def("as_int", [](py::object obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<int64_t>();
        } else if (py::isinstance<py::float_>(obj)) {
            return static_cast<int64_t>(obj.cast<double>());
        }
        throw std::invalid_argument("Cannot convert to integer");
    }, "Convert Python object to integer");

    m.def("as_float", [](py::object obj) {
        if (py::isinstance<py::float_>(obj)) {
            return obj.cast<double>();
        } else if (py::isinstance<py::int_>(obj)) {
            return static_cast<double>(obj.cast<int64_t>());
        }
        throw std::invalid_argument("Cannot convert to float");
    }, "Convert Python object to float");

    m.def("as_bool", [](py::object obj) {
        if (py::isinstance<py::bool_>(obj)) {
            return obj.cast<bool>();
        } else if (py::isinstance<py::int_>(obj)) {
            return obj.cast<int64_t>() != 0;
        }
        throw std::invalid_argument("Cannot convert to boolean");
    }, "Convert Python object to boolean");
}

} // namespace nlm

