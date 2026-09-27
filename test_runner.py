#!/usr/bin/env python3
"""
Main test runner for NLM (Neural Learning Machine) tests.

This script provides a unified interface to run all NLM tests and benchmarks.
It supports:
- Unit tests for individual components
- Integration tests for system-wide functionality
- Performance benchmarks
- Configuration-driven test execution
"""

import argparse
import sys
import os
import time
import json
from pathlib import Path

# Add the src directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

def run_unit_tests():
    """Run unit tests for all components."""
    print("=" * 60)
    print("Running NLM Unit Tests")
    print("=" * 60)
    
    test_results = {
        'total_tests': 0,
        'passed_tests': 0,
        'failed_tests': 0,
        'start_time': time.time(),
        'tests': []
    }
    
    # Test Neuron class
    print("\n1. Testing Neuron class...")
    try:
        from tests.test_neuron import run_neuron_tests
        neuron_tests = run_neuron_tests()
        test_results['total_tests'] += neuron_tests['count']
        test_results['passed_tests'] += neuron_tests['passed']
        test_results['failed_tests'] += neuron_tests['failed']
        test_results['tests'].append({
            'name': 'Neuron Tests',
            'count': neuron_tests['count'],
            'passed': neuron_tests['passed'],
            'failed': neuron_tests['failed'],
            'time': neuron_tests['time']
        })
        print(f"   ✓ Neuron tests: {neuron_tests['passed']}/{neuron_tests['count']} passed")
    except Exception as e:
        print(f"   ✗ Neuron tests failed: {e}")
        test_results['failed_tests'] += 10  # Assume 10 tests if we can't run them
    
    # Test plasticity rules
    print("\n2. Testing plasticity rules...")
    try:
        from tests.test_plasticity import run_plasticity_tests
        plasticity_tests = run_plasticity_tests()
        test_results['total_tests'] += plasticity_tests['count']
        test_results['passed_tests'] += plasticity_tests['passed']
        test_results['failed_tests'] += plasticity_tests['failed']
        test_results['tests'].append({
            'name': 'Plasticity Tests',
            'count': plasticity_tests['count'],
            'passed': plasticity_tests['passed'],
            'failed': plasticity_tests['failed'],
            'time': plasticity_tests['time']
        })
        print(f"   ✓ Plasticity tests: {plasticity_tests['passed']}/{plasticity_tests['count']} passed")
    except Exception as e:
        print(f"   ✗ Plasticity tests failed: {e}")
        test_results['failed_tests'] += 5  # Assume 5 tests if we can't run them
    
    # Test development system
    print("\n3. Testing development system...")
    try:
        from tests.test_development import run_development_tests
        development_tests = run_development_tests()
        test_results['total_tests'] += development_tests['count']
        test_results['passed_tests'] += development_tests['passed']
        test_results['failed_tests'] += development_tests['failed']
        test_results['tests'].append({
            'name': 'Development Tests',
            'count': development_tests['count'],
            'passed': development_tests['passed'],
            'failed': development_tests['failed'],
            'time': development_tests['time']
        })
        print(f"   ✓ Development tests: {development_tests['passed']}/{development_tests['count']} passed")
    except Exception as e:
        print(f"   ✗ Development tests failed: {e}")
        test_results['failed_tests'] += 3  # Assume 3 tests if we can't run them
    
    test_results['end_time'] = time.time()
    test_results['total_time'] = test_results['end_time'] - test_results['start_time']
    
    return test_results

def run_integration_tests():
    """Run integration tests for system-wide functionality."""
    print("\n" + "=" * 60)
    print("Running NLM Integration Tests")
    print("=" * 60)
    
    integration_results = {
        'total_tests': 0,
        'passed_tests': 0,
        'failed_tests': 0,
        'start_time': time.time(),
        'tests': []
    }
    
    # Test brain integration
    print("\n1. Testing brain integration...")
    try:
        from tests.test_brain_integration import run_brain_integration_tests
        brain_tests = run_brain_integration_tests()
        integration_results['total_tests'] += brain_tests['count']
        integration_results['passed_tests'] += brain_tests['passed']
        integration_results['failed_tests'] += brain_tests['failed']
        integration_results['tests'].append({
            'name': 'Brain Integration Tests',
            'count': brain_tests['count'],
            'passed': brain_tests['passed'],
            'failed': brain_tests['failed'],
            'time': brain_tests['time']
        })
        print(f"   ✓ Brain integration tests: {brain_tests['passed']}/{brain_tests['count']} passed")
    except Exception as e:
        print(f"   ✗ Brain integration tests failed: {e}")
        integration_results['failed_tests'] += 2  # Assume 2 tests if we can't run them
    
    # Test world integration
    print("\n2. Testing world integration...")
    try:
        from tests.test_world_integration import run_world_integration_tests
        world_tests = run_world_integration_tests()
        integration_results['total_tests'] += world_tests['count']
        integration_results['passed_tests'] += world_tests['passed']
        integration_results['failed_tests'] += world_tests['failed']
        integration_results['tests'].append({
            'name': 'World Integration Tests',
            'count': world_tests['count'],
            'passed': world_tests['passed'],
            'failed': world_tests['failed'],
            'time': world_tests['time']
        })
        print(f"   ✓ World integration tests: {world_tests['passed']}/{world_tests['count']} passed")
    except Exception as e:
        print(f"   ✗ World integration tests failed: {e}")
        integration_results['failed_tests'] += 2  # Assume 2 tests if we can't run them
    
    integration_results['end_time'] = time.time()
    integration_results['total_time'] = integration_results['end_time'] - integration_results['start_time']
    
    return integration_results

def run_performance_benchmarks():
    """Run performance benchmarks."""
    print("\n" + "=" * 60)
    print("Running NLM Performance Benchmarks")
    print("=" * 60)
    
    benchmark_results = {
        'benchmarks': [],
        'start_time': time.time()
    }
    
    # Benchmark neuron dynamics
    print("\n1. Benchmarking neuron dynamics...")
    try:
        from benchmarks.benchmark_neuron import benchmark_neuron_dynamics
        neuron_bench = benchmark_neuron_dynamics()
        benchmark_results['benchmarks'].append({
            'name': 'Neuron Dynamics Benchmark',
            'neurons_per_second': neuron_bench['neurons_per_second'],
            'time_per_neuron_us': neuron_bench['time_per_neuron_us'],
            'memory_per_neuron_bytes': neuron_bench['memory_per_neuron_bytes'],
            'time': neuron_bench['time']
        })
        print(f"   ✓ Neuron dynamics: {neuron_bench['neurons_per_second']:.0f} neurons/sec")
    except Exception as e:
        print(f"   ✗ Neuron dynamics benchmark failed: {e}")
    
    # Benchmark synaptic processing
    print("\n2. Benchmarking synaptic processing...")
    try:
        from benchmarks.benchmark_synapses import benchmark_synaptic_processing
        synapse_bench = benchmark_synaptic_processing()
        benchmark_results['benchmarks'].append({
            'name': 'Synaptic Processing Benchmark',
            'synapses_per_second': synapse_bench['synapses_per_second'],
            'spike_events_per_second': synapse_bench['spike_events_per_second'],
            'time': synapse_bench['time']
        })
        print(f"   ✓ Synaptic processing: {synapse_bench['synapses_per_second']:.0f} synapses/sec")
    except Exception as e:
        print(f"   ✗ Synaptic processing benchmark failed: {e}")
    
    # Benchmark plasticity
    print("\n3. Benchmarking plasticity...")
    try:
        from benchmarks.benchmark_plasticity import benchmark_plasticity
        plasticity_bench = benchmark_plasticity()
        benchmark_results['benchmarks'].append({
            'name': 'Plasticity Benchmark',
            'weight_changes_per_second': plasticity_bench['weight_changes_per_second'],
            'time': plasticity_bench['time']
        })
        print(f"   ✓ Plasticity: {plasticity_bench['weight_changes_per_second']:.0f} weight changes/sec")
    except Exception as e:
        print(f"   ✗ Plasticity benchmark failed: {e}")
    
    benchmark_results['end_time'] = time.time()
    benchmark_results['total_time'] = benchmark_results['end_time'] - benchmark_results['start_time']
    
    return benchmark_results

def save_results(results, filename):
    """Save test results to a file."""
    try:
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        print(f"\n✓ Results saved to {filename}")
    except Exception as e:
        print(f"\n✗ Failed to save results: {e}")

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Test Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python test_runner.py                    # Run all tests
  python test_runner.py --unit-only         # Run only unit tests
  python test_runner.py --integration-only  # Run only integration tests
  python test_runner.py --benchmarks-only   # Run only benchmarks
  python test_runner.py --save-results results.json  # Save results to JSON
  python test_runner.py --config test_config.json  # Run with custom configuration
        """
    )
    
    parser.add_argument(
        '--unit-only', action='store_true',
        help='Run only unit tests (default: run all tests)'
    )
    
    parser.add_argument(
        '--integration-only', action='store_true',
        help='Run only integration tests (default: run all tests)'
    )
    
    parser.add_argument(
        '--benchmarks-only', action='store_true',
        help='Run only performance benchmarks (default: run all tests)'
    )
    
    parser.add_argument(
        '--save-results', metavar='FILE',
        help='Save test results to the specified file (JSON format)'
    )
    
    parser.add_argument(
        '--config', metavar='FILE',
        help='Load configuration from the specified file (JSON format)'
    )
    
    parser.add_argument(
        '--verbose', '-v', action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--quiet', '-q', action='store_true',
        help='Suppress non-essential output'
    )
    
    args = parser.parse_args()
    
    # Determine which tests to run
    run_unit = not args.integration_only and not args.benchmarks_only
    run_integration = not args.unit_only and not args.benchmarks_only
    run_benchmarks = not args.unit_only and not args.integration_only
    
    if args.unit_only and args.integration_only and args.benchmarks_only:
        # If all are specified, run all tests
        run_unit = run_integration = run_benchmarks = True
    
    # Load configuration if specified
    config = {}
    if args.config:
        try:
            with open(args.config, 'r') as f:
                config = json.load(f)
            print(f"✓ Configuration loaded from {args.config}")
        except Exception as e:
            print(f"✗ Failed to load configuration: {e}")
            return 1
    
    # Run tests based on flags
    all_results = {}
    
    if run_unit:
        all_results['unit_tests'] = run_unit_tests()
    
    if run_integration:
        all_results['integration_tests'] = run_integration_tests()
    
    if run_benchmarks:
        all_results['benchmarks'] = run_performance_benchmarks()
    
    # Save results if requested
    if args.save_results:
        save_results(all_results, args.save_results)
    
    # Print summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    
    total_tests = 0
    total_passed = 0
    total_failed = 0
    
    for category, results in all_results.items():
        if 'total_tests' in results:
            total_tests += results['total_tests']
            total_passed += results['passed_tests']
            total_failed += results['failed_tests']
            print(f"\n{category.replace('_', ' ').title()}:")
            print(f"  Total tests: {results['total_tests']}")
            print(f"  Passed: {results['passed_tests']}")
            print(f"  Failed: {results['failed_tests']}")
            print(f"  Success rate: {results['passed_tests']/max(1, results['total_tests'])*100:.1f}%")
            print(f"  Total time: {results['total_time']:.2f} seconds")
    
    print(f"\nOverall:")
    print(f"  Total tests: {total_tests}")
    print(f"  Passed: {total_passed}")
    print(f"  Failed: {total_failed}")
    print(f"  Success rate: {total_passed/max(1, total_tests)*100:.1f}%")
    
    total_time = sum(r.get('total_time', 0) for r in all_results.values())
    print(f"  Total time: {total_time:.2f} seconds")
    
    if total_failed > 0:
        print(f"\n✗ {total_failed} test(s) failed!")
        return 1
    else:
        print(f"\n✓ All tests passed successfully!")
        return 0

if __name__ == "__main__":
    sys.exit(main())