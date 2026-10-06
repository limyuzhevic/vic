// Enhanced AgentBrain Cognitive Integration Tests
// Tests for NeuralPlanner, ConceptFormation, and AttentionalSelection integration

#include "EnhancedAgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_cognitive_integration {

void testCognitiveSystemInitialization() {
    std::cout << "    Testing cognitive system initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test that cognitive systems are initialized
    assert(agentBrain.getPlanner() != nullptr);
    assert(agentBrain.getConceptFormation() != nullptr);
    assert(agentBrain.getWorkingMemory() != nullptr);
    assert(agentBrain.getAttention() != nullptr);
    
    std::cout << "    testCognitiveSystemInitialization passed" << std::endl;
}

void testConceptBasedBehavior() {
    std::cout << "    Testing concept-based behavior..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create a simple sensory percept for testing
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.1f);  // Simple vision pattern
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    agentBrain.processSensoryInput(percept);
    
    // Test concept-based action selection
    std::vector<float> neuralState = agentBrain.extractNeuralState();
    nlm::MotorCommand conceptAction = agentBrain.decodeConceptBasedAction(neuralState);
    
    // Concept-based action should be valid
    assert(conceptAction == nlm::MotorCommand::Wait || 
           conceptAction == nlm::MotorCommand::MoveForward ||
           conceptAction == nlm::MotorCommand::Interact);
    
    std::cout << "    testConceptBasedBehavior passed" << std::endl;
}

void testAttentionGuidedActionSelection() {
    std::cout << "    Testing attention-guided action selection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Process sensory input to create neural activity
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.1f);
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    agentBrain.processSensoryInput(percept);
    
    // Test attention-guided action selection
    nlm::MotorCommand attentionAction = agentBrain.decodeAttentionGuidedAction();
    
    // Attention action should be valid
    assert(attentionAction == nlm::MotorCommand::Wait || 
           attentionAction == nlm::MotorCommand::MoveForward);
    
    // Check that attention focus level is updated
    float attentionFocus = agentBrain.getAttentionFocusLevel();
    assert(attentionFocus >= 0.0f && attentionFocus <= 1.0f);
    
    std::cout << "    testAttentionGuidedActionSelection passed" << std::endl;
}

void testPredictionBasedPlanning() {
    std::cout << "    Testing prediction-based planning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create test neural state
    std::vector<float> neuralState(10, 0.2f);
    
    // Test prediction-based action selection
    nlm::MotorCommand predictedAction = agentBrain.decodePredictionBasedAction(neuralState);
    
    // Predicted action should be valid
    assert(predictedAction == nlm::MotorCommand::Wait || 
           predictedAction == nlm::MotorCommand::MoveForward ||
           predictedAction == nlm::MotorCommand::MoveBackward ||
           predictedAction == nlm::MotorCommand::TurnLeft ||
           predictedAction == nlm::MotorCommand::TurnRight ||
           predictedAction == nlm::MotorCommand::Interact);
    
    // Check that prediction accuracy is updated
    float predictionAccuracy = agentBrain.getPredictionAccuracy();
    assert(predictionAccuracy >= 0.0f && predictionAccuracy <= 1.0f);
    
    std::cout << "    testPredictionBasedPlanning passed" << std::endl;
}

void testEnhancedMotorCommandSelection() {
    std::cout << "    Testing enhanced motor command selection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Process sensory input
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.1f);
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    agentBrain.processSensoryInput(percept);
    
    // Test enhanced motor command selection
    nlm::MotorCommand enhancedAction = agentBrain.decodeMotorCommand();
    
    // Enhanced action should be valid
    assert(enhancedAction == nlm::MotorCommand::Wait || 
           enhancedAction == nlm::MotorCommand::MoveForward ||
           enhancedAction == nlm::MotorCommand::MoveBackward ||
           enhancedAction == nlm::MotorCommand::TurnLeft ||
           enhancedAction == nlm::MotorCommand::TurnRight ||
           enhancedAction == nlm::MotorCommand::Interact);
    
    // Test cognitive confidence
    float cognitiveConfidence = agentBrain.getCognitiveConfidence();
    assert(cognitiveConfidence >= 0.0f && cognitiveConfidence <= 1.0f);
    
    std::cout << "    testEnhancedMotorCommandSelection passed" << std::endl;
}

void testCognitiveEvaluationWithNeuromodulators() {
    std::cout << "    Testing cognitive evaluation with neuromodulators..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(0.8f, 0.5f);  // Positive reward
    
    // Check that neuromodulation affects cognitive systems
    float predictionError = agentBrain.getPredictionError();
    float neuromodulationLevel = agentBrain.getNeuromodulationLevel();
    
    // Prediction error should be reward - predictedReward = 0.3
    assert(std::abs(predictionError - 0.3f) < 0.01f);
    
    // Neuromodulation level should reflect prediction error
    assert(std::abs(neuromodulationLevel - 0.3f) < 0.01f);
    
    // Apply negative reward modulation
    agentBrain.applyRewardModulation(0.2f, 0.5f);  // Negative reward
    
    // Check that negative reward produces negative prediction error
    predictionError = agentBrain.getPredictionError();
    neuromodulationLevel = agentBrain.getNeuromodulationLevel();
    
    assert(std::abs(predictionError + 0.3f) < 0.01f);
    assert(std::abs(neuromodulationLevel + 0.3f) < 0.01f);
    
    std::cout << "    testCognitiveEvaluationWithNeuromodulators passed" << std::endl;
}

void testConceptStabilityAndBehaviorWeights() {
    std::cout << "    Testing concept stability and behavior weights..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Check initial concept stability
    float initialStability = agentBrain.getConceptStability();
    assert(initialStability == 0.0f);
    
    // Process some sensory input to develop concepts
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.1f);
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    for (int i = 0; i < 5; ++i) {
        agentBrain.processSensoryInput(percept);
    }
    
    // Check that concept stability is updated
    float updatedStability = agentBrain.getConceptStability();
    assert(updatedStability >= 0.0f && updatedStability <= 1.0f);
    
    std::cout << "    testConceptStabilityAndBehaviorWeights passed" << std::endl;
}

void testIntegrationWithPredictionSystem() {
    std::cout << "    Testing integration with prediction system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Enable prediction planning
    agentBrain.enablePredictionPlanning(true);
    assert(agentBrain.isPredictionPlanningEnabled() == true);
    
    // Create test neural state
    std::vector<float> neuralState(10, 0.2f);
    
    // Test prediction-based action selection
    nlm::MotorCommand predictedAction = agentBrain.decodePredictionBasedAction(neuralState);
    
    // Should produce valid action
    assert(predictedAction == nlm::MotorCommand::Wait || 
           predictedAction == nlm::MotorCommand::MoveForward ||
           predictedAction == nlm::MotorCommand::MoveBackward ||
           predictedAction == nlm::MotorCommand::TurnLeft ||
           predictedAction == nlm::MotorCommand::TurnRight ||
           predictedAction == nlm::MotorCommand::Interact);
    
    std::cout << "    testIntegrationWithPredictionSystem passed" << std::endl;
}

void testIntegrationWithMemorySystem() {
    std::cout << "    Testing integration with memory system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Process sensory input to update memory
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.1f);
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    agentBrain.processSensoryInput(percept);
    
    // Check that working memory is being updated
    std::vector<float> neuralState = agentBrain.extractNeuralState();
    assert(!neuralState.empty());
    
    std::cout << "    testIntegrationWithMemorySystem passed" << std::endl;
}

void testIntegrationWithAttentionSystem() {
    std::cout << "    Testing integration with attention system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Enable attention guidance
    agentBrain.enableAttentionGuidance(true);
    assert(agentBrain.isAttentionGuidanceEnabled() == true);
    
    // Process sensory input
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.1f);
    std::vector<float> touch;
    std::vector<float> internal;
    std::vector<float> proprioception;
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    agentBrain.processSensoryInput(percept);
    
    // Test attention-guided action selection
    nlm::MotorCommand attentionAction = agentBrain.decodeAttentionGuidedAction();
    
    // Should produce valid action
    assert(attentionAction == nlm::MotorCommand::Wait || 
           attentionAction == nlm::MotorCommand::MoveForward);
    
    // Check attention focus level
    float attentionFocus = agentBrain.getAttentionFocusLevel();
    assert(attentionFocus >= 0.0f && attentionFocus <= 1.0f);
    
    std::cout << "    testIntegrationWithAttentionSystem passed" << std::endl;
}

void testConfigurationOptions() {
    std::cout << "    Testing configuration options..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Enable different cognitive features
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    agentBrain.enableCognitivePlanning(true);
    agentBrain.enableConceptBasedBehavior(true);
    agentBrain.enableAttentionGuidance(true);
    agentBrain.enablePredictionPlanning(true);
    
    // Verify configuration options
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    assert(agentBrain.isCognitivePlanningEnabled() == true);
    assert(agentBrain.isConceptBasedBehaviorEnabled() == true);
    assert(agentBrain.isAttentionGuidanceEnabled() == true);
    assert(agentBrain.isPredictionPlanningEnabled() == true);
    
    std::cout << "    testConfigurationOptions passed" << std::endl;
}

void testResetFunctionality() {
    std::cout << "    Testing reset functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::EnhancedAgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Store some state
    agentBrain.getPlanner(); // This should initialize if needed
    
    // Apply some neuromodulation to create state
    agentBrain.applyRewardModulation(0.8f, 0.5f);
    
    // Reset
    agentBrain.reset();
    
    // Check that reset worked
    float predictionError = agentBrain.getPredictionError();
    float neuromodulationLevel = agentBrain.getNeuromodulationLevel();
    
    assert(predictionError == 0.0f);
    assert(neuromodulationLevel == 0.0f);
    
    // Check cognitive state
    float cognitiveConfidence = agentBrain.getCognitiveConfidence();
    float conceptStability = agentBrain.getConceptStability();
    float attentionFocus = agentBrain.getAttentionFocusLevel();
    
    assert(cognitiveConfidence == 0.0f);
    assert(conceptStability == 0.0f);
    assert(attentionFocus == 0.0f);
    
    std::cout << "    testResetFunctionality passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced AgentBrain Cognitive Integration Tests:" << std::endl;
    std::cout << std::endl;
    
    testCognitiveSystemInitialization();
    testConceptBasedBehavior();
    testAttentionGuidedActionSelection();
    testPredictionBasedPlanning();
    testEnhancedMotorCommandSelection();
    testCognitiveEvaluationWithNeuromodulators();
    testConceptStabilityAndBehaviorWeights();
    testIntegrationWithPredictionSystem();
    testIntegrationWithMemorySystem();
    testIntegrationWithAttentionSystem();
    testConfigurationOptions();
    testResetFunctionality();
    
    std::cout << std::endl;
    std::cout << "=== All Cognitive Integration Tests PASSED ===" << std::endl;
}

} // namespace test_cognitive_integration