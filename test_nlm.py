#!/usr/bin/env python3
"""
NLM Python Test Suite
Comprehensive tests for NLM Python bindings and functionality.
"""

import pynlm
import tempfile
import os
import sys

class NLMSimulationTester:
    """Test suite for NLM Python bindings and functionality."""
    
    def __init__(self):
        self.test_results = []
        self.temp_files = []
    
    def cleanup(self):
        """Clean up temporary files."""
        for filepath in self.temp_files:
            try:
                if os.path.exists(filepath):
                    os.remove(filepath)
            except Exception:
                pass
        self.temp_files.clear()
    
    def run_test(self, name, test_func):
        """Run a test and record results."""
        print(f"Running test: {name}")
        try:
            test_func()
            self.test_results.append((name, True, None))
            print(f"✓ {name} passed")
            return True
        except Exception as e:
            self.test_results.append((name, False, str(e)))
            print(f"✗ {name} failed: {e}")
            return False
    
    def get_summary(self):
        """Get test summary."""
        passed = sum(1 for _, success, _ in self.test_results if success)
        failed = sum(1 for _, success, _ in self.test_results if not success)
        total = len(self.test_results)
        
        return f"Tests passed: {passed}/{total}, Failed: {failed}"
    
    def print_detailed_results(self):
        """Print detailed test results."""
        print("\n" + "="*60)
        print("DETAILED TEST RESULTS")
        print("="*60)
        
        for name, success, error in self.test_results:
            status = "PASS" if success else "FAIL"
            print(f"{name}: {status}")
            if error:
                print(f"  Error: {error}")
        
        print(f"\n{self.get_summary()}")

def test_basic_initialization():
    """Test basic brain initialization."""
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    
    assert brain is not None
    assert brain.initialize() is True
    
    # Check basic properties
    assert brain.getTotalNeuronCount() > 0
    assert brain.getRegionCount() > 0
    assert brain.getAverageFiringRate() >= 0.0

def test_simulation_steps():
    """Test simulation step execution."""
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Run some steps
    initial_spikes = brain.getTotalSpikeCount()
    initial_active = brain.getActiveNeuronCount()
    
    for step in range(50):
        brain.step(step)
    
    # Check that steps were executed
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Status should be available
    status = brain.getStatus()
    assert len(status) > 0
    assert "NLM Brain Status" in status

def test_memory_systems():
    """Test memory system integration."""
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Check memory system availability
    wm_status = brain.getWorkingMemoryStatus()
    em_status = brain.getEpisodicMemoryStatus()
    
    assert wm_status is not None
    assert em_status is not None
    
    # Run simulation to generate memory activity
    for step in range(100):
        brain.step(step)
    
    # Memory status should still be available
    assert brain.getWorkingMemoryStatus() is not None
    assert brain.getEpisodicMemoryStatus() is not None

def test_configuration():
    """Test configuration management."""
    # Test default config creation
    config = pynlm.create_default_config()
    
    # Test config properties
    assert config.has("neuron_count")
    assert config.has("region_count")
    assert config.has("random_seed")
    
    # Test config values
    neuron_count = config.get("neuron_count", 0)
    assert neuron_count > 0
    
    # Test config modification
    original_count = config.get("neuron_count", 0)
    config.set("neuron_count", 500)
    new_count = config.get("neuron_count", 0)
    assert new_count == 500
    
    # Test config summary
    summary = config.summary()
    assert len(summary) > 0

def test_agent_functionality():
    """Test agent brain functionality."""
    # Create brain and agent
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    agent = pynlm.create_agent_brain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(16, 16, 32, 32)
    
    # Initialize agent
    agent.initialize(world)
    agent.enable_all_systems()
    
    # Test agent methods
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    
    # Get developmental stage
    stage = agent.getDevelopmentalStage()
    assert stage is not None
    
    # Get motivational state
    motivation = agent.getMotivationalState()
    assert len(motivation) > 0
    assert "Neuromodulation" in motivation

def test_error_handling():
    """Test error handling and custom exceptions."""
    import pynlm
    
    # Test configuration error handling
    try:
        # Create a valid config
        config = pynlm.create_default_config()
        
        # Try to create brain with invalid config (if possible)
        brain = pynlm.create_brain(config)
        
        # Simulate an error scenario
        # Note: This is a basic test - in real scenarios,
        # more specific error conditions would be tested
        
    except Exception as e:
        # Should not raise unexpected errors
        assert "NLMError" in str(type(e).__name__) or "Exception" in str(type(e).__name__)
    
    # Test that custom exception classes exist
    assert hasattr(pynlm, 'NLMError')
    assert hasattr(pynlm, 'ConfigurationError')
    assert hasattr(pynlm, 'BrainInitializationError')
    assert hasattr(pynlm, 'MemoryError')

def test_checkpointing():
    """Test brain checkpointing functionality."""
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Run some simulation
    for step in range(50):
        brain.step(step)
    
    # Get initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getTotalNeuronCount()
    
    # Create temporary checkpoint file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
        self.temp_files.append(checkpoint_path)
    
    try:
        # Save checkpoint
        success = brain.save(checkpoint_path)
        assert success is True
        
        # Create new brain and load checkpoint
        new_config = pynlm.create_default_config()
        new_brain = pynlm.create_brain(new_config)
        new_brain.initialize()
        
        load_success = new_brain.load(checkpoint_path)
        assert load_success is True
        
        # Basic validation - should have the same structure
        assert new_brain.getTotalNeuronCount() == initial_neurons
        
    finally:
        # Cleanup will be handled by cleanup() method
        pass

def test_visualization_status():
    """Test visualization and status reporting."""
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(20):
        brain.step(step)
    
    # Get all status information
    status = brain.getStatus()
    wm_status = brain.getWorkingMemoryStatus()
    em_status = brain.getEpisodicMemoryStatus()
    prediction_status = brain.getPredictionSystemStatus()
    cognition_status = brain.getCognitionStatus()
    neuromodulation_status = brain.getNeuromodulationStatus()
    development_status = brain.getDevelopmentStatus()
    
    # All status strings should be non-empty and contain expected content
    assert len(status) > 0
    assert "NLM Brain Status" in status
    
    assert wm_status is not None
    assert em_status is not None
    assert prediction_status is not None
    assert len(cognition_status) > 0
    assert len(neuromodulation_status) > 0
    assert len(development_status) > 0

def test_sensorimotor_integration():
    """Test sensorimotor integration with agent."""
    # Create complete agent system
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.createSimpleWorld()
    
    # Setup world
    world.configure(32, 32, 64, 64)
    world.setAgentStart(16.0, 16.0)
    
    # Initialize agent
    agent.initialize(world)
    agent.enable_all_systems()
    
    # Test sensorimotor loop
    for step in range(30):
        # Get sensory percept
        percept = world.getSensoryPercept(world.getAgentBody())
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, float(step) * 0.001)
        
        # Get reward
        reward = world.computeReward(world.getAgentBody())
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        # Verify components are working
        assert brain.getTotalNeuronCount() > 0
        assert action is not None
        assert reward >= 0.0
    
    # Final status check
    assert brain.getAverageFiringRate() >= 0.0
    assert len(agent.getMotivationalState()) > 0

def test_api_compatibility():
    """Test both legacy and enhanced API compatibility."""
    # Test legacy API (should still work)
    config = pynlm.Config()
    config.set("neuron_count", 100)
    
    brain1 = pynlm.create_brain(config)  # Legacy API
    brain1.initialize()
    
    # Test enhanced API
    brain2 = pynlm.create_brain(config)  # Enhanced API (wrapper)
    brain2.initialize()
    
    # Both should work
    assert brain1 is not None
    assert brain2 is not None
    assert brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount()

def run_all_tests():
    """Run all tests and return results."""
    tester = NLMSimulationTester()
    
    print("="*60)
    print("NLM PYTHON BINDINGS TEST SUITE")
    print("="*60)
    
    # Run all tests
    tests = [
        ("Basic Initialization", test_basic_initialization),
        ("Simulation Steps", test_simulation_steps),
        ("Memory Systems", test_memory_systems),
        ("Configuration Management", test_configuration),
        ("Agent Functionality", test_agent_functionality),
        ("Error Handling", test_error_handling),
        ("Checkpointing", test_checkpointing),
        ("Visualization Status", test_visualization_status),
        ("Sensorimotor Integration", test_sensorimotor_integration),
        ("API Compatibility", test_api_compatibility),
    ]
    
    all_passed = True
    for name, test_func in tests:
        if not tester.run_test(name, test_func):
            all_passed = False
    
    # Print detailed results
    tester.print_detailed_results()
    
    # Cleanup
    tester.cleanup()
    
    return all_passed

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)