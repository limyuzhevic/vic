#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for cleaner binding
namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
    
    // Type aliases for forward declarations
    using ConfigPtr = std::shared_ptr<Config>;
    using BrainPtr = std::shared_ptr<Brain>;
    using AgentBrainPtr = std::shared_ptr<AgentBrain>;
    using WorldPtr = std::shared_ptr<SimpleWorld>;
    using SensoryInputPtr = std::shared_ptr<SensoryInput>;
    using ActionPtr = std::shared_ptr<Action>;
    using WorldObjectPtr = std::shared_ptr<WorldObject>;
    using AgentBodyPtr = std::shared_ptr<AgentBody>;
    using ActionResultPtr = std::shared_ptr<ActionResult>;
    using SensoryPerceptPtr = std::shared_ptr<SensoryPercept>;
}

namespace py = pybind11;
namespace nlm {

// =============================================================================
// UTILITY FUNCTIONS AND HELPERS
// =============================================================================

// Type conversion utilities
template<typename T>
T pyVectorToNative(const py::list& pyList) {
    T result;
    result.reserve(pyList.size());
    for (const auto& item : pyList) {
        result.push_back(item.cast<typename T::value_type>());
    }
    return result;
}

// Factory function utilities
template<typename T, typename... Args>
std::unique_ptr<T> createObject(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// =============================================================================
// TYPE DEFINITIONS AND IDENTIFIERS
// =============================================================================

void bindIdentifiers(py::module_& m) {
    // NeuronId binding with enhanced functionality
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
            return std::to_string(id.value);
        });

    // SynapseId binding with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId binding with enhanced functionality
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId binding with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &NeuronId::operator!=);
}

// =============================================================================
// ENUM DEFINITIONS
// =============================================================================

void bindEnums(py::module_& m) {
    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .doc() = R"pbdoc(
            Neuron types:
            - Excitatory: Increases firing probability of post-synaptic neurons
            - Inhibitory: Decreases firing probability of post-synaptic neurons
            - Modulatory: Releases neuromodulators affecting broader network
            - Sensory: Processes external sensory inputs
            - Motor: Generates motor commands
            - Internal: Handles internal state and cognitive functions
        )pbdoc";

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .doc() = R"pbdoc(
            Synapse types:
            - Excitatory: Releases excitatory neurotransmitters
            - Inhibitory: Releases inhibitory neurotransmitters
            - Modulatory: Releases neuromodulators
            - Electrical: Direct electrical coupling
            - GapJunction: Ion channel coupling
        )pbdoc";

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .doc() = R"pbdoc(
            Developmental stages:
            - Initial: Newborn, high plasticity, basic reflexes
            - CriticalPeriod: Sensitive learning window for core abilities
            - Maturation: Development of complex cognitive functions
            - Adult: Peak cognitive performance
            - Aging: Age-related decline and adaptation
        )pbdoc";

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .doc() = R"pbdoc(
            Neuron firing states:
            - Resting: Below threshold, no activity
            - Active: Firing action potentials
            - Refractory: Cannot fire due to recent activity
            - Inhibited: Suppressed by inhibitory inputs
        )pbdoc";

    // ActionType enumeration
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
        .doc() = R"pbdoc(
            Action types for motor output:
            - MoveForward: Move agent forward
            - MoveBackward: Move agent backward
            - MoveLeft: Move agent left
            - MoveRight: Move agent right
            - TurnLeft: Rotate agent left
            - TurnRight: Rotate agent right
            - Look: Look in current direction
            - LookUp: Look upward
            - LookDown: Look downward
            - Interact: Interact with nearby object
            - Eat: Consume food/drink
            - Drink: Hydrate
            - Rest: Stop and rest
            - Wait: Stay in place
            - Custom: Custom action with parameters
        )pbdoc";

    // MotorCommand enumeration
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .doc() = R"pbdoc(
            World object types:
            - Empty: No object
            - Resource: Consumable resource
            - Hazard: Dangerous object
            - Wall: Impassable obstacle
            - Marker: Location reference
        )pbdoc";
}

// =============================================================================
// CONFIG CLASS BINDINGS
// =============================================================================

void bindConfigClass(py::module_& m) {
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
        .def("getInt", &Config::getOr<int64_t>, py::arg("key"), py::arg("default") = 0,
             "Get integer value with default")
        .def("getFloat", &Config::getOr<double>, py::arg("key"), py::arg("default") = 0.0,
             "Get float value with default")
        .def("getString", &Config::getOr<std::string>, py::arg("key"), py::arg("default") = "",
             "Get string value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::User,
             "Set configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

// =============================================================================
// SENSORY INPUT CLASSES BINDINGS
// =============================================================================

void bindSensoryInputClasses(py::module_& m) {
    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            auto data = self.getData();
            result["data"] = py::list(data.begin(), data.end());
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert to dictionary representation");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getPixel", [](const Vision& self, size_t x, size_t y, size_t c) {
            if (x >= self.getWidth() || y >= self.getHeight() || c >= self.getChannels()) {
                throw py::index_error("Pixel coordinates out of range");
            }
            return self.getData()[((y * self.getWidth() + x) * self.getChannels() + c)];
        }, py::arg("x"), py::arg("y"), py::arg("c") = 0,
           "Get individual pixel value (R,G,B or grayscale)");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData, "Get all signals")
        .def("getSignalCount", &InternalSignals::getDimensions, "Get number of signals");
}

// =============================================================================
// ACTION CLASSES BINDINGS
// =============================================================================

void bindActionClasses(py::module_& m) {
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](const Action& self, size_t index) {
            if (index >= self.getParameters().size()) {
                throw py::index_error("Parameter index out of range");
            }
            return self.getParameters()[index];
        }, py::arg("index"), "Get individual parameter value")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set individual parameter value")
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["name"] = self.getName();
            auto params = self.getParameters();
            result["parameters"] = py::list(params.begin(), params.end());
            return result;
        }, "Convert to dictionary representation");

    // WorldObject class
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
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isCloseTo", [](const WorldObject& self, float x, float y, float threshold) {
            float distance = self.distanceTo(x, y);
            return distance <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if close to position");

    // AgentBody class
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("move", [](AgentBody& self, float dx, float dy, float dt) {
            self.velocityX += dx / dt;
            self.velocityY += dy / dt;
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Move by displacement");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action was successful and beneficial")
        .def("isBad", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if action was unsuccessful or harmful");
}

// =============================================================================
// SENSORY PERCEPT CLASS BINDINGS
// =============================================================================

void bindSensoryPerceptClass(py::module_& m) {
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            self.setTimestamp(other.getTimestamp());
        }, py::arg("other"), "Merge percept data from another")
        .def("clear", &SensoryPercept::clear, "Clear all sensory data")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is present")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if audio data is present")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal signals are present")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if touch data is present");
}

// =============================================================================
// BRAIN CLASS BINDINGS
// =============================================================================

void bindBrainClass(py::module_& m) {
    // Brain class - main neural simulation engine
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
             "Log brain status")
        .def("getSpikeCountAtStep", [](Brain& self, SimulationStep step) {
            // This would require additional methods in the Brain class
            // For now, return 0 as a placeholder
            return size_t(0);
        }, py::arg("step"), "Get spike count at specific step (placeholder)");
}

// =============================================================================
// AGENT BRAIN CLASS BINDINGS
// =============================================================================

void bindAgentBrainClass(py::module_& m) {
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("simulateEpisode", [](AgentBrain& self, SimpleWorld& world, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, i * 0.001);
                self.updateDevelopment(0.001);
            }
        }, py::arg("world"), py::arg("steps"),
           "Simulate complete episode (placeholder implementation)");
}

// =============================================================================
// WORLD CLASS BINDINGS
// =============================================================================

void bindWorldClass(py::module_& m) {
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("simulateEpisode", [](SimpleWorld& self, AgentBrain& agentBrain, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                auto percept = self.getSensoryPercept();
                agentBrain.processSensoryInput(percept);
                auto action = agentBrain.decodeMotorCommand();
                self.applyMotorCommand(action, i * 0.001);
                agentBrain.updateDevelopment(0.001);
            }
            return self.getSensoryPercept();
        }, py::arg("agentBrain"), py::arg("steps"),
           "Simulate episode with agent brain");
}

// =============================================================================
// FACTORY FUNCTIONS
// =============================================================================

void bindFactoryFunctions(py::module_& m) {
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::return_value_policy::copy);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::copy);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world", py::return_value_policy::copy);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::copy);

    // Convenience functions for common use cases
    m.def("runSimpleExperiment", [](std::shared_ptr<Config> config, 
                                   std::shared_ptr<SimpleWorld> world,
                                   size_t steps) {
        auto brain = createBrain(config);
        auto agentBrain = createAgentBrain(brain);
        
        agentBrain->initialize(*world);
        
        for (size_t i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            auto action = agentBrain->decodeMotorCommand();
            world->applyMotorCommand(action, i * 0.001);
            agentBrain->updateDevelopment(0.001);
        }
        
        return std::make_pair(agentBrain, world);
    }, py::arg("config"), py::arg("world"), py::arg("steps"),
       "Run a simple experiment with brain, world, and agent");

    // Constants and utility values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Configuration utilities
    m.def("getDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Get a default configuration with sensible values");

    // Neural utility functions
    m.def("createRandomNetwork", [](size_t neuronCount, float connectionProbability) {
        auto config = createDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::User);
        config->set("connection_probability", connectionProbability, ConfigSource::User);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        return brain;
    }, py::arg("neuronCount"), py::arg("connectionProbability"),
       "Create a random neural network with specified parameters");
}

// =============================================================================
// MAIN MODULE INITIALIZATION
// =============================================================================

PYBIND11_MODULE(pynlm, m) {
    // Set module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides Python access to the NLM neural simulation framework,
        enabling creation and control of artificial brains with spiking neurons,
        synaptic plasticity, and cognitive capabilities.
        
        Key Features:
        - Real spiking neural networks with LIF dynamics
        - Multiple plasticity rules (STDP, Hebbian, reward-modulated)
        - Structural plasticity (synaptogenesis and pruning)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Memory systems (working, episodic, associative)
        - Prediction and planning capabilities
        - Sensory and motor processing
        - Development stages affecting plasticity
        - Save/load functionality for persistent brains
        
        Quick Start Example:
        ```python
        import pynlm as nlm
        
        # Create brain with default configuration
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = nlm.createSimpleWorld()
        
        # Create agent brain interface
        agent_brain = nlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run simulation
        for i in range(1000):
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(action, i * 0.001)
            agent_brain.updateDevelopment(0.001)
        ```
        
        For more examples and advanced usage, see:
        - documentation/ARCHITECTURE.md
        - examples/ (if available)
        - tutorials/ (if available)
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all components in logical order
    bindIdentifiers(m);
    bindEnums(m);
    bindConfigClass(m);
    bindSensoryInputClasses(m);
    bindActionClasses(m);
    bindSensoryPerceptClass(m);
    bindBrainClass(m);
    bindAgentBrainClass(m);
    bindWorldClass(m);
    bindFactoryFunctions(m);

    // Add module-level constants
    m.attr("VERSION") = "0.1.0";
    m.attr("PHASE") = "6 - Final Integration";
    m.attr("MAX_SUPPORTED_NEURONS") = 100000;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;

    // Add module-level utility functions
    m.def("getVersion", []() { return "0.1.0"; },
          "Get the NLM version");

    m.def("printBanner", []() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     Ready for Python integration.                             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }, "Print NLM banner");
}

} // namespace nlm

// End of pybind11 module definition