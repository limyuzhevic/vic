# NLM - Advanced Usage Guide

This guide provides advanced usage examples and demonstrates the full capabilities of the NLM (Neural Learning Machine) Phase 2 system.

## Advanced Configuration

### JSON Configuration File Example

```json
{
    "name": "NLM Phase 2 Advanced Configuration",
    "version": "2.0.0",
    "simulation": {
        "timestep": 0.001,
        "duration": 10.0,
        "neuron_count": 2000,
        "region_count": 4
    },
    "connectivity": {
        "probability": 0.15,
        "mean_weight": 0.5,
        "std_weight": 0.2
    },
    "plasticity": {
        "stdp": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "tau": 20.0
        },
        "hebbian": {
            "learning_rate": 0.01,
            "decay_rate": 0.001
        },
        "structural": {
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 200,
            "decay_rate": 0.01
        },
        "episodic": {
            "max_episodes": 1000,
            "replay_interval": 100
        }
    },
    "neuromodulation": {
        "dopamine": {
            "base_level": 0.1,
            "reward_sensitivity": 1.0
        },
        "curiosity": {
            "exploration_rate": 0.3
        }
    },
    "advanced_features": {
        "enable_prediction": true,
        "enable_planning": true,
        "enable_concept_formation": true,
        "enable_attention": true,
        "development_enabled": true
    }
}
```

### YAML Configuration File Example

```yaml
name: "NLM Phase 2 Advanced Configuration"
version: "2.0.0"

simulation:
  timestep: 0.001
  duration: 10.0
  neuron_count: 2000
  region_count: 4

connectivity:
  probability: 0.15
  mean_weight: 0.5
  std_weight: 0.2

plasticity:
  stdp:
    ltp_weight: 0.02
    ltd_weight: 0.015
    tau: 20.0
  hebbian:
    learning_rate: 0.01
    decay_rate: 0.001
  structural:
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

memory:
  working_memory:
    capacity: 200
    decay_rate: 0.01
  episodic:
    max_episodes: 1000
    replay_interval: 100

neuromodulation:
  dopamine:
    base_level: 0.1
    reward_sensitivity: 1.0
  curiosity:
    exploration_rate: 0.3

advanced_features:
  enable_prediction: true
  enable_planning: true
  enable_concept_formation: true
  enable_attention: true
  development_enabled: true
```

## Advanced Command Line Options

### Test Management

```bash
# Run a specific test with custom parameters
nlm --test plasticity --neuron_count 5000 --connection_probability 0.2

# Run multiple tests in sequence
nlm --test basic --test plasticity --test learning

# Run all tests with advanced features enabled
nlm --run-all-tests --advanced --verbose --auto-save

# Run tests with custom learning rate
nlm --test learning --learning-rate 0.05
```

### Batch Processing Mode

```bash
# Run simulation in batch mode with checkpointing
nlm --batch --max-steps 5000 --auto-save --checkpoint-dir "./simulations/run_1"

# Run with custom configuration and output results
nlm --config configs/experiment.cfg --output results.json --batch --advanced
```

### Interactive Mode

```bash
# Start interactive mode for real-time control
nlm --interactive --verbose

# Interactive mode with specific test
nlm --interactive --test plasticity --max-steps 2000
```

## Advanced Features

### 1. Learning with Memory Integration

```python
import pynlm
import json

# Create advanced configuration
config = {
    "neuron_count": 5000,
    "region_count": 4,
    "connection_probability": 0.2,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "replay_interval": 100,
    "consolidation_interval": 500,
    "enable_prediction": True,
    "enable_planning": True,
    "enable_concept_formation": True,
    "enable_attention": True
}

# Save configuration to JSON file
with open("advanced_config.json", "w") as f:
    json.dump(config, f, indent=2)

# Create brain with advanced configuration
brain = pynlm.createBrain(pynlm.createConfig("advanced_config.json"))
brain.initialize()

# Run simulation with memory tracking
for step in range(10000):
    # Simulate sensory input
    percept = generate_sensory_input(step)
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Periodic learning and memory consolidation
    if step % 100 == 0:
        # Store experience in episodic memory
        experience = capture_experience(brain, step)
        
        # Update prediction system
        prediction_error = compute_prediction_error(brain, percept)
        brain.applyNeuromodulation(prediction_error)
        
        # Print statistics
        print(f"Step {step}: Active neurons={brain.getActiveNeuronCount()}, "
              f"Total spikes={brain.getTotalSpikeCount()}, "
              f"Working memory traces={brain.getWorkingMemory().getActiveTraces()}")

# Analyze results
print("\n=== Simulation Results ===")
print(f"Final active neurons: {brain.getActiveNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.4f}")
if brain.getWorkingMemory():
    print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity()}")
    print(f"Active memory traces: {brain.getWorkingMemory().getActiveTraces()}")
if brain.getEpisodicMemory():
    print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
```

### 2. Multi-Stage Learning Experiment

```python
import pynlm
import time

def run_multi_stage_learning():
    # Stage 1: Random connectivity
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("random_seed", 42)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("=== Stage 1: Random Initialization ===")
    for step in range(1000):
        brain.step(step)
    print(f"Initial connectivity: {brain.getTotalSynapseCount()} synapses")
    
    # Stage 2: Experience with reinforcement learning
    config.set("random_seed", 123)
    config.set("connection_probability", 0.2)
    config.set("stdp_ltp_weight", 0.03)
    config.set("stdp_ltd_weight", 0.01)
    
    brain.reset()
    brain.initialize()
    
    print("\n=== Stage 2: Experience with Learning ===")
    for step in range(5000):
        # Generate task-specific input
        task_input = generate_task_input(step)
        agent.processSensoryInput(task_input)
        
        # Brain processes and learns
        brain.step(step)
        
        # Apply reward modulation
        reward = compute_reward(brain, step)
        if reward > 0:
            agent.applyRewardModulation(reward, brain.getPredictionErrorSignal())
        
        if step % 1000 == 0:
            print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, "
                  f"Learning rate={brain.getDopamine().getLevel():.4f}")
    
    # Stage 3: Consolidation and replay
    config.set("random_seed", 456)
    config.set("replay_interval", 50)
    config.set("consolidation_interval", 200)
    
    brain.reset()
    brain.initialize()
    
    print("\n=== Stage 3: Memory Consolidation ===")
    for step in range(8000):
        brain.step(step)
        
        if step % 200 == 0:
            # Consolidate important memories
            if brain.getEpisodicMemory():
                brain.getEpisodicMemory().consolidate(0.5)
            print(f"Step {step}: Consolidation complete, "
                  f"Episodic episodes={brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
    
    # Stage 4: Development and maturation
    config.set("random_seed", 789)
    config.set("development_enabled", True)
    
    brain.reset()
    brain.initialize()
    
    print("\n=== Stage 4: Development and Maturation ===")
    for step in range(12000):
        brain.step(step)
        
        if step % 3000 == 0:
            stage = brain.getDevelopmentalStage()
            print(f"Step {step}: Developmental stage={stage}")
            print(f"  Plasticity modifier: {stage.getPlasticityModifier():.2f}")
    
    return brain

# Run multi-stage learning
brain = run_multi_stage_learning()

# Final analysis
print("\n=== Multi-Stage Learning Results ===")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Final synaptic weight distribution:")
print(f"  Mean weight: {brain.getAverageSynapticWeight():.4f}")
print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.4f}")
if brain.getWorkingMemory():
    print(f"Working memory efficiency: {brain.getWorkingMemory().getMemoryActivity():.4f}")
if brain.getEpisodicMemory():
    print(f"Memory retention: {brain.getEpisodicMemory().getRetentionRate():.4f}")

return brain
```

### 3. Advanced Neural Architecture

```python
import pynlm

def create_advanced_brain():
    """Create a brain with advanced architectural features"""
    
    # Custom configuration
    config = pynlm.createCustomConfig({
        "neuron_count": 10000,
        "region_count": 8,
        "connection_probability": 0.2,
        "simulation_timestep": 0.0005,
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.02,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002,
        "replay_interval": 200,
        "consolidation_interval": 1000,
        "working_memory_capacity": 500,
        "episodic_memory_limit": 5000,
        "enable_prediction_system": True,
        "enable_planning_system": True,
        "enable_concept_formation": True,
        "enable_attention_system": True,
        "enable_development_system": True,
        "enable_curiosity_system": True,
        "enable_novelty_detection": True
    })
    
    # Create brain with advanced architecture
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create specialized regions
    for i in range(config.get("region_count", 1)):
        region = brain.addRegion(f"Region_{i+1}")
        
        # Create different neuron populations
        sensory_pop = region.addPopulation(
            config.get("neuron_count", 1000) // 8,
            pynlm.NeuronType.Sensory
        )
        
        motor_pop = region.addPopulation(
            config.get("neuron_count", 1000) // 8,
            pynlm.NeuronType.Motor
        )
        
        internal_pop = region.addPopulation(
            config.get("neuron_count", 1000) * 3 // 4,
            pynlm.NeuronType.Internal
        )
        
        # Initialize special connectivity patterns
        initialize_special_connectivity(region)
    
    return brain

def initialize_special_connectivity(region):
    """Initialize specialized connectivity patterns"""
    neurons = region.getAllNeurons()
    n = len(neurons)
    
    # Create feedforward connectivity
    for i in range(0, n, 10):
        for j in range(i + 10, min(i + 20, n), 5):
            create_reinforcing_connection(neurons[i], neurons[j])
    
    # Create feedback connections
    for i in range(n - 10, n):
        for j in range(max(0, i - 15), i, 3):
            create_modulatory_connection(neurons[i], neurons[j])
    
    # Create clustered local connections
    cluster_size = 20
    num_clusters = n // cluster_size
    for cluster in range(num_clusters):
        cluster_start = cluster * cluster_size
        cluster_end = min(cluster_start + cluster_size, n)
        
        # Strong local connections within cluster
        for i in range(cluster_start, cluster_end):
            for j in range(cluster_start, cluster_end):
                if i != j and (j - i) <= 3:  # Only nearby neurons
                    create_local_connection(neurons[i], neurons[j])

def create_reinforcing_connection(pre_neuron, post_neuron):
    """Create a strongly reinforcing connection"""
    config = pynlm.createDefaultConfig()
    config.set("connection_probability", 0.8)
    config.set("mean_weight", 1.5)
    config.set("plasticity_enabled", True)
    
    synapse = pynlm.createConnection(pre_neuron, post_neuron, config)
    synapse.setType(pynlm.SynapseType.Excitatory)
    synapse.setWeight(1.5)
    synapse.enablePlasticity(True, True, False)  # Enable Hebbian and STDP

def create_modulatory_connection(pre_neuron, post_neuron):
    """Create a modulatory feedback connection"""
    config = pynlm.createDefaultConfig()
    config.set("connection_probability", 0.6)
    config.set("mean_weight", 0.8)
    config.set("plasticity_enabled", False)  # No plasticity for feedback
    
    synapse = pynlm.createConnection(pre_neuron, post_neuron, config)
    synapse.setType(pynlm.SynapseType.Inhibitory)
    synapse.setWeight(0.8)
    synapse.enablePlasticity(False, False, True)  # Enable only neuromodulation

def create_local_connection(pre_neuron, post_neuron):
    """Create a local clustered connection"""
    config = pynlm.createDefaultConfig()
    config.set("connection_probability", 0.9)
    config.set("mean_weight", 1.2)
    config.set("plasticity_enabled", True)
    
    synapse = pynlm.createConnection(pre_neuron, post_neuron, config)
    synapse.setType(pynlm.SynapseType.Excitatory)
    synapse.setWeight(1.2)
    synapse.enablePlasticity(True, False, False)  # Enable only STDP
    
    # Add short delay for local processing
    synapse.setDelay(1)

# Create advanced brain
advanced_brain = create_advanced_brain()

# Run simulation with advanced features
print("=== Advanced Brain Simulation ===")
for step in range(20000):
    # Generate complex input patterns
    complex_input = generate_complex_input(step)
    agent.processSensoryInput(complex_input)
    
    # Brain processes with advanced architectures
    advanced_brain.step(step)
    
    # Apply advanced learning mechanisms
    apply_advanced_learning(advanced_brain, step)
    
    # Periodic status updates
    if step % 2000 == 0:
        print(f"Step {step}: Processing complete")
        print(f"  Network statistics: {advanced_brain.getTotalNeuronCount()} neurons, "
              f"{advanced_brain.getTotalSynapseCount()} synapses")
        print(f"  Memory statistics: {advanced_brain.getWorkingMemory().getActiveTraces()} "
              f"working traces, {advanced_brain.getEpisodicMemory().getEpisodeCount()} "
              f"episodic episodes")

return advanced_brain
```

## Performance Optimization

### 1. Memory-Efficient Configuration

```bash
# Use compressed JSON configuration
nlm --config config.json.gz --decompress

# Use binary configuration for faster loading
nlm --config config.bin --optimize-memory
```

### 2. Parallel Processing

```python
import pynlm
import threading

def run_parallel_simulation(brain, num_threads=4):
    """Run simulation in parallel across multiple regions"""
    regions = brain.getRegions()
    region_chunks = distribute_regions(regions, num_threads)
    
    threads = []
    for i, region_chunk in enumerate(region_chunks):
        thread = threading.Thread(
            target=simulate_region,
            args=(brain, region_chunk, i)
        )
        threads.append(thread)
    
    # Start all threads
    for thread in threads:
        thread.start()
    
    # Wait for all threads to complete
    for thread in threads:
        thread.join()
    
    return brain
```

### 3. Visualization and Analysis

```python
import pynlm
import matplotlib.pyplot as plt

def analyze_simulation_results(brain):
    """Analyze and visualize simulation results"""
    
    # Create result analyzer
    analyzer = pynlm.createResultAnalyzer(brain)
    
    # Generate network statistics
    network_stats = analyzer.getNetworkStatistics()
    
    # Plot network activity
    plt.figure(figsize=(15, 10))
    
    # Plot 1: Network activity over time
    plt.subplot(2, 3, 1)
    plt.plot(network_stats['time_steps'], 
             network_stats['total_spikes'], 
             label='Total Spikes')
    plt.plot(network_stats['time_steps'], 
             network_stats['firing_neurons'], 
             label='Firing Neurons')
    plt.xlabel('Time Step')
    plt.ylabel('Count')
    plt.title('Network Activity')
    plt.legend()
    
    # Plot 2: Synaptic weight distribution
    plt.subplot(2, 3, 2)
    analyzer.plotSynapticWeightDistribution()
    plt.title('Synaptic Weight Distribution')
    
    # Plot 3: Memory traces over time
    plt.subplot(2, 3, 3)
    analyzer.plotMemoryTracesOverTime()
    plt.title('Working Memory Traces')
    
    # Plot 4: Episodic memory consolidation
    plt.subplot(2, 3, 4)
    analyzer.plotEpisodicMemoryConsolidation()
    plt.title('Episodic Memory Consolidation')
    
    # Plot 5: Prediction error over time
    plt.subplot(2, 3, 5)
    analyzer.plotPredictionError()
    plt.title('Prediction Error')
    
    # Plot 6: Developmental progression
    plt.subplot(2, 3, 6)
    analyzer.plotDevelopmentalProgression()
    plt.title('Developmental Stage')
    
    plt.tight_layout()
    plt.savefig('simulation_results.png')
    plt.show()
    
    # Generate detailed report
    report = analyzer.generateDetailedReport()
    
    with open('simulation_report.txt', 'w') as f:
        f.write(report)
    
    return report
```

## Advanced Configuration Files

### 1. Performance-Optimized Configuration

```yaml
# performance_config.yaml
name: "Performance Optimized Configuration"
simulation:
  timestep: 0.0001  # Smaller timestep for accuracy
  parallel: true
  multithreading: 8

connectivity:
  probability: 0.05  # Lower connectivity for efficiency
  sparse: true

memory:
  working_memory:
    capacity: 2000  # Larger working memory
    decay_rate: 0.005  # Slower decay
  episodic:
    max_episodes: 5000
    replay_interval: 200

neuromodulation:
  dopamine:
    base_level: 0.2
    reward_sensitivity: 2.0
  curiosity:
    exploration_rate: 0.5

advanced_features:
  enable_prediction: true
  enable_planning: true
  enable_concept_formation: true
  enable_attention: true
  development_enabled: true
  enable_memory_replay: true
  enable_structural_plasticity: true
```

### 2. Development-Focused Configuration

```yaml
# development_config.yaml
name: "Development Focused Configuration"
development:
  stage_initial: true
  stage_critical: true
  stage_maturation: false
  stage_adult: false

plasticity:
  initial_multiplier: 2.0
  critical_multiplier: 1.5
  maturation_multiplier: 0.8
  adult_multiplier: 0.3

synaptogenesis:
  rate_initial: 0.001
  rate_critical: 0.0005
  rate_maturation: 0.0001
  rate_adult: 0.00001

pruning:
  rate_initial: 0.00001
  rate_critical: 0.000005
  rate_maturation: 0.000001
  rate_adult: 0.0000001

memory:
  consolidation_rate_initial: 0.2
  consolidation_rate_critical: 0.3
  consolidation_rate_maturation: 0.5
  consolidation_rate_adult: 0.8
```

## Integration Examples

### 1. Multi-Agent Environment

```python
import pynlm
import numpy as np

def create_multi_agent_environment(num_agents=5):
    """Create an environment with multiple interacting agents"""
    
    # Create shared environment
    environment = pynlm.createSimpleWorld()
    environment.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    environment.reset()
    
    # Create shared brain template
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("neuron_count", 2000)
    brain_config.set("region_count", 3)
    brain_config.set("connection_probability", 0.15)
    
    # Create agents with shared brain template
    agents = []
    for i in range(num_agents):
        # Create individual brain with slight variations
        agent_brain_config = pynlm.createCopyConfig(brain_config)
        agent_brain_config.set("random_seed", 42 + i)  # Different seed for each agent
        
        brain = pynlm.createBrain(agent_brain_config)
        brain.initialize()
        
        # Create agent with the brain
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(environment)
        
        agents.append(agent)
    
    return environment, agents

def run_multi_agent_simulation(environment, agents, num_steps=1000):
    """Run simulation with multiple interacting agents"""
    
    # Enable advanced features for multi-agent learning
    for agent in agents:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    # Simulation loop
    for step in range(num_steps):
        # Update environment
        environment.update(0.1)
        
        # Each agent perceives the environment
        perceptions = []
        for agent in agents:
            percept = environment.getSensoryPercept()
            perceptions.append(percept)
        
        # Agents process their perceptions
        for i, agent in enumerate(agents):
            agent.processSensoryInput(perceptions[i])
        
        # Agents make decisions
        actions = []
        for agent in agents:
            action = agent.decodeMotorCommand()
            actions.append(action)
        
        # Environment applies actions
        for i, action in enumerate(actions):
            environment.applyMotorCommand(action, environment.getSimulationTime())
        
        # Agents update their brains
        for i, agent in enumerate(agents):
            agent.getBrain().step(step)
            
            # Apply inter-agent learning
            if i > 0:
                previous_brain = agents[i-1].getBrain()
                apply_inter_agent_learning(agent.getBrain(), previous_brain, step)
        
        # Periodic multi-agent coordination
        if step % 100 == 0:
            coordinate_agents(agents, step)
    
    return environment, agents
```

### 2. Transfer Learning Between Tasks

```python
import pynlm
import joblib

def create_transfer_learning_system():
    """Create a system that can transfer learning between different tasks"""
    
    # Task 1: Maze Navigation
    print("=== Task 1: Maze Navigation ===")
    maze_config = pynlm.createTaskConfig("maze_navigation", {
        "environment_size": (20, 20),
        "agent_count": 1,
        "sensor_range": 5,
        "reward_positive": 10.0,
        "reward_negative": -5.0
    })
    
    maze_brain = pynlm.createBrain(maze_config)
    maze_brain.initialize()
    
    # Train on maze task
    for step in range(5000):
        maze_input = generate_maze_input(step)
        maze_agent = pynlm.createAgentBrain(maze_brain)
        maze_agent.initialize(maze_environment)
        maze_agent.processSensoryInput(maze_input)
        maze_brain.step(step)
        maze_agent.applyRewardModulation(reward, maze_brain.getPredictionErrorSignal())
    
    # Save maze knowledge
    maze_knowledge = extract_task_knowledge(maze_brain)
    joblib.dump(maze_knowledge, "maze_knowledge.joblib")
    
    # Task 2: Object Recognition
    print("\n=== Task 2: Object Recognition ===")
    object_config = pynlm.createTaskConfig("object_recognition", {
        "environment_size": (30, 30),
        "agent_count": 1,
        "sensor_range": 8,
        "reward_positive": 15.0,
        "reward_negative": -3.0
    })
    
    # Create new brain and initialize with maze knowledge
    object_brain = pynlm.createBrain(object_config)
    object_brain.initialize()
    
    # Transfer maze knowledge
    transfer_knowledge(object_brain, maze_knowledge)
    
    # Fine-tune on object recognition task
    for step in range(8000):
        object_input = generate_object_input(step)
        object_agent = pynlm.createAgentBrain(object_brain)
        object_agent.initialize(object_environment)
        object_agent.processSensoryInput(object_input)
        object_brain.step(step)
        object_agent.applyRewardModulation(reward, object_brain.getPredictionErrorSignal())
    
    # Task 3: Sequential Decision Making
    print("\n=== Task 3: Sequential Decision Making ===")
    seq_config = pynlm.createTaskConfig("sequential_decision", {
        "environment_size": (40, 40),
        "agent_count": 1,
        "sensor_range": 10,
        "reward_positive": 20.0,
        "reward_negative": -1.0
    })
    
    # Create new brain and transfer all knowledge
    seq_brain = pynlm.createBrain(seq_config)
    seq_brain.initialize()
    
    # Transfer all accumulated knowledge
    all_knowledge = {
        "maze_knowledge": maze_knowledge,
        "object_knowledge": extract_task_knowledge(object_brain)
    }
    transfer_all_knowledge(seq_brain, all_knowledge)
    
    # Fine-tune on sequential decision task
    for step in range(10000):
        seq_input = generate_sequential_input(step)
        seq_agent = pynlm.createAgentBrain(seq_brain)
        seq_agent.initialize(seq_environment)
        seq_agent.processSensoryInput(seq_input)
        seq_brain.step(step)
        seq_agent.applyRewardModulation(reward, seq_brain.getPredictionErrorSignal())
    
    return maze_brain, object_brain, seq_brain

# Run transfer learning system
maze_brain, object_brain, seq_brain = create_transfer_learning_system()

print("\n=== Transfer Learning Results ===")
print("Maze Navigation Brain:")
print(f"  Final performance: {evaluate_brain_performance(maze_brain, 'maze_navigation')}")

print("\nObject Recognition Brain:")
print(f"  Final performance: {evaluate_brain_performance(object_brain, 'object_recognition')}")
print(f"  Transfer benefit: {calculate_transfer_benefit(object_brain, 'object_recognition')}")

print("\nSequential Decision Making Brain:")
print(f"  Final performance: {evaluate_brain_performance(seq_brain, 'sequential_decision')}")
print(f"  Transfer benefit: {calculate_transfer_benefit(seq_brain, 'sequential_decision')}")
```

## Conclusion

These advanced usage examples demonstrate the full capabilities of the NLM Phase 2 system, including:

1. **Configuration Management**: JSON/YAML configuration files for complex setups
2. **Command Line Interface**: Advanced options for flexible control
3. **Learning Systems**: Integration of working memory, episodic memory, and prediction systems
4. **Multi-Stage Learning**: Progressive learning across different tasks
5. **Advanced Architecture**: Specialized connectivity patterns and regional organization
6. **Performance Optimization**: Parallel processing and memory-efficient configurations
7. **Analysis and Visualization**: Comprehensive tools for understanding simulation results
8. **Transfer Learning**: Knowledge transfer between different cognitive tasks

The NLM Phase 2 system is now a fully integrated artificial brain capable of sophisticated learning, memory, and decision-making processes, making it suitable for advanced AI research and applications.
EOF