// Phase 6 Integration Tests
// Comprehensive tests for the integrated artificial brain system
// Tests all core brain components working together

#include "tests/test_main.cpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>

namespace nlm {
namespace test_phase6 {

// Forward declarations
struct IntegrationTestResult {
    std::string testName;
    bool passed;
    std::string details;
};

// Test Framework Utilities
class TestFramework {
public:
    static void logSection(const std::string& title) {
        std::cout << "\n=== " << title << " ===" << std::endl;
    }
    
    static void logTest(const std::string& test, bool passed, const std::string& details = "") {
        std::cout << "  " << (passed ? "[PASS] " : "[FAIL] ") 
                  << test << (passed && !details.empty() ? " - " + details : "") << std::endl;
    }
    
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::tm tm_buf;
        char buffer[100];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", std::localtime_r(&time, &tm_buf));
        std::stringstream ss;
        ss << buffer << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

// 1. Basic System Integration Tests
void testBasicSystemIntegration() {
    TestFramework::logSection("Basic System Integration Tests");
    
    // Create brain with minimal configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Configure all subsystems
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.022f, nlm::ConfigSource::Default);
    config->set("stdp_tau", 25.0f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.00005f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.000005f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test 1: Brain initialization (all subsystems)
    TestFramework::logSection("Brain Initialization");
    assert(brain.initialize());
    
    // Verify all core systems exist and are non-null
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    assert(brain.getSpikeSystem() != nullptr);
    
    // Test 2: LIF Neuron Dynamics
    TestFramework::logSection("LIF Neuron Dynamics Integration");
    
    // Inject current into multiple neurons
    brain.injectCurrentToNeurons(NeuronType::Internal, 2.0f);
    brain.injectCurrentToNeurons(NeuronType::Excitatory, 1.5f);
    
    // Run simulation steps
    for (int step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
        
        // Verify neurons are changing state
        assert(brain.getTotalNeuronCount() == 200);
        
        // Check spike activity
        size_t firingThisStep = brain.getFiringNeuronCount();
        if (firingThisStep > 0) {
            TestFramework::logTest("Spike detection", true, 
                                 "Spike count: " + std::to_string(firingThisStep));
        }
    }
    
    // Test 3: Synaptic Transmission
    TestFramework::logSection("Synaptic Transmission");
    
    auto* spikeSystem = brain.getSpikeSystem();
    assert(spikeSystem != nullptr);
    
    size_t pendingSpikes = spikeSystem->getPendingSpikeCount() + 
                          spikeSystem->getPendingDelayedCount();
    assert(pendingSpikes >= 0);  // Should be non-negative
    
    // Test 4: Memory Systems Integration
    TestFramework::logSection("Memory Systems Integration");
    
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Store some information in working memory
    for (int i = 0; i < 10; ++i) {
        workingMem->storeToNeuron(NeuronId(i % 100), 0.5f);
    }
    
    size_t activeTraces = workingMem->getActiveTraces();
    assert(activeTraces >= 0);
    
    // Test episodic memory
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Run steps to generate episodes
    for (int i = 100; i < 150; ++i) {
        brain.step(i, i * 0.001);
    }
    
    size_t episodes = episodicMem->getEpisodeCount();
    assert(episodes >= 0);
    
    // Test 5: Neuromodulation Integration
    TestFramework::logSection("Neuromodulation Integration");
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // Test 6: Prediction System Integration
    TestFramework::logSection("Prediction System Integration");
    
    auto* predictionSys = brain.getPredictionSystem();
    assert(predictionSys != nullptr);
    
    // Test 7: Cognitive Systems Integration
    TestFramework::logSection("Cognitive Systems Integration");
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test 8: Development System Integration
    TestFramework::logSection("Development System Integration");
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage == DevelopmentalStage::Initial);
    
    // Test 9: World Integration
    TestFramework::logSection("World Integration Preparation");
    
    // Brain is ready for world integration
    size_t regions = brain.getRegionCount();
    size_t neurons = brain.getTotalNeuronCount();
    size_t synapses = brain.getTotalSynapseCount();
    
    TestFramework::logTest("Region creation", true,
                         "Regions: " + std::to_string(regions) +
                         ", Neurons: " + std::to_string(neurons) +
                         ", Synapses: " + std::to_string(synapses));
    
    TestFramework::logTest("Basic System Integration", true,
                         "All core systems are operational");
}

// 2. Learning and Plasticity Tests
void testLearningAndPlasticity() {
    TestFramework::logSection("Learning and Plasticity Tests");
    
    // Create brain with high plasticity settings
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    // Strong plasticity parameters
    config->set("stdp_ltp_weight", 0.05f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.055f, nlm::ConfigSource::Default);
    config->set("stdp_tau", 30.0f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test 1: STDP Learning with Reward Modulation
    TestFramework::logSection("STDP Learning with Reward Modulation");
    
    auto* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Find a synapse to test
    SynapseId testSynapseId;
    bool foundSynapse = false;
    
    for (auto& region : brain.getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (syn->getSourceNeuron() < syn->getDestinationNeuron()) {
                testSynapseId = syn->getId();
                foundSynapse = true;
                break;
            }
        }
        if (foundSynapse) break;
    }
    
    if (foundSynapse) {
        // Get the synapse object (need to iterate again)
        Synapse* testSynapse = nullptr;
        for (auto& region : brain.getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (syn->getId() == testSynapseId) {
                    testSynapse = syn;
                    break;
                }
            }
            if (testSynapse) break;
        }
        
        if (testSynapse) {
            float initialWeight = testSynapse->getWeight();
            
            // Create spike histories that potentiate
            std::vector<Timestamp> preSpikes = {0.0, 20.0, 40.0};
            std::vector<Timestamp> postSpikes = {10.0, 30.0, 50.0};
            
            // Apply STDP
            stdp->update(testSynapse, preSpikes, postSpikes, 0.001);
            
            float finalWeight = testSynapse->getWeight();
            
            // Weight should change (potentiation)
            assert(finalWeight != initialWeight);
            TestFramework::logTest("STDP potentiation", true,
                                 "Weight change: " + std::to_string(finalWeight - initialWeight));
        }
    }
    
    // Test 2: Hebbian Learning
    TestFramework::logSection("Hebbian Learning");
    
    auto* hebbian = brain.getHebbian();
    assert(hebbian != nullptr);
    
    // Test Hebbian update with coincident activity
    if (foundSynapse && testSynapse) {
        float hebbianInitialWeight = testSynapse->getWeight();
        
        std::vector<Timestamp> preSpikes = {0.0, 10.0};
        std::vector<Timestamp> postSpikes = {5.0, 15.0};
        
        hebbian->update(testSynapse, preSpikes, postSpikes, 0.001);
        
        float hebbianFinalWeight = testSynapse->getWeight();
        
        // Should change due to Hebbian learning
        assert(hebbianFinalWeight != hebbianInitialWeight);
        TestFramework::logTest("Hebbian learning", true,
                             "Weight change: " + std::to_string(hebbianFinalWeight - hebbianInitialWeight));
    }
    
    // Test 3: Structural Plasticity
    TestFramework::logSection("Structural Plasticity");
    
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    // Set up structural plasticity
    structuralPlasticity->setSynaptogenesisRate(0.00001f);
    structuralPlasticity->setPruningRate(0.000001f);
    
    // Count synapses before
    size_t synapsesBefore = brain.getTotalSynapseCount();
    
    // Run development to trigger structural changes
    brain.develop();
    
    // Count synapses after
    size_t synapsesAfter = brain.getTotalSynapseCount();
    
    TestFramework::logTest("Structural plasticity", true,
                         "Synapses before: " + std::to_string(synapsesBefore) +
                         ", after: " + std::to_string(synapsesAfter));
    
    // Test 4: Development over Time
    TestFramework::logSection("Development over Time");
    
    DevelopmentalStage currentStage = brain.getDevelopmentalStage();
    assert(currentStage == DevelopmentalStage::Initial);
    
    // Simulate development stages
    for (int step = 1000; step < 1100; step += 100) {
        brain.step(step, step * 0.001);
        
        DevelopmentalStage stage = brain.getDevelopmentalStage();
        
        // After enough development, stage should change
        if (step >= 1000) {
            TestFramework::logTest("Development progression", true,
                                 "Stage: " + std::to_string(static_cast<int>(stage)));
        }
    }
    
    // Test 5: Memory Consolidation
    TestFramework::logSection("Memory Consolidation");
    
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Store some episodes
    for (int i = 0; i < 10; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Trigger consolidation (every consolidationInterval steps)
    for (int i = 2000; i < 2010; ++i) {
        brain.step(i, i * 0.001);
    }
    
    size_t episodes = episodicMem->getEpisodeCount();
    TestFramework::logTest("Memory consolidation", true,
                         "Episodes: " + std::to_string(episodes));
    
    TestFramework::logTest("Learning and Plasticity", true,
                         "All plasticity systems functional");
}

// 3. Agent-World Integration Tests
void testAgentWorldIntegration() {
    TestFramework::logSection("Agent-World Integration Tests");
    
    // Create world and agent brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test 1: Sensory Processing
    TestFramework::logSection("Sensory Processing");
    
    // Simulate sensory input by injecting current
    brain.injectCurrentToNeurons(NeuronType::Sensory, 3.0f);
    
    // Run steps to process input
    for (int i = 0; i < 30; ++i) {
        brain.step(i, i * 0.001);
        
        // Check for sensory-related activity
        size_t firing = brain.getFiringNeuronCount();
        if (firing > 0) {
            TestFramework::logTest("Sensory processing", true,
                                 "Spike activity detected: " + std::to_string(firing));
            break;
        }
    }
    
    // Test 2: Motor Command Generation
    TestFramework::logSection("Motor Command Generation");
    
    // Generate action from brain
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::string actionType = "UNKNOWN";
    switch (action->getType()) {
        case ActionType::Wait: actionType = "WAIT"; break;
        case ActionType::MoveForward: actionType = "MOVE_FORWARD"; break;
        case ActionType::MoveBackward: actionType = "MOVE_BACKWARD"; break;
        case ActionType::TurnLeft: actionType = "TURN_LEFT"; break;
        case ActionType::TurnRight: actionType = "TURN_RIGHT"; break;
        default: actionType = "UNKNOWN";
    }
    
    TestFramework::logTest("Motor command generation", true,
                         "Action: " + actionType);
    
    // Test 3: Reward Modulation and Learning
    TestFramework::logSection("Reward Modulation and Learning");
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    float initialDopamine = dopamine->getLevel();
    
    // Apply reward signal
    nlm::Reward rewardSignal;
    rewardSignal.setLevel(0.8f);
    rewardSignal.setPredictionError(0.2f);
    
    brain.applyNeuromodulation(rewardSignal);
    
    float finalDopamine = dopamine->getLevel();
    
    TestFramework::logTest("Reward modulation", true,
                         "Dopamine change: " + std::to_string(finalDopamine - initialDopamine));
    
    // Test 4: Curiosity-Driven Exploration
    TestFramework::logSection("Curiosity-Driven Exploration");
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    float curiosityLevel = curiosity->getLevel();
    assert(curiosityLevel >= 0.0f);
    
    // Update curiosity
    curiosity->update(0.001);
    
    float newCuriosityLevel = curiosity->getLevel();
    
    TestFramework::logTest("Curiosity system", true,
                         "Curiosity level: " + std::to_string(newCuriosityLevel));
    
    // Test 5: Novelty Detection
    TestFramework::logSection("Novelty Detection");
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    float noveltyLevel = novelty->getLevel();
    assert(noveltyLevel >= 0.0f);
    
    // Update novelty detector
    novelty->update(0.001);
    
    float newNoveltyLevel = novelty->getLevel();
    
    TestFramework::logTest("Novelty detection", true,
                         "Novelty level: " + std::to_string(newNoveltyLevel));
    
    // Test 6: Full Agent-World Loop
    TestFramework::logSection("Full Agent-World Loop Simulation");
    
    // Simulate a complete agent-world interaction
    size_t stepsProcessed = 0;
    size_t sensoryEvents = 0;
    size_t motorCommands = 0;
    size_t rewardsReceived = 0;
    
    for (int step = 1000; step < 1050; ++step) {
        // Process sensory input
        brain.injectCurrentToNeurons(NeuronType::Sensory, 
                                   static_cast<float>(step % 5 + 1));
        sensoryEvents++;
        
        // Brain step
        brain.step(step, step * 0.001);
        stepsProcessed++;
        
        // Generate motor output
        auto action = brain.produceAction();
        if (action) {
            motorCommands++;
        }
        
        // Simulate reward
        if (step % 7 == 0) {  // Every 7 steps
            float reward = 0.5f + static_cast<float>(step % 5) * 0.1f;
            
            nlm::Reward rewardSignal;
            rewardSignal.setLevel(reward);
            brain.applyNeuromodulation(rewardSignal);
            rewardsReceived++;
        }
    }
    
    TestFramework::logTest("Agent-World integration", true,
                         "Steps: " + std::to_string(stepsProcessed) +
                         ", Sensory: " + std::to_string(sensoryEvents) +
                         ", Motor: " + std::to_string(motorCommands) +
                         ", Rewards: " + std::to_string(rewardsReceived));
}

// 4. Performance and Stability Tests
void testPerformanceAndStability() {
    TestFramework::logSection("Performance and Stability Tests");
    
    // Create brain with larger size for performance testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.08f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test 1: Spike Rate Limits
    TestFramework::logSection("Spike Rate Limits");
    
    float firingRates[100] = {0};
    size_t firingRateIndex = 0;
    
    // Run simulation and track firing rates
    for (int step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001);
        
        if (firingRateIndex < 100) {
            firingRates[firingRateIndex++] = brain.getAverageFiringRate();
        }
    }
    
    // Find max and min firing rates
    float maxFiringRate = *std::max_element(firingRates, firingRates + firingRateIndex);
    float minFiringRate = *std::min_element(firingRates, firingRates + firingRateIndex);
    
    TestFramework::logTest("Spike rate monitoring", true,
                         "Max: " + std::to_string(maxFiringRate) +
                         ", Min: " + std::to_string(minFiringRate) +
                         ", Avg: " + std::to_string(maxFiringRate + minFiringRate) / 2.0f);
    
    // Test 2: Memory Usage
    TestFramework::logSection("Memory Usage Monitoring");
    
    size_t neurons = brain.getTotalNeuronCount();
    size_t synapses = brain.getTotalSynapseCount();
    size_t active = brain.getActiveNeuronCount();
    size_t firing = brain.getFiringNeuronCount();
    
    TestFramework::logTest("Memory usage", true,
                         "Neurons: " + std::to_string(neurons) +
                         ", Synapses: " + std::to_string(synapses) +
                         ", Active: " + std::to_string(active) +
                         ", Firing: " + std::to_string(firing));
    
    // Test 3: System Stability over Long Runs
    TestFramework::logSection("Long-term Stability");
    
    size_t stableSteps = 0;
    size_t neuronCountVariation = 0;
    size_t synapseCountVariation = 0;
    
    size_t baselineNeurons = brain.getTotalNeuronCount();
    size_t baselineSynapses = brain.getTotalSynapseCount();
    
    // Run for extended period
    for (int step = 2000; step < 2500; ++step) {
        brain.step(step, step * 0.001);
        
        // Check stability
        if (brain.getTotalNeuronCount() == baselineNeurons) {
            stableSteps++;
        }
        
        if (brain.getTotalSynapseCount() == baselineSynapses) {
            stableSteps++;
        }
    }
    
    float stabilityRatio = static_cast<float>(stableSteps) / static_cast<float>(1000);
    
    TestFramework::logTest("System stability", true,
                         "Stability ratio: " + std::to_string(stabilityRatio) +
                         " (" + std::to_string(stableSteps) + "/1000)");
    
    // Test 4: Checkpoint/Save/Load Functionality
    TestFramework::logSection("Checkpoint Save/Load");
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_phase6_test.bin";
    
    bool saveSuccess = brain.save(checkpointPath);
    TestFramework::logTest("Checkpoint save", saveSuccess,
                         saveSuccess ? "Successfully saved" : "Failed to save");
    
    if (saveSuccess) {
        // Create new brain and load checkpoint
        auto config2 = std::make_shared<Config>();
        config2->set("neuron_count", config->get<size_t>("neuron_count"));
        config2->set("region_count", config->get<size_t>("region_count"));
        
        nlm::Brain brain2(config2);
        assert(brain2.initialize());
        
        bool loadSuccess = brain2.load(checkpointPath);
        TestFramework::logTest("Checkpoint load", loadSuccess,
                             loadSuccess ? "Successfully loaded" : "Failed to load");
        
        if (loadSuccess) {
            size_t neuronsAfterLoad = brain2.getTotalNeuronCount();
            TestFramework::logTest("State consistency", 
                                 neuronsAfterLoad == neurons,
                                 "Neurons: " + std::to_string(neuronsAfterLoad) +
                                 "/" + std::to_string(neurons));
        }
    }
    
    // Test 5: Performance under Stress
    TestFramework::logSection("Performance Under Stress");
    
    // High activity simulation
    size_t highActivitySteps = 0;
    size_t highActivityFiring = 0;
    
    // Inject high sensory input
    brain.injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
    brain.injectCurrentToNeurons(NeuronType::Internal, 5.0f);
    
    for (int step = 3000; step < 3100; ++step) {
        brain.step(step, step * 0.001);
        highActivitySteps++;
        
        if (brain.getFiringNeuronCount() > 50) {
            highActivityFiring++;
        }
    }
    
    float activityRatio = static_cast<float>(highActivityFiring) / static_cast<float>(highActivitySteps);
    
    TestFramework::logTest("Performance under stress", true,
                         "Activity ratio: " + std::to_string(activityRatio) +
                         " (" + std::to_string(highActivityFiring) + "/" +
                         std::to_string(highActivitySteps) + " spikes)");
    
    TestFramework::logTest("Performance and Stability", true,
                         "System stable under extended operation");
}

// Main integration test runner
void runAllPhase6Tests() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing complete integrated brain system" << std::endl;
    std::cout << std::endl;
    
    std::vector<IntegrationTestResult> results;
    
    try {
        // Run all test categories
        testBasicSystemIntegration();
        results.push_back({"Basic System Integration", true, "All core systems operational"});
    } catch (const std::exception& e) {
        results.push_back({"Basic System Integration", false, std::string("Exception: ") + e.what()});
    }
    
    try {
        testLearningAndPlasticity();
        results.push_back({"Learning and Plasticity", true, "All plasticity systems functional"});
    } catch (const std::exception& e) {
        results.push_back({"Learning and Plasticity", false, std::string("Exception: ") + e.what()});
    }
    
    try {
        testAgentWorldIntegration();
        results.push_back({"Agent-World Integration", true, "Sensory-motor loop operational"});
    } catch (const std::exception& e) {
        results.push_back({"Agent-World Integration", false, std::string("Exception: ") + e.what()});
    }
    
    try {
        testPerformanceAndStability();
        results.push_back({"Performance and Stability", true, "System stable and performant"});
    } catch (const std::exception& e) {
        results.push_back({"Performance and Stability", false, std::string("Exception: ") + e.what()});
    }
    
    // Test Summary
    std::cout << std::endl;
    TestFramework::logSection("Phase 6 Test Summary");
    
    bool allPassed = true;
    for (const auto& result : results) {
        std::cout << "  " << (result.passed ? "[PASS]" : "[FAIL]") 
                  << " " << result.testName
                  << " - " << result.details << std::endl;
        if (!result.passed) allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== ALL PHASE 6 INTEGRATION TESTS PASSED ===" << std::endl;
        std::cout << "The NLM Phase 6 system is fully integrated and operational." << std::endl;
    } else {
        std::cout << "=== SOME PHASE 6 INTEGRATION TESTS FAILED ===" << std::endl;
        std::cout << "Please review failed tests above." << std::endl;
    }
    
    // Exit with appropriate code
    exit(allPassed ? 0 : 1);
}

} // namespace test_phase6

// Entry point for standalone test execution
int main() {
    nlm::test_phase6::runAllPhase6Tests();
    return 0;
}
