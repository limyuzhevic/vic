# NLM (Neural Learning Machine) - Advanced User Guide

This guide provides advanced examples and comprehensive information for power users working with the NLM neural simulation framework.

## Table of Contents

1. [Advanced Configuration](#advanced-configuration)
2. [Performance Optimization](#performance-optimization)
3. [Advanced Brain Manipulation](#advanced-brain-manipulation)
4. [Memory Management](#memory-management)
5. [Debugging and Testing](#debugging-and-testing)
6. [Batch Processing](#batch-processing)
7. [Model Persistence](#model-persistence)
8. [Integration with External Systems](#integration-with-external-systems)

## Advanced Configuration

### Custom Configuration Files

Create custom configuration files for complex experiments:

```bash
# Create advanced config
cat > advanced_config.cfg << EOF
# High-density neural network
brain.neuron_count = 5000
brain.region_count = 3
brain.connection_probability = 0.2
brain.initial_weight_mean = 0.3
brain.initial_weight_std = 0.05

# Enhanced plasticity
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.005
plasticity.stdp.tau_plus = 25.0
plasticity.stdp.tau_minus = 25.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.0005
plasticity.structural.pruning_rate = 0.00005

# Neuromodulation
neuromod.dopamine.scale = 1.5
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Memory systems
brain.working_memory.capacity = 500
brain.episodic_memory.max_episodes = 5000
brain.associative_memory.capacity = 1000

# Development
brain.development.enabled = true
brain.development.plasticity_factor = 1.2
EOF

# Run with custom config
./nlm --config=advanced_config.cfg --set=neuron_count=5000 --benchmark
```

### Configuration Templates

Create reusable configuration templates for common experiments:

```bash
# Create templates directory
mkdir -p templates

cat > templates/sensory_motor.cfg << EOF
brain.neuron_count = 2000
brain.region_count = 2
brain.connection_probability = 0.1

# Sensory and motor regions
# Region 1: Sensory
# Region 2: Motor

# Enhanced sensory processing
plasticity.stdp.enable = true
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Motor learning
plasticity.hebbian.enable = true
brain.development.enabled = true
EOF

cat > templates/cognitive_learning.cfg << EOF
brain.neuron_count = 3000
brain.region_count = 3
brain.connection_probability = 0.15

# Cognitive regions
# Region 1: Perception
# Region 2: Working Memory
# Region 3: Action Planning

# Strong plasticity for learning
plasticity.stdp.learning_rate = 0.01
plasticity.hebbian.learning_rate = 0.01
plasticity.structural.enable = true

# Advanced memory systems
brain.working_memory.capacity = 800
brain.episodic_memory.max_episodes = 10000
brain.associative_memory.capacity = 2000

# Curiosity-driven exploration
neuromod.curiosity.enable = true
neuromod.curiosity.threshold = 0.3
neuromod.novelty.enable = true
neuromod.novelty.decay_rate = 0.98
EOF
```

## Performance Optimization

### High-Performance Configuration

```bash
# Performance-optimized configuration
cat > high_perf_config.cfg << EOF
brain.neuron_count = 1000  # Balanced for performance
brain.region_count = 1     # Single region for cache efficiency
brain.connection_probability = 0.08

# Optimized plasticity
plasticity.stdp.learning_rate = 0.002
plasticity.stdp.tau_plus = 15.0
plasticity.stdp.tau_minus = 15.0
plasticity.hebbian.enable = false  # Disable for speed
plasticity.structural.enable = false  # Disable for speed

# Memory optimization
brain.working_memory.capacity = 100
brain.episodic_memory.max_episodes = 1000
brain.associative_memory.capacity = 200

# Development optimization
brain.development.enabled = false  # Disable development for speed

# Fast simulation
timestep = 0.0005  # Smaller timestep for accuracy
EOF

# Run optimized
./nlm --config=high_perf_config.cfg --benchmark
```

### Performance Monitoring

Create a performance monitoring script:

```bash
#!/bin/bash
# perf_monitor.sh - Monitor NLM performance

BENCHMARKS=(
    "100 neurons, 1000 steps"
    "500 neurons, 1000 steps" 
    "1000 neurons, 500 steps"
    "2000 neurons, 200 steps"
)

for benchmark in "${BENCHMARKS[@]}"; do
    echo "Running benchmark: $benchmark"
    echo "==========================="
    
    # Parse benchmark parameters
    IFS=', ' read neurons steps <<< "$benchmark"
    
    # Run with timing
    start_time=$(date +%s.%N)
    ./nlm --set=neuron_count=$neurons --benchmark
    end_time=$(date +%s.%N)
    
    duration=$(echo "$end_time - $start_time" | bc)
    echo "Duration: $duration seconds"
    echo ""
    
    sleep 1

done
```

## Advanced Brain Manipulation

### Brain Configuration at Runtime

```python
import pynlm

def advanced_brain_setup():
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for specific experiment
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("brain.working_memory.capacity", 500)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Advanced configuration
    brain.getSTDP()->configure(0.01f, 0.012f, 20.0f)
    brain.getHebbian()->configure(0.02f, 0.01f)
    
    # Configure memory systems
    brain.getWorkingMemory()->setCapacity(500)
    brain.getEpisodicMemory()->setMaxEpisodes(5000)
    brain.getAssociativeMemory()->setCapacity(1000)
    
    return brain

def complex_experiment(brain, world, agent, steps):
    # Advanced experiment with detailed control
    config = brain.getConfig()
    
    # Initial sensory processing
    initial_percept = world.getSensoryPercept()
    agent.processSensoryInput(initial_percept)
    
    # Phase 1: Exploration
    for step in range(steps // 3):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Record baseline metrics
        if step % 100 == 0:
            print(f"Exploration Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    # Phase 2: Structured learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    for step in range(steps // 3, steps // 2):
        world.update(0.1)
        
        # Get percept and apply reward
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Calculate reward based on performance
        reward = calculate_performance_reward(brain, percept)
        predicted_reward = agent.getExpectedReward()
        agent.applyRewardModulation(reward, predicted_reward)
        
        brain.step(step)
        
        # Apply development
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Learning Step {step}:")
            print(f"  Reward: {reward:.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
    
    # Phase 3: Consolidation
    agent.enableCuriosity(True)
    
    for step in range(steps // 2, steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Curiosity-driven exploration
        if agent.getCuriosityLevel() > 0.5:
            # Random action for exploration
            action = agent.decodeMotorCommand()
            # Force exploration action
            action = pynlm.ActionType.MoveForward  # Example
        else:
            action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Consolidation Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Working memory: {brain.getWorkingMemory()->getActiveTraces()}")
    
    return brain

def calculate_performance_reward(brain, percept):
    # Custom reward calculation based on brain state and percept
    firing_rate = brain.getAverageFiringRate()
    spike_count = brain.getTotalSpikeCount()
    
    # Reward for moderate activity
    if firing_rate > 0.5 && firing_rate < 2.0:
        return 1.0 + (firing_rate - 0.5) * 0.5
    elif firing_rate >= 2.0:
        return 0.5  # Penalize too much activity
    else:
        return 0.1  # Low reward for inactivity
```

## Memory Management

### Working Memory Control

```python
import pynlm

def manage_working_memory(brain, target_utilization):
    """Advanced working memory management"""
    working_memory = brain.getWorkingMemory()
    
    # Get current utilization
    current = working_memory.getActiveTraces()
    total_capacity = working_memory.getCapacity()
    utilization = current / total_capacity
    
    if utilization > target_utilization:
        # Compress memory traces
        working_memory.compress(0.5)
        print(f"Compressed working memory: {current} -> {working_memory.getActiveTraces()}")
    
    elif utilization < target_utilization * 0.5:
        # Expand memory capacity
        new_capacity = int(total_capacity * 1.2)
        working_memory.setCapacity(new_capacity)
        print(f"Expanded working memory capacity: {total_capacity} -> {new_capacity}")
    
    return working_memory

def episodic_memory_consolidation(brain, consolidation_threshold):
    """Advanced episodic memory consolidation"""
    episodic_memory = brain.getEpisodicMemory()
    
    # Get statistics
    episode_count = episodic_memory.getEpisodeCount()
    replay_count = episodic_memory.getReplayCount()
    
    if episode_count > consolidation_threshold:
        # Consolidate important memories
        important_episodes = episodic_memory.getImportantEpisodes(0.7)
        for episode in important_episodes:
            episodic_memory.consolidate(episode, 1.0)
        
        print(f"Consolidated {len(important_episodes)} important episodes")
    
    # Schedule replay of recent episodes
    recent_episodes = episodic_memory.getRecentEpisodes(100)
    episodic_memory.scheduleReplay(recent_episodes)
    
    return episodic_memory
```

## Debugging and Testing

### Advanced Debugging Tools

```bash
#!/bin/bash
# debug_nlm.sh - Advanced debugging tools for NLM

# Enable core dump for crash debugging
core_settings="
set hardware
pid unlimited
set threads unlimited
"

echo "$core_settings" | sudo tee /proc/sys/kernel/core_pattern > /dev/null
sudo ulimit -c unlimited

# Run with debugging symbols
./nlm --set=neuron_count=100 --benchmark --verbose 2>&1 | tee debug_output.txt

# Analyze output for errors
grep -E "(ERROR|WARNING|ASSERT)" debug_output.txt > error_log.txt

if [ -s error_log.txt ]; then
    echo "Errors found:"
    cat error_log.txt
else
    echo "No errors found in output"
fi

# Memory debugging
valgrind --leak-check=full --track-origins=yes ./nlm --set=neuron_count=50
```

### Performance Profiling

```python
import pynlm
import time
import matplotlib.pyplot as plt

def profile_simulation(brain, steps):
    """Profile simulation performance and resource usage"""
    times = []
    spike_counts = []
    memory_usage = []
    
    for step in range(steps):
        start_time = time.time()
        
        brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        times.append(elapsed)
        spike_counts.append(brain.getTotalSpikeCount())
        
        # Get memory usage if available
        if hasattr(brain, 'getMemoryUsage'):
            memory_usage.append(brain.getMemoryUsage())
        
        if step % 100 == 0:
            print(f"Step {step}: {elapsed:.4f}s, "
                  f"spikes: {spike_counts[-1]}, "
                  f"memory: {memory_usage[-1] if memory_usage else 'N/A'}MB")
    
    # Performance analysis
    avg_time = sum(times) / len(times)
    max_time = max(times)
    min_time = min(times)
    
    print(f"\nPerformance Summary:")
    print(f"  Average step time: {avg_time:.4f}s")
    print(f"  Max step time: {max_time:.4f}s")
    print(f"  Min step time: {min_time:.4f}s")
    print(f"  Steps per second: {1.0/avg_time:.1f}")
    
    # Plot performance
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 2, 1)
    plt.plot(times)
    plt.title('Step Time (ms)')
    plt.ylabel('Time (ms)')
    
    plt.subplot(2, 2, 2)
    plt.plot(spike_counts)
    plt.title('Cumulative Spike Count')
    plt.ylabel('Total Spikes')
    
    if memory_usage:
        plt.subplot(2, 2, 3)
        plt.plot(memory_usage)
        plt.title('Memory Usage (MB)')
        plt.ylabel('Memory (MB)')
    
    plt.subplot(2, 2, 4)
    plt.hist(times, bins=50, alpha=0.7)
    plt.title('Step Time Distribution')
    plt.xlabel('Time (ms)')
    plt.ylabel('Frequency')
    
    plt.tight_layout()
    plt.savefig('performance_profile.png')
    plt.show()
    
    return {
        'avg_time': avg_time,
        'max_time': max_time,
        'min_time': min_time,
        'steps_per_second': 1.0/avg_time,
        'total_spikes': spike_counts[-1],
        'times': times
    }
```

## Batch Processing

### Automated Experiment Batches

```bash
#!/bin/bash
# run_batch.sh - Run automated experiment batches

EXPERIMENTS=(
    "baseline,100,1000,0.1,0.01"
    "high_plasticity,500,2000,0.2,0.05"
    "low_plasticity,1000,1000,0.05,0.001"
    "curiosity_driven,2000,500,0.15,0.02"
    "development,3000,2000,0.1,0.03"
)

RESULTS_DIR="batch_results"
mkdir -p "$RESULTS_DIR"

for experiment in "${EXPERIMENTS[@]}"; do
    # Parse experiment parameters
    IFS=',' read name neurons steps conn_prob plast_rate <<< "$experiment"
    
    echo "Running experiment: $name"
    echo "  Neurons: $neurons, Steps: $steps, "
    echo "  Connection probability: $conn_prob, Plasticity rate: $plast_rate"
    
    # Create experiment-specific config
    cat > "temp_config_$name.cfg" << EOF
brain.neuron_count = $neurons
brain.region_count = 1
brain.connection_probability = $conn_prob
plasticity.stdp.learning_rate = $plast_rate
plasticity.hebbian.enable = true
neuromod.curiosity.enable = true
neuromod.novelty.enable = true
brain.development.enabled = true
EOF
    
    # Run experiment with timing
    start_time=$(date +%s)
    ./nlm --config="temp_config_$name.cfg" --benchmark 2>&1 | 
        tee "$RESULTS_DIR/experiment_$name.txt"
    end_time=$(date +%s)
    
    duration=$((end_time - start_time))
    echo "  Completed in $duration seconds" >> "$RESULTS_DIR/experiment_$name.txt"
    
    # Clean up
    rm "temp_config_$name.cfg"
    
    echo ""
    
    sleep 2  # Brief pause between experiments

done

# Generate summary report
echo "Generating batch experiment summary..."
cat > "$RESULTS_DIR/batch_summary.txt" << EOF
Batch Experiment Summary
=======================
Total experiments: ${#EXPERIMENTS[@]}

Results for each experiment:
$(for result in "$RESULTS_DIR/experiment_"*.txt; do
    basename=$(basename $result .txt)
    name=${basename#experiment_}
    echo "- $name:"
    grep -E "(Steps/sec|Duration|Completed)" $result
    echo ""
done)

EOF

echo "Batch processing complete. Results saved to $RESULTS_DIR/"
```

## Model Persistence

### Advanced Model Saving and Loading

```python
import pynlm
import numpy as np
import json
import hashlib
from pathlib import Path

def create_model_checkpoint(brain, world, agent, experiment_id, metadata=None):
    """Create advanced model checkpoint with metadata"""
    checkpoint_dir = Path(f"models/experiment_{experiment_id}")
    checkpoint_dir.mkdir(parents=True, exist_ok=True)
    
    # Create checkpoint data
    checkpoint = {
        'timestamp': time.time(),
        'experiment_id': experiment_id,
        'brain_config': brain.getConfig().saveToDict(),
        'brain_state': brain.save(),
        'world_state': save_world_state(world),
        'agent_state': save_agent_state(agent),
        'metadata': metadata or {},
        'performance_stats': collect_performance_stats(brain),
        'checksum': None
    }
    
    # Calculate checksum
    checkpoint['checksum'] = calculate_checksum(checkpoint)
    
    # Save checkpoint
    checkpoint_file = checkpoint_dir / f"checkpoint_{experiment_id:06d}.json"
    with open(checkpoint_file, 'w') as f:
        json.dump(checkpoint, f, indent=2)
    
    # Also save as binary for efficiency
    binary_file = checkpoint_dir / f"checkpoint_{experiment_id:06d}.bin"
    with open(binary_file, 'wb') as f:
        f.write(brain.save())
        f.write(save_world_state_binary(world))
        f.write(save_agent_state_binary(agent))
    
    print(f"Model checkpoint saved to {checkpoint_file}")
    return checkpoint_file

def load_model_checkpoint(checkpoint_path, brain, world, agent):
    """Load advanced model checkpoint"""
    checkpoint_file = Path(checkpoint_path)
    
    # Load JSON metadata
    with open(checkpoint_file.with_suffix('.json'), 'r') as f:
        checkpoint = json.load(f)
    
    # Validate checksum
    if calculate_checksum(checkpoint) != checkpoint['checksum']:
        raise ValueError("Checksum mismatch - checkpoint corrupted")
    
    # Load brain state
    with open(checkpoint_file.with_suffix('.bin'), 'rb') as f:
        brain.load(f.read(1000000))  # Simplified for example
        load_world_state_binary(world, f.read(500000))
        load_agent_state_binary(agent, f.read(500000))
    
    # Restore configuration
    brain.getConfig().loadFromDict(checkpoint['brain_config'])
    
    print(f"Model checkpoint loaded from {checkpoint_path}")
    return checkpoint

def save_world_state(world):
    """Save world state to serializable format"""
    return {
        'agent_position': {'x': world.getAgentPosition().x, 'y': world.getAgentPosition().y},
        'agent_orientation': world.getAgentOrientation(),
        'agent_energy': world.getAgentEnergy(),
        'objects': save_world_objects(world.getObjects()),
        'simulation_time': world.getSimulationTime(),
        'random_seed': world.getRandomSeed()
    }

def load_world_state(world, state):
    """Load world state from serializable format"""
    agent_pos = state['agent_position']
    world.setAgentPosition(agent_pos['x'], agent_pos['y'])
    world.setAgentOrientation(state['agent_orientation'])
    world.setAgentEnergy(state['agent_energy'])
    
    # Clear and restore objects
    world.clearObjects()
    for obj_data in state['objects']:
        obj = load_world_object(obj_data)
        world.addObject(obj)
    
    world.setSimulationTime(state['simulation_time'])
    world.setRandomSeed(state['random_seed'])

def save_agent_state(agent):
    """Save agent state to serializable format"""
    return {
        'neuromodulation': {
            'dopamine': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        },
        'development': agent.getDevelopmentalStage(),
        'configuration': {
            'reward_modulation': agent.isRewardModulationEnabled(),
            'structural_plasticity': agent.isStructuralPlasticityEnabled(),
            'development': agent.isDevelopmentEnabled(),
            'curiosity': agent.isCuriosityEnabled()
        }
    }

def load_agent_state(agent, state):
    """Load agent state from serializable format"""
    # Restore neuromodulation state
    # Note: In practice, you'd need access to internal state
    pass
    
    # Restore configuration
    agent.enableRewardModulation(state['configuration']['reward_modulation'])
    agent.enableStructuralPlasticity(state['configuration']['structural_plasticity'])
    agent.enableDevelopment(state['configuration']['development'])
    agent.enableCuriosity(state['configuration']['curiosity'])

def collect_performance_stats(brain):
    """Collect comprehensive performance statistics"""
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'regions': brain.getRegionCount(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'active_neurons': brain.getActiveNeuronCount(),
        'working_memory_traces': brain.getWorkingMemory().getActiveTraces(),
        'episodic_memory_episodes': brain.getEpisodicMemory().getEpisodeCount()
    }
```

## Integration with External Systems

### Python Integration

```python
import pynlm
import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler

def integrate_with_ml_system(brain, external_data):
    """Integrate NLM with external machine learning systems"""
    
    # Preprocess external data
    scaler = StandardScaler()
    processed_data = scaler.fit_transform(external_data)
    
    # Convert to neural input
    neural_input = convert_to_neural_input(processed_data)
    
    # Inject into brain
    brain.receiveSensoryInput(neural_input)
    
    # Get predictions
    predictions = get_brain_predictions(brain)
    
    # Return both neural and ML predictions
    return {
        'neural_prediction': predictions,
        'ml_prediction': predict_with_ml_system(processed_data),
        'confidence': calculate_confidence(brain, processed_data),
        'combined': combine_predictions(predictions, predict_with_ml_system(processed_data))
    }

def convert_to_neural_input(external_data):
    """Convert external data to neural input format"""
    # Create neural percept
    percept = pynlm.SensoryPercept()
    
    # Map external features to neural channels
    # This depends on your specific external data format
    if external_data.shape[1] >= 256:  # Vision-like input
        percept.setVision(external_data[:, :256])
    if external_data.shape[1] >= 8:   # Touch-like input
        percept.setTouch(external_data[:, 256:264])
    if external_data.shape[1] >= 4:   # Internal signals
        percept.setInternal(external_data[:, 264:268])
    if external_data.shape[1] >= 6:   # Proprioception
        percept.setProprioception(external_data[:, 268:274])
    
    return percept

def log_experiment_results(experiment_id, brain, metrics, external_predictions):
    """Log experiment results for analysis"""
    # Create experiment log
    log_entry = {
        'experiment_id': experiment_id,
        'timestamp': time.time(),
        'brain_metrics': metrics,
        'external_predictions': external_predictions,
        'neural_activity': {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
    }
    
    # Save to database or file
    save_to_experiment_log(log_entry)
    
    # Generate analysis plots
    generate_analysis_plots(log_entry)
    
    return log_entry

# Example usage with ML integration
def ml_system_integration_example():
    """Example of NLM integrated with ML systems"""
    
    # Initialize NLM
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize external ML system (simplified example)
    external_ml_model = initialize_external_ml_model()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable neuromodulation for learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Integration loop
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get external data (simulated)
        external_features = get_external_features(step)
        
        # Integrate with ML system
        ml_input = integrate_with_ml_system(brain, external_features)
        
        # Process with NLM
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log results
        metrics = collect_brain_metrics(brain)
        log_experiment_results(step, brain, metrics, ml_input['neural_prediction'])
        
        # Periodic ML model update (if using online learning)
        if step % 100 == 0:
            update_external_ml_model(external_ml_model, metrics)
    
    return brain
```

This advanced guide provides comprehensive examples for working with NLM at a professional level. The code examples demonstrate how to integrate NLM with external systems, manage complex experiments, optimize performance, and create sophisticated neural architectures.

## Key Improvements Summary

This guide includes significant improvements:

1. **Advanced Configuration**: Custom configuration files and templates
2. **Performance Optimization**: High-performance configurations and monitoring
3. **Brain Manipulation**: Runtime brain configuration and management
4. **Memory Management**: Working and episodic memory control
5. **Debugging Tools**: Comprehensive debugging and profiling utilities
6. **Batch Processing**: Automated experiment batches and result management
7. **Model Persistence**: Advanced checkpointing and state management
8. **External Integration**: Integration with machine learning systems

These capabilities make NLM suitable for professional research and production environments.

### References

- NLM Documentation: docs/ directory
- API Reference: pynlm module documentation
- Scientific Papers: docs/SCIENCE.md
- Architecture: docs/ARCHITECTURE.md