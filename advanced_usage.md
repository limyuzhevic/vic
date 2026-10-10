# NLM - Advanced Usage Guide

This guide covers advanced features and configurations for the NLM (Neural Learning Machine) project. It assumes familiarity with the basic NLM concepts and Python API.

## Advanced Configuration

### Environment-Based Configuration

Set environment variables to configure NLM without files or command-line arguments:

```bash
export NLM_RANDOM_SEED=12345
export NLM_NEURON_COUNT=2000
export NLM_SIMULATION_TIMESTEP=0.0005
export NLM_PLATIVITY_LEARNING_RATE=0.02
export NLM_STDP_LTP_WEIGHT=0.025
```

In Python:
```python
import os
import pynlm

# Load configuration from environment
config = pynlm.createAdvancedConfig()

# Access environment-based settings
print(f"Random seed: {config.getOr('random_seed', 42)}")
print(f"Neuron count: {config.getOr('neuron_count', 1000)}")
```

### Configuration Validation

Validate your configuration before use:

```python
import pynlm

# Create and validate configuration
config = pynlm.createAdvancedConfig()
config.set("neuron_count", 2000)
config.set("plasticity_learning_rate", 0.02)

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration has errors")

# Get validation errors
errors = config.getErrors()
for error in errors:
    print(f"Error: {error}")
```

### Saving and Loading Configurations

Save complex configurations to files:

```python
import pynlm
import os

# Create advanced configuration
config = pynlm.createAdvancedConfig()
config.set("random_seed", 12345)
config.set("neuron_count", 2000)
config.set("plasticity_learning_rate", 0.02)
config.set("stdp_ltp_weight", 0.025)
config.set("stdp_ltd_weight", 0.024)
config.set("environment_name", "GridWorld")
config.set("log_level", "DEBUG")

# Save to file
config.saveToFile("my_experiment.cfg")
print("Configuration saved to my_experiment.cfg")

# Load from file
config2 = pynlm.createAdvancedConfig()
config2.loadFromFile("my_experiment.cfg")
print(f"Loaded {len(config2.getKeys())} configuration values")
```

## Performance Monitoring

### Brain Performance Metrics Dashboard

Monitor brain performance in real-time:

```python
import pynlm
import time

# Create brain and agent
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Create performance metrics dashboard
metrics = pynlm.createPerformanceMetrics()

# Run simulation with performance monitoring
for step in range(100):
    brain.step(step)
    
    # Update performance metrics
    metrics.update(step, time.time())
    
    if step % 10 == 0:
        # Get formatted performance summary
        summary = metrics.getSummary()
        print(f"Step {step}:")
        print(summary)
        print("-" * 50)

# Get final performance statistics
memory_usage = metrics.getMemoryUsage()
spike_rate = metrics.getFiringRate()
avg_firing = metrics.getAverageFiringRate()
print(f"Memory usage: {memory_usage} bytes")
print(f"Average firing rate: {avg_firing} Hz")
```

### Real-Time Monitoring

Track multiple aspects of brain performance:

```python
import pynlm
import threading
import time

class RealTimeMonitor:
    def __init__(self, brain, interval=1.0):
        self.brain = brain
        self.interval = interval
        self.running = True
        self.metrics_history = []
        
    def start(self):
        thread = threading.Thread(target=self.monitor_loop)
        thread.daemon = True
        thread.start()
        return thread
        
    def monitor_loop(self):
        while self.running:
            snapshot = {
                'step': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'memory_usage': self.brain.getMemoryUsage(),
                'cpu_usage': self.getCpuUsage(),
                'timestamp': time.time()
            }
            self.metrics_history.append(snapshot)
            time.sleep(self.interval)
            
    def stop(self):
        self.running = False
        
    def getCpuUsage(self):
        # Placeholder for actual CPU usage calculation
        return 0.0
        
    def analyzePerformance(self):
        if not self.metrics_history:
            return {}
            
        spikes = [m['step'] for m in self.metrics_history]
        rates = [m['firing_rate'] for m in self.metrics_history]
        
        return {
            'total_spikes': sum(spikes),
            'average_firing_rate': sum(rates) / len(rates),
            'peak_firing_rate': max(rates),
            'stability': self.calculateStability(rates)
        }

# Usage
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

monitor = RealTimeMonitor(brain, interval=0.5)
monitor.start()

# Run some work
for step in range(50):
    brain.step(step)
    time.sleep(0.1)

monitor.stop()
performance = monitor.analyzePerformance()
print("Performance Analysis:", performance)
```

## Serialization and Checkpointing

### Save Brain State (Checkpointing)

Save brain state for later restoration:

```python
import pynlm
import os

# Create and run brain
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Run for a while to create interesting state
for step in range(1000):
    brain.step(step)

# Save checkpoint
checkpoint_file = "brain_checkpoint.npkl"
brain.save(checkpoint_file)
print(f"Brain checkpoint saved to {checkpoint_file}")

# Verify checkpoint was created
if os.path.exists(checkpoint_file):
    print(f"Checkpoint size: {os.path.getsize(checkpoint_file)} bytes")
```

### Load Brain State (Resume from Checkpoint)

Continue work from a saved checkpoint:

```python
import pynlm
import os

# Create brain with same configuration
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check if checkpoint exists
checkpoint_file = "brain_checkpoint.npkl"
if os.path.exists(checkpoint_file):
    print("Loading brain from checkpoint...")
    brain.load(checkpoint_file)
    print("Checkpoint loaded successfully")
    
    # Verify loaded state
    print(f"Total spikes after load: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
else:
    print("No checkpoint found, starting fresh")
```

### Experiment State Management

Save and restore complete experiment states:

```python
import pynlm
import json
import hashlib

class ExperimentManager:
    def __init__(self, name):
        self.name = name
        self.checkpoint_dir = f"experiments/{name}/checkpoints"
        os.makedirs(self.checkpoint_dir, exist_ok=True)
        
    def save_experiment_state(self, brain, world, agent, step):
        state = {
            'step': step,
            'brain_state': brain.save(),
            'world_state': world.getState(),
            'agent_state': agent.getState(),
            'brain_config': brain.getConfig().summary(),
            'timestamp': time.time()
        }
        
        # Save JSON metadata
        metadata_file = f"{self.checkpoint_dir}/checkpoint_{step:06d}.json"
        with open(metadata_file, 'w') as f:
            json.dump(state, f, indent=2)
            
        print(f"Experiment state saved at step {step}")
        
    def load_experiment_state(self, brain, world, agent, checkpoint_path):
        with open(checkpoint_path, 'r') as f:
            state = json.load(f)
            
        brain.load(state['brain_state'])
        world.setState(state['world_state'])
        agent.setState(state['agent_state'])
        
        print(f"Experiment state loaded from {checkpoint_path}")
        return state['step']
        
    def list_checkpoints(self):
        checkpoints = []
        for file in os.listdir(self.checkpoint_dir):
            if file.endswith('.json'):
                checkpoint_num = int(file.split('_')[1].split('.')[0])
                checkpoints.append(checkpoint_num)
        return sorted(checkpoints)

# Usage
experiment = ExperimentManager("my_experiment")

# Run simulation and save checkpoints periodically
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

for step in range(5000):
    # Run simulation step...
    
    # Save checkpoint every 500 steps
    if step % 500 == 0:
        experiment.save_experiment_state(brain, world, agent, step)

# Later, resume from checkpoint
if experiment.list_checkpoints():
    latest_checkpoint = max(experiment.list_checkpoints())
    experiment.load_experiment_state(brain, world, agent, 
                                     f"experiments/my_experiment/checkpoints/checkpoint_{latest_checkpoint:06d}.json")
```

## Batch Processing

### Batch Simulation with Multiple Inputs

Process multiple sensory inputs in batch:

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Create batch of sensory inputs
inputs = []
for i in range(100):
    # Create random sensory percepts
    percept = pynlm.SensoryPercept()
    # Fill with random data
    percept.setVision([float(j % 256) / 255.0 for j in range(256)])
    percept.setTouch([float(j % 8) / 7.0 for j in range(8)])
    percept.setInternal([float(j % 4) / 3.0 for j in range(4)])
    percept.setProprioception([float(j % 6) / 5.0 for j in range(6)])
    percept.setTimestamp(i * 0.1)
    inputs.append(percept)

# Create batch of expected rewards
rewards = [i * 0.1 for i in range(100)]  # Linearly increasing rewards

# Run batch simulation
print("Running batch simulation...")
start_time = time.time()

# Run batch processing
results = brain.runBatchSimulation(inputs, rewards, timestep=0.1, steps=100)

end_time = time.time()
print(f"Batch simulation completed in {end_time - start_time:.2f} seconds")
print(f"Total results: {len(results)}")

# Analyze batch results
if results:
    final_result = results[-1]
    print(f"Final reward: {final_result.reward}")
    print(f"Success rate: {sum(1 for r in results if r.success) / len(results) * 100:.1f}%")
    print(f"Average error: {sum(r.error for r in results) / len(results):.3f}")
```

### Parallel Processing

Process multiple simulations in parallel:

```python
import pynlm
import threading
import concurrent.futures

class ParallelSimulator:
    def __init__(self, config):
        self.config = config
        
    def run_single_simulation(self, simulation_id, steps):
        """Run a single simulation"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
            
        return {
            'id': simulation_id,
            'steps': steps,
            'spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'final_stage': brain.getDevelopmentalStage()
        }
        
    def run_parallel_simulations(self, num_simulations, steps_per_sim, max_workers=4):
        """Run multiple simulations in parallel"""
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            # Submit all simulations
            futures = []
            for i in range(num_simulations):
                future = executor.submit(self.run_single_simulation, i, steps_per_sim)
                futures.append(future)
                
            # Collect results
            results = []
            for future in concurrent.futures.as_completed(futures):
                results.append(future.result())
                
            return results

# Usage
config = pynlm.createAdvancedConfig()
simulator = ParallelSimulator(config)

print("Running parallel simulations...")
start_time = time.time()

results = simulator.run_parallel_simulations(
    num_simulations=10,
    steps_per_sim=1000,
    max_workers=4
)

end_time = time.time()
print(f"Parallel simulations completed in {end_time - start_time:.2f} seconds")
print(f"Results: {len(results)} simulations")

# Analyze parallel results
total_spikes = sum(r['spikes'] for r in results)
avg_firing_rate = sum(r['firing_rate'] for r in results) / len(results)
print(f"Total spikes across all simulations: {total_spikes}")
print(f"Average firing rate: {avg_firing_rate:.2f} Hz")

# Find best performing simulation
best_simulation = max(results, key=lambda x: x['firing_rate'])
print(f"Best performing simulation: ID {best_simulation['id']} "
      f"with firing rate {best_simulation['firing_rate']:.2f} Hz")
```

## Advanced Features

### Custom Learning Rules

Configure custom learning rules:

```python
import pynlm

# Create learning rule configuration
learning_rule = pynlm.createLearningRule()
learning_rule.type = "custom_stdp"
learning_rule.parameters["lr_pre_post"] = 0.01
learning_rule.parameters["lr_post_pre"] = 0.012
learning_rule.parameters["tau"] = 20.0
learning_rule.parameters["weight_min"] = 0.0
learning_rule.parameters["weight_max"] = 1.0

# Validate learning rule
if learning_rule.validate():
    print("Learning rule configuration is valid")
    
    # Use learning rule in brain
    brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    brain.initialize()
    
    # Apply learning rule
    brain.setLearningRule(learning_rule)
    print("Custom learning rule applied")
else:
    print("Learning rule configuration has errors")
    for error in learning_rule.getErrors():
        print(f"  Error: {error}")
```

### Learning Rate Tuning

Dynamically adjust learning rates:

```python
import pynlm

class LearningRateScheduler:
    def __init__(self, initial_rate, schedule_type="exponential"):
        self.current_rate = initial_rate
        self.schedule_type = schedule_type
        self.step_count = 0
        
    def get_rate(self):
        return self.current_rate
        
    def update_rate(self):
        self.step_count += 1
        
        if self.schedule_type == "exponential":
            # Exponential decay
            self.current_rate = 0.01 * (0.999 ** self.step_count)
        elif self.schedule_type == "adaptive":
            # Adaptive based on performance
            # Placeholder for actual performance-based adaptation
            self.current_rate = 0.01 * (0.95 ** (self.step_count // 100))
        elif self.schedule_type == "cosine":
            # Cosine annealing
            import math
            self.current_rate = 0.01 * (0.5 * (1 + math.cos(math.pi * self.step_count / 10000)))
            
    def configure_brain(self, brain, agent):
        brain.setLearningRate(self.current_rate)

# Usage
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

scheduler = LearningRateScheduler(initial_rate=0.01, schedule_type="cosine")

for step in range(1000):
    # Run simulation step...
    
    # Update learning rate
    scheduler.update_rate()
    scheduler.configure_brain(brain, agent)
    
    if step % 100 == 0:
        print(f"Step {step}: Learning rate = {scheduler.get_rate():.6f}")
```

### Architecture Customization

Customize brain architecture:

```python
import pynlm

# Create architecture configuration
arch_config = pynlm.createArchitectureConfig()

# Configure regions
arch_config.addRegion("sensory", 512)  # 512 sensory neurons
arch_config.addRegion("motor", 64)    # 64 motor neurons
arch_config.addRegion("hidden", 256)  # 256 hidden neurons

arch_config.neuronCount = 832  # Total neurons
arch_config.regionCount = 3     # Total regions
arch_config.connectionProbability = 0.15
arch_config.regionTypes = ["sensory", "hidden", "motor"]

# Validate architecture
if arch_config.validate():
    print("Architecture configuration is valid")
    print(f"Total neurons: {arch_config.neuronCount}")
    print(f"Regions: {arch_config.regionCount}")
    print(f"Connection probability: {arch_config.connectionProbability}")
else:
    print("Architecture configuration has errors")
    for error in arch_config.getErrors():
        print(f"  Error: {error}")
```

### Experiment Management

Create and manage complex experiments:

```python
import pynlm

class ExperimentManager:
    def __init__(self, name):
        self.name = name
        self.config = pynlm.createAdvancedConfig()
        self.experiment = pynlm.createExperiment(name, self.config)
        
    def run_experiment(self, steps):
        """Run an experiment"""
        print(f"Running experiment '{self.name}' for {steps} steps...")
        
        # Configure experiment parameters
        self.experiment.addParameter("steps", steps)
        self.experiment.addParameter("learning_rate", 0.01)
        self.experiment.addParameter("neuron_count", 1000)
        
        # Run experiment
        self.experiment.run(steps)
        return self.experiment.getResults()
        
    def compare_experiments(self, experiment_configs):
        """Compare multiple experiments"""
        results = []
        
        for exp_config in experiment_configs:
            print(f"Running experiment: {exp_config['name']}")
            
            # Create experiment with specific configuration
            experiment = pynlm.createExperiment(exp_config['name'], 
                                              pynlm.createAdvancedConfig())
            
            # Configure experiment
            for key, value in exp_config['parameters'].items():
                experiment.addParameter(key, value)
                
            # Run experiment
            result = experiment.run(exp_config['steps'])
            results.append(result)
            
        return self.analyze_experiment_results(results)
        
    def analyze_experiment_results(self, results):
        """Analyze and compare experiment results"""
        analysis = {}
        
        for result in results:
            exp_name = result['name']
            analysis[exp_name] = {
                'final_performance': result['final_performance'],
                'learning_efficiency': result['learning_efficiency'],
                'stability': result['stability'],
                'convergence_rate': result['convergence_rate']
            }
            
        # Rank experiments by performance
        ranked = sorted(analysis.items(), 
                       key=lambda x: x[1]['final_performance'], 
                       reverse=True)
        
        print("Experiment Rankings:")
        for i, (name, metrics) in enumerate(ranked):
            print(f"  {i+1}. {name}: {metrics['final_performance']:.3f}")
            
        return analysis

# Usage
experiment_manager = ExperimentManager("learning_comparison")

# Compare different configurations
experiment_configs = [
    {
        'name': 'baseline',
        'parameters': {'learning_rate': 0.01, 'neuron_count': 1000},
        'steps': 1000
    },
    {
        'name': 'high_plasticity',
        'parameters': {'learning_rate': 0.05, 'neuron_count': 2000},
        'steps': 1000
    },
    {
        'name': 'fast_learning',
        'parameters': {'learning_rate': 0.02, 'neuron_count': 500},
        'steps': 1000
    }
]

results = experiment_manager.compare_experiments(experiment_configs)
```

## Troubleshooting

### Common Issues and Solutions

#### "Brain not learning" 

**Problem:** Brain shows no significant learning over time.

**Solutions:**
1. Check plasticity is enabled: `agent.enableStructuralPlasticity(True)`
2. Verify learning rate is appropriate: `brain.setLearningRate(0.01)`
3. Ensure sensory input is connected to brain: `agent.processSensoryInput(percept)`
4. Check developmental stage: `brain.getDevelopmentalStage()`
5. Enable reward modulation if using rewards: `agent.enableRewardModulation(True)`

**Debug Code:**
```python
def debug_learning(brain, agent, world):
    print("=== Learning Debug Info ===")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
    print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
    print(f"Current firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    
    # Check synaptic changes
    if agent.isStructuralPlasticityEnabled():
        print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

#### "Agent not taking actions"

**Problem:** Agent produces no motor commands or doesn't act.

**Solutions:**
1. Ensure sensory processing is happening: `agent.processSensoryInput(percept)`
2. Check motor decoding threshold: `agent.decodeMotorCommand()`
3. Verify sensory neurons are receiving input
4. Check if brain is firing: `brain.getFiringNeuronCount()`
5. Enable curiosity for exploration: `agent.enableCuriosity(True)`

**Debug Code:**
```python
def debug_agent_actions(agent, brain):
    print("=== Agent Debug Info ===")
    print(f"Sensory input size: {agent.getSensoryInputSize()}")
    print(f"Motor output size: {agent.getMotorOutputSize()}")
    
    decoded_action = agent.decodeMotorCommand()
    print(f"Decoded action: {decoded_action}")
    
    print(f"Brain firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total brain spikes: {brain.getTotalSpikeCount()}")
    
    # Check motor neuron activity
    motor_activity = agent.getMotorNeuronActivity()
    print(f"Motor activity: {motor_activity}")
```

#### "Memory usage growing" 

**Problem:** Simulation memory usage increases over time.

**Solutions:**
1. Enable memory monitoring
2. Check if brain is being reset periodically
3. Verify no memory leaks in custom code
4. Monitor performance metrics

**Debug Code:**
```python
def monitor_memory_usage(brain, steps_interval=100):
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    for step in range(0, 10000, steps_interval):
        memory_info = process.memory_info()
        print(f"Step {step}: RSS = {memory_info.rss / 1024 / 1024:.1f} MB, "
              f"VMS = {memory_info.vms / 1024 / 1024:.1f} MB")
        
        if step % 1000 == 0:
            brain.step(step)
```

#### "Checkpoint file not loading"

**Problem:** Cannot load saved checkpoint files.

**Solutions:**
1. Verify file path is correct
2. Check file format and version compatibility
3. Ensure brain configuration matches checkpoint
4. Verify file permissions

**Debug Code:**
```python
def debug_checkpoint(checkpoint_path, config):
    print(f"=== Checkpoint Debug Info ===")
    print(f"Checkpoint file: {checkpoint_path}")
    print(f"File exists: {os.path.exists(checkpoint_path)}")
    print(f"File size: {os.path.getsize(checkpoint_path) if os.path.exists(checkpoint_path) else 0} bytes")
    
    if os.path.exists(checkpoint_path):
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
            brain.load(checkpoint_path)
            print("Checkpoint loaded successfully")
            print(f"Loaded state - Steps: {brain.getTotalSpikeCount()}, "
                  f"Stage: {brain.getDevelopmentalStage()}")
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
```

### Performance Optimization Tips

#### Optimize for Speed
1. **Use batch processing** instead of step-by-step processing
2. **Reduce simulation timestep** if precision allows
3. **Enable performance optimizations** in build configuration
4. **Use appropriate brain size** for your task
5. **Monitor and optimize memory usage**

#### Optimize for Memory
1. **Use checkpointing** to save brain state
2. **Reset brain periodically** if not needed continuously
3. **Monitor memory usage** regularly
4. **Use efficient data structures**
5. **Avoid unnecessary copies** of large data structures

#### Optimize for Learning
1. **Tune learning rates** for your specific task
2. **Use appropriate plasticity rules**
3. **Balance exploration vs. exploitation**
4. **Monitor learning progress**
5. **Use appropriate brain architecture**

### Integration with External Tools

#### Using with Visualization Tools
```python
import pynlm

# Create brain with visualization enabled
config = pynlm.createAdvancedConfig()
config.set("visualization_enabled", True)
config.set("visualization_update_rate", 30.0)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Get visualization data if available
    if brain.hasVisualization():
        visualization_data = brain.getVisualizationData()
        # Send to external visualization tool
        visualize_brain_state(visualization_data)
```

#### Using with Logging Systems
```python
import pynlm
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger('nlm')

# Create brain with logging
config = pynlm.createAdvancedConfig()
config.set("log_level", "DEBUG")
config.set("log_to_file", True)
config.set("log_filename", "nlm_simulation.log")

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with detailed logging
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        logger.info(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f} Hz")
```

## Further Reading

### Key Papers and Resources
1. **Spiking Neural Networks** - Gerstner et al.
2. **Neural Engineering** - Masquelier et al.
3. **Online Learning in Neural Networks** - Lang and Hinton
4. **Spatio-Temporal Dynamics in the Brain** - Rao and Ballard
5. **Neural Plasticity and Learning** - Földiák

### Advanced Topics
1. **STDP Variants** - Different spike-timing-dependent plasticity rules
2. **Neuromodulation** - Dopamine, serotonin, and other neuromodulators
3. **Structural Plasticity** - Growing and pruning synapses
4. **Development** - Stage-based brain development
5. **Curiosity and Exploration** - Intrinsic motivation systems

### Research Applications
1. **Motor Control** - Learning to control prosthetic devices
2. **Sensory Processing** - Pattern recognition and classification
3. **Memory Systems** - Short-term and long-term memory
4. **Decision Making** - Value-based and probabilistic decision making
5. **Social Learning** - Learning from others and environments

This guide provides comprehensive coverage of advanced NLM features. Start with the basics and gradually explore more complex features as you become comfortable with the system.