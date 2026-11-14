# NLM Enhanced Command-Line Interface

## Overview

The enhanced NLM command-line interface provides comprehensive control over neural simulation experiments with advanced configuration options, performance monitoring, and debugging capabilities.

## Available Commands

### Basic Commands

`nlm --help` - Display this help message
`nlm --version` - Display version information
`nlm --list-experiments` - List available experiment types

### Phase Experiments

`nlm --phase2` - Run Phase 2 basic neural dynamics demo
`nlm --phase3` - Run Phase 3 world interaction demo  
`nlm --phase4` - Run Phase 4 cognitive integration demo
`nlm --phase5` - Run Phase 5 performance optimization demo
`nlm --phase6` - Run Phase 6 final integration demo (enhanced)

### Advanced Commands

`nlm --benchmark --neurons 5000 --steps 5000` - Run performance benchmark
`nlm --continuous --timestep 0.001 --duration 60` - Run continuous simulation
`nlm --checkpoint save/path --load save/path` - Save and load brain states
`nlm --config custom_config.json` - Load custom configuration file
`nlm --debug --log-level debug --log-file simulation.log` - Enhanced debugging

### Python Bindings Test

`nlm --python-test` - Test Python bindings functionality
`nlm --python-benchmark` - Benchmark Python bindings performance

## Configuration Options

### Common Options

`--neurons N` - Number of neurons (default: 1000)
`--regions R` - Number of brain regions (default: 1)
`--steps S` - Number of simulation steps (default: 10000)
`--timestep T` - Simulation timestep in seconds (default: 0.001)
`--seed SEED` - Random seed for reproducible results

### Phase-Specific Options

**Phase 2 (Basic Neural Dynamics)**
`--phase2 --plasticity` - Enable plasticity mechanisms
`--phase2 --development` - Enable developmental processes

**Phase 3 (World Interaction)**
`--phase3 --world-width W --world-height H` - World dimensions
`--phase3 --vision-width VW --vision-height VH` - Sensory dimensions
`--phase3 --agent-speed S` - Agent movement speed

**Phase 4 (Cognitive Integration)**
`--phase4 --planning-depth D` - Neural planning depth
`--phase4 --attention-strength A` - Attention modulation strength
`--phase4 --memory-capacity M` - Memory system capacity

**Phase 5 (Performance)**
`--phase5 --simd` - Enable SIMD optimizations
`--phase5 --checkpoint-interval C` - Checkpoint interval
`--phase5 --checkpoint-compression COMP` - Checkpoint compression level

**Phase 6 (Final Integration)**
`--phase6 --goals G` - Number of goals to pursue
`--phase6 --hazards H` - Number of hazards in environment
`--phase6 --enable-reward` - Enable enhanced reward system
`--phase6 --enable-curiosity` - Enable curiosity-driven exploration
`--phase6 --enable-development` - Enable developmental processes

### Debugging and Monitoring

`--log-level LEVEL` - Set logging level (trace, debug, info, warn, error, critical)
`--log-file FILE` - Log output file
`--performance-metrics` - Enable performance monitoring
`--memory-usage` - Monitor memory usage
`--cpu-usage` - Monitor CPU usage
`--validate-integration` - Validate system integration

## Advanced Usage Examples

### 1. Enhanced Phase 6 with Custom Goals

```bash
nlm --phase6 --neurons 2000 --steps 5000 \
    --goals 3 --hazards 5 --enable-reward --enable-curiosity \
    --log-level info --performance-metrics
```

### 2. Performance Benchmark

```bash
nlm --benchmark --neurons 10000 --steps 20000 \
    --timestep 0.0005 --simd --checkpoint-interval 1000
```

### 3. Debugging with Custom Configuration

```bash
nlm --phase6 --debug --config my_config.json \
    --log-file simulation.log --validate-integration
```

### 4. Python Bindings Testing

```bash
nlm --python-test --neurons 500 --steps 1000
```

## Configuration File Format

NLM supports JSON configuration files. Example `config.json`:

```json
{
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.05,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    
    "plasticity": {
        "stdp_enable": true,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "hebbian_enable": true
    },
    
    "neuromodulation": {
        "dopamine_scale": 1.0,
        "curiosity_enable": true,
        "novelty_enable": true
    },
    
    "memory": {
        "working_memory_capacity": 200,
        "episodic_memory_limit": 1000
    },
    
    "development": {
        "enable_development": true,
        "maturation_rate": 0.001
    },
    
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    }
}
```

## Python API Examples

### Basic Usage

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("connection_probability", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Save brain state
brain.save("my_brain.bin")
```

### Advanced Usage with Agent

```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run enhanced simulation
for step in range(5000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on agent performance
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    if step % 500 == 0:
        print(f"Step {step}: Reward={reward:.2f}, "
              f"Firing={brain.getFiringNeuronCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}")
```

### Performance Monitoring

```python
import pynlm
import time

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

brain = pynlm.createBrain(config)
start_time = time.time()

brain.initialize()
for step in range(10000):
    brain.step(step)
    
    # Collect performance metrics
    if step % 1000 == 0:
        elapsed = time.time() - start_time
        spikes_per_step = brain.getTotalSpikeCount() / (step + 1)
        print(f"Step {step}: {spikes_per_step:.1f} spikes/step, "
              f"{elapsed:.1f}s elapsed")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "ImportError: No module named 'pynlm'"
Solution:**
```bash
pip install pynlm
# or for development:
pip install -e .
```

**Issue: "Segmentation fault" when running demos
Solution:**
```bash
# Ensure all dependencies are installed
./nlm_phase6_demo  # Use the correct demo executable
```

**Issue: "Checkpoint save/load failed"
Solution:**
```bash
# Ensure write permissions to checkpoint directory
nlm --phase6 --checkpoint "./checkpoints"
```

**Issue: "Too many neurons, out of memory"
Solution:**
```bash
nlm --phase6 --neurons 1000  # Start with fewer neurons
nlm --phase6 --neurons 5000 --simd  # Use SIMD for larger brains
```

## Performance Tips

1. **Use SIMD:** Enable SIMD optimizations for better performance
2. **Tune Timestep:** Smaller timesteps may be needed for stability
3. **Monitor Memory:** Use `--memory-usage` to track memory consumption
4. **Checkpoint Regularly:** Use frequent checkpoints to avoid data loss
5. **Use Configuration Files:** Keep experiment settings in config files

## Next Steps

1. **Explore Documentation:** Read `docs/ARCHITECTURE.md` for detailed system architecture
2. **Run Examples:** Try the examples in this guide
3. **Customize Configuration:** Create custom config files for specific experiments
4. **Performance Tuning:** Use benchmarking tools to optimize performance
5. **Extend Python API:** Add custom Python bindings for advanced use cases

## Feedback and Support

For issues, questions, or feature requests:
- Check documentation in `docs/` directory
- Report issues on the GitHub repository
- Join the NLM community forums
- Submit enhancement requests through the project tracker

## License

This software is licensed under the MIT License. See `LICENSE` file for details.
