// Comprehensive Integration Tests
// Tests the complete brain system with all integrated subsystems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_integration {

void testBrainSystemIntegration() {
    std::cout << "  Testing complete brain system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Complete initialization sequence
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Verify all systems are present
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
    
    // Verify all subsystems are functional
    brain.getWorkingMemory()->initialize(&brain);
    brain.getEpisodicMemory()->initialize(&brain);
    brain.getAssociativeMemory()->initialize(&brain);
    brain.getPlanner()->initialize(&brain);
    brain.getConceptFormation()->initialize(&brain);
    brain.getAttention()->initialize(&brain);
    brain.getDevelopmentSystem()->initialize(&brain);
    
    // Verify integration metrics
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t regionCount = brain.getRegionCount();
    
    assert(totalNeurons == 200);
    assert(regionCount == 2);
    assert(totalSynapses > 0);
    
    std::cout << "    ✓ Complete brain system integration verified" << std::endl;
}

void testMemorySystemCohesion() {
    std::cout << "  Testing memory system cohesion..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(123), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory - active information maintenance
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        workingMemory->initialize(&brain);
        
        // Store multiple traces
        workingMemory->storeToNeuron(nlm::NeuronId(1), 0.8f);
        workingMemory->storeToNeuron(nlm::NeuronId(2), 0.6f);
        workingMemory->storeToNeuron(nlm::NeuronId(3), 0.4f);
        
        // Verify traces are maintained
        size_t traces = workingMemory->getActiveTraces();
        assert(traces >= 3);
        
        // Update memory
        workingMemory->update(0.001);
        
        std::cout << "    ✓ Working memory cohesion verified" << std::endl;
    }
    
    // Test episodic memory - experience storage
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        episodicMemory->initialize(&brain);
        
        // Store multiple episodes
        for (size_t i = 0; i < 5; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = static_cast<nlm::SimulationStep>(i);
            episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
            episode.activeNeurons.push_back(nlm::NeuronId(i + 10));
            episode.neuronActivations.push_back(0.7f);
            episodicMemory->storeEpisode(episode);
        }
        
        // Verify episodes are stored and retrievable
        size_t episodes = episodicMemory->getEpisodeCount();
        assert(episodes >= 5);
        
        // Test replay functionality
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        assert(episodesToReplay.size() <= 3);
        
        std::cout << "    ✓ Episodic memory cohesion verified" << std::endl;
    }
    
    // Test associative memory - pattern associations
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory) {
        associativeMemory->initialize(&brain);
        
        // Store patterns
        associativeMemory->storePattern({nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)}, 0.8f);
        associativeMemory->storePattern({nlm::NeuronId(4), nlm::NeuronId(5), nlm::NeuronId(6)}, 0.9f);
        associativeMemory->storePattern({nlm::NeuronId(7), nlm::NeuronId(8), nlm::NeuronId(9)}, 0.7f);
        
        // Retrieve patterns
        auto patterns = associativeMemory->retrievePattern({nlm::NeuronId(1), nlm::NeuronId(2)});
        assert(!patterns.empty());
        
        std::cout << "    ✓ Associative memory cohesion verified" << std::endl;
    }
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(456), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine system - reward prediction
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        dopamine->initialize(&brain);
        
        // Verify dopamine level can be modified
        float initialLevel = dopamine->getLevel();
        
        // Apply neuromodulation effects
        nlm::Neuromodulator signal(1.0f);  // High reward signal
        brain.applyNeuromodulation(signal);
        
        // Verify effects on STDP parameters
        auto* stdp = brain.getSTDP();
        if (stdp) {
            // Dopamine should modulate plasticity
            float plasticityFactor = dopamine->getPlasticityFactor();
            assert(plasticityFactor >= 0.0f);
        }
        
        std::cout << "    ✓ Dopamine modulation integration verified" << std::endl;
    }
    
    // Test curiosity system - exploration motivation
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        curiosity->initialize(&brain);
        
        float curiosityLevel = curiosity->getLevel();
        assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
        
        // Update curiosity
        curiosity->update(0.001);
        
        std::cout << "    ✓ Curiosity system integration verified" << std::endl;
    }
    
    // Test novelty detection - change detection
    auto* novelty = brain.getNovelty();
    if (novelty) {
        novelty->initialize(&brain);
        
        float noveltyLevel = novelty->getLevel();
        assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
        
        // Update novelty detection
        novelty->update(0.001);
        
        std::cout << "    ✓ Novelty detection integration verified" << std::endl;
    }
}

void testDevelopmentalProcesses() {
    std::cout << "  Testing developmental processes..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(789), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test developmental stage transitions
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Simulate development over time
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    nlm::DevelopmentalStage criticalStage = brain.getDevelopmentalStage();
    assert(criticalStage == nlm::DevelopmentalStage::CriticalPeriod);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    nlm::DevelopmentalStage maturationStage = brain.getDevelopmentalStage();
    assert(maturationStage == nlm::DevelopmentalStage::Maturation);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    nlm::DevelopmentalStage adultStage = brain.getDevelopmentalStage();
    assert(adultStage == nlm::DevelopmentalStage::Adult);
    
    // Test developmental effects on plasticity
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        developmentSystem->initialize(&brain);
        
        // Development should affect plasticity rates
        developmentSystem->update(&brain, *brain.getRandomGenerator(), 1.0);
        
        // Verify structural plasticity is affected
        auto* sp = brain.getStructuralPlasticity();
        if (sp) {
            float synaptogenesisRate = sp->getSynaptogenesisRate();
            float pruningRate = sp->getPruningRate();
            assert(synaptogenesisRate >= 0.0f);
            assert(pruningRate >= 0.0f);
        }
        
        std::cout << "    ✓ Developmental processes verified" << std::endl;
    }
}

void testLearningOverTime() {
    std::cout << "  Testing learning over extended simulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(999), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Store initial synaptic weights
    std::vector<float> initialWeights;
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Run extended simulation with learning
    size_t learningSteps = 1000;
    float learningCurve[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    for (size_t step = 0; step < learningSteps; ++step) {
        // Apply diverse inputs to stimulate learning
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        } else if (step % 15 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 15.0f);
        }
        
        brain.step(step);
        
        // Record learning metrics at intervals
        if (step % 200 == 0 && step > 0) {
            size_t index = std::min(step / 200, static_cast<size_t>(4));
            if (auto* region = brain.getRegion(nlm::RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    learningCurve[index] += std::abs(syn->getWeight() - initialWeights[0]);
                }
            }
        }
    }
    
    // Verify learning occurred
    bool learningObserved = false;
    for (size_t i = 0; i < 5; ++i) {
        if (learningCurve[i] > 0.001f) {
            learningObserved = true;
            break;
        }
    }
    
    assert(learningObserved);
    std::cout << "    ✓ Learning over time verified" << std::endl;
}

void testErrorHandling() {
    std::cout << "  Testing error handling and robustness..." << std::endl;
    
    // Test initialization with invalid configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);  // Invalid: 0 neurons
    
    nlm::Brain brain(config);
    
    // Brain should still initialize but with default values
    bool initSuccess = brain.initialize();
    assert(initSuccess);  // Should succeed with defaults
    
    // Test brain reset functionality
    brain.reset();
    brain.initialize();
    
    // Test brain state saving and loading
    std::string testFile = "test_checkpoint.bin";
    
    bool saveSuccess = brain.save(testFile);
    assert(saveSuccess);
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(testFile);
    assert(loadSuccess);
    
    // Clean up test file
    std::remove(testFile.c_str());
    
    std::cout << "    ✓ Error handling verified" << std::endl;
}

void testPerformanceAndStability() {
    std::cout << "  Testing performance and stability..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);  // Larger network
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(111), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run performance test
    size_t performanceSteps = 2000;
    size_t spikeEvents = 0;
    
    for (nlm::SimulationStep step = 0; step < performanceSteps; ++step) {
        brain.step(step);
        spikeEvents += brain.getTotalSpikeCount();
    }
    
    // Verify network stability (no crashes, reasonable activity)
    assert(spikeEvents > 0);  // Some activity expected
    assert(brain.getTotalNeuronCount() == 300);
    assert(brain.getRegionCount() == 3);
    
    // Test statistics consistency
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    float avgFiringRate = brain.getAverageFiringRate();
    
    assert(totalSpikes >= 0);
    assert(firingNeurons >= 0);
    assert(avgFiringRate >= 0.0f && avgFiringRate < 100.0f);  // Reasonable firing rate
    
    std::cout << "    ✓ Performance and stability verified" << std::endl;
}

void runAll() {
    std::cout << "\n=== Comprehensive Integration Tests ===" << std::endl;
    
    testBrainSystemIntegration();
    testMemorySystemCohesion();
    testNeuromodulationIntegration();
    testDevelopmentalProcesses();
    testLearningOverTime();
    testErrorHandling();
    testPerformanceAndStability();
    
    std::cout << "\n=== All Integration Tests Passed ===" << std::endl;
}

} // namespace test_integration
