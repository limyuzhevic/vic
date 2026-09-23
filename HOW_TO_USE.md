# NLM - Advanced Usage Guide

## Introduction

This guide covers advanced usage patterns, configuration options, and debugging techniques for the NLM (Neural Learning Machine) Python bindings. It assumes you have basic familiarity with NLM as covered in the `easy_usage.md` guide.

## Configuration Deep Dive

### Configuration Value Types

NLM supports multiple configuration value types:

| Type | Python equivalent | Example |
|------|------------------|---------|
| `int` | `int` | `config.set("neuron_count", 1000)` |
| `int64_t` | `int` | `config.set("region_count", 2)` |
| `double` | `float` | `config.set("connection_probability", 0.1)` |
| `bool` | `bool` | `config.set("enable_learning", True)` |
| `std::string` | `str` | `config.set("brain_name", "my_brain")` |
| `std::vector<int>` | `list[int]` | `config.set("hidden_sizes", [64, 32, 16])` |
| `std::vector<double>` | `list[float]` | `config.set("learning_rates", [0.01, 0.001])` |
| `std::vector<std::string>` | `list[str]` | `config.set("tags", ["test", "production"])` |

### Configuration Keys Reference

Here are the key configuration options for NLM:

#### Brain Configuration
- `neuron_count`: Total number of neurons (int64_t)
- `region_count`: Number of brain regions (int64_t)
- `connection_probability`: Probability of connections between neurons (double)
- `simulation_timestep`: Simulation timestep in seconds (double)
- `random_seed`: Random seed for reproducible experiments (uint64_t)
- `enable_sensory`: Enable sensory processing (bool)
- `enable_motor`: Enable motor output (bool)
- `enable_plasticity`: Enable synaptic plasticity (bool)

#### Memory System Configuration
- `working_memory_capacity`: Size of working memory (int64_t)
- `episodic_memory_capacity`: Size of episodic memory (int64_t)
- `associative_memory_capacity`: Size of associative memory (int64_t)
- `memory_decay_rate`: Memory decay rate (double)

#### Learning Configuration
- `stdp_ltp_weight`: STDP LTP weight (double)
- `stdp_ltd_weight`: STDP LTD weight (double)
- `stdp_tau`: STDP time constant (double)
- `synaptogenesis_rate`: Rate of new synapse formation (double)
- `pruning_rate`: Rate of synapse removal (double)
- `reward_modulation_strength`: Reward modulation strength (double)

#### Development Configuration
- `developmental_stage_duration`: Duration of each developmental stage (double)
- `plasticity_modifier`: Plasticity modifier during development (double)
- `critical_period_enabled`: Enable critical period (bool)

#### Performance Configuration
- `enable_vectorization`: Enable SIMD vectorization (bool)
- `enable_parallel`: Enable parallel processing (bool)
- `memory_pool_size`: Memory pool size (int64_t)
- `max_threads`: Maximum number of threads (int64_t)

### Configuration Best Practices

1. **Always use explicit types**: Set configuration values with the correct type for clarity
2. **Use constants**: Define configuration constants at the top of your script
3. **Validate before use**: Check that required configuration values are set
4. **Use meaningful names**: Choose descriptive configuration keys
5. **Document your config**: Add comments explaining configuration choices

```python
# Best practice: Use named constants
class BrainConfig:
    NEURON_COUNT = 2000
    REGION_COUNT = 3
    CONNECTION_PROBABILITY = 0.15
    SIMULATION_TIMESTEP = 0.001
    RANDOM_SEED = 42
    ENABLE_LEARNING = True
    MEMORY_CAPACITY = 500

# Use constants in your code
config = pynlm.createDefaultConfig()
config.set("neuron_count", BrainConfig.NEURON_COUNT)
config.set("region_count", BrainConfig.REGION_COUNT)
config.set("connection_probability", BrainConfig.CONNECTION_PROBABILITY)
```

## Agent Configuration and Control

### Advanced Agent Features

The `AgentBrain` class provides several advanced configuration and control options:

#### Feature Control

```python
# Enable all features
agent.enableAllFeatures()

# Disable all features  
agent.disableAllFeatures()

# Check which features are enabled
features = agent.getFeatureSummary()
print("Enabled features:", features)

# Individual feature control
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(False)
agent.enableDevelopment(True)
agent.enableCuriosity(False)
```

#### Neuromodulation Control

```python
# Control neuromodulation levels
agent.applyRewardModulation(1.0, 0.5)  # High reward
agent.applyRewardModulation(0.1, 0.8)  # Low reward
agent.applyRewardModulation(-0.5, 0.3) # Negative reward

# Get current neuromodulation status
status = agent.getNeuromodulationStatus()
print(f"Dopamine level: {status['dopamine']}")
print(f"Curiosity level: {status['curiosity']}")
```

### Agent Behavior Customization

#### Custom Motor Command Mapping

```python
def custom_motor_mapping(agent, world, brain):
    # Get raw motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply custom mapping based on brain state
    stage = brain.getDevelopmentalStage()
    
    # During early development, use simpler actions
    if stage == pynlm.DevelopmentalStage.Initial:
        return pynlm.MotorCommand.Wait  # Be conservative
    # During critical period, explore more
    elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
        return custom_exploratory_action(agent, world)
    # In adulthood, use learned behaviors
    else:
        return learned_action(agent, brain)
```

## Memory System Integration

### Working Memory

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory
working_memory = brain.getWorkingMemory()
print(f"Working memory capacity: {working_memory.getCapacity()}")

# Store information in working memory
working_memory.store("task", "navigation")
working_memory.store("target", (10, 10))
working_memory.store("strategy", "avoid_obstacles")

# Retrieve information
stored_task = working_memory.recall("task")
print(f"Current task: {stored_task}")

# Working memory with decay
for step in range(100):
    brain.step(step)
    # Information fades over time
    if step % 50 == 0:
        print(f"Working memory strength: {working_memory.getStrength()}")
```

### Episodic Memory

```python
# Record experiences
episodic_memory = brain.getEpisodicMemory()

# Store an experience
experience = {
    "timestamp": world.getSimulationTime(),
    "location": (agent_body.x, agent_body.y),
    "action": action,
    "outcome": result,
    "reward": result.reward,
    "sensory_input": world.getSensoryPercept()
}

episodic_memory.record(experience)

# Replay experiences for learning
episodic_memory.replay("navigation", 1000)  # Replay navigation experiences
```

### Associative Memory

```python
# Learn associations
associative_memory = brain.getAssociativeMemory()

# Create associations between concepts
associative_memory.associate("food", "reward", 0.9)
associative_memory.associate("water", "thirst", 0.8)
associative_memory.associate("danger", "avoid", 0.95)

# Use associations for inference
related_concepts = associative_memory.getRelatedConcepts("food")
print(f"Concepts related to 'food': {related_concepts}")

# Pattern completion
completed_pattern = associative_memory.completePattern("food_")
print(f"Completed pattern: {completed_pattern}")
```

## Prediction and Planning

### Prediction System

```python
# Access prediction system
prediction_system = brain.getPredictionSystem()

# Set up prediction targets
prediction_system.setPredictionTarget("next_action", predicted_action)
prediction_system.setPredictionTarget("future_state", predicted_state)

# Get prediction error
error = prediction_system.getPredictionError()
print(f"Prediction error: {error}")

# Update predictions based on outcomes
prediction_system.update("next_action", actual_action, error)
```

### Neural Planning

```python
# Access neural planner
planner = brain.getPlanner()

# Set goals
planner.setGoal("reach_target", (target_x, target_y))
planner.setGoal("avoid_danger", danger_region)

# Generate action sequences
sequence = planner.generateSequence()
print(f"Planned action sequence: {sequence}")

# Evaluate plans
for action in sequence:
    evaluation = planner.evaluateAction(action)
    print(f"Action {action}: score {evaluation}")
```

## Cognitive Systems

### Concept Formation

```python
# Access concept formation system
concept_formation = brain.getConceptFormation()

# Learn from sensory input
concept_formation.processInput(sensory_percept)

# Extract features
features = concept_formation.extractFeatures(sensory_percept)
print(f"Extracted features: {features}")

# Form concepts
concept_formation.formConcepts(features)

# Get formed concepts
concepts = concept_formation.getConcepts()
print(f"Formed concepts: {concepts}")
```

### Attention System

```python
# Access attention system
attention = brain.getAttention()

# Process sensory input with attention
focused_input = attention.process(sensory_percept)

# Control attention focus
attention.setFocus("visual", 0.8)
attention.setFocus("auditory", 0.3)
attention.setFocus("internal", 0.1)

# Get attention weights
weights = attention.getAttentionWeights()
print(f"Attention weights: {weights}")
```

## Development System

### Developmental Stages

```python
# Track developmental progression
brain = pynlm.createBrain(config)
brain.initialize()

stages = []
for step in range(300):
    brain.step(step)
    
    current_stage = brain.getDevelopmentalStage()
    if current_stage not in stages:
        stages.append(current_stage)
        print(f"Stage reached: {current_stage}")

print(f"Developmental stages: {stages}")
```

### Developmental Effects

```python
# Modify developmental parameters
config = pynlm.createDefaultConfig()
config.set("critical_period_enabled", True)
config.set("plasticity_modifier", 2.0)  # Enhanced plasticity

brain = pynlm.createBrain(config)
brain.initialize()

# Observe developmental effects
for step in range(200):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    plasticity = brain.getDevelopmentSystem().getPlasticityModifier()
    
    if step % 50 == 0:
        print(f"Step {step}: Stage {stage}, Plasticity {plasticity}")
```

## Plasticity System

### Synaptic Plasticity Rules

```python
# Access plasticity rules
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural = brain.getStructuralPlasticity()

# Configure STDP parameters
stdp.configure(ltp_weight=0.01, ltd_weight=0.012, tau=20.0)

# Configure Hebbian learning
hebbian.configure(learning_rate=0.05, decay_rate=0.001)

# Configure structural plasticity
structural.setSynaptogenesisRate(0.0001)
structural.setPruningRate(0.00001)
```

### Plasticity Monitoring

```python
# Monitor plasticity over time
plasticity_monitor = {
    "stdp_weight": 0.0,
    "hebbian_weight": 0.0,
    "synapse_count": 0,
    "new_synapses": 0,
    "pruned_synapses": 0
}

for step in range(500):
    brain.step(step)
    
    # Get current plasticity statistics
    stdp = brain.getSTDP()
    structural = brain.getStructuralPlasticity()
    
    plasticity_monitor["stdp_weight"] = stdp.getCurrentWeight()
    plasticity_monitor["synapse_count"] = brain.getTotalSynapseCount()
    plasticity_monitor["new_synapses"] = structural.getSynapsesCreated()
    plasticity_monitor["pruned_synapses"] = structural.getSynapsesPruned()
    
    if step % 100 == 0:
        print(f"Step {step}: {plasticity_monitor}")
```

## Neuromodulation

### Neuromodulator Control

```python
# Access individual neuromodulators
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Set neuromodulator levels
dopamine.setLevel(0.5)
curiosity.setLevel(0.3)
novelty.setLevel(0.2)

# Apply neuromodulation effects
brain.applyNeuromodulation(dopamine)
brain.applyNeuromodulation(curiosity)

# Get neuromodulation effects
effects = {
    "learning_rate": dopamine.getLearningRate(),
    "exploration": curiosity.getExplorationBonus(),
    "novelty_detection": novelty.getNoveltyDetection(),
    "prediction_error": prediction_error.getErrorMagnitude()
}

print(f"Neuromodulation effects: {effects}")
```

### Reward Prediction Error

```python
# Track reward prediction errors
rpe_system = {
    "expected_rewards": [],
    "actual_rewards": [],
    "prediction_errors": []
}

for step in range(300):
    # Get expected reward
    expected = agent.getExpectedReward()
    rpe_system["expected_rewards"].append(expected)
    
    # Apply action and get actual reward
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    action_result = world.getActionResult()
    
    rpe_system["actual_rewards"].append(action_result.reward)
    
    # Calculate prediction error
    prediction_error = action_result.reward - expected
    rpe_system["prediction_errors"].append(prediction_error)
    
    # Apply reward modulation
    agent.applyRewardModulation(action_result.reward, expected)
    
    # Get feedback from brain
    brain.step(step)
    
    if step % 50 == 0:
        avg_rpe = sum(rpe_system["prediction_errors"]) / len(rpe_system["prediction_errors"])
        print(f"Step {step}: Average RPE = {avg_rpe:.3f}")
```

## Advanced Brain Operations

### Custom Neural Input Injection

```python
# Inject custom current into specific neurons
brain = pynlm.createBrain(config)
brain.initialize()

# Get neuron IDs
neurons = brain.getRegion(1).getNeurons()
if not neurons.empty():
    target_neuron = neurons.front().getId()
    
    # Inject current
    brain.injectCurrent(target_neuron, 0.5)  # High excitation
    brain.injectCurrent(target_neuron, -0.2) # Inhibition

# Inject to all neurons of a specific type
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 0.3)
brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, -0.1)
```

### Region Management

```python
# Add custom regions
brain.addRegion("Visual_Cortex")
brain.addRegion("Motor_Cortex")
brain.addRegion("Memory_Cortex")

# Connect regions
region1 = brain.getRegion(1)
region2 = brain.getRegion(2)
if region1 && region2:
    # Create inter-region connections
    brain.addInterRegionConnection(region1.getId(), region2.getId(), 0.8)
    brain.addInterRegionConnection(region2.getId(), region1.getId(), 0.6)

# Get region information
for rid in brain.getRegionIds():
    region = brain.getRegion(rid)
    if region:
        print(f"Region {rid}: {region.getName()}, "
              f"{region.getNeuronCount()} neurons, "
              f"{region.getPopulationCount()} populations")
```

### Spike System Control

```python
# Access and control spike system
spike_system = brain.getSpikeSystem()

# Configure spike parameters
spike_system.setRefractoryPeriod(0.005)  # 5ms refractory period
spike_system.setThreshold(0.2)  # Lower threshold
spike_system.setResetPotential(-0.07)  # Higher reset potential

# Monitor spike activity
for step in range(200):
    brain.step(step)
    
    spike_stats = {
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "pending_spikes": brain.getPendingSpikeEventCount()
    }
    
    if step % 50 == 0:
        print(f"Step {step}: {spike_stats}")
```

## Performance and Profiling

### Performance Monitoring

```python
import time
import pynlm

def profile_brain(brain, steps, description=""):
    """Profile brain performance"""
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    
    elapsed_ms = (end_time - start_time) * 1000
    steps_per_second = steps / (elapsed_ms / 1000)
    
    print(f"{description}:")
    print(f"  Steps: {steps:,}")
    print(f"  Time: {elapsed_ms:.2f} ms")
    print(f"  Performance: {steps_per_second:,.0f} steps/second")
    print(f"  Time per step: {elapsed_ms/steps*1000:.2f} μs")
    
    return {
        "steps": steps,
        "time_ms": elapsed_ms,
        "steps_per_second": steps_per_second,
        "time_per_step_us": elapsed_ms/steps*1000
    }

# Profile with different configurations
configs = [
    ("Small brain", 500),
    ("Medium brain", 2000),
    ("Large brain", 5000)
]

for desc, steps in configs:
    profile_brain(brain, steps, desc)
```

### Memory Usage Monitoring

```python
# Monitor memory usage (placeholder implementation)
def monitor_memory_usage(brain, steps):
    """Monitor memory usage during brain operation"""
    memory_samples = []
    
    for step in range(steps):
        # Record memory usage (implementation-dependent)
        memory_usage = get_memory_usage()  # Implementation-dependent
        memory_samples.append(memory_usage)
        
        brain.step(step)
        
        if step % 100 == 0:
            avg_memory = sum(memory_samples) / len(memory_samples)
            print(f"Step {step}: Average memory usage: {avg_memory:.2f} MB")
    
    return memory_samples

# Run memory monitoring
memory_usage = monitor_memory_usage(brain, 1000)
```

## Debugging and Troubleshooting

### Debugging Tools

```python
import pynlm

def debug_brain(brain, world, agent, steps):
    """Comprehensive brain debugging"""
    debug_info = {
        "step": 0,
        "neuron_count": 0,
        "firing_neurons": 0,
        "spike_count": 0,
        "region_count": 0,
        "development_stage": None,
        "excitation_inhibition_ratio": 0.0,
        "memory_usage": 0.0,
        "agent_x": 0.0,
        "agent_y": 0.0,
        "errors": []
    }
    
    for step in range(steps):
        try:
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            if not percept:
                debug_info["errors"].append(f"Step {step}: No sensory input")
                continue
            
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Collect debug info
            debug_info["step"] = step
            debug_info["neuron_count"] = brain.getTotalNeuronCount()
            debug_info["firing_neurons"] = brain.getFiringNeuronCount()
            debug_info["spike_count"] = brain.getTotalSpikeCount()
            debug_info["region_count"] = brain.getRegionCount()
            debug_info["development_stage"] = brain.getDevelopmentalStage()
            debug_info["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio()
            
            if step % 50 == 0:
                print(f"Debug info step {step}:")
                for key, value in debug_info.items():
                    print(f"  {key}: {value}")
                    
        except Exception as e:
            debug_info["errors"].append(f"Step {step}: {str(e)}")
            print(f"Error at step {step}: {e}")
    
    return debug_info

# Run debugging
debug_results = debug_brain(brain, world, agent, 500)
print(f"\nTotal errors encountered: {len(debug_results['errors'])}")
if debug_results['errors']:
    print("Sample errors:")
    for error in debug_results['errors'][:5]:
        print(f"  {error}")
```

### State Validation

```python
def validate_brain_state(brain, expected_neuron_count, expected_region_count):
    """Validate brain state after operations"""
    errors = []
    
    # Check neuron count
    actual_neuron_count = brain.getTotalNeuronCount()
    if actual_neuron_count != expected_neuron_count:
        errors.append(f"Neuron count mismatch: expected {expected_neuron_count}, got {actual_neuron_count}")
    
    # Check region count
    actual_region_count = brain.getRegionCount()
    if actual_region_count != expected_region_count:
        errors.append(f"Region count mismatch: expected {expected_region_count}, got {actual_region_count}")
    
    # Check developmental stage
    stage = brain.getDevelopmentalStage()
    if stage == pynlm.DevelopmentalStage.Aging:
        errors.append("Brain reached aging stage prematurely")
    
    # Check for valid regions
    for rid in brain.getRegionIds():
        region = brain.getRegion(rid)
        if not region:
            errors.append(f"Invalid region ID: {rid}")
    
    if errors:
        print("State validation errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    else:
        print("Brain state validation passed!")
        return True

# Validate brain state
validate_brain_state(brain, 1000, 3)
```

## Example Advanced Projects

### Project: Adaptive Navigation

```python
import pynlm

class AdaptiveNavigator:
    def __init__(self):
        # Create brain with enhanced learning
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 2000)
        config.set("connection_probability", 0.12)
        config.set("reward_modulation_strength", 0.8)
        
        self.brain = pynlm.createBrain(config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        
        # Initialize agent with navigation-specific settings
        self.agent.enableAllFeatures()
        self.agent.enableRewardModulation(True)
        
        # Create navigation environment
        self.setup_navigation_environment()
        
        # Track learning progress
        self.learning_curve = []
        self.performance_metrics = {}
    
    def setup_navigation_environment(self):
        """Create navigation environment with landmarks and goals"""
        # Start position
        self.world.setAgentStart(5, 5)
        self.world.reset()
        
        # Create landmarks (resources to collect)
        for x in range(5, 25, 5):
            for y in range(5, 25, 5):
                if (x != 5 or y != 5) and (x != 25 or y != 25):
                    self.world.addObject(pynlm.WorldObject(x, y, 
                                                          pynlm.WorldObjectType.Resource, 5))
        
        # Create obstacles
        for x in [10, 15, 20]:
            for y in range(10, 20):
                self.world.addObject(pynlm.WorldObject(x, y, 
                                                      pynlm.WorldObjectType.Hazard, 2))
        
        # Set goal
        self.goal = (25, 25)
        self.world.addObject(pynlm.WorldObject(25, 25, 
                                              pynlm.WorldObjectType.Resource, 20))
    
    def run_navigation_episode(self, max_steps=200):
        """Run a single navigation episode"""
        episode_reward = 0
        episode_steps = 0
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get agent's current position
            agent_body = self.world.getAgentBody()
            current_pos = (agent_body.x, agent_body.y)
            
            # Calculate distance to goal
            distance_to_goal = math.sqrt((current_pos[0] - self.goal[0])**2 + 
                                       (current_pos[1] - self.goal[1])**2)
            
            # Provide proximity-based reward
            if distance_to_goal < 2.0:
                reward = 10.0  # Close to goal
            elif distance_to_goal < 5.0:
                reward = 1.0   # Getting closer
            else:
                reward = 0.0   # Far from goal
            
            # Apply navigation action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            episode_reward += reward
            episode_steps += 1
            
            # Check if reached goal
            if distance_to_goal < 1.0:
                break
        
        return episode_reward, episode_steps
    
    def train(self, episodes=50):
        """Train the navigator"""
        print("Training adaptive navigator...")
        
        for episode in range(episodes):
            reward, steps = self.run_navigation_episode()
            self.learning_curve.append(reward)
            
            if episode % 10 == 0:
                avg_reward = sum(self.learning_curve[-10:]) / 10
                print(f"Episode {episode}: Reward = {reward:.1f}, "
                      f"Steps = {steps}, Avg (last 10) = {avg_reward:.1f}")
        
        # Calculate final performance
        self.performance_metrics = {
            "total_episodes": episodes,
            "final_reward": self.learning_curve[-1],
            "average_reward": sum(self.learning_curve) / len(self.learning_curve),
            "success_rate": sum(1 for r in self.learning_curve if r > 10) / len(self.learning_curve),
            "efficiency": sum(self.learning_curve) / sum(range(1, episodes + 1))
        }
        
        return self.performance_metrics
    
    def evaluate(self, episodes=20):
        """Evaluate trained navigator"""
        print("Evaluating trained navigator...")
        
        evaluation_rewards = []
        for episode in range(episodes):
            reward, steps = self.run_navigation_episode()
            evaluation_rewards.append(reward)
        
        evaluation_metrics = {
            "evaluation_episodes": episodes,
            "average_evaluation_reward": sum(evaluation_rewards) / len(evaluation_rewards),
            "evaluation_consistency": max(evaluation_rewards) - min(evaluation_rewards)
        }
        
        return evaluation_metrics

# Use the adaptive navigator
navigator = AdaptiveNavigator()
metrics = navigator.train(episodes=30)
eval_metrics = navigator.evaluate(episodes=10)

print("\nTraining Results:")
for key, value in metrics.items():
    print(f"  {key}: {value:.3f}")

print("\nEvaluation Results:")
for key, value in eval_metrics.items():
    print(f"  {key}: {value:.3f}")
```

### Project: Memory and Concept Learning

```python
import pynlm

class ConceptLearner:
    def __init__(self):
        # Create specialized learning brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1500)
        config.set("region_count", 3)
        config.set("memory_capacity", 300)
        config.set("enable_concept_formation", True)
        
        self.brain = pynlm.createBrain(config)
        self.concept_formation = self.brain.getConceptFormation()
        self.associative_memory = self.brain.getAssociativeMemory()
        self.episodic_memory = self.brain.getEpisodicMemory()
        
        # Initialize learning environment
        self.learning_data = []
        self.concept_database = {}
    
    def generate_training_data(self, num_samples=1000):
        """Generate synthetic training data"""
        import random
        
        for _ in range(num_samples):
            # Generate random visual patterns
            pattern_size = 8
            pattern = [[random.random() for _ in range(pattern_size)] 
                      for _ in range(pattern_size)]
            
            # Determine underlying concept
            if self.is_symmetric(pattern):
                concept = "symmetric"
            elif self.is_gradient(pattern):
                concept = "gradient"
            elif self.is_texture(pattern):
                concept = "texture"
            else:
                concept = "random"
            
            self.learning_data.append({
                "pattern": pattern,
                "concept": concept,
                "timestamp": len(self.learning_data)
            })
        
        return self.learning_data
    
    def is_symmetric(self, pattern):
        """Check if pattern is symmetric"""
        for i in range(len(pattern)):
            for j in range(len(pattern[0])):
                if pattern[i][j] != pattern[len(pattern)-1-i][len(pattern[0])-1-j]:
                    return False
        return True
    
    def is_gradient(self, pattern):
        """Check if pattern has gradient""
        # Check rows
        for i in range(len(pattern)):
            for j in range(len(pattern[0])-1):
                if pattern[i][j+1] - pattern[i][j] < -0.5:  # Decreasing gradient
                    return True
        # Check columns
        for i in range(len(pattern)-1):
            for j in range(len(pattern[0])):
                if pattern[i+1][j] - pattern[i][j] < -0.5:  # Decreasing gradient
                    return True
        return False
    
    def is_texture(self, pattern):
        """Check if pattern has texture"""
        # Look for repeating patterns
        for size in range(2, min(len(pattern), len(pattern[0]))):
            if self.has_repeating_pattern(pattern, size):
                return True
        return False
    
    def has_repeating_pattern(self, pattern, size):
        """Check for repeating pattern blocks"""
        for i in range(0, len(pattern)-size, size):
            for j in range(0, len(pattern[0])-size, size):
                block = [row[j:j+size] for row in pattern[i:i+size]]
                for di in range(i+size, len(pattern)-size, size):
                    for dj in range(0, len(pattern[0])-size, size):
                        compare_block = [row[dj:dj+size] for row in pattern[di:di+size]]
                        if self.blocks_equal(block, compare_block):
                            return True
        return False
    
    def blocks_equal(self, block1, block2):
        """Check if two blocks are equal"""
        for i in range(len(block1)):
            for j in range(len(block1[0])):
                if abs(block1[i][j] - block2[i][j]) > 0.01:
                    return False
        return True
    
    def learn_concepts(self):
        """Train the brain to learn concepts"""
        print("Learning concepts...")
        
        for step, sample in enumerate(self.learning_data):
            # Convert pattern to vision input
            vision = pynlm.Vision(8, 8, 1)  # 8x8 grayscale
            flattened_data = []
            for row in sample["pattern"]:
                for val in row:
                    flattened_data.append(val)
            vision.setData(flattened_data)
            
            # Process with concept formation
            self.concept_formation.processInput(vision)
            
            # Store in episodic memory
            experience = {
                "pattern": sample["pattern"],
                "concept": sample["concept"],
                "timestamp": sample["timestamp"],
                "sensory_input": vision
            }
            self.episodic_memory.record(experience)
            
            # Learn associations
            self.associative_memory.associate(
                sample["concept"], 
                "pattern_feature", 
                0.9
            )
            
            # Learn through episodic memory replay
            if step % 100 == 0:
                self.episodic_memory.replay(sample["concept"], 50)
            
            if step % 200 == 0:
                print(f"Processed {step + 1}/{len(self.learning_data)} samples")
        
        # Extract learned concepts
        self.concept_database = self.concept_formation.getConcepts()
        return self.concept_database
    
    def test_concept_recognition(self, test_samples):
        """Test concept recognition on new data"""
        print("Testing concept recognition...")
        
        correct = 0
        total = 0
        
        for sample in test_samples:
            # Convert to vision input
            vision = pynlm.Vision(8, 8, 1)
            flattened_data = []
            for row in sample["pattern"]:
                for val in row:
                    flattened_data.append(val)
            vision.setData(flattened_data)
            
            # Process with concept formation
            self.concept_formation.processInput(vision)
            
            # Get predicted concept
            predicted_concept = self.concept_formation.getCurrentConcept()
            actual_concept = sample["concept"]
            
            total += 1
            if predicted_concept == actual_concept:
                correct += 1
        
        accuracy = correct / total if total > 0 else 0
        print(f"Concept recognition accuracy: {accuracy:.2%}")
        return accuracy

# Create and train concept learner
learner = ConceptLearner()
print("Generating training data...")
training_data = learner.generate_training_data(1000)
print(f"Generated {len(training_data)} training samples")

print("\nLearning concepts...")
concepts = learner.learn_concepts()
print(f"Discovered {len(concepts)} concepts")

# Create test data
import random
test_data = []
for _ in range(100):
    pattern_size = 8
    pattern = [[random.random() for _ in range(pattern_size)] 
              for _ in range(pattern_size)]
    if learner.is_symmetric(pattern):
        test_concept = "symmetric"
    elif learner.is_gradient(pattern):
        test_concept = "gradient"
    elif learner.is_texture(pattern):
        test_concept = "texture"
    else:
        test_concept = "random"
    test_data.append({"pattern": pattern, "concept": test_concept})

# Test concept recognition
accuracy = learner.test_concept_recognition(test_data)
print(f"\nFinal concept recognition accuracy: {accuracy:.2%}")
```

---

## Mathematical Foundations

### Neural Dynamics

The NLM brain uses Leaky Integrate-and-Fire (LIF) neurons:

```python
# Neuron membrane potential equation
dV/dt = (-V + I)/τ

# When V reaches threshold, neuron fires
if V >= threshold:
    spike = 1
    V = reset_potential
```

### Synaptic Plasticity (STDP)

```python
# Spike-Timing Dependent Plasticity
dw/dt = A⁺·exp(-Δt/τ⁺) - A⁻·exp(Δt/τ⁻)

# Where Δt = t_post - t_pre
# A⁺: LTP (long-term potentiation)
# A⁻: LTD (long-term depression)
# τ⁺, τ⁻: Time constants
```

### Hebbian Learning

```python
# Hebb's rule: "Neurons that fire together, wire together"
Δw = η·x_pre·x_post - λ·w

# Where:
# η: learning rate
# λ: decay rate
# x_pre, x_post: presynaptic and postsynaptic activities
```

### Reward Prediction Error

```python
# Dopamine-based learning
Δw = α·δ·x

# Where:
# δ = reward - predicted_reward
# α: learning rate (dopamine modulation)
# x: eligibility trace
```

## Advanced Python Features

### NumPy Integration

```python
import numpy as np
import pynlm

# Create brain with numpy-compatible interfaces
def create_numpy_compatible_brain(config):
    brain = pynlm.createBrain(config)
    
    # Override methods to work with numpy arrays
    original_step = brain.step
    
    def numpy_step(step_num, time=None):
        if time is None:
            original_step(step_num)
        else:
            original_step(step_num, time)
    
    brain.step = numpy_step
    return brain

# Use with numpy arrays
brain = create_numpy_compatible_brain(config)
vision_data = np.random.rand(64, 64, 3)  # 64x64 RGB image
```

### Multiprocessing Support

```python
from multiprocessing import Pool
import pynlm

def simulate_brain(config, steps):
    """Run brain simulation in separate process"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "average_firing_rate": brain.getAverageFiringRate()
    }

# Run simulations in parallel
if __name__ == "__main__":
    configs = [pynlm.createDefaultConfig() for _ in range(4)]
    with Pool(4) as pool:
        results = pool.starmap(simulate_brain, [(config, 1000) for config in configs])
    
    print("Parallel simulation results:")
    for i, result in enumerate(results):
        print(f"  Simulation {i + 1}: {result}")
```

### Visualization Integration

```python
import matplotlib.pyplot as plt
import pynlm

# Create brain and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Collect spike data
spike_times = []
for step in range(1000):
    brain.step(step)
    if brain.getFiringNeuronCount() > 0:
        spike_times.append(step)

# Plot spike raster
plt.figure(figsize=(12, 6))
plt.hist(spike_times, bins=100, alpha=0.7)
plt.xlabel("Simulation Step")
plt.ylabel("Spike Count")
plt.title("NLM Spike Raster Plot")
plt.grid(True, alpha=0.3)
plt.show()

# Plot firing rate over time
window_size = 50
firing_rates = []
for i in range(0, len(spike_times), window_size):
    window_spikes = len([t for t in spike_times if i <= t < i + window_size])
    firing_rates.append(window_spikes / window_size)

plt.figure(figsize=(12, 4))
plt.plot(range(0, len(spike_times), window_size), firing_rates)
plt.xlabel("Simulation Step")
plt.ylabel("Firing Rate (spikes/step)")
plt.title("NLM Firing Rate Over Time")
plt.grid(True, alpha=0.3)
plt.show()
```

## Performance Optimization

### Memory Management

```python
import gc
import pynlm

class MemoryManagedBrain:
    def __init__(self, config):
        self.config = config
        self.brain = None
    
    def create(self):
        """Create brain with memory pooling"""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        return self.brain
    
    def clear_memory(self):
        """Clear brain memory and force garbage collection"""
        if self.brain:
            self.brain.reset()
        
        # Force garbage collection
        gc.collect()
    
    def simulate_batch(self, batch_size, num_batches):
        """Run batch simulation with memory management"""
        results = []
        
        for batch in range(num_batches):
            # Create fresh brain for each batch
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            
            # Run batch
            for step in range(batch_size):
                brain.step(step)
                
                # Collect statistics
                if step % 100 == 0:
                    stats = {
                        "batch": batch,
                        "step": step,
                        "neuron_count": brain.getTotalNeuronCount(),
                        "firing_neurons": brain.getFiringNeuronCount(),
                        "total_spikes": brain.getTotalSpikeCount()
                    }
                    results.append(stats)
            
            # Clear brain memory
            brain.reset()
            del brain
            
            if batch % 10 == 0:
                print(f"Completed batch {batch + 1}/{num_batches}")
        
        return results
```

### Caching and Serialization

```python
import pickle
import pynlm

class BrainCache:
    def __init__(self):
        self.cache = {}
        self.max_cache_size = 10
    
    def get(self, key):
        """Get brain from cache"""
        if key in self.cache:
            # Load brain state
            brain = self.cache[key]
            return brain
        return None
    
    def put(self, key, brain, filepath=None):
        """Store brain in cache with optional serialization"""
        if len(self.cache) >= self.max_cache_size:
            # Remove oldest entry
            oldest_key = next(iter(self.cache))
            del self.cache[oldest_key]
        
        # Save to file if specified
        if filepath:
            brain.save(filepath)
        
        self.cache[key] = brain
    
    def serialize(self, key, filepath):
        """Serialize brain to file"""
        brain = self.get(key)
        if brain:
            with open(filepath, 'wb') as f:
                pickle.dump(brain, f)
    
    def deserialize(self, filepath):
        """Deserialize brain from file"""
        with open(filepath, 'rb') as f:
            return pickle.load(f)

# Use brain cache
cache = BrainCache()

# Create and cache a brain
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
cache.put("experiment_1", brain1)

# Save to file
cache.serialize("experiment_1", "brain_cache.pkl")

# Load from file
loaded_brain = cache.deserialize("brain_cache.pkl")
```

## Troubleshooting and Common Issues

### Common Error Messages and Solutions

1. **"Brain initialization failed: Configuration cannot be null"**
   - **Solution**: Check that config is not None before creating brain
   ```python
   if config is None:
       config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   ```

2. **"Invalid operation: Unsupported configuration value type"**
   - **Solution**: Check configuration value types before setting
   ```python
   if isinstance(value, (int, float, str, bool)):
       config.set("key", value)
   else:
       raise ValueError("Unsupported configuration value type")
   ```

3. **"No sensory input available"**
   - **Solution**: Ensure world has been updated before getting sensory input
   ```python
   world.update(0.1)
   percept = world.getSensoryPercept()
   if percept:
       agent.processSensoryInput(percept)
   ```

4. **"Memory limit exceeded"**
   - **Solution**: Monitor and manage memory usage
   ```python
   if brain.getTotalNeuronCount() > max_neurons:
       print("Warning: High neuron count may impact performance")
   ```

### Debugging Techniques

1. **Print Brain Statistics**
   ```python
   for step in range(100):
       brain.step(step)
       if step % 10 == 0:
           print(f"Step {step}:")
           print(f"  Neurons: {brain.getTotalNeuronCount()}")
           print(f"  Firing: {brain.getFiringNeuronCount()}")
           print(f"  Spikes: {brain.getTotalSpikeCount()}")
           print(f"  Rate: {brain.getAverageFiringRate():.3f}")
           print(f"  Stage: {brain.getDevelopmentalStage()}")
   ```

2. **Log Brain Events**
   ```python
   # Enable logging
   logger = pynlm.Logger.getGlobal()
   logger.setLevel(pynlm.LogLevel.Info)
   
   # Log brain events
   brain.logStatus()
   
   # Use structured logging
   logger.info(f"Step {step}: Processing complete")
   ```

3. **Validate Configuration**
   ```python
   def validate_config(config):
       errors = []
       
       # Check required keys
       required_keys = ["neuron_count", "region_count"]
       for key in required_keys:
           if not config.has(key):
               errors.append(f"Missing required key: {key}")
       
       # Check values
       if config.has("neuron_count") and config.getInt("neuron_count") < 10:
           errors.append("Neuron count too low")
       
       if config.has("region_count") and config.getInt("region_count") > 10:
           errors.append("Region count too high")
       
       return errors
   
   config_errors = validate_config(config)
   if config_errors:
       print("Configuration errors:")
       for error in config_errors:
           print(f"  - {error}")
   ```

## Best Practices

### Coding Standards

1. **Use descriptive variable names**
2. **Follow the single responsibility principle**
3. **Document your code with docstrings**
4. **Use type hints where possible**
5. **Handle exceptions gracefully**
6. **Write modular, reusable code**

### Performance Considerations

1. **Batch operations when possible**
2. **Reuse objects instead of creating new ones**
3. **Monitor memory usage**
4. **Use efficient data structures**
5. **Profile your code regularly**

### Testing and Validation

1. **Test with different configurations**
2. **Validate brain state after operations**
3. **Check for memory leaks**
4. **Benchmark performance**
5. **Test edge cases**

### Documentation

1. **Document configuration options**
2. **Provide usage examples**
3. **Document performance characteristics**
4. **Include troubleshooting information**
5. **Maintain code comments**

## Further Reading and Resources

### Documentation

- **NLM Architecture Documentation**: `docs/ARCHITECTURE.md`
- **Phase 6 Audit Report**: `docs/PHASE6_FINAL_AUDIT.md`
- **Scientific Background**: `docs/SCIENCE.md`
- **Performance Analysis**: `docs/PERFORMANCE.md`

### Research Papers

- "Artificial Developmental Systems: Principles and Practice"
- "Neural Network Models for Cognitive Development"
- "Spiking Neural Networks with Memory and Learning"

### Software Resources

- **NLM GitHub Repository**: https://github.com/nlm-project/nlm
- **Python Bindings Documentation**: https://nlm.readthedocs.io
- **Phase 6 Demo**: `./build/nlm_phase6_demo`

### Community and Support

- **NLM Research Group**: https://nlm-research.org
- **Python Support**: https://github.com/nlm-project/pynlm/issues
- **Discourse Forum**: https://forum.nlm-research.org
- **Documentation**: https://docs.nlm-research.org

---

## Conclusion

This advanced usage guide provides comprehensive information for working with the NLM Python bindings. Whether you're creating simple neural simulations, implementing complex learning algorithms, or optimizing performance for research applications, NLM provides the tools you need.

Key takeaways:

1. **Start with simple examples** and gradually build complexity
2. **Understand configuration options** and their impact on performance
3. **Use advanced features** like memory systems and prediction
4. **Monitor performance** and optimize for your specific use case
5. **Document your work** and share results with the community

NLM represents a significant step forward in artificial developmental systems, combining real neural dynamics with advanced cognitive capabilities. With the comprehensive Python bindings and advanced features covered in this guide, you have everything you need to create sophisticated neural learning systems.

Enjoy experimenting with NLM and happy coding! 🚀