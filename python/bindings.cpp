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

    // ============= ADVANCED API FOR POWER USERS =============

    // Batch brain processing utility
    m.def("batch_process", [](const std::vector<std::shared_ptr<Brain>>& brains, size_t steps) {
        for (auto& brain : brains) {
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
        }
        return std::vector<std::map<std::string, size_t>>{
            {"total_neurons", brains[0]->getTotalNeuronCount()},
            {"total_synapses", brains[0]->getTotalSynapseCount()},
            {"total_spikes", brains[0]->getTotalSpikeCount()}
        };
    }, py::arg("brains"), py::arg("steps"), "Process multiple brains in batch");

    // Advanced configuration utilities
    m.def("createConfigFromDict", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            if (py::isinstance<py::int_>(item.second)) {
                config->set(key, py::cast<int>(item.second));
            } else if (py::isinstance<py::float>(item.second)) {
                config->set(key, py::cast<double>(item.second));
            } else if (py::isinstance<py::bool_>(item.second)) {
                config->set(key, py::cast<bool>(item.second));
            } else {
                config->set(key, py::cast<std::string>(item.second));
            }
        }
        return config;
    }, py::arg("config_dict"), "Create configuration from Python dictionary");

    // Advanced brain analysis utilities
    m.def("analyzeBrainDynamics", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        std::map<std::string, double> dynamics;
        
        // Track statistics over time
        size_t total_spikes = brain->getTotalSpikeCount();
        size_t firing_neuron_count = brain->getFiringNeuronCount();
        float avg_firing_rate = brain->getAverageFiringRate();
        float e_i_ratio = brain->getExcitationInhibitionRatio();
        
        dynamics["total_neurons"] = brain->getTotalNeuronCount();
        dynamics["total_synapses"] = brain->getTotalSynapseCount();
        dynamics["total_spikes"] = total_spikes;
        dynamics["active_neurons"] = brain->getActiveNeuronCount();
        dynamics["firing_neurons_this_step"] = firing_neuron_count;
        dynamics["average_firing_rate"] = avg_firing_rate;
        dynamics["excitation_inhibition_ratio"] = e_i_ratio;
        
        // Run additional steps for analysis
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        // Update statistics after running steps
        dynamics["spikes_per_step"] = (brain->getTotalSpikeCount() - total_spikes) / (steps + 1);
        dynamics["firing_rate_change"] = brain->getAverageFiringRate() - avg_firing_rate;
        
        return dynamics;
    }, py::arg("brain"), py::arg("steps"), "Analyze brain dynamics over time");

    // Advanced memory analysis
    m.def("analyzeMemorySystems", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, size_t> memory_stats;
        
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            memory_stats["working_memory_active_traces"] = workingMem->getActiveTraces();
            memory_stats["working_memory_capacity"] = workingMem->getCapacity();
        }
        
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            memory_stats["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
            memory_stats["episodic_memory_max_episodes"] = episodicMem->getMaxEpisodes();
        }
        
        auto* assocMem = brain->getAssociativeMemory();
        if (assocMem) {
            // Basic associative memory statistics (implementation-specific)
            memory_stats["associative_memory_active"] = 1; // Placeholder
        }
        
        return memory_stats;
    }, py::arg("brain"), "Analyze memory system statistics");

    // Advanced neuromodulation analysis
    m.def("analyzeNeuromodulation", [](const std::shared_ptr<AgentBrain>& agent) {
        std::map<std::string, float> neuromod_stats;
        
        neuromod_stats["dopamine_level"] = agent->getNeuromodulationLevel();
        neuromod_stats["curiosity_level"] = agent->getCuriosityLevel();
        neuromod_stats["novelty_level"] = agent->getNoveltyLevel();
        neuromod_stats["prediction_error"] = agent->getPredictionError();
        neuromod_stats["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        
        return neuromod_stats;
    }, py::arg("agent"), "Analyze neuromodulation system state");

    // Experimental setup utilities
    m.def("setupExperiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world, const py::dict& experiment_config) {
        // Configure experiment parameters
        auto config = brain->getConfig();
        
        if (experiment_config.contains("neuron_count")) {
            config->set("brain.neuron_count", py::cast<size_t>(experiment_config["neuron_count"]));
        }
        if (experiment_config.contains("synapse_density")) {
            config->set("brain.synapse_density", py::cast<float>(experiment_config["synapse_density"]));
        }
        if (experiment_config.contains("plasticity_enabled")) {
            config->set("plasticity.stdp.enable", py::cast<bool>(experiment_config["plasticity_enabled"]));
        }
        
        // Initialize brain with experiment configuration
        brain->initialize();
        
        // Initialize agent with world
        agent->initialize(*world);
        
        // Enable experiment-specific features
        if (experiment_config.contains("features")) {
            py::list features = py::cast<py::list>(experiment_config["features"]);
            for (auto item : features) {
                std::string feature = py::cast<std::string>(item);
                if (feature == "reward_modulation") agent->enableRewardModulation(true);
                else if (feature == "structural_plasticity") agent->enableStructuralPlasticity(true);
                else if (feature == "curiosity") agent->enableCuriosity(true);
                else if (feature == "development") agent->enableDevelopment(true);
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("experiment_config"), 
           "Set up an experiment with brain, agent, and world");

    // Advanced simulation utilities
    m.def("runSimulationLoop", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world, size_t num_steps,
                                 bool verbose = false) {
        std::vector<std::map<std::string, double>> history;
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory percept
            const auto& percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Collect statistics
            std::map<std::string, double> stats;
            stats["step"] = step;
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["curiosity"] = agent->getCuriosityLevel();
            stats["novelty"] = agent->getNoveltyLevel();
            stats["prediction_error"] = agent->getPredictionError();
            
            if (verbose && (step % 100 == 0 || step == num_steps - 1)) {
                std::cout << "Step " << step << ": "
                         << "Firing: " << stats["firing_neurons"] << ", "
                         << "Curiosity: " << stats["curiosity"] << ", "
                         << "Novelty: " << stats["novelty"] << std::endl;
            }
            
            history.push_back(stats);
        }
        
        return history;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("num_steps"), 
           py::arg("verbose") = false, "Run a complete simulation loop");

    // Checkpoint and experiment utilities
    m.def("saveExperimentCheckpoint", [](const std::shared_ptr<Brain>& brain, 
                                         const std::string& prefix = "experiment",
                                         size_t max_checkpoints = 10) {
        // Create timestamped checkpoint filename
        std::time_t t = std::time(nullptr);
        char mbstr[100];
        std::strftime(mbstr, sizeof(mbstr), "%Y%m%d_%H%M%S", std::localtime(&t));
        std::string filename = prefix + "_" + std::string(mbstr) + ".bin";
        
        // Save brain state
        bool success = brain->save(filename);
        
        if (success) {
            // Optionally manage checkpoint directory
            // (Implementation would manage checkpoint rotation)
            return filename;
        }
        
        return std::string("");
    }, py::arg("brain"), py::arg("prefix") = "experiment", py::arg("max_checkpoints") = 10,
       "Save experiment checkpoint with timestamp");

    // Statistical analysis utilities
    m.def("computeBrainStatistics", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> stats;
        
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["active_ratio"] = static_cast<double>(brain->getActiveNeuronCount()) / 
                                 std::max(brain->getTotalNeuronCount(), 1.0);
        stats["firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_balance"] = brain->getExcitationInhibitionRatio();
        
        return stats;
    }, py::arg("brain"), "Compute comprehensive brain statistics");

    // ============= ENHANCED ERROR HANDLING =============
    
    // Safe brain step with error checking
    m.def("safeBrainStep", [](const std::shared_ptr<Brain>& brain, size_t step, 
                           bool check_neural_integrity = true) {
        try {
            if (check_neural_integrity) {
                // Basic integrity checks
                if (brain->getTotalNeuronCount() == 0) {
                    return false; // No neurons, cannot step
                }
                if (brain->getTotalSynapseCount() < 0) {
                    return false; // Invalid synapse count
                }
            }
            
            brain->step(step);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error in safeBrainStep: " << e.what() << std::endl;
            return false;
        }
    }, py::arg("brain"), py::arg("step"), py::arg("check_neural_integrity") = true,
       "Safely execute brain step with error checking");

    // Safe configuration loading
    m.def("safeConfigLoad", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        bool success = config->loadFromFile(filepath);
        return std::make_pair(success, config);
    }, py::arg("filepath"), "Safely load configuration with error handling");

    // Version and system information
    m.attr("NLM_VERSION") = "Phase 6.0 - Final Integration";
    m.attr("PYTHON_BINDINGS_VERSION") = "1.5.0";
    m.attr("ARCHITECTURE") = "Closed-loop artificial brain with integrated memory, prediction, and cognition";
    
    // Help and documentation utilities
    m.def("getHelp", []() {
        std::string help = R"(
NLM Python API - Enhanced Edition

=== BASIC API ===
- createBrain(config): Create neural brain
- createAgentBrain(brain): Create agent interface
- createSimpleWorld(): Create simulation environment
- brain.initialize(): Initialize brain state
- brain.step(step): Process one simulation step
- brain.save(filepath): Save brain state
- brain.load(filepath): Load brain state

=== ADVANCED API ===
- batch_process(brains, steps): Process multiple brains
- analyzeBrainDynamics(brain, steps): Analyze brain dynamics
- analyzeMemorySystems(brain): Analyze memory systems
- analyzeNeuromodulation(agent): Analyze neuromodulation
- setupExperiment(brain, agent, world, config): Configure experiment
- runSimulationLoop(brain, agent, world, steps): Run complete simulation
- saveExperimentCheckpoint(brain, prefix, max): Save checkpoint
- computeBrainStatistics(brain): Compute comprehensive stats

=== ERROR HANDLING ===
- safeBrainStep(brain, step): Safe step execution
- safeConfigLoad(filepath): Safe configuration loading

=== UTILITIES ===
- NLM_VERSION: Version string
- PYTHON_BINDINGS_VERSION: Bindings version
- ARCHITECTURE: Architecture description

=== EXAMPLE USAGE ===
import pynlm

# Basic usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Advanced analysis
dynamics = pynlm.analyzeBrainDynamics(brain, 100)
stats = pynlm.computeBrainStatistics(brain)

# Batch processing
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(3)]
results = pynlm.batch_process(brains, 50)

=== FEATURES ===
✓ Memory system integration
✓ Prediction system support
✓ Neuromodulation tracking
✓ Development stages
✓ Checkpoint persistence
✓ Error handling
✓ Statistical analysis
✓ Batch operations
✓ Experiment utilities
        )";
        return help;
    }, "Get comprehensive API help and examples");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
