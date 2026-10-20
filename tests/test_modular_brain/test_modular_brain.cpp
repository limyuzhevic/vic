// Integration Tests
// Comprehensive integration tests for NLM modular architecture

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

namespace test_integration {

void testComponentIntegration() {
    std::cout << "    Testing component integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test all components are available
    assert(brain.getNeuralComputation() != nullptr);
    assert(brain.getMemoryIntegration() != nullptr);
    assert(brain.getPredictionCoordinator() != nullptr);
    assert(brain.getCognitiveOrchestrator() != nullptr);
    assert(brain.getNeuromodulationController() != nullptr);
    assert(brain.getPlasticityManager() != nullptr);
    assert(brain.getDevelopmentManager() != nullptr);
    assert(brain.getSensoryMotorInterface() != nullptr);
    
    // Run simulation with all components
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    std::cout << "        Component integration passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("working_memory.capacity", 50, nlm::ConfigSource::Default);
    config->set("episodic_memory.max_episodes", 100, nlm::ConfigSource::Default);
    config->set("associative_memory.max_connections", 200, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    auto* workingMemory = brain.getMemoryIntegration()->getWorkingMemory();
    assert(workingMemory != nullptr);
    assert(workingMemory->getCapacity() == 50);
    
    // Test episodic memory
    auto* episodicMemory = brain.getMemoryIntegration()->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    assert(episodicMemory->getMaxEpisodes() == 100);
    
    // Test associative memory
    auto* associativeMemory = brain.getMemoryIntegration()->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Run simulation to exercise memory systems
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Check memory usage
    auto memoryStats = brain.getMemoryIntegration()->getMemoryStats();
    assert(memoryStats["working_memory"]["capacity"] == 50);
    
    std::cout << "        Memory system integration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation systems
    auto* neuromodController = brain.getNeuromodulationController();
    assert(neuromController != nullptr);
    
    // Test dopamine system
    auto* dopamine = neuromodController->getDopamine();
    assert(dopamine != nullptr);
    assert(dopamine->getScale() > 0);
    
    // Test curiosity system
    auto* curiosity = neuromodController->getCuriosity();
    assert(curiosity != nullptr);
    assert(curiosity->getLevel() >= 0);
    
    // Test novelty system
    auto* novelty = neuromodController->getNovelty();
    assert(novelty != nullptr);
    assert(novelty->getLevel() >= 0);
    
    // Run simulation with neuromodulation
    for (nlm::SimulationStep step = 0; step < 75; ++step) {
        brain.step(step);
    }
    
    // Test neuromodulation effects on learning
    auto neuromodStats = neuromodController->getNeuromodulationStats();
    assert(neuromodStats.find("dopamine") != neuromodStats.end());
    assert(neuromodStats.find("curiosity") != neuromodStats.end());
    assert(neuromodStats.find("novelty") != neuromodStats.end());
    
    std::cout << "        Neuromodulation integration passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "    Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("prediction.sequence_length", 10, nlm::ConfigSource::Default);
    config->set("prediction.confidence_threshold", 0.5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system
    auto* predictionCoordinator = brain.getPredictionCoordinator();
    assert(predictionCoordinator != nullptr);
    
    // Test neural prediction
    auto* neuralPrediction = predictionCoordinator->getNeuralPrediction();
    assert(neuralPrediction != nullptr);
    assert(neuralPrediction->getSequenceLength() == 10);
    assert(neuralPrediction->getConfidenceThreshold() == 0.5f);
    
    // Test action consequence prediction
    auto* actionPredictor = predictionCoordinator->getActionConsequencePredictor();
    assert(actionPredictor != nullptr);
    
    // Run simulation with prediction
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        brain.step(step);
    }
    
    // Test prediction accuracy
    auto predictionStats = predictionCoordinator->getPredictionStats();
    assert(predictionStats.find("sequence_prediction") != predictionStats.end());
    assert(predictionStats.find("action_consequence_prediction") != predictionStats.end());
    
    std::cout << "        Prediction system integration passed" << std::endl;
}

void testCognitiveSystemIntegration() {
    std::cout << "    Testing cognitive system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("cognition.planning_depth", 5, nlm::ConfigSource::Default);
    config->set("cognition.attention_strength", 1.0, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test cognitive systems
    auto* cognitiveOrchestrator = brain.getCognitiveOrchestrator();
    assert(cognitiveOrchestrator != nullptr);
    
    // Test neural planner
    auto* neuralPlanner = cognitiveOrchestrator->getNeuralPlanner();
    assert(neuralPlanner != nullptr);
    assert(neuralPlanner->getPlanningDepth() == 5);
    
    // Test concept formation
    auto* conceptFormation = cognitiveOrchestrator->getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test attentional selection
    auto* attentionalSelection = cognitiveOrchestrator->getAttentionalSelection();
    assert(attentionalSelection != nullptr);
    assert(attentionalSelection->getInhibitionStrength() == 0.5f);
    
    // Run simulation with cognitive processing
    for (nlm::SimulationStep step = 0; step < 85; ++step) {
        brain.step(step);
    }
    
    // Test cognitive performance
    auto cognitiveStats = cognitiveOrchestrator->getCognitiveStats();
    assert(cognitiveStats.find("planning") != cognitiveStats.end());
    assert(cognitiveStats.find("concept_formation") != cognitiveStats.end());
    assert(cognitiveStats.find("attention") != cognitiveStats.end());
    
    std::cout << "        Cognitive system integration passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "    Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("development.stage", "CriticalPeriod", nlm::ConfigSource::Default);
    config->set("development.plasticity_modulation", 0.8, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development systems
    auto* developmentManager = brain.getDevelopmentManager();
    assert(developmentManager != nullptr);
    
    // Test synaptogenesis
    auto* synaptogenesis = developmentManager->getSynaptogenesis();
    assert(synaptogenesis != nullptr);
    
    // Test pruning
    auto* pruning = developmentManager->getPruning();
    assert(pruning != nullptr);
    
    // Test maturation
    auto* maturation = developmentManager->getMaturation();
    assert(maturation != nullptr);
    
    // Run simulation with development
    for (nlm::SimulationStep step = 0; step < 90; ++step) {
        brain.step(step);
    }
    
    // Test developmental stage progression
    auto developmentalStats = developmentManager->getDevelopmentStats();
    assert(developmentalStats.find("synaptogenesis") != developmentalStats.end());
    assert(developmentalStats.find("pruning") != developmentalStats.end());
    assert(developmentalStats.find("maturation") != developmentalStats.end());
    
    std::cout << "        Development system integration passed" << std::endl;
}

void testPlasticitySystemIntegration() {
    std::cout << "    Testing plasticity system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(140), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test plasticity systems
    auto* plasticityManager = brain.getPlasticityManager();
    assert(plasticityManager != nullptr);
    
    // Test STDP
    auto* stdp = plasticityManager->getSTDP();
    assert(stdp != nullptr);
    assert(stdp->getLearningRate() > 0);
    
    // Test Hebbian
    auto* hebbian = plasticityManager->getHebbian();
    assert(hebbian != nullptr);
    assert(hebbian->getLearningRate() > 0);
    
    // Test structural plasticity
    auto* structuralPlasticity = plasticityManager->getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    // Run simulation with plasticity
    for (nlm::SimulationStep step = 0; step < 70; ++step) {
        brain.step(step);
    }
    
    // Test plasticity effects
    auto plasticityStats = plasticityManager->getPlasticityStats();
    assert(plasticityStats.find("stdp") != plasticityStats.end());
    assert(plasticityStats.find("hebbian") != plasticityStats.end());
    assert(plasticityStats.find("structural") != plasticityStats.end());
    
    std::cout << "        Plasticity system integration passed" << std::endl;
}

void testAgentWorldIntegration() {
    std::cout << "    Testing agent-world integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(15, 15, 8, 8);
    
    // Initialize agent with world
    assert(agent->initialize(world));
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run agent-world simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain thinks
        brain.step(step);
        
        // Get action from brain
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        agent->applyRewardModulation(0.1, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
    }
    
    // Test agent performance
    auto performanceStats = agent->getPerformanceStats();
    assert(performanceStats.find("learning_enabled") != performanceStats.end());
    assert(performanceStats.find("neuromodulation_level") != performanceStats.end());
    
    std::cout << "        Agent-world integration passed" << std::endl;
}

void testCompleteIntegration() {
    std::cout << "    Testing complete multi-component integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("working_memory.capacity", 100, nlm::ConfigSource::Default);
    config->set("episodic_memory.max_episodes", 500, nlm::ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
    config->set("cognition.planning_depth", 3, nlm::ConfigSource::Default);
    config->set("development.stage", "Maturation", nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(25, 25, 12, 12);
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    assert(agent->initialize(world));
    
    // Enable all features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run complete simulation
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Update world
        world->update(0.1);
        
        // Process sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Brain computes
        brain.step(step);
        
        // Decode action
        auto action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply learning
        agent->applyRewardModulation(0.05 + (step % 10) * 0.01, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Periodic checkpointing
        if (step % 50 == 0) {
            std::string checkpointFile = "checkpoint_" + std::to_string(step) + ".bin";
            assert(brain.save(checkpointFile));
            assert(brain.load(checkpointFile));
        }
    }
    
    // Final status check
    brain.logStatus();
    
    // Verify all components are working
    assert(brain.getTotalNeuronCount() == 300);
    assert(brain.getRegionCount() == 3);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalSpikeCount() > 0);
    
    std::cout << "        Complete multi-component integration passed" << std::endl;
}

void testPerformanceBenchmark() {
    std::cout << "    Testing performance benchmarks..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.02, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark initialization
    auto startTime = std::chrono::high_resolution_clock::now();
    nlm::Brain brain2(config);
    brain2.initialize();
    auto initEndTime = std::chrono::high_resolution_clock::now();
    auto initializationTime = std::chrono::duration_cast<std::chrono::milliseconds>(initEndTime - startTime);
    
    // Benchmark simulation
    startTime = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    auto simulationEndTime = std::chrono::high_resolution_clock::now();
    auto simulationTime = std::chrono::duration_cast<std::chrono::milliseconds>(simulationEndTime - startTime);
    
    // Calculate performance metrics
    double neuronsPerSecond = 500.0 / (simulationTime.count() / 1000.0);
    double synapsesPerSecond = brain.getTotalSynapseCount() / (simulationTime.count() / 1000.0);
    double spikesPerSecond = brain.getTotalSpikeCount() / (simulationTime.count() / 1000.0);
    
    std::cout << "        Initialization time: " << initializationTime.count() << " ms" << std::endl;
    std::cout << "        Simulation time: " << simulationTime.count() << " ms for 1000 steps" << std::endl;
    std::cout << "        Performance metrics:" << std::endl;
    std::cout << "          Neurons processed: " << neuronsPerSecond << " per second" << std::endl;
    std::cout << "          Synapses processed: " << synapsesPerSecond << " per second" << std::endl;
    std::cout << "          Spikes processed: " << spikesPerSecond << " per second" << std::endl;
    
    // Performance assertions
    assert(initializationTime.count() < 5000);  // Should initialize in under 5 seconds
    assert(simulationTime.count() < 10000);     // Should simulate 1000 steps in under 10 seconds
    assert(neuronsPerSecond > 1000);            // Should process >1000 neurons/second
    assert(spikesPerSecond > 1000);             // Should process >1000 spikes/second
    
    std::cout << "        Performance benchmark passed" << std::endl;
}

void runAll() {
    testComponentIntegration();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testPredictionSystemIntegration();
    testCognitiveSystemIntegration();
    testDevelopmentSystemIntegration();
    testPlasticitySystemIntegration();
    testAgentWorldIntegration();
    testCompleteIntegration();
    testPerformanceBenchmark();
    
    std::cout << "\nAll integration tests completed successfully!" << std::endl;
}

} // namespace test_integration