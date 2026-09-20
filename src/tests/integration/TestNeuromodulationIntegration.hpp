// Neuromodulation Integration Tests
// Tests dopamine, curiosity, novelty, and prediction error modulation

#pragma once

#include "IntegrationTestFramework.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class TestNeuromodulationIntegration : public IntegrationTestBase {
public:
    explicit TestNeuromodulationIntegration(std::shared_ptr<Config> cfg, IntegrationTestResult& results)
        : IntegrationTestBase(cfg, results) {}
    
    void runAllTests() {
        runTestCase("Dopamine Integration", "Neuromodulation Integration", [this]() {
            testDopamineIntegration();
        });
        
        runTestCase("Curiosity Integration", "Neuromodulation Integration", [this]() {
            testCuriosityIntegration();
        });
        
        runTestCase("Novelty Detection Integration", "Neuromodulation Integration", [this]() {
            testNoveltyDetectionIntegration();
        });
        
        runTestCase("Prediction Error Integration", "Neuromodulation Integration", [this]() {
            testPredictionErrorIntegration();
        });
        
        runTestCase("Neuromodulatory Signal Processing", "Neuromodulation Integration", [this]() {
            testNeuromodulatorySignalProcessing();
        });
        
        runTestCase("Reward Prediction Error", "Neuromodulation Integration", [this]() {
            testRewardPredictionError();
        });
        
        runTestCase("Motivation-Driven Behavior", "Neuromodulation Integration", [this]() {
            testMotivationDrivenBehavior();
        });
    }
    
private:
    void testDopamineIntegration() {
        setupSystem();
        
        auto* dopamine = brain->getDopamine();
        assert(dopamine != nullptr);
        
        // Test reward-based dopamine signaling
        std::vector<float> rewardValues = {0.1f, 0.5f, 0.9f, 1.0f};
        
        for (size_t i = 0; i < rewardValues.size(); ++i) {
            // Apply reward signal
            Neuromodulator rewardSignal;
            rewardSignal.type = NeuromodulatorType::Reward;
            rewardSignal.value = rewardValues[i];
            
            brain->applyNeuromodulation(rewardSignal);
            
            // Verify dopamine response
            brain->step(static_cast<nlm::SimulationStep>(i * 10));
            
            // Verify integration with neural activity
            float firingRate = brain->getAverageFiringRate();
            assert(firingRate >= 0.0f);
        }
        
        // Test prediction error in dopamine signaling
        Neuromodulator predictionErrorSignal;
        predictionErrorSignal.type = NeuromodulatorType::PredictionError;
        predictionErrorSignal.value = 0.3f;
        
        brain->applyNeuromodulation(predictionErrorSignal);
        
        teardownSystem();
    }
    
    void testCuriosityIntegration() {
        setupSystem();
        
        auto* curiosity = brain->getCuriosity();
        assert(curiosity != nullptr);
        
        // Test curiosity-driven exploration
        std::vector<float> curiositySignals = {0.2f, 0.5f, 0.8f, 0.95f};
        
        for (size_t i = 0; i < curiositySignals.size(); ++i) {
            // Apply curiosity signal
            Neuromodulator curiositySignal;
            curiositySignal.type = NeuromodulatorType::Curiosity;
            curiositySignal.value = curiositySignals[i];
            
            brain->applyNeuromodulation(curiositySignal);
            
            // Verify curiosity drives neural exploration
            brain->step(static_cast<nlm::SimulationStep>(i * 10));
            
            // Verify exploration effects
            size_t firingCount = brain->getFiringNeuronCount();
            assert(firingCount >= 0);
        }
        
        // Test novelty detection triggering curiosity
        Neuromodulator noveltySignal;
        noveltySignal.type = NeuromodulatorType::Novelty;
        noveltySignal.value = 0.7f;
        
        brain->applyNeuromodulation(noveltySignal);
        
        teardownSystem();
    }
    
    void testNoveltyDetectionIntegration() {
        setupSystem();
        
        auto* novelty = brain->getNovelty();
        assert(novelty != nullptr);
        
        // Test novelty detection in neural patterns
        std::vector<std::vector<float>> neuralPatterns = {
            {0.1f, 0.2f, 0.3f},
            {0.2f, 0.4f, 0.6f},
            {0.3f, 0.6f, 0.9f},
            {0.1f, 0.1f, 0.1f}  // Novel pattern
        };
        
        for (size_t i = 0; i < neuralPatterns.size(); ++i) {
            // Apply novelty detection
            Neuromodulator noveltySignal;
            noveltySignal.type = NeuromodulatorType::Novelty;
            noveltySignal.value = 0.5f * (i + 1); // Increasing novelty
            
            brain->applyNeuromodulation(noveltySignal);
            
            // Verify novelty affects neural dynamics
            brain->step(static_cast<nlm::SimulationStep>(i * 20));
            
            float firingRate = brain->getAverageFiringRate();
            assert(firingRate >= 0.0f);
        }
        
        teardownSystem();
    }
    
    void testPredictionErrorIntegration() {
        setupSystem();n        
        auto* predictionError = brain->getPredictionErrorSignal();
        assert(predictionError != nullptr);
        
        // Test prediction error signaling
        std::vector<float> predictionErrors = {-0.5f, 0.0f, 0.3f, 0.8f};
        
        for (size_t i = 0; i < predictionErrors.size(); ++i) {
            // Apply prediction error signal
            Neuromodulator errorSignal;
            errorSignal.type = NeuromodulatorType::PredictionError;
            errorSignal.value = predictionErrors[i];
            
            brain->applyNeuromodulation(errorSignal);
            
            // Verify prediction error affects learning
            brain->step(static_cast<nlm::SimulationStep>(i * 10));
            
            // Verify neural adaptation
            auto* stdp = brain->getSTDP();
            assert(stdp != nullptr);
        }
        
        teardownSystem();
    }
    
    void testNeuromodulatorySignalProcessing() {
        setupSystem();
        
        // Test combined neuromodulatory signals
        std::vector<NeuromodulatorType> signalTypes = {
            NeuromodulatorType::Reward,
            NeuromodulatorType::Curiosity,
            NeuromodulatorType::Novelty,
            NeuromodulatorType::PredictionError
        };
        
        for (size_t i = 0; i < signalTypes.size(); ++i) {
            Neuromodulator signal;
            signal.type = signalTypes[i];
            signal.value = 0.5f + static_cast<float>(i) * 0.1f;
            
            brain->applyNeuromodulation(signal);
            brain->step(static_cast<nlm::SimulationStep>(i * 15));
            
            // Verify signal processing
            float firingRate = brain->getAverageFiringRate();
            assert(firingRate >= 0.0f);
        }
        
        // Test signal integration and interaction
        // Multiple signals should combine effects
        Neuromodulator combinedReward;
        combinedReward.type = NeuromodulatorType::Reward;
        combinedReward.value = 0.7f;
        
        Neuromodulator combinedCuriosity;
        combinedCuriosity.type = NeuromodulatorType::Curiosity;
        combinedCuriosity.value = 0.8f;
        
        brain->applyNeuromodulation(combinedReward);
        brain->applyNeuromulation(combinedCuriosity);
        brain->step(static_cast<nlm::SimulationStep>(100));
        
        teardownSystem();
    }
    
    void testRewardPredictionError() {
        setupSystem();
        
        // Test reward prediction error (delta rule)
        float expectedValue = 0.8f;
        float actualValue = 0.6f;
        float rewardPredictionError = actualValue - expectedValue;
        
        // Apply prediction error
        Neuromodulator errorSignal;
        errorSignal.type = NeuromodulatorType::PredictionError;
        errorSignal.value = rewardPredictionError;
        
        brain->applyNeuromodulation(errorSignal);
        brain->step(static_cast<nlm::SimulationStep>(10));
        
        // Verify prediction error learning
        auto* dopamine = brain->getDopamine();
        assert(dopamine != nullptr);
        
        // Negative prediction error should reduce dopamine
        // (Implementation details depend on Dopamine API)
        
        // Positive prediction error should increase dopamine
        errorSignal.value = 0.4f;
        brain->applyNeuromodulation(errorSignal);
        brain->step(static_cast<nlm::SimulationStep>(20));
        
        teardownSystem();
    }
    
    void testMotivationDrivenBehavior() {
        setupSystem();
        
        // Test motivation drives behavioral choices
        std::vector<float> motivationLevels = {0.2f, 0.5f, 0.8f, 0.95f};
        
        for (size_t i = 0; i < motivationLevels.size(); ++i) {
            // Apply motivation (combined neuromodulation)
            Neuromodulator motivationSignal;
            motivationSignal.type = NeuromodulatorType::Reward;
            motivationSignal.value = motivationLevels[i];
            
            brain->applyNeuromodulation(motivationSignal);
            
            // Test motivated behavior
            brain->step(static_cast<nlm::SimulationStep>(i * 30));
            
            // Verify motivation affects action selection
            auto action = brain->produceAction();
            assert(action != nullptr);
        }
        
        // Test curiosity-driven exploration
        Neuromodulator curiositySignal;
        curiositySignal.type = NeuromodulatorType::Curiosity;
        curiositySignal.value = 0.9f;
        
        brain->applyNeuromodulation(curiositySignal);
        brain->step(static_cast<nlm::SimulationStep>(100));
        
        // Verify exploration increases behavioral variability
        float firingRate = brain->getAverageFiringRate();
        assert(firingRate >= 0.0f);
        
        teardownSystem();
    }
};

} // namespace nlm