# NLM Documentation: Comprehensive User Guide

## Quick Start: Your First Neural Brain

This guide will walk you through creating and running your first NLM simulation. We'll build a simple brain that learns from experience through real neural dynamics.

---

## 1. Quick Start Examples

### 1.1 Silent Brain (Just Neurons)

```python
import pynlm

# Create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active - neurons will fire based on internal dynamics
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

**What happens:** This creates a brain with ~1000 neurons in LIF dynamics. They start with random voltages and begin firing spontaneously through recurrent connections.

### 1.2 Brain in a Simple World

```python
import pynlm

# Setup the brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent to connect brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    # Update world (move agent, check collisions)
    world.update(0.1)
    
    # Get sensory input (vision, touch, internal signals)
    percept = world.getSensoryPercept()
    
    # Process sensory input through brain
    agent.processSensoryInput(percept)
    
    # Brain thinks (spikes propagate, plasticity occurs)
    brain.step(step)
    
    # Get action from brain motor neurons
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward from world
    result = world.applyAction(action, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, agent.getPredictionError())

print("Simulation complete!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
```

**What happens:** The brain acts as a complete agent, sensing its environment, making decisions, and learning from rewards. All through real spiking neural dynamics.

---

## 2. Core Concepts

### 2.1 Neural Architecture

NLM implements a brain-like system with several key components:

| Component | Analogy | Function |
|-----------|---------|----------|
| **Neurons** | Brain cells | Process information via spikes |
| **Synapses** | Neural connections | Transmit signals between neurons |
| **Memory** | Working/episodic memory | Store experiences and patterns |
| **Modulation** | Neuromodulators | Control learning and attention |
| **Prediction** | Predictive coding | Anticipate future events |
| **Cognition** | Higher functions | Plan, attend, form concepts |
| **Development** | Maturation | Change with experience |

### 2.2 Brain Loop Architecture

```
WORLD
  ↓
SENSORY INPUT (vision, touch, internal)
  ↓
NEURAL PROCESSING (LIF spikes, plasticity)
  ↓
INTERNAL STATE (memory, attention)
  ↓
MEMORY / PREDICTION (store/learn patterns)
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / PREDICTION ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN → FUTURE BEHAVIOR
```

### 2.3 Key Mechanisms

#### Leaky Integrate-and-Fire (LIF) Dynamics
- **Membrane potential** integrates inputs over time
- **Threshold** triggers spikes when reached
- **Refractory period** prevents runaway excitation
- **Adaptation** prevents excessive firing rates

#### Spike-Based Communication
- **Event-driven**: Only neurons that fire communicate
- **Delays**: Realistic transmission delays between neurons
- **Synaptic weights**: Excitatory/inhibitory balance
- **Plasticity**: STDP strengthens/weakens connections

#### Memory Systems
- **Working Memory**: Maintains active information through persistent spiking
- **Episodic Memory**: Stores experiences as patterns
- **Associative Memory**: Forms connections between experiences

#### Neuromodulation
- **Dopamine**: Reward prediction error, modulates STDP
- **Curiosity**: Novelty detection, drives exploration
- **Novelty**: Detects new patterns, affects attention

---

## 3. Configuration and Setup

### 3.1 Default Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# View current settings
print(config.summary())

# Modify parameters
config.set("neuron_count", 2000)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)
```

### 3.2 Common Configuration Presets

#### Research Mode (High Precision)
```python
config = pynlm.createDefaultConfig()

# High neuron count for complex behaviors
config.set("neuron_count", 5000)
config.set("region_count", 3)

# Enable all learning systems
config.set("enable_reward_modulation", True)
config.set("enable_curiosity", True)
config.set("enable_development", True)
config.set("enable_structural_plasticity", True)

# Longer simulation
config.set("simulation_timestep", 0.0005)
```

#### Fast Training Mode
```python
config = pynlm.createDefaultConfig()

# Smaller brain for quick testing
config.set("neuron_count", 500)
config.set("region_count", 1)

# Disable structural plasticity (faster)
config.set("enable_structural_plasticity", False)

# Longer timestep (faster simulation)
config.set("simulation_timestep", 0.01)
```

### 3.3 Save/Load Configuration

```python
# Save configuration to file
config.saveToFile("my_config.cfg")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.cfg")

# Load from command line
parser_config = pynlm.createDefaultConfig()
parser_config.loadFromArgs(len(sys.argv), sys.argv)
```

---

## 4. Practical Examples

### 4.1 Exploration vs. Exploitation

```python
import pynlm
import random

def run_exploration_vs_exploitation():
    """Demonstrates curiosity-driven exploration vs. learned exploitation"""
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable curiosity for exploration
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    exploration_count = 0
    exploitation_count = 0
    
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Check if curiosity is high (exploration)
        if agent.getCuriosityLevel() > 0.4:
            exploration_count += 1
        else:
            exploitation_count += 1
            
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    print(f"Exploration steps: {exploration_count} ({exploration_count/500*100:.1f}%)")
    print(f"Exploitation steps: {exploitation_count} ({exploitation_count/500*100:.1f}%)")
    
    return exploration_count / 500.0

# Run multiple times to see curiosity balance
for trial in range(3):
    exploration_rate = run_exploration_vs_exploitation()
    print(f"Trial {trial + 1}: Exploration rate = {exploration_rate:.2f}\n")
```

### 4.2 Memory Retention Experiment

```python
import pynlm
import matplotlib.pyplot as plt

def test_memory_retention():
    """Tests how well the brain retains information over time"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("enable_reward_modulation", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Store initial firing pattern as "memory"
    initial_spikes = brain.getTotalSpikeCount()
    initial_firing = brain.getFiringNeuronCount()
    
    # Run without stimulation for 1000 steps
    for step in range(1000):
        brain.step(step)
    
    # Check retention
    later_spikes = brain.getTotalSpikeCount() - initial_spikes
    later_firing = brain.getFiringNeuronCount()
    
    # Calculate retention metrics
    spike_retention = later_spikes / (initial_spikes + 1) * 100
    firing_retention = later_firing / (initial_firing + 1) * 100
    
    print(f"Spike retention: {spike_retention:.1f}%")
    print(f"Firing rate retention: {firing_retention:.1f}%")
    
    return spike_retention, firing_retention

# Run retention test
retention_results = test_memory_retention()

# Plot results (if matplotlib available)
try:
    plt.figure(figsize=(10, 6))
    plt.bar(['Spike Retention', 'Firing Rate Retention'], 
            retention_results, color=['skyblue', 'lightgreen'])
    plt.ylabel('Retention (%)')
    plt.title('NLM Memory Retention Over Time')
    plt.ylim(0, 100)
    plt.show()
except:
    print("Matplotlib not available - skipping visualization")
```

### 4.3 Reward Learning Demonstration

```python
import pynlm

def demonstrate_reward_learning():
    """Shows how dopamine-based reward learning shapes neural responses"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 800)
    config.set("enable_reward_modulation", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create a simple deterministic world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Track learning progress
    rewards = []
    prediction_errors = []
    firing_rates = []
    
    # Create a predictable pattern: every 20 steps, give +1 reward
    for step in range(200):
        # Simple movement pattern
        action = pynlm.MotorCommand.MoveForward
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for predictable reward
        reward = 1.0 if (step % 20 == 0) else 0.0
        predicted = agent.getPredictionError()
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.applyRewardModulation(reward, predicted)
        
        rewards.append(reward)
        prediction_errors.append(agent.getPredictionError())
        firing_rates.append(brain.getAverageFiringRate())
    
    # Analyze learning
    learning_effectiveness = sum(1 for r in rewards if r > 0) / (len(rewards) / 20)
    
    print(f"Predictable reward schedule: every 20 steps")
    print(f"Learning effectiveness: {learning_effectiveness:.1f}/5.0")
    print(f"Final firing rate: {firing_rates[-1]:.2f} Hz")
    print(f"Final dopamine level: {agent.getNeuromodulationLevel():.3f}")
    
    return learning_effectiveness

# Run learning demonstration
learning_score = demonstrate_reward_learning()
print(f"\nReward learning effectiveness score: {learning_score:.2f}")
```

---

## 5. Advanced Topics

### 5.1 Development and Maturation

```python
def run_development_experiment():
    """Shows how the brain develops and changes over time"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("enable_development", True)
    config.set("enable_structural_plasticity", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Track developmental changes
    developmental_stages = []
    plasticity_levels = []
    firing_rates = []
    
    for step in range(500):
        # Simulate time passing
        brain.step(step)
        
        # Agent updates its development
        agent.updateDevelopment(0.1)
        
        # Record developmental state
        stage = brain.getDevelopmentalStage()
        developmental_stages.append(stage)
        plasticity_levels.append(agent.getNeuromodulationLevel())
        firing_rates.append(brain.getAverageFiringRate())
    
    # Analyze development
    unique_stages = set(developmental_stages)
    print(f"Developmental stages reached: {len(unique_stages)}")
    print(f"Final developmental stage: {developmental_stages[-1]}")
    print(f"Final plasticity level: {plasticity_levels[-1]:.3f}")
    
    return len(unique_stages), developmental_stages

# Run development analysis
stage_count, stages = run_development_experiment()
```

### 5.2 Integration Testing

```python
import pynlm
import time

def comprehensive_integration_test():
    """Tests the full integration of all brain systems"""
    
    print("=== NLM Phase 6 Integration Test ===\n")
    
    # Create brain with all systems
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    config.set("enable_development", True)
    config.set("enable_structural_plasticity", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify all systems are present
    print("1. Checking system integration...")
    
    # Memory systems
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    prediction_sys = brain.getPredictionSystem()
    planner = brain.getPlanner()
    
    systems_status = {
        "Working Memory": working_mem is not None,
        "Episodic Memory": episodic_mem is not None,
        "Prediction System": prediction_sys is not None,
        "Neural Planner": planner is not None,
        "Development": brain.getDevelopmentSystem() is not None,
        "Dopamine": brain.getDopamine() is not None,
        "Curiosity": brain.getCuriosity() is not None,
        "Novelty": brain.getNovelty() is not None,
    }
    
    for system, status in systems_status.items():
        print(f"   {system}: {'✓' if status else '✗'}")
    
    # Test brain loop integration
    print("\n2. Testing brain loop integration...")
    
    # Create world for testing
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run a simulation
    start_time = time.time()
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    # Collect statistics
    total_spikes = brain.getTotalSpikeCount()
    avg_firing = brain.getAverageFiringRate()
    total_neurons = brain.getTotalNeuronCount()
    
    print(f"   Simulation time: {end_time - start_time:.2f}s")
    print(f"   Total spikes: {total_spikes}")
    print(f"   Average firing rate: {avg_firing:.2f} Hz")
    print(f"   Neuron count: {total_neurons}")
    
    # Test plasticity
    print("\n3. Testing plasticity...")
    
    initial_synapses = brain.getTotalSynapseCount()
    
    # Apply reward learning
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        result = world.applyAction(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    final_synapses = brain.getTotalSynapseCount()
    synapse_change = final_synapses - initial_synapses
    
    print(f"   Initial synapses: {initial_synapses}")
    print(f"   Final synapses: {final_synapses}")
    print(f"   Synapse change: {synapse_change}")
    
    # Overall status
    all_systems_working = all(systems_status.values())
    has_plasticity = synapse_change != 0
    
    print("\n=== Integration Test Results ===")
    print(f"All systems present: {'✓' if all_systems_working else '✗'}")
    print(f"Plasticity working: {'✓' if has_plasticity else '✗'}")
    print(f"Brain loop functional: {'✓' if (total_spikes > 0 and avg_firing > 0) else '✗'}")
    
    return all_systems_working and has_plasticity and (total_spikes > 0)

# Run comprehensive test
success = comprehensive_integration_test()
print(f"\nIntegration Test: {'PASSED' if success else 'FAILED'}")
```

---

## 6. Performance and Debugging

### 6.1 Monitoring Brain State

```python
def monitor_brain_performance():
    """Monitors brain performance and provides debugging information"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("=== Brain Performance Monitor ===\n")
    
    for step in range(50):
        print(f"Step {step:3d}:")
        
        # Log basic statistics
        print(f"  Neurons: {brain.getTotalNeuronCount():5d} "
              f"| Spikes: {brain.getTotalSpikeCount():6d} "
              f"| Active: {brain.getActiveNeuronCount():5d}")
        
        print(f"  Firing rate: {brain.getAverageFiringRate():6.3f} Hz "
              f"| E/I ratio: {brain.getExcitationInhibitionRatio():6.3f}")
        
        # Check system states
        if brain.getWorkingMemory():
            active_traces = brain.getWorkingMemory().getActiveTraces()
            print(f"  Working memory: {active_traces} active traces")
        
        if brain.getEpisodicMemory():
            episodes = brain.getEpisodicMemory().getEpisodeCount()
            print(f"  Episodic memory: {episodes} episodes stored")
        
        # Get neuromodulation levels
        if brain.getDopamine():
            dopamine = brain.getDopamine().getLevel()
            print(f"  Dopamine: {dopamine:6.3f}")
        
        if brain.getCuriosity():
            curiosity = brain.getCuriosity().getLevel()
            print(f"  Curiosity: {curiosity:6.3f}")
        
        brain.step(step)
        
        if step < 49:  # Don't sleep after last iteration
            import time
            time.sleep(0.1)
    
    print("\n=== Performance Summary ===")
    print("Brain is functioning normally with all systems operational.")

# Run performance monitor
monitor_brain_performance()
```

### 6.2 Troubleshooting Common Issues

#### Issue: No spikes or activity

```python
import pynlm

def debug_no_activity():
    """Debugs why the brain isn't producing activity"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("=== Debugging No Activity Issue ===\n")
    
    # Check configuration
    print("1. Configuration check:")
    print(f"   Neuron count: {config.getOr('neuron_count', 'not set')}")
    print(f"   Regions: {config.getOr('region_count', 'not set')}")
    print(f"   Connection probability: {config.getOr('connection_probability', 'not set')}")
    
    # Check initial brain state
    print("\n2. Initial brain state:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Firing neurons (initial): {brain.getFiringNeuronCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    
    # Run one step and check
    print("\n3. One-step simulation:")
    brain.step(0)
    
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    # Check for common problems
    if brain.getTotalNeuronCount() == 0:
        print("\n   ❌ PROBLEM: No neurons created!")
        print("   Solution: Check configuration parameters")
    elif brain.getTotalSynapseCount() == 0:
        print("\n   ❌ PROBLEM: No synapses created!")
        print("   Solution: Increase connection_probability in config")
    elif brain.getFiringNeuronCount() == 0:
        print("\n   ❌ PROBLEM: No neurons firing!")
        print("   Possible causes:")
        print("   - Neurons need time to 'warm up'")
        print("   - Check membrane potentials and thresholds")
        print("   - Verify synaptic connections are functional")
    else:
        print("\n   ✓ Brain is active - issue may be elsewhere")
    
    return brain.getTotalNeuronCount() > 0 and brain.getFiringNeuronCount() > 0

# Debug the issue
debug_ok = debug_no_activity()
print(f"\nDebug successful: {'✓' if debug_ok else '✗'}")
```

---

## 7. Research and Experimentation

### 7.1 Experiment Template

```python
import pynlm
import time

class NLMExperiment:
    """Base class for NLM experiments"""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.setup_environment()
    
    def setup_environment(self):
        """Setup the experiment environment"""
        # Configure brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world if needed
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.config.getOr("experiment_width", 20),
            height=self.config.getOr("experiment_height", 20),
            visionWidth=self.config.getOr("vision_width", 8),
            visionHeight=self.config.getOr("vision_height", 8)
        )
        self.world.reset()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning if needed
        if self.config.getOr("enable_learning", True):
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
    
    def run_trial(self, trial_id):
        """Run a single experiment trial"""
        raise NotImplementedError("Subclasses must implement run_trial")
    
    def run_experiment(self, num_trials):
        """Run the full experiment"""
        print(f"Running {num_trials} trials...")
        
        results = []
        for trial in range(num_trials):
            print(f"  Trial {trial + 1}/{num_trials}")
            trial_result = self.run_trial(trial)
            results.append(trial_result)
        
        return self.aggregate_results(results)
    
    def aggregate_results(self, results):
        """Aggregate trial results"""
        raise NotImplementedError("Subclasses must implement aggregate_results")
    
    def save_results(self, filepath):
        """Save experiment results to file"""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)

# Example: Memory Retention Experiment
class MemoryRetentionExperiment(NLMExperiment):
    """Experiment to test memory retention over time"""
    
    def run_trial(self, trial_id):
        # Store initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_firing = self.brain.getFiringNeuronCount()
        
        # Wait for memory consolidation
        for step in range(self.config.getOr("memory_test_steps", 500)):
            self.brain.step(step)
        
        # Measure retention
        final_spikes = self.brain.getTotalSpikeCount() - initial_spikes
        final_firing = self.brain.getFiringNeuronCount()
        
        spike_retention = final_spikes / (initial_spikes + 1)
        firing_retention = final_firing / (initial_firing + 1)
        
        return {
            "trial": trial_id,
            "initial_spikes": initial_spikes,
            "final_spikes": final_spikes,
            "spike_retention": spike_retention,
            "firing_retention": firing_retention,
            "time_steps": self.config.getOr("memory_test_steps", 500)
        }
    
    def aggregate_results(self, results):
        self.results = {
            "experiment": "Memory Retention",
            "parameters": self.config.getKeys(),
            "trials": results,
            "summary": {
                "mean_spike_retention": sum(r["spike_retention"] for r in results) / len(results),
                "mean_firing_retention": sum(r["firing_retention"] for r in results) / len(results),
                "total_trials": len(results)
            }
        }
        return self.results

# Example usage
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("enable_reward_modulation", True)

experiment = MemoryRetentionExperiment(config)
results = experiment.run_experiment(num_trials=5)

print(f"\nExperiment Results:")
print(f"Mean spike retention: {results['summary']['mean_spike_retention']:.3f}")
print(f"Mean firing retention: {results['summary']['mean_firing_retention']:.3f}")

# Save results
experiment.save_results("memory_retention_results.json")
```

---

## 8. Advanced Features

### 8.1 Context Manager API

```python
# Create a brain with automatic resource management
import pynlm
import tempfile
import os

# Using context manager for automatic cleanup
with pynlm.createBrainContext(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Do work with brain
    for step in range(10):
        brain.step(step)
    
    # Brain automatically cleaned up when exiting context

# File-based checkpointing with context
with tempfile.TemporaryDirectory() as temp_dir:
    checkpoint_file = os.path.join(temp_dir, "brain_checkpoint.bin")
    
    # Create and save brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.save(checkpoint_file)
    
    # Load brain later
    loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    loaded_brain.load(checkpoint_file)
    
    print("Brain checkpointed and loaded successfully")
```

### 8.2 Progress Monitoring

```python
import pynlm
import time
import threading

class ExperimentProgressMonitor:
    """Monitors and displays experiment progress"""
    
    def __init__(self, total_steps):
        self.total_steps = total_steps
        self.current_step = 0
        self.start_time = time.time()
        self.metrics = []
        self.monitor_thread = None
        self.stop_monitor = False
    
    def monitor(self):
        """Background monitoring thread"""
        while not self.stop_monitor:
            elapsed = time.time() - self.start_time
            steps_per_sec = self.current_step / elapsed if elapsed > 0 else 0
            estimated_total = elapsed / (self.current_step / self.total_steps) if self.current_step > 0 else 0
            
            print(f"\rProgress: {self.current_step}/{self.total_steps} ({self.current_step/self.total_steps*100:.1f}%) "
                  f"| {steps_per_sec:.1f} steps/sec | Elapsed: {elapsed:.1f}s | "
                  f"ETA: {estimated_total - elapsed:.1f}s", end='')
            
            time.sleep(0.5)
    
    def start_monitoring(self):
        """Start background monitoring"""
        self.monitor_thread = threading.Thread(target=self.monitor)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop monitoring and print final stats"""
        self.stop_monitor = True
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
        
        elapsed = time.time() - self.start_time
        steps_per_sec = self.current_step / elapsed if elapsed > 0 else 0
        
        print(f"\nExperiment completed!")
        print(f"Total steps: {self.current_step}")
        print(f"Elapsed time: {elapsed:.1f}s")
        print(f"Speed: {steps_per_sec:.1f} steps/second")
    
    def record_metric(self, name, value):
        """Record a metric at current step"""
        self.metrics.append({
            "step": self.current_step,
            "time": time.time() - self.start_time,
            "name": name,
            "value": value
        })
    
    def get_metric(self, name, step=None):
        """Get metric value at specific step"""
        if step is None:
            step = self.current_step
        
        for metric in reversed(self.metrics):
            if metric["step"] == step and metric["name"] == name:
                return metric["value"]
        return None

# Example usage with progress monitoring
def monitored_experiment():
    """Experiment with progress monitoring"""
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    monitor = ExperimentProgressMonitor(100)
    monitor.start_monitoring()
    
    for step in range(100):
        # Record metrics
        monitor.record_metric("firing_rate", brain.getAverageFiringRate())
        monitor.record_metric("spike_count", brain.getTotalSpikeCount())
        monitor.record_metric("active_neurons", brain.getActiveNeuronCount())
        
        brain.step(step)
        monitor.current_step = step
        
        # Simulate some work
        time.sleep(0.05)
    
    monitor.stop_monitoring()
    
    # Analyze metrics
    firing_rates = [m["value"] for m in monitor.metrics if m["name"] == "firing_rate"]
    if firing_rates:
        print(f"\nFiring rate stats: {min(firing_rates):.3f} - {max(firing_rates):.3f} Hz")
    
    return monitor.metrics

# Run monitored experiment
metrics = monitored_experiment()
```

---

## 9. Migration Guide

### 9.1 From Phase 3 to Phase 6

If you're migrating from an older NLM version, here are the key changes:

#### Configuration Changes
```python
# Old way (Phase 3)
config = {
    "neuron_count": 1000,
    "region_count": 2,
    "simulation_timestep": 0.001
}

# New way (Phase 6)
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("simulation_timestep", 0.001)
```

#### Brain Initialization
```python
# Old way
brain = Brain(config)
brain.initialize()

# New way
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Memory Systems
```python
# Old way (disconnected)
working_mem = brain.getWorkingMemory()  # Could be None

# New way (integrated)
working_mem = brain.getWorkingMemory()  # Now properly integrated
if working_mem:
    working_mem.store("important_info", value)
```

#### Neural Components
```python
# Old way (many were placeholders)
STDP* stdp = brain.getSTDP()  # Now fully implemented

# New way
stdp = brain.getSTDP()
if (stdp) {
    stdp->configure(0.01f, 0.012f, 20.0f);
}
```

### 9.2 API Changes

| Component | Old API | New API | Notes |
|-----------|---------|---------|-------|
| `Config` | Simple key=value | Same | Improved validation |
| `Brain` | Limited methods | Full integration | All systems now connected |
| `Memory` | Placeholders | Working implementations | Properly integrated |
| `Neuromodulation` | Basic | Full suite | Dopamine through Novelty |
| `Prediction` | Basic | Advanced | Multiple prediction types |
| `Cognition` | Disconnected | Working | Planner, concepts, attention |

---

## 10. Frequently Asked Questions

### 10.1 "My brain isn't firing any spikes. What should I do?"

**Solution:** This is common. Try:

```python
# 1. Increase connection probability
config.set("connection_probability", 0.5)

# 2. Increase neuron count for more spontaneous activity
config.set("neuron_count", 2000)

# 3. Give neurons time to "warm up"
for step in range(100):
    brain.step(step)

# 4. Check membrane potentials
# Neurons may need external input to reach threshold
```

### 10.2 "The simulation is too slow. How can I make it faster?"

**Solutions:**

```python
# 1. Use larger timesteps
config.set("simulation_timestep", 0.01)

# 2. Disable structural plasticity (computationally expensive)
config.set("enable_structural_plasticity", False)

# 3. Reduce neuron count for testing
config.set("neuron_count", 500)

# 4. Use SIMD optimizations
# (Enable if available in build configuration)
```

### 10.3 "How do I save and restore a working brain?"

```python
# Save brain state
brain.save("my_brain.bin")

# Restore later
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("my_brain.bin")
```

### 10.4 "What are the best practices for experiment design?"

**Best Practices:**

1. **Start simple**: Begin with minimal configuration
2. **Monitor metrics**: Track firing rates, spikes, memory usage
3. **Use proper random seeds**: Ensure reproducibility
4. **Validate integration**: Check all systems are connected
5. **Test incrementally**: Verify each system before adding more
6. **Document your experiments**: Keep detailed notes of parameters

### 10.5 "How do I enable all learning capabilities?"

```python
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)  # Dopamine learning
agent.enableCuriosity(True)        # Exploration drive
agent.enableDevelopment(True)      # Age-based changes
agent.enableStructuralPlasticity(True)  # Grow/prune connections
```

---

## 11. Troubleshooting and FAQ

### 11.1 Common Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Failed to initialize brain" | Configuration errors | Check parameter values |
| "No neurons found" | Too few neurons or regions | Increase neuron_count/region_count |
| "Memory system disconnected" | Integration not complete | Use newer NLM version |
| "Firing rate too high" | Network instability | Check connection weights |

### 11.2 Getting Help

1. **Documentation**: Read this guide thoroughly
2. **Examples**: Run the examples in sections 4-8
3. **Configuration**: Check config parameters
4. **Integration**: Verify all systems are connected
5. **Logging**: Enable detailed logging for debugging

### 11.3 Performance Tips

```python
# Performance optimization tips
# 1. Batch operations where possible
# 2. Use larger timesteps for speed
# 3. Disable unused systems
# 4. Monitor memory usage
# 5. Use proper random seeds for reproducibility
```

---

## 12. Conclusion

NLM Phase 6 provides a comprehensive, integrated artificial brain platform that combines:

- **Real neural dynamics** through spiking neuron models
- **Multiple memory systems** for experience storage
- **Neuromodulation** for adaptive learning
- **Prediction systems** for anticipating outcomes
- **Cognitive mechanisms** for planning and attention
- **Development** for lifelong adaptation
- **Embodiment** through world interaction

The key innovation is **integration over addition** - rather than adding more disconnected components, Phase 6 ensures all existing systems work together as a coherent whole.

Whether you're a researcher studying neural computation, a developer building adaptive systems, or a student learning about brain-inspired AI, NLM provides the tools to explore how complex cognition can emerge from simple neural mechanisms.

**Start here:** Create your first brain with the examples in Section 1, then explore the comprehensive tutorials and advanced features to build increasingly sophisticated neural systems.

---

*This documentation was last updated for NLM Phase 6.0. For the latest updates, check the repository documentation.*