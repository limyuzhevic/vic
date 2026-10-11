# NLM Frequently Asked Questions - FAQs

## Overview

This document provides answers to frequently asked questions about NLM (Neural Learning Machine). It covers common issues, best practices, and troubleshooting tips for users at all skill levels.

## Quick Answers

### 1. What is NLM?

NLM is a brain-inspired spiking neural network simulator and computational neuroscience framework. It creates virtual brains that can learn, remember, and make decisions based on interaction with environments.

### 2. How do I install NLM?

**From PyPI:**
```bash
pip install nlm
```

**From Source:**
```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install -e python/

# Build C++ core (if needed)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 3. How do I get started with NLM?

**Basic Steps:**
1. Install NLM
2. Create a simple brain configuration
3. Initialize the brain
4. Run a simulation

**Example:**
```python
import nlm

# Create NLM system
nlm_system = nlm.NLM()

# Create brain with configuration
brain = nlm_system.create_agent({"regions": 5, "neurons_per_region": 100})

# Run simulation
results = nlm_system.run_simulation(steps=100)

# Evaluate performance
evaluation = nlm_system.evaluate()
```

### 4. What are the main differences between easy_usage.md and HOW_TO_USE.md?

**easy_usage.md:**
- Beginner-friendly quick start guide
- Simple examples and analogies
- Focus on getting started quickly
- 7 comprehensive examples with step-by-step instructions

**HOW_TO_USE.md:**
- Detailed usage instructions
- Covers building the C++ project from source
- Includes advanced configuration and usage
- 533 pages of comprehensive documentation

### 5. What programming languages does NLM support?

**Primary Support:**
- Python (3.8+): Complete API and high-level interface
- C++ (for core implementation): Performance-critical components

**Python API Modules:**
- `nlm.config`: Configuration management
- `nlm.brain`: Brain and neural network management
- `nlm.memory`: Memory system integration
- `nlm.learning`: Learning and plasticity control
- `nlm.neuromodulation`: Neuromodulation and reward systems
- `nlm.agent`: Agent control and management
- `nlm.environment`: Environment and world management
- `nlm.visualization`: Visualization and analysis
- `nlm.automation`: Automation and scripting
- `nlm.ml`: Advanced machine learning integration

### 6. What are the system requirements?

**Minimum Requirements:**
- **Operating System:** Linux, macOS, or Windows
- **Python Version:** 3.8 or higher
- **Memory:** Minimum 8GB RAM, recommended 16GB+
- **Disk Space:** Minimum 1GB for installation
- **Compiler:** C++ compiler for building extensions (if needed)

**Recommended Requirements:**
- **CPU:** Multi-core processor (4+ cores)
- **Memory:** 16GB+ RAM
- **Storage:** SSD for better performance
- **Graphics:** Dedicated GPU for visualization

### 7. How do I run tests?

**Unit Tests:**
```bash
pytest python/nlm/tests/ -v
```

**Integration Tests:**
```bash
pytest python/integration_tests/ -v
```

**Performance Tests:**
```bash
python -m pytest python/performance_tests/ -v
```

**All Tests:**
```bash
pytest -v
```

### 8. How do I create custom brain configurations?

**Using Configuration File:**

Create a configuration file (e.g., `custom_config.yaml`):

```yaml
brain:
  regions: 5
  neurons_per_region: 100
  neuron_type: "lif"
  plasticity: "stdp"
environment:
  type: "simple"
  width: 50
  height: 50
  vision_range: 15
learning:
  rate: 0.01
  algorithm: "stdp"
  discount_factor: 0.99
memory:
  working_capacity: 100
  episodic_capacity: 1000
  consolidation_threshold: 0.7
```

**Using Python API:**
```python
import nlm

# Create configuration
config = nlm.nlm_config()

# Set custom parameters
config.set("brain.regions", 5)
config.set("brain.neurons_per_region", 100)
config.set("brain.neuron_type", "lif")
config.set("brain.plasticity", "stdp")

# Create brain with custom configuration
brain = nlm.BrainNetwork(config)
```

### 9. How do I add new brain regions?

**Using Python API:**
```python
import nlm

# Create brain
brain = nlm.BrainNetwork()
brain.initialize()

# Add different types of regions
brain.add_region("visual_cortex", 300, "excitatory")
brain.add_region("motor_cortex", 200, "motor")
brain.add_region("hippocampus", 150, "internal")
brain.add_region("cerebellum", 100, "coordination")

# Get region information
region_info = brain.get_region_info("visual_cortex")
print(f"Region: {region_info['name']}")
print(f"Neurons: {region_info['neuron_count']}")
print(f"Type: {region_info['neuron_type']}")
```

### 10. How do I enable learning and plasticity?

**Using Agent API:**
```python
import nlm

# Create NLM system
nlm_system = nlm.NLM()

# Create brain and enable learning
brain = nlm_system.create_agent({
    "regions": 3,
    "neurons_per_region": 100
})

# Enable different types of learning
nlm_system.enable_reward_modulation(True)  # Learn from rewards
nlm_system.enable_structural_plasticity(True)  # Grow new connections
nlm_system.enable_development(True)  # Brain matures over time
nlm_system.enable_curiosity(True)  # Explore new things
```

### 11. How do I work with memory systems?

**Episodic Memory:**
```python
import nlm

# Create episodic memory
episodic_memory = nlm.EpisodicMemory(capacity=1000, consolidation_threshold=0.7)

# Encode experiences
episode_id = episodic_memory.encode({
    "stimulus": "red_square",
    "location": (10, 10),
    "action": "approach",
    "outcome": "reward"
}, timestamp=time.time())

# Retrieve episodes
retrieved = episodic_memory.retrieve(filter_keywords=["reward", "approach"])

# Consolidate important memories
consolidated = episodic_memory.consolidate()
```

**Working Memory:**
```python
# Create working memory
working_memory = nlm.WorkingMemory(capacity=50, decay_rate=0.01)

# Add items
working_memory.add_item("stimulus_001", time.time(), {"feature": "red"}, strength=0.9)

# Get active items
active_items = working_memory.get_active_items()
```

### 12. How do I implement neuromodulation?

**Dopamine System:**
```python
import nlm

# Create dopamine system
dopamine_system = nlm.DopamineSystem(learning_rate=0.1, prediction_horizon=10)

# Process reward signals
for step in range(100):
    reward = calculate_reward(step)
    prediction = predict_reward(step)
    
    result = dopamine_system.update(reward, prediction)
    print(f"Dopamine level: {result['dopamine_level']:.3f}")
```

**Reward Engine:**
```python
# Create reward engine
reward_engine = nlm.RewardEngine(discount_factor=0.9, exploration_bonus=0.1)

# Process rewards
for step in range(100):
    reward = calculate_environment_reward(step)
    action = agent_action
    state = current_state
    
    reward_result = reward_engine.process_reward(reward, action, state)
    print(f"Future reward prediction: {reward_result['predicted_future_reward']:.3f}")
```

### 13. How do I train agents?

**Basic Training:**
```python
import nlm

# Create agent trainer
trainer = nlm.AgentTrainer(learning_rate=0.01, batch_size=32)

# Create evaluator
evaluator = nlm.Evaluator()

# Create checkpoint manager
checkpoint_manager = nlm.CheckpointManager()

# Training loop
for episode in range(100):
    # Get state
    state = environment.reset()
    
    # Run episode
    for step in range(100):
        # Select action
        action = select_action(state)
        
        # Execute action
        next_state, reward, done = environment.step(action)
        
        # Store experience
        trainer.add_experience(state, action, reward, next_state, done)
        
        # Train agent
        if len(trainer.experience_replay) >= trainer.batch_size:
            trainer.train_step(agent_brain)
        
        state = next_state
        if done:
            break
    
    # Evaluate periodically
    if episode % 20 == 0:
        evaluation = evaluator.evaluate(agent_brain, environment)
        print(f"Episode {episode}: Reward = {evaluation['average_reward']:.2f}")
```

### 14. How do I use different environment types?

**Simple Environment:**
```python
import nlm

# Create simple environment
simple_env = nlm.create_simple_environment(
    width=50,
    height=50,
    num_resources=10,
    obstacle_density=0.1
)
```

**Complex Environment:**
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

**Procedural Environment:**
```python
# Create procedural environment
procedural_env = nlm.create_procedural_environment(
    env_type="forest",
    seed=42,
    complexity=0.7,
    resource_density=0.3
)
```

### 15. How do I visualize neural activity?

**Basic Visualization:**
```python
import nlm
import matplotlib.pyplot as plt

# Create neural activity plotter
plotter = nlm.Plotter(style="scientific")

# Generate sample data
activity_data = np.random.randn(100, 1000)

# Plot neural activity
fig = plotter.plot_neural_activity(activity_data, title="Neural Activity")
plt.show()

# Plot connectivity matrix
connectivity = np.random.rand(50, 50)
fig = plotter.plot_connectivity_matrix(connectivity, title="Connectivity Matrix")
plt.show()
```

**Heatmap Visualization:**
```python
# Create heatmap generator
heatmap_generator = nlm.HeatmapGenerator(colormap="viridis", normalize=True)

# Generate neural heatmaps
heatmap = heatmap_generator.generate_neural_heatmap(
    activity_data, title="Neural Activity Heatmap"
)

# Generate spatial heatmaps
spatial_heatmap = heatmap_generator.generate_spatial_heatmap(
    positions, values, title="Spatial Heatmap"
)
```

### 16. How do I run automated experiments?

**Experiment Runner:**
```python
import nlm

# Create experiment runner
experiment_runner = nlm.ExperimentRunner()

# Create experiments
experiments = [
    {"name": "simple_brain", "parameters": {"regions": 3}},
    {"name": "complex_brain", "parameters": {"regions": 10}},
]

# Run experiments
for exp in experiments:
    results = experiment_runner.run_experiment(
        exp["name"], 
        iterations=10
    )

# Analyze results
aggregated = experiment_runner.analyze_results(list(results.values()))
```

### 17. How do I integrate machine learning with NLM?

**Neural Network Training:**
```python
import nlm
import numpy as np

# Create neural network trainer
trainer = nlm.NeuralNetworkTrainer(
    learning_rate=0.001,
    batch_size=32,
    optimizer="adam",
    loss_function="mse"
)

# Initialize model
architecture = {
    "input_size": 100,
    "hidden_layers": [256, 128, 64],
    "output_size": 10,
    "output_activation": "softmax"
}

trainer.initialize_model(architecture)

# Train model
results = trainer.train_model(X_train, y_train, X_val, y_val)
```

**Policy Gradient Learning:**
```python
# Create policy gradient learner
pg_learner = nlm.PolicyGradientLearner(
    policy_network,
    learning_rate=0.001,
    discount_factor=0.99,
    entropy_coefficient=0.01
)

# Training loop
for episode in range(max_episodes):
    # Generate episode
    episode_data = generate_episode(pg_learner, environment)
    
    # Store in replay buffer
    pg_learner.add_experience(episode_data)
    
    # Train
    if len(pg_learner.experience_replay) >= pg_learner.batch_size:
        pg_learner.train_step()
```

### 18. How do I save and load agent states?

**Save Checkpoint:**
```python
import nlm

# Create checkpoint manager
checkpoint_manager = nlm.CheckpointManager()

# Create and train agent
agent_brain = nlm.BrainNetwork()
agent_memory = nlm.EpisodicMemory()
agent_world = nlm.WorldSimulator()

# Train agent...

# Save checkpoint
checkpoint_manager.save(
    brain=agent_brain,
    memory=agent_memory,
    world=agent_world,
    name="trained_agent",
    metadata={"episode": 1000, "reward": 850.0, "success": True}
)
```

**Load Checkpoint:**
```python
# Load agent state
loaded_brain, loaded_memory, loaded_world = checkpoint_manager.load("trained_agent")

# Verify loaded components
assert loaded_brain is not None
assert loaded_memory is not None
assert loaded_world is not None
```

### 19. How do I troubleshoot common issues?

**Memory Issues:**
- Increase memory limits
- Implement memory-efficient data structures
- Use garbage collection
- Monitor memory usage

**Performance Issues:**
- Profile code to identify bottlenecks
- Optimize algorithms
- Use vectorized operations
- Implement parallel processing

**Configuration Issues:**
- Validate configuration files
- Use configuration templates
- Check for syntax errors
- Verify parameter types

**Compatibility Issues:**
- Check Python and package versions
- Verify system requirements
- Use compatibility checks
- Install missing dependencies

### 20. How do I contribute to NLM?

**Contribution Process:**

1. **Fork the Repository:** Fork NLM on GitHub
2. **Create Feature Branch:** Create a branch for your changes
3. **Make Changes:** Implement your changes
4. **Test Changes:** Run tests to ensure functionality
5. **Update Documentation:** Update relevant documentation
6. **Create Pull Request:** Create a pull request
7. **Code Review:** Address feedback
8. **Merge:** Merge into main branch

**Code Quality Guidelines:**

1. Follow PEP 8 style guide
2. Use type hints
3. Write comprehensive docstrings
4. Write unit tests
5. Implement error handling
6. Use efficient algorithms

## Common Problems and Solutions

### Problem 1: "Brain isn't doing anything"

**Cause:** Brain not initialized or no input
**Solution:**
- Call `brain.initialize()` before using the brain
- Ensure sensory input is connected
- Add neurons with proper connections

### Problem 2: "Agent isn't moving"

**Cause:** Action execution not connected
**Solution:**
- Call `world.applyMotorCommand()`
- Check that `world.update()` is being called
- Ensure agent's motor neurons are properly connected

### Problem 3: "Everything is 0"

**Cause:** Brain needs time to "warm up"
**Solution:**
- Run more simulation steps
- Add initial input to neurons
- Ensure proper network connectivity

### Problem 4: "Memory consolidation failed"

**Cause:** Insufficient important memories
**Solution:**
- Increase consolidation threshold
- Add more diverse experiences
- Implement stronger consolidation mechanisms

### Problem 5: "Learning is too slow"

**Cause:** Inappropriate learning parameters
**Solution:**
- Adjust learning rate
- Increase plasticity strength
- Add reward signals
- Implement adaptive learning

## Best Practices

### 1. Configuration Management

- Always validate configuration before use
- Use environment-specific configurations
- Implement configuration versioning
- Use configuration templates for common setups

### 2. Memory Management

- Monitor memory usage
- Implement memory-efficient data structures
- Use garbage collection
- Implement memory consolidation strategies

### 3. Performance Optimization

- Profile code to identify bottlenecks
- Use vectorized operations
- Implement parallel processing
- Optimize neural network architectures

### 4. Error Handling

- Implement comprehensive error handling
- Use try-except blocks for critical operations
- Log errors for debugging
- Provide meaningful error messages

### 5. Documentation

- Write clear docstrings
- Include usage examples
- Update documentation regularly
- Maintain consistent formatting

## Frequently Referenced Topics

### Neural Network Configuration

```python
# Example brain configuration
brain_config = {
    "regions": 5,                    # Number of brain regions
    "neurons_per_region": 100,       # Neurons per region
    "neuron_type": "lif",           # Neuron model
    "plasticity": "stdp",           # Plasticity rule
    "learning_rate": 0.01,          # Learning rate
    "connection_probability": 0.15  # Synaptic connection probability
}
```

### Environment Configuration

```python
# Example environment configuration
env_config = {
    "type": "simple",               # Environment type
    "width": 50,                    # Width
    "height": 50,                   # Height
    "vision_range": 15,             # Sensory range
    "obstacle_density": 0.1,        # Obstacle density
    "resource_count": 10            # Number of resources
}
```

### Memory Configuration

```python
# Example memory configuration
memory_config = {
    "working_memory": {
        "capacity": 100,             # Working memory capacity
        "decay_rate": 0.01,          # Decay rate
        "update_frequency": 0.05     # Update frequency
    },
    "episodic_memory": {
        "capacity": 1000,            # Episodic memory capacity
        "consolidation_threshold": 0.7,  # Consolidation threshold
        "replay_frequency": 0.1      # Replay frequency
    }
}
```

## Key Resources

### Documentation

- **Documentation Hub:** https://nlm.readthedocs.io
- **API Reference:** https://nlm.readthedocs.io/en/latest/api/
- **Tutorials:** https://nlm.readthedocs.io/en/latest/tutorials/
- **Examples:** https://github.com/nlm-project/nlm/tree/main/examples

### Community

- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Discussions:** https://github.com/nlm-project/nlm/discussions
- **Issues:** https://github.com/nlm-project/nlm/issues
- **Slack/ Discord:** Join the NLM community for real-time support

### Support

- **Bug Reports:** File issues on GitHub
- **Feature Requests:** Submit feature requests on GitHub
- **Questions:** Ask questions on the discussion forum
- **Help:** Get help from the community

## Troubleshooting Guide

### Symptom: Installation Issues

**Problem:** "ModuleNotFoundError: No module named 'nlm'"
**Solution:**
```bash
pip install nlm
# Or from source
pip install -e python/
```

**Problem:** "ImportError: cannot import name 'pynlm'"
**Solution:**
```python
# Use the correct import
import nlm
# Not pynlm
```

### Symptom: Runtime Errors

**Problem:** "Brain not initialized"
**Solution:**
```python
brain = nlm.BrainNetwork(config)
brain.initialize()  # Always call initialize()
```

**Problem:** "Simulation failed"
**Solution:**
```python
# Check for errors in configuration
if not config.validate():
    print("Invalid configuration")
    # Fix configuration

# Check for memory issues
import psutil
process = psutil.Process()
if process.memory_info().rss > 1024 * 1024 * 1024:  # 1GB
    print("Memory usage too high")
    # Free memory or increase limit
```

### Symptom: Performance Issues

**Problem:** "Simulation is too slow"
**Solution:**
- Profile the simulation to identify bottlenecks
- Use vectorized operations
- Optimize neural network architecture
- Increase parallelization
- Use more efficient data structures

**Problem:** "Memory usage is too high"
**Solution:**
- Monitor memory usage
- Implement memory-efficient data structures
- Use garbage collection
- Reduce simulation complexity
- Use memory limits

### Symptom: Visualization Issues

**Problem:** "Plot not showing"
**Solution:**
```python
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend

import matplotlib.pyplot as plt
plt.savefig('plot.png')  # Save instead of showing
```

**Problem:** "High memory usage for visualization"
**Solution:**
- Use efficient plotting libraries
- Reduce figure size
- Use subplots instead of separate figures
- Save plots to file instead of displaying

## Conclusion

This FAQ document provides answers to the most common questions about NLM. By understanding these topics and following the best practices outlined in this guide, you can effectively use NLM for your neural simulation and research needs.

For additional questions or issues, please:
1. Check the documentation on the NLM website
2. File an issue on GitHub
3. Ask questions on the community forum
4. Join the NLM community for real-time support

The NLM project is constantly evolving, and new features and improvements are regularly added. Stay updated with the latest documentation and releases to make the most of NLM's capabilities.