# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental computational brain project** designed to simulate how biological brains work using spiking neural networks.

Think of it like this:
- **Neurons** = Brain cells that send electrical signals
- **Synapses** = Connections between neurons that transmit signals
- **Learning** = Synaptic plasticity (strengthening/weakening connections based on activity)
- **Memory** = Neural memory systems that store experiences and patterns

NLM simulates **event-driven neural computation** using Leaky Integrate-and-Fire (LIF) neurons with:

- **Real neural dynamics** and spike timing
- **Multiple memory systems** (working, episodic, associative)
- **Advanced cognitive functions** (prediction, attention, planning)
- **Neuromodulation** (curiosity, novelty, reward processing)
- **Development** over time

That's it! NLM simulates the complete brain loop: sensory input → neural processing → memory → action selection.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

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

### Example 2: Complete Brain-World-Agent Loop

```python
import pynlm

# Step 1: Create a brain with enhanced features
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Step 2: Create the world
world = pynlm.createSimpleWorld(width=10, height=10)
world.configure(visionWidth=8, visionHeight=8)
world.reset()

# Step 3: Create the agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 4: Run an episode
for step in range(500):
    # Agent perceives the world
    percept = world.getSensoryPercept()
    processed_input = agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Agent decides what to do
    action = agent.decodeMotorCommand()
    
    # World reacts
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check if episode is over
    if world.isEpisodeComplete():
        print(f"Episode completed at step {step}")
        world.reset()
        agent.initialize(world)

print("✓ Complete episode finished successfully!")
print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
print(f"  - Memory usage: {brain.getMemoryStats()}")
```

### Example 3: Advanced Features

```python
import pynlm

# Create a brain with all Phase 6 features
config = pynlm.createPhase6Config()
config.set("memory.working.capacity", 500)
config.set("memory.episodic.capacity", 100)
config.set("cognition.planning.enabled", True)
config.set("neuromodulation.curiosity.strength", 0.8)

brain = pynlm.createBrain(config)
brain.initialize()

# Get enhanced memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
neuromodulation = brain.getNeuromodulationSystem()

# Store some experiences
working_memory.store("task", "solve_puzzle", "sensory_context")
episodic_memory.store({
    "step": brain.getCurrentStep(),
    "input": "visual_pattern",
    "output": "action_taken",
    "reward": 0.8
})

# Use prediction system
prediction_system.trainSequence([1, 2, 3, 4, 5])
next_prediction = prediction_system.predictNext()

# Use planning system
plan = planner.planAction("explore", 10)

# Use curiosity system
curiosity = neuromodulation.getCuriosity()
novelty = neuromodulation.getNovelty()

print("✓ Advanced features initialized successfully!")
print(f"  - Working memory entries: {working_memory.getEntryCount()}")
print(f"  - Episodic memory entries: {episodic_memory.getEntryCount()}")
print(f"  - Predictions made: {len(prediction_system.getTrainingHistory())}")
print(f"  - Plans generated: {planner.getPlanCount()}")
print(f"  - Curiosity active: {curiosity.isEnabled()}")
print(f"  - Novelty detection: {novelty.isEnabled()}")
```

## Available Commands

### Python Library Commands

```python
# Create different types of configurations
config = pynlm.createDefaultConfig()      # Simple default configuration
config = pynlm.createPhase3Config()       # Phase 3 features
config = pynlm.createPhase4Config()       # Phase 4 cognition
config = pynlm.createPhase6Config()       # Full Phase 6 integration

# Create different types of brains
brain = pynlm.createBrain(config)         # Basic brain
brain = pynlm.createAdvancedBrain(config) # Advanced brain with full integration

# Create different types of worlds
world = pynlm.createSimpleWorld()        # Simple grid world
world = pynlm.createComplexWorld()       # Complex environment

# Create different types of agents
agent = pynlm.createAgentBrain(brain)    # Basic agent
agent = pynlm.createAdvancedAgent(brain, world) # Advanced agent

# Run simulations
brain.initialize()                        # Initialize brain
brain.step(0)                             # Run one simulation step
brain.shutdown()                          # Clean up brain

# World operations
world.reset()                             # Reset world
world.update(delta_time)                  # Update world
world.applyMotorCommand(action, time)     # Apply action to world

# Agent operations
agent.initialize(world)                   # Initialize agent
agent.processSensoryInput(percept)        # Process sensory input
agent.decodeMotorCommand()                # Decode motor command
agent.shutdown()                          # Clean up agent
```

### Command-Line Interface

```bash
# Run Phase 6 integration demo
./nlm_phase6_demo --integration-test

# Run Phase 3 demo
./nlm_phase3_demo 5 200

# Run Phase 4 demo  
./nlm_phase4_demo --test-mode

# Run performance benchmark
./nlm_performance_bench --iterations 100 --output benchmark.json

# Show help for any command
./nlm --help
./nlm_phase3_demo --help
./nlm_phase4_demo --help
```

## Configuration Options

### Basic Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Configure basic parameters
config.set("simulation.steps", 1000)
config.set("simulation.time_step", 0.1)
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_max", 10000)
config.set("world.width", 20)
config.set("world.height", 20)

print(f"Brain configured: {config.get('brain.neuron_count')} neurons")
print(f"World size: {config.get('world.width')}x{config.get('world.height')}")
print(f"Simulation steps: {config.get('simulation.steps')}")
```

### Advanced Configuration

```python
import pynlm

# Create Phase 6 configuration with all features
config = pynlm.createPhase6Config()

# Memory systems
config.set("memory.working.capacity", 500)
config.set("memory.episodic.capacity", 100)
config.set("memory.associative.strength", 0.9)

# Cognitive systems
config.set("cognition.attention.enabled", True)
config.set("cognition.planning.enabled", True)
config.set("cognition.self_model.enabled", True)
config.set("cognition.social_learning.enabled", True)

# Neuromodulation
config.set("neuromodulation.dopamine.enabled", True)
config.set("neuromodulation.curiosity.enabled", True)
config.set("neuromodulation.novelty.enabled", True)

# Performance
config.set("performance.optimization_level", 3)
config.set("performance.parallel_processing", True)
config.set("performance.gpu_acceleration", True)

print("✓ Phase 6 configuration created with all features")
print(f"  - Memory capacity: {config.get('memory.working.capacity')} working slots")
print(f"  - Cognitive features: {config.get('cognition.attention.enabled')} attention")
print(f"  - Performance level: {config.get('performance.optimization_level')}")
```

## Memory Systems

### Working Memory

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_memory = brain.getWorkingMemory()

# Store information
working_memory.store("task_objective", "solve_puzzle")
working_memory.store("current_location", "room_1")
working_memory.store("target_objective", "reach_door")

# Retrieve information
retrieved_task = working_memory.retrieve("task_objective", threshold=0.7)
retrieved_location = working_memory.retrieve("current_location", threshold=0.5)

print(f"Working memory entries: {working_memory.getEntryCount()}")
print(f"Retrieved task: {retrieved_task}")
print(f"Retrieved location: {retrieved_location}")
```

### Episodic Memory

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

episodic_memory = brain.getEpisodicMemory()

# Store an episode
episodic_memory.store({
    "step": brain.getCurrentStep(),
    "input": "visual_pattern_A",
    "output": "action_move_north",
    "reward": 0.8,
    "context": "room_1"
})

# Retrieve similar episodes
similar_episodes = episodic_memory.retrieveSimilar("visual_pattern_A", top_k=5)

print(f"Episodic memory entries: {episodic_memory.getEntryCount()}")
print(f"Retrieved {len(similar_episodes)} similar episodes")
for i, episode in enumerate(similar_episodes[:3]):
    print(f"  Episode {i+1}: reward={episode.get('reward', 0)}, step={episode.get('step', 0)}")
```

### Associative Memory

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

associative_memory = brain.getAssociativeMemory()

# Create associations
associative_memory.associate("stimulus_A", "response_B")
associative_memory.associate("stimulus_C", "response_D")
associative_memory.associate("stimulus_E", "response_F")

# Predict responses
predicted_B = associative_memory.predict("stimulus_A")
predicted_D = associative_memory.predict("stimulus_C")

print(f"Associative memory associations: {associative_memory.getAssociationCount()}")
print(f"Predicted response for A: {predicted_B}")
print(f"Predicted response for C: {predicted_D}")
```

## Cognitive Systems

### Attention System

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

attention = brain.getAttentionSystem()
sensory_input = brain.getSensoryInput()

# Process sensory input
processed_input = attention.processSensoryInput(sensory_input)

# Select attention
attended_signal = attention.selectAttention(processed_input)

print(f"Attention selected signal strength: {attended_signal}")
```

### Planning System

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

planner = brain.getPlanner()

# Create action plan
possible_actions = ["move_north", "move_south", "move_east", "move_west", "stay"]
plan = planner.planAction("explore_room", possible_actions, 10)

print(f"Generated plan: {plan}")
print(f"Plan length: {len(plan)} steps")
print(f"Plan confidence: {planner.getPlanConfidence(plan)}")
```

### Prediction System

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

prediction_system = brain.getPredictionSystem()

# Train with temporal sequence
sequence = [1, 2, 3, 4, 5]
prediction_system.trainSequence(sequence)

# Make predictions
for i in range(len(sequence) - 1):
    predicted_next = prediction_system.predictNext()
    actual_next = sequence[i + 1]
    print(f"Predicted {predicted_next}, Actual {actual_next}")

print(f"Prediction accuracy: {prediction_system.getAccuracy()}")
```

## Neuromodulation

### Curiosity System

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

neuromodulation = brain.getNeuromodulationSystem()
curiosity = neuromodulation.getCuriosity()

print(f"Curiosity system: {'enabled' if curiosity.isEnabled() else 'disabled'}")
print(f"Curiosity threshold: {curiosity.getThreshold()}")
print(f"Curiosity strength: {curiosity.getStrength()}")
```

### Novelty Detection

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

neuromodulation = brain.getNeuromodulationSystem()
novelty = neuromodulation.getNovelty()

print(f"Novelty detection: {'enabled' if novelty.isEnabled() else 'disabled'}")
print(f"Novelty sensitivity: {novelty.getSensitivity()}")

# Detect novelty
novelty_value = novelty.detectNovelty("new_pattern")
print(f"Novelty value for 'new_pattern': {novelty_value}")
```

## Performance Monitoring

### Basic Performance Monitoring

```python
import pynlm

# Create performance monitor
monitor = pynlm.PerformanceMonitor()
monitor.start()

# Create and run brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    monitor.recordStep(step)

# Stop monitoring and get statistics
stats = monitor.stop()

print("Performance Statistics:")
print(f"  Total time: {stats.total_time:.2f}s")
print(f"  Operations per second: {stats.operations_per_second:.2f}")
print(f"  Memory usage: {stats.memory_usage_mb:.2f} MB")
print(f"  CPU utilization: {stats.cpu_utilization:.2f}%")
```

### Advanced Performance Analysis

```python
import pynlm

# Create performance analyzer
analyzer = pynlm.PerformanceAnalyzer()

# Create and run brain with monitoring
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Run simulation with performance tracking
for step in range(500):
    brain.step(step)
    analysis = analyzer.analyzeStep(step)

# Get performance summary
summary = analyzer.getSummary()

print("Performance Analysis Summary:")
print(f"  Average step time: {summary.average_step_time:.4f}s")
print(f"  Total spikes: {summary.total_spikes}")
print(f"  Spike rate: {summary.spike_rate:.2f} Hz")
print(f"  Memory efficiency: {summary.memory_efficiency:.2f}")
print(f"  Stability score: {summary.stability:.2f}")
```

## Error Handling

### Basic Error Handling

```python
import pynlm

# Configure error handling
import logging
logging.basicConfig(level=logging.INFO)

try:
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
    
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
    
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
    
except pynlm.WorldError as e:
    print(f"World error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
    
finally:
    # Cleanup
    if 'brain' in locals():
        brain.shutdown()
```

### Enhanced Error Recovery

```python
import pynlm

class NLMErrorHandler:
    def __init__(self):
        self.error_count = 0
        self.recovery_attempts = 0
        
    def handle_error(self, error, context):
        self.error_count += 1
        print(f"Error #{self.error_count}: {error}")
        print(f"Context: {context}")
        
        # Attempt recovery
        recovery_result = self.attempt_recovery(error, context)
        
        if recovery_result:
            print("✓ Recovery successful")
            self.recovery_attempts += 1
            return True
        else:
            print("✗ Recovery failed")
            return False
    
    def attempt_recovery(self, error, context):
        # Implement recovery logic
        if "memory" in str(error).lower():
            return self.recover_memory()
        elif "brain" in str(error).lower():
            return self.recover_brain()
        elif "config" in str(error).lower():
            return self.recover_config()
        else:
            return False
    
    def recover_memory(self):
        # Memory recovery logic
        return True
    
    def recover_brain(self):
        # Brain recovery logic
        return True
    
    def recover_config(self):
        # Configuration recovery logic
        return True

# Usage
error_handler = NLMErrorHandler()

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    success = error_handler.handle_error(e, f"Step {step}")
    if not success:
        print("Critical error - shutting down")
        if 'brain' in locals():
            brain.shutdown()
```

## Getting Help

### Online Resources

- **Documentation**: https://nlm-project.org/docs
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Support**: support@nlm-project.org

### Local Help

```python
# Get help information
import pynlm
help_info = pynlm.getHelp()
print(help_info)

# Get example code
examples = pynlm.getExamples()
for name, code in examples.items():
    print(f"\n=== {name} ===")
    print(code)
```

## Conclusion

You now have everything you need to start using NLM! This guide covered:

1. **Basic Usage**: Simple examples for getting started
2. **Advanced Features**: Phase 6 integration and advanced systems
3. **Configuration**: Setting up different types of configurations
4. **Memory Systems**: Working, episodic, and associative memory
5. **Cognitive Systems**: Attention, planning, and prediction
6. **Neuromodulation**: Curiosity and novelty detection
7. **Performance Monitoring**: Performance analysis and optimization
8. **Error Handling**: Robust error handling and recovery

Remember:
- Start with simple examples and gradually add complexity
- Use the documentation hub for comprehensive information
- Check the issues and discussions for common problems and solutions
- Don't hesitate to ask for help when needed

Happy simulating with NLM! 🚀
