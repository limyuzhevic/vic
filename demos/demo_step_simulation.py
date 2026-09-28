import pynlm
import time

"""
Demo 3: Step-by-Step Simulation

This demo showcases detailed step-by-step simulation with monitoring
and the enhanced Python API features.
"""

def demo_step_simulation():
    print("=== NLM Step-by-Step Simulation Demo ===")
    
    # Create agent world setup
    print("\n1. Setting up agent-world simulation...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"   Created agent with {agent.getSensoryInputSize()} sensory inputs")
    print(f"   and {agent.getMotorOutputSize()} motor outputs")
    
    # Run step-by-step simulation
    print("\n2. Running step-by-step simulation (100 steps)...")
    
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 20 steps
        if step % 20 == 0:
            stats = pynlm.get_brain_stats(brain)
            print(f"   Step {step}: {stats['firing_neurons']} firing, "
                  f"{stats['total_spikes']} spikes, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"novelty: {agent.getNoveltyLevel():.3f}")
    
    # Final statistics
    print("\n3. Final simulation statistics...")
    final_stats = pynlm.get_brain_stats(brain)
    
    print("   Final Brain Statistics:")
    for key, value in final_stats.items():
        if isinstance(value, float):
            print(f"     {key}: {value:.2f}")
        else:
            print(f"     {key}: {value}")
    
    print("\n   Agent Statistics:")
    print(f"     Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"     Structural plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"     Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"     Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    print(f"     Current curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"     Current novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"     Current neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"     Developmental stage: {agent.getDevelopmentalStage()}")
    
    # Demonstrate learning
    print("\n4. Demonstrating learning...")
    reward = agent.getNoveltyLevel()
    agent.applyRewardModulation(reward, 0.0)
    
    print(f"   Applied reward: {reward:.3f}")
    print(f"   Prediction error: {agent.getPredictionError():.3f}")
    print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    # Demonstrate development
    print("\n5. Demonstrating development...")
    agent.updateDevelopment(0.1)
    
    print(f"   Developmental age: {agent.getNeuromodulationLevel():.3f}")
    print(f"   Current developmental stage: {agent.getDevelopmentalStage()}")
    
    print("\n=== Demo Complete ===")
    return brain, agent, world

if __name__ == "__main__":
    demo_step_simulation()
