// test_persistence_integration.cpp - Persistence integration tests for Phase 6
// Author: Kilo
// Purpose: Test save/load and checkpoint system with all integrated states

#include "brain_test_common.hpp"

namespace nlm {
namespace test {

/**
 * Test save/load properly restores all integrated states
 */
bool testSaveLoadRestoresAllStates() {
    std::cout << "=== Test: Save/Load Restores All States ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 800;
    config.neuronCount = 500;
    config.enableCheckpointing = true;
    config.checkpointPath = "/tmp/nlm_save_load_test.bin";
    
    Phase6TestFixture fixture(config);
    auto brain1 = fixture.createBrain();
    
    if (!brain1) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    // Get initial state metrics
    size_t initialNeurons = brain1->getTotalNeuronCount();
    size_t initialSpikes = brain1->getTotalSpikeCount();
    size_t initialFiring = brain1->getFiringNeuronCount();
    
    float initialFiringRate = brain1->getAverageFiringRate();
    
    // Get all integrated system states
    bool initialWorkingMemory = (brain1->getWorkingMemory() != nullptr);
    bool initialEpisodicMemory = (brain1->getEpisodicMemory() != nullptr);
    bool initialNeuromodulation = (brain1->getDopamine() != nullptr);
    bool initialPrediction = (brain1->getPredictionSystem() != nullptr);
    bool initialDevelopment = (brain1->getDevelopmentSystem() != nullptr);
    
    // Run simulation to generate varied state
    std::cout << "Running simulation to generate state..." << std::endl;
    
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Inject varied sensory input
        float input = sin(step * 0.1f) * 3.0f + cos(step * 0.05f) * 2.0f;
        brain1->injectCurrentToNeurons(NeuronType::Sensory, input);
        
        brain1->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << " completed" << std::endl;
        }
    }
    
    // Get final state metrics
    size_t finalNeurons = brain1->getTotalNeuronCount();
    size_t finalSpikes = brain1->getTotalSpikeCount();
    size_t finalFiring = brain1->getFiringNeuronCount();
    
    float finalFiringRate = brain1->getAverageFiringRate();
    
    // Get all integrated system states after simulation
    bool finalWorkingMemory = (brain1->getWorkingMemory() != nullptr);
    size_t finalWorkingMemoryTraces = (finalWorkingMemory ? brain1->getWorkingMemory()->getActiveTraces() : 0);
    
    bool finalEpisodicMemory = (brain1->getEpisodicMemory() != nullptr);
    size_t finalEpisodicEpisodes = (finalEpisodicMemory ? brain1->getEpisodicMemory()->getEpisodeCount() : 0);
    
    bool finalNeuromodulation = (brain1->getDopamine() != nullptr);
    float finalDopamineLevel = (finalNeuromodulation ? brain1->getDopamine()->getLevel() : 0.0f);
    
    bool finalPrediction = (brain1->getPredictionSystem() != nullptr);
    float finalPredictionError = (finalPrediction ? brain1->getPredictionSystem()->getPredictionError() : 0.0f);
    
    bool finalDevelopment = (brain1->getDevelopmentSystem() != nullptr);
    
    std::cout << "Initial state metrics:" << std::endl;
    std::cout << "  Neurons: " << initialNeurons << std::endl;
    std::cout << "  Total spikes: " << initialSpikes << std::endl;
    std::cout << "  Current firing: " << initialFiring << std::endl;
    std::cout << "  Working memory: " << (initialWorkingMemory ? "YES" : "NO") << std::endl;
    std::cout << "  Episodic memory: " << (initialEpisodicMemory ? "YES" : "NO") << std::endl;
    std::cout << "  Neuromodulation: " << (initialNeuromodulation ? "YES" : "NO") << std::endl;
    std::cout << "  Prediction: " << (initialPrediction ? "YES" : "NO") << std::endl;
    std::cout << "  Development: " << (initialDevelopment ? "YES" : "NO") << std::endl;
    
    std::cout << "Final state metrics:" << std::endl;
    std::cout << "  Neurons: " << finalNeurons << std::endl;
    std::cout << "  Total spikes: " << finalSpikes << std::endl;
    std::cout << "  Current firing: " << finalFiring << std::endl;
    std::cout << "  Working memory traces: " << finalWorkingMemoryTraces << std::endl;
    std::cout << "  Episodic memory episodes: " << finalEpisodicEpisodes << std::endl;
    std::cout << "  Dopamine level: " << finalDopamineLevel << std::endl;
    std::cout << "  Prediction error: " << finalPredictionError << std::endl;
    
    // Test 1: Save the brain state
    std::cout << "Test 1: Save brain state" << std::endl;
    
    bool saveSuccess = brain1->save(config.checkpointPath);
    std::cout << "Save successful: " << (saveSuccess ? "YES" : "NO") << std::endl;
    
    if (!saveSuccess) {
        std::cout << "FAIL: Could not save brain state" << std::endl;
        return false;
    }
    
    // Test 2: Create new brain and load state
    std::cout << "Test 2: Create new brain and load state" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    cfg->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    auto brain2 = std::make_shared<Brain>(cfg);
    bool brain2Initialized = brain2->initialize();
    std::cout << "Brain2 initialization: " << (brain2Initialized ? "SUCCESS" : "FAILED") << std::endl;
    
    if (!brain2Initialized) {
        std::cout << "FAIL: Could not initialize second brain" << std::endl;
        return false;
    }
    
    bool loadSuccess = brain2->load(config.checkpointPath);
    std::cout << "Load successful: " << (loadSuccess ? "YES" : "NO") << std::endl;
    
    if (!loadSuccess) {
        std::cout << "FAIL: Could not load brain state" << std::endl;
        return false;
    }
    
    // Test 3: Compare loaded state with original
    std::cout << "Test 3: Compare loaded state with original" << std::endl;
    
    bool neuronCountMatch = (brain2->getTotalNeuronCount() == finalNeurons);
    bool totalSpikesMatch = (brain2->getTotalSpikeCount() == finalSpikes);
    bool workingMemoryMatch = ((brain2->getWorkingMemory() != nullptr) == finalWorkingMemory);
    bool episodicMemoryMatch = ((brain2->getEpisodicMemory() != nullptr) == finalEpisodicMemory);
    
    std::cout << "Neuron count matches: " << (neuronCountMatch ? "YES" : "NO") << std::endl;
    std::cout << "Total spikes matches: " << (totalSpikesMatch ? "YES" : "NO") << std::endl;
    std::cout << "Working memory availability matches: " << (workingMemoryMatch ? "YES" : "NO") << std::endl;
    std::cout << "Episodic memory availability matches: " << (episodicMemoryMatch ? "YES" : "NO") << std::endl;
    
    // Test 4: Verify all integrated systems are restored
    std::cout << "Test 4: Verify all integrated systems are restored" << std::endl;
    
    bool integratedMemoryWorking = (brain2->getWorkingMemory() != nullptr);
    bool integratedEpisodicMemoryWorking = (brain2->getEpisodicMemory() != nullptr);
    bool integratedNeuromodulationWorking = (brain2->getDopamine() != nullptr);
    bool integratedPredictionWorking = (brain2->getPredictionSystem() != nullptr);
    bool integratedDevelopmentWorking = (brain2->getDevelopmentSystem() != nullptr);
    
    std::cout << "Working memory working: " << (integratedMemoryWorking ? "YES" : "NO") << std::endl;
    std::cout << "Episodic memory working: " << (integratedEpisodicMemoryWorking ? "YES" : "NO") << std::endl;
    std::cout << "Neuromodulation working: " << (integratedNeuromodulationWorking ? "YES" : "NO") << std::endl;
    std::cout << "Prediction working: " << (integratedPredictionWorking ? "YES" : "NO") << std::endl;
    std::cout << "Development working: " << (integratedDevelopmentWorking ? "YES" : "NO") << std::endl;
    
    // Overall test result
    bool overallSuccess = saveSuccess && loadSuccess && neuronCountMatch && 
                         totalSpikesMatch && workingMemoryMatch && episodicMemoryMatch;
    
    std::cout << "=== Save/Load Restores All States Test Result: " 
              << (overallSuccess ? "PASS" : "FAIL") << " ===" << std::endl;
    
    return overallSuccess;
}

/**
 * Test checkpoint system works with new neuromodulators
 */
bool testCheckpointNeuromodulators() {
    std::cout << "=== Test: Checkpoint System with Neuromodulators ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 600;
    config.neuronCount = 400;
    config.enableCheckpointing = true;
    config.checkpointPath = "/tmp/nlm_neuromod_checkpoint_test.bin";
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    // Get all neuromodulators
    auto* dopamine = brain->getDopamine();
    auto* acetylcholine = brain->getAcetylcholine();
    auto* norepinephrine = brain->getNorepinephrine();
    auto* serotonin = brain->getSerotonin();
    auto* adenosine = brain->getAdenosine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    // Test 1: Check which neuromodulators are available
    std::cout << "Test 1: Neuromodulator availability" << std::endl;
    
    std::vector<std::pair<std::string, bool>> neuromodulators;
    neuromodulators.push_back(std::make_pair("Dopamine", dopamine != nullptr));
    neuromodulators.push_back(std::make_pair("Acetylcholine", acetylcholine != nullptr));
    neuromodulators.push_back(std::make_pair("Norepinephrine", norepinephrine != nullptr));
    neuromodulators.push_back(std::make_pair("Serotonin", serotonin != nullptr));
    neuromodulators.push_back(std::make_pair("Adenosine", adenosine != nullptr));
    neuromodulators.push_back(std::make_pair("Curiosity", curiosity != nullptr));
    neuromodulators.push_back(std::make_pair("Novelty", novelty != nullptr));
    neuromodulators.push_back(std::make_pair("Prediction Error", predictionError != nullptr));
    
    for (const auto& mod : neuromodulators) {
        std::cout << "  " << mod.first << ": " << (mod.second ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
    }
    
    // Test 2: Set neuromodulator levels and save
    std::cout << "Test 2: Set neuromodulator levels and save" << std::endl;
    
    if (dopamine) {
        dopamine->setLevel(0.7f);
        std::cout << "  Set dopamine level: 0.7" << std::endl;
    }
    
    if (acetylcholine) {
        acetylcholine->setLevel(0.8f);
        std::cout << "  Set acetylcholine level: 0.8" << std::endl;
    }
    
    if (adenosine) {
        adenosine->setLevel(0.3f);
        std::cout << "  Set adenosine level: 0.3" << std::endl;
    }
    
    if (curiosity) {
        curiosity->setLevel(0.6f);
        std::cout << "  Set curiosity level: 0.6" << std::endl;
    }
    
    // Save brain with neuromodulator levels
    bool saveSuccess = brain->save(config.checkpointPath);
    std::cout << "Save with neuromodulators: " << (saveSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    if (!saveSuccess) {
        std::cout << "FAIL: Could not save with neuromodulators" << std::endl;
        return false;
    }
    
    // Test 3: Load and check neuromodulator levels
    std::cout << "Test 3: Load and check neuromodulator levels" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain2 = std::make_shared<Brain>(cfg);
    bool brain2Initialized = brain2->initialize();
    std::cout << "Brain2 initialization: " << (brain2Initialized ? "SUCCESS" : "FAILED") << std::endl;
    
    if (!brain2Initialized) {
        std::cout << "FAIL: Could not initialize second brain" << std::endl;
        return false;
    }
    
    bool loadSuccess = brain2->load(config.checkpointPath);
    std::cout << "Load with neuromodulators: " << (loadSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    if (!loadSuccess) {
        std::cout << "FAIL: Could not load with neuromodulators" << std::endl;
        return false;
    }
    
    // Check neuromodulator levels after load
    if (dopamine) {
        float loadedDopamine = brain2->getDopamine() ? brain2->getDopamine()->getLevel() : 0.0f;
        std::cout << "  Loaded dopamine level: " << loadedDopamine << std::endl;
    }
    
    if (acetylcholine) {
        float loadedACh = brain2->getAcetylcholine() ? brain2->getAcetylcholine()->getLevel() : 0.0f;
        std::cout << "  Loaded acetylcholine level: " << loadedACh << std::endl;
    }
    
    if (adenosine) {
        float loadedAdenosine = brain2->getAdenosine() ? brain2->getAdenosine()->getLevel() : 0.0f;
        std::cout << "  Loaded adenosine level: " << loadedAdenosine << std::endl;
    }
    
    if (curiosity) {
        float loadedCuriosity = brain2->getCuriosity() ? brain2->getCuriosity()->getLevel() : 0.0f;
        std::cout << "  Loaded curiosity level: " << loadedCuriosity << std::endl;
    }
    
    // Test 4: Checkpoint system with all integrated systems
    std::cout << "Test 4: Checkpoint system integration" << std::endl;
    
    // Run simulation with all systems
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Inject input to activate neuromodulators
        float input = sin(step * 0.05f) * 2.0f + cos(step * 0.025f) * 1.5f;
        brain2->injectCurrentToNeurons(NeuronType::Sensory, input);
        
        brain2->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << " completed" << std::endl;
        }
    }
    
    // Save again with updated neuromodulator levels
    bool save2Success = brain2->save(config.checkpointPath);
    std::cout << "Second save: " << (save2Success ? "SUCCESS" : "FAILED") << std::endl;
    
    // Overall test result
    bool overallSuccess = saveSuccess && loadSuccess && save2Success;
    
    std::cout << "=== Checkpoint System with Neuromodulators Test Result: " 
              << (overallSuccess ? "PASS" : "FAIL") << " ===" << std::endl;
    
    return overallSuccess;
}

/**
 * Run all persistence integration tests
 */
void runAllPersistenceIntegrationTests() {
    TestStats stats;
    PerformanceTimer timer;
    
    // Test save/load with all integrated states
    timer.reset();
    bool test1Passed = testSaveLoadRestoresAllStates();
    stats.addTest("Save/Load Restores All States", test1Passed, timer.elapsed());
    
    // Test checkpoint system with neuromodulators
    timer.reset();
    bool test2Passed = testCheckpointNeuromodulators();
    stats.addTest("Checkpoint System with Neuromodulators", test2Passed, timer.elapsed());
    
    // Report results
    stats.report();
}

} // namespace test
} // namespace nlm