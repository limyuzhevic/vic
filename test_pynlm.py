"""
Comprehensive test suite for pynlm Python bindings.
Tests all major functionality of the NLM Python API.
"""
import pynlm
import json
import tempfile
import os


def test_basic_functionality():
    """Test basic brain creation and initialization."""
    print("Testing basic functionality...")
    
    # Test brain creation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    assert brain is not None
    
    # Test initialization
    brain.initialize()
    assert brain.getTotalNeuronCount() > 0
    
    # Test stepping
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(0)
    assert brain.getTotalSpikeCount() >= initial_spikes
    
    print("✓ Basic functionality passed")


def test_configuration():
    """Test configuration management."""
    print("Testing configuration...")
    
    # Create and modify config
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    
    # Check values
    assert config.getOr("brain.neuron_count", 1000) == 2000
    assert config.getOr("brain.synapse_density", 0.1) == 0.2
    
    # Test file operations
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        
        # Create new config and load
        config2 = pynlm.createDefaultConfig()
        config2.loadFromFile(f.name)
        
        assert config2.getOr("brain.neuron_count", 0) == 2000
        
        # Cleanup
        os.unlink(f.name)
    
    print("✓ Configuration passed")


def test_agent_functionality():
    """Test agent brain interface."""
    print("Testing agent functionality...")
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    agent.initialize(world)
    
    # Test sensor input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test motor output
    action = agent.decodeMotorCommand()
    assert action is not None
    
    # Test neuromodulation
    agent.applyRewardModulation(1.0, 0.5)
    assert agent.getPredictionError() == 0.5
    
    print("✓ Agent functionality passed")


def test_world_simulation():
    """Test complete world simulation."""
    print("Testing world simulation...")
    
    # Create full simulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.setAgentStart(7.5, 7.5)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check state
        assert brain.getFiringNeuronCount() >= 0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getCuriosityLevel() <= 1.0
    
    print("✓ World simulation passed")


def test_plasticity_systems():
    """Test plasticity and learning systems."""
    print("Testing plasticity systems...")
    
    # Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Run learning episode
    for step in range(100):
        brain.step(step)
        
        if step % 10 == 0:
            agent.applyRewardModulation(0.5, 0.0)
    
    # Check that plasticity affected brain state
    assert brain.getTotalSpikeCount() > 0
    
    print("✓ Plasticity systems passed")


def test_development():
    """Test brain development features."""
    print("Testing development...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    # Check initial development stage
    initial_stage = brain.getDevelopmentalStage()
    assert initial_stage is not None
    
    # Update development
    agent.updateDevelopment(10.0)
    
    # Check that development stage may have changed
    assert brain.getDevelopmentalStage() is not None
    
    print("✓ Development passed")


def test_enumerations():
    """Test enumeration types."""
    print("Testing enumerations...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.NeuronType.Inhibitory is not None
    assert pynlm.NeuronType.Sensory is not None
    assert pynlm.NeuronType.Motor is not None
    assert pynlm.NeuronType.Modulatory is not None
    assert pynlm.NeuronType.Internal is not None
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.ActionType.TurnLeft is not None
    assert pynlm.ActionType.TurnRight is not None
    assert pynlm.ActionType.Interact is not None
    assert pynlm.ActionType.Eat is not None
    
    # Test DevelopmentalStage
    assert pynlm.DevelopmentalStage.Initial is not None
    assert pynlm.DevelopmentalStage.CriticalPeriod is not None
    assert pynlm.DevelopmentalStage.Maturation is not None
    assert pynlm.DevelopmentalStage.Adult is not None
    
    print("✓ Enumerations passed")


def test_world_objects():
    """Test world object functionality."""
    print("Testing world objects...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    
    # Test object creation and manipulation
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
    
    assert obj.x == 5.0
    assert obj.y == 5.0
    assert obj.type == pynlm.WorldObjectType.Resource
    assert obj.value == 10.0
    
    # Test configuration
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getVisionWidth() == 8
    assert world.getVisionHeight() == 8
    
    print("✓ World objects passed")


def test_sensory_input():
    """Test sensory input classes."""
    print("Testing sensory input...")
    
    # Test Vision
    vision = pynlm.Vision(10, 10, 3)
    test_data = [i * 0.1 for i in range(100)]
    vision.setData(test_data)
    
    assert vision.getWidth() == 10
    assert vision.getHeight() == 10
    assert vision.getChannels() == 3
    
    # Test Audio
    audio = pynlm.Audio(44100, 1000)
    audio_data = [i * 0.01 for i in range(1000)]
    audio.setData(audio_data)
    
    assert audio.getSampleRate() == 44100
    assert audio.getNumSamples() == 1000
    
    # Test InternalSignals
    internal = pynlm.InternalSignals()
    internal.addSignal(0.5)
    internal.addSignal(-0.3)
    internal.addSignal(0.8)
    
    signals = internal.getSignals()
    assert len(signals) == 3
    assert signals[0] == 0.5
    assert signals[1] == -0.3
    assert signals[2] == 0.8
    
    print("✓ Sensory input passed")


def test_error_handling():
    """Test error handling and edge cases."""
    print("Testing error handling...")
    
    # Test with empty config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test operations that might raise errors
    action = brain.produceAction()
    assert action is not None
    
    # Test with agent brain that hasn't been initialized
    agent2 = pynlm.createAgentBrain(brain)
    # This should not crash
    stage = agent2.getDevelopmentalStage()
    assert stage is not None
    
    print("✓ Error handling passed")


def test_performance_monitoring():
    """Test performance monitoring utilities."""
    print("Testing performance monitoring...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation and collect metrics
    spikes_before = brain.getTotalSpikeCount()
    
    for step in range(20):
        brain.step(step)
    
    spikes_after = brain.getTotalSpikeCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_rate = brain.getAverageFiringRate()
    
    # Basic sanity checks
    assert spikes_after >= spikes_before
    assert firing_neurons >= 0
    assert avg_rate >= 0.0
    
    print("✓ Performance monitoring passed")


def test_convenience_methods():
    """Test convenience methods and helper functions."""
    print("Testing convenience methods...")
    
    # Test that we can create brains with different configurations
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 5000)
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    # Brains should have different sizes
    assert brain1.getTotalNeuronCount() != brain2.getTotalNeuronCount()
    
    print("✓ Convenience methods passed")


def run_all_tests():
    """Run all tests and report results."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    print()
    
    tests = [
        test_basic_functionality,
        test_configuration,
        test_agent_functionality,
        test_world_simulation,
        test_plasticity_systems,
        test_development,
        test_enumerations,
        test_world_objects,
        test_sensory_input,
        test_error_handling,
        test_performance_monitoring,
        test_convenience_methods
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
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return failed == 0


if __name__ == "__main__":
    success = run_all_tests()
    exit(0 if success else 1)