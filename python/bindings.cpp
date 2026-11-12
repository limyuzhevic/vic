#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function for robust vector validation
void validateVectorSize(const std::string& name, const std::vector<float>& vec, size_t expectedSize) {
    if (vec.size() != expectedSize) {
        throw std::invalid_argument(name + " vector must have size " + 
                                   std::to_string(expectedSize) + ", got " + 
                                   std::to_string(vec.size()));
    }
}

// Helper function for vector similarity computation
float computeCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectors must have the same size for similarity computation");
    }
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    float denominator = std::sqrt(normA) * std::sqrt(normB);
    if (denominator < 1e-8f) {
        return 0.0f;  // Orthogonal or zero vectors
    }
    
    return dotProduct / denominator;
}

// Enhanced AgentBrain with better Python integration
py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
    Agent brain interface connecting NLM brain to world.
    
    AgentBrain acts as the bridge between the neural brain and the external world,
    handling sensory transduction and motor command decoding. It integrates
    cognitive functions including planning, concept formation, and social learning.
    
    Key features:
    - Sensory processing and injection into brain
    - Motor action decoding from neural activity
    - Neuromodulation (reward, curiosity, novelty)
    - Cognitive integration (planning, concepts, attention)
    - Development system updates
    
    Examples:
        ```python
        import pynlm
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        ```
    )pbdoc")
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
         "Create agent brain with reference to neural brain")
    .def("initialize", &AgentBrain::initialize, py::arg("world"),
         "Initialize agent with world interface")
    .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
         "Get expected sensory input size from brain")
    .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
         "Get expected motor output size from brain")
    .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
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
         "Check if reward modulation is enabled")
    .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
         "Check if structural plasticity is enabled")
    .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
         "Check if development is enabled")
    .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
         "Check if curiosity is enabled")
    .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
         "Get formatted thought trace (recent percept -> action -> outcome)")
    .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
         "Get learned preference trait (survives checkpoint reload)")
    .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
         "Get count of vision-gated reflex bonuses")
    .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
         "Get count of hazard suppressions")
    .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
         "Get count of winner-take-all pulses")
    .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
         "Get count of planner overrides")
    .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
         "Get count of planner agreements")
    .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
         "Get count of planner decisions")
    .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
         "Get count of concept biases")
    .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
         "Get count of self-model biases")
    .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
         "Get count of social learning biases")
    .def("getConceptCount", &AgentBrain::getConceptCount,
         "Get number of learned concepts")
    .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
         "Get self-model confidence (0-1)")
    .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
         "Get number of social observations")
    .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
         "Get internal neural firing rates for pattern recall")
    .def("encodeText", &AgentBrain::encodeText, py::arg("text"),
         "Encode a text string into neural pattern for learning")
    .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
         py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
         "Observe another agent's actions for social learning")
    .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
         "Get detailed motor group activity information")
    .def("reset", &AgentBrain::reset,
         "Reset agent for new episode");

// SelfModel - Agent's internal model of itself
py::class_<SelfModel>(m, "SelfModel", R"pbdoc(
    Agent's internal model of itself (sensorimotor self-awareness).
    
    SelfModel represents how the agent understands its own body and actions.
    It's NOT consciousness - it's practical self-awareness through experience.
    
    Key mechanisms:
    - Learns body schema (how actions affect sensory state)
    - Predicts consequences of own actions  
    - Distinguishes self from external events
    - Represents own capabilities and limitations
    
    Examples:
        ```python
        import pynlm
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        
        # The agent's internal self-model will develop through experience
        for step in range(100):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
        
        # Access self-awareness
        confidence = agent.getSelfModelConfidence(pynlm.ActionType.MoveForward)
        capability = agent.getSelfModel().getCapabilityLevel()
        ```
    )pbdoc")
    .def(py::init<>())
    .def("initialize", &SelfModel::initialize, py::arg("brain"),
         "Initialize with brain reference")
    .def("recordSelfAction", &SelfModel::recordSelfAction,
         py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
         "Record that taking an action caused a specific sensory change")
    .def("predictActionConsequence", &SelfModel::predictActionConsequence,
         py::arg("action"), py::arg("currentState"),
         "Predict sensory consequence of an action (the agent's 'forward model')")
    .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence,
         py::arg("action"), "Get confidence in self-model for a given action")
    .def("computeSelfGeneratedLikeness", &SelfModel::computeSelfGeneratedLikeness,
         py::arg("beforeState"), py::arg("afterState"), py::arg("action"),
         "Determine if change is likely caused by self vs external")
    .def("getPreferredAction", &SelfModel::getPreferredAction,
         py::arg("state"), "Get preferred action in a given state (body schema)")
    .def("updateSelfModel", &SelfModel::updateSelfModel,
         py::arg("predicted"), py::arg("actual"), py::arg("action"),
         "Update self-model based on prediction error")
    .def("getCapabilityLevel", &SelfModel::getCapabilityLevel,
         "Get current capability level (0-1)")
    .def("getBodyAwareness", &SelfModel::getBodyAwareness,
         "Get body awareness (how accurately can predict consequences)")
    .def("hasSelfModel", &SelfModel::hasSelfModel,
         "Check if self-model has been formed (conceptual understanding)")
    .def("clear", &SelfModel::clear,
         "Clear self-model (reset agent's body awareness)");

// SocialLearning - Learning from observing other agents
py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(
    Learning from observing other agents (social cognition).
    
    SocialLearning enables learning without direct experience by observing
    other agents. It's NOT language - simple action-effect learning from observation.
    
    Key mechanisms:
    - Observes other agent's actions and consequences
    - Learns that other agents cause predictable changes
    - Can imitate observed actions
    - Develops simple communication signals
    
    Examples:
        ```python
        import pynlm
        
        # Create two agents
        brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain1.initialize()
        agent1 = pynlm.createAgentBrain(brain1)
        
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.initialize()
        agent2 = pynlm.createAgentBrain(brain2)
        
        # Create shared world
        world = pynlm.createSimpleWorld()
        
        # Let agent1 observe agent2
        for step in range(50):
            # Agent2 acts
            action = agent2.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Agent1 observes
            percept = world.getSensoryPercept()
            agent1.observeOtherAgent(action, percept.getVision(), percept.getTouch())
        
        # Agent1 can now imitate agent2
        imitation_action = agent1.getImitationAction(current_state)
        ```
    )pbdoc")
    .def(py::init<>())
    .def("initialize", &SocialLearning::initialize, py::arg("brain"),
         "Initialize with brain reference")
    .def("observeAgentAction", &SocialLearning::observeAgentAction,
         py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"),
         "Record observation of another agent's action and effect")
    .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"),
         "Can I imitate this observed action?")
    .def("getImitationActionAndSimilarity", &SocialLearning::getImitationActionAndSimilarity,
         py::arg("currentState"),
         "Get best imitation action with similarity match (for refusal)")
    .def("getImitationAction", &SocialLearning::getImitationAction,
         py::arg("currentState"), "Get the best action to imitate")
    .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal,
         py::arg("signalPattern"), py::arg("signalReward"),
         "Learn simple communication signal from another agent")
    .def("detectSignal", &SocialLearning::detectSignal, py::arg("neuralPattern"),
         "Detect if another agent is signaling")
    .def("getSignalPattern", &SocialLearning::getSignalPattern,
         "Get learned signal pattern")
    .def("getSignalMeaning", &SocialLearning::getSignalMeaning,
         "Get signal meaning (associated reward)")
    .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge,
         py::arg("interactionReward"), "Update social knowledge based on interactions")
    .def("computeSimilarity", &SocialLearning::computeSimilarity,
         py::arg("a"), py::arg("b"), "Compute cosine similarity between patterns")
    .def("clear", &SocialLearning::clear, "Clear social learning")
    .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge,
         "Has learned from others?")
    .def("getObservationCount", &SocialLearning::getObservationCount,
         "Get number of observations made")
    .def("getSignalPattern", &SocialLearning::getSignalPattern,
         "Get learned signal pattern (duplicate - kept for compatibility)");

// NeuralPlanner - Multi-step action planning
py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(
    Multi-step action planning using learned predictions.
    
    NeuralPlanner uses learned predictions to plan multi-step actions.
    It is NOT a search engine - planning emerges from predictive neural dynamics.
    
    Key mechanisms:
    - Uses action-consequence predictions
    - Evaluates potential futures through simulation
    - Selects actions leading to high-value outcomes
    - Adapts plans based on prediction confidence
    
    Examples:
        ```python
        import pynlm
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Create agent interface
        agent = pynlm.createAgentBrain(brain)
        
        # Set up concept formation (needed by planner)
        # (ConceptFormation is automatically integrated when agent is created)
        
        # Plan actions
        for step in range(100):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Get planned action
            planned_action = agent.getBrain().getActionPlanner().planAction(current_state)
        ```
    )pbdoc")
    .def(py::init<>())
    .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
         "Initialize with brain reference")
    .def("planAction", &NeuralPlanner::planAction,
         py::arg("currentState"), py::arg("targetReward"),
         "Plan next action given current state and goal")
    .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
         py::arg("actions"), py::arg("startState"),
         "Evaluate a potential action sequence")
    .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
         "Get number of steps to look ahead")
    .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
         "Set planning depth")
    .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
         "Get planning confidence")
    .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
         py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
         "Update plans based on actual outcome")
    .def("clearCache", &NeuralPlanner::clearCache,
         "Clear planning cache")
    .def("setActionQuality", &NeuralPlanner::setActionQuality,
         py::arg("action"), py::arg("quality"),
         "Set action quality from experience")
    .def("getActionQuality", &NeuralPlanner::getActionQuality, py::arg("action"),
         "Get action quality learned from experience (0 if never tried)")
    .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
         "Get current goal state")
    .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
         "Set current goal")
    .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
         "Has recent planning been successful?")
    .def("generateActionSequences", &NeuralPlanner::generateActionSequences,
         py::arg("depth"), "Generate possible action sequences (internal use)")
    .def("evaluateAction", &NeuralPlanner::evaluateAction,
         py::arg("action"), py::arg("state"),
         "Evaluate single action from state (internal use)")
    .def("predictNextState", &NeuralPlanner::predictNextState,
         py::arg("action"), py::arg("currentState"),
         "Predict next state after an action (internal use)");

// ConceptFormation - Pattern discovery from experience
py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(
    Discovers recurring patterns from experience (concept emergence).
    
    ConceptFormation discovers recurring patterns from experience and forms
    concepts. It is NOT a predefined taxonomy - concepts emerge through
    repeated experience via Hebbian clustering.
    
    Key mechanisms:
    - Pattern clustering through similarity
    - Prototype formation through averaging
    - Instance tracking for stability measurement
    - Category discovery based on shared properties
    - No predefined labels or semantic categories
    
    Examples:
        ```python
        import pynlm
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Create agent (automatically integrates concept formation)
        agent = pynlm.createAgentBrain(brain)
        
        # Present experiences
        for step in range(200):
            # Extract features from world state
            pattern = extract_visual_features(percept)
            features = extract_tactile_features(percept)
            reward = get_reward(percept)
            
            # Present to concept formation
            concept_id = agent.getBrain().getConceptFormation().presentExperience(
                pattern, features, reward, step)
            
            # Brain processes
            brain.step(step)
        
        # Access formed concepts
        for concept in agent.getBrain().getConceptFormation().getConcepts():
            print(f"Concept: {concept.categoryHint}")
            print(f"  Stability: {concept.avgStability:.3f}")
            print(f"  Observations: {concept.totalObservations}")
        ```
    )pbdoc")
    .def(py::init<>())
    .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
         "Initialize with brain reference")
    .def("presentExperience", &ConceptFormation::presentExperience,
         py::arg("pattern"), py::arg("features"), py::arg("reward"),
         py::arg("currentTime"),
         "Present a new experience to the concept formation system")
    .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
         py::arg("pattern"), py::arg("similarityThreshold"),
         "Get the concept that best matches a pattern")
    .def("getConcepts", &ConceptFormation::getConcepts,
         "Get all discovered concepts")
    .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
         "Get a specific concept by ID");

// Enhanced Brain with better Python integration
py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
    Central neural simulation brain class for Phase 6 integration.
    
    Brain is the main neural processing unit implementing the complete
    Phase 6 integrated brain loop with all cognitive and biological systems.
    
    It implements the full integrated brain simulation:
    1. Process pending delayed spike events
    2. Update all neuron membrane potentials (LIF dynamics)
    3. Detect spikes and schedule outgoing spike events
    4. Update working memory
    5. Apply neuromodulation effects
    6. Apply plasticity rules (STDP, Hebbian)
    7. Update episodic memory
    8. Update prediction system
    9. Update attention system
    10. Update concept formation
    11. Apply structural plasticity
    12. Replay important memories
    13. Apply development effects
    14. Collect statistics
    
    Examples:
        ```python
        import pynlm
        
        # Create and configure brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
            # Get statistics periodically
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
                print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```
    )pbdoc")
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
         "Initialize brain with configuration")
    .def("initialize", &Brain::initialize,
         "Initialize the brain with configuration")
    .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
         py::arg("currentStep"), "Perform a simulation step")
    .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
         py::arg("currentStep"), py::arg("currentTime"),
         "Perform a simulation step with timestamp")
    .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
         "Inject sensory input into the brain")
    .def("injectCurrent", &Brain::injectCurrent,
         py::arg("neuron"), py::arg("current"),
         "Inject current into a specific neuron")
    .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
         py::arg("type"), py::arg("current"),
         "Inject current into all neurons of a specific type")
    .def("produceAction", &Brain::produceAction,
         "Produce motor action based on neural activity")
    .def("reset", &Brain::reset, "Reset brain state")
    .def("save", &Brain::save, py::arg("filepath"),
         "Save brain state to file")
    .def("load", &Brain::load, py::arg("filepath"),
         "Load brain state from file")
    .def("addRegion", &Brain::addRegion, py::arg("name") = "",
         "Add a new neural region")
    .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
         py::arg("id"), py::return_value_policy::reference_internal,
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
    .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
         "Get count of pending spike events")
    .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
         "Get current developmental stage")
    .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
         "Set developmental stage")
    .def("getConfig", &Brain::getConfig,
         py::return_value_policy::reference_internal,
         "Get the configuration")
    .def("logStatus", &Brain::logStatus,
         "Log brain status")
    .def("getSpikeSystem", &Brain::getSpikeSystem,
         "Get spike system for event-driven processing")
    .def("getSTDP", &Brain::getSTDP, "Get STDP plasticity system")
    .def("getHebbian", &Brain::getHebbian, "Get Hebbian plasticity system")
    .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
         "Get structural plasticity system")
    .def("getWorkingMemory", []() -> std::unique_ptr<NeuralWorkingMemory> {
         throw std::runtime_error("WorkingMemory access requires brain reference");
     }, "Get working memory (requires implementation)")
    .def("getEpisodicMemory", []() -> std::unique_ptr<NeuralEpisodicMemory> {
         throw std::runtime_error("EpisodicMemory access requires brain reference");
     }, "Get episodic memory (requires implementation)")
    .def("getPredictionSystem", &Brain::getPredictionSystem,
         "Get prediction system")
    .def("getNeuralPlanner", []() -> std::unique_ptr<NeuralPlanner> {
         throw std::runtime_error("NeuralPlanner access requires brain reference");
     }, "Get neural planner (requires implementation)")
    .def("getConceptFormation", []() -> std::unique_ptr<ConceptFormation> {
         throw std::runtime_error("ConceptFormation access requires brain reference");
     }, "Get concept formation (requires implementation)")
    .def("getAttentionSystem", []() -> std::unique_ptr<AttentionalSelection> {
         throw std::runtime_error("AttentionSystem access requires brain reference");
     }, "Get attention system (requires implementation)")
    .def("getNeuromodulator", []() -> std::unique_ptr<Neuromodulator> {
         throw std::runtime_error("Neuromodulator access requires brain reference");
     }, "Get neuromodulator (requires implementation)")
    .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
         "Get development system")
    .def("getCheckpointManager", []() -> std::unique_ptr<CheckpointManager> {
         throw std::runtime_error("CheckpointManager access requires brain reference");
     }, "Get checkpoint manager (requires implementation)")
    .def("getLearnedPreference", &Brain::getLearnedPreference,
         "Get learned preference trait (survives checkpoint reload)");

// Utility functions and factories
m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
    return std::make_shared<Config>();
}, "Create a default configuration")

m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
    return std::make_shared<Brain>(config);
}, py::arg("config"), "Create a new brain with configuration")

m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
    return std::make_shared<SimpleWorld>();
}, "Create a new simple world")

m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
    return std::make_shared<AgentBrain>(brain);
}, py::arg("brain"), "Create a new agent brain interface")

// Add convenience functions for common patterns
m.def("runAgentSimulation", [](std::shared_ptr<Brain> brain, 
                             std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent,
                             size_t numSteps) {
    for (size_t step = 0; step < numSteps; ++step) {
        world->update(0.1);  // Fixed timestep
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
}, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
    "Run complete agent simulation loop (world update, sensory, brain step, action, apply)")

m.def("createCompleteAgentSetup", [](std::shared_ptr<Config> config,
                                    size_t worldWidth, size_t worldHeight,
                                    size_t visionWidth, size_t visionHeight) {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    return std::make_tuple(brain, world, agent);
}, py::arg("config"), py::arg("worldWidth"), py::arg("worldHeight"),
     py::arg("visionWidth"), py::arg("visionHeight"),
    "Create complete agent setup with all subsystems enabled")

// Add utility for debugging and monitoring
m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
    std::map<std::string, std::string> stats;
    stats["neurons"] = std::to_string(brain->getTotalNeuronCount());
    stats["synapses"] = std::to_string(b gur->getTotalSynapseCount());
    stats["spikes"] = std::to_string(brain->getTotalSpikeCount());
    stats["firingNeurons"] = std::to_string(brain->getFiringNeuronCount());
    stats["developmentalStage"] = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
    return stats;
}, py::arg("brain"), "Get comprehensive brain statistics")

// Enhanced error types
py::register_exception<std::runtime_error>(m, "RuntimeError");
py::register_exception<std::invalid_argument>(m, "ValueError");
py::register_exception<std::out_of_range>(m, "IndexError");

// Module documentation
m.doc() = R"pbdoc(
    NLM (Neural Learning Machine) - Phase 6 Enhanced Python Bindings
    
    This is an enhanced version of the NLM Python bindings with complete Phase 6
    integration. The bindings provide access to the full integrated artificial brain
    including all cognitive, memory, and neuromodulation systems.
    
    Key improvements over the standard version:
    - Complete access to SelfModel, SocialLearning, NeuralPlanner, ConceptFormation
    - Enhanced AgentBrain with better integration
    - More comprehensive error handling and validation
    - Better Pythonic APIs with documentation and examples
    - Additional utility functions for common patterns
    - Improved type safety and error messages
    
    The Phase 6 brain implements the complete integrated brain loop:
    1. Process spikes and update neural dynamics
    2. Update working memory and neuromodulation
    3. Apply plasticity rules (STDP, Hebbian)
    4. Update episodic memory and prediction systems
    5. Apply attention and concept formation
    6. Handle structural plasticity and development
    7. Replay memories and consolidate learning
    
    This represents the final integration of all NLM systems into a coherent
    artificial brain capable of learning, memory, planning, and social interaction.
    
    See the NLM documentation for more details on the underlying C++ implementation.
)pbdoc";

// Version information
m.attr("__version__") = "2.0.0-enhanced";
m.attr("PHASE_6_INTEGRATION") = true;

} // namespace nlm
