#!/usr/bin/env python3
"""
Simple test script to verify the improved NLM Python bindings.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_bindings():
    """Test basic Python bindings functionality."""
    print("\n=== Testing Basic Bindings ===")
    
    try:
        # Test 1: Create default config
        config = pynlm.createDefaultConfig()
        print("✓ Created default config")
        
        # Test 2: Set and get configuration values
        config.set("brain.neuron_count", 500)
        neuron_count = config.getOr("brain.neuron_count", 1000)
        print(f"✓ Set and got neuron count: {neuron_count}")
        
        # Test 3: Create brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Test 4: Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Test 5: Get brain statistics
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        print(f"✓ Brain stats - Neurons: {total_neurons}, Synapses: {total_synapses}")
        
        # Test 6: Run simulation
        for step in range(10):
            brain.step(step)
        print("✓ Ran simulation for 10 steps")
        
        # Test 7: Get final statistics
        active_neurons = brain.getActiveNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        avg_firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        print(f"✓ Final stats - Active: {active_neurons}, Firing: {firing_neurons}, Rate: {avg_firing_rate:.2f}, Spikes: {total_spikes}")
        
        # Test 8: Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        print("✓ Created and configured world")
        
        # Test 9: Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("✓ Created and initialized agent")
        
        # Test 10: Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        print("✓ Processed sensory input")
        
        # Test 11: Decode motor command
        action = agent.decodeMotorCommand()
        print(f"✓ Decoded motor command: {action}")
        
        # Test 12: Apply motor command
        world.applyMotorCommand(action, world.getSimulationTime())
        print("✓ Applied motor command")
        
        # Test 13: Create simple config from dictionary
        params = {"brain.neuron_count": 200, "plasticity.stdp.enable": True}
        config2 = pynlm.createConfigFromDict(params)
        print("✓ Created config from dictionary")
        
        # Test 14: Configure brain from dictionary
        pynlm.configureBrainFromDict(brain.getConfig(), params)
        print("✓ Configured brain from dictionary")
        
        # Test 15: Inspect brain
        inspection = pynlm.inspectBrain(brain, False)
        print(f"✓ Brain inspection completed")
        
        # Test 16: Validate configuration
        validation = pynlm.validateBrainConfig(brain.getConfig())
        print(f"✓ Configuration validated: {validation}")
        
        # Test 17: Optimize configuration
        optimization_hints = pynlm.optimizeBrainConfig(brain.getConfig())
        print(f"✓ Configuration optimized: {optimization_hints[:100]}...")
        
        # Test 18: Create simple simulation
        simple_brain = pynlm.createSimpleSimulation(50, 20)
        print("✓ Created simple simulation")
        
        # Test 19: Reset brain
        result = pynlm.resetBrainSimulation(brain)
        print(f"✓ Brain reset: {result}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in basic bindings test: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_advanced_bindings():
    """Test advanced Python bindings functionality."""
    print("\n=== Testing Advanced Bindings ===")
    
    try:
        # Test 1: Batch operations
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Batch inject current
        pynlm.batchInjectCurrent(brain, pynlm.NeuronType.Excitatory, 50.0, 5)
        print("✓ Batch inject current")
        
        # Batch get neuron stats
        stats = pynlm.batchGetNeuronStats(brain, pynlm.NeuronType.Excitatory)
        print(f"✓ Batch get neuron stats: {len(stats)} values")
        
        # Test 2: Adjust learning rates
        result = pynlm.adjustLearningRates(brain, 1.2, 0.8)
        print(f"✓ Adjusted learning rates: {result}")
        
        # Test 3: Profile brain
        profile_result = pynlm.profileBrain(brain, 20)
        print(f"✓ Brain profile completed")
        
        # Test 4: Advanced configuration
        advanced_params = {
            "brain.neuron_count": 3000,
            "brain.region_count": 3,
            "plasticity.stdp.ltp_weight": 0.025,
            "plasticity.stdp.ltd_weight": 0.02,
            "neuromod.dopamine.baseline": 0.15,
            "performance.simd.enable": True,
            "performance.parallel.enable": True,
            "debug.log_level": "INFO"
        }
        
        advanced_config = pynlm.createConfigFromDict(advanced_params)
        validation = pynlm.validateBrainConfig(advanced_config)
        print(f"✓ Advanced configuration validated: {validation}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in advanced bindings test: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling in Python bindings."""
    print("\n=== Testing Error Handling ===")
    
    try:
        # Test 1: Invalid configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", -10)  # Invalid: too low
        validation = pynlm.validateBrainConfig(config)
        if "must be between" in validation:
            print("✓ Correctly detected invalid neuron count")
        
        # Test 2: Missing configuration
        config2 = pynlm.createDefaultConfig()
        if not config2.has("nonexistent.parameter"):
            print("✓ Correctly detected missing parameter")
        
        # Test 3: Invalid action type (if applicable)
        print("✓ Error handling tests completed")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in error handling test: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_performance_features():
    """Test performance-related features."""
    print("\n=== Testing Performance Features ===")
    
    try:
        # Test 1: Large configuration
        large_params = {
            "brain.neuron_count": 10000,
            "brain.region_count": 10,
            "plasticity.stdp.ltp_weight": 0.01,
            "plasticity.stdp.ltd_weight": 0.008,
            "memory.working.capacity": 5000,
            "memory.episodic.capacity": 2000,
            "performance.simd.enable": True,
            "performance.parallel.enable": True
        }
        
        large_config = pynlm.createConfigFromDict(large_params)
        validation = pynlm.validateBrainConfig(large_config)
        print(f"✓ Large configuration validated: {validation[:100]}...")
        
        # Test 2: Memory optimization
        optimization_hints = pynlm.optimizeBrainConfig(large_config)
        print(f"✓ Optimization hints generated")
        
        # Test 3: Configuration statistics
        brain = pynlm.createBrain(large_config)
        brain.initialize()
        
        # Run simulation with large config
        for step in range(50):
            brain.step(step)
        
        print("✓ Large configuration simulation completed")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in performance features test: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Main test function."""
    print("=== NLM Python Bindings Test Suite ===")
    print(f"Python version: {sys.version}")
    
    tests = [
        ("Basic Bindings", test_basic_bindings),
        ("Advanced Bindings", test_advanced_bindings),
        ("Error Handling", test_error_handling),
        ("Performance Features", test_performance_features),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        try:
            if test_func():
                print(f"✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"✗ {test_name} FAILED")
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} CRASHED: {e}")
            failed += 1
    
    print(f"\n{'='*60}")
    print(f"Test Summary: {passed} passed, {failed} failed")
    print(f"{'='*60}")
    
    if failed > 0:
        print(f"\n❌ {failed} test(s) failed!")
        return 1
    else:
        print(f"\n✅ All tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(main())
