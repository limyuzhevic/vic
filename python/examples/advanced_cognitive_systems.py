#!/usr/bin/env python3
"""
Advanced Cognitive Systems Example

This example demonstrates advanced usage of Phase 4+ cognitive systems:
- Complex planning with prediction integration
- Self-model learning and generalization
- Social learning and imitation
- Working memory consolidation
- Neuromodulation effects on learning
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm
import numpy as np

def example_advanced_planning():
    """Demonstrate advanced planning capabilities."""
    print("=== Advanced Planning Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    neural_planner = brain.get_cognitive_systems().get_neural_planner()
    prediction_system = brain.get_cognitive_systems().get_prediction_system()
    
    # Set planning parameters
    neural_planner.setPlanningDepth(5)
    neural_planner.setCurrentGoal(np.array([1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]))
    
    print(f"✓ Set planning depth: {neural_planner.getPlanningDepth()}")
    print(f"✓ Set current goal")
    
    # Test planning with prediction
    current_state = np.array([0.2, 0.3, 0.1, 0.4, 0.2, 0.1, 0.3, 0.2, 0.4, 0.1])
    
    # Make a prediction for planning
    predicted_state = prediction_system.predictNextState(current_state)
    print(f"✓ Predicted next state: {predicted_state.getAllSignals()[:5]}...")
    
    # Plan action based on prediction
    target_reward = 0.9
    best_action = neural_planner.planAction(current_state, target_reward)
    
    print(f"✓ Best action for goal: {best_action}")
    
    # Evaluate planning confidence
    confidence = neural_planner.getPlanningConfidence()
    print(f"✓ Planning confidence: {confidence:.3f}")

def example_concept_learning():
    """Demonstrate concept formation and generalization."""
    print("\n=== Concept Learning Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    concept_formation = brain.get_cognitive_systems().get_concept_formation()
    
    # Set concept formation parameters
    concept_formation.setFormationThreshold(0.6)
    concept_formation.setStabilityWindow(10)
    
    print(f"✓ Set formation threshold: 0.6")
    print(f"✓ Set stability window: 10")
    
    # Generate related patterns to form concepts
    base_pattern = np.array([1.0, 0.5, 0.3, 0.7, 0.2, 0.6, 0.4, 0.8, 0.3, 0.5])
    
    # Present variations of the same concept
    for i in range(10):
        variation = base_pattern * (1.0 + 0.1 * np.random.randn()) + np.random.rand(10) * 0.1
        variation = np.clip(variation, 0.0, 1.0)
        features = variation * 0.5 + np.random.rand(10) * 0.3
        reward = np.random.rand()
        
        concept_id = concept_formation.presentExperience(variation, features, reward, i)
        if concept_id > 0:
            print(f"  Concept {concept_id} formed at iteration {i+1}")
    
    print(f"✓ Total concepts formed: {concept_formation.getConceptCount()}")
    
    # Test concept stability
    if concept_formation.getConceptCount() > 0:
        concept_id = 1
        stability = concept_formation.getConceptStability(concept_id)
        generalization = concept_formation.getGeneralizationAbility(concept_id)
        print(f"✓ Concept stability: {stability:.3f}")
        print(f"✓ Concept generalization: {generalization:.3f}")

def example_memory_consolidation():
    """Demonstrate working memory consolidation."""
    print("\n=== Memory Consolidation Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    working_memory = brain.get_cognitive_systems().get_neural_working_memory()
    episodic_memory = brain.get_cognitive_systems().get_episodic_memory()
    
    # Store multiple patterns
    patterns = [np.random.rand(5).astype(float) for _ in range(5)]
    
    for i, pattern in enumerate(patterns):
        working_memory.store(pattern, 1.0)
        print(f"✓ Stored pattern {i+1}: {pattern[:3]}...")
    
    # Test competition and selection
    print("✓ Running competition...")
    working_memory.runCompetition()
    
    # Verify winners
    for i in range(min(3, len(patterns))):
        is_winner = working_memory.isWinning(i)
        print(f"  Pattern {i+1} is winner: {is_winner}")
    
    # Test memory strengthening
    working_memory.strengthenMemory(1.2)
    print("✓ Strengthened memory traces")
    
    # Test retrieval
    retrieved = working_memory.retrieve()
    print(f"✓ Retrieved memory: {retrieved[:3]}...")

def example_self_model_learning():
    """Demonstrate self-model learning and body schema."""
    print("\n=== Self-Model Learning Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    self_model = brain.get_cognitive_systems().get_self_model()
    
    # Simulate self-action learning
    actions = [nlm.ActionType.MoveForward, nlm.ActionType.TurnLeft, nlm.ActionType.Look]
    
    for i, action in enumerate(actions):
        for j in range(5):  # Learn each action multiple times
            before_state = np.random.rand(10).astype(float)
            after_state = before_state + np.random.rand(10) * 0.2
            
            self_model.recordSelfAction(action, before_state, after_state)
        
        confidence = self_model.getSelfModelConfidence(action)
        print(f"  Action {action}: confidence = {confidence:.3f}")
    
    # Test preferred action selection
    test_states = [np.array([0.1, 0.9, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]),
                   np.array([0.9, 0.1, 0.9, 0.1, 0.9, 0.1, 0.9, 0.1, 0.9, 0.1])]
    
    for i, state in enumerate(test_states):
        preferred = self_model.getPreferredAction(state)
        print(f"  State {i+1}: preferred action = {preferred}")
    
    print(f"✓ Body awareness: {self_model.getBodyAwareness():.3f}")

def example_social_imitation():
    """Demonstrate social learning and imitation."""
    print("\n=== Social Learning Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    social_learning = brain.get_cognitive_systems().get_social_learning()
    
    # Observe other agent's actions
    observed_actions = [nlm.ActionType.MoveForward, nlm.ActionType.TurnRight, nlm.ActionType.Interact]
    observer_state = np.random.rand(10).astype(float)
    
    for i, action in enumerate(observed_actions):
        resulting_state = observer_state + np.random.rand(10) * 0.3
        social_learning.observeAgentAction(action, observer_state, resulting_state)
        print(f"  Observed: {action} -> {resulting_state[:3]}...")
    
    # Test imitation capabilities
    test_state = np.random.rand(10).astype(float)
    imitation = social_learning.getImitationAction(test_state)
    can_imitate = social_learning.canImitate(nlm.ActionType.MoveForward)
    
    print(f"✓ Can imitate MoveForward: {can_imitate}")
    print(f"✓ Best action to imitate: {imitation}")
    
    # Test communication signal learning
    signal_pattern = np.random.rand(5).astype(float)
    signal_reward = np.random.rand()
    
    social_learning.learnCommunicationSignal(signal_pattern, signal_reward)
    detected = social_learning.detectSignal(signal_pattern)
    
    print(f"✓ Learned signal pattern: {signal_pattern[:3]}...")
    print(f"✓ Can detect learned signal: {detected}")

def example_prediction_integration():
    """Demonstrate prediction system integration."""
    print("\n=== Prediction Integration Example ===")
    
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    prediction_system = brain.get_cognitive_systems().get_prediction_system()
    
    # Test prediction accuracy
    test_states = [np.random.rand(10).astype(float) for _ in range(5)]
    
    for i, state in enumerate(test_states):
        predicted = prediction_system.predictNextState(state)
        prediction_system.updatePredictions(predicted, state)
        
        error = prediction_system.getPredictionError()
        confidence = prediction_system.getConfidence()
        
        print(f"  Prediction {i+1}: error={error:.3f}, confidence={confidence:.3f}")
    
    # Train prediction system
    training_states = [np.random.rand(10).astype(float) for _ in range(10)]
    
    for i, state in enumerate(training_states):
        prediction_system.train(state)
        
        if i % 3 == 2:
            pred = prediction_system.predictNextState(state)
            error = prediction_system.getPredictionError()
            print(f"  After training batch {i//3 + 1}: error={error:.3f}")

def main():
    """Run all advanced examples."""
    print("NLM Phase 4+ Advanced Cognitive Systems Examples")
    print("=" * 60)
    
    try:
        example_advanced_planning()
        example_concept_learning()
        example_memory_consolidation()
        example_self_model_learning()
        example_social_imitation()
        example_prediction_integration()
        
        print("\n" + "=" * 60)
        print("All advanced examples completed successfully!")
        
    except Exception as e:
        print(f"\nError during examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
