"""
Automated Validation Examples for NLM Testing Framework

This module provides examples of using the NLM testing framework
for automated validation and verification.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + "/../../src")

from nlm.testing import TestRunner, TestSuiteManager
from nlm.testing import IntegrationTestSuite, PerformanceTestSuite
from nlm.testing import ConfigurationTestSuite, BrainValidationTestSuite
from nlm.testing import MemTestFramework, BenchmarkSuite
from nlm.testing import CoverageReporter, TestDataGenerator

class AutomatedValidationExamples:
    """
    Example class demonstrating automated validation workflows.
    """
    
    def __init__(self):
        self.testRunner = TestRunner()
        self.coverageReporter = CoverageReporter()
        self.testDataGenerator = TestDataGenerator()
        
    def run_basic_validation_example(self):
        """
        Example: Run basic unit and integration tests
        """
        print("=== Basic Validation Example ===")
        
        # Set up test runner
        self.testRunner.setTestTimeout(60.0)
        
        # Run unit tests
        print("Running unit tests...")
        self.testRunner.runTestSuites(["unit"])
        
        # Run integration tests
        print("Running integration tests...")
        self.testRunner.runTestSuites(["integration"])
        
        # Generate report
        print("Generating test report...")
        self.testRunner.generateComprehensiveReport()
        
        print("Basic validation example completed")
        
    def run_performance_validation_example(self):
        """
        Example: Run performance regression tests
        """
        print("\n=== Performance Validation Example ===")
        
        # Create performance test suite
        perfSuite = PerformanceTestSuite()
        self.testRunner.registerPerformanceTestSuite(perfSuite)
        
        # Run performance tests
        print("Running performance tests...")
        perfSuite.runRegressionTests()
        
        # Load baseline and compare
        print("Loading baseline performance data...")
        perfSuite.loadBaseline("baseline_perf.txt")
        perfSuite.compareWithBaseline()
        
        # Generate performance report
        print("Generating performance report...")
        perfSuite.generatePerformanceReport()
        
        print("Performance validation example completed")
        
    def run_configuration_validation_example(self):
        """
        Example: Run configuration validation tests
        """
        print("\n=== Configuration Validation Example ===")
        
        # Create configuration test suite
        configSuite = ConfigurationTestSuite()
        self.testRunner.registerConfigurationTestSuite(configSuite)
        
        # Register configuration tests
        print("Registering configuration tests...")
        configSuite.registerConfigurationTest(
            "brain_config_valid",
            "Validate brain configuration parameters",
            {"neuron_count": "100", "region_count": "2", "connection_probability": "0.1"}
        )
        
        configSuite.registerConfigurationTest(
            "memory_config_valid",
            "Validate memory configuration parameters",
            {"capacity_mb": "1024", "retention_period_ms": "1000"}
        )
        
        # Run configuration tests
        print("Running configuration tests...")
        configSuite.runConfigurationTests()
        
        # Validate cross-component configuration
        print("Validating cross-component configuration...")
        bool isConsistent = configSuite.validateConfigurationConsistency()
        print(f"Configuration consistency: {"PASSED" if isConsistent else "FAILED"}")
        
        print("Configuration validation example completed")
        
    def run_brain_validation_example(self):
        """
        Example: Run brain validation tests
        """
        print("\n=== Brain Validation Example ===")
        
        # Create brain validation test suite
        brainSuite = BrainValidationTestSuite()
        self.testRunner.registerBrainValidationTestSuite(brainSuite)
        
        # Register brain validation tests
        print("Registering brain validation tests...")
        brainSuite.registerBrainValidationTest(
            "brain_state_valid",
            "Validate brain state consistency",
            lambda snapshot: brainSuite.isBrainStateValid(snapshot)
        )
        
        brainSuite.registerNeuralDynamicsTest(
            "neural_dynamics_valid",
            "Validate neural dynamics measurements",
            lambda dynamics: (dynamics.firingRate > 0.0 and dynamics.firingRate < 100.0 and
                            dynamics.synchronyIndex >= 0.0 and dynamics.synchronyIndex <= 1.0)
        )
        
        # Capture brain state
        print("Capturing brain state...")
        # In a real implementation, this would capture from actual NLM system
        brainSuite.captureBrainState(BrainStateSnapshot())
        
        # Run brain validation tests
        print("Running brain validation tests...")
        brainSuite.runBrainValidationTests()
        
        # Validate biological fidelity
        print("Validating biological fidelity...")
        bool isFidelitous = brainSuite.validateBiologicalFidelity(BrainStateSnapshot())
        print(f"Biological fidelity: {"PASSED" if isFidelitous else "FAILED"}")
        
        print("Brain validation example completed")
        
    def run_memory_validation_example(self):
        """
        Example: Run memory leak detection tests
        """
        print("\n=== Memory Validation Example ===")
        
        # Create memory test framework
        memSuite = MemTestFramework()
        self.testRunner.registerMemoryTestSuite(memSuite)
        
        # Register memory leak tests
        print("Registering memory leak tests...")
        memSuite.registerMemoryLeakTest(
            "memory_leak_basic",
            "Test for basic memory leaks",
            lambda: None,  # Simple test function
            1024,  # allocation size in bytes
            10.0   # timeout in seconds
        )
        
        # Run memory tests
        print("Running memory leak tests...")
        memSuite.runMemoryLeakTests()
        
        # Analyze memory leaks
        print("Analyzing memory leaks...")
        memSuite.analyzeMemoryLeaks()
        
        print("Memory validation example completed")
        
    def run_automated_test_generation_example(self):
        """
        Example: Automated test data generation
        """
        print("\n=== Automated Test Generation Example ===")
        
        # Generate test data
        print("Generating test data...")
        
        # Generate configuration data
        configs = self.testDataGenerator.generateConfigurationData("brain", 100)
        print(f"Generated {len(configs)} brain configurations")
        
        # Generate neural data
        neuralData = self.testDataGenerator.generateNeuralData("spiking", 20, 100)
        print(f"Generated {len(neuralData)} neural data sets")
        
        # Generate spike data
        spikes = self.testDataGenerator.generateSpikeData("poisson", 10.0, 50.0)
        print(f"Generated {len(spikes)} spike times")
        
        # Generate performance data
        perfData = self.testDataGenerator.generatePerformanceData("brain_step", 200)
        print(f"Generated {len(perfData)} performance measurements")
        
        # Validate generated data
        print("Validating generated data...")
        bool configValid = self.testDataGenerator.validateGeneratedData("configurations", configs)
        bool spikeValid = self.testDataGenerator.validateGeneratedData("spike_data", spikes)
        
        print(f"Configuration data valid: {"PASSED" if configValid else "FAILED"}")
        print(f"Spike data valid: {"PASSED" if spikeValid else "FAILED"}")
        
        print("Automated test generation example completed")
        
    def run_comprehensive_example(self):
        """
        Example: Run comprehensive validation workflow
        """
        print("=== Comprehensive Validation Workflow Example ===")
        
        # Generate test data
        self.testDataGenerator.generateUnitTestData()
        self.testDataGenerator.generateIntegrationTestData()
        self.testDataGenerator.generatePerformanceTestData()
        
        # Run basic validation
        self.run_basic_validation_example()
        
        # Run performance validation
        self.run_performance_validation_example()
        
        # Run configuration validation
        self.run_configuration_validation_example()
        
        # Run brain validation
        self.run_brain_validation_example()
        
        # Run memory validation
        self.run_memory_validation_example()
        
        # Generate final report
        print("\n=== Generating Final Validation Report ===")
        
        # Export test data
        self.testDataGenerator.exportTestData("final_test_data.txt")
        
        # Generate comprehensive report
        self.testRunner.generateComprehensiveReport()
        
        print("Comprehensive validation workflow example completed")

def main():
    """
    Main function demonstrating all validation examples.
    """
    print("NLM Automated Validation Examples")
    print("=" * 50)
    
    validator = AutomatedValidationExamples()
    
    # Run individual examples
    validator.run_basic_validation_example()
    validator.run_performance_validation_example()
    validator.run_configuration_validation_example()
    validator.run_brain_validation_example()
    validator.run_memory_validation_example()
    validator.run_automated_test_generation_example()
    
    # Run comprehensive example
    print("\n" + "=" * 50)
    validator.run_comprehensive_example()
    
    print("\n" + "=" * 50)
    print("All validation examples completed successfully!")

if __name__ == "__main__":
    main()
