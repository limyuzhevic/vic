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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides a high-level Python interface to the complete NLM Phase 6
        brain simulation system, including:
        
        **Core Brain Functionality:**
        - Brain simulation with real LIF neuron dynamics
        - Sensory input processing and motor output generation
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and cognitive systems
        - Developmental stages and plasticity
        
        **Advanced APIs for Researchers:**
        - Phase 6 optimized configuration creation
        - Comprehensive simulation utilities
        - Research mode with profiling support
        - Agent-brain-world integration
        
        **Key Classes:**
        - Brain: Central neural simulation brain class
        - AgentBrain: Interface connecting NLM brain to world
        - SimpleWorld: 2D world for NLM simulation
        - Config: Configuration management
        - Various sensory, motor, and memory classes
        
        **Installation:**
        pip install pynlm
        
        **Basic Usage Example:**
        ```python
        import pynlm
        
        # Create Phase 6 configuration
        config = pynlm.createPhase6Config()
        
        # Create brain and initialize
        brain = pynlm.createBrain(config)
        if brain.initialize():
            print("Brain initialized successfully")
        
        # Create world and agent
        world = pynlm.createSimulationEnvironment(20, 20)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run demonstration
        spikes, reward, steps = pynlm.runPhase6Demo(brain, world)
        print(f"Demo completed: {spikes} spikes, {reward} total reward")
        ```
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId class
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

    // SynapseId class
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

    // RegionId class
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
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

    // Config class
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

    // SensoryInput base class
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
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
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

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // SimpleWorld class
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

    // Brain class
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
             "Log brain status")
        
        // ========== MEMORY SYSTEMS ==========
        
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system",
             py::return_value_policy::reference_internal)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system",
             py::return_value_policy::reference_internal)
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system",
             py::return_value_policy::reference_internal)
        
        // ========== PREDICTION SYSTEM ==========
        
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system",
             py::return_value_policy::reference_internal)
        
        // ========== COGNITION SYSTEMS ==========
        
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner system",
             py::return_value_policy::reference_internal)
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system",
             py::return_value_policy::reference_internal)
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system",
             py::return_value_policy::reference_internal)
        
        // ========== DEVELOPMENT SYSTEM ==========
        
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system",
             py::return_value_policy::reference_internal)
        
        // ========== NEUROMODULATION SYSTEMS ==========
        
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulation system",
             py::return_value_policy::reference_internal)
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulation system",
             py::return_value_policy::reference_internal)
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty detection system",
             py::return_value_policy::reference_internal)
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal",
             py::return_value_policy::reference_internal)
        
        // ========== ADVANCED BRAIN UTILITIES ==========
        
        .def("hasMemory", &Brain::getWorkingMemory,
             "Check if brain has working memory system (always true for NLM)")
        .def("getTotalMemoryCapacity", [] (const Brain& brain) {
            auto* workingMem = brain.getWorkingMemory();
            auto* episodicMem = brain.getEpisodicMemory();
            size_t totalCapacity = 0;
            if (workingMem) totalCapacity += workingMem->getCapacity();
            if (episodicMem) totalCapacity += episodicMem->getCapacity();
            return totalCapacity;
        }, "Get total memory capacity across all memory systems")
        .def("getActiveMemorySystems", [] (const Brain& brain) {
            std::vector<std::string> systems;
            if (brain.getWorkingMemory()) systems.push_back("WorkingMemory");
            if (brain.getEpisodicMemory()) systems.push_back("EpisodicMemory");
            if (brain.getAssociativeMemory()) systems.push_back("AssociativeMemory");
            return systems;
        }, "Get names of all active memory systems")
        .def("getSystemStatusSummary", &Brain::logStatus,
             "Get comprehensive system status summary (logs to console)")
        .def("isReadyForSimulation", [] (const Brain& brain) {
            return brain.getTotalNeuronCount() > 0 && 
                   brain.getTotalSynapseCount() > 0 &&
                   brain.getRegionCount() > 0;
        }, "Check if brain is properly initialized for simulation");

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

    // Advanced convenience functions
    m.def("createPhase6Config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Phase 6 core settings
        config->set("brain.neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("brain.connection_probability", 0.08f, ConfigSource::Default);
        config->set("simulation.timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        // Memory systems
        config->set("memory.working_memory_capacity", 200, ConfigSource::Default);
        config->set("memory.episodic_max_episodes", 5000, ConfigSource::Default);
        config->set("memory.associative_learning_rate", 0.01f, ConfigSource::Default);
        
        // Neuromodulation
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Development
        config->set("development.initial_plasticity", 1.0f, ConfigSource::Default);
        config->set("development.critical_period_end", 500, ConfigSource::Default);
        
        // Plasticity
        config->set("brain.stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config->set("brain.stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("brain.stdp_time_constant", 25.0f, ConfigSource::Default);
        
        return config;
    }, "Create a Phase 6 optimized configuration");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = createPhase6Config();
        
        // Research-specific settings
        config->set("research_mode.enable", true, ConfigSource::Default);
        config->set("research_mode.debug_mode", true, ConfigSource::Default);
        config->set("research_mode.performance_profiling", true, ConfigSource::Default);
        config->set("research_mode.memory_profiling", true, ConfigSource::Default);
        
        return config;
    }, "Create a research-optimized configuration with profiling enabled");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Phase 6 demonstration and utilities
    m.def("runPhase6Demo", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        // Phase 6 comprehensive integration test
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Initialize the system
        agent->initialize(*world);
        
        // Run simulation for demonstration
        const int demo_steps = 1000;
        float total_reward = 0.0f;
        size_t total_spikes = 0;
        
        for (int step = 0; step < demo_steps; ++step) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input through agent brain
            agent->processSensoryInput(percept);
            
            // Step the brain
            brain->step(step, step * 0.001f);
            
            // Get action from agent
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            float reward = 0.0f; // Could be calculated based on world state
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(1);
            
            // Accumulate statistics
            total_spikes += brain->getTotalSpikeCount();
            total_reward += reward;
        }
        
        // Return demonstration results
        return std::make_tuple(total_spikes, total_reward, demo_steps);
    }, py::arg("brain"), py::arg("world"), 
       "Run Phase 6 comprehensive integration demo (returns spikes, reward, steps)");

    m.def("createSimulationEnvironment", [](int width, int height, int vision_width, int vision_height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        
        // Add some interesting objects to the world
        auto& body = world->getAgentBody();
        body.x = width / 2;
        body.y = height / 2;
        body.energy = 100.0f;
        body.health = 100.0f;
        
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20, 
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Create a configured simulation environment");

    // Configuration validation
    m.def("validateBrainConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing brain.neuron_count");
        }
        
        if (config->getOr<int64_t>("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        if (!config->has("brain.region_count")) {
            errors.push_back("Missing brain.region_count");
        }
        
        if (config->getOr<int64_t>("brain.region_count", 0) <= 0) {
            errors.push_back("brain.region_count must be positive");
        }
        
        if (!config->has("simulation.timestep")) {
            errors.push_back("Missing simulation.timestep");
        }
        
        double timestep = config->getOr<double>("simulation.timestep", 0.0);
        if (timestep <= 0.0) {
            errors.push_back("simulation.timestep must be positive");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors (empty if valid)");

    // Convenience functions and constants
    m.def("getDefaultPhase6Settings", []() {
        py::dict settings;
        settings["neuron_count"] = 2000;
        settings["region_count"] = 4;
        settings["connection_probability"] = 0.08f;
        settings["simulation_timestep"] = 0.001f;
        settings["working_memory_capacity"] = 200;
        settings["episodic_max_episodes"] = 5000;
        settings["dopamine_scale"] = 1.0f;
        settings["curiosity_enabled"] = true;
        settings["novelty_enabled"] = true;
        settings["critical_period_end"] = 500;
        return settings;
    }, "Get default Phase 6 settings as Python dictionary");

    // Constants for Python users
    m.attr("PHASE_6_DEFAULT_NEURON_COUNT") = 2000;
    m.attr("PHASE_6_DEFAULT_REGION_COUNT") = 4;
    m.attr("PHASE_6_DEFAULT_CONNECTION_PROBABILITY") = 0.08f;
    m.attr("PHASE_6_DEFAULT_TIMESTEP") = 0.001f;
    m.attr("PHASE_6_DEFAULT_WORKING_MEMORY_CAPACITY") = 200;
    
    // Invalid identifiers for Python users
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}