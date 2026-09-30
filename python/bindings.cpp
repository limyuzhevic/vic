#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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
#include "../src/core/Logger/Logger.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Advanced Edition
        -----------------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Provides advanced classes and APIs for brain simulation, agent control,
        and sophisticated neural computation with full access to Phase 6 features.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // ===== ADVANCED NEURON ID TYPES =====
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
        .def("to_string", [](const NeuronId& id) {
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

    // ===== ADVANCED NEURON TYPES =====
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Advanced neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .value("Plastic", NeuronType::Plastic)  // New: neurons that can change their type
        .value("Stem", NeuronType::Stem)      // New: developmental progenitor cells
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Advanced synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .value("Hebbian", SynapseType::Hebbian)      // Enhanced learning synapse
        .value("STDP", SynapseType::STDP)            // Spike timing dependent plasticity
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Advanced developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .value("PostChildhood", DevelopmentalStage::PostChildhood)  // Advanced stage
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .value("Adapting", FiringState::Adapting)      // New: adaptation state
        .value("Plastic", FiringState::Plastic)        // New: plastic state
        .export_values();

    // ===== ADVANCED ACTION TYPES =====
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Advanced action type enumeration)pbdoc")
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
        .value("Explore", ActionType::Explore)      // New: exploration behavior
        .value("Social", ActionType::Social)        // New: social interaction
        .value("Construct", ActionType::Construct)  // New: construction activity
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Advanced motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .value("Explore", MotorCommand::Explore)    // New: exploration command
        .value("Social", MotorCommand::Social)      // New: social command
        .value("Construct", MotorCommand::Construct) // New: construction command
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .value("Living", WorldObjectType::Living)    // New: living organism
        .value("Tool", WorldObjectType::Tool)        // New: tool/object
        .export_values();

    // ===== ADVANCED CONFIGURATION SYSTEM =====
    py::class_<Config>(m, "AdvancedConfig", R"pbdoc(Advanced configuration class for NLM system with enhanced capabilities)pbdoc")
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
            return "<AdvancedConfig: " + cfg.summary() + ">";
        })
        // ===== ADVANCED CONFIGURATION METHODS =====
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("setInt", [](Config& self, const std::string& key, int64_t value, ConfigSource source = ConfigSource::Default) {
            return self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default)
        .def("setFloat", [](Config& self, const std::string& key, float value, ConfigSource source = ConfigSource::Default) {
            return self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default)
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Default) {
            return self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default)
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Default) {
            return self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default)
        .def("get", [](Config& self, const std::string& key) {
            return self.getOr<std::string>(key, "");
        }, py::arg("key"))
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue = 0) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0)
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f)
        .def("getDouble", [](Config& self, const std::string& key, double defaultValue = 0.0) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0)
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue = false) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false)
        // ===== ADVANCED CONFIGURATION PRESETS =====
        .def("enableAdvancedFeatures", &Config::enableAdvancedFeatures,
             "Enable advanced NLM features")
        .def("setPerformanceMode", [](Config& self, const std::string& mode) {
            if (mode == "fast") return self.setPerformanceMode(Config::PerformanceMode::Fast);
            else if (mode == "balanced") return self.setPerformanceMode(Config::PerformanceMode::Balanced);
            else return self.setPerformanceMode(Config::PerformanceMode::Precision);
        }, py::arg("mode"))
        .def("setNeuralArchitecture", [](Config& self, const std::string& architecture) {
            if (architecture == "minimal") return self.setNeuralArchitecture(Config::NeuralArchitecture::Minimal);
            else if (architecture == "standard") return self.setNeuralArchitecture(Config::NeuralArchitecture::Standard);
            else return self.setNeuralArchitecture(Config::NeuralArchitecture::Complex);
        }, py::arg("architecture"))
        .def("setLearningStrategy", [](Config& self, const std::string& strategy) {
            if (strategy == "conservative") return self.setLearningStrategy(Config::LearningStrategy::Conservative);
            else if (strategy == "balanced") return self.setLearningStrategy(Config::LearningStrategy::Balanced);
            else return self.setLearningStrategy(Config::LearningStrategy::Aggressive);
        }, py::arg("strategy"));

    // ===== ADVANCED SENSORY INPUT TYPES =====
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for advanced sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "AdvancedVision", R"pbdoc(Advanced vision sensory input with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("enhanceContrast", &Vision::enhanceContrast, py::arg("factor"))
        .def("applyFilter", [](Vision& self, const std::string& filterType) {
            if (filterType == "gaussian") return self.applyFilter(Vision::FilterType::Gaussian);
            else if (filterType == "sobel") return self.applyFilter(Vision::FilterType::Sobel);
            else if (filterType == "edge") return self.applyFilter(Vision::FilterType::EdgeDetection);
            else return self.applyFilter(Vision::FilterType::None);
        }, py::arg("filterType"))
        .def("detectObjects", &Vision::detectObjects)
        .def("getFeatureMap", &Vision::getFeatureMap);

    py::class_<Audio, SensoryInput>(m, "AdvancedAudio", R"pbdoc(Advanced audio sensory input with signal processing)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("applyGain", &Audio::applyGain, py::arg("gainDb"))
        .def("applyFilter", &Audio::applyFilter, py::arg("filterType"))
        .def("detectFrequency", &Audio::detectFrequency, py::arg("frequency"))
        .def("getSpectrum", &Audio::getSpectrum);

    py::class_<InternalSignals, SensoryInput>(m, "AdvancedInternalSignals", R"pbdoc(Advanced internal state signals with homeostatic regulation)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getHomeostaticLevel", &InternalSignals::getHomeostaticLevel)
        .def("getStressLevel", &InternalSignals::getStressLevel)
        .def("reset", &InternalSignals::reset);

    // ===== ADVANCED ACTION SYSTEM =====
    py::class_<Action>(m, "AdvancedAction", R"pbdoc(Advanced action representation for motor output with complex parameters)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("getComplexity", &Action::getComplexity)
        .def("getDuration", &Action::getDuration)
        .def("setDuration", &Action::setDuration, py::arg("duration"))
        .def("getEnergyCost", &Action::getEnergyCost)
        .def("addParameter", &Action::addParameter, py::arg("name"), py::arg("value"))
        .def("getParameter", [](Action& self, const std::string& name) {
            return self.getParameter(name);
        }, py::arg("name"));

    // ===== ADVANCED WORLD OBJECT SYSTEM =====
    py::class_<WorldObject>(m, "AdvancedWorldObject", R"pbdoc(Advanced world object representation with dynamic properties)pbdoc")
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
        .def("setHealth", &WorldObject::setHealth, py::arg("health"))
        .def("getHealth", &WorldObject::getHealth)
        .def("setAge", &WorldObject::setAge, py::arg("age"))
        .def("getAge", &WorldObject::getAge)
        .def("update", [](WorldObject& self, double timestep) {
            self.update(timestep);
        }, py::arg("timestep"));

    // ===== ADVANCED AGENT BODY SYSTEM =====
    py::class_<AgentBody>(m, "AdvancedAgentBody", R"pbdoc(Advanced agent body state with comprehensive physics and state tracking)pbdoc")
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
        .def("updatePhysics", [](AgentBody& self, const std::vector<float>& forces, double timestep) {
            self.updatePhysics(forces, timestep);
        }, py::arg("forces"), py::arg("timestep"))
        .def("applyConstraint", &AgentBody::applyConstraint, py::arg("constraintType"), py::arg("value"))
        .def("getMomentum", &AgentBody::getMomentum)
        .def("getKineticEnergy", &AgentBody::getKineticEnergy);

    // ===== ADVANCED ACTION RESULT SYSTEM =====
    py::class_<ActionResult>(m, "AdvancedActionResult", R"pbdoc(Advanced action result with comprehensive reward and feedback)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("setIntrinsicReward", &ActionResult::setIntrinsicReward, py::arg("reward"))
        .def("getTotalReward", &ActionResult::getTotalReward)
        .def("addSecondaryReward", &ActionResult::addSecondaryReward, py::arg("reward"), py::arg("type"))
        .def("getSecondaryRewards", &ActionResult::getSecondaryRewards)
        .def("setLearningSignal", &ActionResult::setLearningSignal, py::arg("signal"));

    // ===== ADVANCED SENSORY PERCEPT SYSTEM =====
    py::class_<SensoryPercept>(m, "AdvancedSensoryPercept", R"pbdoc(Advanced sensory percept with rich feature extraction and processing)pbdoc")
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
        .def("extractFeatures", &SensoryPercept::extractFeatures)
        .def("getFeatureVector", &SensoryPercept::getFeatureVector)
        .def("normalize", &SensoryPercept::normalize)
        .def("enhance", &SensoryPercept::enhance, py::arg("amount"));

    // ===== ADVANCED WORLD SYSTEM =====
    py::class_<SimpleWorld>(m, "AdvancedWorld", R"pbdoc(Advanced world system with enhanced physics and dynamics)pbdoc")
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
        .def("setWorldDynamics", [](SimpleWorld& self, const std::string& dynamics) {
            if (dynamics == "static") return self.setWorldDynamics(SimpleWorld::DynamicsModel::Static);
            else if (dynamics == "elastic") return self.setWorldDynamics(SimpleWorld::DynamicsModel::Elastic);
            else if (dynamics == "fluid") return self.setWorldDynamics(SimpleWorld::DynamicsModel::Fluid);
            else if (dynamics == "gravity") return self.setWorldDynamics(SimpleWorld::DynamicsModel::Gravity);
            else return self.setWorldDynamics(SimpleWorld::DynamicsModel::None);
        }, py::arg("dynamics"))
        .def("getWorldDynamics", &SimpleWorld::getWorldDynamics)
        .def("setPhysicsEngine", &SimpleWorld::setPhysicsEngine, py::arg("engineType"))
        .def("getCollisionDetection", &SimpleWorld::getCollisionDetection)
        .def("getTerrain", &SimpleWorld::getTerrain)
        .def("setTerrain", &SimpleWorld::setTerrain, py::arg("terrainType"));

    // ===== ADVANCED BRAIN SYSTEM =====
    py::class_<Brain>(m, "AdvancedBrain", R"pbdoc(Advanced neural simulation brain class with Phase 6 capabilities)pbdoc")
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
        // ===== ADVANCED BRAIN CONTROL =====
        .def("enableAdvancedLearning", &Brain::enableAdvancedLearning, py::arg("enable"))
        .def("enableMetaLearning", &Brain::enableMetaLearning, py::arg("enable"))
        .def("enableSelfOptimization", &Brain::enableSelfOptimization, py::arg("enable"))
        .def("setLearningRate", &Brain::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &Brain::getLearningRate)
        .def("setPlasticitySchedule", &Brain::setPlasticitySchedule, py::arg("scheduleType"))
        .def("getPlasticityLevel", &Brain::getPlasticityLevel)
        .def("addAttentionFocus", &Brain::addAttentionFocus, py::arg("region"), py::arg("strength"))
        .def("getAttentionFocus", &Brain::getAttentionFocus)
        .def("clearAttentionFocus", &Brain::clearAttentionFocus)
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal)
        .def("getMemorySystem", &Brain::getMemorySystem,
             py::return_value_policy::reference_internal)
        .def("getNeuromodulationSystem", &Brain::getNeuromodulationSystem,
             py::return_value_policy::reference_internal)
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal);

    // ===== ADVANCED AGENT BRAIN SYSTEM =====
    py::class_<AgentBrain>(m, "AdvancedAgentBrain", R"pbdoc(Advanced agent brain interface connecting NLM brain to world with enhanced control)pbdoc")
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
        // ===== ADVANCED AGENT BRAIN CONFIGURATION =====
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("enableAdvancedLearning", &AgentBrain::enableAdvancedLearning,
             py::arg("enable"))
        .def("enableSelfRepair", &AgentBrain::enableSelfRepair,
             py::arg("enable"))
        .def("enableMetaLearning", &AgentBrain::enableMetaLearning,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("isAdvancedLearningEnabled", &AgentBrain::isAdvancedLearningEnabled)
        .def("isSelfRepairEnabled", &AgentBrain::isSelfRepairEnabled)
        .def("isMetaLearningEnabled", &AgentBrain::isMetaLearningEnabled)
        // ===== ADVANCED AGENT BRAIN CONTROL =====
        .def("setCuriosityThreshold", &AgentBrain::setCuriosityThreshold, py::arg("threshold"))
        .def("setExplorationRate", &AgentBrain::setExplorationRate, py::arg("rate"))
        .def("setNoveltyDecayRate", &AgentBrain::setNoveltyDecayRate, py::arg("decay"))
        .def("setSensoryScaling", &AgentBrain::setSensoryScaling,
             py::arg("vision"), py::arg("touch"), py::arg("internal"), py::arg("proprio"))
        .def("setMotorDecodingMode", &AgentBrain::setMotorDecodingMode, py::arg("mode"))
        .def("forceAction", &AgentBrain::forceAction, py::arg("command"))
        .def("addSensoryPattern", &AgentBrain::addSensoryPattern,
             py::arg("patternName"), py::arg("pattern"))
        .def("clearSensoryMemory", &AgentBrain::clearSensoryMemory)
        .def("setPlasticityRate", &AgentBrain::setPlasticityRate, py::arg("rate"))
        .def("setMemoryConsolidationRate", &AgentBrain::setMemoryConsolidationRate, py::arg("rate"))
        .def("setAttentionStrength", &AgentBrain::setAttentionStrength, py::arg("strength"))
        .def("getSensoryActivations", &AgentBrain::getSensoryActivations, py::arg("modality"))
        .def("getMotorCommandProbabilities", &AgentBrain::getMotorCommandProbabilities, py::arg("command"))
        .def("getAverageFiringRate", &AgentBrain::getAverageFiringRate)
        .def("getActionDiversity", &AgentBrain::getActionDiversity)
        .def("getLearningProgress", &AgentBrain::getLearningProgress)
        .def("getSurvivalScore", &AgentBrain::getSurvivalScore)
        .def("getStatus", &AgentBrain::getStatus)
        .def("isHealthy", &AgentBrain::isHealthy);

    // ===== ADVANCED FACTORY FUNCTIONS =====
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create an advanced configuration with enhanced features");

    m.def("createAdvancedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create an advanced brain with configuration");

    m.def("createAdvancedWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create an advanced world with enhanced physics");

    m.def("createAdvancedAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an advanced agent brain interface");

    // ===== ADVANCED UTILITY FUNCTIONS =====
    m.def("runPhase6Demo", &runPhase6Demo, "Run the advanced Phase 6 integration demo");

    m.def("validateConfiguration", [](const std::string& configPath) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configPath)) {
            return true;
        } else {
            throw std::runtime_error("Failed to load configuration from: " + configPath);
        }
    }, py::arg("configPath"), "Validate configuration file");

    m.def("benchmarkPerformance", [](int steps, int durationSeconds) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 16, 16);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        auto startTime = std::chrono::steady_clock::now();
        int spikeCount = 0;
        
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.001);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.1f, 0.05f);
            agent->updateDevelopment(0.1);
            
            spikeCount = brain->getTotalSpikeCount();
            
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (elapsed >= durationSeconds) {
                break;
            }
        }
        
        return std::make_tuple(spikeCount, brain->getAverageFiringRate(), brain->getTotalNeuronCount());
    }, py::arg("steps") = 1000, py::arg("durationSeconds") = 60,
       "Benchmark brain performance");

    // ===== ADVANCED CONSTANTS =====
    m.attr("PHASE6_DEMO_STEPS") = 10000;
    m.attr("ADVANCED_LEARNING_ENABLED") = true;
    m.attr("ADVANCED_NEURAL_DYNAMICS") = true;
    m.attr("ADVANCED_MEMORY_SYSTEM") = true;
    m.attr("ADVANCED_NEUROMODULATION") = true;
    m.attr("ADVANCED_DEVELOPMENT") = true;

    // ===== ADVANCED ERROR CLASSES =====
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        }
    });

    // ===== ADVANCED DOCUMENTATION =====
    m.doc() += R"pbdoc(

    Advanced NLM Python Bindings Features:
    ------------------------------------

    1. Enhanced Neural Architecture:
       - Advanced neuron types (Stem, Plastic, Progenitor)
       - Sophisticated synapse types (STDP, Hebbian, Electrical)
       - Developmental stage management

    2. Advanced Agent Control:
       - Meta-learning and self-repair capabilities
       - Curiosity-driven exploration with tunable parameters
       - Force action and pattern-based control

    3. Enhanced Configuration:
       - Advanced configuration presets
       - Performance mode selection
       - Neural architecture configuration

    4. Advanced World Physics:
       - Multiple physics engines
       - Enhanced collision detection
       - Dynamic terrain systems

    5. Sophisticated Sensory Processing:
       - Feature extraction and enhancement
       - Advanced object detection
       - Signal filtering and processing

    6. Comprehensive Performance Monitoring:
       - Benchmark utilities
       - Real-time performance metrics
       - Configuration validation

    7. Advanced Memory Management:
       - Memory consolidation control
       - Pattern storage and recall
       - Checkpoint management

    For detailed documentation, visit: https://nlm.readthedocs.io/advanced
    ";
    ";
    
    // ===== ADVANCED EXAMPLE =====
    m.def("exampleAdvancedAgent", []() {
        std::cout << "Advanced Agent Example:" << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "1. Create advanced configuration" << std::endl;
        std::cout << "2. Enable advanced learning features" << std::endl;
        std::cout << "3. Configure curiosity-driven exploration" << std::endl;
        std::cout << "4. Set up memory consolidation" << std::endl;
        std::cout << "5. Initialize brain with world interaction" << std::endl;
        std::cout << "6. Run simulation with advanced monitoring" << std::endl;
        std::cout << "7. Analyze learning progress and behavior" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            std::cout << "✓ Advanced brain initialized successfully" << std::endl;
            return "Advanced agent example completed successfully!";
        } else {
            throw std::runtime_error("Failed to initialize advanced brain");
        }
    }, "Run advanced agent example with all new features");
}

} // namespace nlm