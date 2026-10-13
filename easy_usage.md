# NLM - Enhanced Easy Usage Guide for Beginners

## Quick Start (NEW - Enhanced Version)

### Simplified One-Liner Examples

#### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

# One line to create a virtual brain with preset configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

#### Example 2: Quick Simulation with Statistics

```python
import pynlm

# Create and run a simulation with automatic statistics
brain = pynlm.create_brain_for_task("simple")
brain._initialize_default()

# Run 1000 steps and get automatic statistics
stats = brain.run_simulation(1000, verbose=True)
print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Average firing rate: {stats[2]:.2f} Hz")
```

#### Example 3: Complete Agent in One Go

```python
import pynlm

# Create a complete learning agent with all systems
brain, world, agent = pynlm.setup_complete_agent("learning", "adaptive")

# Run the agent
results = pynlm.run_agent_simulation(brain, world, agent, 500, verbose=True)
print("Agent simulation complete!")
print(f"Performance: {results[2]:.2f} avg firing rate")
```

## The 3 Things You Need to Know (ENHANCED)

### 1. **Brain** - The virtual brain that thinks
- **Enhanced**: Now includes convenience methods for initialization and simulation
- **New**: `brain._initialize_default()` - Quick initialization
- **New**: `brain.run_simulation()` - Complete simulation with statistics

### 2. **World** - The environment the brain lives in
- **Enhanced**: Better configuration options
- **New**: `pynlm.create_simple_world_with_settings()` - Custom world setup
- **New**: `pynlm.setup_complete_agent()` - World setup included

### 3. **Agent** - The bridge connecting brain to world
- **Enhanced**: Common behavior modes
- **New**: `pynlm.create_agent_for_environment()` - Preset agents
- **New**: Multiple behavior modes: "exploratory", "conservative", "adaptive"

## Quick Start (Copy & Paste - ENHANCED)

### Enhanced Example 1: Simplest Brain

```python
import pynlm

# Create brain with preset configuration (NEW)
brain = pynlm.create_brain_for_task("simple")

# Initialize it (NEW - convenience method)
brain._initialize_default()

# Make it think for 100 steps (NEW - includes statistics)
stats = brain.run_simulation(100, verbose=False)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Average firing rate:", stats[2], "Hz")
```

### Enhanced Example 2: Brain in a Simple World

```python
import pynlm

# Create everything (ENHANCED with setup_complete_agent)
brain, world, agent = pynlm.setup_complete_agent("simple", "exploratory")

# Run for 50 steps with automatic stats
results = pynlm.run_agent_simulation(brain, world, agent, 50)

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Average firing rate:", results[2], "Hz")
```

### Enhanced Example 3: Complete Agent with Learning

```python
import pynlm

# Setup with learning configuration and adaptive behavior
brain, world, agent = pynlm.setup_complete_agent("learning", "adaptive")

# Run agent with verbose output
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print("Average firing rate:", brain.getAverageFiringRate(), "Hz")
```

## Common Patterns (ENHANCED)

### Pattern 1: Enhanced Simulation with Statistics

```python
def run_enhanced_simulation(brain, num_steps, verbose=False):
    """Run a simulation with automatic statistics and health checks."""
    # Check if brain is ready
    health = brain.check_health()
    if not health.isHealthy:
        print(f"Warning: {health.statusMessage}")
        return None
    
    # Run simulation with statistics
    return brain.run_simulation(num_steps, verbose)

# Use it like this:
brain = pynlm.create_brain_for_task("learning")
brain._initialize_default()
stats = run_enhanced_simulation(brain, 1000)
```

### Pattern 2: Agent Setup with Error Checking

```python
def setup_agent_safely(task_type="learning", behavior="adaptive"):
    """Setup agent with error checking and validation."""
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Setup world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Setup agent with enhanced convenience
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all systems for learning
        agent.enable_all_systems()
        
        return brain, world, agent
        
    except Exception as e:
        print(f"Setup failed: {e}")
        return None, None, None

# Use it like this:
brain, world, agent = setup_agent_safely()
if brain is not None:
    print("Agent setup successful!")
```

### Pattern 3: Enhanced Configuration

```python
def configure_for_task(task_type):
    """Configure brain for specific task types."""
    config = pynlm.createDefaultConfig()
    
    if task_type == "simple":
        config.set_safe("brain.neuron_count", "100")
        config.set_safe("brain.synapse_density", "0.05")
    elif task_type == "complex":
        config.set_safe("brain.neuron_count", "2000")
        config.set_safe("brain.synapse_density", "0.15")
        config.set_safe("plasticity.stdp.enable", "true")
        config.set_safe("plasticity.hebbian.enable", "true")
    
    return config

# Use it like this:
config = configure_for_task("complex")
brain = pynlm.createBrain(config)
```

## The Simplest Possible Example (ENHANCED)

```python
import pynlm

# Create brain with preset configuration (NEW)
brain = pynlm.create_brain_for_task("simple")

# Initialize it (NEW - convenience method)
brain._initialize_default()

# Make it active (NEW - returns firing count)
firing_count = brain.step(0)

print("Your enhanced brain has", brain.getTotalNeuronCount(), "neurons!")
print("Firing neurons:", firing_count)
```

## Enhanced Common Patterns

### Pattern 4: Performance Monitoring

```python
def run_with_monitoring(brain, num_steps):
    """Run simulation with automatic performance monitoring."""
    # Profile brain performance
    metrics = pynlm.profile_brain_performance(brain, measurementSteps=100)
    
    print(f"Brain performance:")
    print(f"  - Stability: {metrics.stabilityClassification}")
    print(f"  - Avg firing rate: {metrics.avgFiringRate:.2f} Hz")
    print(f"  - Coefficient of variation: {metrics.coefficientOfVariation:.3f}")
    
    # Run simulation
    stats = brain.run_simulation(num_steps, verbose=True)
    
    # Analyze results
    error_analysis = pynlm.analyze_brain_errors(brain)
    print(f"  - Health: {'Good' if error_analysis.confidence > 0.9 else 'Needs attention'}")
    
    return stats, metrics
```

### Pattern 5: Enhanced Agent Lifecycle

```python
def enhanced_agent_lifecycle(task_type="learning", behavior="adaptive"):
    """Complete agent lifecycle with setup, training, and analysis."""
    print(f"=== Enhanced Agent Lifecycle: {behavior} behavior ===")
    
    # 1. Setup
    print("1. Setting up agent...")
    brain, world, agent = pynlm.setup_complete_agent(task_type, behavior)
    agent_status = agent.get_status_report()
    print(f"   ✓ Agent ready: {agent_status.statusMessage}")
    
    # 2. Training
    print("2. Training agent...")
    training_results = pynlm.run_agent_simulation(
        brain, world, agent, 300, verbose=False
    )
    print(f"   ✓ Training complete: {training_results[2]:.2f} avg firing rate")
    
    # 3. Analysis
    print("3. Analyzing performance...")
    metrics = pynlm.profile_brain_performance(brain)
    print(f"   ✓ Performance: {metrics.stabilityClassification} stability")
    
    # 4. Reset and optimization
    print("4. Optimizing agent...")
    agent.reset_complete()
    print("   ✓ Agent reset complete")
    
    return brain, world, agent
```

### Pattern 6: Batch Operations

```python
def batch_agent_training(brain, world, agent, batch_size=10, epochs=5):
    """Train multiple batches of agents for statistical analysis."""
    all_results = []
    
    for epoch in range(epochs):
        print(f"Epoch {epoch + 1}/{epochs}")
        
        # Create new agent for each batch
        epoch_agent = pynlm.create_agent_for_environment(brain, "adaptive")
        epoch_agent.initialize(world)
        
        # Run batch
        results = pynlm.run_agent_simulation(
            brain, world, epoch_agent, batch_size, verbose=False
        )
        
        # Analyze batch performance
        metrics = pynlm.profile_brain_performance(brain)
        
        all_results.append({
            'epoch': epoch + 1,
            'avg_firing': results[2],
            'total_spikes': results[1],
            'stability': metrics.stabilityClassification,
            'confidence': metrics.confidence
        })
        
        print(f"  Batch {epoch + 1}: {results[2]:.2f} avg firing, {metrics.stabilityClassification} stability")
    
    return all_results
```

## Enhanced Quick Reference

| What you want | Enhanced Code | Description |
|--------------|---------------|-------------|
| Create brain | `pynlm.create_brain_for_task("learning")` | Preset configurations |
| Start brain | `brain._initialize_default()` | Quick initialization |
| Make brain think | `stats = brain.run_simulation(100)` | With automatic statistics |
| Create world | `pynlm.create_simple_world_with_settings(20, 20, 8, 8)` | Custom settings |
| Create agent | `pynlm.setup_complete_agent("learning", "adaptive")` | Full setup included |
| Get brain stats | `brain.check_health()` | Health diagnostics |
| See world | `world.update(0.1)` | With timing |
| Make action | `action = agent.decode_motor_command()` | Enhanced decoding |
| Run batch | `brain.batch_step(0, 100)` | Multiple steps |

## Enhanced Troubleshooting

### "My brain isn't doing anything"

**Enhanced solutions:**
- Did you call `brain._initialize_default()`?
- Try increasing the number of steps with `brain.run_simulation()`
- Check `brain.check_health()` for issues

### "The agent isn't moving"

**Enhanced solutions:**
- Did you call `agent.initialize()`?
- Check that `world.update()` is being called
- Use `agent.get_status_report()` for debugging

### "Everything is 0"

**Enhanced solutions:**
- Brains need time to "warm up" - try more steps with `run_simulation()`
- Some neurons need input - use `brain.inject_current_batch()` for testing
- Check `brain.check_health()` for initialization issues

## Enhanced Mini Projects

### Mini Project 1: Silent Enhanced Brain

```python
import pynlm

brain = pynlm.create_brain_for_task("simple")
brain._initialize_default()

for i in range(10):
    brain.step(i)

print("Enhanced silent brain test done!")
```

### Mini Project 2: Enhanced Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.create_brain_for_task("learning")
brain._initialize_default()
world = pynlm.create_simple_world_with_settings(20, 20, 8, 8)
world.reset()
agent = pynlm.create_agent_for_environment(brain, "exploratory")
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched enhanced world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Mini Project 3: Enhanced Learning Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain._initialize_default()
world = pynlm.create_simple_world_with_settings(15, 15, 6, 6)
world.reset()
agent = pynlm.create_agent_for_environment(brain, "adaptive")
agent.initialize(world)

# Enable learning
agent.enable_all_systems()

# Run agent with monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Enhanced agent simulation complete!")
```

## Enhanced Quick Reference Table

| What you want | Enhanced Code |
|--------------|---------------|
| Create brain | `pynlm.create_brain_for_task("learning")` |
| Start brain | `brain._initialize_default()` |
| Make brain think | `stats = brain.run_simulation(100)` |
| Create world | `pynlm.create_simple_world_with_settings(20, 20, 8, 8)` |
| Create agent | `pynlm.setup_complete_agent("learning", "adaptive")` |
| Get brain stats | `brain.check_health()` |
| See world | `world.update(0.1)` |
| Make action | `action = agent.decode_motor_command()` |
| Run batch | `brain.batch_step(0, 100)` |
| Configure preset | `config._create_preset("learning")` |
| Analyze performance | `pynlm.profile_brain_performance(brain)` |

## Next Steps (ENHANCED)

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - Enhanced C++ documentation
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Enhanced architecture docs
3. **Run `enhanced_examples.py`** for comprehensive examples
4. **Explore `EnhancedAPIDocumentation.md`** for detailed API reference
5. **Experiment with different configuration presets** - `get_available_presets()` shows options
6. **Try behavior modes** - `get_behavior_modes()` shows available options

That's it! You're now ready to use the **enhanced NLM** with all the new features, convenience methods, and advanced tools!

## Version Note

This is the **Enhanced NLM API Version**. It includes:
- 25+ new convenience methods
- Configuration presets for different tasks
- Advanced agent behavior modes
- Performance monitoring and analysis tools
- Enhanced error handling and validation
- Full backward compatibility with original API

Enjoy exploring the enhanced NLM capabilities!