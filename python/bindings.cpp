#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all NLM components
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/memory/SemanticMemory.hpp"
#include "../src/memory/ProceduralMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/Observation.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/Phase4Experiment.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/ParallelProcessing.hpp"
#include "../src/performance/SIMDVectorization.hpp"
#include "../src/performance/OptimizedBrain.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Custom NLM Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class IntegrationError : public NLMError {
public:
    explicit IntegrationError(const std::string& msg) : NLMError("Integration error: " + msg) {}
};

// Helper function for parameter validation
template<typename T>
T validateParameter(const T& value, const std::string& name, 
                   const T& min, const T& max, const std::string& context) {
    if (value < min || value > max) {
        throw ValidationError(
            "Parameter " + name + " = " + std::to_string(value) + 
            " out of range [" + std::to_string(min) + ", " + std::to_string(max) + "] " +
            "in " + context
        );
    }
    return value;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Advanced Python API
        ====================================================

        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This API provides complete access to all brain systems including memory,
        neuromodulation, cognition, development, and prediction mechanisms.
        
        Features:
        - Full integration with all NLM brain systems
        - Memory safety with smart pointer management
        - Comprehensive error handling with custom exceptions
        - Advanced batch operations and processing capabilities
        - Extensive documentation and examples
        - Power user features for advanced simulations
        
        Usage example:
        ```python
        import pynlm
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create environment and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(1000):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Simulation complete. Firing neurons: {brain.getFiringNeuronCount()}")
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<IntegrationError>(m, "IntegrationError", PyExc_RuntimeError);

    // ====================================================================
    // ID TYPES AND ENUMS
    // ====================================================================

    // Neuron ID type
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

    // Synapse ID type
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

    // Region ID type
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

    // Population ID type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Neuron types enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse types enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stages enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing states enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action types enum
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

    // Motor commands enum
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

    // World object types enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ====================================================================
    // CORE SYSTEMS
    // ====================================================================

    // Configuration system
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        Manages all brain parameters including neuron count, connectivity,
        plasticity rules, and simulation settings. Configuration can be
        loaded from files, command line arguments, or programmatic setup.
    )pbdoc")
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

    // ====================================================================
    // MEMORY SYSTEMS
    // ====================================================================

    // Working memory - neural implementation
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(
        Neural working memory using persistent activity and recurrent connections
        
        Implements working memory through:
        - Persistent neural activity (reverberating activity)
        - Recurrent synaptic connections for maintenance
        - Activity-dependent short-term plasticity
        - Neural competition for selective retention
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize working memory with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory as neural activity pattern")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate for memory traces")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    // Episodic memory - experience storage
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(
        Neural episodic memory with replay and consolidation
        
        Stores experiences with:
        - Timestamped episode storage
        - Replay during rest/sleep cycles
        - Memory consolidation based on relevance
        - Integration with prediction system
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize episodic memory with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store episode to memory")
        .def("retrieveEpisode", &NeuralEpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of episodes stored")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"),
             "Consolidate episodes to long-term storage")
        .def("getMaxEpisodes", &NeuralEpisodicMemory::getMaxEpisodes,
             "Get maximum episode storage capacity")
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("max"),
             "Set maximum episode storage capacity");

    // Associative memory - pattern associations
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(
        Neural associative memory for pattern completion and recall
        
        Implements associations through:
        - Hebbian-style synaptic strengthening
        - Pattern completion from partial cues
        - Bidirectional pattern relationships
        - Integration with prediction errors
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize associative memory with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f, py::arg("bidirectional") = true,
             "Create association between two patterns")
        .def("retrieveAssociation", &NeuralAssociativeMemory::retrieveAssociation,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Retrieve associated pattern given one pattern")
        .def("getAssociations", &NeuralAssociativeMemory::getAssociations, py::arg("pattern"),
             "Get all associations for a pattern")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association strength between patterns")
        .def("hasAssociations", &NeuralAssociativeMemory::hasAssociations, py::arg("pattern"),
             "Check if pattern has associations")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             "Get association strength between patterns")
        .def("completePattern", &NeuralAssociativeMemory::completePattern,
             py::arg("partialPattern"), py::arg("threshold") = 0.5f,
             "Complete partial pattern using associations")
        .def("consolidateAssociations", &NeuralAssociativeMemory::consolidateAssociations, py::arg("threshold"),
             "Consolidate associations to long-term storage")
        .def("learnFromPredictionError", &NeuralAssociativeMemory::learnFromPredictionError,
             py::arg("predicted"), py::arg("actual"), py::arg("error"),
             "Learn from prediction error")
        .def("applyNeuromodulation", &NeuralAssociativeMemory::applyNeuromodulation, py::arg("modulator"),
             "Apply neuromodulation effects")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear all associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations")
        .def("getCapacity", &NeuralAssociativeMemory::getCapacity,
             "Get associative memory capacity")
        .def("setCapacity", &NeuralAssociativeMemory::setCapacity, py::arg("capacity"),
             "Set associative memory capacity")
        .def("getLearningRate", &NeuralAssociativeMemory::getLearningRate,
             "Get learning rate")
        .def("setLearningRate", &NeuralAssociativeMemory::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("getConsolidationThreshold", &NeuralAssociativeMemory::getConsolidationThreshold,
             "Get consolidation threshold")
        .def("setConsolidationThreshold", &NeuralAssociativeMemory::setConsolidationThreshold, py::arg("threshold"),
             "Set consolidation threshold")
        .def("getMemoryActivity", &NeuralAssociativeMemory::getMemoryActivity,
             "Get memory activity level");

    // Semantic memory - knowledge representation
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(
        Semantic memory for stored facts and knowledge
        
        Stores declarative knowledge through:
        - Key-value fact storage
        - Fact retrieval by key
        - Memory consolidation
        - Integration with episodic memory
    )pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store a fact (key-value pair)")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve fact by key")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if fact exists")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts as key-value pairs")
        .def("clear", &SemanticMemory::clear,
             "Clear all facts");

    // Procedural memory - skills and habits
    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(
        Procedural memory for skills and habits
        
        Stores learned behaviors through:
        - Skill learning from neural patterns
        - Proficiency tracking
        - Pattern recognition for skill execution
        - Integration with motor system
    )pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill,
             py::arg("name"), py::arg("pattern"),
             "Learn a new skill from neural pattern")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             "Retrieve skill by name")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all learned skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency,
             py::arg("name"), py::arg("delta"),
             "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear,
             "Clear all skills");

    // ====================================================================
    // COGNITION SYSTEMS
    // ====================================================================

    // Attentional selection
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(
        Neural attentional selection through competitive dynamics
        
        Implements attention through:
        - Competition between neural populations
        - Salience-based selection
        - Recurrent amplification of winners
        - Inhibitory surround suppression
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize attentional selection with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competition among neural populations")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to specific brain region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get currently attended regions")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attentional dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get current attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attentional bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attentional selection");

    // Neural planner
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(
        Neural planning using predictive coding and action evaluation
        
        Implements planning through:
        - Predictive action-consequence evaluation
        - Multi-step action sequence planning
        - Confidence-based action selection
        - Adaptation based on prediction confidence
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize neural planner with brain reference")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plan based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current planning goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current planning goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent plan was successful");

    // Concept formation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(
        Concept formation through pattern clustering and prototype discovery
        
        Discovers concepts through:
        - Pattern similarity clustering
        - Prototype formation through averaging
        - Instance tracking for stability measurement
        - Category discovery based on shared properties
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize concept formation with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present new experience for concept learning")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get concept that best matches a pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all discovered concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             "Get specific concept by ID")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype pattern")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Measure concept stability")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of discovered concepts")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity,
             py::arg("patternA"), py::arg("patternB"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Find concept for pattern")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             "Get generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set stability window");

    // ====================================================================
    // DEVELOPMENT SYSTEM
    // ====================================================================

    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(
        Developmental system for brain maturation and stage transitions
        
        Manages development through:
        - Developmental stage transitions
        - Age-dependent plasticity modulation
        - Critical period management
        - Experience-dependent development
    )pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance to next developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get name of current developmental stage")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("currentStep"),
             "Update development system")
        .def("updateWithTime", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development system with time delta")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get progress through critical period")
        .def("getAge", &DevelopmentSystem::getAge,
             "Get current developmental age")
        .def("setAge", &DevelopmentSystem::setAge, py::arg("age"),
             "Set developmental age");

    // ====================================================================
    // NEUROMODULATION SYSTEMS
    // ====================================================================

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(
        Abstract base class for neuromodulatory signals
        
        Neuromodulators provide:
        - Global influence on neural excitability
        - Plasticity factor modulation
        - Learning signal modulation
        - Behavioral state regulation
    )pbdoc")
        .def("getName", &Neuromodulator::getName,
             "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current neuromodulator level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             "Set neuromodulator level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get plasticity factor for current level")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             "Update neuromodulator state");

    // Dopamine system
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(
        Dopamine system for reward and reinforcement learning
        
        Implements:
        - Reward signaling
        - Reward prediction error computation
        - Reinforcement learning modulation
        - Motivation and attention modulation
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &Dopamine::initialize, py::arg("brain"),
             "Initialize dopamine system with brain reference")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error")
        .def("getLevel", &Dopamine::getLevel,
             "Get current dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"),
             "Set dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity factor for current dopamine level")
        .def("update", &Dopamine::update, py::arg("dt"),
             "Update dopamine dynamics")
        .def("getBaseline", &Dopamine::getBaseline,
             "Get baseline dopamine level")
        .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"),
             "Set baseline dopamine level")
        .def("getPeakLevel", &Dopamine::getPeakLevel,
             "Get peak dopamine level")
        .def("setPeakLevel", &Dopamine::setPeakLevel, py::arg("peak"),
             "Set peak dopamine level");

    // Novelty system
    py::class_<Novelty>(m, "Novelty", R"pbdoc(
        Novelty detection and surprise computation
        
        Detects novel experiences through:
        - Pattern difference computation
        - Feature space comparison
        - Surprise signal generation
        - Adaptation to novelty patterns
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize novelty detection with brain reference")
        .def("getLevel", &Novelty::getLevel,
             "Get current novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set novelty level")
        .def("detectNovelty", &Novelty::detectNovelty,
             py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from pattern difference")
        .def("update", &Novelty::update, py::arg("dt"),
             "Update novelty (decay over time)")
        .def("getHistory", &Novelty::getHistory,
             "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory,
             "Clear novelty history")
        .def("getLastPattern", &Novelty::getLastPattern,
             "Get last observed pattern")
        .def("getThreshold", &Novelty::getThreshold,
             "Get novelty detection threshold")
        .def("setThreshold", &Novelty::setThreshold, py::arg("threshold"),
             "Set novelty detection threshold")
        .def("reset", &Novelty::reset,
             "Reset novelty detection");

    // Curiosity system
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(
        Curiosity drive for exploration motivation
        
        Combines novelty and prediction error to:
        - Drive exploration behavior
        - Balance exploitation vs exploration
        - Adapt based on environmental statistics
        - Regulate learning rate
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize curiosity system with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive")
        .def("update", &Curiosity::update,
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set weight for novelty influence")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set weight for prediction error influence")
        .def("reset", &Curiosity::reset,
             "Reset curiosity system");

    // Prediction error system
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(
        Prediction error signal for learning and adaptation
        
        Computes difference between predicted and actual values to:
        - Guide learning in predictive coding
        - Drive curiosity and exploration
        - Update internal models
        - Regulate neural plasticity
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize prediction error system with brain reference")
        .def("getError", &PredictionError::getError,
             "Get current prediction error value")
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionError::clearHistory,
             "Clear prediction error history")
        .def("getMagnitude", &PredictionError::getMagnitude,
             "Get error magnitude for neuromodulation")
        .def("reset", &PredictionError::reset,
             "Reset prediction error system");

    // Reward system
    py::class_<Reward>(m, "Reward", R"pbdoc(
        Reward computation and processing
        
        Processes rewards through:
        - Reward detection and filtering
        - Reward prediction error computation
        - Reinforcement learning integration
        - Motivational state regulation
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &Reward::initialize, py::arg("brain"),
             "Initialize reward system with brain reference")
        .def("signalReward", &Reward::signalReward, py::arg("reward"), py::arg("context"),
             "Signal reward reception with context")
        .def("getLevel", &Reward::getLevel,
             "Get current reward level")
        .def("getPredictionError", &Reward::getPredictionError,
             "Get reward prediction error")
        .def("update", &Reward::update, py::arg("dt"),
             "Update reward dynamics")
        .def("reset", &Reward::reset,
             "Reset reward system");

    // ====================================================================
    // PREDICTION SYSTEM
    // ====================================================================

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(
        Predictive coding framework for expectation generation
        
        Generates predictions through:
        - Sensory input prediction
        - Temporal sequence prediction
        - Expectation error computation
        - Model updating based on prediction errors
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"),
             "Initialize prediction system with brain reference")
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"),
             "Predict next state given current state")
        .def("computePredictionError", &PredictionSystem::computePredictionError,
             py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updateModel", &PredictionSystem::updateModel,
             py::arg("predictionError"),
             "Update predictive model based on prediction error")
        .def("getPredictionConfidence", &PredictionSystem::getPredictionConfidence, py::arg("state"),
             "Get prediction confidence for state")
        .def("getPredictionCount", &PredictionSystem::getPredictionCount,
             "Get number of predictions made")
        .def("clear", &PredictionSystem::clear,
             "Clear prediction system")
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"),
             "Set prediction learning rate")
        .def("getLearningRate", &PredictionSystem::getLearningRate,
             "Get prediction learning rate");

    // ====================================================================
    // SENSORY AND MOTOR SYSTEMS
    // ====================================================================

    // Neural prediction
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(
        Neural prediction of sensory consequences
        
        Predicts sensory consequences through:
        - Forward model implementation
        - Action-effect prediction
        - Sensory prediction error
        - Prediction error-based learning
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"),
             "Initialize neural prediction with brain reference")
        .def("predictConsequence", &NeuralPrediction::predictConsequence, py::arg("action"), py::arg("currentState"),
             "Predict sensory consequence of action")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence, py::arg("action"),
             "Get confidence in action prediction")
        .def("updateWithError", &NeuralPrediction::updateWithError,
             py::arg("predicted"), py::arg("actual"), py::arg("action"),
             "Update prediction based on error")
        .def("reset", &NeuralPrediction::reset,
             "Reset prediction system");

    // ====================================================================
    // AGENT SYSTEMS
    // ====================================================================

    // Agent brain interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        Connects neural processing to environment through:
        - Sensory input processing
        - Motor command decoding
        - Reward and neuromodulation application
        - Development and learning coordination
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>, py::arg("brain"),
             "Create agent brain with reference to neural brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent with world reference")
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
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
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
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // ====================================================================
    // FACTORY FUNCTIONS
    // ====================================================================

    // Configuration factory
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")

    // Brain factory
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    // Simple world factory
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    // Agent brain factory
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    // Convenience factory for common simulation setup
    m.def("createSimulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete simulation with default configuration");

    // ====================================================================
    // UTILITY FUNCTIONS
    // ====================================================================

    // Parameter validation utilities
    m.def("validateParameter", py::overload_cast<const double&, const std::string&, 
                                               const double&, const double&, const std::string&>(
                                                   &validateParameter<double>),
          py::arg("value"), py::arg("name"), py::arg("min"), py::arg("max"), py::arg("context"),
          "Validate parameter is within range [min, max]");

    m.def("validateParameter", py::overload_cast<const float&, const std::string&, 
                                               const float&, const float&, const std::string&>(
                                                   &validateParameter<float>),
          py::arg("value"), py::arg("name"), py::arg("min"), py::arg("max"), py::arg("context"),
          "Validate parameter is within range [min, max]");

    // Configuration utilities
    m.def("loadConfigFromFile", &Config::loadFromFile,
          py::arg("filepath"), py::arg("key"),
          "Load configuration from file and validate key exists");

    m.def("validateConfigKey", &Config::has,
          py::arg("config"), py::arg("key"),
          "Validate configuration key exists");

    // Batch operation utilities
    m.def("batchProcessSensorData", [](const std::vector<std::vector<float>>& inputs) {
        std::vector<std::vector<float>> outputs;
        outputs.reserve(inputs.size());
        for (const auto& input : inputs) {
            // Simulate batch processing
            std::vector<float> output(input.size(), 0.0f);
            std::transform(input.begin(), input.end(), output.begin(),
                          [](float x) { return x * 0.5f; });
            outputs.push_back(output);
        }
        return outputs;
    }, py::arg("inputs"), "Process multiple sensor inputs in batch");

    // Progress tracking utilities
    m.def("progressUpdate", [](double& progress, double increment, const std::string& description) {
        progress += increment;
        if (progress > 1.0) progress = 1.0;
        return progress;
    }, py::arg("progress"), py::arg("increment"), py::arg("description"),
        "Update progress tracking with description");

    // ====================================================================
    // SIMULATION HELPERS
    // ====================================================================

    // Run simple simulation
    m.def("runSimpleSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getFiringNeuronCount(),
                              brain->getTotalSpikeCount(),
                              agent->getCuriosityLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        "Run simple simulation and return statistics");

    // Run simulation with learning
    m.def("runLearningSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent, size_t steps,
                                    bool enableLearning = true) {
        agent->enableRewardModulation(enableLearning);
        agent->enableCuriosity(enableLearning);
        
        std::vector<float> rewards;
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward based on action type
            if (action == MotorCommand::MoveForward || action == MotorCommand::MoveBackward) {
                agent->applyRewardModulation(0.1f, 0.05f);
                rewards.push_back(0.1f);
            } else {
                rewards.push_back(0.0f);
            }
        }
        
        return std::make_tuple(brain->getFiringNeuronCount(),
                              brain->getTotalSpikeCount(),
                              agent->getCuriosityLevel(),
                              rewards);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("enableLearning") = true,
        "Run simulation with learning enabled");

    // ====================================================================
    // VALIDATION AND ERROR HANDLING
    // ====================================================================

    // Validate brain configuration
    m.def("validateBrainConfig", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        if (!config->has("neuron_count")) {
            errors.push_back("Missing neuron_count parameter");
        }
        
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount < 10 || neuronCount > 10000) {
            errors.push_back("neuron_count must be between 10 and 10000");
        }
        
        if (!config->has("region_count")) {
            errors.push_back("Missing region_count parameter");
        }
        
        int64_t regionCount = config->getOr<int64_t>("region_count", 0);
        if (regionCount < 1 || regionCount > 100) {
            errors.push_back("region_count must be between 1 and 100");
        }
        
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw ValidationError(errorMsg);
        }
        
        return true;
    }, py::arg("config"), "Validate brain configuration and throw ValidationError if invalid");

    // Validate simulation state
    m.def("validateSimulationState", [](const std::shared_ptr<Brain>& brain,
                                       const std::shared_ptr<SimpleWorld>& world,
                                       const std::shared_ptr<AgentBrain>& agent) {
        std::vector<std::string> errors;
        
        if (!brain || !world || !agent) {
            errors.push_back("Null pointer in simulation state");
        }
        
        if (brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (world->getWidth() == 0 || world->getHeight() == 0) {
            errors.push_back("World has invalid dimensions");
        }
        
        if (!errors.empty()) {
            std::string errorMsg = "Simulation state validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw IntegrationError(errorMsg);
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        "Validate simulation state and throw IntegrationError if invalid");

    // ====================================================================
    // ALIASES AND CONVENIENCE FUNCTIONS
    // ====================================================================

    // Backward compatibility aliases
    m.def("INVALID_NEURON_ID", []() { return INVALID_NEURON_ID; },
          "Get invalid neuron ID (backward compatibility)");
    
    m.def("INVALID_SYNAPSE_ID", []() { return INVALID_SYNAPSE_ID; },
          "Get invalid synapse ID (backward compatibility)");
    
    m.def("INVALID_REGION_ID", []() { return INVALID_REGION_ID; },
          "Get invalid region ID (backward compatibility)");
    
    m.def("INVALID_POPULATION_ID", []() { return INVALID_POPULATION_ID; },
          "Get invalid population ID (backward compatibility)");

    // ====================================================================
    // HELP AND DOCUMENTATION
    // ====================================================================

    // Get NLM version
    m.def("getVersion", []() {
        return "NLM Python Bindings - Version 1.0.0 (Phase 6 Complete)";
    }, "Get NLM version information");

    // Get available features
    m.def("getFeatures", []() {
        return std::vector<std::string>{
            "Memory Systems: Working, Episodic, Associative, Semantic, Procedural",
            "Neuromodulation: Dopamine, Novelty, Curiosity, Prediction Error, Reward",
            "Cognition: Attention, Planning, Concept Formation",
            "Development: Stage-based maturation",
            "Prediction: Forward modeling and prediction errors",
            "Performance: Memory pools, event queues, SIMD optimization",
            "Integration: Complete brain system integration",
            "Error Handling: Custom Python exceptions and validation",
            "Batch Operations: Vectorized processing",
            "Simulation Tools: Complete simulation runners",
            "Visualization: Output generation and analysis"
        };
    }, "Get list of available features");

    // Get system requirements
    m.def("getRequirements", []() {
        return std::vector<std::string>{
            "C++17 compatible compiler",
            "pybind11 >= 2.10",
            "CMake >= 3.16",
            "Python >= 3.8",
            "1 GB RAM minimum",
            "1 CPU core minimum"
        };
    }, "Get system requirements");

    // ====================================================================
    // END OF MODULE
    // ====================================================================

} // namespace nlm
