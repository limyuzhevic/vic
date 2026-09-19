# NLM - Advanced Features Guide

## Overview

This guide covers advanced NLM features for power users and researchers. It builds on the fundamental concepts from the basic documentation and explores the full capabilities of the system.

## Advanced Brain Configuration

### Custom Neuron Types and Properties

#### Creating Specialized Neuron Populations

```python
import pynlm

# Create a custom configuration with specialized neurons
config = pynlm.createDefaultConfig()

# Configure different neuron types in different brain regions
config.set("brain.neuron_count", 5000)

# Excitatory neurons with higher threshold
config.set("brain.excitatory_neurons_per_region", 2000)
config.set("brain.inhibitory_neurons_per_region", 1500)

# Sensory neurons for specific modalities
config.set("brain.sensory_neuron_count", 500)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Configuration Parameters

| Parameter | Type | Default | Advanced Use Case |
|-----------|------|---------|-------------------|
| `brain.synapse_density` | float | 0.1 | High density (0.2-0.3) for dense networks |
| `brain.connection_probability` | float | 0.05 | 0.01 for sparse, 0.1 for dense |
| `brain.initial_weight_mean` | float | 0.5 | Lower for balanced networks (0.1-0.2) |
| `brain.initial_weight_std` | float | 0.1 | Higher for diverse initial states (0.2-0.3) |
| `brain.v_thresh` | float | -50.0 | Adjust for different firing thresholds |
| `brain.tau_ref` | float | 2.0 | Control refractory period duration |

### Regional Brain Architecture

```python
import pynlm

# Create configuration with multiple brain regions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add specialized regions
cortex_id = brain.addRegion("cortex", 2000)
cerebellum_id = brain.addRegion("cerebellum", 500)
hippocampus_id = brain.addRegion("hippocampus", 300)

# Configure region-specific properties
cortex = brain.getRegion(cortex_id)
cortex.setProperty("plasticity_rate", 0.01)
cortex.setProperty("excitability", 1.0)

# Create neuron populations within regions
population_id = brain.addPopulation("cortex_excitatory", cortex_id, 1000)
population = brain.getPopulation(population_id)
```

## Advanced Plasticity Mechanisms

### Custom STDP Rules

```python
import pynlm

# Create brain with custom STDP configuration
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)  # Higher learning rate
config.set("plasticity.stdp.tau_plus", 30.0)       # Longer potentiation window
config.set("plasticity.stdp.tau_minus", 20.0)      # Longer depression window
config.set("plasticity.stdp.voltage_dependence", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Access brain for plasticity monitoring
# (This would be available through agent's enableStructuralPlasticity)
```

### Hebbian Learning with Custom Rules

```python
# Enable and configure Hebbian learning
agent.enableHebbianLearning(True)

# Custom Hebbian parameters
config.set("plasticity.hebbian.threshold", 0.5)
config.set("plasticity.hebbian.decay_rate", 0.001)
config.set("plasticity.hebbian.max_weight", 2.0)

# Trigger Hebbian learning based on correlated activity
agent.triggerHebbianLearning(correlated_patterns)
```

## Neuromodulation Deep Dive

### Custom Neuromodulator Implementation

```python
import pynlm

# Create brain with enhanced neuromodulation
config = pynlm.createDefaultConfig()
config.set("neuromod.dopamine.scale", 2.0)      # Stronger dopamine signal
config.set("neuromod.dopamine.decay_rate", 0.1)
config.set("neuromod.curiosity.threshold", 0.3) # Lower novelty threshold
config.set("neuromod.novelty.sensitivity", 1.5) # Higher sensitivity

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable custom neuromodulation features
agent.enableRewardModulation(True)
agent.enableCuriosityDrivenExploration(True)
agent.enableNoveltyDetection(True)

# Implement custom neuromodulation logic
def custom_neuromodulation_update(agent, reward, prediction_error):
    # Calculate integrated neuromodulation signal
    dopamine = reward * config.get("neuromod.dopamine.scale")
    curiosity = agent.getNoveltyLevel() * config.get("neuromod.novelty.sensitivity")
    
    # Apply combined effect
    agent.applyNeuromodulation(dopamine + curiosity * 0.5)
```

## Advanced World Simulation

### Custom World Environments

```python
import pynlm

# Create advanced world configuration
world = pynlm.createSimpleWorld()
world.configure(
    width=50,                    # Large environment
    height=50,
    visionWidth=16,              # Wide field of view
    visionHeight=16,
    timestep=0.05,               # Faster simulation
    maxEnergy=200.0              # More energy resources
)

# Add complex environment objects
for x in range(0, 50, 5):
    for y in range(0, 50, 5):
        if (x + y) % 10 == 0:
            # Add resource nodes
            world.addObject(
                pynlm.WorldObject(
                    x, y, 
                    pynlm.WorldObjectType.Resource, 
                    value=10.0 + (x + y) * 0.1
                )
            )
        elif (x + y) % 7 == 0:
            # Add hazard zones
            world.addObject(
                pynlm.WorldObject(
                    x, y, 
                    pynlm.WorldObjectType.Hazard, 
                    value=1.0
                )
            )

# Set complex agent behavior
world.setAgentStart(25.0, 25.0)
```

### Advanced World Properties

| Property | Type | Default | Advanced Value |
|----------|------|---------|----------------|
| `timestep` | float | 0.1 | 0.01-0.05 for smoother dynamics |
| `maxEnergy` | float | 100.0 | Adjust based on agent energy budget |
| `visionWidth/Height` | int | 8 | 16-32 for wider perception |
| `width/height` | int | 20 | Scale up for larger environments |

## Advanced Agent Capabilities

### Custom Agent Brain Configuration

```python
import pynlm

# Create sophisticated agent with multiple subsystems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable all subsystems for maximum capability
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enablePrediction(True)
agent.enableWorkingMemory(True)

# Custom agent configuration
agent.setLearningRate(0.01)
agent.setExplorationRate(0.2)
agent.setMemoryCapacity(1000)
```

### Advanced Agent Behavior Patterns

#### Goal-Directed Learning

```python
class GoalDirectedAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.goals = []
        self.priority_weights = []
    
    def setGoal(self, goal_type, priority=1.0):
        """Set a learning goal with priority."""
        self.goals.append(goal_type)
        self.priority_weights.append(priority)
    
    def getActionWithGoals(self, percept):
        """Get action considering multiple goals."""
        self.agent.processSensoryInput(percept)
        
        # Modify action selection based on goals
        base_action = self.agent.decodeMotorCommand()
        
        for goal, weight in zip(self.goals, self.priority_weights):
            base_action = self.applyGoalBias(base_action, goal, weight)
        
        return base_action
```

#### Social Learning Simulation

```python
# Simulate multiple agents learning together
agents = []
brains = []

# Create agent group
for i in range(5):
    brain = pynlm.createBrain(config)
    brain.initialize()
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    agents.append(agent)
    brains.append(brain)

# Social learning phase
def socialLearningPhase(agents, world):
    for step in range(100):
        # Each agent observes others
        for agent in agents:
            # Get observations from nearby agents
            nearby_agents = getNearbyAgents(agent, agents, radius=10.0)
            observations = [a.getRecentActions() for a in nearby_agents]
            
            # Integrate social observations
            agent.integrateSocialLearning(observations)
        
        # Environment update
        world.update(0.1)
```

## Performance Optimization

### Large-Scale Brain Configuration

```python
# Optimize for large brain simulations
config = pynlm.createDefaultConfig()

# Scale configuration for large networks
config.set("brain.neuron_count", 100000)  # 100K neurons
config.set("brain.synapse_density", 0.05) # Sparse connectivity
config.set("performance.optimization.enable", True)
config.set("performance.optimization.use_gpu", True)
config.set("performance.optimization.vectorization", True)

# Memory optimization
config.set("performance.memory_pool.enable", True)
config.set("performance.event_queue.size", 100000)
config.set("performance.parallel.enabled", True)
config.set("performance.parallel.threads", 8)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Simulation Performance Monitoring

```python
import time
import pynlm

# Create performance monitoring class
class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.timings = []
        self.memory_usage = []
        self.spike_counts = []
    
    def step_with_monitoring(self, step_number):
        start_time = time.time()
        
        # Record baseline metrics
        before_spikes = self.brain.getTotalSpikeCount()
        
        # Execute brain step
        self.brain.step(step_number)
        
        # Record performance metrics
        end_time = time.time()
        
        self.timings.append(end_time - start_time)
        self.spike_counts.append(self.brain.getTotalSpikeCount() - before_spikes)
        
        return {
            'time': end_time - start_time,
            'spikes': self.brain.getTotalSpikeCount() - before_spikes,
            'firing_rate': self.brain.getAverageFiringRate(),
            'performance': 1.0 / (end_time - start_time) if end_time > start_time else 0
        }
```

### Simulation Parameters

| Parameter | Recommendation | Notes |
|-----------|----------------|-------|
| `brain.neuron_count` | 1K-10K for prototyping | >10K for production |
| `brain.synapse_density` | 0.01-0.05 | Balance connectivity & performance |
| `performance.parallel.threads` | CPU cores count | Optimal for parallel processing |
| `timestep` | 0.01-0.05 ms | Smaller for accuracy, larger for speed |

## Advanced I/O and State Management

### Checkpoint Management

```python
import pynlm
import os

# Create comprehensive checkpoint system
class CheckpointManager:
    def __init__(self, brain, agent, world, checkpoint_dir):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.checkpoint_dir = checkpoint_dir
        os.makedirs(checkpoint_dir, exist_ok=True)
    
    def saveCheckpoint(self, step_number, metadata=None):
        """Save complete simulation state."""
        checkpoint_file = f"{self.checkpoint_dir}/checkpoint_step_{step_number:06d}.bin"
        
        self.brain.save(checkpoint_file)
        
        # Save agent state
        agent_file = f"{self.checkpoint_dir}/agent_step_{step_number:06d}.json"
        with open(agent_file, 'w') as f:
            import json
            json.dump({
                'step': step_number,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'metadata': metadata or {}
            }, f)
        
        return checkpoint_file
    
    def loadCheckpoint(self, checkpoint_file):
        """Restore simulation from checkpoint."""
        step_number = self.extractStepFromFilename(checkpoint_file)
        self.brain.load(checkpoint_file)
        
        # Load agent state
        agent_file = checkpoint_file.replace('.bin', '.json')
        if os.path.exists(agent_file):
            with open(agent_file, 'r') as f:
                import json
                data = json.load(f)
                # Restore agent state (implementation depends on API)
```

### Data Export and Analysis

```python
# Export simulation data for analysis
class DataExporter:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
    
    def exportNeuralData(self, filename, start_step=0, end_step=None):
        """Export neural activity patterns."""
        with open(filename, 'w') as f:
            f.write("step,firing_rate,spike_count,dev_stage,curiosity,novelty\n")
            
            for step in range(start_step, end_step or 1000):
                self.brain.step(step)
                
                f.write(f"{step},")
                f.write(f"{self.brain.getAverageFiringRate()},")
                f.write(f"{self.brain.getTotalSpikeCount()}, ")
                f.write(f"{self.brain.getDevelopmentalStage()}, ")
                f.write(f"{self.agent.getCuriosityLevel()}, ")
                f.write(f"{self.agent.getNoveltyLevel()}\n")
    
    def exportBehaviorData(self, filename):
        """Export agent behavior patterns."""
        with open(filename, 'w') as f:
            f.write("step,action,x,y,reward,curiosity\n")
            
            for step in range(1000):
                self.world.update(0.1)
                action = self.agent.decodeMotorCommand()
                percept = self.world.getSensoryPercept()
                
                f.write(f"{step},{action.getType()},")
                f.write(f"{self.world.getAgentBody().getX()}, ")
                f.write(f"{self.world.getAgentBody().getY()}, ")
                # Calculate reward from percept
                reward = self.calculateReward(percept)
                f.write(f"{reward}, ")
                f.write(f"{self.agent.getCuriosityLevel()}\n")
```

## Integration and Experimentation

### Complete Research Workflow

```python
# Full research experiment template
class NLMResearchExperiment:
    def __init__(self, config):
        self.config = config
        self.results = {}
    
    def runPhase1_Baseline(self, steps=1000):
        """Run baseline experiment without learning."""
        print("Running baseline experiment...")
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        
        baseline_results = {
            'firing_rates': [],
            'spike_counts': [],
            'movements': 0
        }
        
        for step in range(steps):
            brain.step(step)
            
            baseline_results['firing_rates'].append(
                brain.getAverageFiringRate()
            )
            baseline_results['spike_counts'].append(
                brain.getTotalSpikeCount()
            )
            
            if step % 100 == 0:
                action = agent.decodeMotorCommand()
                if action.getType() != pynlm.ActionType.Idle:
                    baseline_results['movements'] += 1
        
        self.results['baseline'] = baseline_results
        return baseline_results
    
    def runPhase2_Learning(self, steps=1000, enable_learning=True):
        """Run experiment with learning enabled."""
        print("Running learning experiment...")
        
        # Create full agent with all subsystems
        config = self.config
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        learning_results = {
            'firing_rates': [],
            'spike_counts': [],
            'movements': 0,
            'learning_curve': [],
            'novelty_levels': []
        }
        
        for step in range(steps):
            # Full environment loop
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            learning_results['firing_rates'].append(
                brain.getAverageFiringRate()
            )
            learning_results['spike_counts'].append(
                brain.getTotalSpikeCount()
            )
            learning_results['novelty_levels'].append(
                agent.getNoveltyLevel()
            )
            
            if action.getType() != pynlm.ActionType.Idle:
                learning_results['movements'] += 1
            
            if step % 100 == 0:
                curiosity = agent.getCuriosityLevel()
                learning_results['learning_curve'].append({
                    'step': step,
                    'curiosity': curiosity,
                    'firing_rate': brain.getAverageFiringRate()
                })
        
        self.results['learning'] = learning_results
        return learning_results
    
    def analyzeResults(self):
        """Analyze and compare experiment results."""
        print("Analyzing results...")
        
        baseline = self.results.get('baseline', {})
        learning = self.results.get('learning', {})
        
        analysis = {
            'firing_rate_change': 0,
            'movement_change': 0,
            'learning_effectiveness': 0
        }
        
        if baseline and learning:
            # Calculate statistical changes
            baseline_fr = sum(baseline['firing_rates']) / len(baseline['firing_rates'])
            learning_fr = sum(learning['firing_rates']) / len(learning['firing_rates'])
            
            analysis['firing_rate_change'] = ((learning_fr - baseline_fr) / baseline_fr) * 100
            
            analysis['movement_change'] = (
                (learning['movements'] - baseline['movements']) / 
                max(1, baseline['movements'])
            ) * 100
            
            # Learning effectiveness (curiosity growth)
            initial_curiosity = learning['novelty_levels'][0] if learning['novelty_levels'] else 0
            final_curiosity = learning['novelty_levels'][-1] if learning['novelty_levels'] else 0
            analysis['learning_effectiveness'] = final_curiosity - initial_curiosity
        
        return analysis
```

## Troubleshooting Advanced Features

### Common Advanced Issues and Solutions

#### 1. High Memory Usage in Large Networks

**Problem:** Memory usage exceeds available resources with >10K neurons.

**Solution:**
```python
# Configure memory-efficient settings
config.set("performance.memory_pool.enable", True)
config.set("performance.optimization.use_compression", True)
config.set("brain.synapse_density", 0.01)  # Sparse connections
config.set("performance.event_queue.size", 10000)
```

#### 2. Slow Plasticity Performance

**Problem:** STDP and Hebbian learning slow down simulation significantly.

**Solution:**
```python
# Optimize plasticity computation
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.vectorized", True)
config.set("plasticity.hebbian.batch_update", True)
config.set("performance.parallel.plasticity", True)
```

#### 3. Agent Exploration Collapse

**Problem:** Agent stops exploring and gets stuck in local optima.

**Solution:**
```python
# Adjust curiosity and exploration parameters
agent.setExplorationRate(0.3)  # Increase exploration
agent.setCuriosityThreshold(0.1)  # Lower novelty threshold
agent.enableIntrinsicMotivation(True)
```

#### 4. Memory Management Issues

**Problem:** Memory leaks or corruption with long-running simulations.

**Solution:**
```python
# Implement proper memory management
brain.reset()  # Clean state between episodes
brain.clearCaches()  # Clear temporary buffers
agent.resetMemory()  # Reset agent memory
world.cleanup()  # Clean world objects
```

#### 5. Development Stage Transitions

**Problem:** Brain development stages not transitioning correctly.

**Solution:**
```python
# Configure development stages properly
config.set("development.critical_period.duration", 100)
config.set("development.maturation.threshold", 0.5)
config.set("development.plasticity_decay_rate", 0.001)

# Monitor development progress
brain.getDevelopmentalStage()  # Check current stage
brain.getDevelopmentProgress()  # Get progress percentage
```

## Advanced Customization

### Custom Neuron Models

#### Extending Basic LIF Model

```python
# Custom neuron configuration for specific dynamics
class CustomNeuronConfig:
    def __init__(self):
        self.v_thresh = -45.0      # Higher threshold
        self.v_rest = -65.0        # Different resting potential
        self.tau_mem = 25.0        # Slower membrane time constant
        self.tau_ref = 5.0         # Longer refractory period
        self.cm = 0.2              # Different capacitance
        self.gl = 0.01             # Different leak conductance
        
        # Custom plasticity
        self.enable_custom_plasticity = True
        self.custom_stdp_strength = 0.005
        
        # Custom noise
        self.noise_level = 0.1
        self.adaptive_threshold = True
```

### Custom Simulation Loop

```python
# Override default simulation loop for custom control
class CustomSimulationLoop:
    def __init__(self, brain, agent, world, config):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.config = config
        self.custom_hooks = []
    
    def addPreStepHook(self, hook_function):
        """Add function to execute before each brain step."""
        self.custom_hooks.append(('pre_step', hook_function))
    
    def addPostStepHook(self, hook_function):
        """Add function to execute after each brain step."""
        self.custom_hooks.append(('post_step', hook_function))
    
    def runCustomLoop(self, steps):
        """Run custom simulation loop with hooks."""
        for step in range(steps):
            # Pre-step hooks
            for hook_type, hook_func in self.custom_hooks:
                if hook_type == 'pre_step':
                    hook_func(step, self.brain, self.agent, self.world)
            
            # Custom step logic
            self.customBrainStep(step)
            
            # Post-step hooks
            for hook_type, hook_func in self.custom_hooks:
                if hook_type == 'post_step':
                    hook_func(step, self.brain, self.agent, self.world)
    
    def customBrainStep(self, step):
        """Override default brain step logic."""
        # Custom step implementation
        self.brain.processInternalInputs()
        self.brain.updateNeurons()
        self.brain.spreadSpikes()
        self.brain.applyPlasticity(step)
```

## Extending NLM

### Adding New Modules

#### Plugin Architecture Overview

NLM supports extending functionality through plugin modules. Here's how to add a new module:

```python
# Example plugin module structure
class CustomModulePlugin:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.name = "custom_module"
        self.version = "1.0.0"
    
    def initialize(self):
        """Initialize module with brain/agent/world."""
        pass
    
    def update(self, timestep):
        """Update module state."""
        pass
    
    def getState(self):
        """Get module state for checkpointing."""
        return {}
    
    def loadState(self, state):
        """Load module state from checkpoint."""
        pass
```

### Research Integration

#### Experiment Framework

```python
# Advanced experiment framework for research
class NLMExperimentSuite:
    def __init__(self):
        self.experiments = []
        self.results_db = {}
    
    def addExperiment(self, name, experiment_func):
        """Add experiment to suite."""
        self.experiments.append({
            'name': name,
            'func': experiment_func
        })
    
    def runAllExperiments(self):
        """Run all registered experiments."""
        for exp in self.experiments:
            print(f"Running experiment: {exp['name']}")
            try:
                result = exp['func']()
                self.results_db[exp['name']] = result
                print(f"Experiment completed: {exp['name']}")
            except Exception as e:
                print(f"Experiment failed: {exp['name']} - {e}")
                self.results_db[exp['name']] = {'error': str(e)}
    
    def compareExperiments(self, metric_name):
        """Compare experiments by metric."""
        comparison = {}
        for exp_name, result in self.results_db.items():
            if metric_name in str(result):
                comparison[exp_name] = result
        
        return comparison
```

## Conclusion

Advanced NLM features enable researchers and developers to:

1. **Create complex brain architectures** with specialized regions and neuron types
2. **Implement sophisticated learning mechanisms** with custom plasticity rules
3. **Build rich interactive environments** with dynamic world objects
4. **Develop autonomous agents** with advanced cognition and motivation
5. **Optimize performance** for large-scale simulations
6. **Implement comprehensive research workflows** with reproducible experiments

The flexibility of NLM allows you to:
- Customize neuron models beyond basic LIF dynamics
- Implement novel learning algorithms
- Create complex environment interactions
- Build multi-agent systems
- Integrate with external data sources
- Perform advanced statistical analysis

For the latest features and customizations, refer to the ongoing development documentation and release notes.
