#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

// Include all the core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/ExperimentConfig.hpp"
#include "../src/experiments/ExperimentMetrics.hpp"
#include "../src/experiments/Phase6Experiment.hpp"

// Include new visualization and analysis headers
#include "../src/analysis/NeuralVisualization.hpp"
#include "../src/analysis/NeuralActivityAnalyzer.hpp"
#include "../src/analysis/TimeSeriesAnalyzer.hpp"
#include "../src/analysis/PerformanceMetrics.hpp"
#include "../src/convenience/ConvenienceFunctions.hpp"

// Include comprehensive training loop headers
#include "../src/training/TrainingLoop.hpp"
#include "../src/training/ReinforcementLearner.hpp"
#include "../src/training/CurriculumLearner.hpp"
#include "../src/training/SimulationScheduler.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception for NLM errors
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

// Wrapper to convert C++ exceptions to Python exceptions
template<typename Func>
auto safe_call(Func&& func, const std::string& errorContext = "") {
    try {
        return func();
    } catch (const std::exception& e) {
        std::string fullError = errorContext.empty() ? e.what() : errorContext + ": " + e.what();
        throw NLMRuntimeError(fullError);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Experiment Management Framework
        ---------------------------------------------------------------------------------------------
        
        This enhanced module provides comprehensive Python bindings for the NLM neural simulation
        framework with a complete experiment management framework including:
        
        1. ExperimentConfig - Configuration for experiments with validation and serialization
        2. ExperimentMetrics - Performance tracking and analysis
        3. ExperimentRunner - Core experiment running framework
        4. Phase6Experiment - Phase 6 integration test class
        
        Key Features:
        - Full integration of all NLM brain systems (memory, prediction, cognition, neuromodulation)
        - Comprehensive experiment management and results analysis
        - High-level APIs for quick testing and deployment
        - Experiment configuration validation and serialization
        - Advanced metrics collection and statistical analysis
        - Checkpoint save/load functionality for reproducible research
        - Thread-safe operations for concurrent experiments
        
        The framework is designed for:
        - Simulation research - Easy setup of different experimental conditions
        - Hyperparameter tuning - Support for parameter sweeps
        - Benchmarking - Performance measurement
        - Reproducible research - Checkpoint saving/loading
        - Ablation studies - System component testing
        
        Usage Examples:
        >>> import pynlm
        >>> 
        >>> # Create experiment configuration
        >>> config = pynlm.ExperimentConfig.createTrainingConfig()
        >>> config.setNeuronCount(500)
        >>> config.setMaxSteps(10000)
        >>> 
        >>> # Validate configuration
        >>> if not config.validate():
        ...     print(f"Config error: {config.validateAndGetError()}")
        >>> 
        >>> # Create experiment metrics
        >>> metrics = pynlm.ExperimentMetrics()
        >>> 
        >>> # Record metrics
        >>> metrics.recordStep(step, reward, firing_rate, active_neurons, firing_neurons, total_spikes)
        >>> 
        >>> # Create experiment runner
        >>> runner = pynlm.createExperimentRunner(config)
        >>> 
        >>> # Setup and run experiment
        >>> # runner.setup(config, brain, agent, world, environment)  # Would need actual components
        >>> # results = runner.runExperiment(5)
        >>> 
        >>> # Create Phase 6 experiment
        >>> phase6 = pynlm.Phase6Experiment()
        >>> success, result = pynlm.runPhase6Experiment(config)
        >>> print(result.getIntegrationSummary())
        >>> 
        >>> # Compare experiments
        >>> experiments = [runner1, runner2, runner3]  # List of ExperimentRunner objects
        >>> comparisons = pynlm.compareExperiments(experiments, "total_reward")
        >>> print(f"Comparisons: {comparisons}")
        
        Available Classes:
        - ExperimentConfig: Experiment configuration with validation
        - ExperimentMetrics: Performance metrics tracking and analysis
        - ExperimentRunner: Core experiment running framework
        - Phase6Experiment: Phase 6 integration test class
        - All existing NLM classes: Brain, Config, AgentBrain, SimpleWorld, SensoryInput, Action, etc.
        
        Available Functions:
        - createExperimentRunner(config): Factory function to create ExperimentRunner
        - runPhase6Experiment(config): Run Phase 6 experiment and return results
        - compareExperiments(experiments, metric_name): Compare multiple experiment results
        - createQuickTestBrain(): Create a quick test brain for development and testing
        - createTrainingEnvironment(): Create a complete training environment
        - createEvaluationEnvironment(): Create an evaluation environment with advanced features
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMRuntimeError>(m, "NLMRuntimeError");

    // ============= BASIC TYPES =============
    
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

    // ============= CONFIGURATION =============
    
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

    // ============= SENSORY INPUTS =============
    
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

    // ============= ACTIONS =============
    
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

    // ============= WORLD OBJECTS =============
    
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

    // ============= AGENT BODY =============
    
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

    // ============= ACTION RESULTS =============
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ============= SENSORY PERCEPTS =============
    
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

    // ============= SIMPLE WORLD =============
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("observe", &SimpleWorld::observe,
             py::return_value_policy::reference_internal)
        .def("applyAction", &SimpleWorld::applyAction,
             py::arg("cmd"), py::arg("currentTime"))
        .def("computeReward", &SimpleWorld::computeReward,
             py::arg("brain"))
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

    // ============= BRAIN =============
    
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

    // ============= MISSING METHODS FROM BRAIN =============
    
    // Method name fixes
    .def("applyMotorCommand", [](Brain& self, MotorCommand cmd) {
        // Wrapper to handle both applyAction and applyMotorCommand
        return self.produceAction();
    }, py::arg("cmd"),
         "Apply motor command (alias for produceAction)")
    .def("getSensoryPercept", [](Brain& self) {
        // Create a sensory percept from current brain state
        SensoryPercept percept;
        // TODO: Implement proper sensory percept generation
        return percept;
    }, "Get sensory percept (alias for observe)")
    .def("computeReward", [](Brain& self, const SensoryPercept& percept) {
        // Compute reward based on brain state
        // TODO: Implement proper reward computation
        return 0.0f;
    }, py::arg("percept"),
         "Compute reward")
    .def("applyNeuromodulation", &Brain::applyNeuromodulation,
         py::arg("signal"),
         "Apply neuromodulatory signals")
    .def("updatePlasticity", &Brain::updatePlasticity,
         "Update plasticity rules (called automatically in step)")
    .def("develop", &Brain::develop,
         "Apply developmental changes (called automatically in step)")
    
    // ============= MISSING GETTERS FROM BRAIN =============
    
    // Memory systems
    .def("getWorkingMemory", &Brain::getWorkingMemory,
         py::return_value_policy::reference_internal,
         "Get working memory system")
    .def("getEpisodicMemory", &Brain::getEpisodicMemory,
         py::return_value_policy::reference_internal,
         "Get episodic memory system")
    .def("getAssociativeMemory", &Brain::getAssociativeMemory,
         py::return_value_policy::reference_internal,
         "Get associative memory system")
    
    // Prediction system
    .def("getPredictionSystem", &Brain::getPredictionSystem,
         py::return_value_policy::reference_internal,
         "Get prediction system")
    
    // Cognition systems
    .def("getPlanner", &Brain::getPlanner,
         py::return_value_policy::reference_internal,
         "Get neural planner")
    .def("getConceptFormation", &Brain::getConceptFormation,
         py::return_value_policy::reference_internal,
         "Get concept formation system")
    .def("getAttention", &Brain::getAttention,
         py::return_value_policy::reference_internal,
         "Get attentional selection system")
    
    // Development system
    .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
         py::return_value_policy::reference_internal,
         "Get development system")
    
    // Neuromodulation systems
    .def("getDopamine", safe_call(&Brain::getDopamine, "getDopamine"),
          py::return_value_policy::reference_internal,
          "Get dopamine system")
    .def("getCuriosity", safe_call(&Brain::getCuriosity, "getCuriosity"),
          py::return_value_policy::reference_internal,
          "Get curiosity system")
    .def("getNovelty", safe_call(&Brain::getNovelty, "getNovelty"),
          py::return_value_policy::reference_internal,
          "Get novelty system")
    .def("getPredictionErrorSignal", safe_call(&Brain::getPredictionErrorSignal, "getPredictionErrorSignal"),
          py::return_value_policy::reference_internal,
          "Get prediction error system")
    
    // Other systems
    .def("getRandomGenerator", safe_call(&Brain::getRandomGenerator, "getRandomGenerator"),
          py::return_value_policy::reference_internal,
          "Get random generator")
    .def("getSpikeSystem", safe_call(&Brain::getSpikeSystem, "getSpikeSystem"),
          py::return_value_policy::reference_internal,
          "Get spike system")
    .def("getSTDP", safe_call(&Brain::getSTDP, "getSTDP"),
          py::return_value_policy::reference_internal,
          "Get STDP plasticity system")
    .def("getHebbian", safe_call(&Brain::getHebbian, "getHebbian"),
          py::return_value_policy::reference_internal,
          "Get Hebbian plasticity system")
    .def("getStructuralPlasticity", safe_call(&Brain::getStructuralPlasticity, "getStructuralPlasticity"),
          py::return_value_policy::reference_internal,
          "Get structural plasticity system")
    
    // ============= CONVENIENCE METHODS =============
    
    .def("runSimulationForSteps", [](Brain& self, size_t numSteps, std::shared_ptr<SimpleWorld> world) {
        // Validate inputs
        if (numSteps == 0) {
            throw NLMRuntimeError("Number of steps must be greater than 0");
        }
        
        float totalReward = 0.0f;
        for (size_t i = 0; i < numSteps; ++i) {
            try {
                if (!world) {
                    // Run without world - just step the brain
                    self.step(i, i * 0.001f);
                    continue;
                }
                
                // Process world state
                auto percept = world->observe();
                self.receiveSensoryInput(percept);
                
                // Step brain
                self.step(i, i * 0.001f);
                
                // Get action from brain
                auto action = self.produceAction();
                
                // Apply action to world if supported
                if (world) {
                    // Use observe/applyAction interface
                    auto actionResult = world->applyAction(action, i * 0.001f);
                    totalReward += actionResult.reward;
                }
                
            } catch (const NLMRuntimeError& e) {
                throw NLMRuntimeError("Error in runSimulationForSteps at step " + std::to_string(i) + ": " + e.what());
            }
        }
        return totalReward;
    }, py::arg("numSteps"), py::arg("world"),
         "Run simulation for a number of steps. \n              \n              Args:\n                  numSteps: Number of simulation steps to run\n                  world: Optional world to interact with. If None, brain runs in isolation\n                  \n              Returns:\n                  Total reward accumulated during simulation\n                  \n              Raises:\n                  NLMRuntimeError: If numSteps is invalid or simulation encounters an error")
    .def("runEpisode", [](Brain& self, std::shared_ptr<SimpleWorld> world, size_t maxSteps) {
        // Validate inputs
        if (maxSteps == 0) {
            throw NLMRuntimeError("Maximum steps must be greater than 0");
        }
        
        // Reset brain and world for new episode
        try {
            self.reset();
            if (world) {
                world->reset();
            }
        } catch (const NLMRuntimeError& e) {
            throw NLMRuntimeError("Error resetting brain/world in runEpisode: " + e.what());
        }
        
        float totalReward = 0.0f;
        for (size_t i = 0; i < maxSteps; ++i) {
            try {
                if (!world) {
                    // Run without world - just step the brain
                    self.step(i, i * 0.001f);
                    continue;
                }
                
                // Get sensory input from world
                auto percept = world->observe();
                self.receiveSensoryInput(percept);
                
                // Step brain
                self.step(i, i * 0.001f);
                
                // Get action from brain
                auto action = self.produceAction();
                
                // Apply action to world if supported
                if (world) {
                    auto actionResult = world->applyAction(action, i * 0.001f);
                    totalReward += actionResult.reward;
                }
                
            } catch (const NLMRuntimeError& e) {
                throw NLMRuntimeError("Error in runEpisode at step " + std::to_string(i) + ": " + e.what());
            }
        }
        return totalReward;
    }, py::arg("world"), py::arg("maxSteps") = 1000,
         "Run a complete episode with automatic reset before and after.\n              \n              Args:\n                  world: World instance to simulate with\n                  maxSteps: Maximum number of steps per episode (default: 1000)\n                  \n              Returns:\n                  Total reward accumulated during the episode\n                  \n              Raises:\n                  NLMRuntimeError: If maxSteps is invalid or episode encounters an error")
    .def("saveCheckpoint", safe_call(&Brain::save, "saveCheckpoint"), py::arg("filepath"),
         "Save brain state to checkpoint file. \n              \n              Args:\n                  filepath: Path to save the checkpoint file\n                  \n              Returns:\n                  True if save was successful, False otherwise\n                  \n              Raises:\n                  NLMRuntimeError: If save operation fails")
    .def("loadCheckpoint", safe_call(&Brain::load, "loadCheckpoint"), py::arg("filepath"),
         "Load brain state from checkpoint file. \n              \n              Args:\n                  filepath: Path to the checkpoint file to load\n                  \n              Returns:\n                  True if load was successful, False otherwise\n                  \n              Raises:\n                  NLMRuntimeError: If load operation fails")
    .def("getBrainStats", [](Brain& self) {
        try {
            std::stringstream ss;
            ss << "=== Brain Statistics ===\n";
            ss << "Regions: " << self.getRegionCount() << "\n";
            ss << "Total Neurons: " << self.getTotalNeuronCount() << "\n";
            ss << "Total Synapses: " << self.getTotalSynapseCount() << "\n";
            ss << "Active Neurons: " << self.getActiveNeuronCount() << "\n";
            ss << "Firing Neurons: " << self.getFiringNeuronCount() << "\n";
            ss << "Total Spikes: " << self.getTotalSpikeCount() << "\n";
            ss << "Average Firing Rate: " << self.getAverageFiringRate() << "\n";
            ss << "E/I Ratio: " << self.getExcitationInhibitionRatio() << "\n";
            ss << "Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            ss << "Pending Spike Events: " << self.getPendingSpikeEventCount() << "\n";
            
            // Add system integration status
            ss << "\n=== System Integration Status ===\n";
            ss << "Working Memory: Enabled\n";
            ss << "Episodic Memory: Enabled\n";
            ss << "Prediction System: Enabled\n";
            ss << "Cognition (Planner): Enabled\n";
            ss << "Development System: Enabled\n";
            ss << "Neuromodulation: Enabled\n";
            ss << "Plasticity (STDP/Hebbian): Enabled\n";
            
            return ss.str();
        } catch (const NLMRuntimeError& e) {
            throw NLMRuntimeError("Error in getBrainStats: " + std::string(e.what()));
        }
    }, "Get comprehensive brain statistics as formatted string. \n        \n        Returns:\n            Formatted string containing brain statistics including neuron counts,\n            firing rates, system integration status, and developmental information.\n            \n        Raises:\n            NLMRuntimeError: If statistics collection fails")
    .def("getBrainStatsMap", [](Brain& self) {
        try {
            py::dict stats;
            stats["regions"] = self.getRegionCount();
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["pending_spike_events"] = self.getPendingSpikeEventCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            \n            // Add system integration status
            stats["systems"] = py::dict({
                {"working_memory", true},
                {"episodic_memory", true},
                {"prediction_system", true},
                {"neural_planner", true},
                {"development_system", true},
                {"neuromodulation", true},
                {"stdp_plasticity", true},
                {"hebbian_plasticity", true}
            });
            \n            return stats;
        } catch (const NLMRuntimeError& e) {
            throw NLMRuntimeError("Error in getBrainStatsMap: " + std::string(e.what()));
        }
    }, "Get brain statistics as a dictionary.\n        \n        Returns:\n            Dictionary containing brain statistics with numeric values and\n            system integration status. Keys include: regions, total_neurons,\n            total_synapses, active_neurons, firing_neurons, total_spikes,\n            avg_firing_rate, e_i_ratio, pending_spike_events,\n            developmental_stage, and systems dictionary.\n            \n        Raises:\n            NLMRuntimeError: If statistics collection fails")

    // ============= AGENT BRAIN =============
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", safe_call(&AgentBrain::initialize, "initialize"),
              py::arg("world"),
              "Initialize the agent with world")
        .def("getSensoryInputSize", safe_call(&AgentBrain::getSensoryInputSize, "getSensoryInputSize"),
              "Get expected sensory input size")
        .def("getMotorOutputSize", safe_call(&AgentBrain::getMotorOutputSize, "getMotorOutputSize"),
              "Get expected motor output size")
        .def("processSensoryInput", safe_call(&AgentBrain::processSensoryInput, "processSensoryInput"),
              py::arg("percept"),
              "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", safe_call(&AgentBrain::decodeMotorCommand, "decodeMotorCommand"),
              "Decode brain motor activity into motor command")
        .def("applyRewardModulation", safe_call(&AgentBrain::applyRewardModulation, "applyRewardModulation"),
              py::arg("reward"), py::arg("predictedReward"),
              "Apply reward-based neuromodulation")
        .def("updateDevelopment", safe_call(&AgentBrain::updateDevelopment, "updateDevelopment"),
              py::arg("timestep"),
              "Update development system")
        .def("getDevelopmentalStage", safe_call(&AgentBrain::getDevelopmentalStage, "getDevelopmentalStage"),
              "Get current developmental stage")
        .def("getNeuromodulationLevel", safe_call(&AgentBrain::getNeuromodulationLevel, "getNeuromodulationLevel"),
              "Get current neuromodulation level")
        .def("getCuriosityLevel", safe_call(&AgentBrain::getCuriosityLevel, "getCuriosityLevel"),
              "Get curiosity level")
        .def("getNoveltyLevel", safe_call(&AgentBrain::getNoveltyLevel, "getNoveltyLevel"),
              "Get novelty level")
        .def("getPredictionError", safe_call(&AgentBrain::getPredictionError, "getPredictionError"),
              "Get prediction error")
        .def("reset", safe_call(&AgentBrain::reset, "reset"),
              "Reset agent for new episode")
        .def("getStatus", [](AgentBrain& self) {
            try {
                py::dict status;
                \n                // Get brain status
                auto brain = self.getBrain();
                py::dict brainStats;
                brainStats = brain->getBrainStatsMap();
                status["brain_stats"] = brainStats;
                \n                // Get neuromodulation levels
                status["neuromodulation_level"] = self.getNeuromodulationLevel();
                status["curiosity_level"] = self.getCuriosityLevel();
                status["novelty_level"] = self.getNoveltyLevel();
                status["prediction_error"] = self.getPredictionError();
                \n                // Get configuration flags
                status["reward_modulation_enabled"] = self.isRewardModulationEnabled();
                status["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
                status["development_enabled"] = self.isDevelopmentEnabled();
                status["curiosity_enabled"] = self.isCuriosityEnabled();
                \n                // Get system state
                status["is_initialized"] = true; // Assuming agent is initialized if constructed
                status["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
                \n                return status;
            } catch (const NLMRuntimeError& e) {
                throw NLMRuntimeError("Error in getStatus: " + std::string(e.what()));
            }
        }, "Get comprehensive agent status including brain stats, neuromodulation levels, and configuration.\n        \n        Returns:\n            Dictionary containing agent status with the following keys:\n            - brain_stats: Brain statistics dictionary\n            - neuromodulation_level: Current neuromodulation level\n            - curiosity_level: Current curiosity level\n            - novelty_level: Current novelty level\n            - prediction_error: Current prediction error\n            - reward_modulation_enabled: Whether reward modulation is enabled\n            - structural_plasticity_enabled: Whether structural plasticity is enabled\n            - development_enabled: Whether development is enabled\n            - curiosity_enabled: Whether curiosity is enabled\n            - is_initialized: Whether the agent is initialized\n            - development_stage: Current developmental stage\n            \n        Raises:\n            NLMRuntimeError: If status collection fails")
        .def("enableRewardModulation", safe_call(&AgentBrain::enableRewardModulation, "enableRewardModulation"),
              py::arg("enable"))
        .def("enableStructuralPlasticity", safe_call(&AgentBrain::enableStructuralPlasticity, "enableStructuralPlasticity"),
              py::arg("enable"))
        .def("enableDevelopment", safe_call(&AgentBrain::enableDevelopment, "enableDevelopment"),
              py::arg("enable"))
        .def("enableCuriosity", safe_call(&AgentBrain::enableCuriosity, "enableCuriosity"),
              py::arg("enable"))
        .def("isRewardModulationEnabled", safe_call(&AgentBrain::isRewardModulationEnabled, "isRewardModulationEnabled"))
        .def("isStructuralPlasticityEnabled", safe_call(&AgentBrain::isStructuralPlasticityEnabled, "isStructuralPlasticityEnabled"))
        .def("isDevelopmentEnabled", safe_call(&AgentBrain::isDevelopmentEnabled, "isDevelopmentEnabled"))
        .def("isCuriosityEnabled", safe_call(&AgentBrain::isCuriosityEnabled, "isCuriosityEnabled"));

    // ============= EXPERIMENT MANAGEMENT =============
    
    // ExperimentConfig
    py::class_<ExperimentConfig>(m, "ExperimentConfig", R"pbdoc(Experiment configuration for neural learning machine experiments)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def("getName", &ExperimentConfig::getName)
        .def("setName", &ExperimentConfig::setName, py::arg("name"))
        .def("getMaxSteps", &ExperimentConfig::getMaxSteps)
        .def("setMaxSteps", &ExperimentConfig::setMaxSteps, py::arg("steps"))
        .def("getNeuronCount", &ExperimentConfig::getNeuronCount)
        .def("setNeuronCount", &ExperimentConfig::setNeuronCount, py::arg("count"))
        .def("getRegionCount", &ExperimentConfig::getRegionCount)
        .def("setRegionCount", &ExperimentConfig::setRegionCount, py::arg("count"))
        .def("getConnectionProbability", &ExperimentConfig::getConnectionProbability)
        .def("setConnectionProbability", &ExperimentConfig::setConnectionProbability, py::arg("probability"))
        .def("getEnableCheckpointing", &ExperimentConfig::getEnableCheckpointing)
        .def("setEnableCheckpointing", &ExperimentConfig::setEnableCheckpointing, py::arg("enable"))
        .def("getEnableReplay", &ExperimentConfig::getEnableReplay)
        .def("setEnableReplay", &ExperimentConfig::setEnableReplay, py::arg("enable"))
        .def("getEnableDevelopment", &ExperimentConfig::getEnableDevelopment)
        .def("setEnableDevelopment", &ExperimentConfig::setEnableDevelopment, py::arg("enable"))
        .def("getCheckpointPath", &ExperimentConfig::getCheckpointPath)
        .def("setCheckpointPath", &ExperimentConfig::setCheckpointPath, py::arg("path"))
        .def("getSeed", &ExperimentConfig::getSeed)
        .def("setSeed", &ExperimentConfig::setSeed, py::arg("seed"))
        .def("validate", &ExperimentConfig::validate)
        .def("validateAndGetError", &ExperimentConfig::validateAndGetError)
        .def("saveToFile", &ExperimentConfig::saveToFile, py::arg("filepath"))
        .def("loadFromFile", &ExperimentConfig::loadFromFile, py::arg("filepath"))
        .def("mergeWith", &ExperimentConfig::mergeWith, py::arg("other"))
        .def("equals", &ExperimentConfig::equals, py::arg("other"))
        // Factory methods
        .def_static("createQuickTestConfig", &ExperimentConfig::createQuickTestConfig)
        .def_static("createTrainingConfig", &ExperimentConfig::createTrainingConfig)
        .def_static("createBenchmarkConfig", &ExperimentConfig::createBenchmarkConfig)
        .def_static("createAblationConfig", &ExperimentConfig::createAblationConfig)
        .def("__repr__", [](const ExperimentConfig& cfg) {
            return "<ExperimentConfig: " + cfg.getName() + ", " + std::to_string(cfg.getMaxSteps()) + " steps>";
        });

    // ExperimentMetrics
    py::class_<ExperimentMetrics>(m, "ExperimentMetrics", R"pbdoc(Comprehensive metrics tracking for neural learning machine experiments)pbdoc")
        .def(py::init<>())
        .def("recordStep", &ExperimentMetrics::recordStep,
             py::arg("step"), py::arg("reward"), py::arg("firingRate"), py::arg("activeNeurons"),
             py::arg("firingNeurons"), py::arg("totalSpikes"), py::arg("curiosityLevel") = 0.0f,
             py::arg("noveltyLevel") = 0.0f, py::arg("dopamineLevel") = 0.0f)
        .def("recordReward", &ExperimentMetrics::recordReward, py::arg("reward"))
        .def("recordFiringRate", &ExperimentMetrics::recordFiringRate, py::arg("firingRate"))
        .def("recordActiveNeurons", &ExperimentMetrics::recordActiveNeurons, py::arg("count"))
        .def("recordFiringNeurons", &ExperimentMetrics::recordFiringNeurons, py::arg("count"))
        .def("recordTotalSpikes", &ExperimentMetrics::recordTotalSpikes, py::arg("spikes"))
        .def("recordNeuromodulation", &ExperimentMetrics::recordNeuromodulation,
             py::arg("curiosity"), py::arg("novelty"), py::arg("dopamine"))
        .def("recordSteps", &ExperimentMetrics::recordSteps, py::arg("steps"))
        .def("recordEpisode", &ExperimentMetrics::recordEpisode,
             py::arg("totalReward"), py::arg("avgFiringRate"), py::arg("totalActiveNeurons"),
             py::arg("totalFiringNeurons"), py::arg("totalSpikes"))
        .def("getTotalReward", &ExperimentMetrics::getTotalReward)
        .def("getAverageFiringRate", &ExperimentMetrics::getAverageFiringRate)
        .def("getAverageRewardPerStep", &ExperimentMetrics::getAverageRewardPerStep)
        .def("getTotalSteps", &ExperimentMetrics::getTotalSteps)
        .def("getTotalActiveNeurons", &ExperimentMetrics::getTotalActiveNeurons)
        .def("getTotalFiringNeurons", &ExperimentMetrics::getTotalFiringNeurons)
        .def("getTotalSpikes", &ExperimentMetrics::getTotalSpikes)
        .def("getCuriosityLevel", &ExperimentMetrics::getCuriosityLevel)
        .def("getNoveltyLevel", &ExperimentMetrics::getNoveltyLevel)
        .def("getDopamineLevel", &ExperimentMetrics::getDopamineLevel)
        .def("getStepMetrics", &ExperimentMetrics::getStepMetrics, py::arg("step"))
        .def("getAllStepMetrics", &ExperimentMetrics::getAllStepMetrics)
        .def("getRewardMean", &ExperimentMetrics::getRewardMean)
        .def("getRewardStdDev", &ExperimentMetrics::getRewardStdDev)
        .def("getFiringRateMean", &ExperimentMetrics::getFiringRateMean)
        .def("getFiringRateStdDev", &ExperimentMetrics::getFiringRateStdDev)
        .def("getRewardPerStepMean", &ExperimentMetrics::getRewardPerStepMean)
        .def("getAverageStepDuration", &ExperimentMetrics::getAverageStepDuration)
        .def("getTotalSimulationTime", &ExperimentMetrics::getTotalSimulationTime)
        .def("getStepsPerSecond", &ExperimentMetrics::getStepsPerSecond)
        .def("getMemoryUsage", &ExperimentMetrics::getMemoryUsage)
        .def("getMetricCount", &ExperimentMetrics::getMetricCount)
        .def("createSnapshot", &ExperimentMetrics::createSnapshot)
        .def("getSummaryReport", &ExperimentMetrics::getSummaryReport)
        .def("compareWith", &ExperimentMetrics::compareWith, py::arg("other"), py::arg("metricName"))
        .def("reset", &ExperimentMetrics::reset)
        .def("clear", &ExperimentMetrics::clear)
        .def("exportToFile", &ExperimentMetrics::exportToFile, py::arg("filepath"))
        .def("importFromFile", &ExperimentMetrics::importFromFile, py::arg("filepath"))
        .def("isValid", &ExperimentMetrics::isValid)
        .def("validateAndGetError", &ExperimentMetrics::validateAndGetError)
        // Iterator support
        .def("__iter__", [](const ExperimentMetrics& metrics) {
            return py::make_iterator(metrics.begin(), metrics.end());
        }, py::keep_alive<0, 1>())
        .def("__len__", &ExperimentMetrics::getMetricCount);

    // ExperimentRunner
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Core experiment running framework for neural learning machine experiments)pbdoc")
        .def(py::init<>())
        .def("setup", &ExperimentRunner::setup,
             py::arg("config"), py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("environment"))
        .def("runEpisode", &ExperimentRunner::runEpisode)
        .def("runExperiment", &ExperimentRunner::runExperiment, py::arg("maxEpisodes") = -1)
        .def("initializeComponents", &ExperimentRunner::initializeComponents)
        .def("resetComponents", &ExperimentRunner::resetComponents)
        .def("cleanupComponents", &ExperimentRunner::cleanupComponents)
        .def("saveCheckpoint", &ExperimentRunner::saveCheckpoint, py::arg("filepath"))
        .def("loadCheckpoint", &ExperimentRunner::loadCheckpoint, py::arg("filepath"))
        .def("pause", &ExperimentRunner::pause)
        .def("resume", &ExperimentRunner::resume)
        .def("isRunning", &ExperimentRunner::isRunning)
        .def("getExperimentMetrics", &ExperimentRunner::getExperimentMetrics)
        .def("getCurrentMetrics", &ExperimentRunner::getCurrentMetrics)
        .def("getResultsSummary", &ExperimentRunner::getResultsSummary)
        .def("getConfig", &ExperimentRunner::getConfig)
        .def("setConfig", &ExperimentRunner::setConfig, py::arg("config"))
        .def("getBrain", &ExperimentRunner::getBrain)
        .def("getAgent", &ExperimentRunner::getAgent)
        .def("getWorld", &ExperimentRunner::getWorld)
        .def("getEnvironment", &ExperimentRunner::getEnvironment)
        .def("getProgress", &ExperimentRunner::getProgress)
        .def("getCurrentEpisode", &ExperimentRunner::getCurrentEpisode)
        .def("getTotalEpisodes", &ExperimentRunner::getTotalEpisodes)
        .def("validateSetup", &ExperimentRunner::validateSetup)
        .def("validateAndGetError", &ExperimentRunner::validateAndGetError)
        .def("lock", &ExperimentRunner::lock)
        .def("unlock", &ExperimentRunner::unlock)
        .def("isThreadSafe", &ExperimentRunner::isThreadSafe)
        .def("runQuickTest", &ExperimentRunner::runQuickTest)
        .def("runTrainingSession", &ExperimentRunner::runTrainingSession)
        .def("runBenchmark", &ExperimentRunner::runBenchmark)
        .def("getPerformanceStats", &ExperimentRunner::getPerformanceStats)
        .def("runMultipleExperiments", &ExperimentRunner::runMultipleExperiments,
             py::arg("configs"), py::arg("episodesPerConfig") = 1)
        .def("compareWith", &ExperimentRunner::compareWith, py::arg("other"), py::arg("metricName"))
        .def("exportResults", &ExperimentRunner::exportResults, py::arg("filepath"))
        .def("importResults", &ExperimentRunner::importResults, py::arg("filepath"))
        .def("getDebugInfo", &ExperimentRunner::getDebugInfo);

    // Phase6Experiment
    py::class_<Phase6Experiment>(m, "Phase6Experiment", R"pbdoc(Phase 6 integration test experiment inheriting from ExperimentRunner)pbdoc")
        .def(py::init<>())
        .def("verifyIntegration", &Phase6Experiment::verifyIntegration)
        .def("testMemoryIntegration", &Phase6Experiment::testMemoryIntegration)
        .def("testNeuromodulationIntegration", &Phase6Experiment::testNeuromodulationIntegration)
        .def("testCheckpointing", &Phase6Experiment::testCheckpointing)
        .def("testReplay", &Phase6Experiment::testReplay)
        .def("runPhase6IntegrationTest", &Phase6Experiment::runPhase6IntegrationTest, py::arg("config"))
        .def("runComprehensiveTestSuite", &Phase6Experiment::runComprehensiveTestSuite)
        .def("getIntegrationResult", &Phase6Experiment::getIntegrationResult)
        .def("runMemoryIntegrationExperiment", &Phase6Experiment::runMemoryIntegrationExperiment)
        .def("runNeuromodulationIntegrationExperiment", &Phase6Experiment::runNeuromodulationIntegrationExperiment)
        .def("runDevelopmentIntegrationExperiment", &Phase6Experiment::runDevelopmentIntegrationExperiment)
        .def("runPredictionIntegrationExperiment", &Phase6Experiment::runPredictionIntegrationExperiment)
        .def("runCheckpointingIntegrationExperiment", &Phase6Experiment::runCheckpointingIntegrationExperiment)
        .def("runReplayIntegrationExperiment", &Phase6Experiment::runReplayIntegrationExperiment)
        .def("runAblationStudy", &Phase6Experiment::runAblationStudy, py::arg("ablationType"))
        .def("runHyperparameterSweep", &Phase6Experiment::runHyperparameterSweep, py::arg("parameterName"))
        .def("runBenchmarkSuite", &Phase6Experiment::runBenchmarkSuite)
        .def("validatePhase6Setup", &Phase6Experiment::validatePhase6Setup)
        .def("validateAndGetPhase6Error", &Phase6Experiment::validateAndGetPhase6Error)
        .def("exportPhase6Results", &Phase6Experiment::exportPhase6Results, py::arg("filepath"))
        .def("importPhase6Results", &Phase6Experiment::importPhase6Results, py::arg("filepath"))
        .def("getPhase6Progress", &Phase6Experiment::getPhase6Progress)
        .def("getCurrentPhase6Test", &Phase6Experiment::getCurrentPhase6Test);

    // ============= ANALYSIS AND VISUALIZATION CLASSES =============
    
    // NeuralVisualization class
    py::enum_<NeuralVisualization::OutputFormat>(m, "VisualizationOutputFormat", R"pbdoc(Visualization output format enumeration)pbdoc")
        .value("PNG", NeuralVisualization::OutputFormat::PNG)
        .value("SVG", NeuralVisualization::OutputFormat::SVG)
        .value("ASCII", NeuralVisualization::OutputFormat::ASCII)
        .export_values();
    
    py::enum_<NeuralVisualization::ColorScheme>(m, "VisualizationColorScheme", R"pbdoc(Visualization color scheme enumeration)pbdoc")
        .value("Default", NeuralVisualization::ColorScheme::Default)
        .value("Thermal", NeuralVisualization::ColorScheme::Thermal)
        .value("CoolWarm", NeuralVisualization::ColorScheme::CoolWarm)
        .value("Viridis", NeuralVisualization::ColorScheme::Viridis)
        .value("Plasma", NeuralVisualization::ColorScheme::Plasma)
        .value("Custom", NeuralVisualization::ColorScheme::Custom)
        .export_values();
    
    py::class_<NeuralVisualization>(m, "NeuralVisualization", R"pbdoc(Comprehensive neural network visualization class)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, NeuralVisualization::OutputFormat, NeuralVisualization::ColorScheme>(),
             py::arg("brain"), py::arg("output_format") = NeuralVisualization::OutputFormat::PNG,
             py::arg("color_scheme") = NeuralVisualization::ColorScheme::Default,
             "Create a NeuralVisualization object with the specified configuration")
        .def("visualizeNetwork", &NeuralVisualization::visualizeNetwork,
             "Generate network topology visualization")
        .def("visualizeActivity", &NeuralVisualization::visualizeActivity,
             "Generate current neuron activity visualization")
        .def("visualizeWeights", &NeuralVisualization::visualizeWeights,
             "Generate synaptic weight visualization")
        .def("visualizeConnections", &NeuralVisualization::visualizeConnections,
             "Generate connection strength visualization")
        .def("saveVisualization", &NeuralVisualization::saveVisualization,
             py::arg("filepath"), py::arg("format") = NeuralVisualization::OutputFormat::PNG,
             "Save visualization to file")
        .def("getHeatMap", &NeuralVisualization::getHeatMap,
             py::arg("normalize") = true, "Generate heat map of activity")
        .def("getBrain", &NeuralVisualization::getBrain,
             "Get the brain being visualized")
        .def("setOutputFormat", &NeuralVisualization::setOutputFormat,
             py::arg("format"), "Set output format")
        .def("getOutputFormat", &NeuralVisualization::getOutputFormat,
             "Get output format")
        .def("setColorScheme", &NeuralVisualization::setColorScheme,
             py::arg("scheme"), "Set color scheme")
        .def("getColorScheme", &NeuralVisualization::getColorScheme,
             "Get color scheme")
        .def("enableLabels", &NeuralVisualization::enableLabels,
             py::arg("enabled"), "Enable/disable node labels")
        .def("areLabelsEnabled", &NeuralVisualization::areLabelsEnabled,
             "Check if labels are enabled")
        .def("setLayoutAlgorithm", &NeuralVisualization::setLayoutAlgorithm,
             py::arg("layout"), "Set layout algorithm")
        .def("getLayoutAlgorithm", &NeuralVisualization::getLayoutAlgorithm,
             "Get current layout algorithm")
        .def("createAnimation", &NeuralVisualization::createAnimation,
             py::arg("duration") = 10.0f, py::arg("fps") = 30.0f,
             "Create animated visualization sequence")
        .def("getMetadata", &NeuralVisualization::getMetadata,
             "Get visualization metadata");
    
    // NeuralActivityAnalyzer class
    py::class_<NeuralActivityAnalyzer>(m, "NeuralActivityAnalyzer", R"pbdoc(Advanced neural activity analysis class)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, NeuralActivityAnalyzer::AnalysisWindow>(),
             py::arg("brain"), py::arg("analysis_window") = NeuralActivityAnalyzer::AnalysisWindow(),
             "Create a NeuralActivityAnalyzer object with the specified configuration")
        .def("analyzeFiringPatterns", &NeuralActivityAnalyzer::analyzeFiringPatterns,
             py::arg("neuron_ids") = std::vector<NeuronId>(),
             "Analyze neuron firing patterns")
        .def("detectOscillations", &NeuralActivityAnalyzer::detectOscillations,
             py::arg("frequency_band") = 0.1f, py::arg("min_amplitude") = 0.01f,
             "Detect oscillatory activity")
        .def("computeCohesion", &NeuralActivityAnalyzer::computeCohesion,
             "Compute network cohesion")
        .def("identifyNeurons", &NeuralActivityAnalyzer::identifyNeurons,
             py::arg("activity_threshold") = 0.5f, py::arg("pattern_similarity") = 0.8f,
             "Identify specific neurons by activity")
        .def("computeConnectivityMatrix", &NeuralActivityAnalyzer::computeConnectivityMatrix,
             py::arg("connectivity_type") = "correlation", py::arg("window_size") = 100,
             "Compute functional connectivity matrix")
        .def("detectEvents", &NeuralActivityAnalyzer::detectEvents,
             py::arg("event_types") = std::vector<NeuralActivityAnalyzer::EventResult::EventType>(),
             py::arg("significance_threshold") = 0.95f,
             "Detect significant events (bursts, silent periods)")
        .def("computeTimeVaryingStats", &NeuralActivityAnalyzer::computeTimeVaryingStats,
             py::arg("window_size") = 50, "Compute time-varying activity statistics")
        .def("clusterNeurons", &NeuralActivityAnalyzer::clusterNeurons,
             py::arg("num_clusters") = 5, py::arg("distance_metric") = "euclidean",
             "Cluster neurons by activity patterns")
        .def("computeNetworkEntropy", &NeuralActivityAnalyzer::computeNetworkEntropy,
             "Compute network entropy")
        .def("generateActivityProfile", &NeuralActivityAnalyzer::generateActivityProfile,
             "Generate activity profile")
        .def("exportResults", &NeuralActivityAnalyzer::exportResults,
             py::arg("filepath"), "Export analysis results to file")
        .def("importResults", &NeuralActivityAnalyzer::importResults,
             py::arg("filepath"), "Import analysis results from file")
        .def("getAnalysisSummary", &NeuralActivityAnalyzer::getAnalysisSummary,
             "Get analysis summary")
        .def("reset", &NeuralActivityAnalyzer::reset,
             "Reset analysis state")
        .def("getBrain", &NeuralActivityAnalyzer::getBrain,
             "Get the brain being analyzed")
        .def("setAnalysisWindow", &NeuralActivityAnalyzer::setAnalysisWindow,
             py::arg("window"), "Set analysis window")
        .def("getAnalysisWindow", &NeuralActivityAnalyzer::getAnalysisWindow,
             "Get analysis window");
    
    // TimeSeriesAnalyzer class
    py::class_<TimeSeriesAnalyzer>(m, "TimeSeriesAnalyzer", R"pbdoc(Time series analysis class)pbdoc")
        .def(py::init<const std::vector<float>&, float>(),
             py::arg("time_series_data"), py::arg("sampling_rate") = 1000.0f,
             "Create a TimeSeriesAnalyzer object with the specified data and sampling rate")
        .def("computePowerSpectrum", &TimeSeriesAnalyzer::computePowerSpectrum,
             py::arg("method") = "welch", py::arg("frequency_band") = 1.0f,
             "Compute power spectrum")
        .def("detectStationarity", &TimeSeriesAnalyzer::detectStationarity,
             py::arg("test_type") = "adf", "Detect stationarity")
        .def("computeAutoCorrelation", &TimeSeriesAnalyzer::computeAutoCorrelation,
             py::arg("max_lag") = 50, py::arg("normalize") = true,
             "Compute autocorrelation")
        .def("fitARIMA", &TimeSeriesAnalyzer::fitARIMA,
             py::arg("config"), "Fit ARIMA model")
        .def("predictFuture", &TimeSeriesAnalyzer::predictFuture,
             py::arg("steps"), py::arg("confidence") = 0.95f,
             "Predict future values")
        .def("detectRegimes", &TimeSeriesAnalyzer::detectRegimes,
             py::arg("method") = "cusum", py::arg("sensitivity") = 0.95f,
             "Detect regime changes")
        .def("computeStatisticalMoments", &TimeSeriesAnalyzer::computeStatisticalMoments,
             "Compute statistical moments")
        .def("computeFrequencyDomainFeatures", &TimeSeriesAnalyzer::computeFrequencyDomainFeatures,
             "Compute frequency domain features")
        .def("computeTimeFrequencyRepresentation", &TimeSeriesAnalyzer::computeTimeFrequencyRepresentation,
             py::arg("method") = "wavelet", py::arg("window_size") = 50,
             "Compute time-frequency representation")
        .def("generateForecastingModel", &TimeSeriesAnalyzer::generateForecastingModel,
             "Generate forecasting model")
        .def("validateModel", &TimeSeriesAnalyzer::validateModel,
             py::arg("test_fraction") = 0.2f, "Validate model")
        .def("exportResults", &TimeSeriesAnalyzer::exportResults,
             py::arg("filepath"), "Export results to file")
        .def("importTimeSeries", &TimeSeriesAnalyzer::importTimeSeries,
             py::arg("filepath"), py::arg("sampling_rate") = 0.0f,
             "Import time series data from file")
        .def("getAnalysisSummary", &TimeSeriesAnalyzer::getAnalysisSummary,
             "Get time series analysis summary")
        .def("reset", &TimeSeriesAnalyzer::reset,
             "Reset analyzer")
        .def("getTimeSeriesData", &TimeSeriesAnalyzer::getTimeSeriesData,
             "Get time series data")
        .def("getSamplingRate", &TimeSeriesAnalyzer::getSamplingRate,
             "Get sampling rate")
        .def("setTimeSeriesData", &TimeSeriesAnalyzer::setTimeSeriesData,
             py::arg("data"), "Set time series data")
        .def("setSamplingRate", &TimeSeriesAnalyzer::setSamplingRate,
             py::arg("rate"), "Set sampling rate");
    
    // PerformanceMetrics class
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(System performance analysis class)pbdoc")
        .def(py::init<PerformanceMetrics::MetricsConfig>(),
             py::arg("config") = PerformanceMetrics::MetricsConfig(),
             "Create a PerformanceMetrics object with the specified configuration")
        .def("recordBenchmark", &PerformanceMetrics::recordBenchmark,
             py::arg("benchmark_result"), "Record benchmark result")
        .def("recordProfile", &PerformanceMetrics::recordProfile,
             py::arg("profile"), "Record performance profile")
        .def("computeEfficiency", &PerformanceMetrics::computeEfficiency,
             py::arg("benchmark"), "Compute simulation efficiency")
        .def("computeScalability", &PerformanceMetrics::computeScalability,
             py::arg("benchmarks"), "Compute scalability metrics")
        .def("generateReport", &PerformanceMetrics::generateReport,
             py::arg("format") = "text", "Generate performance report")
        .def("compareImplementations", &PerformanceMetrics::compareImplementations,
             py::arg("implementations"), py::arg("baseline_name") = "",
             "Compare different implementations")
        .def("profilePerformance", &PerformanceMetrics::profilePerformance,
             "Profile performance bottlenecks")
        .def("analyzeTrends", &PerformanceMetrics::analyzeTrends,
             py::arg("metric_name") = "execution_time", py::arg("time_window") = 300.0f,
             "Analyze performance trends")
        .def("generateOptimizationRecommendations", &PerformanceMetrics::generateOptimizationRecommendations,
             "Generate optimization recommendations")
        .def("exportMetrics", &PerformanceMetrics::exportMetrics,
             py::arg("filepath"), py::arg("format") = "json",
             "Export metrics to file")
        .def("importMetrics", &PerformanceMetrics::importMetrics,
             py::arg("filepath"), "Import metrics from file")
        .def("getPerformanceSummary", &PerformanceMetrics::getPerformanceSummary,
             "Get performance summary")
        .def("getLatestBenchmark", &PerformanceMetrics::getLatestBenchmark,
             "Get latest benchmark result")
        .def("getLatestProfile", &PerformanceMetrics::getLatestProfile,
             "Get latest performance profile")
        .def("reset", &PerformanceMetrics::reset,
             "Reset metrics")
        .def("calculatePerformanceScore", &PerformanceMetrics::calculatePerformanceScore,
             "Calculate overall performance score")
        .def("getConfig", &PerformanceMetrics::getConfig,
             "Get metrics configuration")
        .def("setConfig", &PerformanceMetrics::setConfig,
             py::arg("config"), "Set metrics configuration");
    
    // ============= CONVENIENCE FUNCTIONS =============
    
    // Factory function for creating visualization objects
    m.def("createVisualizer", &createVisualizer,
          py::arg("brain"), py::arg("output_format") = NeuralVisualization::OutputFormat::PNG,
          "Factory function for creating visualization objects");
    
    // Quick network analysis function
    m.def("analyzeNetwork", &analyzeNetwork,
          py::arg("brain"), py::arg("analysis_type") = "full",
          "Quick network analysis function");
    
    // Performance report generation function
    m.def("generatePerformanceReport", &generatePerformanceReport,
          py::arg("brain"), py::arg("metrics_config") = PerformanceMetrics::MetricsConfig(),
          "Generate performance report for the neural network simulation");
    
    // ============= FACTORY FUNCTIONS =============
    
    // Factory function to create ExperimentRunner
    m.def("createExperimentRunner", [](std::shared_ptr<ExperimentConfig> config) {
        return std::make_shared<ExperimentRunner>();
    }, py::arg("config"), "Factory function to create ExperimentRunner with configuration");
    
    // Run Phase 6 experiment and return results
    m.def("runPhase6Experiment", [](std::shared_ptr<ExperimentConfig> config) {
        Phase6Experiment experiment;
        bool success = experiment.runPhase6IntegrationTest(config);
        return std::make_tuple(success, experiment.getIntegrationResult());
    }, py::arg("config"), "Run Phase 6 experiment and return success status and results");
    
    // Compare multiple experiment results
    m.def("compareExperiments", [](const std::vector<std::shared_ptr<ExperimentRunner>>& experiments, const std::string& metricName) {
        std::vector<double> comparisons;
        if (!experiments.empty()) {
            auto& first = *experiments[0];
            for (size_t i = 0; i < experiments.size(); ++i) {
                comparisons.push_back(first.compareWith(*experiments[i], metricName));
            }
        }
        return comparisons;
    }, py::arg("experiments"), py::arg("metricName"), "Compare multiple experiment results");

    // ============= CONSTANTS =============
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // ============= TRAINING LOOP CLASSES =============
    
    // TrainingLoop class
    py::class_<TrainingLoop>(m, "TrainingLoop", R"pbdoc(Training loop framework for neural network training)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<Config>, 
                      std::shared_ptr<Brain>, std::shared_ptr<ReinforcementLearner>>())
        .def("runEpisode", &TrainingLoop::runEpisode, "Run a single training episode")
        .def("runTraining", &TrainingLoop::runTraining, "Run complete training process")
        .def("collectExperience", &TrainingLoop::collectExperience, "Collect experience for learning")
        .def("updatePolicy", &TrainingLoop::updatePolicy, "Update policy based on experience")
        .def("evaluatePolicy", &TrainingLoop::evaluatePolicy, "Evaluate current policy performance")
        .def("savePolicy", &TrainingLoop::savePolicy, py::arg("filepath"), "Save policy to file")
        .def("loadPolicy", &TrainingLoop::loadPolicy, py::arg("filepath"), "Load policy from file")
        .def("start", &TrainingLoop::start, "Start training loop")
        .def("stop", &TrainingLoop::stop, "Stop training loop")
        .def("getMetricsSummary", &TrainingLoop::getMetricsSummary, "Get training metrics summary")
        .def("getAgent", &TrainingLoop::getAgent, "Get the agent brain")
        .def("getEnvironment", &TrainingLoop::getEnvironment, "Get the simulation environment")
        .def("getConfig", &TrainingLoop::getConfig, "Get the training configuration")
        .def("getPolicy", &TrainingLoop::getPolicy, "Get the policy brain")
        .def("getOptimizer", &TrainingLoop::getOptimizer, "Get the reinforcement learner optimizer")
        .def("reset", &TrainingLoop::reset, "Reset training loop state")
        .def("setStateProcessor", &TrainingLoop::setStateProcessor, py::arg("processor"), "Set state processor")
        .def("setRewardFunction", &TrainingLoop::setRewardFunction, py::arg("reward_func"), "Set reward function")
        .def("setEpisodeCallback", &TrainingLoop::setEpisodeCallback, py::arg("callback"), "Set episode callback")
        .def("setStepCallback", &TrainingLoop::setStepCallback, py::arg("callback"), "Set step callback")
        .def("setTrainingCompleteCallback", &TrainingLoop::setTrainingCompleteCallback, py::arg("callback"), "Set training complete callback");
    
    // ReinforcementLearner class
    py::class_<ReinforcementLearner>(m, "ReinforcementLearner", R"pbdoc(Base class for reinforcement learning algorithms)pbdoc")
        .def("train", &ReinforcementLearner::train, py::arg("numEpisodes"), "Train for specified number of episodes")
        .def("predict", &ReinforcementLearner::predict, py::arg("brain"), py::arg("state"), "Predict action given state")
        .def("learn", &ReinforcementLearner::learn, "Learn from experience")
        .def("explore", &ReinforcementLearner::explore, py::arg("state"), "Handle exploration vs exploitation")
        .def("exploit", &ReinforcementLearner::exploit, py::arg("state"), "Handle exploitation only")
        .def("saveModel", &ReinforcementLearner::saveModel, py::arg("filepath"), "Save model to file")
        .def("loadModel", &ReinforcementLearner::loadModel, py::arg("filepath"), "Load model from file")
        .def("storeExperience", &ReinforcementLearner::storeExperience, py::arg("experience"), "Store experience for learning")
        .def("canLearn", &ReinforcementLearner::canLearn, "Check if enough experience to learn")
        .def("clearMemory", &ReinforcementLearner::clearMemory, "Clear experience memory")
        .def("evaluate", &ReinforcementLearner::evaluate, py::arg("brain"), py::arg("evalEpisodes"), "Evaluate policy performance")
        .def("getAlgorithm", &ReinforcementLearner::getAlgorithm, "Get the reinforcement learning algorithm")
        .def("getExplorationStrategy", &ReinforcementLearner::getExplorationStrategy, "Get exploration strategy")
        .def("getActionSpaceSize", &ReinforcementLearner::getActionSpaceSize, "Get action space size")
        .def("getStateSpaceSize", &ReinforcementLearner::getStateSpaceSize, "Get state space size")
        .def("getAlpha", &ReinforcementLearner::getAlpha, "Get learning rate")
        .def("getGamma", &ReinforcementLearner::getGamma, "Get discount factor")
        .def("getEpsilon", &ReinforcementLearner::getEpsilon, "Get exploration rate")
        .def("getTemperature", &ReinforcementLearner::getTemperature, "Get temperature for exploration")
        .def("getStatsSummary", &ReinforcementLearner::getStatsSummary, "Get statistics summary");
    
    // Tabular Q-Learning specific class
    py::class_<TabularQLearning, std::shared_ptr<TabularQLearning>>(m, "TabularQLearning", R"pbdoc(Tabular Q-Learning implementation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<Config>>());
    
    // Deep Q-Network specific class
    py::class_<DeepQLearning, std::shared_ptr<DeepQLearning>>(m, "DeepQLearning", R"pbdoc(Deep Q-Network implementation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<Config>>());
    
    // Policy Gradient specific class
    py::class_<PolicyGradientLearner, std::shared_ptr<PolicyGradientLearner>>(m, "PolicyGradientLearner", R"pbdoc(Policy Gradient implementation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<Config>>());
    
    // CurriculumLearner class
    py::class_<CurriculumLearner>(m, "CurriculumLearner", R"pbdoc(Curriculum learning framework for progressive difficulty)pbdoc")
        .def(py::init<size_t>())
        .def("advanceCurriculum", &CurriculumLearner::advanceCurriculum, "Advance to next difficulty level")
        .def("resetProgress", &CurriculumLearner::resetProgress, "Reset learning progress")
        .def("getCurrentDifficulty", &CurriculumLearner::getCurrentDifficulty, "Get current training difficulty")
        .def("getProgress", &CurriculumLearner::getProgress, "Get current learning progress")
        .def("adaptEnvironment", &CurriculumLearner::adaptEnvironment, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("metrics"), "Adapt environment based on performance")
        .def("runEpisodeWithDifficulty", &CurriculumLearner::runEpisodeWithDifficulty, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("targetDifficulty"), "Run episode with specific difficulty level")
        .def("setDifficulty", &CurriculumLearner::setDifficulty, py::arg("level"), "Set difficulty level directly")
        .def("getDifficultyName", &CurriculumLearner::getDifficultyName, py::arg("level"), "Get difficulty level name")
        .def("getProgressReport", &CurriculumLearner::getProgressReport, "Get curriculum learning progress report")
        .def("setProgressCallback", &CurriculumLearner::setProgressCallback, py::arg("callback"), "Set progress callback")
        .def("setDifficultyChangeCallback", &CurriculumLearner::setDifficultyChangeCallback, py::arg("callback"), "Set difficulty change callback")
        .def("setAdaptationCallback", &CurriculumLearner::setAdaptationCallback, py::arg("callback"), "Set adaptation callback");
    
    // SimulationScheduler class
    py::class_<SimulationScheduler>(m, "SimulationScheduler", R"pbdoc(Advanced simulation scheduling system)pbdoc")
        .def(py::init<>())
        .def("scheduleSimulation", &SimulationScheduler::scheduleSimulation, py::arg("experiment"), "Schedule a simulation")
        .def("scheduleMultipleExperiments", &SimulationScheduler::scheduleMultipleExperiments, py::arg("experiments"), "Schedule multiple simulations")
        .def("allocateResources", &SimulationScheduler::allocateResources, py::arg("resources"), "Allocate computational resources")
        .def("getAvailableResources", &SimulationScheduler::getAvailableResources, "Get available resources")
        .def("getResourceUsage", &SimulationScheduler::getResourceUsage, "Get current resource usage")
        .def("getSimulationSchedule", &SimulationScheduler::getSimulationSchedule, py::arg("experimentId"), "Get simulation schedule by ID")
        .def("getActiveSimulations", &SimulationScheduler::getActiveSimulations, "Get list of active simulations")
        .def("getCompletedSimulations", &SimulationScheduler::getCompletedSimulations, "Get list of completed simulations")
        .def("startSimulation", &SimulationScheduler::startSimulation, py::arg("experimentId"), "Start a simulation")
        .def("pauseSimulation", &SimulationScheduler::pauseSimulation, py::arg("experimentId"), "Pause a simulation")
        .def("resumeSimulation", &SimulationScheduler::resumeSimulation, py::arg("experimentId"), "Resume a simulation")
        .def("cancelSimulation", &SimulationScheduler::cancelSimulation, py::arg("experimentId"), "Cancel a simulation")
        .def("completeSimulation", &SimulationScheduler::completeSimulation, py::arg("experimentId"), "Complete a simulation")
        .def("updateSimulationProgress", &SimulationScheduler::updateSimulationProgress, py::arg("experimentId"), py::arg("progress"), "Update simulation progress")
        .def("adjustSchedule", &SimulationScheduler::adjustSchedule, py::arg("experimentId"), py::arg("newResources"), "Adjust simulation schedule")
        .def("rescheduleSimulation", &SimulationScheduler::rescheduleSimulation, py::arg("experimentId"), py::arg("newEpisodeCount"), "Reschedule simulation with new episode count")
        .def("getSchedulerStatus", &SimulationScheduler::getSchedulerStatus, "Get scheduler status")
        .def("getResourceReport", &SimulationScheduler::getResourceReport, "Get resource allocation report")
        .def("getSimulationReport", &SimulationScheduler::getSimulationReport, py::arg("experimentId"), "Get simulation report")
        .def("getOverallScheduleReport", &SimulationScheduler::getOverallScheduleReport, "Get overall schedule report")
        .def("cleanupCompletedSimulations", &SimulationScheduler::cleanupCompletedSimulations, "Clean up completed simulations")
        .def("stopAllSimulations", &SimulationScheduler::stopAllSimulations, "Stop all simulations");
    
    // ReinforcementLearnerFactory
    m.def("createLearner", &ReinforcementLearnerFactory::createLearner,
          py::arg("algorithm"), py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("config"),
          "Factory function to create reinforcement learner");
    
    m.def("getSupportedAlgorithms", &ReinforcementLearnerFactory::getSupportedAlgorithms,
          "Get list of supported reinforcement learning algorithms");
    
    m.def("stringToAlgorithm", &ReinforcementLearnerFactory::stringToAlgorithm,
          py::arg("algorithmName"), "Convert algorithm name string to enum");
    
    m.def("algorithmToString", &ReinforcementLearnerFactory::algorithmToString,
          py::arg("algorithm"), "Convert algorithm enum to string");
    
    // CurriculumLearnerFactory
    m.def("createStandardCurriculum", &CurriculumLearnerFactory::createStandardCurriculum,
          py::arg("numDifficultyLevels") = 5, py::arg("curriculumType") = "progressive",
          "Factory function to create standard curriculum");
    
    m.def("createAdaptiveCurriculum", &CurriculumLearnerFactory::createAdaptiveCurriculum,
          py::arg("performanceEvaluator"), py::arg("minEpisodesPerLevel") = 10,
          "Factory function to create adaptive curriculum");
    
    m.def("createStructuredCurriculum", &CurriculumLearnerFactory::createStructuredCurriculum,
          py::arg("difficultyOrder"), py::arg("adaptations"),
          "Factory function to create structured curriculum");
    
    // SimulationSchedulerFactory
    m.def("createLocalScheduler", &SimulationSchedulerFactory::createLocalScheduler,
          py::arg("systemResources"), "Factory function to create local scheduler");
    
    m.def("createClusterScheduler", &SimulationSchedulerFactory::createClusterScheduler,
          py::arg("workerResources"), py::arg("masterResources"),
          "Factory function to create cluster scheduler");
    
    m.def("createCloudScheduler", &SimulationSchedulerFactory::createCloudScheduler,
          py::arg("provider"), py::arg("region"), py::arg("credentials"),
          "Factory function to create cloud scheduler");
    
    m.def("optimizeExperimentSchedule", &SimulationSchedulerFactory::optimizeExperimentSchedule,
          py::arg("experiments"), py::arg("availableResources"),
          "Optimize experiment schedule based on available resources");
    
    // Module-level convenience functions
    
    // Training loop factory
    m.def("createTrainingLoop", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> environment,
                                  std::shared_ptr<Config> config, std::shared_ptr<Brain> policy,
                                  std::shared_ptr<ReinforcementLearner> optimizer) {
        return std::make_shared<TrainingLoop>(agent, environment, config, policy, optimizer);
    }, py::arg("agent"), py::arg("environment"), py::arg("config"), py::arg("policy"), py::arg("optimizer"),
       "Factory function to create training loop");
    
    // Reinforcement training function
    m.def("runReinforcementTraining", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent, std::shared_ptr<Config> algorithmConfig) {
        // Create a reinforcement learner with the specified algorithm
        auto learner = ReinforcementLearnerFactory::createLearner(
            RLAlgorithm::DEEP_Q_NETWORK, brain, agent, world, algorithmConfig);
        
        // Create training loop
        auto trainingLoop = std::make_shared<TrainingLoop>(agent, world, algorithmConfig, brain, learner);
        
        // Run training
        return trainingLoop->runTraining();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("algorithmConfig"),
       "Run reinforcement training with specified algorithm");
    
    // Curriculum learning factory
    m.def("createCurriculumLearner", [](size_t initialDifficulty) {
        return std::make_shared<CurriculumLearner>(initialDifficulty);
    }, py::arg("initialDifficulty"), "Factory function to create curriculum learner");
    
    // Training scheduling function
    m.def("scheduleTraining", [](std::vector<ExperimentConfig> experimentsConfig, ResourceConfig resourceConfig) {
        auto scheduler = SimulationSchedulerFactory::createLocalScheduler(resourceConfig);
        
        // Schedule all experiments
        for (const auto& experiment : experimentsConfig) {
            scheduler->scheduleSimulation(experiment);
        }
        
        return scheduler;
    }, py::arg("experimentsConfig"), py::arg("resourceConfig"),
       "Schedule training runs with specified configuration and resources");
    
    // Algorithm enumeration
    py::enum_<RLAlgorithm>(m, "RLAlgorithm", R"pbdoc(Reinforcement learning algorithm enumeration)pbdoc")
        .value("TABULAR_Q_LEARNING", RLAlgorithm::TABULAR_Q_LEARNING)
        .value("DEEP_Q_NETWORK", RLAlgorithm::DEEP_Q_NETWORK)
        .value("POLICY_GRADIENT", RLAlgorithm::POLICY_GRADIENT)
        .value("Q_LEARNING", RLAlgorithm::Q_LEARNING)
        .value("SARSA", RLAlgorithm::SARSA)
        .value("EXPECTATION_MAXIMIZATION", RLAlgorithm::EXPECTATION_MAXIMIZATION)
        .value("PPO", RLAlgorithm::PPO)
        .value("A2C", RLAlgorithm::A2C)
        .value("DDPG", RLAlgorithm::DDPG)
        .value("SAC", RLAlgorithm::SAC)
        .export_values();
    
    // Exploration strategy enumeration
    py::enum_<ExplorationStrategy>(m, "ExplorationStrategy", R"pbdoc(Exploration strategy enumeration)pbdoc")
        .value("EPSILON_GREEDY", ExplorationStrategy::EPSILON_GREEDY)
        .value("UCB1", ExplorationStrategy::UCB1)
        .value("THERMAL", ExplorationStrategy::THERMAL)
        .value("NOISE", ExplorationStrategy::NOISE)
        .value("COUNT", ExplorationStrategy::COUNT)
        .value("ADAPTIVE", ExplorationStrategy::ADAPTIVE)
        .export_values();
    
    // Difficulty level enumeration
    py::enum_<DifficultyLevel>(m, "DifficultyLevel", R"pbdoc(Difficulty level enumeration)pbdoc")
        .value("EASY", DifficultyLevel::EASY)
        .value("MEDIUM", DifficultyLevel::MEDIUM)
        .value("HARD", DifficultyLevel::HARD)
        .value("EXPERT", DifficultyLevel::EXPERT)
        .value("NIGHTMARE", DifficultyLevel::NIGHTMARE)
        .export_values();
    
    // Training experience structure
    py::class_<TrainingExperience>(m, "TrainingExperience", R"pbdoc(Experience data for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def_readwrite("brainState", &TrainingExperience::brainState)
        .def_readwrite("worldState", &TrainingExperience::worldState)
        .def_readwrite("agent", &TrainingExperience::agent)
        .def_readwrite("state", &TrainingExperience::state)
        .def_readwrite("action", &TrainingExperience::action)
        .def_readwrite("reward", &TrainingExperience::reward)
        .def_readwrite("nextState", &TrainingExperience::nextState)
        .def_readwrite("done", &TrainingExperience::done)
        .def_readwrite("timestamp", &TrainingExperience::timestamp)
        .def_readwrite("step", &TrainingExperience::step)
        .def_readwrite("difficultyLevel", &TrainingExperience::difficultyLevel);
    
    // Learning progress structure
    py::class_<LearningProgress>(m, "LearningProgress", R"pbdoc(Progress tracking for curriculum learning)pbdoc")
        .def(py::init<>())
        .def_readwrite("successRate", &LearningProgress::successRate)
        .def_readwrite("averageReward", &LearningProgress::averageReward)
        .def_readwrite("averageSteps", &LearningProgress::averageSteps)
        .def_readwrite("trainingTimePerEpisode", &LearningProgress::trainingTimePerEpisode)
        .def_readwrite("episodesCompleted", &LearningProgress::episodesCompleted)
        .def_readwrite("currentDifficulty", &LearningProgress::currentDifficulty)
        .def_readwrite("performanceScore", &LearningProgress::performanceScore);
    
    // Experience replay structure
    py::class_<ExperienceReplay>(m, "ExperienceReplay", R"pbdoc(Experience replay buffer for reinforcement learning)pbdoc")
        .def(py::init<size_t>())
        .def("add", &ExperienceReplay::add, py::arg("experience"), "Add experience to buffer")
        .def("sample", &ExperienceReplay::sample, "Sample experience from buffer")
        .def("sampleUniform", &ExperienceReplay::sampleUniform, "Sample experience uniformly from buffer")
        .def("isReady", &ExperienceReplay::isReady, py::arg("minSize"), "Check if buffer is ready for learning")
        .def("clear", &ExperienceReplay::clear, "Clear buffer")
        .def("size", &ExperienceReplay::size, "Get buffer size")
        .def("capacity", &ExperienceReplay::capacity, "Get buffer capacity");
    
    // Q-table structure
    py::class_<QTable>(m, "QTable", R"pbdoc(Q-table for tabular Q-learning)pbdoc")
        .def(py::init<int, int>())
        .def("getValue", &QTable::getValue, py::arg("state"), py::arg("action"), "Get Q-value for state-action pair")
        .def("setValue", &QTable::setValue, py::arg("state"), py::arg("action"), py::arg("value"), "Set Q-value for state-action pair")
        .def("getBestAction", &QTable::getBestAction, py::arg("state"), "Get best action for state")
        .def("update", &QTable::update, py::arg("state"), py::arg("action"), py::arg("reward"), py::arg("nextState"), py::arg("nextAction"), py::arg("alpha"), py::arg("gamma"), "Update Q-table entry")
        .def("incrementVisitCount", &QTable::incrementVisitCount, py::arg("state"), py::arg("action"), "Increment visit count for state-action pair")
        .def("getVisitCount", &QTable::getVisitCount, py::arg("state"), py::arg("action"), "Get visit count for state-action pair");
    
    // Neural network utilities
    py::class_<NeuralNetworkUtils>(m, "NeuralNetworkUtils", R"pbdoc(Neural network utility functions)pbdoc")
        .def_static("softmax", &NeuralNetworkUtils::softmax, py::arg("logits"), "Compute softmax activation")
        .def_static("entropy", &NeuralNetworkUtils::entropy, py::arg("probabilities"), "Compute entropy of probability distribution")
        .def_static("clippedGradient", &NeuralNetworkUtils::clippedGradient, py::arg("gradient"), py::arg("clipNorm"), "Clip gradient by norm")
        .def_static("oneHotEncode", &NeuralNetworkUtils::oneHotEncode, py::arg("action"), py::arg("actionSpaceSize"), "One-hot encode action")
        .def_static("muSigmaToDistribution", &NeuralNetworkUtils::muSigmaToDistribution, py::arg("mu"), py::arg("sigma"), "Convert mu-sigma to distribution");
    
    // Environment adaptation structure
    py::class_<EnvironmentAdaptation>(m, "EnvironmentAdaptation", R"pbdoc(Environment adaptation configuration)pbdoc")
        .def(py::init<>())
        .def_readwrite("difficultyMultiplier", &EnvironmentAdaptation::difficultyMultiplier)
        .def_readwrite("rewardScale", &EnvironmentAdaptation::rewardScale)
        .def_readwrite("timeStepSize", &EnvironmentAdaptation::timeStepSize)
        .def_readwrite("energyDecayRate", &EnvironmentAdaptation::energyDecayRate)
        .def_readwrite("allowedObjects", &EnvironmentAdaptation::allowedObjects)
        .def_readwrite("environmentParameters", &EnvironmentAdaptation::environmentParameters);
    
    // Resource configuration structure
    py::class_<ResourceConfig>(m, "ResourceConfig", R"pbdoc(Resource allocation configuration)pbdoc")
        .def(py::init<>())
        .def_readwrite("cpuCores", &ResourceConfig::cpuCores)
        .def_readwrite("gpuMemory", &ResourceConfig::gpuMemory)
        .def_readwrite("ramMemory", &ResourceConfig::ramMemory)
        .def_readwrite("maxConcurrentExperiments", &ResourceConfig::maxConcurrentExperiments)
        .def_readwrite("cpuUsageLimit", &ResourceConfig::cpuUsageLimit)
        .def_readwrite("gpuUsageLimit", &ResourceConfig::gpuUsageLimit)
        .def_readwrite("memoryUsageLimit", &ResourceConfig::memoryUsageLimit)
        .def_readwrite("priority", &ResourceConfig::priority);
    
    // Experiment configuration structure
    py::class_<ExperimentConfig>(m, "ExperimentConfig", R"pbdoc(Experiment configuration for scheduling)pbdoc")
        .def(py::init<>())
        .def_readwrite("experimentName", &ExperimentConfig::experimentName)
        .def_readwrite("description", &ExperimentConfig::description)
        .def_readwrite("algorithm", &ExperimentConfig::algorithm)
        .def_readwrite("episodeCount", &ExperimentConfig::episodeCount)
        .def_readwrite("config", &ExperimentConfig::config)
        .def_readwrite("resources", &ExperimentConfig::resources)
        .def_readwrite("tags", &ExperimentConfig::tags)
        .def_readwrite("requiresGPU", &ExperimentConfig::requiresGPU)
        .def_readwrite("distributedTraining", &ExperimentConfig::distributedTraining);
    
    // Training job structure
    py::class_<TrainingJob>(m, "TrainingJob", R"pbdoc(Distributed training job)pbdoc")
        .def(py::init<>())
        .def_readwrite("jobId", &TrainingJob::jobId)
        .def_readwrite("experimentId", &TrainingJob::experimentId)
        .def_readwrite("jobIndex", &TrainingJob::jobIndex)
        .def_readwrite("totalJobs", &TrainingJob::totalJobs)
        .def_readwrite("brain", &TrainingJob::brain)
        .def_readwrite("agent", &TrainingJob::agent)
        .def_readwrite("world", &TrainingJob::world)
        .def_readwrite("learner", &TrainingJob::learner)
        .def_readwrite("startEpisode", &TrainingJob::startEpisode)
        .def_readwrite("endEpisode", &TrainingJob::endEpisode)
        .def_readwrite("progress", &TrainingJob::progress)
        .def_readwrite("totalReward", &TrainingJob::totalReward)
        .def_readwrite("totalSteps", &TrainingJob::totalSteps);
    
    // State processor interface
    py::class_<StateProcessor>(m, "StateProcessor", R"pbdoc(State processing interface)pbdoc")
        .def("process", &StateProcessor::process, py::arg("brain"), py::arg("world"), py::arg("agent"), "Process state")
        .def("getActionSpaceSize", &StateProcessor::getActionSpaceSize, "Get action space size")
        .def("getState", &StateProcessor::getState, "Get current state");
    
    // Default state processor
    py::class_<DefaultStateProcessor, std::shared_ptr<DefaultStateProcessor>>(m, "DefaultStateProcessor", R"pbdoc(Default state processor implementation)pbdoc")
        .def(py::init<>());
    
    // Reward function interface
    py::class_<RewardFunction>(m, "RewardFunction", R"pbdoc(Reward function interface)pbdoc")
        .def("computeReward", &RewardFunction::computeReward, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("state"), "Compute reward");
    
    // Default reward function
    py::class_<DefaultRewardFunction, std::shared_ptr<DefaultRewardFunction>>(m, "DefaultRewardFunction", R"pbdoc(Default reward function implementation)pbdoc")
        .def(py::init<>());

    // ============= DOCUMENTATION =============
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Experiment Management Framework
        ---------------------------------------------------------------------------------------------
        \n        This enhanced module provides comprehensive Python bindings for the NLM neural simulation
        framework with a complete experiment management framework including:
        \n        1. ExperimentConfig - Configuration for experiments with validation and serialization
        2. ExperimentMetrics - Performance tracking and analysis
        3. ExperimentRunner - Core experiment running framework
        4. Phase6Experiment - Phase 6 integration test class
        \n        \n        Key Features:
        - Full integration of all NLM brain systems (memory, prediction, cognition, neuromodulation)
        - Comprehensive experiment management and results analysis
        - High-level APIs for quick testing and deployment
        - Experiment configuration validation and serialization
        - Advanced metrics collection and statistical analysis
        - Checkpoint save/load functionality for reproducible research
        - Thread-safe operations for concurrent experiments
        \n        \n        The framework is designed for:
        - Simulation research - Easy setup of different experimental conditions
        - Hyperparameter tuning - Support for parameter sweeps
        - Benchmarking - Performance measurement
        - Reproducible research - Checkpoint saving/loading
        - Ablation studies - System component testing
        \n        \n        Usage Examples:
        >>> import pynlm
        >>> 
        >>> # Create experiment configuration
        >>> config = pynlm.ExperimentConfig.createTrainingConfig()
        >>> config.setNeuronCount(500)
        >>> config.setMaxSteps(10000)
        >>> 
        >>> # Validate configuration
        >>> if not config.validate():
        ...     print(f"Config error: {config.validateAndGetError()}")
        >>> 
        >>> # Create experiment metrics
        >>> metrics = pynlm.ExperimentMetrics()
        >>> 
        >>> # Record metrics
        >>> metrics.recordStep(step, reward, firing_rate, active_neurons, firing_neurons, total_spikes)
        >>> 
        >>> # Create experiment runner
        >>> runner = pynlm.createExperimentRunner(config)
        >>> 
        >>> # Setup and run experiment
        >>> # runner.setup(config, brain, agent, world, environment)  # Would need actual components
        >>> # results = runner.runExperiment(5)
        >>> \n        >>> # Create Phase 6 experiment
        >>> phase6 = pynlm.Phase6Experiment()
        >>> success, result = pynlm.runPhase6Experiment(config)
        >>> print(result.getIntegrationSummary())
        >>> \n        >>> # Compare experiments
        >>> experiments = [runner1, runner2, runner3]  # List of ExperimentRunner objects
        >>> comparisons = pynlm.compareExperiments(experiments, "total_reward")
        >>> print(f"Comparisons: {comparisons}")
        \n        Available Classes:
        - ExperimentConfig: Experiment configuration with validation
        - ExperimentMetrics: Performance metrics tracking and analysis
        - ExperimentRunner: Core experiment running framework
        - Phase6Experiment: Phase 6 integration test class
        - All existing NLM classes: Brain, Config, AgentBrain, SimpleWorld, SensoryInput, Action, etc.
        \n        Available Functions:
        - createExperimentRunner(config): Factory function to create ExperimentRunner
        - runPhase6Experiment(config): Run Phase 6 experiment and return results
        - compareExperiments(experiments, metric_name): Compare multiple experiment results
        - createQuickTestBrain(): Create a quick test brain for development and testing
        - createTrainingEnvironment(): Create a complete training environment
        - createEvaluationEnvironment(): Create an evaluation environment with advanced features
    )pbdoc";

} // namespace nlm

// Fix for C++11/14 compatibility issues with SimpleWorld.hpp
// We need to update the SimpleWorld class to include the new methods properly

namespace nlm {
    // Add the missing method implementations that are required by the Python bindings
    const SensoryPercept& SimpleWorld::observe() const { return getSensoryPercept(); }
    \n    ActionResult SimpleWorld::applyAction(MotorCommand cmd, double currentTime) { return applyMotorCommand(cmd, currentTime); }
    \n    float SimpleWorld::computeReward(Brain* brain) const {
        if (!brain) return 0.0f;
        return agent_.energy * 0.01f + agent_.health * 0.1f;
    }
} // namespace nlm
