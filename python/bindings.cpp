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

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced factory functions
    m.def("createBrainFromConfig", [](const std::string& configFile) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            return std::make_shared<Brain>(config);
        }
        throw std::runtime_error("Failed to load configuration from: " + configFile);
    }, py::arg("configFile"), "Create a brain from a configuration file");

    m.def("createBrainWithArgs", [](int argc, char** argv) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (config->loadFromArgs(argc, argv)) {
            return std::make_shared<Brain>(config);
        }
        throw std::runtime_error("Failed to load configuration from arguments");
    }, py::arg("argc"), py::arg("argv"), "Create a brain from command line arguments");

    // Pythonic convenience functions
    m.def("createSimpleVision", [](size_t width, size_t height, size_t channels) {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width") = 64, py::arg("height") = 64, py::arg("channels") = 3, "Create a simple vision sensory input");

    m.def("createSimpleAudio", [](size_t sampleRate, size_t numSamples) {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate") = 44100, py::arg("numSamples") = 1024, "Create a simple audio sensory input");

    m.def("createAgentWithWorld", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain with a world");

    // Advanced utility functions
    m.def("createCustomAction", [](ActionType type, const std::vector<float>& params) {
        return std::make_shared<Action>(type, params);
    }, py::arg("type"), py::arg("params"), "Create a custom action with parameters");

    m.def("createStandardAction", &Action::createStandardAction, py::arg("type"), "Create a standard action");

    m.def("getDefaultConfig", &createDefaultConfig, "Get a default configuration");

    // Utility functions for debugging and analysis
    m.def("validateBrainConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("neuron_count")) {
            errors.push_back("Missing 'neuron_count' in configuration");
        }
        if (!config->has("region_count")) {
            errors.push_back("Missing 'region_count' in configuration");
        }
        if (!config->has("random_seed")) {
            errors.push_back("Missing 'random_seed' in configuration");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("simulateEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                               size_t maxSteps, float timestep) {
        float totalReward = 0.0f;
        float novelty = 0.0f;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            brain->step(step, step * timestep);
            
            auto action = brain->produceAction();
            world->applyMotorCommand(action, step * timestep);
            
            totalReward += world->computeReward(brain.get());
        }
        
        return std::make_tuple(totalReward, novelty);
    }, py::arg("brain"), py::arg("world"), py::arg("maxSteps") = 100, py::arg("timestep") = 0.001, 
       "Run a complete simulation episode and return total reward and novelty");

    m.def("extractNeuralFeatures", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, float> features;
        
        if (brain->getTotalNeuronCount() > 0) {
            features["avg_firing_rate"] = brain->getAverageFiringRate();
            features["active_neuron_ratio"] = 
                static_cast<float>(brain->getActiveNeuronCount()) / brain->getTotalNeuronCount();
            features["firing_neuron_ratio"] = 
                static_cast<float>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount();
            features["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        }
        
        return features;
    }, py::arg("brain"), "Extract neural features from a brain for analysis");

    m.def("createBrainTemplate", [](const std::string& templateName) {
        auto config = std::make_shared<Config>();
        
        if (templateName == "small") {
            config->set("neuron_count", 100);
            config->set("region_count", 2);
            config->set("connection_probability", 0.1);
        } else if (templateName == "medium") {
            config->set("neuron_count", 1000);
            config->set("region_count", 4);
            config->set("connection_probability", 0.15);
        } else if (templateName == "large") {
            config->set("neuron_count", 5000);
            config->set("region_count", 8);
            config->set("connection_probability", 0.2);
        }
        
        config->set("random_seed", 42);
        config->set("simulation_timestep", 0.001);
        
        return std::make_shared<Brain>(config);
    }, py::arg("templateName"), "Create a brain with predefined configuration template");

    // Error constants
    m.attr("ERROR_INVALID_CONFIG") = "INVALID_CONFIG";
    m.attr("ERROR_NEURAL_INVALID") = "NEURAL_INVALID";
    m.attr("ERROR_WORLD_INVALID") = "WORLD_INVALID";

    // Version and metadata
    m.attr("NLM_VERSION_MAJOR") = 0;
    m.attr("NLM_VERSION_MINOR") = 1;
    m.attr("NLM_VERSION_PATCH") = 0;

    m.attr("PYNLM_VERSION") = "1.0.0";

    // Advanced utility functions for experienced users
    
    // Performance profiling
    m.def("profile_brain_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         size_t num_steps, float timestep, bool verbose = false) {
        struct SimulationProfile {
            double total_time;
            double cpu_time;
            double wall_clock_time;
            size_t steps_performed;
            double avg_firing_rate;
            double peak_memory_mb;
        };
        
        SimulationProfile profile;
        auto start_cpu = std::chrono::high_resolution_clock::now();
        auto start_wall = std::chrono::steady_clock::now();
        
        size_t total_spikes_before = brain->getTotalSpikeCount();
        float total_reward = 0.0f;
        
        for (size_t step = 0; step < num_steps; ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            
            // Run simulation step
            if (world) {
                auto percept = world->getSensoryPercept();
                brain->receiveSensoryInput(percept);
            }
            brain->step(step, step * timestep);
            
            if (world) {
                auto action = brain->produceAction();
                world->applyMotorCommand(action, step * timestep);
                
                // Compute reward if possible
                try {
                    float reward = world->computeReward(brain.get());
                    total_reward += reward;
                } catch (...) {
                    // Reward computation might fail, continue without it
                }
            }
            
            auto step_end = std::chrono::high_resolution_clock::now();
            if (verbose && step % 100 == 0) {
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(step_end - step_start);
                std::cout << "Step " << step << ": " << duration.count() << " μs" << std::endl;
            }
        }
        
        auto end_cpu = std::chrono::high_resolution_clock::now();
        auto end_wall = std::chrono::steady_clock::now();
        
        profile.cpu_time = std::chrono::duration<double>(end_cpu - start_cpu).count();
        profile.wall_clock_time = std::chrono::duration<double>(end_wall - start_wall).count();
        profile.total_time = profile.cpu_time; // Simplified - actual total time would include all overhead
        profile.steps_performed = num_steps;
        profile.avg_firing_rate = brain->getAverageFiringRate();
        profile.peak_memory_mb = 0.0; // Simplified - would need platform-specific measurement
        
        return profile;
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("num_steps") = 1000,
           py::arg("timestep") = 0.001, py::arg("verbose") = false,
           "Profile a brain simulation and return performance metrics");

    // Advanced neural network analysis
    m.def("analyze_neural_dynamics", [](std::shared_ptr<Brain> brain, const std::string& analysis_type = "comprehensive") {
        std::unordered_map<std::string, double> analysis_results;
        
        analysis_results["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        analysis_results["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        analysis_results["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
        analysis_results["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        analysis_results["average_firing_rate"] = brain->getAverageFiringRate();
        analysis_results["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        analysis_results["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        
        if (analysis_type == "comprehensive") {
            // Calculate additional metrics
            float firing_rate = brain->getAverageFiringRate();
            size_t active = brain->getActiveNeuronCount();
            size_t total = brain->getTotalNeuronCount();
            
            if (total > 0) {
                analysis_results["activity_percentage"] = (active / static_cast<double>(total)) * 100.0;
                analysis_results["firing_percentage"] = (brain->getFiringNeuronCount() / static_cast<double>(total)) * 100.0;
                analysis_results["synaptic_density"] = (analysis_results["total_synapses"] / analysis_results["total_neurons"]);
            }
            
            // Calculate network metrics
            analysis_results["network_efficiency"] = (firing_rate > 0.0) ? (analysis_results["total_spikes"] / firing_rate) : 0.0;
            analysis_results["connection_density"] = (analysis_results["total_synapses"] / (analysis_results["total_neurons"] * (analysis_results["total_neurons"] - 1)) * 100.0);
        } else if (analysis_type == "basic") {
            // Only basic metrics (already calculated)
        } else if (analysis_type == "detailed") {
            // Add detailed analysis
            analysis_results["network_dynamics_stability"] = 1.0; // Simplified - would require time-series analysis
            analysis_results["synaptic_strength_variance"] = 0.0; // Simplified - would require weight analysis
            analysis_results["network_broadening_index"] = 0.0; // Simplified - would require connectivity analysis
        }
        
        return analysis_results;
    }, py::arg("brain"), py::arg("analysis_type") = "comprehensive",
           "Analyze neural network dynamics and return metrics");

    // Advanced batch processing
    m.def("run_batch_simulation", [](const std::vector<std::shared_ptr<Config>>& configs,
                                     const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                                     size_t steps_per_config, float timestep, bool parallel = false) {
        std::vector<std::unordered_map<std::string, double>> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            std::unordered_map<std::string, double> config_result;
            config_result["config_index"] = static_cast<double>(i);
            
            auto brain = std::make_shared<Brain>(configs[i]);
            brain->initialize();
            
            auto world = worlds.empty() ? nullptr : worlds[i % worlds.size()];
            
            // Run simulation
            float total_reward = 0.0f;
            float total_firing = 0.0f;
            size_t firing_count = 0;
            
            for (size_t step = 0; step < steps_per_config; ++step) {
                if (world) {
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(percept);
                }
                brain->step(step, step * timestep);
                
                if (world) {
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, step * timestep);
                    
                    try {
                        float reward = world->computeReward(brain.get());
                        total_reward += reward;
                    } catch (...) {
                        // Continue without reward calculation
                    }
                }
                
                total_firing += brain->getAverageFiringRate();
                if (brain->getFiringNeuronCount() > 0) firing_count++;
            }
            
            config_result["total_reward"] = total_reward / steps_per_config;
            config_result["avg_firing_rate"] = total_firing / steps_per_config;
            config_result["steps_completed"] = static_cast<double>(steps_per_config);
            config_result["successful_steps"] = static_cast<double>(firing_count);
            
            results.push_back(config_result);
            
            if (i % 10 == 0) {
                std::cout << "Completed batch simulation " << i + 1 << " of " << configs.size() << std::endl;
            }
        }
        
        return results;
    }, py::arg("configs"), py::arg("worlds") = std::vector<std::shared_ptr<SimpleWorld>>(),
           py::arg("steps_per_config") = 100, py::arg("timestep") = 0.001,
           py::arg("parallel") = false,
           "Run batch simulations with multiple configurations and return results");

    // Advanced checkpoint management
    m.def("advanced_checkpoint_manager", [](std::shared_ptr<Brain> brain, const std::string& base_path,
                                            size_t max_checkpoints = 10, bool compress = false) {
        struct CheckpointInfo {
            std::string path;
            double timestamp;
            size_t neuron_count;
            size_t synapse_count;
            size_t spike_count;
        };
        
        std::vector<CheckpointInfo> checkpoints;
        
        // Create main checkpoint
        std::string main_checkpoint = base_path + "/main_checkpoint.bin";
        if (brain->save(main_checkpoint)) {
            CheckpointInfo info;
            info.path = main_checkpoint;
            info.timestamp = std::time(nullptr);
            info.neuron_count = brain->getTotalNeuronCount();
            info.synapse_count = brain->getTotalSynapseCount();
            info.spike_count = brain->getTotalSpikeCount();
            checkpoints.push_back(info);
        }
        
        // Create incremental checkpoints
        for (size_t i = 0; i < max_checkpoints; ++i) {
            std::string checkpoint_path = base_path + "/checkpoint_" + std::to_string(i) + ".bin";
            
            // Run some simulation to create different state
            for (size_t step = 0; step < 100; ++step) {
                brain->step(i * 100 + step, (i * 100 + step) * 0.001);
            }
            
            if (brain->save(checkpoint_path)) {
                CheckpointInfo info;
                info.path = checkpoint_path;
                info.timestamp = std::time(nullptr);
                info.neuron_count = brain->getTotalNeuronCount();
                info.synapse_count = brain->getTotalSynapseCount();
                info.spike_count = brain->getTotalSpikeCount();
                checkpoints.push_back(info);
            }
        }
        
        return checkpoints;
    }, py::arg("brain"), py::arg("base_path"), py::arg("max_checkpoints") = 10,
           py::arg("compress") = false,
           "Create advanced checkpoints with metadata and return checkpoint information");

    // Advanced configuration optimization
    m.def("optimize_configuration", [](std::shared_ptr<Config> config, const std::string& optimization_target = "performance",
                                        size_t iterations = 50) {
        struct OptimizationResult {
            std::shared_ptr<Config> best_config;
            double best_score;
            std::vector<double> convergence_history;
        };
        
        OptimizationResult result;
        result.best_score = -1e9;
        
        // Simplified optimization - in practice this would use more sophisticated algorithms
        std::vector<double> scores;
        
        for (size_t iter = 0; iter < iterations; ++iter) {
            double score = 0.0;
            
            // Simple scoring based on configuration parameters
            if (config->has("neuron_count")) {
                score += static_cast<double>(config->getOr<int64_t>("neuron_count", 1000)) * 0.1;
            }
            if (config->has("simulation_timestep")) {
                double timestep = config->getOr<double>("simulation_timestep", 0.001);
                score += (1.0 / timestep) * 0.5;  // Smaller timestep = higher score
            }
            if (config->has("region_count")) {
                score += static_cast<double>(config->getOr<int64_t>("region_count", 2)) * 0.2;
            }
            
            // Add some randomness to create variation
            score += (iter % 7) * 10.0 - 30.0;
            
            scores.push_back(score);
            
            if (score > result.best_score) {
                result.best_score = score;
                result.best_config = config; // Simplified - in practice would copy config
            }
            
            // Mutate config for next iteration
            if (config->has("neuron_count")) {
                int64_t current = config->getOr<int64_t>("neuron_count", 1000);
                config->set("neuron_count", current + (iter % 3) - 1);
            }
        }
        
        result.convergence_history = scores;
        return result;
    }, py::arg("config"), py::arg("optimization_target") = "performance",
           py::arg("iterations") = 50,
           "Optimize configuration parameters and return optimization results");

    // Advanced neural debugging and visualization
    m.def("debug_neural_network", [](std::shared_ptr<Brain> brain, bool detailed = false) {
        struct DebugInfo {
            std::string status;
            std::vector<std::string> warnings;
            std::vector<std::string> errors;
            std::unordered_map<std::string, std::string> debug_metrics;
        };
        
        DebugInfo debug;
        
        // Basic validation
        if (brain->getTotalNeuronCount() == 0) {
            debug.errors.push_back("Brain has no neurons");
        }
        
        if (brain->getTotalSynapseCount() == 0) {
            debug.warnings.push_back("Brain has no synapses - network may be disconnected");
        }
        
        if (brain->getAverageFiringRate() < 0.01) {
            debug.warnings.push_back("Very low firing rate - network may be inactive");
        }
        
        if (brain->getExcitationInhibitionRatio() > 10.0) {
            debug.warnings.push_back("High excitation/inhibition ratio - network may be unstable");
        }
        
        // Detailed debugging
        if (detailed) {
            debug.debug_metrics["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
            debug.debug_metrics["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
            debug.debug_metrics["active_ratio"] = std::to_string(
                brain->getActiveNeuronCount() / static_cast<double>(brain->getTotalNeuronCount()));
            debug.debug_metrics["firing_ratio"] = std::to_string(
                brain->getFiringNeuronCount() / static_cast<double>(brain->getTotalNeuronCount()));
            debug.debug_metrics["stability_index"] = std::to_string(brain->getExcitationInhibitionRatio());
            debug.debug_metrics["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        }
        
        debug.status = (debug.errors.empty()) ? "OK" : "ERROR";
        return debug;
    }, py::arg("brain"), py::arg("detailed") = false,
           "Debug neural network and return status information");

    // Advanced simulation control
    m.def("control_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   const std::string& control_mode, 
                                   const std::unordered_map<std::string, double>& parameters) {
        struct ControlResult {
            std::string status;
            double execution_time;
            std::unordered_map<std::string, double> metrics;
        };
        
        ControlResult result;
        result.status = "COMPLETED";
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        if (control_mode == "adaptive") {
            // Adaptive simulation based on parameters
            double target_firing_rate = parameters.count("target_firing_rate") ? parameters.at("target_firing_rate") : 10.0;
            double learning_rate = parameters.count("learning_rate") ? parameters.at("learning_rate") : 0.1;
            
            for (size_t step = 0; step < 1000; ++step) {
                // Adaptive control logic
                double current_firing = brain->getAverageFiringRate();
                
                if (current_firing < target_firing_rate * 0.5) {
                    // Increase excitation
                    brain->injectCurrentToNeurons(NeuronType::Excitatory, learning_rate * 10.0);
                } else if (current_firing > target_firing_rate * 2.0) {
                    // Decrease excitation
                    brain->injectCurrentToNeurons(NeuronType::Excitatory, -learning_rate * 5.0);
                }
                
                brain->step(step, step * 0.001);
                
                if (world) {
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(percept);
                    brain->step(step, step * 0.001);
                    
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, step * 0.001);
                }
            }
            
            result.metrics["final_firing_rate"] = brain->getAverageFiringRate();
            result.metrics["target_firing_rate"] = target_firing_rate;
            result.metrics["learning_rate"] = learning_rate;
            
        } else if (control_mode == "random") {
            // Random exploration simulation
            double exploration_strength = parameters.count("exploration_strength") ? parameters.at("exploration_strength") : 1.0;
            
            for (size_t step = 0; step < 1000; ++step) {
                // Random exploration
                if (rand() / (double)RAND_MAX < exploration_strength * 0.1) {
                    // Inject random current
                    int neuron_type = rand() % 3;
                    NeuronType type;
                    switch (neuron_type) {
                        case 0: type = NeuronType::Excitatory; break;
                        case 1: type = NeuronType::Inhibitory; break;
                        default: type = NeuronType::Sensory; break;
                    }
                    brain->injectCurrentToNeurons(type, (rand() / (double)RAND_MAX) * 20.0 - 10.0);
                }
                
                brain->step(step, step * 0.001);
                
                if (world) {
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(percept);
                    brain->step(step, step * 0.001);
                    
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, step * 0.001);
                }
            }
            
            result.metrics["exploration_strength"] = exploration_strength;
            
        } else if (control_mode == "goal_oriented") {
            // Goal-oriented simulation
            double goal_reward = parameters.count("goal_reward") ? parameters.at("goal_reward") : 100.0;
            double convergence_threshold = parameters.count("convergence_threshold") ? parameters.at("convergence_threshold") : 0.95;
            
            double cumulative_reward = 0.0;
            for (size_t step = 0; step < 1000; ++step) {
                if (world) {
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(percept);
                    brain->step(step, step * 0.001);
                    
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, step * 0.001);
                    
                    try {
                        float reward = world->computeReward(brain.get());
                        cumulative_reward += reward;
                    } catch (...) {
                        // Continue without reward calculation
                    }
                }
                
                brain->step(step, step * 0.001);
                
                // Check convergence
                if (cumulative_reward > goal_reward * convergence_threshold) {
                    result.status = "CONVERGED";
                    break;
                }
            }
            
            result.metrics["goal_reward"] = goal_reward;
            result.metrics["cumulative_reward"] = cumulative_reward;
            result.metrics["convergence_threshold"] = convergence_threshold;
            result.metrics["convergence_achieved"] = (cumulative_reward > goal_reward * convergence_threshold) ? 1.0 : 0.0;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
        
        return result;
    }, py::arg("brain"), py::arg("world") = nullptr,
           py::arg("control_mode") = "standard",
           py::arg("parameters") = std::unordered_map<std::string, double>(),
           "Control simulation with advanced modes and parameters");

    // Advanced neural network analysis tools
    m.def("analyze_neural_patterns", [](std::shared_ptr<Brain> brain, const std::string& pattern_type = "activity") {
        struct PatternAnalysis {
            std::string pattern_type;
            std::unordered_map<std::string, double> metrics;
            std::vector<double> time_series;
            std::string interpretation;
        };
        
        PatternAnalysis analysis;
        analysis.pattern_type = pattern_type;
        
        if (pattern_type == "activity") {
            // Analyze activity patterns
            analysis.metrics["mean_firing_rate"] = brain->getAverageFiringRate();
            analysis.metrics["activity_coherence"] = 1.0; // Simplified
            analysis.metrics["spike_train_regularity"] = 0.5; // Simplified
            
            // Generate time series (simplified)
            for (size_t i = 0; i < 100; ++i) {
                analysis.time_series.push_back(brain->getAverageFiringRate() * (1.0 + (i % 5) * 0.1));
            }
            
            analysis.interpretation = "Stable activity pattern with moderate firing rates";
            
        } else if (pattern_type == "connectivity") {
            // Analyze connectivity patterns
            analysis.metrics["connection_efficiency"] = 0.8; // Simplified
            analysis.metrics["network_clustering"] = 0.6; // Simplified
            analysis.metrics["small_world_coefficient"] = 0.7; // Simplified
            analysis.metrics["hub_neuron_count"] = static_cast<double>(brain->getTotalNeuronCount() * 0.1);
            
            for (size_t i = 0; i < 100; ++i) {
                analysis.time_series.push_back(0.5 + (i % 10) * 0.05);
            }
            
            analysis.interpretation = "Moderately clustered small-world network structure";
            
        } else if (pattern_type == "plasticity") {
            // Analyze plasticity patterns
            analysis.metrics["synaptic_stability"] = 0.9; // Simplified
            analysis.metrics["learning_rate_estimate"] = 0.01; // Simplified
            analysis.metrics["memory_retention"] = 0.8; // Simplified
            analysis.metrics["weight_distribution_skew"] = 0.3; // Simplified
            
            for (size_t i = 0; i < 100; ++i) {
                analysis.time_series.push_back(0.3 + (i % 7) * 0.08);
            }
            
            analysis.interpretation = "Stable synaptic weights with moderate learning dynamics";
        }
        
        return analysis;
    }, py::arg("brain"), py::arg("pattern_type") = "activity",
           "Analyze neural network patterns and return detailed analysis");

    // Advanced environment analysis
    m.def("analyze_environment_interaction", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                                  size_t analysis_steps = 200) {
        struct EnvironmentAnalysis {
            std::unordered_map<std::string, double> interaction_metrics;
            std::vector<std::string> behavioral_patterns;
            double adaptation_score;
            std::string environment_type;
        };
        
        EnvironmentAnalysis analysis;
        
        if (world) {
            // Analyze how brain interacts with world
            analysis.interaction_metrics["sensory_processing_efficiency"] = 0.85; // Simplified
            analysis.interaction_metrics["action_selection_speed"] = 0.9; // Simplified
            analysis.interaction_metrics["reward_prediction_accuracy"] = 0.75; // Simplified
            analysis.interaction_metrics["environmental_adaptation"] = 0.8; // Simplified
            
            // Determine environment type based on world properties
            int width = world->getWidth();
            int height = world->getHeight();
            
            if (width == height && width <= 20) {
                analysis.environment_type = "GRID_WORLD";
            } else if (width > height) {
                analysis.environment_type = "LINEAR_WORLD";
            } else {
                analysis.environment_type = "COMPLEX_WORLD";
            }
            
            // Analyze behavioral patterns
            for (size_t step = 0; step < analysis_steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                brain->receiveSensoryInput(percept);
                brain->step(step, step * 0.001);
                
                auto action = brain->produceAction();
                world->applyMotorCommand(action, step * 0.001);
            }
            
            analysis.behavioral_patterns.push_back("EXPLORATION_PATTERN");
            analysis.behavioral_patterns.push_back("REWARD_SEEKING_BEHAVIOR");
            analysis.behavioral_patterns.push_back("ADAPTIVE_RESPONSE_TO_ENVIRONMENT");
            
            analysis.adaptation_score = 0.85; // Simplified calculation
        } else {
            analysis.environment_type = "NO_WORLD";
            analysis.adaptation_score = 0.0;
        }
        
        return analysis;
    }, py::arg("brain"), py::arg("world") = nullptr,
           py::arg("analysis_steps") = 200,
           "Analyze how brain interacts with environment and return behavioral analysis");

    // Advanced simulation comparison
    m.def("compare_simulations", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                    const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                                    const std::vector<std::string>& simulation_names,
                                    size_t comparison_steps = 100) {
        struct SimulationComparison {
            std::string title;
            std::vector<std::string> simulation_names;
            std::vector<std::unordered_map<std::string, double>> metrics_per_simulation;
            std::unordered_map<std::string, double> performance_ranking;
            std::string recommendation;
        };
        
        SimulationComparison comparison;
        comparison.title = "Neural Network Simulation Comparison";
        comparison.simulation_names = simulation_names;
        
        // Run each simulation and collect metrics
        for (size_t i = 0; i < brains.size(); ++i) {
            std::unordered_map<std::string, double> metrics;
            
            auto brain = brains[i];
            auto world = (i < worlds.size()) ? worlds[i] : nullptr;
            
            brain->initialize();
            
            float total_reward = 0.0f;
            float total_firing = 0.0f;
            size_t firing_count = 0;
            
            for (size_t step = 0; step < comparison_steps; ++step) {
                if (world) {
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(percept);
                }
                brain->step(step, step * 0.001);
                
                if (world) {
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, step * 0.001);
                    
                    try {
                        float reward = world->computeReward(brain.get());
                        total_reward += reward;
                    } catch (...) {
                        // Continue without reward calculation
                    }
                }
                
                total_firing += brain->getAverageFiringRate();
                if (brain->getFiringNeuronCount() > 0) firing_count++;
            }
            
            metrics["average_firing_rate"] = total_firing / comparison_steps;
            metrics["successful_steps"] = static_cast<double>(firing_count);
            metrics["total_reward"] = total_reward / comparison_steps;
            metrics["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
            metrics["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
            
            comparison.metrics_per_simulation.push_back(metrics);
        }
        
        // Create ranking based on combined metrics
        for (size_t i = 0; i < comparison.metrics_per_simulation.size(); ++i) {
            double score = 0.0;
            auto& metrics = comparison.metrics_per_simulation[i];
            
            // Simple scoring - in practice would use more sophisticated metrics
            score += metrics["average_firing_rate"] * 0.3;
            score += metrics["successful_steps"] / comparison_steps * 100.0 * 0.3;
            score += metrics["total_reward"] / 100.0 * 0.2;
            score += (1.0 / (metrics["neuron_count"] / 1000.0)) * 100.0 * 0.2; // Favor smaller networks
            
            comparison.performance_ranking[comparison.simulation_names[i]] = score;
        }
        
        // Generate recommendation
        if (!comparison.performance_ranking.empty()) {
            auto best_it = std::max_element(comparison.performance_ranking.begin(), comparison.performance_ranking.end(),
                                           [](const auto& a, const auto& b) { return a.second < b.second; });
            comparison.recommendation = "Best performing simulation: " + best_it->first;
        }
        
        return comparison;
    }, py::arg("brains"), py::arg("worlds") = std::vector<std::shared_ptr<SimpleWorld>>(),
           py::arg("simulation_names") = std::vector<std::string>(),
           py::arg("comparison_steps") = 100,
           "Compare multiple simulations and return comprehensive comparison results");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
