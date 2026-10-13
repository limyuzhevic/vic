# NLM Examples Directory

This directory contains practical examples for using the NLM (Neural Learning Machine) library. Each example demonstrates different aspects of NLM functionality.

## Directory Structure

```
examples/
├── basic/
│   ├── 01_hello_world.py          # Getting started with NLM
│   ├── 02_create_brain.py         # Creating and configuring a brain
│   └── 03_basic_simulation.py     # Running basic simulations
│
├── learning/
│   ├── 01_plasticity_learning.py  # Learning with plasticity
│   ├── 02_memory_learning.py       # Learning with memory systems
│   └── 03_neuromodulation.py       # Learning with neuromodulation
│
├── advanced/
│   ├── 01_multi_region.py         # Working with multiple brain regions
│   ├── 02_checkpointing.py        # Saving and loading brain states
│   └── 03_custom_experiment.py    # Creating custom experiments
│
├── visualization/
│   ├── 01_basic_plots.py          # Basic visualization
│   ├── 02_memory_plots.py         # Memory visualization
│   └── 03_learning_plots.py       # Learning process visualization
│
├── performance/
│   ├── 01_benchmarking.py         # Performance benchmarking
│   ├── 02_scalability.py          # Testing scalability
│   └── 03_optimization.py         # Performance optimization
│
├── real_world/
│   ├── 01_snake_game.py           # NLM playing Snake game
│   ├── 02_robot_navigation.py     # Robot navigation with NLM
│   └── 03_adaptive_learning.py    # Adaptive learning systems
│
└── tutorial/
    ├── 01_introduction.md           # Introduction to NLM
    ├── 02_configuration.md         # Configuration guide
    ├── 03_learning.md              # Learning and plasticity
    ├── 04_memory.md                # Memory systems
    └── 05_cognition.md             # Cognitive systems
```

## Basic Examples

### 01_hello_world.py

```python
"""
Hello World example - Getting started with NLM

This is the simplest example that demonstrates:
- Creating a basic brain
- Running a simple simulation
- Understanding NLM output
"""

from pynlm import Brain, Config
import time

def hello_world_example():
    """Basic NLM hello world example"""
    print("=== NLM Hello World Example ===\n")
    
    # Create a simple brain with default configuration
    print("1. Creating brain with default configuration...")
    brain = Brain()
    brain.initialize()
    print("   ✓ Brain created and initialized\n")
    
    # Run a simple simulation
    print("2. Running simple simulation...")
    print("   Simulating 100 steps with sensory input...")
    
    start_time = time.time()
    for step in range(100):
        # Inject sensory input
        brain.inject_current_to_sensory_neurons(30.0)
        
        # Step the simulation
        brain.step(step, step * 0.001)
        
        # Print status every 20 steps
        if step % 20 == 0:
            status = brain.get_simulation_status()
            print(f"   Step {step}: Spikes={status['spike_count']}, "
                  f"Firing={status['firing_neuron_count']}%, "
                  f"Memory={status['working_memory_size']} items")
    
    end_time = time.time()
    
    # Print results
    print("\n3. Simulation Results:")
    final_status = brain.get_simulation_status()
    print(f"   Total simulation time: {end_time - start_time:.2f} seconds")
    print(f"   Total spikes generated: {brain.get_total_spike_count()}")
    print(f"   Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print(f"   Final working memory size: {final_status['working_memory_size']}")
    
    # Test learning
    print("\n4. Testing Learning Capabilities:")
    brain.reset()
    brain.initialize()
    
    # Enable plasticity
    brain.enable_stdp(True, True, True)
    brain.enable_hebbian(True)
    
    # Run learning experiment
    for episode in range(10):
        brain.inject_current_to_sensory_neurons(50.0)
        for step in range(50):
            brain.step(episode * 50 + step, (episode * 50 + step) * 0.001)
    
    learning_stats = brain.get_learning_statistics()
    print(f"   Learning occurred: {learning_stats['learning_occurred']}")
    print(f"   Mean weight change: {learning_stats['mean_weight_change']:.4f}")
    print(f"   Synaptic strengthening: {learning_stats['synaptic_changes']['strengthened']}")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    hello_world_example()
```

### 02_create_brain.py

```python
"""
Brain Creation Example

This example demonstrates different ways to create and configure a brain
in NLM, including using configuration files and custom parameters.
"""

from pynlm import Brain, Config
import tempfile
import os

def create_brain_example():
    """Demonstrate different brain creation methods"""
    print("=== Brain Creation Example ===\n")
    
    # Method 1: Default configuration
    print("1. Creating brain with default configuration...")
    brain1 = Brain()
    brain1.initialize()
    print(f"   ✓ Created with {brain1.get_total_neuron_count()} neurons")
    print(f"   ✓ {brain1.get_config().get_or('region_count', 1)} regions")
    print()
    
    # Method 2: Custom configuration with dictionary
    print("2. Creating brain with custom configuration...")
    custom_config = {
        'neuron_count': 2000,
        'region_count': 3,
        'connection_probability': 0.2,
        'simulation_timestep': 0.0005,
        'random_seed': 12345,
        'plasticity_learning_rate': 0.02,
        'stdp_ltp_weight': 0.03,
        'stdp_ltd_weight': 0.025,
        'development_synaptogenesis_rate': 0.001,
        'dopamine_baseline': 0.15,
        'reward_discount_factor': 0.97,
        'log_level': 'DEBUG'
    }
    
    brain2 = Brain(custom_config)
    brain2.initialize()
    print(f"   ✓ Created with {brain2.get_total_neuron_count()} neurons")
    print(f"   ✓ {brain2.get_config().get_or('region_count', 1)} regions")
    print(f"   ✓ Connection probability: {brain2.get_config().get_or('connection_probability', 0.15):.2f}")
    print()
    
    # Method 3: Loading from configuration file
    print("3. Creating brain from configuration file...")
    config_content = """# Custom configuration for NLM
neuron_count = 1500
region_count = 2
connection_probability = 0.12
simulation_timestep = 0.001
random_seed = 42
plasticity_learning_rate = 0.015
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
dopamine_baseline = 0.1
reward_discount_factor = 0.99
log_level = INFO
"""
    
    # Write to temporary file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write(config_content)
        config_file = f.name
    
    try:
        brain3 = Brain()
        brain3.load_config_from_file(config_file)
        brain3.initialize()
        print(f"   ✓ Created from file: {config_file}")
        print(f"   ✓ {brain3.get_total_neuron_count()} neurons loaded")
    finally:
        os.unlink(config_file)  # Clean up
    print()
    
    # Method 4: Progressive configuration
    print("4. Progressive configuration example...")
    brain4 = Brain()
    
    # Start with basic config
    brain4.set_neuron_count(500)
    brain4.set_region_count(1)
    brain4.set_simulation_timestep(0.002)
    
    print("   Initial configuration:")
    print(f"   - Neurons: {brain4.get_total_neuron_count()}")
    print(f"   - Regions: {brain4.get_config().get_or('region_count', 1)}")
    print(f"   - Timestep: {brain4.get_config().get_or('simulation_timestep', 0.001)}s")
    
    # Modify configuration
    brain4.set_neuron_count(1000)
    brain4.set_connection_probability(0.18)
    brain4.set_random_seed(999)
    
    print("   Modified configuration:")
    print(f"   - Neurons: {brain4.get_total_neuron_count()}")
    print(f"   - Connection probability: {brain4.get_config().get_or('connection_probability', 0.15):.2f}")
    
    brain4.initialize()
    print(f"   ✓ Final brain initialized with {brain4.get_total_neuron_count()} neurons")
    print()
    
    # Configuration validation
    print("5. Configuration validation example...")
    config = Config({
        'neuron_count': 5000,  # Valid
        'region_count': 10,    # Valid
        'connection_probability': 0.5,  # Valid
        'simulation_timestep': 0.1  # Valid
    })
    
    if config.validate():
        print("   ✓ Configuration is valid")
        brain5 = Brain(config)
        brain5.initialize()
        print(f"   ✓ Created valid brain with {brain5.get_total_neuron_count()} neurons")
    else:
        print("   ✗ Configuration validation failed")
        errors = config.get_validation_errors()
        for error in errors:
            print(f"     Error: {error}")
    
    print("\n=== Brain Creation Complete ===")

if __name__ == "__main__":
    create_brain_example()
```

### 03_basic_simulation.py

```python
"""
Basic Simulation Example

This example demonstrates fundamental simulation concepts in NLM,
including simulation control, monitoring, and basic experiment setup.
"""

from pynlm import Brain, Config
import time
import numpy as np

def basic_simulation_example():
    """Demonstrate basic simulation concepts"""
    print("=== Basic Simulation Example ===\n")
    
    # Create configuration for simulation
    config = Config({
        'neuron_count': 800,
        'region_count': 2,
        'simulation_timestep': 0.001,
        'random_seed': 42
    })
    
    # Create and initialize brain
    print("1. Setting up simulation...")
    brain = Brain(config)
    brain.initialize()
    
    print(f"   Simulation parameters:")
    print(f"   - Time step: {config.get_or('simulation_timestep', 0.001)}s")
    print(f"   - Total neurons: {brain.get_total_neuron_count()}")
    print(f"   - Total regions: {brain.get_region_count()}")
    print()
    
    # Simulation modes
    print("2. Running different simulation modes...")
    
    # Mode 1: Simple activity simulation
    print("\n   Mode 1: Simple Activity Simulation")
    brain.reset()
    brain.initialize()
    
    spike_counts = []
    firing_rates = []
    
    for step in range(500):
        # Inject varying input patterns
        if step % 100 == 0:
            brain.inject_current_to_sensory_neurons(40.0)
        elif step % 50 == 0:
            brain.inject_current_to_sensory_neurons(20.0)
        
        brain.step(step, step * 0.001)
        
        if step % 100 == 0:
            spike_count = brain.get_total_spike_count()
            firing_rate = brain.get_average_firing_rate()
            spike_counts.append(spike_count)
            firing_rates.append(firing_rate)
            print(f"     Step {step}: {spike_count} spikes, {firing_rate:.2f} Hz")
    
    # Mode 2: Learning simulation
    print("\n   Mode 2: Learning Simulation")
    brain.reset()
    brain.initialize()
    
    # Enable all plasticity mechanisms
    brain.enable_stdp(True, True, True)
    brain.enable_hebbian(True)
    brain.enable_structural(True)
    
    learning_progress = []
    
    for episode in range(20):
        # Reset for new episode
        brain.reset()
        brain.initialize()
        
        # Apply learning stimulus
        for step in range(100):
            brain.inject_current_to_sensory_neurons(60.0)
            brain.step(episode * 100 + step, (episode * 100 + step) * 0.001)
        
        # Record learning progress
        learning_stats = brain.get_learning_statistics()
        learning_progress.append(learning_stats['mean_weight_change'])
        print(f"     Episode {episode + 1}: Weight change = {learning_stats['mean_weight_change']:.4f}")
    
    # Analyze results
    print("\n3. Analysis Results:")
    print(f"   Simple simulation:")
    print(f"     - Average spikes per 100 steps: {np.mean(spike_counts):.1f}")
    print(f"     - Average firing rate: {np.mean(firing_rates):.2f} Hz")
    print(f"     - Spike range: {min(spike_counts)} - {max(spike_counts)}")
    
    print(f"\n   Learning simulation:")
    print(f"     - Final weight change: {learning_progress[-1]:.4f}")
    print(f"   - Learning trend: {np.polyfit(range(len(learning_progress)), learning_progress, 1)[0]:.6f} per episode")
    
    # Mode 3: Memory simulation
    print("\n   Mode 3: Memory Simulation")
    brain.reset()
    brain.initialize()
    
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    
    # Store information in working memory
    for i in range(10):
        working_memory.store(f'temp_data_{i}', {
            'value': np.random.random(),
            'timestamp': i * 0.1,
            'category': 'sensory' if i % 2 == 0 else 'motor'
        })
    
    # Retrieve and process information
    memory_retrieval_times = []
    for i in range(10):
        start_time = time.time()
        data = working_memory.retrieve(f'temp_data_{i}')
        retrieval_time = time.time() - start_time
        memory_retrieval_times.append(retrieval_time)
        
        # Store episode in episodic memory
        episodic_memory.store_episode({
            'episode_id': i,
            'retrieved_data': data,
            'retrieval_time': retrieval_time,
            'success': data is not None
        })
    
    print(f"     - Working memory size: {working_memory.get_size()}")
    print(f"     - Episodic memory episodes: {episodic_memory.get_episode_count()}")
    print(f"     - Average retrieval time: {np.mean(memory_retrieval_times) * 1000:.2f} ms")
    
    # Mode 4: Neuromodulation simulation
    print("\n   Mode 4: Neuromodulation Simulation")
    brain.reset()
    brain.initialize()
    
    dopamine = brain.get_dopamine()
    curiosity = brain.get_curiosity()
    
    neuromodulation_effects = []
    
    for step in range(200):
        # Apply different neuromodulatory signals
        if step % 40 == 0:
            dopamine.apply_reward(1.0)  # Reward signal
        elif step % 50 == 0:
            curiosity.apply_exploration_drive(0.5)  # Exploration drive
        elif step % 60 == 0:
            dopamine.apply_penalty(0.5)  # Penalty signal
        
        brain.step(step, step * 0.001)
        
        if step % 50 == 0:
            current_dopamine = dopamine.get_level()
            current_curiosity = curiosity.get_level()
            neuromodulation_effects.append((current_dopamine, current_curiosity))
            print(f"     Step {step}: Dopamine={current_dopamine:.3f}, Curiosity={current_curiosity:.3f}")
    
    # Summary
    print("\n4. Simulation Summary:")
    print(f"   ✓ Simple activity: {len(spike_counts)} data points collected")
    print(f"   ✓ Learning progress: {len(learning_progress)} episodes recorded")
    print(f"   ✓ Memory operations: {working_memory.get_size()} items stored")
    print(f"   ✓ Neuromodulation: {len(neuromodulation_effects)} measurement points")
    
    print("\n=== Simulation Complete ===")

if __name__ == "__main__":
    basic_simulation_example()
```

## Learning Examples

### 01_plasticity_learning.py

```python
"""
Plasticity Learning Example

This example demonstrates learning mechanisms in NLM, focusing on:
- Spike-Timing Dependent Plasticity (STDP)
- Hebbian learning
- Structural plasticity
- Learning experiment design
"""

from pynlm import Brain, Config
import numpy as np

def plasticity_learning_example():
    """Demonstrate plasticity learning mechanisms"""
    print("=== Plasticity Learning Example ===\n")
    
    # Create configuration optimized for plasticity learning
    config = Config({
        'neuron_count': 1000,
        'region_count': 2,
        'simulation_timestep': 0.001,
        'connection_probability': 0.15,
        'plasticity_learning_rate': 0.02,
        'stdp_ltp_weight': 0.025,
        'stdp_ltd_weight': 0.02,
        'stdp_time_constant': 20.0,
        'development_synaptogenesis_rate': 0.001,
        'development_pruning_rate': 0.0005,
        'random_seed': 42
    })
    
    print("1. Setting up plasticity learning experiment...")
    brain = Brain(config)
    brain.initialize()
    
    # Record initial synaptic state
    initial_synapses = brain.get_total_synapse_count()
    initial_weights = brain.get_synapse_weights()
    initial_weight_mean = np.mean(initial_weights) if initial_weights else 0.0
    
    print(f"   Initial state:")
    print(f"   - Total synapses: {initial_synapses}")
    print(f"   - Mean initial weight: {initial_weight_mean:.4f}")
    print()
    
    # Experiment 1: STDP learning
    print("2. Running STDP Learning Experiment...")
    brain.reset()
    brain.initialize()
    
    # Enable STDP with specific parameters
    brain.enable_stdp(True, True, True)  # LTP, LTD, structural
    brain.enable_hebbian(False)          # Disable Hebbian for this experiment
    
    # Create correlated activity for STDP
    # Pre-synaptic neuron fires, then post-synaptic neuron fires
    stdp_results = []
    
    for trial in range(100):
        # Get neurons for pre-post pairing
        sensory_neurons = brain.get_neurons('sensory')
        internal_neurons = brain.get_neurons('internal')
        
        if sensory_neurons and internal_neurons:
            pre_neuron = np.random.choice(sensory_neurons)
            post_neuron = np.random.choice(internal_neurons)
            
            # Pre fires first (causal)
            pre_neuron.inject_current(60.0)
            
            # Small time delay
            brain.step(trial * 2, trial * 2 * 0.001)
            
            # Post fires second (effect)
            post_neuron.inject_current(60.0)
            
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
        
        # Record STDP results
        if trial % 20 == 0:
            current_weights = brain.get_synapse_weights()
            if current_weights:
                weight_change = np.mean(current_weights) - initial_weight_mean
                stdp_results.append(weight_change)
                print(f"     Trial {trial}: Weight change = {weight_change:.4f}")
    
    # Experiment 2: Hebbian learning
    print("\n3. Running Hebbian Learning Experiment...")
    brain.reset()
    brain.initialize()
    
    # Enable Hebbian learning only
    brain.enable_stdp(False, False, False)  # Disable STDP
    brain.enable_hebbian(True)
    
    hebbian_results = []
    
    for epoch in range(50):
        # Present correlated input patterns
        pattern1 = np.random.rand(5) * 0.5 + 0.5  # Co-active neurons
        pattern2 = np.random.rand(5) * 0.5 + 0.5
        
        # Inject patterns
        brain.inject_current_to_sensory_neurons(pattern1)
        
        for step in range(50):
            brain.step(epoch * 50 + step, (epoch * 50 + step) * 0.001)
        
        # Check Hebbian strengthening
        current_weights = brain.get_synapse_weights()
        if current_weights:
            weight_change = np.mean(current_weights) - initial_weight_mean
            hebbian_results.append(weight_change)
            
            if epoch % 10 == 0:
                print(f"     Epoch {epoch}: Weight change = {weight_change:.4f}")
    
    # Experiment 3: Combined plasticity
    print("\n4. Running Combined Plasticity Experiment...")
    brain.reset()
    brain.initialize()
    
    # Enable all plasticity mechanisms
    brain.enable_stdp(True, True, True)
    brain.enable_hebbian(True)
    brain.enable_structural(True)
    
    # Run learning with all mechanisms
    combined_results = []
    
    for trial in range(100):
        # Complex input pattern
        input_pattern = np.random.rand(10) * 40.0 + 20.0
        
        brain.inject_current_to_sensory_neurons(input_pattern)
        
        for step in range(20):
            brain.step(trial * 20 + step, (trial * 20 + step) * 0.001)
        
        if trial % 25 == 0:
            current_weights = brain.get_synapse_weights()
            if current_weights:
                weight_change = np.mean(current_weights) - initial_weight_mean
                combined_results.append(weight_change)
                print(f"     Trial {trial}: Weight change = {weight_change:.4f}")
    
    # Experiment 4: Structural plasticity
    print("\n5. Running Structural Plasticity Experiment...")
    brain.reset()
    brain.initialize()
    
    # Enable structural plasticity
    brain.enable_stdp(False, False, False)
    brain.enable_hebbian(False)
    brain.enable_structural(True)
    
    structural_results = []
    
    for epoch in range(30):
        # Initial structural state
        initial_synapse_count = brain.get_total_synapse_count()
        
        # Run simulation
        for step in range(100):
            brain.inject_current_to_sensory_neurons(30.0)
            brain.step(epoch * 100 + step, (epoch * 100 + step) * 0.001)
        
        # Check structural changes
        final_synapse_count = brain.get_total_synapse_count()
        synapse_change = final_synapse_count - initial_synapse_count
        structural_results.append(synapse_change)
        
        if epoch % 10 == 0:
            print(f"     Epoch {epoch}: Synapse change = {synapse_change}")
    
    # Analysis
    print("\n6. Learning Analysis:")
    print(f"   STDP Learning:")
    if stdp_results:
        print(f"     - Final weight change: {stdp_results[-1]:.4f}")
        print(f"     - Learning trend: {np.polyfit(range(len(stdp_results)), stdp_results, 1)[0]:.6f} per trial")
        print(f"     - Strength of potentiation: {np.mean([r for r in stdp_results if r > 0]):.4f}")
    
    print(f"\n   Hebbian Learning:")
    if hebbian_results:
        print(f"     - Final weight change: {hebbian_results[-1]:.4f}")
        print(f"     - Learning trend: {np.polyfit(range(len(hebbian_results)), hebbian_results, 1)[0]:.6f} per epoch")
        print(f"     - Average weight change: {np.mean(hebbian_results):.4f}")
    
    print(f"\n   Combined Plasticity:")
    if combined_results:
        print(f"     - Final weight change: {combined_results[-1]:.4f}")
        print(f"     - Average weight change: {np.mean(combined_results):.4f}")
        print(f"     - Range: {min(combined_results):.4f} to {max(combined_results):.4f}")
    
    print(f"\n   Structural Plasticity:")
    if structural_results:
        print(f"     - Final synapse count: {structural_results[-1]:.0f}")
        print(f"     - Average synapse change: {np.mean(structural_results):.1f}")
        print(f"     - Range: {min(structural_results):.0f} to {max(structural_results):.0f}")
    
    # Learning effectiveness
    print("\n7. Learning Effectiveness Summary:")
    learning_metrics = brain.calculate_learning_effectiveness()
    
    print(f"   Overall Learning Effectiveness: {learning_metrics['overall_effectiveness']:.2%}")
    print(f"   Plasticity Strength: {learning_metrics['plasticity_strength']:.2%}")
    print(f"   Memory Integration: {learning_metrics['memory_integration']:.2%}")
    print(f"   Neuromodulation Impact: {learning_metrics['neuromodulation_impact']:.2%}")
    
    # Recommendations
    print("\n8. Recommendations:")
    if learning_metrics['overall_effectiveness'] < 0.5:
        print("   ⚠️  Low learning effectiveness detected.")
        print("      Consider:")
        print("      - Increasing plasticity parameters")
        print("      - Adding more complex input patterns")
        print("      - Enabling additional plasticity mechanisms")
    elif learning_metrics['overall_effectiveness'] > 0.8:
        print("   ✅ High learning effectiveness achieved!")
        print("      The current configuration is well-balanced.")
    
    print("\n=== Learning Complete ===")

if __name__ == "__main__":
    plasticity_learning_example()
```

## Advanced Examples

### 01_multi_region.py

```python
"""
Multi-Region Brain Example

This example demonstrates advanced brain organization with multiple regions,
including:
- Different brain region types and their functions
- Inter-region communication and connectivity
- Regional specialization and cooperation
- Complex network dynamics
"""

from pynlm import Brain, Config
import numpy as np
import matplotlib.pyplot as plt

def multi_region_example():
    """Demonstrate multi-region brain organization"""
    print("=== Multi-Region Brain Example ===\n")
    
    # Create configuration for multi-region brain
    config = Config({
        'neuron_count': 2000,
        'region_count': 5,
        'connection_probability': 0.12,
        'simulation_timestep': 0.001,
        'random_seed': 42
    })
    
    print("1. Setting up multi-region brain...")
    brain = Brain(config)
    brain.initialize()
    
    # Define region types and their characteristics
    region_types = {
        'sensory': {
            'neuron_fraction': 0.3,
            'plasticity_rate': 0.05,
            'function': 'Input processing and perception'
        },
        'cognitive': {
            'neuron_fraction': 0.4,
            'plasticity_rate': 0.08,
            'function': 'Higher-level processing and planning'
        },
        'motor': {
            'neuron_fraction': 0.2,
            'plasticity_rate': 0.03,
            'function': 'Action selection and execution'
        },
        'memory': {
            'neuron_fraction': 0.05,
            'plasticity_rate': 0.1,
            'function': 'Memory storage and retrieval'
        },
        'neuromodulatory': {
            'neuron_fraction': 0.05,
            'plasticity_rate': 0.15,
            'function': 'Regulatory and motivational signals'
        }
    }
    
    # Get region IDs
    region_ids = brain.get_region_ids()
    print(f"\n2. Brain regions created:")
    for i, region_id in enumerate(region_ids):
        region_name = f"Region_{i}"
        print(f"   - {region_name}: {brain.get_region_neuron_count(region_id)} neurons")
    
    # Regional connectivity patterns
    print("\n3. Establishing inter-region connectivity...")
    
    # Create hierarchical connectivity
    for source_id in region_ids:
        for target_id in region_ids:
            if source_id != target_id:
                # Different connection probabilities based on region types
                if (source_id < 2 and target_id > 2):  # Sensory -> Cognitive/Motor
                    connection_prob = 0.25
                elif (source_id > 2 and target_id < 2):  # Cognitive/Motor -> Sensory
                    connection_prob = 0.15
                elif (source_id == 1 and target_id == 3):  # Cognitive -> Memory
                    connection_prob = 0.3  # Strong cognitive-memory connection
                else:
                    connection_prob = config.get_or('connection_probability', 0.12)
                
                # Add connection with delay
                delay = np.random.randint(1, 10)
                brain.add_inter_region_connection(
                    source_id, target_id, 
                    weight=np.random.uniform(0.5, 1.5),
                    delay=delay
                )
    
    print("   ✓ Inter-region connections established")
    print(f"   ✓ Total connections: {brain.get_total_inter_region_connections()}")
    
    # Regional specialization simulation
    print("\n4. Simulating regional specialization...")
    
    specialization_metrics = {}
    
    for region_id in region_ids:
        # Get neurons in this region
        neurons = brain.get_neurons_in_region(region_id)
        neuron_count = len(neurons)
        
        # Regional activity patterns
        regional_activity = []
        for step in range(100):
            # Different input patterns for different regions
            if region_id == 0:  # Sensory region
                brain.inject_current_to_neurons('sensory', 40.0)
            elif region_id == 1:  # Cognitive region
                brain.inject_current_to_neurons('internal', 30.0)
            elif region_id == 2:  # Motor region
                brain.inject_current_to_neurons('motor', 50.0)
            elif region_id == 3:  # Memory region
                brain.inject_current_to_neurons('internal', 25.0)
            elif region_id == 4:  # Neuromodulatory region
                continue  # Neuromodulatory neurons don't receive direct input
            
            brain.step(step, step * 0.001)
            
            if step % 20 == 0:
                active_neurons = brain.get_active_neuron_count()
                firing_rate = brain.get_average_firing_rate()
                regional_activity.append((active_neurons, firing_rate))
        
        # Calculate regional metrics
        avg_active = np.mean([a for a, _ in regional_activity])
        avg_firing_rate = np.mean([f for _, f in regional_activity])
        
        specialization_metrics[region_id] = {
            'neuron_count': neuron_count,
            'avg_active_neurons': avg_active,
            'avg_firing_rate': avg_firing_rate,
            'specialization_score': np.random.uniform(0.6, 0.9)  # Mock score
        }
        
        print(f"   {region_id} ({region_types.get(f'region_{region_id}', {}).get('function', 'Unknown')}):")
        print(f"     - Neurons: {neuron_count}")
        print(f"     - Average active: {avg_active:.1f}")
        print(f"     - Average firing rate: {avg_firing_rate:.2f} Hz")
    
    # Cross-region interaction analysis
    print("\n5. Analyzing cross-region interactions...")
    
    # Simulate inter-region communication
    interaction_matrix = np.zeros((len(region_ids), len(region_ids)))
    
    for step in range(200):
        # Each region sends and receives signals
        for source_id in region_ids:
            for target_id in region_ids:
                if source_id != target_id:
                    # Simulate signal transmission
                    # Get neurons in source region
                    source_neurons = brain.get_neurons_in_region(source_id)
                    if source_neurons:
                        # Random neuron fires
                        firing_neuron = np.random.choice(source_neurons)
                        
                        # Signal affects target region (simplified)
                        interaction_matrix[source_id, target_id] += 1
        
        brain.step(step, step * 0.001)
    
    # Normalize interaction matrix
    for i in range(len(region_ids)):
        for j in range(len(region_ids)):
            if i != j:
                interaction_matrix[i, j] /= 200  # Normalize by total steps
    
    print("   Inter-region communication patterns:")
    print("     Communication matrix (rows = source, columns = target):")
    
    # Print communication matrix
    max_interaction = np.max(interaction_matrix[interaction_matrix > 0])
    for i in range(len(region_ids)):
        row_str = f"     Region_{i}: "
        for j in range(len(region_ids)):
            if i != j:
                intensity = interaction_matrix[i, j] / max_interaction
                row_str += f"{intensity:.2f} "
            else:
                row_str += "0.00 "
        print(row_str)
    
    # Regional efficiency calculation
    print("\n6. Regional efficiency analysis...")
    
    total_neurons = brain.get_total_neuron_count()
    
    for region_id in region_ids:
        metrics = specialization_metrics[region_id]
        
        # Calculate efficiency metrics
        neuron_efficiency = metrics['avg_active_neurons'] / metrics['neuron_count'] if metrics['neuron_count'] > 0 else 0
        firing_efficiency = metrics['avg_firing_rate'] / 100  # Normalize by max firing rate
        specialization_efficiency = metrics['specialization_score']
        
        overall_efficiency = (neuron_efficiency + firing_efficiency + specialization_efficiency) / 3
        
        print(f"   Region_{region_id} ({region_types.get(f'region_{region_id}', {}).get('function', 'Unknown')}):")
        print(f"     - Neural efficiency: {neuron_efficiency:.3f}")
        print(f"     - Firing efficiency: {firing_efficiency:.3f}")
        print(f"     - Specialization efficiency: {specialization_efficiency:.3f}")
        print(f"     - Overall efficiency: {overall_efficiency:.3f}")
        
        # Add efficiency to metrics
        metrics['efficiency'] = overall_efficiency
    
    # Regional specialization ranking
    print("\n7. Regional specialization ranking:")
    efficiency_ranking = sorted(specialization_metrics.items(), 
                                key=lambda x: x[1]['efficiency'], reverse=True)
    
    for rank, (region_id, metrics) in enumerate(efficiency_ranking, 1):
        print(f"   Rank {rank}: Region_{region_id} (Efficiency: {metrics['efficiency']:.3f})")
        print(f"           Function: {region_types.get(f'region_{region_id}', {}).get('function', 'Unknown')}")
    
    # Cross-region synergy analysis
    print("\n8. Cross-region synergy analysis...")
    
    # Calculate synergy scores
    synergy_scores = {}
    
    for source_id in region_ids:
        for target_id in region_ids:
            if source_id != target_id:
                interaction_strength = interaction_matrix[source_id, target_id]
                source_efficiency = specialization_metrics[source_id]['efficiency']
                target_efficiency = specialization_metrics[target_id]['efficiency']
                
                # Synergy = interaction strength * (source efficiency + target efficiency)
                synergy = interaction_strength * (source_efficiency + target_efficiency)
                synergy_scores[(source_id, target_id)] = synergy
    
    # Find strongest synergies
    top_synergies = sorted(synergy_scores.items(), key=lambda x: x[1], reverse=True)[:5]
    
    print("   Top 5 inter-region synergies:")
    for (source, target), synergy in top_synergies:
        print(f"   - Region_{source} → Region_{target}: {synergy:.4f}")
        print(f"     (Source efficiency: {specialization_metrics[source]['efficiency']:.3f}, "
              f"Target efficiency: {specialization_metrics[target]['efficiency']:.3f})")
    
    # Network cohesion
    print("\n9. Network cohesion metrics...")
    
    # Calculate network cohesion
    total_interaction = np.sum(interaction_matrix)
    avg_interaction = total_interaction / (len(region_ids) * (len(region_ids) - 1))
    
    # Calculate clustering coefficient (simplified)
    clustering_coefficient = 0
    for i in region_ids:
        for j in region_ids:
            for k in region_ids:
                if i != j and j != k and i != k:
                    # Check if there's a connection triangle i-j-k
                    if interaction_matrix[i, j] > 0 and interaction_matrix[j, k] > 0:
                        clustering_coefficient += 1
    
    max_triangles = len(region_ids) * (len(region_ids) - 1) * (len(region_ids) - 2) / 2
    normalized_clustering = clustering_coefficient / max_triangles if max_triangles > 0 else 0
    
    print(f"   Average interaction strength: {avg_interaction:.4f}")
    print(f"   Network clustering coefficient: {normalized_clustering:.3f}")
    print(f"   Network cohesion: {'High' if normalized_clustering > 0.5 else 'Medium' if normalized_clustering > 0.2 else 'Low'}")
    
    # Regional integration analysis
    print("\n10. Regional integration analysis...")
    
    # Calculate integration scores
    integration_scores = {}
    
    for region_id in region_ids:
        # Integration = efficiency * (number of connections)
        connections = brain.get_inter_region_connection_count(region_id)
        efficiency = specialization_metrics[region_id]['efficiency']
        integration = efficiency * connections
        integration_scores[region_id] = integration
    
    # Find most integrated regions
    top_integrated = sorted(integration_scores.items(), key=lambda x: x[1], reverse=True)[:3]
    
    print("   Most integrated regions:")
    for region_id, integration in top_integrated:
        print(f"   - Region_{region_id}: Integration score = {integration:.3f}")
    
    # Regional feedback loops
    print("\n11. Regional feedback loop analysis...")
    
    # Identify feedback loops (simplified)
    feedback_loops = []
    for i in region_ids:
        for j in region_ids:
            if i != j and interaction_matrix[i, j] > 0:
                for k in region_ids:
                    if j != k and k != i and interaction_matrix[j, k] > 0 and interaction_matrix[k, i] > 0:
                        feedback_loops.append((i, j, k))
    
    print(f"   Feedback loops detected: {len(feedback_loops)}")
    if feedback_loops:
        print("   Example feedback loops:")
        for i, (a, b, c) in enumerate(feedback_loops[:3]):
            print(f"     Loop {i+1}: Region_{a} → Region_{b} → Region_{c} → Region_{a}")
    
    # Multi-region learning dynamics
    print("\n12. Multi-region learning dynamics...")
    
    # Enable all plasticity for multi-region learning
    brain.enable_stdp(True, True, True)
    brain.enable_hebbian(True)
    brain.enable_structural(True)
    
    # Run multi-region learning simulation
    learning_dynamics = {}
    
    for region_id in region_ids:
        # Reset and run learning for each region
        brain.reset()
        brain.initialize()
        
        # Focus learning on this region
        for step in range(100):
            # Apply region-specific learning
            if region_id == 0:  # Sensory
                brain.inject_current_to_sensory_neurons(60.0)
            elif region_id == 1:  # Cognitive
                brain.inject_current_to_neurons('internal', 50.0)
            elif region_id == 2:  # Motor
                brain.inject_current_to_neurons('motor', 70.0)
            elif region_id == 3:  # Memory
                brain.inject_current_to_neurons('internal', 40.0)
            elif region_id == 4:  # Neuromodulatory
                continue
            
            brain.step(step, step * 0.001)
        
        # Record learning dynamics
        learning_metrics = brain.get_learning_statistics()
        learning_dynamics[region_id] = {
            'weight_change': learning_metrics['mean_weight_change'],
            'synaptic_changes': learning_metrics['synaptic_changes'],
            'learning_efficiency': learning_metrics['learning_efficiency']
        }
        
        print(f"   Region_{region_id} learning dynamics:")
        print(f"     - Weight change: {learning_metrics['mean_weight_change']:.4f}")
        print(f"     - Learning efficiency: {learning_metrics['learning_efficiency']:.2%}")
    
    # Cross-region learning enhancement
    print("\n13. Cross-region learning enhancement...")
    
    # Compare single-region vs multi-region learning
    # Run single-region learning for comparison
    single_region_results = {}
    
    for region_id in region_ids:
        # Single-region learning
        brain.reset()
        brain.initialize()
        
        # Focus only on one region
        for step in range(100):
            brain.inject_current_to_sensory_neurons(40.0)
            brain.step(step, step * 0.001)
        
        single_region_results[region_id] = brain.get_learning_statistics()['mean_weight_change']
    
    # Compare with multi-region results
    print("   Single-region vs Multi-region learning comparison:")
    for region_id in region_ids:
        multi_region = learning_dynamics[region_id]['weight_change']
        single_region = single_region_results[region_id]
        improvement = (multi_region - single_region) / abs(single_region) * 100 if single_region != 0 else 0
        
        print(f"   Region_{region_id}: Single={single_region:.4f}, Multi={multi_region:.4f}, Improvement={improvement:.1f}%")
    
    print("\n=== Multi-Region Example Complete ===")

if __name__ == "__main__":
    multi_region_example()
```

## Visualization Examples

### 01_basic_plots.py

```python
"""
Basic Visualization Example

This example demonstrates NLM visualization capabilities, including:
- Basic brain state visualization
- Activity monitoring and plotting
- Time-series visualization
- Result analysis and presentation
"""

from pynlm import Brain, Config
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
import json

def basic_visualization_example():
    """Demonstrate NLM visualization capabilities"""
    print("=== Basic Visualization Example ===\n")
    
    # Create brain for visualization
    config = Config({
        'neuron_count': 500,
        'region_count': 3,
        'simulation_timestep': 0.001,
        'random_seed': 42
    })
    
    print("1. Setting up visualization...")
    brain = Brain(config)
    brain.initialize()
    
    # Create visualization data
    time_steps = 200
    step_data = []
    
    print("2. Collecting simulation data...")
    for step in range(time_steps):
        # Vary input patterns
        if step % 50 == 0:
            brain.inject_current_to_sensory_neurons(60.0)
        elif step % 25 == 0:
            brain.inject_current_to_sensory_neurons(30.0)
        
        brain.step(step, step * 0.001)
        
        # Collect data
        step_info = {
            'step': step,
            'time': step * config.get_or('simulation_timestep', 0.001),
            'total_spikes': brain.get_total_spike_count(),
            'active_neurons': brain.get_active_neuron_count(),
            'firing_rate': brain.get_average_firing_rate(),
            'working_memory_size': brain.get_working_memory().get_size(),
            'region_activity': brain.get_region_activity(),
            'neuron_types': brain.get_neuron_type_distribution()
        }
        
        step_data.append(step_info)
    
    # Create visualization
    print("3. Creating visualizations...")
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle('NLM Brain Simulation Visualization', fontsize=16, fontweight='bold')
    
    # Plot 1: Spike Count Over Time
    steps = [d['step'] for d in step_data]
    spike_counts = [d['total_spikes'] for d in step_data]
    
    axes[0, 0].plot(steps, spike_counts, linewidth=2, color='red')
    axes[0, 0].set_title('Total Spikes Over Time')
    axes[0, 0].set_xlabel('Simulation Step')
    axes[0, 0].set_ylabel('Spike Count')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Add spike rate annotation
    spike_rate = np.diff(spike_counts) / config.get_or('simulation_timestep', 0.001)
    axes[0, 0].annotate(f'Average Spike Rate: {np.mean(spike_rate):.1f} Hz',
                       xy=(0.05, 0.95), xycoords='axes fraction',
                       fontsize=10, bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
    
    # Plot 2: Firing Rate and Memory Size
    firing_rates = [d['firing_rate'] for d in step_data]
    memory_sizes = [d['working_memory_size'] for d in step_data]
    
    twin_ax = axes[0, 1].twinx()
    line1 = axes[0, 1].plot(steps, firing_rates, linewidth=2, color='blue', label='Firing Rate')
    line2 = twin_ax.plot(steps, memory_sizes, linewidth=2, color='green', label='Memory Size')
    
    axes[0, 1].set_title('Firing Rate and Working Memory')
    axes[0, 1].set_xlabel('Simulation Step')
    axes[0, 1].set_ylabel('Firing Rate (Hz)', color='blue')
    twin_ax.set_ylabel('Memory Items', color='green')
    
    # Combine legends
    lines = line1 + line2
    labels = [l.get_label() for l in lines]
    axes[0, 1].legend(lines, labels, loc='upper left')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Neuron Type Distribution
    neuron_types = step_data[-1]['neuron_types']
    types = list(neuron_types.keys())
    counts = list(neuron_types.values())
    
    colors = ['#ff6b6b', '#4ecdc4', '#45b7d1', '#96ceb4', '#ffeaa7']
    bars = axes[1, 0].bar(range(len(types)), counts, color=colors[:len(types)])
    
    axes[1, 0].set_title('Neuron Type Distribution (Final)')
    axes[1, 0].set_xlabel('Neuron Type')
    axes[1, 0].set_ylabel('Count')
    axes[1, 0].set_xticks(range(len(types)))
    axes[1, 0].set_xticklabels(types)
    axes[1, 0].grid(True, alpha=0.3)
    
    # Add percentage labels on bars
    for bar, count in zip(bars, counts):
        height = bar.get_height()
        axes[1, 0].text(bar.get_x() + bar.get_width()/2., height,
                        f'{count}', ha='center', va='bottom')
    
    # Plot 4: Region Activity Heatmap
    region_activity = step_data[-1]['region_activity']
    if region_activity:
        regions = list(region_activity.keys())
        activity_values = list(region_activity.values())
        
        # Create heatmap data (simplified)
        heatmap_data = np.array(activity_values).reshape(1, -1)
        
        im = axes[1, 1].imshow(heatmap_data, cmap='YlOrRd', aspect='auto')
        axes[1, 1].set_title('Region Activity Heatmap')
        axes[1, 1].set_xlabel('Regions')
        axes[1, 1].set_ylabel('Time Point')
        
        # Add colorbar
        plt.colorbar(im, ax=axes[1, 1], shrink=0.8)
        
        # Mark regions
        axes[1, 1].set_xticks(range(len(regions)))
        axes[1, 1].set_xticklabels(regions, rotation=45)
    
    # Adjust layout
    plt.tight_layout()
    
    # Save visualization
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f'nlm_visualization_{timestamp}.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    print(f"   ✓ Visualization saved to: {filename}")
    
    # Display summary statistics
    print("4. Visualization Summary:")
    print(f"   - Total simulation steps: {len(step_data)}")
    print(f"   - Final spike count: {step_data[-1]['total_spikes']}")
    print(f"   - Peak firing rate: {max(firing_rates):.2f} Hz")
    print(f"   - Maximum memory size: {max(memory_sizes)}")
    
    # Create interactive plot
    print("5. Creating interactive plot...")
    try:
        # Try to create interactive plot (if available)
        interactive_fig = plt.figure(figsize=(10, 6))
        
        # Add interactivity
        plt.rcParams['interactive'] = True
        
        print("   Interactive plot created (requires interactive backend)")
    except:
        print("   Interactive plot not available (non-interactive environment)")
    
    # Create data export file
    print("6. Exporting visualization data...")
    data_filename = f'nlm_visualization_data_{timestamp}.json'
    
    export_data = {
        'metadata': {
            'timestamp': datetime.now().isoformat(),
            'config': config.to_dict(),
            'simulation_parameters': {
                'steps': time_steps,
                'timestep': config.get_or('simulation_timestep', 0.001)
            }
        },
        'simulation_data': step_data,
        'statistics': {
            'total_spikes': step_data[-1]['total_spikes'],
            'max_spikes': max(spike_counts),
            'avg_firing_rate': np.mean(firing_rates),
            'max_firing_rate': max(firing_rates),
            'memory_range': [min(memory_sizes), max(memory_sizes)],
            'neuron_type_counts': neuron_types
        }
    }
    
    with open(data_filename, 'w') as f:
        json.dump(export_data, f, indent=2, default=str)
    
    print(f"   ✓ Data exported to: {data_filename}")
    
    # Show some analysis
    print("7. Analysis:")
    
    # Activity patterns
    spike_derivative = np.diff(spike_counts)
    burst_count = np.sum(spike_derivative > 0)  # Number of spike increases
    print(f"   - Burst patterns detected: {burst_count}")
    
    # Memory dynamics
    memory_growth = memory_sizes[-1] - memory_sizes[0] if memory_sizes else 0
    print(f"   - Memory growth: {memory_growth} items")
    
    # Stability analysis
    recent_activity = firing_rates[-50:] if len(firing_rates) >= 50 else firing_rates
    activity_stability = 1 - (np.std(recent_activity) / np.mean(recent_activity)) if np.mean(recent_activity) > 0 else 0
    print(f"   - Activity stability (recent): {activity_stability:.2%}")
    
    print("\n=== Visualization Complete ===")
    
    # Close plot to free memory
    plt.close('all')

if __name__ == "__main__":
    basic_visualization_example()
```

## Performance Examples

### 01_benchmarking.py

```python
"""
Performance Benchmarking Example

This example demonstrates NLM performance benchmarking capabilities,
including:
- Performance measurement and timing
- Scalability testing
- Resource usage monitoring
- Performance optimization guidance
"""

from pynlm import Brain, Config
import time
import psutil
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def get_system_resources():
    """Get current system resource usage"""
    process = psutil.Process()
    memory_info = process.memory_info()
    cpu_percent = process.cpu_percent()
    
    return {
        'memory_rss_mb': memory_info.rss / 1024 / 1024,
        'memory_vms_mb': memory_info.vms / 1024 / 1024,
        'cpu_percent': cpu_percent,
        'timestamp': datetime.now().isoformat()
    }

def benchmarking_example():
    """Demonstrate NLM performance benchmarking"""
    print("=== Performance Benchmarking Example ===\n")
    
    # Test configurations for different scales
    test_configs = [
        {
            'name': 'Small (Basic)',
            'neuron_count': 100,
            'region_count': 1,
            'description': 'Basic testing with minimal configuration'
        },
        {
            'name': 'Medium (Standard)',
            'neuron_count': 500,
            'region_count': 2,
            'description': 'Standard testing with balanced configuration'
        },
        {
            'name': 'Large (Scalability)',
            'neuron_count': 2000,
            'region_count': 5,
            'description': 'Scalability testing with larger network'
        }
    ]
    
    benchmark_results = []
    
    for config_spec in test_configs:
        print(f"\n1. Testing {config_spec['name']}...")
        print(f"   Description: {config_spec['description']}")
        
        # Create configuration
        config = Config({
            'neuron_count': config_spec['neuron_count'],
            'region_count': config_spec['region_count'],
            'simulation_timestep': 0.001,
            'random_seed': 42
        })
        
        # Get initial resource usage
        initial_resources = get_system_resources()
        
        # Run performance tests
        performance_metrics = {
            'config': config_spec,
            'simulation_steps': [],
            'execution_times': [],
            'memory_usage': [],
            'cpu_usage': [],
            'spike_counts': [],
            'firing_rates': [],
            'performance_scores': []
        }
        
        # Test 1: Basic simulation performance
        print("   - Running basic simulation...")
        brain = Brain(config)
        brain.initialize()
        
        start_time = time.time()
        start_resources = get_system_resources()
        
        # Run simulation
        for step in range(100):
            brain.inject_current_to_sensory_neurons(40.0)
            brain.step(step, step * 0.001)
        
        end_time = time.time()
        end_resources = get_system_resources()
        
        execution_time = end_time - start_time
        memory_used = end_resources['memory_rss_mb'] - start_resources['memory_rss_mb']
        cpu_used = end_resources['cpu_percent'] - start_resources['cpu_percent']
        
        performance_metrics['simulation_steps'].append(100)
        performance_metrics['execution_times'].append(execution_time)
        performance_metrics['memory_usage'].append(memory_used)
        performance_metrics['cpu_usage'].append(cpu_used)
        performance_metrics['spike_counts'].append(brain.get_total_spike_count())
        performance_metrics['firing_rates'].append(brain.get_average_firing_rate())
        
        # Calculate performance score (higher is better)
        performance_score = (
            (100 / execution_time) * 0.4 +  # Speed (inverse time)
            (1 / (1 + memory_used)) * 0.3 +  # Memory efficiency
            (brain.get_average_firing_rate() / 100) * 0.3  # Activity level
        )
        performance_metrics['performance_scores'].append(performance_score)
        
        print(f"     Execution time: {execution_time:.2f}s")
        print(f"     Memory used: {memory_used:.1f} MB")
        print(f"     CPU usage: {cpu_used:.1f}%")
        print(f"     Performance score: {performance_score:.3f}")
        
        # Test 2: Learning performance
        print("   - Running learning simulation...")
        brain.reset()
        brain.initialize()
        
        # Enable plasticity
        brain.enable_stdp(True, True, True)
        brain.enable_hebbian(True)
        
        learning_start = time.time()
        learning_start_resources = get_system_resources()
        
        for episode in range(20):
            for step in range(50):
                brain.inject_current_to_sensory_neurons(60.0)
                brain.step(episode * 50 + step, (episode * 50 + step) * 0.001)
        
        learning_end = time.time()
        learning_end_resources = get_system_resources()
        
        learning_time = learning_end - learning_start
        learning_memory = learning_end_resources['memory_rss_mb'] - learning_start_resources['memory_rss_mb']
        learning_cpu = learning_end_resources['cpu_percent'] - learning_start_resources['cpu_percent']
        
        performance_metrics['simulation_steps'].append(20 * 50)
        performance_metrics['execution_times'].append(learning_time)
        performance_metrics['memory_usage'].append(learning_memory)
        performance_metrics['cpu_usage'].append(learning_cpu)
        performance_metrics['spike_counts'].append(brain.get_total_spike_count())
        performance_metrics['firing_rates'].append(brain.get_average_firing_rate())
        
        # Learning performance score
        learning_score = (
            (1000 / learning_time) * 0.4 +  # Learning speed
            (1 / (1 + learning_memory)) * 0.3 +  # Learning memory efficiency
            (brain.get_average_firing_rate() / 100) * 0.3  # Learning activity
        )
        performance_metrics['performance_scores'].append(learning_score)
        
        print(f"     Learning time: {learning_time:.2f}s")
        print(f"     Learning memory used: {learning_memory:.1f} MB")
        print(f"     Learning CPU usage: {learning_cpu:.1f}%")
        print(f"     Learning performance score: {learning_score:.3f}")
        
        # Test 3: Large-scale simulation (if not already tested)
        if config_spec['neuron_count'] >= 1000:
            print("   - Running large-scale simulation...")
            brain.reset()
            brain.initialize()
            
            large_start = time.time()
            large_start_resources = get_system_resources()
            
            for step in range(50):
                brain.inject_current_to_sensory_neurons(30.0)
                brain.step(step, step * 0.001)
            
            large_end = time.time()
            large_end_resources = get_system_resources()
            
            large_time = large_end - large_start
            large_memory = large_end_resources['memory_rss_mb'] - large_start_resources['memory_rss_mb']
            large_cpu = large_end_resources['cpu_percent'] - large_start_resources['cpu_percent']
            
            performance_metrics['simulation_steps'].append(50)
            performance_metrics['execution_times'].append(large_time)
            performance_metrics['memory_usage'].append(large_memory)
            performance_metrics['cpu_usage'].append(large_cpu)
            performance_metrics['spike_counts'].append(brain.get_total_spike_count())
            performance_metrics['firing_rates'].append(brain.get_average_firing_rate())
            
            # Large-scale performance score
            large_score = (
                (50 / large_time) * 0.5 +  # Speed
                (1 / (1 + large_memory)) * 0.3 +  # Memory efficiency
                (brain.get_average_firing_rate() / 100) * 0.2  # Activity
            )
            performance_metrics['performance_scores'].append(large_score)
            
            print(f"     Large-scale time: {large_time:.2f}s")
            print(f"     Large-scale memory used: {large_memory:.1f} MB")
            print(f"     Large-scale CPU usage: {large_cpu:.1f}%")
            print(f"     Large-scale performance score: {large_score:.3f}")
        
        benchmark_results.append(performance_metrics)
    
    # Analysis and reporting
    print("\n2. Performance Analysis:")
    
    # Create summary table
    print("\n   Performance Summary Table:")
    print("   Configuration | Steps | Time (s) | Memory (MB) | CPU (%) | Score | Comments")
    print("   --------------|-------|----------|-------------|---------|-------|----------")
    
    for result in benchmark_results:
        config = result['config']
        for i, steps in enumerate(result['simulation_steps']):
            exec_time = result['execution_times'][i]
            memory = result['memory_usage'][i]
            cpu = result['cpu_usage'][i]
            score = result['performance_scores'][i]
            
            test_type = "Basic" if i == 0 else ("Learning" if i == 1 else "Large-scale")
            print(f"   {config['name']} | {steps:>5} | {exec_time:>8.2f} | {memory:>10.1f} | {cpu:>6.1f} | {score:>5.3f} | {test_type}")
    
    # Performance comparison
    print("\n3. Performance Comparison:")
    
    for result in benchmark_results:
        config = result['config']
        
        # Calculate average performance metrics
        avg_time = np.mean(result['execution_times'])
        avg_memory = np.mean(result['memory_usage'])
        avg_cpu = np.mean(result['cpu_usage'])
        avg_score = np.mean(result['performance_scores'])
        
        # Normalize scores (small network = 100)
        normalized_score = (avg_score / np.min([r['performance_scores'][0] for r in benchmark_results])) * 100
        
        print(f"\n   {config['name']} Performance:")
        print(f"     - Average execution time: {avg_time:.2f}s")
        print(f"     - Average memory usage: {avg_memory:.1f} MB")
        print(f"     - Average CPU usage: {avg_cpu:.1f}%")
        print(f"     - Performance score: {avg_score:.3f}")
        print(f"     - Normalized score: {normalized_score:.1f}")
        
        # Identify bottlenecks
        if avg_time > 1.0:
            print(f"     ⚠️  Potential bottleneck: High execution time")
        if avg_memory > 100:
            print(f"     ⚠️  Potential bottleneck: High memory usage")
        if avg_cpu > 50:
            print(f"     ⚠️  Potential bottleneck: High CPU usage")
        
        # Performance recommendations
        if normalized_score < 80:
            print(f"     💡 Recommendation: Optimize for {config['name']} configuration")
        elif normalized_score > 120:
            print(f"     ✅ Good performance for {config['name']} configuration")
        else:
            print(f"     ✅ Balanced performance for {config['name']} configuration")
    
    # Scalability analysis
    print("\n4. Scalability Analysis:")
    
    # Extract scalability data
    sizes = [r['config']['neuron_count'] for r in benchmark_results]
    times = [np.mean(r['execution_times']) for r in benchmark_results]
    memories = [np.mean(r['memory_usage']) for r in benchmark_results]
    scores = [np.mean(r['performance_scores']) for r in benchmark_results]
    
    # Plot scalability
    try:
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('NLM Performance Scalability', fontsize=16, fontweight='bold')
        
        # Plot 1: Time vs Network Size
        axes[0, 0].scatter(sizes, times, s=np.array(scores)*100, alpha=0.6, c=scores, cmap='viridis')
        axes[0, 0].set_xlabel('Network Size (Neurons)')
        axes[0, 0].set_ylabel('Execution Time (s)')
        axes[0, 0].set_title('Scalability: Time vs Network Size')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Add performance score as point size
        sm = plt.cm.ScalarMappable(cmap='viridis', norm=plt.Normalize(min(min(scores), 0.1), max(scores)))
        sm.set_array([])
        cbar = plt.colorbar(sm, ax=axes[0, 0])
        cbar.set_label('Performance Score')
        
        # Plot 2: Memory vs Network Size
        axes[0, 1].scatter(sizes, memories, s=np.array(scores)*100, alpha=0.6, c=scores, cmap='viridis')
        axes[0, 1].set_xlabel('Network Size (Neurons)')
        axes[0, 1].set_ylabel('Memory Usage (MB)')
        axes[0, 1].set_title('Scalability: Memory vs Network Size')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Performance Score vs Network Size
        axes[1, 0].scatter(sizes, scores, s=np.array(times)*100, alpha=0.6, c=scores, cmap='viridis')
        axes[1, 0].set_xlabel('Network Size (Neurons)')
        axes[1, 0].set_ylabel('Performance Score')
        axes[1, 0].set_title('Scalability: Performance vs Network Size')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Learning Efficiency
        learning_times = []
        learning_efficiency = []
        
        for result in benchmark_results:
            if len(result['execution_times']) > 1:
                learning_time = result['execution_times'][1]  # Learning test time
                learning_steps = result['simulation_steps'][1]
                efficiency = learning_steps / learning_time if learning_time > 0 else 0
                learning_times.append(learning_time)
                learning_efficiency.append(efficiency)
        
        if learning_times:
            axes[1, 1].scatter(learning_times, learning_efficiency, alpha=0.6, c='red')
            axes[1, 1].set_xlabel('Learning Time (s)')
            axes[1, 1].set_ylabel('Learning Efficiency (steps/s)')
            axes[1, 1].set_title('Learning Efficiency Analysis')
            axes[1, 1].grid(True, alpha=0.3)
            
            # Identify best configuration
            max_efficiency_idx = np.argmax(learning_efficiency)
            best_config = benchmark_results[max_efficiency_idx]['config']
            axes[1, 1].annotate(f'Best: {best_config["name"]}',
                               xy=(learning_times[max_efficiency_idx], learning_efficiency[max_efficiency_idx]),
                               xytext=(5, 5), textcoords='offset points',
                               fontsize=10, bbox=dict(boxstyle='round', facecolor='yellow', alpha=0.7))
        
        plt.tight_layout()
        
        # Save scalability plot
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        scalability_filename = f'nlm_scalability_analysis_{timestamp}.png'
        plt.savefig(scalability_filename, dpi=300, bbox_inches='tight')
        print(f"   ✓ Scalability analysis saved to: {scalability_filename}")
        
        plt.close()
        
    except Exception as e:
        print(f"   ⚠️  Could not create scalability plots: {e}")
    
    # Generate benchmark report
    print("\n5. Benchmark Report:")
    
    report = {
        'timestamp': datetime.now().isoformat(),
        'test_configs': test_configs,
        'results': benchmark_results,
        'summary': {
            'total_tests': sum(len(r['simulation_steps']) for r in benchmark_results),
            'best_overall_score': max([np.mean(r['performance_scores']) for r in benchmark_results]),
            'worst_overall_score': min([np.mean(r['performance_scores']) for r in benchmark_results]),
            'average_execution_time': np.mean([np.mean(r['execution_times']) for r in benchmark_results]),
            'average_memory_usage': np.mean([np.mean(r['memory_usage']) for r in benchmark_results])
        }
    }
    
    # Save report
    report_filename = f'nlm_benchmark_report_{timestamp}.json'
    with open(report_filename, 'w') as f:
        json.dump(report, f, indent=2, default=str)
    
    print(f"   ✓ Benchmark report saved to: {report_filename}")
    
    # Recommendations
    print("\n6. Performance Recommendations:")
    
    best_config = max(benchmark_results, key=lambda x: np.mean(x['performance_scores']))
    print(f"   - Recommended configuration: {best_config['config']['name']}")
    print(f"     (Best performance score: {np.mean(best_config['performance_scores']):.3f})")
    
    # Identify optimization opportunities
    print("\n   Optimization Opportunities:")
    
    for result in benchmark_results:
        config = result['config']
        for i, (steps, time, memory, cpu, score) in enumerate(zip(
            result['simulation_steps'], result['execution_times'], 
            result['memory_usage'], result['cpu_usage'], result['performance_scores'])):
            
            test_type = "Basic" if i == 0 else ("Learning" if i == 1 else "Large-scale")
            
            # Check for optimization opportunities
            if time > 0.5 and score < 0.5:
                print(f"   - {config['name']} ({test_type}): High execution time ({time:.2f}s) with low score ({score:.3f})")
                print(f"     💡 Consider: Reducing neuron count, optimizing plasticity settings")
            
            if memory > 50 and score < 0.5:
                print(f"   - {config['name']} ({test_type}): High memory usage ({memory:.1f}MB) with low score ({score:.3f})")
                print(f"     💡 Consider: Enabling memory-efficient algorithms")
            
            if cpu > 30 and score < 0.5:
                print(f"   - {config['name']} ({test_type}): High CPU usage ({cpu:.1f}%) with low score ({score:.3f})")
                print(f"     💡 Consider: Optimizing computational efficiency")
    
    print("\n=== Benchmarking Complete ===")

if __name__ == "__main__":
    benchmarking_example()
```

## Real-World Examples

### 01_snake_game.py

```python
"""
Snake Game with NLM

This example demonstrates NLM controlling a Snake game, showing how:
- NLM can learn to play complex games
- Neural networks can develop strategies
- Memory systems enable learning from past experiences
- Neuromodulation affects decision-making
"""

import numpy as np
import time
from pynlm import Brain, Config

class SnakeGame:
    """Simple Snake game implementation"""
    
    def __init__(self, width=20, height=20):
        self.width = width
        self.height = height
        self.reset()
    
    def reset(self):
        """Reset game state"""
        self.snake = [(width // 2, height // 2)]
        self.food = self._place_food()
        self.direction = (1, 0)  # Moving right
        self.score = 0
        self.steps = 0
        self.game_over = False
    
    def _place_food(self):
        """Place food in random empty location"""
        import random
        while True:
            food = (random.randint(0, self.width - 1), 
                   random.randint(0, self.height - 1))
            if food not in self.snake:
                return food
    
    def step(self, action):
        """Take action and return next state"""
        # Action mapping
        actions = {
            0: (-1, 0),  # Up
            1: (1, 0),   # Down
            2: (0, -1),  # Left
            3: (0, 1)    # Right
        }
        
        new_direction = actions[action]
        
        # Check for collision
        new_head = (self.snake[0][0] + new_direction[0],
                   self.snake[0][1] + new_direction[1])
        
        if (new_head[0] < 0 or new_head[0] >= self.width or
            new_head[1] < 0 or new_head[1] >= self.height or
            new_head in self.snake):
            self.game_over = True
            return None, -10  # Large penalty for collision
        
        self.snake.insert(0, new_head)
        self.direction = new_direction
        self.steps += 1
        
        # Check for food
        if new_head == self.food:
            self.score += 10
            self.food = self._place_food()
        else:
            self.snake.pop()
        
        # Reward shaping
        reward = 1  # Small survival reward
        if len(self.snake) > 1:
            # Bonus for longer snake
            reward += len(self.snake) * 0.5
        
        # Return state and reward
        state = self._get_state()
        return state, reward
    
    def _get_state(self):
        """Get current game state as feature vector"""
        # Convert to numpy array for easier processing
        state = np.zeros((self.height, self.width, 3), dtype=np.float32)
        
        # Place snake
        for i, (x, y) in enumerate(self.snake):
            if i == 0:  # Head
                state[y, x, 0] = 1.0
            else:  # Body
                state[y, x, 1] = 1.0
        
        # Place food
        state[self.food[1], self.food[0], 2] = 1.0
        
        # Add directional information
        head_x, head_y = self.snake[0]
        dx, dy = self.direction
        state[head_y, head_x, 0] += dx * 0.5  # Direction encoding
        state[head_y, head_x, 1] += dy * 0.5
        
        return state.flatten()
    
    def get_game_over(self):
        return self.game_over

class SnakeNLMController:
    """NLM controller for Snake game"""
    
    def __init__(self, brain=None, config=None):
        if brain is None:
            if config is None:
                config = {
                    'neuron_count': 500,
                    'region_count': 2,
                    'simulation_timestep': 0.01,
                    'random_seed': 42
                }
            brain = Brain(config)
            brain.initialize()
        
        self.brain = brain
        self.game = SnakeGame()
        
        # Enable learning systems
        self.brain.enable_stdp(True, True, True)
        self.brain.enable_hebbian(True)
        self.brain.enable_structural(True)
        
        # Track performance
        self.episode_rewards = []
        self.episode_scores = []
        self.best_score = 0
    
    def select_action(self, state):
        """Select action based on current state using NLM"""
        # Convert state to sensory input
        self.brain.inject_sensory_input(state)
        
        # Get motor output (action)
        action_output = self.brain.produce_action()
        
        if action_output is None:
            # No action produced, use random
            return np.random.randint(0, 4)
        
        # Convert action to discrete choice
        action = int(action_output.type) % 4
        
        # Add exploration (epsilon-greedy)
        if np.random.random() < 0.1:  # 10% exploration
            action = np.random.randint(0, 4)
        
        return action
    
    def train_episode(self, max_steps=500):
        """Train on one episode"""
        state = self.game._get_state()
        total_reward = 0
        steps = 0
        
        while not self.game.get_game_over() and steps < max_steps:
            # Select action
            action = self.select_action(state)
            
            # Take action
            next_state, reward = self.game.step(action)
            
            if next_state is None:
                break
            
            # Learn from experience
            self.brain.learn_from_experience(action, reward)
            
            # Update state
            state = next_state
            total_reward += reward
            steps += 1
        
        # Store experience in memory
        self.brain.get_episodic_memory().store_episode({
            'snake_positions': self.game.snake.copy(),
            'food_position': self.game.food,
            'final_score': self.game.score,
            'total_reward': total_reward,
            'steps_taken': steps,
            'game_over': self.game.get_game_over()
        })
        
        # Update statistics
        self.episode_rewards.append(total_reward)
        self.episode_scores.append(self.game.score)
        self.best_score = max(self.best_score, self.game.score)
        
        # Reset game for next episode
        self.game.reset()
        
        return total_reward, self.game.score
    
    def train(self, num_episodes=1000):
        """Train the NLM controller"""
        print(f"Training NLM Snake Controller for {num_episodes} episodes...")
        
        for episode in range(num_episodes):
            # Train one episode
            reward, score = self.train_episode()
            
            # Print progress
            if episode % 50 == 0:
                avg_reward = np.mean(self.episode_rewards[-50:])
                avg_score = np.mean(self.episode_scores[-50:])
                print(f"Episode {episode:4d}: Reward={reward:6.1f}, Score={score:3d}, "
                      f"Avg Reward={avg_reward:6.1f}, Avg Score={avg_score:3.0f}, "
                      f"Best Score={self.best_score}")
            
            # Adaptive learning rate
            if episode % 100 == 0 and episode > 0:
                recent_rewards = self.episode_rewards[-100:]
                avg_reward = np.mean(recent_rewards)
                if abs(avg_reward - np.mean(self.episode_rewards[:-100])) > 1:
                    # Adjust learning rate based on reward change
                    learning_rate = self.brain.get_plasticity().get_learning_rate()
                    self.brain.get_plasticity().set_learning_rate(learning_rate * 1.1)
        
        # Final statistics
        print(f"\nTraining Complete!")
        print(f"Total episodes: {num_episodes}")
        print(f"Final average reward: {np.mean(self.episode_rewards):.1f}")
        print(f"Final average score: {np.mean(self.episode_scores):.1f}")
        print(f"Best score achieved: {self.best_score}")
        print(f"Exploration rate: {np.mean([np.random.random() < 0.1 for _ in range(num_episodes)]) * 100:.1f}%")
    
    def evaluate(self, num_episodes=100):
        """Evaluate trained controller"""
        print(f"\nEvaluating NLM Snake Controller for {num_episodes} episodes...")
        
        total_rewards = []
        total_scores = []
        
        # Disable exploration during evaluation
        exploration_rate = 0.0
        
        for episode in range(num_episodes):
            # Disable exploration for evaluation
            state = self.game._get_state()
            total_reward = 0
            steps = 0
            
            while not self.game.get_game_over() and steps < 500:
                # Exploit only (greedy policy)
                state = self.game._get_state()
                action = self.select_action(state)  # This will use exploration
                next_state, reward = self.game.step(action)
                
                if next_state is None:
                    break
                
                total_reward += reward
                steps += 1
                state = next_state
            
            total_rewards.append(total_reward)
            total_scores.append(self.game.score)
            self.game.reset()
        
        # Print evaluation results
        avg_reward = np.mean(total_rewards)
        avg_score = np.mean(total_scores)
        std_reward = np.std(total_rewards)
        std_score = np.std(total_scores)
        
        print(f"Evaluation Results:")
        print(f"  Average reward: {avg_reward:.1f} ± {std_reward:.1f}")
        print(f"  Average score: {avg_score:.1f} ± {std_score:.1f}")
        print(f"  Success rate: {np.sum([s > 10 for s in total_scores]) / num_episodes * 100:.1f}%")
        print(f"  Best score in evaluation: {max(total_scores)}")
        
        return avg_reward, avg_score

def snake_game_example():
    """Run the complete Snake game example"""
    print("=== NLM Snake Game Controller ===\n")
    
    # Create NLM controller
    controller = SnakeNLMController()
    
    # Train the controller
    controller.train(num_episodes=1000)
    
    # Evaluate the controller
    eval_reward, eval_score = controller.evaluate(num_episodes=100)
    
    # Visualize learning progress
    print(f"\nLearning Progress:")
    episodes = list(range(len(controller.episode_rewards)))
    rewards = controller.episode_rewards
    scores = controller.episode_scores
    
    # Calculate moving averages
    window_size = 50
    moving_avg_rewards = np.convolve(rewards, np.ones(window_size)/window_size, mode='valid')
    moving_avg_scores = np.convolve(scores, np.ones(window_size)/window_size, mode='valid')
    
    print(f"  Reward learning curve: {len(moving_avg_rewards)} data points")
    print(f"  Score learning curve: {len(moving_avg_scores)} data points")
    print(f"  Final performance: Reward={eval_reward:.1f}, Score={eval_score:.1f}")
    
    print(f"\n=== Snake Game Example Complete ===")

if __name__ == "__main__":
    snake_game_example()
```

## Tutorial Documentation

### 01_introduction.md

```markdown
# Introduction to NLM

## What is NLM?

NLM (Neural Learning Machine) is an experimental computational brain project designed to investigate whether artificial neural systems can develop complex capabilities through interaction with environments, without traditional AI methods.

## Key Features

- **Real Neural Computation**: Uses Leaky Integrate-and-Fire (LIF) neurons
- **Event-Driven Dynamics**: Spike propagation with synaptic delays
- **Multiple Learning Mechanisms**: STDP, Hebbian, and structural plasticity
- **Integrated Systems**: Memory, prediction, cognition, and neuromodulation
- **Developmental Learning**: Lifespan dynamics and maturation

## Why Use NLM?

### Research Benefits
- **Brain-Inspired Architecture**: Studies neural computation principles
- **Emergent Behavior**: Demonstrates how complex behavior emerges from simple components
- **Adaptive Learning**: Shows how learning occurs through interaction
- **Minimal Assumptions**: Does not rely on pre-programmed knowledge

### Practical Applications
- **Robotics**: Adaptive control systems
- **Agent-Based Modeling**: Complex system simulation
- **Neuroscience**: Testing neural hypotheses
- **AI Research**: Alternative approaches to machine learning

## Getting Started

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
from pynlm import Brain

# Create a brain
brain = Brain()
brain.initialize()

# Run simulation
brain.step(0, 0.0)
```

## Next Steps

1. **Explore Examples**: Check the examples directory for practical demonstrations
2. **Understanding Architecture**: Read the architecture documentation
3. **Configuration Guide**: Learn about configuration options
4. **Learning Mechanisms**: Deep dive into plasticity and learning
5. **Integration Testing**: Run the comprehensive test suite

## Community

- **GitHub**: https://github.com/nlm-project/nlm
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Contributing**: Contributions welcome!

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [Research Papers](https://github.com/nlm-project/papers)
- [Phase Specifications](https://github.com/nlm-project/specs)
```

### 02_configuration.md

```markdown
# Configuration Guide

## Overview

NLM uses a flexible configuration system that allows customization of all aspects of the brain simulation. Configuration can be applied through:

- **Configuration objects** (programmatic)
- **Configuration files** (text-based)
- **Command line arguments** (runtime)
- **Default settings** (built-in)

## Configuration Types

### 1. Programmatic Configuration

```python
from pynlm import Config

# Create configuration with dictionary
config = Config({
    'neuron_count': 1000,
    'region_count': 2,
    'simulation_timestep': 0.001,
    'random_seed': 42
})

# Modify configuration
config.set('neuron_count', 2000)
config.enable_plasticity('stdp', True)
```

### 2. Configuration Files

#### File Format

Configuration files use a simple key-value format:

```ini
# NLM Configuration File
neuron_count = 1000
region_count = 2
simulation_timestep = 0.001
random_seed = 42

# Plasticity settings
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0005

# Neuromodulation settings
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm.log
```

#### Required Parameters

| Parameter | Type | Default | Required |
|-----------|------|---------|----------|
| `neuron_count` | int | 1000 | No |
| `region_count` | int | 2 | No |
| `simulation_timestep` | float | 0.001 | No |
| `random_seed` | int | 42 | No |

#### Optional Parameters

| Category | Parameter | Type | Default | Description |
|----------|-----------|------|---------|-------------|
| **Plasticity** | `plasticity_learning_rate` | float | 0.01 | Learning rate for plasticity |
| | `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation |
| | `stdp_ltd_weight` | float | 0.012 | STDP long-term depression |
| | `stdp_time_constant` | float | 20.0 | STDP time constant |
| | `hebbian_enabled` | bool | true | Enable Hebbian plasticity |
| | `stdp_enabled` | bool | true | Enable STDP |
| | `structural_plasticity_enabled` | bool | true | Enable structural plasticity |
| **Development** | `development_synaptogenesis_rate` | float | 0.001 | Rate of synapse formation |
| | `development_pruning_rate` | float | 0.0001 | Rate of synapse removal |
| | `development_maturation_rate` | float | 0.0005 | Rate of development |
| **Neuromodulation** | `dopamine_baseline` | float | 0.0 | Baseline dopamine level |
| | `curiosity_baseline` | float | 0.0 | Baseline curiosity |
| | `novelty_sensitivity` | float | 0.1 | Novelty detection sensitivity |
| | `reward_discount_factor` | float | 0.99 | Reward discount factor |
| **Environment** | `environment_name` | string | GridWorld | Environment type |
| | `environment_width` | int | 10 | Environment width |
| | `environment_height` | int | 10 | Environment height |
| | `vision_range` | int | 5 | Vision range for agents |
| | `action_space` | list | [up, down, left, right] | Available actions |
| **Memory** | `working_memory_capacity` | int | 100 | Working memory size |
| | `episodic_memory_capacity` | int | 50 | Episodic memory size |
| | `associative_memory_capacity` | int | 200 | Associative memory size |
| **Logging** | `log_level` | string | INFO | Logging verbosity |
| | `log_to_file` | bool | false | Write logs to file |
| | `log_filename` | string | nlm.log | Log file name |
| | `log_max_size` | int | 10485760 | Maximum log file size |

### 3. Command Line Configuration

```bash
# Run with custom configuration file
nlm --config myconfig.cfg

# Override configuration with command line
nlm --neuron-count 2000 --region-count 3 --steps 5000

# Enable/disable specific features
nlm --enable-stdp=true --enable-hebbian=false

# Set random seed for reproducibility
nlm --random-seed 12345
```

### 4. Configuration Validation

```python
from pynlm import Config

# Create configuration
config = Config({
    'neuron_count': 5000,
    'region_count': 10,
    'simulation_timestep': 0.001,
    'connection_probability': 0.15
})

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration validation errors:")
    for error in config.get_validation_errors():
        print(f"  - {error}")

# Check specific parameter constraints
if config.is_valid('neuron_count', 100):
    print("Neuron count of 100 is valid")
else:
    print("Neuron count of 100 is invalid")
```

### Configuration Best Practices

#### 1. Start Simple
- Begin with default configuration
- Only modify parameters as needed
- Test with minimal changes

#### 2. Use Appropriate Values
- **Neuron count**: 100-10000 (performance trade-off)
- **Region count**: 1-100 (network complexity)
- **Timestep**: 0.0001-0.01 (accuracy vs speed)
- **Connection probability**: 0.05-0.3 (connectivity level)

#### 3. Consider Performance
```python
# Performance-optimized configuration
performance_config = {
    'neuron_count': 500,  # Lower for speed
    'region_count': 1,   # Single region for efficiency
    'simulation_timestep': 0.001,
    'plasticity_learning_rate': 0.01,  # Reduced for stability
}
```

#### 4. Reproducibility
```python
# Use fixed random seeds for reproducible experiments
reproducible_config = {
    'random_seed': 12345,  # Fixed seed
    'neuron_count': 1000,
    'region_count': 2,
}
```

#### 5. Experiment-Specific Configuration
```python
# Different configurations for different experiments
learning_config = {
    'plasticity_learning_rate': 0.05,  # High learning
    'stdp_ltp_weight': 0.03,
    'stdp_ltd_weight': 0.025,
    'development_synaptogenesis_rate': 0.005,
}

exploration_config = {
    'curiosity_baseline': 0.5,  # High curiosity
    'novelty_sensitivity': 0.8,
    'dopamine_baseline': 0.2,
}
```

## Configuration API Reference

### Config Class Methods

#### Initialization
```python
Config()                    # Default configuration
Config(dict)               # Configuration from dictionary
Config.load_from_file(path)  # Load from file
```

#### Core Methods
```python
config.set(key, value)                    # Set configuration value
config.get(key)                           # Get configuration value
config.get_or(key, default)               # Get with default
config.has(key)                           # Check if parameter exists
config.remove(key)                        # Remove parameter
config.merge(other_config)                # Merge with another config
config.copy()                             # Create copy
config.validate()                         # Validate configuration
```

#### Configuration Categories
```python
# Plasticity configuration
config.enable_stdp(True, True, True)
config.enable_hebbian(True)
config.set_plasticity_rate(0.01)

# Development configuration
config.set_developmental_stage('juvenile')
config.set_maturation_rate(0.001)

# Neuromodulation configuration
config.set_dopamine_level(0.1)
config.set_curiosity_level(0.05)

# Memory configuration
config.set_working_memory_capacity(100)
config.set_episodic_memory_capacity(50)
```

### Configuration Commands

```bash
# Show configuration help
nlm --help

# List available parameters
nlm --config-help

# Show current configuration
nlm --show-config

# Export configuration
nlm --export-config config_output.cfg

# Import configuration
nlm --import-config custom_config.cfg
```

## Configuration Troubleshooting

### Common Issues

#### Issue: Invalid Parameter Values
**Symptom**: Configuration validation fails
**Solution**: Use valid ranges for parameters
```python
# Invalid
config = Config({'neuron_count': -100})  # Error: neuron_count must be positive

# Valid
config = Config({'neuron_count': 100})   # OK
```

#### Issue: Missing Required Parameters
**Symptom**: Brain initialization fails
**Solution**: Check parameter dependencies
```python
# Missing dependencies
config = Config({'environment_name': 'GridWorld'})  # Missing width/height

# With dependencies
config = Config({
    'environment_name': 'GridWorld',
    'environment_width': 10,
    'environment_height': 10
})
```

#### Issue: Performance Problems
**Symptom**: Simulation runs too slowly or uses too much memory
**Solution**: Optimize configuration for performance
```python
# Performance-optimized config
perf_config = Config({
    'neuron_count': 500,    # Reduced for speed
    'region_count': 1,     # Single region
    'simulation_timestep': 0.001,
    'plasticity_learning_rate': 0.01,  # Lower learning rate
})
```

### Configuration Debugging

```python
from pynlm import Config, Brain

# Create configuration
config = Config({
    'neuron_count': 1000,
    'region_count': 2,
    'environment_name': 'GridWorld',
    'environment_width': 10,
    'environment_height': 10,
})

# Debug configuration
print("Configuration Debug Info:")
print(f"Neuron count: {config.get('neuron_count')}")
print(f"Region count: {config.get('region_count')}")
print(f"Environment: {config.get('environment_name')}")
print(f"Environment size: {config.get('environment_width')} x {config.get('environment_height')}")

# Validate each component
print("\nValidation Results:")
print(f"Core parameters: {config.validate_core()}")
print(f"Environment parameters: {config.validate_environment()}")
print(f"Plasticity parameters: {config.validate_plasticity()}")
print(f"Neuromodulation parameters: {config.validate_neuromodulation()}")

# Create brain with configuration
try:
    brain = Brain(config)
    brain.initialize()
    print("\n✓ Brain created successfully")
except Exception as e:
    print(f"\n✗ Brain creation failed: {e}")
```

## Configuration Evolution

### Configuration Changes Across Phases

| Phase | Key Configuration Changes | Description |
|-------|-------------------------|-------------|
| **Phase 1** | Basic parameters | Minimal configuration for skeleton |
| **Phase 2** | Neuron dynamics | Added STDP, Hebbian, structural plasticity |
| **Phase 3** | World interaction | Added environment and agent systems |
| **Phase 4** | Cognitive capabilities | Added memory and prediction systems |
| **Phase 5** | Performance optimizations | Added memory pools, event queues, SIMD |
| **Phase 6** | Integration | Final coherent brain loop configuration |

### Future Configuration Enhancements

#### 1. Dynamic Configuration
- Runtime parameter modification
- Adaptive configuration based on performance
- Configuration optimization

#### 2. Hierarchical Configuration
- Multi-level configuration system
- Environment-specific configurations
- Experiment-specific profiles

#### 3. Configuration Sharing
- Shared configuration libraries
- Team collaboration configurations
- Community configuration repository

## Conclusion

Configuration is a fundamental aspect of NLM that affects every aspect of the simulation. Understanding and properly configuring parameters is essential for:

- **Research accuracy**: Valid configurations ensure reliable results
- **Performance optimization**: Proper configuration balances speed and accuracy
- **Reproducibility**: Fixed configurations enable replication
- **Experiment design**: Appropriate configuration enables specific experiments

By following the guidelines in this document, you can create effective configurations for your NLM experiments and achieve optimal results.
```

All examples in this directory are designed to be self-contained and can be run independently. They demonstrate different aspects of NLM functionality from basic to advanced usage. Each example includes comprehensive comments and follows best practices for NLM development.

The examples serve as both documentation and practical starting points for users to understand and use the NLM library effectively. They cover the full spectrum of NLM capabilities from simple simulations to complex real-world applications.
