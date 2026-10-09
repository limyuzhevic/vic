# NLM - Advanced Usage Guide for Expert Users

## Overview

This guide covers advanced features for experienced NLM users who want to:
- Run complex multi-agent simulations
- Analyze and profile brain performance
- Control experiment execution with checkpoints
- Export and visualize results
- Work with advanced configurations
- Implement custom learning scenarios

## Advanced Configuration

### Advanced Configuration Factory

```python
import pynlm

# Create advanced configuration with optimized parameters
config = pynlm.createAdvancedConfig()

# Customize specific parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2f)
config.set("brain.connection_probability", 0.1f)
config.set("plasticity.stdp.learning_rate", 0.002f)
config.set("plasticity.structural.synaptogenesis_rate", 0.0003f)
config.set("neuromod.dopamine.scale", 1.5f)
```

### Configuration Parameters Reference

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 2000 | Total neurons (higher = more complex) |
| `brain.synapse_density` | float | 0.15 | Connection probability |
| `brain.connection_probability` | float | 0.05 | Sparse connectivity |
| `brain.initial_weight_mean` | float | 0.5 | Initial synaptic weights |
| `brain.v_thresh` | float | -50.0 | Neuron firing threshold |
| `brain.v_rest` | float | -70.0 | Resting membrane potential |
| `plasticity.stdp.enable` | bool | true | Enable spike-timing dependent plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning strength |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `neuromod.dopamine.scale` | float | 1.0 | Reward modulation strength |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Batch Processing and Experiment Control

### Batch Simulation

```python
import pynlm

def run_complex_batch():
    # Create components
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run batch simulation with learning
    print("Running complex batch simulation...")
    success = pynlm.runBatchSimulation(brain, world, agent, 5000, True)
    
    if success:
        print("Batch simulation completed successfully!")
        
        # Analyze results
        stats = pynlm.getBrainStats(brain)
        print(f"Total neurons: {stats['neurons']}")
        print(f"Total synapses: {stats['synapses']}")
        print(f"Total spikes: {stats['total_spikes']}")
        print(f"Final firing rate: {stats['avg_firing_rate']:.3f}")
        print(f"Development stage: {stats['development_stage']}")
```

### Multi-Step Experiment with Checkpoints

```python
import pynlm

def run_multi_step_experiment():
    # Setup brain with advanced configuration
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    experiment_checkpoints = []
    
    def save_progress(step, b, w, a):
        """Save experiment progress at checkpoints"""
        if step % 500 == 0 and step > 0:
            filename = f"experiment_step_{step}.bin"
            if b.save(filename):
                print(f"  Checkpoint saved: {filename}")
            else:
                print(f"  Failed to save checkpoint at step {step}")
    
    experiment_checkpoints.append(save_progress)
    
    def monitor_learning(step, b, w, a):
        """Monitor learning progress"""
        if step % 1000 == 0 and step > 0:
            curiosity = a.getCuriosityLevel()
            novelty = a.getNoveltyLevel()
            pred_error = a.getPredictionError()
            print(f"  Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, PredError={pred_error:.3f}")
    
    experiment_checkpoints.append(monitor_learning)
    
    # Run multi-step experiment with monitoring
    print("Running multi-step experiment with checkpoints...")
    success = pynlm.runMultiStepExperiment(brain, world, agent, experiment_checkpoints, 3000)
    
    if success:
        print("Multi-step experiment completed!")
        
        # Load latest checkpoint
        latest_checkpoint = pynlm.getLatestCheckpoint()
        if not latest_checkpoint.empty():
            print(f"Loading latest checkpoint: {latest_checkpoint}")
            brain.load(latest_checkpoint)
```

## Performance Profiling and Analysis

### Brain Performance Profiling

```python
import pynlm

def profile_brain_performance():
    # Create brain and initialize
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(1000):
        brain.step(i)
    
    # Profile performance
    print("=== Brain Performance Profile ===")
    pynlm.profileBrainPerformance(brain, "Learning Phase Profile")
    
    # Get detailed statistics
    stats = pynlm.getBrainStats(brain)
    
    print("\nDetailed Statistics:")
    for key, value in stats.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.4f}")
        else:
            print(f"  {key}: {value}")
```

### Simulation Benchmarking

```python
import pynlm

def benchmark_simulation():
    # Create brain and warmup
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warmup phase
    for i in range(100):
        brain.step(i)
    
    # Benchmark different configurations
    configs_to_test = [
        {"neurons": 1000, "density": 0.1},
        {"neurons": 2000, "density": 0.15},
        {"neurons": 5000, "density": 0.2},
    ]
    
    print("Benchmarking different configurations...")
    for i, cfg in enumerate(configs_to_test):
        print(f"\nConfiguration {i+1}: {cfg['neurons']} neurons, {cfg['density']} density")
        
        # Create fresh brain for each test
        test_config = pynlm.createAdvancedConfig()
        test_config.set("brain.neuron_count", cfg["neurons"])
        test_config.set("brain.synapse_density", cfg["density"])
        
        test_brain = pynlm.createBrain(test_config)
        test_brain.initialize()
        
        # Benchmark
        steps_per_second = pynlm.benchmarkSimulation(test_brain, 500, 50)
        print(f"  Performance: {steps_per_second:.1f} steps/second")
```

## Memory Management and Checkpointing

### Advanced Checkpoint Operations

```python
import pynlm

def advanced_checkpointing():
    # Create brain with learning enabled
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation with periodic checkpointing
    for step in range(2000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Periodic checkpointing
        if step % 300 == 0 and step > 0:
            print(f"Creating checkpoint at step {step}...")
            if brain.save(f"checkpoint_step_{step}.bin"):
                print(f"  Checkpoint saved successfully")
            else:
                print(f"  Failed to save checkpoint")
        
        # Load latest checkpoint if available
        if step % 500 == 0 and step > 0:
            latest_checkpoint = pynlm.getLatestCheckpoint()
            if !latest_checkpoint.empty() and latest_checkpoint != f"checkpoint_step_{step}.bin":
                print(f"Loading checkpoint: {latest_checkpoint}")
                if brain.load(latest_checkpoint):
                    print(f"  Checkpoint loaded successfully")
                else:
                    print(f"  Failed to load checkpoint")
```

### Memory Statistics and Analysis

```python
import pynlm

def analyze_memory_usage():
    # Create and run brain
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
    
    # Get memory statistics
    print("=== Memory Usage Analysis ===")
    
    stats = pynlm.getBrainStats(brain)
    
    print(f"Neuron memory efficiency: {stats['neurons'] / 2000:.2%}")
    print(f"Synapse connectivity: {stats['synapses'] / (stats['neurons'] * stats['neurons']):.6f}")
    print(f"Activity density: {stats['firing_neurons'] / stats['neurons']:.2%}")
    print(f"Spike rate: {stats['avg_firing_rate']:.4f} spikes/step")
    
    # Calculate theoretical memory requirements
    neuron_memory = stats['neurons'] * 64  # bytes per neuron
    synapse_memory = stats['synapses'] * 32  # bytes per synapse
    total_memory = neuron_memory + synapse_memory
    
    print(f"\nMemory Requirements:")
    print(f"  Neuron storage: {neuron_memory / 1024:.2f} KB")
    print(f"  Synapse storage: {synapse_memory / 1024:.2f} KB")
    print(f"  Total estimated: {total_memory / 1024:.2f} KB")
```

## Prediction and Concept Formation Usage

### Advanced Prediction System

```python
import pynlm

def demonstrate_prediction_system():
    # Setup brain with advanced configuration
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    print("Demonstrating prediction system...")
    for step in range(1500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get prediction information
        prediction_error = agent.getPredictionError()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        # Log prediction-related metrics
        if step % 100 == 0:
            print(f"  Step {step}: PredError={prediction_error:.4f}, "
                  f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
        
        # Apply reward modulation
        reward = 0.0f
        internal = world.getSensoryPercept().getInternal()
        if !internal.empty():
            reward = internal[0]
        
        agent.applyRewardModulation(reward, prediction_error)
    
    print("Prediction system demonstration completed!")
```

### Concept Formation Analysis

```python
import pynlm

def analyze_concept_formation():
    # Create brain with all systems enabled
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation to allow concept formation
    print("Running simulation for concept formation analysis...")
    for step in range(2000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        if step % 300 == 0:
            # Get current concepts
            concepts = brain.getCurrentConcepts()
            print(f"Step {step}: Found {len(concepts)} concepts")
            
            if concepts:
                print(f"  Concept IDs: {concepts}")
            
            # Log development stage
            dev_stage = brain.getDevelopmentalStage()
            print(f"  Development stage: {dev_stage}")
    
    # Final analysis
    print("\n=== Final Concept Formation Analysis ===")
    final_concepts = brain.getCurrentConcepts()
    print(f"Total concepts discovered: {len(final_concepts)}")
    
    # Get brain statistics
    stats = pynlm.getBrainStats(brain)
    print(f"Final firing rate: {stats['avg_firing_rate']:.4f}")
    print(f"Total learning steps: {stats['total_spikes']}")
```

## Advanced Experiment Scenarios

### Learning Environment Adaptation

```python
import pynlm

def adaptive_learning_environment():
    """Demonstrates brain adapting to changing environment"""
    
    # Initialize brain with advanced config
    config = pynlm.createAdvancedConfig()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create dynamic world
    world = pynlm.createSimpleWorld()
    world.configure(width=60, height=60, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Running adaptive learning environment simulation...")
    
    for episode in range(5):
        print(f"\n--- Episode {episode + 1} ---")
        
        # Vary world dynamics each episode
        if episode % 2 == 0:
            # Easy environment
            world.setMaxEnergy(100.0)
            print("Environment: Easy (high resources)")
        else:
            # Hard environment
            world.setMaxEnergy(50.0)
            print("Environment: Hard (low resources)")
        
        # Run episode
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward based on environment
            reward = world.getSimulationTime() * 0.1  # Time-based reward
            agent.applyRewardModulation(reward, 0.0f)
            
            agent.updateDevelopment(0.1)
        
        # Analyze adaptation
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        print(f"  End of episode - Curiosity: {curiosity:.3f}, Novelty: {novelty:.3f}")
    
    print("\nAdaptive learning simulation completed!")
```

## Troubleshooting and Common Issues

### Common Performance Issues

```python
import pynlm

def troubleshoot_performance():
    """Troubleshoot common performance issues"""
    
    print("=== Performance Troubleshooting ===")
    
    # Test 1: Check if brain is initialized
    config = pynlm.createAdvancedConfig()
    brain = pynlm.createBrain(config)
    
    # This will fail if brain is not initialized
    try:
        brain.step(0)
        print("✓ Brain is properly initialized")
    except Exception as e:
        print(f"✗ Brain initialization error: {e}")
    
    # Test 2: Check memory usage
    stats = pynlm.getBrainStats(brain)
    if stats['neurons'] > 10000:
        print(f"⚠ Warning: High neuron count ({stats['neurons']}) may impact performance")
    
    # Test 3: Profile current performance
    print("Profiling current configuration...")
    steps_per_second = pynlm.benchmarkSimulation(brain, 200, 50)
    
    if steps_per_second < 1000:
        print(f"⚠ Warning: Low performance ({steps_per_second:.1f} steps/second)")
        print("  Consider reducing neuron count or increasing timestep")
    
    # Test 4: Check learning system status
    if not brain.hasCurrentConcepts():
        print("ℹ Note: No concepts discovered yet. This may be normal for early simulation stages.")
    
    print("Performance troubleshooting completed!")
```

### Debugging Common Errors

```python
import pynlm

def debug_common_errors():
    """Debug common errors in advanced usage"""
    
    print("=== Common Error Debugging ===")
    
    # Error 1: Checkpoint file not found
    print("\n1. Checking checkpoint file handling...")
    try:
        brain = pynlm.createBrain(pynlm.createAdvancedConfig())
        brain.initialize()
        
        # Try to load non-existent file
        if brain.load("non_existent_file.bin"):
            print("✗ Unexpected: Loading non-existent file succeeded")
        else:
            print("✓ Expected: Loading non-existent file failed gracefully")
    except Exception as e:
        print(f"✓ Expected exception: {type(e).__name__}")
    
    # Error 2: Memory issues
    print("\n2. Checking memory management...")
    try:
        config = pynlm.createAdvancedConfig()
        config.set("brain.neuron_count", 5000)  # High memory requirement
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Memory allocation successful for {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation to check memory stability
        for i in range(100):
            brain.step(i)
            
        print("✓ Memory usage stable during simulation")
        
    except MemoryError as e:
        print(f"✗ Memory error: {e}")
        print("  Consider reducing neuron count or available memory")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
    
    print("\nDebugging completed!")
```

## Next Steps

For more advanced usage, consider:

1. **Custom Learning Algorithms**: Implement custom plasticity rules
2. **Multi-Agent Systems**: Run multiple interacting brains
3. **Advanced Visualization**: Create custom visualization tools
4. **Distributed Computing**: Scale to larger simulations
5. **Real-time Applications**: Integrate with live data sources
6. **Machine Learning Integration**: Use NLM as component in larger ML systems

### Recommended Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)
- [Experiment Framework](docs/EXPERIMENTS.md)
- [Phase 6 Integration Results](docs/PHASE6_FINAL_AUDIT.md)

## License

MIT License - See LICENSE file for details

## Authors

NLM Research Team

---

*This document provides comprehensive guidance for advanced NLM users. For questions or technical support, refer to the main NLM documentation or contact the research team.*