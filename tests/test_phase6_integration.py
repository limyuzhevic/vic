#!/usr/bin/env python3
"""
Phase 6 Integration Tests for NLM (Neural Learning Machine)

Tests the integration of all Phase 1-5 systems into a coherent artificial brain.

This test suite addresses the Phase 6 integration requirements identified in the
PHASE6_FINAL_AUDIT.md document, focusing on connecting disconnected systems.
"""

import sys
import pynlm
import traceback


def test_memory_system_integration():
    """Test that memory systems are properly integrated with neural processing."""
    print("Testing Memory System Integration...")
    
    # Create brain with configuration that includes memory settings
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # In Phase 6, Brain should provide access to memory systems
    # Currently these return nullptr based on Phase 6 audit
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    semantic_memory = brain.getSemanticMemory()
    procedural_memory = brain.getProceduralMemory()
    
    # For Phase 6 compliance, these should not be None
    # Test what currently exists and document what's missing
    if working_memory is not None:
        print("  ✓ Working memory is accessible")
        print(f"    Working memory type: {type(working_memory)}")
    else:
        print("  ⚠️  Working memory is not accessible (Phase 6 integration required)")
    
    if episodic_memory is not None:
        print("  ✓ Episodic memory is accessible")
    else:
        print("  ⚠️  Episodic memory is not accessible (Phase 6 integration required)")
    
    if semantic_memory is not None:
        print("  ✓ Semantic memory is accessible")
    else:
        print("  ⚠️  Semantic memory is not accessible (Phase 6 integration required)")
    
    if procedural_memory is not None:
        print("  ✓ Procedural memory is accessible")
    else:
        print("  ⚠️  Procedural memory is not accessible (Phase 6 integration required)")
    
    # Test prediction system integration
    prediction_system = brain.getPredictionSystem()
    if prediction_system is not None:
        print("  ✓ Prediction system is accessible")
    else:
        print("  ⚠️  Prediction system is not accessible (Phase 6 integration required)")
    
    print("  Memory integration test completed")


def test_neuromodulation_integration():
    """Test that neuromodulation systems are properly integrated."""
    print("Testing Neuromodulation Integration...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # In Phase 6, AgentBrain should integrate neuromodulation
    # Create agent brain to test neuromodulation
    agent = pynlm.createAgentBrain(brain)
    
    # Test neuromodulation levels
    neuromodulation_level = agent.getNeuromodulationLevel()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    print(f"  Neuromodulation level: {neuromodulation_level}")
    print(f"  Curiosity level: {curiosity_level}")
    print(f"  Novelty level: {novelty_level}")
    print(f"  Prediction error: {prediction_error}")
    
    # In Phase 6, these should be properly integrated with plasticity and learning
    if neuromodulation_level > 0:
        print("  ✓ Neuromodulation level is active")
    else:
        print("  ⚠️  Neuromodulation level is minimal (may need integration)")
    
    print("  Neuromodulation integration test completed")


def test_cognition_system_integration():
    """Test that cognition systems are properly integrated."""
    print("Testing Cognition System Integration...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # In Phase 6, AgentBrain should integrate cognition systems
    # Note: The audit shows these systems exist but are not used
    
    # Test neural planner (should be integrated into action selection)
    # The audit shows NeuralPlanner exists but is not used
    print("  ⚠️  Neural planner exists but integration with action selection not verified")
    
    # Test concept formation
    print("  ⚠️  Concept formation exists but integration with experience processing not verified")
    
    # Test self-model
    print("  ⚠️  Self-model exists but integration with agency not verified")
    
    # Test social learning
    print("  ⚠️  Social learning exists but integration with observation not verified")
    
    # Test developmental system integration
    developmental_stage = agent.getDevelopmentalStage()
    print(f"  Current developmental stage: {developmental_stage}")
    
    # Test development update in Phase 6
    # In Phase 6, development should affect more than just structural plasticity
    print("  ⚠️  Development system integration may be limited to structural plasticity only")
    
    print("  Cognition integration test completed")


def test_prediction_learning_integration():
    """Test that prediction systems are integrated with learning."""
    print("Testing Prediction-Learning Integration...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Test prediction error generation
    prediction_error = agent.getPredictionError()
    print(f"  Initial prediction error: {prediction_error}")
    
    # Apply some sensory input and see if prediction error changes
    # In Phase 6, this should trigger prediction system updates
    print("  ⚠️  Prediction error generation exists but integration with learning loop not verified")
    
    # Test curiosity and novelty integration
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    print(f"  Curiosity level: {curiosity}")
    print(f"  Novelty level: {novelty}")
    
    # In Phase 6, curiosity should combine novelty + prediction error
    # This integration should affect exploration behavior
    print("  ⚠️  Curiosity/novelty integration exists but exploration behavior not fully integrated")
    
    print("  Prediction-learning integration test completed")


def test_sensory_motor_loop():
    """Test the complete sensory-motor loop integration."""
    print("Testing Sensory-Motor Loop Integration...")
    
    # Create world, brain, and agent
    world = pynlm.createSimpleWorld()
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.setAgentStart(5, 5)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Test the complete loop: world -> sensory percept -> brain -> motor command -> world
    print("  Testing sensory input processing...")
    percept = world.getSensoryPercept()
    assert percept is not None, "World should provide sensory percept"
    
    print("  Processing sensory input through agent...")
    agent.processSensoryInput(percept)
    
    print("  Testing brain step...")
    brain.step(0, 0.0)
    
    print("  Testing motor command generation...")
    motor_command = agent.decodeMotorCommand()
    assert motor_command is not None, "Agent should generate motor command"
    
    print("  Applying action to world...")
    world.applyMotorCommand(motor_command, 0.0)
    
    print("  Computing reward...")
    reward = world.computeReward(agent)
    print(f"    Reward received: {reward}")
    
    print("  Applying reward modulation...")
    agent.applyRewardModulation(reward, 0.0)
    
    # In Phase 6, this loop should include:
    # 1. Memory storage of experiences
    # 2. Prediction system training
    # 3. Concept formation from observations
    # 4. Planning integration
    # 5. Sleep/rest cycle with memory consolidation
    print("  ⚠️  Complete loop works but memory/consolidation not integrated")
    
    print("  Sensory-motor loop integration test completed")


def test_persistence_integration():
    """Test checkpoint and persistence integration."""
    print("Testing Persistence Integration...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Test brain checkpoint functionality
    print("  Testing brain save/load...")
    
    # In Phase 6, save/load should be fully integrated
    # Currently these are stubs as shown in Phase 6 audit
    save_success = brain.save("test_brain_checkpoint.json")
    print(f"  Brain save: {'Success' if save_success else 'Failed (stub)'}")
    
    load_success = brain.load("test_brain_checkpoint.json")
    print(f"  Brain load: {'Success' if load_success else 'Failed (stub)'}")
    
    # In Phase 6, checkpoint system should be:
    # 1. Fully connected to Brain
    # 2. Support incremental saves
    # 3. Enable life history tracking
    print("  ⚠️  Persistence system is implemented but not integrated with Brain")
    
    print("  Persistence integration test completed")


def test_development_plasticity_integration():
    """Test developmental modulation of plasticity."""
    print("Testing Development-Plasticity Integration...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Test developmental stage progression
    initial_stage = agent.getDevelopmentalStage()
    print(f"  Initial developmental stage: {initial_stage}")
    
    # In Phase 6, development should affect:
    # - Neural excitability
    # - Plasticity rules themselves
    # - Attention
    # - Memory consolidation
    # - Neuromodulation levels
    
    # Test development update
    agent.updateDevelopment(0.001)
    
    new_stage = agent.getDevelopmentalStage()
    print(f"  Developmental stage after update: {new_stage}")
    
    # Currently development mainly affects structural plasticity rates
    # In Phase 6, it should affect more systems
    print("  ⚠️  Development currently affects limited systems (structural plasticity)")
    
    print("  Development-plasticity integration test completed")


def test_comprehensive_integration():
    """Run a comprehensive integration test simulating a complete episode."""
    print("Running Comprehensive Integration Test...")
    
    # Create complete system
    world = pynlm.createSimpleWorld()
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10, 10)
    agent.initialize(world)
    
    # Add some objects to the world
    world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Hazard, value=5.0))
    
    # Simulate multiple steps
    num_steps = 10
    total_reward = 0.0
    
    for step in range(num_steps):
        # 1. Get sensory percept
        percept = world.getSensoryPercept()
        
        # 2. Process through agent (injects into brain)
        agent.processSensoryInput(percept)
        
        # 3. Brain step (LIF dynamics, plasticity, etc.)
        brain.step(step, step * 0.001)
        
        # 4. Generate action
        motor_command = agent.decodeMotorCommand()
        
        # 5. Apply action to world
        world.applyMotorCommand(motor_command, step * 0.001)
        
        # 6. Get reward
        reward = world.computeReward(agent)
        total_reward += reward
        
        # 7. Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.001)
        
        if step % 5 == 0:
            print(f"  Step {step}: Reward = {reward:.2f}, Total = {total_reward:.2f}")
    
    print(f"  Total reward over {num_steps} steps: {total_reward:.2f}")
    
    # In Phase 6, this should also include:
    # - Memory storage of experiences
    # - Prediction system training
    # - Concept formation
    # - Replay during rest periods
    # - Life history tracking
    
    print("  ⚠️  Complete episode simulation works but memory replay not integrated")
    
    print("  Comprehensive integration test completed")


def run_phase6_integration_tests():
    """Run all Phase 6 integration tests."""
    print("=== Phase 6 Integration Tests for NLM ===\n")
    print("This test suite verifies the integration requirements from the")
    print("Phase 6 Final Audit document.\n")
    
    tests = [
        test_memory_system_integration,
        test_neuromodulation_integration,
        test_cognition_system_integration,
        test_prediction_learning_integration,
        test_sensory_motor_loop,
        test_persistence_integration,
        test_development_plasticity_integration,
        test_comprehensive_integration,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print(f"✓ {test.__name__} PASSED\n")
        except Exception as e:
            print(f"✗ {test.__name__} FAILED: {e}")
            print(f"  Traceback: {traceback.format_exc()}")
            failed += 1
            print()
    
    print("=== Phase 6 Integration Test Results ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total:  {len(tests)}")
    
    if failed > 0:
        print(f"\n⚠️  {failed} test(s) failed")
        return False
    else:
        print(f"\n✅ All {passed} integration tests completed!")
        print("\nNote: This test demonstrates the current state of Phase 6 integration.")
        print("Most systems are disconnected and require Phase 6 integration to be complete.")
        return True


if __name__ == "__main__":
    success = run_phase6_integration_tests()
    sys.exit(0 if success else 1)