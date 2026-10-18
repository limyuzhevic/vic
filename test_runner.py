#!/usr/bin/env python3
"""
NLM Comprehensive Test Runner
============================

This script runs comprehensive tests for the NLM (Neural Learning Machine) framework,
including unit tests, integration tests, experiment tests, and visualization tests.

Usage:
    python test_runner.py [options]

Options:
    --unit-only           Run only unit tests
    --integration-only    Run only integration tests
    --experiment-only     Run only experiment tests
    --visualization-only  Run only visualization tests
    --coverage            Generate coverage report
    --verbose             Verbose output
    --output-format       Output format (json, text, html)
    --help                Show this help message
"""

import sys
import os
import argparse
import json
import time
from datetime import datetime
from pathlib import Path

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

from pynlm import (
    Config, Brain, SimpleWorld, AgentBrain, NeuronType, 
    ActionType, MotorCommand, Vision, Audio, InternalSignals,
    LearningExperiment, ExperimentRunner, CommandLineParser,
    ConfigManager, VisualizationInterface, OutputFormat
)

class TestResult:
    def __init__(self, test_name, test_type, status, duration, message=""):
        self.test_name = test_name
        self.test_type = test_type
        self.status = status  # "PASS", "FAIL", "SKIP"
        self.duration = duration
        self.message = message
        self.timestamp = datetime.now()
    
    def to_dict(self):
        return {
            "test_name": self.test_name,
            "test_type": self.test_type,
            "status": self.status,
            "duration": self.duration,
            "message": self.message,
            "timestamp": self.timestamp.isoformat()
        }

class TestRunner:
    def __init__(self, verbose=False):
        self.verbose = verbose
        self.results = []
        self.config = Config()
        
        # Initialize test config
        self.config.set("neuron_count", 100)
        self.config.set("region_count", 1)
        self.config.set("simulation_timestep", 0.001)
        self.config.set("random_seed", 42)
    
    def run_all_tests(self):
        """Run all types of tests"""
        print("=" * 60)
        print("NLM Comprehensive Test Suite")
        print("=" * 60)
        
        test_results = []
        
        # Run unit tests
        print("\n[UNIT TESTS]")
        unit_results = self.run_unit_tests()
        test_results.extend(unit_results)
        
        # Run integration tests
        print("\n[INTEGRATION TESTS]")
        integration_results = self.run_integration_tests()
        test_results.extend(integration_results)
        
        # Run experiment tests
        print("\n[EXPERIMENT TESTS]")
        experiment_results = self.run_experiment_tests()
        test_results.extend(experiment_results)
        
        # Run visualization tests
        print("\n[VISUALIZATION TESTS]")
        visualization_results = self.run_visualization_tests()
        test_results.extend(visualization_results)
        
        # Summary
        self.print_summary(test_results)
        
        return test_results
    
    def run_unit_tests(self):
        """Run unit tests"""
        results = []
        
        # Test Config
        try:
            start_time = time.time()
            config = Config()
            config.set("test_key", "test_value")
            value = config.getOr("test_key", "default")
            assert value == "test_value"
            
            if self.verbose:
                print("  ✓ Config unit test passed")
            
            results.append(TestResult("Config_unit_test", "unit", "PASS", 
                                    time.time() - start_time, "Configuration test successful"))
        except Exception as e:
            results.append(TestResult("Config_unit_test", "unit", "FAIL", 
                                    time.time() - start_time, f"Configuration test failed: {str(e)}"))
        
        # Test Brain creation
        try:
            start_time = time.time()
            brain = Brain(self.config)
            assert brain is not None
            
            if self.verbose:
                print("  ✓ Brain unit test passed")
            
            results.append(TestResult("Brain_creation_unit_test", "unit", "PASS", 
                                    time.time() - start_time, "Brain creation test successful"))
        except Exception as e:
            results.append(TestResult("Brain_creation_unit_test", "unit", "FAIL", 
                                    time.time() - start_time, f"Brain creation test failed: {str(e)}"))
        
        # Test NeuronType enum
        try:
            start_time = time.time()
            assert NeuronType.Excitatory != NeuronType.Inhibitory
            assert NeuronType.Sensory != NeuronType.Motor
            
            if self.verbose:
                print("  ✓ NeuronType enum unit test passed")
            
            results.append(TestResult("NeuronType_enum_unit_test", "unit", "PASS", 
                                    time.time() - start_time, "NeuronType enum test successful"))
        except Exception as e:
            results.append(TestResult("NeuronType_enum_unit_test", "unit", "FAIL", 
                                    time.time() - start_time, f"NeuronType enum test failed: {str(e)}"))
        
        # Test ActionType enum
        try:
            start_time = time.time()
            assert ActionType.MoveForward != ActionType.Rest
            assert ActionType.Interact != ActionType.Wait
            
            if self.verbose:
                print("  ✓ ActionType enum unit test passed")
            
            results.append(TestResult("ActionType_enum_unit_test", "unit", "PASS", 
                                    time.time() - start_time, "ActionType enum test successful"))
        except Exception as e:
            results.append(TestResult("ActionType_enum_unit_test", "unit", "FAIL", 
                                    time.time() - start_time, f"ActionType enum test failed: {str(e)}"))
        
        return results
    
    def run_integration_tests(self):
        """Run integration tests"""
        results = []
        
        # Test ConfigManager
        try:
            start_time = time.time()
            manager = ConfigManager(self.config)
            manager.setConfigValue("test_integration", "integration_value")
            value = manager.getConfigValue("test_integration")
            assert value == "integration_value"
            
            if self.verbose:
                print("  ✓ ConfigManager integration test passed")
            
            results.append(TestResult("ConfigManager_integration_test", "integration", "PASS", 
                                    time.time() - start_time, "ConfigManager integration test successful"))
        except Exception as e:
            results.append(TestResult("ConfigManager_integration_test", "integration", "FAIL", 
                                    time.time() - start_time, f"ConfigManager integration test failed: {str(e)}"))
        
        # Test CommandLineParser
        try:
            start_time = time.time()
            parser = CommandLineParser(3, ["test", "--help", "--version"])
            assert parser.hasOption("--help")
            assert parser.getOption("--help", "false") == "true"
            
            if self.verbose:
                print("  ✓ CommandLineParser integration test passed")
            
            results.append(TestResult("CommandLineParser_integration_test", "integration", "PASS", 
                                    time.time() - start_time, "CommandLineParser integration test successful"))
        except Exception as e:
            results.append(TestResult("CommandLineParser_integration_test", "integration", "FAIL", 
                                    time.time() - start_time, f"CommandLineParser integration test failed: {str(e)}"))
        
        # Test VisualizationInterface
        try:
            start_time = time.time()
            brain = Brain(self.config)
            viz = VisualizationInterface(brain, OutputFormat.Text)
            assert viz is not None
            
            if self.verbose:
                print("  ✓ VisualizationInterface integration test passed")
            
            results.append(TestResult("VisualizationInterface_integration_test", "integration", "PASS", 
                                    time.time() - start_time, "VisualizationInterface integration test successful"))
        except Exception as e:
            results.append(TestResult("VisualizationInterface_integration_test", "integration", "FAIL", 
                                    time.time() - start_time, f"VisualizationInterface integration test failed: {str(e)}"))
        
        return results
    
    def run_experiment_tests(self):
        """Run experiment tests"""
        results = []
        
        # Test ExperimentRunner
        try:
            start_time = time.time()
            brain = Brain(self.config)
            runner = ExperimentRunner(self.config)
            runner.setBrain(brain)
            
            if self.verbose:
                print("  ✓ ExperimentRunner experiment test passed")
            
            results.append(TestResult("ExperimentRunner_experiment_test", "experiment", "PASS", 
                                    time.time() - start_time, "ExperimentRunner experiment test successful"))
        except Exception as e:
            results.append(TestResult("ExperimentRunner_experiment_test", "experiment", "FAIL", 
                                    time.time() - start_time, f"ExperimentRunner experiment test failed: {str(e)}"))
        
        # Test LearningExperiment (this would require a brain with neurons)
        try:
            start_time = time.time()
            brain = Brain(self.config)
            
            # Initialize brain to have neurons
            if brain.initialize():
                LearningExperiment experiment(brain, 123)
                assert experiment is not None
                
                if self.verbose:
                    print("  ✓ LearningExperiment experiment test passed")
                
                results.append(TestResult("LearningExperiment_experiment_test", "experiment", "PASS", 
                                        time.time() - start_time, "LearningExperiment experiment test successful"))
            else:
                results.append(TestResult("LearningExperiment_experiment_test", "experiment", "SKIP", 
                                        time.time() - start_time, "Brain initialization failed, skipping test"))
        except Exception as e:
            results.append(TestResult("LearningExperiment_experiment_test", "experiment", "FAIL", 
                                    time.time() - start_time, f"LearningExperiment experiment test failed: {str(e)}"))
        
        return results
    
    def run_visualization_tests(self):
        """Run visualization tests"""
        results = []
        
        # Test different output formats
        formats = [
            (OutputFormat.Text, "Text format"),
            (OutputFormat.JSON, "JSON format"),
            (OutputFormat.CSV, "CSV format"),
            (OutputFormat.HTML, "HTML format"),
            (OutputFormat.Plot, "Plot format"),
            (OutputFormat.ASCII, "ASCII format")
        ]
        
        brain = Brain(self.config)
        
        for fmt, name in formats:
            try:
                start_time = time.time()
                viz = VisualizationInterface(brain, fmt)
                viz.setDimensions(80, 40)
                assert viz is not None
                
                if self.verbose:
                    print(f"  ✓ VisualizationInterface {name} test passed")
                
                results.append(TestResult(f"VisualizationInterface_{name}_test", "visualization", "PASS", 
                                        time.time() - start_time, f"VisualizationInterface {name} test successful"))
            except Exception as e:
                results.append(TestResult(f"VisualizationInterface_{name}_test", "visualization", "FAIL", 
                                        time.time() - start_time, f"VisualizationInterface {name} test failed: {str(e)}"))
        
        return results
    
    def print_summary(self, results):
        """""""
        print("\n" + "=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        
        # Group results by type
        by_type = {}
        for result in results:
            if result.test_type not in by_type:
                by_type[result.test_type] = []
            by_type[result.test_type].append(result)
        
        # Print results by type
        total_tests = len(results)
        passed_tests = sum(1 for r in results if r.status == "PASS")
        failed_tests = sum(1 for r in results if r.status == "FAIL")
        skipped_tests = sum(1 for r in results if r.status == "SKIP")
        
        print(f"\nTotal Tests: {total_tests}")
        print(f"Passed: {passed_tests} ({passed_tests/max(total_tests, 1)*100:.1f}%)")
        print(f"Failed: {failed_tests} ({failed_tests/max(total_tests, 1)*100:.1f}%)")
        print(f"Skipped: {skipped_tests} ({skipped_tests/max(total_tests, 1)*100:.1f}%)")
        
        # Print details by type
        for test_type, type_results in by_type.items():
            print(f"\n{test_type.upper()} TESTS:")
            for result in type_results:
                status_icon = "✓" if result.status == "PASS" else "✗" if result.status == "FAIL" else "⏭"
                print(f"  {status_icon} {result.test_name} ({result.duration:.3f}s) - {result.message}")
        
        # Overall status
        if failed_tests == 0:
            print(f"\n🎉 ALL TESTS PASSED!")
            return True
        else:
            print(f"\n❌ {failed_tests} TESTS FAILED")
            return False

def main():
    parser = argparse.ArgumentParser(description="NLM Comprehensive Test Runner")
    parser.add_argument("--unit-only", action="store_true", help="Run only unit tests")
    parser.add_argument("--integration-only", action="store_true", help="Run only integration tests")
    parser.add_argument("--experiment-only", action="store_true", help="Run only experiment tests")
    parser.add_argument("--visualization-only", action="store_true", help="Run only visualization tests")
    parser.add_argument("--coverage", action="store_true", help="Generate coverage report")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    parser.add_argument("--output-format", choices=["json", "text", "html"], 
                       default="text", help="Output format")
    parser.add_argument("--help", action="store_true", help="Show this help message")
    
    args = parser.parse_args()
    
    if args.help:
        parser.print_help()
        return 0
    
    runner = TestRunner(verbose=args.verbose)
    
    # Run tests based on arguments
    all_results = []
    
    if args.unit_only:
        print("Running unit tests only...")
        all_results.extend(runner.run_unit_tests())
    elif args.integration_only:
        print("Running integration tests only...")
        all_results.extend(runner.run_integration_tests())
    elif args.experiment_only:
        print("Running experiment tests only...")
        all_results.extend(runner.run_experiment_tests())
    elif args.visualization_only:
        print("Running visualization tests only...")
        all_results.extend(runner.run_visualization_tests())
    else:
        # Run all tests
        all_results = runner.run_all_tests()
    
    # Save results if JSON format requested
    if args.output_format == "json":
        results_data = [result.to_dict() for result in all_results]
        with open("test_results.json", "w") as f:
            json.dump(results_data, f, indent=2)
        print(f"\nResults saved to test_results.json")
    
    # Generate coverage if requested
    if args.coverage:
        print("\nGenerating coverage report...")
        # Coverage generation would go here
        print("Coverage report generation not yet implemented")
    
    # Return exit code based on test results
    failed_tests = sum(1 for r in all_results if r.status == "FAIL")
    return 0 if failed_tests == 0 else 1

if __name__ == "__main__":
    sys.exit(main())