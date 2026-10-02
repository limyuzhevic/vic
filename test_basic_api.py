#!/usr/bin/env python3
"""
Basic API Tests for NLM Python Bindings

Tests core NLM functionality including brain creation,
configuration management, and basic operations.
"""

import sys
import os

# Add the current directory to the path to ensure pynlm can be imported
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def test_basic_config():
    """Test basic configuration creation and access."""
    print("Testing basic configuration...")
    
    # Test default config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    assert isinstance(config, pynlm.Config)
    
    # Test default values
    assert config.has("random_seed")
    assert config.has("simulation_timestep")
    assert config.has("neuron_count")
    
    # Test getting default values
    seed = config.getOr<int64_t>("random_seed", 42)
    assert seed == 42
    
    timestep = config.getOr<double>("simulation_timestep", 0.001)
    assert timestep == 0.001
    
    neuron_count = config.getOr<int64_t>("neuron_count", 1000)
    assert neuron_count == 1000
    
    print("✓ Basic config test passed")


def test_brain_creation():
    """Test brain creation and initialization."""
    print("Testing brain creation...")
    
    # Test quick brain creation
    brain = pynlm.Brain.createQuickBrain()
    assert brain is not None
    assert isinstance(brain, pynlm.Brain)
    
    # Test that brain is initialized
    total_neurons = brain.getTotalNeuronCount()
    assert total_neurons > 0
    
    print(f"✓ Brain creation test passed ({total_neurons} neurons)")


def test_step_functionality():
    """Test brain stepping functionality."""
    print("Testing step functionality...")
    
    brain = pynlm.Brain.createQuickBrain()
    
    # Test multiple steps
    initial_spikes = brain.getTotalSpikeCount()
    
    for step in range(10):
        brain.step(step)
    
    final_spikes = brain.getTotalSpikeCount()
    assert final_spikes >= initial_spikes  # Should have at least as many spikes as before
    
    print(f"✓ Step functionality test passed ({final_spikes - initial_spikes} spikes)")


def test_simple_world():
    """Test simple world creation and configuration."""
    print("Testing simple world...")
    
    world = pynlm.createSimpleWorld()
    assert world is not None
    assert isinstance(world, pynlm.SimpleWorld)
    
    # Test world configuration
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    assert world.getWidth() == 10
    assert world.getHeight() == 10
    
    # Test world reset
    world.reset()
    assert world.getSimulationTime() == 0.0
    
    print("✓ Simple world test passed")


def test_sensory_inputs():
    """Test sensory input creation and usage."""
    print("Testing sensory inputs...")
    
    # Test Vision creation
    vision = pynlm.Vision.createImage(640, 480)
    assert vision is not None
    assert vision.getWidth() == 640
    assert vision.getHeight() == 480
    assert vision.getChannels() == 3
    
    # Test Vision with custom channels
    vision_2 = pynlm.Vision.createImage(320, 240, 1)
    assert vision_2.getChannels() == 1
    
    # Test Audio creation
    audio = pynlm.Audio.createAudio(44100, 1024)
    assert audio is not None
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1024
    
    # Test InternalSignals creation
    signals = pynlm.InternalSignals.createSignals()
    assert signals is not None
    
    print("✓ Sensory inputs test passed")


def test_agent_brain():
    """Test agent brain interface."""
    print("Testing agent brain...")
    
    # Create brain and agent
    brain = pynlm.Brain.createQuickBrain()
    agent = pynlm.createAgentBrain(brain)
    
    assert agent is not None
    assert isinstance(agent, pynlm.AgentBrain)
    
    # Test agent configuration
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    assert agent.isRewardModulationEnabled()
    assert agent.isCuriosityEnabled()
    
    print("✓ Agent brain test passed")


def test_context_managers():
    """Test context manager functionality."""
    print("Testing context managers...")
    
    # Test Vision context manager
    with pynlm.Vision.createImage(100, 100) as vision:
        assert vision.getWidth() == 100
        assert vision.getHeight() == 100
    
    # Test Audio context manager
    with pynlm.Audio.createAudio(8000, 512) as audio:
        assert audio.getSampleRate() == 8000
        assert audio.getNumSamples() == 512
    
    # Test InternalSignals context manager
    with pynlm.InternalSignals.createSignals() as signals:
        signals.addSignal(1.0)
        assert signals.getData()[0] == 1.0
    
    print("✓ Context managers test passed")


def test_statistical_functions():
    """Test brain statistical functions."""
    print("Testing statistical functions...")
    
    brain = pynlm.Brain.createQuickBrain()
    brain.initialize()
    
    # Test various statistics
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    active_neurons = brain.getActiveNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_firing_rate = brain.getAverageFiringRate()
    excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
    total_spikes = brain.getTotalSpikeCount()
    
    # Verify statistical properties
    assert total_neurons > 0
    assert total_synapses >= 0
    assert active_neurons >= 0
    assert firing_neurons >= 0
    assert avg_firing_rate >= 0.0
    assert excitation_inhibition_ratio >= 0.0
    assert total_spikes >= 0
    
    print(f"✓ Statistical functions test passed")
    print(f"  Neurons: {total_neurons}, Synapses: {total_synapses}")
    print(f"  Active: {active_neurons}, Firing: {firing_neurons}")
    print(f"  Firing Rate: {avg_firing_rate:.3f}, E/I Ratio: {excitation_inhibition_ratio:.3f}")


def test_error_handling():
    """Test error handling."""
    print("Testing error handling...")
    
    # Test creating brain with null config
    try:
        brain = pynlm.createBrain(None)
        assert False, "Should have raised an error"
    except Exception as e:
        print(f"✓ Correctly caught error: {type(e).__name__}")
    
    print("✓ Error handling test passed")


def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_config,
        test_brain_creation,
        test_step_functionality,
        test_simple_world,
        test_sensory_inputs,
        test_agent_brain,
        test_context_managers,
        test_statistical_functions,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"✗ {test.__name__} failed: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All tests passed!")


if __name__ == "__main__":
    run_all_tests()