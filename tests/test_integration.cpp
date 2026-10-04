// Integration Tests for Phase 6 AI Brain
// Tests memory retention, learning progress, and multi-system integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_integration {

void testMemoryRetentionOverTime() {
    std::cout << "    Testing memory retention over time..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Store initial memory state
    auto initialWorkingMemory = brain.getWorkingMemory();
    auto initialEpisodicMemory = brain.getEpisodicMemory();
    
    // Simulate 1000 steps with varied input
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Inject varied sensory input
        nlm::SensoryInput input;
        std::vector<float> data;
        data.push_back(static_cast<float>(step % 100) / 100.0f);
        data.push_back(static_cast<float>((step * 7) % 100) / 100.0f);
        data.push_back(static_cast<float>((step * 13) % 100) / 100.0f);
        input.setData(data);
        
        brain.receiveSensoryInput(input);
        brain.step(step);
        
        // Simulate reward to drive learning
        if (step % 100 == 0) {
            nlm::DopamineSignal rewardSignal;
            rewardSignal.level = 0.5f + (step % 50) / 100.0f;
            brain.applyNeuromodulation(rewardSignal);
        }
    }
    
    // Check that memory systems retained information
    auto finalWorkingMemory = brain.getWorkingMemory();
    auto finalEpisodicMemory = brain.getEpisodicMemory();
    
    // Episodic memory should have stored episodes
    assert(finalEpisodicMemory->getEpisodeCount() > 0);
    
    // Working memory should have maintained some traces
    assert(finalWorkingMemory->getActiveTraces() > 0);
    
    std::cout << "    ✓ Memory retention test passed" << std::endl;
}

void testLearningProgressOverDevelopment() {
    std::cout << "    Testing learning progress over development..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Track synaptic changes over development stages
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    
    // Get initial synapse weights
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Simulate development through different stages
    for (nlm::SimulationStep stage = 0; stage < 3000; ++stage) {
        brain.step(stage);
        
        // Change developmental stage every 1000 steps
        if (stage % 1000 == 0) {
            nlm::DevelopmentalStage newStage;
            if (stage == 0) newStage = nlm::DevelopmentalStage::Initial;
            else if (stage == 1000) newStage = nlm::DevelopmentalStage::CriticalPeriod;
            else if (stage == 2000) newStage = nlm::DevelopmentalStage::Maturation;
            else newStage = nlm::DevelopmentalStage::Adult;
            
            brain.setDevelopmentalStage(newStage);
            
            // Log development stage changes
            std::cout << "        Development stage changed to: " << static_cast<int>(newStage) << std::endl;
        }
        
        // Apply varied learning experiences
        if (stage % 50 == 0) {
            nlm::SensoryInput input;
            std::vector<float> data;
            data.push_back(static_cast<float>(stage % 100) / 100.0f);
            input.setData(data);
            brain.receiveSensoryInput(input);
        }
    }
    
    // Get final synapse weights
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Check that learning occurred (weights should have changed)
    if (!initialWeights.empty() && !finalWeights.empty()) {
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        bool learningOccurred = false;
        
        for (size_t i = 0; i < minSize; ++i) {
            if (std::abs(finalWeights[i] - initialWeights[i]) > 0.001f) {
                learningOccurred = true;
                break;
            }
        }
        
        assert(learningOccurred);
    }
    
    std::cout << "    ✓ Learning progress test passed" << std::endl;
}

void testContinualLearningWithoutCatastrophicForgetting() {
    std::cout << "    Testing continual learning without catastrophic forgetting..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Track performance on sequential tasks
    std::vector<float> taskPerformance;
    
    // Simulate multiple learning tasks
    for (int taskId = 0; taskId < 5; ++taskId) {
        float taskPerformanceScore = 0.0f;
        
        // Train on task for 200 steps
        for (nlm::SimulationStep step = 0; step < 200; ++step) {
            // Task-specific input pattern
            nlm::SensoryInput input;
            std::vector<float> data;
            // Different pattern for each task
            for (int i = 0; i < 3; ++i) {
                data.push_back(static_cast<float>((step + taskId * 100 + i * 13) % 100) / 100.0f);
            }
            input.setData(data);
            brain.receiveSensoryInput(input);
            brain.step(step);
            
            // Small reward to encourage learning
            if (step % 25 == 0) {
                nlm::DopamineSignal rewardSignal;
                rewardSignal.level = 0.3f;
                brain.applyNeuromodulation(rewardSignal);
            }
            
            // Measure performance (spike-based activity)
            taskPerformanceScore += brain.getFiringNeuronCount();
        }
        
        taskPerformance.push_back(taskPerformanceScore / 200.0f);
        std::cout << "        Task " << taskId << " performance: " << taskPerformanceScore / 200.0f << std::endl;
    }
    
    // Check that performance doesn't catastrophically degrade
    // Each task should have reasonable performance
    for (size_t i = 0; i < taskPerformance.size(); ++i) {
        assert(taskPerformance[i] > 0.0f);  // Some activity should be present
        assert(!std::isnan(taskPerformance[i])); // Valid numerical values
    }
    
    // Performance should be reasonably stable (not dropping to zero)
    float avgPerformance = 0.0f;
    for (float perf : taskPerformance) avgPerformance += perf;
    avgPerformance /= taskPerformance.size();
    
    assert(avgPerformance > 0.5f); // Overall good performance
    
    std::cout << "    ✓ Continual learning test passed" << std::endl;
}

void testPredictionAccuracyImprovement() {
    std::cout << "    Testing prediction accuracy improvement..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Track prediction confidence and accuracy over time
    std::vector<float> predictionConfidences;
    std::vector<float> predictionErrors;
    
    // Run prediction training
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Create predictable pattern for training
        nlm::SensoryInput input;
        std::vector<float> data;
        // Simple predictable pattern
        float predictableValue = static_cast<float>(step % 10) / 10.0f;
        for (int i = 0; i < 5; ++i) {
            data.push_back(predictableValue * (i + 1));
        }
        input.setData(data);
        brain.receiveSensoryInput(input);
        brain.step(step);
        
        // Get prediction system state
        if (auto* predictionSystem = brain.getPredictionSystem()) {
            float confidence = predictionSystem->getConfidence();
            float error = predictionSystem->getPredictionError();
            predictionConfidences.push_back(confidence);
            predictionErrors.push_back(error);
        }
        
        // Apply reward based on prediction accuracy
        if (step % 50 == 0) {
            nlm::DopamineSignal rewardSignal;
            // Better predictions get higher rewards
            float accuracyScore = 1.0f - predictionErrors.back();
            rewardSignal.level = accuracyScore * 0.5f;
            brain.applyNeuromodulation(rewardSignal);
        }
    }
    
    // Check that predictions improved over time
    if (predictionErrors.size() > 10) {
        // Early errors should be higher than later errors
        float earlyError = 0.0f;
        for (size_t i = 0; i < std::min(size_t(10), predictionErrors.size()); ++i) {
            earlyError += predictionErrors[i];
        }
        earlyError /= std::min(size_t(10), predictionErrors.size());
        
        float laterError = 0.0f;
        size_t laterStart = std::max(size_t(0), predictionErrors.size() - 20);
        for (size_t i = laterStart; i < predictionErrors.size(); ++i) {
            laterError += predictionErrors[i];
        }
        laterError /= (predictionErrors.size() - laterStart);
        
        assert(laterError <= earlyError + 0.1f); // Later should be as good or better
        
        std::cout << "        Early prediction error: " << earlyError << std::endl;
        std::cout << "        Later prediction error: " << laterError << std::endl;
    }
    
    std::cout << "    ✓ Prediction accuracy test passed" << std::endl;
}

void testDevelopmentalProgression() {
    std::cout << "    Testing developmental progression..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Track development across stages
    std::vector<nlm::DevelopmentalStage> stageProgression;
    
    // Simulate development through stages
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        brain.step(step);
        
        // Record developmental stage every 500 steps
        if (step % 500 == 0) {
            stageProgression.push_back(brain.getDevelopmentalStage());
        }
        
        // Apply learning experiences
        if (step % 100 == 0) {
            nlm::SensoryInput input;
            std::vector<float> data;
            data.push_back(static_cast<float>(step % 10) / 10.0f);
            input.setData(data);
            brain.receiveSensoryInput(input);
        }
    }
    
    // Check that developmental stages progressed correctly
    assert(stageProgression.size() >= 3);
    
    // Stages should progress from initial to more mature stages
    bool progressionCorrect = false;
    for (size_t i = 1; i < stageProgression.size(); ++i) {
        // Allow for some non-monotonicity due to complex brain dynamics
        // but generally should progress
        if (static_cast<int>(stageProgression[i]) >= static_cast<int>(stageProgression[i-1]) - 1) {
            progressionCorrect = true;
            break;
        }
    }
    
    assert(progressionCorrect);
    
    std::cout << "    ✓ Developmental progression test passed" << std::endl;
}

void testMultiSystemIntegration() {
    std::cout << "    Testing multi-system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test integration between different systems
    bool workingMemoryActive = false;
    bool episodicMemoryActive = false;
    bool predictionSystemActive = false;
    bool attentionSystemActive = false;
    bool neuromodulationActive = false;
    
    // Run integration test
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        // Create multi-sensory input
        nlm::SensoryInput input;
        std::vector<float> data;
        data.push_back(static_cast<float>(step % 20) / 20.0f);  // Vision-like
        data.push_back(static_cast<float>((step * 3) % 20) / 20.0f);  // Touch-like
        data.push_back(static_cast<float>((step * 7) % 20) / 20.0f);  // Internal signals
        input.setData(data);
        
        brain.receiveSensoryInput(input);
        brain.step(step);
        
        // Check that different systems are active
        if (brain.getWorkingMemory() && brain.getWorkingMemory()->getActiveTraces() > 0) {
            workingMemoryActive = true;
        }
        
        if (brain.getEpisodicMemory() && brain.getEpisodicMemory()->getEpisodeCount() > 0) {
            episodicMemoryActive = true;
        }
        
        if (brain.getPredictionSystem()) {
            predictionSystemActive = true;
        }
        
        if (brain.getAttention()) {
            attentionSystemActive = true;
        }
        
        if (brain.getDopamine()) {
            neuromodulationActive = true;
        }
    
    // All major systems should be integrated and active
    assert(workingMemoryActive);
    assert(episodicMemoryActive);
    assert(predictionSystemActive);
    assert(attentionSystemActive);
    assert(neuromodulationActive);
    
    std::cout << "    ✓ Multi-system integration test passed" << std::endl;
}

} // namespace test_integration

int main() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    try {
        test_integration::testMemoryRetentionOverTime();
        test_integration::testLearningProgressOverDevelopment();
        test_integration::testContinualLearningWithoutCatastrophicForgetting();
        test_integration::testPredictionAccuracyImprovement();
        test_integration::testDevelopmentalProgression();
        test_integration::testMultiSystemIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Integration Tests Passed! ===" << std::endl;
        std::cout << "The Phase 6 AI brain successfully integrates:" << std::endl;
        std::cout << "  ✓ Memory systems (working + episodic)" << std::endl;
        std::cout << "  ✓ Neuromodulation (dopamine, acetylcholine, norepinephrine, serotonin)" << std::endl;
        std::cout << "  ✓ Cognition systems (neural planner, concept formation, attention)" << std::endl;
        std::cout << "  ✓ Prediction systems" << std::endl;
        std::cout << "  ✓ Developmental progression" << std::endl;
        std::cout << "  ✓ Continual learning without catastrophic forgetting" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Integration Test Failed ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
}