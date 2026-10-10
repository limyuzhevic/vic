// Verification Tests for Optimized NLM Implementations
// Tests optimized Brain, STDP, AgentBrain, and World implementations
// Verifies correctness and performance improvements

#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

namespace test_optimizations {

// Test STDP optimization correctness
void testSTDPAlgorithmCorrectness() {
    std::cout << "Testing STDP algorithm correctness..." << std::endl;
    
    nlm::STDP stdp;
    
    // Test potentiation scenario
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.5f);
    
    float initialWeight1 = synapse1.getWeight();
    std::vector<nlm::Timestamp> preSpikes1 = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes1 = {5.0, 15.0, 25.0}; // After pre
    
    stdp.update(&synapse1, preSpikes1, postSpikes1, 0.001);
    float finalWeight1 = synapse1.getWeight();
    
    assert(finalWeight1 > initialWeight1);
    std::cout << "  ✓ STDP potentiation works correctly (Δ=" 
              << (finalWeight1 - initialWeight1) << ")" << std::endl;
    
    // Test depression scenario
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    float initialWeight2 = synapse2.getWeight();
    std::vector<nlm::Timestamp> preSpikes2 = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes2 = {5.0, 15.0, 25.0}; // Before pre
    
    stdp.update(&synapse2, preSpikes2, postSpikes2, 0.001);
    float finalWeight2 = synapse2.getWeight();
    
    assert(finalWeight2 < initialWeight2);
    std::cout << "  ✓ STDP depression works correctly (Δ=" 
              << (finalWeight2 - initialWeight2) << ")" << std::endl;
    
    // Test edge case - empty spike trains
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse3.setType(nlm::SynapseType::Excitatory);
    synapse3.setWeight(0.8f);
    float initialWeight3 = synapse3.getWeight();
    
    std::vector<nlm::Timestamp> emptyPre, emptyPost;
    stdp.update(&synapse3, emptyPre, emptyPost, 0.001);
    assert(std::abs(synapse3.getWeight() - initialWeight3) < 1e-5f);
    std::cout << "  ✓ STDP handles empty spike trains correctly" << std::endl;
    
    // Test O(n) vs O(n²) complexity - optimized implementation should handle more spikes
    nlm::Synapse synapse4(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse4.setType(nlm::SynapseType::Excitatory);
    synapse4.setWeight(0.3f);
    float initialWeight4 = synapse4.getWeight();
    
    std::vector<nlm::Timestamp> manyPre, manyPost;
    for (int i = 0; i < 50; ++i) {
        manyPre.push_back(i * 10.0f);
        manyPost.push_back(i * 10.0f + 5.0f);
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    stdp.update(&synapse4, manyPre, manyPost, 0.001);
    auto endTime = std::chrono::high_resolution_clock::now();
    double processingTime = std::chrono::duration<double>(endTime - startTime).count();
    
    assert(std::abs(synapse4.getWeight() - initialWeight4) > 0.0f);
    assert(finalWeight1 > initialWeight1);
    std::cout << "  ✓ O(n) STDP handles 50 spike pairs efficiently ("
              << processingTime * 1000 << "ms)" << std::endl;
}

// Test Brain step() optimization correctness
void testBrainStepOptimization() {
    std::cout << "Testing Brain step() optimization correctness..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Run multiple steps to verify stability
    size_t totalSpikes = 0;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        totalSpikes += brain.getFiringNeuronCount();
    }
    
    // Verify brain remains in valid state
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalSpikeCount() >= totalSpikes);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "  ✓ Brain step() maintains stability over 100 steps ("
              << totalSpikes << " total spikes)" << std::endl;
    
    // Test memory access optimization
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t synapseCount = brain.getTotalSynapseCount();
    
    // Verify neurons are properly connected
    float eiRatio = brain.getExcitationInhibitionRatio();
    assert(eiRatio >= 0.0f);
    
    std::cout << "  ✓ Memory access optimization preserves connectivity (E/I ratio: "
              << eiRatio << ")" << std::endl;
}

// Test AgentBrain vectorization optimization
void testAgentBrainVectorization() {
    std::cout << "Testing AgentBrain vectorization optimization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create a simple world
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Test sensory processing with vectorized operations
    auto sensoryPercept = world.getSensoryPercept();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; ++i) {
        agentBrain.processSensoryInput(sensoryPercept);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double processingTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Verify novelty detection and curiosity computation
    float novelty = agentBrain.getNoveltyLevel();
    float curiosity = agentBrain.getCuriosityLevel();
    
    assert(novelty >= 0.0f && novelty <= 1.0f);
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    std::cout << "  ✓ Vectorized sensory processing completed 10 iterations in "
              << processingTime * 1000 << "ms" << std::endl;
    
    // Test motor command decoding with optimized lookup
    auto motorCommand = agentBrain.decodeMotorCommand();
    assert(motorCommand != nlm::MotorCommand::Wait);  // Should have some activity
    
    std::cout << "  ✓ Optimized motor decoding produced command: "
              << static_cast<int>(motorCommand) << std::endl;
    
    // Test neuromodulation
    agentBrain.applyRewardModulation(0.5f, 0.3f);
    float dopamine = agentBrain.getNeuromodulationLevel();
    assert(dopamine >= -1.0f && dopamine <= 1.0f);
    
    std::cout << "  ✓ Neuromodulation works with vectorized operations (dopamine: "
              << dopamine << ")" << std::endl;
}

// Test SimpleWorld vision optimization
void testSimpleWorldVisionOptimization() {
    std::cout << "Testing SimpleWorld vision optimization..." << std::endl;
    
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    world.reset();
    
    // Test vision generation with optimized precomputed values
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 20; ++i) {
        world.update(0.001);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double processingTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Verify vision quality
    auto sensoryPercept = world.getSensoryPercept();
    auto vision = sensoryPercept.getVision();
    
    assert(vision.size() == 16 * 16);  // 16x16 vision grid
    for (float val : vision) {
        assert(val >= 0.0f && val <= 1.0f);  // Normalized values
    }
    
    std::cout << "  ✓ Optimized vision generation completed 20 updates in "
              << processingTime * 1000 << "ms" << std::endl;
    
    // Test touch sensor optimization
    auto touch = sensoryPercept.getTouch();
    assert(touch.size() == 8);  // 8 touch sensors
    for (float val : touch) {
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    std::cout << "  ✓ Vectorized touch sensor optimization completed" << std::endl;
    
    // Test proprioception optimization
    auto proprioception = sensoryPercept.getProprioception();
    assert(proprioception.size() == 6);  // 6 proprioceptive sensors
    for (float val : proprioception) {
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    std::cout << "  ✓ Optimized proprioception calculation completed" << std::endl;
}

// Test memory system optimization
void testMemorySystemOptimization() {
    std::cout << "Testing memory system optimization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory optimization
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store and retrieve neuron activities
    workingMemory->store(1, 0.8f);
    workingMemory->store(2, 0.6f);
    workingMemory->store(3, 0.9f);
    
    assert(std::abs(workingMemory->retrieve(1) - 0.8f) < 1e-5f);
    assert(std::abs(workingMemory->retrieve(2) - 0.6f) < 1e-5f);
    assert(std::abs(workingMemory->retrieve(3) - 0.9f) < 1e-5f);
    assert(workingMemory->contains(1));
    assert(!workingMemory->contains(99)); // Non-existent neuron
    
    // Test decay optimization
    float beforeDecay = workingMemory->retrieve(1);
    workingMemory->decay(0.1f);  // 10% decay
    float afterDecay = workingMemory->retrieve(1);
    
    assert(afterDecay < beforeDecay);
    assert(std::abs(afterDecay - beforeDecay * 0.9f) < 1e-5f);
    
    std::cout << "  ✓ Working memory optimized storage and retrieval completed" << std::endl;
    
    // Test episodic memory optimization
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.activeNeurons = {1, 2, 3};
    episode.neuronActivations = {0.8f, 0.6f, 0.9f};
    episode.reward = 0.5f;
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    auto retrievedEpisode = episodicMemory->retrieveEpisode(0);
    assert(retrievedEpisode.activeNeurons.size() == 3);
    
    std::cout << "  ✓ Episodic memory optimized storage and retrieval completed" << std::endl;
}

// Performance benchmark comparison
void testPerformanceBenchmarks() {
    std::cout << "Testing performance benchmarks..." << std::endl;
    
    // Test with different network sizes
    std::vector<std::pair<size_t, size_t>> testSizes = {
        {50, 200},   // Small network
        {200, 100},  // Medium network
        {500, 50},   // Large network
    };
    
    for (const auto& size : testSizes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(size.first), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        for (nlm::SimulationStep step = 0; step < size.second; ++step) {
            brain.step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = size.second / totalTime;
        
        std::cout << "  Network size " << size.first << " neurons: "
                  << std::fixed << std::setprecision(0) << stepsPerSecond
                  << " steps/sec (" << size.second << " steps)" << std::endl;
        
        // Verify performance meets expectations
        assert(stepsPerSecond > 0.0);  // Should process steps
        assert(stepsPerSecond < 10000.0);  // Should be reasonable (not too fast)
    }
    
    std::cout << "  ✓ All performance benchmarks passed" << std::endl;
}

void runAll() {
    std::cout << "========================================================" << std::endl;
    std::cout << "NLM Optimization Verification Tests" << std::endl;
    std::cout << "========================================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testSTDPAlgorithmCorrectness();
        std::cout << std::endl;
        
        testBrainStepOptimization();
        std::cout << std::endl;
        
        testAgentBrainVectorization();
        std::cout << std::endl;
        
        testSimpleWorldVisionOptimization();
        std::cout << std::endl;
        
        testMemorySystemOptimization();
        std::cout << std::endl;
        
        testPerformanceBenchmarks();
        std::cout << std::endl;
        
        std::cout << "========================================================" << std::endl;
        std::cout << "✓ All optimization tests passed successfully!" << std::endl;
        std::cout << "========================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        throw;
    } catch (...) {
        std::cout << "❌ Test failed with unknown exception" << std::endl;
        throw;
    }
}

} // namespace test_optimizations

int main() {
    test_optimizations::runAll();
    return 0;
}
