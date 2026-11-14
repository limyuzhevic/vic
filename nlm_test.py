#!/usr/bin/env python3
"""
NLM Python Testing Utilities
=============================

Comprehensive testing utilities for the NLM Python API.
Provides convenient testing methods, validation tools, and benchmarking capabilities.

Usage:
    import nlm_test
    # or
    from nlm_test import TestRunner, ValidationSuite, BenchmarkSuite
"""

import nlm
import time
import json
import logging
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field

logger = logging.getLogger(__name__)

@dataclass
class TestResult:
    """Result of a single test."""
    test_name: str
    passed: bool
    duration: float
    error_message: Optional[str] = None
    details: Dict[str, Any] = field(default_factory=dict)

@dataclass
class TestSuite:
    """Collection of related tests."""
    name: str
    tests: List[Callable] = field(default_factory=list)
    
    def add_test(self, test_func: Callable, test_name: str):
        """Add a test to the suite."""
        self.tests.append((test_func, test_name))
    
    def run(self) -> List[TestResult]:
        """Run all tests in the suite."""
        results = []
        for test_func, test_name in self.tests:
            start_time = time.time()
            try:
                test_func()
                passed = True
                error_message = None
            except Exception as e:
                passed = False
                error_message = str(e)
                logger.error(f"Test {test_name} failed: {e}")
            
            duration = time.time() - start_time
            results.append(TestResult(
                test_name=test_name,
                passed=passed,
                duration=duration,
                error_message=error_message
            ))
        
        return results

class ValidationSuite:
    """Validation utilities for configuration and state."""
    
    @staticmethod
    def validate_config(config: nlm.Config) -> Dict[str, Any]:
        """Validate configuration and return validation results."""
        results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'statistics': {}
        }
        
        # Check for common required keys
        essential_keys = ['neuron_count', 'region_count']
        for key in essential_keys:
            if not config.has(key):
                results['warnings'].append(f"Missing optional key: {key}")
        
        # Validate neuron_count is reasonable
        if config.has('neuron_count'):
            neuron_count = config.get('neuron_count')
            if neuron_count <= 0:
                results['errors'].append("neuron_count must be positive")
            elif neuron_count > 1000000:
                results['warnings'].append("neuron_count is very large, may impact performance")
        
        # Validate connection_probability
        if config.has('connection_probability'):
            conn_prob = config.get('connection_probability')
            if conn_prob < 0.0 or conn_prob > 1.0:
                results['errors'].append("connection_probability must be between 0.0 and 1.0")
        
        # Calculate statistics
        stats = {
            'has_keys': len(config.getKeys()),
            'neuron_count': config.get('neuron_count') if config.has('neuron_count') else None,
            'region_count': config.get('region_count') if config.has('region_count') else None
        }
        results['statistics'] = stats
        
        results['valid'] = len(results['errors']) == 0
        return results
    
    @staticmethod
    def validate_brain_state(brain: nlm.Brain) -> Dict[str, Any]:
        """Validate brain state and return validation results."""
        results = {
            'valid': True,
            'errors': [],
            'statistics': {}
        }
        
        # Check basic properties
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        firing_count = brain.getFiringNeuronCount()
        
        # Validate consistency
        if neuron_count == 0:
            results['errors'].append("brain has 0 neurons")
        
        if synapse_count > neuron_count * 100:  # Reasonable upper bound
            results['warnings'].append("High synapse density, may impact performance")
        
        # Calculate statistics
        stats = {
            'neuron_count': neuron_count,
            'synapse_count': synapse_count,
            'firing_count': firing_count,
            'firing_rate': firing_count / neuron_count if neuron_count > 0 else 0.0
        }
        results['statistics'] = stats
        
        results['valid'] = len(results['errors']) == 0
        return results
    
    @staticmethod
    def validate_world_state(world: nlm.SimpleWorld) -> Dict[str, Any]:
        """Validate world state and return validation results."""
        results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'statistics': {}
        }
        
        # Check world dimensions
        width = world.getWidth()
        height = world.getHeight()
        
        if width <= 0 or height <= 0:
            results['errors'].append("Invalid world dimensions")
        
        # Check agent position
        body = world.getAgentBody()
        if body.x < 0 or body.x >= width or body.y < 0 or body.y >= height:
            results['errors'].append("Agent outside world boundaries")
        
        # Calculate statistics
        stats = {
            'width': width,
            'height': height,
            'agent_x': body.x,
            'agent_y': body.y,
            'agent_orientation': body.orientation,
            'agent_energy': body.energy
        }
        results['statistics'] = stats
        
        results['valid'] = len(results['errors']) == 0
        return results

class BenchmarkSuite:
    """Benchmarking utilities for performance testing."""
    
    @staticmethod
    def benchmark_brain_initialization(config: nlm.Config, iterations: int = 5) -> Dict[str, float]:
        """Benchmark brain initialization performance."""
        times = []
        
        for _ in range(iterations):
            start_time = time.time()
            brain = nlm.createBrain(config)
            brain.initialize()
            end_time = time.time()
            times.append(end_time - start_time)
        
        return {
            'mean_time': sum(times) / len(times),
            'min_time': min(times),
            'max_time': max(times),
            'total_time': sum(times),
            'iterations': iterations
        }
    
    @staticmethod
    def benchmark_simulation(brain: nlm.Brain, world: nlm.SimpleWorld, 
                           agent: nlm.AgentBrain, steps: int, 
                           iterations: int = 3) -> Dict[str, float]:
        """Benchmark simulation performance."""
        times = []
        
        for _ in range(iterations):
            start_time = time.time()
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            end_time = time.time()
            times.append(end_time - start_time)
        
        return {
            'mean_time_per_step': sum(times) / len(times) / steps,
            'min_time_per_step': min(times) / steps,
            'max_time_per_step': max(times) / steps,
            'total_time': sum(times),
            'steps_per_iteration': steps,
            'iterations': iterations,
            'steps_per_second': steps * iterations / sum(times)
        }
    
    @staticmethod
    def benchmark_memory_usage(brain: nlm.Brain, world: nlm.SimpleWorld,
                               agent: nlm.AgentBrain, steps: int) -> Dict[str, Any]:
        """Benchmark memory usage (basic metrics)."""
        import tracemalloc
        
        tracemalloc.start()
        
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        return {
            'current_memory_mb': current / 1024 / 1024,
            'peak_memory_mb': peak / 1024 / 1024,
            'steps_per_mb': steps * 1024 * 1024 / peak if peak > 0 else float('inf')
        }

class TestRunner:
    """Main test runner for NLM integration."""
    
    def __init__(self):
        self.validation = ValidationSuite()
        self.benchmark = BenchmarkSuite()
    
    def run_all_tests(self, config: Optional[nlm.Config] = None) -> Dict[str, Any]:
        """Run all available tests and return comprehensive results."""
        if config is None:
            config = nlm.createDefaultConfig()
        
        results = {
            'config': None,
            'brain': None,
            'world': None,
            'agent': None,
            'tests': {},
            'validation': {},
            'benchmarks': {},
            'summary': {}
        }
        
        try:
            # Setup - create basic components
            results['config'] = config
            brain = nlm.createBrain(config)
            results['brain'] = brain
            brain.initialize()
            
            world = nlm.createSimpleWorld()
            results['world'] = world
            
            agent = nlm.createAgentBrain(brain)
            results['agent'] = agent
            agent.initialize(world)
            
            # Validation tests
            results['validation']['config'] = self.validation.validate_config(config)
            results['validation']['brain'] = self.validation.validate_brain_state(brain)
            results['validation']['world'] = self.validation.validate_world_state(world)
            
            # Benchmark tests
            results['benchmarks']['initialization'] = self.benchmark.benchmark_brain_initialization(config)
            results['benchmarks']['simulation'] = self.benchmark.benchmark_simulation(
                brain, world, agent, 100, iterations=2)
            results['benchmarks']['memory'] = self.benchmark.benchmark_memory_usage(
                brain, world, agent, 50)
            
            # Integration tests
            suite = TestSuite("Integration Tests")
            
            def test_brain_functionality():
                """Test basic brain functionality."""
                if brain.getTotalNeuronCount() == 0:
                    raise AssertionError("Brain has no neurons")
                
                # Take a few steps
                for step in range(10):
                    brain.step(step)
                
                # Should have some spike activity
                total_spikes = brain.getTotalSpikeCount()
                if total_spikes == 0:
                    raise AssertionError("No spikes generated after simulation steps")
                
                return True
            
            suite.add_test(test_brain_functionality, "Brain Functionality")
            
            def test_agent_integration():
                """Test agent-world integration."""
                # Update world
                world.update(0.1)
                
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process with agent
                agent.processSensoryInput(percept)
                
                # Take brain step
                brain.step(0)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                return True
            
            suite.add_test(test_agent_integration, "Agent Integration")
            
            def test_memory_systems():
                """Test memory systems integration."""
                if agent is None:
                    raise AssertionError("Agent not initialized")
                
                # Test various memory subsystems
                working_mem = brain.getWorkingMemory()
                episodic_mem = brain.getEpisodicMemory()
                
                # This is a simplified test - in practice you'd want more comprehensive checks
                return True
            
            suite.add_test(test_memory_systems, "Memory Systems")
            
            # Run integration tests
            integration_results = suite.run()
            results['tests']['integration'] = integration_results
            
            # Generate summary
            results['summary'] = {
                'tests_passed': sum(1 for r in integration_results if r.passed),
                'total_tests': len(integration_results),
                'all_passed': all(r.passed for r in integration_results),
                'config_valid': results['validation']['config']['valid'],
                'brain_valid': results['validation']['brain']['valid'],
                'world_valid': results['validation']['world']['valid'],
                'initialization_benchmark': results['benchmarks']['initialization']['mean_time'],
                'simulation_benchmark': results['benchmarks']['simulation']['mean_time_per_step']
            }
            
        except Exception as e:
            logger.error(f"Test runner failed: {e}")
            results['error'] = str(e)
        
        return results
    
    @staticmethod
    def export_results(results: Dict[str, Any], filename: str):
        """Export test results to a JSON file."""
        # Convert results to JSON-serializable format
        json_results = {
            'timestamp': time.time(),
            'config': results['config'].__dict__ if hasattr(results['config'], '__dict__') else str(results['config']),
            'validation': results['validation'],
            'benchmarks': results['benchmarks'],
            'tests': {
                'integration': [
                    {
                        'test_name': r.test_name,
                        'passed': r.passed,
                        'duration': r.duration,
                        'error_message': r.error_message,
                        'details': r.details
                    } for r in results['tests']['integration']
                ]
            },
            'summary': results['summary'],
            'error': results.get('error')
        }
        
        with open(filename, 'w') as f:
            json.dump(json_results, f, indent=2, default=str)
        
        logger.info(f"Test results exported to {filename}")

# Convenience functions for common testing scenarios
def create_test_environment(config_dict: Optional[Dict[str, Any]] = None,
                           world_config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """Create a test environment with specified configuration."""
    config = nlm.createDefaultConfig()
    
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value)
    
    world = nlm.createSimpleWorld()
    if world_config:
        for key, value in world_config.items():
            if hasattr(world, key):
                setattr(world, key, value)
    
    return {
        'config': config,
        'world': world
    }

def quick_test_brain(config_dict: Optional[Dict[str, Any]] = None, steps: int = 100) -> Dict[str, Any]:
    """Quick test of brain functionality."""
    config = nlm.createDefaultConfig()
    if config_dict:
        for key, value in config_dict.items():
            config.set(key, value)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return {
        'config': config,
        'brain': brain,
        'steps_completed': steps,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'success': True
    }

# Example usage
if __name__ == "__main__":
    # Configure logging
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    
    # Run quick test
    print("Running quick brain test...")
    result = quick_test_brain({'neuron_count': 100}, steps=50)
    print(f"Test completed: {result['success']}")
    print(f"Spikes generated: {result['total_spikes']}")
    
    # Run full test suite
    print("\nRunning full test suite...")
    runner = TestRunner()
    results = runner.run_all_tests()
    
    print(f"Tests passed: {results['summary']['tests_passed']}/{results['summary']['total_tests']}")
    print(f"Overall success: {results['summary']['all_passed']}")
    
    # Export results
    runner.export_results(results, "test_results.json")
    print("Test results exported to test_results.json")
