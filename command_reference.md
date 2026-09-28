# NLM Advanced Command Reference Guide

This document provides comprehensive reference documentation for all advanced NLM brain management commands.

## Table of Contents

1. [Brain Management Commands](#brain-management-commands)
2. [Memory Control Commands](#memory-control-commands)
3. [Neuromodulation Commands](#neuromodulation-commands)
4. [Development Commands](#development-commands)
5. [Integration Commands](#integration-commands)
6. [Exploration/Testing Commands](#explorationtesting-commands)
7. [Command Line Argument Summary](#command-line-argument-summary)

## Brain Management Commands

### `--set-development-stage <stage>`

**Description:** Set the brain's developmental stage to control age-dependent plasticity and learning capabilities.

**Parameters:**
- `<stage>`: Developmental stage string
  - `initial`: Initial developmental phase
  - `critical`: Critical period for enhanced plasticity
  - `criticalperiod`: Alias for `critical`
  - `maturation`: Maturation phase with moderate plasticity
  - `adult`: Adult stage with normal plasticity
  - `aging`: Aging stage with reduced plasticity

**Valid Values:** `initial`, `critical`, `criticalperiod`, `maturation`, `adult`, `aging`

**Examples:**
```bash
# Set critical period for accelerated learning
./nlm --set-development-stage critical

# Set to adult stage (normal plasticity)
./nlm --set-development-stage adult

# Set to initial developmental stage
./nlm --set-development-stage initial
```

**Technical Details:**
- Modifies developmental stage in the `DevelopmentSystem`
- Affects plasticity rate multipliers based on stage
- Stored in configuration with `ConfigSource::Runtime`

---

### `--set-neuromodulator-levels <dopamine> <curiosity> <novelty>`

**Description:** Adjust neuromodulator levels that influence brain dynamics, learning, and behavior.

**Parameters:**
- `<dopamine>`: Reward signal level (0.0-1.0)
  - Controls neural excitability and reward-based learning
  - High values enhance reinforcement learning
- `<curiosity>`: Exploration drive (0.0-1.0)
  - Controls tendency to explore novel stimuli
  - High values promote exploration over exploitation
- `<novelty>`: Novelty detection threshold (0.0-1.0)
  - Controls sensitivity to new information
  - Low values make system more sensitive to novelty

**Valid Ranges:** All parameters: 0.0 to 1.0

**Examples:**
```bash
# Balanced neuromodulation
./nlm --set-neuromodulator-levels 0.7 0.5 0.3

# High curiosity for exploration
./nlm --set-neuromodulator-levels 0.5 0.9 0.4

# Low novelty detection for focused learning
./nlm --set-neuromodulator-levels 0.8 0.6 0.1
```

**Technical Details:**
- Updates `Dopamine`, `Curiosity`, and `Novelty` system levels
- Affects plasticity and neural dynamics
- Integrates with prediction error signals

---

### `--enable-plasticity <true/false>`

**Description:** Enable or disable synaptic plasticity globally.

**Parameters:**
- `<true/false>`: Enable or disable plasticity

**Examples:**
```bash
# Enable all plasticity mechanisms for learning
./nlm --enable-plasticity true

# Disable plasticity for stable performance
./nlm --enable-plasticity false
```

**Technical Details:**
- Controls `STDP::enable()` and `Hebbian::enable()` flags
- Affects `Synapse::enablePlasticity()` calls
- When disabled, no synaptic weight changes occur

---

### `--set-plasticilty-rates <stdp> <hebbian> <structural>`

**Description:** Adjust plasticity learning rates for different mechanisms.

**Parameters:**
- `<stdp>`: STDP learning rate
  - Controls synaptic weight changes based on spike timing
  - Typical range: 0.001-0.1
- `<hebbian>`: Hebbian learning rate
  - Controls association strength between co-active neurons
  - Typical range: 0.001-0.05
- `<structural>`: Structural plasticity rate
  - Controls synapse formation and pruning
  - Typical range: 0.00001-0.001

**Examples:**
```bash
# Standard rates for balanced learning
./nlm --set-plasticilty-rates 0.02 0.01 0.0005

# High plasticity for rapid learning
./nlm --set-plasticilty-rates 0.1 0.05 0.002

# Low plasticity for stable memory
./nlm --set-plasticilty-rates 0.001 0.0005 0.00001
```

**Technical Details:**
- Updates `STDP::setLTPWeight()`, `Hebbian::setLearningRate()`
- Sets `StructuralPlasticity::setSynaptogenesisRate()` and `setPruningRate()`
- Stored in configuration for persistence

---

## Memory Control Commands

### `--clear-working-memory`

**Description:** Clear the working memory buffer, removing all transient active information.

**Parameters:** None

**Examples:**
```bash
# Clear working memory before new learning task
./nlm --clear-working-memory

# Clear memory before storing new episodic memory
./nlm --clear-working-memory --store-memory-episode 1:0.8,2:0.9
```

**Technical Details:**
- Calls `NeuralWorkingMemory::clear()`
- Removes all active neuron-value associations
- Working memory resets to empty state

---

### `--set-memory-capacity <type> <capacity>`

**Description:** Adjust capacity limits for different memory systems.

**Parameters:**
- `<type>`: Memory system type
  - `working`: Working memory (dynamic based on neuron count)
  - `episodic`: Episodic memory (stored experiences)
  - `associative`: Associative memory (pattern associations)
- `<capacity>`: Memory capacity
  - Working memory: Number of active associations
  - Episodic memory: Maximum number of stored episodes
  - Associative memory: Number of possible associations

**Examples:**
```bash
# Increase episodic memory capacity
./nlm --set-memory-capacity episodic 1000

# Adjust working memory capacity
./nlm --set-memory-capacity working 500

# Set associative memory capacity
./nlm --set-memory-capacity associative 2000
```

**Technical Details:**
- Affects memory system initialization and growth
- Impacts memory consolidation and retrieval
- Configurable for different experimental requirements

---

### `--store-memory-episode <data>`

**Description:** Store episodic memory from current simulation state.

**Parameters:**
- `<data>`: Episodic memory data in comma-separated format
  - Format: `neuron_id:value,neuron_id:value,...`
  - Neuron IDs: unsigned integers representing neuron identifiers
  - Values: float values representing neuron states (typically 0.0-1.0)

**Examples:**
```bash
# Store simple experience with two neurons
./nlm --store-memory-episode 1:0.7,2:0.9

# Store multiple neuron states from simulation
./nlm --store-memory-episode 10:0.8,15:0.6,20:0.9,25:0.4
```

**Technical Details:**
- Parses data using `parseCommaSeparated()` helper function
- Creates `EpisodicMemoryItem` with timestamp and neuron-value pairs
- Stores via `NeuralEpisodicMemory::storeEpisode()`
- Timestamp from simulation step

---

### `--replay-memories`

**Description:** Trigger memory replay consolidation to strengthen important memories.

**Parameters:** None

**Examples:**
```bash
# Replay recent memories
./nlm --replay-memories

# Replay after storing new memories
./nlm --store-memory-episode 1:0.9,2:0.8 --replay-memories
```

**Technical Details:**
- Retrieves recent episodes via `NeuralEpisodicMemory::getRecentEpisodes()`
- Injects current into neurons based on stored values
- Runs simulation steps to replay experiences
- Enhances memory consolidation and strength

---

## Neuromodulation Commands

### `--set-dopamine-level <level>`

**Description:** Set dopamine level affecting reward processing and neural excitability.

**Parameters:**
- `<level>`: Dopamine level (0.0-1.0)

**Examples:**
```bash
# Set moderate dopamine level
./nlm --set-dopamine-level 0.7

# Set high dopamine for enhanced learning
./nlm --set-dopamine-level 0.9

# Set low dopamine for stable performance
./nlm --set-dopamine-level 0.2
```

**Technical Details:**
- Calls `Dopamine::setLevel()`
- Affects prediction error processing
- Modulates synaptic plasticity sensitivity

---

### `--set-curiosity-level <level>`

**Description:** Set curiosity level controlling exploration vs. exploitation balance.

**Parameters:**
- `<level>`: Curiosity level (0.0-1.0)

**Examples:**
```bash
# Set moderate curiosity
./nlm --set-curiosity-level 0.5

# Set high curiosity for exploration
./nlm --set-curiosity-level 0.9

# Set low curiosity for focused learning
./nlm --set-curiosity-level 0.1
```

**Technical Details:**
- Calls `Curiosity::setLevel()`
- Affects action selection and exploration probability
- Integrates with reward prediction error

---

### `--set-novelty-level <level>`

**Description:** Set novelty detection threshold for processing new information.

**Parameters:**
- `<level>`: Novelty level (0.0-1.0)
  - Lower values = more sensitive to novelty
  - Higher values = less sensitive to novelty

**Examples:**
```bash
# Set moderate novelty detection
./nlm --set-novelty-level 0.3

# Set low novelty (high sensitivity)
./nlm --set-novelty-level 0.1

# Set high novelty (low sensitivity)
./nlm --set-novelty-level 0.7
```

**Technical Details:**
- Calls `Novelty::setLevel()`
- Affects stimulus evaluation and encoding
- Integrates with episodic memory storage

---

## Development Commands

### `--set-developmental-stage <stage>` (Alias)

**Description:** Alias for `--set-development-stage` command.

**Parameters:** Same as `--set-development-stage`

**Examples:**
```bash
# Both commands are identical
./nlm --set-development-stage adult
./nlm --set-developmental-stage adult
```

---

### `--set-plasticilty-modifiers <hebbian> <stdp> <structural>`

**Description:** Apply multiplicative modifiers to current plasticity rates based on developmental stage.

**Parameters:**
- `<hebbian>`: Hebbian plasticity multiplier
- `<stdp>`: STDP plasticity multiplier  
- `<structural>`: Structural plasticity multiplier

**Examples:**
```bash
# Double all plasticity rates
./nlm --set-plasticilty-modifiers 2.0 2.0 2.0

# Boost STDP while suppressing structural
./nlm --set-plasticilty-modifiers 1.0 3.0 0.5

# Enhance development during maturation
./nlm --set-plasticilty-modifiers 1.5 1.2 0.8
```

**Technical Details:**
- Multiplies current rates by modifier values
- Affects stage-specific plasticity boosts
- Used during developmental transitions

---

### `--enable-critical-period <true/false>`

**Description:** Enable or disable critical period for enhanced plasticity.

**Parameters:**
- `<true/false>`: Enable or disable critical period

**Examples:**
```bash
# Enable critical period for accelerated learning
./nlm --enable-critical-period true

# Disable critical period
./nlm --enable-critical-period false
```

**Technical Details:**
- Calls `DevelopmentSystem::setStage(DevelopmentalStage::CriticalPeriod)`
- Significantly increases plasticity rate multipliers
- Affects structural plasticity and synapse formation

---

## Integration Commands

### `--sync-memory-prediction`

**Description:** Synchronize memory systems with prediction system for better coordination.

**Parameters:** None

**Examples:**
```bash
# Sync memory systems with prediction
./nlm --sync-memory-prediction

# Sync before running integration tests
./nlm --sync-memory-prediction --run-integration-test
```

**Technical Details:**
- Compares working memory contents with prediction system
- Aligns memory states with expected values
- Updates prediction error signals based on memory discrepancies

---

### `--apply-neuromodulation`

**Description:** Apply neuromodulation effects based on prediction errors.

**Parameters:** None

**Examples:**
```bash
# Apply neuromodulation after learning
./nlm --store-memory-episode 1:0.9,2:0.8 --apply-neuromodulation

# Apply neuromodulation with prediction synchronization
./nlm --sync-memory-prediction --apply-neuromodulation
```

**Technical Details:**
- Calculates neuromodulator levels based on prediction error
- Applies dopamine, acetylcholine, and other neuromodulators
- Modulates plasticity based on error magnitude

---

### `--trigger-plasticity-update`

**Description:** Force immediate plasticity updates regardless of step timing.

**Parameters:** None

**Examples:**
```bash
# Trigger plasticity update after configuration changes
./nlm --set-plasticilty-rates 0.1 0.05 0.001 --trigger-plasticity-update

# Force plasticity update for testing
./nlm --run-plasticity-test --trigger-plasticity-update
```

**Technical Details:**
- Calls `Brain::updatePlasticity()` directly
- Updates all plasticity mechanisms immediately
- Bypasses normal step-based plasticity timing

---

## Exploration/Testing Commands

### `--run-memory-test`

**Description:** Run comprehensive memory system tests.

**Parameters:** None

**Output:** Tests working memory retention and decay, episodic memory storage/retrieval, and associative memory strength.

**Examples:**
```bash
# Run memory tests with working memory cleared
./nlm --clear-working-memory --run-memory-test

# Run memory tests after storing episodes
./nlm --store-memory-episode 1:0.9,2:0.8 --run-memory-test

# Run memory tests with modified capacity
./nlm --set-memory-capacity episodic 2000 --run-memory-test
```

**Technical Details:**
- Tests `NeuralWorkingMemory::store()`/`retrieve()` functionality
- Validates `NeuralEpisodicMemory::storeEpisode()`/`retrieveEpisode()`
- Tests `NeuralAssociativeMemory::associate()`/`getAssociationStrength()`
- Reports pass/fail status for each memory system

---

### `--run-plasticity-test`

**Description:** Test synaptic plasticity mechanisms including STDP and Hebbian learning.

**Parameters:** None

**Output:** Tests plasticity effects with correlated pre-post spike activity.

**Examples:**
```bash
# Run plasticity tests with enabled plasticity
./nlm --enable-plasticity true --run-plasticity-test

# Run plasticity tests with modified rates
./nlm --set-plasticilty-rates 0.05 0.02 0.001 --run-plasticity-test
```

**Technical Details:**
- Tests STDP with pre-post spike timing
- Validates Hebbian association strength changes
- Measures synaptic weight delta values
- Reports learning success/failure

---

### `--run-integration-test`

**Description:** Test cross-system integration between memory, prediction, and neuromodulation.

**Parameters:** None

**Output:** Tests integration between development, neuromodulation, and memory-prediction systems.

**Examples:**
```bash
# Run integration tests after configuration changes
./nlm --set-neuromodulator-levels 0.7 0.5 0.3 --run-integration-test

# Comprehensive integration test
./nlm --enable-plasticity true \
     --set-development-stage critical \
     --set-neuromodulator-levels 0.8 0.7 0.6 \
     --run-integration-test
```

**Technical Details:**
- Tests `DevelopmentSystem::getStageSpecificPlasticityBoost()`
- Validates neuromodulation-plasticity integration
- Tests memory-prediction system coordination
- Reports system coherence and integration quality

---

### `--export-brain-state <file>`

**Description:** Export the entire brain state to a file for analysis or checkpointing.

**Parameters:**
- `<file>`: Output file path

**Examples:**
```bash
# Export brain state to binary file
./nlm --export-brain-state brain_state_20240101_120000.bin

# Export after complex learning
./nlm --set-development-stage adult \
     --set-neuromodulator-levels 0.7 0.5 0.3 \
     --export-brain-state learned_brain.bin
```

**Technical Details:**
- Calls `Brain::save()` method
- Serializes neuron states, synapse weights, memory contents
- Includes configuration and developmental state
- Creates checkpoint for restart or analysis

---

### `--import-brain-state <file>`

**Description:** Import brain state from a file to restore previous state.

**Parameters:**
- `<file>`: Input file path

**Examples:**
```bash
# Import from checkpoint
./nlm --import-brain-state brain_state_20240101_120000.bin

# Import and run tests
./nlm --import-brain-state saved_brain.bin --run-all-tests
```

**Technical Details:**
- Calls `Brain::load()` method
- Restores neuron states, synapse weights, memory contents
- Resets brain and reinitializes with loaded state
- Enables restart from previous experimental states

---

## Command Line Argument Summary

### Basic Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `--help` | None | Show help message |
| `--config <file>` | File path | Load configuration from file |
| `--no-tests` | None | Skip running basic tests |
| `--run-all-tests` | None | Run all tests (default) |

### Advanced Brain Management Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `--set-development-stage <stage>` | Stage string | Set developmental stage |
| `--set-neuromodulator-levels <d> <c> <n>` | 3 floats | Set neuromodulator levels |
| `--enable-plasticity <true/false>` | true/false | Enable/disable plasticity |
| `--set-plasticilty-rates <s> <h> <st>` | 3 floats | Set plasticity rates |

### Memory Control Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `--clear-working-memory` | None | Clear working memory traces |
| `--set-memory-capacity <type> <cap>` | Type, capacity | Adjust memory system capacities |
| `--store-memory-episode <data>` | Neuron:value pairs | Store episodic memory |
| `--replay-memories` | None | Trigger memory replay consolidation |

### Neuromodulation Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `--set-dopamine-level <level>` | Float | Set dopamine level (0.0-1.0) |
| `--set-curiosity-level <level>` | Float | Set curiosity level (0.0-1.0) |
| `--set-novelty-level <level>` | Float | Set novelty level (0.0-1.0) |

### Development Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `--set-developmental-stage <stage>` | Stage string | Alias for `--set-development-stage` |
| `--set-plasticilty-modifiers <h> <s> <st>` | 3 floats | Adjust stage-specific plasticity factors |
| `--enable-critical-period <true/false>` | true/false | Enable/disable critical period plasticity |

### Integration Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `--sync-memory-prediction` | None | Synchronize memory systems with prediction system |
| `--apply-neuromodulation` | None | Apply neuromodulation effects |
| `--trigger-plasticity-update` | None | Force plasticity updates |

### Exploration/Testing Commands

| Command | Parameters | Description |
|---------|------------|-------------|
| `--run-memory-test` | None | Test working memory retention and decay |
| `--run-plasticity-test` | None | Test synaptic plasticity effects |
| `--run-integration-test` | None | Test cross-system integration |
| `--export-brain-state <file>` | File path | Export brain state for analysis |
| `--import-brain-state <file>` | File path | Import brain state for testing |

### Configuration File Support

The `--config <file>` command loads configuration from simple key=value files.

**Example Configuration File (`my_config.cfg`):**
```ini
# NLM Configuration
# Basic settings
run_basic_tests = false
export_file = learned_brain.bin

# Development settings
development_stage = adult
enable_critical_period = false

# Neuromodulator levels
dopamine_level = 0.7
curiosity_level = 0.5
novelty_level = 0.3

# Plasticity settings
plasticity_enabled = true
stdp_rate = 0.02
hebbian_rate = 0.01
structural_rate = 0.0005

# Memory settings
working_memory_capacity = 500
episodic_memory_capacity = 1000
associative_memory_capacity = 2000

# Testing
run_memory_test = true
run_plasticity_test = true
run_integration_test = true
```

**Loading Configuration:**
```bash
./nlm --config my_config.cfg
./nlm --config my_config.cfg --store-memory-episode 1:0.9,2:0.8
```

## Usage Scenarios

### Beginner User
```bash
# Basic setup with defaults
./nlm --run-all-tests

# Show help
./nlm --help

# Use configuration file
./nlm --config beginner.cfg
```

### Intermediate User
```bash
# Customize brain dynamics
./nlm --set-neuromodulator-levels 0.7 0.5 0.3 \
     --enable-plasticity true \
     --store-memory-episode 1:0.8,2:0.9

# Test memory systems
./nlm --clear-working-memory --run-memory-test
```

### Advanced User
```bash
# Complex learning scenario
./nlm --enable-critical-period true \
     --set-neuromodulator-levels 0.8 0.7 0.6 \
     --set-plasticilty-modifiers 2.0 1.5 1.0 \
     --sync-memory-prediction \
     --apply-neuromodulation \
     --run-integration-test \
     --export-brain-state advanced_learning.bin
```

## Command Validation

### Parameter Validation

**String Parameters:**
- `--set-development-stage`: Validates against known stage values
- `--set-memory-capacity`: Validates against memory type strings

**Numeric Parameters:**
- All float parameters: Clamped to reasonable ranges (0.0-1.0 for neuromodulators)
- Plasticity rates: Validated as positive values

**File Parameters:**
- `--export-brain-state`: Validates write permissions
- `--import-brain-state`: Validates file exists and can be read

### Error Handling

**Missing Parameters:**
```
ERROR: Missing arguments for --set-neuromodulator-levels
Usage: --set-neuromodulator-levels <dopamine> <curiosity> <novelty>
```

**Invalid Values:**
```
ERROR: Unknown developmental stage: invalid
Valid stages: initial, critical, criticalperiod, maturation, adult, aging
```

**File Errors:**
```
ERROR: Failed to export brain state
Usage: --export-brain-state <file>
```

## Configuration Integration

### Command Line Override

Command line arguments override configuration file values:
```bash
# config.cfg: development_stage = adult
# Command: --set-development-stage critical
# Result: critical (command line wins)
```

### Configuration Priority (highest to lowest)
1. Command line arguments
2. Configuration file
3. Default values

### Configuration Saving

To save current configuration:
```bash
# Export brain state (includes configuration)
./nlm --export-brain-state checkpoint.bin

# Create configuration from current settings
./nlm --set-development-stage adult \
     --set-neuromodulator-levels 0.7 0.5 0.3 \
     --export-brain-state config_snapshot.bin
```

## Command Interaction Patterns

### Sequential Commands
```bash
# Store memory then replay
./nlm --store-memory-episode 1:0.9,2:0.8 --replay-memories

# Clear memory then test
./nlm --clear-working-memory --run-memory-test

# Enable plasticity then test
./nlm --enable-plasticity true --run-plasticity-test
```

### Parallel Commands
```bash
# Multiple independent operations
./nlm --set-development-stage critical \
     --set-neuromodulator-levels 0.7 0.5 0.3 \
     --enable-plasticity true \
     --run-integration-test
```

### Dependent Commands
```bash
# Store memory, clear working memory, then replay
./nlm --store-memory-episode 1:0.9,2:0.8 \
     --clear-working-memory \
     --replay-memories

# Enable critical period, set modifiers, then test
./nlm --enable-critical-period true \
     --set-plasticilty-modifiers 2.0 1.5 1.0 \
     --run-plasticity-test
```

## Timing and Sequencing Considerations

### Command Execution Order
1. **Help/Configuration**: `--help`, `--config` processed first
2. **Basic Settings**: `--set-development-stage`, `--enable-plasticity`
3. **Memory Operations**: `--store-memory-episode`, `--replay-memories`
4. **Neuromodulation**: `--set-neuromodulator-levels`
5. **Development**: `--enable-critical-period`, `--set-plasticilty-modifiers`
6. **Integration**: `--sync-memory-prediction`, `--apply-neuromodulation`
7. **Testing**: `--run-*` commands
8. **Persistence**: `--export-brain-state`, `--import-brain-state`

### State Management

**Commands Affecting Persistent State:**
- `--set-development-stage`
- `--set-neuromodulator-levels`
- `--enable-plasticity`
- `--set-plasticilty-rates`
- `--set-plasticilty-modifiers`
- `--set-memory-capacity`

**Commands Affecting Transient State:**
- `--clear-working-memory`
- `--store-memory-episode`
- `--replay-memories`

## Troubleshooting

### Common Issues and Solutions

**Issue:** Command not recognized
```bash
Solution: Use --help to see all available commands
./nlm --help
```

**Issue:** Invalid parameter values
```bash
Solution: Use valid ranges (0.0-1.0 for neuromodulators)
./nlm --set-neuromodulator-levels 0.7 0.5 0.3
```

**Issue:** Configuration file format error
```bash
Solution: Use simple key=value format
# Correct:
dopamine_level = 0.7

# Incorrect:
dopamine_level=0.7  # Missing spaces around =
```

**Issue:** File permission errors
```bash
Solution: Use different output directory
./nlm --export-brain-state /tmp/brain_state.bin
```

**Issue:** Memory system errors
```bash
Solution: Check available capacity
./nlm --set-memory-capacity episodic 5000 --run-memory-test
```

### Debugging Commands

```bash
# Show current configuration
./nlm --help  # (shows current settings)

# Test specific functionality
./nlm --run-memory-test
./nlm --run-plasticity-test
./nlm --run-integration-test

# Export for debugging
./nlm --export-brain-state debug_brain.bin
```

## Command Line Argument Reference Table

| Command | Short Form | Required Parameters | Optional Parameters | Default Value |
|---------|------------|---------------------|--------------------|---------------|
| `--help` | `-help` | None | None | N/A |
| `--config` | None | 1 (file path) | None | `configs/default.cfg` |
| `--no-tests` | None | None | None | N/A |
| `--run-all-tests` | None | None | None | Yes (default) |
| `--set-development-stage` | None | 1 (stage) | None | `adult` |
| `--set-neuromodulator-levels` | None | 3 (dop, cur, nov) | None | (0.5, 0.5, 0.5) |
| `--enable-plasticity` | None | 1 (true/false) | None | `true` |
| `--set-plasticilty-rates` | None | 3 (stdp, hebbian, structural) | None | (0.02, 0.01, 0.0005) |
| `--clear-working-memory` | None | None | None | N/A |
| `--set-memory-capacity` | None | 2 (type, capacity) | None | System defaults |
| `--store-memory-episode` | None | 1 (data string) | None | N/A |
| `--replay-memories` | None | None | None | N/A |
| `--set-dopamine-level` | None | 1 (level) | None | `0.5` |
| `--set-curiosity-level` | None | 1 (level) | None | `0.5` |
| `--set-novelty-level` | None | 1 (level) | None | `0.5` |
| `--set-plasticilty-modifiers` | None | 3 (hebbian, stdp, structural) | None | (1.0, 1.0, 1.0) |
| `--enable-critical-period` | None | 1 (true/false) | None | `false` |
| `--sync-memory-prediction` | None | None | None | N/A |
| `--apply-neuromodulation` | None | None | None | N/A |
| `--trigger-plasticity-update` | None | None | None | N/A |
| `--run-memory-test` | None | None | None | N/A |
| `--run-plasticity-test` | None | None | None | N/A |
| `--run-integration-test` | None | None | None | N/A |
| `--export-brain-state` | None | 1 (file path) | None | N/A |
| `--import-brain-state` | None | 1 (file path) | None | N/A |

This comprehensive reference covers all advanced NLM commands with detailed parameter descriptions, examples, and technical implementation details.