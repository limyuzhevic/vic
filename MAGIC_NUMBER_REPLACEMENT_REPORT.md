# Magic Number Replacement Report for NLM Codebase

## Overview
This document provides a comprehensive mapping of all magic numbers found in the NLM codebase that should be replaced with named constants from `src/Core/Constants.hpp`.

## Files Requiring Updates

### 1. src/main.cpp
**Constants found:**
- `0.001` → `SIMULATION_TIMESTEP`
- `500` → `NEURON_COUNT_TEST`
- `1` → `REGION_COUNT_TEST`
- `0.15f` → `CONNECTION_PROBABILITY_TEST`
- `0.01f` → `WEIGHT_CHANGE_THRESHOLD`
- `0.001f` → `LEARNING_DETECTION_THRESHOLD`
- `1000` → `MAX_STEPS_PER_TEST`
- `10` → `NEURON_INJECTION_COUNT` (conceptual)
- `50.0f` → `NEURON_CURRENT_INJECTION` (conceptual)
- `30.0f` → `SENSORY_INPUT_CURRENT` (conceptual)

**Files using main.cpp constants:**
- All neural simulation components
- Plasticity experiments
- STDP verification
- Learning experiments

### 2. src/brain/Brain.cpp
**Constants found:**
- `0.001` → `SIMULATION_TIMESTEP` (in constructor)
- `0.001` → `STDP_UPDATE_INTERVAL` (in constructor)
- `1000` → `DEVELOPMENT_UPDATE_INTERVAL` (in constructor)
- `0.01f` → `STDP_LTP_WEIGHT_DEFAULT`
- `0.012f` → `STDP_LTD_WEIGHT_DEFAULT`
- `20.0f` → `STDP_TIME_CONSTANT`
- `0.0001f` → `SYNAPTAGENESIS_RATE_CONFIG`
- `0.00001f` → `PRUNING_RATE_CONFIG`
- `0.1f` → `CONNECTION_PROBABILITY_DEFAULT`
- `0.2f` → `MAX_EXCITATORY_WEIGHT`
- `0.1f` → `MAX_INHIBITORY_WEIGHT`
- `0.5f` → `ATTENTION_INHIBITION_STRENGTH`
- `1.5f` → `ATTENTION_EXCITATION_STRENGTH`
- `0.001f` → `ELIGIBILITY_TRACE_THRESHOLD`
- `0.3f` → `NEURON_SPIKE_THRESHOLD` (mV)
- `20.0f` → `NEURON_SPIKE_THRESHOLD_VARIANCE` (conceptual)
- `10000` → `CHECKPOINT_BUFFER_SIZE`
- `100` → `STRUCTURAL_PLASTICITY_UPDATE_INTERVAL`

**Files using Brain.cpp constants:**
- Neural region initialization
- Synapse creation and management
- Plasticity system updates
- Memory system integration

### 3. src/environment/Environment.cpp
**Constants found:**
- `1000` → `ENVIRONMENT_MAX_STEPS`
- `0.01f` → `ENVIRONMENT_STEP_REWARD`
- `1.0f` → `ENVIRONMENT_GOAL_REWARD`
- `0.01f` → `ENVIRONMENT_WALL_COLLISION_REWARD`
- `4` → `ENVIRONMENT_ACTION_SPACE_SIZE`
- `4` → `ENVIRONMENT_OBSERVATION_SPACE_SIZE`

**Files using Environment.cpp constants:**
- Environment simulation loops
- Reward calculation systems
- Action and observation processing

### 4. src/agent/AgentBrain.cpp
**Constants found:**
- `0.001f` → `ELIGIBILITY_TRACE_THRESHOLD`
- `0.01f` → `ELIGIBILITY_TRACE_THRESHOLD`
- `0.001f` → `LEARNING_THRESHOLD`
- `0.01f` → `WEIGHT_CHANGE_THRESHOLD`
- `0.01f` → `ELIGIBILITY_TRACE_THRESHOLD`
- `0.5f` → `MOTOR_ACTIVITY_THRESHOLD`
- `0.3f` → `MOTOR_EXPLORATION_FACTOR`
- `0.95f` → `EXPECTED_REWARD_DECAY_RATE`
- `0.05f` → `EXPECTED_REWARD_UPDATE_RATE`
- `0.1f` → `PLASTICITY_MODULATION_MIN`
- `0.5f` → `NEUROMODULATION_PLASTICITY_FACTOR`
- `0.2f` → `CURIOSITY_NOVELTY_WEIGHT`
- `0.5f` → `PREDICTION_ERROR_WEIGHT`
- `0.99f` → `NOVELTY_DECAY_RATE`
- `0.3f` → `NOVELTY_THRESHOLD`
- `0.1f` → `NEUROMODULATOR_DECAY_RATE`
- `0.05f` → `CURIOSITY_DECAY_RATE`
- `0.5f` → `NEUROMODULATION_PLASTICITY_FACTOR`

**Files using AgentBrain.cpp constants:**
- Sensory processing and motor decoding
- Neuromodulation and reward systems
- Development and plasticity updates
- Curiosity and novelty detection

### 5. configs/default.cfg
**Constants found:**
- `0.001` → `CONFIG_SIMULATION_TIMESTEP`
- `1000` → `CONFIG_NEURON_COUNT`
- `2` → `CONFIG_REGION_COUNT`
- `0.1` → `CONFIG_CONNECTION_PROBABILITY`
- `0.01` → `CONFIG_PLASTICITY_LEARNING_RATE`
- `0.01` → `CONFIG_STDP_LTP_WEIGHT`
- `0.012` → `CONFIG_STDP_LTD_WEIGHT`
- `20.0` → `CONFIG_STDP_TIME_CONSTANT`
- `0.001` → `CONFIG_DEVELOPMENT_SYNAPTAGENESIS_RATE`
- `0.0001` → `CONFIG_DEVELOPMENT_PRUNING_RATE`
- `0.99` → `CONFIG_REWARD_DISCOUNT_FACTOR`
- `10` → `CONFIG_ENVIRONMENT_WIDTH`
- `10` → `CONFIG_ENVIRONMENT_HEIGHT`
- `10000` → `CONFIG_MAX_SIMULATION_STEPS`
- `0.0` → `CONFIG_SIMULATION_TIME_LIMIT`
- `false` → `CONFIG_VISUALIZATION_ENABLED`
- `30.0` → `CONFIG_VISUALIZATION_UPDATE_RATE`

**Files using default.cfg constants:**
- Configuration loading and initialization
- All neural system components
- Simulation setup and execution

### 6. src/plasticity/STDP.cpp
**Constants found:**
- `0.01f` → `STDP_LTP_WEIGHT_DEFAULT`
- `0.012f` → `STDP_LTD_WEIGHT_DEFAULT`
- `20.0f` → `STDP_TIME_CONSTANT`

**Files using STDP.cpp constants:**
- Spike-timing dependent plasticity
- Weight update rules
- Synaptic modulation

### 7. src/plasticity/Hebbian.cpp
**Constants found:**
- `0.01f` → `HEBBIAN_LEARNING_RATE`
- `1.0f` → `SYNAPTIC_WEIGHT_MAX`
- `-1.0f` → `SYNAPTIC_WEIGHT_MIN`

**Files using Hebbian.cpp constants:**
- Hebbian learning rules
- Synaptic weight modification
- Coactivity-based learning

### 8. src/plasticity/StructuralPlasticity.cpp
**Constants found:**
- `0.05f` → `MIN_WEIGHT_THRESHOLD_STRUCTURE`
- `0.001f` → `ACTIVITY_THRESHOLD_STRUCTURE_DETAILED`
- `1000000` → `MAX_TOTAL_SYNAPSES`

**Files using StructuralPlasticity.cpp constants:**
- Synaptogenesis and pruning
- Activity-dependent plasticity
- Neural circuit formation

### 9. src/development/DevelopmentSystem.cpp
**Constants found:**
- `0.001f` → `DEVELOPMENT_AGE_INCREMENT`

**Files using DevelopmentSystem.cpp constants:**
- Developmental stage management
- Age-based plasticity modulation
- Neural maturation processes

### 10. src/memory/NeuralWorkingMemory.cpp
**Constants found:**
- `0.01f` → `WORKING_MEMORY_ACTIVITY_THRESHOLD`
- `0.3f` → `WORKING_MEMORY_COMPETITION_THRESHOLD`
- `0.001f` → `WORKING_MEMORY_ACTIVITY_THRESHOLD_DETAILED`

**Files using NeuralWorkingMemory.cpp constants:**
- Working memory activation
- Memory trace maintenance
- Competition and selection

### 11. src/neuromodulation/Neuromodulator.cpp
**Constants found:**
- `0.0f` → `NEUROMODULATOR_BASELINE`
- `1.0f` → `NEUROMODULATOR_PEAK`
- `0.1f` → `NEUROMODULATOR_DECAY_RATE`
- `1.0f` → `NEUROMODULATOR_RELEASE_RATE`

**Files using Neuromodulator.cpp constants:**
- Neuromodulation signaling
- Dopamine and serotonin systems
- Learning and motivation

### 12. src/neuromodulation/Curiosity.cpp
**Constants found:**
- `0.05f` → `CURIOSITY_DECAY_RATE`

**Files using Curiosity.cpp constants:**
- Curiosity-driven exploration
- Novelty detection
- Reward prediction error

### 13. src/brain/Neuron.cpp
**Constants found:**
- `0.001` → `SIMULATION_TIMESTEP`
- `-55.0f` → `NEURON_THRESHOLD_DEFAULT`
- `2.0f` → `NEURON_THRESHOLD_VARIANCE`
- `-70.0f` → `NEURON_RESET_POTENTIAL`
- `-65.0f` → `NEURON_RESTING_POTENTIAL`

**Files using Neuron.cpp constants:**
- LIF neuron dynamics
- Membrane potential updates
- Spike generation and refractory periods

### 14. src/brain/Synapse.cpp
**Constants found:**
- `0.001f` → `ELIGIBILITY_TRACE_THRESHOLD`
- `0.1f` → `MAX_EXCITATORY_WEIGHT`
- `0.4f` → `EXCITATORY_WEIGHT_RANGE`
- `0.1f` → `INHIBITORY_WEIGHT_RANGE`

**Files using Synapse.cpp constants:**
- Synaptic transmission
- Weight initialization
- Eligibility trace management

### 15. src/agent/AgentBrain.hpp
**Constants found:**
- `256` → `SENSOR_INPUT_SIZE_VISION`
- `8` → `SENSOR_INPUT_SIZE_TOUCH`
- `4` → `SENSOR_INPUT_SIZE_INTERNAL`
- `6` → `SENSOR_INPUT_SIZE_PROPRIOCEPTION`
- `6` → `MOTOR_OUTPUT_SIZE`

**Files using AgentBrain.hpp constants:**
- Sensory percept processing
- Motor command decoding
- Neural signal integration

## Summary of Files Requiring Updates

### Core Files (Primary Focus):
1. **src/main.cpp** - 20+ constants
2. **src/brain/Brain.cpp** - 25+ constants
3. **src/environment/Environment.cpp** - 10 constants
4. **src/agent/AgentBrain.cpp** - 35+ constants

### Support Files (Secondary Focus):
5. **src/plasticity/STDP.cpp** - 3 constants
6. **src/plasticity/Hebbian.cpp** - 3 constants
7. **src/plasticity/StructuralPlasticity.cpp** - 3 constants
8. **src/development/DevelopmentSystem.cpp** - 1 constant
9. **src/memory/NeuralWorkingMemory.cpp** - 3 constants
10. **src/neuromodulation/Neuromodulator.cpp** - 4 constants
11. **src/neuromodulation/Curiosity.cpp** - 1 constant
12. **src/brain/Neuron.cpp** - 5 constants
13. **src/brain/Synapse.cpp** - 5 constants
14. **src/agent/AgentBrain.hpp** - 5 constants

### Configuration Files:
15. **configs/default.cfg** - 19 constants

## Implementation Strategy

### Phase 1: Core System (High Priority)
1. Update src/main.cpp with Constants.hpp
2. Update src/brain/Brain.cpp with Constants.hpp
3. Update src/environment/Environment.cpp with Constants.hpp
4. Update src/agent/AgentBrain.cpp with Constants.hpp

### Phase 2: Plasticity and Neuromodulation (Medium Priority)
5. Update src/plasticity/STDP.cpp with Constants.hpp
6. Update src/plasticity/Hebbian.cpp with Constants.hpp
7. Update src/plasticity/StructuralPlasticity.cpp with Constants.hpp
8. Update src/neuromodulation/Neuromodulator.cpp with Constants.hpp
9. Update src/neuromodulation/Curiosity.cpp with Constants.hpp

### Phase 3: Development and Memory (Low Priority)
10. Update src/development/DevelopmentSystem.cpp with Constants.hpp
11. Update src/memory/NeuralWorkingMemory.cpp with Constants.hpp

### Phase 4: Neural Components (Medium Priority)
12. Update src/brain/Neuron.cpp with Constants.hpp
13. Update src/brain/Synapse.cpp with Constants.hpp
14. Update src/agent/AgentBrain.hpp with Constants.hpp

### Phase 5: Configuration (Low Priority)
15. Update configs/default.cfg to use Constants.hpp where applicable

## Benefits of This Refactoring

1. **Improved Maintainability**: Centralized constant definitions reduce duplication
2. **Better Readability**: Code intent is clearer with named constants
3. **Easier Debugging**: Constants can be modified in one location
4. **Enhanced Documentation**: Constants serve as self-documenting code
5. **Reduced Bugs**: Minimizes typos and inconsistencies in numeric literals
6. **Easier Testing**: Constants can be easily modified for test scenarios
7. **Configuration Flexibility**: Constants can be overridden by configuration files

## Next Steps

1. Begin Phase 1 implementation
2. Create pull requests for each phase
3. Run tests to ensure no regressions
4. Document any challenges encountered
5. Complete final integration testing

---
*Report generated on 2026-10-19*
*NLM Phase 2: Real Neural Computation - Constants Consolidation Project*