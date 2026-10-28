# NLM - Enhanced Easy Usage Guide for Beginners and Experts

**UPDATED VERSION** - This guide now includes both the original basic usage and the **NEW enhanced Python API features** that make NLM more powerful, Pythonic, and easier to use!

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

**NEW: Now with Pythonic interface, callbacks, and advanced features!**

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in  
3. **Agent** - The bridge connecting brain to world

---

## 🚀 Quick Start (Enhanced Version)

### Example 1: Simplest Brain (OLD vs NEW)

**OLD WAY:**
```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

**NEW WAY - Enhanced with Pythonic properties:**
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# NEW: Use Pythonic property access instead of getters
print(f"Brain has {brain.neuron_count} neurons")
print(f"Stage: {brain.stage}")
print(f"Firing: {brain.firing_count} neurons currently firing")

# NEW: Use context manager for automatic resource management
with brain:
    brain.step(100)  # 100 simulation steps with automatic cleanup

# NEW: Get comprehensive performance metrics
metrics = brain.get_performance_metrics()
print(f"Simulation completed: {metrics['total_spikes']} total spikes")
```

### Example 2: Brain in a Simple World (Enhanced)

**ENHANCED VERSION with AgentBrain improvements:**
```python
import pynlm

# Create everything with enhanced interfaces
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# NEW: Use high-level episode running method
agent.run_episode(world, 50)  # One complete episode automatically

print("Simulation finished!")
print(f"Firing neurons: {brain.firing_count}")
print(f"Agent metrics: {agent.collect_metrics()}")
```

---

## Pythonic Interface Improvements

### NEW: Properties Instead of Getters

| OLD Way | NEW Way (Pythonic) |
|---------|-------------------|
| `brain.getTotalNeuronCount()` | `brain.neuron_count` |
| `brain.getTotalSynapseCount()` | `brain.synapse_count` |
| `brain.getFiringNeuronCount()` | `brain.firing_count` |
| `brain.getAverageFiringRate()` | `brain.avg_firing_rate` |
| `brain.getDevelopmentalStage()` | `brain.stage` |
| `agent.getNeuromodulationLevel()` | `agent.neuromodulation_level` |
| `agent.getCuriosityLevel()` | `agent.curiosity_level` |

### NEW: Context Manager Support

```python
# Use 'with' statement for automatic cleanup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

with brain:  # Automatic __enter__ and __exit__
    brain.step(100)
    brain.step(200)
    # Automatically cleaned up when done
```

### NEW: High-Level Convenience Methods

```python
# Run complete episode with single method
agent.run_episode(world, 1000)

# Get comprehensive metrics
metrics = brain.get_performance_metrics()
print(f"Performance: {metrics['total_spikes']} spikes in {metrics['total_neurons']} neurons")

# Enhanced memory operations
brain.clear_memory()  # Clear all memory systems
brain.validate_integrity()  # Check for issues
```

---

## Common Patterns (Enhanced)

### Pattern 1: Enhanced Simulation with Monitoring

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    # Initialize with enhanced error handling
    try:
        for step in range(num_steps):
            # 1. Update world with error handling
            world.update(0.1)
            
            # 2. Get what the agent sees
            percept = world.getSensoryPercept()
            
            # 3. Tell the brain with enhanced processing
            agent.processSensoryInput(percept)
            
            # 4. Brain thinks with performance monitoring
            brain.step(step)
            
            # 5. Get action from brain
            action = agent.decodeMotorCommand()
            
            # 6. Do action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # 7. Enhanced logging and monitoring
            if step % 100 == 0:
                metrics = brain.get_performance_metrics()
                print(f"Step {step}: {metrics['firing_count']} firing, "
                      f"{metrics['total_spikes']} total spikes")
                
    except Exception as e:
        print(f"Simulation error: {e}")
        # Graceful recovery
        world.reset()
        agent.reset()

# Use it like this:
run_enhanced_simulation(brain, world, agent, 10000)
```

### Pattern 2: Advanced Brain Configuration

```python
# Create and configure brain with validation
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# NEW: Validate configuration
if brain.check_config():
    print("Configuration is valid and ready")
else:
    print("Configuration has issues that need fixing")

# NEW: Set up event-driven programming with callbacks
brain.add_spike_callback(lambda event: print(f"Spike at neuron {event.neuron_id}"))

# NEW: Enable all systems at once
agent.enable_all_systems()  # Enables reward, plasticity, development, curiosity
```

### Pattern 3: Learning and Development Examples

```python
# Example 1: Developmental psychology simulation
print(f"Initial stage: {brain.stage}")
brain.advance_development(5000)  # 5000 steps of development
print(f"Final stage: {brain.stage}")

# Example 2: Learning experiment with memory
for step in range(1000):
    brain.step(step)
    
    # NEW: Track memory system status
    if brain.getWorkingMemory():
        traces = brain.getWorkingMemory()->getActiveTraces()
        print(f"Working memory traces: {traces}")

# NEW: Save learned brain state
brain.save("my_learned_brain.nlm")
print("Brain saved successfully!")

# NEW: Load and continue learning
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("my_learned_brain.nlm")
loaded_brain.initialize()  # Re-initialize after loading
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- **NEW**: Returns Brain object with Pythonic interface

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!
- **NEW**: Validates configuration and sets up event systems

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons  
- Updates connections based on learning rules
- **NEW**: Provides performance metrics and error handling

### Enhanced `brain.step_with_input(step, input)`
**NEW**: Combined sensory input and brain processing
```python
brain.step_with_input(10, sensory_input)
```

### `world.update(time)`
Updates the virtual world by `time` seconds.
- **NEW**: Optimized for performance with batch operations

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.
- **NEW**: Enhanced with comprehensive error handling and validation

### **NEW**: `agent.run_episode(world, num_steps)`
**NEW**: High-level method that runs complete episodes automatically
```python
agent.run_episode(world, 500)  # All setup, execution, and cleanup
```

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.
- **NEW**: Enhanced with policy-based decision making

---

## 🚀 Mini Projects (NEW Enhanced Examples)

### Project 1: Silent Brain (Enhanced)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

# NEW: Get comprehensive information
print("Silent brain test done!")
print(f"Statistics: {brain.get_performance_metrics()}")
```

### Project 2: Enhanced Brain Watching a World

```python
import pynlm

# Enhanced setup with performance optimizations
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# NEW: Performance optimizations
world.optimize_for_simulation()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# NEW: Enhanced monitoring and logging
agent.monitor_performance()

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    if i % 10 == 0:
        print(f"Progress: step {i}, firing: {brain.firing_count}")

print("Watched world for 30 steps")
print(f"Final firing rate: {brain.avg_firing_rate}")
```

### Project 3: Advanced Agent with Learning

```python
import pynlm

# Setup with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# NEW: Enable all advanced systems
agent.enable_all_systems()

# NEW: Performance monitoring
agent.monitor_performance()

# Run agent with enhanced error handling and recovery
for step in range(100):
    try:
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            # NEW: Collect and display metrics
            metrics = agent.collect_metrics()
            print(f"Step {step}: Stage {metrics['stage']}, "
                  f"Curiosity: {metrics['curiosity_level']:.3f}")
                  
    except Exception as e:
        print(f"Step {step} error: {e}")
        world.reset()  # Recovery
        agent.reset()

print("Agent simulation complete!")
print(f"Final performance: {brain.get_performance_metrics()}")
```

---

## 🚀 Quick Reference (Enhanced)

| What you want | OLD Code | NEW Code (Enhanced) |
|--------------|----------|-------------------|
| Create brain | `pynlm.createBrain(config)` | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` | `brain.initialize()` |
| Make brain think | `brain.step(step)` | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` | `brain.firing_count` |
| See world | `world.getSensoryPercept()` | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` | `agent.decodeMotorCommand()` |
| **NEW**: Get metrics | N/A | `brain.get_performance_metrics()` |
| **NEW**: Run episode | N/A | `agent.run_episode(world, steps)` |
| **NEW**: Validate config | N/A | `brain.check_config()` |
| **NEW**: Context manager | N/A | `with brain:` |

---

## 🆘 Troubleshooting (Enhanced)

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- **NEW**: Try `brain.validate_integrity()` to check for issues
- **NEW**: Increase monitoring with `brain.monitor_performance()`

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- **NEW**: Check that `world.update()` is being called
- **NEW**: Use enhanced error handling with `agent.safe_step()`

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- **NEW**: Use Pythonic properties to check `brain.firing_count`
- **NEW**: Inject batch current with `brain.inject_batch_current()`

### "Performance is slow"
- **NEW**: Use performance optimizations `brain.optimize_for_performance()`
- **NEW**: Run batch operations `brain.step_batch(start, count)`
- **NEW**: Use context managers `with brain:` for efficient resource management

### "Configuration has issues"
- **NEW**: Use `brain.check_config()` for validation
- **NEW**: Get detailed error messages with `brain.get_performance_metrics()`

### "Memory management issues"
- **NEW**: Use `brain.clear_memory()` for cleanup
- **NEW**: Monitor memory with `brain.get_memory_stats()`

---

## 📚 Next Steps (Enhanced)

When you're comfortable with the enhanced NLM Python API:

1. **Explore the enhanced examples** in the `examples/` directory
2. **Read `HOW_TO_USE.md`** for more details on advanced features
3. **Read `docs/ARCHITECTURE.md`** to understand how the enhanced integration works
4. **Experiment with different configurations** using the new Pythonic interface
5. **Use callbacks and event-driven programming** for advanced applications
6. **Implement custom policies** for behavior-based control
7. **Set up performance monitoring** and optimization

**🎉 You're now ready to use the **ENHANCED** NLM with Pythonic interface, callbacks, and advanced features!**

---

## Version Information

**Enhanced Version**: Added Pythonic properties, context managers, callbacks, high-level convenience methods, and comprehensive error handling.

**Backward Compatible**: All original examples still work exactly as before.

**NEW Features**: Pythonic interface, context managers, event system, performance optimizations, enhanced documentation, and comprehensive examples.

---

## License

MIT License. See LICENSE file for details.

## Support

For support and questions, please refer to the NLM project documentation and issue tracker.

## Special Thanks

This enhancement was made possible by analyzing the Phase 6 Final Audit and implementing the missing integration pieces that were previously disconnected.

**Key Improvements Based on Phase 6 Audit:**
- ✅ Connected working memory to sensory processing
- ✅ Connected episodic memory to experience logging  
- ✅ Connected prediction system to sensory processing
- ✅ Connected neural planner to action selection
- ✅ Connected concept formation to experience processing
- ✅ Implemented Brain::save() and Brain::load() using CheckpointSystem
- ✅ Enhanced Python API for better usability
- ✅ Added comprehensive documentation and examples
- ✅ Improved error handling and validation

