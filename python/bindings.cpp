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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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
        .def_readwrite("active", &WorldObject::active);

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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
             "Log brain status");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Factory functions for convenience
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Convenience wrapper for common setup pattern
    m.def("createBrainFromConfig", [](const std::string& config_file) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(config_file)) {
            throw std::runtime_error("Failed to load config file: " + config_file);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config_file"), "Create brain from configuration file");

    // Common configuration presets
    m.def("createBrainPreset", [](const std::string& preset_name, int neuron_count) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::User);
        config->set("brain.synapse_density", 0.1f, ConfigSource::User);
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        
        if (preset_name == "simple") {
            config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::User);
            config->set("plasticity.stdp.enable", true, ConfigSource::User);
        } else if (preset_name == "complex") {
            config->set("brain.neuron_count", static_cast<int64_t>(5000), ConfigSource::User);
            config->set("plasticity.stdp.enable", true, ConfigSource::User);
            config->set("plasticity.hebbian.enable", true, ConfigSource::User);
            config->set("plasticity.structural.enable", true, ConfigSource::User);
            config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::User);
            config->set("neuromod.curiosity.enable", true, ConfigSource::User);
            config->set("neuromod.novelty.enable", true, ConfigSource::User);
        } else if (preset_name == "research") {
            config->set("brain.neuron_count", static_cast<int64_t>(10000), ConfigSource::User);
            config->set("plasticity.stdp.enable", true, ConfigSource::User);
            config->set("plasticity.hebbian.enable", true, ConfigSource::User);
            config->set("plasticity.structural.enable", true, ConfigSource::User);
            config->set("neuromod.dopamine.scale", 2.0f, ConfigSource::User);
            config->set("neuromod.curiosity.enable", true, ConfigSource::User);
            config->set("neuromod.novelty.enable", true, ConfigSource::User);
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("preset_name"), py::arg("neuron_count") = 1000, 
         "Create brain with preset configuration (simple/complex/research)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Enhanced createSimpleWorld with common configuration
    m.def("createSimpleWorld", [](size_t width, size_t height, size_t vision_width, size_t vision_height) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, 
         py::arg("vision_width") = 8, py::arg("vision_height") = 8,
         "Create simple world with default dimensions");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience wrapper for complete agent setup
    m.def("createAgentSystem", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), 
         "Create and initialize agent brain with world");

    // World utilities
    m.def("createAgentStart", [](std::shared_ptr<SimpleWorld> world, float x, float y) {
        world->setAgentStart(x, y);
    }, py::arg("world"), py::arg("x"), py::arg("y"), 
         "Set agent starting position in world");

    m.def("addResourceToWorld", [](std::shared_ptr<SimpleWorld> world, float x, float y, float value) {
        auto obj = std::make_shared<WorldObject>(x, y, WorldObjectType::Resource, value);
        world->addObject(obj);
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
         "Add resource object to world");

    // Simulation utilities
    m.def("runSimulationSteps", [](std::shared_ptr<Brain> brain, size_t steps, size_t log_interval = 100) {
        for (size_t step = 0; step < steps; ++step) {
            brain->step(static_cast<SimulationStep>(step));
            if (log_interval > 0 && step % log_interval == 0) {
                std::cout << "Step " << step << ": " 
                          << brain->getFiringNeuronCount() << " firing neurons, "
                          << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("log_interval") = 100,
         "Run brain simulation for specified number of steps");

    m.def("runAgentEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                              std::shared_ptr<SimpleWorld> world, size_t steps, bool enable_learning = true) {
        world->reset();
        agent->reset();
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
        }
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(static_cast<SimulationStep>(step));
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": "
                          << "Firing: " << brain->getFiringNeuronCount() << ", "
                          << "Curiosity: " << agent->getCuriosityLevel() << ", "
                          << "Novelty: " << agent->getNoveltyLevel() << std::endl;
            }
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), 
         py::arg("steps") = 1000, py::arg("enable_learning") = true,
         "Run complete agent episode with learning enabled");

    // Configuration utilities
    m.def("getBrainConfig", [](std::shared_ptr<Brain> brain) {
        auto config = brain->getConfig();
        return config->summary();
    }, py::arg("brain"), "Get configuration summary as string");

    m.def("setBrainLearningParams", [](std::shared_ptr<AgentBrain> agent, bool reward, bool curiosity, bool plasticity, bool development) {
        agent->enableRewardModulation(reward);
        agent->enableCuriosity(curiosity);
        agent->enableStructuralPlasticity(plasticity);
        agent->enableDevelopment(development);
    }, py::arg("agent"), py::arg("reward"), py::arg("curiosity"), 
         py::arg("plasticity"), py::arg("development"),
         "Enable/disable learning subsystems");

    // Statistics and monitoring utilities
    m.def("getSimulationStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                                 std::shared_ptr<SimpleWorld> world) {
        py::dict stats;
        stats["brain"] = py::dict(
            "neuron_count", brain->getTotalNeuronCount(),
            "synapse_count", brain->getTotalSynapseCount(),
            "firing_neurons", brain->getFiringNeuronCount(),
            "total_spikes", brain->getTotalSpikeCount(),
            "avg_firing_rate", brain->getAverageFiringRate(),
            "e_i_ratio", brain->getExcitationInhibitionRatio()
        );
        
        stats["agent"] = py::dict(
            "curiosity_level", agent->getCuriosityLevel(),
            "novelty_level", agent->getNoveltyLevel(),
            "prediction_error", agent->getPredictionError(),
            "neuromod_level", agent->getNeuromodulationLevel()
        );
        
        stats["world"] = py::dict(
            "width", world->getWidth(),
            "height", world->getHeight(),
            "simulation_time", world->getSimulationTime()
        );
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
         "Get comprehensive simulation statistics as dictionary");

    // Advanced neural manipulation utilities
    m.def("injectCurrentPattern", [](std::shared_ptr<Brain> brain, NeuronType type, 
                                   const std::vector<float>& currents) {
        for (size_t i = 0; i < currents.size() && i < brain->getTotalNeuronCount(); ++i) {
            brain->injectCurrentToNeurons(type, currents[i]);
        }
    }, py::arg("brain"), py::arg("type"), py::arg("currents"),
         "Inject current pattern to neurons of specific type");

    m.def("createNeuralPopulation", [](std::shared_ptr<Brain> brain, const std::string& name, 
                                     NeuronType type, size_t count) {
        auto region = brain->getRegion(RegionId(1)); // Assumes region 1 exists
        if (!region) {
            throw std::runtime_error("No neural region available for population creation");
        }
        
        // Note: This is a placeholder implementation - actual population creation
        // would require additional API in the Brain class
        std::cout << "Population creation requested: " << name << " (type: " 
                  << static_cast<int>(type) << ", count: " << count << ")" << std::endl;
        return brain;
    }, py::arg("brain"), py::arg("name"), py::arg("type"), py::arg("count") = 100,
         "Create a neural population (advanced feature)");

    // Checkpoint and persistence utilities
    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw std::runtime_error("Failed to save brain state to: " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to load brain state from: " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");

    // Error handling utilities
    m.def("handleSimulationError", [](std::function<void()> simulation_step) {
        try {
            simulation_step();
        } catch (const std::exception& e) {
            std::cerr << "Simulation error: " << e.what() << std::endl;
            throw;
        }
    }, py::arg("simulation_step"), "Wrap simulation function with error handling");

    // Phase 6 integration utilities (advanced)
    m.def("createPhase6Config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Phase 6 specific optimizations
        config->set("brain.neuron_count", static_cast<int64_t>(2000), ConfigSource::User);
        config->set("brain.synapse_density", 0.15f, ConfigSource::User);
        config->set("simulation.timestep", 0.001, ConfigSource::User);
        
        // Enable all systems for Phase 6
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        config->set("neuromod.novelty.enable", true, ConfigSource::User);
        config->set("neuromod.prediction_error.enable", true, ConfigSource::User);
        
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        config->set("plasticity.structural.enable", true, ConfigSource::User);
        
        config->set("memory.working.enable", true, ConfigSource::User);
        config->set("memory.episodic.enable", true, ConfigSource::User);
        config->set("memory.semantic.enable", true, ConfigSource::User);
        
        config->set("prediction.system.enable", true, ConfigSource::User);
        config->set("cognition.planner.enable", true, ConfigSource::User);
        config->set("cognition.concept_formation.enable", true, ConfigSource::User);
        
        config->set("development.enable", true, ConfigSource::User);
        
        return config;
    }, "Create Phase 6 integrated system configuration");

    // Batch processing utilities
    m.def("runBatchSimulations", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                                   const std::vector<size_t>& steps, size_t log_interval = 100) {
        py::list results;
        for (size_t i = 0; i < brains.size(); ++i) {
            std::cout << "Running batch simulation " << i << "/" << brains.size() << std::endl;
            runSimulationSteps(brains[i], steps[i], log_interval);
            results.append(py::dict(
                "brain_id", i,
                "steps", steps[i],
                "final_spikes", brains[i]->getTotalSpikeCount()
            ));
        }
        return results;
    }, py::arg("brains"), py::arg("steps"), py::arg("log_interval") = 100,
         "Run batch of brain simulations");

    // Diagnostic and debugging utilities
    m.def("diagnoseBrain", [](std::shared_ptr<Brain> brain) {
        py::dict diagnostics;
        
        diagnostics["basic_stats"] = py::dict(
            "neuron_count", brain->getTotalNeuronCount(),
            "synapse_count", brain->getTotalSynapseCount(),
            "firing_neurons", brain->getFiringNeuronCount(),
            "total_spikes", brain->getTotalSpikeCount(),
            "avg_firing_rate", brain->getAverageFiringRate()
        );
        
        diagnostics["systems_status"] = py::dict(
            "has_spike_system", brain->getSpikeSystem() != nullptr,
            "has_stdp", brain->getSTDP() != nullptr,
            "has_hebbian", brain->getHebbian() != nullptr,
            "has_structural", brain->getStructuralPlasticity() != nullptr,
            "has_working_memory", brain->getWorkingMemory() != nullptr,
            "has_prediction", brain->getPredictionSystem() != nullptr
        );
        
        diagnostics["development_stage"] = brain->getDevelopmentalStage();
        
        return diagnostics;
    }, py::arg("brain"), "Get comprehensive brain diagnostics");

    m.def("testConnectivity", [](std::shared_ptr<Brain> brain) {
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) {
            return false;
        }
        
        // Test basic connectivity by injecting current and checking spikes
        size_t initial_spikes = brain->getTotalSpikeCount();
        neurons[0]->injectCurrent(50.0f);
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t final_spikes = brain->getTotalSpikeCount();
        return (final_spikes > initial_spikes);
    }, py::arg("brain"), "Test basic neural connectivity");

    m.def("testPlasticity", [](std::shared_ptr<Brain> brain) {
        auto region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            return false;
        }
        
        auto& synapses = region->getSynapses();
        std::vector<float> before_weights;
        for (size_t i = 0; i < 5; ++i) {
            before_weights.push_back(synapses[i]->getWeight());
        }
        
        // Apply correlated activity
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) {
            return false;
        }
        
        for (int trial = 0; trial < 20; ++trial) {
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float total_change = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            total_change += synapses[i]->getWeight() - before_weights[i];
        }
        
        return std::abs(total_change) > 0.001f;
    }, py::arg("brain"), "Test plasticity mechanisms (STDP/Hebbian)");

    // Performance monitoring utilities
    m.def("monitorPerformance", [](std::function<void()> operation) {
        auto start_time = std::chrono::high_resolution_clock::now();
        operation();
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "Operation completed in " << duration.count() << " ms" << std::endl;
        return duration.count();
    }, py::arg("operation"), "Monitor execution time of an operation");

    // Complete system setup utilities
    m.def("setupCompleteAgentSystem", [](const std::string& preset, size_t width, size_t height) {
        // Create all components
        auto config = createPhase6Config();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all subsystems for Phase 6
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableNovelty(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        
        return py::make_tuple(brain, agent, world);
    }, py::arg("preset") = "simple", py::arg("width") = 20, py::arg("height") = 20,
         "Setup complete agent system with all subsystems (Phase 6)");

    // Save/Load utilities
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw std::runtime_error("Failed to export brain state");
        }
        std::cout << "Brain state exported to: " << filepath << std::endl;
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->load(filepath)) {
            throw std::runtime_error("Failed to import brain state");
        }
        std::cout << "Brain state imported from: " << filepath << std::endl;
    }, py::arg("brain"), py::arg("filepath"), "Import brain state from file");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
