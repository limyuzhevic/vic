# NLM Testing Framework Guide

**Comprehensive Guide to Testing Neural Learning Machine Components**

## Overview

The NLM testing framework provides a structured approach to validating neural computation, plasticity rules, memory systems, and integration scenarios. This guide covers unit tests, integration tests, performance benchmarks, and testing best practices.

## Testing Philosophy

### Core Principles

1. **Real Neural Computation Testing**: All tests validate actual LIF neuron dynamics and spike propagation
2. **Plasticity Validation**: Comprehensive testing of STDP, Hebbian, and structural plasticity
3. **Integration Verification**: End-to-end testing of complete brain systems
4. **Performance Benchmarking**: Systematic performance measurement
5. **Regression Testing**: Automated validation of existing functionality

### Test Categories

1. **Unit Tests**: Individual component testing
2. **Integration Tests**: System-level interaction testing
3. **End-to-End Tests**: Complete scenario validation
4. **Performance Tests**: Speed and resource usage measurement
5. **Stress Tests**: System limits and boundary conditions

## Test Structure

### Basic Test Template

```cpp
// tests/test_component.cpp
#include "Component.hpp"
#include <gtest/gtest.h>
#include <memory>

namespace nlm {

class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
        component_ = std::make_unique<Component>();
        config_ = std::make_shared<Config>();
        config_->set("test_param", 1.0f);
        component_->configure(config_);
    }
    
    void TearDown() override {
        // Clean up test fixtures
        component_.reset();
        config_.reset();
    }
    
    std::unique_ptr<Component> component_;
    std::shared_ptr<Config> config_;
};

TEST_F(ComponentTest, BasicInitialization) {
    EXPECT_TRUE(component_->isInitialized());
    EXPECT_EQ(component_->getParameter("test_param"), 1.0f);
}

TEST_F(ComponentTest, Functionality) {
    // Test actual functionality
    bool result = component_->processInput(testData_);
    EXPECT_TRUE(result);
    EXPECT_EQ(component_->getOutput(), expectedOutput_);
}

TEST_F(ComponentTest, ErrorHandling) {
    // Test error conditions
    EXPECT_THROW(component_->processInvalidInput(), std::invalid_argument);
}

} // namespace nlm
```

### Integration Test Template

```cpp
// tests/test_integration.cpp
#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace nlm {

class BrainIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create brain with specific configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        
        brain_ = std::make_shared<Brain>(config);
        ASSERT_TRUE(brain_->initialize());
        
        // Initialize memory systems
        brain_->getWorkingMemory()->initialize(brain_.get());
    }
    
    void TearDown() override {
        brain_.reset();
    }
    
    std::shared_ptr<Brain> brain_;
};

TEST_F(BrainIntegrationTest, NeuralConnectivity) {
    // Test that neurons can communicate
    auto* region1 = brain_->getRegion(RegionId(1));
    auto* region2 = brain_->getRegion(RegionId(2));
    
    ASSERT_NE(region1, nullptr);
    ASSERT_NE(region2, nullptr);
    
    // Inject current and check propagation
    region1->injectCurrentToNeurons(NeuronType::Internal, 50.0f);
    brain_->step(0, 0.001);
    
    // Verify activity
    EXPECT_GT(brain_->getTotalSpikeCount(), 0);
}

TEST_F(BrainIntegrationTest, PlasticityIntegration) {
    // Test integration of plasticity mechanisms
    LearningExperiment experiment(brain_, 42);
    
    experiment.recordInitialState();
    
    // Enable all plasticity
    enableAllPlasticity(brain_);
    
    // Apply learning
    applyLearningStimulus(brain_, 1000);
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Verify learning occurred
    EXPECT_GT(brain_->getTotalSynapseCount(), 10);
}

TEST_F(BrainIntegrationTest, MemorySystemIntegration) {
    // Test memory system integration
    WorkingMemory* workingMemory = brain_->getWorkingMemory();
    
    // Create test memory content
    MemoryContent content;
    content.type = MemoryType::Working;
    content.neuronPattern = generatePattern(100);
    content.associationStrength = 0.8f;
    
    // Store memory
    workingMemory->store(content);
    
    // Retrieve memory
    MemoryContent retrieved = workingMemory->retrieve(content.patternHash);
    
    EXPECT_EQ(retrieved.patternHash, content.patternHash);
    EXPECT_FLOAT_EQ(retrieved.associationStrength, content.associationStrength);
}

} // namespace nlm
```

## Testing Components

### Neuron Testing

#### LIF Dynamics Test

```cpp
// tests/test_neuron_lif.cpp
#include "brain/Neuron.hpp"
#include <gtest/gtest.h>
#include <cmath>

namespace nlm {

class LIFNeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        neuron_ = std::make_unique<Neuron>(NeuronId(1));
        neuron_->initializeRandom(randomGenerator_);
        neuron_->setType(NeuronType::Internal);
    }
    
    NeuronId injectCurrentAndSimulate(float current, int steps) {
        neuron_->injectCurrent(current);
        
        for (int i = 0; i < steps; ++i) {
            neuron_->step(i * 0.001);
        }
        
        return neuron_->getId();
    }
    
    std::unique_ptr<Neuron> neuron_;
    RandomGenerator randomGenerator_{42};
};

TEST_F(LIFNeuronTest, RestingState) {
    NeuronState state = neuron_->getState();
    EXPECT_FLOAT_EQ(state.membranePotential, state.restingPotential);
    EXPECT_EQ(state.firingState, FiringState::Resting);
}

TEST_F(LIFNeuronTest, SubthresholdIntegration) {
    NeuronId id = injectCurrentAndSimulate(10.0f, 10);
    NeuronState state = neuron_->getState();
    
    EXPECT_GT(state.membranePotential, neuron_->getRestingPotential());
    EXPECT_EQ(state.firingState, FiringState::Active);
    EXPECT_FALSE(neuron_->checkThreshold());
}

TEST_F(LIFNeuronTest, SuprathresholdResponse) {
    NeuronId id = injectCurrentAndSimulate(100.0f, 50);
    NeuronState state = neuron_->getState();
    
    EXPECT_TRUE(neuron_->checkThreshold());
    EXPECT_TRUE(neuron_->getState().firingState == FiringState::Active);
    EXPECT_GT(neuron_->getLastSpikeTime(), 0.0f);
}

TEST_F(LIFNeuronTest, SpikeFrequencyAdaptation) {
    // Test that sustained input leads to decreasing firing rate
    neuron_->injectCurrent(50.0f);
    
    float firstRate = 0.0f;
    float lastRate = 0.0f;
    
    for (int step = 0; step < 200; ++step) {
        neuron_->step(step * 0.001);
        if (step == 10) firstRate = neuron_->getFiringRate();
        if (step == 190) lastRate = neuron_->getFiringRate();
    }
    
    // Firing rate should decrease due to adaptation
    EXPECT_LE(lastRate, firstRate);
}

TEST_F(LIFNeuronTest, RefractoryPeriod) {
    // Test refractory period behavior
    neuron_->injectCurrent(200.0f);
    neuron_->step(0);
    
    // Should be in refractory state
    EXPECT_EQ(neuron_->getState().firingState, FiringState::Refractory);
    
    // Inject current during refractory period
    float membraneBefore = neuron_->getMembranePotential();
    neuron_->injectCurrent(200.0f);
    
    // Membrane potential should not change significantly
    float membraneAfter = neuron_->getMembranePotential();
    EXPECT_NEAR(membraneBefore, membraneAfter, 0.1f);
}

TEST_F(LIFNeuronTest, InputIntegration) {
    // Test different input types
    neuron_->injectCurrent(30.0f);
    neuron_->receiveExcitatoryInput(10.0f);
    neuron_->receiveInhibitoryInput(5.0f);
    neuron_->receiveModulatoryInput(2.0f);
    
    // Net input should be positive
    float synapticInput = neuron_->getSynapticInput();
    EXPECT_GT(synapticInput, 0.0f);
}

TEST_F(LIFNeuronTest, HistoryTracking) {
    // Test spike history tracking
    for (int i = 0; i < 150; ++i) {
        neuron_->injectCurrent(100.0f);
        neuron_->step(i * 0.001);
    }
    
    // Check that history is bounded
    const auto& history = neuron_->getSpikeHistory();
    EXPECT_LE(history.size(), 100);  // MAX_SPIKE_HISTORY
}

} // namespace nlm
```

#### Plasticity Testing

```cpp
// tests/test_plasticity.cpp
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace nlm {

class STDPTest : public ::testing::Test {
protected:
    void SetUp() override {
        stdp_ = std::make_unique<STDP>();
        synapse_ = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
        synapse_->setWeight(0.5f);
    }
    
    void recordPrePostSpikes() {
        // Create spike timing pairs
        preSpikes_ = {100.0f, 300.0f, 500.0f};   // Pre-synaptic spikes
        postSpikes_ = {150.0f, 350.0f, 550.0f};  // Post-synaptic spikes (pre < post)
    }
    
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Synapse> synapse_;
    std::vector<float> preSpikes_;
    std::vector<float> postSpikes_;
};

TEST_F(STDPTest, Potentiation) {
    recordPrePostSpikes();
    
    float initialWeight = synapse_->getWeight();
    stdp_->update(synapse_.get(), preSpikes_, postSpikes_, 0.001);
    
    float finalWeight = synapse_->getWeight();
    EXPECT_GT(finalWeight, initialWeight);  // LTP should increase weight
}

TEST_F(STDPTest, Depression) {
    // Reverse spike timing for LTD
    preSpikes_ = {100.0f, 300.0f, 500.0f};
    postSpikes_ = {50.0f, 250.0f, 450.0f};   // Post < pre
    
    float initialWeight = synapse_->getWeight();
    stdp_->update(synapse_.get(), preSpikes_, postSpikes_, 0.001);
    
    float finalWeight = synapse_->getWeight();
    EXPECT_LT(finalWeight, initialWeight);  // LTD should decrease weight
}

TEST_F(STDPTest, WeightBounds) {
    // Test that weights stay within bounds
    synapse_->setWeight(0.8f);
    
    // Force large potentiation
    preSpikes_ = {100.0f};
    postSpikes_ = {150.0f};
    stdp_->update(synapse_.get(), preSpikes_, postSpikes_, 0.001);
    
    float weight = synapse_->getWeight();
    EXPECT_LE(weight, 1.0f);  // STDP max weight
    EXPECT_GE(weight, -1.0f); // STDP min weight
}

TEST_F(STDPTest, TimeConstant) {
    float initialWeight = synapse_->getWeight();
    
    // Long time constant
    stdp_->setTimeConstant(100.0f);
    preSpikes_ = {100.0f};
    postSpikes_ = {250.0f};  // Δt = 150ms, should have small effect with tau=100
    stdp_->update(synapse_.get(), preSpikes_, postSpikes_, 0.001);
    
    float weightWithLongTau = synapse_->getWeight();
    
    // Short time constant
    stdp_->setTimeConstant(10.0f);
    synapse_->setWeight(initialWeight);
    stdp_->update(synapse_.get(), preSpikes_, postSpikes_, 0.001);
    
    float weightWithShortTau = synapse_->getWeight();
    
    // Short tau should produce larger changes
    float diffLong = std::abs(weightWithLongTau - initialWeight);
    float diffShort = std::abs(weightWithShortTau - initialWeight);
    EXPECT_GT(diffShort, diffLong);
}

} // namespace nlm
```

### Memory System Testing

```cpp
// tests/test_memory_systems.cpp
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include <gtest/gtest.h>

namespace nlm {

class WorkingMemoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        memory_ = std::make_unique<WorkingMemory>();
        memory_->setCapacity(100);
        memory_->setDecayRate(0.95f);
        
        // Mock brain pointer
        brain_ = nullptr;
        memory_->initialize(brain_);
    }
    
    MemoryContent createTestContent() {
        MemoryContent content;
        content.type = MemoryType::Working;
        content.patternHash = 123456;
        content.associationStrength = 0.7f;
        content.recallProbability = 0.9f;
        content.activationLevel = 0.8f;
        return content;
    }
    
    std::unique_ptr<WorkingMemory> memory_;
    Brain* brain_;
};

TEST_F(WorkingMemoryTest, StorageCapacity) {
    // Fill memory to capacity
    for (size_t i = 0; i < 150; ++i) {
        MemoryContent content = createTestContent();
        content.patternHash = i;  // Unique pattern
        memory_->store(content);
    }
    
    // Memory should be at capacity
    EXPECT_EQ(memory_->getMemoryLoad(), 100);
}

TEST_F(WorkingMemoryTest, DecayMechanism) {
    MemoryContent content = createTestContent();
    memory_->store(content);
    
    float initialStrength = content.associationStrength;
    
    // Step through time
    for (int step = 0; step < 100; ++step) {
        memory_->update(0.001f);
    }
    
    // Content should decay
    MemoryContent retrieved = memory_->retrieve(content.patternHash);
    EXPECT_LT(retrieved.associationStrength, initialStrength);
}

TEST_F(WorkingMemoryTest, Competition) {
    // Store multiple similar patterns
    MemoryContent content1 = createTestContent();
    content1.patternHash = 1;
    content1.associationStrength = 0.9f;
    
    MemoryContent content2 = createTestContent();
    content2.patternHash = 2;
    content2.associationStrength = 0.8f;
    
    memory_->store(content1);
    memory_->store(content2);
    
    // Recall should favor stronger association
    MemoryContent recalled = memory_->retrieve(1);
    EXPECT_EQ(recalled.patternHash, 1);
}

} // namespace nlm
```

## Integration Testing

### Brain Integration Test Suite

```cpp
// tests/test_brain_integration.cpp
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace nlm {

class BrainIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create realistic brain configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000);
        config->set("region_count", 4);
        config->set("connection_probability", 0.08f);
        config->set("random_seed", 42);
        
        brain_ = std::make_shared<Brain>(config);
        ASSERT_TRUE(brain_->initialize());
        
        // Verify basic brain structure
        EXPECT_GE(brain_->getTotalNeuronCount(), 1500);
        EXPECT_GE(brain_->getTotalSynapseCount(), 5000);
        
        // Collect neurons by type
        collectNeuronTypes(brain_.get());
    }
    
    void TearDown() override {
        brain_.reset();
    }
    
    void collectNeuronTypes(Brain* brain) {
        for (size_t i = 0; i < 4; ++i) {
            auto* region = brain->getRegion(RegionId(i + 1));
            if (region) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    switch (neuron->getType()) {
                        case NeuronType::Sensory:
                            sensoryNeurons_.push_back(neuron);
                            break;
                        case NeuronType::Motor:
                            motorNeurons_.push_back(neuron);
                            break;
                        case NeuronType::Internal:
                            internalNeurons_.push_back(neuron);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    
    std::shared_ptr<Brain> brain_;
    std::vector<Neuron*> sensoryNeurons_;
    std::vector<Neuron*> motorNeurons_;
    std::vector<Neuron*> internalNeurons_;
};

TEST_F(BrainIntegrationTest, BasicConnectivity) {
    // Test that all neurons can communicate
    float totalActivity = 0.0f;
    
    for (auto* neuron : internalNeurons_) {
        neuron->injectCurrent(10.0f);
    }
    
    // Simulate for reasonable duration
    for (int step = 0; step < 500; ++step) {
        brain_->step(step, step * 0.001);
        totalActivity += brain_->getFiringNeuronCount();
    }
    
    // Verify widespread activity
    EXPECT_GT(totalActivity, 1000);
    EXPECT_GT(brain_->getTotalSpikeCount(), 500);
}

TEST_F(BrainIntegrationTest, PlasticityIntegration) {
    // Test integration of all plasticity systems
    float initialTotalWeight = 0.0f;
    
    // Collect initial weights
    for (auto& region : brain_->getRegions()) {
        for (auto& synapse : region->getSynapses()) {
            initialTotalWeight += synapse->getWeight();
        }
    }
    
    // Enable plasticity
    enablePlasticity(brain_.get());
    
    // Apply learning experience
    LearningExperiment experiment(brain_, 42);
    experiment.recordInitialState();
    
    // Simulate learning
    for (int step = 0; step < 1000; ++step) {
        // Create sensory input
        brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        brain_->step(step, step * 0.001);
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Verify learning occurred
    float finalTotalWeight = 0.0f;
    for (auto& region : brain_->getRegions()) {
        for (auto& synapse : region->getSynapses()) {
            finalTotalWeight += synapse->getWeight();
        }
    }
    
    // Weight should change (not required to be specific, just different)
    EXPECT_NE(finalTotalWeight, initialTotalWeight);
}

TEST_F(BrainIntegrationTest, MemoryPlasticityIntegration) {
    // Test that memory systems affect plasticity
    WorkingMemory* workingMemory = brain_->getWorkingMemory();
    
    // Create memory of recent activity
    MemoryContent content;
    content.type = MemoryType::Working;
    content.patternHash = 999;
    content.associationStrength = 0.95f;
    content.recallProbability = 0.8f;
    
    workingMemory->store(content);
    
    // Enable plasticity
    enablePlasticity(brain_.get());
    
    // Simulate with memory present
    for (int step = 0; step < 500; ++step) {
        brain_->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        
        // Check memory and modulate plasticity based on it
        if (workingMemory->shouldReinforceMemory(content.patternHash)) {
            reinforceRelatedPlasticity(brain_.get(), content.patternHash);
        }
        
        brain_->step(step, step * 0.001);
    }
    
    // Verify that plasticity was modulated by memory
    // (Implementation-dependent check)
    EXPECT_TRUE(memory_->getConsolidationStrength() > 0.5f);
}

TEST_F(BrainIntegrationTest, NeuromodulationIntegration) {
    // Test neuromodulation integration
    Dopamine* dopamine = brain_->getNeuromodulationSystem()->getDopamine();
    Curiosity* curiosity = brain_->getNeuromodulationSystem()->getCuriosity();
    
    ASSERT_NE(dopamine, nullptr);
    ASSERT_NE(curiosity, nullptr);
    
    // Record baseline
    float baselineDopamine = dopamine->getLevel();
    float baselineCuriosity = curiosity->getLevel();
    
    // Apply reward
    applyReward(brain_.get(), 1.0f);
    
    // Check neuromodulation response
    EXPECT_GT(dopamine->getLevel(), baselineDopamine);
    EXPECT_GT(curiosity->getLevel(), baselineCuriosity);
    
    // Apply punishment
    applyPunishment(brain_.get(), 0.5f);
    
    // Dopamine should decrease
    EXPECT_LT(dopamine->getLevel(), baselineDopamine);
}

TEST_F(BrainIntegrationTest, DevelopmentPlasticity) {
    // Test developmental changes in plasticity
    DevelopmentalStage initialStage = getDevelopmentalStage(brain_.get());
    
    // Simulate development
    for (int step = 0; step < 10000; ++step) {
        brain_->step(step, step * 0.001);
        
        // Update development
        updateDevelopment(brain_.get(), step * 0.001);
    }
    
    DevelopmentalStage finalStage = getDevelopmentalStage(brain_.get());
    
    // Development should progress
    EXPECT_GE(finalStage, initialStage);
    
    // Plasticity rates should change with development
    float initialPlasticityRate = getPlasticityRate(brain_.get(), initialStage);
    float finalPlasticityRate = getPlasticityRate(brain_.get(), finalStage);
    
    // Plasticity typically decreases with age
    EXPECT_LE(finalPlasticityRate, initialPlasticityRate);
}

} // namespace nlm
```

## Performance Testing

### Benchmark Framework

```cpp
// benchmarks/brain_benchmarks.cpp
#include "brain/Brain.hpp"
#include <benchmark/benchmark.h>
#include <thread>
#include <chrono>

namespace nlm {

class BrainBenchmarks {
public:
    static void BM_StepPerformance(benchmark::State& state) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", state.range(0));
        config->set("region_count", state.range(1));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (auto _ : state) {
            brain->step(state.iterations, 0.001);
        }
        
        state.SetItems(state.iterations * brain->getTotalNeuronCount());
    }
    
    static void BM_PlasticityUpdate(benchmark::State& state) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", state.range(0));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (auto _ : state) {
            // Trigger plasticity update
            brain->triggerPlasticity();
        }
    }
    
    static void BM_MemoryAccess(benchmark::State& state) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", state.range(0));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto workingMemory = brain->getWorkingMemory();
        
        for (auto _ : state) {
            // Memory operations
            MemoryContent content;
            content.patternHash = state.iterations;
            content.associationStrength = 0.5f;
            
            workingMemory->store(content);
            workingMemory->retrieve(content.patternHash);
        }
    }
};

// Register benchmarks
BENCHMARK_REGISTER_F(BrainBenchmarks, BM_StepPerformance)
    ->ArgPair(100, 1)
    ->ArgPair(1000, 2)
    ->ArgPair(5000, 4)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(BrainBenchmarks, BM_PlasticityUpdate)
    ->Arg(100)
    ->Arg(1000)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_REGISTER_F(BrainBenchmarks, BM_MemoryAccess)
    ->Arg(100)
    ->Arg(1000)
    ->Unit(benchmark::kMillisecond);

} // namespace nlm
```

### Stress Testing

```cpp
// tests/test_stress.cpp
#include "brain/Brain.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace nlm {

class StressTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create brain at system limits
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 10000);  // Large brain
        config->set("region_count", 8);
        config->set("connection_probability", 0.05f);
        
        brain_ = std::make_shared<Brain>(config);
        ASSERT_TRUE(brain_->initialize());
    }
    
    void TearDown() override {
        brain_.reset();
    }
    
    std::shared_ptr<Brain> brain_;
};

TEST_F(StressTest, MemoryStress) {
    // Test memory system under load
    WorkingMemory* workingMemory = brain_->getWorkingMemory();
    
    // Rapid memory storage and retrieval
    for (int i = 0; i < 5000; ++i) {
        MemoryContent content;
        content.type = MemoryType::Working;
        content.patternHash = i;
        content.associationStrength = 0.5f + (i % 10) * 0.05f;
        
        workingMemory->store(content);
        
        // Occasionally retrieve
        if (i % 10 == 0) {
            workingMemory->retrieve(content.patternHash);
        }
        
        // Update memory decay
        workingMemory->update(0.001f);
    }
    
    // Memory system should not crash
    EXPECT_TRUE(workingMemory->getMemoryLoad() >= 0);
}

TEST_F(StressTest, PlasticityStress) {
    // Test plasticity system under load
    enablePlasticity(brain_.get());
    
    // High-frequency spike generation
    for (int step = 0; step < 2000; ++step) {
        // Inject current into many neurons
        for (auto& region : brain_->getRegions()) {
            region->injectCurrentToNeurons(NeuronType::Internal, 20.0f);
        }
        
        // Step simulation
        brain_->step(step, step * 0.001);
        
        // Trigger plasticity updates periodically
        if (step % 10 == 0) {
            brain_->triggerPlasticity();
        }
    }
    
    // Plasticity system should remain stable
    float totalWeightChange = calculateTotalWeightChange(brain_.get());
    EXPECT_LT(totalWeightChange, 1000.0f);  // Reasonable bound
}

TEST_F(StressTest, ConcurrencyStress) {
    // Test thread safety
    std::vector<std::thread> threads;
    int numThreads = 4;
    int stepsPerThread = 500;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([this, t, stepsPerThread]() {
            for (int step = 0; step < stepsPerThread; ++step) {
                brain_->step(t * stepsPerThread + step, (t * stepsPerThread + step) * 0.001);
            }
        });
    }
    
    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Brain should remain in valid state
    EXPECT_GE(brain_->getTotalSpikeCount(), 0);
}

} // namespace nlm
```

## Testing Best Practices

### 1. Deterministic Testing

```cpp
// Use fixed random seed for reproducible tests
TEST(DeterministicTest, ConsistentResults) {
    auto config1 = std::make_shared<Config>();
    config1->set("random_seed", 12345);
    
    auto config2 = std::make_shared<Config>();
    config2->set("random_seed", 12345);
    
    auto brain1 = std::make_shared<Brain>(config1);
    auto brain2 = std::make_shared<Brain>(config2);
    
    brain1->initialize();
    brain2->initialize();
    
    // Both brains should have identical structure
    EXPECT_EQ(brain1->getTotalNeuronCount(), brain2->getTotalNeuronCount());
    EXPECT_EQ(brain1->getTotalSynapseCount(), brain2->getTotalSynapseCount());
}
```

### 2. Memory Management Testing

```cpp
TEST(MemoryManagement, NoLeaks) {
    // Use memory leak detector if available
    #ifdef ADDRESS_SANITIZER
    // MemorySanitizer will catch leaks automatically
    #endif
    
    // Test with Valgrind-style checks
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Delete brain and check for leaks
    brain.reset();
    
    // All memory should be freed
    EXPECT_EQ(getAllocatedMemory(), 0);
}
```

### 3. Performance Regression Testing

```cpp
TEST(PerformanceRegression, BenchmarkComparison) {
    // Store baseline performance
    auto baseline = measurePerformance(1000, 2);
    
    // Run new test
    auto current = measurePerformance(1000, 2);
    
    // Performance should not degrade significantly
    double degradation = (current.time - baseline.time) / baseline.time;
    EXPECT_LT(degradation, 0.5);  // Allow 50% degradation
}

PerformanceMetrics measurePerformance(int neurons, int regions) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", regions);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    PerformanceMetrics metrics;
    metrics.time = duration.count();
    metrics.spikesPerSecond = brain->getTotalSpikeCount() / 0.1;
    metrics.neuronsProcessed = neurons;
    
    return metrics;
}
```

## Test Automation

### Continuous Integration

```yaml
# .github/workflows/ci.yml
name: NLM CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake g++ git libgtest-dev
        cd /usr/src/gtest && sudo cmake . && sudo make && sudo cp lib/*.a /usr/lib
    
    - name: Configure
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
                 -DNLM_ENABLE_TESTS=ON \
                 -DNLM_ENABLE_BENCHMARKS=ON
    
    - name: Build
      run: |
        cd build
        make -j4
    
    - name: Run Tests
      run: |
        cd build
        ./test_nlm
        ./benchmarks/brain_benchmarks
    
    - name: Upload Results
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: test-results-${{ matrix.build_type }}
        path: build/test_results/
```

### Test Reports

```bash
#!/bin/bash
# generate_test_report.sh

# Generate comprehensive test report
./build/test_nlm --gtest_output=xml:test_results.xml

# Generate HTML report
python3 -c "
import xml.etree.ElementTree as ET
import sys

tree = ET.parse('test_results.xml')
root = tree.getroot()

print('=== NLM Test Report ===')
print(f'Total tests: {len(root.findall(\"testcase\"))}')
print(f'Passed: {len(root.findall(\"testcase\")[filter(lambda x: 'FAILED' not in x.attrib]))}')
print(f'Failed: {len(root.findall(\"testcase\")[filter(lambda x: 'FAILED' in x.attrib]))}')

if root.find('failure'):
    print('Failures:')
    for failure in root.findall('failure'):
        print(f'  - {failure.get(\"message\")}')
"
```

## Validation Scripts

### Validation Framework

```cpp
// tests/test_validation.cpp
#include <gtest/gtest.h>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

class ValidationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Load validation configuration
        std::ifstream configFile("validation_config.json");
        config_ = nlohmann::json::parse(configFile);
    }
    
    void TearDown() override {
        config_.clear();
    }
    
    nlohmann::json config_;
};

TEST_F(ValidationTest, ConfigurationValidation) {
    // Validate configuration against schema
    bool isValid = validateConfig(config_);
    EXPECT_TRUE(isValid) << "Configuration is invalid";
}

TEST_F(ValidationTest, PerformanceValidation) {
    // Check that performance meets requirements
    auto metrics = measurePerformance(config_);
    
    EXPECT_LE(metrics.stepTime, config_["performance"]["max_step_time"]);
    EXPECT_GE(metrics.spikeRate, config_["performance"]["min_spike_rate"]);
}

TEST_F(ValidationTest, PlasticityValidation) {
    // Validate plasticity behavior
    auto config = std::make_shared<Config>(config_);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Apply learning
    applyLearning(brain.get(), config_);
    
    // Check learning occurred
    EXPECT_GT(calculateLearning(brain.get()), config_["validation"]["min_learning"]);
}

} // namespace nlm
```

## Conclusion

Testing is essential for ensuring the reliability and correctness of the NLM framework. By following these testing guidelines, developers can:

1. **Maintain code quality** through comprehensive test coverage
2. **Prevent regressions** by automated testing
3. **Ensure performance** through benchmarking
4. **Validate functionality** through integration testing
5. **Document behavior** through test cases

The testing framework should be continuously improved and expanded as new features are added to the NLM framework.

---

**Version**: 0.1.0  
**Last Updated**: $(date)  
**Documentation**: NLM Testing Framework Guide