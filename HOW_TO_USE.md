# NLM - Advanced Usage Guide

## Introduction

This guide covers advanced usage patterns and commands for NLM (Neural Learning Machine), building on the basic usage in `easy_usage.md`. This guide is designed for users who want to:

- Work with more complex configurations
- Use advanced brain features
- Understand the development process
- Access Python bindings for integration

---

## Advanced Brain Configuration

### Loading Brain from Command Line

```bash
# Run with default config
./nlm --config configs/default.cfg

# Override specific parameters
./nlm \
  --config configs/my_config.cfg \
  --random_seed=123 \
  --neuron_count=1000 \
  --connection_probability=0.2

# Quick testing mode (faster with fewer neurons)
./nlm \
  --neuron_count=100 \
  --region_count=2 \
  --stdp_ltp_weight=0.05 \
  --stdp_ltd_weight=0.03
```

### Configuration File Format

NLM uses JSON configuration files. Example `configs/my_config.json`:

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.15,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "learning_enabled": true,
  "structural_plasticity": true,
  "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_detection": true
    }
}
```

### Programmatically Create Complex Brain

```python
import pynlm

# Create advanced config
config = pynlm.createDefaultConfig()
config.set("random_seed", 12345)
config.set("neuron_count", 1000)
config.set("connection_probability", 0.2)
config.set("stdp_ltp_weight", 0.05)
config.set("stdp_ltd_weight", 0.03)

# Create brain with advanced config
brain = pynlm.createBrain(config)
brain.initialize()

# Enable all advanced features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

---

## Advanced Python Bindings

### Complete Agent Simulation

```python
import pynlm

def run_advanced_simulation(config_file="configs/default.cfg", steps=1000):
    """Run a complete simulation with all features enabled."""
    
    # 1. Setup configuration
    config = pynlm.createDefaultConfig()
    if config_file and pynlm.file_exists(config_file):
        config.loadFromFile(config_file)
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, height=30,
        visionWidth=8, visionHeight=8
    )
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 5. Run simulation
    rewards = []
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = agent.getCurrentReward()
        rewards.append(reward)
        
        # Apply neuromodulation if available
        if agent.hasPredictionError():
            pred_error = agent.getPredictionError()
            agent.applyNeuromodulation(pred_error)
    
    # 6. Return results
    return {
        "brain": brain,
        "world": world,
        "agent": agent,
        "total_reward": sum(rewards),
        "avg_reward": sum(rewards) / len(rewards) if rewards else 0,
        "steps": steps,
        "spike_count": brain.getTotalSpikeCount(),
        "learning_metrics": get_learning_metrics(brain)
    }

def get_learning_metrics(brain):
    """Get comprehensive learning metrics."""
    return {
        "total_spikes": brain.getTotalSpikeCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "excitatory_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "region_count": brain.getRegionCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "developmental_stage": str(brain.getDevelopmentalStage()),
        "has_structural_plasticity": brain.getStructuralPlasticity().isEnabled(),
        "has_stdp": brain.getSTDP().getName() if brain.getSTDP() else None
    }
```

### Batch Processing

```python
import pynlm
import json

def run_multiple_simulations(configs, steps_per_config=500):
    """Run multiple simulations with different configurations."""
    results = []
    
    for i, config_dict in enumerate(configs):
        print(f"Running simulation {i+1}/{len(configs)}...")
        
        # Create config
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            config.set(key, value)
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_config):
            brain.step(step)
        
        # Collect metrics
        metrics = {
            "config_id": i,
            "total_spikes": brain.getTotalSpikeCount(),
            "final_firing_rate": brain.getAverageFiringRate(),
            "region_count": brain.getRegionCount()
        }
        
        results.append(metrics)
    
    return results

# Example configurations
experiment_configs = [
    {"neuron_count": 100, "connection_probability": 0.1},
    {"neuron_count": 500, "connection_probability": 0.2},
    {"neuron_count": 1000, "connection_probability": 0.15}
]

results = run_multiple_simulations(experiment_configs)
print(json.dumps(results, indent=2))
```

---

## Advanced Features

### Structural Plasticity

```python
# Check structural plasticity status
sp = brain.getStructuralPlasticity()

# Configure structural plasticity
sp.setSynaptogenesisRate(0.0001)
sp.setPruningRate(0.00001)
sp.setMaxWeightThreshold(0.05)

# Force structural plasticity update
import random
agent = pynlm.createAgentBrain(brain)
rng = random.Random(42)  # Example random generator
sp.update(brain, rng)
```

### Neuromodulation

```python
# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

# Configure neuromodulation
dopamine.setLearningRate(0.01)
dopamine.setBaselineActivity(0.1)

curiosity.setExplorationRate(0.2)
curiosity.setNoveltyThreshold(0.5)

# Apply neuromodulatory signals
from nlm.neuromodulation.Neuromodulator import Neuromodulator
reward_signal = Neuromodulator(0.5, 0.8, "reward")  # strength, duration, type
brain.applyNeuromodulation(reward_signal)
```

### Memory Systems

```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Store and retrieve experiences
working_memory.store("visual_pattern", [0.1, 0.5, 0.9, 0.3])
working_memory.store("temporal_context", step)

# Retrieve based on similarity
similar_memories = working_memory.recall("visual_pattern", threshold=0.7)
```

### Prediction and Planning

```python
# Access prediction systems
prediction_system = brain.getPredictionSystem()
neural_planner = brain.getPlanner()

# Generate predictions
prediction = prediction_system.predict(observation)
prediction_error = prediction_system.getPredictionError(observation, reward)

# Planning
plan = neural_planner.plan(current_state, goal_state, action_options)
```

---

## Development and Evolution

### Development Stages

NLM supports developmental stages that affect plasticity:

```python
# Access development system
development = brain.getDevelopmentSystem()

# Set developmental stage
development.setStage("growth")  # or "critical_period", "maturation", "adult"

# Check current stage
current_stage = brain.getDevelopmentalStage()

# Development affects plasticity rates
print(f"Current development stage: {current_stage}")
print(f"Plasticity multiplier: {development.getPlasticityMultiplier()}")
```

### Developmental Stages Details

- **Initial**: Primitive connections, high plasticity
- **Critical Period**: Sensitive learning window, enhanced plasticity
- **Maturation**: Refinement, pruning of unused connections
- **Adult**: Stable, fine-tuning only
- **Aging**: Gradual decline in plasticity

---

## Performance and Profiling

### Brain Profiling

```python
import time

def profile_brain_performance(brain, steps=100):
    """Profile brain performance metrics."""
    
    # Time brain operations
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    end_time = time.time()
    
    elapsed = end_time - start_time
    steps_per_second = steps / elapsed
    
    # Get system stats
    stats = {
        "computation_time_ms": elapsed * 1000,
        "steps_per_second": steps_per_second,
        "memory_usage_mb": get_memory_usage(brain),
        "spike_generation_rate": brain.getTotalSpikeCount() / steps,
        "neuron_activity": {
            "active": brain.getActiveNeuronCount(),
            "firing": brain.getFiringNeuronCount(),
            "total": brain.getTotalNeuronCount()
        }
    }
    
    return stats

def get_memory_usage(brain):
    """Estimate memory usage of brain."""
    # This is a simplified estimation
    neuron_memory = brain.getTotalNeuronCount() * 1024  # ~1KB per neuron
    synapse_memory = brain.getTotalSynapseCount() * 64   # ~64 bytes per synapse
    
    return (neuron_memory + synapse_memory) / (1024 * 1024)  # Convert to MB
```

### Checkpoints and Persistence

```python
# Save brain state for later resumption
save_path = "checkpoints/brain_checkpoint_123.json"
if brain.save(save_path):
    print(f"Brain saved to {save_path}")
else:
    print("Failed to save brain")

# Load brain state
load_path = "checkpoints/brain_checkpoint_456.json"
if os.path.exists(load_path):
    new_brain = pynlm.createBrain(config)
    if new_brain.load(load_path):
        print("Brain loaded successfully")
        brain = new_brain
    else:
        print("Failed to load brain")
```

---

## Experimentation

### Running Experiments

```python
import pynlm

# Create experiment framework
experiment = pynlm.createExperiment()

# Add variables to track
experiment.add_variable("spike_count", 0)
experiment.add_variable("firing_rate", 0.0)
experiment.add_variable("reward_total", 0.0)

# Run experiment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    
    # Collect data
    experiment.record("spike_count", brain.getTotalSpikeCount())
    experiment.record("firing_rate", brain.getAverageFiringRate())
    
    # Your experiment logic here
    # e.g., inject specific patterns, test specific conditions

# Analyze results
results = experiment.analyze()
print(f"Average firing rate: {results['firing_rate']['mean']}")
print(f"Spike count statistics: {results['spike_count']}")
```

### A/B Testing

```python
import pynlm
from collections import defaultdict

def run_ab_test(config_a, config_b, steps=500, iterations=10):
    """Run A/B test comparing two configurations."""
    
    results_a = defaultdict(list)
    results_b = defaultdict(list)
    
    for iteration in range(iterations):
        # Configuration A
        brain_a = pynlm.createBrain(config_a)
        brain_a.initialize()
        
        for step in range(steps):
            brain_a.step(step)
            record_metrics(results_a, brain_a, step)
        
        # Configuration B  
        brain_b = pynlm.createBrain(config_b)
        brain_b.initialize()
        
        for step in range(steps):
            brain_b.step(step)
            record_metrics(results_b, brain_b, step)
    
    return compare_results(results_a, results_b)

def record_metrics(results, brain, step):
    results['spike_count'].append(brain.getTotalSpikeCount())
    results['firing_rate'].append(brain.getAverageFiringRate())
    results['neuron_count'].append(brain.getTotalNeuronCount())

def compare_results(results_a, results_b):
    """Compare and return statistical significance."""
    from statistics import mean, stdev
    
    comparison = {}
    
    for metric in ['spike_count', 'firing_rate', 'neuron_count']:
        a_values = results_a[metric]
        b_values = results_b[metric]
        
        comparison[metric] = {
            'A_mean': mean(a_values),
            'A_std': stdev(a_values) if len(a_values) > 1 else 0,
            'B_mean': mean(b_values),
            'B_std': stdev(b_values) if len(b_values) > 1 else 0,
            'difference': mean(b_values) - mean(a_values),
            'relative_improvement': (mean(b_values) - mean(a_values)) / mean(a_values) * 100
        }
    
    return comparison
```

---

## Advanced Python Integration

### Real-time Integration

```python
import pynlm
import asyncio
import websockets

class NLMWebSocketServer:
    def __init__(self, brain):
        self.brain = brain
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 5, 5)
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(self.world)
        
    async def handle_client(self, websocket, path):
        """Handle WebSocket client connections."""
        while True:
            try:
                # Receive command from client
                data = await websocket.receive_text()
                command = json.loads(data)
                
                # Process command
                response = await self.process_command(command)
                await websocket.send(json.dumps(response))
                
            except websockets.exceptions.ConnectionClosed:
                break
            except Exception as e:
                await websocket.send(json.dumps({"error": str(e)}))
    
    async def process_command(self, command):
        """Process various commands."""
        cmd_type = command.get("type")
        
        if cmd_type == "step":
            # Advance simulation
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(command.get("step", 0))
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            return {
                "status": "step_completed",
                "spike_count": self.brain.getTotalSpikeCount(),
                "action": str(action)
            }
            
        elif cmd_type == "config":
            # Modify configuration
            key = command.get("key")
            value = command.get("value")
            # This would need a method to modify brain config
            return {"status": "config_updated", "key": key, "value": value}
            
        elif cmd_type == "stats":
            # Get current stats
            return {
                "spikes": self.brain.getTotalSpikeCount(),
                "neurons": self.brain.getTotalNeuronCount(),
                "regions": self.brain.getRegionCount(),
                "firing_rate": self.brain.getAverageFiringRate()
            }

# Start server
server = NLMWebSocketServer(brain)
start_server = websockets.serve(server.handle_client, "localhost", 8765)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
```

### Machine Learning Integration

```python
import pynlm
import numpy as np

class NLMBrainTrainer:
    """Train brain with ML integration patterns."""
    
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
    def train_on_pattern(self, pattern, label, steps=100):
        """Train brain on a specific pattern."""
        
        # Reset brain for clean training
        self.brain.reset()
        self.brain.initialize()
        
        # Inject pattern multiple times
        for step in range(steps):
            # Convert pattern to sensory input
            percept = self.create_sensory_input(pattern)
            
            # Feed to brain
            agent = self.create_agent_if_needed()
            agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Apply reward based on label
            reward = self.calculate_reward(brain, label, pattern)
            self.apply_reward_modulation(reward)
        
        return self.evaluate_training(brain, pattern, label)
    
    def create_sensory_input(self, pattern):
        """Convert numerical pattern to sensory input."""
        # Create Vision input for patterns
        from nlm.sensory.Vision import Vision
        vision = Vision()
        
        # Convert pattern to 2D array if needed
        if isinstance(pattern, (list, np.ndarray)):
            # Reshape to 2D if 1D
            if len(pattern) <= 100:  # Reasonable size for vision
                size = int(np.sqrt(len(pattern)))
                if size * size == len(pattern):
                    data = [float(x) for x in pattern]
                    vision = Vision(size, size, 1)
                    vision.setData(data)
        
        return vision
    
    def evaluate_training(self, brain, pattern, expected_label):
        """Evaluate how well brain learned the pattern."""
        
        # Get current state
        firing_rate = brain.getAverageFiringRate()
        spike_count = brain.getTotalSpikeCount()
        
        # Analyze connectivity
        total_synapses = brain.getTotalSynapseCount()
        
        # Get network statistics
        stats = {
            "firing_rate": firing_rate,
            "spike_count": spike_count,
            "total_synapses": total_synapses,
            "neurons": brain.getTotalNeuronCount(),
            "regions": brain.getRegionCount(),
            "learning_success": self.analyze_learning_success(brain, pattern, expected_label)
        }
        
        return stats
    
    def analyze_learning_success(self, brain, pattern, expected_label):
        """Analyze if learning was successful."""
        # This is a simplified analysis - in practice would need
        # more sophisticated metrics
        firing_rate = brain.getAverageFiringRate()
        
        # Successful learning if firing rate is above threshold
        # and shows patterns related to input
        success_threshold = 0.5  # Hz
        success = firing_rate > success_threshold
        
        return {
            "success": success,
            "firing_rate": firing_rate,
            "threshold": success_threshold,
            "pattern_responses": self.detect_pattern_responses(brain, pattern)
        }
    
    def detect_pattern_responses(self, brain, pattern):
        """Detect if brain responds to specific patterns."""
        # Simplified pattern detection
        # In practice would use more sophisticated algorithms
        firing_rate = brain.getAverageFiringRate()
        
        # Response strength based on firing rate
        response_strength = min(firing_rate / 5.0, 1.0)  # Cap at 1.0
        
        return {
            "detected": response_strength > 0.3,
            "strength": response_strength,
            "pattern_match": self.match_to_expected(pattern)
        }
    
    def match_to_expected(self, pattern):
        """Match learned pattern to expected."""
        # This would depend on what was expected
        # For now, return a simple similarity
        return 0.5  # Placeholder

# Example usage
trainer = NLMBrainTrainer(pynlm.createDefaultConfig())

# Train on simple pattern
pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
results = trainer.train_on_pattern(pattern, "simple_pattern")
print(f"Training results: {results}")
```

---

## Troubleshooting

### Common Issues

#### Brain is not firing

**Symptoms:** No spikes, low activity
**Causes:**
- Brain not initialized
- No sensory input
- Threshold too high
- No current injection

**Solutions:**
```python
# Ensure brain is properly initialized
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Inject current to make neurons fire
for i in range(10):
    brain.injectCurrentToNeurons(NeuronType.Sensory, 50.0)
    brain.step(i)

# Check neuron parameters
region = brain.getRegion(RegionId(1))
if region:
    neurons = region.getAllNeurons()
    for neuron in neurons[:3]:
        print(f"Neuron: V={neuron.getMembranePotential()}, thresh={neuron.getThreshold()}")
```

#### No sensory input

**Symptoms:** Agent doesn't respond to environment
**Causes:**
- World not configured
- Agent not initialized
- Missing sensory percepts

**Solutions:**
```python
# Properly initialize world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Check sensory input
percept = world.getSensoryPercept()
print(f"Sensory type: {percept.getType()}")
print(f"Sensory data shape: {percept.getData()}")
```

#### Learning not working

**Symptoms:** Weights not changing, no plasticity
**Causes:**
- Plasticity rules disabled
- No spike activity
- Plasticity rules not enabled on synapses

**Solutions:**
```python
# Enable all plasticity systems
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check synapse plasticity
for region in brain.getRegions():
    for syn in region.getSynapses():
        print(f"Synapse {syn.getId()}:")
        print(f"  Weight: {syn.getWeight()}")
        print(f"  Plasticity flags: Hebbian={syn.getPlasticityFlags().hebbian}, STDP={syn.getPlasticityFlags().stdp}")
```

---

## Advanced Configuration Examples

### High-Performance Configuration

```python
# Optimize for performance over accuracy
perf_config = pynlm.createDefaultConfig()
perf_config.set("neuron_count", 200)
perf_config.set("region_count", 2)
perf_config.set("connection_probability", 0.1)
perf_config.set("stdp_ltp_weight", 0.01)
perf_config.set("stdp_ltd_weight", 0.012)
perf_config.set("stdp_tau", 10.0)
perf_config.set("synaptogenesis_rate", 0.00005)
perf_config.set("pruning_rate", 0.000001)

# Use optimized brain mode
brain = pynlm.createBrain(perf_config)
brain.initialize()
```

### Exploration-Focused Configuration

```python
# Optimize for exploration and novelty detection
explore_config = pynlm.createDefaultConfig()
explore_config.set("neuron_count", 500)
explore_config.set("random_seed", 12345)
explore_config.set("curiosity_rate", 0.8)
explore_config.set("novelty_threshold", 0.3)

# Enable curiosity-driven exploration
brain = pynlm.createBrain(explore_config)
brain.initialize()

curiosity = brain.getCuriosity()
curiosity.setExplorationRate(0.7)
curiosity.setNoveltyThreshold(0.2)
```

### Memory-Intensive Configuration

```python
# Optimize for memory capacity and retention
memory_config = pynlm.createDefaultConfig()
memory_config.set("neuron_count", 1000)
memory_config.set("working_memory_size", 1000)
memory_config.set("episodic_memory_capacity", 10000)
memory_config.set("associative_memory_size", 500)

# Enable all memory systems
brain = pynlm.createBrain(memory_config)
brain.initialize()

working_memory = brain.getWorkingMemory()
working_memory.setCapacity(1000)

episodic_memory = brain.getEpisodicMemory()
episodic_memory.setMaxCapacity(10000)
```

---

## Conclusion

This guide covers the advanced usage patterns for NLM. The system provides:

- **Flexible configuration** through JSON files and programmatic API
- **Comprehensive plasticity** including STDP, Hebbian, and structural learning
- **Integrated memory systems** for working, episodic, and associative memory
- **Neuromodulation** for reward-based and curiosity-driven learning
- **Prediction and planning** systems for goal-directed behavior
- **Development stages** for brain maturation over time
- **Performance optimization** options for different use cases

The key to effective NLM usage is understanding the brain-world-agent loop:

1. **Create** brain, world, and agent
2. **Initialize** all components
3. **Loop**: World updates → Sensory input → Brain processing → Action → World consequences
4. **Enable** learning systems to adapt and improve

Start with the examples and gradually add complexity as you understand the system better. NLM is designed to grow with your needs, from simple neural simulation to complex artificial developmental systems.

---

## Additional Resources

- `docs/ARCHITECTURE.md` - Detailed system architecture
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment examples
- `easy_usage.md` - Basic usage patterns
- `tests/` - Test suite for functionality verification
- `examples/` - Additional usage examples (if available)
