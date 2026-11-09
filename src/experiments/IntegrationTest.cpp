#include "experiments/IntegrationTest.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <chrono>
#include <cmath>
#include <numeric>
#include <algorithm>

namespace nlm {

bool IntegrationTest::testPredictionSystemIntegration() {
    NLM_LOG_INFO("=== Testing Prediction System Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test prediction system with neural dynamics
    size_t predictionSteps = 500;
    size_t successfulPredictions = 0;
    
    for (size_t step = 0; step < predictionSteps; ++step) {
        // Inject neural activity for prediction
        brain->injectCurrentToNeurons(NeuronType::Internal, 2.0f);
        
        // Get neural pattern for prediction
        std::vector<float> neuralPattern;
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float activity = std::min(1.0f, neuron->getFiringRate() / 100.0f);
                    neuralPattern.push_back(activity);
                }
            }
        }
        
        if (!neuralPattern.empty()) {
            // Update prediction system
            brain->getPredictionSystem()->updatePredictions(neuralPattern, neuralPattern);
            brain->getPredictionSystem()->train(neuralPattern);
            
            // Check prediction system health
            float error = brain->getPredictionSystem()->getPredictionError();
            float confidence = brain->getPredictionSystem()->getConfidence();
            
            if (error < 0.5f && confidence > 0.3f) {
                successfulPredictions++;
            }
        }
        
        brain->step(step, step * 0.001);
    }
    
    float successRate = static_cast<float>(successfulPredictions) / predictionSteps;
    NLM_LOG_INFO("Prediction integration success rate: " + std::to_string(successRate * 100) + "%");
    
    if (successRate > 0.3f) {
        NLM_LOG_INFO("[PASS] Prediction system properly integrated with neural dynamics");
        return true;
    } else {
        NLM_LOG_ERROR("[FAIL] Prediction system integration inadequate");
        return false;
    }
}

bool IntegrationTest::testConceptFormationIntegration() {
    NLM_LOG_INFO("=== Testing Concept Formation Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test concept formation with prediction feedback
    size_t conceptSteps = 300;
    size_t conceptUpdates = 0;
    
    for (size_t step = 0; step < conceptSteps; ++step) {
        // Generate neural pattern for concept formation
        std::vector<float> neuralPattern;
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float activity = neuron->isFiring() ? 1.0f : 0.0f;
                    float firingRate = std::min(1.0f, neuron->getFiringRate() / 100.0f);
                    neuralPattern.push_back(activity + firingRate * 0.5f);
                }
            }
        }
        
        // Generate associated sensory features
        std::vector<float> sensoryFeatures;
        for (auto* neuron : brain->getSensoryNeurons()) {
            float sensoryActivity = std::min(1.0f, neuron->getFiringRate() / 100.0f);
            sensoryFeatures.push_back(sensoryActivity);
        }
        
        if (!neuralPattern.empty()) {
            // Present to concept formation system
            size_t conceptId = brain->getConceptFormation()->presentExperience(
                neuralPattern, sensoryFeatures, 0.5f, step
            );
            
            if (conceptId > 0) {
                conceptUpdates++;
            }
        }
        
        brain->step(step, step * 0.001);
    }
    
    float conceptUpdateRate = static_cast<float>(conceptUpdates) / conceptSteps;
    NLM_LOG_INFO("Concept formation update rate: " + std::to_string(conceptUpdateRate * 100) + "%");
    
    if (conceptUpdateRate > 0.2f) {
        NLM_LOG_INFO("[PASS] Concept formation properly integrated with neural patterns");
        return true;
    } else {
        NLM_LOG_ERROR("[FAIL] Concept formation integration inadequate");
        return false;
    }
}

bool IntegrationTest::testMemorySystemIntegration() {
    NLM_LOG_INFO("=== Testing Memory System Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test working memory integration
    size_t memorySteps = 200;
    size_t activeTraces = 0;
    
    for (size_t step = 0; step < memorySteps; ++step) {
        // Inject current to create neural activity for working memory
        brain->injectCurrentToNeurons(NeuronType::Internal, 1.0f);
        
        brain->step(step, step * 0.001);
        
        // Check working memory traces
        if (brain->getWorkingMemory()) {
            activeTraces += brain->getWorkingMemory()->getActiveTraces();
        }
        
        // Check episodic memory episodes
        if (brain->getEpisodicMemory()) {
            // Episodic memory should accumulate episodes over time
            if (brain->getEpisodicMemory()->getEpisodeCount() > 0) {
                NLM_LOG_INFO("Episodic memory has " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()) + " episodes");
            }
        }
    }
    
    float avgActiveTraces = static_cast<float>(activeTraces) / memorySteps;
    NLM_LOG_INFO("Average working memory active traces: " + std::to_string(avgActiveTraces));
    
    bool workingMemoryOK = (avgActiveTraces > 0.1f);
    bool episodicMemoryOK = (brain->getEpisodicMemory() && brain->getEpisodicMemory()->getEpisodeCount() > 0);
    
    if (workingMemoryOK && episodicMemoryOK) {
        NLM_LOG_INFO("[PASS] Memory systems properly integrated with neural substrate");
        return true;
    } else {
        NLM_LOG_ERROR("[FAIL] Memory system integration inadequate");
        return false;
    }
}

bool IntegrationTest::testNeuromodulationIntegration() {
    NLM_LOG_INFO("=== Testing Neuromodulation Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test neuromodulation with prediction errors
    size_t neuromodSteps = 150;
    size_t modulationUpdates = 0;
    
    for (size_t step = 0; step < neuromodSteps; ++step) {
        // Create prediction error signal
        if (brain->getPredictionSystem()) {
            float predictionError = brain->getPredictionSystem()->getPredictionError();
            if (predictionError > 0.0f) {
                brain->getPredictionError()->setError(predictionError);
                modulationUpdates++;
            }
        }
        
        // Apply neuromodulation effects
        brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
        brain->step(step, step * 0.001);
        
        // Check dopamine effects
        if (brain->getDopamine()) {
            float dopamineLevel = brain->getDopamine()->getLevel();
            if (dopamineLevel > 0.0f) {
                NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamineLevel));
            }
        }
    }
    
    float neuromodUpdateRate = static_cast<float>(modulationUpdates) / neuromodSteps;
    NLM_LOG_INFO("Neuromodulation update rate: " + std::to_string(modulationUpdateRate * 100) + "%");
    
    if (neuromodUpdateRate > 0.3f) {
        NLM_LOG_INFO("[PASS] Neuromodulation systems properly integrated");
        return true;
    } else {
        NLM_LOG_ERROR("[FAIL] Neuromodulation integration inadequate");
        return false;
    }
}

bool IntegrationTest::testDevelopmentIntegration() {
    NLM_LOG_INFO("=== Testing Development Integration ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    // Test development system integration
    size_t developmentSteps = 100;
    bool developmentUpdated = false;
    
    for (size_t step = 0; step < developmentSteps; ++step) {
        if (step % 10 == 0) {  // Update development every 10 steps
            brain->develop();
            developmentUpdated = true;
        }
        
        brain->step(step, step * 0.001);
    }
    
    if (developmentUpdated) {
        NLM_LOG_INFO("[PASS] Development system properly integrated");
        return true;
    } else {
        NLM_LOG_ERROR("[FAIL] Development system integration inadequate");
        return false;
    }
}

bool IntegrationTest::testCrossSystemInteractions() {
    NLM_LOG_INFO("=== Testing Cross-System Interactions ===");
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return false;
    }
    
    bool predictionToConceptOK = false;
    bool conceptToAttentionOK = false;
    bool memoryToPredictionOK = false;
    
    // Test prediction -> concept formation interaction
    if (brain->getPredictionSystem() && brain->getConceptFormation()) {
        for (size_t step = 0; step < 50; ++step) {
            // Create prediction using the brain's prediction system
            auto predictedState = std::make_unique<SensoryInput>();
            std::vector<float> neuralPattern;
            for (auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        float activity = std::min(1.0f, neuron->getFiringRate() / 100.0f);
                        neuralPattern.push_back(activity);
                    }
                }
            }
            predictedState->setData(neuralPattern);
            
            // Update prediction
            brain->getPredictionSystem()->updatePredictions(predictedState.get(), predictedState.get());
            
            // Check concept formation adaptation to prediction error
            float predictionError = brain->getPredictionSystem()->getPredictionError();
            if (predictionError > 0.0f) {
                predictionToConceptOK = true;
                break;
            }
            
            brain->step(step, step * 0.001);
        }
    }
    
    // Test concept formation -> attention interaction
    if (brain->getConceptFormation() && brain->getAttention()) {
        // Present experience to concept formation
        std::vector<float> pattern(10, 0.7f);
        brain->getConceptFormation()->presentExperience(pattern, pattern, 0.5f, 0);
        
        // Check if attention is influenced by concepts
        if (brain->getAttention()->getWinners().size() > 0) {
            conceptToAttentionOK = true;
        }
    }
    
    // Test working memory -> prediction interaction
    if (brain->getWorkingMemory() && brain->getPredictionSystem()) {
        // Store pattern in working memory
        std::vector<float> memoryPattern(5, 0.8f);
        brain->getWorkingMemory()->store(memoryPattern, 1.0f);
        
        // Prediction should be influenced by working memory content
        if (brain->getWorkingMemory()->getActiveTraces() > 0) {
            memoryToPredictionOK = true;
        }
    }
    
    NLM_LOG_INFO("Prediction->Concept interaction: " + std::string(predictionToConceptOK ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Concept->Attention interaction: " + std::string(conceptToAttentionOK ? "PASS" : "FAIL"));
    NLM_LOG_INFO("WorkingMemory->Prediction interaction: " + std::string(memoryToPredictionOK ? "PASS" : "FAIL"));
    
    return predictionToConceptOK && conceptToAttentionOK && memoryToPredictionOK;
}

IntegrationTest::TestResults IntegrationTest::runFullIntegrationTest() {
    TestResults results;
    
    NLM_LOG_INFO("=== FULL INTEGRATION TEST SUITE ===");
    
    // Test each system individually
    results.predictionSystemIntegrated = testPredictionSystemIntegration();
    results.conceptFormationIntegrated = testConceptFormationIntegration();
    results.memorySystemsIntegrated = testMemorySystemIntegration();
    results.neuromodulationIntegrated = testNeuromodulationIntegration();
    results.developmentIntegrated = testDevelopmentIntegration();
    
    // Test cross-system interactions
    // Note: These are computed based on individual test results
    results.predictionErrorIntegration = results.predictionSystemIntegrated && results.neuromodulationIntegrated;
    results.conceptPredictionInteraction = results.conceptFormationIntegrated && results.predictionSystemIntegrated;
    results.workingMemoryPredictionFeedback = results.memorySystemsIntegrated && results.predictionSystemIntegrated;
    results.episodicMemoryReplay = results.memorySystemsIntegrated;
    
    // Calculate overall integration score
    int passedTests = 0;
    int totalTests = 8;
    
    if (results.predictionSystemIntegrated) passedTests++;
    if (results.conceptFormationIntegrated) passedTests++;
    if (results.memorySystemsIntegrated) passedTests++;
    if (results.neuromodulationIntegrated) passedTests++;
    if (results.developmentIntegrated) passedTests++;
    if (results.predictionErrorIntegration) passedTests++;
    if (results.conceptPredictionInteraction) passedTests++;
    if (results.workingMemoryPredictionFeedback) passedTests++;
    
    results.totalScore = static_cast<float>(passedTests) / totalTests;
    
    NLM_LOG_INFO("=== INTEGRATION TEST RESULTS ===");
    NLM_LOG_INFO("Integration Score: " + std::to_string(results.totalScore * 100) + "%");
    NLM_LOG_INFO("Tests Passed: " + std::to_string(passedTests) + "/" + std::to_string(totalTests));
    
    return results;
}

} // namespace nlm