# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

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

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 2 (Enhanced): Comprehensive Brain Monitoring

```python
import statistics
import time

def enhanced_brain_monitoring(brain, duration=1000):
    """Enhanced brain monitoring with detailed analytics."""
    
    monitoring_data = {
        'timestamp': [],
        'step': [],
        'total_neurons': [],
        'total_synapses': [],
        'firing_neurons': [],
        'firing_rate': [],
        'total_spikes': [],
        'development_stage': [],
        'neuromodulation_level': [],
        'prediction_error': [],
        'curiosity_level': [],
        'novelty_level': [],
        'action_distribution': {'forward': 0, 'backward': 0, 'turn_left': 0, 
                                'turn_right': 0, 'interact': 0, 'wait': 0}
    }
    
    print(f"Starting enhanced monitoring for {duration} steps...")
    
    for step in range(duration):
        start_time = time.time()
        
        # Record baseline before step
        monitoring_data['timestamp'].append(time.time() - start_time)
        monitoring_data['step'].append(step)
        monitoring_data['total_neurons'].append(brain.getTotalNeuronCount())
        monitoring_data['total_synapses'].append(brain.getTotalSynapseCount())
        monitoring_data['firing_neurons'].append(brain.getFiringNeuronCount())
        monitoring_data['firing_rate'].append(brain.getAverageFiringRate())
        monitoring_data['total_spikes'].append(brain.getTotalSpikeCount())
        monitoring_data['development_stage'].append(str(brain.getDevelopmentalStage()))
        
        # Agent metrics (if available)
        if hasattr(brain, 'getNeuromodulationLevel'):
            monitoring_data['neuromodulation_level'].append(brain.getNeuromodulationLevel())
        if hasattr(brain, 'getPredictionError'):
            monitoring_data['prediction_error'].append(brain.getPredictionError())
        if hasattr(brain, 'getCuriosityLevel'):
            monitoring_data['curiosity_level'].append(brain.getCuriosityLevel())
        if hasattr(brain, 'getNoveltyLevel'):
            monitoring_data['novelty_level'].append(brain.getNoveltyLevel())
        
        # Record action distribution
        action = brain.produceAction()
        if action:
            action_name = str(action.getType())
            if action_name in monitoring_data['action_distribution']:
                monitoring_data['action_distribution'][action_name] += 1
        
        # Execute brain step
        brain.step(step, time.time())
        
        # Progress updates
        if step % 100 == 0:
            avg_firing_rate = statistics.mean(monitoring_data['firing_rate'])
            avg_curiosity = statistics.mean(monitoring_data['curiosity_level']) if monitoring_data['curiosity_level'] else 0
            print(f"Step {step}: {monitoring_data['firing_neurons'][-1]} firing, "
                  f"avg firing rate: {avg_firing_rate:.2f} Hz, "
                  f"curiosity: {avg_curiosity:.3f}")
    
    # Analysis of monitoring data
    print("\n=== Enhanced Monitoring Analysis ===")
    
    analysis = {
        'steps_completed': duration,
        'final_neuron_count': monitoring_data['total_neurons'][-1],
        'final_synapse_count': monitoring_data['total_synapses'][-1],
        'total_spikes_generated': monitoring_data['total_spikes'][-1],
        'average_firing_rate': statistics.mean(monitoring_data['firing_rate']),
        'firing_rate_std': statistics.stdev(monitoring_data['firing_rate']) if len(monitoring_data['firing_rate']) > 1 else 0,
        'development_stage_changes': list(set(monitoring_data['development_stage'])),
        'action_distribution': monitoring_data['action_distribution']
    }
    
    # Add neuromodulation metrics if available
    if monitoring_data['neuromodulation_level']:
        analysis['neuromodulation_stats'] = {
            'average_level': statistics.mean(monitoring_data['neuromodulation_level']),
            'min_level': min(monitoring_data['neuromodulation_level']),
            'max_level': max(monitoring_data['neuromodulation_level'])
        }
    
    if monitoring_data['prediction_error']:
        analysis['prediction_error_stats'] = {
            'average_error': statistics.mean(monitoring_data['prediction_error']),
            'min_error': min(monitoring_data['prediction_error']),
            'max_error': max(monitoring_data['prediction_error'])
        }
    
    if monitoring_data['curiosity_level']:
        analysis['curiosity_stats'] = {
            'average_level': statistics.mean(monitoring_data['curiosity_level']),
            'min_level': min(monitoring_data['curiosity_level']),
            'max_level': max(monitoring_data['curiosity_level'])
        }
    
    print(f"Analysis complete:")
    for key, value in analysis.items():
        if key != 'action_distribution':
            print(f"  {key}: {value}")
    
    print(f"  Action distribution: {analysis['action_distribution']}")
    
    return monitoring_data, analysis
```

---

## Common Patterns

### Pattern 1: Advanced Learning Experiment

```python
import pynlm
import time
import statistics

def run_advanced_learning_experiment(brain, world, agent, num_steps=1000):
    """Run a comprehensive learning experiment with detailed analysis."""
    
    # Enable advanced learning mechanisms
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Learning metrics collection
    metrics = {
        'steps_completed': 0,
        'total_spikes': 0,
        'firing_rate_history': [],
        'curiosity_history': [],
        'novelty_history': [],
        'development_stage_history': [],
        'action_distribution': {'forward': 0, 'backward': 0, 'turn_left': 0, 
                                'turn_right': 0, 'interact': 0, 'wait': 0}
    }
    
    print("Starting advanced learning experiment...")
    
    for step in range(num_steps):
        # 1. Update world and get percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # 2. Process sensory input
        agent.processSensoryInput(percept)
        
        # 3. Brain processes (with development updates)
        brain.step(step)
        agent.updateDevelopment(0.1)
        
        # 4. Decode action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # 5. Apply reward and learning
        reward = (percept.getInternal()[0] if percept.getInternal() else 0.0) / 10.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect metrics
        metrics['steps_completed'] += 1
        metrics['total_spikes'] += brain.getTotalSpikeCount()
        metrics['firing_rate_history'].append(brain.getAverageFiringRate())
        metrics['curiosity_history'].append(agent.getCuriosityLevel())
        metrics['novelty_history'].append(agent.getNoveltyLevel())
        metrics['development_stage_history'].append(str(brain.getDevelopmentalStage()))
        
        # Track action distribution
        if motor_cmd == pynlm.MotorCommand.MoveForward:
            metrics['action_distribution']['forward'] += 1
        elif motor_cmd == pynlm.MotorCommand.MoveBackward:
            metrics['action_distribution']['backward'] += 1
        elif motor_cmd == pynlm.MotorCommand.TurnLeft:
            metrics['action_distribution']['turn_left'] += 1
        elif motor_cmd == pynlm.MotorCommand.TurnRight:
            metrics['action_distribution']['turn_right'] += 1
        elif motor_cmd == pynlm.MotorCommand.Interact:
            metrics['action_distribution']['interact'] += 1
        else:
            metrics['action_distribution']['wait'] += 1
        
        # Progress reporting
        if step % 100 == 0:
            avg_firing = statistics.mean(metrics['firing_rate_history']))
            avg_curiosity = statistics.mean(metrics['curiosity_history']))
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"avg firing rate: {avg_firing:.2f} Hz, "
                  f"curiosity: {avg_curiosity:.3f}")
    
    # Analysis of results
    print("\n=== Experiment Results ===")
    avg_firing_rate = statistics.mean(metrics['firing_rate_history'])
    avg_curiosity = statistics.mean(metrics['curiosity_history'])
    avg_novelty = statistics.mean(metrics['novelty_history'])
    
    print(f"Completed {metrics['steps_completed']} steps")
    print(f"Total spikes generated: {metrics['total_spikes']}")
    print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"Average curiosity: {avg_curiosity:.3f}")
    print(f"Average novelty: {avg_novelty:.3f}")
    print(f"Action distribution: {metrics['action_distribution']}")
    
    # Determine learning effectiveness
    firing_rate_variance = statistics.stdev(metrics['firing_rate_history']))
    curiosity_variance = statistics.stdev(metrics['curiosity_history']))
    
    learning_metrics = {
        'final_firing_rate': metrics['firing_rate_history'][-1],
        'firing_rate_trend': 'increasing' if metrics['firing_rate_history'][-1] > metrics['firing_rate_history'][0] else 'stable',
        'exploration_level': 'high' if avg_curiosity > 0.3 else 'medium' if avg_curiosity > 0.1 else 'low',
        'learning_effectiveness': 'good' if firing_rate_variance > 1.0 else 'limited'
    }
    
    return metrics, learning_metrics
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Advanced Configuration and Monitoring

```python
import pynlm
import json
import time

def advanced_configuration_example():
    """Demonstrate advanced configuration options and monitoring."""
    
    # Create configuration with all advanced options
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.15)
    config.set("brain.v_thresh", -55.0)  # Higher threshold
    config.set("brain.v_rest", -65.0)   # Resting potential
    config.set("brain.tau_mem", 15.0)   # Slower membrane time constant
    config.set("brain.tau_ref", 3.0)    # Longer refractory period
    
    # Advanced plasticity configuration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.threshold", 5.0)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0005)
    config.set("plasticity.structural.pruning_rate", 0.00005)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.base_rate", 0.01)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.decay_rate", 0.95)
    
    # Advanced development
    config.set("development.enable", True)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.critical_period_end", 300.0)
    config.set("development.adult_plasticity", 0.2)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("=== Advanced Configuration Analysis ===")
    print("Configuration Summary:")
    for key in config.getKeys():
        if key.startswith("brain."):
            print(f"  {key}: {config.getKeys()[key]}")
    
    return brain, config

# Run advanced configuration example
brain, config = advanced_configuration_example()

# Monitor system performance over time
print("\n=== Performance Monitoring ===")
monitoring_data = {
    'steps': [],
    'firing_rates': [],
    'spike_counts': [],
    'action_distribution': {'forward': 0, 'turn_left': 0, 'turn_right': 0, 
                            'interact': 0, 'wait': 0}
}

start_time = time.time()
for step in range(500):
    brain.step(step, time.time())
    
    monitoring_data['steps'].append(step)
    monitoring_data['firing_rates'].append(brain.getAverageFiringRate())
    monitoring_data['spike_counts'].append(brain.getTotalSpikeCount())
    
    # Monitor action distribution
    action = brain.produceAction()
    if action:
        action_name = str(action.getType())
        if action_name in monitoring_data['action_distribution']:
            monitoring_data['action_distribution'][action_name] += 1

end_time = time.time()

# Analyze monitoring data
print(f"Simulation time: {end_time - start_time:.2f} seconds")
print(f"Steps completed: {len(monitoring_data['steps'])}")
print(f"Average firing rate: {sum(monitoring_data['firing_rates']) / len(monitoring_data['firing_rates']):.2f} Hz")
print(f"Total spikes: {monitoring_data['spike_counts'][-1]}")
print(f"Action distribution: {monitoring_data['action_distribution']}")
```

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number, timestamp=None)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- Applies neuromodulation
- Updates development state

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### Advanced Features
- `agent.applyRewardModulation(reward, predicted_reward)` - Learn from outcomes
- `agent.updateDevelopment(timestep)` - Brain matures and changes over time
- `agent.enable[Feature](True/False)` - Enable/disable subsystems
- `brain.produceAction()` - Get motor command from brain
- `brain.injectCurrent(neuron_id, current)` - Directly stimulate neurons
- `brain.save/load()` - Save/restore brain state
- `agent.getThoughtTrace()` - Get brain's internal narrative
- `agent.getDevelopmentalStage()` - Get current developmental phase
- `agent.getCuriosityLevel()` - Get exploration drive
- `agent.getNoveltyLevel()` - Get surprise detection
- `agent.getPredictionError()` - Get prediction accuracy
- `agent.getNeuromodulationLevel()` - Get neurotransmitter levels

### `agent.enableAdvancedLearning(enabled)`
Enable/disable advanced learning including:
- Complex reward prediction
- Concept formation
- Self-model development
- Social learning
- Memory consolidation
- Structural plasticity
- Developmental trajectories

### `agent.setLearningRate(rate)`
Set the overall learning rate for the agent (0.0 to 2.0).

### `agent.configureForTask(task_type, difficulty)`
Automatically configure the agent for specific tasks:
- `task_type`: "navigation", "problem_solving", "social_interaction", "exploration"
- `difficulty`: "easy", "medium", "hard", "expert"

### `agent.getPerformanceMetrics()`
Get comprehensive performance metrics including:
- Learning efficiency
- Decision quality
- Adaptation speed
- Exploration-exploitation balance
- Memory retention
- Prediction accuracy

### `world.createCustomEnvironment(type, parameters)`
Create specialized environments:
- "maze", "open_field", "social_space", "resource_scarce"
- Custom parameters for complexity, risk, reward distribution

### `world.addChallenge(challenge_type, challenge_params)`
Add challenges to the world:
- "obstacle", "time_pressure", "resource_depletion", "predator"
- Create dynamic environments that require adaptive behavior

### `agent.simulateEpisode(world, max_steps, show_thoughts=False)`
Run a complete episode with full monitoring:
- Returns comprehensive episode statistics
- Optional thought trace output
- Detailed learning analysis

### `brain.analyzeConnectomics()`
Analyze brain connectivity patterns:
- Network topology
- Hub neurons
- Module structure
- Connection strength distribution
- Integration with cognitive functions

### `agent.exportNeurogenesisData(filepath)`
Export brain development data:
- Synaptic history
- Structural changes
- Learning trajectories
- Developmental milestones
- Ready for analysis tools

### `brain.loadNeurogenesisData(filepath)`
Load previously saved brain development data:
- Restore structural changes
- Replay learning trajectories
- Preserve developmental state
- Enable cross-experiment comparisons


---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### Advanced Mini Projects

#### Project 4: Brain Development Analysis

```python
import pynlm
import time
import statistics

def brain_development_project():
    """Analyze how a brain develops over time."""
    
    # Create a development-focused brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("development.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Track development stages
    developmental_stages = []
    plasticity_modifiers = []
    firing_rates = []
    
    print("Tracking brain development...")
    
    for step in range(100):
        brain.step(step, time.time())
        
        developmental_stages.append(str(brain.getDevelopmentalStage()))
        plasticity_modifiers.append(brain.getPlasticityModifier())
        firing_rates.append(brain.getAverageFiringRate())
        
        # Inject structured input to drive learning
        if step % 20 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
    
    # Analyze development progression
    print(f"Final developmental stage: {developmental_stages[-1]}")
    print(f"Plasticity modifier evolution: {plasticity_modifiers[:5]}...")
    print(f"Firing rate progression: {[f'{rate:.1f}' for rate in firing_rates[:5]]}...")
    
    # Calculate development metrics
    final_firing_rate = firing_rates[-1]
    plasticity_trend = "increasing" if plasticity_modifiers[-1] > plasticity_modifiers[0] else "decreasing"
    
    development_metrics = {
        'final_stage': developmental_stages[-1],
        'final_firing_rate': final_firing_rate,
        'plasticity_trend': plasticity_trend,
        'development_time': len(developmental_stages)
    }
    
    print(f"Development analysis complete: {development_metrics}")
    return development_metrics

brain_development_project()
```

#### Project 5: Agent Memory System

```python
import pynlm

def agent_memory_project():
    """Test the agent's memory systems."""
    
    # Create brain with memory systems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("neuromod.dopamine.scale", 2.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with memory
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Memory test sequence
    memory_test_results = []
    
    for episode in range(3):
        print(f"Memory test episode {episode + 1}")
        
        # Store initial state
        initial_firing = brain.getFiringNeuronCount()
        initial_spikes = brain.getTotalSpikeCount()
        initial_memory = brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
        
        # Run episode with varied input
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward to strengthen memory
            reward = 1.0 if step % 10 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Store final state
        final_firing = brain.getFiringNeuronCount()
        final_spikes = brain.getTotalSpikeCount()
        final_memory = brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0
        
        episode_result = {
            'episode': episode + 1,
            'firing_change': final_firing - initial_firing,
            'spike_change': final_spikes - initial_spikes,
            'memory_traces': final_memory,
            'memory_growth': final_memory - initial_memory
        }
        memory_test_results.append(episode_result)
        
        print(f"  Memory traces: {episode_result['memory_traces']}")
        print(f"  Memory growth: {episode_result['memory_growth']}")
    
    # Memory analysis
    print("\n=== Memory System Analysis ===")
    total_growth = sum(result['memory_growth'] for result in memory_test_results)
    avg_growth = total_growth / len(memory_test_results)
    
    memory_analysis = {
        'total_memory_traces': memory_test_results[-1]['memory_traces'],
        'average_growth_per_episode': avg_growth,
        'growth_trend': 'increasing' if memory_test_results[-1]['memory_growth'] > memory_test_results[0]['memory_growth'] else 'stable',
        'final_memory_strength': memory_test_results[-1]['memory_traces']
    }
    
    print(f"Memory system analysis: {memory_analysis}")
    return memory_analysis

agent_memory_project()
```

#### Project 6: Advanced Learning with Curiosity

```python
import pynlm
import random

def curiosity_learning_project():
    """Demonstrate curiosity-driven learning with advanced features."""
    
    # Create curiosity-enabled agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("neuromod.dopamine.scale", 1.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Curiosity experiment data
    curiosity_experiment = {
        'steps': [],
        'novelty_levels': [],
        'curiosity_levels': [],
        'actions_taken': [],
        'rewards_received': [],
        'exploration_events': 0,
        'exploitation_events': 0
    }
    
    print("Curiosity-driven learning experiment...")
    print("Watch the agent balance exploration vs exploitation!")
    
    for step in range(200):
        # Get current world state
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Get curiosity level and decide action
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        # Curiosity-driven action selection
        if curiosity > 0.7 and random.random() < 0.3:
            # High curiosity -> exploration
            curiosity_experiment['exploration_events'] += 1
            action = select_random_action()  # Random action for exploration
        else:
            # Normal curiosity level -> exploitation
            curiosity_experiment['exploitation_events'] += 1
            action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply learning
        reward = percept.getInternal()[0] / 10.0 if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Record experiment data
        curiosity_experiment['steps'].append(step)
        curiosity_experiment['novelty_levels'].append(novelty)
        curiosity_experiment['curiosity_levels'].append(curiosity)
        curiosity_experiment['actions_taken'].append(str(action))
        curiosity_experiment['rewards_received'].append(reward)
        
        # Progress updates
        if step % 20 == 0:
            avg_curiosity = sum(curiosity_experiment['curiosity_levels']) / len(curiosity_experiment['curiosity_levels'])
            avg_novelty = sum(curiosity_experiment['novelty_levels']) / len(cur180
            print(f"Step {step}: Curiosity={avg_curiosity:.3f}, Novelty={avg_novelty:.3f}, "
                  f"Exploration={curiosity_experiment['exploration_events']}, "
                  f"Exploitation={curiosity_experiment['exploitation_events']}")
    
    # Analyze curiosity experiment
    print("\n=== Curiosity Learning Analysis ===")
    avg_curiosity = sum(curiosity_experiment['curiosity_levels']) / len(curiosity_experiment['curiosity_levels'])
    avg_novelty = sum(curiosity_experiment['novelty_levels']) / len(curiosity_experiment['novelty_levels'])
    
    curiosity_metrics = {
        'final_curiosity_level': curiosity_experiment['curiosity_levels'][-1],
        'average_curiosity': avg_curiosity,
        'final_novelty_level': curiosity_experiment['novelty_levels'][-1],
        'average_novelty': avg_novelty,
        'exploration_ratio': curiosity_experiment['exploration_events'] / len(curiosity_experiment['steps']),
        'exploitation_ratio': curiosity_experiment['exploitation_events'] / len(curiosity_experiment['steps']),
        'total_steps': len(curiosity_experiment['steps'])
    }
    
    print(f"Curiosity metrics: {curiosity_metrics}")
    
    # Determine learning effectiveness
    if avg_curiosity > 0.5:
        print("✓ High curiosity - agent was highly exploratory")
    elif avg_curiosity > 0.2:
        print("⚠ Moderate curiosity - balanced exploration/exploitation")
    else:
        print("⚠ Low curiosity - agent was primarily exploitative")
    
    return curiosity_experiment, curiosity_metrics

def select_random_action():
    """Helper function to select random actions for exploration."""
    actions = [
        pynlm.MotorCommand.MoveForward,
        pynlm.MotorCommand.MoveBackward,
        pynlm.MotorCommand.TurnLeft,
        pynlm.MotorCommand.TurnRight,
        pynlm.MotorCommand.Interact,
        pynlm.MotorCommand.Wait
    ]
    return random.choice(actions)

# Run the curiosity learning experiment
experiment_data, metrics = curiosity_learning_project()
```

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more detailed technical information
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the advanced examples above to see the power of the system
5. Explore the other documentation files for specialized topics

That's it! You're now equipped with comprehensive knowledge to use NLM from basic to advanced applications.

