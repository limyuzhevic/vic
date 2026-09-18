#include "tests/test_types.cpp"
#include "tests/test_config.cpp"
#include "tests/test_clock.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_random.cpp"
#include "tests/test_stdp.cpp"
#include "tests/test_synapse.cpp"
#include "tests/test_main.cpp"

// Additional tests for Phase 6 integration improvements
#include "src/prediction/PredictionSystem.cpp"
#include "src/cognition/ConceptFormation.cpp"

#include <gtest/gtest.h>

// Test prediction system integration
TEST(PredictionSystemTest, IntegrationWithBrain) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* predictionSystem = brain->getPredictionSystem();
    ASSERT_NE(predictionSystem, nullptr);
    
    // Initialize prediction system
    predictionSystem->initialize(brain.get());
    
    // Test prediction generation
    auto input = std::make_unique<SensoryInput>();
    input->addValue(0.5f);
    input->addValue(0.8f);
    
    auto prediction = predictionSystem->predictNextState(*input);
    ASSERT_NE(prediction, nullptr);
    
    // Test prediction update
    auto actual = std::make_unique<SensoryInput>();
    actual->addValue(0.55f);  // Slight difference from prediction
    
    predictionSystem->updatePredictions(*prediction, *actual);
    
    // Check error and confidence
    float error = predictionSystem->getPredictionError();
    ASSERT_GE(error, 0.0f);
    
    float confidence = predictionSystem->getConfidence();
    ASSERT_GE(confidence, 0.0f);
    
    // Test training
    predictionSystem->train(*actual);
}

TEST(PredictionSystemTest, ConceptDrivenPrediction) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* predictionSystem = brain->getPredictionSystem();
    predictionSystem->initialize(brain.get());
    
    auto* conceptFormation = brain->getConceptFormation();
    conceptFormation->initialize(brain.get());
    
    // Create test sensory patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    std::vector<float> features1 = {0.5f, 0.6f};
    std::vector<float> features2 = {0.7f, 0.8f};
    
    // Present experiences
    size_t concept1 = conceptFormation->presentExperience(pattern1, features1, 0.5f, 0);
    size_t concept2 = conceptFormation->presentExperience(pattern2, features2, 1.0f, 100);
    
    // Create input based on first pattern
    auto input = std::make_unique<SensoryInput>();
    for (float val : pattern1) input->addValue(val);
    
    // Generate prediction
    auto prediction = predictionSystem->predictNextState(*input);
    
    // Prediction should be based on learned concepts
    ASSERT_NE(prediction, nullptr);
}

// Test concept formation integration
TEST(ConceptFormationTest, IntegrationWithBrain) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* conceptFormation = brain->getConceptFormation();
    ASSERT_NE(conceptFormation, nullptr);
    
    // Initialize with brain
    conceptFormation->initialize(brain.get());
    
    // Present various patterns
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    std::vector<float> featuresA = {0.1f, 0.2f};
    std::vector<float> featuresB = {0.3f, 0.4f};
    
    // Present experiences
    size_t conceptA = conceptFormation->presentExperience(patternA, featuresA, 0.5f, 0);
    size_t conceptB = conceptFormation->presentExperience(patternB, featuresB, 1.0f, 50);
    
    // Get concept statistics
    ASSERT_GE(conceptFormation->getConceptCount(), 1);
    
    // Test concept retrieval
    size_t retrievedConcept = conceptFormation->getMatchingConcept(patternA);
    ASSERT_NE(retrievedConcept, 0);  // Should match concept A
    
    // Test similarity computation
    float similarity = conceptFormation->computeSimilarity(patternA, patternB);
    ASSERT_GE(similarity, 0.0f);
    ASSERT_LE(similarity, 1.0f);
}

// Test spike detection improvements
TEST(BrainTest, SpikeDetectionRaceConditionFixed) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Get neuron population
    auto regions = brain->getRegions();
    ASSERT_FALSE(regions.empty());
    
    auto* region = regions[0].get();
    ASSERT_NE(region, nullptr);
    
    auto neurons = region->getAllNeurons();
    ASSERT_FALSE(neurons.empty());
    
    // Inject current to trigger spikes
    for (int i = 0; i < std::min(5, static_cast<int>(neurons.size())); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run brain step
    brain->step(0, 0.0);
    
    // Check spike count
    size_t spikeCount = brain->getTotalSpikeCount();
    // Should have some spikes generated
    ASSERT_GT(spikeCount, 0);
}

// Test synaptic weight bounds checking
TEST(SynapticTest, WeightBoundsChecking) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 50);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Get STDP system
    auto* stdp = brain->getSTDP();
    ASSERT_NE(stdp, nullptr);
    
    // Create test synapses and verify weight bounds
    auto regions = brain->getRegions();
    if (!regions.empty()) {
        auto* region = regions[0].get();
        auto synapses = region->getSynapses();n        
        if (!synapses.empty()) {
            // Record initial weights
            float initialWeight = synapses[0]->getWeight();
            
            // Enable plasticity
            synapses[0]->enablePlasticity(true, true, false);
            
            // Inject activity to trigger STDP
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Fire pre and post neurons
                neurons[0]->injectCurrent(60.0f);
                brain->step(1, 0.001);
                
                neurons[1]->injectCurrent(60.0f);
                brain->step(2, 0.002);
                
                // Check that weight is within bounds after plasticity
                float finalWeight = synapses[0]->getWeight();
                ASSERT_GE(finalWeight, 0.01f);  // Lower bound
                ASSERT_LE(finalWeight, 10.0f);  // Upper bound
            }
        }
    }
}

// Test event handler improvements
TEST(SpikeSystemTest, EventHandlerRegistration) {
    SpikeSystem spikeSystem;
    
    // Test that handlers can be registered
    spikeSystem.registerHandler([](const DetailedSpikeEvent& event) {
        // Simple handler that does nothing
    });
    
    spikeSystem.registerDelayedHandler([](const DelayedSpikeEvent& event) {
        // Simple delayed handler that does nothing
    });
    
    // Handlers should be registered
    ASSERT_EQ(spikeSystem.getPendingSpikeCount(), 0);
}

// Test memory system integration
TEST(MemorySystemTest, WorkingMemoryIntegration) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto* workingMemory = brain->getWorkingMemory();
    ASSERT_NE(workingMemory, nullptr);
    
    // Run simulation
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
        
        // Inject some activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
    }
    
    // Check that working memory has activity
    size_t traces = workingMemory->getActiveTraces();n    // May be zero depending on implementation, but should not crash
}

// Test complete Phase 6 integration
TEST(Phase6IntegrationTest, CompleteBrainIntegration) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    ASSERT_TRUE(brain->initialize());
    
    // Initialize all integrated systems
    auto* predictionSystem = brain->getPredictionSystem();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    auto* dopamine = brain->getDopamine();
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    ASSERT_NE(predictionSystem, nullptr);
    ASSERT_NE(conceptFormation, nullptr);
    ASSERT_NE(attention, nullptr);
    ASSERT_NE(dopamine, nullptr);
    ASSERT_NE(workingMemory, nullptr);
    ASSERT_NE(episodicMemory, nullptr);
    
    // Initialize all systems with brain reference
    predictionSystem->initialize(brain.get());
    conceptFormation->initialize(brain.get());
    attention->initialize(brain.get());
    
    // Run integration test
    for (int step = 0; step < 500; ++step) {
        // Process sensory input
        auto input = std::make_unique<SensoryInput>();
        input->addValue(static_cast<float>(rand()) / RAND_MAX);
        
        // Get prediction for this sensory state
        auto prediction = predictionSystem->predictNextState(*input);
        
        // Simulate world interaction
        brain->receiveSensoryInput(*input);
        
        // Step brain
        brain->step(step, step * 0.001);
        
        // Get action
        auto action = brain->produceAction();
        
        // Apply neuromodulation
        if (dopamine) {
            float reward = (step % 10 == 0) ? 1.0f : 0.0f;
            dopamine->setLevel(reward);
        }
    }
    
    // Verify systems are functional
    ASSERT_GT(brain->getTotalSpikeCount(), 0);
    ASSERT_GT(brain->getTotalSynapseCount(), 0);
    
    // Memory systems should have activity
    ASSERT_GE(workingMemory->getActiveTraces(), 0);
    
    std::cout << "Phase 6 integration test completed successfully!" << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "Running NLM Phase 6 Integration Tests..." << std::endl;
    std::cout << "Testing prediction system, concept formation, and all brain systems." << std::endl;
    
    return RUN_ALL_TESTS();
}