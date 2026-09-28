# NLM Advanced Usage Guide

This document provides comprehensive guidance for using the advanced brain management commands in NLM (Neural Learning Machine).

## Introduction

NLM Phase 2 introduces advanced command-line options for fine-tuning brain development, learning, and memory systems. This guide covers all advanced commands with practical examples for different user levels.

## Basic Setup

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run with help to see all advanced commands
./nlm --help
```

## Brain Management Commands

### 1. Setting Developmental Stage

**Purpose:** Control age-dependent plasticity and learning capabilities.

**Command:**
```bash
./nlm --set-development-stage <stage>
```

**Parameters:**
- `<stage>`: One of `initial`, `critical`, `criticalperiod`, `maturation`, `adult`, `aging`

**Examples:**
```bash
# Set to critical period for enhanced plasticity
./nlm --set-development-stage critical

# Set to adult stage with normal plasticity
./nlm --set-development-stage adult

# Set to maturation phase
./nlm --set-development-stage maturation
```

**Advanced Usage:**
```bash
# Set critical period with memory testing
./nlm --set-development-stage critical --run-memory-test

# Set adult stage and export brain state
./nlm --set-development-stage adult --export-brain-state adult_brain.bin
```

### 2. Adjusting Neuromodulator Levels

**Purpose:** Fine-tune brain chemistry that affects learning, motivation, and exploration.

**Command:**
```bash
./nlm --set-neuromodulator-levels <dopamine> <curiosity> <novelty>
```

**Parameters:**
- `<dopamine>`: Reward signal (0.0-1.0)
- `<curiosity>`: Exploration drive (0.0-1.0)
- `<novelty>`: Novelty detection (0.0-1.0)

**Examples:**
```bash
# Balanced neuromodulation
./nlm --set-neuromodulator-levels 0.7 0.5 0.3

# High curiosity for exploration
./nlm --set-neuromodulator-levels 0.5 0.9 0.4

# Low novelty detection for focused learning
./nlm --set-neuromodulator-levels 0.8 0.6 0.1
```

**Advanced Usage:**
```bash
# Combine with configuration
./nlm --config custom.cfg --set-neuromodulator-levels 0.6 0.8 0.5 --no-tests

# Set neuromodulation and enable critical period
./nlm --set-neuromodulator-levels 0.9 0.7 0.6 \
     --enable-critical-period true \
     --set-plasticilty-modifiers 2.0 1.5 1.0
```

### 3. Enabling/Disabling Plasticity

**Purpose:** Control when and how synapses can change strength.

**Command:**
```bash
./nlm --enable-plasticity <true/false>
```

**Parameters:**
- `<true/false>`: Enable or disable all plasticity mechanisms

**Examples:**
```bash
# Enable all plasticity for learning
./nlm --enable-plasticity true

# Disable plasticity for stable performance
./nlm --enable-plasticity false
```

**Advanced Usage:**
```bash
# Enable plasticity with specific rates
./nlm --enable-plasticity true \
     --set-plasticilty-rates 0.05 0.02 0.001

# Disable plasticity during memory testing
./nlm --run-memory-test --enable-plasticity false
```

### 4. Setting Plasticity Rates

**Purpose:** Control the speed and magnitude of synaptic changes.

**Command:**
```bash
./nlm --set-plasticilty-rates <stdp> <hebbian> <structural>
```

**Parameters:**
- `<stdp>`: STDP learning rate
- `<hebbian>`: Hebbian learning rate
- `<structural>`: Structural plasticity rate

**Examples:**
```bash
# Standard rates for balanced learning
./nlm --set-plasticilty-rates 0.02 0.01 0.0005

# High plasticity for rapid learning
./nlm --set-plasticilty-rates 0.1 0.05 0.002

# Low plasticity for stable memory
./nlm --set-plasticilty-rates 0.001 0.0005 0.00001
```

**Advanced Usage:**
```bash
# Boost during critical period
./nlm --enable-critical-period true \
     --set-plasticilty-rates 0.1 0.05 0.001

# Suppress plasticity during consolidation
./nlm --enable-plasticity false
```

## Memory Control Commands

### 5. Clearing Working Memory

**Purpose:** Remove transient active information from working memory.

**Command:**
```bash
./nlm --clear-working-memory
```

**Usage:**
```bash
# Clear working memory before new learning task
./nlm --clear-working-memory --run-plasticity-test

# Clear memory before storing new episode
./nlm --clear-working-memory --store-memory-episode 1:0.8,2:0.9
```

### 6. Setting Memory Capacity

**Purpose:** Adjust memory system limits.

**Command:**
```bash
./nlm --set-memory-capacity <type> <capacity>
```

**Parameters:**
- `<type>`: `working`, `episodic`, or `associative`
- `<capacity>`: Memory capacity (system-specific)

**Examples:**
```bash
# Increase episodic memory
./nlm --set-memory-capacity episodic 1000

# Adjust working memory capacity
./nlm --set-memory-capacity working 500

# Set associative memory capacity
./nlm --set-memory-capacity associative 2000
```

### 7. Storing Episodic Memory

**Purpose:** Save experience data to episodic memory.

**Command:**
```bash
./nlm --store-memory-episode <data>
```

**Parameters:**
- `<data>`: Comma-separated neuron:value pairs

**Examples:**
```bash
# Store simple experience
./nlm --store-memory-episode 1:0.7,2:0.9,3:0.5

# Store multiple neuron states
./nlm --store-memory-episode 10:0.8,15:0.6,20:0.9,25:0.4
```

**Advanced Usage:**
```bash
# Store memory and replay it
./nlm --store-memory-episode 1:0.9,2:0.8 --replay-memories

# Clear existing memories first
./nlm --clear-working-memory --store-memory-episode 1:0.5
```

### 8. Replaying Memories

**Purpose:** Trigger memory replay to strengthen important memories.

**Command:**
```bash
./nlm --replay-memories
```

**Usage:**
```bash
# Replay recent memories
./nlm --replay-memories

# Replay after storing new memories
./nlm --store-memory-episode 1:0.9,2:0.8 --replay-memories
```

## Neuromodulation Commands

### 9. Setting Individual Neuromodulator Levels

**Purpose:** Fine-tune specific neuromodulators independently.

**Commands:**
```bash
./nlm --set-dopamine-level <level>
./nlm --set-curiosity-level <level>
./nlm --set-novelty-level <level>
```

**Parameters:** All range from 0.0 to 1.0

**Examples:**
```bash
# High motivation, low exploration
./nlm --set-dopamine-level 0.9 \
     --set-curiosity-level 0.2 \
     --set-novelty-level 0.1

# Balanced neuromodulation
./nlm --set-dopamine-level 0.7 \
     --set-curiosity-level 0.5 \
     --set-novelty-level 0.3
```

## Development Commands

### 10. Enabling Critical Period

**Purpose:** Activate enhanced plasticity phase for accelerated learning.

**Command:**
```bash
./nlm --enable-critical-period <true/false>
```

**Examples:**
```bash
# Enable critical period
./nlm --enable-critical-period true

# Disable critical period
./nlm --enable-critical-period false
```

**Advanced Usage:**
```bash
# Enable critical period with high plasticity
./nlm --enable-critical-period true \
     --set-plasticilty-modifiers 2.0 1.5 1.0

# Critical period with neuromodulation
./nlm --enable-critical-period true \
     --set-neuromodulator-levels 0.8 0.9 0.7
```

### 11. Setting Plasticity Modifiers

**Purpose:** Apply multiplicative factors to plasticity rates.

**Command:**
```bash
./nlm --set-plasticilty-modifiers <hebbian> <stdp> <structural>
```

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
```

## Integration Commands

### 12. Synchronizing Memory and Prediction

**Purpose:** Align memory systems with prediction system.

**Command:**
```bash
./nlm --sync-memory-prediction
```

**Usage:**
```bash
# Sync after learning new memories
./nlm --store-memory-episode 1:0.9,2:0.8 --sync-memory-prediction

# Sync before running integration tests
./nlm --sync-memory-prediction --run-integration-test
```

### 13. Applying Neuromodulation

**Purpose:** Apply neuromodulation effects based on prediction errors.

**Command:**
```bash
./nlm --apply-neuromodulation
```

**Usage:**
```bash
# Apply neuromodulation after learning
./nlm --store-memory-episode 1:0.9,2:0.8 --apply-neuromodulation

# Apply neuromodulation with prediction synchronization
./nlm --sync-memory-prediction --apply-neuromodulation
```

### 14. Triggering Plasticity Update

**Purpose:** Force immediate plasticity updates.

**Command:**
```bash
./nlm --trigger-plasticity-update
```

**Usage:**
```bash
# Trigger plasticity update after changes
./nlm --set-plasticilty-rates 0.1 0.05 0.001 --trigger-plasticivity-update

# Force plasticity update for testing
./nlm --run-plasticity-test --trigger-plasticity-update
```

## Exploration/Testing Commands

### 15. Running Memory Tests

**Purpose:** Comprehensive testing of memory systems.

**Command:**
```bash
./nlm --run-memory-test
```

**Output:** Tests working memory, episodic memory, and associative memory.

**Examples:**
```bash
# Run memory tests with working memory cleared
./nlm --clear-working-memory --run-memory-test

# Run memory tests after storing episodes
./nlm --store-memory-episode 1:0.9,2:0.8 --run-memory-test

# Run memory tests with modified capacity
./nlm --set-memory-capacity episodic 2000 --run-memory-test
```

### 16. Running Plasticity Tests

**Purpose:** Test synaptic plasticity mechanisms.

**Command:**
```bash
./nlm --run-plasticity-test
```

**Output:** Tests STDP and Hebbian learning with correlated activity.

**Examples:**
```bash
# Run plasticity tests with enabled plasticity
./nlm --enable-plasticity true --run-plasticity-test

# Run plasticity tests with modified rates
./nlm --set-plasticilty-rates 0.05 0.02 0.001 --run-plasticity-test
```

### 17. Running Integration Tests

**Purpose:** Test cross-system integration.

**Command:**
```bash
./nlm --run-integration-test
```

**Output:** Tests integration between memory, prediction, and neuromodulation.

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

### 18. Exporting Brain State

**Purpose:** Save brain state for analysis or checkpointing.

**Command:**
```bash
./nlm --export-brain-state <file>
```

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

### 19. Importing Brain State

**Purpose:** Restore brain state from checkpoint.

**Command:**
```bash
./nlm --import-brain-state <file>
```

**Parameters:**
- `<file>`: Input file path

**Examples:**
```bash
# Import from checkpoint
./nlm --import-brain-state brain_state_20240101_120000.bin

# Import and run tests
./nlm --import-brain-state saved_brain.bin --run-all-tests
```

## Configuration Files

### Creating Custom Configuration Files

**Purpose:** Save and load command configurations for consistent experiments.

**Basic Configuration Format:**
```ini
# NLM Configuration File
# Set developmental stage
development_stage = adult

# Neuromodulator levels
 dopamine = 0.7
curiosity = 0.5
novelty = 0.3

# Plasticity settings
enable_plasticity = true
stdp_rate = 0.02
hebbian_rate = 0.01
structural_rate = 0.0005

# Memory settings
working_memory_capacity = 500
episodic_memory_capacity = 1000
associative_memory_capacity = 2000

# Test settings
run_basic_tests = false
run_memory_test = true
run_plasticity_test = true
run_integration_test = true

# Export settings
export_file = learned_brain.bin
```

**Loading Configuration:**
```bash
# Load configuration file
./nlm --config my_experiment.cfg

# Use configuration with additional commands
./nlm --config my_experiment.cfg --store-memory-episode 1:0.9,2:0.8
```

**Saving Configuration:**
```bash
# Save current settings
./nlm --config my_experiment.cfg --export-brain-state test_result.bin

# Create configuration from command line
./nlm --set-development-stage adult \
     --set-neuromodulator-levels 0.7 0.5 0.3 \
     --enable-plasticity true \
     --config my_experiment.cfg
```

## Advanced Command Combinations

### Creating Complex Learning Scenarios

**Example: Critical Period Accelerated Learning**
```bash
./nlm --enable-critical-period true \
     --set-neuromodulator-levels 0.8 0.9 0.7 \
     --set-plasticilty-rates 0.1 0.05 0.002 \
     --enable-plasticity true \
     --run-plasticity-test
```

**Example: Memory Consolidation**
```bash
./nlm --store-memory-episode 1:0.9,2:0.8,3:0.7 \
     --clear-working-memory \
     --replay-memories \
     --sync-memory-prediction
```

**Example: Neuromodulation-Based Learning**
```bash
./nlm --set-neuromodulator-levels 0.9 0.8 0.6 \
     --apply-neuromodulation \
     --trigger-plasticity-update \
     --run-integration-test
```

## Command Line Argument Summary

| Command | Parameters | Description |
|---------|------------|-------------|
| `--help` | None | Show help message |
| `--config <file>` | File path | Load configuration |
| `--no-tests` | None | Skip basic tests |
| `--run-all-tests` | None | Run all tests (default) |
| `--set-development-stage <stage>` | Stage | Set developmental stage |
| `--set-neuromodulator-levels <d> <c> <n>` | 3 floats | Set neuromodulator levels |
| `--enable-plasticity <true/false>` | true/false | Enable/disable plasticity |
| `--set-plasticilty-rates <s> <h> <st>` | 3 floats | Set plasticity rates |
| `--clear-working-memory` | None | Clear working memory |
| `--set-memory-capacity <type> <cap>` | Type, capacity | Set memory capacity |
| `--store-memory-episode <data>` | Neuron:value pairs | Store episodic memory |
| `--replay-memories` | None | Replay memories |
| `--set-dopamine-level <level>` | Float | Set dopamine level |
| `--set-curiosity-level <level>` | Float | Set curiosity level |
| `--set-novelty-level <level>` | Float | Set novelty level |
| `--set-plasticilty-modifiers <h> <s> <st>` | 3 floats | Set plasticity modifiers |
| `--enable-critical-period <true/false>` | true/false | Enable critical period |
| `--sync-memory-prediction` | None | Sync memory and prediction |
| `--apply-neuromodulation` | None | Apply neuromodulation |
| `--trigger-plasticity-update` | None | Force plasticity update |
| `--run-memory-test` | None | Run memory tests |
| `--run-plasticity-test` | None | Run plasticity tests |
| `--run-integration-test` | None | Run integration tests |
| `--export-brain-state <file>` | File path | Export brain state |
| `--import-brain-state <file>` | File path | Import brain state |

## Troubleshooting

### Common Issues

**Issue:** Command not recognized
```bash
# Solution: Use --help to see all available commands
./nlm --help
```

**Issue:** Invalid parameter values
```bash
# Solution: Use valid ranges (0.0-1.0 for neuromodulators)
./nlm --set-neuromodulator-levels 0.7 0.5 0.3
```

**Issue:** Configuration file not found
```bash
# Solution: Create config file or use default
cat > my_config.cfg
./nlm --config my_config.cfg
```

**Issue:** Brain state export fails
```bash
# Solution: Ensure write permissions to output directory
./nlm --export-brain-state /tmp/brain_state.bin
```

### Getting Help

```bash
# Show basic help
./nlm --help

# Show advanced commands help
./nlm  # (when no commands specified)

# Get help for specific command (if implemented)
./nlm --set-development-stage help
```

## Best Practices

1. **Use configuration files** for complex experiments to maintain consistency
2. **Always test with `--no-tests`** for faster development iterations
3. **Save checkpoints** before major changes to brain configuration
4. **Monitor integration tests** after configuration changes
5. **Combine related commands** for efficient workflows
6. **Document your experiments** in configuration files for reproducibility

## Next Steps

After mastering basic advanced commands:

1. Learn to create **custom configuration files** for automated experiments
2. Explore **command combination patterns** for complex learning scenarios
3. Use **memory testing** to validate system behavior
4. Implement **checkpoint/restart** for long-running experiments
5. Consider **batch processing** multiple commands in scripts

This guide covers all advanced NLM commands. For more technical details, refer to the technical documentation in `NLM_Advanced_Features.md`.