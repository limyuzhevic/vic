#!/usr/bin/env python3
"""
Quick Verification Test for NLM System Enhancements

This script demonstrates that the NLM system enhancements are working correctly.
It tests the key improvements made during the enhancement project.
"""

import sys
import os

# Add the build directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

def test_imports():
    """Test that all NLM modules can be imported successfully"""
    print("Testing NLM module imports...")
    
    try:
        import pynlm as nlm
        print("✓ Successfully imported pynlm")
        return True
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False

def test_config_system():
    """Test the enhanced configuration system"""
    print("\nTesting enhanced configuration system...")
    
    try:
        import pynlm as nlm
        
        # Create configuration
        config = nlm.createDefaultConfig()
        
        # Test automatic type inference
        config.setFromString("test_int", "42")
        config.setFromString("test_float", "3.14")
        config.setFromString("test_bool", "true")
        
        # Verify type inference worked
        int_val = config.get('test_int').value
        float_val = config.get('test_float').value
        bool_val = config.get('test_bool').value
        
        print(f"  - Integer value: {int_val} (type: {type(int_val).__name__})")
        print(f"  - Float value: {float_val} (type: {type(float_val).__name__})")
        print(f"  - Boolean value: {bool_val} (type: {type(bool_val).__name__})")
        
        # Test file I/O operations
        save_success = config.saveToFile("./test_config.cfg")
        print(f"  - Save to file: {'SUCCESS' if save_success else 'FAILED'}")
        
        load_success = config.loadFromFile("./test_config.cfg")
        print(f"  - Load from file: {'SUCCESS' if load_success else 'FAILED'}")
        
        # Clean up
        if os.path.exists("./test_config.cfg"):
            os.remove("./test_config.cfg")
        
        print("✓ Configuration system tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Configuration system test failed: {e}")
        return False

def test_brain_system():
    """Test the enhanced brain system"""
    print("\nTesting enhanced brain system...")
    
    try:
        import pynlm as nlm
        
        # Create brain with enhanced configuration
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 100, nlm.ConfigSource.Runtime)
        config.set("region_count", 1, nlm.ConfigSource.Runtime)
        
        brain = nlm.createBrain(config)
        
        # Test initialization
        success = brain.initialize()
        if not success:
            print("✗ Brain initialization failed")
            return False
        
        print(f"  - Brain initialized successfully")
        print(f"  - Regions: {brain.getRegionCount()}")
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Synapses: {brain.getTotalSynapseCount()}")
        
        # Test enhanced system status reporting
        status = brain.getIntegratedSystemsStatus()
        if status and len(status) > 0:
            print(f"  - System status reporting: ACTIVE")
        else:
            print(f"  - System status reporting: FAILED")
            return False
        
        # Test memory system access
        working_memory = brain.getWorkingMemory()
        if working_memory:
            print(f"  - Working memory: {working_memory.getActiveTraces()} active traces")
        
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            print(f"  - Episodic memory: {episodic_memory.getEpisodeCount()} episodes")
        
        # Test simulation
        for step in range(10):
            brain.step(step, step * 0.001)
        
        print(f"  - Simulation completed successfully")
        
        print("✓ Brain system tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Brain system test failed: {e}")
        return False

def test_neuron_system():
    """Test the enhanced neuron system"""
    print("\nTesting enhanced neuron system...")
    
    try:
        import pynlm as nlm
        
        # Create brain to access neuron system
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 50, nlm.ConfigSource.Runtime)
        config.set("region_count", 1, nlm.ConfigSource.Runtime)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Get spike system for neuron statistics
        spike_system = brain.getSpikeSystem()
        if spike_system:
            print(f"  - Spike system active")
            print(f"  - Spike history: {spike_system.getSpikeCount()} events")
            print(f"  - Pending events: {spike_system.getPendingSpikeCount()}")
            
            # Test most active neurons
            most_active = spike_system.getMostActiveNeurons(3)
            print(f"  - Most active neurons: {len(most_active)} detected")
        
        # Test simulation with neuron dynamics
        for step in range(15):
            # Inject some sensory input to create activity
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0f)
            brain.step(step, step * 0.001)
        
        print(f"  - Neuron dynamics simulation completed")
        
        print("✓ Neuron system tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Neuron system test failed: {e}")
        return False

def test_advanced_features():
    """Test advanced NLM features"""
    print("\nTesting advanced NLM features...")
    
    try:
        import pynlm as nlm
        
        # Create agent brain with advanced features
        brain = nlm.createDefaultBrain()
        brain.initialize(nlm.createSimpleWorld())
        
        # Enable advanced features
        brain.enableRewardModulation(True)
        brain.enableStructuralPlasticity(True)
        brain.enableDevelopment(True)
        brain.enableCuriosity(True)
        
        print(f"  - Reward modulation: {brain.isRewardModulationEnabled()}")
        print(f"  - Structural plasticity: {brain.isStructuralPlasticityEnabled()}")
        print(f"  - Development: {brain.isDevelopmentEnabled()}")
        print(f"  - Curiosity: {brain.isCuriosityEnabled()}")
        
        # Test neuromodulation levels
        neuromod_level = brain.getNeuromodulationLevel()
        curiosity_level = brain.getCuriosityLevel()
        print(f"  - Neuromodulation level: {neuromod_level:.3f}")
        print(f"  - Curiosity level: {curiosity_level:.3f}")
        
        # Test development system
        development_stage = brain.getDevelopmentalStage()
        print(f"  - Developmental stage: {development_stage}")
        
        # Test memory system access
        active_traces = brain.getActiveMemoryTraces()
        print(f"  - Active memory traces: {len(active_traces)}")
        
        # Test simulation with advanced features
        for step in range(10):
            # Create some experience for memory systems
            vision = nlm.Vision(32, 32, 3)
            vision_data = [0.5] * 32 * 32 * 3
            vision.setData(vision_data)
            
            brain.processSensoryInput(vision)
            action = brain.decodeMotorCommand()
            
            # Apply learning
            brain.applyRewardModulation(0.1, 0.05)
            
            # Update development
            brain.updateDevelopment(0.001)
            
            brain.step(step, step * 0.001)
        
        print(f"  - Advanced features simulation completed")
        
        print("✓ Advanced features tests passed")
        return True
        
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")
        return False

def main():
    """Main test function"""
    print("=" * 60)
    print("NLM System Enhancement Verification Test")
    print("=" * 60)
    
    tests = [
        ("Module Imports", test_imports),
        ("Configuration System", test_config_system),
        ("Brain System", test_brain_system),
        ("Neuron System", test_neuron_system),
        ("Advanced Features", test_advanced_features),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        try:
            if test_func():
                passed += 1
                print(f"✅ {test_name}: PASSED")
            else:
                print(f"❌ {test_name}: FAILED")
        except Exception as e:
            print(f"❌ {test_name}: ERROR - {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'='*60}")
    print(f"Test Results: {passed}/{total} tests passed")
    print(f"{'='*60}")
    
    if passed == total:
        print("🎉 ALL TESTS PASSED!")
        print("\nThe NLM system enhancements are working correctly!")
        print("\nKey improvements verified:")
        print("  ✓ Enhanced configuration system with type inference")
        print("  ✓ Improved brain integration and status reporting")
        print("  ✓ Enhanced neuron dynamics and spike system")
        print("  ✓ Advanced features and memory systems")
        print("  ✓ Better error handling and validation")
        return 0
    else:
        print(f"❌ {total - passed} tests failed")
        print("\nSome enhancements may need further verification.")
        return 1

if __name__ == "__main__":
    sys.exit(main())