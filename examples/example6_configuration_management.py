#!/usr/bin/env python3
"""
Example 6: Configuration Management

This example demonstrates various ways to manage and experiment with
different brain configurations.
"""

import sys
import os

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm

def main():
    print("\n=== Example 6: Configuration Management ===")
    
    # Create different configurations for experimentation
    # Configuration 1: High learning rate
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 1000)
    config1.set("brain.v_thresh", -45.0f)  # More excitable
    config1.set("plasticity.stdp.learning_rate", 0.02)
    config1.set("plasticity.hebbian.learning_rate", 0.02)
    config1.set("neuromod.curiosity.strength", 0.8)
    
    # Configuration 2: Low learning rate
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 500)
    config2.set("brain.v_thresh", -60.0f)  # Less excitable
    config2.set("plasticity.stdp.learning_rate", 0.001)
    config2.set("plasticity.hebbian.learning_rate", 0.001)
    config2.set("neuromod.curiosity.strength", 0.3)
    
    # Configuration 3: Balanced
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 1500)
    config3.set("brain.v_thresh", -55.0f)
    config3.set("plasticity.stdp.learning_rate", 0.01)
    config3.set("plasticity.hebbian.learning_rate", 0.01)
    config3.set("neuromod.curiosity.strength", 0.5)
    
    configurations = [config1, config2, config3]
    
    for i, config in enumerate(configurations, 1):
        print(f"\n--- Configuration {i} ---")
        
        # Save configuration
        filename = f"config_{i}.json"
        config.saveToFile(filename)
        print(f"Saved configuration to {filename}")
        
        # Load and verify
        loaded_config = pynlm.createDefaultConfig()
        loaded_config.loadFromFile(filename)
        
        print(f"  Neuron count: {loaded_config.getOr('brain.neuron_count', 0)}")
        print(f"  Threshold: {loaded_config.getOr('brain.v_thresh', 0.0)}")
        print(f"  STDP LR: {loaded_config.getOr('plasticity.stdp.learning_rate', 0.0)}")
    
    # Run experiment with each configuration
    print("\n=== Experiment Results ===")
    
    for i, config in enumerate(configurations, 1):
        print(f"\nRunning experiment with Configuration {i}...")
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
        world.reset()
        world.setAgentStart(4, 4)
        agent.initialize(world)
        
        # Run short test
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.0, 0.0)
        
        print(f"  Final firing rate: {brain.getFiringNeuronCount()} Hz")
        print(f"  Average rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Curiosiy: {agent.getCuriosityLevel():.3f}")
    
    # Clean up temporary files
    for i in range(1, 4):
        filename = f"config_{i}.json"
        if os.path.exists(filename):
            os.remove(filename)
            print(f"Cleaned up {filename}")
    
    print("\nConfiguration management example complete")
    return True

if __name__ == "__main__":
    main()
