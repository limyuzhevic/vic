"""
NLM (Neural Learning Machine) - Network Analysis Example

This example demonstrates network analysis techniques for understanding brain connectivity,
synaptic organization, and network properties in NLM.

Author: NLM Research Team
Version: 6.0.0
"""

import pynlm
import time

def analyze_network_properties(brain):
    """Perform comprehensive network analysis"""
    print("\nNetwork Analysis Results:")
    print("-" * 40)
    
    # Basic connectivity
    analysis = pynlm.analyze_network_connectivity(brain)
    print(f"Basic Connectivity:")
    print(f"  Neurons: {analysis.totalNeurons}")
    print(f"  Synapses: {analysis.totalSynapses}")
    print(f"  Avg connections per neuron: {analysis.averageConnectionsPerNeuron}")
    print(f"  Min connections per neuron: {analysis.minConnectionsPerNeuron}")
    print(f"  Max connections per neuron: {analysis.maxConnectionsPerNeuron}")
    
    # Weight statistics
    print(f"\nSynaptic Weight Statistics:")
    print(f"  Average weight: {analysis.averageWeight:.3f}")
    print(f"  Weight std deviation: {analysis.weightStandardDeviation:.3f}")
    
    # Network properties
    ei_ratio = brain.getExcitationInhibitionRatio()
    print(f"\nNetwork Properties:")
    print(f"  Excitation/Inhibition ratio: {ei_ratio:.3f}")
    if ei_ratio > 10:
        print(f"  Status: Strong excitation dominance")
    elif ei_ratio < 0.1:
        print(f"  Status: Strong inhibition dominance")
    else:
        print(f"  Status: Balanced excitation/inhibition")
    
    # Network density
    total_possible_connections = analysis.totalNeurons * (analysis.totalNeurons - 1) / 2
    if total_possible_connections > 0:
        network_density = analysis.totalSynapses / total_possible_connections
        print(f"  Network density: {network_density:.4f} ({network_density*100:.2f}%)")
    
    # Activity patterns
    activity_stats = pynlm.get_activity_statistics(brain)
    print(f"\nActivity Patterns:")
    print(f"  Average firing rate: {activity_stats.averageFiringRate:.3f} Hz")
    print(f"  Max firing neurons: {activity_stats.maxFiringNeurons}")
    print(f"  Activity variability: {activity_stats.activityVariability:.3f}")
    
    # Learning indicators
    learning = pynlm.analyze_learning_progress(brain, 10)
    print(f"\nLearning Indicators:")
    print(f"  Learning rate: {learning.totalLearning:.3f}")
    print(f"  Neural plasticity: {'Active' if learning.learningSteps > 0 else 'Minimal'}")
    
    return analysis, activity_stats, learning

def compare_network_states(brain1, brain2, label1="Brain 1", label2="Brain 2"):
    """Compare two network states"""
    print(f"\nNetwork Comparison: {label1} vs {label2}")
    print("-" * 50)
    
    analysis1 = pynlm.analyze_network_connectivity(brain1)
    analysis2 = pynlm.analyze_network_connectivity(brain2)
    
    print(f"Metric               {label1:<15} {label2:<15} Difference")
    print(f"{'='*60}")
    print(f"Neurons              {analysis1.totalNeurons:<15} {analysis2.totalNeurons:<15} {analysis2.totalNeurons - analysis1.totalNeurons}")
    print(f"Synapses              {analysis1.totalSynapses:<15} {analysis2.totalSynapses:<15} {analysis2.totalSynapses - analysis1.totalSynapses}")
    print(f"E/I Ratio             {analysis1.excitationInhibitionRatio:<15.3f} {analysis2.excitationInhibitionRatio:<15.3f} {analysis2.excitationInhibitionRatio - analysis1.excitationInhibitionRatio:.3f}")
    
    stats1 = pynlm.get_activity_statistics(brain1)
    stats2 = pynlm.get_activity_statistics(brain2)
    print(f"Avg Firing Rate      {stats1.averageFiringRate:<15.3f} {stats2.averageFiringRate:<15.3f} {stats2.averageFiringRate - stats1.averageFiringRate:.3f}")
    
    learning1 = pynlm.analyze_learning_progress(brain1, 10)
    learning2 = pynlm.analyze_learning_progress(brain2, 10)
    print(f"Learning Rate        {learning1.totalLearning:<15.3f} {learning2.totalLearning:<15.3f} {learning2.totalLearning - learning1.totalLearning:.3f}


def main():
    print("NLM Network Analysis Example")
    print("=" * 60)
    print("This example demonstrates various network analysis techniques")
    print("including connectivity analysis, weight statistics, and learning indicators.")
    
    # Create brain with different configurations for comparison
    print("\n1. Creating baseline brain...")
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 500)
    config1.set("brain.region_count", 1)
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    print(f"   Created baseline brain with {brain1.getTotalNeuronCount()} neurons")
    
    print("\n2. Creating advanced brain...")
    config2 = pynlm.create_advanced_config()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    print(f"   Created advanced brain with {brain2.getTotalNeuronCount()} neurons")
    
    # Simulate both networks
    print("\n3. Simulating baseline network...")
    for step in range(50):
        brain1.step(step)
    print(f"   Baseline simulation completed ({brain1.getTotalSpikeCount()} spikes)")
    
    print("\n4. Simulating advanced network...")
    for step in range(100):
        brain2.step(step)
        if step % 25 == 0:
            learning = pynlm.analyze_learning_progress(brain2, 10)
            print(f"   Advanced simulation step {step}: Learning rate = {learning.totalLearning:.3f}")
    
    # Perform comprehensive analysis
    print("\n5. Performing comprehensive network analysis...")
    
    # Analyze each brain separately
    print("\nBaseline Brain Analysis:")
    analyze_network_properties(brain1)
    
    print("\nAdvanced Brain Analysis:")
    analyze_network_properties(brain2)
    
    # Compare the two networks
    compare_network_states(brain1, brain2, "Baseline", "Advanced")
    
    # Demonstrate learning dynamics
    print("\n6. Learning dynamics analysis...")
    print("\nLearning Progress Over Time:")
    print("-" * 30)
    
    for episode in range(10):
        # Create fresh brain for each episode
        fresh_config = pynlm.create_advanced_config()
        fresh_brain = pynlm.createBrain(fresh_config)
        fresh_brain.initialize()
        
        # Run simulation
        for step in range(30):
            fresh_brain.step(step)
        
        # Analyze learning
        learning = pynlm.analyze_learning_progress(fresh_brain, 5)
        analysis = pynlm.analyze_network_connectivity(fresh_brain)
        
        print(f"Episode {episode + 1}: Learning={learning.totalLearning:.3f}, "
              f"Neurons={analysis.totalNeurons}, Synapses={analysis.totalSynapses}")
    
    print("\n✅ Network analysis example completed successfully!")
    print("\nKey techniques demonstrated:")
    print("  • Network connectivity analysis")
    print("  • Synaptic weight statistics")
    print("  • Network property calculations")
    print("  • Activity pattern analysis")
    print("  • Learning dynamics tracking")
    print("  • Comparative network analysis")
    print("  • Evolution over multiple episodes")

if __name__ == "__main__":
    main()
