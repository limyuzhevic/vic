// NLM Modular Agent Testing Framework
// Enhanced testing framework for AgentBrain and related components
// Provides comprehensive testing capabilities for different scenarios

#ifndef NLM_AGENT_TESTING_FRAMEWORK_H
#define NLM_AGENT_TESTING_FRAMEWORK_H

#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <algorithm>
#include <cassert>
#include <random>
#include <fstream>

namespace nlm {
namespace testing {

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    double executionTime;
    std::string message;
    std::map<std::string, std::string> metrics;
    
    TestResult(const std::string& name, bool success, const std::string& msg = "")
        : testName(name), passed(success), executionTime(0.0), message(msg) {}
};

// Test configuration
struct TestConfig {
    bool verbose = false;
    bool recordMetrics = true;
    double timeout = 30.0; // seconds
    int randomSeed = 42;
    std::string outputFile = "test_results.json";
    bool continueOnFailure = false;
    std::vector<std::string> enabledTests;
    std::vector<std::string> disabledTests;
    
    // Test-specific parameters
    struct {
        int numSteps = 1000;
        float plasticityStrength = 1.0f;
        float noveltyThreshold = 0.1f;
        bool enableCuriosity = true;
        bool enableDevelopment = true;
        int neuronCount = 500;
        float connectionProbability = 0.15f;
    } agentParams;
};

// Base test class
class AgentTest {
protected:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    TestConfig config_;
    std::vector<TestResult> results_;
    
public:
    virtual ~AgentTest() = default;
    
    // Initialize test with brain and agent
    virtual bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
        brain_ = brain;
        agent_ = agent;
        return true;
    }
    
    // Run the test
    virtual TestResult run() = 0;
    
    // Setup before test
    virtual void setup() {}
    
    // Cleanup after test
    virtual void teardown() {}
    
    // Get test name
    virtual std::string getTestName() const = 0;
    
    // Add result to collection
    void addResult(const TestResult& result) {
        results_.push_back(result);
    }
    
    // Get all results
    const std::vector<TestResult>& getResults() const { return results_; }
    
    // Check if test should run
    virtual bool shouldRun() const {
        return true;
    }
};

// Test 1: Basic Sensory Processing Test
class SensoryProcessingTest : public AgentTest {
public:
    TestResult run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup world with known pattern
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(width=20, height=20, visionWidth=8, visionHeight=8);
            world_->reset();
            
            // Initialize agent
            agent_->initialize(*world_);
            
            // Create simple visual pattern
            std::vector<float> vision(64, 0.0f);
            // Center patch bright
            for (int y = 2; y < 6; ++y) {
                for (int x = 2; x < 6; ++x) {
                    vision[y * 8 + x] = 1.0f;
                }
            }
            
            // Create percept
            SensoryPercept percept;
            percept.setVision(vision);
            percept.setTimestamp(0.0);
            
            // Process sensory input
            agent_->processSensoryInput(percept);
            
            // Verify that neurons received input
            bool visionInputReceived = false;
            for (const auto& neuron : agent_->getSensoryVisionNeurons()) {
                if (neuron && neuron->getState().membranePotential > -60.0f) {
                    visionInputReceived = true;
                    break;
                }
            }
            
            bool passed = visionInputReceived;
            std::string message = passed ? 
                "Sensory processing test passed - vision input detected" :
                "Sensory processing test failed - no vision input detected";
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResult(getTestName(), passed, message);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResult(getTestName(), false, 
                            std::string("Exception: ") + e.what());
        }
    }
    
    std::string getTestName() const override { return "SensoryProcessingTest"; }
    
    // Helper to get sensory vision neurons
    std::vector<Neuron*> getSensoryVisionNeurons() {
        return agent_->getSensoryVisionNeurons();
    }
};

// Test 2: Motor Decoding Test
class MotorDecodingTest : public AgentTest {
public:
    TestResult run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup world
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(width=10, height=10, visionWidth=8, visionHeight=8);
            world_->reset();
            
            // Initialize agent
            agent_->initialize(*world_);
            
            // Inject activity into motor neurons to test decoding
            injectMotorActivity();
            
            // Get motor command
            MotorCommand action = agent_->decodeMotorCommand();
            
            // Verify action is valid
            bool actionValid = (action == MotorCommand::MoveForward || 
                               action == MotorCommand::MoveBackward || 
                               action == MotorCommand::TurnLeft ||
                               action == MotorCommand::TurnRight ||
                               action == MotorCommand::Interact ||
                               action == MotorCommand::Wait);
            
            // Test different motor neuron groups
            bool forwardActive = isMotorGroupActive(agent_->getMotorForwardNeurons());
            bool backwardActive = isMotorGroupActive(agent_->getMotorBackwardNeurons());
            
            bool passed = actionValid && (forwardActive || backwardActive || 
                                        isMotorGroupActive(agent_->getMotorTurnLeftNeurons()) ||
                                        isMotorGroupActive(agent_->getMotorTurnRightNeurons()));
            
            std::string message = passed ? 
                "Motor decoding test passed - valid action decoded" :
                "Motor decoding test failed - invalid or no action decoded";
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult result(getTestName(), passed, message);
            result.metrics["action_type"] = std::to_string(static_cast<int>(action));
            result.metrics["forward_active"] = std::to_string(forwardActive);
            result.metrics["backward_active"] = std::to_string(backwardActive);
            
            return result;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResult(getTestName(), false, 
                            std::string("Exception: ") + e.what());
        }
    }
    
    std::string getTestName() const override { return "MotorDecodingTest"; }
    
private:
    void injectMotorActivity() {
        // Inject activity into a random motor neuron group
        auto& motorGroups = {
            agent_->getMotorForwardNeurons(),
            agent_->getMotorBackwardNeurons(),
            agent_->getMotorTurnLeftNeurons(),
            agent_->getMotorTurnRightNeurons(),
            agent_->getMotorInteractNeurons(),
            agent_->getMotorWaitNeurons()
        };
        
        if (!motorGroups.empty()) {
            auto& selectedGroup = motorGroups[0]; // Use first group for simplicity
            if (!selectedGroup.empty()) {
                selectedGroup[0]->injectCurrent(50.0f);
            }
        }
    }
    
    bool isMotorGroupActive(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return false;
        
        for (const auto& neuron : neurons) {
            if (neuron && neuron->getState().membranePotential > -50.0f) {
                return true;
            }
        }
        return false;
    }
};

// Test 3: Neuromodulation Integration Test
class NeuromodulationTest : public AgentTest {
public:
    TestResult run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup world
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(width=15, height=15, visionWidth=8, visionHeight=8);
            world_->reset();
            
            // Initialize agent
            agent_->initialize(*world_);
            
            // Record initial neuromodulation levels
            float initialCuriosity = agent_->getCuriosityLevel();
            float initialNovelty = agent_->getNoveltyLevel();
            float initialPredictionError = agent_->getPredictionError();
            float initialDopamine = agent_->getNeuromodulationLevel();
            
            // Create sensory input
            SensoryPercept percept;
            std::vector<float> vision(64, 0.0f);
            percept.setVision(vision);
            percept.setTimestamp(0.0);
            
            // Process sensory input
            agent_->processSensoryInput(percept);
            
            // Apply reward modulation
            agent_->applyRewardModulation(1.0f, 0.5f);
            
            // Update development
            agent_->updateDevelopment(0.1);
            
            // Record final neuromodulation levels
            float finalCuriosity = agent_->getCuriosityLevel();
            float finalNovelty = agent_->getNoveltyLevel();
            float finalPredictionError = agent_->getPredictionError();
            float finalDopamine = agent_->getNeuromodulationLevel();
            
            // Check for changes
            bool curiosityChanged = std::abs(finalCuriosity - initialCuriosity) > 0.001f;
            bool noveltyChanged = std::abs(finalNovelty - initialNovelty) > 0.001f;
            bool predictionErrorChanged = std::abs(finalPredictionError - initialPredictionError) > 0.001f;
            bool dopamineChanged = std::abs(finalDopamine - initialDopamine) > 0.001f;
            
            bool passed = curiosityChanged || noveltyChanged || predictionErrorChanged || dopamineChanged;
            
            std::string message = passed ? 
                "Neuromodulation test passed - neuromodulation systems activated" :
                "Neuromodulation test failed - neuromodulation systems not responding";
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult result(getTestName(), passed, message);
            result.metrics["curiosity_change"] = std::to_string(finalCuriosity - initialCuriosity);
            result.metrics["novelty_change"] = std::to_string(finalNovelty - initialNovelty);
            result.metrics["prediction_error_change"] = std::to_string(finalPredictionError - initialPredictionError);
            result.metrics["dopamine_change"] = std::to_string(finalDopamine - initialDopamine);
            
            return result;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResult(getTestName(), false, 
                            std::string("Exception: ") + e.what());
        }
    }
    
    std::string getTestName() const override { return "NeuromodulationTest"; }
};

// Test 4: Developmental Progression Test
class DevelopmentProgressionTest : public AgentTest {
public:
    TestResult run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup world
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(width=10, height=10, visionWidth=8, visionHeight=8);
            world_->reset();
            
            // Initialize agent
            agent_->initialize(*world_);
            
            // Record initial developmental stage
            DevelopmentalStage initialStage = agent_->getDevelopmentalStage();
            
            // Apply development over time
            for (int i = 0; i < 10; ++i) {
                agent_->updateDevelopment(0.1);
            }
            
            // Record final developmental stage
            DevelopmentalStage finalStage = agent_->getDevelopmentalStage();
            
            // Check for developmental progression
            bool stageChanged = (initialStage != finalStage);
            
            bool passed = stageChanged;
            
            std::string message = passed ? 
                "Developmental progression test passed - stage changed over time" :
                "Developmental progression test failed - stage did not change";
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult result(getTestName(), passed, message);
            result.metrics["initial_stage"] = std::to_string(static_cast<int>(initialStage));
            result.metrics["final_stage"] = std::to_string(static_cast<int>(finalStage));
            result.metrics["development_time"] = std::to_string(1.0); // seconds
            
            return result;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResult(getTestName(), false, 
                            std::string("Exception: ") + e.what());
        }
    }
    
    std::string getTestName() const override { return "DevelopmentProgressionTest"; }
};

// Test 5: Curiosity-Driven Exploration Test
class CuriosityDrivenExplorationTest : public AgentTest {
public:
    TestResult run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup world
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(width=20, height=20, visionWidth=8, visionHeight=8);
            world_->reset();
            
            // Initialize agent
            agent_->initialize(*world_);
            
            // Get initial curiosity level
            float initialCuriosity = agent_->getCuriosityLevel();
            
            // Create different sensory inputs to increase novelty
            for (int step = 0; step < 5; ++step) {
                SensoryPercept percept;
                std::vector<float> vision(64, 0.0f);
                
                // Create different pattern each step
                int pattern = step % 4;
                switch (pattern) {
                    case 0: // Center square
                        for (int y = 2; y < 6; ++y) {
                            for (int x = 2; x < 6; ++x) {
                                vision[y * 8 + x] = 1.0f;
                            }
                        }
                        break;
                    case 1: // Corners
                        vision[0] = vision[63] = vision[8] = vision[55] = 1.0f;
                        break;
                    case 2: // Random pattern
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
                        for (int i = 0; i < 64; ++i) {
                            vision[i] = dis(gen);
                        }
                        break;
                    case 3: // Stripes
                        for (int i = 0; i < 64; ++i) {
                            vision[i] = (i % 8) < 4 ? 1.0f : 0.0f;
                        }
                        break;
                }
                
                percept.setVision(vision);
                percept.setTimestamp(static_cast<double>(step));
                
                // Process sensory input
                agent_->processSensoryInput(percept);
                
                // Get motor command
                MotorCommand action = agent_->decodeMotorCommand();
                
                // Apply action to world
                world_->applyMotorCommand(action, world_->getSimulationTime());
            }
            
            // Get final curiosity level
            float finalCuriosity = agent_->getCuriosityLevel();
            
            // Check if curiosity increased
            bool curiosityIncreased = finalCuriosity > initialCuriosity + 0.01f;
            
            bool passed = curiosityIncreased;
            
            std::string message = passed ? 
                "Curiosity-driven exploration test passed - curiosity increased with novelty" :
                "Curiosity-driven exploration test failed - curiosity did not increase";
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult result(getTestName(), passed, message);
            result.metrics["initial_curiosity"] = std::to_string(initialCuriosity);
            result.metrics["final_curiosity"] = std::to_string(finalCuriosity);
            result.metrics["curiosity_increase"] = std::to_string(finalCuriosity - initialCuriosity);
            
            return result;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResult(getTestName(), false, 
                            std::string("Exception: ") + e.what());
        }
    }
    
    std::string getTestName() const override { return "CuriosityDrivenExplorationTest"; }
};

// Test 6: Reward Learning Test
class RewardLearningTest : public AgentTest {
public:
    TestResult run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup world
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(width=15, height=15, visionWidth=8, visionHeight=8);
            world_->reset();
            
            // Initialize agent
            agent_->initialize(*world_);
            
            // Get initial prediction error
            float initialPredictionError = agent_->getPredictionError();
            
            // Apply different rewards
            std::vector<float> rewards = {0.5f, 1.0f, -0.5f, 2.0f};
            
            for (float reward : rewards) {
                // Apply reward modulation
                agent_->applyRewardModulation(reward, 0.0f); // Assume predicted reward is 0
                
                // Update development
                agent_->updateDevelopment(0.1);
            }
            
            // Get final prediction error
            float finalPredictionError = agent_->getPredictionError();
            
            // Check if prediction error changed
            bool predictionErrorChanged = std::abs(finalPredictionError - initialPredictionError) > 0.01f;
            
            bool passed = predictionErrorChanged;
            
            std::string message = passed ? 
                "Reward learning test passed - prediction error updated with rewards" :
                "Reward learning test failed - prediction error not affected by rewards";
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            TestResult result(getTestName(), passed, message);
            result.metrics["initial_prediction_error"] = std::to_string(initialPredictionError);
            result.metrics["final_prediction_error"] = std::to_string(finalPredictionError);
            result.metrics["prediction_error_change"] = std::to_string(finalPredictionError - initialPredictionError);
            
            return result;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResult(getTestName(), false, 
                            std::string("Exception: ") + e.what());
        }
    }
    
    std::string getTestName() const override { return "RewardLearningTest"; }
};

// Agent Test Suite - manages multiple tests
class AgentTestSuite {
private:
    TestConfig config_;
    std::vector<std::shared_ptr<AgentTest>> tests_;
    std::vector<TestResult> results_;
    
public:
    AgentTestSuite(const TestConfig& config) : config_(config) {}
    
    // Add a test
    void addTest(std::shared_ptr<AgentTest> test) {
        tests_.push_back(test);
    }
    
    // Add default tests
    void addDefaultTests() {
        tests_.push_back(std::make_shared<SensoryProcessingTest>());
        tests_.push_back(std::make_shared<MotorDecodingTest>());
        tests_.push_back(std::make_shared<NeuromodulationTest>());
        tests_.push_back(std::make_shared<DevelopmentProgressionTest>());
        tests_.push_back(std::make_shared<CuriosityDrivenExplorationTest>());
        tests_.push_back(std::make_shared<RewardLearningTest>());
    }
    
    // Run all tests
    void runAll() {
        std::cout << "=== NLM Agent Testing Framework ===" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  - Verbose: " << (config_.verbose ? "Yes" : "No") << std::endl;
        std::cout << "  - Continue on failure: " << (config_.continueOnFailure ? "Yes" : "No") << std::endl;
        std::cout << "  - Timeout: " << config_.timeout << " seconds" << std::endl;
        std::cout << "  - Random seed: " << config_.randomSeed << std::endl;
        std::cout << std::endl;
        
        int testsRun = 0;
        int testsPassed = 0;
        
        for (const auto& test : tests_) {
            if (!test->shouldRun()) continue;
            
            std::cout << "Running test: " << test->getTestName() << "..." << std::endl;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            TestResult result = test->run();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            result.executionTime = elapsed;
            
            results_.push_back(result);
            
            if (result.passed) {
                testsPassed++;
                std::cout << "  PASSED (" << std::fixed << std::setprecision(3) << elapsed << "s)" << std::endl;
            } else {
                std::cout << "  FAILED (" << std::fixed << std::setprecision(3) << elapsed << "s)" << std::endl;
                std::cout << "  Message: " << result.message << std::endl;
                
                if (!config_.continueOnFailure) {
                    std::cout << "Stopping test suite (continue-on-failure disabled)." << std::endl;
                    break;
                }
            }
            
            testsRun++;
        
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << testsRun << std::endl;
        std::cout << "Tests passed: " << testsPassed << std::endl;
        std::cout << "Tests failed: " << (testsRun - testsPassed) << std::endl;
        std::cout << "Success rate: " << (testsRun > 0 ? (testsPassed * 100.0 / testsRun) : 0.0) << "%" << std::endl;
        
        // Save results if configured
        if (config_.outputFile != "") {
            saveResults();
        }
    }
    
    // Save results to file
    void saveResults() {
        // Simple JSON output for now
        std::ofstream file(config_.outputFile);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open output file: " << config_.outputFile << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"test_suite\": \"NLM Agent Testing Framework\",\n";
        file << "  \"configuration\": {\n";
        file << "    \"verbose\": " << (config_.verbose ? "true" : "false") << ",\n";
        file << "    \"continue_on_failure\": " << (config_.continueOnFailure ? "true" : "false") << ",\n";
        file << "    \"timeout\": " << config_.timeout << ",\n";
        file << "    \"random_seed\": " << config_.randomSeed << ",\n";
        file << "    \"agent_params\": {\n";
        file << "      \"num_steps\": " << config_.agentParams.numSteps << ",\n";
        file << "      \"plasticity_strength\": " << config_.agentParams.plasticityStrength << ",\n";
        file << "      \"novelty_threshold\": " << config_.agentParams.noveltyThreshold << ",\n";
        file << "      \"enable_curiosity\": " << (config_.agentParams.enableCuriosity ? "true" : "false") << ",\n";
        file << "      \"enable_development\": " << (config_.agentParams.enableDevelopment ? "true" : "false") << ",\n";
        file << "      \"neuron_count\": " << config_.agentParams.neuronCount << ",\n";
        file << "      \"connection_probability\": " << config_.agentParams.connectionProbability << "\n";
        file << "    }\n";
        file << "  },\n";
        file << "  \"results\": [\n";
        
        for (size_t i = 0; i < results_.size(); ++i) {
            const auto& result = results_[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << result.testName << "\",\n";
            file << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
            file << "      \"execution_time\": " << result.executionTime << ",\n";
            file << "      \"message\": \"" << result.message << "\"";
            if (!result.metrics.empty()) {
                file << ",\n      \"metrics\": {\n";
                bool firstMetric = true;
                for (const auto& metric : result.metrics) {
                    if (!firstMetric) file << ",\n";
                    file << "        \"" << metric.first << "\": \"" << metric.second << "\"";
                    firstMetric = false;
                }
                file << "\n      }\n";
            } else {
                file << "\n";
            }
            
            if (i < results_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
        std::cout << "Results saved to: " << config_.outputFile << std::endl;
    }
    
    // Get all results
    const std::vector<TestResult>& getResults() const { return results_; }
};

// Convenience function to run agent tests
inline void runAgentTests(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                         const TestConfig& config = TestConfig()) {
    AgentTestSuite testSuite(config);
    testSuite.addDefaultTests();
    testSuite.runAll();
}

} // namespace testing
} // namespace nlm

#endif // NLM_AGENT_TESTING_FRAMEWORK_H
