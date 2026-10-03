# NLM - How to Use Guide

## Detailed Implementation Guide

This guide provides detailed instructions for implementing NLM systems and solving common problems.

## Installation and Setup

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Install (optional)
sudo make install
```

### Python Package Installation

```bash
# Install Python bindings (they are built as part of the main build)
# The pynlm module will be available in your Python environment

# Test installation
python3 -c "import pynlm; print('NLM version:', pynlm.getVersion())"
```

## Basic Implementation

### Creating a Simple Brain

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation for multiple steps
for step in range(100):
    brain.step(step)

print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent World Simulation

```python
import pynlm

def createCompleteAgentSetup(width=20, height=20):
    """Create a complete agent setup with world, brain, and agent"""
    
    # Create configuration
    config = pynlm.createAdvancedConfig(
        numNeurons=200,
        learningRate=0.01,
        enableDevelopment=True,
        enableRewardModulation=True,
        enableCuriosity=True
    )
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 8, 8)
    world.setAgentStart(width/2, height/2)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableAllFeatures()
    
    return brain, world, agent

def runCompleteSimulation(brain, world, agent, steps=100):
    """Run a complete simulation with world interaction"""
    
    # Statistics collection
    stats = {
        "steps_completed": 0,
        "total_spikes": 0,
        "average_firing_rate": 0,
        "rewards_earned": 0,
        "actions_taken": 0
    }
    
    for step in range(steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation
        reward = result.reward if result.success else 0.0
        if reward > 0:
            agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Collect statistics
        stats["steps_completed"] += 1
        stats["total_spikes"] = brain.getTotalSpikeCount()
        stats["average_firing_rate"] = brain.getAverageFiringRate()
        stats["rewards_earned"] += reward
        stats["actions_taken"] += 1
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: FR={stats['average_firing_rate']:.2f}, "
                  f"Spikes={stats['total_spikes']}, Reward={reward}")
    
    return stats

# Usage example
brain, world, agent = createCompleteAgentSetup(15, 15)
stats = runCompleteSimulation(brain, world, agent, 100)
print(f"\nSimulation Results: {stats}")
```

## Configuration Deep Dive

### Configuration Parameters

```python
import pynlm

# Create configuration with detailed parameters
config = pynlm.createAdvancedConfig(
    numNeurons=300,
    learningRate=0.005,
    enableDevelopment=True,
    enableRewardModulation=True,
    enableCuriosity=True,
    enableStructuralPlasticity=True
)

# Set custom parameters
config.set("developmental_rate", 0.03)
config.set("plasticity_threshold", 0.25)
config.set("reward_decay", 0.98)
config.set("novelty_threshold", 0.6)
config.set("prediction_horizon", 15)
config.set("max_energy", 100.0)
config.set("energy_decay_rate", 0.01)

# Validate configuration
errors = pynlm.checkConfigValidity(config)
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("Configuration is valid")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")
```

### Configuration File Format

NLM configuration uses JSON format. Here's an example configuration file:

```json
{
    "num_neurons": 300,
    "learning_rate": 0.005,
    "enable_development": true,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_structural_plasticity": true,
    "developmental_rate": 0.03,
    "plasticity_threshold": 0.25,
    "reward_decay": 0.98,
    "novelty_threshold": 0.6,
    "prediction_horizon": 15,
    "max_energy": 100.0,
    "energy_decay_rate": 0.01,
    "simulation_time_step": 0.1,
    "random_seed": 42
}
```

## World Configuration

### Simple World Setup

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)

# Set agent starting position
world.setAgentStart(5, 5)

# Reset world to initial state
world.reset()

# Add resources
for x in range(5, 25, 5):
    for y in range(5, 25, 5):
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 10.0, 0.5))

# Add walls around perimeter
world.addObject(pynlm.WorldObject(0, 0, pynlm.WorldObjectType.Wall, 0.0f, 1.0f))
world.addObject(pynlm.WorldObject(30, 0, pynlm.WorldObjectType.Wall, 0.0f, 1.0f))
world.addObject(pynlm.WorldObject(0, 30, pynlm.WorldObjectType.Wall, 0.0f, 1.0f))
world.addObject(pynlm.WorldObject(30, 30, pynlm.WorldObjectType.Wall, 0.0f, 1.0f))

# Set world properties
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(12345)
```

### World Object Types

```python
import pynlm

# Define world object types
WorldObjectType = pynlm.WorldObjectType

# Resource - positive value, agent can collect
resource = pynlm.WorldObject(10, 10, WorldObjectType.Resource, 20.0, 0.6)

# Hazard - negative value, agent loses energy
hazard = pynlm.WorldObject(20, 20, WorldObjectType.Hazard, -15.0, 0.8)

# Wall - impassable object
wall = pynlm.WorldObject(5, 5, WorldObjectType.Wall, 0.0f, 1.0f)

# Marker - neutral object, can be used for navigation
marker = pynlm.WorldObject(15, 15, WorldObjectType.Marker, 0.0f, 0.3f)

# Empty - empty space
empty = pynlm.WorldObject(25, 25, WorldObjectType.Empty, 0.0f, 0.5f)
```

## Agent Configuration

### Agent Brain Setup

```python
import pynlm

# Create brain and agent
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Configure agent features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Get agent information
print(f"Sensory input size: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
```

### Neuromodulation Control

```python
import pynlm

# Create agent with neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Apply reward modulation
agent.applyRewardModulation(reward=10.0, predicted_reward=5.0)

# Get current neuromodulation levels
print(f"Dopamine level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")

# Reset agent for new episode
agent.reset()
```

## Sensorimotor Loop

### Complete Sensorimotor Implementation

```python
import pynlm

def implementSensorimotorLoop(brain, world, agent, steps=100):
    """
    Implement the complete sensorimotor loop:
    1. Sense environment
    2. Process sensory input
    3. Generate motor command
    4. Act in environment
    5. Receive feedback
    """
    
    loop_count = 0
    
    for step in range(steps):
        # 1. SENSE: Update world and get sensory input
        world.update(0.1)  # Update world physics
        percept = world.getSensoryPercept()  # What agent sees/hears/etc.
        
        # 2. PROCESS: Brain processes sensory input
        agent.processSensoryInput(percept)  # Brain receives sensory input
        
        # 3. ACT: Brain generates motor command
        brain.step(step)  # Brain processes and learns
        action = agent.decodeMotorCommand()  # Brain decides what to do
        
        # 4. INTERACT: Execute action in world
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # 5. FEEDBACK: Apply reward modulation
        if action_result.success:
            reward = action_result.reward
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
        
        loop_count += 1
        
        # Optional: Print progress
        if step % 20 == 0:
            print(f"Loop {loop_count}: Action={pynlm.actionTypeToString(action.getType())}, "
                  f"Reward={reward}, Success={action_result.success}")
    
    return loop_count

# Example usage
brain, world, agent = createCompleteAgentSetup(20, 20)
implemented_loops = implementSensorimotorLoop(brain, world, agent, 50)
print(f"Implemented {implemented_loops} sensorimotor loops")
```

## Data Collection and Analysis

### Collecting Simulation Metrics

```python
import pynlm
import json
import time

class SimulationRecorder:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.recording = {
            "timestamps": [],
            "brain_metrics": {
                "neuron_count": [],
                "synapse_count": [],
                "firing_rate": [],
                "total_spikes": [],
                "developmental_stage": []
            },
            "agent_metrics": {
                "neuromodulation": [],
                "curiosity": [],
                "novelty": [],
                "prediction_error": []
            },
            "world_metrics": {
                "time": [],
                "agent_energy": [],
                "agent_position": [],
                "agent_age": []
            },
            "action_metrics": {
                "action_types": [],
                "rewards": []
            }
        }
    
    def recordStep(self, step, action=None, reward=0.0):
        """Record state and metrics for a single simulation step"""
        current_time = self.world.getSimulationTime()
        
        # Record brain metrics
        self.recording["brain_metrics"]["neuron_count"].append(self.brain.getTotalNeuronCount())
        self.recording["brain_metrics"]["synapse_count"].append(self.brain.getTotalSynapseCount())
        self.recording["brain_metrics"]["firing_rate"].append(self.brain.getAverageFiringRate())
        self.recording["brain_metrics"]["total_spikes"].append(self.brain.getTotalSpikeCount())
        self.recording["brain_metrics"]["developmental_stage"].append(int(self.brain.getDevelopmentalStage()))
        
        # Record agent metrics
        self.recording["agent_metrics"]["neuromodulation"].append(self.agent.getNeuromodulationLevel())
        self.recording["agent_metrics"]["curiosity"].append(self.agent.getCuriosityLevel())
        self.recording["agent_metrics"]["novelty"].append(self.agent.getNoveltyLevel())
        self.recording["agent_metrics"]["prediction_error"].append(self.agent.getPredictionError())
        
        # Record world metrics
        self.recording["world_metrics"]["time"].append(current_time)
        agent_body = self.world.getAgentBody()
        self.recording["world_metrics"]["agent_energy"].append(agent_body.energy)
        self.recording["world_metrics"]["agent_position"].append((agent_body.x, agent_body.y))
        self.recording["world_metrics"]["agent_age"].append(agent_body.age)
        
        # Record action metrics
        if action:
            self.recording["action_metrics"]["action_types"].append(action.getType())
            self.recording["action_metrics"]["rewards"].append(reward)
        
        self.recording["timestamps"].append(time.time())
    
    def saveRecording(self, filename="simulation_recording.json"):
        """Save recorded simulation data to file"""
        with open(filename, 'w') as f:
            json.dump(self.recording, f, indent=2)
    
    def analyzeRecording(self):
        """Analyze recorded simulation data and return statistics"""
        analysis = {}
        
        # Brain analysis
        analysis["brain_analysis"] = {
            "avg_neuron_count": sum(self.recording["brain_metrics"]["neuron_count"]) / len(self.recording["brain_metrics"]["neuron_count"]),
            "avg_firing_rate": sum(self.recording["brain_metrics"]["firing_rate"]) / len(self.recording["brain_metrics"]["firing_rate"]),
            "final_spike_count": self.recording["brain_metrics"]["total_spikes"][-1],
            "growth_rate": (self.recording["brain_metrics"]["total_spikes"][-1] - self.recording["brain_metrics"]["total_spikes"][0]) / max(1, len(self.recording["brain_metrics"]["total_spikes"]) - 1)
        }
        
        # Agent analysis
        analysis["agent_analysis"] = {
            "avg_curiosity": sum(self.recording["agent_metrics"]["curiosity"]) / len(self.recording["agent_metrics"]["curiosity"]),
            "final_neuromodulation": self.recording["agent_metrics"]["neuromodulation"][-1],
            "learning_trajectory": self._calculateLearningTrajectory()
        }
        
        return analysis
    
    def _calculateLearningTrajectory(self):
        """Calculate learning trajectory from recorded data"""
        neuromodulation = self.recording["agent_metrics"]["neuromodulation"]
        curiosity = self.recording["agent_metrics"]["curiosity"]
        
        trajectory = []
        for i in range(len(neuromodulation)):
            trajectory.append({
                "step": i,
                "neuromodulation": neuromodulation[i],
                "curiosity": curiosity[i],
                "learning_score": neuromodulation[i] * curiosity[i]
            })
        
        return trajectory

# Usage example
brain, world, agent = createCompleteAgentSetup(15, 15)
recorder = SimulationRecorder(brain, world, agent)

# Run simulation with recording
for step in range(100):
    # Execute sensorimotor loop
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record step
    recorder.recordStep(step, action, result.reward)

# Save and analyze
recorder.saveRecording("my_simulation.json")
analysis = recorder.analyzeRecording()
print(f"Simulation Analysis: {analysis}")
```

## Troubleshooting Common Issues

### Issue: Brain Not Firing

```python
import pynlm

def troubleshootBrainFiring(brain, world, agent):
    """Troubleshoot brain firing issues"""
    issues = []
    solutions = []
    
    # Check neuron count
    neuron_count = brain.getTotalNeuronCount()
    if neuron_count < 10:
        issues.append("Too few neurons")
        solutions.append("Increase num_neurons in configuration")
    
    # Check developmental stage
    stage = brain.getDevelopmentalStage()
    if stage == pynlm.DevelopmentalStage.Initial:
        issues.append("Brain still in initial developmental stage")
        solutions.append("Enable development or wait for maturation")
    
    # Check sensory input
    if agent:
        percept = world.getSensoryPercept()
        if agent.getSensoryInputSize() == 0:
            issues.append("Agent has no sensory input")
            solutions.append("Check world configuration and sensory neurons")
    
    # Check brain step execution
    try:
        brain.step(0)
        print("Brain step executed successfully")
    except Exception as e:
        issues.append(f"Brain step failed: {e}")
        solutions.append("Check brain configuration and dependencies")
    
    return {"issues": issues, "solutions": solutions}

# Usage
troubleshooting = troubleshootBrainFiring(brain, world, agent)
print("Troubleshooting results:")
for issue in troubleshooting["issues"]:
    print(f"  Issue: {issue}")
    print(f"  Solution: {issue}")
```

### Issue: Agent Not Moving

```python
import pynlm

def troubleshootAgentMovement(brain, world, agent):
    """Troubleshoot agent movement issues"""
    issues = []
    solutions = []
    
    # Check motor neuron groups
    action = agent.decodeMotorCommand()
    if action.getType() == pynlm.ActionType.Wait:
        issues.append("Agent choosing to wait")
        solutions.append("Check agent configuration and enable curiosity/exploration")
    
    # Check world state
    agent_body = world.getAgentBody()
    if agent_body.energy <= 0:
        issues.append("Agent has no energy")
        solutions.append("Check world energy settings or add more resources")
    
    # Check world collisions
    if not world.isValidPosition(agent_body.x, agent_body.y):
        issues.append("Agent in invalid position (collision)")
        solutions.append("Check world obstacles or agent movement logic")
    
    # Check world updates
    current_time = world.getSimulationTime()
    if current_time <= 0:
        issues.append("World not being updated")
        solutions.append("Ensure world.update() is called in simulation loop")
    
    return {"issues": issues, "solutions": solutions}

# Usage
troubleshooting = troubleshootAgentMovement(brain, world, agent)
print("Movement troubleshooting results:")
for issue in troubleshooting["issues"]:
    print(f"  Issue: {issue}")
    print(f"  Solution: {issue}")
```

## Performance Tips

### Optimizing Simulation Performance

```python
import pynlm
import time

def optimizePerformance(brain, world, agent, target_fps=30):
    """Optimize simulation performance for target FPS"""
    
    timestep = 1.0 / target_fps
    start_time = time.time()
    
    # Performance measurement
    for step in range(50):
        step_start = time.time()
        
        # Core simulation loop
        world.update(timestep)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        step_end = time.time()
        elapsed_ms = (step_end - step_start) * 1000
        
        if elapsed_ms > 33:  # More than 33ms (30 FPS)
            print(f"Step {step}: Slow performance - {elapsed_ms:.2f}ms")
            
            # Optimization suggestions
            if elapsed_ms > 50:
                print("  Suggestion: Reduce brain size or enable SIMD optimizations")
            elif elapsed_ms > 33:
                print("  Suggestion: Increase timestep or reduce simulation complexity")
    
    total_time = time.time() - start_time
    actual_fps = 50 / total_time
    
    return {
        "target_fps": target_fps,
        "actual_fps": actual_fps,
        "performance_ratio": target_fps / actual_fps,
        "optimization_needed": actual_fps < target_fps * 0.8
    }

# Usage
performance_results = optimizePerformance(brain, world, agent, target_fps=30)
print(f"Performance Results: {performance_results}")
```

### Memory Optimization

```python
import pynlm
import gc
import psutil
import os

def optimizeMemoryUsage(brain, world, agent, max_memory_mb=500):
    """Optimize memory usage for simulation"""
    
    process = psutil.Process(os.getpid())
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Force garbage collection
    gc.collect()
    
    # Run simulation and monitor memory
    for step in range(20):
        # Execute simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Check memory usage
        current_memory = process.memory_info().rss / 1024 / 1024
        memory_growth = current_memory - initial_memory
        
        if memory_growth > max_memory_mb:
            print(f"Memory usage high: {current_memory:.1f}MB (growth: {memory_growth:.1f}MB)")
            
            # Memory optimization suggestions
            if memory_growth > 800:
                print("  Suggestion: Implement custom memory pools")
            elif memory_growth > 500:
                print("  Suggestion: Reduce brain size or enable garbage collection")
            else:
                print("  Suggestion: Check for memory leaks in simulation objects")
        
        # Simulate some cleanup
        if step % 10 == 0:
            gc.collect()
    
    final_memory = process.memory_info().rss / 1024 / 1024
    total_growth = final_memory - initial_memory
    
    return {
        "initial_memory_mb": initial_memory,
        "final_memory_mb": final_memory,
        "total_growth_mb": total_growth,
        "memory_efficiency": "good" if total_growth < 100 else "moderate" if total_growth < 300 else "poor"
    }

# Usage
memory_results = optimizeMemoryUsage(brain, world, agent)
print(f"Memory Optimization Results: {memory_results}")
```

## Advanced Examples

### Example 1: Learning Maze Navigation

```python
import pynlm

def createMazeNavigationAgent(width=30, height=30):
    """Create agent that learns to navigate a maze"""
    
    # Create maze world
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 12, 12)
    
    # Create maze walls
    maze_walls = [
        (5, 5, 15, 5, 2),   # Horizontal wall
        (5, 10, 15, 10, 2), # Horizontal wall
        (20, 5, 20, 15, 2), # Vertical wall
    ]
    
    for x1, y1, x2, y2, thickness in maze_walls:
        for x in range(x1, x2 + 1):
            for y in range(y1, y2 + 1):
                if 0 < x < width - 1 and 0 < y < height - 1:
                    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, 0.0f, 1.0f))
    
    # Create goal
    goal = pynlm.WorldObject(width-2, height-2, pynlm.WorldObjectType.Resource, 100.0, 0.5)
    world.addObject(goal)
    
    # Create agent
    config = pynlm.createAdvancedConfig(
        numNeurons=250,
        learningRate=0.01,
        enableDevelopment=True,
        enableRewardModulation=True,
        enableCuriosity=True
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    # Set agent start near entrance
    world.setAgentStart(1, 1)
    world.reset()
    
    return brain, world, agent

def trainMazeNavigation(brain, world, agent, steps=200):
    """Train agent to navigate maze"""
    
    training_stats = {
        "steps_completed": 0,
        "goals_reached": 0,
        "average_reward": 0.0,
        "learning_curve": []
    }
    
    for step in range(steps):
        # Execute sensorimotor loop
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for goal reached
        agent_body = world.getAgentBody()
        if agent_body.x > world.getWidth() - 3 and agent_body.y > world.getHeight() - 3:
            training_stats["goals_reached"] += 1
            print(f"Goal reached at step {step}!")
            world.reset()  # Reset for next episode
            world.setAgentStart(1, 1)  # Start from entrance
        
        # Apply reward modulation
        if result.success:
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        training_stats["steps_completed"] += 1
        training_stats["average_reward"] = (
            training_stats["average_reward"] * (step - 1) + result.reward
        ) / step if step > 0 else result.reward
        
        training_stats["learning_curve"].append({
            "step": step,
            "reward": result.reward,
            "success": result.success,
            "firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel()
        })
        
        if step % 50 == 0:
            print(f"Step {step}: FR={brain.getAverageFiringRate():.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Reward={result.reward}")
    
    return training_stats

# Usage
brain, world, agent = createMazeNavigationAgent(25, 25)
stats = trainMazeNavigation(brain, world, agent, 200)
print(f"\nMaze Navigation Training Results: {stats}")
```

### Example 2: Complex Environment Exploration

```python
import pynlm
import random

def createComplexEnvironment(width=40, height=40):
    """Create complex environment with diverse elements"""
    
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 16, 16)
    
    # Add varied terrain
    for x in range(0, width, 3):
        for y in range(0, height, 3):
            terrain_type = random.choice([
                pynlm.WorldObjectType.Resource,
                pynlm.WorldObjectType.Resource,
                pynlm.WorldObjectType.Hazard,
                pynlm.WorldObjectType.Wall
            ])
            
            if terrain_type == pynlm.WorldObjectType.Resource:
                value = random.uniform(5, 20)
            elif terrain_type == pynlm.WorldObjectType.Hazard:
                value = random.uniform(-10, -2)
            else:
                value = 0.0
            
            radius = 0.5
            if terrain_type == pynlm.WorldObjectType.Wall:
                radius = 1.0
            
            world.addObject(pynlm.WorldObject(x, y, terrain_type, value, radius))
    
    # Add interactive objects
    for i in range(15):
        obj_type = random.choice([
            pynlm.WorldObjectType.Resource,
            pynlm.WorldObjectType.Hazard
        ])
        value = 15.0 if obj_type == pynlm.WorldObjectType.Resource else -8.0
        x = random.uniform(5, width-5)
        y = random.uniform(5, height-5)
        world.addObject(pynlm.WorldObject(x, y, obj_type, value, 0.7))
    
    world.setAgentStart(width/2, height/2)
    world.reset()
    
    return world

def exploreComplexEnvironment(brain, world, agent, steps=300):
    """Explore complex environment with diverse challenges"""
    
    exploration_stats = {
        "steps_completed": 0,
        "resources_collected": 0,
        "hazards_avoided": 0,
        "total_reward": 0.0,
        "environment_changes": [],
        "adaptation_score": 0.0
    }
    
    initial_state = {
        "agent_energy": world.getAgentBody().energy,
        "brain_neuron_count": brain.getTotalNeuronCount()
    }
    
    for step in range(steps):
        # Update environment periodically
        if step % 50 == 0:
            # Add new objects dynamically
            for i in range(3):
                obj_type = random.choice([
                    pynlm.WorldObjectType.Resource,
                    pynlm.WorldObjectType.Hazard
                ])
                value = 10.0 if obj_type == pynlm.WorldObjectType.Resource else -5.0
                x = random.uniform(5, world.getWidth()-5)
                y = random.uniform(5, world.getHeight()-5)
                world.addObject(pynlm.WorldObject(x, y, obj_type, value, 0.6))
            
            exploration_stats["environment_changes"].append({
                "step": step,
                "new_objects": 3,
                "object_types": ["Resource", "Hazard", "Resource"]
            })
        
        # Execute sensorimotor loop
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track exploration metrics
        if result.reward > 0:
            exploration_stats["resources_collected"] += 1
        elif result.reward < 0:
            exploration_stats["hazards_avoided"] += 1
        
        exploration_stats["total_reward"] += result.reward
        
        # Apply learning
        if result.success:
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        exploration_stats["steps_completed"] += 1
        
        if step % 50 == 0:
            print(f"Step {step}: Energy={world.getAgentBody().energy:.1f}, "
                  f"Resources={exploration_stats['resources_collected']}, "
                  f"Hazards={exploration_stats['hazards_avoided']}, "
                  f"Reward={result.reward:.2f}")
    
    # Calculate adaptation score
    final_state = {
        "agent_energy": world.getAgentBody().energy,
        "brain_neuron_count": brain.getTotalNeuronCount()
    }
    
    exploration_stats["adaptation_score"] = (
        (final_state["agent_energy"] - initial_state["agent_energy"]) /
        max(initial_state["agent_energy"], 1) +
        (final_state["brain_neuron_count"] - initial_state["brain_neuron_count"]) /
        max(initial_state["brain_neuron_count"], 1)
    ) / 2.0
    
    return exploration_stats

# Usage
world = createComplexEnvironment(35, 35)
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableAllFeatures()

stats = exploreComplexEnvironment(brain, world, agent, 300)
print(f"\nComplex Environment Exploration Results: {stats}")
```

## Quick Reference

### Core Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createDefaultConfig()` | Create default configuration | `config = pynlm.createDefaultConfig()` |
| `createBrain(config)` | Create brain with configuration | `brain = pynlm.createBrain(config)` |
| `createSimpleWorld()` | Create simple world | `world = pynlm.createSimpleWorld()` |
| `createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |

### Essential Methods

| Method | Category | Description |
|--------|----------|-------------|
| `brain.initialize()` | Brain | Initialize brain with configuration |
| `brain.step(step)` | Brain | Execute one simulation step |
| `brain.getAverageFiringRate()` | Brain | Get average firing rate |
| `agent.processSensoryInput(percept)` | Agent | Process sensory input |
| `agent.decodeMotorCommand()` | Agent | Decode brain activity to action |
| `world.update(timestep)` | World | Update world state |
| `world.applyMotorCommand(action, time)` | World | Execute agent action |

### Common Patterns

```python
# Pattern 1: Simple Brain Simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(100):
    brain.step(step)

# Pattern 2: Complete Agent Simulation
brain, world, agent = createCompleteAgentSetup()
stats = runCompleteSimulation(brain, world, agent)

# Pattern 3: Learning with Rewards
agent.applyRewardModulation(reward=10.0, predicted_reward=5.0)

# Pattern 4: Environment Exploration
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

### Error Handling

```python
import pynlm

def safeExecution(brain, world=None, agent=None, steps=100):
    """Safely execute simulation with error handling"""
    try:
        if brain and not pynlm.validateBrainState(brain):
            raise ValueError("Invalid brain state")
        
        for step in range(steps):
            # Your simulation logic here
            pass
            
    except Exception as e:
        print(f"Error: {e}")
        return False
    
    return True
```

### Performance Monitoring

```python
import time
import psutil

def monitorPerformance(brain, world, agent):
    """Monitor simulation performance"""
    start_time = time.time()
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
    
    end_time = time.time()
    
    # Calculate metrics
    total_time = end_time - start_time
    fps = 100 / total_time
    
    process = psutil.Process()
    memory_mb = process.memory_info().rss / 1024 / 1024
    
    return {
        "total_time_seconds": total_time,
        "frames_per_second": fps,
        "memory_usage_mb": memory_mb,
        "performance_score": min(100, fps / 30 * 100)  # Normalize to 0-100
    }
```

## Best Practices

### 1. Configuration Management

```python
# Always validate configuration
config = pynlm.createAdvancedConfig()
errors = pynlm.checkConfigValidity(config)
if errors:
    print(f"Configuration errors: {errors}")
    sys.exit(1)
```

### 2. Error Handling

```python
# Use safe stepping for critical operations
if not pynlm.safeBrainStep(brain, step):
    print(f"Failed to execute step {step}")
    # Handle error or recovery
```

### 3. Performance Optimization

```python
# Monitor performance regularly
if performance_results["optimization_needed"]:
    # Adjust parameters
    timestep = min(timestep * 1.1, 0.2)  # Increase timestep up to 0.2s
```

### 4. Resource Management

```python
# Use garbage collection for large simulations
import gc
gc.collect()

# Monitor memory usage
if memory_results["memory_efficiency"] == "poor":
    # Implement memory optimization
    pass
```

### 5. Reproducibility

```python
# Set random seed for reproducibility
world.setRandomSeed(12345)
```

## Common Pitfalls and Solutions

### Problem: Brain Not Learning
**Symptoms:** Zero reward, no firing rate changes, stuck in initial state
**Solutions:**
1. Check if `enableRewardModulation` is True
2. Verify agent is receiving sensory input
3. Ensure world has reward sources
4. Check developmental stage progression

### Problem: Agent Stuck in Location
**Symptoms:** Agent not moving, position unchanged
**Solutions:**
1. Check agent energy levels
2. Verify world is being updated
3. Check for obstacles in path
4. Enable curiosity and exploration

### Problem: High Memory Usage
**Symptoms:** Memory grows continuously, crashes
**Solutions:**
1. Implement custom memory pools
2. Reduce brain size
3. Enable garbage collection
4. Use object pooling for frequently created objects

### Problem: Unstable Simulation
**Symptoms:** Rapid firing rate changes, crashes
**Solutions:**
1. Reduce learning rate
2. Enable safe stepping
3. Check configuration validity
4. Add error handling and recovery

## Conclusion

This comprehensive guide provides everything you need to implement, debug, and optimize NLM simulations. Start with the basic examples and gradually move to advanced patterns as you gain experience. Remember to:

1. **Validate configurations** before use
2. **Monitor performance** regularly
3. **Handle errors** gracefully
4. **Use safe stepping** for critical operations
5. **Document your experiments** for reproducibility

The NLM framework offers powerful capabilities for creating intelligent, adaptive agents. With proper configuration and implementation, you can build sophisticated simulation systems that learn, adapt, and explore complex environments.

For the most advanced features and research applications, refer to `advanced_usage.md` and the extensive documentation in the `docs/` directory.