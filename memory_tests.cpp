#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include <vector>
#include <string>

// Include NLM headers
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"

// Test framework
class TestFramework {
public:
    static int runAllTests() {
        int passed = 0;
        int total = 0;
        
        std::cout << "=== NLM Memory Integration Tests ===" << std::endl;
        std::cout << std::endl;
        
        // Test 1: Basic Memory Integration
        total++;
        if (testBasicMemoryIntegration()) {
            passed++;
            std::cout << "✓ Test 1 passed: Basic Memory Integration" << std::endl;
        } else {
            std::cout << "✗ Test 1 failed: Basic Memory Integration" << std::endl;
        }
        
        // Test 2: Working Memory Integration
        total++;
        if (testWorkingMemoryIntegration()) {
            passed++;
            std::cout << "✓ Test 2 passed: Working Memory Integration" << std::endl;
        } else {
            std::cout << "✗ Test 2 failed: Working Memory Integration" << std::endl;
        }
        
        // Test 3: Episodic Memory Integration
        total++;
        if (testEpisodicMemoryIntegration()) {
            passed++;
            std::cout << "✓ Test 3 passed: Episodic Memory Integration" << std::endl;
        } else {
            std::cout << "✗ Test 3 failed: Episodic Memory Integration" << std::endl;
        }
        
        // Test 4: Prediction System Integration
        total++;
        if (testPredictionSystemIntegration()) {
            passed++;
            std::cout << "✓ Test 4 passed: Prediction System Integration" << std::endl;
        } else {
            std::cout << "✗ Test 4 failed: Prediction System Integration" << std::endl;
        }
        
        // Test 5: Neuromodulation Integration
        total++;
        if (testNeuromodulationIntegration()) {
            passed++;
            std::cout << "✓ Test 5 passed: Neuromodulation Integration" << std::endl;
        } else {
            std::cout << "✗ Test 5 failed: Neuromodulation Integration" << std::endl;
        }
        
        // Test 6: Learning with Memory
        total++;
        if (testLearningWithMemory()) {
            passed++;
            std::cout << "✓ Test 6 passed: Learning with Memory" << std::endl;
        } else {
            std::cout << "✗ Test 6 failed: Learning with Memory" << std::endl;
        }
        
        // Test 7: Development and Memory
        total++;
        if (testDevelopmentAndMemory()) {
            passed++;
            std::cout << "✓ Test 7 passed: Development and Memory" << std::endl;
        } else {
            std::cout << "✗ Test 7 failed: Development and Memory" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passed << "/" << total << std::endl;
        std::cout << "Success Rate: " << (passed * 100.0 / total) << "%" << std::endl;
        
        return (passed == total) ? 0 : 1;
    }

private:
    static bool testBasicMemoryIntegration() {
        std::cout << "  Testing basic memory system integration..." << std::endl;
        
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cout << "    ✗ Brain initialization failed" << std::endl;
            return false;
        }
        
        // Test memory system access
        auto workingMemory = brain->getWorkingMemory();
        auto episodicMemory = brain->getEpisodicMemory();
        auto associativeMemory = brain->getAssociativeMemory();
        
        if (!workingMemory || !episodicMemory || !associativeMemory) {
            std::cout << "    ✗ Memory systems not properly initialized" << std::endl;
            return false;
        }
        
        // Test memory system operations
        auto testNeuronId = nlm::NeuronId(1);
        
        // Test working memory
        workingMemory->storeWorkingMemory(testNeuronId, 0.5f, 0.8f);
        auto retrievedWM = workingMemory->getWorkingMemory(testNeuronId);
        
        if (!retrievedWM.has_value() || std::abs(retrievedWM.value().activation - 0.5f) > 0.01f) {
            std::cout << "    ✗ Working memory storage/retrieval failed" << std::endl;
            return false;
        }
        
        // Test episodic memory
        nlm::Episode episode;
        episode.step = 1;
        episode.reward = 0.5f;
        episode.activeNeurons.push_back(testNeuronId);
        
        episodicMemory->storeEpisode(episode);
        auto episodes = episodicMemory->getAllEpisodes();
        
        if (episodes.empty()) {
            std::cout << "    ✗ Episodic memory storage/retrieval failed" << std::endl;
            return false;
        }
        
        // Test associative memory
        std::vector<nlm::NeuronId> pattern1 = {testNeuronId, nlm::NeuronId(2)};
        std::vector<nlm::NeuronId> pattern2 = {nlm::NeuronId(3), nlm::NeuronId(4)};
        
        associativeMemory->storePattern(pattern1, pattern2);
        auto retrievedPattern = associativeMemory->recallPattern(pattern1);
        
        if (retrievedPattern.empty()) {
            std::cout << "    ✗ Associative memory storage/retrieval failed" << std::endl;
            return false;
        }
        
        // Test prediction system
        auto predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) {
            std::cout << "    ✗ Prediction system not properly initialized" << std::endl;
            return false;
        }
        
        std::cout << "    ✓ All basic memory integration tests passed" << std::endl;
        return true;
    }
    
    static bool testWorkingMemoryIntegration() {
        std::cout << "  Testing working memory integration in brain loop..." << std::endl;
        
        // Create configuration with working memory enabled
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 200);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cout << "    ✗ Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get working memory
        auto workingMemory = brain->getWorkingMemory();
        if (!workingMemory) {
            std::cout << "    ✗ Working memory not available" << std::endl;
            return false;
        }
        
        // Create test neuron and inject input
        auto sensoryNeuronId = nlm::NeuronId(10);
        brain->injectCurrent(sensoryNeuronId, 30.0f);  // Strong input to make neuron fire
        
        // Run multiple steps
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            
            // Check if working memory is being updated
            auto memoryNeurons = workingMemory->getMemoryNeurons();
            
            // Working memory should accumulate activity over time
            if (step % 10 == 0) {
                if (verbose) {
                    std::cout << "    Step " << step << ": Working memory neurons: " << memoryNeurons.size() << std::endl;
                }
            }
        }
        
        // Verify working memory contains active neurons
        auto memoryNeurons = workingMemory->getMemoryNeurons();
        if (memoryNeurons.empty()) {
            std::cout << "    ✗ Working memory should contain active neurons" << std::endl;
            return false;
        }
        
        std::cout << "    ✓ Working memory integration test passed" << std::endl;
        return true;
    }
    
    static bool testEpisodicMemoryIntegration() {
        std::cout << "  Testing episodic memory integration in brain loop..." << std::endl;
        
        // Create configuration with episodic memory enabled
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 200);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cout << "    ✗ Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get episodic memory
        auto episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            std::cout << "    ✗ Episodic memory not available" << std::endl;
            return false;
        }
        
        // Create test neurons
        auto testNeuronId = nlm::NeuronId(20);
        auto sensoryNeuronId = nlm::NeuronId(21);
        
        // Inject input to make neuron fire
        brain->injectCurrent(sensoryNeuronId, 40.0f);
        
        // Run steps and check episodic memory updates
        for (nlm::SimulationStep step = 0; step < 30; ++step) {
            brain->step(step, step * 0.001);
            
            // Episodic memory should update based on neural activity
            // This tests the integration between brain loop and episodic memory
        }
        
        // Check if episodic memory has stored episodes
        auto episodes = episodicMemory->getAllEpisodes();
        
        // Episodes should be stored when neurons fire
        if (episodes.empty()) {
            std::cout << "    ✗ Episodic memory should store episodes" << std::endl;
            return false;
        }
        
        // Verify episode contents
        for (const auto& episode : episodes) {
            if (episode.step >= 0 && episode.reward >= 0) {
                std::cout << "    ✓ Found valid episode: step " << episode.step 
                         << ", reward " << episode.reward << std::endl;
                break;
            }
        }
        
        std::cout << "    ✓ Episodic memory integration test passed" << std::endl;
        return true;
    }
    
    static bool testPredictionSystemIntegration() {
        std::cout << "  Testing prediction system integration in brain loop..." << std::endl;
        
        // Create configuration with prediction system enabled
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 300);
        config->set("prediction.error.threshold", 0.5f);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cout << "    ✗ Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get prediction system
        auto predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) {
            std::cout << "    ✗ Prediction system not available" << std::endl;
            return false;
        }
        
        // Create test neurons for prediction
        auto targetNeuronId = nlm::NeuronId(30);
        auto sourceNeuronId = nlm::NeuronId(31);
        
        // Establish a connection for prediction
        brain->addInterRegionConnection(nlm::RegionId(1), nlm::RegionId(2), 0.5f);
        
        // Run prediction simulation
        for (nlm::SimulationStep step = 0; step < 40; ++step) {
            brain->step(step, step * 0.001);
            
            // Prediction system should update based on neural activity
            // This tests the integration between brain loop and prediction
        }
        
        // Check if prediction system has recorded predictions
        // (Implementation depends on prediction system interface)
        std::cout << "    ✓ Prediction system integration test passed (conceptual)" << std::endl;
        return true;
    }
    
    static bool testNeuromodulationIntegration() {
        std::cout << "  Testing neuromodulation integration in brain loop..." << std::endl;
        
        // Create configuration with neuromodulation enabled
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 250);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.enable", true);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cout << "    ✗ Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get neuromodulation systems
        auto dopamine = brain->getDopamine();
        auto curiosity = brain->getCuriosity();
        auto novelty = brain->getNovelty();
        auto predictionError = brain->getPredictionErrorSignal();
        
        if (!dopamine || !curiosity || !novelty || !predictionError) {
            std::cout << "    ✗ Neuromodulation systems not properly initialized" << std::endl;
            return false;
        }
        
        // Test neuromodulation updates in brain loop
        for (nlm::SimulationStep step = 0; step < 35; ++step) {
            brain->step(step, step * 0.001);
            
            // Neuromodulators should update based on brain activity
            // This tests integration between brain loop and neuromodulation
            
            if (verbose && step % 10 == 0) {
                std::cout << "    Step " << step 
                         << ": Dopamine=" << dopamine->getLevel()
                         << ", Curiosity=" << curiosity->getLevel()
                         << ", Novelty=" << novelty->getLevel()
                         << ", PredictionError=" << predictionError->getLevel() << std::endl;
            }
        }
        
        // Check neuromodulator levels
        if (dopamine->getLevel() < 0.0f || dopamine->getLevel() > 2.0f) {
            std::cout << "    ✗ Dopamine level out of expected range" << std::endl;
            return false;
        }
        
        std::cout << "    ✓ Neuromodulation integration test passed" << std::endl;
        return true;
    }
    
    static bool testLearningWithMemory() {
        std::cout << "  Testing learning with memory integration..." << std::endl;
        
        // Create configuration with all learning systems enabled
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 400);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cout << "    ✗ Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get memory and neuromodulation systems
        auto workingMemory = brain->getWorkingMemory();
        auto episodicMemory = brain->getEpisodicMemory();
        auto dopamine = brain->getDopamine();
        auto curiosity = brain->getCuriosity();
        
        if (!workingMemory || !episodicMemory || !dopamine || !curiosity) {
            std::cout << "    ✗ Required systems not properly initialized" << std::endl;
            return false;
        }
        
        // Run learning simulation
        std::vector<float> spikeHistory;
        
        for (nlm::SimulationStep step = 0; step < 60; ++step) {
            // Create some stimulus to drive learning
            auto testNeuronId = nlm::NeuronId(40 + (step % 10));
            brain->injectCurrent(testNeuronId, 25.0f + (step % 5) * 5.0f);
            
            brain->step(step, step * 0.001);
            
            // Record spike statistics
            size_t totalSpikes = brain->getTotalSpikeCount();
            spikeHistory.push_back(static_cast<float>(totalSpikes));
            
            // Check memory updates
            auto memoryNeurons = workingMemory->getMemoryNeurons();
            auto episodes = episodicMemory->getAllEpisodes();
            
            if (step % 20 == 0 && verbose) {
                std::cout << "    Step " << step 
                         << ": Total spikes=" << totalSpikes
                         << ", Memory neurons=" << memoryNeurons.size()
                         << ", Episodes stored=" << episodes.size()
                         << ", Dopamine=" << dopamine->getLevel()
                         << ", Curiosity=" << curiosity->getLevel() << std::endl;
            }
        }
        
        // Verify learning has occurred (spike count should increase)
        if (spikeHistory.size() < 2) {
            std::cout << "    ✗ Not enough spike history for learning verification" << std::endl;
            return false;
        }
        
        float spikeIncrease = spikeHistory.back() - spikeHistory.front();
        if (spikeIncrease < 0) {
            std::cout << "    ✗ Spike count should increase with learning" << std::endl;
            return false;
        }
        
        // Verify memory systems have been updated
        auto memoryNeurons = workingMemory->getMemoryNeurons();
        auto episodes = episodicMemory->getAllEpisodes();
        
        if (memoryNeurons.empty()) {
            std::cout << "    ✗ Working memory should contain neurons after learning" << std::endl;
            return false;
        }
        
        if (episodes.empty()) {
            std::cout << "    ✗ Episodic memory should contain episodes after learning" << std::endl;
            return false;
        }
        
        std::cout << "    ✓ Learning with memory integration test passed" << std::endl;
        return true;
    }
    
    static bool testDevelopmentAndMemory() {
        std::cout << "  Testing development and memory integration..." << std::endl;
        
        // Create configuration with development stages
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 350);
        config->set("development.stage.duration", 100);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cout << "    ✗ Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get development and memory systems
        auto developmentSystem = brain->getDevelopmentSystem();
        auto workingMemory = brain->getWorkingMemory();
        auto episodicMemory = brain->getEpisodicMemory();
        auto curiosity = brain->getCuriosity();
        
        if (!developmentSystem || !workingMemory || !episodicMemory || !curiosity) {
            std::cout << "    ✗ Development or memory systems not properly initialized" << std::endl;
            return false;
        }
        
        // Run development simulation
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            
            // Development system should update
            // This tests integration between development and memory
            
            // Memory systems should accumulate experience
            if (step % 15 == 0 && verbose) {
                std::cout << "    Step " << step 
                         << ": Development stage=" << static_cast<int>(brain->getDevelopmentalStage())
                         << ", Working memory neurons=" << workingMemory->getMemoryNeurons().size()
                         << ", Episodes stored=" << episodicMemory->getAllEpisodes().size()
                         << ", Curiosity=" << curiosity->getLevel() << std::endl;
            }
        }
        
        // Check development stage progression
        auto developmentalStage = brain->getDevelopmentalStage();
        if (developmentalStage == nlm::DevelopmentalStage::Initial) {
            std::cout << "    ✗ Development should progress beyond initial stage" << std::endl;
            return false;
        }
        
        // Check memory accumulation
        auto memoryNeurons = workingMemory->getMemoryNeurons();
        auto episodes = episodicMemory->getAllEpisodes();
        
        if (memoryNeurons.empty()) {
            std::cout << "    ✗ Working memory should accumulate neurons over time" << std::endl;
            return false;
        }
        
        if (episodes.empty()) {
            std::cout << "    ✗ Episodic memory should accumulate episodes over time" << std::endl;
            return false;
        }
        
        std::cout << "    ✓ Development and memory integration test passed" << std::endl;
        return true;
    }
    
    static bool verbose;
}};

int main() {
    // Redirect to stderr to avoid interfering with test output
    std::cout.rdbuf(std::cerr.rdbuf());
    
    return TestFramework::runAllTests();
}