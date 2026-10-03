# NLM Python API - Complete Reference Guide

## Overview

This guide provides comprehensive documentation for the NLM (Neural Learning Machine) Python API. The API enables you to create, configure, and run artificial brain simulations programmatically, from simple demonstrations to complex research experiments.

## Installation

### Prerequisites
- Python 3.8 or higher
- pip (Python package installer)

### Installation Options

#### Option 1: Direct Installation
```bash
pip install pynlm
```

#### Option 2: Development Installation (from source)
```bash
# Clone the repository
cd /path/to/nlm

# Build and install
pip install -e .
```

#### Option 3: Build from Source Wheel
```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build wheel
python -m build

# Install wheel
pip install dist/*.whl
```

## Getting Started

### Quick Start Examples

#### Example 1: Silent Brain (Basic Neuron Simulation)
```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Brain simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

#### Example 2: Complete Agent in World
```python
import pynlm
import time

def run_complete_simulation(steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(1000)
print("Agent simulation complete!")
```

#### Example 3: Brain Training and Learning
```python
import pynlm
import matplotlib.pyplot as plt

def train_brain_and_record_stats(num_steps=500):
    """Train a brain and record detailed statistics."""
    
    # Create config optimized for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Statistics collection
    spike_history = []
    weight_changes = []
    firing_rates = []
    novelty_levels = []
    curiosity_levels = []
    developmental_stages = []
    
    # Run training simulation
    for step in range(num_steps):
        # Inject random sensory input to drive learning
        if step % 10 == 0:
            # Random input to sensory neurons
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0 + (brain.getRandomGenerator()->nextFloat() * 10.0))
        
        brain.step(step)
        
        # Record statistics
        spike_history.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getAverageFiringRate())
        
        # Get agent statistics if available
        if agent:
            novelty_levels.append(agent.getNoveltyLevel())
            curiosity_levels.append(agent.getCuriosityLevel())
        
        developmental_stages.append(brain.getDevelopmentalStage())
        
        # Record synaptic weight changes (simplified)
        if step % 50 == 0:
            weight_changes.append(brain.getExcitationInhibitionRatio())
    
    return {
        'spike_history': spike_history,
        'firing_rates': firing_rates,
        'novelty_levels': novelty_levels,
        'curiosity_levels': curiosity_levels,
        'developmental_stages': developmental_stages,
        'weight_changes': weight_changes,
        'final_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_ei_ratio': brain.getExcitationInhibitionRatio()
    }

# Run training
stats = train_brain_and_record_stats(1000)

# Plot results
plt.figure(figsize=(15, 10))

plt.subplot(2, 3, 1)
plt.plot(stats['spike_history'])
plt.title('Total Spikes Over Time')
plt.xlabel('Step')
plt.ylabel('Spike Count')

plt.subplot(2, 3, 2)
plt.plot(stats['firing_rates'])
plt.title('Average Firing Rate')
plt.xlabel('Step')
plt.ylabel('Firing Rate')

plt.subplot(2, 3, 3)
plt.plot(stats['weight_changes'])
plt.title('Excitation/Inhibition Ratio')
plt.xlabel('Step')
plt.ylabel('E/I Ratio')

if stats['novelty_levels']:
    plt.subplot(2, 3, 4)
    plt.plot(stats['novelty_levels'])
    plt.title('Novelty Level')
    plt.xlabel('Step')
    plt.ylabel('Novelty')

if stats['curiosity_levels']:
    plt.subplot(2, 3, 5)
    plt.plot(stats['curiosity_levels'])
    plt.title('Curiosity Level')
    plt.xlabel('Step')
    plt.ylabel('Curiosity')

plt.subplot(2, 3, 6)
stage_counts = {}
for stage in stats['developmental_stages']:
    stage_str = str(stage)
    stage_counts[stage_str] = stage_counts.get(stage_str, 0) + 1
plt.bar(stage_counts.keys(), stage_counts.values())
plt.title('Developmental Stage Distribution')
plt.xlabel('Stage')
plt.ylabel('Frequency')

plt.tight_layout()
plt.show()
```

## Advanced Usage Patterns

### Pattern 1: Multi-Brain Competition
```python
import pynlm

class CompetitionEnvironment:
    def __init__(self, num_brains=5):
        self.brains = []
        self.scores = []
        
        for i in range(num_brains):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 500 + i * 100)
            config.set("random_seed", i * 1000)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            self.brains.append({
                'brain': brain,
                'agent': pynlm.createAgentBrain(brain),
                'world': pynlm.createSimpleWorld(),
                'score': 0.0,
                'step': 0
            })
            
            # Configure world
            world = self.brains[-1]['world']
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.setAgentStart(10.0, 10.0)
            
            # Initialize agent
            agent = self.brains[-1]['agent']
            agent.initialize(world)
            agent.enableRewardModulation(True)
    
    def run_competition_step(self):
        """Run one step of competition between all brains."""
        for brain_data in self.brains:
            brain = brain_data['brain']
            agent = brain_data['agent']
            world = brain_data['world']
            
            # World update
            world.update(0.1)
            
            # Sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(brain_data['step'])
            
            # Action selection
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Reward calculation and neuromodulation
            reward = self.calculate_reward(world)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Update score
            brain_data['score'] += reward
            brain_data['step'] += 1
        
        return self.get_scores()
    
    def calculate_reward(self, world):
        """Calculate reward based on agent's position and performance."""
        body = world.getAgentBody()
        
        # Reward for reaching center
        distance_from_center = ((body.x - 10) ** 2 + (body.y - 10) ** 2) ** 0.5
        center_reward = max(0.0, 10.0 - distance_from_center)
        
        # Reward for avoiding boundaries
        boundary_penalty = 0.0
        if body.x < 2.0 or body.x > 18.0 or body.y < 2.0 or body.y > 18.0:
            boundary_penalty = 5.0
        
        # Combined reward
        return center_reward - boundary_penalty
    
    def get_scores(self):
        """Get current scores for all brains."""
        return [brain_data['score'] for brain_data in self.brains]

# Run competition
env = CompetitionEnvironment(10)
for step in range(100):
    scores = env.run_competition_step()
    if step % 20 == 0:
        print(f"Step {step}: Top scores = {sorted(scores, reverse=True)[:3]}")

# Get winner
winner_idx = max(range(len(env.brains)), key=lambda i: env.brains[i]['score'])
print(f"Winner: Brain {winner_idx} with score {env.brains[winner_idx]['score']}")
```

### Pattern 2: Memory and Episode Recording
```python
import pynlm
import json

def experiment_with_memory_tracking(duration_steps=500):
    """Run experiment with detailed memory tracking."""
    
    # Create config
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("memory.max_episodes", 2000)
    config.set("memory.working_memory_capacity", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.setAgentStart(20.0, 20.0)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Experiment data
    experiment_data = {
        'steps': [],
        'episodes': [],
        'memory_stats': [],
        'brain_stats': [],
        'agent_stats': []
    }
    
    for step in range(duration_steps):
        # Run environment step
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Collect step data
        step_data = {
            'step': step,
            'time': world.getSimulationTime(),
            'agent_x': world.getAgentBody().x,
            'agent_y': world.getAgentBody().y,
            'reward': reward,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'ei_ratio': brain.getExcitationInhibitionRatio(),
            'dev_stage': brain.getDevelopmentalStage()
        }
        
        experiment_data['steps'].append(step_data)
        
        # Periodically save brain state (checkpoint)
        if step % 100 == 0:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            brain.save(checkpoint_file)
            print(f"Checkpoint saved: {checkpoint_file}")
        
        # Periodically analyze memory
        if step % 200 == 0:
            memory_analysis = {
                'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                'episodic_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel()
            }
            experiment_data['memory_stats'].append(memory_analysis)
    
    # Save experiment data
    with open('experiment_data.json', 'w') as f:
        json.dump(experiment_data, f, indent=2)
    
    # Load and return summary
    summary = {
        'total_steps': duration_steps,
        'final_agent_position': (world.getAgentBody().x, world.getAgentBody().y),
        'total_rewards': sum(s['reward'] for s in experiment_data['steps']),
        'final_spike_count': brain.getTotalSpikeCount(),
        'avg_firing_rate': sum(s['firing_rate'] for s in experiment_data['steps']) / len(experiment_data['steps']),
        'memory_stats': experiment_data['memory_stats'][-1] if experiment_data['memory_stats'] else None
    }
    
    with open('experiment_summary.json', 'w') as f:
        json.dump(summary, f, indent=2)
    
    print(f"Experiment completed. Data saved to experiment_data.json")
    return summary

# Run experiment
summary = experiment_with_memory_tracking(1000)
print(f"Experiment summary: {summary}")
```

### Pattern 3: Brain Configuration and Parameter Optimization
```python
import pynlm
import numpy as np

def optimize_brain_configuration(param_grid, num_trials=50):
    """Optimize brain configuration using parameter search."""
    
    best_config = None
    best_score = -float('inf')
    results = []
    
    for trial in range(num_trials):
        # Randomly sample parameters from grid
        config = pynlm.createDefaultConfig()
        
        # Neuron count
        neuron_count = np.random.choice(param_grid['neuron_count'])
        config.set("brain.neuron_count", neuron_count)
        
        # Connection probability
        conn_prob = np.random.choice(param_grid['connection_probability'])
        config.set("brain.connection_probability", conn_prob)
        
        # Plasticity settings
        stdp_enabled = np.random.choice([True, False])
        config.set("plasticity.stdp.enable", stdp_enabled)
        
        hebbian_enabled = np.random.choice([True, False])
        config.set("plasticity.hebbian.enable", hebbian_enabled)
        
        # Neuromodulation
        dopamine_scale = np.random.choice(param_grid['dopamine_scale'])
        config.set("neuromod.dopamine.scale", dopamine_scale)
        
        # Development
        config.set("development.plasticity_modulation", np.random.choice([0.5, 1.0, 1.5]))
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Simple training loop
        for step in range(100):
            # Inject some input
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0)
            brain.step(step)
        
        # Evaluate performance
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        ei_ratio = brain.getExcitationInhibitionRatio()
        
        # Simple scoring function (balance between activity and stability)
        score = spike_count * 0.01 + firing_rate * 0.5 - abs(ei_ratio - 1.0) * 10.0
        
        results.append({
            'trial': trial,
            'neuron_count': neuron_count,
            'conn_prob': conn_prob,
            'stdp_enabled': stdp_enabled,
            'hebbian_enabled': hebbian_enabled,
            'dopamine_scale': dopamine_scale,
            'spike_count': spike_count,
            'firing_rate': firing_rate,
            'ei_ratio': ei_ratio,
            'score': score
        })
        
        if score > best_score:
            best_score = score
            best_config = config
        
        print(f"Trial {trial}: Score = {score:.2f}, Spikes = {spike_count}, E/I = {ei_ratio:.2f}")
    
    # Find best configuration from grid search
    best_grid_result = max(results, key=lambda x: x['score'])
    
    print(f"\nBest trial: Score = {best_grid_result['score']:.2f}")
    print(f"Best params: Neuron count={best_grid_result['neuron_count']}, "
          f"Conn prob={best_grid_result['conn_prob']}, "
          f"STDP={best_grid_result['stdp_enabled']}, "
          f"Hebbian={best_grid_result['hebbian_enabled']}, "
          f"Dopamine scale={best_grid_result['dopamine_scale']}")
    
    return {
        'best_config': best_config,
        'best_score': best_score,
        'best_grid_result': best_grid_result,
        'all_results': results
    }

# Define parameter grid
param_grid = {
    'neuron_count': [500, 1000, 1500, 2000],
    'connection_probability': [0.05, 0.1, 0.15, 0.2],
    'dopamine_scale': [0.5, 1.0, 2.0]
}

# Run optimization
optimization_results = optimize_brain_configuration(param_grid, num_trials=20)
```

## Reference Documentation

### Brain Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `createBrain(config)` | Creates a new brain instance | `config` - Configuration object | Brain instance |
| `initialize()` | Initializes the brain with configuration | None | bool - Success |
| `step(currentStep)` | Performs a simulation step | `currentStep` - Step number | None |
| `step(currentStep, currentTime)` | Performs step with timestamp | `currentStep` - Step number<br>`currentTime` - Time | None |
| `receiveSensoryInput(input)` | Injects sensory input | `input` - SensoryInput object | None |
| `injectCurrent(neuron, current)` | Injects current into specific neuron | `neuron` - NeuronId<br>`current` - MembranePotential | None |
| `injectCurrentToNeurons(type, current)` | Injects current to all neurons of type | `type` - NeuronType<br>`current` - MembranePotential | None |
| `produceAction()` | Produces motor action from brain activity | None | Action instance |
| `reset()` | Resets brain state | None | None |
| `save(filepath)` | Saves brain state to file | `filepath` - File path | bool - Success |
| `load(filepath)` | Loads brain state from file | `filepath` - File path | bool - Success |
| `addRegion(name)` | Adds a new neural region | `name` - Region name | RegionId |
| `getRegion(id)` | Gets region by ID | `id` - RegionId | NeuralRegion pointer |
| `getTotalNeuronCount()` | Gets total neuron count | None | size_t |
| `getTotalSynapseCount()` | Gets total synapse count | None | size_t |
| `getFiringNeuronCount()` | Gets currently firing neurons | None | size_t |
| `getAverageFiringRate()` | Gets average firing rate | None | float |
| `getExcitationInhibitionRatio()` | Gets E/I balance | None | float |

### AgentBrain Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `createAgentBrain(brain)` | Creates agent brain interface | `brain` - Brain instance | AgentBrain instance |
| `initialize(world)` | Initializes with world | `world` - SimpleWorld instance | None |
| `processSensoryInput(percept)` | Processes sensory percept | `percept` - SensoryPercept | None |
| `decodeMotorCommand()` | Decodes motor command | None | Action instance |
| `applyRewardModulation(reward, predictedReward)` | Applies reward modulation | `reward` - float<br>`predictedReward` - float | None |
| `updateDevelopment(timestep)` | Updates development system | `timestep` - float | None |
| `getCuriosityLevel()` | Gets curiosity level | None | float |
| `getNoveltyLevel()` | Gets novelty level | None | float |
| `getPredictionError()` | Gets prediction error | None | float |
| `enableRewardModulation(enable)` | Enables/disables reward modulation | `enable` - bool | None |
| `enableStructuralPlasticity(enable)` | Enables/disables structural plasticity | `enable` - bool | None |

### Configuration Methods

#### Brain Configuration
- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Synapse connectivity density
- `brain.connection_probability`: Probability of connection between neurons
- `brain.initial_weight_mean`: Mean initial synaptic weight
- `brain.initial_weight_std`: Standard deviation of initial weights
- `brain.v_thresh`: Neuron threshold potential (mV)
- `brain.v_rest`: Resting potential (mV)
- `brain.v_reset`: Reset potential after spike (mV)

#### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.tau_plus`: STDP time constant (ms)
- `plasticity.stdp.tau_minus`: STDP time constant (mV)
- `plasticity.hebbian.enable`: Enable Hebbian learning
- `plasticity.structural.enable`: Enable structural plasticity

#### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration
- `neuromod.novelty.enable`: Enable novelty detection

### Utility Functions

#### Creation Functions
- `createDefaultConfig()`: Creates default configuration
- `createBrain(config)`: Creates new brain
- `createSimpleWorld()`: Creates simple 2D world
- `createAgentBrain(brain)`: Creates agent interface

#### Constants
- `NeuronType.Excitatory`, `NeuronType.Inhibitory`, etc.
- `ActionType.MoveForward`, `ActionType.TurnLeft`, etc.
- `DevelopmentalStage.Initial`, `DevelopmentalStage.Maturation`, etc.

### Error Handling

The Python API raises the following exceptions:

1. **RuntimeError**: For runtime errors during simulation
2. **ValueError**: For invalid parameter values
3. **FileNotFoundError**: When loading non-existent checkpoint files

### Performance Tips

1. **Memory Management**: 
   - Reuse brain instances for multiple simulations
   - Use checkpoint/save-load instead of full re-initialization

2. **Configuration Optimization**:
   - Use configuration files for complex setups
   - Cache frequently used configuration objects

3. **Simulation Efficiency**:
   - Batch multiple steps when possible
   - Use appropriate simulation timesteps

4. **Memory Usage**:
   - Monitor memory usage for long-running simulations
   - Use `getWorkingMemory()` and `getEpisodicMemory()` for memory statistics

## Advanced Features

### Debugging and Monitoring

The API includes several debugging and monitoring capabilities:

#### Logging
```python
import pynlm

# Enable logging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.getConfig().set("logging.level", "debug")
brain.initialize()

# Log status periodically
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        brain.logStatus()
```

#### Performance Monitoring
```python
import time
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

start_time = time.time()
for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        elapsed = time.time() - start_time
        steps_per_second = step / elapsed
        print(f"Step {step}: {steps_per_second:.1f} steps/sec")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

### Configuration Management

#### Loading/Saving Configurations
```python
import pynlm

# Save current configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Create brain from loaded config
brain = pynlm.createBrain(config2)
brain.initialize()
```

#### Configuration Validation
```python
def validate_configuration(config):
    """Validate configuration parameters."""
    errors = []
    
    # Check neuron count
    if not config.has("brain.neuron_count"):
        errors.append("Missing brain.neuron_count")
    else:
        neuron_count = config.getOr<size_t>("brain.neuron_count", 0)
        if neuron_count == 0 or neuron_count > 10000:
            errors.append(f"Invalid neuron_count: {neuron_count}")
    
    # Check connection probability
    if config.has("brain.connection_probability"):
        conn_prob = config.getOr<float>("brain.connection_probability", 0.0)
        if conn_prob < 0.0 or conn_prob > 1.0:
            errors.append(f"Invalid connection_probability: {conn_prob}")
    
    return errors

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
errors = validate_configuration(config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid")
```

## Migration Guide

### From Version 0.0.x to 0.1.0

#### API Changes

1. **Method Renames**:
   - `getTotalSpikeCount()` → `getTotalSpikeCount()` (unchanged)
   - `getActiveNeuronCount()` → `getActiveNeuronCount()` (unchanged)
   - `produceAction()` → `produceAction()` (unchanged)

2. **New Features**:
   - Agent brain interface (`createAgentBrain()`)
   - Memory system access (`getWorkingMemory()`, `getEpisodicMemory()`)
   - Development stages (`getDevelopmentalStage()`)
   - Neuromodulation integration

3. **Configuration Updates**:
   - Added new parameters for enhanced control
   - Better validation of configuration values

#### Code Migration Examples

#### Old Code (0.0.x)
```python
import pynlm

# Old way - limited API
config = pynlm.Config()
config.set("brain.neuron_count", 1000)
brain = pynlm.Brain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### New Code (0.1.0)
```python
import pynlm

# New way - enhanced API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced features
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(100):
    brain.step(step)
    
    # Log progress
    if step % 20 == 0:
        brain.logStatus()
```

### Troubleshooting

#### Common Issues and Solutions

1. **ImportError: No module named 'pynlm'**
   - Solution: Ensure pynlm is installed with `pip install pynlm`
   - Solution: Build from source with `pip install -e .`

2. **RuntimeError: Failed to initialize brain**
   - Solution: Check that `brain.initialize()` is called before `brain.step()`
   - Solution: Ensure configuration is valid

3. **Segmentation fault**
   - Solution: Use smaller brain configurations for debugging
   - Solution: Add logging and step through the simulation

4. **Memory issues**
   - Solution: Use checkpoint/save-load for long simulations
   - Solution: Monitor memory usage with `getWorkingMemory()`

#### Debug Mode
```python
import pynlm
import sys

# Enable debug logging
pynlm.Config.setLogLevel("debug")

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize with error checking
if not brain.initialize():
    print("Failed to initialize brain!")
    sys.exit(1)

# Run with periodic debugging
for step in range(100):
    try:
        brain.step(step)
        
        # Check for errors every 10 steps
        if step % 10 == 0:
            status = brain.logStatus()
            
    except Exception as e:
        print(f"Error at step {step}: {e}")
        break
```

## License

NLM Python API is licensed under the MIT License.

## Copyright

© 2024-2026 NLM Project. All rights reserved.