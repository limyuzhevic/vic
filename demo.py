#!/usr/bin/env python3
"""
Comprehensive Examples for NLM Python Bindings

This file demonstrates all Pythonic features and usage patterns of the NLM
Python bindings, including advanced features like context managers,
Pythonic operators, batch operations, and vector conversions.
"""

import sys
import os

# Add the python directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from pynlm import (
    Neuron, Synapse, NeuralRegion, NeuralPopulation,
    Dopamine, Acetylcholine, Norepinephrine, Serotonin,
    NeuronType, SynapseType, FiringState, RegionId, PopulationId,
    create_neuron, create_synapse, create_region, create_population,
    batch_step_neurons, batch_get_potential, batch_get_spike_counts,
    calculate_mean, calculate_variance, calculate_stddev,
    to_vector, from_array
)

def example_1_basic_neural_simulation():
    """
    Example 1: Basic neural simulation with Pythonic API
    Demonstrates property-based access and step operations
    """
    print("=" * 60)
    print("Example 1: Basic Neural Simulation")
    print("=" * 60)
    
    # Create neural region using Pythonic property access
    with NeuralRegion(RegionId(1), "hippocampus") as region:
        print(f"Created region: {region.name}")
        
        # Create neurons using Pythonic property access
        neuron1 = Neuron(1)
        neuron1.type = NeuronType.Excitatory
        neuron1.V = -65.0
        neuron1.threshold = -55.0
        neuron1.refractory_period = 5
        
        neuron2 = Neuron(2)
        neuron2.type = NeuronType.Inhibitory
        neuron2.V = -70.0
        neuron2.threshold = -50.0
        neuron2.refractory_period = 3
        
        print(f"Created neuron1: V={neuron1.V}mV, threshold={neuron1.threshold}mV")
        print(f"Created neuron2: V={neuron2.V}mV, threshold={neuron2.threshold}mV")
        
        # Create synapse using Pythonic operators
        synapse = Synapse(1, neuron1.id, neuron2.id)
        synapse.weight = 0.3
        synapse.delay = 2
        
        print(f"Created synapse: {synapse.source} -> {synapse.destination}, weight={synapse.weight}")
        
        # Simulate neural activity
        print("\nSimulating neural activity...")
        for step in range(10):
            current_time = float(step * 0.001)
            
            # Step neurons
            fired1 = neuron1.stepLIF(current_time, 0.001)
            fired2 = neuron2.stepLIF(current_time, 0.001)
            
            # Record spikes
            if fired1:
                neuron1.recordSpike(current_time)
            if fired2:
                neuron2.recordSpike(current_time)
            
            print(f"Step {step}: Neuron1 fired={fired1}, Neuron2 fired={fired2}")
            print(f"  Neuron1 V={neuron1.V:.2f}mV, Neuron2 V={neuron2.V:.2f}mV")
        
        # Access statistics
        print(f"\nFinal Statistics:")
        print(f"  Neuron1 spike count: {neuron1.spike_count}")
        print(f"  Neuron2 spike count: {neuron2.spike_count}")
        print(f"  Total synapses: {region.getSynapseCount()}")
        print(f"  Total neurons: {region.getTotalNeuronCount()}")

def example_2_population_dynamics():
    """
    Example 2: Population dynamics and statistics
    Demonstrates population management and statistical analysis
    """
    print("\n" + "=" * 60)
    print("Example 2: Population Dynamics")
    print("=" * 60)
    
    # Create population with Pythonic API
    with NeuralRegion(RegionId(2), "cortex") as region:
        print(f"Creating population of 100 excitatory neurons...")
        population = region.addPopulation(100, NeuronType.Excitatory)
        
        print(f"Population created: {population.getSize()} neurons")
        print(f"Neuron type: {population.getNeuronType()}")
        
        # Initialize all neurons
        for i in range(min(5, population.getSize())):
            neuron = population.getNeuron(i)
            if neuron:
                neuron.V = -70.0 + (i * 0.5)
                neuron.threshold = -55.0 + (i * 0.2)
        
        # Simulate population activity
        print(f"\nSimulating population activity for 50 steps...")
        for t in range(50):
            current_time = float(t * 0.001)
            population.step(current_time)
        
        # Access comprehensive statistics
        print(f"\nPopulation Statistics:")
        print(f"  Active neurons: {population.active_count}/{population.getSize()}")
        print(f"  Firing neurons: {population.firing_count}/{population.getSize()}")
        print(f"  Activity level: {population.activity_level:.3f}")
        print(f"  Average firing rate: {population.average_firing_rate:.3f} Hz")
        print(f"  Mean membrane potential: {population.mean_membrane_potential:.3f}mV")
        print(f"  Variance membrane potential: {population.variance_membrane_potential:.3f}mV²")
        
        # Demonstrate Pythonic iteration
        print(f"\nFirst 5 neurons in population:")
        for i, neuron in enumerate(population[:5]):
            print(f"  Neuron {i+1}: V={neuron.V:.2f}mV, threshold={neuron.threshold}mV")
        
        # Demonstrate vector conversion
        print(f"\nVector conversion example:")
        vector = to_vector(population.getNeuron(0))
        print(f"  Neuron vector: {vector}")

def example_3_neuromodulation():
    """
    Example 3: Neuromodulation system
    Demonstrates dopamine, acetylcholine, norepinephrine, and serotonin
    """
    print("\n" + "=" * 60)
    print("Example 3: Neuromodulation System")
    print("=" * 60)
    
    # Create neuromodulators
    dopamine = Dopamine()
    acetylcholine = Acetylcholine()
    norepinephrine = Norepinephrine()
    serotonin = Serotonin()
    
    print(f"Neuromodulators created:")
    print(f"  Dopamine: {dopamine.get_name()}")
    print(f"  Acetylcholine: {acetylcholine.get_name()}")
    print(f"  Norepinephrine: {norepinephrine.get_name()}")
    print(f"  Serotonin: {serotonin.get_name()}")
    
    # Simulate neuromodulation dynamics over time
    print(f"\nSimulating neuromodulation dynamics...")
    for time in [0.0, 0.5, 1.0, 2.0, 5.0]:
        # Apply biological dynamics
        dopamine.setLevel(0.5 * (1.0 - time / 5.0))
        acetylcholine.setLevel(0.3 + 0.2 * (time / 5.0))
        norepinephrine.setLevel(0.2 + 0.3 * (1.0 - abs(time - 2.5) / 2.5))
        serotonin.setLevel(0.4 + 0.1 * ((time % 3.0) / 3.0))
        
        print(f"  Time {time:.1f}s:")
        print(f"    Dopamine: {dopamine.getLevel():.3f}")
        print(f"    Acetylcholine: {acetylcholine.getLevel():.3f}")
        print(f"    Norepinephrine: {norepinephrine.getLevel():.3f}")
        print(f"    Serotonin: {serotonin.getLevel():.3f}")
    
    # Apply reward signals
    print(f"\nApplying reward signals...")
    dopamine.signalReward(1.0)
    dopamine.signalRewardPredictionError(0.2)
    
    print(f"Dopamine plasticity factor: {dopamine.getPlasticityFactor():.3f}")
    print(f"Acetylcholine plasticity factor: {acetylcholine.getPlasticityFactor():.3f}")

def example_4_batch_operations():
    """
    Example 4: Batch operations and vector conversions
    Demonstrates efficiency with large-scale neural processing
    """
    print("\n" + "=" * 60)
    print("Example 4: Batch Operations")
    print("=" * 60)
    
    with NeuralRegion(RegionId(3), "batch_test") as region:
        print(f"Creating batch of 50 neurons...")
        neurons = []
        for i in range(50):
            neuron = Neuron(i + 1)
            neuron.type = NeuronType.Excitatory if i % 2 == 0 else NeuronType.Inhibitory
            neuron.V = -70.0 + (i * 0.2)
            neuron.threshold = -55.0 + (i * 0.1)
            neurons.append(neuron)
        
        print(f"Created batch: {len(neurons)} neurons")
        
        # Demonstrate Pythonic operations
        print(f"\nPythonic operations:")
        neuron_ids = [n.id for n in neurons]
        print(f"  Neuron IDs: {neuron_ids[:10]}...")  # Show first 10
        
        neuron_types = [n.type for n in neurons]
        excitatory_count = sum(1 for t in neuron_types if t == NeuronType.Excitatory)
        print(f"  Excitatory neurons: {excitatory_count}/{len(neuron_types)}")
        
        # Use batch operations
        print(f"\nUsing batch operations...")
        current_time = 0.0
        batch_step_neurons(neurons, current_time)
        
        potentials = batch_get_potential(neurons)
        spike_counts = batch_get_spike_counts(neurons)
        
        print(f"  Potentials (first 10): {[f'{p:.2f}' for p in potentials[:10]]}...")
        print(f"  Spike counts (first 10): {spike_counts[:10]}...")
        
        # Statistics with utility functions
        print(f"\nStatistics utilities:")
        mean_potential = calculate_mean(potentials)
        variance_potential = calculate_variance(potentials)
        stddev_potential = calculate_stddev(potentials)
        
        print(f"  Mean potential: {mean_potential:.3f}mV")
        print(f"  Potential variance: {variance_potential:.3f}mV²")
        print(f"  Potential stddev: {stddev_potential:.3f}mV")
        
        # Vector conversion examples
        print(f"\nVector conversion examples:")
        if neurons:
            neuron_vector = to_vector(neurons[0])
            print(f"  Neuron vector: {neuron_vector}")
        
        array_list = from_array([1.0, 2.0, 3.0, 4.0, 5.0])
        print(f"  Array to list: {array_list}")

def example_5_context_managers():
    """
    Example 5: Context managers and resource management
    Demonstrates safe resource management with automatic cleanup
    """
    print("\n" + "=" * 60)
    print("Example 5: Context Managers")
    print("=" * 60)
    
    print(f"Demonstrating context manager for resource management...")
    
    # Use context manager for automatic resource cleanup
    with NeuralRegion(RegionId(4), "context_test") as region:
        print(f"Region '{region.name}' created with context manager")
        print(f"Region ID: {region.id}")
        
        # Perform operations
        neuron1 = region.add_neuron(NeuronType.Excitatory)
        neuron2 = region.add_neuron(NeuronType.Inhibitory)
        
        neuron1.V = -60.0
        neuron2.V = -65.0
        
        print(f"Added neurons to region")
        print(f"Region contains {region.getPopulationCount()} populations")
        print(f"Region contains {region.getTotalNeuronCount()} neurons")
        print(f"Region contains {region.getSynapseCount()} synapses")
        
        # Simulate briefly
        for t in range(5):
            region.step(float(t) * 0.001)
        
        print(f"\nContext manager automatically cleaned up resources when exiting block")
        print(f"Region '{region.name}' is no longer accessible")

def example_6_pythonic_operators():
    """
    Example 6: Pythonic operators and dictionary-like access
    Demonstrates dictionary-like property access and iteration
    """
    print("\n" + "=" * 60)
    print("Example 6: Pythonic Operators")
    print("=" * 60)
    
    # Create neural components
    neuron = Neuron(1, NeuronType.Excitatory)
    neuron.V = -65.0
    neuron.threshold = -55.0
    neuron.firing_rate = 10.0
    
    synapse = Synapse(1, neuron.id, neuron.id)
    synapse.weight = 0.5
    synapse.delay = 1
    
    print(f"Demonstrating Pythonic operators:")
    
    # Property-based access
    print(f"\nProperty-based access:")
    print(f"  neuron['V'] = {neuron['V']}mV")
    print(f"  neuron['threshold'] = {neuron['threshold']}mV")
    print(f"  neuron['firing_rate'] = {neuron['firing_rate']}Hz")
    print(f"  neuron['spike_count'] = {neuron['spike_count']}")
    
    # Set properties using dictionary syntax
    neuron["V"] = -60.0
    neuron["threshold"] = -50.0
    print(f"  After setting neuron['V'] = -60.0 and neuron['threshold'] = -50.0")
    print(f"    neuron.V = {neuron.V}mV")
    print(f"    neuron.threshold = {neuron.threshold}mV")
    
    # Synapse operators
    print(f"\nSynapse operators:")
    print(f"  synapse['weight'] = {synapse['weight']}")
    print(f"  synapse['delay'] = {synapse['delay']}")
    print(f"  synapse['type'] = {synapse['type']}")
    
    synapse["weight"] = 0.7
    print(f"  After setting synapse['weight'] = 0.7")
    print(f"    synapse.weight = {synapse.weight}")
    
    # Dictionary representation
    print(f"\nDictionary representations:")
    neuron_dict = neuron.as_dict()
    print(f"  Neuron dict: {neuron_dict}")
    
    synapse_dict = synapse.as_dict()
    print(f"  Synapse dict: {synapse_dict}")
    
    # Iteration
    print(f"\nIteration example:")
    print(f"  Neuron properties: {list(neuron_dict.keys())}")
    print(f"  Synapse properties: {list(synapse_dict.keys())}")

def example_7_factory_functions():
    """
    Example 7: Factory functions and utilities
    Demonstrates convenience factory functions and utility operations
    """
    print("\n" + "=" * 60)
    print("Example 7: Factory Functions and Utilities")
    print("=" * 60)
    
    print(f"Using factory functions:")
    
    # Create neural components using factory functions
    neuron1 = create_neuron(1, NeuronType.Excitatory)
    neuron2 = create_neuron(2, NeuronType.Inhibitory)
    
    synapse = create_synapse(1, neuron1.id, neuron2.id, weight=0.8)
    
    region = create_region(RegionId(5), "factory_test")
    population = create_population(RegionId(6), 20, NeuronType.Excitatory)
    
    print(f"  Created neuron1: {neuron1}")
    print(f"  Created neuron2: {neuron2}")
    print(f"  Created synapse: {synapse}")
    print(f"  Created region: {region}")
    print(f"  Created population: {population}")
    
    # Demonstrate utility functions
    print(f"\nUtility functions:")
    
    # Statistics
    values = [1.0, 2.0, 3.0, 4.0, 5.0]
    print(f"  Values: {values}")
    print(f"  Mean: {calculate_mean(values):.2f}")
    print(f"  Variance: {calculate_variance(values):.2f}")
    print(f"  Stddev: {calculate_stddev(values):.2f}")
    
    # Vector operations
    print(f"\nVector operations:")
    vector = to_vector(neuron1)
    print(f"  Neuron vector: {vector}")
    
    arr_list = from_array([10.0, 20.0, 30.0])
    print(f"  Array to list: {arr_list}")
    
    # Batch operations
    print(f"\nBatch operations:")
    neurons = [neuron1, neuron2, neuron]
    potentials = batch_get_potential(neurons)
    print(f"  Batch potentials: {potentials}")

def example_8_advanced_simulation():
    """
    Example 8: Advanced simulation with multiple regions
    Demonstrates complex neural network simulation
    """
    print("\n" + "=" * 60)
    print("Example 8: Advanced Simulation with Multiple Regions")
    print("=" * 60)
    
    # Create multiple regions
    regions = []
    for i in range(3):
        region = create_region(RegionId(i + 10), f"region_{i}")
        regions.append(region)
    
    print(f"Created {len(regions)} regions")
    
    # Create neurons and connections across regions
    neurons = []
    synapses = []
    
    for region_idx, region in enumerate(regions):
        # Add neurons to each region
        for j in range(3):
            neuron = create_neuron(region_idx * 10 + j)
            neuron.type = NeuronType.Excitatory if (region_idx + j) % 2 == 0 else NeuronType.Inhibitory
            neurons.append(neuron)
            
            # Connect to neurons in other regions
            if region_idx > 0:
                target_region = regions[region_idx - 1]
                target_neuron = target_region.add_neuron(NeuronType.Excitatory)
                synapse = create_synapse(len(synapses) + 1, neuron.id, target_neuron.id)
                synapses.append(synapse)
    
    print(f"Total neurons: {len(neurons)}")
    print(f"Total synapses: {len(synapses)}")
    
    # Simulate the network
    print(f"\nSimulating network for 20 steps...")
    for t in range(20):
        current_time = float(t) * 0.001
        
        # Step all neurons
        batch_step_neurons(neurons, current_time)
        
        # Step all synapses
        for synapse in synapses:
            synapse.step(current_time)
        
        # Record statistics every 5 steps
        if t % 5 == 0:
            potentials = batch_get_potential(neurons)
            spike_counts = batch_get_spike_counts(neurons)
            
            mean_pot = calculate_mean(potentials)
            total_spikes = sum(spike_counts)
            
            print(f"  Step {t}: Mean potential={mean_pot:.2f}mV, Total spikes={total_spikes}")
    
    print(f"\nSimulation complete!")

def run_all_examples():
    """
    Run all examples in sequence
    """
    print("🚀 NLM Python Bindings - Comprehensive Example Suite")
    print("=" * 80)
    print("This suite demonstrates all Pythonic features and usage patterns.")
    print("=" * 80)
    
    examples = [
        example_1_basic_neural_simulation,
        example_2_population_dynamics,
        example_3_neuromodulation,
        example_4_batch_operations,
        example_5_context_managers,
        example_6_pythonic_operators,
        example_7_factory_functions,
        example_8_advanced_simulation,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"❌ Error in example {i}: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 80)
    print("✅ All examples completed successfully!")
    print("=" * 80)
    print("\nThe NLM Python bindings provide:")
    print("• Comprehensive neural component bindings")
    print("• Advanced Pythonic API features")
    print("• Context managers for resource management")
    print("• Pythonic operators and property access")
    print("• Vector conversions and batch operations")
    print("• Statistics utilities and factory functions")
    print("• Full neuromodulator system support")
    print("=" * 80)

if __name__ == "__main__":
    run_all_examples()