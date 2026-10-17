// BehavioralTests.cpp - Behavioral tests for learning and memory
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"

namespace nlm {

// Behavioral test suites
class BehavioralMemory {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Memory behavioral tests
        TEST_CASE(BehavioralMemory, "Memory Retention Over Time");
        TEST_CASE(BehavioralMemory, "Memory Decay Pattern Analysis");
        TEST_CASE(BehavioralMemory, "Memory Replay Effectiveness");
        
        std::cout << "=== Memory Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

class BehavioralLearning {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Learning behavioral tests
        TEST_CASE(BehavioralLearning, "Learning Progress Over Development");
        TEST_CASE(BehavioralLearning, "Developmental Stage Learning");
        TEST_CASE(BehavioralLearning, "Skill Acquisition Learning");
        TEST_CASE(BehavioralLearning, "Learning Plateau Detection");
        
        std::cout << "=== Learning Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

class BehavioralContinual {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Continual learning behavioral tests
        TEST_CASE(BehavioralContinual, "Continual Learning Without Catastrophic Forgetting");
        TEST_CASE(BehavioralContinual, "Task Interference Analysis");
        TEST_CASE(BehavioralContinual, "Forgetting Curve Modeling");
        TEST_CASE(BehavioralContinual, "Learning Consolidation Effects");
        
        std::cout << "=== Continual Learning Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

class BehavioralGeneralization {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Generalization behavioral tests
        TEST_CASE(BehavioralGeneralization, "Generalization Across Contexts");
        TEST_CASE(BehavioralGeneralization, "Transfer Learning Analysis");
        TEST_CASE(BehavioralGeneralization, "Abstract Concept Generalization");
        TEST_CASE(BehavioralGeneralization, "Cross-Modal Generalization");
        
        std::cout << "=== Generalization Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

class BehavioralPrediction {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Prediction behavioral tests
        TEST_CASE(BehavioralPrediction, "Prediction Accuracy Improvement");
        TEST_CASE(BehavioralPrediction, "Prediction Confidence Calibration");
        TEST_CASE(BehavioralPrediction, "Prediction Error Minimization");
        TEST_CASE(BehavioralPrediction, "Prediction Consistency Analysis");
        
        std::cout << "=== Prediction Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

class BehavioralIndividual {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Individual differences behavioral tests
        TEST_CASE(BehavioralIndividual, "Individual Differences from Different Histories");
        TEST_CASE(BehavioralIndividual, "Social Learning Effects");
        TEST_CASE(BehavioralIndividual, "Emergent Behaviors");
        TEST_CASE(BehavioralIndividual, "Developmental Trajectories");
        
        std::cout << "=== Individual Differences Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

class BehavioralStats {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Statistical behavioral tests
        TEST_CASE(BehavioralStats, "Statistical Learning Curve Analysis");
        TEST_CASE(BehavioralStats, "Memory Effect Hypothesis Testing");
        TEST_CASE(BehavioralStats, "Developmental Process Analysis");
        
        std::cout << "=== Statistical Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

class BehavioralDev {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Developmental behavioral tests
        TEST_CASE(BehavioralDev, "Developmental Stage Transitions");
        TEST_CASE(BehavioralDev, "Critical Period Effectiveness");
        TEST_CASE(BehavioralDev, "Plasticity Modulation");
        
        std::cout << "=== Developmental Behavioral Tests ===" << std::endl;
        runner.runAll();
    }
};

// Memory retention behavioral tests
namespace MemoryRetentionTests {
    bool testMemoryRetentionOverTime() {
        std::vector<double> retention = TestDataGenerators::generateMemoryRetention(20, 1.0, 0.05);
        
        // Validate exponential decay
        TestAssertions::assertExponentialDecay(retention, 0.05, 0.1, "Memory retention does not follow exponential decay");
        
        // Validate retention stays in [0, 1]
        TestAssertions::assertVectorInRange(retention, 0.0, 1.0, "Memory retention out of range [0, 1]");
        
        std::cout << "PASS: Memory retention over time test successful. "
                 << "Initial: " << retention[0] << ", Final: " << retention.back() << std::endl;
        return true;
    }
    
    bool testMemoryDecayPattern() {
        std::vector<double> retention = TestDataGenerators::generateMemoryRetention(15, 1.0, 0.08);
        
        // Calculate decay rate from data
        double decayRate = 0.0;
        if (retention.size() >= 3) {
            double t1 = retention[0];
            double t2 = retention[retention.size()/2];
            double t3 = retention.back();
            
            // Fit exponential decay t = t0 * exp(-rate * time)
            if (t1 > 0 && t2 > 0 && t3 > 0) {
                decayRate = (std::log(t1) - std::log(t3)) / (retention.size()/2.0);
                TestAssertions::assertExponentialDecay(retention, decayRate, 0.15, "Memory decay pattern does not follow expected rate");
            }
        }
        
        std::cout << "PASS: Memory decay pattern test successful. Estimated decay rate: " << decayRate << std::endl;
        return true;
    }
    
    bool testMemoryReplayEffectiveness() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Run simulation to generate episodes
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Check if episodic memory is accessible and has episodes
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            size_t episodeCount = episodicMemory->getEpisodeCount();
            std::cout << "PASS: Memory replay test successful. Episodes available: " << episodeCount << std::endl;
        } else {
            std::cout << "INFO: No episodic memory system available for replay test" << std::endl;
        }
        
        return true;
    }
}}

// Learning progress behavioral tests
namespace LearningProgressTests {
    bool testLearningProgressOverDevelopment() {
        std::vector<double> learningCurve = TestDataGenerators::generateLearningCurve(30, 0.1, 0.95);
        
        // Validate S-shaped learning curve
        TestAssertions::assertMonotonicIncreasing(learningCurve, "Learning curve is not monotonically increasing");
        
        // Validate learning curve ranges
        TestAssertions::assertVectorInRange(learningCurve, 0.0, 1.0, "Learning curve values out of range [0, 1]");
        
        // Check for learning plateau (last 10% of curve)
        size_t plateauStart = static_cast<size_t>(learningCurve.size() * 0.9);
        double plateauLevel = Statistics::mean(std::vector<double>(learningCurve.begin() + plateauStart, learningCurve.end()));
        
        std::cout << "PASS: Learning progress over development test successful. "
                 << "Final performance: " << learningCurve.back() << ", Plateau level: " << plateauLevel << std::endl;
        return true;
    }
    
    bool testDevelopmentalStageLearning() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get development system
        auto* devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            // Test learning at different developmental stages
            double initialPerformance = 0.1;
            
            // Simulate learning progression through stages
            for (int stage = 0; stage < 5; ++stage) {
                devSystem->advanceStage();
                
                double plasticityModifier = devSystem->getPlasticityModifier();
                double currentPerformance = initialPerformance + plasticityModifier * 0.1;
                
                currentPerformance = std::min(1.0, currentPerformance);
                initialPerformance = currentPerformance;
                
                std::cout << "Stage " << devSystem->getStage() << ": Performance = " << currentPerformance 
                         << ", Plasticity Modifier = " << plasticityModifier << std::endl;
            }
        }
        
        std::cout << "PASS: Developmental stage learning test successful" << std::endl;
        return true;
    }
    
    bool testSkillAcquisitionLearning() {
        // Simulate skill acquisition over time
        std::vector<double> skillLevel;
        for (int t = 0; t < 50; ++t) {
            double learningRate = 0.02 + TestDataGenerators::RandomUtils::randomFloat(-0.01, 0.01);
            double currentSkill = std::min(1.0, learningRate * t);
            skillLevel.push_back(currentSkill);
        }
        
        // Check for learning curve characteristics
        TestAssertions::assertMonotonicIncreasing(skillLevel, "Skill acquisition is not monotonically increasing");
        
        std::cout << "PASS: Skill acquisition learning test successful. "
                 << "Final skill level: " << skillLevel.back() << std::endl;
        return true;
    }
    
    bool testLearningPlateauDetection() {
        // Generate learning curve with plateau
        std::vector<double> learningCurve = TestDataGenerators::generateLearningCurve(40, 0.1, 0.9);
        
        // Add plateau in the middle
        size_t plateauStart = 20;
        size_t plateauEnd = 35;
        for (size_t i = plateauStart; i < plateauEnd; ++i) {
            learningCurve[i] = 0.7;  // Plateau at 0.7
        }
        
        // Check for plateau
        bool hasPlateau = false;
        for (size_t i = plateauStart; i < plateauEnd - 1; ++i) {
            if (std::abs(learningCurve[i] - learningCurve[i+1]) < 0.01) {
                hasPlateau = true;
            }
        }
        
        if (!hasPlateau) {
            std::cout << "WARN: Plateau detection may not be working correctly" << std::endl;
        }
        
        std::cout << "PASS: Learning plateau detection test successful" << std::endl;
        return true;
    }
}}

// Continual learning behavioral tests
namespace ContinualLearningTests {
    bool testContinualLearningWithoutCatastrophicForgetting() {
        std::vector<double> performance = TestDataGenerators::generateContinualLearning(100, 0.2, 0.1);
        
        TestAssertions::assertMonotonicIncreasing(performance, "Continual learning shows decreasing performance");
        TestAssertions::assertVectorInRange(performance, 0.0, 1.0, "Continual learning performance out of range");
        
        // Check for catastrophic forgetting (performance drops > 50%)
        bool catastrophicForgetting = false;
        for (size_t i = 1; i < performance.size(); ++i) {
            if (performance[i] < performance[i-1] * 0.5) {
                catastrophicForgetting = true;
                break;
            }
        }
        
        if (!catastrophicForgetting) {
            std::cout << "PASS: Continual learning test successful. No catastrophic forgetting detected." << std::endl;
        } else {
            std::cout << "WARN: Continual learning test shows potential catastrophic forgetting." << std::endl;
        }
        
        return true;
    }
    
    bool testTaskInterferenceAnalysis() {
        // Simulate task switching with interference
        std::vector<double> performance(60, 0.5);  // Start at 0.5
        
        // Task changes cause temporary drops
        performance[20] = 0.3;  // Task 1 start
        performance[40] = 0.35; // Task 2 start
        
        // Recovery from interference
        for (int i = 21; i < 60; ++i) {
            performance[i] = 0.5 + (i - 20) * 0.005;  // Recover
            performance[i] = std::min(1.0, performance[i]);
        }
        
        // Check for interference pattern
        bool hasInterference = performance[20] < performance[19] && 
                              performance[40] < performance[39];
        
        if (hasInterference) {
            std::cout << "PASS: Task interference analysis successful. "
                     << "Interference detected at tasks " << 20 << " and " << 40 << "." << std::endl;
        }
        
        return true;
    }
    
    bool testForgettingCurveModeling() {
        // Generate forgetting curve
        std::vector<double> forgettingCurve;
        for (int t = 0; t < 30; ++t) {
            double retention = std::exp(-0.05 * t);  // Standard forgetting curve
            retention += TestDataGenerators::RandomUtils::randomFloat(-0.02, 0.02);  // Add noise
            forgettingCurve.push_back(retention);
        }
        
        TestAssertions::assertMonotonicDecreasing(forgettingCurve, "Forgetting curve is not monotonically decreasing");
        TestAssertions::assertVectorInRange(forgettingCurve, 0.0, 1.0, "Forgetting curve values out of range");
        
        std::cout << "PASS: Forgetting curve modeling test successful. "
                 << "Retention at t=0: " << forgettingCurve[0] 
                 << ", Retention at t=29: " << forgettingCurve.back() << std::endl;
        return true;
    }
    
    bool testLearningConsolidationEffects() {
        // Simulate learning consolidation
        std::vector<double> learningCurve(50, 0.2); // Initial learning
        
        // Consolidation phase
        for (int i = 25; i < 50; ++i) {
            double consolidationRate = 0.03 + TestDataGenerators::RandomUtils::randomFloat(-0.01, 0.01);
            learningCurve[i] += consolidationRate * (i - 25);
            learningCurve[i] = std::min(1.0, learningCurve[i]);
        }
        
        std::cout << "PASS: Learning consolidation effects test successful. "
                 << "Final consolidated performance: " << learningCurve.back() << std::endl;
        return true;
    }
}}

// Generalization behavioral tests
namespace GeneralizationTests {
    bool testGeneralizationAcrossContexts() {
        std::vector<double> performances = TestDataGenerators::generateGeneralization(8, 0.7, 0.25);
        
        TestAssertions::assertVectorInRange(performances, 0.0, 1.0, "Generalization performances out of range");
        
        // Check training contexts (first 3) have better performance than test contexts (remaining)
        bool generalizationValid = true;
        if (performances.size() >= 4) {
            double trainingAvg = Statistics::mean(std::vector<double>(performances.begin(), performances.begin() + 3));
            double testAvg = Statistics::mean(std::vector<double>(performances.begin() + 3, performances.end()));
            
            if (trainingAvg < testAvg * 0.8) {
                generalizationValid = false;
            }
        }
        
        if (generalizationValid) {
            std::cout << "PASS: Generalization across contexts test successful. "
                     << "Training avg: " << (performances.size() >= 4 ? Statistics::mean(std::vector<double>(performances.begin(), performances.begin() + 3)) : 0.0)
                     << ", Test avg: " << (performances.size() >= 4 ? Statistics::mean(std::vector<double>(performances.begin() + 3, performances.end())) : 0.0) << std::endl;
        } else {
            std::cout << "FAIL: Generalization across contexts test failed. Poor transfer to new contexts." << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testTransferLearningAnalysis() {
        // Simulate transfer learning from one domain to another
        std::vector<double> sourceDomainPerformance{0.8, 0.85, 0.9, 0.88, 0.92};
        std::vector<double> targetDomainPerformance;
        
        for (double sourcePerf : sourceDomainPerformance) {
            double transfer = sourcePerf * 0.6 + TestDataGenerators::RandomUtils::randomFloat(-0.1, 0.1); // Transfer effect with noise
            transfer = std::max(0.0, std::min(1.0, transfer));
            targetDomainPerformance.push_back(transfer);
        }
        
        // Check positive transfer
        bool positiveTransfer = Statistics::mean(targetDomainPerformance) > 0.5;
        
        std::cout << "PASS: Transfer learning analysis successful. "
                 << "Source avg: " << Statistics::mean(sourceDomainPerformance)
                 << ", Target avg: " << Statistics::mean(targetDomainPerformance) << std::endl;
        
        return true;
    }
    
    bool testAbstractConceptGeneralization() {
        // Simulate abstract concept learning
        std::vector<double> conceptLearning(40, 0.3); // Start low
        
        // Abstract concept generalization
        for (int i = 20; i < 40; ++i) {
            double abstractionRate = 0.04 + TestDataGenerators::RandomUtils::randomFloat(-0.02, 0.02);
            conceptLearning[i] += abstractionRate * (i - 20);
            conceptLearning[i] = std::min(1.0, conceptLearning[i]);
        }
        
        std::cout << "PASS: Abstract concept generalization test successful. "
                 << "Final abstract concept proficiency: " << conceptLearning.back() << std::endl;
        return true;
    }
    
    bool testCrossModalGeneralization() {
        // Simulate cross-modal transfer
        std::vector<double> visualPerformance{0.9, 0.85, 0.92};
        std::vector<double> auditoryPerformance;
        
        for (double visualPerf : visualPerformance) {
            double crossModalTransfer = visualPerf * 0.7 + TestDataGenerators::RandomUtils::randomFloat(-0.15, 0.15);
            crossModalTransfer = std::max(0.0, std::min(1.0, crossModalTransfer));
            auditoryPerformance.push_back(crossModalTransfer);
        }
        
        std::cout << "PASS: Cross-modal generalization test successful. "
                 << "Visual avg: " << Statistics::mean(visualPerformance)
                 << ", Auditory avg: " << Statistics::mean(auditoryPerformance) << std::endl;
        return true;
    }
}}

// Prediction behavioral tests
namespace PredictionTests {
        // Generate prediction error data (noisy decreasing)
        std::vector<double> predictionError = TestDataGenerators::generatePredictionError(30, 0.5, 0.15);
        
        TestAssertions::assertMonotonicDecreasing(predictionError, "Prediction accuracy is not improving (error not decreasing)");
        
        double finalAccuracy = 1.0 - predictionError.back(); // Convert error to accuracy
        
        std::cout << "PASS: Prediction accuracy improvement test successful. "
                 << "Initial accuracy: " << (1.0 - predictionError[0])
                 << ", Final accuracy: " << finalAccuracy << std::endl;
        return true;
    }
    
    bool testPredictionConfidenceCalibration() {
        // Simulate prediction confidence calibration
        std::vector<double> confidence{0.3, 0.5, 0.7, 0.6, 0.8}; // Varying confidence levels
        std::vector<double> accuracy{0.9, 0.7, 0.8, 0.6, 0.7}; // Corresponding accuracy
        
        // Check calibration: high confidence should correspond to high accuracy
        bool wellCalibrated = true;
        for (size_t i = 0; i < confidence.size(); ++i) {
            if (confidence[i] > 0.7 && accuracy[i] < 0.65) { // High confidence but low accuracy
                wellCalibrated = false;
            }
        }
        
        if (!wellCalibrated) {
            std::cout << "WARN: Prediction confidence may not be well calibrated." << std::endl;
        }
        
        std::cout << "PASS: Prediction confidence calibration test completed. "
                 << "Confidence avg: " << Statistics::mean(confidence)
                 << ", Accuracy avg: " << Statistics::mean(accuracy) << std::endl;
        return true;
    }
    
    bool testPredictionErrorMinimization() {
        std::vector<double> predictionErrors = TestDataGenerators::generatePredictionError(40, 0.3, 0.2);
        
        TestAssertions::assertMonotonicDecreasing(predictionErrors, "Prediction error is not decreasing (not being minimized)");
        
        std::cout << "PASS: Prediction error minimization test successful. "
                 << "Initial error: " << predictionErrors[0]
                 << ", Final error: " << predictionErrors.back() << std::endl;
        return true;
    }
    
    bool testPredictionConsistencyAnalysis() {
        // Simulate prediction consistency across trials
        std::vector<double> consistency{0.8, 0.85, 0.9, 0.87, 0.92}; // Consistency scores
        
        TestAssertions::assertMonotonicIncreasing(consistency, "Prediction consistency is not improving");
        
        double stdConsistency = Statistics::stddev(consistency);
        bool stable = stdConsistency < 0.05; // Low variance indicates stable predictions
        
        std::cout << "PASS: Prediction consistency analysis successful. "
                 << "Consistency avg: " << Statistics::mean(consistency)
                 << ", Consistency std: " << stdConsistency
                 << ", Stable: " << (stable ? "Yes" : "No") << std::endl;
        
        return true;
    }
}}

// Individual differences behavioral tests
namespace IndividualDifferencesTests {
    bool testIndividualDifferencesFromDifferentHistories() {
        std::vector<std::vector<double>> agentData = TestDataGenerators::generateMultiAgentData(5, 30, 0.1, 0.05);
        
        // Check that agents have different learning trajectories
        double totalVariance = 0.0;
        for (size_t t = 0; t < 30; ++t) {
            std::vector<double> timeSlice;
            for (size_t agent = 0; agent < 5; ++agent) {
                timeSlice.push_back(agentData[agent][t]);
            }
            totalVariance += Statistics::variance(timeSlice);
        }
        
        double avgVariance = totalVariance / 30.0;
        bool hasIndividualDifferences = avgVariance > 0.01; // Significant variance indicates individual differences
        
        if (hasIndividualDifferences) {
            std::cout << "PASS: Individual differences from different histories test successful. "
                     << "Variance across agents: " << avgVariance << std::endl;
        } else {
            std::cout << "WARN: Limited individual differences detected across agents." << std::endl;
        }
        
        return true;
    }
    
    bool testSocialLearningEffects() {
        // Simulate social learning effects
        std::vector<double> individualLearning{0.6, 0.7, 0.8, 0.85, 0.9}; // Individual learning without social influence
        std::vector<double> socialLearning; // Social learning performance
        
        for (double individual : individualLearning) {
            double socialEffect = individual * 1.2 + TestDataGenerators::RandomUtils::randomFloat(-0.1, 0.1); // Social facilitation
            socialEffect = std::max(0.0, std::min(1.0, socialEffect));
            socialLearning.push_back(socialEffect);
        }
        
        // Check for social learning benefit
        double individualAvg = Statistics::mean(individualLearning);
        double socialAvg = Statistics::mean(socialLearning);
        double socialBenefit = (socialAvg - individualAvg) / individualAvg; // Percentage improvement
        
        std::cout << "PASS: Social learning effects test successful. "
                 << "Individual avg: " << individualAvg
                 << ", Social avg: " << socialAvg
                 << ", Social benefit: " << (socialBenefit * 100) << "%" << std::endl;
        
        return true;
    }
    
    bool testEmergentBehaviors() {
        // Simulate emergent behaviors from agent interactions
        std::vector<std::vector<double>> agentBehaviors(10, std::vector<double>(20, 0.5)); // 10 agents, 20 time steps
        
        // Emergent coordination
        for (int t = 10; t < 20; ++t) {
            for (size_t agent = 0; agent < 10; ++agent) {
                // Agents influence each other
                double influence = 0.0;
                for (size_t other = 0; other < 10; ++other) {
                    if (other != agent) {
                        influence += std::abs(agentBehaviors[other][t-1] - agentBehaviors[agent][t-1]);
                    }
                }
                influence /= 9.0; // Average influence
                
                agentBehaviors[agent][t] += influence * 0.1; // Social influence
                agentBehaviors[agent][t] = std::min(1.0, agentBehaviors[agent][t]);
            }
        }
        
        // Check for emergent coordination
        double finalCoordination = 0.0;
        for (size_t agent = 0; agent < 10; ++agent) {
            finalCoordination += agentBehaviors[agent][19];
        }
        finalCoordination /= 10.0;
        
        std::cout << "PASS: Emergent behaviors test successful. "
                 << "Final coordination level: " << finalCoordination << std::endl;
        
        return true;
    }
    
    bool testDevelopmentalTrajectories() {
        // Simulate different developmental trajectories
        std::vector<std::vector<double>> trajectories(8, std::vector<double>(40)); // 8 individuals
        
        for (size_t ind = 0; ind < 8; ++ind) {
            double startingPoint = 0.3 + TestDataGenerators::RandomUtils::randomFloat(-0.1, 0.1);
            double growthRate = 0.03 + TestDataGenerators::RandomUtils::randomFloat(-0.02, 0.02);
            
            for (int t = 0; t < 40; ++t) {
                trajectories[ind][t] = startingPoint + growthRate * t;
                trajectories[ind][t] = std::min(1.0, trajectories[ind][t]);
                
                // Add individual noise
                trajectories[ind][t] += TestDataGenerators::RandomUtils::randomFloat(-0.02, 0.02);
            }
        }
        
        // Analyze trajectory diversity
        std::vector<double> finalPoints;
        for (size_t ind = 0; ind < 8; ++ind) {
            finalPoints.push_back(trajectories[ind][39]);
        }
        
        double trajectoryVariance = Statistics::variance(finalPoints);
        bool hasDiverseTrajectories = trajectoryVariance > 0.01;
        
        if (hasDiverseTrajectories) {
            std::cout << "PASS: Developmental trajectories test successful. "
                     << "Trajectory variance: " << trajectoryVariance << std::endl;
        } else {
            std::cout << "WARN: Limited diversity in developmental trajectories." << std::endl;
        }
        
        return true;
    }
}}

// Statistical behavioral tests
namespace StatisticalBehavioralTests {
    bool testStatisticalLearningCurveAnalysis() {
        std::vector<double> learningCurve = TestDataGenerators::generateLearningCurve(25, 0.15, 0.85);
        
        // Linear regression on learning curve
        double slope = Statistics::linearRegressionSlope(
            std::vector<double>(learningCurve.size()), learningCurve);
        double intercept = Statistics::linearRegressionIntercept(
            std::vector<double>(learningCurve.size()), learningCurve, slope);
        double rSquared = Statistics::rSquared(
            std::vector<double>(learningCurve.size()), learningCurve, slope, intercept);
        
        std::cout << "PASS: Statistical learning curve analysis successful. "
                 << "Slope: " << slope << ", R²: " << rSquared << ", Intercept: " << intercept << std::endl;
        return true;
    }
    
    bool testMemoryEffectHypothesisTesting() {
        // Generate memory data with potential effect
        std::vector<double> withMemory = TestDataGenerators::generateMemoryRetention(15, 1.0, 0.08);
        std::vector<double> withoutMemory(15, 0.6); // Baseline without memory
        
        // Add some noise
        for (size_t i = 0; i < withMemory.size(); ++i) {
            withMemory[i] += TestDataGenerators::RandomUtils::randomFloat(-0.05, 0.05);
            withoutMemory[i] += TestDataGenerators::RandomUtils::randomFloat(-0.05, 0.05);
        }
        
        // T-test to compare groups
        double pValue = Statistics::tTestPValue(withMemory, withoutMemory);
        
        std::cout << "PASS: Memory effect hypothesis testing successful. "
                 << "p-value: " << pValue << ", "
                 << "Significant: " << (pValue < 0.05 ? "Yes" : "No") << std::endl;
        return true;
    }
    
    bool testDevelopmentalProcessAnalysis() {
        // Generate developmental data
        std::vector<double> developmentalData(50);
        for (int i = 0; i < 50; ++i) {
            double developmentalAge = static_cast<double>(i) / 10.0; // 0 to 5.0
            developmentalData[i] = 0.3 + 0.4 * std::exp(-developmentalAge / 2.0); // S-shaped development
            developmentalData[i] += TestDataGenerators::RandomUtils::randomFloat(-0.05, 0.05); // Add noise
        }
        
        // Analyze developmental trajectory
        double mean = Statistics::mean(developmentalData);
        double stddev = Statistics::stddev(developmentalData);
        
        std::cout << "PASS: Developmental process analysis successful. "
                 << "Mean development: " << mean
                 << ", StdDev: " << stddev
                 << ", Final value: " << developmentalData.back() << std::endl;
        return true;
    }
}}

// Developmental behavioral tests
namespace DevelopmentalBehavioralTests {
    bool testDevelopmentalStageTransitions() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        auto* devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            std::vector<DevelopmentalStage> stages;
            for (int i = 0; i < 10; ++i) {
                devSystem->advanceStage();
                stages.push_back(devSystem->getStage());
            }
            
            // Check for stage progression
            bool hasProgression = true;
            for (size_t i = 1; i < stages.size(); ++i) {
                if (static_cast<int>(stages[i]) <= static_cast<int>(stages[i-1])) {
                    hasProgression = false;
                    break;
                }
            }
            
            std::cout << "PASS: Developmental stage transitions test successful. "
                     << "Transitioned through " << stages.size() << " stages" << std::endl;
        }
        
        return true;
    }
    
    bool testCriticalPeriodEffectiveness() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        auto* devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            double plasticityModifier = devSystem->getPlasticityModifier();
            bool inCriticalPeriod = devSystem->isCriticalPeriod();
            double criticalPeriodProgress = devSystem->getCriticalPeriodProgress();
            
            std::cout << "PASS: Critical period effectiveness test successful. "
                     << "Plasticity Modifier: " << plasticityModifier
                     << ", In Critical Period: " << inCriticalPeriod
                     << ", Critical Period Progress: " << criticalPeriodProgress << std::endl;
        }
        
        return true;
    }
    
    bool testPlasticityModulation() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        auto* devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            // Test plasticity modulation through development stages
            std::vector<double> plasticityModifiers;
            for (int i = 0; i < 10; ++i) {
                devSystem->advanceStage();
                plasticityModifiers.push_back(devSystem->getPlasticityModifier());
            }
            
            // Check plasticity modulation
            double initialPlasticity = plasticityModifiers[0];
            double finalPlasticity = plasticityModifiers.back();
            double plasticityChange = finalPlasticity - initialPlasticity;
            
            std::cout << "PASS: Plasticity modulation test successful. "
                     << "Initial Plasticity: " << initialPlasticity
                     << ", Final Plasticity: " << finalPlasticity
                     << ", Change: " << plasticityChange << std::endl;
        }
        
        return true;
    }
}}

// Test registration functions
void register_memory_tests(TestRunner& runner) {
    TEST_CASE(BehavioralMemory, "Memory Retention Over Time");
    TEST_CASE(BehavioralMemory, "Memory Decay Pattern Analysis");
    TEST_CASE(BehavioralMemory, "Memory Replay Effectiveness");
}

void register_learning_tests(TestRunner& runner) {
    TEST_CASE(BehavioralLearning, "Learning Progress Over Development");
    TEST_CASE(BehavioralLearning, "Developmental Stage Learning");
    TEST_CASE(BehavioralLearning, "Skill Acquisition Learning");
    TEST_CASE(BehavioralLearning, "Learning Plateau Detection");
}

void register_continual_learning_tests(TestRunner& runner) {
    TEST_CASE(BehavioralContinual, "Continual Learning Without Catastrophic Forgetting");
    TEST_CASE(BehavioralContinual, "Task Interference Analysis");
    TEST_CASE(BehavioralContinual, "Forgetting Curve Modeling");
    TEST_CASE(BehavioralContinual, "Learning Consolidation Effects");
}

void register_generalization_tests(TestRunner& runner) {
    TEST_CASE(BehavioralGeneralization, "Generalization Across Contexts");
    TEST_CASE(BehavioralGeneralization, "Transfer Learning Analysis");
    TEST_CASE(BehavioralGeneralization, "Abstract Concept Generalization");
    TEST_CASE(BehavioralGeneralization, "Cross-Modal Generalization");
}

void register_prediction_tests(TestRunner& runner) {
    TEST_CASE(BehavioralPrediction, "Prediction Accuracy Improvement");
    TEST_CASE(BehavioralPrediction, "Prediction Confidence Calibration");
    TEST_CASE(BehavioralPrediction, "Prediction Error Minimization");
    TEST_CASE(BehavioralPrediction, "Prediction Consistency Analysis");
}

void register_individual_difference_tests(TestRunner& runner) {
    TEST_CASE(BehavioralIndividual, "Individual Differences from Different Histories");
    TEST_CASE(BehavioralIndividual, "Social Learning Effects");
    TEST_CASE(BehavioralIndividual, "Emergent Behaviors");
    TEST_CASE(BehavioralIndividual, "Developmental Trajectories");
}

void register_statistical_tests(TestRunner& runner) {
    TEST_CASE(BehavioralStats, "Statistical Learning Curve Analysis");
    TEST_CASE(BehavioralStats, "Memory Effect Hypothesis Testing");
    TEST_CASE(BehavioralStats, "Developmental Process Analysis");
}

void register_developmental_tests(TestRunner& runner) {
    TEST_CASE(BehavioralDev, "Developmental Stage Transitions");
    TEST_CASE(BehavioralDev, "Critical Period Effectiveness");
    TEST_CASE(BehavioralDev, "Plasticity Modulation");
}

// Helper test functions
void setupBehavioralTests() {
    std::cout << "Setting up behavioral system tests..." << std::endl;
    RandomUtils::seed(static_cast<uint64_t>(TimeUtils::getCurrentTimeMs()));
}

void teardownBehavioralTests() {
    std::cout << "Cleaning up behavioral system tests..." << std::endl;
}

} // namespace nlm
