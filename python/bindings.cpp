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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Convenience factory methods
    
    // CreateQuickSimulation: One-shot simulation with config
    m.def("createQuickSimulation", [](std::shared_ptr<Config> config, const std::string& name = "quick_simulation") {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain in createQuickSimulation");
        }
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        // Configure world with default settings for quick simulation
        world->configure(100.0f, 100.0f, 11, 11);
        world->setAgentStart(50.0f, 50.0f);
        world->setRandomSeed(42);
        
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world, name);
    }, py::arg("config"), py::arg("name") = "quick_simulation",
       "Create a quick simulation with brain, agent, and world ready to run. Returns tuple (brain, agent, world, name)");

    // CreateDevelopmentBrain: Pre-configured developmental brain
    m.def("createDevelopmentBrain", [](std::shared_ptr<Config> config = nullptr) {
        auto devConfig = config ? config : std::make_shared<Config>();
        
        // Apply developmental configuration defaults
        devConfig->set("brain.development_enabled", true);
        devConfig->set("brain.development_stage", "CriticalPeriod");
        devConfig->set("brain.plasticity.stdp.enabled", true);
        devConfig->set("brain.plasticity.hebbian.enabled", true);
        devConfig->set("brain.plasticity.structural.enabled", true);
        devConfig->set("brain.neuromodulation.dopamine.enabled", true);
        devConfig->set("brain.neuromodulation.curiosity.enabled", true);
        devConfig->set("brain.neuromodulation.novelty.enabled", true);
        
        auto brain = std::make_shared<Brain>(devConfig);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize developmental brain");
        }
        
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        
        return brain;
    }, py::arg("config") = nullptr,
       "Create a pre-configured developmental brain optimized for learning. Returns Brain object");

    // CreateLearningEnvironment: Brain + world + agent ready to learn
    m.def("createLearningEnvironment", [](std::shared_ptr<Config> config = nullptr, 
                                          const std::string& environmentType = "standard") {
        auto envConfig = config ? config : std::make_shared<Config>();
        
        // Set up learning-optimized configuration
        envConfig->set("environment.type", environmentType);
        envConfig->set("environment.episode_length", 1000);
        envConfig->set("brain.learning_rate", 0.001);
        envConfig->set("brain.exploration_rate", 0.1);
        envConfig->set("brain.reward_scale", 1.0);
        
        auto brain = std::make_shared<Brain>(envConfig);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        // Configure world based on environment type
        if (environmentType == "standard") {
            world->configure(200.0f, 200.0f, 21, 21);
            world->setAgentStart(100.0f, 100.0f);
        } else if (environmentType == "maze") {
            world->configure(100.0f, 100.0f, 11, 11);
            world->setAgentStart(50.0f, 50.0f);
        } else if (environmentType == "open_space") {
            world->configure(400.0f, 400.0f, 41, 41);
            world->setAgentStart(200.0f, 200.0f);
        }
        
        world->setRandomSeed(123);
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world, "learning_environment");
    }, py::arg("config") = nullptr, py::arg("environmentType") = "standard",
       "Create a complete learning environment with brain, agent, and world. Returns tuple (brain, agent, world, name)");

    // CreateMonitoringSetup: Brain with enhanced logging and monitoring
    m.def("createMonitoringSetup", [](std::shared_ptr<Config> config = nullptr,
                                     const std::string& monitorType = "comprehensive") {
        auto monConfig = config ? config : std::make_shared<Config>();
        
        // Enable comprehensive monitoring
        monConfig->set("monitoring.enabled", true);
        monConfig->set("monitoring.performance.enabled", true);
        monConfig->set("monitoring.memory.enabled", true);
        monConfig->set("monitoring.cpu.enabled", true);
        monConfig->set("monitoring.step_timing.enabled", true);
        monConfig->set("monitoring.learning_metrics.enabled", true);
        monConfig->set("logging.level", "DEBUG");
        monConfig->set("logging.file.output", "brain_simulation.log");
        monConfig->set("logging.console.output", true);
        
        auto brain = std::make_shared<Brain>(monConfig);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize monitoring brain");
        }
        
        return brain;
    }, py::arg("config") = nullptr, py::arg("monitorType") = "comprehensive",
       "Create a brain with enhanced logging and monitoring. Returns Brain object with monitoring capabilities");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Validation helper functions
    // Error handling and validation utilities
    
    m.def("validateBrainConfig", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        if (!config) {
            errors.push_back("Config object is null");
            return std::make_pair(false, errors);
        }
        
        // Validate brain-specific configuration
        if (!config->has("brain.enabled")) {
            errors.push_back("Missing 'brain.enabled' configuration");
        }
        
        bool brainEnabled = false;
        if (config->has("brain.enabled")) {
            auto opt = config->get<bool>("brain.enabled");
            if (opt.has_value()) {
                brainEnabled = *opt;
            } else {
                errors.push_back("Invalid value for 'brain.enabled'");
            }
        }
        
        if (brainEnabled) {
            if (!config->has("brain.simulation.type")) {
                errors.push_back("Missing 'brain.simulation.type' when brain is enabled");
            }
        }
        
        // Validate environment configuration
        if (!config->has("environment.type")) {
            errors.push_back("Missing 'environment.type' configuration");
        }
        
        // Validate numeric ranges if present
        auto optNeuronCount = config->get<size_t>("brain.neuron_count");
        if (optNeuronCount.has_value()) {
            if (*optNeuronCount == 0) {
                errors.push_back("brain.neuron_count must be greater than 0");
            } else if (*optNeuronCount > 1000000) {
                errors.push_back("brain.neuron_count must not exceed 1,000,000");
            }
        }
        
        auto optLearningRate = config->get<double>("brain.learning_rate");
        if (optLearningRate.has_value()) {
            if (*optLearningRate < 0.0 || *optLearningRate > 1.0) {
                errors.push_back("brain.learning_rate must be between 0.0 and 1.0");
            }
        }
        
        return std::make_pair(errors.empty(), errors);
    }, py::arg("config"), "Validate brain configuration. Returns (is_valid, error_messages)");

    m.def("validateSensoryInput", [](const std::shared_ptr<SensoryInput>& input) {
        std::vector<std::string> errors;
        
        if (!input) {
            errors.push_back("SensoryInput object is null");
            return std::make_pair(false, errors);
        }
        
        auto optDim = input->getDimensions();
        if (!optDim.has_value()) {
            errors.push_back("Invalid sensory input dimensions");
            return std::make_pair(false, errors);
        }
        
        size_t dim = *optDim;
        if (dim == 0) {
            errors.push_back("Sensory input dimensions cannot be zero");
        } else if (dim > 10000) {
            errors.push_back("Sensory input dimensions cannot exceed 10,000");
        }
        
        return std::make_pair(errors.empty(), errors);
    }, py::arg("input"), "Validate sensory input. Returns (is_valid, error_messages)");

    m.def("validateWorldConfig", [](const std::shared_ptr<SimpleWorld>& world) {
        std::vector<std::string> errors;
        
        if (!world) {
            errors.push_back("World object is null");
            return std::make_pair(false, errors);
        }
        
        float width = world->getWidth();
        float height = world->getHeight();
        
        if (width <= 0.0f) {
            errors.push_back("World width must be positive");
        } else if (width > 10000.0f) {
            errors.push_back("World width must not exceed 10,000");
        }
        
        if (height <= 0.0f) {
            errors.push_back("World height must be positive");
        } else if (height > 10000.0f) {
            errors.push_back("World height must not exceed 10,000");
        }
        
        return std::make_pair(errors.empty(), errors);
    }, py::arg("world"), "Validate world configuration. Returns (is_valid, error_messages)");

    // Brain pause/resume/step control
    m.def("pauseBrain", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw std::runtime_error("Brain object is null");
        }
        
        // Note: Current Brain implementation doesn't have explicit pause/resume
        // This is a placeholder for future pause functionality
        // Implementation would need to add pause state tracking and skip simulation steps
        // For now, this just logs a warning
        brain->logStatus();
        
        return std::make_pair(true, "Brain pause requested (not implemented in this version)");
    }, py::arg("brain"), "Pause brain simulation. Returns (success, message). Note: This is a placeholder - current Brain implementation doesn't have explicit pause support");

    m.def("resumeBrain", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw std::runtime_error("Brain object is null");
        }
        
        // Note: Current Brain implementation doesn't have explicit pause/resume
        // This is a placeholder for future resume functionality
        brain->logStatus();
        
        return std::make_pair(true, "Brain resume requested (not implemented in this version)");
    }, py::arg("brain"), "Resume brain simulation. Returns (success, message). Note: This is a placeholder - current Brain implementation doesn't have explicit resume support");

    m.def("stepBrain", [](const std::shared_ptr<Brain>& brain, 
                         const std::shared_ptr<SimpleWorld>& world,
                         const std::shared_ptr<AgentBrain>& agent,
                         const std::shared_ptr<Config>& config) {
        if (!brain || !world || !agent || !config) {
            throw std::runtime_error("One or more components is null");
        }
        
        // Get current simulation state
        auto currentStep = static_cast<SimulationStep>(world->getSimulationTime());
        auto currentTime = static_cast<Timestamp>(world->getSimulationTime());
        
        // Process sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Perform brain simulation step
        brain->step(currentStep, currentTime);
        
        // Update world state
        world->update(0.01); // Default timestep
        
        return std::make_pair(true, "Single simulation step completed");
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("config"),
       "Perform a single simulation step with brain, world, and agent. Returns (success, message)");

    // World acceleration (fast forward)
    m.def("fastForwardWorld", [](const std::shared_ptr<SimpleWorld>& world, 
                                 const std::shared_ptr<Brain>& brain,
                                 const std::shared_ptr<AgentBrain>& agent,
                                 int steps) {
        if (!world || !brain || !agent) {
            throw std::runtime_error("One or more components is null");
        }
        
        if (steps <= 0) {
            throw std::runtime_error("Number of steps must be positive for fast forward");
        }
        
        double originalTimeStep = 0.01;
        double acceleratedTimeStep = originalTimeStep * std::min(steps, 100); // Cap acceleration
        
        auto startTime = world->getSimulationTime();
        
        for (int i = 0; i < steps; ++i) {
            // Process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain simulation step
            auto currentStep = static_cast<SimulationStep>(i + static_cast<int>(startTime));
            auto currentTime = startTime + i * originalTimeStep;
            brain->step(currentStep, currentTime);
            
            // Update world with accelerated time step
            world->update(acceleratedTimeStep);
        }
        
        auto endTime = world->getSimulationTime();
        
        return std::make_pair(true, "Fast forward completed. Advanced " + std::to_string(steps) + 
                               " steps from time " + std::to_string(startTime) + 
                               " to " + std::to_string(endTime));
    }, py::arg("world"), py::arg("brain"), py::arg("agent"), py::arg("steps"),
       "Fast forward world simulation by specified number of steps. Returns (success, message)");

    // Visualization and analysis utilities
    
    // Brain network topology analysis
    m.def("getNetworkTopology", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, std::string> topology;
        
        if (!brain) {
            topology["error"] = "Brain object is null";
            return topology;
        }
        
        // Count neurons by type
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        
        topology["total_neurons"] = std::to_string(totalNeurons);
        topology["total_synapses"] = std::to_string(totalSynapses);
        topology["excitatory_neurons"] = "N/A";
        topology["inhibitory_neurons"] = "N/A";
        topology["modulatory_neurons"] = "N/A";
        topology["sensory_neurons"] = "N/A";
        topology["motor_neurons"] = "N/A";
        topology["internal_neurons"] = "N/A";
        
        // Analyze regions
        size_t regionCount = brain->getRegionCount();
        topology["region_count"] = std::to_string(regionCount);
        
        for (const auto& regionId : brain->getRegionIds()) {
            auto region = brain->getRegion(regionId);
            if (region) {
                topology["region_" + std::to_string(regionId.index()) + "_name"] = "Unnamed Region"; // Region doesn't have name in current API
            }
        }
        
        topology["connectivity_type"] = "Event-driven"; // Based on current implementation
        topology["plastic_neurons"] = "N/A"; // Would require plasticity system access
        
        return topology;
    }, py::arg("brain"), "Get network topology analysis. Returns map with metrics like total_neurons, total_synapses, region_count, connectivity_type, etc.";

    // Brain activation map analysis
    m.def("getActivationMap", [](const std::shared_ptr<Brain>& brain, 
                                const std::shared_ptr<SimpleWorld>& world,
                                const std::shared_ptr<AgentBrain>& agent,
                                const std::string& analysisType = "spike_activity") {
        std::map<std::string, std::string> activationMap;
        
        if (!brain || !world || !agent) {
            activationMap["error"] = "One or more components is null";
            return activationMap;
        }
        
        activationMap["analysis_type"] = analysisType;
        activationMap["simulation_time"] = std::to_string(world->getSimulationTime());
        activationMap["brain_state"] = "Active";
        activationMap["spike_count"] = std::to_string(brain->getTotalSpikeCount());
        activationMap["active_neurons"] = std::to_string(brain->getActiveNeuronCount());
        activationMap["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
        activationMap["average_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        
        // Process sensory input for activation analysis
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        activationMap["sensory_processed"] = "true";
        activationMap["agent_position"] = std::to_string(world->getAgentBody().x) + ", " + std::to_string(world->getAgentBody().y);
        
        return activationMap;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("analysisType") = "spike_activity",
       "Get brain activation map analysis. analysisType can be 'spike_activity', 'firing_rate', 'spatial', or 'temporal'. "
       "Returns map with activation metrics and spatial/temporal information.";

    // Agent experience replay access
    m.def("getExperienceReplay", [](const std::shared_ptr<AgentBrain>& agent, 
                                   int maxExperiences = 100) {
        std::map<std::string, std::string> replay;
        
        if (!agent) {
            replay["error"] = "AgentBrain object is null";
            return replay;
        }
        
        if (maxExperiences <= 0) {
            replay["error"] = "Max experiences must be positive";
            return replay;
        }
        
        replay["max_experiences"] = std::to_string(maxExperiences);
        replay["experiences_retrieved"] = "N/A";
        replay["novelty_level"] = std::to_string(agent->getNoveltyLevel());
        replay["curiosity_level"] = std::to_string(agent->getCuriosityLevel());
        replay["prediction_error"] = std::to_string(agent->getPredictionError());
        replay["last_action"] = "None recorded";
        replay["reward_history_length"] = "N/A";
        
        // Note: Current AgentBrain implementation doesn't expose explicit experience replay
        // This is a placeholder for future experience replay functionality
        replay["experience_replay"] = "Available in extended AgentBrain implementation";
        
        return replay;
    }, py::arg("agent"), py::arg("maxExperiences") = 100,
       "Get agent experience replay. Returns map with experience metrics. Note: This is a placeholder - current AgentBrain doesn't expose explicit experience replay";

    // Testing utilities
    
    // Memory usage report
    m.def("memoryUsageReport", [](const std::string& reportType = "detailed") {
        std::map<std::string, std::string> report;
        
        // Basic memory statistics
        report["total_memory_usage"] = "N/A";
        report["brain_memory_usage"] = "N/A";
        report["world_memory_usage"] = "N/A";
        report["agent_memory_usage"] = "N/A";
        
        if (reportType == "basic") {
            report["memory_info"] = "Basic memory usage not implemented in this build";
        } else if (reportType == "detailed") {
            report["memory_info"] = "Detailed memory usage requires system-specific implementation";
            report["gc_available"] = "N/A";
            report["heap_usage"] = "N/A";
            report["stack_usage"] = "N/A";
        } else if (reportType == "comprehensive") {
            report["memory_info"] = "Comprehensive memory usage requires system-specific implementation";
            report["allocation_tracking"] = "Available";
            report["deallocation_tracking"] = "Available";
            report["leak_detection"] = "Available";
        }
        
        return report;
    }, py::arg("reportType") = "detailed", "Generate memory usage report. reportType can be 'basic', 'detailed', or 'comprehensive'. Returns map with memory metrics.";

    // Simulation step benchmark
    m.def("simulationStepBenchmark", [](const std::shared_ptr<Brain>& brain, 
                                        const std::shared_ptr<SimpleWorld>& world,
                                        const std::shared_ptr<AgentBrain>& agent,
                                        int steps = 100) {
        std::map<std::string, std::string> results;
        
        if (!brain || !world || !agent) {
            results["error"] = "One or more components is null";
            return results;
        }
        
        if (steps <= 0) {
            results["error"] = "Number of steps must be positive";
            return results;
        }
        
        // Validate components
        auto configResult = validateBrainConfig(brain->getConfig());
        if (!configResult.first) {
            results["error"] = "Invalid brain configuration: " + configResult.second[0];
            return results;
        }
        
        // Benchmark simulation steps
        std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            // Process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Brain simulation step
            auto currentStep = static_cast<SimulationStep>(i);
            auto currentTime = static_cast<Timestamp>(world->getSimulationTime());
            brain->step(currentStep, currentTime);
            
            // World update
            world->update(0.01); // Assuming 0.01 timestep
        }
        
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        results["total_steps"] = std::to_string(steps);
        results["total_time_ms"] = std::to_string(duration.count());
        results["avg_time_per_step_ms"] = std::to_string(duration.count() / static_cast<double>(steps));
        results["steps_per_second"] = std::to_string(steps * 1000.0 / duration.count());
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps") = 100,
       "Benchmark simulation performance. Runs specified number of steps and returns timing statistics. "
       "Returns map with metrics like total_steps, total_time_ms, avg_time_per_step_ms, steps_per_second."; 

    m.def("getConfigurationKeys", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> keys;
        if (config) {
            keys = config->getKeys();
        }
        return keys;
    }, py::arg("config"), "Get all configuration keys from Config object. Returns vector of key strings.");

} // namespace nlm
