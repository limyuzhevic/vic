# NLM Phase 6 Integration - User Guide

## Overview

NLM (熙然, meaning "serene flow") Phase 6 represents a major leap forward in artificial brain development. Unlike previous phases that focused on individual components, Phase 6 integrates all systems into a coherent artificial brain that learns from experience.

**Key Achievement:** The brain now functions as a unified system rather than a collection of disconnected components. All cognitive systems (memory, prediction, planning, concept formation) are connected and working together.

## Quick Start

### Installation and Building

The NLM Phase 6 system uses CMake for building. To get started:

```bash
# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# The demo executables will be available:
./nlm          # Basic Phase 2 neural computation
./nlm_phase6_demo  # Complete Phase 6 integration demo
```

### Running the Phase 6 Integration Demo

The easiest way to see Phase 6 integration in action:

```bash
# Run the Phase 6 demo with default settings
./nlm_phase6_demo

# Run with custom settings
./nlm_phase6_demo --steps 5000 --duration 20 --checkpoint

# Run with configuration file
./nlm_phase6_demo --config my_config.json
```

## Command-Line Interface

The Phase 6 system provides a comprehensive command-line interface for research and experimentation.

### Available Modes

- **`basic`**: Basic neural connectivity test (for debugging)
- **`demo`**: Quick integration verification (recommended for new users)
- **`experiment`**: Full integration experiment with learning (recommended for research)
- **`debug`**: Detailed diagnostics of all systems
- **`benchmark`**: Performance testing and optimization

### Common Command Examples

```bash
# Run quick integration test
./nlm_phase6_demo --mode demo

# Run comprehensive learning experiment
./nlm_phase6_demo --mode experiment --steps 10000 --duration 30

# Run with checkpointing for long experiments
./nlm_phase6_demo --mode experiment --checkpoint --replay

# Debug mode for troubleshooting
./nlm_phase6_demo --mode debug --verbose

# Performance benchmark
./nlm_phase6_demo --mode benchmark --steps 2000
```

### Configuration Options

**Config File Formats:**
- **`key=value`**: Simple text format (existing format)
- **`JSON`**: Structured data format
- **`YAML`**: Human-readable data format

**Example Configuration Files:**

#### Key=Value Format (`config.txt`)
```text
# NLM Phase 6 Configuration
# Brain parameters
neuron_count = 2000
region_count = 2
connection_probability = 0.15

# Neuromodulation
 dopamine_baseline = 0.3
 serotonin_modulation = 0.2
 noradrenaline_decay = 0.1
 acetylcholine_baseline = 0.4

# Memory systems
working_memory_capacity = 500
max_episodes = 2000
consolidation_interval = 500

# Performance
checkpoint_interval = 5000
replay_interval = 100
```

#### JSON Format (`config.json`)
```json
{
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.15,
    "neuromodulation": {
        "dopamine": {"baseline": 0.3, "modulation": 0.2},
        "serotonin": {"baseline": 0.3, "modulation": 0.2},
        "noradrenaline": {"baseline": 0.5, "decay": 0.1},
        "acetylcholine": {"baseline": 0.4, "modulation": 0.15}
    },
    "memory_systems": {
        "working_memory": {"capacity": 500},
        "episodic_memory": {"max_episodes": 2000},
        "consolidation": {"interval": 500}
    },
    "performance": {
        "checkpoint": {"interval": 5000},
        "replay": {"interval": 100}
    }
}
```

#### YAML Format (`config.yaml`)
```yaml
# NLM Phase 6 Configuration
brain:
  neuron_count: 2000
  region_count: 2
  connection_probability: 0.15

neuromodulation:
  dopamine:
    baseline: 0.3
    modulation: 0.2
  serotonin:
    baseline: 0.3
    modulation: 0.2
  noradrenaline:
    baseline: 0.5
    decay: 0.1
  acetylcholine:
    baseline: 0.4
    modulation: 0.15

memory_systems:
  working_memory:
    capacity: 500
  episodic_memory:
    max_episodes: 2000
  consolidation:
    interval: 500

performance:
  checkpoint:
    interval: 5000
  replay:
    interval: 100
```

## Understanding the Integrated Brain

### System Integration Verification

Phase 6 verifies that all systems are properly connected. When running in demo or experiment mode, you'll see output like:

```
=== Integration Verification ===
✓ Working Memory: CONNECTED
✓ Episodic Memory: CONNECTED  
✓ Neuromodulation: CONNECTED
✓ Prediction System: CONNECTED
✓ Neural Planner: CONNECTED
✓ Concept Formation: CONNECTED
✓ Development: CONNECTED
✓ Checkpointing: WORKING
✓ Replay: WORKING
```

### Performance Metrics

Phase 6 tracks and reports key performance metrics:

**Learning Metrics:**
- **Total Reward**: Accumulated reward over the experiment
- **Average Firing Rate**: Neural activity level
- **Memory Episodes Stored**: Number of experiences recorded
- **Dopamine Level**: Reward prediction signal strength

**System Health:**
- **Working Memory Capacity**: Information held in short-term memory
- **Episodic Memory Storage**: Long-term experience storage
- **Prediction Accuracy**: Future state prediction capability
- **Planning Quality**: Action sequence effectiveness

### Memory Systems in Action

The integrated brain demonstrates several key capabilities:

1. **Working Memory**: Holds recent sensory input for immediate processing
2. **Episodic Memory**: Stores complete experiences with timestamps and context
3. **Prediction**: Anticipates future sensory states
4. **Planning**: Sequences actions to achieve goals
5. **Concept Formation**: Extracts patterns and abstractions from experience

## Research and Experimentation

### Setting Up Experiments

The Phase 6 system is designed for scientific research. Here are common experiment patterns:

#### 1. Learning Progress Tracking
```bash
# Track learning over time with checkpoints
./nlm_phase6_demo --mode experiment --steps 10000 --checkpoint --replay

# The system will save checkpoints every 5000 steps
# You can resume interrupted experiments
./nlm_phase6_demo --mode experiment --checkpoint-interval 2500
```

#### 2. Memory Retention Testing
```bash
# Test memory retention with replay
./nlm_phase6_demo --mode experiment --replay --steps 8000

# Enable development stages to see aging effects
./nlm_phase6_demo --mode experiment --development --duration 60
```

#### 3. Neuromodulator Studies
```bash
# Study effects of different neuromodulator levels
./nlm_phase6_demo --mode experiment --dopamine-baseline 0.7
./nlm_phase6_demo --mode experiment --serotonin-modulation 0.5
./nlm_phase6_demo --mode experiment --acetylcholine-baseline 0.8
```

### Analysis and Results

Results from Phase 6 experiments are saved in JSON format:

```
Experiment results saved to: results/experiment_20250603_123456.json

Sample results file format:
{
    "experiment_name": "learning_experiment",
    "seed": 42,
    "duration_seconds": 30.0,
    "total_reward": 125.6,
    "performance_metrics": {
        "avg_firing_rate": 0.15,
        "memory_episodes_stored": 47,
        "prediction_error": 0.08
    },
    "system_integration": {
        "working_memory_connected": true,
        "episodic_memory_connected": true,
        "prediction_connected": true,
        "planner_connected": true,
        "concepts_connected": true
    }
}
```

## Configuration Reference

### Command-Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--mode <mode>` | Execution mode (basic, demo, experiment, debug, benchmark) | `experiment` |
| `--steps <n>` | Number of simulation steps | `1000` |
| `--duration <s>` | Total simulation duration in seconds | `10.0` |
| `--checkpoint` | Enable checkpointing | `true` |
| `--replay` | Enable replay | `true` |
| `--development` | Enable development stages | `true` |
| `--config <file>` | Load configuration from file | `configs/default.cfg` |
| `--config-format <format>` | Config file format (auto, key=value, json, yaml) | `auto` |
| `--seed <n>` | Random seed | `42` |
| `--verbose` | Increase verbosity level | `false` |
| `--help` | Show help information | - |

### Neuromodulator Configuration

Advanced users can configure all neuromodulation systems:

```bash
# Full neuromodulator configuration
./nlm_phase6_demo \
    --dopamine-baseline 0.5 \
    --dopamine-modulation 0.2 \
    --dopamine-decay 0.1 \
    --serotonin-baseline 0.3 \
    --serotonin-modulation 0.2 \
    --serotonin-decay 0.15 \
    --noradrenaline-baseline 0.4 \
    --noradrenaline-modulation 0.25 \
    --noradrenaline-decay 0.1 \
    --acetylcholine-baseline 0.6 \
    --acetylcholine-modulation 0.3 \
    --acetylcholine-decay 0.2
```

## Examples for Advanced Users

### 1. Multi-Agent Experiments
```bash
# Run multiple agents with different configurations
for seed in 42 123 456; do
    ./nlm_phase6_demo --mode experiment \
        --seed $seed \
        --config agent_config_$seed.json
    # Analyze results with custom scripts
    python analyze_results.py results/agent_$seed.json
Done
```

### 2. Developmental Studies
```bash
# Study development effects over time
for duration in 30 60 120 240; do
    ./nlm_phase6_demo --mode experiment \
        --duration $duration \
        --development \
        --replay \
        --output developmental_${duration}s.json
Done
```

### 3. Memory Consolidation Studies
```bash
# Test memory consolidation with different intervals
for interval in 100 500 1000 5000; do
    ./nlm_phase6_demo --mode experiment \
        --consolidation-interval $interval \
        --steps 5000 \
        --replay
    # Compare memory retention across intervals
    python compare_memory.py consolidation_results_${interval}.json
Done
```

### 4. Neuromodulator Interactions
```bash
# Study neuromodulator interactions
./nlm_phase6_demo --mode experiment \
    --dopamine-baseline 0.5 \
    --serotonin-modulation 0.3 \
    --acetylcholine-baseline 0.4

# Modify one neuromodulator to see interaction effects
./nlm_phase6_demo --mode experiment \
    --dopamine-baseline 0.8 \
    --serotonin-modulation 0.3 \
    --acetylcholine-baseline 0.4
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "Integration verification failed"**
```
Solution: Run in debug mode to see which systems are disconnected
./nlm_phase6_demo --mode debug --verbose
```

**Issue: "Configuration file not found"**
```
Solution: Use absolute path or copy file to working directory
cp /path/to/config.json .
./nlm_phase6_demo --config ./config.json
```

**Issue: Slow performance**
```
Solution: Use fewer neurons and steps for testing
./nlm_phase6_demo --mode demo --steps 100

Or use benchmark mode to identify bottlenecks
./nlm_phase6_demo --mode benchmark --steps 500
```

**Issue: Memory usage too high**
```
Solution: Reduce memory system sizes
./nlm_phase6_demo --working-memory-capacity 100 --max-episodes 500
```

## Getting Help

For additional help:

1. **Documentation**: Read the complete documentation in `/docs/`
2. **Examples**: Check `/examples/` for more usage examples
3. **Troubleshooting**: Visit `/troubleshooting/` for common issues
4. **Research**: Refer to `/research/` for scientific background

The NLM Phase 6 system is continuously being improved. If you encounter issues or have questions, please refer to the documentation or contact the development team.

---

**Last Updated:** 2026-10-03
**Version:** Phase 6 Integration
**License:** MIT

This guide provides comprehensive documentation for using the NLM Phase 6 integrated artificial brain system. The system is designed to be accessible for beginners while providing advanced capabilities for research and experimentation.