# NLM Tutorial 2: Learning and Plasticity

## Overview

Welcome to Tutorial 2! In this tutorial, you'll dive deep into the learning mechanisms that make NLM special. You'll explore:

1. **STDP (Spike-Timing-Dependent Plasticity)** - The core learning mechanism
2. **Hebbian Learning** - Correlation-based learning
3. **Structural Plasticity** - Formation and elimination of connections
4. **Neuromodulation** - How reward and curiosity guide learning
5. **Learning Rates and Development** - How learning changes over time

This tutorial assumes you've completed Tutorial 1 and have a basic understanding of NLM components.

---

## Prerequisites

- Tutorial 1 completed (or equivalent NLM knowledge)
- Understanding of neural basic concepts
- Basic Python programming skills

## Part 1: Introduction to Plasticity Mechanisms

### Why Plasticity is Central to NLM

NLM's learning capabilities come from its plasticity mechanisms, which are fundamentally different from traditional AI systems:

| Traditional AI | NLM |
|---------------|-----|
| Weight updates based on error signals | Spike-timing dependent updates |
| Static connectivity patterns | Dynamic connection formation/removal |
| Supervised learning only | Unsupervised and reinforcement learning |
| Fixed learning rates | Developmental modulation |
| Global plasticity | Local synaptic changes |

### Key Plasticity Concepts

#### 1. Spike-Timing-Dependent Plasticity (STDP)
```cpp
// STDP Principle:
// When a pre-synaptic spike occurs BEFORE a post-synaptic spike:
//   → Long-Term Potentiation (LTP): Synaptic strength increases
// 
// When a pre-synaptic spike occurs AFTER a post-synaptic spike:
//   → Long-Term Depression (LTD): Synaptic strength decreases
//
// The exact timing difference determines the magnitude
```

#### 2. Hebbian Learning
```cpp
// Hebbian Principle:
// "Neurons that fire together, wire together"
// 
// When pre- and post-synaptic neurons are active simultaneously:
//   → Synaptic strength increases
//   → Reinforces correlated activity
```

#### 3. Structural Plasticity
```cpp
// Structural Plasticity:
// Formation and elimination of synaptic connections
// - Synaptogenesis: Creating new connections
// - Pruning: Removing weak or unused connections
// - Activity-dependent: Connections form based on neural activity
```

### Understanding the Learning Equation

```python
# General plasticity update (simplified):
# Δw = learning_rate * contribution_factor * neuromodulation_factor

# Where:
# - Δw: Change in synaptic weight
# - learning_rate: Base learning speed
# - contribution_factor: Spike timing or correlation
# - neuromodulation_factor: Reward, curiosity, or prediction error
```

---

## Part 2: STDP Deep Dive

### Step 1: Configure STDP

```python
import pynlm
import numpy as np

print("=== STDP Deep Dive ===")

# Create configuration with STDP enabled
stdp_config = pynlm.createLearningConfig()
stdp_config.set("brain.neuron_count", 200)
stdp_config.set("plasticity.stdp.enable", True)
stdp_config.set("plasticity.stdp.learning_rate", 0.01)
stdp_config.set("plasticity.stdp.tau_plus", 20.0)
stdp_config.set("plasticity.stdp.tau_minus", 20.0)

print("STDP Configuration:")
print(f"  Learning rate: {stdp_config.getFloat('plasticity.stdp.learning_rate')}")
print(f"  Time constant (LTP): {stdp_config.getFloat('plasticity.stdp.tau_plus')} ms")
print(f"  Time constant (LTD): {stdp_config.getFloat('plasticity.stdp.tau_minus')} ms")
```

### Step 2: Create STDP Test Network

```python
def create_stdp_test_network():
    """Create a simple network for STDP testing"""
    
    config = pynlm.createLearningConfig()
    config.set("brain.neuron_count", 50)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Identify pre- and post-synaptic neurons
    pre_neuron_id = 0
    post_neuron_id = 1
    
    # Get neuron references
    pre_neuron = brain.getNeuron(pre_neuron_id)
    post_neuron = brain.getNeuron(post_neuron_id)
    
    return brain, pre_neuron_id, post_neuron_id, pre_neuron, post_neuron

brain, pre_id, post_id, pre_neuron, post_neuron = create_stdp_test_network()
```

### Step 3: Generate Spike Trains

```python
import random

def generate_spike_trains(trials=1000, pre_rate=10.0, post_rate=8.0):
    """Generate spike trains for STDP testing"""
    
    print(f"\nGenerating spike trains (Trials: {trials})")
    print(f"Pre-synaptic rate: {pre_rate} spikes/step")
    print(f"Post-synaptic rate: {post_rate} spikes/step")
    
    # Generate Poisson spike trains
    pre_spikes = np.random.poisson(pre_rate, trials)
    post_spikes = np.random.poisson(post_rate, trials)
    
    # Convert to spike times
    pre_spike_times = np.where(pre_spikes > 0)[0]
    post_spike_times = np.where(post_spikes > 0)[0]
    
    print(f"  Pre-synaptic spikes: {len(pre_spike_times)}")
    print(f"  Post-synaptic spikes: {len(post_spike_times)}")
    
    return pre_spike_times, post_spike_times, pre_spikes, post_spikes

pre_times, post_times, pre_spikes, post_spikes = generate_spike_trains(500)
```

### Step 4: Apply STDP Rules

```python
def apply_stdp_learning(brain, pre_id, post_id, pre_times, post_times):
    """Apply STDP learning rules"""
    
    print(f"\nApplying STDP learning rules:")
    
    # Record initial weight
    initial_weight = brain.getNeuron(pre_id).getWeight(post_id)
    print(f"  Initial weight: {initial_weight:.6f}")
    
    # Create spike train visualization
    print("  Spike timing visualization:")
    print("  Blue squares: Pre-synaptic spikes")
    print("  Red circles: Post-synaptic spikes")
    print("  Time on horizontal axis")
    
    # Simulate STDP using brain plasticity
    weight_changes = []
    
    for trial in range(min(100, len(pre_times), len(post_times))):
        # Check for spikes
        pre_spike = pre_spikes[trial] if trial < len(pre_spikes) else 0
        post_spike = post_spikes[trial] if trial < len(post_spikes) else 0
        
        if pre_spike > 0:
            brain.getNeuron(pre_id).spike()
        
        if post_spike > 0:
            brain.getNeuron(post_id).spike()
        
        # Apply STDP at each step
        brain.updatePlasticity(trial)
        
        # Record weight change
        current_weight = brain.getNeuron(pre_id).getWeight(post_id)
        weight_change = current_weight - initial_weight
        weight_changes.append(weight_change)
        
        if trial % 20 == 0:
            print(f"  Trial {trial}: Weight = {current_weight:.6f}")
    
    # Calculate statistics
    final_weight = brain.getNeuron(pre_id).getWeight(post_id)
    avg_weight_change = np.mean(weight_changes)
    weight_std = np.std(weight_changes)
    
    print(f"\nSTDP Results:")
    print(f"  Final weight: {final_weight:.6f}")
    print(f"  Average weight change: {avg_weight_change:.6f}")
    print(f"  Weight change standard deviation: {weight_std:.6f}")
    
    if abs(avg_weight_change) > 0.001:
        print(f"  ✓ Significant weight change detected")
        print(f"  {'Strengthening' if avg_weight_change > 0 else 'Weakening'} of connection")
    else:
        print(f"  ⚠️  Minimal weight change - STDP may not be working")
    
    return brain, final_weight, weight_changes

brain, final_weight, weight_changes = apply_stdp_learning(
    brain, pre_id, post_id, pre_times, post_times
)
```

### Step 5: STDP Timing Analysis

```python
def analyze_stdp_timing():
    """Analyze the relationship between spike timing and weight changes"""
    
    print(f"\n=== STDP Timing Analysis ===")
    
    # Simulate controlled STDP with specific timing patterns
    print("Creating controlled STDP scenarios:")
    
    scenarios = [
        ("Pre before Post (LTP)", "Strong potentiation"),
        ("Pre after Post (LTD)", "Strong depression"),
        ("Simultaneous (Neutral)", "No significant change"),
        ("Large time gap (Baseline)", "Minimal effect")
    ]
    
    for scenario, description in scenarios:
        print(f"  {scenario}: {description}")
    
    print(f"\nSTDP Window:")
    print(f"  Potentiation window (Δt > 0): 0 < Δt < τ_plus")
    print(f"  Depression window (Δt < 0): -τ_minus < Δt < 0")
    print(f"  Time constants: τ_plus = {stdp_config.getFloat('plasticity.stdp.tau_plus')} ms")
    print(f"                   τ_minus = {stdp_config.getFloat('plasticity.stdp.tau_minus')} ms")
    
    # STDP equation demonstration
    print(f"\nSTDP Weight Change Equation:")
    print(f"  For Δt > 0 (pre before post):")
    print(f"    Δw = A_plus * exp(-Δt / τ_plus)")
    print(f"    where A_plus = {stdp_config.getFloat('plasticity.stdp.learning_rate')}")
    
    print(f"  For Δt < 0 (pre after post):")
    print(f"    Δw = -A_minus * exp(Δt / τ_minus)")
    print(f"    where A_minus = {stdp_config.getFloat('plasticity.stdp.learning_rate')}")
    
    # Example calculation
    delta_t = 10  # 10ms pre before post
    dw = stdp_config.getFloat('plasticity.stdp.learning_rate') * np.exp(-delta_t / stdp_config.getFloat('plasticity.stdp.tau_plus'))
    print(f"\nExample: Δt = {delta_t}ms → Δw = {dw:.6f}")

analyze_stdp_timing()
```

---

## Part 3: Hebbian Learning

### Step 6: Configure Hebbian Learning

```python
def demonstrate_hebbian_learning():
    """Demonstrate Hebbian learning mechanism"""
    
    print(f"\n=== Hebbian Learning Demonstration ===")
    
    # Create configuration with Hebbian learning
    hebbian_config = pynlm.createLearningConfig()
    hebbian_config.set("brain.neuron_count", 100)
    hebbian_config.set("plasticity.stdp.enable", False)  # Disable STDP
    hebbian_config.set("plasticity.hebbian.enable", True)
    hebbian_config.set("plasticity.hebbian.learning_rate", 0.005)
    
    brain = pynlm.createBrain(hebbian_config)
    brain.initialize()
    
    # Get neurons
    pre_neuron = brain.getNeuron(0)
    post_neuron = brain.getNeuron(1)
    
    # Record initial connection
    initial_weight = pre_neuron.getWeight(1)
    print(f"Initial weight: {initial_weight:.6f}")
    
    # Simulate correlated activity
    print("Simulating correlated pre- and post-synaptic activity...")
    
    # Create correlated spike trains
    steps = 200
    pre_activity = np.ones(steps) * 1.0  # Constant activity
    post_activity = np.ones(steps) * 0.8  # Constant but lower activity
    
    # Add some random noise
    pre_activity += np.random.normal(0, 0.1, steps)
    post_activity += np.random.normal(0, 0.1, steps)
    
    # Apply Hebbian learning
    weight_history = []
    
    for step in range(steps):
        # Generate spikes based on activity
        if pre_activity[step] > np.random.random() * 2.0:
            pre_neuron.spike()
        
        if post_activity[step] > np.random.random() * 2.0:
            post_neuron.spike()
        
        # Update plasticity
        brain.updatePlasticity(step)
        
        # Record weight
        if step % 20 == 0:
            weight = pre_neuron.getWeight(1)
            weight_history.append(weight)
            print(f"  Step {step}: Weight = {weight:.6f}")
    
    # Analyze results
    final_weight = pre_neuron.getWeight(1)
    weight_change = final_weight - initial_weight
    
    print(f"\nHebbian Results:")
    print(f"  Initial weight: {initial_weight:.6f}")
    print(f"  Final weight: {final_weight:.6f}")
    print(f"  Weight change: {weight_change:.6f}")
    print(f"  Relative change: {weight_change/initial_weight:.1%}")
    
    if weight_change > 0:
        print(f"  ✓ Hebbian strengthening: Correlated activity increased connection")
    else:
        print(f"  ⚠️  Unexpected weight change")
    
    return brain, weight_history

brain, hebbian_history = demonstrate_hebbian_learning()
```

### Step 7: Compare STDP vs Hebbian

```python
def compare_plasticity_mechanisms():
    """Compare different plasticity mechanisms"""
    
    print(f"\n=== Plasticity Mechanism Comparison ===")
    
    # Create three test networks with different plasticity
    configs = {
        "STDP Only": pynlm.createLearningConfig(),
        "Hebbian Only": pynlm.createLearningConfig(),
        "Both": pynlm.createLearningConfig(),
        "Neither": pynlm.createLearningConfig()
    }
    
    # Configure each
    configs["STDP Only"].set("plasticity.stdp.enable", True)
    configs["STDP Only"].set("plasticity.hebbian.enable", False)
    configs["STDP Only"].set("brain.neuron_count", 50)
    
    configs["Hebbian Only"].set("plasticity.stdp.enable", False)
    configs["Hebbian Only"].set("plasticity.hebbian.enable", True)
    configs["Hebbian Only"].set("brain.neuron_count", 50)
    
    configs["Both"].set("plasticity.stdp.enable", True)
    configs["Both"].set("plasticity.hebbian.enable", True)
    configs["Both"].set("brain.neuron_count", 50)
    
    configs["Neither"].set("plasticity.stdp.enable", False)
    configs["Neither"].set("plasticity.hebbian.enable", False)
    configs["Neither"].set("brain.neuron_count", 50)
    
    # Test each configuration
    results = {}
    
    for name, config in configs.items():
        print(f"\nTesting {name}:")
        
        test_brain = pynlm.createBrain(config)
        test_brain.initialize()
        
        # Run simulation
        for step in range(100):
            test_brain.step(step)
        
        # Record results
        final_weight = test_brain.getNeuron(0).getWeight(1)
        initial_weight = 0.5  # Default initial weight
        weight_change = final_weight - initial_weight
        
        results[name] = {
            'final_weight': final_weight,
            'weight_change': weight_change,
            'change_percentage': (weight_change / initial_weight) * 100 if initial_weight != 0 else 0
        }
        
        print(f"  Initial weight: {initial_weight:.6f}")
        print(f"  Final weight: {final_weight:.6f}")
        print(f"  Weight change: {weight_change:.6f} ({results[name]['change_percentage']:.1f}%)")
    
    # Compare results
    print(f"\n=== Comparison Summary ===")
    print(f"{'Mechanism':<15} {'Final Weight':<15} {'Change':<12} {'Change %':<10}")
    print(f"{'-' * 60}")
    
    for name, result in results.items():
        print(f"{name:<15} {result['final_weight']:<15.6f} "
              f"{result['weight_change']:<12.6f} {result['change_percentage']:<10.1f}")
    
    return results

comparison_results = compare_plasticity_mechanisms()
```

---

## Part 4: Structural Plasticity

### Step 8: Configure Structural Plasticity

```python
def demonstrate_structural_plasticity():
    """Demonstrate structural plasticity (formation/removal of connections)"""
    
    print(f"\n=== Structural Plasticity Demonstration ===")
    
    # Create configuration with structural plasticity
    structural_config = pynlm.createDevelopmentConfig()
    structural_config.set("brain.neuron_count", 100)
    structural_config.set("plasticity.structural.enable", True)
    structural_config.set("plasticity.structural.formation_rate", 0.02)
    structural_config.set("plasticity.structural.pruning_threshold", 0.1)
    structural_config.set("development.critical_period.active", True)
    
    brain = pynlm.createBrain(structural_config)
    brain.initialize()
    
    # Create agent for world interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    
    print("Structural Plasticity Configuration:")
    print(f"  Synapse formation rate: {structural_config.getFloat('plasticity.structural.formation_rate')}")
    print(f"  Pruning threshold: {structural_config.getFloat('plasticity.structural.pruning_threshold')}")
    print(f"  Critical period active: {structural_config.getBool('development.critical_period.active')}")
    
    # Track network development
    network_stats = {
        'steps': [],
        'neuron_count': [],
        'synapse_count': [],
        'formation_events': [],
        'pruning_events': []
    }
    
    print(f"\nSimulating network development...")
    
    for step in range(200):
        # Update world and agent
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply development
        agent.updateDevelopment(0.1)
        
        # Record statistics
        if step % 20 == 0:
            network_stats['steps'].append(step)
            network_stats['neuron_count'].append(brain.getTotalNeuronCount())
            network_stats['synapse_count'].append(brain.getTotalSynapseCount())
            
            # Simulate formation/pruning events
            formations = np.random.poisson(structural_config.getFloat('plasticity.structural.formation_rate') * 10)
            pruning = np.random.poisson(0.5)  # Lower pruning rate
            
            network_stats['formation_events'].append(formations)
            network_stats['pruning_events'].append(pruning)
            
            print(f"  Step {step}: Neurons {network_stats['neuron_count'][-1]}, "
                  f"Synapses {network_stats['synapse_count'][-1]}, "
                  f"Formations {formations}, Pruning {pruning}")
    
    # Analyze network development
    print(f"\nNetwork Development Analysis:")
    
    initial_neurons = network_stats['neuron_count'][0]
    final_neurons = network_stats['neuron_count'][-1]
    neuron_change = final_neurons - initial_neurons
    
    initial_synapses = network_stats['synapse_count'][0]
    final_synapses = network_stats['synapse_count'][-1]
    synapse_change = final_synapses - initial_synapses
    
    total_formations = sum(network_stats['formation_events'])
    total_pruning = sum(network_stats['pruning_events'])
    
    print(f"  Neuron count change: {neuron_change:+d} ({((final_neurons/initial_neurons - 1) * 100):+.1f}%)")
    print(f"  Synapse count change: {synapse_change:+d} ({((final_synapses/initial_synapses - 1) * 100):+.1f}%)")
    print(f"  Total formation events: {total_formations}")
    print(f"  Total pruning events: {total_pruning}")
    
    if final_synapses > initial_synapses:
        print(f"  ✓ Net synapse formation: Network growing")
    elif final_synapses < initial_synapses:
        print(f"  ✓ Net synapse pruning: Network stabilizing")
    else:
        print(f"  → Synapse count stable")
    
    return brain, world, agent, network_stats

brain, world, agent, dev_stats = demonstrate_structural_plasticity()
```

### Step 9: Developmental Stages and Plasticity

```python
def analyze_developmental_plasticity():
    """Analyze how plasticity changes across developmental stages"""
    
    print(f"\n=== Developmental Plasticity Analysis ===")
    
    # Create development configuration
    dev_config = pynlm.createDevelopmentConfig()
    dev_config.set("brain.neuron_count", 150)
    
    brain = pynlm.createBrain(dev_config)
    brain.initialize()
    
    # Track plasticity over time
    developmental_data = {
        'stage': [],
        'plasticity_factor': [],
        'learning_rate': [],
        'synaptogenesis': [],
        'pruning': []
    }
    
    print("Simulating developmental trajectory...")
    print("Developmental Stage | Plasticity Factor | Learning Rate | Synapogenesis | Pruning")
    print("-" * 80)
    
    # Simulate through developmental stages
    for stage_num in range(4):
        # Simulate each developmental stage
        stage_steps = 50
        stage_plasticity = []
        stage_learning = []
        
        for step in range(stage_steps):
            brain.step(stage_num * stage_steps + step)
            
            # Get developmental metrics
            current_stage = brain.getDevelopmentalStage()
            current_plasticity = agent.getPlasticityFactor() if hasattr(agent, 'getPlasticityFactor') else 1.0
            current_learning = agent.getLearningRate() if hasattr(agent, 'getLearningRate') else 0.001
            
            stage_plasticity.append(current_plasticity)
            stage_learning.append(current_learning)
            
            # Record if stage changed
            if step % 10 == 0:
                developmental_data['stage'].append(current_stage.value)
                developmental_data['plasticity_factor'].append(current_plasticity)
                developmental_data['learning_rate'].append(current_learning)
                
                # Simulate synaptogenesis and pruning based on stage
                if current_stage == pynlm.DevelopmentalStage.INITIAL:
                    synaptogenesis = 0.03
                    pruning = 0.005
                elif current_stage == pynlm.DevelopmentalStage.CRITICAL_PERIOD:
                    synaptogenesis = 0.02
                    pruning = 0.01
                elif current_stage == pynlm.DevelopmentalStage.MATURATION:
                    synaptogenesis = 0.01
                    pruning = 0.02
                else:  # ADULT
                    synaptogenesis = 0.001
                    pruning = 0.015
                
                developmental_data['synaptogenesis'].append(synaptogenesis)
                developmental_data['pruning'].append(pruning)
                
                print(f"{current_stage.value:<20} | {current_plasticity:<18.3f} | "
                      f"{current_learning:<12.4f} | {synaptogenesis:<12.3f} | "
                      f"{pruning:<6.3f}")
    
    # Analyze developmental trends
    print(f"\nDevelopmental Trends:")
    
    avg_plasticity = np.mean(developmental_data['plasticity_factor'])
    avg_learning = np.mean(developmental_data['learning_rate'])
    total_synaptogenesis = sum(developmental_data['synaptogenesis'])
    total_pruning = sum(developmental_data['pruning'])
    
    print(f"  Average plasticity factor: {avg_plasticity:.3f}")
    print(f"  Average learning rate: {avg_learning:.4f}")
    print(f"  Total synaptogenesis events: {total_synaptogenesis:.1f}")
    print(f"  Total pruning events: {total_pruning:.1f}")
    
    # Determine developmental pattern
    if avg_plasticity > 1.0:
        print(f"  Pattern: High plasticity phase (e.g., Critical Period)")
    elif avg_plasticity > 0.5:
        print(f"  Pattern: Moderate plasticity phase (e.g., Maturation)")
    else:
        print(f"  Pattern: Low plasticity phase (e.g., Adult)")
    
    return brain, developmental_data

brain, dev_data = analyze_developmental_plasticity()
```

---

## Part 5: Neuromodulation

### Step 10: Configure Neuromodulation Systems

```python
def demonstrate_neuromodulation():
    """Demonstrate neuromodulation systems (dopamine, curiosity, novelty)"""
    
    print(f"\n=== Neuromodulation Systems Demonstration ===")
    
    # Create configuration with all neuromodulators
    neuromod_config = pynlm.createCuriosityConfig()
    neuromod_config.set("brain.neuron_count", 200)
    neuromod_config.set("neuromod.dopamine.scale", 2.0)
    neuromod_config.set("neuromod.curiosity.enable", True)
    neuromod_config.set("neuromod.novelty.enable", True)
    neuromod_config.set("neuromod.prediction_error.scale", 1.5)
    
    brain = pynlm.createBrain(neuromod_config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width(15), height(15))
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Neuromodulation Configuration:")
    print(f"  Dopamine scale: {neuromod_config.getFloat('neuromod.dopamine.scale')}")
    print(f"  Curiosity enabled: {neuromod_config.getBool('neuromod.curiosity.enable')}")
    print(f"  Novelty enabled: {neuromod_config.getBool('neuromod.novelty.enable')}")
    print(f"  Prediction error scale: {neuromod_config.getFloat('neuromod.prediction_error.scale')}")
    
    # Track neuromodulation dynamics
    neuromod_data = {
        'step': [],
        'dopamine': [],
        'curiosity': [],
        'novelty': [],
        'reward': [],
        'prediction_error': []
    }
    
    print(f"\nSimulating neuromodulation dynamics...")
    print(f"Step | Dopamine | Curiosity | Novelty | Reward | Prediction Error")
    print(f"-----|----------|-----------|---------|--------|------------------")
    
    # Run simulation with neuromodulation
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with neuromodulation
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get neuromodulation levels
        dopamine = agent.getNeuromodulationLevel('dopamine') if hasattr(agent, 'getNeuromodulationLevel') else 0.5
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        reward = world.getReward()
        error = agent.getPredictionError()
        
        # Record data
        neuromod_data['step'].append(step)
        neuromod_data['dopamine'].append(dopamine)
        neuromod_data['curiosity'].append(curiosity)
        neuromod_data['novelty'].append(novelty)
        neuromod_data['reward'].append(reward)
        neuromod_data['prediction_error'].append(error)
        
        # Print progress
        if step % 10 == 0:
            print(f"{step:<5} | {dopamine:<9.3f} | {curiosity:<10.3f} | "
                  f"{novelty:<7.3f} | {reward:<6.2f} | {error:<17.3f}")
        
        # Apply neuromodulation
        agent.applyRewardModulation(reward, error)
        agent.updateDevelopment(0.1)
        
        # Update world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Analyze neuromodulation patterns
    print(f"\nNeuromodulation Analysis:")
    
    dopamine_avg = np.mean(neuromod_data['dopamine'])
    curiosity_avg = np.mean(neuromod_data['curiosity'])
    novelty_avg = np.mean(neuromod_data['novelty'])
    reward_avg = np.mean(neuromod_data['reward'])
    error_avg = np.mean(neuromod_data['prediction_error'])
    
    print(f"  Average dopamine: {dopamine_avg:.3f}")
    print(f"  Average curiosity: {curiosity_avg:.3f}")
    print(f"  Average novelty: {novelty_avg:.3f}")
    print(f"  Average reward: {reward_avg:.2f}")
    print(f"  Average prediction error: {error_avg:.3f}")
    
    # Calculate neuromodulation balance
    neuromod_balance = {
        'exploration_drive': curiosity_avg * novelty_avg,
        'learning_efficiency': reward_avg / (error_avg + 0.01),
        'adaptation_speed': error_avg / (error_avg + 0.1)
    }
    
    print(f"\nNeuromodulation Balance:")
    print(f"  Exploration drive: {neuromod_balance['exploration_drive']:.3f}")
    print(f"  Learning efficiency: {neuromod_balance['learning_efficiency']:.3f}")
    print(f"  Adaptation speed: {neuromod_balance['adaptation_speed']:.3f}")
    
    # Determine behavioral pattern
    if neuromod_balance['exploration_drive'] > 0.5 and neuromod_balance['learning_efficiency'] > 1.0:
        pattern = "High exploration with effective learning"
    elif neuromod_balance['exploration_drive'] < 0.2:
        pattern = "Routine behavior, low exploration"
    elif neuromod_balance['learning_efficiency'] < 0.5:
        pattern = "High exploration but ineffective learning"
    else:
        pattern = "Balanced exploration and learning"
    
    print(f"  Behavioral pattern: {pattern}")
    
    return brain, world, agent, neuromod_data

brain, world, agent, neuromod_data = demonstrate_neuromodulation()
```

---

## Part 6: Advanced Learning Concepts

### Step 11: Learning Rate Modulation

```python
def demonstrate_learning_rate_modulation():
    """Demonstrate how learning rates change over development"""
    
    print(f"\n=== Learning Rate Modulation ===")
    
    # Create developmental configuration
    learning_config = pynlm.createDevelopmentConfig()
    learning_config.set("brain.neuron_count", 180)
    
    brain = pynlm.createBrain(learning_config)
    brain.initialize()
    
    # Create agent for interaction
    world = pynlm.createSimpleWorld()
    world.configure(width(20), height(20))
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning and development
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Track learning rate over time
    learning_rates = []
    developmental_stages = []
    performance = []
    
    print(f"Learning Rate Development:")
    print(f"Step | Development Stage | Learning Rate")
    print(f"-----|------------------|---------------")
    
    for step in range(150):
        # Update development
        agent.updateDevelopment(0.1)
        
        # Get current learning rate and stage
        current_stage = brain.getDevelopmentalStage()
        current_learning_rate = agent.getLearningRate() if hasattr(agent, 'getLearningRate') else 0.001
        
        learning_rates.append(current_learning_rate)
        developmental_stages.append(current_stage.value)
        
        # Print progress
        if step % 20 == 0:
            print(f"{step:<5} | {current_stage.value:<18} | {current_learning_rate:<12.4f}")
        
        # Run simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(world.getReward(), agent.getPredictionError())
    
    # Analyze learning rate changes
    print(f"\nLearning Rate Analysis:")
    
    # Find learning rate by developmental stage
    stage_data = {}
    for stage, rate in zip(developmental_stages, learning_rates):
        if stage not in stage_data:
            stage_data[stage] = []
        stage_data[stage].append(rate)
    
    for stage, rates in stage_data.items():
        avg_rate = np.mean(rates)
        print(f"  {stage}: Average learning rate = {avg_rate:.4f}")
    
    # Determine learning trajectory
    initial_rate = learning_rates[0]
    final_rate = learning_rates[-1]
    rate_change = (final_rate - initial_rate) / initial_rate if initial_rate != 0 else 0
    
    print(f"\nLearning Trajectory:")
    print(f"  Initial learning rate: {initial_rate:.4f}")
    print(f"  Final learning rate: {final_rate:.4f}")
    print(f"  Relative change: {rate_change:.1%}")
    
    if rate_change > 0:
        print(f"  Pattern: Learning acceleration (e.g., Critical Period)")
    elif rate_change < 0:
        print(f"  Pattern: Learning deceleration (e.g., Maturation)")
    else:
        print(f"  Pattern: Stable learning rate")
    
    return brain, world, agent, learning_rates

brain, world, agent, learning_rates = demonstrate_learning_rate_modulation()
```

### Step 12: Comprehensive Learning Analysis

```python
def comprehensive_learning_analysis():
    """Comprehensive analysis of all learning mechanisms"""
    
    print(f"\n=== Comprehensive Learning Analysis ===")
    
    # Run comparison across all plasticity mechanisms
    print("Running comprehensive learning analysis...")
    print("This will test all plasticity mechanisms and neuromodulation.")
    
    # Create comprehensive configuration
    comprehensive_config = pynlm.createLearningConfig()
    comprehensive_config.set("brain.neuron_count", 250)
    comprehensive_config.set("plasticity.structural.enable", True)
    comprehensive_config.set("neuromod.curiosity.enable", True)
    comprehensive_config.set("neuromod.novelty.enable", True)
    
    # Create test brain and agent
    brain = pynlm.createBrain(comprehensive_config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width(25), height(25))
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Comprehensive metrics collection
    metrics = {
        'step': [],
        'plasticity_score': [],
        'learning_efficiency': [],
        'curiosity_level': [],
        'novelty_sensitivity': [],
        'development_progress': [],
        'performance': []
    }
    
    print(f"\nRunning comprehensive analysis (200 steps)...")
    
    for step in range(200):
        # Update development
        agent.updateDevelopment(0.1)
        
        # Run simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = world.getReward()
        error = agent.getPredictionError()
        agent.applyRewardModulation(reward, error)
        
        # Record metrics
        if step % 20 == 0:
            plasticity_score = calculate_plasticity_score(brain)
            learning_efficiency = reward / (error + 0.01)
            curiosity_level = agent.getCuriosityLevel()
            novelty_sensitivity = agent.getNoveltyLevel()
            development_progress = brain.getDevelopmentalProgress()
            performance = calculate_performance(brain, world, agent)
            
            metrics['step'].append(step)
            metrics['plasticity_score'].append(plasticity_score)
            metrics['learning_efficiency'].append(learning_efficiency)
            metrics['curiosity_level'].append(curiosity_level)
            metrics['novelty_sensitivity'].append(novelty_sensitivity)
            metrics['development_progress'].append(development_progress)
            metrics['performance'].append(performance)
            
            print(f"  Step {step:3d}: Plasticity {plasticity_score:.3f}, "
                  f"Learning {learning_efficiency:.2f}, "
                  f"Curiosity {curiosity_level:.3f}, "
                  f"Dev {development_progress:.1%}")
    
    # Comprehensive analysis
    print(f"\n=== Comprehensive Analysis Results ===")
    
    analysis = {
        'total_plasticity': np.mean(metrics['plasticity_score']),
        'average_learning_efficiency': np.mean(metrics['learning_efficiency']),
        'average_curiosity': np.mean(metrics['curiosity_level']),
        'average_novelty': np.mean(metrics['novelty_sensitivity']),
        'developmental_maturation': np.mean(metrics['development_progress']),
        'average_performance': np.mean(metrics['performance'])
    }
    
    print("Learning Mechanism Performance:")
    for mechanism, value in analysis.items():
        print(f"  {mechanism.replace('_', ' ').title()}: {value:.3f}")
    
    # Determine overall learning quality
    quality_indicators = {
        'plasticity_strength': analysis['total_plasticity'] > 0.5,
        'learning_efficiency': analysis['average_learning_efficiency'] > 1.0,
        'curiosity_drive': analysis['average_curiosity'] > 0.3,
        'novelty_response': analysis['average_novelty'] > 0.3,
        'developmental_progression': analysis['developmental_maturation'] > 0.7,
        'task_performance': analysis['average_performance'] > 0.5
    }
    
    quality_score = sum(quality_indicators.values()) / len(quality_indicators)
    
    print(f"\nOverall Learning Quality: {quality_score:.1%} ({sum(quality_indicators.values())}/{len(quality_indicators)} indicators)")
    
    for indicator, passed in quality_indicators.items():
        status = "✓" if passed else "✗"
        print(f"  {status} {indicator.replace('_', ' ').title()}")
    
    # Learning system effectiveness ranking
    effectiveness = {
        'neuromodulation': analysis['average_curiosity'] + analysis['average_novelty'],
        'plasticity': analysis['total_plasticity'],
        'development': analysis['developmental_maturation'],
        'efficiency': analysis['average_learning_efficiency']
    }
    
    print(f"\nLearning System Effectiveness (ranking):")
    sorted_systems = sorted(effectiveness.items(), key=lambda x: x[1], reverse=True)
    for system, score in sorted_systems:
        print(f"  {system.replace('_', ' ').title()}: {score:.3f}")
    
    return brain, world, agent, metrics

brain, world, agent, comp_metrics = comprehensive_learning_analysis()
```

---

## Part 7: Applications and Use Cases

### Step 13: Real-World Applications

```python
print(f"\n=== Real-World Learning Applications ===")

# Demonstrate different learning scenarios

applications = [
    ("Skill Acquisition", "Learning motor skills through practice"),
    ("Exploration Learning", "Discovering new environments and objects"),
    ("Social Learning", "Learning from observing others"),
    ("Problem Solving", "Finding solutions to challenges"),
    ("Memory Development", "Building long-term knowledge"),
    ("Adaptive Behavior", "Adjusting to changing environments")
]

print("NLM Learning Applications:")
print("-" * 50)

for application, description in applications:
    print(f"\n{application}:")
    print(f"  {description}")
    
    # Show how NLM supports this application
    if application == "Skill Acquisition":
        print(f"    → Reinforcement learning with reward signals")
    elif application == "Exploration Learning":
        print(f"    → Curiosity-driven exploration with novelty detection")
    elif application == "Social Learning":
        print(f"    → Observation learning with social imitation")
    elif application == "Problem Solving":
        print(f"    → Planning and prediction with lookahead")
    elif application == "Memory Development":
        print(f"    → Working, episodic, and associative memory systems")
    elif application == "Adaptive Behavior":
        print(f"    → Developmental plasticity with critical periods")

# Example: Skill acquisition simulation
print(f"\n=== Example: Skill Acquisition Simulation ===")

# Create skill learning configuration
skill_config = pynlm.createDevelopmentConfig()
skill_config.set("brain.neuron_count", 200)
skill_config.set("neuromod.dopamine.scale", 2.0)

# Create skill learning brain
skill_brain = pynlm.createBrain(skill_config)
skill_brain.initialize()

# Create environment for skill practice
skill_world = pynlm.createSimpleWorld()
skill_world.configure(width(30), height(30))
skill_agent = pynlm.createAgentBrain(skill_brain)
skill_agent.initialize(skill_world)

# Enable skill learning
skill_agent.enableRewardModulation(True)
skill_agent.enableDevelopment(True)

# Simulate skill learning
target_skill_level = 0.8
current_skill_level = 0.0
learning_curve = []

print(f"Simulating skill acquisition to reach level {target_skill_level}...")

for step in range(200):
    # Update world
    skill_world.update(0.1)
    
    # Process sensory input
    percept = skill_world.getSensoryPercept()
    skill_agent.processSensoryInput(percept)
    
    # Neural computation
    skill_brain.step(step)
    
    # Motor action
    action = skill_agent.decodeMotorCommand()
    skill_world.applyMotorCommand(action, skill_world.getSimulationTime())
    
    # Learning with reward
    skill_reward = calculate_skill_reward(skill_agent, skill_world)
    skill_error = skill_agent.getPredictionError()
    skill_agent.applyRewardModulation(skill_reward, skill_error)
    
    # Development
    skill_agent.updateDevelopment(0.1)
    
    # Update skill level (simplified)
    current_skill_level = min(target_skill_level, 
                             current_skill_level + 
                             0.01 * skill_reward * (1 - current_skill_level))
    
    learning_curve.append(current_skill_level)
    
    if step % 40 == 0:
        print(f"  Step {step}: Skill level {current_skill_level:.3f} (Reward: {skill_reward:.2f})")

# Analyze skill learning
final_skill_level = learning_curve[-1]
learning_speed = final_skill_level / 200  # steps to reach target

print(f"\nSkill Learning Results:")
print(f"  Final skill level: {final_skill_level:.3f} (target: {target_skill_level})")
print(f"  Learning speed: {learning_speed:.4f} per step")
print(f"  Time to 50% proficiency: {next((i for i, level in enumerate(learning_curve) if level > 0.5), -1)} steps")
print(f"  Learning curve efficiency: {final_skill_level/target_skill_level:.1%}")
```

### Helper functions for demonstrations

```python
def calculate_plasticity_score(brain):
    """Calculate a plasticity score based on brain activity"""
    # Simple proxy: firing rate variability
    return min(1.0, brain.getAverageFiringRate() / 50.0)

def calculate_performance(brain, world, agent):
    """Calculate overall performance metric"""
    # Composite performance metric
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    energy = world.getAgentEnergy() / 100.0  # Normalize to 0-1
    
    return (curiosity + novelty + energy) / 3.0

def calculate_skill_reward(agent, world):
    """Calculate reward for skill acquisition"""
    # Positive reward for progress, negative for inefficiency
    curiosity = agent.getCuriosityLevel()
    energy = world.getAgentEnergy()
    
    return curiosity * energy / 100.0
```

---

## Part 8: Summary and Next Steps

### What You've Learned in Tutorial 2

✅ **STDP Mechanisms**: Spike-timing dependent plasticity
✅ **Hebbian Learning**: Correlation-based synaptic strengthening
✅ **Structural Plasticity**: Connection formation and elimination
✅ **Neuromodulation**: Dopamine, curiosity, and novelty systems
✅ **Developmental Plasticity**: Learning rate modulation over time
✅ **Comprehensive Analysis**: Multi-system learning integration
✅ **Real-World Applications**: Practical learning scenarios

### Key Concepts Mastered

1. **Temporal Learning**: STDP's role in timing-based learning
2. **Correlation Learning**: Hebbian principles in action
3. **Network Development**: Structural changes in connectivity
4. **Motivational Systems**: How neuromodulation guides behavior
5. **Developmental Trajectories**: Learning across lifespan
6. **Multi-system Integration**: How all mechanisms work together
7. **Application Scenarios**: Real-world learning contexts

### Technical Skills Developed

1. **Configuration Management**: Setting up different plasticity mechanisms
2. **Simulation Control**: Managing complex learning simulations
3. **Data Analysis**: Interpreting plasticity and learning metrics
4. **Comparative Analysis**: Evaluating different learning strategies
5. **Visualization**: Presenting complex learning patterns

### Next Tutorial Options

1. **Tutorial 3: Memory Systems** - Deep dive into memory mechanisms
2. **Tutorial 4: Environment Interaction** - Advanced agent-world dynamics
3. **Tutorial 5: Performance Optimization** - Speed and efficiency improvements
4. **Tutorial 6: Experimental Design** - Research methodology

### Assessment Questions

Answer these questions to check your understanding:

1. **STDP Basics**: What happens when pre-synaptic spikes occur before post-synaptic spikes?
2. **Hebbian Learning**: How does correlated activity strengthen connections?
3. **Structural Plasticity**: What are the differences between synaptogenesis and pruning?
4. **Neuromodulation**: How do dopamine and curiosity work together?
5. **Development**: How does learning change across developmental stages?
6. **Integration**: Why is it important to have multiple learning mechanisms?
7. **Application**: Which learning mechanism is most important for skill acquisition?

### Quick Reference Summary

```python
# Key plasticity mechanisms to remember:

# 1. STDP (Spike-Timing-Dependent Plasticity)
#    Rule: Pre before Post → Potentiation
#          Post before Pre → Depression
#    Equation: Δw = A * exp(-|Δt|/τ)

# 2. Hebbian Learning  
#    Rule: "Neurons that fire together, wire together"
#    Equation: Δw = η * x_pre * x_post

# 3. Structural Plasticity
#    Formation: Synaptogenesis
#    Elimination: Pruning
#    Regulation: Activity-dependent

# 4. Neuromodulation
#    Dopamine: Reward prediction error
#    Curiosity: Exploration drive
#    Novelty: Surprise detection

# 5. Developmental Plasticity
#    Critical periods: High plasticity windows
#    Maturation: Plasticity decreases with age
#    Adaptation: Learning rates change over time
```

### Recommended Next Steps

1. **Complete Tutorial 3** - Memory systems deep dive
2. **Read `docs/MEMORY_SYSTEMS.md`** - Comprehensive memory documentation
3. **Try `docs/EXAMPLES.md`** - More complex learning examples
4. **Study `docs/ADVANCED_GUIDE.md`** - Advanced learning concepts
5. **Practice with `docs/PRESETS.md`** - Experiment with different configurations

---

## Welcome to Tutorial 2!

You've completed a deep dive into NLM learning mechanisms! You now understand:

- **STDP**: The temporal core of NLM learning
- **Hebbian**: The correlation-based reinforcement
- **Structural Plasticity**: The architectural changes
- **Neuromodulation**: The motivational guidance
- **Development**: The temporal trajectory of learning
- **Integration**: How everything works together

You're well-equipped to:

1. **Design learning experiments** with specific plasticity mechanisms
2. **Analyze neural learning dynamics** across different conditions
3. **Build complex learning systems** for various applications
4. **Optimize learning performance** through configuration tuning
5. **Teach others** about biological learning principles

The NLM learning system is powerful because it's **multi-modal** (multiple mechanisms), **multi-level** (different time scales), and **multi-context** (various environmental interactions).

**Your learning journey continues:** Next up is Tutorial 3 on Memory Systems, where you'll explore how NLM stores and retrieves information!

**Score your learning:** How many of the advanced concepts did you understand? Which learning mechanism surprised you the most?

Proceed to Tutorial 3 when you're ready to advance your NLM expertise!

---

*Tutorial 2 Complete!* You've mastered the core learning mechanisms that make NLM biologically inspired and computationally powerful.

**Documentation Index:** Continue exploring with `docs/tutorials/Memory_Systems` (Tutorial 3), `docs/tutorials/Environment_Interaction` (Tutorial 4), and `docs/tutorials/Performance_Optimization` (Tutorial 5).

**Need help?** Check the Troubleshooting Guide at `docs/TROUBLESHOOTING.md` or revisit concepts in `docs/ADVANCED_GUIDE.md`.

Happy learning! 🚀