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

    py::class_<NeuronId>(m, "NeuronId", "Unique identifier for a neuron")
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

    py::class_<SynapseId>(m, "SynapseId", "Unique identifier for a synapse")
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

    py::class_<RegionId>(m, "RegionId", "Unique identifier for a brain region")
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

    py::class_<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", "Neuron firing state enumeration")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", "Action type enumeration")
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

    py::enum_<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", "Configuration class for NLM system")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    // Enhanced Configuration methods for Pythonic interface
    py::class_<Config, std::shared_ptr<Config>>(m, "ConfigExtended")
        .def("setString", &Config::setString, py::arg("key"), py::arg("value"),
             "Set string configuration value (Pythonic interface)")
        .def("getString", &Config::getString, py::arg("key"), py::arg("defaultValue") = "",
             "Get string configuration value (Pythonic interface)")
        .def("setInt", &Config::setInt, py::arg("key"), py::arg("value"),
             "Set integer configuration value (Pythonic interface)")
        .def("getInt", &Config::getInt, py::arg("key"), py::arg("defaultValue") = 0,
             "Get integer configuration value (Pythonic interface)")
        .def("setDouble", &Config::setDouble, py::arg("key"), py::arg("value"),
             "Set double configuration value (Pythonic interface)")
        .def("getDouble", &Config::getDouble, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get double configuration value (Pythonic interface)")
        .def("setBool", &Config::setBool, py::arg("key"), py::arg("value"),
             "Set boolean configuration value (Pythonic interface)")
        .def("getBool", &Config::getBool, py::arg("key"), py::arg("defaultValue") = false,
             "Get boolean configuration value (Pythonic interface)")
        .def("getSource", &Config::getSource, py::arg("key"),
             "Get configuration source (Default/File/CommandLine/Runtime)")
        .def("merge", &Config::mergeFromConfig, py::arg("other"), py::arg("source") = 0,
             "Merge another configuration into this one")
        .def("validate", &Config::validate,
             "Validate configuration consistency")
        .def("entryCount", &Config::entryCount,
             "Get number of configuration entries")
        .def("entryCountBySource", &Config::entryCount, py::arg("source"),
             "Get number of configuration entries by source");

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // Enhanced brain class with Pythonic methods
    py::class_<Brain, std::shared_ptr<Brain>>(m, "BrainEnhanced")
        .def("resetAndStep", &Brain::resetAndStep, py::arg("currentStep"),
             "Reset brain and perform a single step (Pythonic interface)")
        .def("runTestSequence", &Brain::runTestSequence,
             "Run a complete test sequence (Pythonic interface)")
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (Pythonic interface)")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (Pythonic interface)")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get active neuron count (Pythonic interface)")
        .def("getFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate (Pythonic interface)")
        .def("injectCurrentToType", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current to all neurons of a specific type (Pythonic interface)")
        .def("produceActionWithContext", &Brain::produceActionWithContext,
             py::arg("sensoryInput"), "Produce action with sensory context (Pythonic interface)")
        .def("saveWithBackup", &Brain::saveWithBackup, py::arg("basePath"),
             "Save brain state with automatic backup naming (Pythonic interface)")
        .def("loadWithValidation", &Brain::loadWithValidation, py::arg("filepath"),
             "Load brain state with validation (Pythonic interface)")
        .def("simulateStep", &Brain::simulateStep, py::arg("timestep"),
             "Simulate a single timestep (Pythonic interface)")
        .def("createSensoryInput", [](std::shared_ptr<Brain> brain, size_t size) {
            return brain->createDefaultSensoryInput(size);
        }, py::arg("size"), "Create default sensory input (Pythonic interface)")
        .def("getConnectedRegions", &Brain::getConnectedRegions,
             "Get all regions connected to a region (Pythonic interface)")
        .def("analyzeNetworkConnectivity", &Brain::analyzeNetworkConnectivity,
             "Analyze network connectivity statistics (Pythonic interface)")
        .def("exportForExport", &Brain::exportForExport, py::arg("format"),
             "Export brain state in specified format (Pythonic interface)")
        .def("importFromExport", &Brain::importFromExport, py::arg("format"), py::arg("data"),
             "Import brain state from specified format (Pythonic interface)")
        .def("getDevelopmentStatus", &Brain::getDevelopmentStatus,
             "Get brain development status (Pythonic interface)")
        .def("getNeuromodulationStatus", &Brain::getNeuromodulationStatus,
             "Get neuromodulation status (Pythonic interface)")
        .def("getLearningMetrics", &Brain::getLearningMetrics,
             "Get learning metrics and statistics (Pythonic interface)");

    // Enhanced AgentBrain with Pythonic interfaces
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrainEnhanced")
        .def("initializeWithWorld", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world (Pythonic interface)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input and inject into brain (Pythonic interface)")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command (Pythonic interface)")
        .def("applyModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation (Pythonic interface)")
        .def("updateDevelopmentSystem", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system (Pythonic interface)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (Pythonic interface)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (Pythonic interface)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error (Pythonic interface)")
        .def("resetEpisode", &AgentBrain::reset,
             "Reset agent for new episode (Pythonic interface)")
        .def("getDevelopmentStatus", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage (Pythonic interface)")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level (Pythonic interface)")
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all advanced features (Pythonic interface)")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all advanced features (Pythonic interface)")
        .def("getFeatureStatus", [](AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get all feature status as dictionary (Pythonic interface)")
        .def("createDefaultWorld", []() -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(800, 600, 80, 60);
            world->setAgentStart(400, 300);
            return world;
        }, "Create default world (Pythonic interface)")
        .def("createSimpleSensoryInput", [](std::shared_ptr<AgentBrain> agent, size_t width, size_t height) {
            return agent->createDefaultSensoryInput(width, height);
        }, py::arg("width"), py::arg("height"), "Create simple sensory input (Pythonic interface)")
        .def("getActionMapping", &AgentBrain::getActionMapping,
             "Get internal action mapping (Pythonic interface)")
        .def("analyzeBehavior", &AgentBrain::analyzeBehavior,
             "Analyze agent behavior and performance (Pythonic interface)")
        .def("exportPersonality", &AgentBrain::exportPersonality,
             "Export personality traits for serialization (Pythonic interface)")
        .def("importPersonality", &AgentBrain::importPersonality, py::arg("traits"),
             "Import personality traits from serialization (Pythonic interface)");

    // Create a simple world with common configuration (Pythonic interface)
    m.def("createSimpleWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 80, 60); // Default 800x600 world with 80x60 vision
        world->setAgentStart(400, 300); // Center start position
        return world;
    }, "Create a configured simple world with default settings (Pythonic interface)");
    
    // Create a brain with optimal Phase 2 configuration (Pythonic interface)
    m.def("createDefaultBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, "Create a brain with optimal Phase 2 configuration (Pythonic interface)");
    
    // Create an agent brain with world integration (Pythonic interface)
    m.def("createAgentBrainWithWorld", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), 
       "Create an agent brain connected to a world (Pythonic interface)");
    
    // Convenience factory functions for common use cases
    m.def("createTrainingSetup", []() {
        py::dict setup;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.2f);
        
        auto brain = std::make_shared<Brain>(config);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(1024, 768, 128, 96);
        world->setAgentStart(512, 384);
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.01f);
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        agentBrain->enableAllFeatures();
        
        setup["config"] = config;
        setup["brain"] = brain;
        setup["world"] = world;
        setup["agentBrain"] = agentBrain;
        
        return setup;
    }, "Create a complete training setup with all features enabled (Pythonic interface)");
    
    m.def("createTestingSetup", []() {
        py::dict setup;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        
        auto brain = std::make_shared<Brain>(config);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 64, 48);
        world->setAgentStart(400, 300);
        
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        setup["config"] = config;
        setup["brain"] = brain;
        setup["world"] = world;
        setup["agentBrain"] = agentBrain;
        
        return setup;
    }, "Create a testing setup with minimal configuration (Pythonic interface)");
    
    // Utility functions for configuration management
    m.def("createConfigFromDictionary", [](py::dict configDict) {
        auto config = std::make_shared<Config>();
        
        for (auto& item : configDict) {
            std::string key = py::cast<std::string>(item.first);
            
            if (py::isinstance(item.second, py::int_())) {
                config->setInt(key, py::cast<int>(item.second));
            } else if (py::isinstance(item.second, py::float_())) {
                config->setDouble(key, py::cast<double>(item.second));
            } else if (py::isinstance(item.second, py::bool_())) {
                config->setBool(key, py::cast<bool>(item.second));
            } else if (py::isinstance(item.second, py::str())) {
                config->setString(key, py::cast<std::string>(item.second));
            }
        }
        
        return config;
    }, py::arg("configDict"), 
       "Create configuration from Python dictionary (Pythonic interface)");
    
    m.def("configToDictionary", [](const std::shared_ptr<Config>& config) {
        py::dict dict;
        
        for (const auto& key : config->getKeys()) {
            dict[key] = config->getString(key, "");
        }
        
        return dict;
    }, py::arg("config"), 
       "Convert configuration to Python dictionary (Pythonic interface)");

} // namespace nlm
