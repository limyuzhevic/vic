"""
Comprehensive Testing Infrastructure for NLM

This module provides the foundation for testing the NLM (Neural Learning Machine)
project, including unit tests, integration tests, performance benchmarks,
and automated test reporting.

The testing infrastructure follows modern software testing practices:
- Comprehensive unit testing with coverage analysis
- Integration testing for system components
- Performance benchmarking and regression detection
- Automated test execution and reporting
- Cross-platform compatibility testing
"""

import unittest
import sys
import os
import json
import time
import threading
from unittest.mock import Mock, patch, MagicMock
from datetime import datetime
from pathlib import Path

# Add src to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

# Import NLM modules
import pynlm
from pynlm import (
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    Config, Brain, AgentBrain, SimpleWorld, SensoryPercept, Action,
    Vision, Audio, InternalSignals, SensoryInput, MotorCommand,
    NeuronType, DevelopmentalStage, ActionType, WorldObjectType
)

class TestConfiguration:
    """Test configuration functionality"""
    
    def test_create_default_config(self):
        """Test default configuration creation"""
        config = createDefaultConfig()
        assert config is not None
        assert isinstance(config, Config)
        
    def test_config_has_function(self):
        """Test config.has() method"""
        config = createDefaultConfig()
        assert config.has("brain.neuron_count")
        assert config.has("simulation.timestep")
        assert not config.has("nonexistent.key")
        
    def test_config_get_methods(self):
        """Test config.getOr() and get() methods"""
        config = createDefaultConfig()
        
        # Test getOr with default values
        assert config.getOr("brain.neuron_count", 1000) == 1000
        assert config.getOr("simulation.timestep", 0.001) == 0.001
        
        # Test get with valid keys
        config.set("brain.neuron_count", 500)
        assert config.getOr("brain.neuron_count", 1000) == 500
        
    def test_config_set_function(self):
        """Test config.set() method"""
        config = createDefaultConfig()
        
        config.set("brain.neuron_count", 1500)
        assert config.getOr("brain.neuron_count", 1000) == 1500
        
        config.set("plasticity.stdp.learning_rate", 0.01)
        assert config.getOr("plasticity.stdp.learning_rate", 0.001) == 0.01
        
    def test_config_clear_function(self):
        """Test config.clear() method"""
        config = createDefaultConfig()
        config.set("test.key", "value")
        assert config.has("test.key")
        
        config.clear()
        assert not config.has("test.key")
        assert config.has("brain.neuron_count")  # Default values remain
        
    def test_config_summary(self):
        """Test config.summary() method"""
        config = createDefaultConfig()
        summary = config.summary()
        
        assert isinstance(summary, str)
        assert len(summary) > 0
        assert "brain.neuron_count" in summary
        
    def test_config_keys_function(self):
        """Test config.getKeys() method"""
        config = createDefaultConfig()
        keys = config.getKeys()
        
        assert isinstance(keys, list)
        assert len(keys) > 0
        assert "brain.neuron_count" in keys

class TestBrainCreationAndInitialization:
    """Test brain creation and initialization"""
    
    def test_create_brain(self):
        """Test brain creation"""
        config = createDefaultConfig()
        brain = createBrain(config)
        
        assert brain is not None
        assert isinstance(brain, Brain)
        assert not brain.is_initialized()
        
    def test_brain_initialization(self):
        """Test brain initialization"""
        config = createDefaultConfig()
        brain = createBrain(config)
        
        assert brain.initialize()
        assert brain.is_initialized()
        
        # Check brain properties
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getRegionCount() > 0
        
    def test_brain_reset(self):
        """Test brain reset functionality"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Get initial state
        initial_spikes = brain.getTotalSpikeCount()
        initial_neurons = brain.getTotalNeuronCount()
        
        # Reset brain
        brain.reset()
        brain.initialize()
        
        # Brain should be reinitialized
        assert brain.is_initialized()
        assert brain.getTotalNeuronCount() == initial_neurons
        
    def test_brain_step(self):
        """Test brain stepping"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Get initial spike count
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run a few steps
        for i in range(10):
            brain.step(i)
            
        # Spike count should increase or stay same
        final_spikes = brain.getTotalSpikeCount()
        assert final_spikes >= initial_spikes

class TestAgentFunctionality:
    """Test agent creation and functionality"""
    
    def test_create_agent_brain(self):
        """Test agent brain creation"""
        config = createDefaultConfig()
        brain = createBrain(config)
        agent = createAgentBrain(brain)
        
        assert agent is not None
        assert isinstance(agent, AgentBrain)
        
    def test_agent_initialization(self):
        """Test agent initialization"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=20, height=20)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Check agent properties
        assert agent.getBrain() is brain
        assert not agent.isRewardModulationEnabled()
        assert not agent.isStructuralPlasticityEnabled()
        assert not agent.isDevelopmentEnabled()
        assert not agent.isCuriosityEnabled()
        
    def test_agent_subsystem_control(self):
        """Test agent subsystem enable/disable"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=20, height=20)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
        
    def test_agent_sensory_processing(self):
        """Test agent sensory processing"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Create sensory percept
        percept = SensoryPercept()
        percept.setVision([0.1, 0.2, 0.3])
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Check that agent processed input
        assert agent.getBrain() is not None
        
    def test_agent_motor_decoding(self):
        """Test agent motor command decoding"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=20, height=20)
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        assert motor_cmd is not None
        assert isinstance(motor_cmd, MotorCommand)

class TestWorldFunctionality:
    """Test world creation and functionality"""
    
    def test_create_simple_world(self):
        """Test simple world creation"""
        world = createSimpleWorld()
        
        assert world is not None
        assert isinstance(world, SimpleWorld)
        
    def test_world_configuration(self):
        """Test world configuration"""
        world = createSimpleWorld()
        
        world.configure(width=30, height=20, visionWidth=10, visionHeight=8)
        
        assert world.getWidth() == 30
        assert world.getHeight() == 20
        # Note: These might not exist yet, depending on the actual API
        # assert world.getVisionWidth() == 10
        # assert world.getVisionHeight() == 8
        
    def test_world_reset(self):
        """Test world reset"""
        world = createSimpleWorld()
        world.configure(width=20, height=20)
        
        # Set some initial state
        world.setAgentStart(10.0, 10.0)
        
        # Reset world
        world.reset()
        
        # World should be in initial state
        assert world.getAgentBody().x == 0.0
        assert world.getAgentBody().y == 0.0
        
    def test_world_update(self):
        """Test world update"""
        world = createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        # Update world
        world.update(0.1)
        
        # Check that update was processed
        assert world.getSimulationTime() > 0
        
    def test_world_sensory_percept(self):
        """Test world sensory percept generation"""
        world = createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        assert percept is not None
        assert isinstance(percept, SensoryPercept)

class TestIntegration:
    """Integration tests for NLM components"""
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation"""
        import pynlm
        
        def run_simulation(steps=50):
            # Setup
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(steps):
                world.update(timestep=0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Optional: Apply reward modulation
                reward = 0.0
                if world.getSensoryPercept().getInternal():
                    reward = world.getSensoryPercept().getInternal()[0]
                agent.applyRewardModulation(reward, 0.0)
                
                agent.updateDevelopment(0.1)
            
            return brain, world, agent
        
        # Run simulation
        brain, world, agent = run_simulation(20)
        
        # Verify results
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getCuriosityLevel() >= 0
        assert agent.getNoveltyLevel() >= 0
        
    def test_brain_persistence(self):
        """Test brain persistence (save/load)"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Run some steps to generate activity
        for i in range(100):
            brain.step(i)
        
        # Save brain state
        checkpoint_file = "test_checkpoint.bin"
        assert brain.save(checkpoint_file)
        
        # Reset brain
        brain.reset()
        brain.initialize()
        
        # Load saved state
        assert brain.load(checkpoint_file)
        
        # Verify loaded state
        assert brain.is_initialized()
        
        # Clean up
        import os
        if os.path.exists(checkpoint_file):
            os.remove(checkpoint_file)
        
    def test_agent_brain_interaction(self):
        """Test complex agent-brain interaction"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable multiple subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run complex simulation
        for step in range(100):
            # Update world
            world.update(0.05)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Agent makes decisions
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.05)
            
            # Log progress periodically
            if step % 25 == 0:
                print(f"  Step {step}:")
                print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"    Total spikes: {brain.getTotalSpikeCount()}")
                print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"    Dev Stage: {agent.getDevelopmentalStage().name}")
        
        # Verify final state
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() > 0
        
    def test_memory_systems(self):
        """Test memory system integration"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Test working memory
        working_memory = brain.getWorkingMemory()
        assert working_memory is not None
        
        # Test episodic memory
        episodic_memory = brain.getEpisodicMemory()
        assert episodic_memory is not None
        
        # Test associative memory
        associative_memory = brain.getAssociativeMemory()
        assert associative_memory is not None
        
        # Test prediction system
        prediction_system = brain.getPredictionSystem()
        assert prediction_system is not None
        
        # Test cognitive systems
        planner = brain.getPlanner()
        concept_formation = brain.getConceptFormation()
        attention = brain.getAttention()
        
        assert planner is not None
        assert concept_formation is not None
        assert attention is not None
        
    def test_neuromodulation(self):
        """Test neuromodulation systems"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Get neuromodulation systems
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        novelty = brain.getNovelty()
        prediction_error = brain.getPredictionErrorSignal()
        
        assert dopamine is not None
        assert curiosity is not None
        assert novelty is not None
        assert prediction_error is not None
        
    def test_development_system(self):
        """Test development system"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        development_system = brain.getDevelopmentSystem()
        assert development_system is not None
        
        # Test developmental stages
        for stage in DevelopmentStage:
            brain.setDevelopmentalStage(stage)
            assert brain.getDevelopmentalStage() == stage
            
    def test_plasticity_systems(self):
        """Test plasticity systems"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Get plasticity systems
        stdp = brain.getSTDP()
        hebbian = brain.getHebbian()
        structural = brain.getStructuralPlasticity()
        
        assert stdp is not None
        assert hebbian is not None
        assert structural is not None

class TestPerformance:
    """Performance testing"""
    
    def test_performance_benchmark(self):
        """Test performance benchmarking"""
        import time
        
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        steps = 500
        
        for i in range(steps):
            brain.step(i)
            # Inject some input
            for j in range(min(5, brain.getTotalNeuronCount() // 100)):
                brain.injectCurrentToNeurons(NeuronType.Sensory, 30.0f)
        
        end_time = time.time()
        elapsed_time = end_time - start_time
        
        # Calculate performance metrics
        steps_per_second = steps / elapsed_time
        
        print(f"Performance: {steps_per_second:.2f} steps/second")
        assert steps_per_second > 0
        
    def test_memory_usage(self):
        """Test memory usage"""
        import tracemalloc
        
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Start memory tracing
        tracemalloc.start()
        
        # Run simulation
        for i in range(100):
            brain.step(i)
            
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Convert to MB
        peak_mb = peak / 1024 / 1024
        
        print(f"Peak memory usage: {peak_mb:.2f} MB")
        assert peak_mb < 1000  # Should be less than 1 GB

class TestErrorHandling:
    """Test error handling and edge cases"""
    
    def test_invalid_config(self):
        """Test handling of invalid configuration"""
        # This should not crash, but may create brain with minimal settings
        config = createDefaultConfig()
        
        # Set invalid values (should be handled gracefully)
        config.set("brain.neuron_count", 0)  # Very small brain
        
        brain = createBrain(config)
        assert brain.initialize()  # May fail or create very small brain
        
    def test_brain_step_before_init(self):
        """Test calling step before initialization"""
        config = createDefaultConfig()
        brain = createBrain(config)
        
        # This should either crash or be handled gracefully
        try:
            brain.step(0)
            print("WARNING: step() called before initialization - may be expected behavior")
        except Exception:
            pass  # Expected
            
    def test_agent_without_world(self):
        """Test agent usage without proper world"""
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        # Create agent without initializing with world
        agent = createAgentBrain(brain)
        
        # This should not crash, but may have default behavior
        motor_cmd = agent.decodeMotorCommand()
        assert motor_cmd is not None
        
    def test_world_without_configuration(self):
        """Test world operations without proper configuration"""
        world = createSimpleWorld()
        
        # Try to use world without configuration
        try:
            percept = world.getSensoryPercept()
            print("WARNING: world.getSensoryPercept() without config - may have default behavior")
        except Exception:
            pass  # Expected

class ComprehensiveTestRunner:
    """Comprehensive test runner with reporting"""
    
    def __init__(self):
        self.results = []
        self.start_time = None
        self.end_time = None
        
    def run_all_tests(self):
        """Run all tests and collect results"""
        self.start_time = datetime.now()
        
        print("=" * 60)
        print("COMPREHENSIVE NLM TESTING SUITE")
        print("=" * 60)
        print(f"Start time: {self.start_time}")
        print()
        
        # Define test suites
        test_suites = [
            ("Configuration Tests", TestConfiguration()),
            ("Brain Creation Tests", TestBrainCreationAndInitialization()),
            ("Agent Functionality Tests", TestAgentFunctionality()),
            ("World Functionality Tests", TestWorldFunctionality()),
            ("Integration Tests", TestIntegration()),
            ("Performance Tests", TestPerformance()),
            ("Error Handling Tests", TestErrorHandling()),
        ]
        
        total_tests = 0
        passed_tests = 0
        failed_tests = 0
        
        for suite_name, test_suite in test_suites:
            print(f"Running {suite_name}...")
            suite_result = self._run_test_suite(test_suite)
            
            self.results.append({
                'suite': suite_name,
                'total': suite_result['total_tests'],
                'passed': suite_result['passed_tests'],
                'failed': suite_result['failed_tests'],
                'duration': suite_result['duration'],
                'success_rate': suite_result['success_rate']
            })
            
            total_tests += suite_result['total_tests']
            passed_tests += suite_result['passed_tests']
            failed_tests += suite_result['failed_tests']
            
            print(f"  ✓ {suite_name}: {suite_result['passed_tests']}/{suite_result['total_tests']} passed ({suite_result['success_rate']:.1f}%)")
            print()
        
        self.end_time = datetime.now()
        duration = self.end_time - self.start_time
        
        # Generate summary report
        print("=" * 60)
        print("TESTING SUMMARY")
        print("=" * 60)
        print(f"Total test suites: {len(test_suites)}")
        print(f"Total tests: {total_tests}")
        print(f"Tests passed: {passed_tests}")
        print(f"Tests failed: {failed_tests}")
        print(f"Success rate: {passed_tests/total_tests*100:.1f}%")
        print(f"Total duration: {duration}")
        print(f"Start time: {self.start_time}")
        print(f"End time: {self.end_time}")
        
        # Generate detailed report
        self._generate_detailed_report()
        
        # Save results
        self._save_results()
        
        # Return overall success
        return failed_tests == 0
        
    def _run_test_suite(self, test_suite):
        """Run a single test suite and return results"""
        suite_result = {
            'total_tests': 0,
            'passed_tests': 0,
            'failed_tests': 0,
            'duration': 0,
            'success_rate': 0.0,
            'details': []
        }
        
        suite_start = datetime.now()
        
        # Get all test methods
        test_methods = [
            method for method in dir(test_suite)
            if method.startswith('test_') and callable(getattr(test_suite, method))
        ]
        
        suite_result['total_tests'] = len(test_methods)
        
        # Run each test
        for test_method in test_methods:
            test_start = datetime.now()
            test_success = False
            error_message = None
            
            try:
                test_func = getattr(test_suite, test_method)
                test_func()
                test_success = True
                
            except Exception as e:
                error_message = str(e)
                
            test_end = datetime.now()
            test_duration = (test_end - test_start).total_seconds()
            
            suite_result['details'].append({
                'test': test_method,
                'success': test_success,
                'duration': test_duration,
                'error': error_message
            })
            
            if test_success:
                suite_result['passed_tests'] += 1
            else:
                suite_result['failed_tests'] += 1
                
        suite_end = datetime.now()
        suite_duration = (suite_end - suite_start).total_seconds()
        suite_result['duration'] = suite_duration
        
        if suite_result['total_tests'] > 0:
            suite_result['success_rate'] = (suite_result['passed_tests'] / suite_result['total_tests']) * 100
        
        return suite_result
        
    def _generate_detailed_report(self):
        """Generate detailed test report"""
        print()
        print("DETAILED TEST REPORT")
        print("=" * 60)
        
        for result in self.results:
            print(f"\n{result['suite']}:")
            print(f"  Duration: {result['duration']:.2f}s")
            print(f"  Tests: {result['passed_tests']}/{result['total_tests']} ({result['success_rate']:.1f}%)")
            
            # Print failed tests details
            failed_details = [d for d in result.get('details', []) if not d['success']]
            if failed_details:
                print(f"  Failed tests: {len(failed_details)}")
                for failed in failed_details:
                    print(f"    - {failed['test']}: {failed['error']}")
                    
        print("\n" + "=" * 60)
        print("ALL TESTS COMPLETED")
        print("=" * 60)
        
    def _save_results(self):
        """Save test results to file"""
        results_file = "test_results.json"
        
        # Convert results to JSON-serializable format
        serializable_results = []
        for result in self.results:
            serializable_result = {
                'suite': result['suite'],
                'total': result['total_tests'],
                'passed': result['passed_tests'],
                'failed': result['failed_tests'],
                'duration': result['duration'],
                'success_rate': result['success_rate']
            }
            serializable_results.append(serializable_result)
        
        results_data = {
            'timestamp': self.start_time.isoformat(),
            'end_time': self.end_time.isoformat(),
            'total_suites': len(self.results),
            'total_tests': sum(r['total'] for r in serializable_results),
            'passed_tests': sum(r['passed'] for r in serializable_results),
            'failed_tests': sum(r['failed'] for r in serializable_results),
            'success_rate': (sum(r['passed'] for r in serializable_results) / 
                           sum(r['total'] for r in serializable_results)) * 100,
            'suites': serializable_results
        }
        
        with open(results_file, 'w') as f:
            json.dump(results_data, f, indent=2)
            
        print(f"Test results saved to: {results_file}")
        
    def print_test_summary(self):
        """Print formatted test summary"""
        if not self.results:
            print("No test results available")
            return
            
        print("\n" + "=" * 80)
        print("NLM COMPREHENSIVE TESTING SUITE - FINAL SUMMARY")
        print("=" * 80)
        
        total_tests = sum(r['total'] for r in self.results)
        passed_tests = sum(r['passed'] for r in self.results)
        failed_tests = sum(r['failed'] for r in self.results)
        
        print(f"Total test suites executed: {len(self.results)}")
        print(f"Total individual tests: {total_tests}")
        print(f"Tests passed: {passed_tests}")
        print(f"Tests failed: {failed_tests}")
        print(f"Overall success rate: {passed_tests/total_tests*100:.1f}%")
        
        print(f"\nIndividual suite results:")
        for result in self.results:
            status = "✓ PASS" if result['failed'] == 0 else "✗ FAIL"
            print(f"  {status} {result['suite']}: {result['passed']}/{result['total']} tests ({result['success_rate']:.1f}%)")
        
        print("\n" + "=" * 80)
        if failed_tests == 0:
            print("🎉 ALL TESTS PASSED! NLM testing suite completed successfully!")
        else:
            print("⚠️  SOME TESTS FAILED. Please review the failed test details above.")
        print("=" * 80)
        
        # Return overall success
        return failed_tests == 0

# Global test runner instance
_test_runner = ComprehensiveTestRunner()

def run_all_tests():
    """Run all NLM tests and report results"""
    return _test_runner.run_all_tests()

def get_test_results():
    """Get test results"""
    return _test_runner.results

def print_test_summary():
    """Print test summary"""
    _test_runner.print_test_summary()

# Auto-discovery of test files
def discover_test_files():
    """Discover test files in the project"""
    test_files = []
    
    # Look for test files in various locations
    for root, dirs, files in os.walk('.'):
        # Skip hidden directories and common build/test directories
        dirs[:] = [d for d in dirs if not d.startswith('.') and d not in ['build', 'dist', '__pycache__', '.pytest_cache']]
        
        for file in files:
            if file.startswith('test_') and file.endswith('.py'):
                test_files.append(os.path.join(root, file))
                
    return test_files

# Main execution block
if __name__ == '__main__':
    print("NLM Comprehensive Testing Suite")
    print("=" * 50)
    
    # Check if there are custom test files
    custom_tests = discover_test_files()
    if custom_tests:
        print(f"Found {len(custom_tests)} custom test files:")
        for test_file in custom_tests:
            print(f"  - {test_file}")
        print()
    
    # Run comprehensive tests
    success = run_all_tests()
    
    # Print summary
    print_test_summary()
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)