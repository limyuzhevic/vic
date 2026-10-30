#!/usr/bin/env python3
"""
Example script demonstrating enhanced NLM Python bindings

This script showcases the new high-level convenience functions and features
added to the NLM Python bindings.
"""

import pynlm
import time

def main():
    print("=== NLM Python Bindings - Enhanced Demo ===\n")
    
    # Example 1: Create configuration using convenience functions
    print("1. Creating configurations using convenience functions...")
    
    # Small configuration for quick testing
    small_config = pynlm.createSmallConfig()
    print(f"   Small config neurons: {small_config.getKeys()}")
    
    # Medium configuration for development
    medium_config = pynlm.createMediumConfig()
    print(f"   Medium config neurons: {medium_config.getOr('neuron_count', 'N/A')}")
    
    # Large configuration for research
    large_config = pynlm.createLargeConfig()
    print(f"   Large config regions: {large_config.getOr('region_count', 'N/A')}")
    
    print()
    
    # Example 2: Create a simple sensory-motor loop
    print("2. Creating a simple sensory-motor loop...")
    
    brain, world = pynlm.createSimpleSensoryMotorLoop(
        worldSize="16x16",
        neuronCount=300,
        enableDevelopment=True,
        enableMemory=True,
        enableNeuromodulation=True
    )
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"   Created world: {world.getWidth()}x{world.getHeight()} pixels")
    print(f"   Created agent with sensory input size: {agent.getSensoryInputSize()}")
    print(f"   Created agent with motor output size: {agent.getMotorOutputSize()}")
    
    print()
    
    # Example 3: Run a learning experiment
    print("3. Running a learning experiment...")
    
    results = pynlm.runSensoryMotorExperiment(
        brain, world,
        numEpisodes=5,
        maxStepsPerEpisode=100,
        enableVisualization=True
    )
    
    print(f"   Experiment completed:")
    print(f"   - Total reward: {results.totalReward:.2f}")
    print(f"   - Average firing rate: {results.avgFiringRate:.2f} Hz")
    print(f"   - Memory episodes stored: {results.memoryEpisodes}")
    print(f"   - Final curiosity level: {results.curiosityLevel:.2f}")
    print(f"   - Final novelty level: {results.noveltyLevel:.2f}")
    print(f"   - Final dopamine level: {results.dopamineLevel:.2f}")
    print(f"   - Total steps: {results.totalSteps}")
    
    print()
    
    # Example 4: Configuration validation
    print("4. Demonstrating configuration validation...")
    
    errors = []
    is_valid = pynlm.validateConfig(medium_config, errors)
    
    if is_valid:
        print("   ✓ Configuration is valid")
    else:
        print("   ✗ Configuration has errors:")
        for error in errors:
            print(f"     - {error}")
    
    print()
    
    # Example 5: Brain manipulation and analysis
    print("5. Brain manipulation and analysis...")
    
    # Check brain health
    brain_status = brain.getBrainState()
    print(f"   Brain status: {brain_status}")
    
    is_healthy = brain.isHealthy()
    print(f"   Brain healthy: {is_healthy}")
    
    # Get developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"   Current developmental stage: {stage}")
    
    # Set developmental stage to adult
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
    
    print("   ✓ Set developmental stage to Adult")
    
    print()
    
    # Example 6: Agent feature demonstration
    print("6. Agent feature demonstration...")
    
    # Enable different features
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"   Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    print(f"   Development enabled: {agent.isDevelopmentEnabled()}")
    print(f"   Curiosity enabled: {agent.isCuriosityEnabled()}")
    
    # Get current neuromodulation levels
    neuromodulation = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    print(f"   Neuromodulation level: {neuromodulation:.2f}")
    print(f"   Curiosity level: {curiosity:.2f}")
    print(f"   Novelty level: {novelty:.2f}")
    
    print()
    
    # Example 7: World interaction simulation
    print("7. World interaction simulation...")
    
    # Reset for fresh simulation
    agent.reset()
    brain.reset()
    world.reset()
    
    # Run a few interaction steps
    for step in range(10):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Produce action
        action = brain.produceAction()
        
        # Apply to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.01)
        
        # Log step
        print(f"   Step {step}: Reward={result.reward:.2f}, Success={result.success}")
    
    print()
    
    # Summary
    print("=== Summary ===")
    print(f"Successfully demonstrated {7} NLM Python binding features:")
    print("1. Configuration convenience functions")
    print("2. Simple sensory-motor loop creation")
    print("3. Learning experiment execution")
    print("4. Configuration validation")
    print("5. Brain manipulation and analysis")
    print("6. Agent feature management")
    print("7. World interaction simulation")
    print()
    print("The enhanced NLM Python bindings provide:")
    print("- High-level convenience functions for common use cases")
    print("- Built-in configuration validation and error handling")
    print("- Comprehensive example usage")
    print("- Enhanced documentation and type hints")
    print("- Ready-to-use workflows for experimentation")

if __name__ == "__main__":
    main()
