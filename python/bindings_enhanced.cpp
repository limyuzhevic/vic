#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include "../src/core/Logger/Logger.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function to convert C++ exceptions to Python exceptions
void setupPythonErrorHandling() {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });
}

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides comprehensive bindings for all integrated brain systems:
        - Core neural systems (Brain, Config, Neurons, Synapses)
        - Memory systems (Working, Episodic, Associative memory)
        - Cognitive systems (Planning, Self-model, Social learning)
        - Neuromodulation (Dopamine, Curiosity, Novelty, Prediction error)
        - Development and plasticity systems
        
        Features:
        - Full error handling with Python exceptions
        - Type safety and validation
        - Memory system integration with neural dynamics
        - Advanced cognitive capabilities
        - Configuration management with validation
        - Checkpoint/save-load functionality
        
        Usage:
            import pynlm_enhanced as nlm
            
            # Create brain with configuration
            config = nlm.Config()
            brain = nlm.Brain(config)
            
            # Configure memory systems
            working_memory = brain.getWorkingMemory()
            episodic_memory = brain.getEpisodicMemory()
            
            # Use cognitive systems
            planner = brain.getPlanner()
            self_model = brain.getSelfModel()
            social_learning = brain.getSocialLearning()
            
            # Main simulation loop
            for step in range(max_steps):
                # Process sensory input
                brain.receiveSensoryInput(percept)
                
                # Update all integrated systems
                brain.step(step)
                
                # Generate action
                action = brain.produceAction()
    )pbdoc";

    // Setup error handling
    setupPythonErrorHandling();

    // Register NLM exceptions for Python
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<ConfigValidationError>(m, "ConfigError");
    py::register_exception<MemorySystemError>(m, "MemoryError");
    py::register_exception<NeuralDynamicsError>(m, "NeuralDynamicsError");
    py::register_exception<PlasticityError>(m, "PlasticityError");
    py::register_exception<CognitiveError>(m, "CognitiveError");
    py::register_exception<PredictionError>(m, "PredictionError");
    py::register_exception<NeuromodulationError>(m, "NeuromodulationError");
    py::register_exception<DevelopmentError>(m, "DevelopmentError");
    py::register_exception<CheckpointError>(m, "CheckpointError");

    // Basic types
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

    // Enum types
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

    // Core classes
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
             "Get a summary string of the configuration");

    // Memory Systems
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory",
        R"pbdoc(Working memory using persistent neural activity and recurrent connections.
        
        NOT a simple storage vector - uses persistent activity and recurrent connections.
        Key mechanisms:
        - Persistent neural activity (reverberating activity)
        - Recurrent synaptic connections for maintaining information
        - Activity-dependent short-term plasticity
        - Neural competition for selective retention
        
        Methods:
        - store(pattern, strength): Store information as neural activity pattern
        - retrieve(): Get current working memory content as activity levels
        - update(dt): Update working memory (maintenance and decay)
        - strengthenMemory(factor): Strengthen working memory representation
        - runCompetition(): Competition between memory traces
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set capacity")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory");

    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory",
        R"pbdoc(Episodic memory for storing experiences and episodes.
        
        NOT a database - episodes are encoded as neural activity patterns and can influence neural activity.
        Key mechanisms:
        - Episodes stored as neural activity patterns
        - Retrieval via pattern completion (Hopfield-like)
        - Temporal ordering through timestamps
        - Relevance-weighted recall
        - Replay during offline periods
        
        Methods:
        - storeEpisode(episode): Store a new experience
        - retrieveSimilar(pattern, maxResults): Retrieve similar episodes by pattern
        - retrieveTemporal(startTime, endTime, maxResults): Retrieve episodes by time window
        - replayEpisode(episode): Replay an episode to reactivate neural patterns
        - getEpisodesForReplay(count): Get episodes for replay during consolidation
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to a query pattern")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episode - reactivates the neural patterns associated with it")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"), "Get episodes for replay (selection based on relevance and recency)")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes,
             py::arg("count"), "Get recent episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Clear old episodes based on relevance");

    py::class_<NeuralAssociativeMemory>(m, "AssociativeMemory",
        R"pbdoc(Associative memory for relationships between experiences.
        
        NOT a lookup table - associations are encoded in synaptic connectivity.
        Key mechanisms:
        - Co-activating neurons become associated via Hebbian plasticity
        - Stronger associations form with repeated co-activation
        - Can retrieve related items given a partial cue
        - Spreading activation for associative recall
        
        Methods:
        - associate(patternA, patternB, strength): Create association between patterns
        - retrieve(pattern, maxResults): Get patterns associated with a query
        - spreadActivation(cuePattern, steps): Spread activation through associations
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with a query pattern")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"),
             "Get association strength between two patterns")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear all associations");

    // Cognitive Systems
    py::class_<NeuralPlanner>(m, "Planner",
        R"pbdoc(Neural planner for action planning using predictive neural dynamics.
        
        NOT a search engine - planning emerges from predictive neural dynamics.
        Key mechanisms:
        - Uses action-consequence predictions
        - Evaluates potential futures through simulation
        - Selects actions leading to high-value outcomes
        - Adapts plans based on prediction confidence
        
        Methods:
        - planAction(currentState, targetReward): Plan next action
        - evaluateSequence(actions, startState): Evaluate action sequence
        - updatePlanQuality(plannedActions, actualActions, reward): Update based on outcomes
        - setPlanningDepth(depth): Set lookahead depth
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set number of steps to look ahead")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get current planning confidence");

    py::class_<SelfModel>(m, "SelfModel",
        R"pbdoc(Self model for internal representation of the agent's own actions and effects.
        
        NOT consciousness - sensorimotor self-awareness through experience.
        Key mechanisms:
        - Learns body schema (how actions affect sensory state)
        - Predicts consequences of own actions
        - Distinguishes self from external events
        - Represents own capabilities and limitations
        
        Methods:
        - recordSelfAction(action, beforeState, afterState): Record action effect
        - predictActionConsequence(action, currentState): Predict sensory consequence
        - updateSelfModel(predicted, actual, action): Update based on prediction error
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
             "Record that taking an action caused a specific sensory change")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict sensory consequence of an action")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel,
             "Get current capability level (0-1)")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness,
             "Get body awareness (how accurately can predict consequences");

    py::class_<SocialLearning>(m, "SocialLearning",
        R"pbdoc(Social learning for observing other agents and learning from their actions.
        
        NOT language - simple action-effect learning from observation.
        Key mechanisms:
        - Observes other agent's actions
        - Learns that other agents cause predictable changes
        - Can imitate observed actions
        - Develops simple communication signals
        
        Methods:
        - observeAgentAction(action, state, resultingState): Record observation
        - getImitationAction(currentState): Get best action to imitate
        - learnCommunicationSignal(pattern, reward): Learn communication signal
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("observeAgentAction", &SocialLearning::observeAgentAction,
             py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"),
             "Record observation of another agent's action")
        .def("getImitationAction", &SocialLearning::getImitationAction, py::arg("currentState"),
             "Get the best action to imitate given current state")
        .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"),
             "Can I imitate this observed action?")
        .def("getObservationCount", &SocialLearning::getObservationCount,
             "Get observation count");

    // Other integrated systems
    py::class_<ConceptFormation>(m, "ConceptFormation",
        R"pbdoc(Concept formation for discovering recurring patterns from experience.
        
        NOT a predefined taxonomy - concepts emerge through repeated experience.
        Key mechanisms:
        - Pattern clustering through similarity
        - Prototype formation through averaging
        - Instance tracking for stability measurement
        - Category discovery based on shared properties
        - No predefined labels or semantic categories
        
        Methods:
        - presentExperience(pattern, features, reward, time): Present new experience
        - getMatchingConcept(pattern, threshold): Get matching concept
        - findConceptForPattern(pattern): Find concept for given pattern
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present a new experience to the concept formation system")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get the concept that best matches a pattern")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts");

    py::class_<AttentionalSelection>(m, "Attention",
        R"pbdoc(Attentional selection for focus on relevant information.
        
        NOT Transformer attention - uses lateral inhibition and gain modulation.
        Key mechanisms:
        - Competition between neural populations
        - Salience-based selection
        - Recurrent amplification of winners
        - Inhibitory surround suppression
        
        Methods:
        - processCompetition(competitors, inhibition): Process competition
        - applyTopDownBias(neuron, strength): Apply goal-directed bias
        - applyBottomUpSalience(neuron, strength): Apply stimulus-driven salience
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics");

    // Brain (main class)
    py::class_<Brain>(m, "Brain",
        R"pbdoc(Central neural simulation brain class with all integrated systems.
        
        Core neural processing with event-driven spike propagation and integrated
        memory, prediction, cognition, and neuromodulation systems.
        
        Key methods:
        - initialize(): Initialize all integrated systems
        - step(currentStep, currentTime): Main simulation step
        - receiveSensoryInput(input): Inject sensory input into brain
        - produceAction(): Generate motor action based on neural activity
        - save(filepath)/load(filepath): Checkpoint/save brain state
        - getWorkingMemory()/getEpisodicMemory()/getAssociativeMemory(): Access memory systems
        - getPlanner()/getSelfModel()/getSocialLearning()/getConceptFormation()/getAttention(): Access cognitive systems
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getSelfModel", &Brain::getSelfModel,
             "Get self model")
        .def("getSocialLearning", &Brain::getSocialLearning,
             "Get social learning system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Constants
    m.attr("DEFAULT_NEURON_COUNT") = DEFAULT_NEURON_COUNT;
    m.attr("DEFAULT_TIMESTEP") = DEFAULT_TIMESTEP;
    m.attr("MAX_SYNAPSE_WEIGHT") = MAX_SYNAPSE_WEIGHT;
    m.attr("MIN_SYNAPSE_WEIGHT") = MIN_SYNAPSE_WEIGHT;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = DEFAULT_CONNECTION_PROBABILITY;
    m.attr("DEFAULT_STDP_LTP_WEIGHT") = DEFAULT_STDP_LTP_WEIGHT;
    m.attr("DEFAULT_STDP_LTD_WEIGHT") = DEFAULT_STDP_LTD_WEIGHT;
    m.attr("DEFAULT_WORKING_MEMORY_CAPACITY") = DEFAULT_WORKING_MEMORY_CAPACITY;
    m.attr("DEFAULT_EPISODIC_MAX_EPISODES") = DEFAULT_EPISODIC_MAX_EPISODES;

    // Error types (for user catching)
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, ("NLM Error: " + e.what()).c_str());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

} // namespace nlm

// Enhanced module declaration for Python
PYBIND11_PLUGIN(pynlm_enhanced)
