# NLM Python API - IMPROVED EDITION

## Quick Start for New Users

Welcome to the improved NLM Python API! These enhancements make neural brain simulation much easier to use and more powerful for both beginners and advanced users.

## What\u2019s New

### 1. 🚀 One-Line Brain Creation
```python
import pynlm

# Simple brain with 500 neurons (like a small human brain)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate 100 steps instantly
for step in range(100):
    brain.step(step)

print(f"Brain thought {brain.getTotalSpikeCount()} times!")
```

### 2. 🤖 Quick Agent Setup
```python
import pynlm
from pynlm_extended import create_simple_agent

# Create brain, world, and agent in ONE line
brain, agent, world = create_simple_agent()

# Run a complete simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 3. 🧠 Advanced Training Sessions
```python
import pynlm
from training_session import AdvancedTrainingSession

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run advanced training with monitoring
trainer = AdvancedTrainingSession(brain, world, agent, "my_training")
results = trainer.run_training_episode(steps=500, verbose=True)

print(f"Training complete! Average firing rate: {results['average_firing_rate']:.3f}")
```

## Key Improvements

### 📚 Better Documentation
- **Easy Usage**: New `easy_usage.md` with beginner-friendly examples
- **Comprehensive Guide**: Enhanced `HOW_TO_USE.md` with detailed instructions
- **Example Gallery**: Working examples for every major feature

### 🛠️ Enhanced API
- **Factory Methods**: `create_simple_agent()`, `run_simple_simulation()`
- **Builder Pattern**: `NLMBuilder` for complex configurations
- **Convenience Functions**: `batch_run_simulations()`, `create_curiosity_driven_agent()`

### 🔧 Smart Defaults
```python
from pynlm_extended import NLMBuilder

# Create a brain with optimal settings for learning
builder = NLMBuilder()
builder.set_brain_size(1000, 3)                    # 1000 neurons, 3 regions
builder.set_plasticity(stdp=True, hebbian=True)   # Enable learning
builder.enable_curiosity(True, 0.8)                # Exploration drive
builder.set_learning_rate(stdp_rate=0.01)         # Faster learning

brain = builder.create_brain()
```

### 📊 Advanced Monitoring
```python
from training_session import AdvancedTrainingSession

# Real-time monitoring during training
trainer = AdvancedTrainingSession(brain, world, agent, "research")
trainer.start_monitoring(interval=1.0)  # Monitor every second

# Run training
for episode in range(10):
    result = trainer.run_training_episode(steps=100)
    print(f"Episode {episode}: Curiosity={result['curiosity_level']:.3f}")

# Get comprehensive statistics
stats = trainer.get_training_statistics()
print(f"Stability Index: {stats['aggregate_metrics']['firing_rate_stability']:.3f}")
```

### 🚀 Batch Processing
```python
from pynlm_extended import batch_run_simulations
import pynlm

# Run 5 simulations with different parameters
simulations = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500 + i * 100)
    
    simulations.append({
        'name': f'sim_{i}',
        'config': config,
        'steps': 100
    })

# Run all simulations
results = batch_run_simulations(simulations)
print(f"Completed {len(results)} simulations successfully!")
```

## For Different User Levels

### 👑 BEGINNER (Getting Started)
```python
# Use these functions for quick learning
brain, agent, world = create_simple_agent()
run_simple_simulation(100)
quick_test()  # Just tests the basic functionality
```

### 🎓 INTERMEDIATE (Experimenting)
```python
# Use these functions for controlled experiments
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Custom configuration
brain.getConfig().set("plasticity.stdp.learning_rate", 0.05)

# Run simulation with world interaction
for step in range(500):
    world.update(0.1)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### 🚀 ADVANCED (Research & Development)
```python
# Use these functions for advanced research
from pynlm_extended import NLMBuilder, TrainingSession

# Complex configuration with builder pattern
builder = NLMBuilder()
builder.set_brain_size(2000, 5)
builder.set_plasticity(structural=True, reward_modulated=True)
builder.enable_developmental_maturation(True)
builder.enable_curiosity(True, 1.5)

# Advanced training with monitoring
trainer = TrainingSession(brain, world, agent, "research")
trainer.train_multiple_episodes(
    num_episodes=20,
    steps_per_episode=1000,
    save_checkpoints=True,
    verbose=False
)

# Analyze results
stats = trainer.get_performance_summary()
print(f"Final developmental stage: {stats['final_developmental_stage']}")
```

## New Files Added

### 📁 pynlm_extended.py
- High-level abstractions and convenience functions
- NLMBuilder, SimulationContext, TrainingSession classes
- Factory functions for common use cases

### 📁 training_session.py  
- Advanced training session management
- Real-time monitoring and checkpointing
- Performance analysis and comparison tools

### 📁 test_improvements.py
- Comprehensive test suite for all improvements
- Validation of error handling and edge cases
- Performance benchmarks

### 📁 IMPROVEMENTS_SUMMARY.md
- Complete documentation of all improvements
- Technical details and migration guides
- Future enhancement roadmap

## Installation

### Basic Installation (Unchanged)
```bash
pip install .
```

### Development Installation (With All Improvements)
```bash
pip install -e .  # Installs with all new extensions
```

### Quick Test
```bash
python test_improvements.py
```

## Backwards Compatibility

✅ **Fully Compatible**: All existing code continues to work unchanged
✅ **Optional Imports**: New functionality available via imports
✅ **Gradual Adoption**: Add new features as needed

## Troubleshooting

### Common Issues

**Q: "My brain isn't doing anything"
A:**
- Make sure you call `brain.initialize()` before `brain.step()`
- Check if your brain has enough neurons for activity
- Try adding sensory input with `agent.processSensoryInput()`

**Q: "The agent isn't learning"
A:**
- Enable learning subsystems: `agent.enableRewardModulation(True)`
- Check curiosity settings: `agent.enableCuriosity(True)`
- Verify reward signals are being generated

**Q: "My simulation is too slow"
A:**
- Use batch processing for multiple simulations
- Reduce brain size for quick testing
- Enable optimization settings in builder

### Getting Help

1. **Documentation**: Check `README.md`, `HOW_TO_USE.md`, `easy_usage.md`
2. **Examples**: Look at `test_improvements.py` for working examples
3. **Community**: Check the project documentation for community resources

## Next Steps

1. **Start Simple**: Use `create_simple_agent()` for quick experiments
2. **Learn Gradually**: Progress from basic examples to advanced features
3. **Experiment**: Use `NLMBuilder` for custom configurations
4. **Analyze**: Use `TrainingSession` for comprehensive research
5. **Scale**: Use `batch_run_simulations()` for large-scale studies

## Quick Reference

### Core Functions
```python
# Basic creation (unchanged)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Enhanced creation (new)
brain = pynlm.createBrain(pynlm.createStandardConfig())
```

### Factory Functions (New)
```python
# Quick agent setup
brain, agent, world = create_simple_agent()

# Simple simulation
result = run_simple_simulation(100)

# Curiosity-driven agent
brain, agent, world = create_curiosity_driven_agent()

# Batch processing
results = batch_run_simulations(simulation_configs)
```

### Builder Pattern (New)
```python
# Custom brain configuration
builder = NLMBuilder()
builder.set_brain_size(1000, 3)
builder.set_plasticity(stdp=True, hebbian=True)
builder.enable_curiosity(True)

brain = builder.create_brain()
```

### Training Sessions (New)
```python
# Basic training
from training_session import AdvancedTrainingSession
trainer = AdvancedTrainingSession(brain, world, agent, "my_research")
result = trainer.run_training_episode(100)

# Advanced training with monitoring
results = trainer.train_multiple_episodes(10, 500)
stats = trainer.get_training_statistics()
```

## Conclusion

The NLM Python API is now significantly more accessible and powerful. Whether you're a beginner wanting to experiment with neural brains, an intermediate developer building simulations, or an advanced researcher conducting complex experiments, you'll find tools that make your work easier and more productive.

**Key Benefits:**
- ✅ **Easier to start** - Quick setup functions for beginners
- ✅ **More powerful** - Advanced features for research
- ✅ **Better documentation** - Comprehensive examples and guides
- ✅ **Backwards compatible** - Existing code continues to work
- ✅ **Future-ready** - Solid foundation for future enhancements

Get started today and discover what's possible with neural brain simulation! 🎯