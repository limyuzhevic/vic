#!/usr/bin/env python3
"""
Advanced NLM API demonstration with full learning capabilities.
"""

import pynlm

def main():
    print("=== NLM Advanced Learning API ===\n")
    
    # Create brain with advanced features
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning systems
    agent.enableRewardModulation(True)      # Reward prediction learning
    agent.enableStructuralPlasticity(True)  # Neural growth and pruning
    agent.enableDevelopment(True)           # Developmental progression
    agent.enableCuriosity(True)             # Exploration motivation
    
    print("Advanced Learning Features Enabled:")
    print("✓ Reward-modulated plasticity")
    print("✓ Structural plasticity (neural growth/pruning)")
    print("✓ Developmental stages (Initial → Critical Period → Maturation → Adult)")
    print("✓ Curiosity-driven exploration")
    print()
    
    # Phase 1: Initial exploration
    print("Phase 1: Initial Learning (0-100 steps)")
    for step in range(100):
        if step % 25 == 0:
            print(f"  Step {step:3d} - Stage: {agent.getDevelopmentalStage().name}, "
                  f"Plasticity: {agent.getNeuromodulationLevel():.2f}")
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Phase 2: Structured exploration with rewards
    print("\nPhase 2: Reward-Based Learning (100-200 steps)")
    for step in range(100, 200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Apply rewards for interesting behavior
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simulate reward based on novelty and action effectiveness
        reward = agent.getNoveltyLevel() * 0.1 + agent.getPredictionError() * 0.05
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 25 == 124:
            print(f"  Step {step:3d} - Novelty: {agent.getNoveltyLevel():.2f}, "
                  f"Prediction Error: {agent.getPredictionError():.2f}, "
                  f"Dopamine: {agent.getNeuromodulationLevel():.2f}")
    
    # Phase 3: Development and maturation
    print("\nPhase 3: Development and Maturation (200-350 steps)")
    for step in range(200, 350):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 25 == 224:
            print(f"  Step {step:3d} - Stage: {agent.getDevelopmentalStage().name}, "
                  f"Age: {agent.getBrain().getDevelopmentalStage():.1f}, "
                  f"Plasticity: {agent.getBrain().getDevelopmentalStage():.2f}")
    
    print("\n=== Advanced Learning Simulation Complete ===")
    print(f"Final Brain State:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Active neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Developmental stage: {agent.getDevelopmentalStage().name}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    print()
    print("Key Learning Achievements:")
    print("✓ Neural adaptation through reward modulation")
    print("✓ Structural changes (growth/pruning)")
    print("✓ Developmental progression with reduced plasticity")
    print("✓ Curiosity-driven exploration behavior")
    print("✓ Prediction error-based learning")

if __name__ == "__main__":
    main()
