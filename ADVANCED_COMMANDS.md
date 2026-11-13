# Advanced NLM Commands for Expert Users

This document provides advanced commands and techniques for experienced NLM users who want to maximize the potential of the neural simulation framework.

---

## Part 1: Advanced Brain Manipulation

### 1.1 Direct Neural Circuit Control

```python
import pynlm

# Create brain with many neurons for complex experiments
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # Large brain for experiments
config.set("brain.synapse_density", 0.05)  # Sparse connectivity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Access specific neural populations for targeted experiments
region_count = brain.getRegionCount()
for i in range(region_count):
    region_id = brain.getRegionIds()[i]
    region = brain.getRegion(region_id)
    print(f"Region {i}: {region_id.value}")

# Inject specific current patterns for experimental control
for step in range(100):
    # Create specific firing patterns
    brain.injectCurrent(NeuronId(42), 100.0)  # Strong excitation to neuron 42
    brain.step(step, step * 0.001)
```

### 1.2 Advanced Plasticity Control

```python
# Enable specific plasticity mechanisms
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure STDP parameters
config.set("stdp_ltp_weight", 0.01)  # Long-term potentiation
config.set("stdp_ltd_weight", 0.008)  # Long-term depression
config.set("stdp_tau", 20.0)  # Time constant

# Configure Hebbian learning
config.set("hebbian_threshold", 0.5)
config.set("hebbian_strength", 0.1)

# Configure structural plasticity
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)

# Run experiment with plasticity tracking
plasticity_history = []
for step in range(500):
    # ... normal simulation loop ...
    
    # Record plasticity state
    plasticity_history.append({
        'step': step,
        'synapse_count': brain.getTotalSynapseCount(),
        'structural_synapses': brain.getStructuralPlasticity()->getActiveSynapses(),
        'learning_rate': brain.getSTDP()->getLearningRate()
    })
```

## Part 2: Memory System Integration

### 2.1 Episodic Memory Implementation

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("memory.episodic.enable", True)
config.set("memory.associative.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run experience collection
episodes = []
for step in range(1000):
    world.update(0.001)
    
    # Create episode with rich information
    episode = {
        'timestamp': step,
        'sensory': world.getSensoryPercept(),
       �ล่र्ल object states
        'action': agent.decodeMotorCommand(),
        'outcome': world.applyMotorCommand(action, world.getSimulationTime()),
        'position': agent.getPosition(),  # Assuming this exists
        'neural_pattern': brain.getActiveNeurons(),  # Assuming this exists
        'reward': world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    }
    
    episodes.append(episode)
    
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Store episodes in episodic memory (for Phase 6 implementation)
for episode in episodes:
    brain.storeExperience(episode)
```

### 2.2 Working Memory Management

```python
# Create working memory with specific parameters
brain.enableWorkingMemory(True)

# Configure working memory capacity
brain.configureWorkingMemory(
    capacity=1000,  # Number of active traces
    decay_rate=0.1,  # Decay rate for old traces
    competition=True  # Enable trace competition
)

# Working memory integration example
for step in range(500):
    # Process sensory input into working memory
    agent.processSensoryInput(world.getSensoryPercept())
    
    # Update working memory with new information
    brain.updateWorkingMemory(
        sensory_pattern=percept.getVision(),
        reward=reward,
        action=action
    )
    
    brain.step(step)
    
    # Access working memory contents
    active_traces = brain.getWorkingMemoryTraces()
    for trace in active_traces:
        print(f"Trace: {trace.content}, strength: {trace.strength}")
```

## Part 3: Neuromodulation Control

### 3.1 Advanced Neuromodulator Configuration

```python
# Create configuration with custom neuromodulators
config = pynlm.createDefaultConfig()

# Dopamine (reward prediction)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.dopamine.baseline", 0.1)
config.set("neuromod.dopamine.adaptation", 0.01)

# Novelty detection
config.set("neuromod.novelty.threshold", 0.1)
config.set("neuromod.novelty.sensitivity", 0.5)
config.set("neuromod.novelty.decay", 0.9)

# Curiosity
config.set("neuromod.curiosity.baseline", 0.05)
config.set("neuromod.curiosity.sensing", 0.1)
config.set("neuromod.curiosity.exploration", 0.01)

# Prediction error
config.set("neuromod.error.threshold", 0.2)
config.set("neuromod.error.learning_rate", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Custom neuromodulation example
for step in range(300):
    # Compute novelty
    novelty = computeNovelty(percept, agent)
    
    # Compute prediction error
    prediction_error = computePredictionError(brain, percept)
    
    # Apply custom neuromodulation
    brain.applyNeuromodulation(novelty, prediction_error)
    
    # Apply reward-based learning
    agent.applyRewardModulation(reward, prediction_error)
```

### 3.2 Neuromodulator Diagnostics

```python
# Monitor neuromodulator levels
neurodiagnostic = agent.getNeuromodulatorLevels()

print(f"Dopamine level: {neurodiagnostic['dopamine']:.3f}")
print(f"Novelty level: {neurodiagnostic['novelty']:.3f}")
print(f"Curiosity level: {neurodiagnostic['curiosity']:.3f}")
print(f"Prediction error: {neurodiagnostic['prediction_error']:.3f}")

# Plot neuromodulator dynamics
import matplotlib.pyplot as plt

steps = list(range(len(neurodiagnostic['dopamine'])))

plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(steps, neurodiagnostic['dopamine'])
plt.title('Dopamine Level')
plt.xlabel('Step')
plt.ylabel('Level')

plt.subplot(2, 2, 2)
plt.plot(steps, neurodiagnostic['novelty'])
plt.title('Novelty Level')
plt.xlabel('Step')
plt.ylabel('Level')

plt.subplot(2, 2, 3)
plt.plot(steps, neurodiagnostic['curiosity'])
plt.title('Curiosity Level')
plt.xlabel('Step')
plt.ylabel('Level')

plt.subplot(2, 2, 4)
plt.plot(steps, neurodiagnostic['prediction_error'])
plt.title('Prediction Error')
plt.xlabel('Step')
plt.ylabel('Error')

plt.tight_layout()
plt.savefig('neuromodulator_dynamics.png')
plt.show()
```

## Part 4: Performance Optimization

### 4.1 Memory Management

```python
# Memory pool configuration for large-scale simulations
brain.configureMemoryPool(
    neuron_pool_size=5000,
    synapse_pool_size=20000,
    event_pool_size=10000,
    recurrent_pool_size=2000
)

# Memory-efficient simulation loop
for step in range(5000):
    # Reuse objects where possible
    brain.step(step)
    
    # Clear unused references
    if step % 100 == 0:
        brain.clearUnusedObjects()
        
    # Force garbage collection periodically
    if step % 500 == 0:
        import gc
        gc.collect()
```

### 4.2 Performance Monitoring

```python
import time
import tracemalloc

# Start performance monitoring
tracemalloc.start()
start_time = time.time()

# Run performance test
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

memory_snapshots = []
for step in range(1000):
    brain.step(step)
    
    # Take memory snapshot every 100 steps
    if step % 100 == 0:
        current, peak = tracemalloc.get_traced_memory()
        memory_snapshots.append({
            'step': step,
            'current_mb': current / 1024 / 1024,
            'peak_mb': peak / 1024 / 1024
        })

end_time = time.time()
tracemalloc.stop()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average memory usage: {sum(s['current_mb'] for s in memory_snapshots) / len(memory_snapshots):.2f} MB")

# Performance analysis
import pandas as pd

performance_df = pd.DataFrame(memory_snapshots)

plt.figure(figsize=(10, 6))
plt.plot(performance_df['step'], performance_df['current_mb'])
plt.title('Memory Usage Over Time')
plt.xlabel('Step')
plt.ylabel('Memory (MB)')
plt.grid(True)
plt.savefig('performance_analysis.png')
```

## Part 5: Advanced State Management

### 5.1 Custom Checkpointing

```python
# Advanced checkpoint with compression and metadata
checkpoint_config = {
    'compression': 'gzip',  # Options: none, gzip, zstd
    'version': '2.0',
    'include_metadata': True,
    'include_analysis': True,
    'min_disk_space': 1024 * 1024 * 100,  # 100MB minimum
    'auto_checkpoint': {
        'enabled': True,
        'interval': 1000,
        'max_checkpoints': 10,
        'backup_location': './checkpoints_backup/'
    }
}

# Custom checkpoint manager
checkpoint_manager = CheckpointManager(checkpoint_config)

# Create and manage checkpoints
for step in range(3000):
    brain.step(step)
    
    # Automatic checkpointing
    if checkpoint_manager.shouldCheckpoint(step):
        checkpoint_name = f"experiment_{step}"
        checkpoint_manager.createCheckpoint(checkpoint_name)
        
        # Store metadata
        checkpoint_manager.storeMetadata(checkpoint_name, {
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'reward': current_reward,
            'neuromod_level': agent.getNeuromodulationLevel()
        })

# Load specific checkpoint
best_checkpoint = checkpoint_manager.findBestCheckpoint(
    min_firing_rate=0.1,
    min_reward=50.0,
    max_steps=2000
)

if best_checkpoint:
    brain.load(best_checkpoint.path)
```

### 5.2 Parallel Processing

```python
# Enable parallel processing for large simulations
brain.configureParallelProcessing(
    neuron_parallel=True,
    synapse_parallel=True,
    plasticity_parallel=True,
    memory_parallel=True,
    num_threads=4  # CPU core count
)

# Parallel simulation loop
import concurrent.futures
import threading

# Thread-safe simulation function
def simulate_step(step, brain, world, agent):
    thread_id = threading.current_thread().ident
    step_start = time.time()
    
    world.update(0.001)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    step_duration = time.time() - step_start
    return {
        'step': step,
        'thread_id': thread_id,
        'duration': step_duration
    }

# Run simulation with parallel processing
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = []
    
    for step in range(2000):
        future = executor.submit(simulate_step, step, brain, world, agent)
        futures.append(future)
    
    # Collect results
    results = []
    for future in concurrent.futures.as_completed(futures):
        results.append(future.result())

# Analyze parallel performance
step_times = [r['duration'] for r in results]
print(f"Average step time: {sum(step_times) / len(step_times):.3f}s")
print(f"Max step time: {max(step_times):.3f}s")
print(f"Min step time: {min(step_times):.3f}s")
```

## Part 6: Advanced Analysis and Visualization

### 6.1 Network Analysis

```python
# Create network analysis from brain state
network_analysis = brain.createNetworkAnalysis()

# Compute network metrics
network_metrics = {
    'density': network_analysis.computeDensity(),
    'betweenessness': network_analysis.computeBetweenness(),
    'clustering': network_analysis.computeClustering(),
    'small_worldness': network_analysis.computeSmallWorldness(),
    'efficiency': network_analysis.computeEfficiency(),
    'robustness': network_analysis.computeRobustness()
}

# Visualize network
network_analysis.visualizeNetwork(
    layout='force_directed',
    threshold=0.5,
    visualize_neurons=True,
    visualize_synapses=True,
    highlight_active=True
)
```

### 6.2 Learning Analysis

```python
# Comprehensive learning analysis
learning_analyzer = LearningAnalyzer(brain)

# Analyze learning curves
learning_curves = {
    'firing_rate': learning_analyzer.getFiringRateCurve(),
    'synaptic_weight': learning_analyzer.getSynapticWeightChange(),
    'prediction_accuracy': learning_analyzer.getPredictionAccuracy(),
    'reward_efficiency': learning_analyzer.getRewardEfficiency()
}

# Plot learning analysis
fig, axes = plt.subplots(2, 3, figsize=(18, 12))

# Firing rate over time
axes[0, 0].plot(learning_curves['firing_rate'])
axes[0, 0].set_title('Firing Rate Over Time')
axes[0, 0].set_xlabel('Step')
axes[0, 0].set_ylabel('Rate (Hz)')

# Synaptic weight changes
axes[0, 1].plot(learning_curves['synaptic_weight'])
axes[0, 1].set_title('Synaptic Weight Changes')
axes[0, 1].set_xlabel('Step')
axes[0, 1].set_ylabel('Weight')

# Prediction accuracy
axes[0, 2].plot(learning_curves['prediction_accuracy'])
axes[0, 2].set_title('Prediction Accuracy')
axes[0, 2].set_xlabel('Step')
axes[0, 2].set_ylabel('Accuracy')

# Reward efficiency
axes[1, 0].plot(learning_curves['reward_efficiency'])
axes[1, 0].set_title('Reward Efficiency')
axes[1, 0].set_xlabel('Step')
axes[1, 0].set_ylabel('Efficiency')

# Learning speed
axes[1, 1].hist(step_times, bins=50)
axes[1, 1].set_title('Step Time Distribution')
axes[1, 1].set_xlabel('Time (s)')
axes[1, 1].set_ylabel('Frequency')

# Neuromodulator levels
axes[1, 2].plot(range(len(neuromod_levels)), neuromod_levels)
axes[1, 2].set_title('Neuromodulator Levels')
axes[1, 2].set_xlabel('Step')
axes[1, 2].set_ylabel('Level')

plt.tight_layout()
plt.savefig('advanced_learning_analysis.png')
```

## Part 7: Advanced Scripting and Automation

### 7.1 Batch Processing

```python
# Create automated batch processor
batch_processor = BatchProcessor()

# Define experiment parameters
experiments = [
    {'name': 'small_brain', 'neuron_count': 100, 'steps': 100},
    {'name': 'medium_brain', 'neuron_count': 1000, 'steps': 1000},
    {'name': 'large_brain', 'neuron_count': 5000, 'steps': 5000},
    {'name': 'complex_env', 'neuron_count': 2000, 'steps': 2000, 'world_size': (50, 50)}
]

# Run batch of experiments
batch_results = batch_processor.run_batch(experiments)

# Analyze batch results
for exp_name, result in batch_results.items():
    print(f"Experiment: {exp_name}")
    print(f"  Total reward: {result['total_reward']:.2f}")
    print(f"  Average firing rate: {result['avg_firing_rate']:.2f} Hz")
    print(f"  Final memory: {result['memory_episodes_stored']}")
    print(f"  Duration: {result['duration']:.2f}s")
    print()
```

### 7.2 Automated Experimentation

```python
# Create automated experimentation framework
experiment_manager = ExperimentManager()

# Define experiment pipeline
pipeline = ExperimentPipeline()

# Add stages
pipeline.addStage('initialization', initialize_brain)
pipeline.addStage(' exploration', run_exploration_phase)
pipeline.addStage(' learning', run_learning_phase)
pipeline.addStage(' evaluation', run_evaluation_phase)
pipeline.addStage(' analysis', analyze_results)

# Run automated experimentation
experiment_manager.runPipeline(pipeline, config)

# Real-time monitoring
monitor = RealTimeMonitor()

@monitor.track('brain_state')
def track_brain_state(step):
    return {
        'step': step,
        'neurons': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing': brain.getFiringNeuronCount(),
        'reward': current_reward,
        'curiosity': agent.getCuriosityLevel()
    }

@monitor.track('memory_usage')
def track_memory_usage(step):
    import psutil
    process = psutil.Process()
    return process.memory_info().rss / 1024 / 1024  # MB

# Run with monitoring
results = experiment_manager.runWithMonitoring(pipeline, config, monitor)
```

## Part 8: Integration with Scientific Tools

### 8.1 MATLAB Integration

```python
# Integrate with MATLAB for scientific analysis
import matlab.engine

# Start MATLAB engine
eng = matlab.engine.start_matlab()

# Export data for MATLAB analysis
brain.exportToMatlab('brain_state.mat')

# Run MATLAB analysis
eng.run('nlm_analysis_script(', 'brain_state.mat')

# Load MATLAB results
results = eng.load('analysis_results.mat')

# Close MATLAB engine
eng.quit()
```

### 8.2 Jupyter Integration

```python
# Create Jupyter notebook integration
from nlm.jupyter import create_interactive_dashboard

# Create dashboard for real-time monitoring
dashboard = create_interactive_dashboard(brain, agent, world)

# Add widgets
widgets = dashboard.add_widgets(
    brain_state_monitor,
    performance_monitor,
    learning_analyzer,
    neuromodulator_display
)

# Create interactive plots
plots = dashboard.create_plots(
    firing_rate_plot,
    reward_plot,
    neuromodulator_plot,
    network_analysis
)

# Display dashboard
dashboard.show()
```

## Part 9: Production Deployment

### 9.1 Docker Deployment

```dockerfile
# Dockerfile for NLM production deployment
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    python3 python3-pip \
    cmake \
    g++ \
    libomp-dev \
    git \
    wget

# Install Python dependencies
COPY requirements.txt .
RUN pip3 install -r requirements.txt

# Copy NLM source
COPY . /nlm
WORKDIR /nlm

# Build NLM
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j4

# Install Python module
RUN pip3 install .

# Create application user
RUN useradd -m -s /bin/bash nlmuser
USER nlmuser

# Set up directories
RUN mkdir -p /nlm/checkpoints /nlm/logs /nlm/models

# Copy configuration
COPY config/ /nlm/config/

# Set working directory
WORKDIR /nlm

# Run command
CMD ["python3", "-m", "nlm", "--config", "config/production.ini"]
```

### 9.2 Systemd Service

```ini
# /etc/systemd/system/nlm.service
[Unit]
Description=NLM Neural Learning Machine Service
After=network.target

[Service]
Type=simple
User=nlmuser
Group=nlmuser
WorkingDirectory=/nlm
ExecStart=/usr/local/bin/nlm --config /nlm/config/production.ini
Restart=always
RestartSec=30
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

# Enable and start service
sudo systemctl daemon-reload
sudo systemctl enable nlm.service
sudo systemctl start nlm.service
```

## Part 10: Troubleshooting and Support

### 10.1 Common Issues and Solutions

```python
# Troubleshooting guide

def troubleshoot_simulation(brain, agent, world, error_type):
    if error_type == 'memory_error':
        # Memory management issues
        brain.clearUnusedObjects()
        import gc
        gc.collect()
        return "Memory cleared and garbage collected"
        
    elif error_type == 'performance_error':
        # Performance optimization
        brain.enableParallelProcessing(True)
        return "Parallel processing enabled"
        
    elif error_type == 'plasticity_error':
        # Plasticity system issues
        brain.resetPlasticityState()
        return "Plasticity state reset"
        
    elif error_type == 'memory_error':
        # Memory system issues
        brain.resetMemorySystems()
        return "Memory systems reset"
        
    else:
        return f"Unknown error type: {error_type}"
```

### 10.2 Support Commands

```bash
# Support and debugging commands

# Check system status
nlm status

# View logs
nlm logs --follow

# Get performance metrics
nlm metrics --format json

# Restart service
sudo systemctl restart nlm.service

# Check service health
sudo systemctl is-active nlm.service

# Get service logs
sudo journalctl -u nlm.service -f

# Analyze performance
nlm profile --output profile.html

# Export data
nlm export --format mat --output analysis.mat

# Run validation tests
nlm validate --suite comprehensive
```

## Conclusion

These advanced commands and techniques enable expert users to:

1. **Maximize performance** through parallel processing and memory optimization
2. **Analyze deeply** through comprehensive network and learning analysis
3. **Automate experimentation** through batch processing and pipelines
4. **Deploy in production** through Docker and systemd integration
5. **Integrate with scientific tools** through MATLAB and Jupyter support
6. **Troubleshoot effectively** through comprehensive monitoring and logging

The NLM framework provides extensive capabilities for advanced users who need sophisticated neural simulation capabilities for research and production applications.