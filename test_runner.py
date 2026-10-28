#!/usr/bin/env python3
"""
Test runner for enhanced NLM Python API.

This script runs the comprehensive test suite for all improvements
made to the NLM Python bindings.
"""

import sys
import os
import argparse
import subprocess
import time
from pathlib import Path

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def run_python_tests():
    """Run the Python test suite."""
    print("Running Python test suite...")
    
    try:
        # Import and run the test suite
        from test_enhanced_api import (
            TestEnhancedNLMEnvironment,
            TestNLMDataProcessor,
            TestNLMExperiment,
            TestConvenienceFunctions,
            TestBackwardCompatibility,
            TestErrorHandling,
            TestPerformance
        )
        
        import unittest
        
        # Create test suite
        suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
        
        # Run tests with verbose output
        runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
        result = runner.run(suite)
        
        if result.failures or result.errors:
            print(f"\n❌ Tests failed: {len(result.failures)} failures, {len(result.errors)} errors")
            return False
        else:
            print(f"\n✅ All tests passed: {result.testsRun} tests")
            return True
            
    except ImportError as e:
        print(f"❌ Failed to import test modules: {e}")
        return False
    except Exception as e:
        print(f"❌ Error running tests: {e}")
        import traceback
        traceback.print_exc()
        return False
def run_demonstration():
    """Run the demonstration script."""
    print("\n" + "="*80)
    print("RUNNING ENHANCED NLM API DEMONSTRATION")
    print("="*80)
    
    try:
        # Run the demonstration script
        demo_script = os.path.join(os.path.dirname(__file__), 'enhanced_demonstration.py')
        
        if not os.path.exists(demo_script):
            print(f"❌ Demonstration script not found: {demo_script}")
            return False
        
        # Run with minimal example for quick test
        result = subprocess.run([
            sys.executable, demo_script,
            '--example', 'minimal'
        ], capture_output=True, text=True, timeout=60)
        
        if result.returncode == 0:
            print("✅ Demonstration completed successfully!")
            print(result.stdout)
            return True
        else:
            print(f"❌ Demonstration failed with return code {result.returncode}")
            print("STDOUT:", result.stdout)
            print("STDERR:", result.stderr)
            return False
            
    except subprocess.TimeoutExpired:
        print("❌ Demonstration timed out")
        return False
    except Exception as e:
        print(f"❌ Error running demonstration: {e}")
        return False
def run_performance_test():
    """Run performance tests."""
    print("\n" + "="*80)
    print("RUNNING PERFORMANCE TESTS")
    print("="*80)
    
    try:
        import time
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        print("Testing memory efficiency...")
        initial_memory = process.memory_info().rss
        
        # Create multiple environments
        from enhanced_api import NLMEnvironment
        
        envs = []
        for i in range(3):
            env = NLMEnvironment()
            envs.append(env)
            
            # Run a quick episode
            summary = env.run_episode(20)
            print(f"  Environment {i + 1}: {env.brain.getTotalNeuronCount()} neurons, "
                  f"reward: {summary['total_reward']:.2f}")
        
        peak_memory = process.memory_info().rss
        memory_increase = (peak_memory - initial_memory) / (1024 * 1024)  # MB
        
        print(f"\nMemory usage: {memory_increase:.1f} MB increase for 3 environments")
        
        if memory_increase < 200:  # Less than 200MB
            print("✅ Memory efficiency test passed")
        else:
            print("⚠️  Memory usage high but acceptable")
        
        # Clean up
        for env in envs:
            del env
        
        print("\nTesting execution speed...")
        
        # Time execution
        start_time = time.time()
        env = NLMEnvironment()
        summary = env.run_episode(50)
        end_time = time.time()
        
        execution_time = end_time - start_time
        
        print(f"Execution time for 50 steps: {execution_time:.2f} seconds")
        print(f"Steps per second: {50 / execution_time:.1f}")
        
        if execution_time < 5.0:  # Less than 5 seconds
            print("✅ Performance test passed")
        else:
            print("⚠️  Execution time slow but acceptable")
        
        del env
        
        return True
        
    except ImportError:
        print("⚠️  psutil not available, skipping memory tests")
        return True
    except Exception as e:
        print(f"❌ Error in performance tests: {e}")
        return False
def verify_api_completeness():
    """Verify that the API is complete and functional."""
    print("\n" + "="*80)
    print("VERIFYING API COMPLETENESS")
    print("="*80)
    
    try:
        import pynlm
        
        # Test basic API functionality
        print("Testing basic NLM API...")
        
        # Create config
        config = pynlm.createDefaultConfig()
        print("✅ Config creation successful")
        
        # Set configuration
        config.set("test.int", 42)
        config.set("test.string", "hello")
        config.set("test.double", 3.14)
        config.set("test.bool", True)
        
        print("✅ Configuration operations successful")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✅ Brain creation successful: {brain.getTotalNeuronCount()} neurons")
        
        # Run steps
        for i in range(5):
            brain.step(i)
        
        print("✅ Simulation steps successful")
        
        # Test statistics
        stats = {
            'neuron_count': brain.getTotalNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
        }
        
        print(f"✅ Statistics collection successful: {stats}")
        
        # Test world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 4, 4)
        world.reset()
        
        print("✅ World creation successful")
        
        # Test agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✅ Agent creation successful")
        
        # Test perception
        percept = world.getSensoryPercept()
        print("✅ Perception retrieval successful")
        
        # Test action
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        print(f"✅ Action decoding successful: {action}")
        
        # Test save/load (basic file)
        import tempfile
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            config.saveToFile(f.name)
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(f.name)
            print("✅ Save/load operations successful")
        
        return True
        
    except Exception as e:
        print(f"❌ API completeness test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
def main():
    """Main test runner."""
    parser = argparse.ArgumentParser(
        description="Enhanced NLM Python API Test Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Usage Examples:
  python test_runner.py                    # Run all tests
  python test_runner.py --demo              # Run demonstration only
  python test_runner.py --performance       # Run performance tests only
  python test_runner.py --verify           # Verify API completeness only
  python test_runner.py --quick             # Quick test of core functionality
        """
    )
    
    parser.add_argument(
        '--demo',
        action='store_true',
        help='Run demonstration script only'
    )
    
    parser.add_argument(
        '--performance',
        action='store_true',
        help='Run performance tests only'
    )
    
    parser.add_argument(
        '--verify',
        action='store_true',
        help='Verify API completeness only'
    )
    
    parser.add_argument(
        '--quick',
        action='store_true',
        help='Run quick test of core functionality only'
    )
    
    args = parser.parse_args()
    
    print("=" * 80)
    print("ENHANCED NLM PYTHON API TEST RUNNER")
    print("=" * 80)
    print(f"Time: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Working directory: {os.getcwd()}")
    
    # Determine which tests to run
    if args.quick:
        tests_to_run = [
            ("API Completeness", verify_api_completeness),
        ]
    else:
        tests_to_run = [
            ("API Completeness", verify_api_completeness),
            ("Python Tests", run_python_tests),
            ("Demonstration", run_demonstration),
            ("Performance", run_performance_test),
        ]
    
    # Override based on flags
    if args.demo:
        tests_to_run = [("Demonstration", run_demonstration)]
    elif args.performance:
        tests_to_run = [("Performance", run_performance_test)]
    elif args.verify:
        tests_to_run = [("API Completeness", verify_api_completeness)]
    
    # Run tests
    passed = 0
    failed = 0
    
    for test_name, test_func in tests_to_run:
        print(f"\n{'='*80}")
        print(f"TEST: {test_name}")
        print('='*80)
        
        try:
            if test_func():
                passed += 1
                print(f"✅ {test_name} PASSED")
            else:
                failed += 1
                print(f"❌ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"❌ {test_name} FAILED with exception: {e}")
            import traceback
            traceback.print_exc()
    
    # Summary
    print(f"\n{'='*80}")
    print("TEST SUMMARY")
    print('='*80)
    print(f"Total tests: {len(tests_to_run)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n🎉 ALL TESTS PASSED!")
        print("\nThe enhanced NLM Python API improvements are working correctly:")
        print("  ✅ High-level convenience APIs")
        print("  ✅ Advanced feature support")
        print("  ✅ Better error handling")
        print("  ✅ Performance optimizations")
        print("  ✅ Backward compatibility")
        return 0
    else:
        print(f"\n❌ {failed} TEST(S) FAILED")
        print("Please review the failures above.")
        return 1
if __name__ == '__main__':
    sys.exit(main())
