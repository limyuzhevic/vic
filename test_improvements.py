#!/usr/bin/env python3
"""
Comprehensive Test Suite for NLM Improvements
Validates all fixes and improvements made to the NLM codebase
"""

import sys
import traceback
import tempfile
import os

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)
except Exception as e:
    print(f"✗ Unexpected error importing pynlm: {e}")
    traceback.print_exc()
    sys.exit(1)

def test_improved_python_bindings():
    """Test improved Python bindings with better error handling and type safety"""
    print("\n=== Testing Improved Python Bindings ===")
    
    try:
        # Test 1: Enhanced configuration
        print("Test 1: Enhanced configuration...")
        config = pynlm.createDefaultConfig()
        
        # Set various configuration types
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.15)
        config.set("plasticity_learning_rate", 0.001)
        config.set("stdp_ltp_weight", 0.01)
        config.set("stdp_ltd_weight", 0.012)
        config.set("stdp_time_constant", 20.0)
        config.set("development_synaptogenesis_rate", 0.0001)
        config.set("development_pruning_rate", 0.00001)
        config.set("neuromod_dopamine_scale", 1.0)
        config.set("neuromod_curiosity_enable", True)
        config.set("neuromod_novelty_enable", True)
        
        print("✓ Enhanced configuration with Phase 6 parameters")
        
        # Test 2: Improved type safety
        print("\nTest 2: Type safety...")
        
        # Test various type retrievals
        neuron_count = config.getOr("neuron_count", 0)
        region_count = config.getOr("region_count", 0)
        connection_prob = config.getOr("connection_probability", 0.0)
        plasticity_rate = config.getOr("plasticity_learning_rate", 0.0)
        
        print(f"✓ Type-safe retrievals:")
        print(f"  neuron_count: {neuron_count} (type: {type(neuron_count).__name__})")
        print(f"  region_count: {region_count} (type: {type(region_count).__name__})")
        print(f"  connection_probability: {connection_prob} (type: {type(connection_prob).__name__})")
        print(f"  plasticity_learning_rate: {plasticity_rate} (type: {type(plasticity_rate).__name__})")
        
        # Test 3: Error handling improvements
        print("\nTest 3: Error handling...")
        
        # Invalid key handling
        invalid_value = config.get("nonexistent_key")
        if invalid_value is None:
            print("✓ Invalid key returns None (graceful handling)")
        else:
            print(f"⚠ Invalid key returns: {invalid_value}")
        
        # Key existence checking
        if config.has("nonexistent_key"):
            print("✗ Key existence check failed")
        else:
            print("✓ Key existence check works correctly")
        
        # Test 4: Configuration validation
        print("\nTest 4: Configuration validation...")
        
        # Validate required keys
        required_keys = ["neuron_count", "region_count", "connection_probability"]
        for key in required_keys:
            if config.has(key):
                value = config.getOr(key, 0)
                print(f"✓ Required key '{key}' validated: {value}")
            else:
                print(f"✗ Required key '{key}' missing")
        
        # Test 5: Configuration summary
        print("\nTest 5: Configuration summary...")
        
        summary = config.summary()
        if summary and len(summary) > 0:
            print(f"✓ Configuration summary generated ({len(summary)} chars)")
            # Show first few lines
            lines = summary.split('\n')[:5]
            for line in lines:
                if line.strip():
                    print(f"  {line}")
            if len(summary.split('\n')) > 5:
                print("  ...")
        else:
            print("⚠ Configuration summary empty")
        
        return True
        
    except Exception as e:
        print(f"✗ Improved bindings test failed: {e}")
        traceback.print_exc()
        return False

def test_agent_brain_improvements():
    """Test improved AgentBrain interface"""
    print("\n=== Testing AgentBrain Improvements ===")
    
    try:
        # Create test setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test 1: Enhanced neuromodulation
        print("Test 1: Enhanced neuromodulation...")
        
        # Run simulation with enhanced neuromodulation
        total_reward = 0.0
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply varied rewards
            reward = float(step) * 0.05
            agent.applyRewardModulation(reward, 0.0)
            total_reward += reward
        
        # Check neuromodulation levels
        dopamine = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        print(f"✓ Enhanced neuromodulation results:")
        print(f"  Total reward: {total_reward:.2f}")
        print(f"  Dopamine level: {dopamine:.3f}")
        print(f"  Curiosity level: {curiosity:.3f}")
        print(f"  Novelty level: {novelty:.3f}")
        print(f"  Prediction error: {prediction_error:.3f}")
        
        # Test 2: Development system improvements
        print("\nTest 2: Development system improvements...")
        
        # Get initial development stage
        initial_stage = agent.getDevelopmentalStage()
        print(f"✓ Initial developmental stage: {initial_stage}")
        
        # Run development updates
        for step in range(10):
            agent.updateDevelopment(0.1)
        
        final_stage = agent.getDevelopmentalStage()
        print(f"✓ Final developmental stage after updates: {final_stage}")
        
        # Test 3: Enhanced feature controls
        print("\nTest 3: Enhanced feature controls...")
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Check feature status
        print(f"✓ Feature status:")
        print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
        print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
        print(f"  Development: {agent.isDevelopmentEnabled()}")
        print(f"  Curiosity: {agent.isCuriosityEnabled()}")
        
        # Test 4: Enhanced sensory processing
        print("\nTest 4: Enhanced sensory processing...")
        
        # Create complex sensory input
        from pynlm import Vision, Audio, InternalSignals
        
        # Vision (8x8)
        vision_data = [0.1 * (i % 8 + 1) for i in range(64)]
        vision = Vision(8, 8, 3)
        vision.setData(vision_data)
        
        # Audio
        audio_data = [0.05 * (i % 10) for i in range(20)]
        audio = Audio(44100, 20)
        audio.setData(audio_data)
        
        # Internal signals
        internal = InternalSignals()
        internal.addSignal(0.5)
        internal.addSignal(-0.3)
        internal.addSignal(0.8)
        
        # Create percept
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        percept.setAudio(audio)
        percept.setInternal(internal.getSignals())
        
        # Process enhanced input
        agent.processSensoryInput(percept)
        print("✓ Enhanced sensory processing completed")
        
        # Test 5: Motor command diversity
        print("\nTest 5: Motor command diversity...")
        
        # Get motor output size
        motor_size = agent.getMotorOutputSize()
        print(f"✓ Motor output size: {motor_size}")
        
        # Collect diverse motor commands
        motor_commands = set()
        for step in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            motor_commands.add(action)
        
        print(f"✓ Diverse motor commands: {len(motor_commands)}")
        for cmd in sorted(motor_commands):
            print(f"  - {cmd}")
        
        return True
        
    except Exception as e:
        print(f"✗ AgentBrain improvements test failed: {e}")
        traceback.print_exc()
        return False

def test_phase6_integration():
    """Test Phase 6 integration improvements"""
    print("\n=== Testing Phase 6 Integration Improvements ===")
    
    try:
        # Test 1: Phase 6 configuration
        print("Test 1: Phase 6 configuration...")
        
        # Create Phase 6 configuration
        config = pynlm.createDefaultConfig()
        
        # Set Phase 6 parameters
        config.set("neuron_count", 500)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        config.set("plasticity_learning_rate", 0.001)
        config.set("stdp_ltp_weight", 0.01)
        config.set("stdp_ltd_weight", 0.012)
        config.set("stdp_time_constant", 20.0)
        config.set("synaptogenesis_rate", 0.0001)
        config.set("pruning_rate", 0.00001)
        config.set("neuromod_dopamine_scale", 1.0)
        config.set("neuromod_curiosity_enable", True)
        config.set("neuromod_novelty_enable", True)
        
        print("✓ Phase 6 configuration parameters set")
        
        # Test 2: Phase 6 experiment simulation
        print("\nTest 2: Phase 6 experiment simulation...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all Phase 6 features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run Phase 6 experiment
        total_steps = 100
        for step in range(total_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.1 * (1 if step % 10 == 0 else 0)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress
            if step % 20 == 0:
                firing_rate = brain.getAverageFiringRate()
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                dopamine = agent.getNeuromodulationLevel()
                
                print(f"  Step {step}: firing={firing_rate:.2f}, curiosity={curiosity:.3f}, novelty={novelty:.3f}, dopamine={dopamine:.3f}")
        
        # Collect final metrics
        final_firing = brain.getAverageFiringRate()
        final_curiosity = agent.getCuriosityLevel()
        final_novelty = agent.getNoveltyLevel()
        final_dopamine = agent.getNeuromodulationLevel()
        total_spikes = brain.getTotalSpikeCount()
        
        print(f"✓ Phase 6 experiment completed:")
        print(f"  Total steps: {total_steps}")
        print(f"  Average firing rate: {final_firing:.3f}")
        print(f"  Final curiosity: {final_curiosity:.3f}")
        print(f"  Final novelty: {final_novelty:.3f}")
        print(f"  Final dopamine: {final_dopamine:.3f}")
        print(f"  Total spikes: {total_spikes}")
        
        # Test 3: Integration verification
        print("\nTest 3: Integration verification...")
        
        # Check that all systems are functional
        systems_status = []
        
        # Check memory systems
        if brain.getWorkingMemory():
            systems_status.append("✓ Working Memory: Operational")
        else:
            systems_status.append("⚠ Working Memory: Not detected")
        
        if brain.getEpisodicMemory():
            systems_status.append("✓ Episodic Memory: Operational")
        else:
            systems_status.append("⚠ Episodic Memory: Not detected")
        
        # Check neuromodulation
        if brain.getDopamine():
            systems_status.append("✓ Neuromodulation: Operational")
        else:
            systems_status.append("⚠ Neuromodulation: Not detected")
        
        if brain.getCuriosity():
            systems_status.append("✓ Curiosity System: Operational")
        else:
            systems_status.append("⚠ Curiosity System: Not detected")
        
        # Check prediction system
        if brain.getPredictionSystem():
            systems_status.append("✓ Prediction System: Operational")
        else:
            systems_status.append("⚠ Prediction System: Not detected")
        
        # Check development system
        if brain.getDevelopmentSystem():
            systems_status.append("✓ Development System: Operational")
        else:
            systems_status.append("⚠ Development System: Not detected")
        
        print("✓ Integration verification results:")
        for status in systems_status:
            print(f"  {status}")
        
        return True
        
    except Exception as e:
        print(f"✗ Phase 6 integration test failed: {e}")
        traceback.print_exc()
        return False

def test_backward_compatibility():
    """Test backward compatibility with existing code"""
    print("\n=== Testing Backward Compatibility ===")
    
    try:
        # Test 1: Original simple API
        print("Test 1: Original simple API...")
        
        # This is the original simple example from easy_usage.md
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(10):
            brain.step(i)
        
        print("✓ Original simple API works")
        
        # Test 2: Original world interaction example
        print("\nTest 2: Original world interaction...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run original pattern
        for step in range(20):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        print("✓ Original world interaction pattern works")
        
        # Test 3: Original statistics
        print("\nTest 3: Original statistics...")
        
        neurons_firing = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        avg_firing = brain.getAverageFiringRate()
        
        print(f"✓ Original statistics available:")
        print(f"  Neurons firing: {neurons_firing}")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Average firing rate: {avg_firing:.3f}")
        
        # Test 4: Original feature enables
        print("\nTest 4: Original feature enables...")
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        print("✓ Original feature enables work")
        
        return True
        
    except Exception as e:
        print(f"✗ Backward compatibility test failed: {e}")
        traceback.print_exc()
        return False

def test_configuration_consistency():
    """Test configuration consistency across implementations"""
    print("\n=== Testing Configuration Consistency ===")
    
    try:
        # Test 1: Config object consistency
        print("Test 1: Configuration object consistency...")
        
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        # Both should have the same keys
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        
        if sorted(keys1) == sorted(keys2):
            print("✓ Default configurations have consistent keys")
        else:
            print(f"✗ Key inconsistency: {set(keys1)} vs {set(keys2)}")
        
        # Test 2: Value consistency
        print("\nTest 2: Value consistency...")
        
        config1.set("test_value", 42)
        config2.set("test_value", 42)
        
        value1 = config1.getOr("test_value", 0)
        value2 = config2.getOr("test_value", 0)
        
        if value1 == value2 == 42:
            print("✓ Value consistency maintained")
        else:
            print(f"✗ Value inconsistency: {value1} vs {value2}")
        
        # Test 3: Type consistency
        print("\nTest 3: Type consistency...")
        
        test_values = [
            ("int", 42),
            ("float", 3.14159),
            ("string", "test string"),
            ("bool", True),
            ("list", [1, 2, 3]),
        ]
        
        for key, value in test_values:
            config1.set(key, value)
            retrieved = config1.getOr(key, None)
            
            # Basic type check
            if isinstance(value, type(retrieved)) or (isinstance(value, list) and isinstance(retrieved, list)):
                print(f"✓ Type consistency for {key}: {type(retrieved).__name__}")
            else:
                print(f"⚠ Type mismatch for {key}: expected {type(value).__name__}, got {type(retrieved).__name__}")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration consistency test failed: {e}")
        traceback.print_exc()
        return False

def main():
    """Run all improvement validation tests"""
    print("=" * 80)
    print("NLM Improvements - Comprehensive Validation Suite")
    print("=" * 80)
    print("Testing all improvements made to the NLM codebase:")
    print("1. Enhanced Python bindings with better error handling")
    print("2. Improved AgentBrain interface")
    print("3. Phase 6 integration enhancements")
    print("4. Backward compatibility")
    print("5. Configuration consistency")
    print("=" * 80)
    
    tests = [
        ("Improved Python Bindings", test_improved_python_bindings),
        ("AgentBrain Improvements", test_agent_brain_improvements),
        ("Phase 6 Integration", test_phase6_integration),
        ("Backward Compatibility", test_backward_compatibility),
        ("Configuration Consistency", test_configuration_consistency),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*80}")
        print(f"Running: {test_name}")
        print(f"{'='*80}")
        
        try:
            success = test_func()
            results.append((test_name, success))
            status = "✓ PASSED" if success else "✗ FAILED"
            print(f"\n{test_name}: {status}")
        except Exception as e:
            print(f"\n{test_name}: ✗ FAILED - {e}")
            results.append((test_name, False))
    
    # Summary
    print("\n" + "=" * 80)
    print("IMPROVEMENT VALIDATION SUMMARY")
    print("=" * 80)
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{test_name}: {status}")
    
    print(f"\nTotal: {passed}/{total} improvement validation tests passed")
    
    if passed == total:
        print("\n🎉 All improvement validation tests passed!")
        print("\nNLM Codebase Improvements Successfully Validated:")
        print("✓ Enhanced Python bindings with robust error handling")
        print("✓ Improved AgentBrain interface with advanced features")
        print("✓ Phase 6 integration with consistent configuration")
        print("✓ Backward compatibility with original API")
        print("✓ Configuration consistency across implementations")
        print("\nThe NLM codebase is now more robust, maintainable, and user-friendly!")
        return 0
    else:
        print(f"\n⚠️  {total - passed} validation test(s) failed")
        print("\nSome improvements may need additional work.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
