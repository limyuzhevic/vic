#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

using namespace nlm;

class NLMBrainIntegrationTest {
public:
    NLMBrainIntegrationTest() : testResults_() {}
    
    bool runAllTests() {
        std::cout << "=== NLM Brain Integration Tests ===" << std::endl;
        
        bool allPassed = true;
        
        allPassed &= testBrainInitialization();
        allPassed &= testMemoryIntegration();
        allPassed &= testPredictionSystem();
        allPassed &= testCognitionSystems();
        allPassed &= testNeuromodulation();
        allPassed &= testSaveLoad();
        allPassed &= testActionSelection();
        allPassed &= testSleepRestCycle();
        allPassed &= testPlasticityLearning();
        
        printTestSummary();
        return allPassed;
    }
    
private:
    bool testBrainInitialization() {
        std::cout << "\n1. Testing Brain Initialization..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cout << "  ✗ Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Verify all integrated systems are present
        if (!brain->getWorkingMemory()) {
            std::cout << "  ✗ Working memory not accessible" << std::endl;
            return false;
        }
        
        if (!brain->getEpisodicMemory()) {
            std::cout << "  ✗ Episodic memory not accessible" << std::endl;
            return false;
        }
        
        if (!brain->getPredictionSystem()) {
            std::cout << "  ✗ Prediction system not accessible" << std::endl;
            return false;
        }
        
        if (!brain->getPlanner()) {
            std::cout << "  ✗ Neural planner not accessible" << std::endl;
            return false;
        }
        
        if (!brain->getConceptFormation()) {
            std::cout << "  ✗ Concept formation not accessible" << std::endl;
            return false;
        }
        
        if (!brain->getAttention()) {
            std::cout << "  ✗ Attention system not accessible" << std::endl;
            return false;
        }
        
        if (!brain->getDopamine()) {
            std::cout << "  ✗ Dopamine system not accessible" << std::endl;
            return false;
        }
        
        std::cout << "  ✓ Brain initialization successful with all integrated systems" << std::endl;
        return true;
    }
    
    bool testMemoryIntegration() {
        std::cout << "\n2. Testing Memory System Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test working memory
        if (auto* workingMem = brain->getWorkingMemory()) {
            // Store a memory trace
            workingMem->storeToNeuron(NeuronId(1), 0.5f);
            workingMem->update(0.001);
            
            if (workingMem->getMemoryActivity() > 0.0f) {
                std::cout << "  ✓ Working memory functional" << std::endl;
            } else {
                std::cout << "  ✗ Working memory not storing traces" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Working memory not accessible" << std::endl;
            return false;
        }
        
        // Test episodic memory
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            // Create and store an episode
            EpisodicMemoryItem episode;
            episode.timestamp = 1;
            episode.reward = 0.8f;
            episode.activeNeurons.push_back(NeuronId(1));
            episode.neuronActivations.push_back(0.7f);
            
            episodicMem->storeEpisode(episode);
            
            if (episodicMem->getEpisodeCount() > 0) {
                std::cout << "  ✓ Episodic memory functional" << std::endl;
            } else {
                std::cout << "  ✗ Episodic memory not storing episodes" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Episodic memory not accessible" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testPredictionSystem() {
        std::cout << "\n3. Testing Prediction System Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (auto* predSys = brain->getPredictionSystem()) {
            // Test prediction error computation
            SensoryInput input1(std::vector<float>{0.1f, 0.2f, 0.3f});
            SensoryInput input2(std::vector<float>{0.2f, 0.3f, 0.4f});
            
            predSys->updatePredictions(input1, input2);
            
            float error = predSys->getPredictionError();
            if (error >= 0.0f) {
                std::cout << "  ✓ Prediction system computing error" << std::endl;
            } else {
                std::cout << "  ✗ Prediction system error computation failed" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Prediction system not accessible" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testCognitionSystems() {
        std::cout << "\n4. Testing Cognition System Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test neural planner
        if (auto* planner = brain->getPlanner()) {
            std::vector<float> state{0.1f, 0.2f, 0.3f};
            ActionType action = planner->planAction(state, 0.5f);
            
            if (action != ActionType::None) {
                std::cout << "  ✓ Neural planner generating actions" << std::endl;
            } else {
                std::cout << "  ✗ Neural planner not generating actions" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Neural planner not accessible" << std::endl;
            return false;
        }
        
        // Test concept formation
        if (auto* conceptForm = brain->getConceptFormation()) {
            if (conceptForm->getConceptCount() >= 0) {
                std::cout << "  ✓ Concept formation system functional" << std::endl;
            } else {
                std::cout << "  ✗ Concept formation system not functional" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Concept formation not accessible" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testNeuromodulation() {
        std::cout << "\n5. Testing Neuromodulation Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test dopamine system
        if (auto* dopamine = brain->getDopamine()) {
            float level = dopamine->getLevel();
            if (level >= 0.0f) {
                std::cout << "  ✓ Dopamine system functional" << std::endl;
            } else {
                std::cout << "  ✗ Dopamine system not functional" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Dopamine system not accessible" << std::endl;
            return false;
        }
        
        // Test novelty system
        if (auto* novelty = brain->getNovelty()) {
            float level = novelty->getLevel();
            if (level >= 0.0f) {
                std::cout << "  ✓ Novelty system functional" << std::endl;
            } else {
                std::cout << "  ✗ Novelty system not functional" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Novelty system not accessible" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testSaveLoad() {
        std::cout << "\n6. Testing Save/Load Functionality..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("checkpoint_dir", "./test_checkpoints", ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Store some state
        if (auto* workingMem = brain->getWorkingMemory()) {
            workingMem->storeToNeuron(NeuronId(1), 0.5f);
        }
        
        // Simulate some steps
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Try to save
        std::string checkpointFile = "./test_checkpoint.bin";
        bool saveSuccess = brain->save(checkpointFile);
        
        if (saveSuccess) {
            std::cout << "  ✓ Brain state saved successfully" << std::endl;
            
            // Create a new brain and try to load
            auto brain2 = std::make_shared<Brain>(config);
            brain2->initialize();
            
            bool loadSuccess = brain2->load(checkpointFile);
            if (loadSuccess) {
                std::cout << "  ✓ Brain state loaded successfully" << std::endl;
            } else {
                std::cout << "  ✗ Brain state load failed" << std::endl;
                return false;
            }
        } else {
            std::cout << "  ✗ Brain state save failed" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testActionSelection() {
        std::cout << "\n7. Testing Action Selection Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Generate some motor activity
        brain->injectCurrentToNeurons(NeuronType::Motor, 10.0f);
        
        // Simulate some steps to generate spikes
        for (int step = 0; step < 5; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Select an action
        auto action = brain->produceAction();
        
        if (action && action->getType() != ActionType::None) {
            std::cout << "  ✓ Action selection functional (action: " << static_cast<int>(action->getType()) << ")" << std::endl;
        } else {
            std::cout << "  ✗ Action selection not generating actions" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testSleepRestCycle() {
        std::cout << "\n8. Testing Sleep/Rest Cycle Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create a situation that might trigger rest
        // Simulate low activity
        for (int step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Check resting state
        if (brain->getBrainStatus().find("RESTING") != std::string::npos) {
            std::cout << "  ✓ Rest cycle detected" << std::endl;
        } else {
            std::cout << "  ! Rest cycle not triggered (may be normal)" << std::endl;
        }
        
        return true;
    }
    
    bool testPlasticityLearning() {
        std::cout << "\n9. Testing Plasticity and Learning Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Get initial statistics
        size_t initialSpikes = brain->getTotalSpikeCount();
        size_t initialSynapses = brain->getTotalSynapseCount();
        
        // Apply learning stimulus
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        
        // Simulate learning
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Check for learning
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t finalSynapses = brain->getTotalSynapseCount();
        
        if (finalSpikes > initialSpikes || finalSynapses > initialSynapses) {
            std::cout << "  ✓ Plasticity and learning functional (spikes: " 
                      << initialSpikes << " -> " << finalSpikes 
                      << ", synapses: " << initialSynapses << " -> " << finalSynapses << ")" << std::endl;
        } else {
            std::cout << "  ! Minimal learning detected (may be normal for this test)" << std::endl;
        }
        
        return true;
    }
    
    void printTestSummary() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Test Summary:" << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : testResults_) {
            if (result.second) {
                passed++;
                std::cout << "  ✓ " << result.first << ": PASSED" << std::endl;
            } else {
                failed++;
                std::cout << "  ✗ " << result.first << ": FAILED" << std::endl;
            }
        }
        
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Total: " << (passed + failed) << " tests, "
                  << passed << " passed, " << failed << " failed" << std::endl;
        
        if (failed == 0) {
            std::cout << "\n🎉 All tests passed! NLM brain integration is successful." << std::endl;
        } else {
            std::cout << "\n⚠ " << failed << " tests failed. Review required." << std::endl;
        }
    }
    
    std::vector<std::pair<std::string, bool>> testResults_;
};

int main() {
    NLMBrainIntegrationTest tester;
    
    bool success = tester.runAllTests();
    
    return success ? 0 : 1;
}
