# Advanced User Guide - NLM

## Overview

This document provides comprehensive guidance for advanced NLM users, covering configuration management, brain system customization, memory systems, learning algorithms, neuromodulation, agent training, and environment management. This guide builds upon the basic usage concepts and introduces advanced features for power users and researchers.

## Configuration Management

### Working with Config Files

NLM provides a comprehensive configuration system with support for multiple formats:

```python
import nlm

# Create configuration manager
config = nlm.Config()

# Load from file (auto-detects format)
config.load_file("./configs/neural_network.json")

# Or load from plain text
config.load_text("""
# Neural network configuration
brain_regions = 5
neurons_per_region = 100
learning_rate = 0.01
plasticity_type = "hebbian"
""")

# Save with different formats
config.save("output/config.yaml", format="yaml")
config.save("output/config.json", format="json")

# Validate configuration
if config.validate():
    print("Configuration is valid!")
```

### Advanced Configuration Commands

```bash
# List available configuration files
config list

# Validate configuration file format
config validate configs/neural_network.json

# Convert between JSON and YAML formats
config convert configs/network.json configs/network.yaml

# Merge multiple configuration files
config merge configs/simple.cfg configs/advanced.json configs/experiment.yaml output.cfg

# Extract specific configuration parameters
config extract configs/complex.json brain_regions neurons_per_region learning_rate

# Generate configuration templates
config template neural my_neural_template.cfg

# Show configuration statistics
config statistics configs/neural_network.json
```

### Brain System Configuration

```python
# Advanced brain configuration
brain_config = nlm.BrainConfig()

# Set neuron parameters
brain_config.set("neuron_type", "lif")
brain_config.set("threshold", -55.0)
brain_config.set("resting_potential", -70.0)
brain_config.set("refractory_period", 5)

# Set synaptic parameters
brain_config.set("synaptic_strength", 0.1)
brain_config.set("synaptic_delay", 1.0)
brain_config.set("inhibition_strength", 0.05)

# Create brain with advanced configuration
brain = nlm.create_brain(brain_config)
```

#### Brain Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_type` | string | "lif" | Neuron model type ("lif", "ife", "hh") |
| `threshold` | float | -55.0 | Membrane potential threshold |
| `resting_potential` | float | -70.0 | Membrane resting potential |
| `refractory_period` | int | 5 | Refractory period in ms |
| `synaptic_strength` | float | 0.1 | Default synaptic strength |
| `synaptic_delay` | float | 1.0 | Synaptic transmission delay |
| `inhibition_strength` | float | 0.05 | Inhibition strength |

#### Adding Brain Regions

```python
# Add multiple brain regions with different properties
brain_config.add_region("visual_cortex", {
    "neuron_count": 300,
    "neuron_type": "excitatory",
    "receptive_field_size": 5,
    "plasticity": "stdp"
})

brain_config.add_region("motor_cortex", {
    "neuron_count": 200,
    "neuron_type": "motor",
    "output_type": "action",
    "plasticity": "hebbian"
})

brain_config.add_region("hippocampus", {
    "neuron_count": 150,
    "neuron_type": "internal",
    "memory_function": "episodic",
    "plasticity": "hebbian_ltp"
})
```

## Memory System Management

### Memory System Configuration

```python
# Configure memory systems
memory_config = nlm.MemoryConfig()

# Episodic memory settings
memory_config.set("max_episodes", 1000)
memory_config.set("consolidation_threshold", 0.7)
memory_config.set("replay_frequency", 0.1)

# Working memory settings
memory_config.set("working_memory_capacity", 100)
memory_config.set("decay_rate", 0.01)
memory_config.set("update_frequency", 0.05)

# Semantic memory settings
memory_config.set("semantic_retention", 3600)  # seconds
memory_config.set("semantic_decay", 0.001)
memory_config.set("fact_limit", 10000)

# Create memory system
memory = nlm.create_memory_system(memory_config)
```

### Memory System Operations

#### Episodic Memory

```python
# Encode experiences
episode_id = episodic_memory.encode({
    "stimulus": "red_square",
    "location": (10, 10),
    "action": "approach",
    "outcome": "reward",
    "context": {"time_of_day": "morning", "weather": "sunny"}
}, timestamp=time.time())

# Retrieve episodes with filtering
retrieved_episodes = episodic_memory.retrieve(
    filter_keywords=["reward", "approach"],
    time_window=3600,  # last hour
    min_strength=0.7
)

# Consolidate important memories
consolidated = episodic_memory.consolidate(
    importance_threshold=0.8
)

# Analyze patterns in memory
patterns = episodic_memory.analyze_patterns()
```

#### Working Memory

```python
# Add items to working memory
working_memory.add_item(
    item_id="stimulus_001",
    timestamp=time.time(),
    data={"feature": "red", "shape": "square", "size": "large"},
    strength=0.9
)

# Get active items
active_items = working_memory.get_active_items()

# Get competition strength
competition = working_memory.get_competition_strength("stimulus_001")
```

## Learning and Plasticity Control

### Plasticity Configuration

```python
# Configure plasticity rules
plasticity_config = nlm.PlasticityConfig()

# STDP parameters
plasticity_config.set("stdp", {
    "learning_rate": 0.01,
    "time_window": 20,
    " LTP_weight": 0.02,
    " LTD_weight": 0.015,
    "tau": 20.0
})

# Hebbian parameters
plasticity_config.set("hebbian", {
    "learning_rate": 0.02,
    "correlation_window": 10
})

# Structural plasticity parameters
plasticity_config.set("structural", {
    "growth_rate": 0.001,
    "pruning_rate": 0.0005,
    "max_synapses_per_neuron": 100
})
```

### Learning Algorithms

#### Spike-Timing-Dependent Plasticity (STDP)

```python
# Configure STDP learning rule
stdp_config = nlm.STDPConfig()
stdp_config.learning_rate = 0.01
stdp_config.time_window = 20
stdp_config.LTP_weight = 0.02
stdp_config.LTD_weight = 0.015

# Apply STDP rule
brain.apply_plasticity_rule("stdp", stdp_config)
```

#### Hebbian Learning

```python
# Configure Hebbian learning
hebbian_config = nlm.HebbianConfig()
hebbian_config.learning_rate = 0.02
hebbian_config.correlation_window = 10

# Apply Hebbian rule
brain.apply_plasticity_rule("hebbian", hebbian_config)
```

#### Adaptive Learning

```python
# Configure adaptive learning
adaptive_config = nlm.AdaptiveLearningConfig()
adaptive_config.base_learning_rate = 0.01
adaptive_config.performance_threshold = 0.7
adaptive_config.adaptation_rate = 0.1

# Create adaptive learning controller
adaptive_learning = nlm.AdaptiveLearning(adaptive_config)
```

### Learning Scheduler

```python
# Configure learning scheduler
scheduler = nlm.LearningScheduler(
    base_interval=1.0,
    adaptation_rate=0.1,
    max_concurrent_sessions=3
)

# Schedule learning based on performance
scheduler.schedule_learning(
    trigger="performance",
    performance=0.8,
    priority="high"
)

# Execute scheduled sessions
executed_sessions = scheduler.execute_scheduled_sessions()
```

## Neuromodulation and Reward Systems

### Neuromodulator Configuration

```python
# Configure neuromodulation systems
neuromod_config = nlm.NeuromodConfig()

# Dopamine system
neuromod_config.dopamine.set({
    "baseline": 0.1,
    "learning_rate": 0.05,
    "prediction_horizon": 10
})

# Serotonin system
neuromod_config.serotonin.set({
    "baseline": 0.05,
    "modulation_strength": 0.8
})

# Curiosity system
neuromod_config.curiosity.set({
    "exploration_rate": 0.1,
    "novelty_threshold": 0.7
})
```

### Reward Processing

#### Dopamine System

```python
# Create dopamine system
dopamine_system = nlm.DopamineSystem(
    learning_rate=0.1,
    prediction_horizon=10
)

# Process reward signals
for step in range(100):
    reward = calculate_reward(step)
    prediction = predict_reward(step)
    
    result = dopamine_system.update(reward, prediction)
    print(f"Dopamine level: {result['dopamine_level']:.3f}")
```

#### Reward Engine

```python
# Create reward engine
reward_engine = nlm.RewardEngine(
    discount_factor=0.9,
    exploration_bonus=0.1
)

# Process rewards
for step in range(100):
    reward = calculate_environment_reward(step)
    action = agent_action
    state = current_state
    
    reward_result = reward_engine.process_reward(reward, action, state)
    print(f"Future reward prediction: {reward_result['predicted_future_reward']:.3f}")
```

#### Prediction Error Tracker

```python
# Create prediction error tracker
error_tracker = nlm.PredictionErrorTracker(
    time_window=100,
    smoothing_factor=0.9
)

# Track prediction errors
for step in range(100):
    predicted = predict_outcome(step)
    actual = get_actual_outcome(step)
    
    error = actual - predicted
    error_tracker.record_error(error, timestamp=step)

# Analyze prediction errors
error_analysis = error_tracker.get_error_analysis()
print(f"Average prediction error: {error_analysis['average_error']:.3f}")
```

### Motivation System

```python
# Configure motivation system
motivation_config = nlm.MotivationConfig()
motivation_config.set("goal_formation_threshold", 0.6)
motivation_config.set("reward_prediction_threshold", 0.8)

# Create motivation system
motivation_system = nlm.MotivationSystem(motivation_config)

# Add goals
motivation_system.add_goal(
    goal_id="reach_target",
    description="Reach the target location",
    target_value=100,
    deadline=1000
)

# Update goal progress
motivation_system.update_goal_progress("reach_target", 50)
```

## Agent Training and Evaluation

### Agent Trainer Configuration

```python
# Configure agent trainer
trainer_config = nlm.AgentTrainerConfig(
    learning_rate=0.01,
    batch_size=32,
    discount_factor=0.99,
    entropy_coefficient=0.01,
    target_network_update_freq=100
)

# Create agent trainer
trainer = nlm.AgentTrainer(trainer_config)
```

### Training Process

```python
# Training loop
def train_agent(agent_brain, environment, trainer, max_episodes=1000):
    for episode in range(max_episodes):
        state = environment.reset()
        done = False
        episode_reward = 0
        
        while not done:
            # Select action
            action = select_action(state)
            
            # Execute action
            next_state, reward, done = environment.step(action)
            
            # Store experience
            trainer.add_experience(state, action, reward, next_state, done)
            
            # Train on batch
            if len(trainer.experience_replay) >= trainer.batch_size:
                trainer.train_step(agent_brain)
            
            state = next_state
            episode_reward += reward
        
        # Periodically evaluate
        if episode % 100 == 0:
            evaluation = evaluator.evaluate(agent_brain, environment, episodes=10)
            print(f"Episode {episode}: Average reward = {evaluation['average_reward']:.2f}")
```

### Agent Evaluation

```python
# Configure evaluator
evaluator_config = nlm.EvaluatorConfig(
    evaluation_metrics=["reward", "success_rate", "learning_speed", "stability"],
    baseline_results_file="baseline_performance.json",
    comparison_threshold=0.9
)

# Create evaluator
evaluator = nlm.Evaluator(evaluator_config)

# Run evaluation
evaluation_results = evaluator.evaluate(
    agent_brain=agent_brain,
    environment=environment,
    episodes=50,
    max_steps=200
)

# Generate evaluation report
report = evaluator.generate_report(
    format="detailed",
    include_baseline_comparison=True
)
```

### Checkpoint Management

```python
# Configure checkpoint manager
checkpoint_config = nlm.CheckpointManagerConfig(
    checkpoint_dir="checkpoints",
    max_checkpoints=20,
    save_frequency=100,
    include_optimizer_state=True
)

# Create checkpoint manager
checkpoint_manager = nlm.CheckpointManager(checkpoint_config)

# Save agent state
checkpoint_manager.save(
    brain=agent_brain,
    memory=memory_system,
    world=environment,
    name="episode_1000",
    metadata={"episode": 1000, "reward": 850.0, "success": True}
)

# Load agent state
brain, memory, world = checkpoint_manager.load("episode_1000")
```

## Environment and World Simulation

### Environment Configuration

```python
# Configure world simulation
world_config = nlm.WorldConfig(
    width=100,
    height=100,
    time_step=0.1,
    physics_enabled=True,
    boundary_type="toroidal"
)

# Create world simulator
world = nlm.WorldSimulator(world_config)
```

### Environment Types

#### Simple Environment

```python
# Create simple environment
simple_env = nlm.create_simple_environment(
    width=50,
    height=50,
    num_resources=10,
    obstacle_density=0.1
)
```

#### Complex Environment

```python
# Create complex environment
complex_env = nlm.create_complex_environment(
    width=100,
    height=100,
    num_hazards=5,
    num_resources=20,
    terrain_types=["grass", "water", "mountain"],
    dynamic_elements=True
)
```

#### Procedural Environment

```python
# Create procedural environment
procedural_env = nlm.create_procedural_environment(
    env_type="forest",
    seed=42,
    complexity=0.7,
    resource_density=0.3
)
```

### Environment Operations

```python
# Configure environment
environment = world.create_environment("simple")
environment.configure(width=80, height=60, vision_range=15)

# Reset to initial state
environment.reset()

# Set agent starting position
environment.set_agent_start(40.0, 30.0)

# Add objects to environment
environment.add_object("target", (70.0, 40.0), {"type": "goal", "reward": 100})
environment.add_object("obstacle", (50.0, 30.0), {"type": "hazard", "damage": -20})

# Update environment
for step in range(100):
    environment.update(time_step=0.1)
    
    # Get agent percept
    percept = environment.get_sensory_percept()
    
    # Get world state for analysis
    world_state = environment.get_world_state()
    
    # Check if position is valid
    is_valid = environment.is_valid_position(agent_x, agent_y)
```

### Environment Factory

```python
# Use environment factory for different scenarios
factory = nlm.EnvironmentFactory()

# Create specific environments
neural_world = factory.create_simple_environment(100, 100, 15)
challenge_world = factory.create_complex_environment(200, 200, 10, 25)
research_world = factory.create_procedural_environment("desert", seed=123)
```

### Scenario Management

```python
# Configure scenario manager
scenario_config = nlm.ScenarioConfig(
    default_scenario="standard_training",
    enable_scenarios=True,
    validate_scenarios=True
)

# Create scenario manager
scenario_manager = nlm.ScenarioManager(scenario_config)

# Create experimental scenarios
scenario_manager.create_scenario(
    name="novelty_learning",
    description="Learning with novel stimuli",
    parameters={
        "environment_type": "simple",
        "stimulus_variety": "high",
        "learning_rate": 0.05,
        "curiosity_weight": 0.8
    }
)

# Load scenario for use
current_scenario = scenario_manager.load_scenario("novelty_learning")
```

## Performance Optimization

### Memory Management

```python
# Configure performance-optimized NLM system
optimized_nlm = nlm.NLM()

# Configure for performance
optimized_nlm.set("thread_pool_size", 8)
optimized_nlm.set("memory_limit", "4GB")
optimized_nlm.set("cache_size", 1000)
optimized_nlm.set("auto_cleanup", True)
```

### Profiling and Monitoring

```python
# Enable profiling
profiling = optimized_nlm.start_profiling()

# Run simulation with profiling
results = optimized_nlm.run_simulation(steps=500)

# Get performance metrics
metrics = profiling.get_metrics()
print(f"Performance score: {metrics.get('performance_score', 0):.2f}")
print(f"Memory usage: {metrics.get('memory_usage', 0) / 1024 / 1024:.2f} MB")
print(f"CPU utilization: {metrics.get('cpu_utilization', 0):.1f}%")
```

### Batch Processing

```python
# Configure batch processor
batch_config = nlm.BatchProcessorConfig(
    batch_size=10,
    n_workers=4,
    parallelization="thread",
    checkpoint_frequency=5
)

# Create batch processor
batch_processor = nlm.BatchProcessor(batch_config)

# Create and start batch
batch_id = batch_processor.create_batch(
    batch_id="experiment_1",
    experiment_names=["simple_brain", "complex_brain"],
    iterations_per_experiment=20,
    parameter_combinations=[
        {"regions": 5, "neurons_per_region": 100},
        {"regions": 10, "neurons_per_region": 200}
    ]
)

batch_processor.start_batch(batch_id)
```

## Extension and Customization

### Custom Extensions

```python
import nlm
from nlm.extensions import NLMExtension

# Create custom extension
class CustomExtension(NLMExtension):
    def __init__(self, nlm_instance):
        self.nlm = nlm_instance
    
    def setup(self):
        """Setup extension"""
        self.nlm.register_custom_rule("custom_plasticity", self.custom_rule)
    
    def custom_rule(self, synapse, pre_spikes, post_spikes):
        """Custom plasticity rule"""
        # Implement custom learning rule
        weight_change = calculate_custom_rule(pre_spikes, post_spikes)
        return weight_change

# Register extension
extension = CustomExtension(nlm_system)
extension.setup()
```

### Plugin System

```python
# Create plugin for custom functionality
class AgentPlugin:
    def __init__(self, agent_trainer):
        self.agent_trainer = agent_trainer
    
    def add_custom_feature(self):
        """Add custom feature to agent"""
        # Implement custom agent feature
        pass
    
    def modify_training(self):
        """Modify training process"""
        # Modify training algorithm
        pass

# Register plugin
plugin = AgentPlugin(trainer)
plugin.add_custom_feature()
```

## Key Advanced Features

### 1. **Adaptive Learning**
   - Dynamic learning rate adjustment based on performance
   - Algorithm selection based on current state
   - Performance-based plasticity modulation

### 2. **Multi-Modal Memory**
   - Working memory for immediate processing
   - Episodic memory for experience storage
   - Semantic memory for knowledge representation
   - Cross-memory consolidation

### 3. **Neuromodulation Integration**
   - Dopamine-based reward prediction
   - Serotonin-based exploration-exploitation balance
   - Acetylcholine-based attention modulation
   - Noradrenaline-based arousal control

### 4. **Advanced Plasticity**
   - Spike-timing-dependent plasticity (STDP)
   - Hebbian learning with correlation
   - Structural plasticity for connection growth
   - Neuromodulator-gated plasticity

### 5. **Complex Environment Simulation**
   - Physics-based interactions
   - Dynamic elements and obstacles
   - Resource management and foraging
   - Multi-agent environments

### 6. **Machine Learning Integration**
   - Neural network training alongside spiking networks
   - Policy gradient methods for control
   - Meta-learning for rapid adaptation
   - Transfer learning between tasks

### 7. **Experimental Automation**
   - Automated experiment design
   - Batch processing of parameter sweeps
   - Adaptive experimental protocols
   - Results aggregation and analysis

## Next Steps

1. **Master configuration management** - Learn to create, load, and save complex configuration files
2. **Explore brain customization** - Experiment with different neuron types and network topologies
3. **Deepen memory system knowledge** - Understand memory consolidation and retrieval mechanisms
4. **Study neuromodulation** - Learn how different neuromodulators affect learning and behavior
5. **Practice agent training** - Implement advanced training algorithms and evaluation metrics
6. **Optimize performance** - Learn profiling, monitoring, and batch processing techniques
7. **Create extensions** - Develop custom functionality for specific research needs
8. **Participate in research** - Use these tools for neuroscience, AI, and machine learning research

The advanced NLM system provides a powerful platform for creating intelligent agents with biologically plausible learning mechanisms. By mastering these features, you can build sophisticated simulation experiments that advance our understanding of learning, memory, and intelligent behavior.