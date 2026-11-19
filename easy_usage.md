# NLM Command Line Interface

## Overview

A comprehensive command-line interface for NLM with advanced features for both beginners and expert users.

## Usage

### Basic Usage

```bash
# Run the main neural computation demo
./nlm

# Run specific phase demos
./nlm_phase3_demo [episodes] [steps]
./nlm_phase4_demo [trials]
./nlm_phase6_demo

# Run the pattern probe + sandbox + checkpoint demo
./nlm_phase5_demo

# Run Phase 6 text/teaching demo
./nlm_phase6_text_demo

# Run benchmark
./nlm_benchmark
```

### Advanced Configuration

```bash
# Full configuration options
./nlm --help
./nlm --config configs/advanced.cfg
./nlm --neuron-count 2000 --connection-prob 0.05 --simulation-timestep 0.0005
```

## Commands

### Core Commands

#### `nlm`
Phase 2: Basic neural computation with STDP learning.

**Arguments:**
- `[episodes]`: Number of learning episodes (default: 5)
- `[steps]`: Steps per episode (default: 200)

**Advanced Options:**
```bash
./nlm --neuron-count 1000 --region-count 2 --connection-prob 0.1
./nlm --enable-stdp --enable-hebbian --enable-structural
./nlm --save-checkpoint my_checkpoint
```

#### `nlm_phase3_demo`
Phase 3: World interaction and behavior.

**Arguments:**
- `[episodes]`: Number of episodes (default: 5)
- `[steps]`: Steps per episode (default: 200)

**Advanced Options:**
```bash
./nlm_phase3_demo --enable-curiosity --enable-structural-plastic --enable-development
./nlm_phase3_demo --environment-width 50 --environment-height 50
./nlm_phase3_demo --world-update-interval 0.05
```

#### `nlm_phase4_demo`
Phase 4: Emerging cognitive mechanisms.

**Arguments:**
- `[trials]`: Number of trials per experiment (default: 50)

**Advanced Options:**
```bash
./nlm_phase4_demo --enable-concept-formation --enable-planning --enable-social-learning
./nlm_phase4_demo --prediction-trial-count 200 --memory-capacity 50
./nlm_phase4_demo --disable-attention-focus --enable-self-model
```

#### `nlm_phase5_demo`
Phase 5: Advanced brain personality and checkpointing.

This demo demonstrates three advanced capabilities:
1. **Pattern Probe**: Shows the brain's ability to discriminate between patterns
2. **Sandbox Run**: Real-world interaction with learning
3. **Checkpoint Round-trip**: Saves and reloads brain state

**Advanced Options:**
```bash
./nlm_phase5_demo --probe-mode extended --sandbox-steps 500 --checkpoint-verification
./nlm_phase5_demo --enable-neuromodulation --enable-development
./nlm_phase5_demo --output-dir ./phase5_results --save-detailed-checkpoint
```

#### `nlm_phase6_demo`
Phase 6: Complete brain integration and verification.

**Advanced Options:**
```bash
./nlm_phase6_demo --verify-integration-only --skip-sandbox
./nlm_phase6_demo --enable-memory-debug --enable-neuromodulation-debug
./nlm_phase6_demo --output-csv phase6_results.csv --generate-visualizations
./nlm_phase6_demo --enable-replay-debug --enable-development-debug
```

#### `nlm_phase6_text_demo`
Phase 6: Text teaching and advanced cognition.

**Advanced Options:**
```bash
./nlm_phase6_text_demo --text-encoding-length 32 --enable-text-innovation
./nlm_phase6_text_demo --planner-precision high --enable-concept-learning
./nlm_phase6_text_demo --social-learning-intensity advanced
```

### Configuration Commands

#### `nlm-config`
Manage NLM configurations.

**Subcommands:**
- `list`: List all available configurations
- `show <name>`: Display configuration details
- `create <name> <file>`: Create new configuration
- `set <name> <key> <value>`: Update configuration value
- `validate <file>`: Validate configuration file

**Examples:**
```bash
nlm-config list
nlm-config show expert_neural
nlm-config create my_brain configs/expert_neural.cfg
nlm-config set expert_neural neuron_count 5000
```

### Analysis Commands

#### `nlm-analyze`
Analyze NLM simulation results.

**Subcommands:**
- `pattern-discrimination <file>`: Analyze pattern separation
- `learning-curves <file>`: Plot learning over time
- `behavior-stats <file>`: Statistical analysis of behavior
- `memory-metrics <file>`: Memory system performance analysis

**Examples:**
```bash
nlm-analyze pattern-discrimination phase3_results.dat
nlm-analyze learning-curves sandbox_output.dat --output-plot learning.png
nlm-analyze behavior-stats phase6_data.csv --stats-summary
```

### Development Commands

#### `nlm-dev`
Development and debugging tools.

**Subcommands:**
- `benchmark <config>`: Run performance benchmarks
- `profile <demo>`: Profile demo execution
- `debug <component>`: Enable debugging for specific component
- `replay <checkpoint> <steps>`: Replay from checkpoint

**Examples:**
```bash
nlm-dev benchmark configs/expert_neural.cfg
nlm-dev profile nlm_phase3_demo --profile-memory --profile-cpu
nlm-dev debug neuromodulation --debug-level verbose
nlm-dev replay /tmp/nlm_checkpoint.bin 1000
```

## Configuration File Format

### Basic Configuration

```json
{
    "name": "expert_neural_brain",
    "version": "1.0",
    "description": "High-performance neural simulation",
    
    "neural": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.08,
        "simulation_timestep": 0.0005,
        "max_spike_frequency": 200.0
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 30.0
    },
    
    "memory": {
        "working_memory_capacity": 100,
        "episodic_memory_limit": 50,
        "replay_frequency": 0.1
    },
    
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true,
        "attention_window_size": 10
    },
    
    "environment": {
        "world_width": 100,
        "world_height": 100,
        "vision_range": 16,
        "action_interval": 0.1
    },
    
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_curiosity": true,
        "enable_novelty": true,
        "dopamine_sensitivity": 1.0
    }
}
```

### Advanced Configuration

```json
{
    "name": "research_experiment",
    "neural": {
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.05,
        "sparse_connectivity": true,
        "enable_synaptic_gates": true
    },
    
    "development": {
        "enable_development": true,
        "developmental_stages": [
            "initial",
            "critical_period",
            "maturation",
            "adult",
            "aging"
        ],
        "plasticity_rate_by_stage": {
            "initial": 1.0,
            "critical_period": 2.5,
            "maturation": 1.0,
            "adult": 0.5,
            "aging": 0.2
        }
    },
    
    "advanced_features": {
        "enable_multithreading": true,
        "thread_count": 8,
        "enable_simd": true,
        "enable_event_driven": true,
        "checkpoint_interval": 10000,
        "enable_replay": true,
        "replay_window": 1000
    },
    
    "output": {
        "enable_profiling": true,
        "profile_interval": 100,
        "save_checkpoints": true,
        "checkpoint_path": "./checkpoints",
        "generate_visualizations": true,
        "visualization_formats": ["png", "svg", "pdf"],
        "output_csv": true,
        "csv_path": "./results"
    }
}
```

## Environment Variables

### NLM Environment

```bash
# Core configuration
export NLM_NEURON_COUNT=2000
export NLM_CONNECTION_PROB=0.1
export NLM_SIMULATION_TIMESTEP=0.001

# Learning and plasticity
export NLM_ENABLE_STDP=true
export NLM_ENABLE_HEBBIAN=true
export NLM_ENABLE_STRUCTURAL=true
export NLM_STDP_LTP_WEIGHT=0.02
export NLM_STDP_LTD_WEIGHT=0.015

# Memory systems
export NLM_WORKING_MEMORY_CAPACITY=100
export NLM_EPISODIC_MEMORY_LIMIT=50
export NLM_ENABLE_REPLAY=true

# Cognition
export NLM_ENABLE_CONCEPT_FORMATION=true
export NLM_ENABLE_PLANNING=true
export NLM_ENABLE_ATTENTION=true

# Neuromodulation
export NLM_ENABLE_DOPAMINE=true
export NLM_ENABLE_CURIOUSITY=true
export NLM_ENABLE_NOVELTY=true

# Environment and behavior
export NLM_WORLD_WIDTH=50
export NLM_WORLD_HEIGHT=50
export NLM_VISION_RANGE=16
export NLM_ACTION_INTERVAL=0.1

# Development
export NLM_ENABLE_DEVELOPMENT=true
export NLM_DEVELOPMENT_STAGE=adult

# Output and debugging
export NLM_ENABLE_PROFILING=true
export NLM_LOG_LEVEL=info
export NLM_OUTPUT_DIR=./results
export NLM_SAVE_CHECKPOINTS=true
```

## Scripting Interface

### NLM Batch Processing

Create `batch_process.nml`:

```bash
#!/bin/bash

# Process multiple configurations
echo "Starting NLM batch processing..."

# Create output directory
mkdir -p batch_results

# Process different brain sizes
for neurons in 500 1000 2000 5000; do
    echo "Processing brain with $neurons neurons..."
    ./nlm_phase6_demo \
        --neuron-count $neurons \
        --enable-memory-debug \
        --enable-neuromodulation-debug \
        --output-csv batch_results/brain_${neurons}_neurons.csv
    
    # Run pattern discrimination test
    ./nlm_phase5_demo \
        --probe-mode extended \
        --output-dir batch_results/brain_${neurons}_neurons
    
    echo "Completed $neurons neuron brain"
    echo ""
done

# Process different learning scenarios
echo "Processing learning scenarios..."

./nlm_phase3_demo \
    --enable-curiosity \
    --enable-structural-plastic \
    --environment-width 100 \
    --environment-height 100 \
    --episodes 20 \
    --steps 500

./nlm_phase4_demo \
    --enable-concept-formation \
    --enable-planning \
    --enable-social-learning \
    --trials 100

echo "Batch processing complete!"
```

### Configuration Generation

```bash
#!/bin/bash

# Generate configuration templates
generate_nlm_config() {
    local config_name="$1"
    local output_file="${2:-configs/${config_name}.cfg}"
    
    cat > "$output_file" << EOF
{
    "name": "${config_name}",
    "description": "Generated ${config_name} configuration",
    
    "neural": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012
    },
    
    "environment": {
        "world_width": 50,
        "world_height": 50,
        "vision_range": 16,
        "action_interval": 0.1
    }
}
EOF
    
    echo "Generated configuration: $output_file"
}

# Generate preset configurations
generate_nlm_config "beginner_brain" "configs/beginner.cfg"
generate_nlm_config "expert_brain" "configs/expert.cfg"
generate_nlm_config "research_brain" "configs/research.cfg"
generate_nlm_config "minimal_brain" "configs/minimal.cfg"
```

## Performance and Optimization

### Command Line Performance Options

```bash
# High-performance mode
./nlm_phase6_demo \
    --enable-multithreading \
    --thread-count 8 \
    --enable-simd \
    --enable-sparse-connectivity

# Memory-efficient mode
./nlm_phase6_demo \
    --neuron-count 1000 \
    --connection-prob 0.05 \
    --enable-sparse-connectivity

# Real-time monitoring
./nlm_phase6_demo \
    --enable-profiling \
    --profile-interval 50 \
    --output-csv real_time_results.csv
```

## Extensibility and Plugin System

### Writing NLM Plugins

```cpp
// Plugin interface example
#include <nlm/core/PluginInterface.hpp>

class MyPlugin : public nlm::IPlugin {
public:
    std::string name() const override { return "my_neural_plugin"; }
    std::string version() const override { return "1.0"; }
    
    void initialize(const nlm::Config& config) override {
        // Plugin initialization
    }
    
    void process(nlm::Brain* brain, const nlm::SensoryPercept& input) override {
        // Plugin processing
    }
    
    void cleanup() override {
        // Plugin cleanup
    }
};

extern "C" nlm::IPlugin* create_plugin() {
    return new MyPlugin();
}
```

## Examples and Templates

### Expert Configuration Template

```json
{
    "name": "expert_neural_brain",
    "version": "2.0",
    "expert_mode": true,
    
    "neural": {
        "neuron_count": 10000,
        "region_count": 5,
        "connection_probability": 0.05,
        "enable_advanced_dynamics": true,
        "enable_sparse_connectivity": true,
        "enable_event_driven": true,
        "max_spike_frequency": 400.0,
        "refractory_period": 0.002
    },
    
    "plasticity": {
        "enable_stdp": true,
        "enable_hebbian": true,
        "enable_structural": true,
        "enable_homeostatic": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 30.0,
        "homeostatic_target": 1.0
    },
    
    "memory": {
        "working_memory_capacity": 200,
        "episodic_memory_limit": 100,
        "enable_replay": true,
        "replay_window": 2000,
        "replay_frequency": 0.2,
        "enable_conjunctive_memory": true
    },
    
    "cognition": {
        "enable_concept_formation": true,
        "enable_planning": true,
        "enable_attention": true,
        "attention_selectivity": 0.8,
        "enable_self_model": true,
        "self_model_confidence": 0.9
    },
    
    "neuromodulation": {
        "enable_dopamine": true,
        "enable_curiosity": true,
        "enable_novelty": true,
        "dopamine_sensitivity": 1.5,
        "curiosity_drive": 0.8,
        "novelty_threshold": 0.1
    },
    
    "development": {
        "enable_development": true,
        "developmental_stages": [
            "initial", "critical_period", "maturation", "adult", "aging"
        ],
        "plasticity_rate_by_stage": {
            "initial": 2.0,
            "critical_period": 4.0,
            "maturation": 2.0,
            "adult": 1.0,
            "aging": 0.5
        }
    },
    
    "advanced_features": {
        "enable_multithreading": true,
        "thread_count": 12,
        "enable_simd": true,
        "enable_event_driven": true,
        "checkpoint_interval": 5000,
        "enable_profiling": true,
        "profile_interval": 100,
        "generate_visualizations": true,
        "visualization_formats": ["png", "svg", "pdf"],
        "output_csv": true,
        "csv_path": "./expert_results"
    }
}
```

## Quick Reference

### Command Line Options

| Command | Description | Key Arguments |
|---------|-------------|---------------|
| `nlm` | Basic neural computation | `--neuron-count`, `--connection-prob` |
| `nlm_phase3_demo` | World interaction | `--enable-curiosity`, `--environment-width` |
| `nlm_phase4_demo` | Cognitive mechanisms | `--enable-concept-formation`, `--enable-planning` |
| `nlm_phase5_demo` | Advanced personality + checkpointing | `--probe-mode`, `--sandbox-steps` |
| `nlm_phase6_demo` | Complete integration | `--verify-integration-only`, `--enable-memory-debug` |
| `nlm_phase6_text_demo` | Text teaching | `--text-encoding-length`, `--enable-text-innovation` |

### Configuration Keys

| Category | Key | Type | Default | Description |
|----------|-----|------|---------|-------------|
| Neural | `neuron_count` | int | 500 | Number of neurons |
| Neural | `connection_probability` | float | 0.1 | Synapse formation probability |
| Plasticity | `enable_stdp` | bool | true | Enable STDP learning |
| Memory | `working_memory_capacity` | int | 50 | Working memory size |
| Cognition | `enable_planning` | bool | true | Enable planning module |
| Environment | `world_width` | int | 50 | World width |
| Neuromodulation | `enable_dopamine` | bool | true | Enable dopamine system |
| Development | `enable_development` | bool | true | Enable developmental system |
| Output | `output_csv` | bool | true | Save results as CSV |

## Conclusion

The NLM CLI provides a comprehensive interface for:

- **Easy Use**: Simple commands for beginners
- **Expert Control**: Advanced configuration for research
- **Batch Processing**: Automated multi-experiment workflows
- **Performance**: Optimized for large-scale simulations
- **Extensibility**: Plugin system for custom functionality
- **Analysis**: Built-in analysis and visualization tools

Start with `nlm --help` for the complete command reference, or use `nlm-config` for configuration management.
