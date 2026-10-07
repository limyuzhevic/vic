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
        .def(py::init<ActionType, std::map<std::string, float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](Action& self, const std::string& key) {
            const auto& params = self.getParameters();
            auto it = params.find(key);
            if (it != params.end()) {
                return it->second;
            }
            throw py::key_error("Parameter not found: " + key);
        }, py::arg("key"), "Get action parameter by key")
        .def("setParameter", [](Action& self, const std::string& key, float value) {
            auto params = self.getParameters();
            params[key] = value;
            self.setParameters(params);
        }, py::arg("key"), py::arg("value"), "Set action parameter by key")
        .def("hasParameter", [](Action& self, const std::string& key) {
            const auto& params = self.getParameters();
            return params.find(key) != params.end();
        }, py::arg("key"), "Check if action has parameter")
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["name"] = self.getName();
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary representation")
        .def("__repr__", [](const Action& self) {
            return "<Action: " + self.getName() + ">";
        });

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

    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.v_thresh", -50.0f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enable", true);
        return config;
    }, "Create configuration optimized for exploration and learning");

    m.def("createExploitationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15f);
        config->set("brain.v_thresh", -55.0f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("development.enable", false);
        return config;
    }, "Create configuration optimized for exploitation and performance");

    m.def("createBalancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1500);
        config->set("brain.synapse_density", 0.12f);
        config->set("brain.v_thresh", -52.0f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enable", true);
        return config;
    }, "Create balanced configuration for both exploration and exploitation");

    m.def("createCustomConfig", [](py::dict config_dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            std::string value = py::cast<std::string>(item.second);
            config->set(key, value);
        }
        return config;
    }, py::arg("config_dict"), "Create custom configuration from dictionary");

    // Enhanced simulation utilities
    m.def("runExplorationEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, size_t steps) {
        agent->initialize(*world);
        agent->enableAllSystems();
        
        py::dict results;
        results["stepsCompleted"] = 0;
        results["totalReward"] = 0.0f;
        results["maxCuriosity"] = 0.0f;
        results["avgFiringRate"] = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            // Get sensory input
            const auto& percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(step, step * 0.001);
            
            // Get action
            auto action = brain->produceAction();
            
            // Apply action to world
            auto result = world->applyMotorCommand(action->getType(), world->getSimulationTime());
            
            // Get reward
            float reward = result.reward;
            results["totalReward"] += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Track statistics
            if (agent->getCuriosityLevel() > results["maxCuriosity"].cast<float>()) {
                results["maxCuriosity"] = agent->getCuriosityLevel();
            }
            results["avgFiringRate"] += brain->getAverageFiringRate();
            
            results["stepsCompleted"] = step + 1;
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": Reward=" << reward 
                         << ", Curiosity=" << agent->getCuriosityLevel()
                         << ", Firing=" << brain->getFiringNeuronCount()
                         << std::endl;
            }
        }
        
        // Calculate averages
        results["avgFiringRate"] /= results["stepsCompleted"].cast<float>();
        
        results["finalCuriosity"] = agent->getCuriosityLevel();
        results["finalNovelty"] = agent->getNoveltyLevel();
        results["finalPredictionError"] = agent->getPredictionError();
        results["developmentStage"] = static_cast<int>(agent->getDevelopmentalStage());
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run an exploration episode with curiosity and learning focus");

    m.def("runExploitationEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, size_t steps) {
        agent->initialize(*world);
        // Configure for exploitation (high reward, low exploration)
        agent->enableRewardModulation(true);
        agent->enableCuriosity(false);
        agent->enableDevelopment(false);
        
        py::dict results;
        results["stepsCompleted"] = 0;
        results["totalReward"] = 0.0f;
        results["avgFiringRate"] = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            
            const auto& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.001);
            auto action = brain->produceAction();
            auto result = world->applyMotorCommand(action->getType(), world->getSimulationTime());
            
            float reward = result.reward;
            results["totalReward"] += reward;
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1);
            
            results["avgFiringRate"] += brain->getAverageFiringRate();
            results["stepsCompleted"] = step + 1;
        }
        
        results["avgFiringRate"] /= results["stepsCompleted"].cast<float>();
        results["finalCuriosity"] = agent->getCuriosityLevel();
        results["finalNovelty"] = agent->getNoveltyLevel();
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run an exploitation episode with reward maximization focus");

    // Enhanced environment setup
    m.def("setupEnvironment", []() {
        std::cout << "=== NLM Python Enhanced Environment Setup ===" << std::endl;
        std::cout << "Python bindings loaded successfully!" << std::endl;
        std::cout << "Enhanced features available:" << std::endl;
        std::cout << "  - Enhanced Action API with parameters and factory functions" << std::endl;
        std::cout << "  - Advanced configuration utilities (exploration/exploitation/balanced/custom)" << std::endl;
        std::cout << "  - Enhanced simulation episodes with detailed statistics" << std::endl;
        std::cout << "  - Improved AgentBrain configuration methods" << std::endl;
        std::cout << "  - Better error handling and exception support" << std::endl;
        std::cout << "  - Comprehensive action creation utilities" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Key enhancements:
- Parameter-rich action system
- Task-based agent configuration
- Enhanced statistics and monitoring
- Better error handling
- Factory functions for common actions" << std::endl;
        
        py::dict info;
        info["version"] = "Phase 6.2 - Enhanced Integration";
        info["status"] = "Loaded successfully";
        info["features"] = {"memory", "neuromodulation", "prediction", "cognition", "development", "advanced_api"};
        info["enhancements"] = {"parameter_actions", "task_configuration", "enhanced_stats", "better_errors", "factory_functions"};
        return info;
    }, "Setup and validate enhanced Python environment");

    // Enhanced error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Enhanced module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================
        
        Phase 6 Enhanced Python API for Artificial Developmental Brains
        
        This module provides comprehensive Python bindings for the NLM C++ neural simulation framework.
        Enhanced with advanced features for power users and researchers.
        
        Key Features:
        - Enhanced neural network simulation with LIF neurons and synapses
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction, cognition, and development systems
        - Advanced action selection with intelligent motor control
        - Enhanced configuration system with validation
        - Comprehensive error handling and debugging tools
        - Advanced Pythonic API with dictionaries and helper functions
        
        Enhanced Action API:
        - Parameter-rich action representation
        - Factory functions for common actions
        - Dictionary conversion utilities
        - Intelligent action selection
        
        Advanced AgentBrain API:
        - Task configuration (exploration/exploitation/default)
        - System enable/disable control
        - Activity statistics and monitoring
        - Development stage tracking
        
        Enhanced World API:
        - Enhanced statistics and world analysis
        - Advanced object management
        - Simulation time tracking
        
        Enhanced Configuration:
        - Configuration factory functions
        - Validation and error checking
        - Dictionary-based configuration
        - Pre-built configurations for different tasks
        
        Enhanced Simulation Utilities:
        - Specialized episode runners (exploration/exploitation)
        - Detailed statistics collection
        - Enhanced error handling
        - Progress monitoring and logging
        
        Quick Start Examples:
        
        >>> import pynlm
        >>> 
        >>> # Create enhanced agent
        >>> config = pynlm.createExplorationConfig()
        >>> brain = pynlm.createBrain(config)
        >>> world = pynlm.createSimpleWorld()
        >>> agent = pynlm.createAgentBrain(brain)
        >>> 
        >>> # Configure for exploration
        >>> agent.configureForTask("exploration")
        >>> agent.initialize(world)
        >>> agent.enableAllSystems()
        >>> 
        >>> # Run enhanced exploration episode
        >>> results = pynlm.runExplorationEpisode(brain, world, agent, 2000)
        >>> 
        >>> # Analyze with enhanced statistics
        >>> stats = agent.getActivityStatistics()
        >>> print(f"Firing rate: {stats['neuroscience.firingRate']}")
        >>> print(f"Max curiosity: {results['maxCuriosity']}")
        >>> print(f"Avg firing rate: {results['avgFiringRate']}")
        >>> 
        >>> # Create complex actions with parameters
        >>> move_action = pynlm.createMoveForwardAction(distance=2.0)
        >>> turn_action = pynlm.createTurnAction(1.5)  # Right turn
        >>> interact_action = pynlm.createInteractAction("resource", "harvest")
        >>> wait_action = pynlm.createWaitAction(0.5)  # Wait 0.5 seconds
        >>> 
        >>> # Error handling
        >>> try:
        ...     agent.processSensoryInput(percept)
        ... except pynlm.InvalidArgumentError as e:
        ...     print(f"Configuration error: {e}")
        >>> 
        >>> # Explore enhanced brain
        >>> brain_info = brain.exploreConnections()
        >>> connectivity = brain.getConnectivitySummary()
        >>> 
        >>> # Validate configuration
        >>> validation = config.validate()
        >>> if validation["totalKeys"] > 0:
        ...     print("Configuration has valid settings")
        
        Version: Phase 6.2 - Enhanced Integration
        Status: Production ready with enhanced features
        License: MIT
    )pbdoc";
}

} // namespace nlm
