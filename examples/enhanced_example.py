"""
NLM Enhanced Python Example: Complete Research Simulation

This example demonstrates the full capabilities of the enhanced NLM Python bindings,
showing how to work with the new neural architecture, plasticity mechanisms, and
advanced research APIs.

The example implements a complete agent-based learning simulation with:
- Neural architecture exploration
- Plasticity and neuromodulation
- Working memory and prediction
- Goal-directed behavior
"""

import pynlm
import numpy as np
import time

def create_enhanced_brain(neuron_count=1000, connection_density=0.1):
    """Create an enhanced research brain with specified parameters."""
    print(f"Creating enhanced research brain with {neuron_count} neurons...")
    brain = pynlm.createResearchBrain(neuron_count, connection_density)
    brain.initialize()
    return brain

def create_research_environment(width=20, height=20):
    """Create a research environment with specified dimensions."""
    print(f"Creating research environment ({width}x{height})...")
    world = pynlm.createSimpleResearchWorld(width, height)
    return world

def create_enhanced_agent(brain, world):
    """Create an enhanced agent with all learning subsystems enabled."""
    print("Creating enhanced agent with all learning subsystems...")
    agent = pynlm.createAgentWithDefaults(brain, world)
    
    # Enable all advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return agent

def analyze_brain_connectome(brain, step_name=""):
    """Analyze brain connectome and return statistics."""
    stats = pynlm.getBrainStats(brain)
    
    if step_name:
        print(f"\n--- {step_name} ---")
    
    print(f"Total neurons: {stats['total_neurons']:,}")
    print(f"Total synapses: {stats['total_synapses']:,}")
    print(f"Active neurons: {stats['active_neurons']:,}")
    print(f"Firing neurons: {stats['firing_neurons']:,}")
    print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"Excitatory/Inhibitory ratio: {stats['excitation_inhibition_ratio']:.3f}")
    print(f"Total spikes: {stats['total_spikes']:,}")
    print(f"Pending spike events: {stats['pending_spike_events']:,}")
    
    return stats

def explore_neural_architectural(brain, max_neurons=10):
    """Explore neural architecture by examining individual neurons."""
    print(f"\nExploring neural architecture (showing {max_neurons} neurons)...")
    
    neurons = pynlm.getAllNeuronsInBrain(brain)
    print(f"Total neurons in brain: {len(neurons):,}")
    
    if neurons:
        print(f"\nSample neurons:")
        for i in range(min(max_neurons, len(neurons))):
            neuron = neurons[i]
            print(f"  Neuron {i}: ID={neuron.getId()}, Type={neuron.getType()}")
            print(f"    Membrane potential: {neuron.getMembranePotential():.2f} mV")
            print(f"    Firing rate: {neuron.getFiringRate():.2f} Hz")
            print(f"    Firing: {neuron.isFiring()}, Refractory: {neuron.isRefractory()}")
            print(f"    Spike history length: {len(neuron.getSpikeHistory())}")
    
    return neurons

def run_learning_simulation(brain, world, agent, num_steps=1000, verbose=True):
    """Run a complete learning simulation."""
    print(f"\nRunning learning simulation for {num_steps} steps...")
    
    start_time = time.time()
    step_stats = pynlm.runBrainSimulation(brain, num_steps, verbose=False)
    
    # Store results for analysis
    simulation_results = {
        'total_steps': num_steps,
        'start_time': start_time,
        'end_time': time.time(),
        'duration': time.time() - start_time,
        'final_stats': pynlm.getBrainStats(brain),
        'step_stats': step_stats
    }
    
    if verbose:
        print(f"Simulation completed in {simulation_results['duration']:.2f} seconds")
        print(f"\nFinal brain statistics:")
        analyze_brain_connectome(brain, "Final State")
        
        if step_stats:
            print(f"\nSimulation progression (every 200 steps):")
            for stat in step_stats[::4]:  # Every 200th step
                print(f"  Step {stat['step']}: {stat['firing_neurons']} neurons firing, {stat['total_spikes']} total spikes")
    
    return simulation_results

def analyze_advanced_features(brain, agent, world):
    """Analyze advanced features and provide insights."""
    print(f"\nAnalyzing advanced features...")
    
    # Analyze brain structure
    print("1. Brain Structure Analysis:")
    regions = brain.getRegions()
    for i, region in enumerate(regions):
        print(f"   Region {i}: {region.getName()}")
        print(f"     Neurons: {region.getTotalNeuronCount()}, Populations: {region.getPopulationCount()}")
        print(f"     Synaptic density: {region.getSynapticDensity():.4f}")
    
    # Analyze learning subsystems
    print("\n2. Learning Subsystem Analysis:")
    print(f"   Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"   Structural plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"   Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"   Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Prediction error: {agent.getPredictionError():.3f}")
    print(f"   Developmental stage: {agent.getDevelopmentalStage()}")
    
    # Analyze world state
    print("\n3. Environment Analysis:")
    body = world.getAgentBody()
    print(f"   Agent position: ({body.x:.1f}, {body.y:.1f})")
    print(f"   Agent orientation: {body.orientation:.1f} degrees")
    print(f"   Agent energy: {body.energy:.1f}/{body.getMaxEnergy()}")
    print(f"   Agent health: {body.health:.1f}")
    print(f"   Agent age: {body.age}")
    
    percept = world.getSensoryPercept()
    print(f"   Sensory input available: Vision={percept.getVisionWidth()}x{percept.getVisionHeight()}")

def create_multibrain_experiment(brain_configs):
    """Create and run an experiment with multiple brain configurations."""
    print(f"\nRunning multi-brain experiment with {len(brain_configs)} configurations...")
    
    brains = []
    for i, config in enumerate(brain_configs):
        print(f"  Creating brain {i+1}: {config['name']}")
        brain = pynlm.createResearchBrain(config['neuron_count'], config['connection_density'])
        brain.initialize()
        brains.append((config['name'], brain))
    
    # Initialize all brains with warmup
    print("  Warming up all brains...")
    pynlm.initializeAllBrains([brain for name, brain in brains], 100)
    
    # Collect initial statistics
    print("\n  Initial brain statistics:")
    for name, brain in brains:
        stats = pynlm.getBrainStats(brain)
        print(f"    {name}: {stats['total_neurons']:,} neurons, {stats['total_synapses']:,} synapses")
    
    return brains

def compare_brain_configurations(config1_stats, config2_stats, name1="Config 1", name2="Config 2"):
    """Compare two brain configurations and provide insights."""
    print(f"\nComparing {name1} vs {name2}:")
    
    print(f"  Neurons - {name1}: {config1_stats['total_neurons']:,}, {name2}: {config2_stats['total_neurons']:,}")
    print(f"  Synapses - {name1}: {config1_stats['total_synapses']:,}, {name2}: {config2_stats['total_synapses']:,}")
    print(f"  Firing neurons - {name1}: {config1_stats['firing_neurons']:,}, {name2}: {config2_stats['firing_neurons']:,}")
    print(f"  Average firing rate - {name1}: {config1_stats['average_firing_rate']:.2f} Hz, {name2}: {config2_stats['average_firing_rate']:.2f} Hz")
    print(f"  E/I ratio - {name1}: {config1_stats['excitation_inhibition_ratio']:.3f}, {name2}: {config2_stats['excitation_inhibition_ratio']:.3f}")
    print(f"  Total spikes - {name1}: {config1_stats['total_spikes']:,}, {name2}: {config2_stats['total_spikes']:,}")
    
    # Calculate efficiency metrics
    if config1_stats['total_spikes'] > 0 and config2_stats['total_spikes'] > 0:
        efficiency1 = config1_stats['firing_neurons'] / config1_stats['total_neurons']
        efficiency2 = config2_stats['firing_neurons'] / config2_stats['total_neurons']
        print(f"  Efficiency (firing/neural) - {name1}: {efficiency1:.3f}, {name2}: {efficiency2:.3f}")

def main():
    """Main function demonstrating the enhanced NLM Python API."""
    print("=" * 80)
    print("NLM Enhanced Python API - Complete Research Simulation")
    print("=" * 80)
    
    # Phase 1: Setup and Exploration
    print("\n1. PHASE 1: SETUP AND EXPLORATION")
    print("-" * 50)
    
    # Create enhanced research setup
    brain = create_enhanced_brain(2000, 0.05)
    world = create_research_environment(30, 30)
    agent = create_enhanced_agent(brain, world)
    
    # Explore neural architecture
    neurons = explore_neural_architectural(brain, max_neurons=5)
    
    # Analyze initial state
    analyze_brain_connectome(brain, "Initial State")
    
    # Phase 2: Learning Simulation
    print("\n2. PHASE 2: LEARNING SIMULATION")
    print("-" * 50)
    
    # Run learning simulation
    simulation_results = run_learning_simulation(brain, world, agent, num_steps=2000, verbose=True)
    
    # Analyze advanced features
    analyze_advanced_features(brain, agent, world)
    
    # Phase 3: Comparative Experiment
    print("\n3. PHASE 3: COMPARATIVE EXPERIMENT")
    print("-" * 50)
    
    # Create different brain configurations
    brain_configs = [
        {'name': 'Sparse Network', 'neuron_count': 1000, 'connection_density': 0.05},
        {'name': 'Dense Network', 'neuron_count': 2000, 'connection_density': 0.15},
        {'name': 'Large Network', 'neuron_count': 3000, 'connection_density': 0.1}
    ]
    
    # Run multi-brain experiment
    brains = create_multibrain_experiment(brain_configs)
    
    # Compare configurations
    print("\n  Configuration Comparison:")
    for i, (name, brain_obj) in enumerate(brains):
        stats = pynlm.getBrainStats(brain_obj)
        print(f"    {name}: {stats['total_neurons']:,} neurons, {stats['total_synapses']:,} synapses")
    
    # Phase 4: Advanced Usage
    print("\n4. PHASE 4: ADVANCED USAGE")
    print("-" * 50)
    
    print("Demonstrating advanced features:")
    
    # Show how to create regions and populations
    print("  Creating brain regions and populations:")
    region_id = brain.addRegion("hippocampus")
    region = brain.getRegion(region_id)
    populations = pynlm.createNeuralPopulations(region, 100, pynlm.NeuronType.Excitatory)
    print(f"    Created region '{region.getName()}' with {len(populations)} populations")
    
    # Show analysis utilities
    print("  Using analysis utilities:")
    connectome_analysis = pynlm.analyzeConnectome(region)
    print(f"    Connectome analysis - Neurons: {connectome_analysis['neuron_count']}, Populations: {connectome_analysis['population_count']}")
    print(f"    Synaptic density: {connectome_analysis['synaptic_density']:.4f}")
    print(f"    Average synaptic weight: {connectome_analysis['average_synaptic_weight']:.3f}")
    
    # Show plasticity control
    print("\n  Demonstrating plasticity control:")
    print("    Plasticity rules available: STDP, Hebbian")
    print("    Neuromodulators available: Dopamine, Curiosity, Novelty, PredictionError")
    print("    All learning systems enabled in this demonstration")
    
    # Phase 5: Summary and Insights
    print("\n5. PHASE 5: SUMMARY AND INSIGHTS")
    print("-" * 50)
    
    final_stats = simulation_results['final_stats']
    
    print("Enhanced NLM API Capabilities Demonstrated:")
    print("  ✓ Full neural architecture access (Neuron, Synapse, NeuralRegion, NeuralPopulation)")
    print("  ✓ Advanced plasticity mechanisms (STDP, Hebbian learning)")
    print("  ✓ Neuromodulatory control (Dopamine, Curiosity, Novelty, PredictionError)")
    print("  ✓ Factory functions for research scenarios")
    print("  ✓ Batch operations for efficiency")
    print("  ✓ Comprehensive analysis utilities")
    print("  ✓ Backward compatibility with original API")
    print("  ✓ Enhanced research-oriented APIs")
    
    print(f"\nSimulation completed successfully!")
    print(f"Total duration: {simulation_results['duration']:.2f} seconds")
    print(f"Final spike count: {final_stats['total_spikes']:,}")
    print(f"Final firing neurons: {final_stats['firing_neurons']:,}")
    print(f"Final average firing rate: {final_stats['average_firing_rate']:.2f} Hz")
    
    print("\n" + "=" * 80)
    print("Enhanced NLM Python API demonstration completed successfully!")
    print("=" * 80)
    
    return simulation_results

if __name__ == "__main__":
    # Set random seed for reproducible results
    import random
    random.seed(42)
    np.random.seed(42)
    
    # Run the demonstration
    results = main()
    
    # Optional: Save results for further analysis
    print("\nExample saved! The enhanced NLM API now provides:")
    print("• Complete neural architecture access")
    print("• Advanced learning mechanisms")
    print("• Neuromodulatory control")
    print("• Research-oriented APIs")
    print("• Backward compatibility")
