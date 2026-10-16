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

    // Add convenience constructors and factory methods
    m.def("create_config", &createDefaultConfig, "Create a default configuration");
    m.def("create_brain", &createBrain, py::arg("config"), "Create a new brain with configuration");
    m.def("create_world", &createSimpleWorld, "Create a new simple world");
    m.def("create_agent", &createAgentBrain, py::arg("brain"), "Create a new agent brain interface");
    
    // Add utility functions for common operations
    m.def("run_learning_experiment", [](std::shared_ptr<Brain> brain, const std::string& configFile) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            brain->initialize();
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("configFile"), "Run a learning experiment with given configuration");
    
    m.def("export_brain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to file");
    
    m.def("import_brain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Import brain state from file");
    
    // Add configuration utilities
    m.def("config_from_json", [](const std::string& jsonContent) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (config->importFromJSON(jsonContent)) {
            return config;
        }
        return nullptr;
    }, py::arg("jsonContent"), "Create configuration from JSON string");
    
    m.def("config_from_yaml", [](const std::string& yamlContent) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (config->importFromYAML(yamlContent)) {
            return config;
        }
        return nullptr;
    }, py::arg("yamlContent"), "Create configuration from YAML string");
    
    m.def("config_to_json", [](const Config& config) {
        return ConfigParser::serializeToJSON(config.getKeys());
    }, py::arg("config"), "Convert configuration to JSON string");
    
    m.def("config_to_yaml", [](const Config& config) {
        return ConfigParser::serializeToYAML(config.getKeys());
    }, py::arg("config"), "Convert configuration to YAML string");
    
    // Add world configuration utilities
    m.def("configure_world", [](std::shared_ptr<SimpleWorld> world, size_t width, size_t height, 
                              size_t visionWidth, size_t visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
    }, py::arg("world"), py::arg("width"), py::arg("height"), 
       py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions and vision");
    
    // Add action utilities
    m.def("create_action", [](ActionType type, const std::vector<float>& parameters = {}) {
        return std::make_unique<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters"), "Create an action with optional parameters");
    
    m.def("action_name", [](ActionType type) {
        return Action::getName(type);
    }, py::arg("type"), "Get human-readable name for action type");
    
    // Add sensory input utilities
    m.def("create_vision", [](size_t width, size_t height, size_t channels = 3) {
        return std::make_unique<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels"), "Create vision sensory input");
    
    m.def("create_audio", [](size_t sampleRate, size_t numSamples) {
        return std::make_unique<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create audio sensory input");
    
    m.def("create_internal_signals", [] {
        return std::make_unique<InternalSignals>();
    }, "Create internal signals sensory input");
    
    // Add agent brain utilities
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface");
    
    m.def("simulate_step", [](std::shared_ptr<AgentBrain> agentBrain, std::shared_ptr<SimpleWorld> world, 
                            double timestep = 0.001) {
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Decode motor command
        MotorCommand cmd = agentBrain->decodeMotorCommand();
        
        // Apply command in world
        world->applyMotorCommand(cmd, world->getSimulationTime());
        
        // Update world
        world->update(timestep);
        
        // Return new percept
        return world->getSensoryPercept();
    }, py::arg("agentBrain"), py::arg("world"), py::arg("timestep"), "Run one simulation step");
    
    // Add advanced configuration utilities
    m.def("validate_config_schema", [](const Config& config, const std::string& schemaType) {
        return Config::validateAgainstSchema(config, schemaType);
    }, py::arg("config"), py::arg("schemaType"), "Validate configuration against schema");
    
    m.def("merge_configs", [](const Config& config1, const Config& config2) {
        auto mergedConfig = std::make_shared<Config>(config1);
        mergedConfig->merge(config2);
        return mergedConfig;
    }, py::arg("config1"), py::arg("config2"), "Merge two configurations");
    
    m.def("compare_configs", [](const Config& config1, const Config& config2, const std::string& key = "") {
        return config1.compare(config2, key);
    }, py::arg("config1"), py::arg("config2"), py::arg("key"), "Compare two configurations");
    
    // Add backup and restore utilities
    m.def("backup_config", [](const Config& config, const std::string& backupPath) {
        return config.backup(backupPath);
    }, py::arg("config"), py::arg("backupPath"), "Backup configuration to file");
    
    m.def("restore_config", [](Config& config, const std::string& backupPath) {
        return config.restore(backupPath);
    }, py::arg("config"), py::arg("backupPath"), "Restore configuration from backup file");
    
    // Add logging utilities
    m.def("set_log_level", [](const std::string& level) {
        Logger::setLogLevel(level);
    }, py::arg("level"), "Set global log level");
    
    m.def("get_log_level", []() {
        return Logger::getLogLevel();
    }, "Get current global log level");
    
    m.def("log_config_status", [](const Config& config) {
        std::cout << config.summary() << std::endl;
    }, py::arg("config"), "Log configuration status");
    
    // Add experiment utilities
    m.def("run_default_experiment", [](std::shared_ptr<Brain> brain) {
        // Run basic connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        // Inject current into first neurons
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return true;
    }, py::arg("brain"), "Run default experiment");
    
    // Add performance utilities
    m.def("get_brain_statistics", [](const Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        return stats;
    }, py::arg("brain"), "Get brain statistics as dictionary");
    
    m.def("get_memory_statistics", [](const Brain& brain) {
        py::dict stats;
        auto* workingMemory = brain.getWorkingMemory();
        if (workingMemory) {
            stats["working_memory_traces"] = workingMemory->getActiveTraces();
            stats["working_memory_capacity"] = workingMemory->getCapacity();
        }
        
        auto* episodicMemory = brain.getEpisodicMemory();
        if (episodicMemory) {
            stats["episodic_memory_episodes"] = episodicMemory->getEpisodeCount();
            stats["episodic_memory_max_episodes"] = episodicMemory->getMaxEpisodes();
        }
        
        auto* associativeMemory = brain.getAssociativeMemory();
        if (associativeMemory) {
            stats["associative_memory_patterns"] = associativeMemory->getPatternCount();
        }
        
        return stats;
    }, py::arg("brain"), "Get memory system statistics");
    
    // Add development utilities
    m.def("get_developmental_stage", [](const Brain& brain) {
        return brain.getDevelopmentalStage();
    }, py::arg("brain"), "Get current developmental stage");
    
    m.def("set_developmental_stage", [](Brain& brain, DevelopmentalStage stage) {
        brain.setDevelopmentalStage(stage);
    }, py::arg("brain"), py::arg("stage"), "Set developmental stage");
    
    m.def("get_neuromodulation_level", [](const AgentBrain& agentBrain) {
        return agentBrain.getNeuromodulationLevel();
    }, py::arg("agentBrain"), "Get current neuromodulation level");
    
    // Add curiosity and novelty utilities
    m.def("get_curiosity_level", [](const AgentBrain& agentBrain) {
        return agentBrain.getCuriosityLevel();
    }, py::arg("agentBrain"), "Get current curiosity level");
    
    m.def("get_novelty_level", [](const AgentBrain& agentBrain) {
        return agentBrain.getNoveltyLevel();
    }, py::arg("agentBrain"), "Get current novelty level");
    
    // Add prediction utilities
    m.def("get_prediction_error", [](const AgentBrain& agentBrain) {
        return agentBrain.getPredictionError();
    }, py::arg("agentBrain"), "Get current prediction error");
    
    // Add sensory utilities
    m.def("get_sensory_input_size", [](const AgentBrain& agentBrain) {
        return agentBrain.getSensoryInputSize();
    }, py::arg("agentBrain"), "Get expected sensory input size");
    
    m.def("get_motor_output_size", [](const AgentBrain& agentBrain) {
        return agentBrain.getMotorOutputSize();
    }, py::arg("agentBrain"), "Get expected motor output size");
    
    // Add simulation utilities
    m.def("get_simulation_time", [](const SimpleWorld& world) {
        return world.getSimulationTime();
    }, py::arg("world"), "Get current simulation time");
    
    m.def("get_world_dimensions", [](const SimpleWorld& world) {
        return py::make_tuple(world.getWidth(), world.getHeight());
    }, py::arg("world"), "Get world dimensions as tuple");
    
    // Add agent body utilities
    m.def("get_agent_energy", [](const AgentBody& body) {
        return body.energy;
    }, py::arg("body"), "Get agent energy level");
    
    m.def("get_agent_health", [](const AgentBody& body) {
        return body.health;
    }, py::arg("body"), "Get agent health level");
    
    m.def("get_agent_age", [](const AgentBody& body) {
        return body.age;
    }, py::arg("body"), "Get agent age");
    
    // Add convenience functions for common use cases
    m.def("create_default_brain", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a default brain with standard configuration");
    
    m.def("create_simple_agent", [](std::shared_ptr<Brain> brain) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        return agentBrain;
    }, py::arg("brain"), "Create a simple agent brain interface");
    
    m.def("create_standard_world", [](size_t width = 50, size_t height = 50, 
                                    size_t visionWidth = 16, size_t visionHeight = 16) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), 
       py::arg("visionWidth"), py::arg("visionHeight"), "Create a standard world");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
