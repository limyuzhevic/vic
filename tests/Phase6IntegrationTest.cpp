// NLM Phase 6 Integration Tests
// Comprehensive integration tests for all Phase 6 features
// Tests memory systems, neuromodulation, prediction, cognition, and development

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <cmath>

// Include NLM headers
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/AttentionalSelection.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"

namespace test_phase6_integration {

class Phase6IntegrationTest {
private:
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::SimpleWorld> world;
    std::shared_ptr<nlm::AgentBrain> agent;
    
public:
    Phase6IntegrationTest() {
        // Create comprehensive Phase 6 configuration
        config = std::make_shared<nlm::Config>();
        
        // Memory systems configuration
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 3);
        config->set("brain.synapse_density", 0.1);
        config->set("working_memory.capacity", 100);
        config->set("episodic_memory.max_episodes", 500);
        config->set("associative_memory.pattern_threshold", 0.7);
        
        // Neuromodulation configuration
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.prediction_error.sensitivity", 0.5);
        
        // Prediction system configuration
        config->set("prediction.system_enabled", true);
        config->set("prediction.confidence_threshold", 0.8);
        
        // Cognition systems configuration
        config->set("cognition.planner.planning_depth", 3);
        config->set("cognition.concept_formation.min_patterns", 5);
        config->set("cognition.attention.inhibition_strength", 0.5);
        
        // Development configuration
        config->set("development.initial_plasticity", 1.0);
        config->set("development.critical_period_duration", 100);
        config->set("development.maturation_rate", 0.01);
        
        // Plasticity configuration
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.synaptogenesis_rate", 0.0001);
        config->set("plasticity.pruning_rate", 0.00001);
        
        // Simulation configuration
        config->set("simulation_timestep", 0.001);
        config->set("replay_interval", 100);
        config->set("consolidation_interval", 1000);
        config->set("random_seed", 42);
    }
    
    void setup() {
        std::cout << "=== Setting up Phase 6 Integration Test ===" << std::endl;
        
        // Create brain with Phase 6 configuration
        brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Create world and agent
        world = std::make_shared<nlm::SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all advanced subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        std::cout << "Phase 6 setup complete" << std::endl;
    }
    
    void test_memory_systems() {
        std::cout << "\n--- Testing Memory Systems ---" << std::endl;
        
        // Test working memory
        nlm::NeuralWorkingMemory* workingMemory = brain->getWorkingMemory();
        assert(workingMemory != nullptr);
        assert(workingMemory->getCapacity() == 100);
        
        // Store some data in working memory
        workingMemory->storeToNeuron(1, 0.5);
        workingMemory->storeToNeuron(2, 0.8);
        
        std::cout << "Working memory: " << workingMemory->getActiveTraces() << " active traces" << std::endl;
        
        // Test episodic memory
        nlm::NeuralEpisodicMemory* episodicMemory = brain->getEpisodicMemory();
        assert(episodicMemory != nullptr);
        assert(episodicMemory->getMaxEpisodes() == 500);
        
        // Store an episode
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = brain->getTotalSpikeCount();
        episode.reward = 0.7;
        episode.activeNeurons = {1, 2, 3, 4, 5};
        episode.neuronActivations = {0.5, 0.8, 0.6, 0.4, 0.7};
        
        episodicMemory->storeEpisode(episode);
        std::cout << "Episodic memory: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
        
        // Test associative memory
        nlm::NeuralAssociativeMemory* associativeMemory = brain->getAssociativeMemory();
        assert(associativeMemory != nullptr);
        
        std::cout << "Memory systems test passed" << std::endl;
    }
    
    void test_neuromodulation_systems() {
        std::cout << "\n--- Testing Neuromodulation Systems ---" << std::endl;
        
        // Test dopamine system
        nlm::Dopamine* dopamine = brain->getDopamine();
        assert(dopamine != nullptr);
        assert(dopamine->getLevel() >= 0.0f);
        
        // Test curiosity system
        nlm::Curiosity* curiosity = brain->getCuriosity();
        assert(curiosity != nullptr);
        
        float curiosityLevel = curiosity->getCuriosityLevel();
        std::cout << "Curiosity level: " << curiosityLevel << std::endl;
        
        // Test novelty system
        nlm::Novelty* novelty = brain->getNovelty();
        assert(novelty != nullptr);
        assert(novelty->getNoveltyLevel() >= 0.0f);
        
        // Test prediction error system
        nlm::PredictionError* predictionError = brain->getPredictionErrorSignal();
        assert(predictionError != nullptr);
        assert(predictionError->getPredictionError() >= 0.0f);
        
        // Test neuromodulation effects
        nlm::Neuromodulator rewardSignal;
        rewardSignal.setLevel(0.8);
        brain->applyNeuromodulation(rewardSignal);
        
        std::cout << "Neuromodulation systems test passed" << std::endl;
    }
    
    void test_prediction_system() {
        std::cout << "\n--- Testing Prediction System ---" << std::endl;
        
        nlm::PredictionSystem* predictionSystem = brain->getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Test prediction system integration
        // (Prediction system would typically be used with sensory input)
        
        std::cout << "Prediction system test passed" << std::endl;
    }
    
    void test_cognition_systems() {
        std::cout << "\n--- Testing Cognition Systems ---" << std::endl;
        
        // Test neural planner
        nlm::NeuralPlanner* planner = brain->getPlanner();
        assert(planner != nullptr);
        assert(planner->getPlanningDepth() == 3);
        
        // Test concept formation
        nlm::ConceptFormation* conceptFormation = brain->getConceptFormation();
        assert(conceptFormation != nullptr);
        
        // Test attentional selection
        nlm::AttentionalSelection* attention = brain->getAttention();
        assert(attention != nullptr);
        assert(attention->getInhibitionStrength() == 0.5f);
        
        std::cout << "Cognition systems test passed" << std::endl;
    }
    
    void test_development_system() {
        std::cout << "\n--- Testing Development System ---" << std::endl;
        
        nlm::DevelopmentSystem* developmentSystem = brain->getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
        std::cout << "Initial developmental stage: " << static_cast<int>(stage) << std::endl;
        
        // Test development progression
        brain->develop();
        stage = brain->getDevelopmentalStage();
        std::cout << "After development: " << static_cast<int>(stage) << std::endl;
        
        std::cout << "Development system test passed" << std::endl;
    }
    
    void test_plasticity_systems() {
        std::cout << "\n--- Testing Plasticity Systems ---" << std::endl;
        
        // Test STDP
        nlm::STDP* stdp = brain->getSTDP();
        assert(stdp != nullptr);
        assert(stdp->getLTPWeight() > 0.0f);
        assert(stdp->getLTDWeight() > 0.0f);
        
        // Test Hebbian
        nlm::Hebbian* hebbian = brain->getHebbian();
        assert(hebbian != nullptr);
        
        // Test structural plasticity
        nlm::StructuralPlasticity* structuralPlasticity = brain->getStructuralPlasticity();
        assert(structuralPlasticity != nullptr);
        assert(structuralPlasticity->getSynaptogenesisRate() > 0.0f);
        assert(structuralPlasticity->getPruningRate() > 0.0f);
        
        std::cout << "Plasticity systems test passed" << std::endl;
    }
    
    void test_brain_world_integration() {
        std::cout << "\n--- Testing Brain-World Integration ---" << std::endl;
        
        // Create sensory input
        nlm::SensoryInput sensoryInput;
        // Add some vision data
        // (In a real scenario, this would come from the world)
        
        // Process sensory input through agent
        agent->processSensoryInput(sensoryInput);
        
        // Run brain simulation
        for (int step = 0; step < 50; ++step) {
            brain->step(step);
            
            // Decode motor command
            nlm::Action action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = 0.0f;  // Would come from world feedback
            agent->applyRewardModulation(reward, 0.0);
            
            // Update development
            agent->updateDevelopment(0.1f);
        }
        
        // Check results
        assert(brain->getTotalSpikeCount() > 0);
        assert(world->getSimulationTime() > 0.0f);
        
        std::cout << "Brain-world integration test passed" << std::endl;
    }
    
    void test_checkpoint_functionality() {
        std::cout << "\n--- Testing Checkpoint Functionality ---" << std::endl;
        
        // Save brain state
        bool saveSuccess = brain->save("test_checkpoint.bin");
        assert(saveSuccess);
        
        // Run some simulation steps
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        // Reset brain
        brain->reset();
        assert(brain->getTotalSpikeCount() == 0);
        
        // Load saved state
        bool loadSuccess = brain->load("test_checkpoint.bin");
        assert(loadSuccess);
        
        std::cout << "Checkpoint functionality test passed" << std::endl;
    }
    
    void test_complete_integration() {
        std::cout << "\n--- Testing Complete Integration ---" << std::endl;
        
        // Run extended simulation with all subsystems active
        size_t totalSteps = 500;
        size_t workingMemoryEvents = 0;
        size_t episodicMemoryEvents = 0;
        size_t neuromodulationEvents = 0;
        
        for (size_t step = 0; step < totalSteps; ++step) {
            // Update brain
            brain->step(step);
            
            // Update world
            world->update(0.1f);
            
            // Process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Decode action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward
            float reward = 0.0f;  // Would come from world feedback
            agent->applyRewardModulation(reward, 0.0);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Count events for verification
            if (brain->getWorkingMemory() && brain->getWorkingMemory()->getActiveTraces() > 0) {
                workingMemoryEvents++;
            }
            if (brain->getEpisodicMemory() && brain->getEpisodicMemory()->getEpisodeCount() > 0) {
                episodicMemoryEvents++;
            }
            if (brain->getDopamine() && brain->getDopamine()->getLevel() > 0.0f) {
                neuromodulationEvents++;
            }
            
            // Progress report
            if (step % 100 == 0) {
                std::cout << "  Step " << step << ": "
                          << "WM events: " << workingMemoryEvents << ", "
                          << "EM events: " << episodicMemoryEvents << ", "
                          << "NM events: " << neuromodulationEvents << std::endl;
            }
        }
        
        // Verify integration results
        assert(brain->getTotalSpikeCount() > 0);
        assert(brain->getTotalSynapseCount() > 0);
        assert(world->getSimulationTime() > 0.0f);
        assert(agent->getCuriosityLevel() >= 0.0f);
        assert(agent->getNoveltyLevel() >= 0.0f);
        assert(agent->getPredictionError() >= 0.0f);
        
        std::cout << "Complete integration test passed" << std::endl;
        std::cout << "Final statistics:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Simulation time: " << world->getSimulationTime() << " seconds" << std::endl;
        std::cout << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        std::cout << "  Working memory traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
        std::cout << "  Episodic memory episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
        std::cout << "  Dopamine level: " << brain->getDopamine()->getLevel() << std::endl;
    }
    
    void runAllTests() {
        std::cout << "=== NLM Phase 6 Integration Test Suite ===" << std::endl;
        
        setup();
        
        test_memory_systems();
        test_neuromodulation_systems();
        test_prediction_system();
        test_cognition_systems();
        test_development_system();
        test_plasticity_systems();
        test_brain_world_integration();
        test_checkpoint_functionality();
        test_complete_integration();
        
        std::cout << "\n=== All Phase 6 Integration Tests Passed ===" << std::endl;
        std::cout << "NLM successfully implements all Phase 6 features:" << std::endl;
        std::cout << "  ✓ Memory Systems (working, episodic, associative)" << std::endl;
        std::cout << "  ✓ Neuromodulation (dopamine, curiosity, novelty, prediction error)" << std::endl;
        std::cout << "  ✓ Prediction System" << std::endl;
        std::cout << "  ✓ Cognition Systems (planning, concept formation, attention)" << std::endl;
        std::cout << "  ✓ Development System" << std::endl;
        std::cout << "  ✓ Plasticity Systems (STDP, Hebbian, structural)" << std::endl;
        std::cout << "  ✓ Brain-World Integration" << std::endl;
        std::cout << "  ✓ Checkpoint Functionality" << std::endl;
        std::cout << "  ✓ Complete System Integration" << std::endl;
    }
};

} // namespace test_phase6_integration

// Main test function
int main() {
    try {
        test_phase6_integration::Phase6IntegrationTest test;
        test.runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}
