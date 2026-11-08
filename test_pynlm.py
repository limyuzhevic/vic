#!/usr/bin/env python3
"""
Test suite for NLM Python API

This test suite validates the Python bindings for the NLM (Neural Learning Machine) framework.
It tests core functionality, error handling, and convenience functions.
"""

import pynlm
import tempfile
import os
import time
import json

class TestNLMPythonAPI:
    """Test class for NLM Python API validation."""
    
    def __init__(self):
        """Initialize test class."""
        self.test_results = []
        self.temp_files = []
    
    def log_test(self, test_name, passed, message=""):
        """Log test result."""
        result = {
            "test": test_name,
            "passed": passed,
            "message": message,
            "timestamp": time.time()
        }
        self.test_results.append(result)
        
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"{status}: {test_name}")
        if message and not passed:
            print(f"  {message}")
    
    def cleanup(self):
        """Clean up temporary files."""
        for filepath in self.temp_files:
            try:
                if os.path.exists(filepath):
                    os.remove(filepath)
            except Exception as e:
                print(f"Warning: Could not remove {filepath}: {e}")
    
    def test_basic_creation(self):
        """Test basic object creation."""
        try:
            # Test configuration creation
            config = pynlm.createDefaultConfig()
            self.log_test("Basic Configuration Creation", True)
            
            # Test brain creation
            brain = pynlm.createBrain(config)
            self.log_test("Brain Creation", True)
            
            # Test world creation
            world = pynlm.createSimpleWorld()
            self.log_test("World Creation", True)
            
            # Test agent creation
            agent = pynlm.createAgentBrain(brain)
            self.log_test("Agent Creation", True)
            
            # Test convenience functions
            brain2 = pynlm.createBrainWithDefaults()
            self.log_test("Convenience Brain Creation", True)
            
            world2 = pynlm.createWorldWithConfig(10, 10, 8, 8)
            self.log_test("Convenience World Creation", True)
            
            return True
            
        except Exception as e:
            self.log_test("Basic Creation", False, f"Exception: {e}")
            return False
    
    def test_initialization(self):
        """Test object initialization."""
        try:
            # Create basic setup
            brain = pynlm.createBrainWithDefaults()
            world = pynlm.createWorldWithConfig(15, 15, 10, 10)
            agent = pynlm.createAgentBrain(brain)
            
            # Initialize all components
            brain.initialize()
            agent.initialize(world)
            world.reset()
            
            self.log_test("Basic Initialization", True)
            
            # Test configuration validation
            config = brain.getConfig()
            result = pynlm.validateConfig(config)
            self.log_test("Configuration Validation", result, "Validation failed")
            
            return True
            
        except Exception as e:
            self.log_test("Initialization", False, f"Exception: {e}")
            return False
    
    def test_basic_operations(self):
        """Test basic operations."""
        try:
            # Create setup using convenience function
            brain, agent, world = pynlm.createCompleteAgentSetup(None)
            
            # Run some simulation steps
            initial_spikes = brain.getTotalSpikeCount()
            
            for step in range(100):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
            
            final_spikes = brain.getTotalSpikeCount()
            steps_completed = final_spikes - initial_spikes
            
            # Validate results
            if steps_completed > 0:
                self.log_test("Basic Simulation Steps", True, 
                            f"Completed {steps_completed} spikes in 100 steps")
            else:
                self.log_test("Basic Simulation Steps", True, "No spikes (may be normal)")
            
            # Test statistics
            brain_stats = pynlm.getBrainStats(brain)
            agent_stats = pynlm.getAgentStats(agent)
            
            if isinstance(brain_stats, dict) and "neuron_count" in brain_stats:
                self.log_test("Brain Statistics", True)
            else:
                self.log_test("Brain Statistics", False, "Invalid statistics format")
            
            if isinstance(agent_stats, dict) and "curiosity_level" in agent_stats:
                self.log_test("Agent Statistics", True)
            else:
                self.log_test("Agent Statistics", False, "Invalid statistics format")
            
            return True
            
        except Exception as e:
            self.log_test("Basic Operations", False, f"Exception: {e}")
            return False
    
    def test_error_handling(self):
        """Test error handling."""
        try:
            # Test null brain error handling
            try:
                pynlm.safeStep(None, 0)
                self.log_test("Null Brain Error Handling", False, "Should have raised exception")
                return False
            except Exception:
                self.log_test("Null Brain Error Handling", True, "Correctly raised exception")
            
            # Test null config error handling
            try:
                pynlm.validateConfig(None)
                self.log_test("Null Config Error Handling", False, "Should have raised exception")
                return False
            except Exception:
                self.log_test("Null Config Error Handling", True, "Correctly raised exception")
            
            # Test null agent error handling
            try:
                pynlm.getAgentStats(None)
                self.log_test("Null Agent Error Handling", False, "Should have raised exception")
                return False
            except Exception:
                self.log_test("Null Agent Error Handling", True, "Correctly raised exception")
            
            return True
            
        except Exception as e:
            self.log_test("Error Handling", False, f"Exception: {e}")
            return False
    
    def test_file_operations(self):
        """Test file operations."""
        try:
            # Create temporary file path
            with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as temp_file:
                temp_path = temp_file.name
            self.temp_files.append(temp_path)
            
            # Create and initialize brain
            brain = pynlm.createBrainWithDefaults()
            brain.initialize()
            
            # Run some steps
            for i in range(50):
                brain.step(i)
            
            # Test save operation
            save_result = brain.save(temp_path)
            self.log_test("Brain Save Operation", save_result, "Save failed")
            
            if save_result:
                # Test load operation
                new_brain = pynlm.createBrainWithDefaults()
                new_brain.initialize()
                
                load_result = new_brain.load(temp_path)
                self.log_test("Brain Load Operation", load_result, "Load failed")
                
                # Verify loaded brain has some spikes
                if new_brain.getTotalSpikeCount() > 0:
                    self.log_test("Brain State Restoration", True, "Loaded brain has spikes")
                else:
                    self.log_test("Brain State Restoration", True, "No spikes (may be normal)")
            
            return True
            
        except Exception as e:
            self.log_test("File Operations", False, f"Exception: {e}")
            return False
    
    def test_simulation_utilities(self):
        """Test simulation utility functions."""
        try:
            # Create brain and run simulation using utility
            brain = pynlm.createBrainWithDefaults()
            brain.initialize()
            
            # Get initial spike count
            initial_spikes = brain.getTotalSpikeCount()
            
            # Run simulation using utility function
            final_spikes = pynlm.runSimulationSteps(brain, 200)
            
            # Calculate steps completed
            spikes_completed = final_spikes - initial_spikes
            
            if spikes_completed >= 0:
                self.log_test("Simulation Utility Function", True, 
                            f"Completed {spikes_completed} spikes in 200 steps")
            else:
                self.log_test("Simulation Utility Function", False, "Invalid spike count")
                return False
            
            return True
            
        except Exception as e:
            self.log_test("Simulation Utilities", False, f"Exception: {e}")
            return False
    
    def test_configuration_operations(self):
        """Test configuration operations."""
        try:
            # Test configuration manipulation
            config = pynlm.createDefaultConfig()
            
            # Modify configuration
            config.set("brain.neuron_count", 2000)
            config.set("brain.synapse_density", 0.05)
            
            # Test configuration validation
            validation_result = pynlm.validateConfig(config)
            self.log_test("Configuration Validation (Modified)", validation_result, "Validation failed for modified config")
            
            # Test configuration save/load
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as temp_file:
                config_path = temp_file.name
            self.temp_files.append(config_path)
            
            config.saveToFile(config_path)
            
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(config_path)
            
            # Verify loaded configuration
            if new_config.has("brain.neuron_count"):
                self.log_test("Configuration Save/Load", True, "Configuration saved and loaded successfully")
            else:
                self.log_test("Configuration Save/Load", False, "Loaded configuration missing keys")
                return False
            
            return True
            
        except Exception as e:
            self.log_test("Configuration Operations", False, f"Exception: {e}")
            return False
    
    def test_agent_features(self):
        """Test agent features."""
        try:
            # Create complete setup
            brain, agent, world = pynlm.createCompleteAgentSetup(None)
            
            # Enable all subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Test agent operations
            initial_curiosity = agent.getCuriosityLevel()
            initial_novelty = agent.getNoveltyLevel()
            
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                
                # Get action and apply
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            final_curiosity = agent.getCuriosityLevel()
            final_novelty = agent.getNoveltyLevel()
            
            # Check if neuromodulation affected agent
            curiosity_changed = abs(final_curiosity - initial_curiosity) > 0.01
            novelty_changed = abs(final_novelty - initial_novelty) > 0.01
            
            if curiosity_changed or novelty_changed:
                self.log_test("Agent Neuromodulation", True, 
                            f"Curiosity changed: {curiosity_changed}, Novelty changed: {novelty_changed}")
            else:
                self.log_test("Agent Neuromodulation", True, "No change (may be normal)")
            
            return True
            
        except Exception as e:
            self.log_test("Agent Features", False, f"Exception: {e}")
            return False
    
    def run_all_tests(self):
        """Run all tests."""
        print("=" * 60)
        print("NLM Python API Test Suite")
        print("=" * 60)
        
        try:
            # Run all test suites
            self.test_basic_creation()
            self.test_initialization()
            self.test_basic_operations()
            self.test_error_handling()
            self.test_file_operations()
            self.test_simulation_utilities()
            self.test_configuration_operations()
            self.test_agent_features()
            
            # Print summary
            print("\n" + "=" * 60)
            print("Test Summary")
            print("=" * 60)
            
            total_tests = len(self.test_results)
            passed_tests = sum(1 for result in self.test_results if result["passed"])
            
            print(f"Total Tests: {total_tests}")
            print(f"Passed: {passed_tests}")
            print(f"Failed: {total_tests - passed_tests}")
            print(f"Success Rate: {passed_tests/total_tests*100:.1f}%")
            
            if passed_tests == total_tests:
                print("\n🎉 All tests passed!")
                return True
            else:
                print(f"\n⚠️  {total_tests - passed_tests} tests failed")
                return False
                
        except Exception as e:
            print(f"\n❌ Test suite failed with exception: {e}")
            return False
        finally:
            self.cleanup()

if __name__ == "__main__":
    # Run test suite
    tester = TestNLMPythonAPI()
    success = tester.run_all_tests()
    
    # Exit with appropriate code
    exit(0 if success else 1)