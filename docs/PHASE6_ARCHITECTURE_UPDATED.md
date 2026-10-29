# NLM/熙然 Phase 6 Architecture Documentation (Updated)

## Overview

Phase 6 integrates all previously separate systems into a coherent artificial brain. The architecture follows a closed-loop design where neural activity, plasticity, memory, neuromodulation, and behavior continuously influence each other.

**Updated in Phase 6.1:** Enhanced configuration system, improved Python bindings, and comprehensive test framework added.

## Brain Loop Architecture

The main simulation loop (`Brain::step()`) implements the complete brain simulation:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events
    // ... spike detection and propagation ...
    
    // 4. Update working memory
    pImpl->workingMemory->update(pImpl->timestep);
    
    // 5. Apply neuromodulation effects
    // - Dopamine affects neural excitability
    // - Novelty detection updates
    // - Curiosity computation
    
    // 6. Apply plasticity rules (STDP and Hebbian)
    // with neuromodulation modulation
    
    // 7. Update episodic memory with current experience
    
    // 8. Update prediction system
    
    // 9. Update attention system
    
    // 10. Update concept formation
    
    // 11. Apply structural plasticity (synaptogenesis/pruning)
    
    // 12. Replay important memories
    
    // 13. Apply development effects
    
    // 14. Memory consolidation
    
    // 15. Checkpoint management
}
```

**Updated Features:**
- Enhanced configuration validation and JSON/YAML support
- Improved Python bindings with better error handling and type checking
- Comprehensive test framework for validation
- Better error handling and parameter validation throughout

## Memory Systems

### Working Memory (NeuralWorkingMemory)

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp`

**Updated Features:**
- Enhanced parameter validation in Python bindings
- Better memory capacity management
- Improved competition dynamics

**Mechanisms:**
- Persistent neural activity for maintaining information
- Recurrent connections for maintenance
- Competition among memory traces
- Decay of weak traces

**Integration:**
- Stores firing neurons each step
- Updated each simulation step
- Influences attention competition
- Decays over time if not reinforced

### Episodic Memory (NeuralEpisodicMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp`

**Updated Features:**
- Improved Python bindings for better API access
- Enhanced episode capture and replay mechanisms
- Better integration with new test framework

**Mechanisms:**
- Stores experiences as neural activity patterns
- Temporal ordering through timestamps
- Relevance-weighted recall
- Replay during offline periods

**Integration:**
- Episodes captured every 10 simulation steps
- Replay triggered every 100 steps
- Consolidation every 1000 steps
- Influences future behavior through replay

### Associative Memory (NeuralAssociativeMemory)

**Location:** `src/memory/NeuralAssociativeMemory.hpp/cpp`

**Updated Features:**
- Enhanced pattern association capabilities
- Better integration with prediction systems

**Mechanisms:**
- Hebbian associations between patterns
- Spreading activation for recall
- Strength modification through experience

**Integration:**
- Associated with episodic memory
- Updated during experience encoding

## Neuromodulation Systems

### Dopamine

**Location:** `src/neuromodulation/Neuromodulator.hpp/cpp`

**Updated Features:**
- Enhanced Python bindings with better error handling
- Improved reward prediction error computation
- Better integration with learning systems

**Mechanisms:**
- Reward prediction error computation
- Phasic and tonic signaling
- Behavioral vigor modulation

**Integration:**
- Affects neural excitability (dopamine level modulates membrane potential)
- Scales STDP learning rates
- Modulates plasticity through eligibility traces
- Affects action selection indirectly through learning

### Curiosity

**Location:** `src/neuromodulation/Curiosity.hpp/cpp`

**Updated Features:**
- Enhanced parameter validation
- Better integration with AgentBrain
- Improved exploration behavior

**Mechanisms:**
- Combines novelty and prediction error
- Exploration drive computation
- Decay over time

**Integration:**
- Used by AgentBrain for exploration decisions
- Stored in AgentBrain for action selection
- Affects motor command decoding

### Novelty

**Location:** `src/neuromodulation/Novelty.hpp/cpp`

**Updated Features:**
- Improved Python bindings
- Better integration with other neuromodulators

**Mechanisms:**
- Novelty detection from sensory patterns
- Comparison with previous patterns
- Decay over time

**Integration:**
- Computed from vision changes in AgentBrain
- Used for curiosity computation
- Stored for later use

## Prediction System

**Location:** `src/prediction/PredictionSystem.hpp/cpp`

**Updated Features:**
- Enhanced configuration loading and validation
- Better Python bindings for API access
- Improved integration with other brain systems

**Mechanisms:**
- Next state prediction
- Prediction error computation
- Confidence tracking
- History tracking

**Integration:**
- Called in brain loop (placeholder for full implementation)
- Prediction error affects curiosity
- Future: will affect attention and planning

## Cognition Systems

### Neural Planner

**Location:** `src/cognition/NeuralPlanner.hpp/cpp`

**Updated Features:**
- Enhanced Python bindings
- Better integration with action selection
- Improved planning capabilities

**Mechanisms:**
- Multi-step action sequence evaluation
- Planning depth configuration
- Action quality from experience
- Plan success tracking

**Integration:**
- Initialized in Brain
- Future: will be used for goal-directed behavior

### Concept Formation

**Location:** `src/cognition/ConceptFormation.hpp/cpp`

**Updated Features:**
- Enhanced pattern recognition
- Better integration with sensory systems

**Mechanisms:**
- Pattern clustering through similarity
- Prototype formation
- Instance tracking
- Stability measurement

**Integration:**
- Initialized in Brain
- Future: will process sensory patterns for generalization

### Attentional Selection

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp` (AttentionalSelection class)

**Updated Features:**
- Enhanced parameter validation
- Better Python bindings
- Improved competition dynamics

**Mechanisms:**
- Competition between neural populations
- Winner-take-more dynamics
- Top-down and bottom-up bias
- Inhibition and excitation

**Integration:**
- Processes working memory competitors
- Updated each step
- Applies excitation to winners, inhibition to losers

## Development System

**Location:** `src/development/DevelopmentSystem.hpp/cpp`

**Updated Features:**
- Enhanced stage progression
- Better integration with plasticity
- Improved Python bindings

**Mechanisms:**
- Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)
- Plasticity modification by stage
- Auto-advance based on age

**Integration:**
- Updated every 1000 steps
- Modifies structural plasticity rates
- Affects neural excitability indirectly through plasticity

## Checkpoint System

**Location:** `src/performance/CheckpointSystem.hpp`

**Updated Features:**
- Enhanced error handling and validation
- Better file format support (JSON/YAML)
- Improved serialization format

**Mechanisms:**
- Brain state serialization
- Neuron and synapse state saving
- Checkpoint manager for automated saves

**Integration:**
- `Brain::save()` writes checkpoint
- `Brain::load()` reads checkpoint
- CheckpointManager handles periodic saves

## Data Flow

### Sensory Processing

**Updated Features:**
- Enhanced error handling in sensory processing
- Better parameter validation
- Improved Python bindings for sensory APIs

```
World → SensoryPercept → AgentBrain::processSensoryInput()
                               ↓
                         Brain::receiveSensoryInput()
                               ↓
                         Sensory neurons receive current
                               ↓
                         LIF dynamics integrate input
```

### Action Selection

**Updated Features:**
- Enhanced Python bindings for action selection
- Better parameter validation
- Improved error handling

```
Motor neuron activity → AgentBrain::decodeMotorCommand()
                               ↓
                         Curiosity exploration check
                               ↓
                         MotorCommand selected
                               ↓
                         World::applyAction()
```

### Memory Formation

**Updated Features:**
- Enhanced episodic memory with Python bindings
- Better working memory integration
- Improved consolidation mechanisms

```
Brain step (spikes) → Working memory stores firing neurons
                               ↓
                         Every 10 steps: Episodic memory captures state
                               ↓
                         Every 100 steps: Replay of important episodes
                               ↓
                         Every 1000 steps: Consolidation
```

### Learning

**Updated Features:**
- Enhanced error handling in learning systems
- Better parameter validation for plasticity
- Improved Python bindings for learning APIs

```
Reward from environment → AgentBrain::applyRewardModulation()
                               ↓
                         Dopamine computed (prediction error)
                               ↓
                         Synapses with eligibility traces updated
                               ↓
                         STDP weights modulated by dopamine
```

## System Independence and Coupling

### Loose Coupling (can function somewhat independently):
- Working memory (stores current activity)
- Episodic memory (stores experiences)
- Prediction system (computes errors)
- Concept formation (organizes patterns)

### Tight Coupling (strongly interconnected):
- Neurons ↔ Synapses ↔ Plasticity
- Dopamine ↔ Eligibility traces ↔ Synaptic weights
- Novelty ↔ Curiosity ↔ Exploration
- Development ↔ Plasticity rates ↔ Structural plasticity

## Enhanced Configuration System

**New Configuration Features:**
- **JSON/YAML Support**: Automatic detection and parsing of JSON and YAML configuration files
- **Schema Validation**: Parameter type checking and validation
- **Enhanced Command Line**: Better argument parsing and type conversion
- **File Format Validation**: Extension checking and content validation

**Configuration File Examples:**

**JSON Configuration (`config.json`):**
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0
    },
    "neuromodulation": {
        "dopamine_baseline": 0.1,
        "curiosity_baseline": 0.5
    },
    "memory": {
        "working_memory_capacity": 100,
        "episodic_memory_max_episodes": 1000
    }
}
```

**YAML Configuration (`config.yaml`):**
```yaml
brain:
    neuron_count: 1000
    region_count: 1
    connection_probability: 0.1

plasticity:
    stdp_ltp_weight: 0.01
    stdp_ltd_weight: 0.012
    stdp_tau: 20.0

neuromodulation:
    dopamine_baseline: 0.1
    curiosity_baseline: 0.5

memory:
    working_memory_capacity: 100
    episodic_memory_max_episodes: 1000
```

**Command Line Usage:**
```bash
./nlm --config config.json
./nlm --neuron_count 2000 --region_count 2 --connection_probability 0.2
./nlm --stdp_ltp_weight 0.02 --stdp_ltd_weight 0.018
```

## Enhanced Python API

**New Python Features:**
- **Comprehensive Parameter Validation**: All methods include parameter validation
- **Better Error Handling**: Detailed error messages and exception handling
- **Enhanced Type Checking**: Proper type conversion and validation
- **Convenience Methods**: Added `to_dict()`, `to_array()` for easier data manipulation

**Python Usage Examples:**

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 1)
config.set("plasticity.stdp_ltp_weight", 0.01)

# Create brain with validation
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced sensory input processing
vision_data = numpy.array([...], dtype=float)
action_result = brain.receiveSensoryInput(
    type="vision",
    data=vision_data,
    width=64,
    height=48,
    channels=3
)

# Enhanced action selection with validation
action = brain.produceAction()
print(f"Action type: {action['type']}")
print(f"Action parameters: {action['parameters']}")

# Enhanced brain state access
brain_stats = brain.to_dict()
print(f"Total neurons: {brain_stats['totalNeuronCount']}")
print(f"Total spikes: {brain_stats['totalSpikeCount']}")
print(f"Development stage: {brain_stats['developmentalStage']}")

# Enhanced agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Process percept with validation (accepts dictionaries)
percept_dict = {
    "vision": vision_array,
    "audio": audio_array,
    "internal": internal_signals,
    "timestamp": 1.5
}
agent.processSensoryInput(percept_dict)

# Action with error handling
action_dict = agent.decodeMotorCommand()
agent.applyRewardModulation(reward=1.0, predictedReward=0.8)
```

## Comprehensive Test Framework

**New Test Framework Features:**
- **Centralized Test Runner**: All tests executed from a single interface
- **Better Assertion Macros**: Detailed error messages with context
- **Performance Timing**: Each test reports execution time
- **Test Organization**: Tests grouped into logical suites
- **Custom Test Cases**: Support for setup/teardown methods
- **Easy Migration**: Simple conversion for existing tests

**Test Usage Examples:**

```cpp
// Include the test framework
#include "nlm/test/TestFramework.hpp"
using namespace nlm::test;

// Basic test using TEST_CASE macro
TEST_CASE("Neuron Tests", "testNeuronCreation") {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    
    TEST_ASSERT_NE(id1, id2);
    TEST_ASSERT_LT(id1, id2);
    TEST_ASSERT_EQ(id1.index(), 1);
}

// Test with custom TestCase class (for setup/teardown)
class EnhancedNeuronTest : public TestCase {
public:
    std::string getName() const override { return "EnhancedNeuronTest"; }
    std::string getSuite() const override { return "Neuron"; }
    
    void setup() override {
        // Initialize test data
        neuron_ = nlm::NeuronId(1);
        setupCalled_ = true;
    }
    
    void run() override {
        TEST_ASSERT_TRUE(setupCalled_);
        TEST_ASSERT_EQ(neuron_.index(), 1);
        
        // Test the actual functionality
        TEST_ASSERT_NE(neuron_, nlm::INVALID_NEURON_ID);
    }
    
    void teardown() override {
        // Clean up test data
        teardownCalled_ = true;
    }
    
private:
    nlm::NeuronId neuron_;
    bool setupCalled_ = false;
    bool teardownCalled_ = false;
};

// Run the test
TEST_CASE("Neuron", "testEnhancedNeuronTest") {
    EnhancedNeuronTest test;
    test.run();
}
```

## New Test Files Added

### Comprehensive Test Framework Demo
- **File**: `tests/test_framework_demo.cpp`
- **Purpose**: Demonstrates the new test framework capabilities
- **Features**: Basic assertions, custom TestCase classes, setup/teardown, exception handling

### Migrated Test Types
- **File**: `tests/test_types_migrated.cpp`
- **Purpose**: Shows migration of existing tests to new framework
- **Features**: Uses TEST_CASE macro, better assertions, organized into suites

### Migration Guide
- **File**: `docs/TEST_MIGRATION_GUIDE.md`
- **Purpose**: Complete guide for migrating existing tests
- **Features**: Step-by-step migration instructions, examples, best practices

## Extension Points

### Where to add new mechanisms:

1. **New plasticity rules**: Add to `Brain::step()` after STDP/Hebbian with enhanced validation
2. **New neuromodulators**: Add to `Brain::Impl` and update `Brain::step()` with Python bindings
3. **New memory types**: Add to `Brain::Impl` and integrate with existing memory systems
4. **New cognition**: Add to `Brain::Impl` and connect to perception/action with validation

### Key integration hooks:

1. `receiveSensoryInput()` - Enhanced with parameter validation
2. `produceAction()` - Improved with Python dictionary support
3. `step()` - Main loop with all systems updated
4. `applyRewardModulation()` - Enhanced learning signal handling
5. `save()/load()` - Improved configuration file support

## Phase 6.1 Enhancement Summary

Phase 6.1 introduces significant improvements:

### ✅ Configuration System
- JSON/YAML file format support
- Enhanced parameter validation
- Better error handling and messaging
- Command line argument improvements

### ✅ Python Bindings
- Comprehensive type checking
- Parameter validation
- Enhanced error handling
- Convenience methods for easier usage

### ✅ Test Framework
- Centralized test runner
- Better assertion macros
- Performance timing
- Test organization and suites
- Easy migration for existing tests

### ✅ Documentation
- Updated Phase 6 architecture documentation
- Enhanced Python API documentation
- Comprehensive migration guide
- Configuration file examples

### ✅ Bug Fixes
- Configuration file parsing errors
- Parameter validation issues
- Error handling improvements
- Memory management enhancements

### ✅ Performance Improvements
- Enhanced validation reduces runtime errors
- Better error handling improves debugging
- Improved test framework provides better performance insights

## Next Steps

1. **Immediate**: Run the new test framework demo
2. **Short-term**: Migrate existing tests to use new framework
3. **Medium-term**: Add comprehensive integration tests
4. **Long-term**: Add performance benchmarks and profiling tools
5. **Future**: Add distributed computing capabilities

The enhanced Phase 6.1 architecture provides a robust foundation for building advanced artificial developmental brains with improved usability, reliability, and test coverage.