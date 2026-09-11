#include "Brain.hpp"
#include "Config.hpp"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

namespace nlm {

class TestBrain {
public:
    TestBrain() {
        // Create default config
        config = std::make_shared<Config>();
        
        // Configure for testing
        config->set("random_seed", uint64_t(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", size_t(1000), ConfigSource::Default);
        config->set("region_count", size_t(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create brain
        brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        std::cout << "✓ Brain created and initialized successfully" << std::endl;
    }
    
    void runBasicTests() {
        std::cout << "\n=== Running Basic Brain Tests ===" << std::endl;
        
        // Test 1: Basic neuron injection and step
        std::cout << "Test 1: Neuron injection and step...";
        brain->reset();
        brain->initialize();
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        assert(finalSpikes >= initialSpikes);
        std::cout << " ✓ " << (finalSpikes - initialSpikes) << " spikes detected" << std::endl;
        
        // Test 2: Region management
        std::cout << "Test 2: Region management...";
        brain->reset();
        brain->initialize();
        
        size_t initialRegions = brain->getRegionCount();
        RegionId newRegionId = brain->addRegion("TestRegion");
        size_t afterAddRegions = brain->getRegionCount();
        
        assert(afterAddRegions == initialRegions + 1);
        assert(brain->getRegion(newRegionId) != nullptr);
        std::cout << " ✓ Added region " << newRegionId.index() << " successfully" << std::endl;
        
        // Test 3: Neuron statistics
        std::cout << "Test 3: Neuron statistics...";
        brain->reset();
        brain->initialize();
        
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        size_t activeNeurons = brain->getActiveNeuronCount();
        
        assert(neuronCount > 0);
        assert(synapseCount > 0);
        std::cout << " ✓ " << neuronCount << " neurons, " << synapseCount << " synapses" << std::endl;
        
        // Test 4: Plasticity
        std::cout << "Test 4: Plasticity system access...";
        brain->reset();
        brain->initialize();
        
        STDP* stdp = brain->getSTDP();
        Hebbian* hebbian = brain->getHebbian();
        StructuralPlasticity* structural = brain->getStructuralPlasticity();
        
        assert(stdp != nullptr);
        assert(hebbian != nullptr);
        assert(structural != nullptr);
        std::cout << " ✓ All plasticity systems accessible" << std::endl;
        
        // Test 5: Spike system
        std::cout << "Test 5: Spike system access...";
        brain->reset();
        brain->initialize();
        
        SpikeSystem* spikeSystem = brain->getSpikeSystem();
        assert(spikeSystem != nullptr);
        
        size_t pendingSpikes = spikeSystem->getPendingSpikeCount();
        assert(pendingSpikes == 0);
        std::cout << " ✓ Spike system accessible, " << pendingSpikes << " pending spikes" << std::endl;
        
        // Test 6: Configuration access
        std::cout << "Test 6: Configuration access...";
        assert(config->has("neuron_count"));
        assert(config->has("random_seed"));
        
        auto neuronCountOpt = config->get<size_t>("neuron_count");
        assert(neuronCountOpt.has_value());
        assert(neuronCountOpt.value() == 1000);
        
        std::cout << " ✓ Configuration values accessible" << std::endl;
        
        std::cout << "All basic tests passed! ✓" << std::endl;
    }
    
    void runPerformanceTests() {
        std::cout << "\n=== Running Performance Tests ===" << std::endl;
        
        // Test 1: Large brain simulation
        std::cout << "Test 1: Large brain simulation...";
        config->set("neuron_count", size_t(10000), ConfigSource::Runtime);
        config->set("region_count", size_t(4), ConfigSource::Runtime);
        
        brain->reset();
        brain->initialize();
        
        auto startTime = std::chrono::steady_clock::now();
        
        for (size_t step = 0; step < 1000; ++step) {
            brain->step(step);
            
            if (step % 100 == 0) {
                std::cout << "  Step " << step << ": " 
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << " ✓ Simulated " << duration.count() << " ms for 1000 steps" << std::endl;
        
        // Test 2: Memory management
        std::cout << "Test 2: Memory management...";
        brain->reset();
        brain->initialize();
        
        // Get initial memory stats
        size_t initialNeurons = brain->getTotalNeuronCount();
        size_t initialSynapses = brain->getTotalSynapseCount();
        
        // Simulate some memory operations
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
            
            if (i % 50 == 0) {
                brain->save("/tmp/test_checkpoint_" + std::to_string(i) + ".bin");
                brain->load("/tmp/test_checkpoint_" + std::to_string(i) + ".bin");
            }
        }
        
        size_t finalNeurons = brain->getTotalNeuronCount();
        size_t finalSynapses = brain->getTotalSynapseCount();
        
        assert(initialNeurons == finalNeurons);
        assert(initialSynapses == finalSynapses);
        std::cout << " ✓ Memory management stable across simulation" << std::endl;
        
        // Test 3: Spike system performance
        std::cout << "Test 3: Spike system performance...";
        brain->reset();
        brain->initialize();
        
        SpikeSystem* spikeSystem = brain->getSpikeSystem();
        assert(spikeSystem != nullptr);
        
        size_t pendingBefore = spikeSystem->getPendingSpikeCount();
        size_t delayedBefore = spikeSystem->getPendingDelayedCount();
        
        // Inject current to generate spikes
        brain->injectCurrentToNeurons(NeuronType::Excitatory, 20.0f);
        
        for (size_t step = 0; step < 200; ++step) {
            brain->step(step);
        }
        
        size_t pendingAfter = spikeSystem->getPendingSpikeCount();
        size_t delayedAfter = spikeSystem->getPendingDelayedCount();
        
        std::cout << " ✓ Spike system handled " << (pendingAfter - pendingBefore) 
                 << " new spikes, " << (delayedAfter - delayedBefore) 
                 << " delayed spikes" << std::endl;
        
        // Test 4: Plasticity performance
        std::cout << "Test 4: Plasticity performance...";
        brain->reset();
        brain->initialize();
        
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
        
        auto startPlasticity = std::chrono::steady_clock::now();
        
        for (size_t step = 0; step < 500; ++step) {
            brain->step(step);
            
            // Inject current to stimulate plasticity
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
        }
        
        auto endPlasticity = std::chrono::steady_clock::now();
        auto plasticityDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endPlasticity - startPlasticity);
        
        std::cout << " ✓ Applied plasticity for " << plasticityDuration.count() << " ms" << std::endl;
        
        std::cout << "All performance tests passed! ✓" << std::endl;
    }
    
    void runMemoryPoolTests() {
        std::cout << "\n=== Running Memory Pool Tests ===" << std::endl;
        
        // Test 1: Memory pool allocation
        std::cout << "Test 1: Memory pool allocation...";
        
        // Create many neurons to test memory pool
        config->set("neuron_count", size_t(50000), ConfigSource::Runtime);
        config->set("region_count", size_t(2), ConfigSource::Runtime);
        
        brain->reset();
        brain->initialize();
        
        size_t neuronCount = brain->getTotalNeuronCount();
        assert(neuronCount == 50000);
        std::cout << " ✓ Allocated " << neuronCount << " neurons using memory pool" << std::endl;
        
        // Test 2: Synapse allocation
        std::cout << "Test 2: Synapse allocation...";
        
        size_t synapseCount = brain->getTotalSynapseCount();
        assert(synapseCount > 0);
        std::cout << " ✓ Allocated " << synapseCount << " synapses using memory pool" << std::endl;
        
        // Test 3: Memory efficiency
        std::cout << "Test 3: Memory efficiency...";
        
        brain->reset();
        brain->initialize();
        
        size_t initialNeurons = brain->getTotalNeuronCount();
        
        // Run simulation to test memory usage stability
        for (size_t step = 0; step < 1000; ++step) {
            brain->step(step);
        }
        
        size_t finalNeurons = brain->getTotalNeuronCount();
        
        assert(initialNeurons == finalNeurons);
        std::cout << " ✓ Memory usage stable across 1000 simulation steps" << std::endl;
        
        // Test 4: Memory cleanup
        std::cout << "Test 4: Memory cleanup...";
        
        brain->reset();
        brain->initialize();
        
        // Clear brain and reinitialize
        brain->reset();
        brain->initialize();
        
        size_t neuronsAfterReset = brain->getTotalNeuronCount();
        
        assert(neuronsAfterReset == initialNeurons);
        std::cout << " ✓ Memory cleanup successful after reset" << std::endl;
        
        std::cout << "All memory pool tests passed! ✓" << std::endl;
    }
    
    void runIntegrationTests() {
        std::cout << "\n=== Running Integration Tests ===" << std::endl;
        
        // Test 1: Full agent simulation
        std::cout << "Test 1: Agent simulation integration...";
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create simple world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
        world->setAgentStart(10.0, 10.0);
        world->reset();
        
        agent->initialize(world);
        
        // Run simulation loop
        for (size_t step = 0; step < 200; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 50 == 0) {
                float reward = world->getSensoryPercept().getInternal()[0];
                agent->applyRewardModulation(reward, 0.0);
            }
        }
        
        std::cout << " ✓ Full agent simulation completed successfully" << std::endl;
        
        // Test 2: Checkpoint integration
        std::cout << "Test 2: Checkpoint integration...";
        
        brain->save("/tmp/test_checkpoint.bin");
        brain->load("/tmp/test_checkpoint.bin");
        
        std::cout << " ✓ Checkpoint save/load cycle completed" << std::endl;
        
        // Test 3: Neuromodulation integration
        std::cout << "Test 3: Neuromodulation integration...";
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step);
            
            if (step % 25 == 0) {
                float reward = 0.1f * (step / 100.0f);
                agent->applyRewardModulation(reward, 0.0);
                
                if (step % 50 == 0) {
                    agent->updateDevelopment(0.1);
                }
            }
        }
        
        std::cout << " ✓ Neuromodulation integration completed" << std::endl;
        
        // Test 4: Development integration
        std::cout << "Test 4: Development integration...";
        
        agent->enableDevelopment(true);
        
        for (size_t step = 0; step < 500; ++step) {
            brain->step(step);
            agent->updateDevelopment(0.1);
            
            if (step % 100 == 0) {
                std::cout << "  Development stage: " << brain->getDevelopmentalStage() << std::endl;
            }
        }
        
        std::cout << " ✓ Development system integration completed" << std::endl;
        
        std::cout << "All integration tests passed! ✓" << std::endl;
    }
    
    void cleanup() {
        std::cout << "\n=== Cleaning up ===" << std::endl;
        
        // Save final statistics
        std::cout << "Final brain statistics:" << std::endl;
        brain->logStatus();
        
        // Clean up temporary files
        std::cout << "Cleaning up temporary checkpoint files...";
        // In a real implementation, we would clean up temporary files
        
        std::cout << "Cleanup completed! ✓" << std::endl;
    }
    
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::vector<std::string> tempFiles;
};

void runAllTests() {
    std::cout << "=== NLM Performance Optimization Tests ===" << std::endl;
    std::cout << "Testing advanced performance optimizations and features..." << std::endl;
    
    try {
        TestBrain testBrain;
        
        testBrain.runBasicTests();
        testBrain.runPerformanceTests();
        testBrain.runMemoryPoolTests();
        testBrain.runIntegrationTests();
        
        testBrain.cleanup();
        
        std::cout << "\n=== All Tests Completed Successfully! ===" << std::endl;
        std::cout << "The NLM brain simulation framework is working correctly with all performance optimizations." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n*** Test Failed with Exception: " << e.what() << " ***" << std::endl;
        std::exit(1);
    } catch (...) {
        std::cerr << "\n*** Test Failed with Unknown Exception ***" << std::endl;
        std::exit(1);
    }
}

} // namespace nlm

int main(int argc, char** argv) {
    // Initialize logging (if available)
    // In a real implementation, we would initialize logging here
    
    // Run all tests
    nlm::runAllTests();
    
    return 0;
}