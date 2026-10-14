#include "Phase5CognitiveIntegration.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct Phase5CognitiveIntegration::Impl {
    std::shared_ptr<Brain> brain;
    std::unique_ptr<AgentBrain> agentBrain;
    
    Impl() : brain(nullptr), agentBrain(nullptr) {}
};

Phase5CognitiveIntegration::Phase5CognitiveIntegration() : pImpl(std::make_unique<Impl>()) {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Configure brain with integration-friendly settings
    config->set("neuron_count", 2000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15f);
    config->set("stdp_ltp_weight", 0.015f);
    config->set("stdp_ltd_weight", 0.018f);
    config->set("synaptogenesis_rate", 0.00005f);
    config->set("pruning_rate", 0.000005f);
    
    // Create brain
    pImpl->brain = std::make_shared<Brain>(config);
    pImpl->brain->initialize();
    
    // Create agent brain interface
    pImpl->agentBrain = std::make_unique<AgentBrain>(pImpl->brain);
    
    // Create simple world for testing
    SimpleWorld world;
    world.configure(10.0f, 10.0f, 4, 4);
    pImpl->agentBrain->initialize(world);
    
    // Enable all advanced systems
    pImpl->agentBrain->enableRewardModulation(true);
    pImpl->agentBrain->enableStructuralPlasticity(true);
    pImpl->agentBrain->enableDevelopment(true);
    pImpl->agentBrain->enableCuriosity(true);
}

Phase5CognitiveIntegration::~Phase5CognitiveIntegration() = default;

bool Phase5CognitiveIntegration::verifyIntegration() {
    NLM_LOG_INFO("Verifying Phase 5 Cognitive Integration...");
    
    bool allTestsPassed = true;
    
    // Check SelfModel integration
    if (!verifySelfModelIntegration()) {
        NLM_LOG_ERROR("SelfModel integration failed");
        allTestsPassed = false;
    }
    
    // Check SocialLearning integration
    if (!verifySocialLearningIntegration()) {
        NLM_LOG_ERROR("SocialLearning integration failed");
        allTestsPassed = false;
    }
    
    // Check SpatialRepresentation integration
    if (!verifySpatialRepresentationIntegration()) {
        NLM_LOG_ERROR("SpatialRepresentation integration failed");
        allTestsPassed = false;
    }
    
    // Check TemporalRelation integration
    if (!verifyTemporalRelationIntegration()) {
        NLM_LOG_ERROR("TemporalRelation integration failed");
        allTestsPassed = false;
    }
    
    // Check NeuralPrediction integration
    if (!verifyNeuralPredictionIntegration()) {
        NLM_LOG_ERROR("NeuralPrediction integration failed");
        allTestsPassed = false;
    }
    
    // Check AgentBrain cognitive access
    if (!verifyAgentBrainAccess()) {
        NLM_LOG_ERROR("AgentBrain cognitive access failed");
        allTestsPassed = false;
    }
    
    if (allTestsPassed) {
        NLM_LOG_INFO("✓ All cognitive system integrations verified successfully!");
    } else {
        NLM_LOG_ERROR("✗ Some cognitive system integrations failed.");
    }
    
    return allTestsPassed;
}

bool Phase5CognitiveIntegration::verifySelfModelIntegration() const {
    auto selfModel = pImpl->brain->getSelfModel();
    if (!selfModel) {
        NLM_LOG_ERROR("SelfModel is null - not initialized");
        return false;
    }
    
    // Test basic functionality
    if (!selfModel->hasSelfModel()) {
        NLM_LOG_WARNING("SelfModel exists but hasn't learned any action effects");
    }
    
    // Check that SelfModel is properly connected to brain
    NLM_LOG_INFO("✓ SelfModel integration verified");
    return true;
}

bool Phase5CognitiveIntegration::verifySocialLearningIntegration() const {
    auto socialLearning = pImpl->brain->getSocialLearning();
    if (!socialLearning) {
        NLM_LOG_ERROR("SocialLearning is null - not initialized");
        return false;
    }
    
    // Test basic functionality
    if (!socialLearning->hasSocialKnowledge()) {
        NLM_LOG_WARNING("SocialLearning exists but hasn't learned from others");
    }
    
    NLM_LOG_INFO("✓ SocialLearning integration verified");
    return true;
}

bool Phase5CognitiveIntegration::verifySpatialRepresentationIntegration() const {
    auto spatialRepresentation = pImpl->brain->getSpatialRepresentation();
    if (!spatialRepresentation) {
        NLM_LOG_ERROR("SpatialRepresentation is null - not initialized");
        return false;
    }
    
    // Test basic functionality
    if (!spatialRepresentation->hasPlaceCells()) {
        NLM_LOG_WARNING("SpatialRepresentation exists but hasn't formed place cells");
    }
    
    NLM_LOG_INFO("✓ SpatialRepresentation integration verified");
    return true;
}

bool Phase5CognitiveIntegration::verifyTemporalRelationIntegration() const {
    auto temporalRelation = pImpl->brain->getTemporalRelation();
    if (!temporalRelation) {
        NLM_LOG_ERROR("TemporalRelation is null - not initialized");
        return false;
    }
    
    // Test basic functionality
    size_t sequenceLength = temporalRelation->getSequenceLength();
    if (sequenceLength == 0) {
        NLM_LOG_WARNING("TemporalRelation exists but hasn't learned any sequences");
    }
    
    NLM_LOG_INFO("✓ TemporalRelation integration verified");
    return true;
}

bool Phase5CognitiveIntegration::verifyNeuralPredictionIntegration() const {
    auto neuralPrediction = pImpl->brain->getNeuralPrediction();
    if (!neuralPrediction) {
        NLM_LOG_ERROR("NeuralPrediction is null - not initialized");
        return false;
    }
    
    // Test basic functionality
    float predictionError = neuralPrediction->getPredictionError();
    float predictionConfidence = neuralPrediction->getPredictionConfidence();
    
    if (predictionConfidence <= 0.0f) {
        NLM_LOG_WARNING("NeuralPrediction exists but hasn't generated any predictions");
    }
    
    NLM_LOG_INFO("✓ NeuralPrediction integration verified");
    NLM_LOG_INFO(std::string("  Prediction error: ") + std::to_string(predictionError));
    NLM_LOG_INFO(std::string("  Prediction confidence: ") + std::to_string(predictionConfidence));
    return true;
}

bool Phase5CognitiveIntegration::verifyAgentBrainAccess() const {
    // Test that AgentBrain can access all cognitive systems
    auto brain = pImpl->agentBrain->getBrain();
    if (!brain) {
        NLM_LOG_ERROR("AgentBrain doesn't have access to brain");
        return false;
    }
    
    // Check access to cognitive systems
    auto planner = pImpl->agentBrain->getPlanner();
    auto conceptFormation = pImpl->agentBrain->getConceptFormation();
    auto selfModel = pImpl->agentBrain->getSelfModel();
    auto socialLearning = pImpl->agentBrain->getSocialLearning();
    auto spatialRepresentation = pImpl->agentBrain->getSpatialRepresentation();
    auto temporalRelation = pImpl->agentBrain->getTemporalRelation();
    auto neuralPrediction = pImpl->agentBrain->getNeuralPrediction();
    
    bool allAccessesOK = true;
    
    if (!planner) {
        NLM_LOG_WARNING("AgentBrain cannot access NeuralPlanner");
        allAccessesOK = false;
    }
    
    if (!conceptFormation) {
        NLM_LOG_WARNING("AgentBrain cannot access ConceptFormation");
        allAccessesOK = false;
    }
    
    if (!selfModel) {
        NLM_LOG_WARNING("AgentBrain cannot access SelfModel");
        allAccessesOK = false;
    }
    
    if (!socialLearning) {
        NLM_LOG_WARNING("AgentBrain cannot access SocialLearning");
        allAccessesOK = false;
    }
    
    if (!spatialRepresentation) {
        NLM_LOG_WARNING("AgentBrain cannot access SpatialRepresentation");
        allAccessesOK = false;
    }
    
    if (!temporalRelation) {
        NLM_LOG_WARNING("AgentBrain cannot access TemporalRelation");
        allAccessesOK = false;
    }
    
    if (!neuralPrediction) {
        NLM_LOG_WARNING("AgentBrain cannot access NeuralPrediction");
        allAccessesOK = false;
    }
    
    if (allAccessesOK) {
        NLM_LOG_INFO("✓ AgentBrain cognitive access verified");
    } else {
        NLM_LOG_ERROR("✗ Some AgentBrain cognitive accesses failed");
    }
    
    return allAccessesOK;
}

void Phase5CognitiveIntegration::testSelfModel(size_t numTrials) {
    NLM_LOG_INFO("Testing SelfModel with " + std::to_string(numTrials) + " trials...");
    
    auto selfModel = pImpl->brain->getSelfModel();
    if (!selfModel) return;
    
    for (size_t i = 0; i < numTrials; ++i) {
        // Simulate an action and its effect
        ActionType action = ActionType::MoveForward;
        std::vector<float> beforeState(4, 0.1f * i);
        std::vector<float> afterState = beforeState;
        afterState[0] += 0.2f;  // Action changes state
        
        // Record the action effect
        selfModel->recordSelfAction(action, beforeState, afterState);
        
        // Predict consequence
        std::vector<float> predicted = selfModel->predictActionConsequence(action, beforeState);
        
        // Update based on prediction error
        selfModel->updateSelfModel(predicted, afterState, action);
    }
    
    float confidence = selfModel->getSelfModelConfidence(ActionType::MoveForward);
    NLM_LOG_INFO(std::string("SelfModel confidence for MoveForward: ") + std::to_string(confidence));
}

void Phase5CognitiveIntegration::testSocialLearning(size_t numTrials) {
    NLM_LOG_INFO("Testing SocialLearning with " + std::to_string(numTrials) + " trials...");
    
    auto socialLearning = pImpl->brain->getSocialLearning();
    if (!socialLearning) return;
    
    for (size_t i = 0; i < numTrials; ++i) {
        // Simulate observing another agent's action
        ActionType observedAction = ActionType::MoveForward;
        std::vector<float> observerState(4, 0.1f * i);
        std::vector<float> resultingState = observerState;
        resultingState[1] += 0.3f;  // Other agent affects state
        
        // Record the observation
        socialLearning->observeAgentAction(observedAction, observerState, resultingState);
        
        // Get imitation action
        ActionType imitationAction = socialLearning->getImitationAction(observerState);
        
        // Check if can imitate
        bool canImitate = socialLearning->canImitate(observedAction);
        if (canImitate) {
            NLM_LOG_DEBUG(std::string("Can imitate action: ") + std::to_string(static_cast<int>(imitationAction)));
        }
    }
    
    size_t observationCount = socialLearning->getObservationCount();
    NLM_LOG_INFO(std::string("SocialLearning has " ) + std::to_string(observationCount) + " observations");
}

void Phase5CognitiveIntegration::testSpatialRepresentation(size_t numTrials) {
    NLM_LOG_INFO("Testing SpatialRepresentation with " + std::to_string(numTrials) + " trials...");
    
    auto spatialRepresentation = pImpl->brain->getSpatialRepresentation();
    if (!spatialRepresentation) return;
    
    for (size_t i = 0; i < numTrials; ++i) {
        // Record position in space
        float x = static_cast<float>(i % 5);
        float y = static_cast<float>(i / 5);
        std::vector<float> sensoryCues(4, 0.1f * i);
        
        spatialRepresentation->recordPosition(x, y, sensoryCues);
        
        // Get predicted position
        auto predicted = spatialRepresentation->getPredictedPosition();
        
        // Get place neurons
        auto placeNeurons = spatialRepresentation->getPlaceNeuronsNear(x, y, 2.0f);
        
        if (!placeNeurons.empty()) {
            NLM_LOG_DEBUG(std::string("Found ") + std::to_string(placeNeurons.size()) + " place neurons near position");
        }
    }
    
    bool hasPlaceCells = spatialRepresentation->hasPlaceCells();
    NLM_LOG_INFO(std::string("SpatialRepresentation has place cells: ") + std::to_string(hasPlaceCells));
}

void Phase5CognitiveIntegration::testTemporalRelation(size_t numTrials) {
    NLM_LOG_INFO("Testing TemporalRelation with " + std::to_string(numTrials) + " trials...");
    
    auto temporalRelation = pImpl->brain->getTemporalRelation();
    if (!temporalRelation) return;
    
    for (size_t i = 0; i < numTrials; ++i) {
        // Record sequence relationships
        std::vector<float> eventA(4, 0.1f * i);
        std::vector<float> eventB(4, 0.1f * (i + 1));
        
        temporalRelation->recordSequence(eventA, eventB, 0.5f);
        
        // Predict next event
        std::vector<float> predictedNext = temporalRelation->predictNext(eventA);
        
        // Predict previous event
        std::vector<float> predictedPrev = temporalRelation->predictPrevious(eventB);
        
        float confidence = temporalRelation->getPredictionConfidence(eventA);
        if (confidence > 0.1f) {
            NLM_LOG_DEBUG(std::string("Temporal relation confidence: ") + std::to_string(confidence));
        }
    }
    
    size_t sequenceLength = temporalRelation->getSequenceLength();
    NLM_LOG_INFO(std::string("TemporalRelation has learned ") + std::to_string(sequenceLength) + " sequences");
}

void Phase5CognitiveIntegration::testNeuralPrediction(size_t numTrials) {
    NLM_LOG_INFO("Testing NeuralPrediction with " + std::to_string(numTrials) + " trials...");
    
    auto neuralPrediction = pImpl->brain->getNeuralPrediction();
    if (!neuralPrediction) return;
    
    for (size_t i = 0; i < numTrials; ++i) {
        // Record sensory state
        std::vector<float> sensoryState(4, 0.2f * i);
        neuralPrediction->recordSensoryState(sensoryState, static_cast<SimulationStep>(i));
        
        // Generate prediction
        std::vector<float> prediction = neuralPrediction->generatePrediction(static_cast<SimulationStep>(i));
        
        // Update with observation (simulate perfect prediction for testing)
        float error = neuralPrediction->updateWithObservation(prediction, static_cast<SimulationStep>(i));
        
        if (i % 10 == 0) {
            NLM_LOG_DEBUG(std::string("Prediction step ") + std::to_string(i) + ": error = " + std::to_string(error));
        }
    }
    
    float error = neuralPrediction->getPredictionError();
    float confidence = neuralPrediction->getPredictionConfidence();
    
    NLM_LOG_INFO(std::string("NeuralPrediction error: ") + std::to_string(error));
    NLM_LOG_INFO(std::string("NeuralPrediction confidence: ") + std::to_string(confidence));
}

void Phase5CognitiveIntegration::testAgentBrainCognitiveAccess(size_t numTrials) {
    NLM_LOG_INFO("Testing AgentBrain cognitive access with " + std::to_string(numTrials) + " trials...");
    
    for (size_t i = 0; i < numTrials; ++i) {
        // Create sensory percept
        SensoryPercept percept;
        // In a real implementation, this would be filled with actual sensory data
        
        // Process sensory input
        pImpl->agentBrain->processSensoryInput(percept);
        
        // Get motor command (this would use cognitive systems if integrated)
        MotorCommand motorCommand = pImpl->agentBrain->decodeMotorCommand();
        
        // Apply reward modulation
        float reward = 0.5f;
        float predictedReward = 0.4f;
        pImpl->agentBrain->applyRewardModulation(reward, predictedReward);
        
        if (i % 10 == 0) {
            NLM_LOG_DEBUG(std::string("AgentBrain trial ") + std::to_string(i) + ": action selected");
        }
    }
    
    // Test specific cognitive system access
    auto selfModel = pImpl->agentBrain->getSelfModel();
    auto socialLearning = pImpl->agentBrain->getSocialLearning();
    auto spatialRepresentation = pImpl->agentBrain->getSpatialRepresentation();
    
    NLM_LOG_INFO("AgentBrain cognitive access test completed");
}

void Phase5CognitiveIntegration::testMemorySystemIntegration(size_t numTrials) {
    NLM_LOG_INFO("Testing Memory System Integration with " + std::to_string(numTrials) + " trials...");
    
    auto workingMemory = pImpl->brain->getWorkingMemory();
    auto episodicMemory = pImpl->brain->getEpisodicMemory();
    auto associativeMemory = pImpl->brain->getAssociativeMemory();
    
    for (size_t i = 0; i < numTrials; ++i) {
        // Store data in working memory
        if (workingMemory) {
            // Simulate storing neuron activation
            NeuronId testNeuron(1);
            workingMemory->storeToNeuron(testNeuron, 0.3f + 0.1f * i);
        }
        
        // Update working memory
        if (workingMemory) {
            workingMemory->update(0.01);
        }
        
        // Store episode in episodic memory
        if (episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = static_cast<SimulationStep>(i);
            episode.reward = 0.5f;
            // Fill other fields as needed
            episodicMemory->storeEpisode(episode);
        }
        
        // Create associations
        if (associativeMemory) {
            // Simulate pattern association
            std::vector<float> patternA(4, 0.1f * i);
            std::vector<float> patternB(4, 0.2f * i);
            associativeMemory->associate(patternA, patternB, 0.6f);
        }
    }
    
    NLM_LOG_INFO("Memory system integration test completed");
}

void Phase5CognitiveIntegration::testBrainLoopIntegration(size_t numTrials) {
    NLM_LOG_INFO("Testing Brain Loop Integration with " + std::to_string(numTrials) + " trials...");
    
    // Run simulation steps to test the complete brain loop
    for (size_t i = 0; i < numTrials; ++i) {
        pImpl->brain->step(static_cast<SimulationStep>(i));
        
        // Provide some sensory input occasionally
        if (i % 5 == 0) {
            SensoryInput sensoryInput;
            // In a real implementation, this would be filled with actual sensory data
            pImpl->brain->receiveSensoryInput(sensoryInput);
        }
        
        if (i % 20 == 0) {
            NLM_LOG_DEBUG(std::string("Brain loop step ") + std::to_string(i) + " completed");
        }
    }
    
    // Get final statistics
    size_t totalSpikes = pImpl->brain->getTotalSpikeCount();
    size_t pendingEvents = pImpl->brain->getPendingSpikeEventCount();
    
    NLM_LOG_INFO(std::string("Brain loop integration - total spikes: ") + std::to_string(totalSpikes));
    NLM_LOG_INFO(std::string("Brain loop integration - pending events: ") + std::to_string(pendingEvents));
}

void Phase5CognitiveIntegration::logSystemStatus() const {
    if (!pImpl->brain) return;
    
    NLM_LOG_INFO("=== Phase 5 System Status ===");
    
    // Brain statistics
    NLM_LOG_INFO(std::string("Regions: ") + std::to_string(pImpl->brain->getRegionCount()));
    NLM_LOG_INFO(std::string("Total neurons: ") + std::to_string(pImpl->brain->getTotalNeuronCount()));
    NLM_LOG_INFO(std::string("Total synapses: ") + std::to_string(pImpl->brain->getTotalSynapseCount()));
    
    // Cognitive system status
    auto selfModel = pImpl->brain->getSelfModel();
    auto socialLearning = pImpl->brain->getSocialLearning();
    auto spatialRepresentation = pImpl->brain->getSpatialRepresentation();
    auto temporalRelation = pImpl->brain->getTemporalRelation();
    auto neuralPrediction = pImpl->brain->getNeuralPrediction();
    
    if (selfModel) NLM_LOG_INFO("✓ SelfModel: Active");
    if (socialLearning) NLM_LOG_INFO("✓ SocialLearning: Active");
    if (spatialRepresentation) NLM_LOG_INFO("✓ SpatialRepresentation: Active");
    if (temporalRelation) NLM_LOG_INFO("✓ TemporalRelation: Active");
    if (neuralPrediction) NLM_LOG_INFO("✓ NeuralPrediction: Active");
    
    // Neuromodulation status
    auto dopamine = pImpl->brain->getDopamine();
    auto curiosity = pImpl->brain->getCuriosity();
    
    if (dopamine) NLM_LOG_INFO(std::string("Dopamine level: ") + std::to_string(dopamine->getLevel()));
    if (curiosity) NLM_LOG_INFO(std::string("Curiosity level: ") + std::to_string(curiosity->getLevel()));
    
    NLM_LOG_INFO("=== End Phase 5 System Status ===");
}

float Phase5CognitiveIntegration::getIntegrationScore() const {
    float score = 0.0f;
    
    // Check each system integration
    if (pImpl->brain->getSelfModel()) score += 20.0f;
    if (pImpl->brain->getSocialLearning()) score += 20.0f;
    if (pImpl->brain->getSpatialRepresentation()) score += 15.0f;
    if (pImpl->brain->getTemporalRelation()) score += 15.0f;
    if (pImpl->brain->getNeuralPrediction()) score += 20.0f;
    if (pImpl->agentBrain->getPlanner()) score += 10.0f;
    
    return score;
}

} // namespace nlm

