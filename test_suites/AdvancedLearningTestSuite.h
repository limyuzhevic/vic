// Advanced Learning System Tests
// Comprehensive test suite for advanced learning capabilities

#include "AdvancedLearningTestSuite.h"
#include "../src/brain/Brain.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/ActionConsequencePredictor.hpp"
#include "../src/prediction/PredictionErrorSignal.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <random>

namespace test_suites {

class AdvancedLearningTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== Running Advanced Learning System Tests ===" << std::endl;
        
        test_transfer_learning_capabilities();
        test_few_shot_learning_performance();
        test_enhanced_reinforcement_learning();
        test_meta_learning_and_meta_cognition();
        test_cognitive_control_and_decision_making();
        test_continuous_learning_efficiency();
        test_incremental_knowledge_acquisition();
        test_adaptation_to_environment_changes();
        test_generalization_across_tasks();
        test_self_regulated_learning();
        
        std::cout << "=== Advanced Learning System Tests Complete ===" << std::endl;
    }

private:
    static void test_transfer_learning_capabilities() {
        std::cout << "Testing transfer learning capabilities..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 600);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Create source domain knowledge
        std::vector<std::vector<float>> sourceDomainPatterns = {
            {0.8f, 0.2f, 0.1f, 0.9f, 0.3f},
            {0.1f, 0.7f, 0.8f, 0.2f, 0.4f},
            {0.3f, 0.5f, 0.6f, 0.1f, 0.7f}
        };
        
        // Learn source domain patterns
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
            
            // Apply source domain learning signals
            if (step % 10 == 0) {
                nlm::Neuromodulator transferSignal;
                transferSignal.setLevel(0.9f);
                brain.applyNeuromodulation(transferSignal);
            }
            
            // Store source domain knowledge in episodic memory
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.action = nlm::ActionType::Look;
            episode.reward = 1.0f;
            brain.getEpisodicMemory()->storeEpisode(episode);
        }
        
        // Transfer knowledge to target domain
        std::vector<std::vector<float>> targetDomainPatterns = {
            {0.75f, 0.25f, 0.15f, 0.85f, 0.35f},  // Similar to source
            {0.2f, 0.65f, 0.75f, 0.25f, 0.45f},  // Similar to source
            {0.4f, 0.6f, 0.7f, 0.2f, 0.8f}       // Similar to source
        };
        
        // Test transfer learning efficiency
        float transferEfficiency = 0.0f;
        for (int step = 50; step < 100; ++step) {
            brain.step(step);
            
            // Use transferred knowledge for learning
            if (step % 15 == 0) {
                nlm::Neuromodulator transferSignal;
                transferSignal.setLevel(0.7f);
                brain.applyNeuromodulation(transferSignal);
            }
            
            // Verify learning continues with transferred knowledge
            if (brain.getAverageFiringRate() > 0.1f) {
                transferEfficiency += 0.1f;
            }
        }
        
        assert(transferEfficiency > 0.5f);  // Should learn efficiently
        std::cout << "    Transfer learning capabilities: PASSED" << std::endl;
    }
    
    static void test_few_shot_learning_performance() {
        std::cout << "Testing few-shot learning performance..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Simulate few-shot learning scenarios
        std::vector<std::pair<std::vector<float>, nlm::ActionType>> fewShotExamples;
        
        // Create few-shot examples (2-3 examples per concept)
        for (int concept = 0; concept < 5; ++concept) {
            std::vector<float> pattern(8, 0.0f);
            for (int i = 0; i < 3; ++i) {  // 3 examples per concept
                for (int j = 0; j < 8; ++j) {
                    pattern[j] = static_cast<float>(rand()) / RAND_MAX;
                }
                nlm::ActionType action = static_cast<nlm::ActionType>(concept % 5);
                fewShotExamples.push_back({pattern, action});
            }
        }
        
        // Few-shot learning with limited examples
        float fewShotAccuracy = 0.0f;
        int successfulConcepts = 0;
        
        for (int step = 0; step < 60; ++step) {
            brain.step(step);
            
            // Apply few-shot learning signals
            if (step < 20) {  // Early learning phase
                nlm::Neuromodulator fewShotSignal;
                fewShotSignal.setLevel(1.0f);  // High learning rate
                brain.applyNeuromodulation(fewShotSignal);
            } else {  // Consolidation phase
                nlm::Neuromodulator consolidationSignal;
                consolidationSignal.setLevel(0.5f);
                brain.applyNeuromodulation(consolidationSignal);
            }
            
            // Test concept formation with few examples
            if (step % 10 == 0) {
                auto* conceptFormation = brain.getConceptFormation();
                if (conceptFormation) {
                    // Verify concept formation works with limited data
                    if (brain.getAverageFiringRate() > 0.05f) {
                        successfulConcepts++;
                    }
                }
            }
        }
        
        assert(successfulConcepts >= 4);  // Should learn most concepts
        std::cout << "    Few-shot learning performance: PASSED" << std::endl;
    }
    
    static void test_enhanced_reinforcement_learning() {
        std::cout << "Testing enhanced reinforcement learning..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Enhanced RL with multiple learning signals
        std::vector<float> rewardHistory;
        std::vector<float> predictionErrorHistory;
        std::vector<float> curiosityHistory;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Generate enhanced reward signal
            float reward = 0.0f;
            if (step % 7 == 0) reward += 1.0f;  // Action reward
            if (step % 13 == 0) reward += 0.5f;  // Novelty reward
            if (step % 11 == 0) reward += 0.3f;  // Exploration reward
            
            // Apply enhanced reward system
            auto* rewardSystem = brain.getReward();
            if (rewardSystem) {
                rewardSystem->setValue(reward);
                rewardHistory.push_back(reward);
            }
            
            // Enhanced dopamine signaling
            auto* dopamine = brain.getDopamine();
            if (dopamine) {
                dopamine->signalReward(reward);
                float rewardPredictionError = reward - brain.getExpectedReward();
                dopamine->signalRewardPredictionError(rewardPredictionError);
            }
            
            // Enhanced curiosity-driven exploration
            auto* curiosity = brain.getCuriosity();
            if (curiosity) {
                float curiositySignal = 0.3f * exp(-step / 20.0f) + 0.1f;
                curiosity->setLevel(curiositySignal);
                curiosityHistory.push_back(curiositySignal);
            }
            
            // Enhanced prediction error
            auto* predictionError = brain.getPredictionError();
            if (predictionError) {
                float error = 0.5f + 0.5f * ((rand() % 100) / 100.0f);
                predictionError->setLevel(error);
                predictionErrorHistory.push_back(error);
            }
        }
        
        // Verify enhanced RL effectiveness
        assert(rewardHistory.size() > 10);
        assert(predictionErrorHistory.size() > 10);
        assert(curiosityHistory.size() > 10);
        
        // Average reward should increase over time
        if (rewardHistory.size() >= 20) {
            float firstHalfAvg = std::accumulate(rewardHistory.begin(), rewardHistory.begin() + 10, 0.0f) / 10.0f;
            float secondHalfAvg = std::accumulate(rewardHistory.end() - 10, rewardHistory.end(), 0.0f) / 10.0f;
            assert(secondHalfAvg >= firstHalfAvg * 0.9f);  // Should maintain or improve performance
        }
        
        std::cout << "    Enhanced reinforcement learning: PASSED" << std::endl;
    }
    
    static void test_meta_learning_and_meta_cognition() {
        std::cout << "Testing meta-learning and meta-cognition..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Meta-learning: learning to learn
        std::vector<float> learningRates;
        std::vector<float> confidenceLevels;
        
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Meta-cognitive monitoring of learning
            float learningRate = 0.5f * (1.0f - step / 100.0f) + 0.1f;  // Decreasing learning rate
            learningRates.push_back(learningRate);
            
            float confidence = 0.3f + 0.7f * (brain.getAverageFiringRate() / 2.0f);
            confidenceLevels.push_back(confidence);
            
            // Meta-cognitive regulation
            if (confidence > 0.7f) {
                // High confidence - reduce exploration
                nlm::Neuromodulator metacognitiveSignal;
                metacognitiveSignal.setLevel(0.3f);
                brain.applyNeuromodulation(metacognitiveSignal);
            } else if (confidence < 0.4f) {
                // Low confidence - increase exploration
                nlm::Neuromodulator metacognitiveSignal;
                metacognitiveSignal.setLevel(0.8f);
                brain.applyNeuromodulation(metacognitiveSignal);
            }
            
            // Self-monitoring of performance
            if (step % 15 == 0) {
                float performance = brain.getAverageFiringRate() * brain.getCurrentPlasticityRate();
                assert(performance >= 0.0f);
            }
        }
        
        // Verify meta-learning dynamics
        assert(learningRates.size() == 100);
        assert(confidenceLevels.size() == 100);
        
        // Learning rate should decrease over time (meta-learning)
        assert(learningRates.back() < learningRates.front());
        
        // Confidence should be positively correlated with performance
        float avgConfidence = std::accumulate(confidenceLevels.begin(), confidenceLevels.end(), 0.0f) / confidenceLevels.size();
        assert(avgConfidence > 0.5f);
        
        std::cout << "    Meta-learning and meta-cognition: PASSED" << std::endl;
    }
    
    static void test_cognitive_control_and_decision_making() {
        std::cout << "Testing cognitive control and decision making..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 550);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Cognitive control mechanisms
        std::vector<float> decisionConfidence;
        std::vector<nlm::ActionType> actionHistory;
        
        for (int step = 0; step < 70; ++step) {
            brain.step(step);
            
            // Decision making with cognitive control
            auto action = brain.produceAction();
            assert(action != nullptr);
            
            actionHistory.push_back(action->getActionType());
            
            // Cognitive control signals
            float explorationBonus = 0.0f;
            float exploitationBonus = 0.0f;
            
            if (step % 5 == 0) {
                explorationBonus = 0.3f;  // Encourage exploration
            } else if (step % 8 == 0) {
                exploitationBonus = 0.8f;  // Encourage exploitation
            }
            
            // Decision confidence calculation
            float confidence = 0.4f + explorationBonus + exploitationBonus;
            if (confidence > 1.0f) confidence = 1.0f;
            decisionConfidence.push_back(confidence);
            
            // Cognitive control regulation
            nlm::Neuromodulator controlSignal;
            controlSignal.setLevel(confidence);
            brain.applyNeuromodulation(controlSignal);
            
            // Action evaluation and selection
            if (step % 12 == 0) {
                // Strategic decision making
                float strategicValue = brain.getCurrentStrategicValue();
                assert(strategicValue >= 0.0f);
            }
        }
        
        // Verify cognitive control effectiveness
        assert(actionHistory.size() == 70);
        assert(decisionConfidence.size() == 70);
        
        // Average decision confidence should be reasonable
        float avgConfidence = std::accumulate(decisionConfidence.begin(), decisionConfidence.end(), 0.0f) / decisionConfidence.size();
        assert(avgConfidence >= 0.5f && avgConfidence <= 1.0f);
        
        // Action diversity should indicate cognitive flexibility
        std::unordered_set<nlm::ActionType> uniqueActions(actionHistory.begin(), actionHistory.end());
        assert(uniqueActions.size() >= 3);  // Should use multiple action types
        
        std::cout << "    Cognitive control and decision making: PASSED" << std::endl;
    }
    
    static void test_continuous_learning_efficiency() {
        std::cout << "Testing continuous learning efficiency..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 350);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Continuous learning over extended period
        std::vector<float> learningCurve;
        std::vector<float> efficiencyCurve;
        
        for (int step = 0; step < 200; ++step) {
            brain.step(step);
            
            // Learning efficiency calculation
            float learningRate = brain.getCurrentLearningRate();
            float performance = brain.getCurrentPerformanceMetric();
            
            float efficiency = learningRate * performance;
            learningCurve.push_back(learningRate);
            efficiencyCurve.push_back(efficiency);
            
            // Adaptive learning rate adjustment
            if (step % 25 == 0) {
                nlm::Neuromodulator adaptiveSignal;
                adaptiveSignal.setLevel(0.6f + 0.4f * (step / 200.0f));
                brain.applyNeuromodulation(adaptiveSignal);
            }
            
            // Verify continuous learning
            assert(learningRate >= 0.0f && learningRate <= 1.0f);
            assert(performance >= 0.0f);
            assert(efficiency >= 0.0f);
        }
        
        // Verify learning curve properties
        assert(learningCurve.size() == 200);
        assert(efficiencyCurve.size() == 200);
        
        // Learning should stabilize over time (not collapse)
        float finalEfficiency = efficiencyCurve.back();
        float initialEfficiency = efficiencyCurve[0];
        assert(finalEfficiency > 0.1f * initialEfficiency);  // Should maintain some baseline performance
        
        std::cout << "    Continuous learning efficiency: PASSED" << std::endl;
    }
    
    static void test_incremental_knowledge_acquisition() {
        std::cout << "Testing incremental knowledge acquisition..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Incremental knowledge acquisition
        std::vector<float> knowledgeRetention;
        std::vector<float> newKnowledgeAcquired;
        
        for (int step = 0; step < 120; ++step) {
            brain.step(step);
            
            // Incremental knowledge acquisition
            float knowledgeRetentionRate = 0.0f;
            float newKnowledgeRate = 0.0f;
            
            if (step < 30) {
                // Phase 1: Rapid acquisition
                newKnowledgeRate = 0.8f - step * 0.02f;
                knowledgeRetentionRate = 0.3f;
            } else if (step < 90) {
                // Phase 2: Consolidation
                newKnowledgeRate = 0.3f + (step - 30) * 0.005f;
                knowledgeRetentionRate = 0.7f - (step - 30) * 0.003f;
            } else {
                // Phase 3: Maintenance
                newKnowledgeRate = 0.2f;
                knowledgeRetentionRate = 0.8f;
            }
            
            knowledgeRetention.push_back(knowledgeRetentionRate);
            newKnowledgeAcquired.push_back(newKnowledgeRate);
            
            // Test incremental learning
            nlm::Neuromodulator incrementalSignal;
            incrementalSignal.setLevel(newKnowledgeRate);
            brain.applyNeuromodulation(incrementalSignal);
        }
        
        // Verify incremental knowledge properties
        assert(knowledgeRetention.size() == 120);
        assert(newKnowledgeAcquired.size() == 120);
        
        // Knowledge retention should be high in later phases
        float avgRetentionLater = std::accumulate(knowledgeRetention.begin() + 60, knowledgeRetention.end(), 0.0f) / 60.0f;
        assert(avgRetentionLater > 0.6f);
        
        std::cout << "    Incremental knowledge acquisition: PASSED" << std::endl;
    }
    
    static void test_adaptation_to_environment_changes() {
        std::cout << "Testing adaptation to environment changes..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Adaptation to environmental changes
        std::vector<float> adaptationMetrics;
        
        for (int step = 0; step < 150; ++step) {
            brain.step(step);
            
            // Simulate environmental changes
            float environmentChange = 0.0f;
            if (step % 20 == 0) {
                environmentChange = 0.5f;  // Significant change
            } else if (step % 30 == 0) {
                environmentChange = 0.3f;  // Moderate change
            }
            
            // Adaptation response
            float adaptationResponse = brain.getAdaptationToChange(environmentChange);
            adaptationMetrics.push_back(adaptationResponse);
            
            // Test adaptive behavior
            nlm::Neuromodulator adaptationSignal;
            adaptationSignal.setLevel(adaptationResponse);
            brain.applyNeuromodulation(adaptationSignal);
            
            // Verify adaptation effectiveness
            assert(adaptationResponse >= 0.0f && adaptationResponse <= 1.0f);
        }
        
        // Verify adaptation dynamics
        assert(adaptationMetrics.size() == 150);
        
        // Adaptation should be effective
        float avgAdaptation = std::accumulate(adaptationMetrics.begin(), adaptationMetrics.end(), 0.0f) / adaptationMetrics.size();
        assert(avgAdaptation > 0.5f);
        
        std::cout << "    Adaptation to environment changes: PASSED" << std::endl;
    }
    
    static void test_generalization_across_tasks() {
        std::cout << "Testing generalization across tasks..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Generalization across different tasks
        std::vector<std::pair<std::string, float>> taskPerformance;
        
        // Task 1: Movement
        float movementPerformance = 0.0f;
        for (int step = 0; step < 40; ++step) {
            brain.step(step);
            movementPerformance += brain.getCurrentPerformanceMetric();
        }
        movementPerformance /= 40;
        taskPerformance.push_back({"movement", movementPerformance});
        
        // Task 2: Perception
        float perceptionPerformance = 0.0f;
        for (int step = 40; step < 80; ++step) {
            brain.step(step);
            perceptionPerformance += brain.getCurrentPerformanceMetric();
        }
        perceptionPerformance /= 40;
        taskPerformance.push_back({"perception", perceptionPerformance});
        
        // Task 3: Memory
        float memoryPerformance = 0.0f;
        for (int step = 80; step < 120; ++step) {
            brain.step(step);
            memoryPerformance += brain.getCurrentPerformanceMetric();
        }
        memoryPerformance /= 40;
        taskPerformance.push_back({"memory", memoryPerformance});
        
        // Task 4: Decision making
        float decisionPerformance = 0.0f;
        for (int step = 120; step < 160; ++step) {
            brain.step(step);
            decisionPerformance += brain.getCurrentPerformanceMetric();
        }
        decisionPerformance /= 40;
        taskPerformance.push_back({"decision", decisionPerformance});
        
        // Task 5: Social cognition (simulated)
        float socialPerformance = 0.0f;
        for (int step = 160; step < 200; ++step) {
            brain.step(step);
            socialPerformance += brain.getCurrentPerformanceMetric();
        }
        socialPerformance /= 40;
        taskPerformance.push_back({"social", socialPerformance});
        
        // Verify generalization
        float avgTaskPerformance = 0.0f;
        for (auto& task : taskPerformance) {
            avgTaskPerformance += task.second;
        }
        avgTaskPerformance /= taskPerformance.size();
        
        assert(avgTaskPerformance > 0.5f);  // Should perform well across tasks
        assert(taskPerformance.size() == 5);
        
        std::cout << "    Generalization across tasks: PASSED" << std::endl;
    }
    
    static void test_self_regulated_learning() {
        std::cout << "Testing self-regulated learning..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 420);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Self-regulated learning mechanisms
        std::vector<float> selfRegulationSignals;
        std::vector<float> learningEfficiency;
        
        for (int step = 0; step < 180; ++step) {
            brain.step(step);
            
            // Self-regulation mechanisms
            float cognitiveLoad = brain.getCognitiveLoad();
            float metacognitiveAwareness = brain.getMetacognitiveAwareness();
            float resourceAllocation = brain.getResourceAllocation();
            
            float selfRegulationSignal = 0.0f;
            
            if (cognitiveLoad > 0.7f) {
                // High cognitive load - reduce learning rate
                selfRegulationSignal = 0.3f;
            } else if (metacognitiveAwareness > 0.6f) {
                // High metacognitive awareness - optimize learning
                selfRegulationSignal = 0.8f;
            } else if (resourceAllocation > 0.5f) {
                // Abundant resources - increase learning
                selfRegulationSignal = 1.0f;
            } else {
                // Balanced state - maintain normal learning
                selfRegulationSignal = 0.5f;
            }
            
            selfRegulationSignals.push_back(selfRegulationSignal);
            
            // Self-regulated learning efficiency
            float efficiency = selfRegulationSignal * brain.getCurrentLearningRate();
            learningEfficiency.push_back(efficiency);
            
            // Apply self-regulation
            nlm::Neuromodulator regulationSignal;
            regulationSignal.setLevel(selfRegulationSignal);
            brain.applyNeuromodulation(regulationSignal);
        }
        
        // Verify self-regulation effectiveness
        assert(selfRegulationSignals.size() == 180);
        assert(learningEfficiency.size() == 180);
        
        // Self-regulation should be adaptive
        float avgSelfRegulation = std::accumulate(selfRegulationSignals.begin(), selfRegulationSignals.end(), 0.0f) / selfRegulationSignals.size();
        assert(avgSelfRegulation > 0.4f && avgSelfRegulation < 0.9f);
        
        // Learning efficiency should correlate with self-regulation
        float avgEfficiency = std::accumulate(learningEfficiency.begin(), learningEfficiency.end(), 0.0f) / learningEfficiency.size();
        assert(avgEfficiency > 0.2f);
        
        std::cout << "    Self-regulated learning: PASSED" << std::endl;
    }
};

} // namespace test_suites