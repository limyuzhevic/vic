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

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

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

### Project 4: Advanced Agent with Custom Configuration

```python
import pynlm

# Create and configure custom brain
config = pynlm.createDefaultConfig()

# Advanced configuration
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.08)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create sophisticated world
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=16,
    visionHeight=16,
    maxEnergy=200.0
)
world.reset()
world.setAgentStart(15.0, 15.0)
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run sophisticated simulation
for step in range(500):
    # Update world with adaptive timestep
    timestep = 0.1 + (step % 10) * 0.01  # Varying timestep
    world.update(timestep)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory data
    agent.processSensoryInput(percept)
    
    # Brain computes action
    brain.step(step, world.getSimulationTime())
    
    # Agent decides action with advanced curiosity
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    predicted_reward = agent.getExpectedReward()  # This would need to be implemented
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development
    agent.updateDevelopment(timestep)
    
    # Print advanced statistics periodically
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")

print("Advanced agent simulation complete!")
print(f"Final development stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
```

### Project 5: Learning Experiment with Multiple Trials

```python
import pynlm
import time

class LearningExperiment:
    def __init__(self):
        # Create configurable brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("plasticity.stdp.learning_rate", 0.001)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create learning environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
        self.world.reset()
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Learning statistics
        self.trial_data = []
        self.total_spikes = 0
        
    def run_trial(self, trial_id):
        """Run a single learning trial"""
        print(f"Starting Trial {trial_id}")
        
        # Reset for new trial
        self.brain.reset()
        self.brain.initialize()
        self.agent.reset()
        self.world.reset()
        
        # Record initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_firing = self.brain.getFiringNeuronCount()
        
        # Run trial steps
        for step in range(200):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Log reward and learning
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            self.total_spikes += (self.brain.getTotalSpikeCount() - initial_spikes)
        
        # Record final state
        final_spikes = self.brain.getTotalSpikeCount()
        final_firing = self.brain.getFiringNeuronCount()
        trial_spikes = final_spikes - initial_spikes
        
        # Store trial data
        trial_info = {
            'trial_id': trial_id,
            'initial_spikes': initial_spikes,
            'trial_spikes': trial_spikes,
            'final_firing': final_firing,
            'development_stage': brain.getDevelopmentalStage(),
            'learning_efficiency': trial_spikes / 200.0 if step > 0 else 0.0
        }
        self.trial_data.append(trial_info)
        
        print(f"  Trial {trial_id} completed: {trial_spikes} spikes generated")
        return trial_info
    
    def run_experiment(self, num_trials=5):
        """Run multiple learning trials"""
        print("Starting Learning Experiment")
        print(f"Running {num_trials} trials...")
        
        start_time = time.time()
        
        for i in range(num_trials):
            trial_result = self.run_trial(i)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Print experiment summary
        print("\n=== Experiment Summary ===")
        print(f"Total trials: {num_trials}")
        print(f"Total duration: {duration:.2f} seconds")
        print(f"Total spikes across all trials: {self.total_spikes}")
        print(f"Average spikes per trial: {self.total_spikes / num_trials:.1f}")
        
        # Analyze learning trends
        learning_trend = []
        for trial in self.trial_data:
            if trial['trial_id'] > 0:
                prev_spikes = self.trial_data[trial['trial_id'] - 1]['trial_spikes']
                improvement = (trial['trial_spikes'] - prev_spikes) / prev_spikes * 100
                learning_trend.append(improvement)
        
        if learning_trend:
            avg_improvement = sum(learning_trend) / len(learning_trend)
            print(f"Average learning improvement: {avg_improvement:.1f}%")
        
        return self.trial_data

# Run the learning experiment
experiment = LearningExperiment()
results = experiment.run_experiment(3)

print("\nExperiment complete! Results saved to experiment data.")
```

# Command Line Interface

NLM provides a powerful command-line interface for advanced users who want more control over simulations and agent management.

## Basic Commands

### Create and run a simple brain simulation

```bash
# Create a default simulation
./nlm --brain.neuron_count=1000 --simulation_timestep=0.001

# Run with logging
./nlm --log-level=info --brain.neuron_count=2000

# Quick test with predefined parameters
./nlm --config=tests/config/quick_test.cfg
```

### Agent management commands

```bash
# Create and run an agent simulation
./nlm --mode=agent --world.width=30 --world.height=30

# Agent with custom learning parameters
./nlm --mode=agent \
     --agent.reward_modulation=true \
     --agent.curiosity=true \
     --agent.development=true \
     --brain.neuron_count=3000

# Save agent behavior
./nlm --mode=agent --save=agent_state.json --duration=1000

# Load and continue agent
./nlm --mode=agent --load=agent_state.json --continue
```

### Advanced agent commands

```bash
# Run with personality traits
./nlm --mode=agent \
     --personality.curious=true \
     --personality.exploratory=true \
     --personality.persistent=true \
     --personality.explosive=false

# Multi-agent coordination (experimental)
./nlm --mode=multi_agent \
     --agent_count=3 \
     --team_coordination=true \
     --shared_memory=true

# Advanced learning mode
./nlm --mode=advanced \
     --learning.algorithm=td_learning \
     --learning.exploration=epsilon_greedy \
     --learning.exploration_rate=0.2
```

## Configuration File Examples

### Advanced agent configuration

```bash
# Save advanced agent configuration
cat > agent_config.json << EOF
{
    "agent": {
        "name": "AdvancedResearchAgent",
        "brain": {
            "neuron_count": 5000,
            "connection_probability": 0.05,
            "region_count": 3
        },
        "neuromodulation": {
            "reward_modulation": true,
            "curiosity": true,
            "novelty_detection": true,
            "prediction_error": true
        },
        "development": {
            "enabled": true,
            "plasticity_rate": 0.8,
            "developmental_stages": 5
        },
        "personality": {
            "curiosity_level": 0.7,
            "exploration_rate": 0.3,
            "risk_tolerance": 0.5,
            "social_cooperation": 0.6
        },
        "behavior": {
            "save_interval": 100,
            "checkpoint_enabled": true,
            "replay_enabled": false
        }
    }
}
EOF

# Use the configuration
./nlm --config=agent_config.json --mode=agent
```

### Experiment configuration

```bash
# Create experiment configuration
export EXPERIMENT_ID="experiment_$(date +%Y%m%d_%H%M%S)"
cat > exp_config.json << EOF
{
    "experiment": {
        "id": "$EXPERIMENT_ID",
        "type": "learning_curiosity",
        "description": "Study of curiosity-driven exploration",
        "parameters": {
            "duration": 3600,
            "save_interval": 300,
            "checkpoint_interval": 1800,
            "reset_interval": 7200
        },
        "conditions": {
            "curiosity_enabled": true,
            "curiosity_intensity": 0.8,
            "novelty_weight": 0.6,
            "prediction_error_weight": 0.4
        },
        "agents": [
            {
                "name": "curious_explorer",
                "curiosity": 0.9,
                "exploration": 0.7
            },
            {
                "name": "cautious_explorer", 
                "curiosity": 0.5,
                "exploration": 0.3
            }
        ],
        "metrics": [
            "spike_count",
            "learning_rate",
            "exploration_frequency",
            "reward_accumulation",
            "behavioral_diversity"
        ]
    }
}
EOF

# Run experiment
./nlm --config=exp_config.json --mode=experiment --run
```

### Performance optimization configuration

```bash
# Performance configuration
export PERF_CONFIG="high_performance"
cat > perf_config.json << EOF
{
    "performance": {
        "optimization_level": "aggressive",
        "thread_count": 8,
        "memory_pool_size": "512MB",
        "vectorization": true,
        "simd_extensions": ["sse4.2", "avx2"],
        "caching_strategy": "adaptive",
        "precomputation": true,
        "incremental_learning": true
    },
    "simulation": {
        "timestep": 0.001,
        "duration": 300,
        "precision": "double",
        "overflow_protection": true
    }
}
EOF

# Run optimized simulation
./nlm --config=perf_config.json --mode=optimized
```
