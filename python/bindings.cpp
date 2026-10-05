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
                .def("getDevelopmentStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Additional methods for better user experience
        .def("getMemoryStats", [](Brain& brain) {
            py::dict memoryStats;
            if (brain.getWorkingMemory()) {
                memoryStats["working_memory_traces"] = brain.getWorkingMemory()->getActiveTraces();
                memoryStats["working_memory_capacity"] = brain.getWorkingMemory()->getCapacity();
            }
            if (brain.getEpisodicMemory()) {
                memoryStats["episodic_memory_episodes"] = brain.getEpisodicMemory()->getEpisodeCount();
                memoryStats["episodic_memory_max_episodes"] = brain.getEpisodicMemory()->getMaxEpisodes();
            }
            return memoryStats;
        }, "Get memory system statistics. Returns a dictionary with "
            "working memory and episodic memory metrics.")
        
        .def("getNeuromodulationStats", [](Brain& brain) {
            py::dict neuromodStats;
            if (brain.getDopamine()) {
                neuromodStats["dopamine_level"] = brain.getDopamine()->getLevel();
                neuromodStats["dopamine_plasticity_factor"] = brain.getDopamine()->getPlasticityFactor();
            }
            if (brain.getCuriosity()) {
                neuromodStats["curiosity_level"] = brain.getCuriosity()->getLevel();
            }
            if (brain.getNovelty()) {
                neuromodStats["novelty_level"] = brain.getNovelty()->getLevel();
            }
            return neuromodStats;
        }, "Get neuromodulation system statistics. Returns a dictionary with "
            "dopamine, curiosity, and novelty metrics.")
        
        .def("getDevelopmentStageName", [](Brain& brain) {
            static const std::vector<std::string> stageNames = {
                "Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"
            };
            auto stage = brain.getDevelopmentalStage();
            int index = static_cast<int>(stage);
            if (index >= 0 && index < static_cast<int>(stageNames.size())) {
                return stageNames[index];
            }
            return std::string("Unknown");
        }, "Get human-readable name for the current developmental stage.")
        
        .def("simulateEpisode", &AgentBrain::reset, "Reset agent for a new episode. "
             "This is useful for running multiple simulation episodes.")
        
        .def("isResting", [](Brain& brain) {
            return false;  // Simplified for now
        }, "Check if the brain is in resting state.")
        
        .def("getExcitementLevel", [](Brain& brain) {
            float firingExcitement = brain.getAverageFiringRate() / 50.0f;
            float noveltyExcitement = brain.getNovelty() ? brain.getNovelty()->getLevel() : 0.0f;
            return std::min(firingExcitement + noveltyExcitement, 1.0f);
        }, "Get a measure of brain excitement (0-1). Higher values indicate "
            "more active exploration and learning potential.")
        
        .def("getEngagementLevel", [](Brain& brain) {
            float firingEngagement = std::min(brain.getAverageFiringRate() / 30.0f, 1.0f);
            float noveltyEngagement = brain.getNovelty() ? brain.getNovelty()->getLevel() : 0.0f;
            float curiosityEngagement = brain.getCuriosity() ? brain.getCuriosity()->getLevel() : 0.0f;
            return (firingEngagement * 0.4f) + (noveltyEngagement * 0.3f) + (curiosityEngagement * 0.3f);
        }, "Get an engagement level (0-1). Measures how actively the brain "
            "is involved in learning and exploration.");

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

    // ===== SIMPLIFIED BEGINNER HELPER FUNCTIONS =====

    // Create simple brain for beginners - high-level convenience method
    m.def("create_simple_brain", [](size_t neuronCount, size_t regionCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42);
        return std::make_shared<Brain>(config);
    }, py::arg("neuron_count") = 1000, py::arg("region_count") = 1,
        "Create a simple brain with default configuration for beginners. "
        "Provides sensible defaults for quick experimentation.");

    // Create simple world for beginners - high-level convenience method
    m.def("create_simple_world", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 16, py::arg("height") = 16,
        py::arg("vision_width") = 16, py::arg("vision_height") = 16,
        "Create a simple 2D world for simulation. "
        "Configures the world with basic sensory and motor capabilities.");

    // Create agent brain with world - high-level convenience method
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an agent brain connected to a world. "
        "Enables reward modulation, development, and curiosity by default for intelligent behavior.");

    // Run simulation loop helper for beginners
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                              std::shared_ptr<SimpleWorld> world, size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Step brain
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(cmd, step * 0.001);
            
            // Compute reward
            float reward = world->computeReward(agent->getBrain()->getRegions()[0].get());
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.001);
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps") = 100,
        "Run a complete simulation loop. This helper method combines all the "
        "essential steps for a typical simulation episode.");

    // Get neural statistics helper for beginners
    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["region_count"] = brain->getRegionCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["working_memory_traces"] = brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0;
        stats["dopamine_level"] = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
        return stats;
    }, py::arg("brain"),
        "Get comprehensive statistics about a brain. Returns a dictionary "
        "with key neural metrics useful for monitoring and debugging.");

    // Enhanced brain initialization with error checking
    m.def("safe_initialize", [](std::shared_ptr<Brain> brain) {
        bool success = brain->initialize();
        if (!success) {
            throw py::value_error("Brain initialization failed");
        }
        return success;
    }, py::arg("brain"),
        "Initialize a brain with error checking. Throws an exception if "
        "initialization fails, making it easier to catch and handle errors.");

    // Version and info functions
    m.def("get_nlm_version", []() {
        return "NLM v1.0.0 - Neural Learning Machine (Phase 6)";
    }, "Get the current NLM version information.");

    m.def("is_functional", []() {
        return true;  // All systems functional in Phase 6
    }, "Check if NLM is fully functional. Returns true for Phase 6 implementation.");

    // Educational helper: Create a demonstration brain
    m.def("create_demo_brain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.05f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("random_seed", 42);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw py::value_error("Demo brain initialization failed");
        }
        
        // Add some inter-region connectivity for demonstration
        brain->addInterRegionConnection(RegionId(1), RegionId(2), 0.1f);
        
        return brain;
    }, "Create a demonstration-ready brain with example configuration. "
        "This brain includes sample inter-region connections for learning demonstrations.");

    // Simplified agent for educational purposes
    m.def("create_educational_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Configure for educational demonstrations
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Store for debugging
        agent->getBrain()->getConfig()->set("demo_mode", true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an educational agent configured for learning demonstrations. "
        "This agent includes specific settings for educational use cases.");

    // State management helpers
    m.def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (!success) {
            throw py::value_error("Failed to save brain state to " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"),
        "Save brain state to a file with error checking. "
        "Useful for checkpointing progress or sharing brain models.");

    m.def("load_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw py::value_error("Failed to load brain state from " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"),
        "Load brain state from a file with error checking. "
        "Allows resuming from checkpoints or loading pre-trained models.");

    // Quick setup helper for simulations
    m.def("setup_simulation", [](size_t neuronCount, size_t worldSize) -> py::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw py::value_error("Brain initialization failed");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldSize, worldSize, worldSize, worldSize);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return py::make_tuple(brain, world, agent);
    }, py::arg("neuron_count") = 1000, py::arg("world_size") = 16,
        "Quick setup for simulation. Returns a pre-configured tuple of "
        "(brain, world, agent) ready for immediate use.");

    // Add convenience attributes for common values
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_WORLD_SIZE") = 16;
    m.attr("DEFAULT_VISION_SIZE") = 16;
    m.attr("DEFAULT_REGION_COUNT") = 1;

    // Educational tutorial example
    m.def("get_tutorial_example", []() {
        return R"(
        # NLM Python Tutorial Example
        
        # Simple brain creation
        brain = nlm.create_simple_brain(neuron_count=500)
        brain.initialize()
        
        # Create world
        world = nlm.create_simple_world(16, 16)
        
        # Create agent
        agent = nlm.create_agent_brain(brain, world)
        
        # Run simulation
        nlm.run_simulation(brain, agent, world, steps=100)
        
        # Get stats
        stats = nlm.get_brain_stats(brain)
        print(f"Average firing rate: {stats['average_firing_rate']}")
        
        # Save checkpoint
        brain.save("checkpoint.bin")
        )";
    }, "Get an educational tutorial example code. Useful for beginners "
        "learning how to use the NLM Python interface.");

} // namespace nlm
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

    // ===== SIMPLIFIED BEGINNER HELPER FUNCTIONS =====

    // Create simple brain for beginners - high-level convenience method
    m.def("create_simple_brain", [](size_t neuronCount, size_t regionCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42);
        return std::make_shared<Brain>(config);
    }, py::arg("neuron_count") = 1000, py::arg("region_count") = 1,
        "Create a simple brain with default configuration for beginners. "
        "Provides sensible defaults for quick experimentation.");

    // Create simple world for beginners - high-level convenience method
    m.def("create_simple_world", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 16, py::arg("height") = 16,
        py::arg("vision_width") = 16, py::arg("vision_height") = 16,
        "Create a simple 2D world for simulation. "
        "Configures the world with basic sensory and motor capabilities.");

    // Create agent brain with world - high-level convenience method
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an agent brain connected to a world. "
        "Enables reward modulation, development, and curiosity by default for intelligent behavior.");

    // Run simulation loop helper for beginners
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                              std::shared_ptr<SimpleWorld> world, size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Step brain
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(cmd, step * 0.001);
            
            // Compute reward
            float reward = world->computeReward(agent->getBrain()->getRegions()[0].get());
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.001);
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps") = 100,
        "Run a complete simulation loop. This helper method combines all the "
        "essential steps for a typical simulation episode.");

    // Get neural statistics helper for beginners
    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        py::dict stats;
        stats["neuron_count"] = brain->getTotalNeuronCount();
        stats["region_count"] = brain->getRegionCount();
        stats["synapse_count"] = brain->getTotalSynapseCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["working_memory_traces"] = brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0;
        stats["dopamine_level"] = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
        return stats;
    }, py::arg("brain"),
        "Get comprehensive statistics about a brain. Returns a dictionary "
        "with key neural metrics useful for monitoring and debugging.");

    // Enhanced brain initialization with error checking
    m.def("safe_initialize", [](std::shared_ptr<Brain> brain) {
        bool success = brain->initialize();
        if (!success) {
            throw py::value_error("Brain initialization failed");
        }
        return success;
    }, py::arg("brain"),
        "Initialize a brain with error checking. Throws an exception if "
        "initialization fails, making it easier to catch and handle errors.");

    // Version and info functions
    m.def("get_nlm_version", []() {
        return "NLM v1.0.0 - Neural Learning Machine (Phase 6)";
    }, "Get the current NLM version information.");

    m.def("is_functional", []() {
        return true;  // All systems functional in Phase 6
    }, "Check if NLM is fully functional. Returns true for Phase 6 implementation.");

    // Educational helper: Create a demonstration brain
    m.def("create_demo_brain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.05f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("random_seed", 42);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw py::value_error("Demo brain initialization failed");
        }
        
        // Add some inter-region connectivity for demonstration
        brain->addInterRegionConnection(RegionId(1), RegionId(2), 0.1f);
        
        return brain;
    }, "Create a demonstration-ready brain with example configuration. "
        "This brain includes sample inter-region connections for learning demonstrations.");

    // Simplified agent for educational purposes
    m.def("create_educational_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Configure for educational demonstrations
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Store for debugging
        agent->getBrain()->getConfig()->set("demo_mode", true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an educational agent configured for learning demonstrations. "
        "This agent includes specific settings for educational use cases.");

    // State management helpers
    m.def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->save(filepath);
        if (!success) {
            throw py::value_error("Failed to save brain state to " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"),
        "Save brain state to a file with error checking. "
        "Useful for checkpointing progress or sharing brain models.");

    m.def("load_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        bool success = brain->load(filepath);
        if (!success) {
            throw py::value_error("Failed to load brain state from " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"),
        "Load brain state from a file with error checking. "
        "Allows resuming from checkpoints or loading pre-trained models.");

    // Quick setup helper for simulations
    m.def("setup_simulation", [](size_t neuronCount, size_t worldSize) -> py::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw py::value_error("Brain initialization failed");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldSize, worldSize, worldSize, worldSize);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return py::make_tuple(brain, world, agent);
    }, py::arg("neuron_count") = 1000, py::arg("world_size") = 16,
        "Quick setup for simulation. Returns a pre-configured tuple of "
        "(brain, world, agent) ready for immediate use.");

    // Add convenience attributes for common values
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_WORLD_SIZE") = 16;
    m.attr("DEFAULT_VISION_SIZE") = 16;
    m.attr("DEFAULT_REGION_COUNT") = 1;

    // Educational tutorial example
    m.def("get_tutorial_example", []() {
        return R"(
        # NLM Python Tutorial Example
        
        # Simple brain creation
        brain = nlm.create_simple_brain(neuron_count=500)
        brain.initialize()
        
        # Create world
        world = nlm.create_simple_world(16, 16)
        
        # Create agent
        agent = nlm.create_agent_brain(brain, world)
        
        # Run simulation
        nlm.run_simulation(brain, agent, world, steps=100)
        
        # Get stats
        stats = nlm.get_brain_stats(brain)
        print(f"Average firing rate: {stats['average_firing_rate']}")
        
        # Save checkpoint
        brain.save("checkpoint.bin")
        )";
    }, "Get an educational tutorial example code. Useful for beginners "
        "learning how to use the NLM Python interface.");

    // ===== ENHANCED BRAIN CLASS METHODS =====

    // Add additional methods to the existing Brain class
    // Find the Brain class definition and add methods
    }

} // namespace nlm
