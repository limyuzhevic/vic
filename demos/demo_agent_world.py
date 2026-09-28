import pynlm
import time

"""
Demo 4: Agent-World Integration

This demo showcases complete agent-world integration using the NLM Python API.
"""

def demo_agent_world():
    print("=== NLM Agent-World Integration Demo ===")
    
    # Setup using the convenience function
    print("\n1. Creating complete agent-world simulation...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("   Created all components successfully")
    print(f"   - Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"   - World: {world.getWidth()}x{height} cells")
    print(f"   - Agent: {agent.getSensoryInputSize()} inputs, {agent.getMotorOutputSize()} outputs")
    
    # Run complete simulation using convenience function
    print("\n2. Running complete agent simulation (200 steps)...")
    total_spikes = pynlm.run_agent_simulation(brain, world, agent, 200, verbose=True)
    
    print(f"   Simulation completed! Total spikes: {total_spikes}")
    
    # Get final statistics
    print("\n3. Final simulation statistics...")
    stats = pynlm.get_brain_stats(brain)
    
    print("   Brain Statistics:")
    for key, value in stats.items():
        if isinstance(value, float):
            print(f"     {key}: {value:.2f}")
        else:
            print(f"     {key}: {value}")
    
    print("\n   Agent Statistics:")
    print(f"     Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"     Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"     Final prediction error: {agent.getPredictionError():.3f}")
    
    print("\n=== Demo Complete ===")
    return brain, agent, world

if __name__ == "__main__":
    demo_agent_world()
