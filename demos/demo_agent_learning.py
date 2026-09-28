import pynlm
import time

"""
Demo 5: Learning Experiment

This demo showcases the learning experiment functionality from the NLM Python API.
"""

def demo_agent_learning():
    print("=== NLM Learning Experiment Demo ===")
    
    # Setup for learning experiment
    print("\n1. Setting up learning experiment...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("   Learning experiment setup complete")
    print(f"   - Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"   - Agent with learning systems enabled")
    
    # Run learning experiment using convenience function
    print("\n2. Running learning experiment (300 steps)...")
    
    result = pynlm.create_learning_experiment(brain, agent, world, 300)
    initial_weight, final_weight, total_spikes, weight_change = result
    
    print(f"   Learning experiment results:")
    print(f"     Initial mean synaptic weight: {initial_weight:.4f}")
    print(f"     Final mean synaptic weight: {final_weight:.4f}")
    print(f"     Weight change: {weight_change:.4f}")
    print(f"     Total spikes generated: {total_spikes}")
    print(f"     Learning {'detected' if abs(weight_change) > 0.01 else 'not detected'}")
    
    # Analyze learning
    print("\n3. Learning analysis...")
    learning_efficiency = abs(weight_change) / max(total_spikes, 1)
    
    print(f"   Learning efficiency: {learning_efficiency:.6f}")
    print(f"   Stability: {'Good' if abs(weight_change) < 0.1 else 'Variable'}")
    
    # Demonstrate plasticity systems
    print("\n4. Demonstrating plasticity systems...")
    print(f"   Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"   Structural plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"   Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"   Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    
    # Monitor learning progress
    print("\n5. Monitoring learning progress...")
    history = pynlm.monitor_brain_health(brain, duration_steps=300, update_interval=60.0)
    
    if history:
        # Calculate learning rate
        weight_changes = []
        for i in range(1, len(history)):
            if history[i-1]["total_spikes"] > 0:
                change = (history[i]["total_spikes"] - history[i-1]["total_spikes"]) / \
                         history[i-1]["total_spikes"]
                weight_changes.append(change)
        
        if weight_changes:
            avg_change = sum(weight_changes) / len(weight_changes)
            print(f"   Average spike change per update: {avg_change:.3f}")
            print(f"   Learning trajectory: {'Increasing' if avg_change > 0 else 'Decreasing' if avg_change < 0 else 'Stable'}")
    
    print("\n=== Demo Complete ===")
    return brain, agent, world, result

if __name__ == "__main__":
    demo_agent_learning()

