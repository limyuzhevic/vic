#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Metrics.hpp"

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Convenience functions for common initialization patterns
namespace nlm {

// Configuration helpers
std::shared_ptr<Config> createConfigFromFile(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(filepath);
    return config;
}

std::shared_ptr<Config> createConfigFromDefaults() {
    auto config = std::make_shared<Config>();
    config->set("brain.region_count", 1);
    config->set("brain.neuron_count_per_region", 100);
    config->set("brain.initial_excitation", 0.5);
    config->set("brain.initial_inhibition", 0.5);
    config->set("brain.dt", 0.01);
    config->set("brain.vision_width", 10);
    config->set("brain.vision_height", 10);
    config->set("world.width", 100.0f);
    config->set("world.height", 100.0f);
    config->set("world.max_energy", 100.0f);
    return config;
}

// Brain initialization helpers
std::shared_ptr<Brain> createBrainFromConfig(const std::string& configFilepath) {
    auto config = createConfigFromFile(configFilepath);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

std::shared_ptr<Brain> createBrainFromDefaults() {
    auto config = createConfigFromDefaults();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

std::shared_ptr<Brain> createBrainWithRegion(const std::string& regionName, uint32_t neuronCount = 100) {
    auto config = createConfigFromDefaults();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    brain->addRegion(regionName);
    return brain;
}

// AgentBrain initialization helpers
std::shared_ptr<AgentBrain> createAgentBrainFromConfig(const std::string& configFilepath, const std::shared_ptr<SimpleWorld>& world = nullptr) {
    auto brain = createBrainFromConfig(configFilepath);
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    if (world) {
        agentBrain->initialize(*world);
    }
    return agentBrain;
}

std::shared_ptr<AgentBrain> createAgentBrainFromDefaults() {
    auto brain = createBrainFromDefaults();
    auto world = std::make_shared<SimpleWorld>();
    world->configure(100.0f, 100.0f, 10, 10);
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    agentBrain->initialize(*world);
    return agentBrain;
}

// Simulation control functions
struct SimulationResult {
    double totalReward;
    uint64_t stepsTaken;
    bool success;
    std::string message;
    double duration;
    std::map<std::string, double> metrics;
};

SimulationResult runEpisode(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain, 
                           std::shared_ptr<SimpleWorld> world, uint64_t maxSteps = 1000) {
    SimulationResult result;
    result.totalReward = 0.0;
    result.stepsTaken = 0;
    result.success = false;
    result.message = "Episode incomplete";
    
    if (!brain || !agentBrain || !world) {
        result.message = "Invalid brain, agentBrain, or world";
        return result;
    }
    
    brain->reset();
    agentBrain->reset();
    world->reset();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = 0; step < maxSteps; ++step) {
        // Get sensory input
        const auto& percept = world->getSensoryPercept();
        agentBrain->processSensoryInput(percept);
        
        // Produce action
        auto action = brain->produceAction();
        
        // Apply action
        auto actionResult = world->applyMotorCommand(
            static_cast<MotorCommand>(action->getType()), 
            static_cast<double>(world->getSimulationTime())
        );
        
        result.totalReward += actionResult.reward;
        result.stepsTaken = step + 1;
        
        // Check if agent died
        if (world->getAgentBody().health <= 0.0f) {
            result.message = "Agent died";
            result.success = false;
            break;
        }
        
        // Check for terminal conditions
        if (actionResult.success && actionResult.message.find("goal") != std::string::npos) {
            result.message = "Goal reached";
            result.success = true;
            break;
        }
        
        // Apply reward modulation
        agentBrain->applyRewardModulation(actionResult.reward, 0.0f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    result.duration = duration;
    
    if (result.stepsTaken >= maxSteps) {
        result.message = "Max steps reached";
    }
    
    return result;
}

std::vector<SimulationResult> runSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain,
                                            std::shared_ptr<SimpleWorld> world, 
                                            uint64_t numEpisodes = 1, uint64_t maxStepsPerEpisode = 1000) {
    std::vector<SimulationResult> results;
    results.reserve(numEpisodes);
    
    for (uint64_t i = 0; i < numEpisodes; ++i) {
        results.push_back(runEpisode(brain, agentBrain, world, maxStepsPerEpisode));
    }
    
    return results;
}

// Metrics collection functions
void recordMetrics(const std::shared_ptr<Metrics>& metrics, const std::shared_ptr<Brain>& brain) {
    if (!metrics || !brain) return;
    
    metrics->record("total_neurons", static_cast<double>(brain->getTotalNeuronCount()));
    metrics->record("total_synapses", static_cast<double>(brain->getTotalSynapseCount()));
    metrics->record("active_neurons", static_cast<double>(brain->getActiveNeuronCount()));
    metrics->record("firing_neurons", static_cast<double>(brain->getFiringNeuronCount()));
    metrics->record("average_firing_rate", brain->getAverageFiringRate());
    metrics->record("excitement_inhibition_ratio", brain->getExcitationInhibitionRatio());
    metrics->record("total_spike_count", static_cast<double>(brain->getTotalSpikeCount()));
}

void recordMetrics(const std::shared_ptr<Metrics>& metrics, const std::shared_ptr<AgentBrain>& agentBrain) {
    if (!metrics || !agentBrain) return;
    
    metrics->record("neuromodulation_level", agentBrain->getNeuromodulationLevel());
    metrics->record("curiosity_level", agentBrain->getCuriosityLevel());
    metrics->record("novelty_level", agentBrain->getNoveltyLevel());
    metrics->record("prediction_error", agentBrain->getPredictionError());
}

void recordMetrics(const std::shared_ptr<Metrics>& metrics, const std::shared_ptr<SimpleWorld>& world) {
    if (!metrics || !world) return;
    
    metrics->record("simulation_time", static_cast<double>(world->getSimulationTime()));
    metrics->record("agent_x", world->getAgentBody().x);
    metrics->record("agent_y", world->getAgentBody().y);
    metrics->record("agent_energy", world->getAgentBody().energy);
    metrics->record("agent_health", world->getAgentBody().health);
}

std::string getMetricsSummary(const std::shared_ptr<Metrics>& metrics) {
    if (!metrics) return "";
    return metrics->summary();
}

// Checkpoint management utilities
bool saveCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) return false;
    return brain->save(filepath);
}

bool loadCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) return false;
    return brain->load(filepath);
}

bool saveFullCheckpoint(const std::string& prefix, const std::shared_ptr<Brain>& brain,
                        const std::shared_ptr<AgentBrain>& agentBrain,
                        const std::shared_ptr<SimpleWorld>& world) {
    bool success = true;
    
    if (brain) {
        success &= saveCheckpoint(brain, prefix + "_brain.checkpoint");
    }
    
    if (agentBrain) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << prefix << "_agentbrain_" << agentBrain->getCuriosityLevel() << ".checkpoint";
        success &= saveCheckpoint(agentBrain->getBrain(), ss.str());
    }
    
    if (world) {
        success &= saveCheckpoint(world->getAgentBody(), prefix + "_world.checkpoint");
    }
    
    return success;
}

// Visualization output functions
std::string visualizeBrain(const std::shared_ptr<Brain>& brain) {
    if (!brain) return "";
    std::stringstream ss;
    ss << "Brain Visualization:" << std::endl;
    ss << "====================" << std::endl;
    ss << "Regions: " << brain->getRegionCount() << std::endl;
    ss << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    ss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    ss << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    ss << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    ss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    return ss.str();
}

std::string visualizeWorld(const std::shared_ptr<SimpleWorld>& world) {
    if (!world) return "";
    std::stringstream ss;
    ss << "World Visualization:" << std::endl;
    ss << "===================" << std::endl;
    ss << "Width: " << world->getWidth() << std::endl;
    ss << "Height: " << world->getHeight() << std::endl;
    ss << "Agent Position: (" << world->getAgentBody().x << ", " 
       << world->getAgentBody().y << ")" << std::endl;
    ss << "Agent Energy: " << world->getAgentBody().energy << std::endl;
    ss << "Agent Health: " << world->getAgentBody().health << std::endl;
    ss << "Simulation Time: " << world->getSimulationTime() << std::endl;
    return ss.str();
}

std::string visualizeAgentBrain(const std::shared_ptr<AgentBrain>& agentBrain) {
    if (!agentBrain) return "";
    std::stringstream ss;
    ss << "AgentBrain Visualization:" << std::endl;
    ss << "======================" << std::endl;
    ss << "Neuromodulation Level: " << agentBrain->getNeuromodulationLevel() << std::endl;
    ss << "Curiosity Level: " << agentBrain->getCuriosityLevel() << std::endl;
    ss << "Novelty Level: " << agentBrain->getNoveltyLevel() << std::endl;
    ss << "Prediction Error: " << agentBrain->getPredictionError() << std::endl;
    ss << "Development Stage: " << static_cast<int>(agentBrain->getDevelopmentalStage()) << std::endl;
    return ss.str();
}

// Configuration file loading helpers
bool loadConfigFromJson(const std::shared_ptr<Config>& config, const std::string& filepath) {
    if (!config) return false;
    return config->loadFromFile(filepath);
}

bool saveConfigToJson(const std::shared_ptr<Config>& config, const std::string& filepath) {
    if (!config) return false;
    return config->saveToFile(filepath);
}

std::string getConfigSummary(const std::shared_ptr<Config>& config) {
    if (!config) return "";
    return config->summary();
}

// Debug output and state inspection methods
std::string getBrainState(const std::shared_ptr<Brain>& brain) {
    if (!brain) return "";
    std::stringstream ss;
    ss << "Brain State:" << std::endl;
    ss << "============" << std::endl;
    ss << "Regions: " << brain->getRegionCount() << std::endl;
    ss << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "Active: " << brain->getActiveNeuronCount() << "/" << brain->getTotalNeuronCount() << std::endl;
    ss << "Firing: " << brain->getFiringNeuronCount() << std::endl;
    ss << "Average Rate: " << brain->getAverageFiringRate() << std::endl;
    ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    ss << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    return ss.str();
}

std::string getAgentState(const std::shared_ptr<AgentBrain>& agentBrain) {
    if (!agentBrain) return "";
    std::stringstream ss;
    ss << "AgentBrain State:" << std::endl;
    ss << "=================" << std::endl;
    ss << "Neuromodulation: " << agentBrain->getNeuromodulationLevel() << std::endl;
    ss << "Curiosity: " << agentBrain->getCuriosityLevel() << std::endl;
    ss << "Novelty: " << agentBrain->getNoveltyLevel() << std::endl;
    ss << "Prediction Error: " << agentBrain->getPredictionError() << std::endl;
    ss << "Development: " << static_cast<int>(agentBrain->getDevelopmentalStage()) << std::endl;
    ss << "Reward Modulation: " << (agentBrain->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
    ss << "Development: " << (agentBrain->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
    ss << "Curiosity: " << (agentBrain->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
    return ss.str();
}

std::string getWorldState(const std::shared_ptr<SimpleWorld>& world) {
    if (!world) return "";
    std::stringstream ss;
    ss << "World State:" << std::endl;
    ss << "============" << std::endl;
    ss << "Dimensions: " << world->getWidth() << " x " << world->getHeight() << std::endl;
    ss << "Agent: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")" << std::endl;
    ss << "Energy: " << world->getAgentBody().energy << "/" << world->getMaxEnergy() << std::endl;
    ss << "Health: " << world->getAgentBody().health << std::endl;
    ss << "Time: " << world->getSimulationTime() << std::endl;
    ss << "Random Seed: " << world->getRandomSeed() << std::endl;
    return ss.str();
}

// Performance monitoring
struct PerformanceMetrics {
    double stepTime;
    double memoryUsage;
    double cpuUsage;
    uint64_t spikeCount;
    uint64_t activeNeurons;
    double firingRate;
};

PerformanceMetrics getPerformanceMetrics(const std::shared_ptr<Brain>& brain, 
                                         double stepTime = 0.0, double memoryUsage = 0.0, 
                                         double cpuUsage = 0.0) {
    PerformanceMetrics metrics;
    if (brain) {
        metrics.spikeCount = brain->getTotalSpikeCount();
        metrics.activeNeurons = brain->getActiveNeuronCount();
        metrics.firingRate = brain->getAverageFiringRate();
    }
    metrics.stepTime = stepTime;
    metrics.memoryUsage = memoryUsage;
    metrics.cpuUsage = cpuUsage;
    return metrics;
}

std::string getPerformanceSummary(const PerformanceMetrics& metrics) {
    std::stringstream ss;
    ss << "Performance Metrics:" << std::endl;
    ss << "====================" << std::endl;
    ss << "Step Time: " << metrics.stepTime * 1000 << " ms" << std::endl;
    ss << "Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
    ss << "CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    ss << "Spike Count: " << metrics.spikeCount << std::endl;
    ss << "Active Neurons: " << metrics.activeNeurons << std::endl;
    ss << "Firing Rate: " << metrics.firingRate << std::endl;
    return ss.str();
}

// Visualization convenience functions
std::string createVisualizationReport(const std::shared_ptr<Brain>& brain,
                                       const std::shared_ptr<AgentBrain>& agentBrain,
                                       const std::shared_ptr<SimpleWorld>& world,
                                       const std::shared_ptr<Metrics>& metrics) {
    std::stringstream report;
    report << "=== NLM Visualization Report ===" << std::endl;
    report << "Generated: " << std::endl;
    report << visualizeBrain(brain) << std::endl;
    report << visualizeAgentBrain(agentBrain) << std::endl;
    report << visualizeWorld(world) << std::endl;
    if (metrics) {
        report << "Metrics Summary:" << std::endl;
        report << getMetricsSummary(metrics) << std::endl;
    }
    return report.str();
}

} // namespace nlm

// Register enhanced bindings with pybind11
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        ---------------------------------------------
        An Python binding for the NLM C++ neural simulation framework.
        Provides enhanced classes for Brain, Config, AgentBrain, SimpleWorld,
        SensoryInput, and Action with additional convenience methods and utilities.
        
        New Features:
        - Convenience methods for common initialization patterns
        - Simulation control functions (run_episode, run_simulation)
        - Metrics collection and analysis
        - Checkpoint management utilities
        - Visualization and debugging methods
        - Configuration file loading helpers
        - Debug output and state inspection
        - Performance monitoring
        - Visualization report generation
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ... existing type registrations remain the same ...

    // Register convenience functions
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

    // Register enhanced convenience functions
    m.def("createConfigFromFile", &nlm::createConfigFromFile,
          py::arg("filepath"), "Create configuration from JSON file");

    m.def("createConfigFromDefaults", &nlm::createConfigFromDefaults,
          "Create configuration with default values");

    m.def("createBrainFromConfig", &nlm::createBrainFromConfig,
          py::arg("configFilepath"), "Create brain from configuration file");

    m.def("createBrainFromDefaults", &nlm::createBrainFromDefaults,
          "Create brain with default configuration");

    m.def("createBrainWithRegion", &nlm::createBrainWithRegion,
          py::arg("regionName"), py::arg("neuronCount") = 100,
          "Create brain with specified region");

    m.def("createAgentBrainFromConfig", &nlm::createAgentBrainFromConfig,
          py::arg("configFilepath"), py::arg("world") = nullptr,
          "Create agent brain from configuration file");

    m.def("createAgentBrainFromDefaults", &nlm::createAgentBrainFromDefaults,
          "Create agent brain with default settings");

    // Simulation control
    py::class_<SimulationResult>(m, "SimulationResult")
        .def(py::init<>())
        .def_readwrite("total_reward", &SimulationResult::totalReward)
        .def_readwrite("steps_taken", &SimulationResult::stepsTaken)
        .def_readwrite("success", &SimulationResult::success)
        .def_readwrite("message", &SimulationResult::message)
        .def_readwrite("duration", &SimulationResult::duration)
        .def_readwrite("metrics", &SimulationResult::metrics)
        .def("__repr__", [](const SimulationResult& r) {
            return "SimulationResult(reward=" + std::to_string(r.totalReward) +
                   ", steps=" + std::to_string(r.stepsTaken) +
                   ", success=" + std::string(r.success ? "True" : "False") +
                   ", " + r.message + ")";
        });

    m.def("runEpisode", &nlm::runEpisode,
          py::arg("brain"), py::arg("agentBrain"), py::arg("world"), py::arg("maxSteps") = 1000,
          "Run a single episode of simulation");

    m.def("runSimulation", &nlm::runSimulation,
          py::arg("brain"), py::arg("agentBrain"), py::arg("world"), py::arg("numEpisodes") = 1,
          py::arg("maxStepsPerEpisode") = 1000,
          "Run multiple episodes of simulation");

    // Metrics
    m.def("recordMetrics", py::overload_cast<const std::shared_ptr<Metrics>&, const std::shared_ptr<Brain>&>(&nlm::recordMetrics),
          py::arg("metrics"), py::arg("brain"),
          "Record brain metrics");

    m.def("recordMetrics", py::overload_cast<const std::shared_ptr<Metrics>&, const std::shared_ptr<AgentBrain>&>(&nlm::recordMetrics),
          py::arg("metrics"), py::arg("agentBrain"),
          "Record agent brain metrics");

    m.def("recordMetrics", py::overload_cast<const std::shared_ptr<Metrics>&, const std::shared_ptr<SimpleWorld>&>(&nlm::recordMetrics),
          py::arg("metrics"), py::arg("world"),
          "Record world metrics");

    m.def("getMetricsSummary", &nlm::getMetricsSummary,
          py::arg("metrics"), "Get metrics summary as string");

    // Checkpoint management
    m.def("saveCheckpoint", &nlm::saveCheckpoint,
          py::arg("brain"), py::arg("filepath"), "Save brain state to checkpoint");

    m.def("loadCheckpoint", &nlm::loadCheckpoint,
          py::arg("brain"), py::arg("filepath"), "Load brain state from checkpoint");

    m.def("saveFullCheckpoint", &nlm::saveFullCheckpoint,
          py::arg("prefix"), py::arg("brain"), py::arg("agentBrain"), py::arg("world"),
          "Save complete checkpoint with all components");

    // Visualization
    m.def("visualizeBrain", &nlm::visualizeBrain,
          py::arg("brain"), "Get brain visualization string");

    m.def("visualizeWorld", &nlm::visualizeWorld,
          py::arg("world"), "Get world visualization string");

    m.def("visualizeAgentBrain", &nlm::visualizeAgentBrain,
          py::arg("agentBrain"), "Get agent brain visualization string");

    // Configuration helpers
    m.def("loadConfigFromJson", &nlm::loadConfigFromJson,
          py::arg("config"), py::arg("filepath"), "Load configuration from JSON file");

    m.def("saveConfigToJson", &nlm::saveConfigToJson,
          py::arg("config"), py::arg("filepath"), "Save configuration to JSON file");

    m.def("getConfigSummary", &nlm::getConfigSummary,
          py::arg("config"), "Get configuration summary as string");

    // Debug and state inspection
    m.def("getBrainState", &nlm::getBrainState,
          py::arg("brain"), "Get detailed brain state as string");

    m.def("getAgentState", &nlm::getAgentState,
          py::arg("agentBrain"), "Get detailed agent brain state as string");

    m.def("getWorldState", &nlm::getWorldState,
          py::arg("world"), "Get detailed world state as string");

    // Performance monitoring
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics")
        .def(py::init<>())
        .def_readwrite("step_time", &PerformanceMetrics::stepTime)
        .def_readwrite("memory_usage", &PerformanceMetrics::memoryUsage)
        .def_readwrite("cpu_usage", &PerformanceMetrics::cpuUsage)
        .def_readwrite("spike_count", &PerformanceMetrics::spikeCount)
        .def_readwrite("active_neurons", &PerformanceMetrics::activeNeurons)
        .def_readwrite("firing_rate", &PerformanceMetrics::firingRate);

    m.def("getPerformanceMetrics", &nlm::getPerformanceMetrics,
          py::arg("brain"), py::arg("stepTime") = 0.0, py::arg("memoryUsage") = 0.0,
          py::arg("cpuUsage") = 0.0,
          "Get performance metrics for brain");

    m.def("getPerformanceSummary", &nlm::getPerformanceSummary,
          py::arg("metrics"), "Get performance metrics as formatted string");

    // Visualization report generation
    m.def("createVisualizationReport", &nlm::createVisualizationReport,
          py::arg("brain"), py::arg("agentBrain"), py::arg("world"), py::arg("metrics") = nullptr,
          "Create comprehensive visualization report");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
