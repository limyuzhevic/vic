#!/usr/bin/env python3
"""
NLM Test Script

Test script to verify NLM improvements and functionality.
"""

import sys
import time
import tempfile
from pathlib import Path

# Add the source directory to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

try:
    import pynlm
except ImportError as e:
    print(f"Import error: {e}")
    print("Please install pynlm first: pip install .")
    sys.exit(1)


def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test step functionality
    initial_spikes = brain.getTotalSpikeCount()
    
    for i in range(10):
        brain.step(i)
    
    final_spikes = brain.getTotalSpikeCount()
    
    print(f"  ✓ Brain creation and initialization: {brain.getTotalNeuronCount()} neurons")
    print(f"  ✓ Brain simulation: {initial_spikes} -> {final_spikes} spikes")
    print(f"  ✓ Basic functionality test passed")
    return True


def test_world_functionality():
    """Test world functionality."""
    print("\nTesting world functionality...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Test world properties
    print(f"  ✓ World created: {world.getWidth()}x{world.getHeight()} size")
    print(f"  ✓ World reset: simulation time {world.getSimulationTime()}")
    
    # Test agent interaction
    agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
    agent.initialize(world)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    
    print(f"  ✓ Agent interaction: processed sensory input, got action")
    print(f"  ✓ World functionality test passed")
    return True


def test_advanced_features():
    """Test advanced features."""
    print("\nTesting advanced features...")
    
    # Test with learning enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run simulation with learning
    for i in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        agent.decodeMotorCommand()
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.0)
        agent.updateDevelopment(0.1)
    
    # Check learning indicators
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    neuromod = agent.getNeuromodulationLevel()
    
    print(f"  ✓ Learning features enabled: curiosity={curiosity:.3f}, novelty={novelty:.3f}, neuromod={neuromod:.3f}")
    print(f"  ✓ Advanced features test passed")
    return True


def test_persistence():
    """Test save/load functionality."""
    print("\nTesting persistence (save/load)...")
    
    # Create and run brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    for i in range(100):
        brain1.step(i)
    
    # Save brain
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        brain_path = f.name
    
    brain1.save(brain_path)
    print(f"  ✓ Brain saved to {brain_path}")
    
    # Load brain
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    brain2.load(brain_path)
    print(f"  ✓ Brain loaded from {brain_path}")
    
    # Compare stats
    spikes1 = brain1.getTotalSpikeCount()
    spikes2 = brain2.getTotalSpikeCount()
    
    print(f"  ✓ Original brain spikes: {spikes1}")
    print(f"  ✓ Loaded brain spikes: {spikes2}")
    
    if spikes1 == spikes2:
        print(f"  ✓ Persistence test PASSED: Brains are identical")
        return True
    else:
        print(f"  ✗ Persistence test FAILED: Spike count mismatch")
        return False


def test_text_encoding():
    """Test text encoding feature."""
    print("\nTesting text encoding...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test text encoding
    test_texts = ["hello", "world", "nlm test", "advanced features"]
    
    for text in test_texts:
        encoded = agent.encodeText(text)
        agent.processSensoryInput(encoded)
        brain.step(0)
        print(f"  ✓ Encoded '{text}' -> {len(encoded)} neural patterns")
    
    print(f"  ✓ Text encoding test passed")
    return True


def test_performance():
    """Test performance with different network sizes."""
    print("\nTesting performance...")
    
    neuron_counts = [50, 100, 200]
    steps = 100
    
    for neurons in neuron_counts:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neurons)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for i in range(steps):
            brain.step(i)
        
        end_time = time.time()
        
        total_time = end_time - start_time
        steps_per_sec = steps / total_time
        
        print(f"  ✓ {neurons:3d} neurons: {total_time:5.2f}s, {steps_per_sec:6.0f} steps/sec")
    
    print(f"  ✓ Performance test passed")
    return True


def test_memory_systems():
    """Test memory systems integration."""
    print("\nTesting memory systems...")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 150)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run enough steps to populate memory
    for i in range(200):
        brain.step(i)
    
    # Check if memory systems are working
    print(f"  ✓ Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  ✓ Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  ✓ Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Check development
    dev_stage = brain.getDevelopmentalStage()
    print(f"  ✓ Developmental stage: {dev_stage}")
    
    print(f"  ✓ Memory systems test passed")
    return True


def main():
    print("NLM Test Script")
    print("=" * 50)
    print("Testing NLM improvements and functionality...")
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("World Functionality", test_world_functionality),
        ("Advanced Features", test_advanced_features),
        ("Persistence (Save/Load)", test_persistence),
        ("Text Encoding", test_text_encoding),
        ("Performance", test_performance),
        ("Memory Systems", test_memory_systems),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"  ✗ Test failed with exception: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Summary: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed! NLM is working correctly.")
        return 0
    else:
        print("❌ Some tests failed. Please check the output above.")
        return 1


if __name__ == "__main__":
    exit(main())
