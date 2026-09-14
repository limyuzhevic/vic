# NLM Phase 6 Advanced Commands

## Quick Reference

```bash
# Run Phase 6 demo with custom configuration
./nlm_phase6_demo --steps 50000 --neurons 2000 --regions 3 --development

# Save and load checkpoint for persistence testing
./nlm_phase6_demo --checkpoint test.bin --load-checkpoint test.bin

# Run with advanced neuromodulation
./nlm_phase6_demo --enable-curiosity --enable-development --enable-prediction

# Phase 6 metrics dump
./nlm_phase6_demo --metrics --output metrics.json
```

## Command Line Options

### Core Simulation

**`--steps N`** (default: 10000)
Number of simulation steps to run. For Phase 6, 50,000+ steps are recommended for development.

**`--neurons N`** (default: 1000)
Total number of neurons in the brain. Increase for more complex cognition.

**`--regions M`** (default: 1)
Number of brain regions. More regions enables regional specialization.

**`--connections P`** (default: 0.1)
Connection probability between neurons (0.0-1.0). Higher values create denser networks.

### Development and Learning

**`--enable-development`**
Enables developmental stages (Initial → CriticalPeriod → Maturation → Adult). Affects plasticity rates.

**`--enable-curiosity`**
Enables curiosity-driven exploration using prediction errors.

**`--enable-prediction`**
Enables prediction system integration. Uses prediction errors to guide learning and exploration.

**`--enable-replay`**
Enables memory replay system for consolidation. Replays important episodes periodically.

### Persistence and Storage

**`--checkpoint PATH`**
Saves brain state to checkpoint file after simulation completes.

**`--load-checkpoint PATH`**
Loads brain state from checkpoint file and continues simulation.

**`--checkpoint-frequency N`**
Saves checkpoint every N steps (default: 1000). Set to 0 to disable.

**`--checkpoint-compression LEVEL`**
Compression level for checkpoints (0=none, 1=fast, 2=balanced, 3=best, default: 2).

### Advanced Configuration

**`--stdp-ltp FLOAT`** (default: 0.01)
Long-term potentiation weight for STDP.

**`--stdp-ltd FLOAT`** (default: 0.012)
Long-term depression weight for STDP.

**`--synaptogenesis-rate FLOAT`** (default: 0.0001)
Rate of new synapse formation during development.

**`--pruning-rate FLOAT`** (default: 0.00001)
Rate of synapse elimination during development.

**`--replay-interval N`**
Replay interval in steps (default: 100). How often to replay episodes.

**`--consolidation-interval N`**
Consolidation interval in steps (default: 1000). How often to consolidate memories.

### Output and Metrics

**`--metrics FILE`**
Saves detailed metrics to JSON file for analysis.

**`--log-level LEVEL`**
Log level (Trace, Debug, Info, Warn, Error, Critical, Off). Default: Info.

**`--verbose`**
Enables verbose logging for debugging integration issues.

**`--summary`**
Prints final integration status summary.

### Development and Testing

**`--development-mode`**
Enables development-specific settings (more plasticity, shorter developmental stages).

**`--test-memory`**
Tests memory system integration specifically.

**`--test-neuromodulation`**
Tests neuromodulation system integration.

**`--test-prediction`**
Tests prediction system integration.

**`--test-cognition`**
Tests cognition system integration (Planner, Concepts, Attention, SelfModel).

**`--health-check`**
Performs comprehensive health check of all integrated systems.

### Performance and Optimization

**`--threads N`**
Number of threads for parallel processing (0 = auto-detect, default: 0).

**`--memory-limit MB`**
Memory limit in MB (0 = unlimited, default: 0).

**`--save-performance`**
Enables performance-optimized checkpoint saving.

## Phase 6 Integration Commands

### Run Complete Integration Test

```bash
./nlm_phase6_demo \
    --steps 50000 \
    --neurons 2000 \
    --regions 3 \
    --connections 0.15 \
    --enable-development \
    --enable-curiosity \
    --enable-prediction \
    --enable-replay \
    --checkpoint phase6_checkpoint.bin \
    --metrics phase6_metrics.json
```

### Development Workflow

```bash
# Stage 1: Test basic integration
./nlm_phase6_demo --steps 5000 --test-memory --test-neuromodulation

# Stage 2: Test learning and development  
./nlm_phase6_demo --steps 20000 --enable-development --enable-curiosity --development-mode

# Stage 3: Full integration with replay and prediction
./nlm_phase6_demo --steps 50000 --enable-prediction --enable-replay --enable-cognition

# Stage 4: Persistence testing
./nlm_phase6_demo --steps 10000 --checkpoint test.bin
./nlm_phase6_demo --load-checkpoint test.bin --steps 10000

# Stage 5: Advanced analysis
./nlm_phase6_demo --steps 100000 --metrics phase6_analysis.json --summary
```

### Batch Processing

```bash
#!/bin/bash
# Run multiple experiments with different configurations
for connections in 0.1 0.15 0.2; do
    ./nlm_phase6_demo \
        --steps 10000 \
        --connections $connections \
        --enable-development \
        --checkpoint phase6_${connections//./}_checkpoint.bin \
        --metrics phase6_${connections//./}_metrics.json
    echo "Completed with connection probability: $connections"
done
```

## Environment Variables

**`NLM_LOG_LEVEL`**
Set default log level (overrides `--log-level`).

**`NLM_NUM_THREADS`**
Set number of threads for parallel processing.

**`NLM_CHECKPOINT_DIR`**
Set default directory for checkpoint files (default: ./checkpoints).

**`NLM_MEMORY_LIMIT_MB`**
Set memory limit for the simulation.

## Configuration Files

### Phase 6 Configuration Templates

Create `phase6_base.cfg`:

```ini
# Base Phase 6 configuration
[nlm]
name = NLM Phase 6 Integration
version = 6.0.0

[simulation]
neuron_count = 2000
region_count = 3
connection_probability = 0.15
max_steps = 50000
simulation_timestep = 0.001

[development]
enabled = true
development_stages = 4
plasticity_modifier = 1.0

[memory]
working_memory_capacity = 200
max_episodes = 5000
replay_interval = 100
consolidation_interval = 1000

[neuromodulation]
enable_dopamine = true
enable_curiosity = true
enable_novelty = true
enable_prediction_error = true

[cognition]
enable_planner = true
enable_concept_formation = true
enable_attention = true
enable_self_model = true

[plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Load and Use:

```bash
./nlm_phase6_demo --config phase6_base.cfg --checkpoint test.bin
```

## Advanced Tips

1. **Memory Management**: Use `--checkpoint-frequency` and `--checkpoint-compression` to balance between storage and speed.

2. **Development Monitoring**: Use `--verbose --log-level Debug` during development to understand system interactions.

3. **Performance Testing**: Use `--threads` and `--save-performance` for production simulations.

4. **Integration Testing**: Use `--health-check` to verify all systems are working together.

5. **Data Analysis**: Use the `--metrics` option to generate JSON reports for automated analysis.

6. **Debugging**: Start with simple tests (`--steps 1000 --verbose`) to identify integration issues before scaling up.

7. **Persistence Strategy**: Combine `--checkpoint-frequency` with `--checkpoint` for recovery from long-running simulations.

8. **Multi-Experiment**: Use batch processing scripts to run parameter sweeps efficiently.

## Technical Notes

- The Phase 6 advanced commands are designed for research and production use
- All options can be combined for comprehensive integration testing
- Checkpoints support compression for efficient storage
- Memory systems scale with neuron count and configuration
- Neuromodulation systems interact automatically based on system state
- Cognition systems use prediction and memory for intelligent behavior

These commands leverage all the integration fixes and improvements made in Phase 6.