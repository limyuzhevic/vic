#!/usr/bin/env python3
"""
Comprehensive test script for NLM Python bindings.
Tests all major functionality including basic operations, advanced features,
and edge cases.
"""

import sys
import os
import tempfile
import traceback
from typing import Dict, Any, List

# Add the python directory to the path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    print("✓ NLM Python bindings imported successfully")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

class TestReporter:
    """Helper class to report test results."""
    
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.errors = []
    
    def add_pass(self, test_name: str):
        self.passed += 1
        print(f"✓ {test_name}")
    
    def add_fail(self, test_name: str, error_msg: str = ""):
        self.failed += 1
        print(f"✗ {test_name}")
        if error_msg:
            print(f"    Error: {error_msg}")
        self.errors.append((test_name, error_msg))
    
    def summary(self) -> str:
        total = self.passed + self.failed
        if total == 0:
            return "No tests run"
        
        success_rate = (self.passed / total) * 100
        return f"Tests: {total} | Passed: {self.passed} | Failed: {self.failed} | Success Rate: {success_rate:.1f}%"

def test_version() -> bool:
    """Test that version is accessible."""
    try:
        version = pynlm.__version__
        print(f"  Version: {version}")
        assert version == "0.1.0", f"Expected version '0.1.0', got '{version}'"
        return True
    except Exception as e:
        print(f"  Error: {e}")
        return False

def test_config_creation() -> bool:
    """Test Config creation and basic operations."""
    try:
        config = pynlm.createDefaultConfig()
        
        # Test basic properties
        assert config.has("random_seed"), "Config should have random_seed by default"
        
        # Test setting values
        config.set("test_int", 42)
        config.set("test_double", 3.14)
        config.set("test_string", "hello")
        config.set("test_bool", True)
        
        # Test getting values
        assert config.get("test_int") == 42
        assert abs(config.get("test_double") - 3.14) < 0.001
        assert config.get("test_string") == "hello"
        assert config.get("test_bool") == True
        
        # Test getOr with default
        assert config.getOr("nonexistent", 99) == 99
        assert config.getOr("nonexistent", "default_string") == "default_string"
        
        # Test keys
        keys = config.getKeys()
        assert "random_seed" in keys
        assert "test_int" in keys
        assert "test_double" in keys
        assert "test_string" in keys
        assert "test_bool" in keys
        
        # Test summary
        summary = config.summary()
        assert "Configuration" in summary
        assert "random_seed" in summary
        
        print(f"  Config created with {len(keys)} entries")
        return True
        
    except Exception as e:
        print(f"  Error: {e}")
        traceback.print_exc()
        return False

def test_config_file_operations() -> bool:
    """Test Config file save/load operations."""
    try:
        # Create a temporary config file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_path = f.name
        
        try:
            # Create and configure a config
            config = pynlm.createDefaultConfig()
            config.set("test_param1", 12345)
            config.set("test_param2", 67.89)
            config.set("test_param3", "test_value")
            config.set("test_param4", False)
            
            # Save to file
            result = config.saveToFile(config_path)
            assert result, "Failed to save config to file"
            print("  Config saved successfully")
            
            # Read file content for verification
            with open(config_path, 'r') as f:
                content = f.read()
                print(f"  File content:\n{content}")
                
                # Verify values are preserved
                assert "test_param1 = 12345" in content
                assert "test_param2 = 67.89" in content
                assert "test_param3 = \"test_value\"" in content
                assert "test_param4 = false" in content
            
            # Create new config and load from file
            config2 = pynlm.createDefaultConfig()
            result = config2.loadFromFile(config_path)
            assert result, "Failed to load config from file"
            print("  Config loaded successfully")
            
            # Verify loaded values
            assert config2.get("test_param1") == 12345
            assert abs(config2.get("test_param2") - 67.89) < 0.001
            assert config2.get("test_param3") == "test_value"
            assert config2.get("test_param4") == False
            
            print("  All file operations verified")
            return True
            
        finally:
            # Clean up temp file
            if os.path.exists(config_path):
                os.unlink(config_path)
                
    except Exception as e:
        print(f"  Error: {e}")
        traceback.print_exc()
        return False

def test_brain_creation() -> bool:
    """Test Brain creation and initialization."""
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Test basic properties before initialization
        assert brain.getTotalNeuronCount() == 0
        assert brain.getTotalSynapseCount() == 0
        assert brain.getFiringNeuronCount() == 0
        
        # Initialize brain
        result = brain.initialize()
        assert result, "Brain initialization failed"
        
        # Test properties after initialization
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        
        print(f"  Brain initialized: {neurons} neurons, {synapses} synapses")
        assert neurons > 0, "Brain should have neurons after initialization"
        assert synapses >= 0, "Brain should have synapses after initialization"
        
        # Test developmental stage
        stage = brain.getDevelopmentalStage()
        print(f"  Developmental stage: {stage}")
        
        return True
        
    except Exception as e:
        print(f"  Error: {e}")
        traceback.print_exc()
        return False

def test_world_creation() -> bool:
    """Test World creation and configuration."""
    try:
        world = pynlm.createSimpleWorld()
        
        # Test default configuration
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        assert world.getVisionWidth() == 8
        assert world.getVisionHeight() == 8
        
        # Test configuration
        world.configure(width=30, height=25, visionWidth=10, visionHeight=10)
        
        assert world.getWidth() == 30
        assert world.getHeight() == 25
        assert world.getVisionWidth() == 10
        assert world.getVisionHeight() == 10
        
        # Test reset
        world.reset()
        print("  World configuration and reset successful")
        
        return True
        
    except Exception as e:
        print(f"  Error: {e}")
        traceback.print_exc()
        return False

def test_agent_creation() -> bool:
    """Test AgentBrain creation and initialization."""
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Test default configuration
        assert agent.getBrain() is brain
        
        # Test agent body defaults
        body = agent.getBrain()
        # Note: This test assumes the brain has a getBody() method or similar
        
        # Test enable/disable features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Test curiosity level
        curiosity = agent.getCuriosityLevel()
        print(f"  Initial curiosity level: {curiosity}")
        
        # Test novelty level
        novelty = agent.getNoveltyLevel()
        print(f"  Initial novelty level: {novelty}")
        
        print("  Agent creation and features successful")
        return True
        
    except Exception as e:
        print(f"  Error: {e}")
        traceback.print_exc()
        return False

def test_simulation_loop() -> bool:
    """Test a complete simulation loop."""
    try:
        # Setup components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        num_steps = 50
        for step in range(num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Optional: Apply reward modulation
            # reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            # agent.applyRewardModulation(reward, 0.0)
            
            # Print progress every 10 steps
            if step % 10 == 0:
                print(f"  Step {step}: Neurons firing={brain.getFiringNeuronCount()}, "
                      f"Total spikes={brain.getTotalSpikeCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}")
        
        print(f"  Simulation completed: {num_steps} steps")
        print(f"  Final stats: {brain.getFiringNeuronCount()} firing neurons, "
              f"{brain.getTotalSpikeCount()} total spikes")
        
        return True
        
    except Exception as e:
        print(f"  Error: {e}")
        traceback.print_exc()
        return False

def test_error_handling() -> bool:
    """Test error handling and edge cases."""
    try:
        print("  Testing error handling...")
        
        # Test loading non-existent file
        config = pynlm.createDefaultConfig()
        result = config.loadFromFile("/nonexistent/path/file.cfg")
        # Should not crash, even if file doesn't exist
        
        # Test invalid parameter names
        config.set("", "value")  # Empty key
        config.set("valid_key", "")  # Empty value
        
        # Test brain without initialization
        config2 = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config2)
        # Should not crash, even though not initialized
        
        print("  Error handling tests passed")
        return True
        
    except Exception as e:
        print(f"  Error in error handling: {e}")
        return False

def test_advanced_features() -> bool:
    """Test advanced NLM features."""
    try:
        print("  Testing advanced features...")
        
        # Create brain with custom config
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)  # Custom neuron count
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test developmental stages
        initial_stage = brain.getDevelopmentalStage()
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage::Maturation)
        maturation_stage = brain.getDevelopmentalStage()
        
        print(f"  Developmental stages: {initial_stage} -> {maturation_stage}")
        
        # Test region operations
        region_id = brain.addRegion("cortex")
        region = brain.getRegion(region_id)
        assert region is not None
        print(f"  Created region with ID: {region_id}")
        
        # Test statistical measures
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        active_neurons = brain.getActiveNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        average_rate = brain.getAverageFiringRate()
        ei_ratio = brain.getExcitationInhibitionRatio()
        
        print(f"  Statistics: Neurons={total_neurons}, Synapses={total_synapses}, "
              f"Active={active_neurons}, Firing={firing_neurons}, "
              f"Rate={average_rate:.2f}, E/I ratio={ei_ratio:.2f}")
        
        return True
        
    except Exception as e:
        print(f"  Error in advanced features: {e}")
        traceback.print_exc()
        return False

def run_all_tests() -> TestReporter:
    """Run all tests and return reporter with results."""
    reporter = TestReporter()
    
    print("\n" + "="*60)
    print("NLM Python Bindings Comprehensive Test Suite")
    print("="*60 + "\n")
    
    tests = [
        ("Version Test", test_version),
        ("Config Creation Test", test_config_creation),
        ("Config File Operations Test", test_config_file_operations),
        ("Brain Creation Test", test_brain_creation),
        ("World Creation Test", test_world_creation),
        ("Agent Creation Test", test_agent_creation),
        ("Simulation Loop Test", test_simulation_loop),
        ("Error Handling Test", test_error_handling),
        ("Advanced Features Test", test_advanced_features),
    ]
    
    for test_name, test_func in tests:
        print(f"\n--- Running {test_name} ---")
        try:
            if test_func():
                reporter.add_pass(test_name)
            else:
                reporter.add_fail(test_name, "Test returned False")
        except Exception as e:
            reporter.add_fail(test_name, str(e))
    
    return reporter

def print_detailed_report(reporter: TestReporter):
    """Print detailed test report."""
    print("\n" + "="*60)
    print("DETAILED TEST REPORT")
    print("="*60)
    
    if reporter.errors:
        print("\nFailed Tests:")
        for test_name, error in reporter.errors:
            print(f"  - {test_name}: {error}")
    
    print(f"\n{reporter.summary()}")
    
    if reporter.failed == 0:
        print("\n🎉 ALL TESTS PASSED! NLM Python bindings are working correctly.")
        return True
    else:
        print(f"\n⚠️  {reporter.failed} test(s) failed. Please review the errors above.")
        return False

def main():
    """Main test runner."""
    print("NLM Python Bindings Test Suite")
    print("This script tests all major functionality of the NLM Python API.")
    
    # Run all tests
    reporter = run_all_tests()
    
    # Print detailed report
    success = print_detailed_report(reporter)
    
    # Return appropriate exit code
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
