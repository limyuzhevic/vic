NLM (Neural Learning Machine) - Configuration Options Reference

## Configuration Options for NLM

This document describes all available configuration options for NLM, organized by category.

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Phase 2: Real Neural Computation

Phase 2 focuses on implementing real spiking neural computation. These settings control the core neural dynamics.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep (seconds) |
| `simulation.max_steps` | int | 1000 | Maximum simulation steps |
| `phase2.neuron_count` | int | 500 | Neuron count for Phase 2 (smaller for faster testing) |
| `phase2.enable_stdp` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `phase2.enable_hebbian` | bool | true | Enable Hebbian learning |

### Phase 3: World Integration

Phase 3 adds world interaction and sensory-motor loops.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.width` | int | 20 | World width |
| `world.height` | int | 20 | World height |
| `world.enable_ai` | bool | true | Enable AI-controlled agent |
| `world.reward_explore` | float | 1.0 | Reward for exploration |
| `world.reward_goal` | float | 10.0 | Reward for reaching goal |

### Phase 4: Cognitive Integration

Phase 4 integrates cognitive systems like working memory, attention, and prediction.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.working_memory_size` | int | 100 | Size of working memory |
| `cognition.attention_strength` | float | 0.5 | Attention inhibition strength |
| `cognition.planning_depth` | int | 3 | Planning depth for neural planner |

### Phase 5: Performance

Phase 5 optimizes performance and adds checkpointing.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.checkpoint_interval` | int | 100 | Checkpoint save interval |
| `performance.compress_checkpoints` | bool | true | Compress checkpoint files |
| `performance.use_memory_pools` | bool | true | Use memory pools for efficiency |
| `performance.max_threads` | int | 4 | Maximum threads for parallel processing |

### Phase 6: Final Integration

Phase 6 integrates all systems into a complete artificial brain.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `integration.enable_all_systems` | bool | true | Enable all integrated systems |
| `integration.memory_integration` | bool | true | Integrate memory systems |
| `integration.neuromodulation` | bool | true | Enable neuromodulation effects |
| `integration.development` | bool | true | Enable developmental stages |
| `integration.prediction` | bool | true | Enable prediction system |
| `integration.curiosity` | bool | true | Enable curiosity-driven learning |
| `integration.replay` | bool | true | Enable episodic memory replay |

### Performance Monitoring

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `monitoring.enable_stats` | bool | true | Enable performance statistics |
| `monitoring.log_level` | int | 2 | Logging level (0=Debug, 1=Info, 2=Warning, 3=Error) |
| `monitoring.profile_interval` | int | 100 | Performance profiling interval |
| `monitoring.stats_file` | string | "stats.json" | File for statistics output |

### Debug and Development

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `debug.enable_debug_mode` | bool | false | Enable debug mode |
| `debug.enable_profiling` | bool | false | Enable profiling tools |
| `debug.validation_checks` | bool | true | Enable validation checks |
| `debug.log_to_file` | bool | false | Log to file instead of stdout |

### Advanced Options for Power Users

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `advanced.custom_neurons` | bool | false | Use custom neuron types |
| `advanced.custom_plasticity` | bool | false | Use custom plasticity rules |
| `advanced.enable_fast_simulation` | bool | false | Enable optimized simulation mode |
| `advanced.hardware_acceleration` | bool | false | Enable hardware acceleration |
| `advanced.max_memory_usage` | int | 4096 | Maximum memory usage (MB) |

## Command-Line Options

### Common Options

- `--help, -h`: Show help message
- `--version, -v`: Show version information
- `--config <file>`: Load configuration from file
- `--phase <number>`: Run specific phase (1-6)
- `--steps <number>`: Number of simulation steps
- `--verbose, -v`: Enable verbose output
- `--quiet, -q`: Suppress output
- `--benchmark`: Run benchmark mode
- `--validate`: Validate configuration

### Phase-Specific Options

#### Phase 2 Options

- `--phase2.neuron-count <int>`: Number of neurons
- `--phase2.enable-stdp`: Enable STDP
- `--phase2.enable-hebbian`: Enable Hebbian learning
- `--phase2.timestep <float>`: Simulation timestep

#### Phase 3 Options

- `--phase3.world-width <int>`: World width
- `--phase3.world-height <int>`: World height
- `--phase3.enable-ai`: Enable AI agent
- `--phase3.goal-reward <float>`: Reward for reaching goal

#### Phase 4 Options

- `--phase4.memory-size <int>`: Working memory size
- `--phase4.attention-strength <float>`: Attention strength
- `--phase4.planning-depth <int>`: Planning depth

#### Phase 5 Options

- `--phase5.checkpoint-interval <int>`: Checkpoint interval
- `--phase5.use-memory-pools`: Use memory pools
- `--phase5.max-threads <int>`: Maximum threads

#### Phase 6 Options

- `--phase6.enable-all-systems`: Enable all integrated systems
- `--phase6.memory-integration`: Integrate memory systems
- `--phase6.curiosity`: Enable curiosity
- `--phase6.replay`: Enable replay

### Advanced Options

- `--config <file>`: Custom configuration file
- `--load-state <file>`: Load saved brain state
- `--save-state <file>`: Save brain state
- `--random-seed <int>`: Random seed
- `--duration <int>`: Simulation duration in seconds
- `--output-dir <dir>`: Output directory
- `--stats <file>`: Statistics output file

## Configuration File Format

Configuration files are JSON format:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    },
    "phase2": {
        "neuron_count": 500,
        "enable_stdp": true,
        "enable_hebbian": true
    },
    "phase3": {
        "world_width": 20,
        "world_height": 20,
        "enable_ai": true
    },
    "performance": {
        "checkpoint_interval": 100,
        "use_memory_pools": true
    }
}
```

## Environment Variables

NLM can be configured via environment variables (prefixed with `NLM_`):

- `NLM_BRAIN_NEURON_COUNT`: Total number of neurons
- `NLM_PHASE2_ENABLE_STDP`: Enable STDP (true/false)
- `NLM_PERFORMANCE_MAX_THREADS`: Maximum threads
- `NLM_DEBUG_ENABLE_DEBUG_MODE`: Enable debug mode
- `NLM_STATS_FILE`: Statistics output file

Example:
```bash
export NLM_BRAIN_NEURON_COUNT=2000
export NLM_PHASE2_ENABLE_STDP=true
export NLM_PERFORMANCE_MAX_THREADS=8
./nlm --phase 6
```

## File Modification Tracking

This file tracks configuration changes:
- `2026-09-21`: Initial configuration options
- `2026-09-21`: Added Phase 6 integration options
- `2026-09-21`: Added performance monitoring options

## Related Files

- `CMakeLists.txt`: Build configuration
- `configs/`: Example configuration files
- `docs/`: Detailed documentation
- `README.md`: Quick start guide
- `HOW_TO_USE.md`: Detailed usage instructions
- `easy_usage.md`: Beginner's guide