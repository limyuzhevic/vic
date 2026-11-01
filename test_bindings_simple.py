#!/usr/bin/env python3
"""
Quick test script to verify the enhanced NLM Python bindings.
This script tests all the new functionality added to pynlm.
"""

import sys
import traceback

def test_import():
    """Test that pynlm can be imported."""
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
        return pynlm
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return None

def test_basic_functionality(pynlm):
    """Test basic Python bindings functionality."""
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
        
        return True
        
    except Exception as e:
        print(f"✗ Error in basic functionality test: {e}")
        traceback.print_exc()
        return False

def test_advanced_functionality(pynlm):
    """Test advanced Python bindings functionality."""
    try:
        # Test 1: Configuration from dictionary
        print("\n--- Testing Advanced Configuration ---")
        advanced_params = {
            "brain.neuron_count": 2000,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.ltp_weight": 0.025,
            "neuromod.dopamine.baseline": 0.15,
            "performance.simd.enable": True
        }
        
        config = pynlm.createConfigFromDict(advanced_params)
        print("✓ Created config from dictionary")
        
        # Test 2: Configure brain from dictionary
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        pynlm.configureBrainFromDict(brain.getConfig(), advanced_params)
        print("✓ Configured brain from dictionary")
        
        # Test 3: Inspect brain
        inspection = pynlm.inspectBrain(brain, False)
        print("✓ Brain inspection completed")
        
        # Test 4: Validate configuration
        validation = pynlm.validateBrainConfig(brain.getConfig())
        print(f"✓ Configuration validated: {validation[:100]}...")
        
        # Test 5: Optimize configuration
        optimization_hints = pynlm.optimizeBrainConfig(brain.getConfig())
        print(f"✓ Configuration optimized: {optimization_hints[:100]}...")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in advanced functionality test: {e}")
        traceback.print_exc()
        return False

def test_batch_operations(pynlm):
    """Test batch operations functionality."""
    try:
        print("\n--- Testing Batch Operations ---")
        
        # Create brain for batch operations demo
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Batch inject current
        pynlm.batchInjectCurrent(brain, pynlm.NeuronType.Excitatory, 50.0, 5)
        print("✓ Batch inject current")
        
        # Batch get neuron stats
        stats = pynlm.batchGetNeuronStats(brain, pynlm.NeuronType.Excitatory)
        if stats:
            print(f"✓ Batch stats collected - Count: {stats[0]}, Avg potential: {stats[1]:.3f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in batch operations test: {e}")
        traceback.print_exc()
        return False

def test_learning_rate_control(pynlm):
    """Test learning rate control functionality."""
    try:
        print("\n--- Testing Learning Rate Control ---")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Adjust learning rates
        result = pynlm.adjustLearningRates(brain, 1.5, 0.9)
        print(f"✓ Learning rates adjusted: {result}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in learning rate control test: {e}")
        traceback.print_exc()
        return False

def test_performance_features(pynlm):
    """Test performance features."""
    try:
        print("\n--- Testing Performance Features ---")
        
        # Create larger brain for performance testing
        large_params = {
            "brain.neuron_count": 3000,
            "performance.simd.enable": True,
            "performance.parallel.enable": True
        }
        
        large_config = pynlm.createConfigFromDict(large_params)
        brain = pynlm.createBrain(large_config)
        brain.initialize()
        
        # Run simulation
        for step in range(50):
            brain.step(step)
        
        print("✓ Large configuration simulation completed")
        
        # Get optimization hints
        hints = pynlm.optimizeBrainConfig(large_config)
        print("✓ Optimization hints generated")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in performance features test: {e}")
        traceback.print_exc()
        return False

def test_simple_simulation(pynlm):
    """Test simple simulation features."""
    try:
        print("\n--- Testing Simple Simulation ---")
        
        # Create simple simulation
        simple_brain = pynlm.createSimpleSimulation(100, 20)
        print(f"✓ Simple simulation created with {simple_brain.getTotalNeuronCount()} neurons")
        
        # Reset brain
        result = pynlm.resetBrainSimulation(simple_brain)
        print(f"✓ Brain reset: {result}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in simple simulation test: {e}")
        traceback.print_exc()
        return False

def main():
    """Main test function."""
    print("=== QUICK NLM PYTHON BINDINGS TEST ===")
    print("Testing enhanced NLM Python bindings functionality")
    print("=" * 60)
    
    # Test import
    pynlm = test_import()
    if not pynlm:
        return 1
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Advanced Configuration", test_advanced_functionality),
        ("Batch Operations", test_batch_operations),
        ("Learning Rate Control", test_learning_rate_control),
        ("Performance Features", test_performance_features),
        ("Simple Simulation", test_simple_simulation),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        try:
            if test_func(pynlm):
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
        print("\nThe enhanced NLM Python bindings are working correctly!")
        return 0

if __name__ == "__main__":
    sys.exit(main())
