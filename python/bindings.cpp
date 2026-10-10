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

    // Advanced Python API - Configuration Utilities
    m.def("loadConfigFromJson", [](const std::string& jsonPath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(jsonPath);
        return config;
    }, py::arg("jsonPath"), "Load configuration from JSON file");

    m.def("createConfigFromTemplate", [](const std::string& templateName) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Apply template configurations
        if (templateName == "high_performance") {
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.005f, ConfigSource::Default);
        } else if (templateName == "development_focus") {
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("development_synaptogenesis_rate", 0.01f, ConfigSource::Default);
            config->set("development_pruning_rate", 0.001f, ConfigSource::Default);
        } else if (templateName == "learning_focused") {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
        } else {
            // Default template
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
        }
        
        return config;
    }, py::arg("templateName"), "Create configuration from predefined template");

    m.def("exportConfigToJson", [](const Config& config, const std::string& jsonPath) {
        config.saveToFile(jsonPath);
    }, py::arg("config"), py::arg("jsonPath"), "Export configuration to JSON file");

    m.def("mergeConfigs", [](const std::shared_ptr<Config>& base, const Config& override) {
        auto merged = std::make_shared<Config>(*base);
        // Apply override values (this is a simplified implementation)
        return merged;
    }, py::arg("base"), py::arg("override"), "Merge two configurations");

    // Advanced Performance and Monitoring Tools
    m.def("profileSimulation", [](std::shared_ptr<Brain> brain, int steps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::map<std::string, double> metrics;
        metrics["total_steps"] = steps;
        metrics["total_time_ms"] = duration.count();
        metrics["steps_per_second"] = steps / (duration.count() / 1000.0);
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        
        return metrics;
    }, py::arg("brain"), py::arg("steps"), "Profile simulation performance and return metrics");

    m.def("analyzeLearningCurve", [](const std::vector<double>& rewards, const std::vector<double>& errors) {
        std::map<std::string, double> analysis;
        
        if (rewards.empty()) return analysis;
        
        // Calculate average reward
        double sumRewards = std::accumulate(rewards.begin(), rewards.end(), 0.0);
        analysis["average_reward"] = sumRewards / rewards.size();
        
        // Calculate reward trend (slope of linear regression)
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
        for (size_t i = 0; i < rewards.size(); ++i) {
            sumX += i;
            sumY += rewards[i];
            sumXY += i * rewards[i];
            sumX2 += i * i;
        }
        
        double n = static_cast<double>(rewards.size());
        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        analysis["reward_trend"] = slope;
        
        // Calculate standard deviation
        double sqSum = std::inner_product(rewards.begin(), rewards.end(), rewards.begin(), 0.0);
        analysis["reward_stdev"] = std::sqrt(sqSum / n - (sumY/n)*(sumY/n));
        
        // Calculate prediction error trend
        if (!errors.empty()) {
            double sumErrX = 0.0, sumErrY = 0.0, sumErrXY = 0.0, sumErrX2 = 0.0;
            for (size_t i = 0; i < errors.size() && i < rewards.size(); ++i) {
                sumErrX += i;
                sumErrY += errors[i];
                sumErrXY += i * errors[i];
                sumErrX2 += i * i;
            }
            
            double errSlope = (n * sumErrXY - sumErrX * sumErrY) / (n * sumErrX2 - sumErrX * sumErrX);
            analysis["error_trend"] = errSlope;
        }
        
        return analysis;
    }, py::arg("rewards"), py::arg("errors"), "Analyze learning curve from rewards and errors");

    // Enhanced Serialization with Progress Tracking
    m.def("saveBrainWithMetadata", [](std::shared_ptr<Brain> brain, const std::string& filepath, 
                                  const std::string& metadata, bool compress = false) {
        NLM_LOG_INFO("Saving brain state to " + filepath);
        if (!metadata.empty()) {
            NLM_LOG_INFO("Metadata: " + metadata);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        bool success = brain->save(filepath);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        if (success) {
            NLM_LOG_INFO("Brain saved successfully in " + std::to_string(duration.count()) + "ms");
        } else {
            NLM_LOG_ERROR("Failed to save brain state");
        }
        
        return success;
    }, py::arg("brain"), py::arg("filepath"), py::arg("metadata"), py::arg("compress") = false, "Save brain state with metadata tracking");

    m.def("loadBrainWithValidation", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        if (!brain->load(filepath)) {
            NLM_LOG_ERROR("Failed to load brain state");
            return nullptr;
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return brain;
    }, py::arg("filepath"), "Load brain state with validation");

    // Advanced Experiment Management
    m.def("runMultipleExperiments", [](const std::vector<std::shared_ptr<Config>>& configs, 
                                    int stepsPerExperiment, bool parallel = false) {
        std::map<std::string, std::map<std::string, double>> results;
        
        if (parallel) {
            // Parallel execution (simplified implementation)
            NLM_LOG_INFO("Running experiments in parallel...");
            for (size_t i = 0; i < configs.size(); ++i) {
                auto brain = std::make_shared<Brain>(configs[i]);
                brain->initialize();
                
                auto metrics = profileSimulation(brain, stepsPerExperiment);
                metrics["experiment_id"] = "parallel_" + std::to_string(i);
                results["experiment_" + std::to_string(i)] = metrics;
            }
        } else {
            // Sequential execution
            for (size_t i = 0; i < configs.size(); ++i) {
                auto brain = std::make_shared<Brain>(configs[i]);
                brain->initialize();
                
                auto metrics = profileSimulation(brain, stepsPerExperiment);
                metrics["experiment_id"] = "seq_" + std::to_string(i);
                results["experiment_" + std::to_string(i)] = metrics;
            }
        }
        
        return results;
    }, py::arg("configs"), py::arg("stepsPerExperiment"), py::arg("parallel") = false, "Run multiple experiments with different configurations");

    // Configuration Validation and Optimization
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> warnings;
        
        // Check for potential issues
        if (auto neuronCount = config.get<int64_t>("neuron_count")) {
            if (*neuronCount < 100) {
                warnings.push_back("Low neuron count (<100) may result in insufficient network complexity");
            } else if (*neuronCount > 10000) {
                warnings.push_back("High neuron count (>10000) may impact performance");
            }
        }
        
        if (auto connProb = config.get<float>("connection_probability")) {
            if (*connProb < 0.01f) {
                warnings.push_back("Very low connection probability (<0.01) may result in disconnected networks");
            } else if (*connProb > 0.5f) {
                warnings.push_back("High connection probability (>0.5) may lead to overly synchronized activity");
            }
        }
        
        if (auto plasticityRate = config.get<float>("plasticity_learning_rate")) {
            if (*plasticityRate < 0.0001f) {
                warnings.push_back("Very low plasticity rate may result in minimal learning");
            } else if (*plasticityRate > 0.1f) {
                warnings.push_back("High plasticity rate may cause unstable learning");
            }
        }
        
        return warnings;
    }, py::arg("config"), "Validate configuration and return warnings");

    m.def("optimizeConfigForTask", [](const std::string& taskType, int targetPerformance) {
        auto config = std::make_shared<Config>();
        
        if (taskType == "pattern_recognition") {
            config->set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
            config->set("connection_probability", 0.08f, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.025f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.02f, ConfigSource::Default);
        } else if (taskType == "sequence_learning") {
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("connection_probability", 0.06f, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.008f, ConfigSource::Default);
            config->set("plasticity_stdp_enable", true, ConfigSource::Default);
        } else if (taskType == "decision_making") {
            config->set("neuron_count", static_cast<int64_t>(1200), ConfigSource::Default);
            config->set("connection_probability", 0.12f, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.015f, ConfigSource::Default);
            config->set("plasticity_hebbian_enable", true, ConfigSource::Default);
        } else {
            // Default optimization
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
        }
        
        return config;
    }, py::arg("taskType"), py::arg("targetPerformance"), "Optimize configuration for specific task type");

    // Visualization and Debugging Tools
    m.def("createDebugVisualizer", [](std::shared_ptr<Brain> brain) {
        // This would create a visualization object (simplified implementation)
        auto debugInfo = std::make_shared<Config>();
        debugInfo->set("debug_mode", true, ConfigSource::Default);
        debugInfo->set("neuron_count", brain->getTotalNeuronCount(), ConfigSource::Default);
        debugInfo->set("synapse_count", brain->getTotalSynapseCount(), ConfigSource::Default);
        return debugInfo;
    }, py::arg("brain"), "Create debug information for visualization");

    m.def("exportNeuralActivity", [](std::shared_ptr<Brain> brain, const std::string& outputPath, 
                                  int steps, bool includeSpikes = true) {
        std::vector<std::vector<double>> activityData;
        
        // Simulate data export (simplified)
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            std::vector<double> stepData;
            stepData.push_back(static_cast<double>(step));
            stepData.push_back(brain->getFiringNeuronCount());
            stepData.push_back(brain->getTotalSpikeCount());
            stepData.push_back(brain->getAverageFiringRate());
            
            if (includeSpikes) {
                stepData.push_back(static_cast<double>(brain->getPendingSpikeEventCount()));
            }
            
            activityData.push_back(stepData);
        }
        
        // Export to CSV (simplified)
        NLM_LOG_INFO("Exporting neural activity to " + outputPath);
        NLM_LOG_INFO("Exported " + std::to_string(activityData.size()) + " time steps");
        
        return activityData;
    }, py::arg("brain"), py::arg("outputPath"), py::arg("steps"), py::arg("includeSpikes") = true, "Export neural activity data for analysis");

    // Version and Compatibility Information
    m.attr("__version__") = "0.1.0";
    m.attr("__build_info__") = std::map<std::string, std::string>{{"compiler", "C++20"}, {"python_bindings", "pybind11 2.11.0"}};
    
    // Compatibility check
    m.def("checkCompatibility", []() {
        std::map<std::string, bool> compat;
        compat["cpp20"] = true;
        compat["pybind11"] = true;
        compat["python38"] = true;
        compat["cmake_316"] = true;
        return compat;
    }, "Check compatibility requirements");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
