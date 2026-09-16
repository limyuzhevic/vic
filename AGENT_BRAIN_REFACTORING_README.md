# NLM AgentBrain Refactoring - Component-Based Architecture

## Overview

This document describes the refactored AgentBrain architecture that separates concerns into modular components:

- **MotorDecoder**: Handles motor neuron activity decoding and command selection
- **SensoryProcessor**: Processes sensory input from the environment
- **NeuromodulationManager**: Manages dopamine, curiosity, and reward signals
- **DevelopmentManager**: Handles developmental stages and plasticity changes
- **AgentBrain**: Orchestrates all components while maintaining backward compatibility

## Key Improvements

### 1. Separation of Concerns
Each component has a single, well-defined responsibility:
- Motor decoding strategies are configurable via the `MotorDecoder` class
- Sensory processing supports multiple modalities and processing strategies
- Neuromodulation manages all neuromodulator signals centrally
- Development manages growth and plasticity over time

### 2. Configuration-Driven Design
All components support extensive configuration:
- Motor commands can be weighted and thresholded differently
- Sensory processing supports linear, logarithmic, and adaptive strategies
- Neuromodulation parameters are fully customizable
- Developmental stages and transitions are configurable

### 3. Backward Compatibility
The public API of `AgentBrain` remains unchanged:
- All existing methods work identically
- Legacy neuron groups are maintained for compatibility
- State variables are synchronized with component managers

### 4. Enhanced Error Handling
Comprehensive validation and error checking:
- Null pointer checks throughout
- State validation before operations
- Graceful fallbacks for invalid states

### 5. Improved Testability
Components are independently testable:
- Each component can be unit tested in isolation
- Mock objects can be injected for testing
- Configuration can be customized for specific test scenarios

## Component Documentation

### MotorDecoder

**Purpose**: Converts neural activity into motor commands using configurable strategies.

**Key Features**:
- Multiple decoding strategies: ThresholdBased, WeightedAverage, TopN, AdaptiveThreshold
- Configurable action space and motor command weights
- Curiosity-driven exploration
- Support for different action types (Move, Turn, Look, Interact)

**Configuration Options**:
```cpp
struct MotorConfig {
    float activityThreshold_;      // Minimum activity to trigger action
    float explorationThreshold_;   // Curiosity threshold for exploration
    float noveltyExploration_;     // Novelty threshold for random actions
    // ... many more options
};
```

**Usage Example**:
```cpp
// Configure motor decoder
MotorConfig config;
config.weightForward_ = 1.5f;  // Prioritize forward movement
config.weightWait_ = 0.1f;      // Penalize waiting
config.activityThreshold_ = 0.6f; // Higher threshold for action selection

MotorDecoder decoder(config);
decoder.setStrategy(DecodingStrategy::WeightedAverage);
```

### SensoryProcessor

**Purpose**: Processes sensory input from the environment with configurable filtering and enhancement.

**Key Features**:
- Multi-modal sensory processing (Vision, Touch, Internal, Proprioception)
- Multiple processing strategies: Linear, Logarithmic, Sigmoid, Adaptive
- Cross-modal integration
- Novelty detection
- Neuromodulatory effects integration

**Configuration Options**:
```cpp
struct SensoryConfig {
    float visionScaleFactor_;           // Scale factor for vision input
    float touchNoiseLevel_;             // Touch sensor noise
    bool enableSensoryNormalization_;   // Enable normalization
    bool enableSensoryCompression_;    // Enable dynamic range compression
    // ... many more options
};
```

**Usage Example**:
```cpp
// Configure sensory processor
SensoryConfig config;
config.enableVisionFiltering_ = true;
config.visionSmoothing_ = 0.9f;
config.enableCrossModalIntegration_ = true;
config.integrationWeightVision_ = 0.5f;

SensoryProcessor processor(config);
processor.setVisionStrategy(SensoryConfig::ProcessingStrategy::Sigmoid);
```

### NeuromodulationManager

**Purpose**: Manages all neuromodulatory signals (dopamine, novelty, curiosity) and their effects.

**Key Features**:
- Reward prediction error computation
- Novelty detection and tracking
- Curiosity-driven exploration motivation
- Signal generation and decay
- Plasticity calculation based on neuromodulators

**Configuration Options**:
```cpp
struct NeuromodulationConfig {
    float rewardLearningRate_;          // Learning rate for reward prediction
    float dopamineGain_;               // Dopamine signal scaling
    float noveltyThreshold_;           // Novelty detection threshold
    float curiosityGain_;              // Curiosity response to novelty
    // ... many more options
};
```

**Usage Example**:
```cpp
// Configure neuromodulation manager
NeuromodulationConfig config;
config.enableRewardModulation_ = true;
config.enableNoveltyDetection_ = true;
config.enableCuriosityDriven_ = true;
config.curiosityGain_ = 1.5f;

NeuromodulationManager neuromodulation(config);
```

### DevelopmentManager

**Purpose**: Manages developmental stages, plasticity changes, and age-dependent behaviors.

**Key Features**:
- Configurable developmental stages (Initial, CriticalPeriod, Maturation, Adult, Aging)
- Stage-specific plasticity modifiers
- Synaptogenesis and pruning control
- Activity-dependent plasticity
- Backward compatibility with legacy development logic

**Configuration Options**:
```cpp
struct DevelopmentConfig {
    float initialStageDuration_;         // Duration of Initial stage
    float criticalPeriodDuration_;      // Duration of CriticalPeriod stage
    float maturationDuration_;          // Duration of Maturation stage
    float initialPlasticity_;            // Plasticity in Initial stage
    float adultPlasticity_;              // Plasticity in Adult stage
    // ... many more options
};
```

**Usage Example**:
```cpp
// Configure development manager
DevelopmentConfig config;
config.enableDevelopment_ = true;
config.enableStructuralPlasticity_ = true;
config.initialStageDuration_ = 60.0f;
config.criticalPeriodDuration_ = 240.0f;

DevelopmentManager development(config);
```

## Migration Guide

### For Existing Users

The refactored `AgentBrain` maintains full backward compatibility:

1. **Existing Code Works**: All existing `AgentBrain` methods work identically
2. **Legacy State Variables**: Internal state variables are synchronized with components
3. **Legacy Neuron Groups**: Neuron groups are maintained for compatibility

**Example - Unchanged Usage**:
```cpp
// This still works exactly the same
AgentBrain brain(std::make_shared<Brain>(config));
brain.initialize(world);

// All these methods remain unchanged
brain.processSensoryInput(percept);
MotorCommand cmd = brain.decodeMotorCommand();
brain.applyRewardModulation(reward, predictedReward);
brain.updateDevelopment(timestep);
```

### For Advanced Users

You can now access and configure individual components:

**Example - Advanced Configuration**:
```cpp
AgentBrain brain(std::make_shared<Brain>(config));

// Configure individual components
brain.getMotorDecoder().configure(customMotorConfig);
brain.getSensoryProcessor().configure(customSensoryConfig);
brain.getNeuromodulationManager().configure(customNeuromodConfig);
brain.getDevelopmentManager().configure(customDevConfig);

// Enable/disable features
brain.enableRewardModulation(false);
brain.enableCuriosity(true);
```

## Best Practices

### 1. Configuration
- Use the component getters to access and modify configurations
- Create configuration objects and pass them to component constructors
- Update configurations before initialization for best results

### 2. Error Handling
- Always check component validity before use: `if (!component.isValid())`
- Handle null pointer cases gracefully
- Use component state checking before operations

### 3. Testing
- Test each component independently
- Mock dependencies for unit testing
- Use configuration objects for reproducible tests

### 4. Performance
- Configure components once at initialization
- Minimize runtime configuration changes
- Use appropriate processing strategies for your use case

## Build and Installation

The new components are part of the existing NLM framework:

1. Include headers as needed:
   ```cpp
   #include "MotorDecoder.hpp"
   #include "SensoryProcessor.hpp"
   #include "NeuromodulationManager.hpp"
   #include "DevelopmentManager.hpp"
   ```

2. Link with the standard NLM libraries

3. Ensure C++11 or later compiler support

## Examples

See the test directory for comprehensive examples of:
- Component initialization
- Configuration usage
- Integration testing
- Performance benchmarks

## Future Extensions

The component architecture supports future extensions:

1. **New Components**: Easy to add new processing units (e.g., MemoryManager, AttentionManager)
2. **Plugin Architecture**: Components can be extended or replaced
3. **Distributed Processing**: Components could be distributed across multiple processes
4. **Hardware Acceleration**: Components optimized for specific hardware targets

## Acknowledgments

This refactoring was motivated by:
- The need for better modularity and testability
- The complexity of the original monolithic AgentBrain
- The desire to support configurable behavior
- Best practices in software architecture

The new architecture maintains the power and flexibility of the original while being much more maintainable and extensible.