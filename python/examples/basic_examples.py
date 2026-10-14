"""
NLM Python Bindings - Basic Examples

This module provides basic examples for using the NLM Python bindings.
"""

import pynlm

def quick_test():
    """
    Quick test of basic NLM functionality.
    
    This function creates a brain with default configuration and runs
    a few simulation steps to verify that the bindings are working.
    
    Returns:
        The brain object for further testing.
    """
    # Create default brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run for 50 steps
    for i in range(50):
        brain.step(i)
    
    print(f"Quick test passed! Brain has {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain

def create_basic_simulation():
    """
    Create a basic simulation with brain, world, and agent.
    
    Returns:
        Dictionary containing 'brain', 'world', 'agent', and 'config'.
    """
    sim = pynlm.create_simulation()
    brain = sim['brain']
    world = sim['world']
    agent = sim['agent']
    
    print(f"Created basic simulation:")
    print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  World: {world.getWidth()}x{world.getHeight()}")
    print(f"  Agent: Configured for {agent.getSensoryInputSize()} sensory inputs")
    
    return sim

def run_simple_episode(steps=20, enable_learning=False):
    """
    Run a simple agent episode.
    
    Args:
        steps: Number of simulation steps to run.
        enable_learning: Whether to enable learning features.
    
    Returns:
        Dictionary containing simulation components and statistics.
    """
    sim = pynlm.create_simulation()
    brain = sim['brain']
    world = sim['world']
    agent = sim['agent']
    
    # Enable learning features if requested
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if enabled
        if enable_learning:
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    # Collect statistics
    stats = {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel()
    }
    
    print(f"Simple episode completed: {steps} steps")
    print(f"  Final spikes: {stats['total_spikes']}")
    print(f"  Final firing rate: {stats['avg_firing_rate']:.2f} Hz")
    print(f"  Final curiosity: {stats['curiosity']:.3f}")
    
    return {
        'simulation': sim,
        'statistics': stats,
        'steps_run': steps
    }

if __name__ == "__main__":
    print("NLM Python Bindings - Basic Examples")
    print("=" * 60)
    
    # Run quick test
    print("\n1. Running quick_test()...")
    brain = quick_test()
    
    # Run basic simulation
    print("\n2. Running create_basic_simulation()...")
    sim = create_basic_simulation()
    
    # Run simple episode
    print("\n3. Running run_simple_episode()...")
    results = run_simple_episode(steps=10, enable_learning=False)
    
    print("\n" + "=" * 60)
    print("All basic examples completed successfully!")
