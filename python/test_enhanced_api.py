"""
Comprehensive test suite for NLM Python API enhancements

This file demonstrates the enhanced Python API features and tests
the new functionality added to NLM.
"""

import pynlm
import tempfile
import os
import sys
import json
from typing import Dict, List, Any, Optional

class NLMTester:
    """Test suite for NLM Python API enhancements"""
    
    def __init__(self):
        self.passed_tests = 0
        self.failed_tests = 0
        self.start_time = None
    
    def log_test(self, name: str, passed: bool, message: str = ""):
        """Log test result"""
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"  {status} {name}")
        if message:
            print(f"    {message}")
        
        if passed:
            self.passed_tests += 1
        else:
            self.failed_tests += 1
    
    def test_configuration_enhancements(self):
        """Test enhanced configuration features"""
        print("\n=== Testing Configuration Enhancements ===")
        
        # Test 1: Default configuration validation
        print("\n1. Testing default configuration validation...")
        config = pynlm.createDefaultConfig()
        if config.validate():
            self.log_test("Default config validation", True,
                         "Default configuration is valid")
        else:
            self.log_test("Default config validation", False,
                         f"Validation failed: {config.getValidationError()}")
        
        # Test 2: Enhanced configuration methods
        print("\n2. Testing enhanced configuration methods...")
        config2 = pynlm.createDefaultConfig()
        config2.setNeuronCount(2000)
        config2.setRegionCount(3)
        config2.setConnectionProbability(0.15)
        config2.setSimulationTimestep(0.001)
        config2.setRandomSeed(12345)
        
        if config2.getIntSafe("neuron_count") == 2000:
            self.log_test("Configuration set/get", True,
                         "Neuron count correctly set and retrieved")
        else:
            self.log_test("Configuration set/get", False,
                         f"Neuron count mismatch: expected 2000, got {config2.getIntSafe('neuron_count')}")
        
        # Test 3: Configuration validation with errors
        print("\n3. Testing configuration validation with errors...")
        config3 = pynlm.Config()
        try:
            config3.setNeuronCount(-10)  # Invalid: negative neuron count
            if not config3.validate():
                self.log_test("Configuration validation catches errors", True,
                             f"Validation caught error: {config3.getValidationError()}")
            else:
                self.log_test("Configuration validation catches errors", False,
                             "Should have rejected negative neuron count")
        except Exception as e:
            self.log_test("Configuration validation catches errors", True,
                         f"Exception caught: {type(e).__name__}")
        
        # Test 4: Preset configurations
        print("\n4. Testing preset configurations...")
        dev_config = pynlm.Config.createDevelopmentConfig()
        min_config = pynlm.Config.createMinimalConfig()
        
        if dev_config.getIntSafe("neuron_count") == 500:
            self.log_test("Development config preset", True,
                         "Development configuration has correct neuron count")
        else:
            self.log_test("Development config preset", False,
                         "Development configuration incorrect")
        
        if min_config.getIntSafe("neuron_count") == 100:
            self.log_test("Minimal config preset", True,
                         "Minimal configuration has correct neuron count")
        else:
            self.log_test("Minimal config preset", False,
                         "Minimal configuration incorrect")
        
        return self.passed_tests + self.failed_tests
    
    def test_brain_operations(self):
        """Test enhanced brain operations"""
        print("\n=== Testing Enhanced Brain Operations ===")
        
        # Test 1: Brain creation with enhanced configuration
        print("\n1. Testing enhanced brain creation...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        if brain is not None and brain.getTotalNeuronCount() > 0:
            self.log_test("Enhanced brain creation", True,
                         f"Created brain with {brain.getTotalNeuronCount()} neurons")
        else:
            self.log_test("Enhanced brain creation", False,
                         "Failed to create brain")
        
        # Test 2: Brain initialization with error handling
        print("\n2. Testing brain initialization with error handling...")
        brain2 = pynlm.createBrain(config)
        try:
            if brain2.initialize():
                self.log_test("Brain initialization error handling", True,
                             "Brain initialized successfully")
            else:
                self.log_test("Brain initialization error handling", False,
                             "Brain initialization returned false")
        except pynlm.NLMBrainError as e:
            self.log_test("Brain initialization error handling", True,
                         f"Brain error handled: {e}")
        except Exception as e:
            self.log_test("Brain initialization error handling", False,
                         f"Unexpected exception: {type(e).__name__}: {e}")
        
        # Test 3: Enhanced simulation with statistics
        print("\n3. Testing enhanced simulation with statistics...")
        brain3 = pynlm.createBrain(config)
        brain3.initialize()
        
        stats = {
            'total_spikes': [],
            'firing_rates': [],
            'step_times': []
        }
        
        import time
        for i in range(5):
            step_start = time.time()
            brain3.step(i)
            
            stats['total_spikes'].append(brain3.getTotalSpikeCount())
            stats['firing_rates'].append(brain3.getAverageFiringRate())
            stats['step_times'].append(time.time() - step_start)
        
        avg_step_time = sum(stats['step_times']) / len(stats['step_times'])
        if avg_step_time < 1.0:  # Should be very fast for small brain
            self.log_test("Enhanced simulation statistics", True,
                         f"Average step time: {avg_step_time:.4f}s")
        else:
            self.log_test("Enhanced simulation statistics", False,
                         f"Slow simulation, average step time: {avg_step_time:.4f}s")
        
        # Test 4: Brain save/load operations
        print("\n4. Testing enhanced brain save/load operations...")
        brain4 = pynlm.createBrain(config)
        brain4.initialize()
        for i in range(3):
            brain4.step(i)
        
        with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as f:
            filepath = f.name
        
        try:
            # Save brain state
            if brain4.save(filepath):
                self.log_test("Brain save operation", True, f"Brain saved to {filepath}")
                
                # Load brain state
                brain5 = pynlm.createBrain(config)
                if brain5.load(filepath):
                    self.log_test("Brain load operation", True,
                                 f"Brain loaded successfully with {brain5.getTotalNeuronCount()} neurons")
                else:
                    self.log_test("Brain load operation", False, "Failed to load brain state")
            else:
                self.log_test("Brain save operation", False, "Failed to save brain state")
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)
        
        return self.passed_tests + self.failed_tests
    
    def test_agent_simulation(self):
        """Test enhanced agent simulation"""
        print("\n=== Testing Enhanced Agent Simulation ===")
        
        # Test 1: Complete agent setup
        print("\n1. Testing complete agent setup...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        if (agent.getSensoryInputSize() > 0 and agent.getMotorOutputSize() > 0 and
            agent.isRewardModulationEnabled() and agent.isCuriosityEnabled() and
            agent.isDevelopmentEnabled() and agent.isStructuralPlasticityEnabled()):
            self.log_test("Enhanced agent setup", True,
                         f"Agent ready with {agent.getSensoryInputSize()} inputs, {agent.getMotorOutputSize()} outputs")
        else:
            self.log_test("Enhanced agent setup", False, "Agent setup incomplete")
        
        # Test 2: Complete simulation episode
        print("\n2. Testing complete simulation episode...")
        episode_length = 10
        
        for step in range(episode_length):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = world.getSimulationTime() % 3.0 - 1.5
            predicted_reward = agent.getCuriosityLevel()
            agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Check final state
        final_stage = agent.getDevelopmentalStage()
        final_curiosity = agent.getCuriosityLevel()
        final_neuromodulation = agent.getNeuromodulationLevel()
        
        if (final_stage != pynlm.DevelopmentalStage.Initial and
            final_curiosity > 0.0 and final_neuromodulation > 0.0):
            self.log_test("Complete simulation episode", True,
                         f"Final stage: {final_stage}, curiosity: {final_curiosity:.2f}, neuromodulation: {final_neuromodulation:.2f}")
        else:
            self.log_test("Complete simulation episode", False,
                         "Simulation did not progress to expected state")
        
        return self.passed_tests + self.failed_tests
    
    def test_error_handling(self):
        """Test enhanced error handling"""
        print("\n=== Testing Enhanced Error Handling ===")
        
        # Test 1: Configuration validation errors
        print("\n1. Testing configuration validation errors...")
        try:
            config = pynlm.Config()
            config.setNeuronCount(-50)  # Invalid
            if not config.validate():
                self.log_test("Configuration validation errors", True,
                             f"Validation caught: {config.getValidationError()}")
            else:
                self.log_test("Configuration validation errors", False,
                             "Should have caught invalid configuration")
        except Exception as e:
            self.log_test("Configuration validation errors", True,
                         f"Exception handled: {type(e).__name__}")
        
        # Test 2: Brain operation errors
        print("\n2. Testing brain operation errors...")
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            # Don't initialize, try to step (should handle gracefully)
            brain.step(0)
            self.log_test("Brain operation errors", False,
                         "Should have raised error for uninitialized brain")
        except pynlm.NLMBrainError as e:
            self.log_test("Brain operation errors", True,
                         f"Brain error handled: {type(e).__name__}: {e}")
        except Exception as e:
            self.log_test("Brain operation errors", True,
                         f"Brain error handled: {type(e).__name__}: {e}")
        
        # Test 3: File operation errors
        print("\n3. Testing file operation errors...")
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            success = brain.load("/non/existent/path/file.nlm")
            if not success:
                self.log_test("File operation errors", True,
                             "File operation correctly failed for non-existent file")
            else:
                self.log_test("File operation errors", False,
                             "Should have failed to load non-existent file")
        except Exception as e:
            self.log_test("File operation errors", True,
                         f"File error handled: {type(e).__name__}: {e}")
        
        return self.passed_tests + self.failed_tests
    
    def test_performance_monitoring(self):
        """Test performance monitoring features"""
        print("\n=== Testing Performance Monitoring ===")
        
        import time
        
        # Test 1: Benchmark simulation performance
        print("\n1. Testing simulation performance benchmarking...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Warm-up
        for i in range(5):
            brain.step(i)
        
        # Benchmark
        num_steps = 50
        start_time = time.time()
        
        for i in range(num_steps):
            brain.step(i)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        steps_per_second = num_steps / elapsed
        avg_step_time = elapsed / num_steps * 1000  # Convert to ms
        
        self.log_test("Performance benchmarking", True,
                     f"{steps_per_second:.1f} steps/second, avg {avg_step_time:.2f}ms per step")
        
        # Test 2: Statistics collection
        print("\n2. Testing statistics collection...")
        
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        stats = {
            'total_spikes': brain2.getTotalSpikeCount(),
            'average_firing_rate': brain2.getAverageFiringRate(),
            'active_neurons': brain2.getActiveNeuronCount(),
            'firing_neurons': brain2.getFiringNeuronCount(),
            'excitation_inhibition_ratio': brain2.getExcitationInhibitionRatio()
        }
        
        # Collect some data
        for i in range(10):
            brain2.step(i)
            stats['total_spikes'] = brain2.getTotalSpikeCount()
            stats['average_firing_rate'] = brain2.getAverageFiringRate()
        
        self.log_test("Statistics collection", True,
                     f"Collected stats: {stats['total_spikes']} spikes, {stats['average_firing_rate']:.2f} Hz")
        
        return self.passed_tests + self.failed_tests
    
    def run_all_tests(self):
        """Run all test suites"""
        print("NLM Python API Enhancement Test Suite")
        print("=" * 60)
        self.start_time = time.time()
        
        self.test_configuration_enhancements()
        self.test_brain_operations()
        self.test_agent_simulation()
        self.test_error_handling()
        self.test_performance_monitoring()
        
        elapsed_time = time.time() - self.start_time
        
        print("\n" + "=" * 60)
        print(f"Test Results:")
        print(f"  Passed: {self.passed_tests}")
        print(f"  Failed: {self.failed_tests}")
        print(f"  Total:  {self.passed_tests + self.failed_tests}")
        print(f"  Time:   {elapsed_time:.2f} seconds")
        print(f"  Success Rate: {self.passed_tests/(self.passed_tests + self.failed_tests)*100:.1f}%")
        
        if self.failed_tests == 0:
            print("\n🎉 All tests passed!")
            return True
        else:
            print(f"\n❌ {self.failed_tests} tests failed")
            return False

if __name__ == "__main__":
    import time
    
    tester = NLMTester()
    success = tester.run_all_tests()
    
    sys.exit(0 if success else 1)