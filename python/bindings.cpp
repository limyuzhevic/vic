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

    // Create convenience functions for common patterns
    m.def("run_agent_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent, int num_steps, bool enableLearning = true) {
        if (!world || !agent || !brain) {
            throw std::runtime_error("Invalid brain, world, or agent provided");
        }
        
        world->reset();
        agent->reset();
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        double totalReward = 0.0;
        size_t maxFiring = 0;
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto reward = percept.getInternal();
            if (!reward.empty()) {
                totalReward += reward[0];
            }
            
            agent->updateDevelopment(0.1);
            
            maxFiring = std::max(maxFiring, brain->getFiringNeuronCount());
        }
        
        return std::make_tuple(totalReward, maxFiring, brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), py::arg("enable_learning") = true,
       "Run a complete agent episode in the world, returning (total_reward, max_firing, avg_firing_rate)");
    
    // Advanced brain analysis functions
    m.def("analyze_brain_network", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> metrics;
        
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        metrics["avg_firing_rate"] = brain->getAverageFiringRate();
        metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        return metrics;
    }, py::arg("brain"), "Analyze brain network properties and return metrics dictionary");
    
    // Enhanced configuration presets
    m.def("create_expert_config", []() {
        auto config = std::make_shared<Config>();
        
        // Expert-level configuration for advanced research
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.15, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Advanced plasticity settings
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 20.0, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 20.0, ConfigSource::Default);
        
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        // Neuromodulation settings
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Development settings
        config->set("development.initial_plasticity", 1.0, ConfigSource::Default);
        config->set("development.critical_period_duration", 1000, ConfigSource::Default);
        config->set("development.maturation_rate", 0.001, ConfigSource::Default);
        
        return config;
    }, "Create an expert-level configuration optimized for advanced research");
    
    m.def("create_learning_config", []() {
        auto config = std::make_shared<Config>();
        
        // Configuration optimized for learning and development
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Strong plasticity for learning
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.002, ConfigSource::Default); // Higher learning rate
        
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", 0.001, ConfigSource::Default);
        
        // Enable all neuromodulation for learning
        config->set("neuromod.dopamine.scale", 2.0, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Development focused
        config->set("development.initial_plasticity", 1.5, ConfigSource::Default);
        config->set("development.maturation_rate", 0.002, ConfigSource::Default);
        
        return config;
    }, "Create a learning-focused configuration optimized for skill acquisition");
    
    // Batch simulation utilities
    m.def("run_batch_simulations", [](std::function<std::shared_ptr<Brain>()> brain_factory,
                                     std::function<void(std::shared_ptr<Brain>)> setup_func,
                                     int num_simulations, int steps_per_simulation) {
        std::vector<std::shared_ptr<Brain>> results;
        
        for (int i = 0; i < num_simulations; ++i) {
            auto brain = brain_factory();
            if (brain) {
                brain->initialize();
                setup_func(brain);
                
                for (int step = 0; step < steps_per_simulation; ++step) {
                    brain->step(step);
                }
                
                results.push_back(brain);
            }
        }
        
        return results;
    }, py::arg("brain_factory"), py::arg("setup_func"), py::arg("num_simulations"), py::arg("steps_per_simulation"),
       "Run multiple simulations with different random seeds, returning list of brains");
    
    // Data export utilities
    m.def("export_brain_stats", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "# NLM Brain Statistics Export" << std::endl;
        file << "# Generated at: " << std::chrono::system_clock::now() << std::endl << std::endl;
        
        file << "Brain Statistics:" << std::endl;
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        file << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        file << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        file << "  Configuration Summary:" << std::endl;
        
        std::string configSummary = brain->getConfig()->summary();
        file << configSummary << std::endl;
        
        return true;
    }, py::arg("brain"), py::arg("filename"), "Export brain statistics to a text file");
    
    // Quick benchmark function
    m.def("benchmark_brain_performance", [](std::shared_ptr<Brain> brain, int steps) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double ms_per_step = (duration.count() * 1000.0) / steps;
        
        return std::make_tuple(
            duration.count() / 1000.0,  // Total seconds
            ms_per_step,               // Milliseconds per step
            brain->getAverageFiringRate(),  // Average firing rate
            brain->getTotalSynapseCount()    // Total synapses
        );
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance, returning (total_seconds, ms_per_step, avg_firing_rate, total_synapses)");
    
    // System information
    m.def("get_nlm_version", []() {
        return "NLM Python Bindings v0.1.0";
    }, "Get NLM Python bindings version");
    
    m.def("get_system_info", []() {
        std::map<std::string, std::string> info;
        
        // Python version info
        info["python_version"] = PYBIND11_VERSION;
        info["python_bindings_version"] = "0.1.0";
        
        // Build information
        info["build_type"] = "Release";
        info["compiler"] = "C++20";
        
        // Memory and system info (simplified)
        info["memory_mode"] = "Managed (shared_ptr)";
        info["threading"] = "Single-threaded (for now)";
        
        return info;
    }, "Get NLM system information");
    
    // Validation and testing utilities
    m.def("validate_brain_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        
        if (!config) {
            errors.push_back("Config is null");
            return std::make_tuple(errors, warnings);
        }
        
        // Check basic parameters
        auto neuronCount = config->get<int64_t>("neuron_count");
        if (!neuronCount) {
            errors.push_back("Missing or invalid 'neuron_count' parameter");
        } else if (*neuronCount < 100) {
            warnings.push_back("Small neuron count (< 100) may not produce interesting dynamics");
        } else if (*neuronCount > 10000) {
            warnings.push_back("Large neuron count (> 10000) may be computationally expensive");
        }
        
        auto connectionProb = config->get<double>("connection_probability");
        if (!connectionProb) {
            warnings.push_back("Missing 'connection_probability' parameter");
        } else if (*connectionProb < 0.01 || *connectionProb > 0.5) {
            warnings.push_back("Unusual connection probability may affect network properties");
        }
        
        auto timestep = config->get<double>("simulation_timestep");
        if (!timestep) {
            errors.push_back("Missing 'simulation_timestep' parameter");
        } else if (*timestep <= 0.0 || *timestep > 1.0) {
            errors.push_back("Invalid simulation timestep, must be > 0 and <= 1.0");
        }
        
        return std::make_tuple(errors, warnings);
    }, py::arg("config"), "Validate brain configuration, returning (errors, warnings) lists");
    
    // Convenience factory methods for common configurations
    m.def("create_research_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a research-oriented configuration");
    
    m.def("create_artificial_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
        config->set("simulation_timestep", 0.01, ConfigSource::Default);  // Larger timestep for faster simulation
        return config;
    }, "Create an artificial/ toy configuration for quick testing");
    
    m.def("create_scalable_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(0), ConfigSource::Default);  // Deterministic
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a scalable configuration for performance testing");
    
    // Event-driven simulation utilities
    m.def("run_steps_until", [](std::shared_ptr<Brain> brain, 
                              std::function<bool(std::shared_ptr<Brain>)> condition,
                              int max_steps = 10000) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        int steps_run = 0;
        for (int step = 0; step < max_steps; ++step) {
            brain->step(step);
            steps_run++;
            
            if (condition(brain)) {
                break;
            }
        }
        
        return steps_run;
    }, py::arg("brain"), py::arg("condition"), py::arg("max_steps") = 10000,
       "Run simulation steps until a condition is met, returning number of steps taken");
    
    // Configuration comparison
    m.def("compare_configs", [](std::shared_ptr<Config> config1, std::shared_ptr<Config> config2) {
        std::map<std::string, std::string> differences;
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        for (const auto& key : keys1) {
            auto val1 = config1->get<std::string>(key);
            auto val2 = config2->get<std::string>(key);
            
            if (!val2 || *val1 != *val2) {
                differences[key + " (config1)"] = *val1 ? *val1 : "[missing]";
                differences[key + " (config2)"] = val2 ? *val2 : "[missing]";
            }
        }
        
        for (const auto& key : keys2) {
            if (!config1->has(key)) {
                differences[key + " (config2 only)"] = "[present]";
                differences[key + " (config1 missing)"] = "[missing]";
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"), "Compare two configurations, returning differences dictionary");
    
    // Memory snapshot and restoration
    m.def("create_brain_snapshot", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing snapshot: " + filename);
        }
        
        // Simple text-based snapshot for now
        file << "=== NLM Brain Snapshot ===" << std::endl;
        file << "Timestamp: " << std::chrono::system_clock::now() << std::endl;
        file << "Neuron Count: " << brain->getTotalNeuronCount() << std::endl;
        file << "Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
        file << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "Avg Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        
        return true;
    }, py::arg("brain"), py::arg("filename"), "Create a text-based snapshot of brain state");
    
    // Utility functions for working with neuron populations
    m.def("create_neuron_population", [](std::shared_ptr<Brain> brain, NeuronType type, 
                                        double min_firing_threshold, double max_firing_threshold) {
        auto regionIds = brain->getRegionIds();
        if (regionIds.empty()) {
            throw std::runtime_error("No regions found in brain");
        }
        
        auto* region = brain->getRegion(regionIds[0]);
        if (!region) {
            throw std::runtime_error("Could not access region");
        }
        
        auto neurons = region->getAllNeurons();
        std::vector<NeuronId> selectedNeurons;
        
        for (auto* neuron : neurons) {
            if (neuron && neuron->getType() == type && 
                neuron->getAverageFiringRate() >= min_firing_threshold &&
                neuron->getAverageFiringRate() <= max_firing_threshold) {
                selectedNeurons.push_back(neuron->getId());
            }
        }
        
        return selectedNeurons;
    }, py::arg("brain"), py::arg("type"), py::arg("min_firing_threshold"), py::arg("max_firing_threshold"),
       "Find neurons of a specific type within a firing rate range, returning list of NeuronIds");
    
    // Statistical analysis functions
    m.def("compute_neuron_statistics", [](std::shared_ptr<Brain> brain, NeuronType type) {
        std::map<std::string, double> stats;
        
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        auto regionIds = brain->getRegionIds();
        if (regionIds.empty()) {
            throw std::runtime_error("No regions found in brain");
        }
        
        auto* region = brain->getRegion(regionIds[0]);
        if (!region) {
            throw std::runtime_error("Could not access region");
        }
        
        auto neurons = region->getAllNeurons();
        std::vector<double> firingRates;
        std::vector<double> membranePotentials;
        
        for (auto* neuron : neurons) {
            if (neuron && neuron->getType() == type) {
                firingRates.push_back(neuron->getAverageFiringRate());
                membranePotentials.push_back(neuron->getMembranePotential());
            }
        }
        
        if (firingRates.empty()) {
            return stats; // Empty stats
        }
        
        // Compute statistics
        double sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0);
        stats["count"] = static_cast<double>(firingRates.size());
        stats["mean"] = sum / firingRates.size();
        
        double sq_sum = std::inner_product(firingRates.begin(), firingRates.end(), firingRates.begin(), 0.0);
        stats["stddev"] = std::sqrt(sq_sum / firingRates.size() - stats["mean"] * stats["mean"]);
        
        double min_val = *std::min_element(firingRates.begin(), firingRates.end());
        double max_val = *std::max_element(firingRates.begin(), firingRates.end());
        stats["min"] = min_val;
        stats["max"] = max_val;
        stats["range"] = max_val - min_val;
        
        // Membrane potential statistics
        double mem_sum = std::accumulate(membranePotentials.begin(), membranePotentials.end(), 0.0);
        stats["mem_mean"] = mem_sum / membranePotentials.size();
        
        double mem_sq_sum = std::inner_product(membranePotentials.begin(), membranePotentials.end(), membranePotentials.begin(), 0.0);
        stats["mem_stddev"] = std::sqrt(mem_sq_sum / membranePotentials.size() - stats["mem_mean"] * stats["mem_mean"]);
        
        return stats;
    }, py::arg("brain"), py::arg("type"), "Compute statistics for neurons of a specific type, returning dictionary");
    
    // Export configuration to Python dict
    m.def("config_to_dict", [](std::shared_ptr<Config> config) {
        py::dict result;
        
        if (!config) {
            return result; // Empty dict
        }
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            result[key] = config->get<std::string>(key).value_or("[unreadable]");
        }
        
        return result;
    }, py::arg("config"), "Convert configuration to Python dictionary");
    
    // Import configuration from Python dict
    m.def("dict_to_config", [](py::dict config_dict) {
        auto config = std::make_shared<Config>();
        
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            std::string value = py::cast<std::string>(item.second);
            config->set(key, value, ConfigSource::Python);
        }
        
        return config;
    }, py::arg("config_dict"), "Create configuration from Python dictionary");
    
    // Diagnostic and debugging functions
    m.def("debug_brain_state", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::string> debug_info;
        
        if (!brain) {
            debug_info["error"] = "Brain is null";
            return debug_info;
        }
        
        debug_info["brain_initialized"] = brain->initialize() ? "true" : "false";
        debug_info["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
        debug_info["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
        debug_info["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
        debug_info["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        debug_info["avg_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        debug_info["e_i_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
        debug_info["developmental_stage"] = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
        
        auto regionIds = brain->getRegionIds();
        debug_info["region_count"] = std::to_string(regionIds.size());
        
        for (size_t i = 0; i < regionIds.size() && i < 3; ++i) { // Show first 3 regions
            auto* region = brain->getRegion(regionIds[i]);
            if (region) {
                debug_info["region_" + std::to_string(i) + "_neurons"] = std::to_string(region->getNeuronCount());
                debug_info["region_" + std::to_string(i) + "_synapses"] = std::to_string(region->getSynapseCount());
            }
        }
        
        return debug_info;
    }, py::arg("brain"), "Get detailed debug information about brain state, returning dictionary");
    
    // Quick test functions
    m.def("quick_test_brain", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        brain->initialize();
        
        for (int step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        return std::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getFiringNeuronCount(),
            brain->getTotalSpikeCount()
        );
    }, py::arg("brain"), "Run a quick test of the brain (10 steps), returning (neurons, synapses, firing, total_spikes)");

} // namespace nlm
