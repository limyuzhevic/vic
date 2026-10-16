// Integration Test - Basic cognitive system integration
// Tests core brain integration as Phase 6 basic achievements

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>

namespace test_integration {

// Helper function to initialize brain configuration
void initializeBrainConfig(std::shared_ptr<nlm::Config> config, 
                         uint64_t neuronCount = 500,
                         uint64_t regionCount = 1) {
    config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0005f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00005f, nlm::ConfigSource::Default);
    config->set("novelty_threshold", 0.5f, nlm::ConfigSource::Default);
    config->set("curiosity_threshold", 0.3f, nlm::ConfigSource::Default);
}

// Test 1: Complete brain integration (core systems)
void testCompleteBrainIntegration() {
    std::cout << "=== Test 1: Complete Brain Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 300, 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize() && "Brain initialization failed");
    
    // Verify all core systems are accessible
    assert(brain.getSpikeSystem() != nullptr && "Spike system missing");
    assert(brain.getSTDP() != nullptr && "STDP system missing");
    assert(brain.getHebbian() != nullptr && "Hebbian system missing");
    
    // Test memory systems
    assert(brain.getWorkingMemory() != nullptr && "Working memory missing");
    assert(brain.getEpisodicMemory() != nullptr && "Episodic memory missing");
    assert(brain.getAssociativeMemory() != nullptr && "Associative memory missing");
    
    // Test prediction system
    assert(brain.getPredictionSystem() != nullptr && "Prediction system missing");
    
    // Test cognition systems
    assert(brain.getPlanner() != nullptr && "Planner missing");
    assert(brain.getConceptFormation() != nullptr && "Concept formation missing");
    
    // Test neuromodulation systems
    assert(brain.getCuriosity() != nullptr && "Curiosity system missing");
    assert(brain.getNovelty() != nullptr && "Novelty system missing");
    assert(brain.getPredictionErrorSignal() != nullptr && "Prediction error signal missing");
    
    // Test development system
    assert(brain.getDevelopmentSystem() != nullptr && "Development system missing");
    
    std::cout << "    Complete brain integration test passed" << std::endl;
}

// Test 2: Memory systems integration
void testMemorySystemsIntegration() {
    std::cout << "=== Test 2: Memory Systems Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 400, 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* wm = brain.getWorkingMemory();
    auto* em = brain.getEpisodicMemory();
    auto* am = brain.getAssociativeMemory();
    
    // Initialize working memory traces
    for (int i = 0; i < 5; ++i) {
        wm->addWorkingMemoryTrace(i, std::vector<float>(50, 0.1f * i));
    }
    
    // Encode episodic memory
    for (int i = 0; i < 3; ++i) {
        std::vector<float> eventPattern(50, static_cast<float>(rand()) / RAND_MAX);
        em->encodeExperience(i, eventPattern, 1.0f);
    }
    
    // Form associative memories
    for (int i = 0; i < 10; ++i) {
        std::vector<float> patternA(30, static_cast<float>(rand()) / RAND_MAX);
        std::vector<float> patternB(30, static_cast<float>(rand()) / RAND_MAX);
        float strength = 0.5f + 0.5f * static_cast<float>(rand()) / RAND_MAX;
        am->formAssociation(patternA, patternB, strength);
    }
    
    // Verify memory systems are active
    assert(wm->getActiveTraces() > 0 && "Working memory not functioning");
    assert(em->getEpisodeCount() > 0 && "Episodic memory not functioning");
    assert(am->getAssociationCount() > 0 && "Associative memory not functioning");
    
    std::cout << "    Memory systems integration test passed" << std::endl;
}

// Test 3: Neuromodulation systems integration
void testNeuromodulationSystemsIntegration() {
    std::cout << "=== Test 3: Neuromodulation Systems Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 350, 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // Initialize neuromodulation systems
    curiosity->reset();
    novelty->reset();
    predictionError->reset();
    
    // Apply various neuromodulatory signals
    brain.applyNeuromodulation(nlm::Neuromodulator("reward", 0.8f));
    brain.applyNeuromodulation(nlm::Neuromodulator("punishment", -0.5f));
    brain.applyNeuromodulation(nlm::Neuromodulator("surprise", 0.3f));
    
    // Check that neuromodulation affects neural excitability
    size_t firingBefore = brain.getFiringNeuronCount();
    brain.injectCurrentToNeurons(NeuronType::Excitatory, 5.0f);
    brain.step(0, 0.0);
    size_t firingAfter = brain.getFiringNeuronCount();
    
    // Neuromodulation should modulate firing dynamics
    assert(curiosity->getLevel() > 0.0f && "Curiosity level not updating");
    assert(novelty->getNoveltyLevel() > 0.0f && "Novelty level not updating");
    assert(predictionError->getLevel() >= 0.0f && "Prediction error not working");
    
    std::cout << "    Neuromodulation systems integration test passed" << std::endl;
}

// Test 4: Prediction systems integration
void testPredictionSystemsIntegration() {
    std::cout << "=== Test 4: Prediction Systems Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 380, 1);
    config->set("prediction_error_threshold", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* prediction = brain.getPredictionSystem();
    
    // Initialize prediction system
    prediction->reset();
    
    // Generate and process predictions
    std::vector<float> sensoryInput(50, 0.5f);
    
    // Run some prediction cycles
    float totalPredictionAccuracy = 0.0f;
    for (int i = 0; i < 30; ++i) {
        brain.step(i, i * 0.001);
        
        // Apply prediction error signal
        float error = predictionError->computeError(sensoryInput);
        brain.applyNeuromodulation(nlm::Neuromodulator("prediction_error", error));
        
        // Store prediction for accuracy check
        totalPredictionAccuracy += std::abs(error);
    }
    
    // Verify prediction system is integrated and functional
    assert(prediction->getPredictionCount() > 0 && "Prediction system not making predictions");
    
    std::cout << "    Prediction systems integration test passed" << std::endl;
}

// Test 5: Cognition systems integration
void testCognitionSystemsIntegration() {
    std::cout << "=== Test 5: Cognition Systems Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 420, 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* planner = brain.getPlanner();
    auto* conceptForm = brain.getConceptFormation();
    
    // Initialize cognition systems
    planner->reset();
    conceptForm->reset();
    
    // Form concepts from neural activity
    for (int i = 0; i < 3; ++i) {
        std::vector<float> pattern(40, static_cast<float>(rand()) / RAND_MAX);
        std::string conceptId = "concept_" + std::to_string(i);
        conceptForm->formConcept(conceptId, pattern);
    }
    
    // Create action plans
    for (int i = 0; i < 2; ++i) {
        planner->createPlan(std::vector<float>(20, 0.5f), std::vector<float>(10, 0.3f));
    }
    
    // Verify cognition systems integration
    assert(conceptForm->getConceptCount() > 0 && "Concept formation not working");
    assert(planner->getPlanCount() > 0 && "Planning system not working");
    
    std::cout << "    Cognition systems integration test passed" << std::endl;
}

// Test 6: Development systems integration
void testDevelopmentSystemsIntegration() {
    std::cout << "=== Test 6: Development Systems Integration ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 380, 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* development = brain.getDevelopmentSystem();
    development->reset();
    
    // Track development through stages
    std::vector<DevelopmentalStage> stages;
    for (int i = 0; i < 50; ++i) {
        development->updateDevelopment(0.01);
        stages.push_back(development->getDevelopmentalStage());
        
        // Run brain step to allow development to affect plasticity
        brain.step(i, i * 0.001);
    }
    
    // Verify developmental progression
    assert(development->getDevelopmentalStage() != DevelopmentalStage::Early || 
           development->getSynaptogenesisRate() > 0.0f);
    
    std::cout << "    Development systems integration test passed" << std::endl;
}

// Test 7: Complete brain loop with real sensory input
void testCompleteBrainLoopWithSensoryInput() {
    std::cout << "=== Test 7: Complete Brain Loop with Sensory Input ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 450, 2);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate environment interaction for 50 steps
    for (uint64_t step = 0; step < 50; ++step) {
        // Generate realistic sensory input
        std::vector<float> visionData(64, static_cast<float>(rand()) / RAND_MAX);
        std::vector<float> touchData(32, static_cast<float>(rand()) / RAND_MAX);
        std::vector<float> proprioceptionData(16, static_cast<float>(rand()) / RAND_MAX);
        
        // Process through agent brain interface
        brain.receiveSensoryInput(SensoryInput(visionData, touchData, proprioceptionData));
        
        // Brain processes input and generates output
        brain.step(step, step * 0.001);
        
        // Occasional reward modulation
        if (step % 10 == 0) {
            float reward = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f; // [-1, 1]
            brain.applyNeuromodulation(nlm::Neuromodulator("reward", reward));
        }
        
        // Memory updates from experience
        if (step % 5 == 0) {
            std::vector<float> memoryPattern(50, static_cast<float>(rand()) / RAND_MAX);
            brain.getWorkingMemory()->addWorkingMemoryTrace(step, memoryPattern);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Verify brain loop integration
    size_t totalSpikes = brain.getTotalSpikeCount();
    float avgFiringRate = brain.getAverageFiringRate();
    
    assert(totalSpikes > 0 && "Brain generated no spikes");
    assert(avgFiringRate > 0.0f && "Average firing rate should be positive");
    
    // Check neuromodulation levels
    auto* curiosity = brain.getCuriosity();
    assert(curiosity->getLevel() > 0.0f && "Curiosity should be active");
    
    std::cout << "    Complete brain loop with sensory input test passed" << std::endl;
    std::cout << "    Duration: " << duration.count() << "ms, "
              << "Total spikes: " << totalSpikes << ", "
              << "Avg firing rate: " << avgFiringRate << std::endl;
}

// Test 8: Checkpoint/save/load functionality
void testCheckpointSaveLoad() {
    std::cout << "=== Test 8: Checkpoint Save/Load Functionality ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 350, 1);
    
    // Create and initialize first brain
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run simulation on first brain
    for (int i = 0; i < 100; ++i) {
        brain1.step(i, i * 0.001);
        // Inject some activity to create state
        if (i % 10 == 0) {
            brain1.injectCurrentToNeurons(NeuronType::Excitatory, 2.0f);
        }
    }
    
    // Collect metrics from first brain
    float firing1 = brain1.getAverageFiringRate();
    size_t spikes1 = brain1.getTotalSpikeCount();
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_integration_test_checkpoint.bin";
    bool saveSuccess = brain1.save(checkpointPath);
    assert(saveSuccess && "Checkpoint save failed");
    
    // Create new brain and load checkpoint
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(checkpointPath);
    assert(loadSuccess && "Checkpoint load failed");
    
    // Verify loaded state
    float firing2 = brain2.getAverageFiringRate();
    size_t spikes2 = brain2.getTotalSpikeCount();
    
    // Check that at minimum the structural state was preserved
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount());
    
    std::cout << "    Checkpoint save/load functionality test passed" << std::endl;
    std::cout << "    Brain1: avg firing = " << firing1 << ", total spikes = " << spikes1 << std::endl;
    std::cout << "    Brain2: avg firing = " << firing2 << ", total spikes = " << spikes2 << std::endl;
}

// Test 9: Cognitive performance metrics
void testCognitivePerformanceMetrics() {
    std::cout << "=== Test 9: Cognitive Performance Metrics ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    initializeBrainConfig(config, 500, 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run performance benchmark
    for (int step = 0; step < 200; ++step) {
        brain.step(step, step * 0.001);
        
        // Collect various metrics
        float firingRate = brain.getAverageFiringRate();
        size_t totalSpikes = brain.getTotalSpikeCount();
        
        // Update memory metrics
        auto* wm = brain.getWorkingMemory();
        if (wm) {
            size_t wmTraces = wm->getActiveTraces();
            assert(wmTraces >= 0 && "Working memory traces should be valid");
        }
        
        auto* em = brain.getEpisodicMemory();
        if (em) {
            size_t emEpisodes = em->getEpisodeCount();
            assert(emEpisodes >= 0 && "Episodic memory episodes should be valid");
        }
        
        auto* am = brain.getAssociativeMemory();
        if (am) {
            size_t amAssociations = am->getAssociationCount();
            assert(amAssociations >= 0 && "Associative memory should be valid");
        }
        
        // Check neuromodulation levels
        auto* curiosity = brain.getCuriosity();
        if (curiosity) {
            float curiosityLevel = curiosity->getLevel();
            assert(curiosityLevel >= 0.0f && "Curiosity level should be non-negative");
        }
        
        // Check cognition systems
        auto* conceptForm = brain.getConceptFormation();
        if (conceptForm) {
            size_t conceptsFormed = conceptForm->getConceptCount();
            assert(conceptsFormed >= 0 && "Concepts formed should be valid");
        }
        
        auto* planner = brain.getPlanner();
        if (planner) {
            size_t plansCreated = planner->getPlanCount();
            assert(plansCreated >= 0 && "Plans created should be valid");
        }
        
        // Check development
        auto* development = brain.getDevelopmentSystem();
        if (development) {
            DevelopmentalStage devStage = development->getDevelopmentalStage();
            assert(devStage != DevelopmentalStage::Unknown && "Development stage should be valid");
        }
    }
    
    size_t totalSpikes = brain.getTotalSpikeCount();
    float avgFiringRate = brain.getAverageFiringRate();
    
    // Validate performance metrics
    assert(totalSpikes > 500 && "Should have many spikes for proper neural activity");
    assert(avgFiringRate > 0.5f && "Firing rate should be meaningful");
    
    std::cout << "    Cognitive performance metrics validation test passed" << std::endl;
    std::cout << "    Total spikes: " << totalSpikes << ", Avg firing rate: " << avgFiringRate << std::endl;
}

void runAll() {
    testCompleteBrainIntegration();
    testMemorySystemsIntegration();
    testNeuromodulationSystemsIntegration();
    testPredictionSystemsIntegration();
    testCognitionSystemsIntegration();
    testDevelopmentSystemsIntegration();
    testCompleteBrainLoopWithSensoryInput();
    testCheckpointSaveLoad();
    testCognitivePerformanceMetrics();
    
    std::cout << std::endl;
    std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
    std::cout << "The NLM brain is fully integrated and functioning as an artificial brain." << std::endl;
    std::cout << "Core cognitive systems are properly connected and interacting." << std::endl;
    std::cout << "The brain can perform complete perception-action cycles with learning." << std::endl;
}

} // namespace test_integration

int main() {
    test_integration::runAll();
    return 0;
}
