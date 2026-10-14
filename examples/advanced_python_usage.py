"""
NLM (Neural Learning Machine) - Advanced Python Usage Example

This example demonstrates the advanced Python features available in NLM 6.0,
including network analysis, learning curve tracking, and experiment management.

Author: NLM Research Team
Version: 6.0.0
"""

import pynlm
import time

def main():
    print("NLM Advanced Python Usage Example")
    print("=" * 60)
    
    # 1. Create advanced configuration
    print("\n1. Creating advanced configuration...")
    config = pynlm.create_advanced_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Advanced brain created with {brain.getTotalNeuronCount()} neurons")
    
    # 2. Network analysis
    print("\n2. Analyzing network connectivity...")
    analysis = pynlm.analyze_network_connectivity(brain)
    print(f"   Network Analysis Results:")
    print(f"   - Total neurons: {analysis.totalNeurons}")
    print(f"   - Total synapses: {analysis.totalSynapses}")
    print(f"   - Average connections per neuron: {analysis.averageConnectionsPerNeuron}")
    print(f"   - E/I ratio: {analysis.excitationInhibitionRatio:.3f}")
    
    # 3. Learning progress analysis
    print("\n3. Analyzing learning progress...")
    learning = pynlm.analyze_learning_progress(brain, 50)
    print(f"   Learning Analysis Results:")
    print(f"   - Learning rate: {learning.totalLearning:.3f}")
    print(f"   - Learning steps: {learning.learningSteps}")
    print(f"   - Activity level: {learning.totalLearning:.3f}")
    
    # 4. Activity statistics
    print("\n4. Getting activity statistics...")
    stats = pynlm.get_activity_statistics(brain)
    print(f"   Activity Statistics:")
    print(f"   - Average firing rate: {stats.averageFiringRate:.3f} Hz")
    print(f"   - Max firing neurons: {stats.maxFiringNeurons}")
    print(f"   - Spike count: {stats.spikeCount}")
    print(f"   - Activity variability: {stats.activityVariability:.3f}")
    
    # 5. Simulate learning
    print("\n5. Simulating learning process...")
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            current_learning = pynlm.analyze_learning_progress(brain, 10)
            print(f"   Step {step}: Learning rate = {current_learning.totalLearning:.3f}")
    
    # 6. Final analysis
    print("\n6. Final advanced analysis...")
    final_analysis = pynlm.analyze_network_connectivity(brain)
    final_stats = pynlm.get_activity_statistics(brain)
    final_learning = pynlm.analyze_learning_progress(brain, 20)
    
    print(f"   Final Network Analysis:")
    print(f"   - Synapses created: {final_analysis.totalSynapses}")
    print(f"   - Network efficiency: {final_analysis.excitationInhibitionRatio:.3f}")
    
    print(f"   Final Activity:")
    print(f"   - Average firing: {final_stats.averageFiringRate:.3f} Hz")
    print(f"   - Total activity: {final_stats.spikeCount} spikes")
    
    print(f"   Final Learning:")
    print(f"   - Total learning: {final_learning.totalLearning:.3f}")
    
    print("\n✅ Advanced example completed successfully!")
    print("\nKey features demonstrated:")
    print("  • Network connectivity analysis")
    print("  • Learning progress tracking")
    print("  • Activity statistics")
    print("  • Dynamic analysis during simulation")
    print("  • Integration with advanced configuration")

if __name__ == "__main__":
    main()
