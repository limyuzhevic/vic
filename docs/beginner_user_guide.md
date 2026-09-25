# NLM User Guides

## Overview

The NLM user guides provide **tailored documentation** for different user types and skill levels. These guides cover everything from basic setup and simple examples to advanced configuration and research applications. Each guide builds on the previous one, allowing users to progress their skills systematically.

> **Key Feature:** User guides are **progressively structured** - beginners start with basic concepts, intermediate users learn advanced features, and advanced users explore research capabilities.

## Table of Contents

- [Beginner User Guide](#beginner-user-guide)
- [Intermediate User Guide](#intermediate-user-guide)
- [Advanced User Guide](#advanced-user-guide)
- [User Guide Navigation](#user-guide-navigation)

---

## Beginner User Guide

### What You'll Learn

This guide is designed for users who are **new to NLM** or **new to brain-inspired AI**. It covers:

- Basic NLM concepts and terminology
- Simple setup and installation
- Creating your first brain simulation
- Running basic experiments
- Understanding NLM architecture

> **Target Audience:** Complete beginners, students, hobbyists
> **Prerequisites:** Basic programming knowledge, no NLM experience required

### Chapter 1: Getting Started with NLM

#### What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental computational brain project**. It simulates how biological brains work using spiking neural networks. Think of it like this:

- **Neurons** = Brain cells that send electrical signals
- **Synapses** = Connections between neurons that transmit signals
- **Learning** = Synaptic plasticity (strengthening/weakening connections)
- **Memory** = Neural memory systems that store experiences

NLM simulates **event-driven neural computation** using Leaky Integrate-and-Fire (LIF) neurons with:

- Real neural dynamics and spike timing
- Multiple memory systems (working, episodic, associative)
- Advanced cognitive functions (prediction, attention, planning)
- Neuromodulation (curiosity, novelty, reward processing)
- Development over time

That's it! NLM simulates the complete brain loop: **sensory input → neural processing → memory → action selection**.

#### The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

#### Installing NLM

**Option 1: Using Package Manager (Recommended for beginners)**

```bash
# Install NLM using package manager
# For Ubuntu/Debian
 sudo apt-get install nlm-dev

# For macOS with Homebrew
brew install nlm

# For Windows with Chocolatey
choco install nlm
```

**Option 2: Building from Source (For advanced users)**

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure with default settings
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4
```

**Option 3: Python Package (For quick start)**

```bash
# Install Python dependencies
pip install numpy matplotlib

# Install NLM Python bindings
pip install nlm-python
```

#### Creating Your First Brain

**Method 1: Using Command Line**

```bash
# Create and run a simple brain simulation
cd build
./nlm --help  # Show available commands
```

**Method 2: Using Python (Recommended for beginners)**

```python
#!/usr/bin/env python3
"""
Your first NLM brain simulation
"""
import pynlm

print("=== Creating Your First NLM Brain ===")

# Step 1: Create a brain with default configuration
print("1. Creating brain configuration...")
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize the brain
print("2. Initializing brain...")
brain.initialize()

# Step 3: Make the brain think
print("3. Running brain simulation...")
for i in range(50):
    brain.step(i)

# Step 4: Get results
print("4. Getting results...")
total_spikes = brain.getTotalSpikeCount()
print(f"   Your brain thought {total_spikes} times!")

# Step 5: Clean up
print("5. Cleaning up...")
brain.shutdown()

print("\n=== Success! Your first brain simulation completed ===")
```

#### Understanding Brain Components

**Simple Brain Structure:**

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access different brain components
print("Brain Components:")
print(f"  - Working Memory: {'enabled' if brain.getWorkingMemory() else 'disabled'}")
print(f"  - Episodic Memory: {'enabled' if brain.getEpisodicMemory() else 'disabled'}")
print(f"  - Prediction System: {'enabled' if brain.getPredictionSystem() else 'disabled'}")
print(f"  - Planning System: {'enabled' if brain.getPlanner() else 'disabled'}")
print(f"  - Neuromodulation: {'enabled' if brain.getNeuromodulationSystem() else 'disabled'}")

# Store and retrieve information with working memory
if brain.getWorkingMemory():
    working_memory = brain.getWorkingMemory()
    
    # Store information
    working_memory.store("task", "solve_puzzle")
    working_memory.store("location", "room_1")
    working_memory.store("goal", "reach_door")
    
    # Retrieve information
    task = working_memory.retrieve("task", threshold=0.7)
    location = working_memory.retrieve("location", threshold=0.5)
    
    print(f"\nWorking Memory Results:")
    print(f"  - Retrieved task: {task}")
    print(f"  - Retrieved location: {location}")
    print(f"  - Total entries: {working_memory.getEntryCount()}")

brain.shutdown()
```

#### Running Your First Experiment

**Setting up a Simple Experiment:**

```python
#!/usr/bin/env python3
"""
Your first NLM experiment
"""
import pynlm

print("=== Setting Up Your First Experiment ===")

# Create brain with experiment configuration
config = pynlm.createDefaultConfig()
config.set("simulation.steps", 200)  # Run 200 steps
config.set("simulation.time_step", 0.1)  # 0.1 ms per step

brain = pynlm.createBrain(config)
brain.initialize()

# Create simple world
world = pynlm.createSimpleWorld(width=10, height=10)
world.configure(visionWidth=5, visionHeight=5)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Experiment Setup:")
print(f"  - Simulation steps: {config.get('simulation.steps')}")
print(f"  - World size: {config.get('world.width')}x{config.get('world.height')}")
print(f"  - Brain neurons: {config.get('brain.neuron_count')}")

# Run experiment
print("\nRunning experiment...")
for step in range(200):
    # Process sensory input
    percept = world.getSensoryPercept()
    processed_input = agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Action selection
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for termination
    if world.isEpisodeComplete():
        print(f"  - Episode completed at step {step}")
        world.reset()
        agent.initialize(world)

# Get experiment results
print("\nExperiment Results:")
print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
print(f"  - Memory usage: {brain.getMemoryStats()}")
print(f"  - Simulation time: {config.get('simulation.steps') * config.get('simulation.time_step')} seconds")

# Clean up
brain.shutdown()

print("\n=== Experiment completed successfully! ===")
```

#### Understanding NLM Terminology

**Key Terms and Concepts:**

- **Neuron (Neurons)**: Basic processing units that receive inputs, integrate them, and produce outputs (spikes)
- **Synapse**: Connection between neurons that transmits signals
- **Spike**: Electrical signal generated by a neuron
- **LIF Model**: Leaky Integrate-and-Fire model for neuron dynamics
- **Working Memory**: Temporary storage of current information and goals
- **Episodic Memory**: Storage of specific events and experiences
- **Associative Memory**: Pattern completion and association formation
- **Prediction System**: Forecasting of future events and actions
- **Planning System**: Multi-step action sequence generation
- **Attention**: Selection of relevant information from multiple inputs
- **Neuromodulation**: Systems that guide learning and exploration
- **Development**: Age-dependent changes in brain capabilities

#### Common Problems and Solutions

**Problem 1: Brain Not Initializing**
```python
# Check if brain initialized successfully
brain = pynlm.createBrain(pynlm.createDefaultConfig())
if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Brain initialization failed")
    # Check configuration
    config = pynlm.createDefaultConfig()
    print(f"Brain neurons: {config.get('brain.neuron_count')}")
    print(f"World size: {config.get('world.width')}x{config.get('world.height')}")
```

**Problem 2: Memory System Not Working**
```python
# Check if memory systems are available
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

if brain.getWorkingMemory():
    print("Working memory is available")
    # Try storing and retrieving
    working_memory = brain.getWorkingMemory()
    working_memory.store("test", "value")
    retrieved = working_memory.retrieve("test", threshold=0.5)
    print(f"Retrieved value: {retrieved}")
else:
    print("Working memory not available")
    print("This may be due to configuration settings")
```

**Problem 3: No Output or Errors**
```python
# Enable logging for debugging
import logging
logging.basicConfig(level=logging.DEBUG)

# Try simple brain test
brain = pynlm.createBrain(pynlm.createDefaultConfig())
if brain.initialize():
    print("Brain initialized successfully")
    for i in range(10):
        brain.step(i)
    print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
else:
    print("Brain initialization failed")
```

#### Getting Help and Resources

**Built-in Help:**
```python
import pynlm

# Get help information
help_info = pynlm.getHelp()
print(help_info)

# Get example code
examples = pynlm.getExamples()
for name, code in examples.items():
    print(f"\n=== {name} ===")
    print(code)
```

**Online Resources:**
- **Documentation**: https://nlm-project.org/docs
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Support**: support@nlm-project.org

**Local Help:**
```python
import pynlm

# Get local help information
local_help = pynlm.getLocalHelp()
print(local_help)
```

### Chapter 2: Building on Your Skills

#### Creating Different Types of Brains

**1. Simple Brain (Default Configuration)**
```python
import pynlm

# Create simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simple simulation
for i in range(100):
    brain.step(i)

brain.shutdown()
```

**2. Advanced Brain (Phase 6 Configuration)**
```python
import pynlm

# Create Phase 6 brain with all features
config = pynlm.createPhase6Config()
config.set("memory.working.capacity", 200)
config.set("memory.episodic.capacity", 50)
config.set("cognition.planning.enabled", True)
config.set("neuromodulation.curiosity.strength", 0.8)

brain = pynlm.createBrain(config)
brain.initialize()

# Access all cognitive systems
working_memory = brain.getWorkingMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
neuromodulation = brain.getNeuromodulationSystem()

print("Phase 6 Brain Created Successfully")
print(f"  - Working memory: {'enabled' if working_memory else 'disabled'}")
print(f"  - Prediction system: {'enabled' if prediction_system else 'disabled'}")
print(f"  - Planning system: {'enabled' if planner else 'disabled'}")
print(f"  - Neuromodulation: {'enabled' if neuromodulation else 'disabled'}")

brain.shutdown()
```

**3. Performance Optimized Brain**
```python
import pynlm

# Create performance-optimized configuration
config = pynlm.createPhase6Config()
config.set("optimization.performance_mode", "maximum")
config.set("simulation.batch_size", 100)
config.set("memory.cache_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run batch simulation
batch_size = 1000
for batch in range(batch_size // 100):
    for step in range(100):
        brain.step(batch * 100 + step)

brain.shutdown()
```

#### Exploring Different Memory Systems

**Working Memory Operations:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_memory = brain.getWorkingMemory()

# Store different types of information
working_memory.store("task_objective", "solve_puzzle", "context")
working_memory.store("current_location", "room_1", "context")
working_memory.store("target_objective", "reach_door", "context")

# Retrieve information
retrieved_task = working_memory.retrieve("task_objective", threshold=0.7)
retrieved_location = working_memory.retrieve("current_location", threshold=0.5)

print(f"Working memory entries: {working_memory.getEntryCount()}")
print(f"Retrieved task: {retrieved_task}")
print(f"Retrieved location: {retrieved_location}")
```

**Episodic Memory Operations:**
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
    "context": "room_1",
    "prediction_error": 0.1
})

# Retrieve similar episodes
similar_episodes = episodic_memory.retrieveSimilar("visual_pattern_A", top_k=5)

print(f"Episodic memory entries: {episodic_memory.getEntryCount()}")
print(f"Retrieved {len(similar_episodes)} similar episodes")
```

**Associative Memory Operations:**
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

#### Understanding Cognitive Systems

**1. Prediction System:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

prediction_system = brain.getPredictionSystem()

if prediction_system:
    # Train with temporal sequence
    sequence = [1, 2, 3, 4, 5]
    prediction_system.trainSequence(sequence)
    
    # Make predictions
    for i in range(len(sequence) - 1):
        predicted_next = prediction_system.predictNext()
        actual_next = sequence[i + 1]
        print(f"Predicted {predicted_next}, Actual {actual_next}")
    
    # Get prediction accuracy
    accuracy = prediction_system.getAccuracy()
    print(f"Prediction accuracy: {accuracy}")
```

**2. Planning System:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

planner = brain.getPlanner()

if planner:
    # Create action plan
    possible_actions = ["move_north", "move_south", "move_east", "move_west", "stay"]
    plan = planner.planAction("explore_room", possible_actions, 10)
    
    print(f"Generated plan: {plan}")
    print(f"Plan length: {len(plan)} steps")
    print(f"Plan confidence: {planner.getPlanConfidence(plan)}")
```

**3. Attention System:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

attention = brain.getAttentionSystem()

if attention:
    # Set attention priorities
    attention.setPriority("task", 0.8)
    attention.setPriority("sensory", 0.5)
    attention.setPriority("memory", 0.3)
    
    # Get attended information
    attended = attention.getAttendedInput()
    print(f"Attended input: {attended}")
    print(f"Attention focus: {attention.getAttentionFocus('task')}")
```

**4. Self-Model System:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

self_model = brain.getSelfModel()

if self_model:
    # Get internal state
    internal_state = self_model.getInternalState()
    print(f"Internal state features: {len(internal_state)}")
    
    # Check consistency
    consistency = self_model.checkConsistency()
    print(f"Self-model consistency: {consistency}")
    
    # Predict action consequences
    action = {"move": "north", "strength": 0.8}
    prediction = self_model.predictActionConsequences(action)
    print(f"Action prediction: {prediction}")
```

#### Working with the Agent-World System

**Basic Agent-World Setup:**
```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

world = pynlm.createSimpleWorld(width=20, height=20)
world.configure(visionWidth=5, visionHeight=5)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Agent-World System Initialized")
print(f"  - Brain initialized: {brain.isInitialized()}")
print(f"  - Agent initialized: {agent.isInitialized()}")
print(f"  - World size: {world.getWidth()}x{world.getHeight()}")
```

**Running an Episode:**
```python
import pynlm

# Create systems
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

world = pynlm.createSimpleWorld(width=20, height=20)
world.configure(visionWidth=5, visionHeight=5)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Running agent episode...")

# Run episode
for step in range(500):
    # Sensory processing
    percept = world.getSensoryPercept()
    processed_input = agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Action selection
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for termination
    if world.isEpisodeComplete():
        print(f"  - Episode completed at step {step}")
        world.reset()
        agent.initialize(world)

print("Episode completed successfully")
print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
print(f"  - Memory usage: {brain.getMemoryStats()}")

# Clean up
agent.shutdown()
brain.shutdown()
```

### Chapter 3: Advancing Your Skills

#### Intermediate Concepts

**1. Error Handling and Debugging**

```python
import pynlm
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

try:
    # Create and configure brain
    config = pynlm.createPhase6Config()
    config.set("validation.enabled", True)
    config.set("debug.logging_enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
    print(f"Error code: {e.error_code}")
    print(f"Suggestions: {e.suggestions}")
    
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
    if 'agent' in locals():
        agent.shutdown()
    if 'world' in locals():
        world.reset()
```

**2. Performance Monitoring**

```python
import pynlm

brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Create performance monitor
monitor = pynlm.PerformanceMonitor()
monitor.start()

# Run simulation
for step in range(100):
    brain.step(step)
    monitor.recordStep(step)

# Get performance statistics
stats = monitor.stop()

print("Performance Statistics:")
print(f"  Total time: {stats.total_time:.2f}s")
print(f"  Operations per second: {stats.operations_per_second:.2f}")
print(f"  Memory usage: {stats.memory_usage_mb:.2f} MB")
print(f"  CPU utilization: {stats.cpu_utilization:.2f}%")
print(f"  Spike rate: {stats.spike_rate:.2f} Hz")
print(f"  Memory efficiency: {stats.memory_efficiency:.2f}")
print(f"  Stability score: {stats.stability:.2f}")
```

**3. Advanced Memory Operations**

```python
import pynlm

brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

working_memory = brain.getWorkingMemory()

# Store multiple items with different priorities
working_memory.store("urgent_task", "fix_bug", "high_priority")
working_memory.store("background_task", "update_docs", "low_priority")
working_memory.store("current_goal", "complete_feature", "medium_priority")

# Retrieve with different thresholds
urgent_task = working_memory.retrieve("urgent_task", threshold=0.9)
background_task = working_memory.retrieve("background_task", threshold=0.3)

print(f"Working memory entries: {working_memory.getEntryCount()}")
print(f"Urgent task: {urgent_task}")
print(f"Background task: {background_task}")

# Get memory statistics
print(f"Memory capacity: {working_memory.getMemoryCapacity()}")
print(f"Remaining capacity: {working_memory.getRemainingCapacity()}")
```

#### Research and Experimental Usage

**1. Setting Up Research Experiments**

```python
#!/usr/bin/env python3
"""
Research experiment setup for NLM
"""
import pynlm
import json
import time

class NLMExperiment:
    def __init__(self, experiment_name):
        self.experiment_name = experiment_name
        self.results = {}
        self.start_time = None
        
    def setup(self, config_type="phase6", duration=1000):
        """Setup the experiment"""
        self.start_time = time.time()
        
        if config_type == "phase6":
            self.config = pynlm.createPhase6Config()
            self.config.set("memory.working.capacity", 500)
            self.config.set("memory.episodic.capacity", 100)
            self.config.set("cognition.planning.enabled", True)
        elif config_type == "default":
            self.config = pynlm.createDefaultConfig()
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.results["setup_time"] = time.time() - self.start_time
        self.results["config_type"] = config_type
        self.results["duration"] = duration
        
    def run(self, world_size=20, episodes=10):
        """Run the experiment"""
        self.world = pynlm.createSimpleWorld(world_size, world_size)
        self.world.configure(visionWidth=world_size//2, visionWidth=world_size//2)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        self.results["world_size"] = world_size
        self.results["episodes"] = episodes
        
        episode_times = []
        
        for episode in range(episodes):
            episode_start = time.time()
            
            for step in range(self.results["duration"]):
                # Process sensory input
                percept = self.world.getSensoryPercept()
                processed_input = self.agent.processSensoryInput(percept)
                
                # Brain processing
                self.brain.step(step)
                
                # Action selection
                action = self.agent.decodeMotorCommand()
                
                # Apply action to world
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Check for termination
                if self.world.isEpisodeComplete():
                    episode_times.append(time.time() - episode_start)
                    self.world.reset()
                    self.agent.initialize(self.world)
                    break
            
            self.results[f"episode_{episode}_time"] = episode_times[-1] if episode_times else 0
        
        self.results["total_simulation_time"] = time.time() - self.start_time
        self.results["average_episode_time"] = sum(episode_times) / len(episode_times) if episode_times else 0
        
    def analyze(self):
        """Analyze experiment results"""
        self.results["total_spikes"] = self.brain.getTotalSpikeCount()
        self.results["memory_stats"] = self.brain.getMemoryStats()
        
        # Calculate performance metrics
        if "total_simulation_time" in self.results:
            self.results["spikes_per_second"] = self.results["total_spikes"] / self.results["total_simulation_time"]
        
        return self.results
        
    def save_results(self, filename):
        """Save experiment results"""
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2)
        
    def cleanup(self):
        """Cleanup experiment resources"""
        if hasattr(self, 'agent'):
            self.agent.shutdown()
        if hasattr(self, 'brain'):
            self.brain.shutdown()
        if hasattr(self, 'world'):
            self.world.reset()

# Usage example
experiment = NLMExperiment("NLM_Research_Experiment")
experiment.setup(config_type="phase6", duration=500)
experiment.run(world_size=10, episodes=5)
results = experiment.analyze()

print(f"Experiment completed successfully!")
print(f"Results: {json.dumps(results, indent=2)}")

experiment.save_results("experiment_results.json")
experiment.cleanup()
```

**2. Creating Custom Brain Configurations**

```python
#!/usr/bin/env python3
"""
Create custom brain configurations for NLM
"""
import pynlm

def create_research_config(memory_capacity=300, prediction_enabled=True, planning_enabled=True):
    """Create a custom research configuration"""
    config = pynlm.createPhase6Config()
    
    # Memory settings
    config.set("memory.working.capacity", memory_capacity)
    config.set("memory.episodic.capacity", memory_capacity // 3)
    config.set("memory.associative.strength", 0.8)
    
    # Cognitive settings
    config.set("cognition.attention.enabled", True)
    config.set("cognition.planning.enabled", planning_enabled)
    config.set("cognition.self_model.enabled", True)
    config.set("cognition.social_learning.enabled", False)  # Disable for controlled experiments
    
    # Neuromodulation settings
    config.set("neuromodulation.dopamine.enabled", True)
    config.set("neuromodulation.curiosity.enabled", True)
    config.set("neuromodulation.novelty.enabled", True)
    config.set("neuromodulation.curiosity.strength", 0.7)
    
    # Prediction settings
    config.set("prediction.temporal.enabled", prediction_enabled)
    config.set("prediction.action_consequence.enabled", prediction_enabled)
    
    # Development settings
    config.set("development.stages", True)
    config.set("development.plasticity", "age_dependent")
    
    return config

def create_performance_config(performance_mode="maximum", batch_size=100):
    """Create a performance-optimized configuration"""
    config = pynlm.createPhase6Config()
    
    # Performance settings
    config.set("optimization.performance_mode", performance_mode)
    config.set("simulation.batch_size", batch_size)
    config.set("memory.cache_enabled", True)
    config.set("memory.compression_enabled", True)
    config.set("neuromodulation.lazy_evaluation", True)
    
    # Simulation settings
    config.set("simulation.max_steps", 10000)
    config.set("simulation.checkpoint_interval", 1000)
    
    return config

def create_experimental_config(modify_memory=True, modify_cognition=True, modify_neuromodulation=True):
    """Create an experimental configuration for testing"""
    config = pynlm.createPhase6Config()
    
    if modify_memory:
        config.set("memory.working.capacity", 200)
        config.set("memory.episodic.capacity", 50)
        config.set("memory.associative.strength", 0.6)
    
    if modify_cognition:
        config.set("cognition.attention.enabled", True)
        config.set("cognition.planning.enabled", True)
        config.set("cognition.self_model.enabled", True)
    
    if modify_neuromodulation:
        config.set("neuromodulation.dopamine.enabled", True)
        config.set("neuromodulation.curiosity.enabled", True)
        config.set("neuromodulation.novelty.enabled", True)
        config.set("neuromodulation.curiosity.strength", 0.5)
    
    # Add experimental parameters
    config.set("experiment.custom_parameter_1", 1.0)
    config.set("experiment.custom_parameter_2", 2.0)
    config.set("experiment.custom_parameter_3", 3.0)
    
    return config

# Usage examples
print("=== Custom Configuration Examples ===")

# Research configuration
research_config = create_research_config(memory_capacity=400, planning_enabled=True)
brain = pynlm.createBrain(research_config)
brain.initialize()
print(f"Research brain created with {brain.getNeuronCount()} neurons")
brain.shutdown()

# Performance configuration
performance_config = create_performance_config(performance_mode="maximum")
brain = pynlm.createBrain(performance_config)
brain.initialize()
print(f"Performance brain created with optimization mode: {performance_config.get('optimization.performance_mode')}")
brain.shutdown()

# Experimental configuration
experimental_config = create_experimental_config(modify_memory=True, modify_cognition=True)
brain = pynlm.createBrain(experimental_config)
brain.initialize()
print(f"Experimental brain created with custom parameters")
brain.shutdown()

print("\nCustom configurations created successfully!")
```

## User Guide Navigation

### Primary Navigation

Use the sections in this guide to progress through NLM skills:

1. **Beginner Documentation** - Basic concepts and simple examples
2. **Intermediate Documentation** - Advanced features and error handling
3. **Advanced Documentation** - Research and experimental usage
4. **Performance Optimization** - High-performance configurations
5. **Memory Systems** - Deep dive into memory operations
6. **Cognitive Systems** - Understanding brain functions
7. **Research Applications** - Setting up experiments
8. **Troubleshooting** - Common problems and solutions

### Navigation Features

- **Progressive Learning**: Each chapter builds on previous knowledge
- **Code Examples**: Copy-paste ready code with explanations
- **Error Handling**: Robust error handling and debugging
- **Performance Optimization**: Tips for optimal performance
- **Research Applications**: Templates for experimental research
- **Community Resources**: Links to additional documentation and support

### Quick Reference

**Common Operations:**
```python
# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Access systems
working_memory = brain.getWorkingMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()

# Cleanup
brain.shutdown()
```

**Key Methods:**
- `createDefaultConfig()`: Basic configuration
- `createPhase6Config()`: Advanced configuration
- `createBrain()`: Create brain instance
- `initialize()`: Initialize brain
- `step()`: Run simulation step
- `shutdown()`: Cleanup resources

### Learning Path

**Week 1:** Basic Concepts
- Chapter 1: Getting Started with NLM
- Chapter 2: Building on Your Skills

**Week 2:** Intermediate Skills
- Creating different types of brains
- Exploring memory systems
- Understanding cognitive systems

**Week 3:** Advanced Skills
- Error handling and debugging
- Performance monitoring
- Research and experimental usage

### Support Resources

**Built-in Help:**
- `pynlm.getHelp()` - Get comprehensive help information
- `pynlm.getExamples()` - Get working code examples

**Online Resources:**
- **Documentation**: https://nlm-project.org/docs
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Support**: support@nlm-project.org

**Community:**
- **Slack/Discord**: Real-time community support
- **Forums**: Technical discussions and Q&A
- **Email**: Direct technical support

### Next Steps

After completing this beginner guide:

1. **Practice**: Create and run your own brain simulations
2. **Experiment**: Try different configuration options
3. **Explore**: Use the intermediate guide for advanced features
4. **Research**: Use the advanced guide for experimental applications
5. **Contribute**: Share your experiences and improvements with the community

### Conclusion

This beginner guide provides you with the **foundational knowledge** and **practical skills** needed to start using NLM effectively. By following this guide, you will:

1. **Understand** how NLM simulates brain-like computation
2. **Set up** your first brain simulation environment
3. **Create** and run basic experiments
4. **Use** memory and cognitive systems
5. **Debug** and troubleshoot common issues
6. **Explore** advanced features and capabilities

Remember: The best way to learn NLM is through **hands-on practice**. Start with the simple examples, gradually add complexity, and explore the full capabilities of Phase 6 integration.

**Good luck with your NLM journey!** 🚀