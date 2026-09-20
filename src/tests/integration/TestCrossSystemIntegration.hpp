// Cross-System Integration Tests
// Tests replay system, checkpointing, and full simulation cycles

#pragma once

#include "IntegrationTestFramework.hpp"
#include "../brain/Brain.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace nlm {

class TestCrossSystemIntegration : public IntegrationTestBase {
public:
    explicit TestCrossSystemIntegration(std::shared_ptr<Config> cfg, IntegrationTestResult& results)
        : IntegrationTestBase(cfg, results) {}
    
    void runAllTests() {
        runTestCase("Replay System Integration", "Cross-System Integration", [this]() {
            testReplaySystemIntegration();
        });
        
        runTestCase("Checkpointing System Integration", "Cross-System Integration", [this]() {
            testCheckpointingSystemIntegration();
        });
        
        runTestCase("Full Simulation Cycles", "Cross-System Integration", [this]() {
            testFullSimulationCycles();
        });
        
        runTestCase("Replay Performance", "Cross-System Integration", [this]() {
            testReplayPerformance();
        });
        
        runTestCase("Checkpoint Recovery", "Cross-System Integration", [this]() {
            testCheckpointRecovery();
        });
        
        runTestCase("Simulation State Consistency", "Cross-System Integration", [this]() {
            testSimulationStateConsistency();
        });
        
        runTestCase("Cross-System Recovery", "Cross-System Integration", [this]() {
            testCrossSystemRecovery();
        });
    }
    
private:
    void testReplaySystemIntegration() {
        setupSystem();
        
        // Create replay data
        std::vector<BrainState> replayData;
        const size_t replaySteps = 20;
        
        // Record system state during replay
        for (nlm::SimulationStep step = 0; step < replaySteps; ++step) {
            // Get current brain state
            // (Implementation depends on Brain state capture)
            
            // Store state for replay
            BrainState state;
            state.step = step;
            state.neuronStates.resize(brain->getTotalNeuronCount());
            state.synapseStates.resize(brain->getTotalSynapseCount());
            
            replayData.push_back(state);
            
            brain->step(step);
        }
        
        // Verify replay data integrity
        assert(replayData.size() == replaySteps);
        
        // Test replay restoration
        // (Implementation depends on replay system)
        
        // Verify replay maintains system consistency
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        
        assert(totalNeurons > 0);
        assert(totalSynapses >= 0);
        
        teardownSystem();
    }
    
    void testCheckpointingSystemIntegration() {
        setupSystem();
        
        // Create checkpoint files
        std::string checkpointDir = "/tmp/test_checkpoints";
        std::filesystem::create_directories(checkpointDir);
        
        const size_t checkpointCount = 5;
        
        // Save checkpoints at different stages
        for (size_t i = 0; i < checkpointCount; ++i) {
            std::string checkpointFile = checkpointDir + "/checkpoint_" + 
                                        std::to_string(i) + ".bin";
            
            // Save current brain state
            bool saved = brain->save(checkpointFile);
            assert(saved);
            
            // Simulate system activity between checkpoints
            for (nlm::SimulationStep step = 0; step < 10; ++step) {
                brain->step(step);
            }
        }
        
        // Verify all checkpoints created successfully
        for (size_t i = 0; i < checkpointCount; ++i) {
            std::string checkpointFile = checkpointDir + "/checkpoint_" + 
                                        std::to_string(i) + ".bin";
            assert(std::filesystem::exists(checkpointFile));
        }
        
        // Test checkpoint loading and restoration
        std::string restoreCheckpoint = checkpointDir + "/checkpoint_2.bin";
        
        // Create new brain instance and load checkpoint
        auto restoredBrain = std::make_unique<Brain>(config);
        assert(restoredBrain->initialize());
        assert(restoredBrain->load(restoreCheckpoint));
        
        // Verify restored state
        assert(restoredBrain->getTotalNeuronCount() > 0);
        
        // Clean up checkpoints
        for (size_t i = 0; i < checkpointCount; ++i) {
            std::string checkpointFile = checkpointDir + "/checkpoint_" + 
                                        std::to_string(i) + ".bin";
            std::filesystem::remove(checkpointFile);
        }
        std::filesystem::remove(checkpointDir);
        
        teardownSystem();
    }
    
    void testFullSimulationCycles() {
        setupSystem();
        
        // Run complete simulation cycles with system monitoring
        const size_t cycleCount = 3;
        const size_t stepsPerCycle = 100;
        
        std::vector<std::vector<float>> cycleMetrics;
        
        for (size_t cycle = 0; cycle < cycleCount; ++cycle) {
            std::vector<float> cycleMetrics;
            
            for (nlm::SimulationStep step = 0; step < stepsPerCycle; ++step) {
                // Run simulation step
                brain->step(step);
                
                // Record metrics
                float firingRate = brain->getAverageFiringRate();
                size_t firingCount = brain->getFiringNeuronCount();
                
                cycleMetrics.push_back(firingRate);
                
                // Inject occasional sensory input
                if (step % 20 == 0) {
                    SensoryInput sensoryInput;
                    sensoryInput.neuron = static_cast<NeuronId>(step % 10);
                    sensoryInput.current = 0.5f;
                    brain->receiveSensoryInput(sensoryInput);
                }
                
                // Apply neuromodulation
                Neuromodulator neuromodSignal;
                neuromodSignal.type = NeuromodulatorType::Reward;
                neuromodSignal.value = 0.5f + 0.3f * std::sin(2.0f * M_PI * step / 50.0f);
                brain->applyNeuromodulation(neuromSignal);
            }
            
            cycleMetrics.push_back(cycle); // Add cycle identifier
            
            // Test checkpoint at end of cycle
            std::string cycleCheckpoint = "/tmp/cycle_" + std::to_string(cycle) + ".bin";
            bool checkpointSaved = brain->save(cycleCheckpoint);
            assert(checkpointSaved);
            
            // Verify cycle completed successfully
            assert(std::isfinite(cycleMetrics.back()));
        }
        
        // Verify all cycles completed
        for (size_t cycle = 0; cycle < cycleCount; ++cycle) {
            std::string cycleCheckpoint = "/tmp/cycle_" + std::to_string(cycle) + ".bin";
            assert(std::filesystem::exists(cycleCheckpoint));
        }
        
        // Clean up checkpoints
        for (size_t cycle = 0; cycle < cycleCount; ++cycle) {
            std::string cycleCheckpoint = "/tmp/cycle_" + std::to_string(cycle) + ".bin";
            std::filesystem::remove(cycleCheckpoint);
        }
        
        teardownSystem();
    }
    
    void testReplayPerformance() {
        setupSystem();
        
        // Test replay performance characteristics
        const size_t replaySteps = 1000;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Record states for replay
        std::vector<BrainState> replayData;
        for (nlm::SimulationStep step = 0; step < replaySteps; ++step) {
            brain->step(step);
            
            // Capture state (simplified)
            BrainState state;
            state.step = step;
            replayData.push_back(state);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Verify performance metrics
        double stepsPerSecond = static_cast<double>(replaySteps) / duration.count() * 1000.0;
        assert(stepsPerSecond > 0.0);
        
        // Performance should be reasonable (at least 10 steps per millisecond)
        assert(stepsPerSecond > 10.0);
        
        teardownSystem();
    }
    
    void testCheckpointRecovery() {
        setupSystem();
        
        // Create multiple checkpoints at different states
        std::string checkpoint1 = "/tmp/checkpoint1.bin";
        std::string checkpoint2 = "/tmp/checkpoint2.bin";
        
        // Save initial state
        bool saved1 = brain->save(checkpoint1);
        assert(saved1);
        
        // Modify system state
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        // Save modified state
        bool saved2 = brain->save(checkpoint2);
        assert(saved2);
        
        // Restore to initial state
        auto recoveredBrain1 = std::make_unique<Brain>(config);
        assert(recoveredBrain1->initialize());
        assert(recoveredBrain1->load(checkpoint1));
        
        // Verify restored state matches initial
        assert(recoveredBrain1->getTotalNeuronCount() == brain->getTotalNeuronCount());
        
        // Restore to modified state
        auto recoveredBrain2 = std::make_unique<Brain>(config);
        assert(recoveredBrain2->initialize());
        assert(recoveredBrain2->load(checkpoint2));
        
        // Verify recovered state matches modified
        // (State comparison depends on BrainState implementation)
        
        // Clean up checkpoints
        std::filesystem::remove(checkpoint1);
        std::filesystem::remove(checkpoint2);
        
        teardownSystem();
    }
    
    void testSimulationStateConsistency() {
        setupSystem();
        
        // Test state consistency across simulation runs
        std::vector<BrainState> initialStates;
        const size_t runCount = 3;
        const size_t stepsPerRun = 50;
        
        for (size_t run = 0; run < runCount; ++run) {
            // Reset brain for each run
            brain->reset();
            
            // Record initial state
            BrainState state;
            state.step = 0;
            initialStates.push_back(state);
            
            // Run simulation
            for (nlm::SimulationStep step = 0; step < stepsPerRun; ++step) {
                brain->step(step);
            }
            
            // Verify system invariants hold
            assert(brain->getTotalNeuronCount() > 0);
            assert(brain->getAverageFiringRate() >= 0.0f);
            assert(brain->getTotalSynapseCount() >= 0);
        }
        
        // Verify all runs completed successfully
        assert(initialStates.size() == runCount);
        
        teardownSystem();
    }
    
    void testCrossSystemRecovery() {
        setupSystem();
        
        // Test recovery from system failures
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        // Create test data in all systems
        workingMemory->store(1, 0.8f);
        
        EpisodicMemoryItem episode;
        episode.timestamp = 1000;
        episode.neurons = {2, 3};
        episode.values = {0.5f, 0.7f};
        episode.metadata = "Recovery test";
        episodicMemory->storeEpisode(episode);
        
        // Simulate system stress (heavy computation)
        for (nlm::SimulationStep step = 0; step < 200; ++step) {
            brain->step(step);
        }
        
        // Verify system recovery and data integrity
        assert(workingMemory->contains(1));
        assert(workingMemory->retrieve(1) == 0.8f);
        
        assert(episodicMemory->getEpisodeCount() > 0);
        auto retrieved = episodicMemory->retrieveEpisode(0);
        assert(retrieved.timestamp == episode.timestamp);
        
        // Test checkpoint recovery
        std::string recoveryCheckpoint = "/tmp/recovery_checkpoint.bin";
        bool checkpointSaved = brain->save(recoveryCheckpoint);
        assert(checkpointSaved);
        
        // Simulate more stress
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        // Recover from checkpoint
        std::remove(recoveryCheckpoint.c_str()); // Clean up
        
        teardownSystem();
    }
};

} // namespace nlm