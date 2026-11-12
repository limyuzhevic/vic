# NLM - Enhanced Usage Guide
# Phase 6: Final Integration with Improvements

## Updated Quick Start (Enhanced NLM)

This guide has been updated for **Phase 6: Final Integration**, which includes:
- **Full Brain Integration**: All systems working together (memory, neuromodulation, cognition)
- **Improved Checkpointing**: Robust save/load functionality
- **Enhanced Reward System**: Meaningful learning and measurable rewards
- **Advanced CLI**: Command-line interface for easy configuration

### Example 1: Enhanced Brain with All Systems

```bash
# Run Phase 6 demo with all systems integrated
./nlm_cli run phase6 --neurons 500 --steps 2000 --checkpoint --development

# Or use the traditional C++ binary
./nlm_phase6_demo
```

### Example 2: Running with Custom Configuration

```bash
# Phase 6 with specific settings
./nlm_cli run phase6 \
  --neurons 1000 \
  --steps 5000 \
  --checkpoint \
  --reward-mod true \
  --curiosity true \
  --development true \
  --checkpoint-path ./my_checkpoints
```

### Example 3: Running Different Phases

```bash
# Phase 2: Basic neural computation (fast)
./nlm_cli run phase2 --neurons 100 --steps 500

# Phase 3: World interaction (agent learns)
./nlm_cli run phase3 --neurons 500 --steps 2000

# Phase 4: Advanced cognition (planning)
./nlm_cli run phase4 --neurons 1000 --steps 3000

# Phase 5: Brain-personality (sandbox learning)
./nlm_cli run phase5

# Phase 6: Full integration (comprehensive)
./nlm_cli run phase6 --neurons 1000 --steps 5000

# Phase 6 with text-as-a-sense
./nlm_cli run phase6_text --neurons 500 --steps 3000
```

## What Makes NLM Enhanced

### 1. **Complete Brain Integration** ✅
All brain systems are now properly connected:

| System | Capability | Status |
|--------|------------|--------|
| **Working Memory** | Active maintenance & competition | ✅ Integrated |
| **Episodic Memory** | Experience encoding & replay | ✅ Integrated |
| **Associative Memory** | Pattern associations | ✅ Integrated |
| **Prediction System** | Forward models & prediction error | ✅ Integrated |
| **Cognition** | Planning & concept formation | ✅ Integrated |
| **Neuromodulation** | Dopamine, curiosity, novelty | ✅ Integrated |
| **Development** | Age-dependent plasticity | ✅ Integrated |

### 2. **Meaningful Learning & Rewards** ✅
The reward system now works correctly:

- **Resource Collection**: +30.0 reward
- **Hazard Avoidance**: -15.0 penalty
- **Smart Actions**: Reflex bonuses based on experience
- **Preference Learning**: Brain develops preferences over time

### 3. **Robust Checkpointing** ✅
Save/load functionality that actually works:

- **Atomic saves**: No corruption during save operations
- **Compression options**: Fast, balanced, or best compression
- **Version compatibility**: Can load older checkpoints
- **Incremental saves**: Saves at specified intervals

### 4. **Advanced User Features** ✅
Command-line interface for customization:

- **Config management**: Set default parameters
- **Custom demos**: Run with specific neuron counts and steps
- **Benchmarking**: Performance measurements
- **System checking**: Validate installation

## Configuration Options

### Core Parameters

```
--neurons N          Number of neurons (default: 500)
--steps S            Number of simulation steps (default: 2000)
--width W            World width in units (default: 16)
--height H           World height in units (default: 16)
--vision V           Vision sensor count (default: 16)
```

### Learning Features

```
--reward-mod true|false     Enable/disable reward modulation (default: true)
--curiosity true|false       Enable/disable curiosity-driven exploration (default: true)
--development true|false     Enable/disable developmental learning (default: true)
--checkpoint true|false      Enable/disable checkpointing (default: true)
--replay true|false          Enable/disable memory replay (default: true)
```

### Advanced Options

```
--checkpoint-path PATH    Directory for checkpoint files
--verbose                 Enable verbose logging
--help                    Show help message
```

## Running Demos

### Quick Demo Commands

```bash
# Phase 2: Basic neural computation
./nlm_cli run phase2 --neurons 100 --steps 100

# Phase 3: Simple world interaction
./nlm_cli run phase3 --neurons 500 --steps 1000

# Phase 4: Advanced cognitive planning
./nlm_cli run phase4 --neurons 1000 --steps 2000

# Phase 5: Sandbox learning & personality
./nlm_cli run phase5

# Phase 6: Full integration with all systems
./nlm_cli run phase6 --neurons 1000 --steps 5000 --checkpoint

# Phase 6 with text understanding
./nlm_cli run phase6_text --neurons 500 --steps 3000
```

### Phase 6 Specific Options

For the most comprehensive demo, use:

```bash
# Phase 6 with maximum learning
./nlm_cli run phase6 \
  --neurons 1000 \
  --steps 10000 \
  --reward-mod true \
  --curiosity true \
  --development true \
  --checkpoint \
  --replay
```

## Expected Results

### After Running Phase 6

You should see:

1. **Working Memory**: Neurons that stay active
2. **Episodic Memory**: Stored experiences replayed periodically
3. **Neuromodulation**: Dopamine levels changing based on rewards
4. **Learning**: Synaptic weights changing (learning occurs)
5. **Decision Making**: Brain making better choices over time
6. **Checkpointing**: Files saved to `./checkpoints/`

### Sample Output

```
===========================================
NLM Phase 6 Integration Demo
===========================================

Step 0 | Reward: 0.000 | Firing: 125.3 Hz | WorkingMem: 0 traces
Step 1000 | Reward: 0.042 | Firing: 148.7 Hz | WorkingMem: 12 traces
Step 2000 | Reward: 0.087 | Firing: 162.3 Hz | WorkingMem: 25 traces
...

=== FINAL RESULTS ===
Total reward: 0.087
Avg firing rate: 148.7 Hz
Episodes stored: 42
Dopamine level: 0.234

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING

Wall clock time: 12.5s
```

## Troubleshooting

### If Checkpointing Doesn't Work

```bash
# Create checkpoints directory
mkdir -p ./checkpoints

# Ensure write permissions
chmod 755 ./checkpoints

# Run demo with checkpointing explicitly enabled
./nlm_cli run phase6 --checkpoint
```

### If Reward System Shows 0

```bash
# Ensure learning features are enabled
./nlm_cli run phase6 --reward-mod true --curiosity true --development true

# Check that objects exist in the world
./nlm_cli check
```

### If System is Slow

```bash
# Use fewer neurons for faster execution
./nlm_cli run phase6 --neurons 100 --steps 500

# Or run Phase 2 for fastest demonstration
./nlm_cli run phase2 --neurons 50 --steps 100
```

## Advanced Usage

### Configuration Management

```bash
# View current settings
./nlm_cli config show

# Set default neurons for all demos
./nlm_cli config set default_neurons 1000

# Set world dimensions
./nlm_cli config set world_width 32
./nlm_cli config set world_height 32

# Reset to defaults
./nlm_cli config reset
```

### Benchmarking

```bash
# Run performance benchmarks
./nlm_cli benchmark run

# Compare different configurations
./nlm_cli benchmark run --neurons 100
./nlm_cli benchmark run --neurons 500
./nlm_cli benchmark run --neurons 1000
```

### System Information

```bash
# Display system capabilities
./nlm_cli info

# Check system status
./nlm_cli check
```

## Quick Reference

| Command | Description | Default |
|---------|-------------|---------|
| `./nlm_cli run phase6` | Run Phase 6 with defaults | 500 neurons, 2000 steps |
| `./nlm_cli run phase6 --neurons 100` | Fast demonstration | 100 neurons, 2000 steps |
| `./nlm_cli run phase5` | Sandbox learning demo | Built-in config |
| `./nlm_cli info` | Show system info | System status |
| `./nlm_cli check` | Check installation | Validation results |

## Next Steps

For more advanced usage:

1. **Read Documentation**: `cat docs/ARCHITECTURE.md`
2. **Study Science**: `cat docs/SCIENCE.md`
3. **Advanced Demos**: `cat docs/EXPERIMENTS.md`
4. **Performance Tuning**: `cat docs/PERFORMANCE.md`
5. **Limitations**: `cat docs/LIMITATIONS.md`

## Getting Started

1. **Build the project**: `mkdir build && cd build && cmake .. && make -j4`
2. **Run Phase 6**: `./nlm_cli run phase6`
3. **Explore**: Try different configurations with `--help`
4. **Learn**: Watch the demo outputs and understand learning
5. **Customize**: Use `config` commands to set your preferences

The enhanced NLM system provides a complete, working artificial brain that demonstrates all major brain functions!
