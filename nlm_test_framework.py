#!/usr/bin/env python3
"""
NLM Comprehensive Test Framework

This script provides advanced testing capabilities for the NLM neural simulation framework.
It includes unit tests, integration tests, performance tests, and stress testing.
"""

import json
import subprocess
import time
import threading
import statistics
from datetime import datetime
from typing import Dict, Any, List, Optional
import numpy as np
import psutil
import os
from pathlib import Path

class NLMTestFramework:
    """Comprehensive testing framework for NLM."""
    
    def __init__(self, nlm_path: str = "nlm", test_config: str = "test_config.json"):
        self.nlm_path = Path(nlm_path)
        self.test_config = self.load_test_config(test_config)
        self.results = {}
        self.performance_data = {}
    
    def load_test_config(self, config_file: str) -> Dict[str, Any]:
        """Load test configuration from JSON file."""
        default_config = {
            "test_scenarios": [
                {
                    "name": "basic_brain_creation",
                    "type": "unit",
                    "neuron_count": 100,
                    "region_count": 1,
                    "expected_spikes": 50,
                    "timeout": 30
                },
                {
                    "name": "plasticity_learning",
                    "type": "integration",
                    "neuron_count": 500,
                    "plasticity_enabled": True,
                    "learning_steps": 1000,
                    "expected_weight_change": 0.1
                },
                {
                    "name": "memory_system",
                    "type": "integration",
                    "neuron_count": 200,
                    "memory_capacity": 100,
                    "expected_episodes": 50
                },
                {
                    "name": "neuromodulation",
                    "type": "integration",
                    "neuron_count": 300,
                    "neuromodulators": ["dopamine", "curiosity", "novelty"],
                    "expected_modulation": 0.5
                },
                {
                    "name": "performance_stress",
                    "type": "stress",
                    "neuron_count": 1000,
                    "duration": 60,
                    "max_memory_mb": 1024,
                    "max_cpu_percent": 80
                }
            ],
            "test_settings": {
                "parallel_tests": 4,
                "retry_failed": 3,
                "save_results": True,
                "generate_report": True,
                "performance_monitoring": True
            }
        }
        
        try:
            with open(config_file, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            print(f"Test config file not found: {config_file}. Using default config.")
            return default_config
        except json.JSONDecodeError as e:
            print(f"Error parsing test config: {e}. Using default config.")
            return default_config
    
    def run_unit_tests(self):
        """Run unit tests for individual components."""
        print("\n" + "="*60)
        print("RUNNING UNIT TESTS")
        print("="*60)
        
        unit_tests = [t for t in self.test_config["test_scenarios"] if t["type"] == "unit"]
        results = {}
        
        for test in unit_tests:
            print(f"\nRunning unit test: {test['name']}")
            result = self._run_single_test(test)
            results[test['name']] = result
            
            if result['status'] == 'passed':
                print(f"✓ {test['name']} PASSED")
            else:
                print(f"✗ {test['name']} FAILED: {result['error']}")
        
        self.results['unit_tests'] = results
        return results
    
    def run_integration_tests(self):
        """Run integration tests for complex system interactions."""
        print("\n" + "="*60)
        print("RUNNING INTEGRATION TESTS")
        print("="*60)
        
        integration_tests = [t for t in self.test_config["test_scenarios"] if t["type"] == "integration"]
        results = {}
        
        for test in integration_tests:
            print(f"\nRunning integration test: {test['name']}")
            result = self._run_single_test(test)
            results[test['name']] = result
            
            if result['status'] == 'passed':
                print(f"✓ {test['name']} PASSED")
            else:
                print(f"✗ {test['name']} FAILED: {result['error']}")
        
        self.results['integration_tests'] = results
        return results
    
    def run_stress_tests(self):
        """Run stress tests for system limits and robustness."""
        print("\n" + "="*60)
        print("RUNNING STRESS TESTS")
        print("="*60)
        
        stress_tests = [t for t in self.test_config["test_scenarios"] if t["type"] == "stress"]
        results = {}
        
        for test in stress_tests:
            print(f"\nRunning stress test: {test['name']}")
            result = self._run_stress_test(test)
            results[test['name']] = result
            
            if result['status'] == 'passed':
                print(f"✓ {test['name']} PASSED")
            else:
                print(f"✗ {test['name']} FAILED: {result['error']}")
        
        self.results['stress_tests'] = results
        return results
    
    def run_performance_tests(self):
        """Run performance tests and benchmark."""
        print("\n" + "="*60)
        print("RUNNING PERFORMANCE TESTS")
        print("="*60)
        
        perf_tests = [t for t in self.test_config["test_scenarios"] if t["name"].endswith('_performance')]
        if not perf_tests:
            # Create performance tests if none exist
            perf_tests = self._create_performance_tests()
        
        results = {}
        
        for test in perf_tests:
            print(f"\nRunning performance test: {test['name']}")
            result = self._run_performance_test(test)
            results[test['name']] = result
            
            if result['status'] == 'passed':
                print(f"✓ {test['name']} PASSED - {result.get('metrics', {})}")
            else:
                print(f"✗ {test['name']} FAILED: {result['error']}")
        
        self.results['performance_tests'] = results
        return results
    
    def _run_single_test(self, test_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run a single test scenario."""
        start_time = time.time()
        
        # Create test command
        cmd = self._build_test_command(test_config)
        
        # Execute test
        try:
            result = subprocess.run(
                cmd,
                shell=True,
                capture_output=True,
                text=True,
                timeout=test_config.get('timeout', 30)
            )
            
            end_time = time.time()
            duration = end_time - start_time
            
            test_result = {
                'status': 'passed' if result.returncode == 0 else 'failed',
                'command': cmd,
                'return_code': result.returncode,
                'stdout': result.stdout,
                'stderr': result.stderr,
                'duration': duration,
                'error': None if result.returncode == 0 else result.stderr,
                'timestamp': datetime.now().isoformat()
            }
            
            # Validate test results
            if test_config['type'] == 'unit':
                test_result['validation'] = self._validate_unit_test(test_result, test_config)
            elif test_config['type'] == 'integration':
                test_result['validation'] = self._validate_integration_test(test_result, test_config)
            
            return test_result
            
        except subprocess.TimeoutExpired:
            end_time = time.time()
            duration = end_time - start_time
            
            return {
                'status': 'timeout',
                'command': cmd,
                'return_code': -1,
                'stdout': '',
                'stderr': f"Test timed out after {test_config.get('timeout', 30)} seconds",
                'duration': duration,
                'error': f"Test timed out after {test_config.get('timeout', 30)} seconds",
                'timestamp': datetime.now().isoformat(),
                'validation': {'passed': False, 'reason': 'timeout'}
            }
        
        except Exception as e:
            end_time = time.time()
            duration = end_time - start_time
            
            return {
                'status': 'error',
                'command': cmd,
                'return_code': -1,
                'stdout': '',
                'stderr': str(e),
                'duration': duration,
                'error': str(e),
                'timestamp': datetime.now().isoformat(),
                'validation': {'passed': False, 'reason': 'exception'}
            }
    
    def _run_stress_test(self, test_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run a stress test with resource monitoring."""
        start_time = time.time()
        process = psutil.Process()
        
        # Record initial resource usage
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        initial_cpu = process.cpu_percent()
        
        # Start resource monitoring thread
        stop_monitoring = threading.Event()
        resource_data = []
        
        def monitor_resources():
            while not stop_monitoring.is_set():
                resource_data.append({
                    'timestamp': time.time(),
                    'memory_mb': process.memory_info().rss / 1024 / 1024,
                    'cpu_percent': process.cpu_percent(),
                    'threads': process.num_threads()
                })
                time.sleep(1)
        
        monitor_thread = threading.Thread(target=monitor_resources)
        monitor_thread.start()
        
        # Run stress test command
        cmd = self._build_stress_command(test_config)
        
        try:
            result = subprocess.run(
                cmd,
                shell=True,
                capture_output=True,
                text=True,
                timeout=test_config.get('duration', 60)
            )
            
            stop_monitoring.set()
            monitor_thread.join()
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Analyze resource usage
            peak_memory = max(d['memory_mb'] for d in resource_data) if resource_data else 0
            avg_cpu = statistics.mean(d['cpu_percent'] for d in resource_data) if resource_data else 0
            max_cpu = max(d['cpu_percent'] for d in resource_data) if resource_data else 0
            
            # Validate against limits
            validation_passed = True
            validation_errors = []
            
            if peak_memory > test_config.get('max_memory_mb', 1024):
                validation_passed = False
                validation_errors.append(f"Peak memory {peak_memory:.1f}MB exceeds limit {test_config.get('max_memory_mb', 1024)}MB")
            
            if max_cpu > test_config.get('max_cpu_percent', 80):
                validation_passed = False
                validation_errors.append(f"Max CPU {max_cpu:.1f}% exceeds limit {test_config.get('max_cpu_percent', 80)}%")
            
            if duration > test_config.get('duration', 60):
                validation_passed = False
                validation_errors.append(f"Duration {duration:.1f}s exceeds limit {test_config.get('duration', 60)}s")
            
            test_result = {
                'status': 'passed' if result.returncode == 0 and validation_passed else 'failed',
                'command': cmd,
                'return_code': result.returncode,
                'stdout': result.stdout,
                'stderr': result.stderr,
                'duration': duration,
                'error': None if (result.returncode == 0 and validation_passed) else 
                         (result.stderr if result.returncode != 0 else "; ".join(validation_errors)),
                'timestamp': datetime.now().isoformat(),
                'resource_usage': {
                    'initial_memory_mb': initial_memory,
                    'peak_memory_mb': peak_memory,
                    'avg_cpu_percent': avg_cpu,
                    'max_cpu_percent': max_cpu,
                    'sample_count': len(resource_data)
                },
                'validation': {
                    'passed': validation_passed,
                    'errors': validation_errors
                }
            }
            
            return test_result
            
        except subprocess.TimeoutExpired:
            stop_monitoring.set()
            monitor_thread.join()
            
            end_time = time.time()
            duration = end_time - start_time
            
            return {
                'status': 'timeout',
                'command': cmd,
                'return_code': -1,
                'stdout': '',
                'stderr': f"Stress test timed out after {test_config.get('duration', 60)} seconds",
                'duration': duration,
                'error': f"Stress test timed out after {test_config.get('duration', 60)} seconds",
                'timestamp': datetime.now().isoformat(),
                'validation': {'passed': False, 'reason': 'timeout'}
            }
    
    def _run_performance_test(self, test_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run performance benchmark test."""
        start_time = time.time()
        
        # Run multiple iterations for statistical significance
        iterations = test_config.get('iterations', 5)
        results = []
        
        for i in range(iterations):
            print(f"  Running iteration {i+1}/{iterations}...")
            cmd = self._build_performance_command(test_config, i)
            
            try:
                iteration_start = time.time()
                result = subprocess.run(
                    cmd,
                    shell=True,
                    capture_output=True,
                    text=True,
                    timeout=test_config.get('timeout', 30)
                )
                iteration_duration = time.time() - iteration_start
                
                results.append({
                    'iteration': i + 1,
                    'status': 'passed' if result.returncode == 0 else 'failed',
                    'return_code': result.returncode,
                    'duration': iteration_duration,
                    'output': result.stdout,
                    'error': result.stderr if result.returncode != 0 else None
                })
                
            except Exception as e:
                results.append({
                    'iteration': i + 1,
                    'status': 'error',
                    'return_code': -1,
                    'duration': time.time() - iteration_start,
                    'output': '',
                    'error': str(e)
                })
        
        end_time = time.time()
        total_duration = end_time - start_time
        
        # Analyze results
        successful_results = [r for r in results if r['status'] == 'passed']
        
        if successful_results:
            durations = [r['duration'] for r in successful_results]
            avg_duration = statistics.mean(durations)
            std_duration = statistics.stdev(durations) if len(durations) > 1 else 0
            min_duration = min(durations)
            max_duration = max(durations)
            
            throughput = len(successful_results) / total_duration
            
            performance_metrics = {
                'iterations': iterations,
                'successful': len(successful_results),
                'success_rate': len(successful_results) / iterations,
                'avg_duration': avg_duration,
                'std_duration': std_duration,
                'min_duration': min_duration,
                'max_duration': max_duration,
                'throughput': throughput,
                'operations_per_second': throughput * test_config.get('operations_per_test', 1)
            }
            
            status = 'passed' if len(successful_results) == iterations else 'partial'
        else:
            performance_metrics = {
                'iterations': iterations,
                'successful': 0,
                'success_rate': 0,
                'avg_duration': 0,
                'std_duration': 0,
                'min_duration': 0,
                'max_duration': 0,
                'throughput': 0,
                'operations_per_second': 0
            }
            status = 'failed'
        
        return {
            'status': status,
            'command': self._build_performance_command(test_config),
            'return_code': -1 if status == 'failed' else 0,
            'stdout': '',
            'stderr': '',
            'duration': total_duration,
            'error': None if status == 'passed' else "All test iterations failed",
            'timestamp': datetime.now().isoformat(),
            'performance_metrics': performance_metrics,
            'detailed_results': results
        }
    
    def _build_test_command(self, test_config: Dict[str, Any]) -> str:
        """Build test command from test configuration."""
        base_cmd = f"{self.nlm_path}"
        
        if test_config['type'] == 'unit':
            cmd_parts = [
                f"--test-type unit",
                f"--neurons {test_config['neuron_count']}",
                f"--regions {test_config['region_count']}",
                f"--expected-spikes {test_config.get('expected_spikes', 50)}"
            ]
        elif test_config['type'] == 'integration':
            cmd_parts = [
                f"--test-type integration",
                f"--neurons {test_config['neuron_count']}",
                f"--plasticity {str(test_config.get('plasticity_enabled', True)).lower()}",
                f"--learning-steps {test_config.get('learning_steps', 1000)}"
            ]
            if test_config['name'] == 'memory_system':
                cmd_parts.append(f"--memory-capacity {test_config['memory_capacity']}")
            elif test_config['name'] == 'neuromodulation':
                cmd_parts.append(f"--neuromodulators {','.join(test_config['neuromodulators'])}")
        else:
            cmd_parts = [
                f"--test-type stress",
                f"--neurons {test_config['neuron_count']}",
                f"--duration {test_config.get('duration', 60)}"
            ]
        
        return base_cmd + " " + " ".join(cmd_parts)
    
    def _build_stress_command(self, test_config: Dict[str, Any]) -> str:
        """Build stress test command."""
        return self._build_test_command(test_config)
    
    def _build_performance_command(self, test_config: Dict[str, Any], iteration: int = 0) -> str:
        """Build performance test command."""
        base_cmd = self._build_test_command(test_config)
        
        # Add performance-specific flags
        if test_config['name'] == 'performance_stress':
            cmd_parts = [
                f"--perf-test",
                f"--scale-factor {test_config.get('scale_factor', 1.0) * (iteration + 1)}",
                f"--warmup {test_config.get('warmup_steps', 100)}",
                f"--measure-latency"
            ]
        else:
            cmd_parts = [
                f"--perf-benchmark",
                f"--iterations {test_config.get('benchmark_iterations', 10)}",
                f"--concurrent {test_config.get('concurrent_tests', 1)}"
            ]
        
        return base_cmd + " " + " ".join(cmd_parts)
    
    def _validate_unit_test(self, test_result: Dict[str, Any], test_config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate unit test results."""
        validation = {'passed': True, 'checks': []}
        
        # Check return code
        if test_result['return_code'] != 0:
            validation['passed'] = False
            validation['checks'].append({
                'name': 'return_code',
                'passed': False,
                'expected': 0,
                'actual': test_result['return_code']
            })
        
        # Check duration
        if test_result['duration'] > test_config.get('timeout', 30):
            validation['passed'] = False
            validation['checks'].append({
                'name': 'duration',
                'passed': False,
                'expected': f"< {test_config.get('timeout', 30)}s",
                'actual': f"{test_result['duration']:.2f}s"
            })
        
        # Check output for expected patterns
        if 'expected_spikes' in test_config:
            try:
                spike_count = self._extract_value_from_output(test_result['stdout'], 'spike_count')
                if spike_count is not None:
                    validation['checks'].append({
                        'name': 'spike_count',
                        'passed': abs(spike_count - test_config['expected_spikes']) <= test_config.get('tolerance', 5),
                        'expected': f"~ {test_config['expected_spikes']}",
                        'actual': spike_count
                    })
                    if not validation['checks'][-1]['passed']:
                        validation['passed'] = False
            except:
                validation['checks'].append({
                    'name': 'spike_count_extraction',
                    'passed': False,
                    'reason': 'Failed to extract spike count from output'
                })
                validation['passed'] = False
        
        return validation
    
    def _validate_integration_test(self, test_result: Dict[str, Any], test_config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate integration test results."""
        validation = {'passed': True, 'checks': []}
        
        # Similar validation as unit tests but with integration-specific checks
        if test_config['name'] == 'plasticity_learning':
            if 'expected_weight_change' in test_config:
                try:
                    weight_change = self._extract_value_from_output(test_result['stdout'], 'weight_change')
                    if weight_change is not None:
                        validation['checks'].append({
                            'name': 'weight_change',
                            'passed': abs(weight_change - test_config['expected_weight_change']) <= test_config.get('tolerance', 0.05),
                            'expected': f"~ {test_config['expected_weight_change']}",
                            'actual': weight_change
                        })
                        if not validation['checks'][-1]['passed']:
                            validation['passed'] = False
                except:
                    validation['checks'].append({
                        'name': 'weight_change_extraction',
                        'passed': False,
                        'reason': 'Failed to extract weight change from output'
                    })
                    validation['passed'] = False
        
        return validation
    
    def _extract_value_from_output(self, output: str, pattern: str) -> Optional[float]:
        """Extract numeric value from test output using regex pattern."""
        import re
        
        # Try to find pattern in output
        match = re.search(rf'{pattern}:\s*([\d\.]+)', output)
        if match:
            return float(match.group(1))
        
        # Try alternative patterns
        patterns = [
            rf'{pattern}=([\d\.]+)',
            rf'{pattern}\s*([\d\.]+)',
            rf'\b{pattern}\s*[:=]\s*([\d\.]+)'
        ]
        
        for pat in patterns:
            match = re.search(pat, output)
            if match:
                return float(match.group(1))
        
        return None
    
    def _create_performance_tests(self) -> List[Dict[str, Any]]:
        """Create performance test scenarios if none exist."""
        return [
            {
                'name': 'performance_benchmark',
                'type': 'performance',
                'iterations': 5,
                'timeout': 30,
                'scale_factor': 1.0,
                'warmup_steps': 100
            },
            {
                'name': 'concurrent_performance',
                'type': 'performance',
                'iterations': 3,
                'timeout': 60,
                'scale_factor': 2.0,
                'warmup_steps': 200,
                'concurrent_tests': 2
            }
        ]
    
    def run_all_tests(self) -> Dict[str, Any]:
        """Run all configured tests and generate comprehensive report."""
        print("\n" + "="*80)
        print("NLM COMPREHENSIVE TEST SUITE")
        print("="*80)
        
        start_time = time.time()
        
        # Run all test categories
        self.run_unit_tests()
        self.run_integration_tests()
        self.run_stress_tests()
        self.run_performance_tests()
        
        end_time = time.time()
        total_duration = end_time - start_time
        
        # Generate comprehensive report
        report = self._generate_test_report(total_duration)
        
        # Save results
        self._save_test_results(report)
        
        return report
    
    def _generate_test_report(self, total_duration: float) -> Dict[str, Any]:
        """Generate comprehensive test report."""
        report = {
            'test_suite_info': {
                'name': 'NLM Comprehensive Test Suite',
                'version': '1.0',
                'description': 'Comprehensive testing for NLM neural simulation framework',
                'start_time': datetime.now().isoformat(),
                'duration': total_duration,
                'test_scenarios': len(self.test_config["test_scenarios"])
            },
            'test_results': self.results,
            'summary': self._generate_summary(),
            'recommendations': self._generate_recommendations(),
            'generated_at': datetime.now().isoformat()
        }
        
        return report
    
    def _generate_summary(self) -> Dict[str, Any]:
        """Generate test summary statistics."""
        summary = {
            'total_tests': 0,
            'passed_tests': 0,
            'failed_tests': 0,
            'timeout_tests': 0,
            'error_tests': 0,
            'success_rate': 0,
            'test_categories': {},
            'performance_metrics': {}
        }
        
        # Calculate totals
        for category, results in self.results.items():
            category_total = len(results)
            category_passed = sum(1 for r in results.values() if r['status'] == 'passed')
            category_failed = sum(1 for r in results.values() if r['status'] == 'failed')
            category_timeout = sum(1 for r in results.values() if r['status'] == 'timeout')
            category_error = sum(1 for r in results.values() if r['status'] == 'error')
            
            summary['total_tests'] += category_total
            summary['passed_tests'] += category_passed
            summary['failed_tests'] += category_failed
            summary['timeout_tests'] += category_timeout
            summary['error_tests'] += category_error
            
            summary['test_categories'][category] = {
                'total': category_total,
                'passed': category_passed,
                'failed': category_failed,
                'timeout': category_timeout,
                'error': category_error,
                'success_rate': category_passed / category_total if category_total > 0 else 0
            }
        
        # Calculate overall success rate
        if summary['total_tests'] > 0:
            summary['success_rate'] = summary['passed_tests'] / summary['total_tests']
        
        # Extract performance metrics
        if 'performance_tests' in self.results:
            for test_name, result in self.results['performance_tests'].items():
                if 'performance_metrics' in result:
                    summary['performance_metrics'][test_name] = result['performance_metrics']
        
        return summary
    
    def _generate_recommendations(self) -> List[str]:
        """Generate test-based recommendations."""
        recommendations = []
        
        # Analyze results for issues and recommendations
        if 'unit_tests' in self.results:
            failed_unit_tests = [
                name for name, result in self.results['unit_tests'].items() 
                if result['status'] == 'failed'
            ]
            if failed_unit_tests:
                recommendations.append(
                    f"Fix {len(failed_unit_tests)} failed unit tests: {', '.join(failed_unit_tests)}"
                )
        
        if 'integration_tests' in self.results:
            failed_integration_tests = [
                name for name, result in self.results['integration_tests'].items() 
                if result['status'] == 'failed'
            ]
            if failed_integration_tests:
                recommendations.append(
                    f"Address {len(failed_integration_tests)} failed integration tests: {', '.join(failed_integration_tests)}"
                )
        
        if 'stress_tests' in self.results:
            failed_stress_tests = [
                name for name, result in self.results['stress_tests'].items() 
                if result['status'] == 'failed'
            ]
            if failed_stress_tests:
                recommendations.append(
                    f"Improve system robustness for {len(failed_stress_tests)} stress tests: {', '.join(failed_stress_tests)}"
                )
        
        # Performance recommendations
        if 'performance_tests' in self.results:
            slow_performance_tests = []
            for name, result in self.results['performance_tests'].items():
                if 'performance_metrics' in result:
                    metrics = result['performance_metrics']
                    if metrics.get('avg_duration', 0) > 10:  # More than 10 seconds average
                        slow_performance_tests.append(name)
            
            if slow_performance_tests:
                recommendations.append(
                    f"Optimize performance for {len(slow_performance_tests)} tests: {', '.join(slow_performance_tests)}"
                )
        
        # Success recommendations
        success_rate = self._generate_summary().get('success_rate', 0)
        if success_rate >= 0.9:
            recommendations.append("Excellent test suite performance! Consider adding more comprehensive stress tests.")
        elif success_rate >= 0.7:
            recommendations.append("Good test suite performance. Consider fixing integration test failures.")
        elif success_rate >= 0.5:
            recommendations.append("Moderate test suite performance. Focus on fixing unit and integration test failures.")
        else:
            recommendations.append("Poor test suite performance. Immediate attention needed to fix failing tests.")
        
        return recommendations
    
    def _save_test_results(self, report: Dict[str, Any]):
        """Save test results to file."""
        if self.test_config["test_settings"]["save_results"]:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            
            # Save detailed results
            results_file = f"test_results_{timestamp}.json"
            with open(results_file, 'w') as f:
                json.dump(report, f, indent=2)
            
            print(f"\nDetailed test results saved to: {results_file}")
            
            # Generate human-readable report
            if self.test_config["test_settings"]["generate_report"]:
                report_file = f"test_report_{timestamp}.md"
                self._generate_html_report(report, report_file)
                print(f"Human-readable test report saved to: {report_file}")
    
    def _generate_html_report(self, report: Dict[str, Any], output_file: str):
        """Generate HTML test report."""
        # This is a simplified version of the HTML report generation
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Test Suite Report</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 40px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .test-category {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
                .passed {{ background-color: #d4edda; border-color: #c3e6cb; }}
                .failed {{ background-color: #f8d7da; border-color: #f5c6cb; }}
                .timeout {{ background-color: #fff3cd; border-color: #ffeaa7; }}
                .error {{ background-color: #f8d7da; border-color: #f5c6cb; }}
                table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
                th, td {{ padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }}
                th {{ background-color: #f8f9fa; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Test Suite Report</h1>
                <p>Generated: {report['generated_at']}</p>
                <p>Duration: {report['test_suite_info']['duration']:.2f} seconds</p>
                <p>Total Tests: {report['summary']['total_tests']}</p>
                <p>Success Rate: {report['summary']['success_rate']:.1%}</p>
            </div>
            
            <h2>Recommendations</h2>
            <ul>
        """
        
        for recommendation in report['recommendations']:
            html_content += f"<li>{recommendation}</li>\n"
        
        html_content += """
            </ul>
        </body>
        </html>
        """
        
        with open(output_file, 'w') as f:
            f.write(html_content)


def main():
    """Main entry point for NLM Test Framework."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Comprehensive Test Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_test_framework --config my_tests.json
  nlm_test_framework --quick
  nlm_test_framework --unit-only
  nlm_test_framework --integration-only
  nlm_test_framework --performance-only
  nlm_test_framework --stress-only
  nlm_test_framework --generate-report
  nlm_test_framework --save-results
        """
    )
    
    parser.add_argument('--config', default='test_config.json', 
                       help='Path to test configuration file')
    parser.add_argument('--quick', action='store_true', 
                       help='Run quick test suite (unit and integration tests only)')
    parser.add_argument('--unit-only', action='store_true', 
                       help='Run only unit tests')
    parser.add_argument('--integration-only', action='store_true', 
                       help='Run only integration tests')
    parser.add_argument('--performance-only', action='store_true', 
                       help='Run only performance tests')
    parser.add_argument('--stress-only', action='store_true', 
                       help='Run only stress tests')
    parser.add_argument('--generate-report', action='store_true', default=True,
                       help='Generate HTML test report')
    parser.add_argument('--save-results', action='store_true', default=True,
                       help='Save test results to file')
    parser.add_argument('--nlm-path', default='nlm', 
                       help='Path to NLM executable')
    
    args = parser.parse_args()
    
    # Create test framework
    framework = NLMTestFramework(args.nlm_path, args.config)
    
    # Override test categories based on command line arguments
    if args.quick:
        framework.test_config['test_scenarios'] = [
            t for t in framework.test_config['test_scenarios']
            if t['type'] in ['unit', 'integration']
        ]
    
    if args.unit_only:
        framework.test_config['test_scenarios'] = [
            t for t in framework.test_config['test_scenarios']
            if t['type'] == 'unit'
        ]
    
    if args.integration_only:
        framework.test_config['test_scenarios'] = [
            t for t in framework.test_config['test_scenarios']
            if t['type'] == 'integration'
        ]
    
    if args.performance_only:
        framework.test_config['test_scenarios'] = [
            t for t in framework.test_config['test_scenarios']
            if t['type'] == 'performance'
        ]
    
    if args.stress_only:
        framework.test_config['test_scenarios'] = [
            t for t in framework.test_config['test_scenarios']
            if t['type'] == 'stress'
        ]
    
    # Update test settings
    framework.test_config['test_settings']['generate_report'] = args.generate_report
    framework.test_config['test_settings']['save_results'] = args.save_results
    
    # Run all tests
    report = framework.run_all_tests()
    
    # Print summary
    print("\n" + "="*80)
    print("TEST SUITE EXECUTION COMPLETE")
    print("="*80)
    print(f"Total Tests: {report['summary']['total_tests']}")
    print(f"Passed: {report['summary']['passed_tests']}")
    print(f"Failed: {report['summary']['failed_tests']}")
    print(f"Timeouts: {report['summary']['timeout_tests']}")
    print(f"Errors: {report['summary']['error_tests']}")
    print(f"Success Rate: {report['summary']['success_rate']:.1%}")
    print("\nRecommendations:")
    for i, recommendation in enumerate(report['recommendations'], 1):
        print(f"  {i}. {recommendation}")
    
    # Exit with appropriate code
    if report['summary']['failed_tests'] > 0 or report['summary']['timeout_tests'] > 0:
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    main()