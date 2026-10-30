"""
NLM Python Examples for Beginners
==================================

Quick start examples for users new to NLM Python bindings.
"""

import pynlm

def basic_example():
    """
    Example 1: Create and run a simple brain
    =========================================
    
    This is the most basic way to use NLM.
    """
    print("=== Basic NLM Example ===")
    
    # Create a brain with default settings
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()

def complete_system_example():
    """
    Example 2: Complete NLM system
    ===============================
    
    Create a brain, world, and agent - the complete NLM setup.
    """
    print("=== Complete NLM System Example ===")
    
    # Create complete system
    brain, world, agent = pynlm.createCompleteSystem()
    
    print(f"Created system with:")
    print(f"  - {brain.getTotalNeuronCount()} neurons")
    print(f"  - {world.getWidth()}x{world.getHeight()} world")
    print(f"  - Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Enable learning
    agent.enableAllSystems()
    
    # Run simulation
    brain, world, agent = pynlm.runSimpleSimulation(brain, world, agent, 200)
    
    print(f"\nAfter simulation:")
    print(f"  - Firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  - Agent curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Agent novelty: {agent.getNoveltyLevel():.3f}")
    print()

def configuration_example():
    """
    Example 3: Configuration
    ===========================
    
    Learn how to configure NLM for your needs.
    """
    print("=== Configuration Example ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Modify configuration
    config.setInt("neuron_count", 2000)
    config.setFloat("connection_probability", 0.12f)
    config.setInt("region_count", 2)
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with custom config:")
    print(f"  - {brain.getTotalNeuronCount()} neurons")
    print(f"  - {brain.getRegionCount()} regions")
    print(f"  - Connection probability: {config.getFloat('connection_probability')}")
    print()

def analysis_example():
    """
    Example 4: Network Analysis
    ==========================
    
    Analyze your neural network.
    """
    print("=== Network Analysis Example ===")
    
    # Create and run brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
    
    # Get network statistics
    stats = brain.getBrainStats()
    print("Network statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Get connectivity
    connectivity = brain.getConnectivityStats()
    print("\nConnectivity:")
    for key, value in connectivity.items():
        print(f"  {key}: {value}")
    
    # Get neuron types
    distribution = brain.getNeuronDistribution()
    print("\nNeuron distribution:")
    for key, value in distribution.items():
        print(f"  {key}: {value}")
    print()

def agent_example():
    """
    Example 5: Advanced Agent
    =========================
    
    Create an intelligent agent with learning capabilities.
    """
    print("=== Advanced Agent Example ===")
    
    # Create system
    brain, world, agent = pynlm.createCompleteSystem()
    
    # Enable all learning systems
    agent.enableAllSystems()
    
    # Create custom action
    action = pynlm.Action(pynlm.ActionType.Custom)
    action.addParameter(0.5f)
    action.addParameter(0.3f)
    action.addParameter(0.8f)
    
    print(f"Created custom action: {action.getParameters()}")
    
    # Run simulation with learning
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward with curiosity
        reward = 0.5f + agent.getCuriosityLevel() * 0.5f
        agent.applyRewardModulation(reward, 0.0f)
        
        agent.updateDevelopment(0.1)
    
    print(f"\nAgent statistics:")
    print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  - Reward: {agent.getNeuromodulationLevel():.3f}")
    print(f"  - Development: {agent.getDevelopmentalStage()}")
    print()

if __name__ == '__main__':
    print("NLM Python Examples for Beginners")
    print("=" * 50)
    print()
    
    examples = [
        ("Basic Usage", basic_example),
        ("Complete System", complete_system_example),
        ("Configuration", configuration_example),
        ("Network Analysis", analysis_example),
        ("Advanced Agent", agent_example),
    ]
    
    for name, example in examples:
        print(f"--- {name} ---")
        try:
            example()
        except Exception as e:
            print(f"Error: {e}")
        print()
    
    print("All examples completed!")
    print("\nFor more information, visit:")
    print("  - https://github.com/nlm-project/nlm")
    print("  - https://nlm.readthedocs.io")