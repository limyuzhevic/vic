#!/usr/bin/env python3
"""
NLM Testing and Validation Framework

This module provides comprehensive testing and validation capabilities for the NLM
(Neural Learning Machine) framework. It includes unit tests, integration tests,
performance benchmarks, and validation suites to ensure the correctness and
reliability of NLM functionality.

Author: NLM Development Team
License: MIT
"""

import sys
import os
import time
import math
import json
import tempfile
import traceback
from typing import Dict, List, Any, Optional, Callable, Tuple, Union
from dataclasses import dataclass, asdict
from enum import Enum
import unittest
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
@dataclass
class TestResult:
    """Test result data structure."""
    test_name: str
    test_type: str
    status: str  # "passed", "failed", "error", "skipped"
    message: str = ""
    duration: float = 0.0
    details: Dict[str, Any] = None
    timestamp: float = None
    
    def __post_init__(self):
        if self.details is None:
            self.details = {}
        if self.timestamp is None:
            self.timestamp = time.time()
class TestSuite:
    """
    Comprehensive test suite for NLM functionality.
    
    Provides organized test execution, result collection, and reporting
    for both unit tests and integration tests.
    """
    
    def __init__(self, name: str = "NLM Test Suite"):
        self.name = name
        self.results: List[TestResult] = []
        self.setup_hooks: List[Callable] = []
        self.teardown_hooks: List[Callable] = []
        self.test_cases: List[Tuple[str, Callable]] = []
    
    def add_setup_hook(self, hook: Callable):
        """Add a setup hook that runs before each test."""
        self.setup_hooks.append(hook)
    
    def add_teardown_hook(self, hook: Callable):
        """Add a teardown hook that runs after each test."""
        self.teardown_hooks.append(hook)
    
    def add_test(self, name: str, test_func: Callable, test_type: str = "unit"):
        """Add a test case to the suite."""
        self.test_cases.append((name, test_func, test_type))
    
    def run_test(self, test_name: str, test_func: Callable, test_type: str) -> TestResult:
        """Run a single test and return the result."""
        start_time = time.time()
        result = TestResult(
            test_name=test_name,
            test_type=test_type,
            status="error",
            message="Test execution failed"
        )
        
        try:
            # Run setup hooks
            for hook in self.setup_hooks:
                hook()
            
            # Execute the test
            test_func()
            
            # If we get here, the test passed
            result.status = "passed"
            result.message = "Test completed successfully"
            
        except AssertionError as e:
            result.status = "failed"
            result.message = f"Assertion failed: {str(e)}"
        except Exception as e:
            result.status = "error"
            result.message = f"Unexpected error: {str(e)}"
            result.details = {"traceback": traceback.format_exc()}
        finally:
            # Run teardown hooks
            for hook in self.teardown_hooks:
                try:
                    hook()
                except Exception as e:
                    if result.status == "passed":
                        result.status = "error"
                        result.message += f"; Teardown error: {str(e)}"
        
        # Calculate duration
        result.duration = time.time() - start_time
        
        return result
    
    def run_all_tests(self) -> List[TestResult]:
        """Run all tests in the suite."""
        print(f"Running test suite: {self.name}")
        print("=" * 60)
        
        for test_name, test_func, test_type in self.test_cases:
            print(f"Running test: {test_name} ({test_type})...")
            result = self.run_test(test_name, test_func, test_type)
            self.results.append(result)
            
            # Print result
            status_icon = {
                "passed": "✓",
                "failed": "✗", 
                "error": "✗",
                "skipped": "-"
            }.get(result.status, "?")
            
            print(f"  {status_icon} {result.status.upper()}")
            if result.message:
                print(f"    Message: {result.message}")
            print(f"    Duration: {result.duration:.3f}s")
            print()
        
        return self.results
    
    def get_summary(self) -> Dict[str, Any]:
        """Get test suite summary statistics."""
        total = len(self.results)
        if total == 0:
            return {"total": 0}
        
        passed = sum(1 for r in self.results if r.status == "passed")
        failed = sum(1 for r in self.results if r.status == "failed")
        errors = sum(1 for r in self.results if r.status == "error")
        skipped = sum(1 for r in self.results if r.status == "skipped")
        
        total_duration = sum(r.duration for r in self.results)
        
        return {
            "total": total,
            "passed": passed,
            "failed": failed,
            "errors": errors,
            "skipped": skipped,
            "success_rate": (passed / total) * 100 if total > 0 else 0,
            "total_duration": total_duration,
            "average_duration": total_duration / total if total > 0 else 0
        }
class BrainValidator:
    """
    Specialized validator for NLM brain functionality.
    
    Provides comprehensive validation of brain components, including
    neural properties, plasticity mechanisms, and simulation correctness.
    """
    
    def __init__(self):
        self.validation_results = []
    
    def validate_basic_brain_creation(self) -> TestResult:
        """Validate basic brain creation and initialization."""
        start_time = time.time()
        
        try:
            # Create brain with default configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            # Initialize brain
            brain.initialize()
            
            # Validate basic properties
            assert brain.getTotalNeuronCount() > 0, "Brain should have neurons"
            assert brain.getTotalSynapseCount() > 0, "Brain should have synapses"
            assert brain.getDevelopmentalStage() != pynlm.DevelopmentalStage.Aging, "Brain should not be in aging stage initially"
            
            duration = time.time() - start_time
            
            return TestResult(
                test_name="Basic Brain Creation",
                test_type="validation",
                status="passed",
                duration=duration,
                details={
                    "neuron_count": brain.getTotalNeuronCount(),
                    "synapse_count": brain.getTotalSynapseCount(),
                    "development_stage": str(brain.getDevelopmentalStage())
                }
            )
            
        except Exception as e:
            duration = time.time() - start_time
            return TestResult(
                test_name="Basic Brain Creation",
                test_type="validation",
                status="failed",
                message=str(e),
                duration=duration
            )
    
    def validate_simulation_correctness(self) -> TestResult:
        """Validate simulation correctness and consistency."""
        start_time = time.time()
        
        try:
            # Create and initialize brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Get initial state
            initial_spikes = brain.getTotalSpikeCount()
            initial_firing = brain.getFiringNeuronCount()
            initial_neurons = brain.getTotalNeuronCount()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
                
                # Validate invariants
                current_spikes = brain.getTotalSpikeCount()
                current_firing = brain.getFiringNeuronCount()
                
                assert current_spikes >= initial_spikes, "Spike count should not decrease"
                assert current_firing >= 0 and current_firing <= initial_neurons, "Firing neurons should be within valid range"
            
            duration = time.time() - start_time
            
            return TestResult(
                test_name="Simulation Correctness",
                test_type="validation",
                status="passed",
                duration=duration,
                details={
                    "initial_spikes": initial_spikes,
                    "final_spikes": brain.getTotalSpikeCount(),
                    "initial_firing": initial_firing,
                    "final_firing": brain.getFiringNeuronCount(),
                    "steps_ran": 100
                }
            )
            
        except Exception as e:
            duration = time.time() - start_time
            return TestResult(
                test_name="Simulation Correctness",
                test_type="validation",
                status="failed",
                message=str(e),
                duration=duration
            )
    
    def validate_plasticity_mechanisms(self) -> TestResult:
        """Validate plasticity mechanisms (STDP, Hebbian, structural)."""
        start_time = time.time()
        
        try:
            # Create brain with plasticity enabled
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Get initial synapse weights
            initial_weights = []
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    for synapse in region.getSynapses():
                        initial_weights.append(synapse.getWeight())
            
            initial_avg_weight = sum(initial_weights) / len(initial_weights) if initial_weights else 0
            
            # Run plasticity experiment
            for step in range(500):
                # Inject correlated activity to trigger plasticity
                for i in range(5):
                    brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 30.0f)
                
                brain.step(step)
            
            # Get final synapse weights
            final_weights = []
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    for synapse in region.getSynapses():
                        final_weights.append(synapse.getWeight())
            
            final_avg_weight = sum(final_weights) / len(final_weights) if final_weights else 0
            
            weight_change = final_avg_weight - initial_avg_weight
            
            # Validate plasticity occurred
            assert abs(weight_change) > 0.001, "Plasticity should change synaptic weights"
            
            duration = time.time() - start_time
            
            return TestResult(
                test_name="Plasticity Mechanisms",
                test_type="validation",
                status="passed",
                duration=duration,
                details={
                    "initial_avg_weight": initial_avg_weight,
                    "final_avg_weight": final_avg_weight,
                    "weight_change": weight_change,
                    "plasticity_detected": abs(weight_change) > 0.001
                }
            )
            
        except Exception as e:
            duration = time.time() - start_time
            return TestResult(
                test_name="Plasticity Mechanisms",
                test_type="validation",
                status="failed",
                message=str(e),
                duration=duration
            )
    
    def validate_agent_functionality(self) -> TestResult:
        """Validate agent functionality and integration."""
        start_time = time.time()
        
        try:
            # Create brain and agent
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 5, 5)
            world.reset()
            world.setAgentStart(5.0, 5.0)
            
            # Initialize agent
            agent.initialize(world)
            brain.initialize()
            
            # Enable all learning capabilities
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run agent-world interaction
            for step in range(100):
                # Update world
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain processes
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward
                agent.applyRewardModulation(0.1, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            # Validate agent functionality
            assert agent.getNeuromodulationLevel() > 0, "Neuromodulation should be active"
            assert agent.getCuriosityLevel() > 0, "Curiosity should be active"
            assert agent.getNoveltyLevel() > 0, "Novelty detection should be active"
            assert agent.getPredictionError() >= 0, "Prediction error should be non-negative"
            
            duration = time.time() - start_time
            
            return TestResult(
                test_name="Agent Functionality",
                test_type="validation",
                status="passed",
                duration=duration,
                details={
                    "final_neuromodulation": agent.getNeuromodulationLevel(),
                    "final_curiosity": agent.getCuriosityLevel(),
                    "final_novelty": agent.getNoveltyLevel(),
                    "final_prediction_error": agent.getPredictionError(),
                    "development_stage": str(brain.getDevelopmentalStage())
                }
            )
            
        except Exception as e:
            duration = time.time() - start_time
            return TestResult(
                test_name="Agent Functionality",
                test_type="validation",
                status="failed",
                message=str(e),
                duration=duration
            )
    
    def validate_configuration_system(self) -> TestResult:
        """Validate configuration system functionality."""
        start_time = time.time()
        
        try:
            # Create default configuration
            config = pynlm.createDefaultConfig()
            
            # Test configuration properties
            assert config.has("random_seed"), "Default config should have random_seed"
            assert config.has("simulation_timestep"), "Default config should have simulation_timestep"
            assert config.has("neuron_count"), "Default config should have neuron_count"
            
            # Test key retrieval
            keys = config.getKeys()
            assert len(keys) > 0, "Config should have some keys"
            
            # Test configuration modification
            config.set("brain.neuron_count", 2000)
            assert config.has("brain.neuron_count"), "Modified config should have new key"
            
            # Test configuration serialization
            temp_file = tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False)
            temp_file.close()
            
            config.saveToFile(temp_file.name)
            
            # Load back and verify
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(temp_file.name)
            
            assert new_config.has("brain.neuron_count"), "Loaded config should have brain.neuron_count"
            
            # Cleanup
            os.unlink(temp_file.name)
            
            duration = time.time() - start_time
            
            return TestResult(
                test_name="Configuration System",
                test_type="validation",
                status="passed",
                duration=duration,
                details={
                    "keys_count": len(keys),
                    "neuron_count": config.getOr("brain.neuron_count", 0),
                    "config_summary": config.summary()
                }
            )
            
        except Exception as e:
            duration = time.time() - start_time
            return TestResult(
                test_name="Configuration System",
                test_type="validation",
                status="failed",
                message=str(e),
                duration=duration
            )
    
    def run_all_validations(self) -> List[TestResult]:
        """Run all validations and return results."""
        validations = [
            self.validate_basic_brain_creation,
            self.validate_simulation_correctness,
            self.validate_plasticity_mechanisms,
            self.validate_agent_functionality,
            self.validate_configuration_system
        ]
        
        results = []
        for validation in validations:
            result = validation()
            results.append(result)
        
        return results
class PerformanceBenchmark:
    """
    Performance benchmarking for NLM operations.
    
    Measures and compares the performance of various NLM operations
    including brain creation, simulation steps, and agent interactions.
    """
    
    def __init__(self):
        self.benchmark_results = []
    
    def benchmark_brain_creation(self, iterations: int = 10) -> Dict[str, Any]:
        """Benchmark brain creation performance."""
        print(f"Benchmarking brain creation ({iterations} iterations)...")
        
        times = []
        for i in range(iterations):
            start_time = time.time()
            
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            end_time = time.time()
            times.append(end_time - start_time)
        
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        
        return {
            "test_name": "Brain Creation",
            "iterations": iterations,
            "avg_time": avg_time,
            "min_time": min_time,
            "max_time": max_time,
            "times": times,
            "ops_per_second": iterations / avg_time if avg_time > 0 else 0
        }
    
    def benchmark_simulation_step(self, steps: int = 1000) -> Dict[str, Any]:
        """Benchmark simulation step performance."""
        print(f"Benchmarking simulation steps ({steps} steps)...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        total_time = end_time - start_time
        avg_time_per_step = total_time / steps
        steps_per_second = steps / total_time if total_time > 0 else 0
        
        return {
            "test_name": "Simulation Step",
            "steps": steps,
            "total_time": total_time,
            "avg_time_per_step": avg_time_per_step,
            "steps_per_second": steps_per_second
        }
    
    def benchmark_agent_processing(self, steps: int = 500) -> Dict[str, Any]:
        """Benchmark agent processing performance."""
        print(f"Benchmarking agent processing ({steps} steps)...")
        
        # Create full agent-world setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        agent.initialize(world)
        brain.initialize()
        
        # Enable all capabilities
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        start_time = time.time()
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
        end_time = time.time()
        
        total_time = end_time - start_time
        avg_time_per_step = total_time / steps
        steps_per_second = steps / total_time if total_time > 0 else 0
        
        return {
            "test_name": "Agent Processing",
            "steps": steps,
            "total_time": total_time,
            "avg_time_per_step": avg_time_per_step,
            "steps_per_second": steps_per_second
        }
    
    def benchmark_memory_usage(self) -> Dict[str, Any]:
        """Benchmark memory usage (basic implementation)."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        # Get initial memory
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and use brain components
        configs = []
        brains = []
        agents = []
        worlds = []
        
        for i in range(10):
            config = pynlm.createDefaultConfig()
            configs.append(config)
            
            brain = pynlm.createBrain(config)
            brains.append(brain)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agents.append(agent)
            
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 5, 5)
            worlds.append(world)
        
        # Run simulation
        for brain in brains:
            for step in range(100):
                brain.step(step)
        
        # Get final memory
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        # Cleanup
        configs.clear()
        brains.clear()
        agents.clear()
        worlds.clear()
        
        return {
            "test_name": "Memory Usage",
            "initial_memory_mb": initial_memory,
            "final_memory_mb": final_memory,
            "memory_increase_mb": memory_increase,
            "components_created": 10 * 4  # configs, brains, agents, worlds
        }
    
    def run_all_benchmarks(self) -> List[Dict[str, Any]]:
        """Run all benchmarks and return results."""
        results = []
        
        try:
            results.append(self.benchmark_brain_creation(10))
        except Exception as e:
            print(f"Brain creation benchmark failed: {e}")
        
        try:
            results.append(self.benchmark_simulation_step(1000))
        except Exception as e:
            print(f"Simulation step benchmark failed: {e}")
        
        try:
            results.append(self.benchmark_agent_processing(500))
        except Exception as e:
            print(f"Agent processing benchmark failed: {e}")
        
        try:
            results.append(self.benchmark_memory_usage())
        except Exception as e:
            print(f"Memory usage benchmark failed: {e}")
        
        return results
class ValidationReporter:
    """
    Comprehensive reporting for validation and benchmark results.
    
    Provides multiple output formats for test results including
    console reports, JSON reports, and detailed analysis.
    """
    
    def __init__(self, output_file: str = "nlm_validation_report.json"):
        self.output_file = output_file
        self.full_report = {
            "validation_results": [],
            "benchmark_results": [],
            "summary": {}
        }
    
    def add_validation_results(self, results: List[TestResult]):
        """Add validation results to the report."""
        self.full_report["validation_results"] = [asdict(result) for result in results]
    
    def add_benchmark_results(self, results: List[Dict[str, Any]]):
        """Add benchmark results to the report."""
        self.full_report["benchmark_results"] = results
    
    def generate_summary(self, validation_results: List[TestResult], 
                        benchmark_results: List[Dict[str, Any]]):
        """Generate comprehensive summary statistics."""
        # Validation summary
        total_validations = len(validation_results)
        passed_validations = sum(1 for r in validation_results if r.status == "passed")
        failed_validations = sum(1 for r in validation_results if r.status == "failed")
        error_validations = sum(1 for r in validation_results if r.status == "error")
        
        # Benchmark summary
        total_benchmarks = len(benchmark_results)
        total_benchmark_time = sum(b.get("total_time", 0) for b in benchmark_results)
        
        self.full_report["summary"] = {
            "validation": {
                "total": total_validations,
                "passed": passed_validations,
                "failed": failed_validations,
                "errors": error_validations,
                "success_rate": (passed_validations / total_validations * 100) if total_validations > 0 else 0
            },
            "benchmarks": {
                "total": total_benchmarks,
                "total_time": total_benchmark_time,
                "average_time": total_benchmark_time / total_benchmarks if total_benchmarks > 0 else 0
            },
            "overall_status": "PASSED" if failed_validations == 0 and error_validations == 0 else "FAILED"
        }
    
    def print_console_report(self, validation_results: List[TestResult], 
                           benchmark_results: List[Dict[str, Any]]):
        """Print detailed console report."""
        print("\n" + "="*80)
        print("NLM VALIDATION AND BENCHMARKING REPORT")
        print("="*80)
        
        # Validation report
        print("\nVALIDATION RESULTS:")
        print("-" * 80)
        
        passed = sum(1 for r in validation_results if r.status == "passed")
        failed = sum(1 for r in validation_results if r.status == "failed")
        errors = sum(1 for r in validation_results if r.status == "error")
        
        print(f"Total Tests: {len(validation_results)}")
        print(f"Passed: {passed}")
        print(f"Failed: {failed}")
        print(f"Errors: {errors}")
        print(f"Success Rate: {passed/len(validation_results)*100:.1f}%")
        
        if failed > 0 or errors > 0:
            print(f"\nFAILED/ERROR TESTS:")
            for result in validation_results:
                if result.status in ["failed", "error"]:
                    print(f"  {result.test_name}: {result.status.upper()} - {result.message}")
        
        # Benchmark report
        print(f"\nBENCHMARK RESULTS:")
        print("-" * 80)
        
        for benchmark in benchmark_results:
            print(f"\n{benchmark['test_name']}:")
            if 'avg_time' in benchmark:
                print(f"  Average Time: {benchmark['avg_time']:.4f}s")
                print(f"  Operations per Second: {benchmark['ops_per_second']:.0f}")
            elif 'steps_per_second' in benchmark:
                print(f"  Steps per Second: {benchmark['steps_per_second']:.0f}")
                print(f"  Average Time per Step: {benchmark['avg_time_per_step']:.6f}s")
            elif 'memory_increase_mb' in benchmark:
                print(f"  Memory Increase: {benchmark['memory_increase_mb']:.2f} MB")
                print(f"  Components Created: {benchmark['components_created']}")
    
    def save_json_report(self):
        """Save the complete report as JSON."""
        with open(self.output_file, 'w') as f:
            json.dump(self.full_report, f, indent=2, default=str)
        
        print(f"\nJSON report saved to: {self.output_file}")
class NLMTestFramework:
    """
    Main test framework class that integrates all testing components.
    
    Provides a unified interface for running unit tests, integration tests,
    validation, and benchmarking of NLM functionality.
    """
    
    def __init__(self, name: str = "NLM Test Framework"):
        self.name = name
        self.test_suite = TestSuite(f"{name} - Unit Tests")
        self.validation_suite = TestSuite(f"{name} - Validations")
        self.brain_validator = BrainValidator()
        self.performance_benchmark = PerformanceBenchmark()
        self.reporter = ValidationReporter()
    
    def add_unit_test(self, name: str, test_func: Callable, test_type: str = "unit"):
        """Add a unit test to the test suite."""
        self.test_suite.add_test(name, test_func, test_type)
    
    def add_validation_test(self, name: str, test_func: Callable, test_type: str = "validation"):
        """Add a validation test to the validation suite."""
        self.validation_suite.add_test(name, test_func, test_type)
    
    def setup_validation_framework(self):
        """Setup validation framework with common hooks."""
        
        def reset_globals():
            """Reset global state before each test."""
            pass  # Add global reset logic if needed
        
        self.validation_suite.add_setup_hook(reset_globals)
    
    def run_all_tests(self) -> Dict[str, Any]:
        """Run all tests, validations, and benchmarks."""
        print(f"Starting {self.name}")
        print("="*80)
        
        # Run unit tests
        print("\n1. RUNNING UNIT TESTS")
        print("-" * 80)
        unit_results = self.test_suite.run_all_tests()
        unit_summary = self.test_suite.get_summary()
        
        # Run validation tests
        print("\n2. RUNNING VALIDATIONS")
        print("-" * 80)
        validation_results = self.brain_validator.run_all_validations()
        
        # Run benchmarks
        print("\n3. RUNNING BENCHMARKS")
        print("-" * 80)
        benchmark_results = self.performance_benchmark.run_all_benchmarks()
        
        # Generate comprehensive report
        print("\n4. GENERATING REPORT")
        print("-" * 80)
        self.reporter.add_validation_results(validation_results)
        self.reporter.add_benchmark_results(benchmark_results)
        self.reporter.generate_summary(validation_results, benchmark_results)
        
        # Print console report
        self.reporter.print_console_report(validation_results, benchmark_results)
        
        # Save JSON report
        self.reporter.save_json_report()
        
        # Overall summary
        overall_summary = {
            "test_framework": {
                "name": self.name,
                "unit_tests": unit_summary,
                "validations": {
                    "total": len(validation_results),
                    "passed": sum(1 for r in validation_results if r.status == "passed"),
                    "failed": sum(1 for r in validation_results if r.status == "failed"),
                    "errors": sum(1 for r in validation_results if r.status == "error")
                },
                "benchmarks": {
                    "total": len(benchmark_results)
                },
                "reporter": {
                    "output_file": self.reporter.output_file
                }
            },
            "all_results": {
                "unit_tests": unit_results,
                "validations": validation_results,
                "benchmarks": benchmark_results
            }
        }
        
        return overall_summary
def quick_validation():
    """
    Quick validation of NLM basic functionality.
    
    Fast check of essential NLM operations for continuous integration
    or regular testing.
    """
    print("Running quick NLM validation...")
    
    framework = NLMTestFramework("Quick Validation")
    
    # Add basic unit tests
    def test_basic_creation():
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
    
    def test_simulation_step():
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        for step in range(10):
            brain.step(step)
        assert brain.getTotalSpikeCount() >= 0
    
    def test_agent_functionality():
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 5, 5)
        world.reset()
        agent.initialize(world)
        assert agent.getCuriosityLevel() >= 0
    
    framework.add_unit_test("Basic Brain Creation", test_basic_creation)
    framework.add_unit_test("Simulation Step", test_simulation_step)
    framework.add_unit_test("Agent Functionality", test_agent_functionality)
    
    return framework.run_all_tests()
def comprehensive_test_suite():
    """
    Comprehensive test suite with all validation and benchmarks.
    
    Full validation suite including unit tests, integration tests,
    performance benchmarks, and correctness validations.
    """
    print("Starting comprehensive NLM test suite...")
    
    framework = NLMTestFramework("Comprehensive Test Suite")
    
    # Add comprehensive unit tests
    def test_all_basic_operations():
        # Test brain creation and initialization
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        assert brain.getTotalNeuronCount() > 0
        
        # Test simulation
        for step in range(50):
            brain.step(step)
        
        # Test agent creation
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Test world creation
        world = pynlm.createSimpleWorld()
        assert world is not None
    
    def test_configuration_system():
        config = pynlm.createDefaultConfig()
        assert config.has("random_seed")
        assert config.has("neuron_count")
        config.set("brain.neuron_count", 999)
        assert config.getOr("brain.neuron_count", 0) == 999
    
    def test_enumerations():
        # Test that all expected enumerations are available
        neuron_types = [pynlm.NeuronType.Excitatory, pynlm.NeuronType.Inhibitory]
        synapse_types = [pynlm.SynapseType.Excitatory, pynlm.SynapseType.Inhibitory]
        developmental_stages = [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.Maturation]
        
        assert len(neuron_types) >= 2
        assert len(synapse_types) >= 2
        assert len(developmental_stages) >= 2
    
    framework.add_unit_test("Basic Operations", test_all_basic_operations)
    framework.add_unit_test("Configuration System", test_configuration_system)
    framework.add_unit_test("Enumerations", test_enumerations)
    
    # Setup validation framework
    framework.setup_validation_framework()
    
    return framework.run_all_tests()
if __name__ == "__main__":
    print("NLM Testing and Validation Framework")
    print("Available test modes:")
    print("  1. Quick Validation - Fast check of basic functionality")
    print("  2. Comprehensive Test Suite - Full validation suite")
    print("  3. Exit")
    
    # Uncomment to run specific test modes
    # quick_validation()
    # comprehensive_test_suite()
    
    print("\nTo run tests, uncomment the appropriate function call in main()")
    print("Or use the test framework programmatically.")
