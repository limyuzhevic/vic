#!/usr/bin/env python3
"""
Example: Phase 4+ Cognitive Systems with NLM

This example demonstrates the use of Phase 4+ cognitive systems:
- NeuralPlanner for action planning
- ConceptFormation for concept learning
- NeuralWorkingMemory for working memory
- Neuromodulators for motivation
- PredictionSystem for forward models
- SelfModel for body schema
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
import numpy as np

def example_basic_cognition():
    """Demonstrate basic cognitive system usage."""
    print("=== Basic Cognitive Systems Example ===")
    
    # Create configuration and brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # Initialize cognitive systems
    neural_planner = brain.get_cognitive_systems().get_neural_planner()
    concept_formation = brain.get_cognitive_systems().get_concept_formation()
    prediction_system = brain.get_cognitive_systems().get_prediction_system()
    
    print("✓ Created cognitive systems")
    
    # Test concept formation with some simple experiences
    for i in range(5):
        # Simulate sensory input as random pattern
        pattern = np.random.rand(10).astype(float)
        features = pattern * 0.5 + np.random.rand(10) * 0.5
        reward = np.random.rand() - 0.5  # Random reward
        
        concept_id = concept_formation.presentExperience(pattern, features, reward, 0)
        print(f"  Experience {i+1}: Pattern similarity = {concept_id}")
    
    print(f"✓ Discovered {concept_formation.getConceptCount()} concepts")
    
    # Test planning
    current_state = np.random.rand(10).astype(float)
    target_reward = 0.8
    
    best_action = neural_planner.planAction(current_state, target_reward)
    print(f"✓ Planned action: {best_action}")
    
    # Test prediction
    prediction = prediction_system.predictNextState(current_state)
    print(f"✓ Made prediction: {prediction}")
    
    # Test neuromodulation
    curiosity = brain.get_neuromodulators().get_curiosity()
    curiosity_level = curiosity.getCuriosityLevel()
    print(f"✓ Curiosity level: {curiosity_level:.3f}")

def example_working_memory():
    """Demonstrate working memory usage."""
    print("\n=== Working Memory Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    working_memory = brain.get_cognitive_systems().get_neural_working_memory()
    
    # Store some information
    pattern = np.random.rand(5).astype(float)
    print(f"✓ Storing pattern: {pattern[:3]}...")
    
    working_memory.store(pattern, 1.0)
    
    # Retrieve and verify
    retrieved = working_memory.retrieve()
    print(f"✓ Retrieved pattern: {retrieved[:3]}...")
    
    # Test competition
    print("✓ Running memory competition...")
    working_memory.runCompetition()
    
    # Test strengthening
    working_memory.strengthenMemory(1.5)
    print("✓ Strengthened memory")

def example_neuromodulation():
    """Demonstrate neuromodulation integration."""
    print("\n=== Neuromodulation Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # Get all neuromodulators
    reward = brain.get_neuromodulators().get_reward()
    curiosity = brain.get_neuromodulators().get_curiosity()
    prediction_error = brain.get_neuromodulators().get_prediction_error()
    novelty = brain.get_neuromodulators().get_novelty()
    
    print("✓ Created neuromodulators")
    
    # Process some inputs
    test_state = np.random.rand(10).astype(float)
    test_action = nlm.ActionType.ActionType.Wait
    
    reward.processReward(test_state, test_state, test_action, 0.5)
    curiosity.processSensoryInput(test_state)
    prediction_error.processPrediction(test_state, test_state)
    novelty.processInput(test_state)
    
    print(f"  Reward prediction error: {reward.getPredictionError():.3f}")
    print(f"  Curiosity level: {curiosity.getCuriosityLevel():.3f}")
    print(f"  Prediction error: {prediction_error.getPredictionError():.3f}")
    print(f"  Novelty level: {novelty.getNoveltyLevel():.3f}")

def example_self_model():
    """Demonstrate self-model usage."""
    print("\n=== Self-Model Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    self_model = brain.get_cognitive_systems().get_self_model()
    
    print("✓ Created self model")
    
    # Test with some simulated experiences
    for i in range(3):
        before_state = np.random.rand(10).astype(float)
        action = nlm.ActionType.ActionType.MoveForward
        after_state = before_state * 0.9 + np.random.rand(10) * 0.2
        
        self_model.recordSelfAction(action, before_state, after_state)
        confidence = self_model.getSelfModelConfidence(action)
        print(f"  Experience {i+1}: Self-model confidence for {action} = {confidence:.3f}")
    
    preferred_action = self_model.getPreferredAction(np.random.rand(10).astype(float))
    print(f"✓ Preferred action: {preferred_action}")
    print(f"✓ Body awareness: {self_model.getBodyAwareness():.3f}")

def example_complete_loop():
    """Demonstrate a complete cognitive loop."""
    print("\n=== Complete Cognitive Loop Example ===")
    
    # Create world and agent
    world = nlm.createSimpleWorld()
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    agent_brain = nlm.createAgentBrain(brain)
    
    print("✓ Created complete agent-brain system")
    
    # Run a simple cognitive loop
    for step in range(5):
        print(f"\n--- Step {step + 1} ---")
        
        # Get sensory input
        percept = world.getSensoryPercept()
        print(f"  Sensory input: {percept.getAllSignals()[:3]}...")
        
        # Process through cognitive systems
        action = agent_brain.decodeMotorCommand()
        print(f"  Selected action: {action}")
        
        # Apply action in world
        result = world.applyMotorCommand(action, 0.0)
        print(f"  Action result: reward={result.reward:.3f}, success={result.success}")
        
        # Update curiosity based on experience
        curiosity = brain.get_neuromodulators().get_curiosity()
        curiosity_level = curiosity.getCuriosityLevel()
        print(f"  Curiosity level: {curiosity_level:.3f}")
        
        # Small sleep for visualization
        import time
        time.sleep(0.1)
    
    print("\n✓ Complete cognitive loop finished")

def main():
    """Run all examples."""
    print("NLM Phase 4+ Cognitive Systems - Python Bindings Examples")
    print("=" * 60)
    
    try:
        example_basic_cognition()
        example_working_memory()
        example_neuromodulation()
        example_self_model()
        example_complete_loop()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("\nFor more comprehensive examples, see:")
        print("  - python/examples/advanced_cognition.py")
        print("  - python/examples/brain_development.py")
        print("  - python/examples/social_learning.py")
        
    except Exception as e:
        print(f"\nError during examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
