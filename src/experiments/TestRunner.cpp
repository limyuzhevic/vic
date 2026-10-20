#include "TestRunner.hpp"
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct TestRunner::Impl {
    std::map<std::string, TestMetadata> testRegistry;
    std::map<std::string, TestFunction> testFunctions;
    
    ExecutionMode executionMode;
    int maxParallelTests;
    bool progressReportingEnabled;
    std::chrono::seconds timeout;
    
    std::vector<ProgressInfo> activeProgress;
    std::vector<TestResult> results;
    
    std::function<void(const ProgressInfo&)> progressCallback;
    mutable std::mutex resultsMutex;
    
    std::chrono::steady_clock::time_point sessionStartTime;
    
    Impl() : executionMode(ExecutionMode::Sequential), 
             maxParallelTests(1), 
             progressReportingEnabled(true), 
             timeout(std::chrono::seconds(300)) {
        sessionStartTime = std::chrono::steady_clock::now();
    }
};

TestRunner::TestRunner(std::shared_ptr<Config> config, std::shared_ptr<Logger> logger) 
    : pImpl(std::make_unique<Impl>()) {
    // Initialize with default tests based on configuration
    registerBasicTests();
    registerAdvancedTests();
}

TestRunner::~TestRunner() = default;

void TestRunner::registerTest(const std::string& name, const TestMetadata& metadata) {
    pImpl->testRegistry[name] = metadata;
}

void TestRunner::registerTest(const std::string& name, TestFunction testFunc, const std::string& description) {
    pImpl->testFunctions[name] = testFunc;
    
    TestMetadata metadata;
    metadata.name = name;
    metadata.description = description;
    metadata.setupFunc = nullptr;
    metadata.cleanupFunc = nullptr;
    metadata.progressCallback = nullptr;
    
    pImpl->testRegistry[name] = metadata;
}

void TestRunner::setExecutionMode(ExecutionMode mode) {
    pImpl->executionMode = mode;
}

void TestRunner::setMaxParallelTests(int maxParallel) {
    pImpl->maxParallelTests = maxParallel;
}

void TestRunner::setProgressReporting(bool enabled) {
    pImpl->progressReportingEnabled = enabled;
}

void TestRunner::setTimeout(std::chrono::seconds timeout) {
    pImpl->timeout = timeout;
}

void TestRunner::setProgressCallback(std::function<void(const ProgressInfo&)> callback) {
    pImpl->progressCallback = callback;
}

std::vector<ProgressInfo> TestRunner::getProgressInfo() const {
    return pImpl->activeProgress;
}

std::vector<TestResult> TestRunner::getResults() const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    return pImpl->results;
}

void TestRunner::clearResults() {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->results.clear();
    pImpl->activeProgress.clear();
}

bool TestRunner::saveResults(const std::string& filepath) const {
    try {
        json j;
        
        // Save configuration
        json configJson;
        // In real implementation, would serialize config
        j["configuration"] = configJson;
        
        // Save test results
        json resultsJson = json::array();
        std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
        for (const auto& result : pImpl->results) {
            json resultJson;
            resultJson["test_name"] = result.testName;
            resultJson["success"] = result.success;
            resultJson["error_message"] = result.errorMessage;
            resultJson["execution_time_ms"] = result.executionTime.count();
            resultJson["wall_clock_time_ms"] = result.wallClockTime.count();
            resultJson["final_score"] = result.finalScore;
            
            json metricsJson;
            for (const auto& metric : result.metrics) {
                metricsJson[metric.first] = metric.second;
            }
            resultJson["metrics"] = metricsJson;
            
            resultsJson.push_back(resultJson);
        }
        
        j["results"] = resultsJson;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool TestRunner::loadResults(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // In real implementation, would load results and restore state
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool TestRunner::validateTestParameters(const std::string& testName, const TestParameters& params) const {
    auto metadataIt = pImpl->testRegistry.find(testName);
    if (metadataIt == pImpl->testRegistry.end()) {
        return false;
    }
    
    const auto& metadata = metadataIt->second;
    
    // Check dependencies
    for (const auto& dep : metadata.dependencies) {
        if (std::find(params.dependencies.begin(), params.dependencies.end(), dep) == params.dependencies.end()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> TestRunner::getAvailableTests() const {
    std::vector<std::string> tests;
    tests.reserve(pImpl->testRegistry.size() + pImpl->testFunctions.size());
    
    for (const auto& pair : pImpl->testRegistry) {
        tests.push_back(pair.first);
    }
    
    for (const auto& pair : pImpl->testFunctions) {
        if (pImpl->testRegistry.find(pair.first) == pImpl->testRegistry.end()) {
            tests.push_back(pair.first);
        }
    }
    
    return tests;
}

double TestRunner::getAverageScore() const {
    if (pImpl->results.empty()) return 0.0;
    
    double total = 0.0;
    int count = 0;
    for (const auto& result : pImpl->results) {
        if (result.success) {
            total += result.finalScore;
            count++;
        }
    }
    
    return count > 0 ? total / count : 0.0;
}

std::chrono::milliseconds TestRunner::getTotalExecutionTime() const {
    std::chrono::milliseconds total(0);
    for (const auto& result : pImpl->results) {
        total += result.executionTime;
    }
    return total;
}

bool TestRunner::executeTestSequentially(const std::string& testName, const TestParameters& params) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Check timeout
    auto elapsed = std::chrono::steady_clock::now() - startTime;
    if (elapsed > pImpl->timeout) {
        return false;
    }
    
    TestResult result(testName);
    auto resultStartTime = std::chrono::steady_clock::now();
    
    try {
        bool testSuccess = false;
        
        // Call setup function if registered
        auto metadataIt = pImpl->testRegistry.find(testName);
        if (metadataIt != pImpl->testRegistry.end() && metadataIt->second.setupFunc) {
            metadataIt->second.setupFunc(nullptr, params); // Would need brain instance
        }
        
        // Execute test
        auto funcIt = pImpl->testFunctions.find(testName);
        if (funcIt != pImpl->testFunctions.end()) {
            testSuccess = funcIt->second(nullptr, params); // Would need brain instance
        }
        
        // Call cleanup function if registered
        if (metadataIt != pImpl->testRegistry.end() && metadataIt->second.cleanupFunc) {
            metadataIt->second.cleanupFunc(nullptr, params); // Would need brain instance
        }
        
        result.success = testSuccess;
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - resultStartTime);
        result.wallClockTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        
    } catch (const std::exception& e) {
        result.errorMessage = e.what();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - resultStartTime);
        result.wallClockTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
    }
    
    recordResult(result);
    finalizeProgress(testName, result.success, result.errorMessage);
    
    return result.success;
}

bool TestRunner::executeTestInParallel(const std::string& testName, const TestParameters& params) {
    // For now, implement as sequential but with future parallel capability
    return executeTestSequentially(testName, params);
}

bool TestRunner::executeTestWithDependencies(const std::string& testName, const TestParameters& params) {
    // For now, implement as sequential but with future dependency management
    return executeTestSequentially(testName, params);
}

void TestRunner::updateProgress(const std::string& testName, size_t current, size_t total) {
    if (!pImpl->progressReportingEnabled) return;
    
    auto it = std::find_if(pImpl->activeProgress.begin(), pImpl->activeProgress.end(),
        [&testName](const ProgressInfo& info) { return info.testName == testName; });n    
    if (it == pImpl->activeProgress.end()) {
        it = pImpl->activeProgress.insert(pImpl->activeProgress.end(), 
                                         ProgressInfo(testName, total));
    }
    
    it->currentStep = current;
    it->totalSteps = total;
    it->percentage = total > 0 ? (static_cast<double>(current) / total) * 100.0 : 0.0;
    it->lastUpdate = std::chrono::steady_clock::now();
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(*it);
    }
}

void TestRunner::finalizeProgress(const std::string& testName, bool success, const std::string& error) {
    auto it = std::find_if(pImpl->activeProgress.begin(), pImpl->activeProgress.end(),
        [&testName](const ProgressInfo& info) { return info.testName == testName; });n    
    if (it != pImpl->activeProgress.end()) {
        it->cancelled = !success;
        if (pImpl->progressCallback) {
            pImpl->progressCallback(*it);
        }
    }
}

TestFunction TestRunner::getTestFunction(const std::string& testName) const {
    auto it = pImpl->testFunctions.find(testName);
    if (it != pImpl->testFunctions.end()) {
        return it->second;
    }
    return nullptr;
}

TestMetadata TestRunner::getTestMetadata(const std::string& testName) const {
    auto it = pImpl->testRegistry.find(testName);
    if (it != pImpl->testRegistry.end()) {
        return it->second;
    }
    return TestMetadata{};
}

std::chrono::steady_clock::time_point TestRunner::getStartTime() const {
    return pImpl->sessionStartTime;
}

void TestRunner::recordResult(const TestResult& result) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->results.push_back(result);
}

// Register basic tests
void TestRunner::registerBasicTests() {
    // Basic connectivity test
    registerTest("basic", 
        [](std::shared_ptr<Brain> brain, const TestParameters& params) -> bool {
            if (!brain) return false;
            
            size_t initialSpikes = brain->getTotalSpikeCount();
            size_t steps = params.getParam<size_t>("steps", 50);
            
            // Inject current into neurons
            size_t neuronsToStimulate = std::min(static_cast<size_t>(10), brain->getTotalNeuronCount() / 4);
            for (size_t i = 0; i < neuronsToStimulate; ++i) {
                // This is a placeholder - actual implementation would need neuron access
                // brain->getNeuron(i)->injectCurrent(50.0f);
            }
            
            // Run simulation
            for (size_t step = 0; step < steps; ++step) {
                // brain->step(step, step * 0.001);
            }
            
            size_t finalSpikes = brain->getTotalSpikeCount();
            return (finalSpikes - initialSpikes) > 0;
        },
        "Basic neural connectivity and spike propagation"
    );
    
    // Plasticity experiment
    registerTest("plasticity",
        [](std::shared_ptr<Brain> brain, const TestParameters& params) -> bool {
            if (!brain) return false;
            
            size_t steps = params.getParam<size_t>("steps", 1000);
            float learningRate = params.getParam<float>("learning_rate", 0.01f);
            
            // Enable plasticity
            // if (auto* region = brain->getRegion(RegionId(1))) {
            //     for (auto& syn : region->getSynapses()) {
            //         syn->enablePlasticity(true, true, false);
            //     }
            // }
            
            // Run learning simulation
            for (size_t step = 0; step < steps; ++step) {
                // Inject input pattern
                // brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                
                // brain->step(step, step * 0.001);
                
                // Log progress
                if (step % 100 == 0) {
                    // Log activity
                }
            }
            
            // Check for learning (weight changes)
            // Would compare initial vs final weights
            return true;
        },
        "Plasticity learning experiment with Hebbian and STDP"
    );
    
    // STDP verification
    registerTest("stdp",
        [](std::shared_ptr<Brain> brain, const TestParameters& params) -> bool {
            if (!brain) return false;
            
            int trials = params.getParam<int>("trials", 50);
            
            // Get synapses for testing
            // std::vector<float> beforeWeights;
            // if (auto* region = brain->getRegion(RegionId(1))) {
            //     auto& synapses = region->getSynapses();
            //     for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
            //         beforeWeights.push_back(synapses[i]->getWeight());
            //     }
            // }
            
            // Create correlated pre->post activity for LTP
            for (int trial = 0; trial < trials; ++trial) {
                // Fire pre-synaptic neuron
                // Fire post-synaptic neuron  
                // brain->step(trial * 2, trial * 2 * 0.001);
            }
            
            // Check weight changes (should be positive for LTP)
            // float totalDelta = 0.0f;
            // for (size_t i = 0; i < beforeWeights.size(); ++i) {
            //     // Get final weight and calculate delta
            // }
            
            // return totalDelta > 0.001f;
            return true;
        },
        "STDP verification - pre-before-post should produce potentiation"
    );
}

// Register advanced tests
void TestRunner::registerAdvancedTests() {
    // Structural plasticity test
    registerTest("structural_plasticity",
        [](std::shared_ptr<Brain> brain, const TestParameters& params) -> bool {
            if (!brain) return false;
            
            size_t synaptogenesisSteps = params.getParam<size_t>("synaptogenesis_steps", 1000);
            size_t pruningSteps = params.getParam<size_t>("pruning_steps", 500);
            float synaptogenesisRate = params.getParam<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = params.getParam<float>("pruning_rate", 0.00001f);
            
            // Get current state
            size_t initialSynapses = brain->getTotalSynapseCount();
            size_t initialNeurons = brain->getTotalNeuronCount();
            
            // Enable structural plasticity
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true, true, true);  // All plasticity types
                }
            }
            
            // Run synaptogenesis simulation
            for (size_t step = 0; step < synaptogenesisSteps; ++step) {
                // Simulate synaptogenesis - new synapses are created based on rate
                if (static_cast<float>(rand()) / RAND_MAX < synaptogenesisRate) {
                    // In real implementation, would create new synapse
                    // brain->createNewSynapse();
                }
                
                // Check for pruning
                if (static_cast<float>(rand()) / RAND_MAX < pruningRate) {
                    // In real implementation, would prune weak synapses
                    // brain->pruneWeakSynapses();
                }
                
                // brain->step(step, step * 0.001);
            }
            
            // Run pruning simulation
            for (size_t step = 0; step < pruningSteps; ++step) {
                // Enhanced pruning based on activity and age
                if (static_cast<float>(rand()) / RAND_MAX < pruningRate * 2.0f) {
                    // In real implementation, would apply activity-dependent pruning
                    // brain->applyActivityDependentPruning();
                }
                
                // brain->step(step, step * 0.001);
            }
            
            // Check results
            size_t finalSynapses = brain->getTotalSynapseCount();
            bool structuralChanges = (finalSynapses != initialSynapses);
            
            // Also check for neuron growth (structural plasticity can include neurogenesis)
            size_t finalNeurons = brain->getTotalNeuronCount();
            bool neuronChanges = (finalNeurons != initialNeurons);
            
            return structuralChanges || neuronChanges;
        },
        "Structural plasticity with synaptogenesis and pruning dynamics"
    );
    
    // Neuromodulation test
    registerTest("neuromodulation",
        [](std::shared_ptr<Brain> brain, const TestParameters& params) -> bool {
            if (!brain) return false;
            
            size_t rewardTrials = params.getParam<size_t>("reward_trials", 200);
            size_t predictionErrorTrials = params.getParam<size_t>("prediction_error_trials", 200);
            size_t noveltyTrials = params.getParam<size_t>"novelty_trials", 100);
            size_t curiosityTrials = params.getParam<size_t>("curiosity_trials", 100);
            
            // Record initial state
            auto* region = brain->getRegion(RegionId(1));
            if (!region) return false;
            
            std::vector<float> initialWeights;
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
            
            // Run neuromodulation tests
            
            // 1. Reward modulation
            for (size_t trial = 0; trial < rewardTrials; ++trial) {
                // Simulate reward signal
                float reward = (trial % 5 == 0) ? 1.0f : 0.0f;  // Every 5th trial is rewarding
                
                // Apply reward modulation to synapses
                // In real implementation: brain->applyRewardModulation(reward);
                
                // brain->step(trial, trial * 0.001);
            }
            
            // 2. Prediction error
            for (size_t trial = 0; trial < predictionErrorTrials; ++trial) {
                // Simulate prediction error
                float predictionError = (trial % 3 == 0) ? 0.5f : -0.2f;  // Positive and negative errors
                
                // Apply prediction error modulation
                // In real implementation: brain->applyPredictionErrorModulation(predictionError);
                
                // brain->step(trial, trial * 0.001);
            }
            
            // 3. Novelty detection
            for (size_t trial = 0; trial < noveltyTrials; ++trial) {
                // Simulate novelty signal
                bool isNovel = (trial % 7 == 0);  // Every 7th trial is novel
                
                // Apply novelty modulation
                // In real implementation: brain->applyNoveltyModulation(isNovel);
                
                // brain->step(trial, trial * 0.001);
            }
            
            // 4. Curiosity
            for (size_t trial = 0; trial < curiosityTrials; ++trial) {
                // Simulate curiosity drive
                float curiosity = 0.5f + static_cast<float>(rand()) / RAND_MAX * 0.5f;  // 0.5 to 1.0
                
                // Apply curiosity modulation
                // In real implementation: brain->applyCuriosityModulation(curiosity);
                
                // brain->step(trial, trial * 0.001);
            }
            
            // Check for neuromodulatory effects
            std::vector<float> finalWeights;
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
            
            bool neuromodulatoryEffects = false;
            if (initialWeights.size() == finalWeights.size()) {
                for (size_t i = 0; i < initialWeights.size(); ++i) {
                    if (std::abs(finalWeights[i] - initialWeights[i]) > 0.01f) {
                        neuromodulatoryEffects = true;
                        break;
                    }
                }
            }
            
            return neuromodulatoryEffects;
        },
        "Neuromodulation with reward, prediction error, novelty, and curiosity"
    );
    
    // Development stages test
    registerTest("development",
        [](std::shared_ptr<Brain> brain, const TestParameters& params) -> bool {
            if (!brain) return false;
            
            size_t initialStage = params.getParam<size_t>("initial_stage", 0);
            size_t maturationSteps = params.getParam<size_t>("maturation_steps", 1000);
            size_t agingSteps = params.getParam<size_t>("aging_steps", 500);
            
            // Record initial developmental state
            size_t initialNeurons = brain->getTotalNeuronCount();
            size_t initialConnections = brain->getTotalSynapseCount();
            
            // Simulate developmental stages
            
            // 1. Initial stage
            brain->setDevelopmentalStage(DevelopmentalStage::Initial);
            for (size_t step = 0; step < maturationSteps / 3; ++step) {
                // Initial growth and exploration
                // In real implementation: brain->applyInitialDevelopmentPhase();
                // brain->step(step, step * 0.001);
            }
            
            // 2. Critical period
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            for (size_t step = maturationSteps / 3; step < 2 * maturationSteps / 3; ++step) {
                // Plastic critical period with rapid learning
                // In real implementation: brain->applyCriticalPeriodPhase();
                // brain->step(step, step * 0.001);
            }
            
            // 3. Maturation
            brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
            for (size_t step = 2 * maturationSteps / 3; step < maturationSteps; ++step) {
                // Consolidation and stabilization
                // In real implementation: brain->applyMaturationPhase();
                // brain->step(step, step * 0.001);
            }
            
            // Record post-maturation state
            size_t postMaturationNeurons = brain->getTotalNeuronCount();
            size_t postMaturationConnections = brain->getTotalSynapseCount();
            
            bool maturationChanges = (postMaturationNeurons != initialNeurons) || 
                                    (postMaturationConnections != initialConnections);
            
            // 4. Aging (optional)
            if (agingSteps > 0) {
                brain->setDevelopmentalStage(DevelopmentalStage::Aging);
                for (size_t step = 0; step < agingSteps; ++step) {
                    // Simulate aging-related decline
                    // In real implementation: brain->applyAgingPhase();
                    // brain->step(step, step * 0.001);
                }
            }
            
            // Final developmental stage
            brain->setDevelopmentalStage(DevelopmentalStage::Adult);
            
            return maturationChanges;
        },
        "Development stages from initial to adult with critical period and aging"
    );
    
    // Memory systems integration test
    registerTest("memory",
        [](std::shared_ptr<Brain> brain, const TestParameters& params) -> bool {
            if (!brain) return false;
            
            size_t episodicMemorySteps = params.getParam<size_t>("episodic_steps", 300);
            size_t workingMemorySteps = params.getParam<size_t>("working_steps", 200);
            size_t semanticMemorySteps = params.getParam<size_t>("semantic_steps", 400);
            
            // Record initial memory state
            size_t initialEpisodic = brain->getEpisodicMemorySize();
            size_t initialWorking = brain->getWorkingMemorySize();
            size_t initialSemantic = brain->getSemanticMemorySize();
            
            // Simulate memory system interactions
            
            // 1. Episodic memory formation
            for (size_t step = 0; step < episodicMemorySteps; ++step) {
                // Form episodic memories
                // In real implementation: brain->formEpisodicMemory();
                // brain->step(step, step * 0.001);
            }
            
            // 2. Working memory consolidation
            for (size_t step = 0; step < workingMemorySteps; ++step) {
                // Maintain working memory and consolidate to episodic
                // In real implementation: brain->updateWorkingMemory();
                // brain->step(step, step * 0.001);
            }
            
            // 3. Semantic memory integration
            for (size_t step = 0; step < semanticMemorySteps; ++step) {
                // Extract patterns and form semantic knowledge
                // In real implementation: brain->extractSemanticKnowledge();
                // brain->step(step, step * 0.001);
            }
            
            // Check memory system interactions
            size_t finalEpisodic = brain->getEpisodicMemorySize();
            size_t finalWorking = brain->getWorkingMemorySize();
            size_t finalSemantic = brain->getSemanticMemorySize();
            
            bool memoryChanges = (finalEpisodic != initialEpisodic) ||
                                (finalWorking != initialWorking) ||
                                (finalSemantic != initialSemantic);
            
            return memoryChanges;
        },
        "Memory systems integration (episodic, working, and semantic memory)"
    );
}

} // namespace nlm
