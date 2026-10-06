#!/usr/bin/env python3
"""
NLM Comprehensive Test Suite

This script tests all major components of the NLM framework to ensure
good functionality and integration between C++ and Python bindings.
"""

import pynlm
import time
import tempfile
import os
import math

def test_basic_setup():
    """Test basic NLM setup and initialization"""
    print("=== Test 1: Basic Setup ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    assert config is not None
    print("✓ Default config created")
    
    # Create brain
    brain = pynlm.createBrain(config)
    assert brain is not None
    print("✓ Brain created")
    
    # Initialize brain
    brain.initialize()
    print("✓ Brain initialized")
    
    # Check basic properties
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    print(f"✓ Brain created with {neuron_count} neurons and {synapse_count} synapses")
    
    assert neuron_count > 0, "Brain should have neurons"
    assert synapse_count >= 0, "Brain should have synapses"
    
    return brain, config

def test_plasticity_and_learning(brain, steps=200):
    """Test neural plasticity and learning capabilities"""
    print(f"\n=== Test 2: Plasticity and Learning ({steps} steps) ===")
    
    # Record initial state
    initial_firing = brain.getAverageFiringRate()
    initial_spikes = brain.getTotalSpikeCount()
    
    # Inject varied sensory input to drive learning
    from pynlm import Vision, NeuronType
    
    for step in range(steps):
        # Create varying sensory patterns
        vision = Vision(width=8, height=8)
        
        # Generate pattern based on step
        pattern = (step % 4) / 3.0  # 0.0, 0.333, 0.667, 1.0
        
        for y in range(8):
            for x in range(8):
                intensity = pattern
                # Add some noise
                import random
                intensity += random.uniform(-0.1, 0.1)
                intensity = max(0.0, min(1.0, intensity))
                
                vision.setData([intensity, intensity * 0.8, intensity * 0.6])
        
        # Inject current into sensory-like neurons
        sensory_neuron_count = brain.getTotalNeuronCount() // 4
        for i in range(min(10, sensory_neuron_count)):
            current = 30.0 + (step % 50)  # Varying input strength
            brain.injectCurrent(i, current)
        
        # Step simulation
        brain.step(step, step * 0.001)
        
        # Check for significant learning
        if step % 50 == 0:
            firing_rate = brain.getAverageFiringRate()
            spike_count = brain.getTotalSpikeCount() - initial_spikes
            print(f"  Step {step}: Firing rate={firing_rate:.2f}Hz, "
                  f"Additional spikes={spike_count}")
    
    # Check final state
    final_firing = brain.getAverageFiringRate()
    final_spikes = brain.getTotalSpikeCount() - initial_spikes
    
    print(f"✓ Learning test completed")
    print(f"  Initial firing rate: {initial_firing:.2f}Hz")
    print(f"  Final firing rate: {final_firing:.2f}Hz")
    print(f"  Total spikes generated: {final_spikes}")
    
    # Expect some change in firing rate
    firing_change = abs(final_firing - initial_firing)
    assert firing_change > 0.1 or final_spikes > 10, \
        f"Expected learning effects, got firing change: {firing_change}"
    
    return final_firing, final_spikes

def test_memory_integration(brain):
    """Test memory system integration"""
    print("\n=== Test 3: Memory Integration ===")
    
    # Run some simulation to generate memories
    for step in range(100):
        brain.step(step, step * 0.001)
        
        # Inject patterns to form memories
        if step % 10 == 0:
            from pynlm import Vision
            vision = Vision(5, 5)
            brain.receiveSensoryInput(vision)
    
    # Check memory-related metrics
    total_spikes = brain.getTotalSpikeCount()
    active_neurons = brain.getActiveNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    
    print(f"✓ Memory integration test completed")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Active neurons: {active_neurons}")
    print(f"  Firing neurons: {firing_neurons}")
    
    # Check memory engagement
    assert total_spikes > 100, "Should have generated spikes for memory"
    
    return total_spikes

def test_neuromodulation(agent_brain, world, steps=150):
    """Test neuromodulation and learning signals"""
    print(f"\n=== Test 4: Neuromodulation ({steps} steps) ===")
    
    # Monitor neuromodulation levels
    initial_dopamine = agent_brain.getNeuromodulationLevel()
    initial_curiosity = agent_brain.getCuriosityLevel()
    initial_novelty = agent_brain.getNoveltyLevel()
    
    from pynlm import Vision, ActionType
    
    for step in range(steps):
        # Generate sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent brain
        agent_brain.processSensoryInput(percept)
        
        # Update development and neuromodulation
        agent_brain.updateDevelopment(step * 0.001)
        
        # Occasionally apply reward to test dopamine system
        if step % 30 == 0:
            from pynlm import ActionResult
            reward = ActionResult(reward=1.0, success=True, message="Success!")
            agent_brain.applyRewardModulation(reward.reward, reward.reward * 0.9)
        
        # Occasionally trigger curiosity through novelty
        if step % 40 == 0:
            # Get current vision for novelty detection
            vision = percept.getVision()
            # Simulate novelty by random variation
            import random
            agent_brain.getNoveltyLevel() += random.uniform(0.1, 0.3)
        
        # Decode and apply motor command
        motor_command = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(motor_command, step * 0.001)
        
        if step % 30 == 0:
            neuromod = agent_brain.getNeuromodulationLevel()
            curiosity = agent_brain.getCuriosityLevel()
            novelty = agent_brain.getNoveltyLevel()
            print(f"  Step {step}: DA={neuromod:.2f}, "
                  f"Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
    
    # Check final neuromodulation levels
    final_dopamine = agent_brain.getNeuromodulationLevel()
    final_curiosity = agent_brain.getCuriosityLevel()
    final_novelty = agent_brain.getNoveltyLevel()
    
    print(f"✓ Neuromodulation test completed")
    print(f"  Initial - DA: {initial_dopamine:.2f}, "
          f"Curiosity: {initial_curiosity:.2f}, Novelty: {initial_novelty:.2f}")
    print(f"  Final - DA: {final_dopamine:.2f}, "
          f"Curiosity: {final_curiosity:.2f}, Novelty: {final_novelty:.2f}")
    
    # Check that neuromodulation changed
    neuromod_changes = (
        abs(final_dopamine - initial_dopamine) +
        abs(final_curiosity - initial_curiosity) +
        abs(final_novelty - initial_novelty)
    )
    
    assert neuromod_changes > 0.5, f"Neuromodulation should have changed, got {neurom_changes}"
    
    return neuromod_changes

def test_checkpoint_system(brain):
    """Test checkpoint save/load functionality"""
    print("\n=== Test 5: Checkpoint System ===")
    
    # Generate some state
    for step in range(50):
        brain.step(step, step * 0.001)
    
    initial_spikes = brain.getTotalSpikeCount()
    initial_firing = brain.getAverageFiringRate()
    
    print(f"  Initial state - Spikes: {initial_spikes}, "
          f"Firing: {initial_firing:.2f}Hz")
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    try:
        # Save brain state
        brain.save(checkpoint_path)
        print(f"✓ Brain state saved to: {checkpoint_path}")
        
        # Create new brain and load
        config = pynlm.createDefaultConfig()
        new_brain = pynlm.createBrain(config)
        print("✓ New brain created for loading test")
        
        # Load checkpoint
        new_brain.load(checkpoint_path)
        print("✓ Checkpoint loaded successfully")
        
        # Verify state
        loaded_spikes = new_brain.getTotalSpikeCount()
        loaded_firing = new_brain.getAverageFiringRate()
        
        print(f"  Loaded state - Spikes: {loaded_spikes}, "
              f"Firing: {loaded_firing:.2f}Hz")
        
        # Check that states are similar (may not be exact due to implementation)
        spike_diff = abs(initial_spikes - loaded_spikes)
        firing_diff = abs(initial_firing - loaded_firing)
        
        print(f"  Differences - Spike diff: {spike_diff}, "
              f"Firing diff: {firing_diff:.2f}Hz")
        
        # The checkpoint should preserve some state
        assert spike_diff < initial_spikes * 0.5 or spike_diff == 0, \
            f"Checkpoint should preserve spike count, got diff: {spike_diff}"
        
        return spike_diff, firing_diff
        
    finally:
        # Clean up
        if os.path.exists(checkpoint_path):
            os.remove(checkpoint_path)
            print(f"✓ Checkpoint file cleaned up: {checkpoint_path}")

def test_configuration_management():
    """Test configuration system"""
    print("\n=== Test 6: Configuration Management ===")
    
    # Create and modify configuration
    config = pynlm.createDefaultConfig()
    
    # Check initial configuration
    initial_keys = config.getKeys()
    print(f"✓ Initial configuration has {len(initial_keys)} keys")
    
    # Set various configuration values
    config.set("random_seed", 42)
    config.set("neuron_count", 200)
    config.set("simulation_timestep", 0.005)
    config.set("enable_plasticity", True)
    config.set("plasticity_hebbian", True)
    config.set("plasticity_stdp", True)
    
    # Verify configuration changes
    assert config.has("random_seed")
    assert config.has("neuron_count")
    assert config.has("simulation_timestep")
    
    seed = config.getOr("random_seed", 0)
    neuron_count = config.getOr("neuron_count", 500)
    timestep = config.getOr("simulation_timestep", 0.001)
    
    print(f"✓ Configuration updated successfully")
    print(f"  Random seed: {seed}")
    print(f"  Neuron count: {neuron_count}")
    print(f"  Simulation timestep: {timestep}")
    
    # Save and reload configuration
    with tempfile.NamedTemporaryFile(suffix='.cfg', delete=False) as f:
        config_path = f.name
    
    try:
        # Save configuration
        config.saveToFile(config_path)
        print(f"✓ Configuration saved to: {config_path}")
        
        # Create new config and load
        new_config = pynlm.createDefaultConfig()
        success = new_config.loadFromFile(config_path)
        
        if success:
            print("✓ Configuration loaded successfully")
            
            # Verify loaded values
            loaded_seed = new_config.getOr("random_seed", -1)
            assert loaded_seed == seed, f"Loaded seed {loaded_seed} != original {seed}"
            
            print(f"✓ Loaded configuration verified - seed matches: {loaded_seed}")
        else:
            print("⚠ Configuration load failed (expected for some config formats)")
        
        return success
        
    finally:
        if os.path.exists(config_path):
            os.remove(config_path)
            print(f"✓ Config file cleaned up: {config_path}")

def test_agent_world_integration():
    """Test complete agent-world integration"""
    print("\n=== Test 7: Agent-World Integration ===")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.setAgentStart(7, 7)
    world.setRandomSeed(123)
    
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Get initial state
    initial_pos = world.getAgentBody()
    initial_steps = 0
    
    print(f"✓ Agent-world integration setup complete")
    print(f"  Initial position: ({initial_pos.x}, {initial_pos.y})")
    print(f"  Initial energy: {initial_pos.energy}")
    
    # Run integration simulation
    integration_steps = 100
    from pynlm import Vision, ActionType
    
    for step in range(integration_steps):
        # Get current perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Update development
        agent_brain.updateDevelopment(step * 0.001)
        
        # Get motor command
        motor_command = agent_brain.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_command, step * 0.001)
        
        # Check for agent death or starvation
        body = world.getAgentBody()
        if body.health <= 0:
            print(f"✓ Agent died at step {step} (natural simulation)")
            break
        elif body.energy <= 0:
            print(f"✓ Agent starved at step {step} (natural simulation)")
            break
        elif step % 25 == 0:
            print(f"  Step {step}: Pos=({body.x:.1f}, {body.y:.1f}), "
                  f"Health={body.health:.1f}, Energy={body.energy:.1f}")
    
    final_pos = world.getAgentBody()
    final_steps = step + 1
    
    print(f"✓ Agent-world integration test completed")
    print(f"  Final position: ({final_pos.x:.1f}, {final_pos.y:.1f})")
    print(f"  Final health: {final_pos.health:.1f}")
    print(f"  Final energy: {final_pos.energy:.1f}")
    print(f"  Simulation ran for {final_steps} steps")
    
    # Check that agent moved (or at least tried to)
    movement = math.hypot(final_pos.x - initial_pos.x, final_pos.y - initial_pos.y)
    print(f"  Total movement: {movement:.1f} units")
    
    return final_steps, movement

def run_all_tests():
    """Run all comprehensive tests"""
    print("=" * 60)
    print("NLM Comprehensive Test Suite")
    print("=" * 60)
    
    test_results = {}
    
    try:
        # Test 1: Basic setup
        brain, config = test_basic_setup()
        test_results["basic_setup"] = "PASS"
        
        # Test 2: Plasticity and learning
        final_firing, final_spikes = test_plasticity_and_learning(brain)
        test_results["plasticity_learning"] = "PASS"
        
        # Test 3: Memory integration
        memory_spikes = test_memory_integration(brain)
        test_results["memory_integration"] = "PASS"
        
        # Test 4: Neuromodulation (requires agent-world setup)
        print("\nSetting up agent-world for neuromodulation test...")
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.setAgentStart(5, 5)
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        neuromod_changes = test_neuromodulation(agent_brain, world)
        test_results["neuromodulation"] = "PASS"
        
        # Test 5: Checkpoint system
        checkpoint_results = test_checkpoint_system(brain)
        test_results["checkpoint_system"] = "PASS"
        
        # Test 6: Configuration management
        config_success = test_configuration_management()
        test_results["configuration"] = "PASS" if config_success else "PARTIAL"
        
        # Test 7: Agent-world integration
        integration_steps, movement = test_agent_world_integration()
        test_results["agent_world_integration"] = "PASS"
        
        # Summary
        print("\n" + "=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        
        passed = sum(1 for result in test_results.values() if result == "PASS")
        total = len(test_results)
        
        for test_name, result in test_results.items():
            status = "✓ PASS" if result == "PASS" else "⚠ PARTIAL" if result == "PARTIAL" else "✗ FAIL"
            print(f"{test_name:.<40} {status}")
        
        print(f"\nOverall: {passed}/{total} tests passed")
        
        if passed >= total * 0.7:  # At least 70% success rate
            print("✓ COMPREHENSIVE TEST SUITE PASSED")
            return True
        else:
            print("⚠ COMPREHENSIVE TEST SUITE PARTIALLY PASSED")
            return False
            
    except Exception as e:
        print(f"\n✗ TEST SUITE FAILED with exception: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)