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

// Template to reduce duplication in ID class definitions
template<typename IDType, typename ValueType>
void bindIDClass(py::module_& m, const std::string& name,
                 const std::string& doc) {
    py::class_<IDType>(m, name.c_str(), py::doc(doc.c_str()))
        .def(py::init<>())
        .def(py::init<ValueType>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { 
            return std::hash<ValueType>{}(id.value); 
        })
        .def("__repr__", [](const IDType& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

// Template for enum binding
template<typename EnumType>
void bindEnumClass(py::module_& m, const std::string& name,
                   const std::string& doc) {
    py::enum_<EnumType>(m, name.c_str(), py::doc(doc.c_str()))
        .export_values();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Advanced Features:
        - Complete Phase 6 integration support
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and cognitive systems
        - Development and plasticity control
        - Agent brain interface for world interaction
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind ID classes using template to reduce duplication
    bindIDClass<NeuronId, uint64_t>(m, "NeuronId", 
                                  "Unique identifier for a neuron");
    bindIDClass<SynapseId, uint64_t>(m, "SynapseId", 
                                    "Unique identifier for a synapse");
    bindIDClass<RegionId, uint64_t>(m, "RegionId", 
                                   "Unique identifier for a brain region");
    bindIDClass<PopulationId, uint64_t>(m, "PopulationId", 
                                       "Unique identifier for a neuron population");

    // Bind enum classes
    bindEnumClass<NeuronType>(m, "NeuronType", "Neuron type enumeration");
    bindEnumClass<SynapseType>(m, "SynapseType", "Synapse type enumeration");
    bindEnumClass<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration");
    bindEnumClass<FiringState>(m, "FiringState", "Neuron firing state enumeration");
    bindEnumClass<ActionType>(m, "ActionType", "Action type enumeration");
    bindEnumClass<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration");
    bindEnumClass<WorldObjectType>(m, "WorldObjectType", "World object type enumeration");

    // Core configuration class
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

    // Sensory input base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), 
             py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), 
             py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Internal signals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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

    // World object class
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

    // Agent body class
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

    // Action result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory percept class
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

    // Simple world class
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

    // Brain class - comprehensive neural simulation
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

    // AgentBrain class - advanced interface for world interaction
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

    // Factory functions for easy creation
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Phase 6 integration helpers
    m.def("createPhase6Config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        return config;
    }, "Create a Phase 6 optimized configuration");

    m.def("runPhase6Demo", []() {
        // This would integrate with the existing Phase6Demo.cpp
        // For now, it's a placeholder that can be expanded
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return "Phase 6 demo initialized successfully";
        }
        return "Failed to initialize Phase 6 demo";
    }, "Run Phase 6 integration demo");

    // Memory system access helpers
    m.def("getWorkingMemory", [](std::shared_ptr<Brain> brain) {
        auto* wm = brain->getWorkingMemory();
        if (!wm) throw std::runtime_error("Working memory not available");
        return wm;
    }, py::arg("brain"), "Get working memory system");

    m.def("getEpisodicMemory", [](std::shared_ptr<Brain> brain) {
        auto* em = brain->getEpisodicMemory();
        if (!em) throw std::runtime_error("Episodic memory not available");
        return em;
    }, py::arg("brain"), "Get episodic memory system");

    m.def("getPredictionSystem", [](std::shared_ptr<Brain> brain) {
        auto* ps = brain->getPredictionSystem();
        if (!ps) throw std::runtime_error("Prediction system not available");
        return ps;
    }, py::arg("brain"), "Get prediction system");

    m.def("getDevelopmentSystem", [](std::shared_ptr<Brain> brain) {
        auto* ds = brain->getDevelopmentSystem();
        if (!ds) throw std::runtime_error("Development system not available");
        return ds;
    }, py::arg("brain"), "Get development system");

    m.def("getNeuromodulator", [](std::shared_ptr<Brain> brain, const std::string& type) {
        if (type == "dopamine") {
            auto* mod = brain->getDopamine();
            if (!mod) throw std::runtime_error("Dopamine system not available");
            return mod;
        } else if (type == "curiosity") {
            auto* mod = brain->getCuriosity();
            if (!mod) throw std::runtime_error("Curiosity system not available");
            return mod;
        } else if (type == "novelty") {
            auto* mod = brain->getNovelty();
            if (!mod) throw std::runtime_error("Novelty system not available");
            return mod;
        }
        throw std::runtime_error("Unknown neuromodulator type: " + type);
    }, py::arg("brain"), py::arg("type"), "Get neuromodulator system");

    // Cognitive system access helpers
    m.def("getNeuralPlanner", [](std::shared_ptr<Brain> brain) {
        auto* planner = brain->getPlanner();
        if (!planner) throw std::runtime_error("Neural planner not available");
        return planner;
    }, py::arg("brain"), "Get neural planner");

    m.def("getConceptFormation", [](std::shared_ptr<Brain> brain) {
        auto* cf = brain->getConceptFormation();
        if (!cf) throw std::runtime_error("Concept formation not available");
        return cf;
    }, py::arg("brain"), "Get concept formation system");

    m.def("getAttentionalSelection", [](std::shared_ptr<Brain> brain) {
        auto* attention = brain->getAttention();
        if (!attention) throw std::runtime_error("Attentional selection not available");
        return attention;
    }, py::arg("brain"), "Get attentional selection system");

    // Development helpers
    m.def("enableAllPlasticity", [](std::shared_ptr<Brain> brain) {
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
    }, py::arg("brain"), "Enable all plasticity rules (Hebbian, STDP, reward-modulated)");

    m.def("setDevelopmentStage", [](std::shared_ptr<Brain> brain, DevelopmentalStage stage) {
        brain->setDevelopmentalStage(stage);
    }, py::arg("brain"), py::arg("stage"), "Set developmental stage");

    // Error handling utilities
    m.def("checkBrainSystems", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, bool> systemStatus;
        systemStatus["working_memory"] = (brain->getWorkingMemory() != nullptr);
        systemStatus["episodic_memory"] = (brain->getEpisodicMemory() != nullptr);
        systemStatus["prediction"] = (brain->getPredictionSystem() != nullptr);
        systemStatus["development"] = (brain->getDevelopmentSystem() != nullptr);
        systemStatus["dopamine"] = (brain->getDopamine() != nullptr);
        systemStatus["curiosity"] = (brain->getCuriosity() != nullptr);
        systemStatus["novelty"] = (brain->getNovelty() != nullptr);
        systemStatus["planner"] = (brain->getPlanner() != nullptr);
        systemStatus["concept_formation"] = (brain->getConceptFormation() != nullptr);
        systemStatus["attention"] = (brain->getAttention() != nullptr);
        return systemStatus;
    }, py::arg("brain"), "Check status of all integrated brain systems");
}

} // namespace nlm