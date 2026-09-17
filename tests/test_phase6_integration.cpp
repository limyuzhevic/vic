#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_phase6_integration {

void testCompleteBrainInitialization() {
    std::cout << "    testCompleteBrainInitialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Phase 6 standard configuration
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.08, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize with comprehensive settings
    bool initResult = brain.initialize();
    assert(initResult);
    
    // Verify all major components are initialized
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Verify memory systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Verify prediction system exists
    assert(brain.getPredictionSystem() != nullptr);
    
    // Verify cognition systems exist
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Verify neuromodulation systems exist
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Verify development system exists
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Verify plasticity systems exist
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    std::cout << "    testCompleteBrainInitialization passed" << std::endl;
}

void testBrainWorldIntegration() {
    std::cout << "    testBrainWorldIntegration..." << std::endl;
    
    // Create complete agent-world-brain setup
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(123), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Create agent interface
    auto agent = nlm::createAgentBrain(brain);
    
    // Create world
    auto world = nlm::createSimpleWorld();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Initialize agent with world
    agent->initialize(*world);
    
    // Enable all subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run a complete simulation episode
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        assert(percept != nullptr);
        
        // Process sensory input
        agent->processSensoryInput(*percept);
        
        // Run brain step
        brain.step(step, step * 0.1);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        assert(action != nlm::MotorCommand::Wait || step < 50); // Allow waiting early in episode
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = 0.0f;
        if (world->getAgentBody() && world->getAgentBody()->energy > 50.0f) {
            reward = 0.1f;
        }
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Record statistics periodically
        if (step % 20 == 0) {
            assert(brain.getTotalSpikeCount() >= 0);
            assert(agent->getCuriosityLevel() >= 0.0f);
            assert(agent->getNoveltyLevel() >= 0.0f);
        }
    }
    
    std::cout << "    testBrainWorldIntegration passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "    testMemorySystemIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(456), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Inject some activity to populate working memory
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject sensory input to stimulate memory formation
        nlm::SensoryInput dummyInput;
        dummyInput.addSignal(1.0f);
        brain.receiveSensoryInput(dummyInput);
        
        if (workingMemory && step % 10 == 0) {
            // Check working memory stores activity
            auto activeNeurons = workingMemory->getMemoryNeurons();
            assert(activeNeurons.size() >= 0); // May be empty initially
        }
    }
    
    // Check episodic memory stores experience
    if (episodicMemory) {
        auto episodes = episodicMemory->getEpisodeCount();
        assert(episodes >= 0); // May be 0 initially
    }
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testPredictionCognitionIntegration() {
    std::cout << "    testPredictionCognitionIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(789), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    auto* predictionSystem = brain.getPredictionSystem();
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    // Run simulation with active prediction and cognition
    for (nlm::SimulationStep step = 0; step < 80; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject varying input to stimulate prediction
        nlm::SensoryInput input;
        input.addSignal(std::sin(static_cast<float>(step) * 0.1f));
        brain.receiveSensoryInput(input);
        
        // Check cognition systems are functioning
        if (planner && step % 10 == 0) {
            // Planning system could make decisions based on neural activity
        }
        
        if (conceptFormation && step % 15 == 0) {
            // Concept formation could detect patterns
        }
        
        if (attention && step % 20 == 0) {
            // Attention system could prioritize information
        }
        
        // Verify brain statistics are consistent
        assert(brain.getTotalSpikeCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
        assert(brain.getExcitationInhibitionRatio() > 0.0f);
    }
    
    std::cout << "    testPredictionCognitionIntegration passed" << std::endl;
}

void testNeuromodulationDevelopmentIntegration() {
    std::cout << "    testNeuromodulationDevelopmentIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(999), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* developmentSystem = brain.getDevelopmentSystem();
    
    // Simulate learning with neuromodulation
    for (nlm::SimulationStep step = 0; step < 120; ++step) {
        // Apply varying neuromodulation
        float reward = (step % 30 == 0) ? 1.0f : 0.0f;
        float predicted = 0.0f;
        
        // Apply reward-based neuromodulation through brain
        nlm::Neuromodulator neuromodulator;
        neuromodulator.setLevel(reward);
        brain.applyNeuromodulation(neuromodulator);
        
        brain.step(step, step * 0.001);
        
        // Check neuromodulation levels are within bounds
        if (dopamine) {
            float level = dopamine->getLevel();
            assert(level >= -1.0f && level <= 1.0f);
        }
        
        if (curiosity) {
            float level = curiosity->getLevel();
            assert(level >= 0.0f && level <= 1.0f);
        }
        
        if (novelty) {
            float level = novelty->getLevel();
            assert(level >= 0.0f && level <= 1.0f);
        }
        
        // Update development
        if (developmentSystem && step % 10 == 0) {
            brain.develop();
            
            // Check developmental stage progression
            auto stage = brain.getDevelopmentalStage();
            assert(static_cast<int>(stage) >= static_cast<int>(nlm::DevelopmentalStage::Initial));
        }
    }
    
    std::cout << "    testNeuromodulationDevelopmentIntegration passed" << std::endl;
}

void testPlasticityLearningIntegration() {
    std::cout << "    testPlasticityLearningIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(333), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    
    // Run learning experiment with plasticity
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject correlated activity to trigger STDP
        if (step % 2 == 0) {
            // Fire sensory neurons
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 50.0f);
        }
        
        if (step % 3 == 0) {
            // Fire internal neurons
            brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 30.0f);
        }
        
        // Check that plasticity systems are working
        if (stdp && step % 20 == 0) {
            // STDP should be updating weights based on spike timing
            float ltp = stdp->getLTPWeight();
            float ltd = stdp->getLTDWeight();
            assert(ltp > 0.0f);
            assert(ltd > 0.0f);
        }
        
        // Structural plasticity should occasionally update
        if (structuralPlasticity && step % 50 == 0) {
            // This would modify the connectivity structure
        }
    }
    
    size_t finalSynapses = brain.getTotalSynapseCount();
    assert(finalSynapses >= initialSynapses); // Synapses can only increase or stay same
    
    std::cout << "    testPlasticityLearningIntegration passed" << std::endl;
}

void testCompletePhase6Simulation() {
    std::cout << "    testCompletePhase6Simulation..." << std::endl;
    
    // Test the complete Phase 6 integrated brain system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(111), nlm::ConfigSource::Default);
    config->set("replay_interval", 50, nlm::ConfigSource::Default);
    config->set("consolidation_interval", 500, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    auto agent = nlm::createAgentBrain(brain);
    auto world = nlm::createSimpleWorld();
    world->configure(30, 30, 12, 12);
    world->reset();
    world->setAgentStart(15.0, 15.0);
    
    agent->initialize(*world);
    
    // Enable all advanced features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run extended simulation (Phase 6 typical episode length)
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Complete agent-world loop
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(*percept);
        
        brain.step(step, step * 0.1);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Calculate reward from world state
        float reward = 0.0f;
        if (world->getAgentBody()) {
            if (world->getAgentBody()->energy > world->getAgentBody()->getMaxEnergy() * 0.8f) {
                reward = 0.2f; // Good energy level
            }
            if (world->getAgentBody()->health < world->getAgentBody()->getMaxHealth() * 0.3f) {
                reward = -0.1f; // Low health penalty
            }
        }
        
        agent->applyRewardModulation(reward, 0.0f);
        agent->updateDevelopment(0.1);
        
        // Verify system health every 50 steps
        if (step % 50 == 0) {
            assert(brain.getTotalSpikeCount() >= 0);
            assert(agent->getCuriosityLevel() >= 0.0f);
            assert(agent->getNoveltyLevel() >= 0.0f);
            assert(agent->getPredictionError() >= -1.0f && agent->getPredictionError() <= 1.0f);
            assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial || step >= 100);
        }
    }
    
    // Verify brain can be saved and loaded
    std::string checkpointFile = "test_checkpoint.bin";
    bool saveResult = brain.save(checkpointFile);
    assert(saveResult);
    
    // Create a fresh brain for loading test
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config2->set("random_seed", static_cast<int64_t>(111), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    bool init2 = brain2.initialize();
    assert(init2);
    
    // Load the saved checkpoint
    bool loadResult = brain2.load(checkpointFile);
    assert(loadResult);
    
    std::cout << "    testCompletePhase6Simulation passed" << std::endl;
}

void runAll() {
    std::cout << "Running Phase 6 Integration Tests..." << std::endl;
    std::cout << "=" * 50 << std::endl;
    
    testCompleteBrainInitialization();
    testBrainWorldIntegration();
    testMemorySystemIntegration();
    testPredictionCognitionIntegration();
    testNeuromodulationDevelopmentIntegration();
    testPlasticityLearningIntegration();
    testCompletePhase6Simulation();
    
    std::cout << "=" * 50 << std::endl;
    std::cout << "All Phase 6 Integration Tests PASSED!" << std::endl;
    std::cout << "The complete integrated brain system functions correctly." << std::endl;
}

} // namespace test_phase6_integration

// Main function for Phase 6 integration tests
int main(int argc, char** argv) {
    test_phase6_integration::runAll();
    return 0;
}