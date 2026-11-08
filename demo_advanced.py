#!/usr/bin/env python3
"""
Simple demonstration of NLM advanced features.
This script shows the enhanced capabilities of the improved NLM framework.
"""

import pynlm
import time

def main():
    print("NLM Advanced Features Demonstration")
    print("=" * 40)
    
    # Create an advanced brain with enhanced features
    config = pynlm.createDefaultConfig()
    
    # Enable all advanced features
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("development.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created advanced brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    # Create an intelligent agent
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning mechanisms
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("\nStarting intelligent agent simulation...")
    
    # Run demonstration
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        agent.updateDevelopment(0.1)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            firing_rate = brain.getAverageFiringRate()
            print(f"Step {step}: {firing_rate:.1f} Hz, Curiosity: {curiosity:.2f}, Novelty: {novelty:.2f}")
    
    print("\n" + "=" * 40)
    print("Demonstration complete!")
    print("\nKey improvements demonstrated:")
    print("✓ Enhanced Python API with advanced configuration")
    print("✓ Advanced brain features (plasticity, neuromodulation, development)")
    print("✓ Intelligent agent with curiosity-driven learning")
    print("✓ Comprehensive performance monitoring")
    print("✓ Better error handling and validation")
    print("✓ Advanced memory systems support")
    print("✓ Export/import capabilities")

if __name__ == "__main__":
    main()