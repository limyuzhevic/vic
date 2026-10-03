# NLM Step-by-Step Tutorial

## Welcome to NLM!

This tutorial will guide you through using the NLM (Neural Learning Machine) from your first steps to more advanced concepts. Whether you're a complete beginner or an experienced user, this guide will help you understand how to build and run neural simulations effectively.

## What is NLM?

NLM is a brain-inspired spiking neural network simulator that models how biological brains work. It implements Leaky Integrate-and-Fire (LIF) neurons with real neural dynamics, learning mechanisms, and memory systems.

Think of NLM as a virtual brain that:
- Receives sensory input (like what you see)
- Processes information through neural networks
- Makes decisions (motor commands)
- Learns from experience
- Stores memories

## Section 1: Your First NLM Program

### 1.1 Setting Up Your Environment

First, let's make sure you have NLM installed and can import it:

```python
import pynlm

print("NLM imported successfully!")
print("Available classes:", [c for c in dir(pynlm) if not c.startswith('_')])
```

### 1.2 The Three Fundamental Components

NLM has three main components that work together:

1. **Brain** - The virtual brain that thinks and processes information
2. **World** - The environment where the brain operates
3. **Agent** - The bridge that connects brain to world

Let's create a simple simulation with all three:

```python
import pynlm

# Step 1: Create the brain (like a virtual brain)
brain = pynlm.createBrain()  # Uses default configuration
brain.initialize()  # Turn the brain on

# Step 2: Create the world (like the environment)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()  # Reset world to initial state

# Step 3: Create the agent (the bridge)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Connect agent to world

print("✓ All components initialized successfully!")
print(f"  - Brain has {brain.getTotalNeuronCount()} neurons")
print(f"  - World is {world.getWidth()}x{world.getHeight()} units")
```

### 1.3 Your First Simulation Loop

Now let's run a complete simulation step by step:

```python
import pynlm

# Create components
brain = pynlm.createBrain()
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
print("Starting simulation...")
for step in range(50):
    # 1. Update world (time passes)
    world.update(0.1)
    
    # 2. Get what the agent sees (sensory input)
    percept = world.getSensoryPercept()
    
    # 3. Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks/decides
    brain.step(step)
    
    # 5. Brain decides on action
    action = agent.decodeMotorCommand()
    
    # 6. Agent executes action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 10 steps
    if step % 10 == 0:
        print(f"Step {step}: Action={action}, " +
              f"Firing neurons={brain.getFiringNeuronCount()}")

print("✓ Simulation completed!")
```

**What just happened?**
- The brain processed sensory input from the world
- Neural activity occurred (neurons fired)
- The brain selected an action (move, turn, etc.)
- The action affected the world state

### 1.4 Adding Learning to Your Agent

Let's enable learning so the agent can improve its performance:

```python
import pynlm

# Create components
brain = pynlm.createBrain()
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(12, 12, 6, 6)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore new things
agent.enableDevelopment(True)       # Brain matures over time

print("Learning features enabled!")

# Run learning simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get some reward (simplified)
    reward = 1.0 if step % 20 == 0 else 0.1
    agent.applyRewardModulation(reward, 0.0)

print(f"Learning complete! Final curiosity: {agent.getCuriosityLevel():.2f}")
```

## Section 2: Understanding NLM Components

### 2.1 Brain Architecture

The Brain class contains many subsystems working together:

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

# Check different brain statistics
print("Brain Architecture:")
print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
print(f"  - Active neurons: {brain.getFiringNeuronCount()}")
print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"  - Development stage: {brain.getDevelopmentalStage()}")
```

### 2.2 World Configuration

The world represents the agent's environment:

```python
import pynlm

world = pynlm.createSimpleWorld()

# Configure world dimensions
print("World Configuration:")
print(f"  - Width: {world.getWidth()}")
print(f"  - Height: {world.getHeight()}")
print(f"  - Vision: {world.getAgentBody().getVisionWidth()}x{world.getAgentBody().getVisionHeight()}")

# Set agent starting position
world.setAgentStart(5.0, 5.0)
world.reset()

print("  - Agent starts at (5.0, 5.0)")
```

### 2.3 Agent Capabilities

The AgentBrain handles the interface between brain and world:

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Agent capabilities
print("Agent Capabilities:")
print(f"  - Sensory input size: {agent.getSensoryInputSize()}")
print(f"  - Motor output size: {agent.getMotorOutputSize()}")
print(f"  - Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
print(f"  - Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
print(f"  - Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
```

## Section 3: Advanced NLM Techniques

### 3.1 Using the SimpleEnvironment

The SimpleEnvironment class provides a high-level interface for common use cases:

```python
import pynlm

# Create environment (high-level interface)
env = pynlm.SimpleEnvironment()
env.initialize(width=16, height=16, visionWidth=8, visionHeight=8)

# Configure brain for your needs
env.configureBrain(neuronCount=2000, connectionProbability=0.1)

# Enable all learning subsystems
env.enableAllSubsystems()

# Run complete episode
print("Running simulation episode...")
env.runEpisode(numSteps=500, render=True)

# Get detailed statistics
stats = env.getEpisodeStats()
print(f"Episode Results:")
print(f"  - Steps completed: {stats.stepsCompleted}")
print(f"  - Total reward: {stats.totalReward:.2f}")
print(f"  - Average firing rate: {stats.avgFiringRate:.2f}")
print(f"  - Final curiosity: {stats.curiosityLevel:.2f}")
```

### 3.2 Step-by-Step Environment Control

```python
import pynlm

# Create environment
env = pynlm.SimpleEnvironment()
env.initialize(10, 10, 5, 5)

# Configure for specific task
env.configureBrain(neuronCount=1500, regionCount=2)

# Enable learning
env.enableAllSubsystems()

# Run step-by-step with monitoring
print("Running step-by-step simulation...")
for step in range(200):
    if not env.step(timestep=0.05):
        print("Simulation stopped!")
        break
    
    # Get current statistics
    stats = env.getEpisodeStats()
    
    # Print progress every 25 steps
    if step % 25 == 0:
        print(f"Step {step}: Reward={stats.totalReward:6.2f}, " +
              f"Firing={stats.avgFiringRate:5.2f}, " +
              f"Curiosity={stats.curiosityLevel:5.2f}")

print("✓ Step-by-step simulation completed!")
```

### 3.3 Experiment Framework

Create reusable experiment templates:

```python
import pynlm

def run_experiment(experiment_name, **config):
    """Run a standardized experiment with given configuration."""
    print(f"Running experiment: {experiment_name}")
    
    # Create environment
    env = pynlm.SimpleEnvironment()
    env.initialize(width=12, height=12, visionWidth=6, visionHeight=6)
    
    # Apply configuration
    default_config = {
        'neuronCount': 1000,
        'connectionProbability': 0.08,
        'enableDevelopment': True,
        'enableCuriosity': True
    }
    default_config.update(config)
    
    env.configureBrain(
        neuronCount=default_config['neuronCount'],
        connectionProbability=default_config['connectionProbability']
    )
    
    # Enable subsystems based on config
    if default_config.get('enableDevelopment', True):
        # Note: enableAllSubsystems() enables all
        pass
    
    env.enableAllSubsystems()
    
    # Run simulation
    env.runEpisode(numSteps=300, render=False)
    
    # Return results
    return {
        'experiment': experiment_name,
        'stats': env.getEpisodeStats(),
        'config': default_config
    }

# Run different experiments
results1 = run_experiment("Baseline", neuronCount=500)
results2 = run_experiment("High Plasticity", neuronCount=2000, connectionProbability=0.15)

print("Experiments completed!")
```

## Section 4: Common Patterns and Best Practices

### 4.1 Reusable Brain Factory

Create a factory function for common brain configurations:

```python
def create_research_brain(experiment_type="standard"):
    """Create brain configured for research experiments."""
    config = pynlm.createDefaultConfig()
    
    if experiment_type == "standard":
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
    elif experiment_type == "high_plasticity":
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
    elif experiment_type == "conservative":
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Use the factory
brain1 = create_research_brain("standard")
brain2 = create_research_brain("high_plasticity")
```

### 4.2 Agent Training Loop

Create a reusable training loop:

```python
def train_agent(brain, world, agent, num_episodes, steps_per_episode):
    """Train agent using reinforcement learning."""
    all_stats = []
    
    for episode in range(num_episodes):
        # Reset for new episode
        world.reset()
        agent.reset()
        brain.reset()
        
        # Run episode
        episode_stats = {
            'episode': episode,
            'steps': 0,
            'total_reward': 0.0,
            'final_curiosity': 0.0
        }
        
        for step in range(steps_per_episode):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simple reward function
            reward = calculate_reward(world, agent)
            agent.applyRewardModulation(reward, 0.0)
            
            episode_stats['steps'] += 1
            episode_stats['total_reward'] += reward
        
        episode_stats['final_curiosity'] = agent.getCuriosityLevel()
        all_stats.append(episode_stats)
        
        if episode % 10 == 0:
            print(f"Episode {episode}: " +
                  f"Reward={episode_stats['total_reward']:.2f}, " +
                  f"Curiosity={episode_stats['final_curiosity']:.2f}")
    
    return all_stats

def calculate_reward(world, agent):
    """Calculate reward based on agent performance."""
    # Example: reward for approaching center
    body = world.getAgentBody()
    distance_to_center = ((body.x - 5.0)**2 + (body.y - 5.0)**2)**0.5
    reward = max(0, 1.0 - distance_to_center / 10.0)
    
    # Add curiosity bonus for exploration
    curiosity_bonus = agent.getCuriosityLevel() * 0.5
    return reward + curiosity_bonus
```

### 4.3 Visualization Helper Functions

Create helper functions for visualizing results:

```python
import matplotlib.pyplot as plt
import numpy as np

def plot_simulation_results(stats_list, title="Simulation Results"):
    """Plot simulation statistics."""
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle(title, fontsize=16)
    
    # Episode rewards
    episodes = [s['episode'] for s in stats_list]
    rewards = [s['total_reward'] for s in stats_list]
    axes[0, 0].plot(episodes, rewards)
    axes[0, 0].set_title('Total Reward per Episode')
    axes[0, 0].set_xlabel('Episode')
    axes[0, 0].set_ylabel('Total Reward')
    axes[0, 0].grid(True)
    
    # Final curiosity levels
    curiosities = [s['final_curiosity'] for s in stats_list]
    axes[0, 1].plot(episodes, curiosities)
    axes[0, 1].set_title('Final Curiosity Level')
    axes[0, 1].set_xlabel('Episode')
    axes[0, 1].set_ylabel('Curiosity')
    axes[0, 1].grid(True)
    
    # Rolling average of rewards
    window = 5
    rolling_avg = np.convolve(rewards, np.ones(window)/window, mode='valid')
    axes[1, 0].plot(episodes[window-1:], rolling_avg)
    axes[1, 0].set_title(f'Rolling Average Reward (window={window})')
    axes[1, 0].set_xlabel('Episode')
    axes[1, 0].set_ylabel('Average Reward')
    axes[1, 0].grid(True)
    
    # Scatter plot: reward vs curiosity
    axes[1, 1].scatter(rewards, curiosities, alpha=0.7)
    axes[1, 1].set_title('Reward vs Final Curiosity')
    axes[1, 1].set_xlabel('Total Reward')
    axes[1, 1].set_ylabel('Final Curiosity')
    axes[1, 1].grid(True)
    
    plt.tight_layout()
    plt.show()
```

## Section 5: Common Issues and Solutions

### 5.1 "Brain Isn't Doing Anything"

**Problem:** Brain appears inactive after initialization.

**Diagnosis:**
- Did you call `brain.initialize()`?
- Are there enough neurons for activity?
- Is there sufficient sensory input?

**Solution:**
```python
# Ensure proper initialization
brain = pynlm.createBrain()
brain.initialize()

# Add initial sensory activity
from pynlm import Vision
vision = Vision(8, 8, 3)
vision.setData([0.3] * 192)  # Add some activity
brain.receiveSensoryInput(vision)

# Run warm-up steps
for i in range(50):
    brain.step(i)

# Check if brain is active
if brain.getFiringNeuronCount() == 0:
    print("Warning: No neurons firing. Try more input or steps.")
```

### 5.2 "Agent Isn't Moving"

**Problem:** Agent motor commands don't produce movement.

**Diagnosis:**
- Did you call `world.update()`?
- Is `agent.decodeMotorCommand()` returning valid actions?
- Are world objects in the way?

**Solution:**
```python
# Ensure complete simulation loop
for step in range(100):
    # Update world (required for time advancement)
    world.update(0.1)
    
    # Process sensory input
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Run brain computation
    brain.step(step)
    
    # Get motor command
    action = agent.decodeMotorCommand()
    print(f"Step {step}: Action={action}")
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print agent position
    body = world.getAgentBody()
    print(f"  Agent at ({body.x:.1f}, {body.y:.1f})")
```

### 5.3 "Everything is Zero"

**Problem:** All activity values are zero.

**Diagnosis:**
- Brains need time to "warm up"
- Neurons need input to fire
- Initial conditions might be too quiet

**Solution:**
```python
# Start with a quiet simulation
brain = pynlm.createBrain()
brain.initialize()

# Warm-up period with minimal activity
for i in range(30):
    brain.step(i)

# Add some initial stimulation
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 0.5)

# Continue with gradual activity increase
for i in range(50):
    brain.step(i)
    if i % 10 == 9:
        # Add more stimulation every 10 steps
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 0.2)

print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
```

### 5.4 "Performance is Slow"

**Problem:** Simulation running slowly.

**Diagnosis:**
- Too many neurons/synapses
- Inefficient configuration
- Missing performance optimizations

**Solution:**
```python
# Use performance-optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Start smaller
config.set("brain.synapse_density", 0.05)
brain = pynlm.createBrain(config)
brain.initialize()

# Use SimpleEnvironment for optimized setup
env = pynlm.SimpleEnvironment()
env.initialize(8, 8)  # Smaller world
env.runEpisode(100)  # Fewer steps for testing
```

## Section 6: Advanced Usage Examples

### 6.1 Learning in a Grid World

Create a task where the agent learns to navigate a grid:

```python
def create_grid_world_task(width=12, height=12, num_goals=3):
    """Create a grid world with goals for the agent to learn."""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 6, 6)
    
    # Add goal objects
    for i in range(num_goals):
        goal_x = (i * 3) + 1.5
        goal_y = (i * 3) + 1.5
        from pynlm import WorldObject, WorldObjectType
        goal = WorldObject(goal_x, goal_y, WorldObjectType.Resource, value=10.0)
        world.addObject(goal)
    
    # Add some obstacles (walls)
    for x in range(0, width, 2):
        for y in range(0, height, 2):
            if (x > 2 and x < width-2 and y > 2 and y < height-2):
                from pynlm import WorldObject, WorldObjectType
                wall = WorldObject(float(x), float(y), WorldObjectType.Wall, radius=0.4)
                world.addObject(wall)
    
    return world

def run_grid_world_learning():
    """Run agent learning in a grid world."""
    # Setup environment
    env = pynlm.SimpleEnvironment()
    env.initialize(12, 12, 6, 6)
    env.configureBrain(neuronCount=1500, connectionProbability=0.1)
    env.enableAllSubsystems()
    
    # Replace world with custom grid world
    custom_world = create_grid_world_task(12, 12, 3)
    # Note: This would require extending SimpleEnvironment
    # For now, use the built-in world
    
    # Run learning episode
    env.runEpisode(numSteps=400, render=False)
    
    stats = env.getEpisodeStats()
    print(f"Grid world learning: Reward={stats.totalReward:.2f}, " +
          f"Curiosity={stats.curiosityLevel:.2f}")
    
    return stats
```

### 6.2 Comparative Experiment Framework

Run multiple conditions for scientific experiments:

```python
def run_comparative_experiment():
    """Run comparative experiment across different configurations."""
    conditions = [
        {"name": "Baseline", "neurons": 500, "plasticity": "medium"},
        {"name": "High Plasticity", "neurons": 2000, "plasticity": "high"},
        {"name": "Conservative", "neurons": 200, "plasticity": "low"},
        {"name": "No Curiosity", "neurons": 1000, "plasticity": "medium", "curiosity": False}
    ]
    
    results = {}
    
    for condition in conditions:
        print(f"\nRunning condition: {condition['name']}")
        
        # Create environment
        env = pynlm.SimpleEnvironment()
        env.initialize(10, 10, 5, 5)
        
        # Configure based on condition
        env.configureBrain(
            neuronCount=condition["neurons"],
            connectionProbability=0.08 if condition["plasticity"] != "low" else 0.03
        )
        
        # Enable subsystems
        env.enableAllSubsystems()
        if "curiosity" in condition and not condition["curiosity"]:
            # Note: Would need method to disable curiosity specifically
            pass
        
        # Run simulation
        env.runEpisode(numSteps=300, render=False)
        
        # Store results
        stats = env.getEpisodeStats()
        results[condition["name"]] = {
            'statistics': stats,
            'config': condition,
            'brain': env.getBrain(),
            'world': env.getWorld(),
            'agent': env.getAgent()
        }
        
        print(f"  Completed: {stats.stepsCompleted} steps, " +
              f"Reward={stats.totalReward:.2f}")
    
    return results

# Run comparative experiment
comparative_results = run_comparative_experiment()

# Analyze results
print("\nComparative Results Summary:")
for name, result in comparative_results.items():
    stats = result['statistics']
    print(f"{name:20s}: Reward={stats.totalReward:8.2f}, " +
          f"Firing={stats.avgFiringRate:6.2f}, " +
          f"Curiosity={stats.curiosityLevel:5.2f}")
```

### 6.3 Statistical Analysis Helper

Analyze simulation results statistically:

```python
import numpy as np
from scipy import stats
import matplotlib.pyplot as plt

def analyze_simulation_results(results_list, alpha=0.05):
    """Perform statistical analysis on simulation results."""
    # Extract data
    rewards = [r['stats'].total_reward for r in results_list]
    firing_rates = [r['stats'].avg_firing_rate for r in results_list]
    curiosity_levels = [r['stats'].curiosity_level for r in results_list]
    
    # Basic statistics
    print("Statistical Analysis:")
    print(f"Rewards: Mean={np.mean(rewards):.2f}, " +
          f"Std={np.std(rewards):.2f}, " +
          f"Min={np.min(rewards):.2f}, " +
          f"Max={np.max(rewards):.2f}")
    
    # Normality test (Shapiro-Wilk)
    _, p_value_norm = stats.shapiro(rewards)
    print(f"Normality test (Shapiro-Wilk): p-value={p_value_norm:.4f}")
    
    # Independence test (runs test)
    _, p_value_runs = stats.runs_test(rewards)
    print(f"Independence test (runs): p-value={p_value_runs:.4f}")
    
    # Confidence interval for mean reward
    ci = stats.t.interval(1 - alpha, len(rewards)-1, 
                         loc=np.mean(rewards), scale=stats.sem(rewards))
    print(f"95% Confidence interval for mean reward: [{ci[0]:.2f}, {ci[1]:.2f}]")
    
    # Correlation analysis
    corr_matrix = np.corrcoef([rewards, firing_rates, curiosity_levels])
    print(f"\nCorrelation Matrix:")
    print(f"{'':10s}{'Rewards':10s}{'Firing':10s}{'Curiosity':10s}")
    print(f"{'Rewards':10s}{corr_matrix[0,0]:10.3f}{corr_matrix[0,1]:10.3f}{corr_matrix[0,2]:10.3f}")
    print(f"{'Firing':10s}{corr_matrix[1,0]:10.3f}{corr_matrix[1,1]:10.3f}{corr_matrix[1,2]:10.3f}")
    print(f"{'Curiosity':10s}{corr_matrix[2,0]:10.3f}{corr_matrix[2,1]:10.3f}{corr_matrix[2,2]:10.3f}")
    
    return {
        'rewards': rewards,
        'firing_rates': firing_rates,
        'curiosity_levels': curiosity_levels,
        'statistics': {
            'reward_mean': np.mean(rewards),
            'reward_std': np.std(rewards),
            'normality_p': p_value_norm,
            'independence_p': p_value_runs,
            'confidence_interval': ci
        }
    }
```

## Section 7: Project Structure and Files

### 7.1 Recommended Workspace Organization

```
YourProject/
├── src/
│   ├── agent/
│   │   ├── AgentBrain.cpp
│   │   ├── AgentBrain.hpp
│   │   ├── SimpleEnvironment.cpp
│   │   └── SimpleEnvironment.hpp
│   ├── brain/
│   ├── core/
│   ├── memory/
│   ├── tests/
│   └── ...
├── python/
│   └── bindings.cpp
├── docs/
│   ├── API_REFERENCE.md
│   ├── STEP_BY_STEP_TUTORIAL.md
│   ├── README.md
│   └── HOW_TO_USE.md
├── examples/
│   ├── basic_simulation.py
│   ├── learning_examples.py
│   └── experiments/
├── notebooks/
│   ├── tutorial.ipynb
│   └── examples/
└── config/
    ├── default_config.json
    └── experiment_configs/
```

### 7.2 Essential Project Files

- **README.md**: Project overview and installation
- **HOW_TO_USE.md**: Practical usage guide
- **STEP_BY_STEP_TUTORIAL.md**: This tutorial
- **docs/API_REFERENCE.md**: Complete API documentation
- **python/bindings.cpp**: Python bindings implementation
- **src/agent/SimpleEnvironment.***: High-level environment interface

## Section 8: Getting Help and Community Resources

### 8.1 Online Resources

- **Official Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Issue Tracker**: Report bugs and feature requests
- **Discussion Forum**: Community support and discussions

### 8.2 Local Documentation

Generate local documentation with:

```bash
# Generate HTML documentation
doxygen docs/Doxyfile

# Build Python API documentation
mkdocs build --site-dir docs/html
```

### 8.3 Quick Reference Cards

Create quick reference cheat sheets:

**Brain Cheat Sheet:**
```python
# Create brain
brain = pynlm.createBrain()
brain.initialize()

# Check status
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
```

**Environment Cheat Sheet:**
```python
# Quick simulation
env = pynlm.SimpleEnvironment()
env.initialize(8, 8)
env.runEpisode(200)
stats = env.getEpisodeStats()
print(f"Reward: {stats.totalReward:.2f}")
```

## Section 9: Future Extensions and Roadmap

### 9.1 Planned Features

1. **Enhanced Visualization**: Built-in plotting tools for brain activity
2. **Real-time Monitoring**: Live visualization during simulation
3. **Advanced Learning**: More sophisticated reinforcement learning algorithms
4. **Multi-agent Systems**: Support for multiple interacting agents
5. **Export/Import**: Save/load simulations in standard formats
6. **Web Interface**: Browser-based simulation control

### 9.2 Research Applications

NLM is designed for research in:

- **Neural Development**: How brains develop and learn
- **Cognitive Modeling**: Mental processes and reasoning
- **Reinforcement Learning**: Decision-making and optimization
- **Memory Systems**: How memories are formed and retrieved
- **Neuromodulation**: The role of dopamine and other signals

## Section 10: Troubleshooting Common Issues

### 10.1 Installation Problems

**Problem:** Cannot import pynlm

**Solution:**
```bash
# Check installation
pip list | grep pynlm

# Reinstall if needed
pip install --force-reinstall .

# Check for compilation errors
cd .
python setup.py build_ext --inplace
```

### 10.2 Performance Issues

**Problem:** Simulation too slow

**Solutions:**
1. Reduce number of neurons
2. Use SimpleEnvironment
3. Disable unnecessary subsystems
4. Reduce simulation steps

```python
# Performance-optimized example
env = pynlm.SimpleEnvironment()
env.initialize(8, 8)  # Smaller world
env.configureBrain(neuronCount=500)  # Fewer neurons
env.runEpisode(100)  # Fewer steps
```

### 10.3 Memory Issues

**Problem:** Out of memory error

**Solutions:**
1. Close unused environments
2. Clear large data structures
3. Use smaller configurations
4. Monitor memory usage

```python
# Monitor memory usage
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")
```

## Section 11: Contributing and Extending NLM

### 11.1 Adding New Features

**To add new Python features:**
1. Extend `python/bindings.cpp`
2. Update API reference documentation
3. Add examples in `examples/`
4. Update tutorials if needed

**To add new C++ features:**
1. Add to appropriate source files in `src/`
2. Update headers in `include/`
3. Add tests in `tests/`
4. Update build system if needed

### 11.2 Creating Custom Environments

Extend SimpleEnvironment for specific tasks:

```python
class CustomEnvironment(SimpleEnvironment):
    def __init__(self):
        super().__init__()
        # Custom initialization
    
    def setup_task_specific(self, task_type):
        """Setup environment for specific task."""
        if task_type == "navigation":
            # Setup navigation task
            pass
        elif task_type == "memory":
            # Setup memory task
            pass
        # Add more task types
```

### 11.3 Writing Custom Tutorials

Add tutorial examples in `examples/`:

```python
# examples/custom_tutorial.py
"""Custom tutorial demonstrating advanced NLM usage."""

import pynlm

def advanced_tutorial():
    """Demonstrate advanced NLM features."""
    print("Advanced NLM Tutorial")
    print("=" * 50)
    
    # Example: Custom learning loop
    env = pynlm.SimpleEnvironment()
    env.initialize(12, 12, 6, 6)
    env.configureBrain(neuronCount=2000)
    env.enableAllSubsystems()
    
    # Custom training loop
    for episode in range(10):
        env.runEpisode(200, render=False)
        stats = env.getEpisodeStats()
        
        if episode % 3 == 0:
            print(f"Episode {episode}: Reward={stats.totalReward:.2f}")
    
    print("Tutorial completed!")

if __name__ == "__main__":
    advanced_tutorial()
```

## Section 12: License and Attribution

NLM is released under the MIT License. This tutorial is part of the NLM project documentation.

For research use, please cite:
```
@misc{nlm2026tutorial,
  title={NLM Python Tutorial},
  author={NLM Research Team},
  year={2026},
  url={https://github.com/nlm-project/nlm}
}
```

## Conclusion

This tutorial provides a comprehensive guide to using NLM from beginner to advanced levels. You now have:

✅ **Basic skills**: Creating and running simple simulations
✅ **Intermediate techniques**: Using environments and learning features  
✅ **Advanced methods**: Custom experiments and analysis
✅ **Troubleshooting**: Solving common issues
✅ **Best practices**: Writing efficient and maintainable code

**Next Steps:**
1. Complete the examples in this tutorial
2. Try creating your own experiment
3. Explore the API reference for advanced features
4. Contribute to the NLM community

Happy simulating! 🎯

---

*End of Tutorial - Version 1.0*  
*Created: 2026-10-03*  
*For more resources, visit: https://github.com/nlm-project/nlm*