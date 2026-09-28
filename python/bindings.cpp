#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <vector>
#include <memory>

namespace py = pybind11;
namespace nlm {

// Error handling utilities
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

template<typename T>
T safe_call(const std::string& operation, std::function<T()> func) {
    try {
        return func();
    } catch (const std::exception& e) {
        throw NLMError("Error in " + operation + ": " + std::string(e.what()));
    }
}

// Smart pointer converters for Python
template<typename T>
std::shared_ptr<T> create_shared_ptr(T* ptr) {
    return std::shared_ptr<T>(ptr);
}

// Custom return_value_policy for weak_ptr
py::object weak_ptr_to_python(std::weak_ptr<void> weakPtr) {
    if (auto shared = weakPtr.lock()) {
        return py::cast(shared);
    }
    return py::none();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Advanced Python Bindings
        -----------------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        Features:
        - Advanced neural dynamics (SpikeSystem, NeuralDynamics)
        - Multiple plasticity rules (STDP, Hebbian, StructuralPlasticity)
        - Neuromodulation (ACh, NE, 5-HT, Dopamine, Curiosity, Novelty, PredictionError)
        - Advanced memory systems (NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory)
        - Prediction systems (NeuralPrediction, PredictionSystem)
        - Cognitive systems (NeuralPlanner, ConceptFormation, AttentionalSelection)
        - Development systems (DevelopmentSystem, Synaptogenesis, Pruning, Maturation)
        - Performance monitoring (CheckpointManager, PerformanceProfiler)
        - Advanced AgentBrain with goal-based planning and memory-guided action selection
        - Comprehensive error handling and documentation
        
        Usage Examples:
            import pynlm
            brain = pynlm.createBrain()
            spike_system = brain.getSpikeSystem()
            spike_system.queueSpike(neuron_id, timestamp, step)
            spike_system.processSpikes(step)
    )pbdoc";

    // Register all NLM exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<NLMError>(m, "NLMError");

    // Type bindings with error handling
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

    // Enumeration types
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

    // Basic configuration and world
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

    // Advanced Spike System
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Event-driven neural computation system)
    
    SpikeSystem handles spike event processing and delayed synaptic transmission.
    It uses an event-driven architecture for efficient neural simulation.
    )pbdoc")
        .def(py::init<>())
        .def("queueSpike", [](SpikeSystem& self, NeuronId neuron, Timestamp timestamp, SimulationStep step) {
            self.queueSpike(SpikeEvent{neuron, timestamp, step});
        }, py::arg("neuron"), py::arg("timestamp"), py::arg("step"),
           "Queue a spike event")
        .def("queueDelayedSpike", &SpikeSystem::queueDelayedSpike, py::arg("event"),
             "Queue a delayed spike event for synaptic transmission")
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentStep"),
             "Process all queued spikes for current step")
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes,
             py::arg("currentStep"), py::arg("currentTime"),
             "Process pending delayed spikes that should be delivered now")
        .def("registerHandler", &SpikeSystem::registerHandler, py::arg("handler"),
             "Register spike handler callback")
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler,
             py::arg("handler"), "Register delayed spike handler callback")
        .def("getSpikeHistory", &SpikeSystem::getSpikeHistory,
             py::return_value_policy::reference_internal,
             "Get spike history")
        .def("clearHistory", &SpikeSystem::clearHistory, "Clear spike history")
        .def("getSpikeCount", &SpikeSystem::getSpikeCount, "Get total spike count")
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount,
             "Get pending spike event count")
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount,
             "Get pending delayed spike count")
        .def("getAverageSpikeRate", &SpikeSystem::getAverageSpikeRate,
             "Get average spike rate")
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons,
             py::arg("count"), "Get most active neurons")
        .def("reset", &SpikeSystem::reset, "Reset spike system")
        .def("__repr__", [](const SpikeSystem& ss) {
            return "<SpikeSystem: " + std::to_string(ss.getSpikeCount()) + 
                   " spikes, " + std::to_string(ss.getPendingSpikeCount()) +
                   " pending>";
        });

    // Neural Dynamics
    py::class_<NeuralDynamics>(m, "NeuralDynamics", py::abstractattr("abstract"),
        R"pbdoc(Abstract base class for neural dynamics models)pbdoc")
        .def("updateNeuron", &NeuralDynamics::updateNeuron,
             py::arg("neuron"), py::arg("dt"),
             "Update neuron state for one timestep")
        .def("updateSynapse", &NeuralDynamics::updateSynapse,
             py::arg("synapse"), py::arg("dt"),
             "Update synapse state for one timestep")
        .def("applySpikeInput", &NeuralDynamics::applySpikeInput,
             py::arg("neuron"), py::arg("synapse"),
             "Apply synaptic input from spike event")
        .def("shouldFire", &NeuralDynamics::shouldFire,
             py::arg("neuron"), "Check if neuron should fire")
        .def("reset", &NeuralDynamics::reset, "Reset dynamics state");

    py::class_<IntegrateAndFireDynamics, NeuralDynamics>(m, "IntegrateAndFireDynamics",
        R"pbdoc(Integrate-and-fire neural dynamics model)pbdoc")
        .def(py::init<>())
        .def("setMembraneTimeConstant", &IntegrateAndFireDynamics::setMembraneTimeConstant,
             py::arg("tau"), "Set membrane time constant")
        .def("getMembraneTimeConstant", &IntegrateAndFireDynamics::getMembraneTimeConstant,
             "Get membrane time constant");

    // Plasticity Rules
    py::class_<PlasticityRule>(m, "PlasticityRule", py::abstractattr("abstract"),
        R"pbdoc(Abstract base class for synaptic plasticity rules)pbdoc")
        .def("update", &PlasticityRule::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weight based on spike timing")
        .def("applyWeightChange", &PlasticityRule::applyWeightChange,
             py::arg("synapse"), py::arg("delta"),
             "Apply weight change to synapse")
        .def("getName", &PlasticityRule::getName,
             "Get name of plasticity rule");

    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)
    
    STDP is a Hebbian learning rule where synaptic strength is adjusted based
    on the precise timing of pre- and post-synaptic spikes.
    )pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"),
             "Set long-term potentiation weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"),
             "Set long-term depression weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"),
             "Set STDP time constant")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get time constant")
        .def("configure", &STDP::configure,
             py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters");

    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule)
    
    Hebbian learning strengthens connections between neurons that fire together.
    Simple but effective for many learning tasks.
    )pbdoc")
        .def(py::init<>())
        .def("setLearningRate", [](Hebbian& self, float rate) {
            self.setLearningRate(rate);
        }, py::arg("rate"), "Set learning rate");

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity system)
    
    StructuralPlasticity handles creation and removal of synapses and neurons.
    This allows the network to change its structure based on activity and development.
    )pbdoc")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse,
             py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"),
             "Create new synapse")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse,
             py::arg("brain"), py::arg("synapse"), "Remove synapse")
        .def("createNeuron", &StructuralPlasticity::createNeuron,
             py::arg("brain"), py::arg("type"), "Create new neuron")
        .def("removeNeuron", &StructuralPlasticity::removeNeuron,
             py::arg("brain"), py::arg("neuron"), "Remove neuron")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate,
             py::arg("rate"), "Set synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate,
             "Get pruning rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate,
             py::arg("rate"), "Set pruning rate")
        .def("update", &StructuralPlasticity::update,
             py::arg("brain"), py::arg("rng"), "Update structural plasticity");

    // Neuromodulation
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base class for neuromodulators)
    
    Neuromodulators (like acetylcholine, norepinephrine, serotonin) modulate
    neural activity globally to influence learning, attention, and behavior.
    )pbdoc")
        .def(py::init<>())
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             "Set neuromodulator level")
        .def("getLevel", &Neuromodulator::getLevel, "Get current level")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)
    
    Dopamine modulates reward prediction and motivation. It influences prediction
    error signals and reinforces successful actions.
    )pbdoc")
        .def(py::init<>())
        .def("setRewardPredictionError", &Dopamine::setRewardPredictionError,
             py::arg("error"), "Set reward prediction error")
        .def("getRewardPredictionError", &Dopamine::getRewardPredictionError,
             "Get reward prediction error");

    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)
    
    Curiosity drives exploration and learning in novel environments. It increases
    when encountering unexpected or novel stimuli, encouraging exploration.
    )pbdoc")
        .def(py::init<>())
        .def("setNoveltyLevel", &Curiosity::setNoveltyLevel, py::arg("level"),
             "Set novelty level")
        .def("getNoveltyLevel", &Curiosity::getNoveltyLevel, "Get novelty level")
        .def("update", &Curiosity::update, py::arg("dt"),
             "Update curiosity based on environment");

    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc( novelty detector)
    
    Novelty detects new and surprising events in the environment. High novelty
    increases attention and learning rate, promoting exploration.
    )pbdoc")
        .def(py::init<>())
        .def("computeNovelty", &Novelty::computeNovelty,
             py::arg("sensoryInput"), "Compute novelty from sensory input")
        .def("getCurrentNovelty", &Novelty::getCurrentNovelty,
             "Get current novelty level");

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)
    
    Prediction error signals the difference between expected and actual outcomes.
    This drives learning and adaptation in predictive coding frameworks.
    )pbdoc")
        .def(py::init<>())
        .def("setError", &PredictionError::setError, py::arg("error"),
             "Set prediction error value")
        .def("getError", &PredictionError::getError, "Get prediction error")
        .def("reset", &PredictionError::reset, "Reset prediction error");

    // Memory Systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)
    
    NeuralWorkingMemory maintains information through persistent neural activity.
    Unlike simple storage, it uses reverberating activity and recurrent connections
    to maintain information over time.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information as neural activity pattern")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"), "Get activation of specific neuron")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
             py::arg("capacity"), "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate,
             py::arg("rate"), "Set decay rate")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"), "Strengthen memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is winning")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)
    
    NeuralEpisodicMemory stores experiences as neural activity patterns that can
    be retrieved and replayed. This enables memory-based learning and decision making.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode,
             py::arg("episode"), "Store a new experience")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve similar episodes by sensory pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes after specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode,
             py::arg("episode"), "Replay an episode")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("delta"),
             "Update episode relevance")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode,
             py::arg("index"), "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes,
             py::arg("count"), "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate,
             py::arg("relevanceThreshold"), "Consolidate memory")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay,
             py::arg("enable"), "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"), "Get episodes for replay")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence,
             py::arg("episodeIds"), "Replay sequence of episodes");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)
    
    NeuralAssociativeMemory creates and retrieves relationships between experiences
    through synaptic associations. This enables pattern completion and associative recall.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two patterns")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"), "Create associations from experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with query")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             "Get association strength")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association strength")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation through associations")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear all associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations");

    // Cognitive Systems
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attentional selection)
    
    AttentionalSelection implements neural attention through competition and selection.
    It selects which neural populations receive increased processing priority.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competition between neural populations")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion,
             py::arg("region"), "Apply attention to specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get currently attended regions")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
             py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
             py::arg("strength"), "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold,
             py::arg("threshold"), "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor,
             py::arg("neuron"), "Get inhibition level for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor,
             py::arg("neuron"), "Get excitation level for neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attentional bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up attentional salience")
        .def("reset", &AttentionalSelection::reset, "Reset attention");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Neural concept formation)
    
    ConceptFormation creates abstract concepts from sensory experiences.
    It identifies patterns and forms generalized representations that can be
    used for categorization and prediction.
    )pbdoc")
        .def(py::init<>())
        .def("learnConcept", &ConceptFormation::learnConcept,
             py::arg("experience"), "Learn concept from experience")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get learned concepts")
        .def("classify", &ConceptFormation::classify, py::arg("pattern"),
             "Classify pattern using learned concepts")
        .def("getConceptStrength", &ConceptFormation::getConceptStrength,
             py::arg("conceptId"), "Get concept strength")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("delta"),
             "Update concept strength");

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system)
    
    NeuralPlanner generates action sequences to achieve goals using neural mechanisms.
    It considers available actions, their consequences, and rewards to plan effectively.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("setGoal", &NeuralPlanner::setGoal, py::arg("goal"),
             "Set current goal")
        .def("getGoal", &NeuralPlanner::getGoal, "Get current goal")
        .def("planAction", &NeuralPlanner::planAction,
             "Generate plan for current goal")
        .def("getActionSequence", &NeuralPlanner::getActionSequence,
             "Get current action sequence")
        .def("updatePlan", &NeuralPlanner::updatePlan, py::arg("dt"),
             "Update planning based on new information")
        .def("evaluatePlan", &NeuralPlanner::evaluatePlan,
             "Evaluate quality of current plan")
        .def("cancelPlan", &NeuralPlanner::cancelPlan, "Cancel current plan")
        .def("isPlanning", &NeuralPlanner::isPlanning, "Check if planning active")
        .def("getPlanningTime", &NeuralPlanner::getPlanningTime,
             "Get planning time")
        .def("setPlanningTime", &NeuralPlanner::setPlanningTime,
             py::arg("time"), "Set planning time limit");

    // Development Systems
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Neural development system)
    
    DevelopmentSystem manages developmental stages and changes in brain structure.
    It controls synaptogenesis, pruning, and maturation processes.
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("update", &DevelopmentSystem::update, py::arg("dt"),
             "Update development processes")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("getStage", &DevelopmentSystem::getStage, "Get current stage")
        .def("getStageProgress", &DevelopmentSystem::getStageProgress,
             "Get progress through current stage")
        .def("isStageComplete", &DevelopmentSystem::isStageComplete,
             "Check if current stage complete")
        .def("startNextStage", &DevelopmentSystem::startNextStage,
             "Start next developmental stage")
        .def("getCriticalPeriod", &DevelopmentSystem::getCriticalPeriod,
             "Get critical period status")
        .def("setCriticalPeriodActive", &DevelopmentSystem::setCriticalPeriodActive,
             py::arg("active"), "Set critical period active state");

    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis - synapse formation)
    
    Synaptogenesis creates new synaptic connections during development.
    It establishes initial connectivity patterns that are later refined.
    )pbdoc")
        .def(py::init<>())
        .def("createInitialConnections", &Synaptogenesis::createInitialConnections,
             py::arg("brain"), "Create initial synaptic connections")
        .def("addNewSynapse", &Synaptogenesis::addNewSynapse,
             py::arg("brain"), py::arg("source"), py::arg("target"),
             "Add new synapse between neurons")
        .def("getConnectionDensity", &Synaptogenesis::getConnectionDensity,
             "Get current connection density")
        .def("setConnectionRate", &Synaptogenesis::setConnectionRate,
             py::arg("rate"), "Set connection formation rate")
        .def("getConnectionHistory", &Synaptogenesis::getConnectionHistory,
             "Get connection formation history");

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning - synaptic elimination)
    
    Pruning removes unused or weak synaptic connections.
    It refines neural circuits to improve efficiency and functionality.
    )pbdoc")
        .def(py::init<>())
        .def("evaluateConnections", &Pruning::evaluateConnections,
             py::arg("brain"), "Evaluate synaptic connections for pruning")
        .def("pruneWeakConnections", &Pruning::pruneWeakConnections,
             py::arg("threshold"), "Prune weak synaptic connections")
        .def("pruneUnusedConnections", &Pruning::pruneUnusedConnections,
             py::arg("idleThreshold"), "Prune unused connections")
        .def("getPruningStats", &Pruning::getPruningStats,
             "Get pruning statistics")
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"),
             "Set pruning rate")
        .def("getPruningRate", &Pruning::getPruningRate, "Get pruning rate");

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Matuation - neural refinement)
    
    Maturation refines neural properties and connectivity.
    It strengthens important connections and improves overall circuit efficiency.
    )pbdoc")
        .def(py::init<>())
        .def("refineConnections", &Maturation::refineConnections,
             py::arg("brain"), "Refine synaptic connections")
        .def("strengthenImportantConnections", &Maturation::strengthenImportantConnections,
             py::arg("brain"), py::arg("threshold"),
             "Strengthen important connections")
        .def("updateNeuronProperties", &Maturation::updateNeuronProperties,
             py::arg("brain"), "Update neuron properties")
        .def("getMaturationProgress", &Maturation::getMaturationProgress,
             "Get maturation progress")
        .def("setMaturationRate", &Maturation::setMaturationRate,
             py::arg("rate"), "Set maturation rate")
        .def("getMaturationRate", &Maturation::getMaturationRate,
             "Get maturation rate");

    // Performance and Checkpoint Systems
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance monitoring system)
    
    PerformanceProfiler monitors and analyzes brain performance metrics.
    It tracks simulation efficiency, accuracy, and resource usage.
    )pbdoc")
        .def(py::init<>())
        .def("startProfiling", &PerformanceProfiler::startProfiling,
             "Start performance profiling")
        .def("stopProfiling", &PerformanceProfiler::stopProfiling,
             "Stop performance profiling")
        .def("isProfiling", &PerformanceProfiler::isProfiling,
             "Check if profiling active")
        .def("getPerformanceMetrics", &PerformanceProfiler::getPerformanceMetrics,
             "Get current performance metrics")
        .def("getMetricHistory", &PerformanceProfiler::getMetricHistory,
             py::arg("metric"), "Get history for specific metric")
        .def("resetMetrics", &PerformanceProfiler::resetMetrics,
             "Reset all metrics")
        .def("setMetricThreshold", &PerformanceProfiler::setMetricThreshold,
             py::arg("metric"), py::arg("threshold"),
             "Set metric threshold for alerts")
        .def("getAlertHistory", &PerformanceProfiler::getAlertHistory,
             "Get performance alerts");

    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint and recovery system)
    
    CheckpointManager saves and restores brain state for recovery and experimentation.
    It enables reproducible simulations and continuation of interrupted runs.
    )pbdoc")
        .def(py::init<>())
        .def("createCheckpoint", &CheckpointManager::createCheckpoint,
             py::arg("name"), "Create new checkpoint")
        .def("saveCheckpoint", &CheckpointManager::saveCheckpoint,
             py::arg("name"), py::arg("brain"),
             "Save brain state to checkpoint")
        .def("loadCheckpoint", &CheckpointManager::loadCheckpoint,
             py::arg("name"), "Load brain state from checkpoint")
        .def("deleteCheckpoint", &CheckpointManager::deleteCheckpoint,
             py::arg("name"), "Delete checkpoint")
        .def("getCheckpointList", &CheckpointManager::getCheckpointList,
             "Get list of all checkpoints")
        .def("getCheckpointInfo", &CheckpointManager::getCheckpointInfo,
             py::arg("name"), "Get checkpoint information")
        .def("hasCheckpoint", &CheckpointManager::hasCheckpoint,
             py::arg("name"), "Check if checkpoint exists")
        .def("restoreToCheckpoint", &CheckpointManager::restoreToCheckpoint,
             py::arg("name"), "Restore to specific checkpoint")
        .def("getAutoSaveInterval", &CheckpointManager::getAutoSaveInterval,
             "Get automatic save interval")
        .def("setAutoSaveInterval", &CheckpointManager::setAutoSaveInterval,
             py::arg("interval"), "Set automatic save interval");

    // Advanced AgentBrain with comprehensive bindings
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface)
    
    AgentBrain connects the NLM brain to a simulated world and provides
    high-level interfaces for goal-directed behavior, memory-guided action
    selection, and adaptive control.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world simulation")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation enabled")
        .def("isStructuralPlasticityEnabled",
             &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity enabled");

    // Goal-based planning methods
    m.def("planGoalDirectedAction", [](std::shared_ptr<AgentBrain> agentBrain,
                                    const std::vector<float>& goal,
                                    const std::vector<float>& currentState) {
        return agentBrain->planGoalDirectedAction(goal, currentState);
    }, py::arg("agentBrain"), py::arg("goal"), py::arg("currentState"),
       "Plan action sequence to achieve goal");

    m.def("selectActionMemoryGuided", [](std::shared_ptr<AgentBrain> agentBrain,
                                      const std::vector<float>& sensoryInput,
                                      const std::vector<float>& memoryState) {
        return agentBrain->selectActionMemoryGuided(sensoryInput, memoryState);
    }, py::arg("agentBrain"), py::arg("sensoryInput"), py::arg("memoryState"),
       "Select action guided by memory systems");

    // Advanced neuromodulation controls
    m.def("adjustNeuromodulation", [](std::shared_ptr<AgentBrain> agentBrain,
                                   const std::string& modulator,
                                   float level, float timescale) {
        agentBrain->adjustNeuromodulation(modulator, level, timescale);
    }, py::arg("agentBrain"), py::arg("modulator"), py::arg("level"),
       py::arg("timescale"), "Adjust neuromodulator levels");

    // Memory system access
    m.def("getWorkingMemoryState", [](std::shared_ptr<AgentBrain> agentBrain) {
        auto brain = agentBrain->getBrain();
        auto& memory = brain->getWorkingMemory();
        return memory.retrieve();
    }, py::arg("agentBrain"), "Get working memory state as vector");

    m.def("getEpisodicMemoryCount", [](std::shared_ptr<AgentBrain> agentBrain) {
        auto brain = agentBrain->getBrain();
        auto& episodicMemory = brain->getEpisodicMemory();
        return episodicMemory.getEpisodeCount();
    }, py::arg("agentBrain"), "Get number of stored episodic memories");

    m.def("recallSimilarMemory", [](std::shared_ptr<AgentBrain> agentBrain,
                                const std::vector<float>& pattern,
                                size_t maxResults = 3) {
        auto brain = agentBrain->getBrain();
        auto& episodicMemory = brain->getEpisodicMemory();
        return episodicMemory.retrieveSimilar(pattern, maxResults);
    }, py::arg("agentBrain"), py::arg("pattern"), py::arg("maxResults") = 3,
       "Recall episodic memories similar to pattern");

    // Prediction system integration
    m.def("getPredictionError", [](std::shared_ptr<AgentBrain> agentBrain) {
        return agentBrain->getPredictionError();
    }, py::arg("agentBrain"), "Get prediction error signal");

    m.def("updatePredictionSystem", [](std::shared_ptr<AgentBrain> agentBrain,
                                    const std::vector<float>& sensoryInput) {
        agentBrain->updatePredictionSystem(sensoryInput);
    }, py::arg("agentBrain"), py::arg("sensoryInput"),
       "Update prediction system with new input");

    // Development stage manipulation
    m.def("advanceDevelopmentStage", [](std::shared_ptr<AgentBrain> agentBrain) {
        agentBrain->advanceDevelopmentStage();
    }, py::arg("agentBrain"), "Advance to next development stage");

    m.def("setDevelopmentStage", [](std::shared_ptr<AgentBrain> agentBrain,
                                 const DevelopmentalStage& stage) {
        agentBrain->setDevelopmentStage(stage);
    }, py::arg("agentBrain"), py::arg("stage"), "Set development stage");

    // Performance monitoring methods
    m.def("getPerformanceMetrics", [](std::shared_ptr<AgentBrain> agentBrain) {
        auto brain = agentBrain->getBrain();
        return brain->getPerformanceMetrics();
    }, py::arg("agentBrain"), "Get brain performance metrics");

    m.def("getSimulationStats", [](std::shared_ptr<AgentBrain> agentBrain) {
        auto brain = agentBrain->getBrain();
        return brain->getSimulationStats();
    }, py::arg("agentBrain"), "Get simulation statistics");

    // Configuration profile management
    m.def("createAgentProfile", [](const std::string& name,
                                 const std::unordered_map<std::string, float>& parameters) {
        auto config = std::make_shared<Config>();
        for (const auto& pair : parameters) {
            config->set(pair.first, pair.second);
        }
        config->set("agent.name", name);
        return config;
    }, py::arg("name"), py::arg("parameters"),
       "Create configuration profile for agent");

    m.def("applyAgentProfile", [](std::shared_ptr<AgentBrain> agentBrain,
                               const std::shared_ptr<Config>& profile) {
        agentBrain->applyConfigurationProfile(profile);
    }, py::arg("agentBrain"), py::arg("profile"),
       "Apply configuration profile to agent");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSpikeSystem", []() -> std::shared_ptr<SpikeSystem> {
        return std::make_shared<SpikeSystem>();
    }, "Create a new spike system");

    m.def("createSTDP", []() -> std::shared_ptr<STDP> {
        return std::make_shared<STDP>();
    }, "Create a new STDP plasticity rule");

    m.def("createNeuromodulator", []() -> std::shared_ptr<Neuromodulator> {
        return std::make_shared<Neuromodulator>();
    }, "Create a new neuromodulator");

    m.def("createWorkingMemory", []() -> std::shared_ptr<NeuralWorkingMemory> {
        return std::make_shared<NeuralWorkingMemory>();
    }, "Create a new working memory system");

    m.def("createEpisodicMemory", []() -> std::shared_ptr<NeuralEpisodicMemory> {
        return std::make_shared<NeuralEpisodicMemory>();
    }, "Create a new episodic memory system");

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create a new development system");

    m.def("createPerformanceProfiler", []() -> std::shared_ptr<PerformanceProfiler> {
        return std::make_shared<PerformanceProfiler>();
    }, "Create a new performance profiler");

    m.def("createCheckpointManager", []() -> std::shared_ptr<CheckpointManager> {
        return std::make_shared<CheckpointManager>();
    }, "Create a new checkpoint manager");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants and utility functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Exception utilities
    py::class_<NLMError, std::runtime_error>(m, "NLMError", py::module_local())
        .def(py::init<const std::string&>());

    // Helper function for error handling in Python
    m.def("safe_execute", [](const std::string& operation, py::function<void()> func) {
        try {
            func();
        } catch (const std::exception& e) {
            throw NLMError("Error in " + operation + ": " + std::string(e.what()));
        }
    }, py::arg("operation"), py::arg("func"), "Execute function with error handling");

    // Smart pointer utilities
    m.def("weak_ref", [](const py::object& obj) {
        return py::weakref(obj);
    }, py::arg("obj"), "Create weak reference to Python object");

    // Serialization utilities
    m.def("to_dict", [](const Brain& brain) {
        return brain.to_dict();
    }, py::arg("brain"), "Convert brain state to dictionary");

    m.def("from_dict", [](const py::dict& state) {
        // This would require implementing __init__ from dict
        throw NLMError("from_dict not yet implemented - requires additional constructor");
    }, py::arg("state"), "Create brain from dictionary state");

    // Example usage patterns
    m.def("example_learning_loop", [](std::shared_ptr<AgentBrain> agentBrain,
                                    std::shared_ptr<SimpleWorld> world,
                                    size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            auto action = agentBrain->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            agentBrain->applyRewardModulation(result.reward, agentBrain->getPredictionError());
        }
    }, py::arg("agentBrain"), py::arg("world"), py::arg("steps"),
       "Example learning loop for agent-environment interaction");

    m.def("example_memory_guided_behavior", [](std::shared_ptr<AgentBrain> agentBrain,
                                            const std::vector<float>& goal) {
        auto workingMemory = agentBrain->getWorkingMemory();
        workingMemory->store(goal, 1.0f);
        
        auto memoryState = workingMemory->retrieve();
        auto action = agentBrain->selectActionMemoryGuided(
            agentBrain->getSensoryInputSize(), memoryState);
        
        return action;
    }, py::arg("agentBrain"), py::arg("goal"),
       "Example memory-guided behavior");
}

} // namespace nlm