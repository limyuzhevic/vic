#!/usr/bin/env python3
"""Demonstrate neuromodulation features in NLM.

This example shows how neuromodulators affect brain function:
- Dopamine: reward prediction and learning
- Curiosity: exploration motivation  
- Novelty: detection of new patterns
- Prediction error: learning from mistakes
"""

import pynlm
import sys
import math
import random

def demonstrate_dopamine():
    """Demonstrate dopamine neuromodulation."""
    print("=== Dopamine Neuromodulation ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.scale", 2.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    print("Setting up reward-based learning...")
    
    # Simulate exploration with rewards
    for step in range(40):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        body = world.getAgentBody()
        target = (12.0, 12.0)
        distance = math.sqrt((body.x - target[0])**2 + (body.y - target[1])**2)
        reward = max(0, 10.0 - distance)
        
        # Apply dopamine-modulated reward learning
        predicted = 0.0  # Simplified prediction
        agent.applyRewardModulation(reward, predicted)
        
        if step % 10 == 0:
            print(f"  Step {step}: Pos({body.x:.1f},{body.y:.1f}), "
                  f"Reward {reward:.1f}, "
                  f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    
    print("✓ Dopamine-based learning completed")
    return agent

def demonstrate_curiosity():
    """Demonstrate curiosity-driven exploration."""
    print("\n=== Curiosity-Driven Exploration ===")
    
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    
    print("Setting up curiosity-driven exploration...")
    
    exploration_count = 0
    
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get curiosity level
        curiosity = agent.getCuriosityLevel()
        
        # Choose action based on curiosity
        if curiosity > 0.7 and exploration_count < 5:
            # High curiosity = explore new areas
            action = pynlm.Action(pynlm.ActionType.TurnLeft)
            exploration_count += 1
            print(f"    Step {step}: Exploring (curiosity={curiosity:.3f})")
        else:
            # Normal behavior
            action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"  Step {step}: Pos({body.x:.1f},{body.y:.1f}), "
                  f"Curiosity: {curiosity:.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    print(f"✓ Curiosity-driven exploration completed ({exploration_count} exploratory actions)")
    return agent

def demonstrate_novelty_detection():
    """Demonstrate novelty detection and adaptation."""
    print("\n=== Novelty Detection and Adaptation ===")
    
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableNovelty(True) if hasattr(agent, 'enableNovelty') else None
    
    print("Setting up novelty detection...")
    
    # Create predictable and novel patterns
    predictable_pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
    novel_pattern = [0.9, -0.8, 0.3, -0.2, 0.7]
    
    print("Presenting predictable pattern first:")
    for i in range(25):
        signal = predictable_pattern[i % len(predictable_pattern)]
        
        internal = pynlm.InternalSignals()
        internal.addSignal(signal)
        
        agent.processSensoryInput(internal)
        brain.step(i)
        
        novelty = agent.getNoveltyLevel()
        if i % 5 == 0:
            print(f"  Step {i}: Signal {signal:.1f}, Novelty {novelty:.3f}")
    
    print("\nIntroducing novel pattern:")
    for i in range(25, 50):
        # Switch to novel pattern
        signal = novel_pattern[(i - 25) % len(novel_pattern)]
        
        internal = pynlm.InternalSignals()
        internal.addSignal(signal)
        
        agent.processSensoryInput(internal)
        brain.step(i)
        
        novelty = agent.getNoveltyLevel()
        if i % 5 == 0:
            print(f"  Step {i}: Signal {signal:.1f}, Novelty {novelty:.3f}")
        
        # Novelty should trigger learning/adaptation
        if novelty > 0.5:
            print(f"    *** High novelty detected! Triggering adaptation...")
    
    print("✓ Novelty detection and adaptation completed")
    return agent

def demonstrate_prediction_error():
    """Demonstrate prediction error learning."""
    print("\n=== Prediction Error Learning ===")
    
    config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Setting up prediction error learning...")
    
    # Simulate prediction vs actual outcomes
    predictions = [0.0] * 40
    actual_rewards = []
    
    print("Simulating actions with prediction errors:")
    
    for step in range(40):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Make a prediction
        prediction = predictions[step % len(predictions)]
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simulate actual outcome (with some randomness)
        body = world.getAgentBody()
        base_reward = (math.sin(step * 0.2) + 1) / 2  # 0 to 1
        noise = random.uniform(-0.1, 0.1)
        actual = base_reward + noise
        
        actual_rewards.append(actual)
        
        # Apply prediction error learning
        error = actual - prediction
        agent.applyRewardModulation(actual, prediction)
        
        # Update predictions based on error
        predictions[step % len(predictions)] = actual
        
        if step % 10 == 0:
            print(f"  Step {step}: Pred {prediction:.2f}, Actual {actual:.2f}, "
                  f"Error {error:+.2f}, Neuromod {agent.getNeuromodulationLevel():.3f}")
    
    # Calculate prediction error statistics
    if len(actual_rewards) > 0:
        avg_error = sum(abs(actual_rewards[i] - predictions[i % len(predictions)]) 
                       for i in range(len(actual_rewards))) / len(actual_rewards)
        print(f"\nPrediction error statistics:")
        print(f"  Average absolute error: {avg_error:.3f}")
        print(f"  Mean actual reward: {sum(actual_rewards)/len(actual_rewards):.3f}")
    
    print("✓ Prediction error learning completed")
    return agent

def demonstrate_neuromodulation_integration():
    """Demonstrate integration of all neuromodulators."""
    print("\n=== Complete Neuromodulation Integration ===")
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.scale", 1.5)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=18, height=18, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(9.0, 9.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    
    print("Integrating all neuromodulation systems...")
    
    for step in range(60):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward calculation
        body = world.getAgentBody()
        target = (15.0, 15.0)
        distance = math.sqrt((body.x - target[0])**2 + (body.y - target[1])**2)
        reward = max(0, 15.0 - distance) * 0.1
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 15 == 0:
            print(f"  Step {step}:")
            print(f"    Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"    Neuromod level: {agent.getNeuromodulationLevel():.3f}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Prediction error: {agent.getPredictionError():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    print("✓ Complete neuromodulation integration completed")
    return agent

def main():
    print("=== NLM Neuromodulation Demo ===\n")
    
    # Run all demonstrations
    print("1. Dopamine Neuromodulation")
    demonstrate_dopamine()
    
    print("\n2. Curiosity-Driven Exploration")
    demonstrate_curiosity()
    
    print("\n3. Novelty Detection and Adaptation")
    demonstrate_novelty_detection()
    
    print("\n4. Prediction Error Learning")
    demonstrate_prediction_error()
    
    print("\n5. Complete Neuromodulation Integration")
    demonstrate_neuromodulation_integration()
    
    print("\n=== Neuromodulation Demo Complete ===")
    print("\nNeuromodulation capabilities demonstrated:")
    print("• Dopamine-based reward learning")
    print("• Curiosity-driven exploration")
    print("• Novelty detection and adaptation")
    print("• Prediction error learning")
    print("• Integrated neuromodulation system")
    print("\nAll neuromodulators working together to guide adaptive behavior!")

if __name__ == "__main__":
    try:
        main()
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Make sure NLM Python bindings are installed.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running neuromodulation demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
