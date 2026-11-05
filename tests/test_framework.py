#!/usr/bin/env python3
"""
Comprehensive NLM Test Framework

This module provides a comprehensive test suite for the NLM (Neural Learning Machine)
project, including unit tests, integration tests, and performance benchmarks.
"""

import unittest
import time
import sys
from typing import Dict, List, Any, Optional

# Import NLM Python bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: NLM Python bindings not available - tests will be skipped")

from dataclasses import dataclass
from enum import Enum
import json

class TestStatus(Enum):
    PASSED = "passed"
    FAILED = "failed"
    SKIPPED = "skipped"
    ERROR = "error"

@dataclass
class TestResult:
    test_name: str
    status: TestStatus
    message: str
    duration: float
    details: Optional[Dict] = None

@dataclass
class TestSuiteResult:
    suite_name: str
    total_tests: int
    passed_tests: int
    failed_tests: int
    skipped_tests: int
    error_tests: int
    duration: float
    results: List[TestResult]
    metadata: Optional[Dict] = None

class NLMBrainTest(unittest.TestCase):
    """Base test class for NLM brain tests"""
    
    def setUp(self):
        """Set up test environment"""
        if not PYNLM_AVAILABLE:
            self.skipTest("NLM Python bindings not available")
        
        # Create a standard brain configuration for testing
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 100, pynlm.ConfigSource.Runtime)
        self.config.set("brain.synapse_density", 0.1f, pynlm.ConfigSource.Runtime)
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after tests"""
        self.brain = None
        self.config = None
    
    def run_test_with_timing(self, test_func, *args, **kwargs):
        """Run a test function with timing"""
        start_time = time.time()
        try:
            test_func(*args, **kwargs)
            return TestResult(
                test_name=test_func.__name__,
                status=TestStatus.PASSED,
                message="Test passed",
                duration=time.time() - start_time
            )
        except Exception as e:
            return TestResult(
                test_name=test_func.__name__,
                status=TestStatus.FAILED,
                message=str(e),
                duration=time.time() - start_time,
                details={"exception": str(e)}
            )
class BasicBrainFunctionalityTests(NLMBrainTest):
    """Test basic brain functionality"""
    
    def test_brain_initialization(self):
        """Test that brain initializes correctly"""
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
        self.assertEqual(self.brain.getFiringNeuronCount(), 0)
    
    def test_neuron_properties(self):
        """Test neuron properties and states"""
        # Get neurons (implementation would depend on actual API)
        pass
    
    def test_synapse_connectivity(self):
        """Test synaptic connectivity"""
        # Implementation would test connection patterns
        pass
class PlasticityTests(NLMBrainTest):
    """Test plasticity mechanisms"""
    
    def test_stdp_plasticity(self):
        """Test Spike-Timing Dependent Plasticity"""
        # Reset and record initial weights
        self.brain.reset()
        self.brain.initialize()
        
        # Apply correlated pre-post stimulation
        pass
    
    def test_hebbian_learning(self):
        """Test Hebbian learning"""
        pass
    
    def test_structural_plasticity(self):
        """Test structural plasticity (synaptogenesis/pruning)"""
        pass
class LearningTests(NLMBrainTest):
    """Test learning capabilities"""
    
    def test_experience_based_learning(self):
        """Test learning through experience"""
        # Run simulation with input patterns
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Apply sensory input over multiple steps
        for step in range(10):
            self.brain.step(step)
        
        # Check for learning indicators
        pass
    
    def test_memory_formation(self):
        """Test memory formation and consolidation"""
        pass
class NeuromodulationTests(NLMBrainTest):
    """Test neuromodulation systems"""
    
    def test_reward_modulation(self):
        """Test reward-based learning"""
        pass
    
    def test_curiosity_driven_exploration(self):
        """Test curiosity and exploration"""
        pass
    
    def test_novelty_detection(self):
        """Test novelty detection"""
        pass
class IntegrationTests(NLMBrainTest):
    """Integration tests combining multiple systems"""
    
    def test_brain_world_interaction(self):
        """Test interaction between brain and world"""
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        
        # Run a simulation cycle
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            self.brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Verify interaction
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
    
    def test_full_agent_lifecycle(self):
        """Test complete agent lifecycle"""
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 8, 8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            self.brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            if step % 10 == 0:
                print(f"Step {step}: Firing neurons: {self.brain.getFiringNeuronCount()}")
        
        # Check agent state
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
class PerformanceTests(NLMBrainTest):
    """Performance benchmarking tests"""
    
    def test_simulation_speed(self):
        """Test simulation speed for different brain sizes"""
        sizes = [50, 100, 200]
        times = []
        
        for size in sizes:
            # Create brain with specific size
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", size, pynlm.ConfigSource.Runtime)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time simulation
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            times.append(duration)
            
            self.assertLess(duration, size * 0.01)  # Reasonable time bound
        
        # Verify scaling
        print(f"Performance: {dict(zip(sizes, times))} seconds for 100 steps")
    
    def test_memory_usage(self):
        """Test memory usage patterns"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss
        
        # Run multiple brains
        brains = []
        for i in range(10):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 50, pynlm.ConfigSource.Runtime)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            brains.append(brain)
        
        final_memory = process.memory_info().rss
        memory_increase = (final_memory - initial_memory) / (1024 * 1024)  # MB
        
        print(f"Memory increase: {memory_increase:.2f} MB for 10 brains")
        
        # Clean up
        brains.clear()
class ConfigurationTests(unittest.TestCase):
    """Test configuration system"""
    
    def test_config_creation(self):
        """Test default configuration creation"""
        config = pynlm.createDefaultConfig()
        
        self.assertTrue(config.has("brain.neuron_count"))
        self.assertTrue(config.has("brain.synapse_density"))
        
        neuron_count = config.get("brain.neuron_count").value
        self.assertEqual(neuron_count, 1000)
    
    def test_config_modification(self):
        """Test configuration modification"""
        config = pynlm.createDefaultConfig()
        
        # Modify configuration
        config.set("brain.neuron_count", 500)
        config.set("plasticity.stdp.learning_rate", 0.005)
        
        # Verify changes
        self.assertEqual(config.get("brain.neuron_count").value, 500)
        self.assertEqual(config.get("plasticity.stdp.learning_rate").value, 0.005)
    
    def test_config_file_operations(self):
        """Test file operations"""
        import tempfile
        import os
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 750)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
        
        try:
            result = config.saveToFile(config_file)
            self.assertTrue(result)
            
            # Load from file
            config2 = pynlm.createDefaultConfig()
            result = config2.loadFromFile(config_file)
            self.assertTrue(result)
            
            # Verify loaded values
            self.assertEqual(config2.get("brain.neuron_count").value, 750)
        finally:
            os.unlink(config_file)
class AdvancedFunctionalityTests(unittest.TestCase):
    """Test advanced functionality for power users"""
    
    def test_custom_network_architecture(self):
        """Test creating custom network architectures"""
        # This would test creating brains with specific connectivity patterns
        pass
    
    def test_advanced_plasticity_scenarios(self):
        """Test advanced plasticity scenarios"""
        pass
    
    def test_emergent_behavior_analysis(self):
        """Test analysis of emergent behavior"""
        pass
    
    def test_reproducible_experiments(self):
        """Test experiment reproducibility"""
        pass
class NLMSimulationRunner:
    """High-level interface for running NLM simulations"""
    
    def __init__(self, config_file=None):
        self.config_file = config_file
        self.config = pynlm.createDefaultConfig()
        
        if config_file:
            self.config.loadFromFile(config_file)
    
    def create_brain(self, neuron_count=None):
        """Create a brain with optional custom size"""
        if neuron_count:
            self.config.set("brain.neuron_count", neuron_count, pynlm.ConfigSource.Runtime)
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        return brain
    
    def run_simple_simulation(self, steps, visualize=False):
        """Run a simple simulation"""
        brain = self.create_brain()
        
        results = {
            'steps': [],
            'firing_rates': [],
            'total_spikes': 0,
            'start_time': time.time()
        }
        
        for step in range(steps):
            brain.step(step)
            
            if visualize and step % 10 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
            
            results['steps'].append(step)
            results['firing_rates'].append(brain.getFiringNeuronCount())
            results['total_spikes'] = brain.getTotalSpikeCount()
        
        results['end_time'] = time.time()
        return results
    
    def run_agent_simulation(self, world_config, steps, enable_subsystems=None):
        """Run an agent simulation"""
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        # Create brain
        brain = self.create_brain()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems if specified
        if enable_subsystems:
            for subsystem, enabled in enable_subsystems.items():
                if hasattr(agent, f'enable{subsystem}'):
                    getattr(agent, f'enable{subsystem}')(enabled)
        
        # Run simulation
        results = {
            'steps': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'reward_sums': []
        }
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            results['steps'].append(step)
            results['firing_rates'].append(brain.getFiringNeuronCount())
            results['curiosity_levels'].append(agent.getCuriosityLevel())
            results['novelty_levels'].append(agent.getNoveltyLevel())
            results['reward_sums'].append(agent.getNeuromodulationLevel())
        
        return results, brain, world, agent

# Test suite runner
class NLMSimulationTestSuite:
    """Run comprehensive test suite for NLM"""
    
    def __init__(self):
        self.results = []
        self.start_time = time.time()
    
    def run_all_tests(self):
        """Run all available test suites"""
        test_classes = [
            BasicBrainFunctionalityTests,
            PlasticityTests,
            LearningTests,
            NeuromodulationTests,
            IntegrationTests,
            PerformanceTests,
            ConfigurationTests,
            AdvancedFunctionalityTests
        ]
        
        for test_class in test_classes:
            print(f"\nRunning {test_class.__name__}...")
            
            # Create test suite
            suite = unittest.TestLoader().loadTestsFromTestCase(test_class)
            
            # Run tests with timing
            runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
            result = runner.run(suite)
            
            # Record results
            self.results.append({
                'class': test_class.__name__,
                'tests_run': result.testsRun,
                'failures': len(result.failures),
                'errors': len(result.errors),
                'skipped': len(result.skipped) if hasattr(result, 'skipped') else 0
            })
    
    def get_summary(self):
        """Get test suite summary"""
        end_time = time.time()
        
        total_tests = sum(r['tests_run'] for r in self.results)
        passed_tests = sum(r['tests_run'] - r['failures'] - r['errors'] for r in self.results)
        failed_tests = sum(r['failures'] for r in self.results)
        error_tests = sum(r['errors'] for r in self.results)
        
        return {
            'suite_name': 'NLM Comprehensive Test Suite',
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': failed_tests,
            'error_tests': error_tests,
            'duration': end_time - self.start_time,
            'details': self.results
        }
# Convenience functions for common use cases
def run_quick_demo():
    """Run a quick demonstration of NLM"""
    print("=== NLM Quick Demo ===")
    
    # Create a small brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    
    # Run a few steps
    for i in range(20):
        brain.step(i)
    
    print(f"Total spikes after 20 steps: {brain.getTotalSpikeCount()}")
    print("Demo completed successfully!")
    
    return brain
def run_advanced_demo(neuron_count=200, steps=100):
    """Run an advanced demonstration with world interaction"""
    print(f"=== NLM Advanced Demo ({neuron_count} neurons, {steps} steps) ===")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count, pynlm.ConfigSource.Runtime)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Created agent in {world.getWidth()}x{world.getHeight()} world")
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    print(f"Advanced demo completed!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, world, agent
def save_experiment_results(filename, brain, world, agent, metadata=None):
    """Save experiment results to a file"""
    if metadata is None:
        metadata = {}
    
    results = {
        'metadata': metadata,
        'brain_stats': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        },
        'agent_stats': {
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel()
        },
        'world_stats': {
            'width': world.getWidth(),
            'height': world.getHeight()
        }
    }
    
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Experiment results saved to {filename}")
    return results
# Example usage and main execution block
if __name__ == "__main__":
    print("NLM Test Framework - Phase 2 Demo Improvements")
    print("=" * 50)
    
    if PYNLM_AVAILABLE:
        # Run quick demo
        brain = run_quick_demo()
        print()
        
        # Run advanced demo
        brain, world, agent = run_advanced_demo(neuron_count=100, steps=50)
        print()
        
        # Save results
        save_experiment_results("demo_results.json", brain, world, agent,
                               {'demo_type': 'advanced', 'neurons': 100, 'steps': 50})
    else:
        print("NLM Python bindings not available. Skipping demo.")
        print("Please ensure pynlm is properly installed and accessible.")

__all__ = [
    'NLMBrainTest',
    'BasicBrainFunctionalityTests',
    'PlasticityTests',
    'LearningTests',
    'NeuromodulationTests',
    'IntegrationTests',
    'PerformanceTests',
    'ConfigurationTests',
    'AdvancedFunctionalityTests',
    'NLMSimulationRunner',
    'NLMSimulationTestSuite',
    'TestResult',
    'TestSuiteResult',
    'run_quick_demo',
    'run_advanced_demo',
    'save_experiment_results'
]
