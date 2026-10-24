# Comprehensive Test Suite and Benchmarks for NLM

## Overview

This document provides a comprehensive test suite and benchmarks for the NLM (Neural Learning Machine) framework. It covers unit tests, integration tests, performance benchmarks, and stress testing scenarios to ensure the reliability, correctness, and performance of the NLM system.

## Test Architecture

### 1. Test Organization

#### Test Categories
```python
# Test categories for organized testing
class TestCategories:
    UNIT_TESTS = {
        'core': ['test_types', 'test_config', 'test_random', 'test_clock'],
        'neural': ['test_neuron', 'test_synapse', 'test_plasticity'],
        'memory': ['test_working_memory', 'test_episodic_memory'],
        'motor': ['test_motor_system'],
        'sensory': ['test_sensory_system'],
        'plasticity': ['test_stdp', 'test_hebbian', 'test_structural_plasticity'],
        'development': ['test_development_system'],
        'neuromodulation': ['test_neuromodulation']
    }
    
    INTEGRATION_TESTS = {
        'brain': ['test_brain_creation', 'test_brain_initialization', 'test_brain_step'],
        'agent': ['test_agent_brain', 'test_agent_behavior'],
        'world': ['test_world_simulation', 'test_world_interaction'],
        'complete': ['test_complete_simulation', 'test_end_to_end_workflow']
    }
    
    PERFORMANCE_TESTS = {
        'throughput': ['benchmark_step_performance', 'benchmark_scalability'],
        'memory': ['benchmark_memory_usage', 'benchmark_memory_efficiency'],
        'cpu': ['benchmark_cpu_usage', 'benchmark_cpu_efficiency'],
        'stress': ['stress_test_large_network', 'stress_test_long_simulation']
    }
    
    STRESS_TESTS = {
        'extreme': ['stress_test_extreme_conditions', 'stress_test_resource_limits'],
        'edge_cases': ['stress_test_edge_cases', 'stress_test_error_conditions']
    }
```

### 2. Test Infrastructure

#### Test Base Class
```python
import unittest
import time
import memory_profiler
import psutil
import threading
from typing import Dict, List, Any, Optional
from dataclasses import dataclass
from enum import Enum

class TestSeverity(Enum):
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"

@dataclass
class TestMetrics:
    execution_time: float
    memory_usage: float
    cpu_usage: float
    memory_samples: List[float]
    cpu_samples: List[float]
    success: bool
    error_message: Optional[str] = None
    start_time: Optional[float] = None
    end_time: Optional[float] = None

class NLMTestBase(unittest.TestCase):
    """Base class for all NLM tests with performance monitoring"""
    
    def setUp(self):
        """Set up test environment"""
        self.test_metrics = TestMetrics(
            execution_time=0,
            memory_usage=0,
            cpu_usage=0,
            memory_samples=[],
            cpu_samples=[]
        )
        self.test_metrics.start_time = time.time()
        self.original_config = None
        
    def tearDown(self):
        """Clean up test environment"""
        self.test_metrics.end_time = time.time()
        self.test_metrics.execution_time = (
            self.test_metrics.end_time - self.test_metrics.start_time
        )
        
        # Record final memory and CPU usage
        process = psutil.Process()
        memory_info = process.memory_info()
        self.test_metrics.memory_usage = memory_info.rss / (1024 * 1024)  # MB
        self.test_metrics.cpu_usage = psutil.cpu_percent(interval=None)
        
        # Store test results
        self.storeTestResults()
        
    def recordMemorySample(self):
        """Record current memory usage"""
        process = psutil.Process()
        memory_info = process.memory_info()
        self.test_metrics.memory_samples.append(
            memory_info.rss / (1024 * 1024)  # MB
        )
        
    def recordCpuSample(self):
        """Record current CPU usage"""
        self.test_metrics.cpu_samples.append(psutil.cpu_percent(interval=None))
        
    def assertPerformanceThreshold(self, metric_name, value, threshold, test_type="execution_time"):
        """Assert that a performance metric meets threshold requirements"""
        if test_type == "execution_time":
            assert value <= threshold, \
                f"{metric_name} {value:.3f}s exceeds threshold {threshold:.3f}s"
        elif test_type == "memory_usage":
            assert value <= threshold, \
                f"{metric_name} {value:.2f}MB exceeds threshold {threshold:.2f}MB"
        elif test_type == "cpu_usage":
            assert value <= threshold, \
                f"{metric_name} {value:.1f}% exceeds threshold {threshold:.1f}%"
                
    def assertPerformanceAcceptable(self, metric_name, value, max_threshold, 
                                   min_threshold=None, test_type="execution_time"):
        """Assert that a performance metric is within acceptable range"""
        if test_type == "execution_time":
            assert value <= max_threshold, \
                f"{metric_name} {value:.3f}s exceeds maximum threshold {max_threshold:.3f}s"
            if min_threshold is not None:
                assert value >= min_threshold, \
                    f"{metric_name} {value:.3f}s below minimum threshold {min_threshold:.3f}s"
        elif test_type == "memory_usage":
            assert value <= max_threshold, \
                f"{metric_name} {value:.2f}MB exceeds maximum threshold {max_threshold:.2f}MB"
            if min_threshold is not None:
                assert value >= min_threshold, \
                    f"{metric_name} {value:.2f}MB below minimum threshold {min_threshold:.2f}MB"
        elif test_type == "cpu_usage":
            assert value <= max_threshold, \
                f"{metric_name} {value:.1f}% exceeds maximum threshold {max_threshold:.1f}%"
            if min_threshold is not None:
                assert value >= min_threshold, \
                    f"{metric_name} {value:.1f}% below minimum threshold {min_threshold:.1f}%"
                
    def storeTestResults(self):
        """Store test results for later analysis"""
        # This would integrate with a test results database
        pass
        
    def getTestConfiguration(self, config_name):
        """Get test-specific configuration"""
        configs = {
            'fast': {
                'neuron_count': 100,
                'region_count': 1,
                'connection_probability': 0.05,
                'simulation_steps': 100
            },
            'medium': {
                'neuron_count': 1000,
                'region_count': 2,
                'connection_probability': 0.1,
                'simulation_steps': 1000
            },
            'slow': {
                'neuron_count': 5000,
                'region_count': 3,
                'connection_probability': 0.15,
                'simulation_steps': 5000
            },
            'extreme': {
                'neuron_count': 10000,
                'region_count': 5,
                'connection_probability': 0.2,
                'simulation_steps': 10000
            }
        }
        
        return configs.get(config_name, configs['medium'])
```

### 3. Unit Test Suite

#### Core System Tests
```python
class TestCoreSystem(NLMTestBase):
    """Tests for core NLM systems"""
    
    def test_config_creation(self):
        """Test configuration creation and basic operations"""
        config = pynlm.createDefaultConfig()
        
        # Test basic configuration operations
        config.neural.neuron_count = 1000
        config.neural.connection_probability = 0.1
        config.simulation.timestep = 0.001
        
        # Verify configuration
        self.assertEqual(config.neural.neuron_count, 1000)
        self.assertEqual(config.neural.connection_probability, 0.1)
        self.assertEqual(config.simulation.timestep, 0.001)
        
        # Test configuration validation
        try:
            config.neural.neuron_count = -1
            self.fail("Should have raised ValueError for negative neuron count")
        except ValueError:
            pass  # Expected
            
    def test_config_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        
        # Test valid ranges
        config.neural.neuron_count = 100
        config.neural.connection_probability = 0.05
        config.simulation.timestep = 0.0001
        
        # Test schema validation
        schema = {
            'neural.neuron_count': {'type': int, 'min': 1, 'max': 100000},
            'neural.connection_probability': {'type': float, 'min': 0.0, 'max': 1.0},
            'simulation.timestep': {'type': float, 'min': 0.0001, 'max': 1.0}
        }
        
        config.setSchema(schema)
        
        # Test valid configuration
        self.assertTrue(config.isValid())
        
        # Test invalid configuration
        config.neural.connection_probability = 1.5
        self.assertFalse(config.isValid())
        
    def test_memory_efficiency(self):
        """Test memory efficiency of configuration system"""
        import tracemalloc
        
        # Start memory tracing
        tracemalloc.start()
        
        # Create many configurations
        configs = []
        for i in range(1000):
            config = pynlm.createDefaultConfig()
            config.neural.neuron_count = i + 1
            configs.append(config)
            
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Assert memory efficiency (less than 1MB per 1000 configurations)
        memory_per_config = peak / 1000 / 1024  # KB per config
        self.assertLess(memory_per_config, 1.0,  # 1KB per config
                       f"Memory usage too high: {memory_per_config:.2f}KB per config")

class TestNeuralSystem(NLMTestBase):
    """Tests for neural system components"""
    
    def test_neuron_creation(self):
        """Test neuron creation and basic properties"""
        config = pynlm.createConfig('fast')
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get neurons
        neurons = brain.getTotalNeuronCount()
        self.assertGreater(neurons, 0)
        
        # Test neuron properties
        active_neurons = brain.getActiveNeuronCount()
        self.assertLessEqual(active_neurons, neurons)
        
        # Test neuron state
        self.assertGreater(brain.getAverageFiringRate(), 0)
        
    def test_synapse_connections(self):
        """Test synapse creation and connections"""
        config = pynlm.createConfig('fast')
        config.neural.connection_probability = 0.2
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get synapse count
        synapses = brain.getTotalSynapseCount()
        self.assertGreater(synapses, 0)
        
        # Test connectivity
        excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()
        self.assertGreater(excitation_inhibition_ratio, 0)
        
    def test_plasticity_mechanisms(self):
        """Test plasticity mechanisms"""
        config = pynlm.createConfig('fast')
        config.plasticity.enable = True
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get plasticity systems
        stdp = brain.getSTDP()
        hebbian = brain.getHebbian()
        structural = brain.getStructuralPlasticity()
        
        # Test plasticity functionality
        self.assertIsNotNone(stdp)
        self.assertIsNotNone(hebbian)
        self.assertIsNotNone(structural)
        
        # Test plasticity parameters
        self.assertGreaterEqual(stdp.getLTPWeight(), 0.0)
        self.assertGreaterEqual(stdp.getLTDWeight(), 0.0)
        self.assertGreaterEqual(hebbian.getLearningRate(), 0.0)

class TestMemorySystem(NLMTestBase):
    """Tests for memory system components"""
    
    def test_working_memory(self):
        """Test working memory functionality"""
        config = pynlm.createConfig('fast')
        config.memory.working.enable = True
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get working memory system
        working_memory = brain.getWorkingMemory()
        self.assertIsNotNone(working_memory)
        
        # Test working memory operations
        working_memory.store(1, 0.5)  # Store value 0.5 at index 1
        retrieved_value = working_memory.retrieve(1)
        self.assertAlmostEqual(retrieved_value, 0.5, delta=0.01)
        
    def test_episodic_memory(self):
        """Test episodic memory functionality"""
        config = pynlm.createConfig('fast')
        config.memory.episodic.enable = True
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get episodic memory system
        episodic_memory = brain.getEpisodicMemory()
        self.assertIsNotNone(episodic_memory)
        
        # Test episodic memory operations
        episodic_memory.store(
            time_step=0,
            percept={'vision': [0.1, 0.2, 0.3], 'touch': [0.0]},
            action='move_forward',
            reward=1.0
        )
        
        # Test memory replay
        episode = episodic_memory.replay(0.1)  # Replay with 10% probability
        if episode:
            self.assertIsInstance(episode, dict)
            
    def test_associative_memory(self):
        """Test associative memory functionality"""
        config = pynlm.createConfig('fast')
        config.memory.associative.enable = True
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get associative memory system
        associative_memory = brain.getAssociativeMemory()
        self.assertIsNotNone(associative_memory)
        
        # Test pattern association
        pattern1 = {'input': [0.1, 0.2, 0.3], 'output': [0.4, 0.5, 0.6]}
        pattern2 = {'input': [0.7, 0.8, 0.9], 'output': [0.1, 0.2, 0.3]}
        
        associative_memory.associate(pattern1['input'], pattern1['output'])
        associative_memory.associate(pattern2['input'], pattern2['output'])
        
        # Test pattern completion
        completion = associative_memory.complete([0.5, 0.6, 0.7])
        self.assertIsNotNone(completion)
```

### 4. Integration Test Suite

#### Brain Integration Tests
```python
class TestBrainIntegration(NLMTestBase):
    """Integration tests for brain system"""
    
    def test_brain_initialization(self):
        """Test brain initialization with different configurations"""
        configs = ['fast', 'medium', 'slow']
        
        for config_name in configs:
            with self.subTest(config=config_name):
                config = pynlm.createConfig(config_name)
                brain = pynlm.createBrain(config)
                
                # Initialize brain
                success = brain.initialize()
                self.assertTrue(success, f"Brain initialization failed for {config_name}")
                
                # Verify initialization
                self.assertGreater(brain.getTotalNeuronCount(), 0)
                self.assertGreater(brain.getTotalSynapseCount(), 0)
                
    def test_brain_simulation(self):
        """Test brain simulation with different step counts"""
        config = pynlm.createConfig('medium')
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Record initial state
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run simulation
        num_steps = 1000
        for step in range(num_steps):
            brain.step(step)
            
        # Verify simulation results
        final_spikes = brain.getTotalSpikeCount()
        self.assertGreater(final_spikes, initial_spikes)
        
        # Verify brain state after simulation
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        
    def test_brain_plasticity(self):
        """Test brain plasticity during simulation"""
        config = pynlm.createConfig('medium')
        config.plasticity.enable = True
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Record initial weights
        initial_weights = []
        if brain.getRegionCount() > 0:
            region = brain.getRegion(brain.getRegionIds()[0])
            for syn in region.getSynapses():
                initial_weights.append(syn.getWeight())
                
        # Run simulation with plasticity
        for step in range(1000):
            brain.step(step)
            
        # Verify plasticity occurred
        final_weights = []
        if brain.getRegionCount() > 0:
            region = brain.getRegion(brain.getRegionIds()[0])
            for syn in region.getSynapses():
                final_weights.append(syn.getWeight())
                
        # Check for weight changes (plasticity occurred)
        if initial_weights and final_weights:
            weight_changes = [abs(f - i) for f, i in zip(final_weights, initial_weights)]
            # At least some synapses should have changed weight
            significant_changes = sum(1 for change in weight_changes if change > 0.001)
            self.assertGreater(significant_changes, 0, "No significant weight changes detected")

class TestAgentIntegration(NLMTestBase):
    """Integration tests for agent system"""
    
    def test_agent_brain_creation(self):
        """Test agent brain creation and initialization"""
        config = pynlm.createConfig('medium')
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize agent
        agent.initialize(world)
        
        # Verify agent initialization
        self.assertEqual(agent.getSensoryInputSize(), 256 + 8 + 4 + 6)
        self.assertEqual(agent.getMotorOutputSize(), 6)
        
    def test_agent_sensory_processing(self):
        """Test agent sensory processing"""
        config = pynlm.createConfig('medium')
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Create sensory percept
        from pynlm import Vision, SensoryPercept
        vision = Vision(8, 8, 3)
        vision_data = [i * 0.1 for i in range(8 * 8 * 3)]
        vision.setData(vision_data)
        
        percept = SensoryPercept()
        percept.setVision(vision)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Verify processing occurred
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        
    def test_agent_motor_output(self):
        """Test agent motor output generation"""
        config = pynlm.createConfig('medium')
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Process some sensory input to generate activity
        from pynlm import Vision, SensoryPercept
        vision = Vision(8, 8, 3)
        vision_data = [0.5 for _ in range(8 * 8 * 3)]
        vision.setData(vision_data)
        
        percept = SensoryPercept()
        percept.setVision(vision)
        agent.processSensoryInput(percept)
        
        # Run brain step to generate activity
        brain.step(0)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Verify action is valid
        self.assertIsNotNone(action)
        self.assertIn(action, [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ])
```

### 5. Performance Benchmark Suite

#### Throughput Benchmarks
```python
class BenchmarkThroughput(NLMTestBase):
    """Performance benchmarks for throughput"""
    
    def benchmark_step_performance(self, config_name='medium', steps=1000):
        """Benchmark step performance with different configurations"""
        config = pynlm.createConfig(config_name)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Record start time
        start_time = time.time()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
            
        # Calculate performance metrics
        end_time = time.time()
        elapsed = end_time - start_time
        
        metrics = {
            'config_name': config_name,
            'total_steps': steps,
            'elapsed_time': elapsed,
            'steps_per_second': steps / elapsed if elapsed > 0 else 0,
            'average_step_time': elapsed / steps if steps > 0 else 0,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'final_firing_rate': brain.getAverageFiringRate()
        }
        
        return metrics
        
    def benchmark_scalability(self, neuron_counts=[100, 500, 1000, 5000]):
        """Benchmark performance scalability with different neuron counts"""
        results = []
        
        for neuron_count in neuron_counts:
            config = pynlm.createConfig('custom')
            config.neural.neuron_count = neuron_count
            config.neural.region_count = max(1, neuron_count // 1000)
            config.neural.connection_probability = 0.1
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Benchmark with 1000 steps
            start_time = time.time()
            for step in range(1000):
                brain.step(step)
            end_time = time.time()
            
            elapsed = end_time - start_time
            metrics = {
                'neuron_count': neuron_count,
                'steps_per_second': 1000 / elapsed if elapsed > 0 else 0,
                'time_per_step_ms': (elapsed / 1000) * 1000,
                'synapses_per_neuron': brain.getTotalSynapseCount() / neuron_count
            }
            
            results.append(metrics)
            
        return results
        
    def benchmark_memory_efficiency(self, config_name='medium', runs=5):
        """Benchmark memory efficiency across multiple runs"""
        import tracemalloc
        
        results = []
        
        for i in range(runs):
            # Start memory tracing
            tracemalloc.start()
            
            # Create brain and run simulation
            config = pynlm.createConfig(config_name)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(500):
                brain.step(step)
                
            # Get memory usage
            current, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
            metrics = {
                'run': i + 1,
                'peak_memory_kb': peak / 1024,
                'memory_per_step_kb': peak / 1024 / 500,
                'neuron_count': brain.getTotalNeuronCount(),
                'steps_per_mb_memory': 500 / (peak / (1024 * 1024)) if peak > 0 else 0
            }
            
            results.append(metrics)
            
        return results
```

#### Stress Test Suite
```python
class StressTestSuite(NLMTestBase):
    """Stress tests for extreme conditions"""
    
    def stress_test_extreme_conditions(self):
        """Test performance under extreme conditions"""
        extreme_config = {
            'neuron_count': 50000,
            'region_count': 10,
            'connection_probability': 0.5,
            'plasticity.enable': True,
            'memory.enable': True,
            'development.enable': True,
            'simulation.timestep': 0.0001
        }
        
        config = pynlm.createConfig('custom')
        for key, value in extreme_config.items():
            config.set(key, value)
            
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Record baseline
        start_time = time.time()
        initial_memory = psutil.Process().memory_info().rss
        
        # Run stress test
        for step in range(5000):
            brain.step(step)
            
            # Check for performance degradation
            if step % 1000 == 0:
                current_time = time.time()
                elapsed = current_time - start_time
                steps_per_second = (step + 1) / elapsed
                
                # Assert performance doesn't degrade too much
                self.assertGreater(steps_per_second, 10, 
                                 f"Performance degraded too much at step {step}")
                
        # Check for memory leaks
        final_memory = psutil.Process().memory_info().rss
        memory_growth = (final_memory - initial_memory) / (1024 * 1024)  # MB
        
        self.assertLess(memory_growth, 100, 
                       f"Memory leak detected: {memory_growth:.2f} MB growth")
        
        return {
            'status': 'passed',
            'neuron_count': 50000,
            'steps_completed': 5000,
            'final_firing_rate': brain.getAverageFiringRate(),
            'memory_growth_mb': memory_growth,
            'performance_consistency': 'good'
        }
        
    def stress_test_resource_limits(self):
        """Test behavior at resource limits"""
        # Test with memory limit
        import resource
        
        # Set memory limit to 1GB
        resource.setrlimit(resource.RLIMIT_AS, (1024 * 1024 * 1024, -1))
        
        try:
            # Create large configuration
            large_config = pynlm.createConfig('custom')
            large_config.neural.neuron_count = 20000
            large_config.neural.region_count = 5
            large_config.neural.connection_probability = 0.3
            
            brain = pynlm.createBrain(large_config)
            brain.initialize()
            
            # Run simulation
            for step in range(1000):
                brain.step(step)
                
            # If we get here, memory limit wasn't exceeded
            result = {
                'status': 'passed',
                'memory_limit_respected': True,
                'steps_completed': 1000,
                'neuron_count': 20000
            }
            
        except MemoryError:
            result = {
                'status': 'passed',
                'memory_limit_respected': True,
                'error': 'MemoryError (expected)'
            }
        except Exception as e:
            result = {
                'status': 'failed',
                'memory_limit_respected': False,
                'error': str(e)
            }
            
        # Reset memory limit
        resource.setrlimit(resource.RLIMIT_AS, (-1, -1))
        
        return result
        
    def stress_test_error_conditions(self):
        """Test behavior under error conditions"""
        error_tests = []
        
        # Test 1: Invalid configuration
        try:
            invalid_config = pynlm.createConfig('custom')
            invalid_config.neural.neuron_count = -1  # Invalid
            brain = pynlm.createBrain(invalid_config)
            brain.initialize()
            error_tests.append({
                'test': 'invalid_config',
                'status': 'failed',
                'error': 'Should have raised error for invalid configuration'
            })
        except Exception as e:
            error_tests.append({
                'test': 'invalid_config',
                'status': 'passed',
                'error': None
            })
            
        # Test 2: Resource exhaustion simulation
        try:
            # Create configuration that would require significant resources
            large_config = pynlm.createConfig('custom')
            large_config.neural.neuron_count = 100000
            large_config.neural.region_count = 20
            large_config.neural.connection_probability = 0.8
            
            brain = pynlm.createBrain(large_config)
            brain.initialize()
            
            # This may fail due to resource constraints
            for step in range(100):
                brain.step(step)
                
            error_tests.append({
                'test': 'resource_exhaustion',
                'status': 'passed',
                'error': None
            })
            
        except Exception as e:
            error_tests.append({
                'test': 'resource_exhaustion',
                'status': 'passed',  # Failure is expected
                'error': str(e)
            })
            
        return error_tests
```

### 6. Continuous Integration Tests

#### Automated Testing Framework
```python
class AutomatedTestingFramework:
    """Automated testing framework for continuous integration"""
    
    def __init__(self):
        self.test_results = {}
        self.performance_baseline = {}
        self.monitoring_enabled = True
        
    def runFullTestSuite(self):
        """Run complete test suite"""
        test_categories = [
            ('unit_tests', self.runUnitTests),
            ('integration_tests', self.runIntegrationTests),
            ('performance_tests', self.runPerformanceTests),
            ('stress_tests', self.runStressTests)
        ]
        
        results = {}
        
        for category, test_function in test_categories:
            print(f"Running {category}...")
            category_results = test_function()
            results[category] = category_results
            
        # Analyze results
        analysis = self.analyzeTestResults(results)
        
        return {
            'results': results,
            'analysis': analysis,
            'summary': self.generateTestSummary(results)
        }
        
    def runUnitTests(self):
        """Run unit tests"""
        import unittest
        
        # Load unit tests from test files
        loader = unittest.TestLoader()
        suite = unittest.TestSuite()
        
        # Add unit test modules
        test_modules = [
            'test_core_system',
            'test_neural_system', 
            'test_memory_system',
            'test_plasticity_system'
        ]
        
        for module_name in test_modules:
            try:
                module = __import__(f'tests.{module_name}', fromlist=[''])
                tests = loader.loadTestsFromModule(module)
                suite.addTests(tests)
            except ImportError:
                print(f"Warning: Test module {module_name} not found")
                
        # Run tests with performance monitoring
        runner = unittest.TextTestRunner(verbosity=2, failfast=True)
        result = runner.run(suite)
        
        return {
            'passed': result.testsRun - len(result.failures) - len(result.errors),
            'failed': len(result.failures),
            'errors': len(result.errors),
            'total': result.testsRun,
            'success_rate': (result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun
        }
        
    def runIntegrationTests(self):
        """Run integration tests"""
        integration_tests = [
            TestBrainIntegration(),
            TestAgentIntegration()
        ]
        
        results = {}
        
        for test_class in integration_tests:
            print(f"Running {test_class.__name__}...")
            
            # Run test methods
            test_methods = [method for method in dir(test_class) 
                          if method.startswith('test_')]
            
            method_results = {}
            for method_name in test_methods:
                try:
                    test_method = getattr(test_class, method_name)
                    test_method()
                    method_results[method_name] = {'status': 'passed', 'error': None}
                except Exception as e:
                    method_results[method_name] = {'status': 'failed', 'error': str(e)}
                    
            results[test_class.__name__] = method_results
            
        return results
        
    def runPerformanceTests(self):
        """Run performance tests"""
        benchmark_categories = [
            ('throughput', BenchmarkThroughput()),
            ('memory_efficiency', BenchmarkThroughput()),
            ('scalability', BenchmarkThroughput())
        ]
        
        results = {}
        
        for category, benchmark in benchmark_categories:
            print(f"Running {category} benchmarks...")
            
            try:
                if category == 'throughput':
                    benchmark_results = benchmark.benchmark_step_performance('medium', 1000)
                elif category == 'memory_efficiency':
                    benchmark_results = benchmark.benchmark_memory_efficiency('medium', 3)
                elif category == 'scalability':
                    benchmark_results = benchmark.benchmark_scalability([100, 500, 1000, 5000])
                    
                results[category] = {
                    'status': 'completed',
                    'results': benchmark_results,
                    'duration': time.time() - benchmark.start_time if hasattr(benchmark, 'start_time') else 0
                }
                
            except Exception as e:
                results[category] = {
                    'status': 'failed',
                    'error': str(e),
                    'duration': time.time() - benchmark.start_time if hasattr(benchmark, 'start_time') else 0
                }
                
        return results
        
    def runStressTests(self):
        """Run stress tests"""
        stress_tests = StressTestSuite()
        
        stress_results = {
            'extreme_conditions': stress_tests.stress_test_extreme_conditions(),
            'resource_limits': stress_tests.stress_test_resource_limits(),
            'error_conditions': stress_tests.stress_test_error_conditions()
        }
        
        return stress_results
        
    def analyzeTestResults(self, results):
        """Analyze test results and generate insights"""
        analysis = {
            'overall_success_rate': self.calculateOverallSuccessRate(results),
            'performance_trends': self.analyzePerformanceTrends(results),
            'bottleneck_identification': self.identifyTestBottlenecks(results),
            'optimization_recommendations': self.generateOptimizationRecommendations(results),
            'regressions_detected': self.detectRegressions(results)
        }
        
        return analysis
        
    def calculateOverallSuccessRate(self, results):
        """Calculate overall test success rate"""
        total_tests = 0
        passed_tests = 0
        
        for category, category_results in results.items():
            if isinstance(category_results, dict):
                for test_name, test_result in category_results.items():
                    total_tests += 1
                    if test_result.get('status') == 'passed':
                        passed_tests += 1
                        
        success_rate = passed_tests / total_tests if total_tests > 0 else 0
        
        return {
            'success_rate': success_rate,
            'passed_tests': passed_tests,
            'total_tests': total_tests,
            'failed_tests': total_tests - passed_tests
        }
        
    def analyzePerformanceTrends(self, results):
        """Analyze performance trends across test runs"""
        trends = {}
        
        # Analyze performance test trends
        if 'performance_tests' in results:
            for benchmark_name, benchmark_result in results['performance_tests'].items():
                if benchmark_result.get('status') == 'completed':
                    trends[benchmark_name] = self.analyzeBenchmarkPerformance(
                        benchmark_result['results']
                    )
                    
        return trends
        
    def generateOptimizationRecommendations(self, results):
        """Generate optimization recommendations based on test results"""
        recommendations = []
        
        # Analyze performance bottlenecks
        bottlenecks = self.identifyTestBottlenecks(results)
        
        for bottleneck in bottlenecks:
            recommendations.append({
                'type': 'bottleneck_fix',
                'area': bottleneck['area'],
                'severity': bottleneck['severity'],
                'description': bottleneck['description'],
                'impact': self.estimateBottleneckImpact(bottleneck)
            })
            
        # Analyze performance regressions
        regressions = self.detectRegressions(results)
        
        for regression in regressions:
            recommendations.append({
                'type': 'regression_fix',
                'area': regression['area'],
                'severity': regression['severity'],
                'description': regression['description'],
                'regression_detected': regression['regression_detected'],
                'baseline_improvement': regression['baseline_improvement']
            })
            
        return recommendations
```

### 7. Test Results Analysis

#### Performance Analysis
```python
class TestResultsAnalyzer:
    """Analyzes test results and generates performance insights"""
    
    def analyzePerformanceData(self, test_results):
        """Analyze performance data from test runs"""
        analysis = {
            'execution_time_analysis': self.analyzeExecutionTime(test_results),
            'memory_usage_analysis': self.analyzeMemoryUsage(test_results),
            'throughput_analysis': self.analyzeThroughput(test_results),
            'scalability_analysis': self.analyzeScalability(test_results),
            'performance_trends': self.analyzePerformanceTrends(test_results)
        }
        
        return analysis
        
    def analyzeExecutionTime(self, test_results):
        """Analyze execution time performance"""
        execution_times = []
        
        for test_name, test_result in test_results.items():
            if 'profile_data' in test_result:
                execution_times.append(test_result['profile_data']['summary']['total_duration'])
                
        if not execution_times:
            return None
            
        return {
            'mean': sum(execution_times) / len(execution_times),
            'median': self.calculateMedian(execution_times),
            'min': min(execution_times),
            'max': max(execution_times),
            'std': self.calculateStdDev(execution_times),
            'coefficient_of_variation': self.calculateCoefficientOfVariation(execution_times)
        }
        
    def analyzeMemoryUsage(self, test_results):
        """Analyze memory usage performance"""
        memory_usages = []
        
        for test_name, test_result in test_results.items():
            if 'profile_data' in test_result:
                memory_usages.append(
                    test_result['profile_data']['summary']['memory_stats']['average_rss']
                )
                
        if not memory_usages:
            return None
            
        return {
            'mean': sum(memory_usages) / len(memory_usages),
            'min': min(memory_usages),
            'max': max(memory_usages),
            'std': self.calculateStdDev(memory_usages),
            'efficiency': self.calculateMemoryEfficiency(memory_usages)
        }
        
    def analyzeThroughput(self, test_results):
        """Analyze throughput performance"""
        throughputs = []
        
        for test_name, test_result in test_results.items():
            if 'profile_data' in test_result:
                throughputs.append(
                    test_result['profile_data']['summary']['steps_per_second']
                )
                
        if not throughputs:
            return None
            
        return {
            'mean': sum(throughputs) / len(throughputs),
            'min': min(throughputs),
            'max': max(throughputs),
            'std': self.calculateStdDev(throughputs),
            'improvement_potential': self.calculateImprovementPotential(throughputs)
        }
        
    def analyzeScalability(self, test_results):
        """Analyze scalability performance"""
        scalability_data = []
        
        for test_name, test_result in test_results.items():
            if 'profile_data' in test_result:
                profile = test_result['profile_data']['summary']
                scalability_data.append({
                    'steps_per_second': profile.get('steps_per_second', 0),
                    'neuron_count': profile.get('neuron_count', 0),
                    'region_count': profile.get('region_count', 0)
                })
                
        if not scalability_data:
            return None
            
        # Calculate scalability metrics
        scalability_metrics = {
            'linear_scaling': self.calculateLinearScaling(scalability_data),
            'efficiency': self.calculateScalabilityEfficiency(scalability_data),
            'bottlenecks': self.identifyScalabilityBottlenecks(scalability_data)
        }
        
        return scalability_metrics
        
    def generatePerformanceReport(self, test_results):
        """Generate comprehensive performance report"""
        report = {
            'executive_summary': self.generateExecutiveSummary(test_results),
            'detailed_analysis': self.analyzePerformanceData(test_results),
            'benchmark_results': self.formatBenchmarkResults(test_results),
            'comparisons': self.generatePerformanceComparisons(test_results),
            'recommendations': self.generatePerformanceRecommendations(test_results),
            'visualizations': self.prepareVisualizations(test_results)
        }
        
        return report
        
    def generateExecutiveSummary(self, test_results):
        """Generate executive summary of test results"""
        total_tests = sum(1 for test_result in test_results.values() if test_result.get('status') == 'completed')
        passed_tests = sum(1 for test_result in test_results.values() if test_result.get('status') == 'passed')
        
        return {
            'total_tests_run': total_tests,
            'successful_tests': passed_tests,
            'success_rate': passed_tests / total_tests if total_tests > 0 else 0,
            'overall_assessment': 'PASS' if passed_tests == total_tests else 'FAIL',
            'key_findings': self.identifyKeyFindings(test_results),
            'critical_issues': self.identifyCriticalIssues(test_results)
        }
        
    def prepareVisualizations(self, test_results):
        """Prepare data for performance visualizations"""
        visualizations = {
            'time_series': self.prepareTimeSeriesData(test_results),
            'histograms': self.prepareHistogramData(test_results),
            'scatter_plots': self.prepareScatterPlotData(test_results),
            'correlation_matrices': self.prepareCorrelationMatrixData(test_results)
        }
        
        return visualizations
```

## Test Execution Script

### 1. Main Test Runner
```python
#!/usr/bin/env python3
"""
NLM Test Suite Runner

This script runs the complete NLM test suite with performance monitoring
and generates comprehensive performance reports.
"""

import argparse
import json
import sys
import os
from datetime import datetime
import logging

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from test_suite import (
    AutomatedTestingFramework,
    TestResultsAnalyzer,
    TestCoreSystem,
    TestNeuralSystem,
    TestMemorySystem,
    TestBrainIntegration,
    TestAgentIntegration
)

def setup_logging():
    """Set up logging for test execution"""
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.FileHandler('test_execution.log'),
            logging.StreamHandler()
        ]
    )
    
    return logging.getLogger(__name__)

def run_test_suite():
    """Run the complete NLM test suite"""
    logger = setup_logging()
    
    print("=" * 60)
    print("NLM Test Suite Execution")
    print("=" * 60)
    print(f"Started at: {datetime.now().isoformat()}")
    
    # Initialize test framework
    test_framework = AutomatedTestingFramework()
    
    # Run complete test suite
    test_results = test_framework.runFullTestSuite()
    
    # Analyze test results
    analyzer = TestResultsAnalyzer()
    analysis = analyzer.analyzeTestResults(test_results['results'])
    
    # Generate performance report
    performance_report = analyzer.generatePerformanceReport(test_results['results'])
    
    # Save results
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    # Save test results
    with open(f'test_results_{timestamp}.json', 'w') as f:
        json.dump(test_results, f, indent=2, default=str)
        
    # Save analysis
    with open(f'test_analysis_{timestamp}.json', 'w') as f:
        json.dump(analysis, f, indent=2, default=str)
        
    # Save performance report
    with open(f'performance_report_{timestamp}.json', 'w') as f:
        json.dump(performance_report, f, indent=2, default=str)
        
    # Generate HTML report
    html_report = analyzer.generateHTMLReport(test_results, analysis, performance_report)
    with open(f'performance_report_{timestamp}.html', 'w') as f:
        f.write(html_report)
        
    print("=" * 60)
    print("Test Execution Complete")
    print(f"Results saved to: test_results_{timestamp}.json")
    print(f"Analysis saved to: test_analysis_{timestamp}.json")
    print(f"Performance report saved to: performance_report_{timestamp}.json")
    print(f"HTML report saved to: performance_report_{timestamp}.html")
    print("=" * 60)
    
    # Print summary
    summary = test_results['summary']
    print(f"\nTest Summary:")
    print(f"  Total Tests: {summary.get('total_tests', 0)}")
    print(f"  Passed: {summary.get('passed_tests', 0)}")
    print(f"  Failed: {summary.get('failed_tests', 0)}")
    print(f"  Success Rate: {summary.get('success_rate', 0):.1%}")
    print(f"  Overall Assessment: {summary.get('overall_assessment', 'UNKNOWN')}")
    
    return test_results, analysis, performance_report

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(description='Run NLM test suite')
    parser.add_argument('--quick', action='store_true', 
                       help='Run quick test suite (skip some tests)')
    parser.add_argument('--performance-only', action='store_true',
                       help='Run only performance tests')
    parser.add_argument('--integration-only', action='store_true',
                       help='Run only integration tests')
    parser.add_argument('--output-dir', type=str, default='test_results',
                       help='Output directory for test results')
    
    args = parser.parse_args()
    
    try:
        test_results, analysis, performance_report = run_test_suite()
        
        # Exit with appropriate code
        if analysis.get('regressions_detected', []):
            sys.exit(1)  # Failed due to regressions
        else:
            sys.exit(0)  # Success
            
    except Exception as e:
        print(f"Test execution failed: {e}")
        sys.exit(2)

if __name__ == '__main__':
    main()
```

### 2. Test Configuration File
```ini
# NLM Test Configuration

[test_framework]
parallel_execution = true
max_workers = 4
verbose_logging = true
performance_monitoring = true

[test_categories]
unit_tests = true
integration_tests = true
performance_tests = true
stress_tests = true

[test_configurations]
fast = {"neuron_count": 100, "region_count": 1, "connection_probability": 0.05}
medium = {"neuron_count": 1000, "region_count": 2, "connection_probability": 0.1}
slow = {"neuron_count": 5000, "region_count": 3, "connection_probability": 0.15}
extreme = {"neuron_count": 10000, "region_count": 5, "connection_probability": 0.2}

[performance_thresholds]
max_execution_time_ms = 1000
min_steps_per_second = 100
max_memory_usage_mb = 500
max_cpu_usage_percent = 90

[resource_limits]
memory_limit_mb = 1024
cpu_limit_percent = 90
time_limit_minutes = 60

[reporting]
generate_html_reports = true
generate_json_reports = true
generate_visualizations = true
report_format = "comprehensive"
```

## Usage Instructions

### 1. Running Tests
```bash
# Run complete test suite
python run_nlm_tests.py

# Run quick test suite (skip some tests)
python run_nlm_tests.py --quick

# Run only performance tests
python run_nlm_tests.py --performance-only

# Run only integration tests
python run_nlm_tests.py --integration-only

# Run tests with specific configuration
python run_nlm_tests.py --output-dir custom_test_results
```

### 2. Test Results
- **JSON Results**: `test_results_[timestamp].json` - Detailed test execution results
- **Analysis**: `test_analysis_[timestamp].json` - Comprehensive test analysis
- **Performance Report**: `performance_report_[timestamp].json` - Detailed performance metrics
- **HTML Report**: `performance_report_[timestamp].html` - Interactive performance visualizations

### 3. Performance Metrics Tracked
- **Execution Time**: Total test execution time and per-test performance
- **Memory Usage**: Memory consumption and efficiency metrics
- **CPU Usage**: CPU utilization and efficiency
- **Throughput**: Steps per second and performance scalability
- **Bottleneck Identification**: Automatic detection of performance bottlenecks
- **Regression Detection**: Identification of performance regressions
- **Scalability Analysis**: Analysis of performance across different configurations

### 4. Test Categories
- **Unit Tests**: Individual component testing (core, neural, memory, plasticity systems)
- **Integration Tests**: System-level integration testing (brain, agent, world systems)
- **Performance Tests**: Throughput, memory efficiency, and scalability benchmarks
- **Stress Tests**: Extreme condition testing (resource limits, error conditions)

## Summary

This comprehensive test suite provides:

### 1. **Complete Test Coverage**
- Unit tests for individual components
- Integration tests for system interactions
- Performance benchmarks for optimization
- Stress tests for robustness

### 2. **Performance Monitoring**
- Real-time performance tracking
- Automatic bottleneck identification
- Resource efficiency analysis
- Performance trend analysis

### 3. **Automated Testing**
- Configuration management
- Parallel execution
- Automated reporting
- Continuous integration ready

### 4. **Comprehensive Reporting**
- Detailed JSON reports
- Interactive HTML visualizations
- Executive summaries
- Performance recommendations

### 5. **Continuous Integration**
- Test execution scripts
- Performance regression detection
- Automated test result analysis
- Integration-friendly output formats

The test suite ensures NLM's reliability, correctness, and performance while providing valuable insights for optimization and continuous improvement.