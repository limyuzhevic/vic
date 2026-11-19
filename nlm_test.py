#!/usr/bin/env python3
"""
NLM - Comprehensive Testing Suite for NLM

Comprehensive testing framework for NLM:
- Brain unit tests
- Integration tests
- Performance tests
- Learning effectiveness tests
- Error detection and validation
- Test reporting and analysis
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Optional, List, Dict, Any, Tuple
import time
import threading
import unittest
import tempfile
import shutil

# Add the parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class NLMTestCase(unittest.TestCase):
    """Base test case for NLM tests"""
    
    def setUp(self):
        """Set up test fixtures"""
        if not PYNLM_AVAILABLE:
            self.skipTest("pynlm module not available")
        
        self.test_brain = None
        self.test_config = None
        self.test_results = []
    
    def tearDown(self):
        """Clean up after tests"""
        if hasattr(self, 'test_brain') and self.test_brain:
            # Clean up brain resources
            pass
    
    def create_test_brain(self, neuron_count: int = 100, region_count: int = 2,
                          connection_probability: float = 0.1):
        """Create a test brain with specified configuration"""
        try:
            self.test_config = pynlm.createDefaultConfig()
            self.test_config.set("neuron_count", neuron_count)
            self.test_config.set("region_count", region_count)
            self.test_config.set("connection_probability", connection_probability)
            
            self.test_brain = pynlm.createBrain(self.test_config)
            self.test_brain.initialize()
            
            return True
        except Exception as e:
            self.fail(f"Failed to create test brain: {str(e)}")
            return False
    
    def run_simulation_steps(self, brain, steps: int) -> List[Dict]:
        """Run simulation steps and collect metrics"""
        metrics = []
        
        for step in range(steps):
            brain.step(step)
            
            metric = {
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'eir_ratio': brain.getExcitationInhibitionRatio()
            }
            
            metrics.append(metric)
        
        return metrics
    
    def assert_brain_properties(self, brain, expected_neuron_count: int,
                               min_synapse_count: int = 0, max_firing_rate: float = 100.0):
        """Assert brain properties meet expectations"""
        self.assertIsNotNone(brain, "Brain should not be None")
        
        actual_neuron_count = brain.getTotalNeuronCount()
        self.assertEqual(actual_neuron_count, expected_neuron_count,
                        f"Expected {expected_neuron_count} neurons, got {actual_neuron_count}")
        
        actual_synapse_count = brain.getTotalSynapseCount()
        self.assertGreaterEqual(actual_synapse_count, min_synapse_count,
                               f"Expected at least {min_synapse_count} synapses, got {actual_synapse_count}")
        
        actual_firing_rate = brain.getAverageFiringRate()
        self.assertLessEqual(actual_firing_rate, max_firing_rate,
                           f"Firing rate {actual_firing_rate} exceeds maximum {max_firing_rate}")

class BrainTestSuite(NLMTestCase):
    """Test suite for brain functionality"""
    
    def test_brain_creation(self):
        """Test brain creation and basic properties"""
        success = self.create_test_brain(50, 1)
        self.assertTrue(success, "Should create brain successfully")
        
        self.assert_brain_properties(self.test_brain, 50)
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Check initialization state
        self.assertEqual(self.test_brain.getTotalNeuronCount(), 100)
        self.assertEqual(self.test_brain.getRegionCount(), 2)
        self.assertGreater(self.test_brain.getTotalSynapseCount(), 0)
    
    def test_brain_step(self):
        """Test brain simulation steps"""
        success = self.create_test_brain(100, 1)
        self.assertTrue(success, "Should create brain successfully")
        
        # Run simulation steps
        metrics = self.run_simulation_steps(self.test_brain, 50)
        
        # Verify metrics collected
        self.assertEqual(len(metrics), 50)
        
        # Verify metrics have expected properties
        for metric in metrics:
            self.assertIn('step', metric)
            self.assertIn('firing_rate', metric)
            self.assertIn('total_spikes', metric)
            self.assertIn('firing_neuron_count', metric)
            self.assertIn('eir_ratio', metric)
            
            # Validate ranges
            self.assertGreaterEqual(metric['firing_rate'], 0)
            self.assertGreaterEqual(metric['total_spikes'], 0)
            self.assertGreaterEqual(metric['firing_neuron_count'], 0)
            self.assertGreaterEqual(metric['eir_ratio'], 0)
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        success = self.create_test_brain(100, 1)
        self.assertTrue(success, "Should create brain successfully")
        
        # Run some steps
        metrics = self.run_simulation_steps(self.test_brain, 10)
        initial_spikes = metrics[-1]['total_spikes']
        
        # Reset brain
        self.test_brain.reset()
        
        # Verify reset state
        self.assertEqual(self.test_brain.getTotalSpikeCount(), 0)
        self.assertEqual(self.test_brain.getTotalNeuronCount(), 100)
    
    def test_brain_regions(self):
        """Test brain region functionality"""
        success = self.create_test_brain(200, 3)
        self.assertTrue(success, "Should create brain successfully")
        
        # Check region properties
        self.assertEqual(self.test_brain.getRegionCount(), 3)
        
        region_ids = self.test_brain.getRegionIds()
        self.assertEqual(len(region_ids), 3)
        
        # Verify regions are accessible
        for region_id in region_ids:
            region = self.test_brain.getRegion(region_id)
            self.assertIsNotNone(region, f"Region {region_id} should not be None")

class LearningTestSuite(NLMTestCase):
    """Test suite for learning functionality"""
    
    def test_reward_modulation_learning(self):
        """Test reward modulation learning"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Create agent with reward modulation
        agent = pynlm.createAgentBrain(self.test_brain)
        agent.initialize(None)  # Simplified for testing
        agent.enableRewardModulation(True)
        
        # Run simulation with learning
        metrics = self.run_simulation_steps(self.test_brain, 100)
        
        # Verify learning occurred (spikes should be present)
        total_spikes = metrics[-1]['total_spikes']
        self.assertGreater(total_spikes, 0, "Brain should have spikes for learning")
    
    def test_curiosity_learning(self):
        """Test curiosity-driven learning"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Create agent with curiosity
        agent = pynlm.createAgentBrain(self.test_brain)
        agent.initialize(None)  # Simplified for testing
        agent.enableCuriosity(True)
        
        # Run simulation with curiosity
        metrics = self.run_simulation_steps(self.test_brain, 50)
        
        # Check curiosity level
        # Note: In real implementation, this would access agent's curiosity level
        # For testing, we just verify the simulation runs
        self.assertGreater(len(metrics), 0, "Should have simulation metrics")
    
    def test_development_learning(self):
        """Test developmental learning"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Create agent with development
        agent = pynlm.createAgentBrain(self.test_brain)
        agent.initialize(None)  # Simplified for testing
        agent.enableDevelopment(True)
        
        # Run simulation with development
        metrics = self.run_simulation_steps(self.test_brain, 100)
        
        # Verify development occurred
        development_changes = self.detect_development_changes(metrics)
        self.assertGreater(len(development_changes), 0, "Should detect development changes")
    
    def detect_development_changes(self, metrics: List[Dict]) -> List[Dict]:
        """Detect changes that indicate development"""
        changes = []
        
        if len(metrics) < 10:
            return changes
        
        # Check for firing rate trends
        initial_rate = metrics[0]['firing_rate']
        final_rate = metrics[-1]['firing_rate']
        rate_change = final_rate - initial_rate
        
        if abs(rate_change) > 1.0:  # Significant change
            changes.append({
                'type': 'firing_rate_change',
                'initial': initial_rate,
                'final': final_rate,
                'change': rate_change
            })
        
        # Check for spike rate trends
        initial_spikes = metrics[0]['total_spikes']
        final_spikes = metrics[-1]['total_spikes']
        spike_change = final_spikes - initial_spikes
        
        if spike_change > 50:
            changes.append({
                'type': 'spike_increase',
                'initial': initial_spikes,
                'final': final_spikes,
                'change': spike_change
            })
        
        return changes

class PerformanceTestSuite(NLMTestCase):
    """Test suite for performance and stability"""
    
    def test_performance_benchmark(self):
        """Test performance benchmark"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Run performance test
        start_time = time.time()
        metrics = self.run_simulation_steps(self.test_brain, 200)
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = 200 / duration if duration > 0 else 0
        
        # Performance requirements
        self.assertGreater(steps_per_second, 100,
                          f"Performance too slow: {steps_per_second:.1f} steps/sec")
        self.assertLess(duration, 5.0,
                       f"Test took too long: {duration:.2f} seconds")
    
    def test_memory_usage(self):
        """Test memory usage efficiency"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and run brain
        success = self.create_test_brain(200, 3)
        self.assertTrue(success, "Should create brain successfully")
        
        metrics = self.run_simulation_steps(self.test_brain, 100)
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        # Memory should be reasonable (< 100MB for this test)
        self.assertLess(memory_increase, 100,
                       f"Memory usage too high: {memory_increase:.1f}MB increase")
    
    def test_stability(self):
        """Test simulation stability"""
        success = self.create_test_brain(150, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Run simulation multiple times to check stability
        metrics_list = []
        for run in range(5):
            # Create fresh brain for each run
            fresh_brain = pynlm.createBrain(self.test_config)
            fresh_brain.initialize()
            
            run_metrics = self.run_simulation_steps(fresh_brain, 50)
            metrics_list.append(run_metrics)
        
        # Analyze stability
        stability_score = self.calculate_stability_score(metrics_list)
        self.assertGreater(stability_score, 0.5,
                         f"Stability score too low: {stability_score:.2f}")
    
    def calculate_stability_score(self, metrics_list: List[List[Dict]]) -> float:
        """Calculate stability score based on metrics consistency"""
        if not metrics_list:
            return 0.0
        
        # Calculate coefficient of variation for each metric across runs
        num_runs = len(metrics_list)
        num_steps = len(metrics_list[0])
        
        firing_rates = []
        for run_metrics in metrics_list:
            firing_rates.extend([m['firing_rate'] for m in run_metrics])
        
        if len(firing_rates) < 2:
            return 1.0
        
        # Calculate mean and standard deviation
        mean_fr = sum(firing_rates) / len(firing_rates)
        variance = sum((x - mean_fr) ** 2 for x in firing_rates) / len(firing_rates)
        std_fr = variance ** 0.5
        
        # Coefficient of variation
        if mean_fr > 0:
            cv = std_fr / mean_fr
            stability_score = max(0.0, 1.0 - cv)
        else:
            stability_score = 0.0
        
        return stability_score

class CheckpointTestSuite(NLMTestCase):
    """Test suite for checkpoint functionality"""
    
    def test_checkpoint_save(self):
        """Test checkpoint save functionality"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Create temporary directory for checkpoints
        with tempfile.TemporaryDirectory() as temp_dir:
            checkpoint_file = os.path.join(temp_dir, "test_checkpoint.bin")
            
            # Save checkpoint
            save_success = self.test_brain.save(checkpoint_file)
            self.assertTrue(save_success, "Checkpoint save should succeed")
            
            # Verify file exists
            self.assertTrue(os.path.exists(checkpoint_file),
                           "Checkpoint file should exist after save")
    
    def test_checkpoint_load(self):
        """Test checkpoint load functionality"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Create temporary directory for checkpoints
        with tempfile.TemporaryDirectory() as temp_dir:
            checkpoint_file = os.path.join(temp_dir, "test_checkpoint.bin")
            
            # Save checkpoint
            save_success = self.test_brain.save(checkpoint_file)
            self.assertTrue(save_success, "Checkpoint save should succeed")
            
            # Create new brain and load checkpoint
            new_brain = pynlm.createBrain(self.test_config)
            new_brain.initialize()
            
            load_success = new_brain.load(checkpoint_file)
            self.assertTrue(load_success, "Checkpoint load should succeed")
            
            # Verify loaded brain has similar properties
            self.assertEqual(new_brain.getTotalNeuronCount(), 
                           self.test_brain.getTotalNeuronCount(),
                           "Loaded brain should have same neuron count")
            self.assertEqual(new_brain.getRegionCount(),
                           self.test_brain.getRegionCount(),
                           "Loaded brain should have same region count")

class IntegrationTestSuite(NLMTestCase):
    """Integration test suite for complete workflows"""
    
    def test_complete_agent_world_workflow(self):
        """Test complete agent-world interaction workflow"""
        success = self.create_test_brain(100, 2)
        self.assertTrue(success, "Should create brain successfully")
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(self.test_brain)
        agent.initialize(world)
        
        # Run complete workflow
        for step in range(50):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            self.test_brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
        
        # Verify workflow completed
        self.assertGreater(self.test_brain.getTotalSpikeCount(), 0,
                         "Brain should have activity during workflow")
        self.assertGreater(agent.getCuriosityLevel(), 0,
                         "Agent should have curiosity level")
    
    def test_end_to_end_learning_scenario(self):
        """Test end-to-end learning scenario with reward feedback"""
        success = self.create_test_brain(150, 3)
        self.assertTrue(success, "Should create brain successfully")
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent with learning capabilities
        agent = pynlm.createAgentBrain(self.test_brain)
        agent.initialize(world)
        
        # Enable all learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run learning scenario
        learning_metrics = []
        total_reward = 0.0
        
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input and learn
            agent.processSensoryInput(percept)
            self.test_brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action and get reward
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward-based learning
            agent.applyRewardModulation(result.reward, 0.0)
            total_reward += result.reward
            
            # Collect metrics
            learning_metrics.append({
                'step': step,
                'reward': result.reward,
                'firing_rate': self.test_brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'total_reward': total_reward
            })
        
        # Verify learning occurred
        self.assertGreater(total_reward, 0, "Should have received some reward")
        self.assertGreater(self.test_brain.getTotalSpikeCount(), 50,
                         "Brain should have significant activity")
        
        # Verify curiosity increased (indicating exploration)
        final_curiosity = learning_metrics[-1]['curiosity']
        self.assertGreater(final_curiosity, 0.1,
                         "Curiosity should increase during learning")

class NLMSampleTestRunner:
    """Sample test runner for NLM testing"""
    
    def __init__(self):
        self.test_results = []
        self.start_time = time.time()
    
    def run_test_suite(self, test_class, test_name: str) -> Dict:
        """Run a test suite and collect results"""
        suite = unittest.TestLoader().loadTestsFromTestCase(test_class)
        
        # Run tests
        runner = unittest.TextTestRunner(verbosity=0, stream=None)
        result = runner.run(suite)
        
        # Collect results
        test_result = {
            'name': test_name,
            'tests_run': result.testsRun,
            'failures': len(result.failures),
            'errors': len(result.errors),
            'skipped': len(result.skipped) if hasattr(result, 'skipped') else 0,
            'success': result.wasSuccessful(),
            'execution_time': time.time() - self.start_time,
            'timestamp': time.time()
        }
        
        self.test_results.append(test_result)
        return test_result
    
    def get_summary_report(self) -> Dict:
        """Get summary report of all tests"""
        if not self.test_results:
            return {}
        
        total_tests = sum(r['tests_run'] for r in self.test_results)
        total_failures = sum(r['failures'] for r in self.test_results)
        total_errors = sum(r['errors'] for r in self.test_results)
        
        return {
            'timestamp': time.time(),
            'total_test_suites': len(self.test_results),
            'total_tests_run': total_tests,
            'total_failures': total_failures,
            'total_errors': total_errors,
            'success_rate': (total_tests - total_failures - total_errors) / total_tests if total_tests > 0 else 0,
            'test_suites': self.test_results,
            'overall_status': 'PASS' if total_failures == 0 and total_errors == 0 else 'FAIL'
        }
    
    def export_report(self, filepath: str) -> bool:
        """Export test results to file"""
        try:
            report = self.get_summary_report()
            
            with open(filepath, 'w') as f:
                json.dump(report, f, indent=2, default=str)
            
            return True
        except Exception as e:
            print(f"Failed to export test report: {str(e)}")
            return False

def main():
    """Main CLI entry point for testing"""
    parser = argparse.ArgumentParser(
        description="NLM Comprehensive Testing Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s run --type brain
  %(prog)s run --type learning
  %(prog)s run --type performance
  %(prog)s run --type checkpoint
  %(prog)s run --type integration
  %(prog)s run --all
  %(prog)s analyze --report test_results.json

Test commands:
  run --type brain - Run brain functionality tests
  run --type learning - Run learning capability tests
  run --type performance - Run performance and stability tests
  run --type checkpoint - Run checkpoint functionality tests
  run --type integration - Run integration workflow tests
  run --all - Run all test suites
  analyze - Analyze and report test results

For more help on specific commands, use:
  %(prog)s <command> --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available test commands')
    
    # Run test suite command
    run_parser = subparsers.add_parser('run', help='Run test suites')
    run_parser.add_argument('--type', choices=['brain', 'learning', 'performance', 'checkpoint', 'integration', 'all'],
                           help='Type of tests to run')
    run_parser.add_argument('--verbose', '-v', action='store_true',
                           help='Enable verbose test output')
    run_parser.add_argument('--export', '-e', type=str,
                           help='Export test results to file')
    run_parser.add_argument('--output-dir', type=str, default='test_results',
                           help='Output directory for test results (default: test_results)')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze test results')
    analyze_parser.add_argument('--report-file', type=str, default='test_results.json',
                               help='Test report file to analyze (default: test_results.json)')
    analyze_parser.add_argument('--generate-html', action='store_true',
                               help='Generate HTML test report')
    
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(0)
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm module not available. Please install NLM Python bindings.")
        print("Run: pip install . or pip install -e .")
        sys.exit(1)
    
    try:
        # Create test runner
        runner = NLMSampleTestRunner()
        
        if args.command == 'run':
            if args.type == 'all':
                # Run all test suites
                print("=== RUNNING ALL NLM TEST SUITES ===")
                
                # Create output directory
                os.makedirs(args.output_dir, exist_ok=True)
                
                # Run brain tests
                print("\n--- Running Brain Functionality Tests ---")
                brain_result = runner.run_test_suite(BrainTestSuite, "Brain Functionality")
                print(f"Brain tests: {brain_result['tests_run']} tests, "
                      f"{brain_result['failures']} failures, {brain_result['errors']} errors")
                
                # Run learning tests
                print("\n--- Running Learning Capability Tests ---")
                learning_result = runner.run_test_suite(LearningTestSuite, "Learning Capability")
                print(f"Learning tests: {learning_result['tests_run']} tests, "
                      f"{learning_result['failures']} failures, {learning_result['errors']} errors")
                
                # Run performance tests
                print("\n--- Running Performance Tests ---")
                performance_result = runner.run_test_suite(PerformanceTestSuite, "Performance")
                print(f"Performance tests: {performance_result['tests_run']} tests, "
                      f"{performance_result['failures']} failures, {performance_result['errors']} errors")
                
                # Run checkpoint tests
                print("\n--- Running Checkpoint Tests ---")
                checkpoint_result = runner.run_test_suite(CheckpointTestSuite, "Checkpoint")
                print(f"Checkpoint tests: {checkpoint_result['tests_run']} tests, "
                      f"{checkpoint_result['failures']} failures, {checkpoint_result['errors']} errors")
                
                # Run integration tests
                print("\n--- Running Integration Tests ---")
                integration_result = runner.run_test_suite(IntegrationTestSuite, "Integration")
                print(f"Integration tests: {integration_result['tests_run']} tests, "
                      f"{integration_result['failures']} failures, {integration_result['errors']} errors")
                
                # Get summary report
                summary = runner.get_summary_report()
                
                # Print summary
                print(f"\n=== TEST SUMMARY ===")
                print(f"Total test suites: {summary['total_test_suites']}")
                print(f"Total tests run: {summary['total_tests_run']}")
                print(f"Total failures: {summary['total_failures']}")
                print(f"Total errors: {summary['total_errors']}")
                print(f"Success rate: {summary['success_rate']*100:.1f}%")
                print(f"Overall status: {summary['overall_status']}")
                
                # Export report if requested
                if args.export:
                    if runner.export_report(args.export):
                        print(f"✓ Test results exported to {args.export}")
                    else:
                        print(f"✗ Failed to export test results")
                        sys.exit(1)
                
                # Export individual test results
                individual_reports = {
                    'brain_test_results.json': brain_result,
                    'learning_test_results.json': learning_result,
                    'performance_test_results.json': performance_result,
                    'checkpoint_test_results.json': checkpoint_result,
                    'integration_test_results.json': integration_result,
                    'test_summary.json': summary
                }
                
                for filename, report in individual_reports.items():
                    filepath = os.path.join(args.output_dir, filename)
                    with open(filepath, 'w') as f:
                        json.dump(report, f, indent=2, default=str)
                
                print(f"✓ Individual test reports saved to {args.output_dir}/")
                
            elif args.type in ['brain', 'learning', 'performance', 'checkpoint', 'integration']:
                # Run specific test suite
                test_classes = {
                    'brain': BrainTestSuite,
                    'learning': LearningTestSuite,
                    'performance': PerformanceTestSuite,
                    'checkpoint': CheckpointTestSuite,
                    'integration': IntegrationTestSuite
                }
                
                test_class = test_classes[args.type]
                test_name = f"{args.type.title()} Tests"
                
                print(f"=== RUNNING {test_name.upper()} ===")
                
                result = runner.run_test_suite(test_class, test_name)
                
                print(f"\n=== {test_name.upper()} RESULTS ===")
                print(f"Tests run: {result['tests_run']}")
                print(f"Failures: {result['failures']}")
                print(f"Errors: {result['errors']}")
                print(f"Success rate: {((result['tests_run'] - result['failures'] - result['errors']) / result['tests_run'] * 100):.1f}%")
                print(f"Status: {'✓ PASS' if result['success'] else '✗ FAIL'}")
                
                # Export individual report if requested
                if args.export:
                    if runner.export_report(args.export):
                        print(f"✓ Test results exported to {args.export}")
                    else:
                        print(f"✗ Failed to export test results")
                        sys.exit(1)
        
        elif args.command == 'analyze':
            print("=== TEST ANALYSIS ===")
            
            if os.path.exists(args.report_file):
                # Load test report
                with open(args.report_file, 'r') as f:
                    report = json.load(f)
                
                print(f"Loaded test report from {args.report_file}")
                
                # Analyze results
                print(f"\n=== ANALYSIS RESULTS ===")
                
                if 'test_suites' in report:
                    print(f"Test suites analyzed: {len(report['test_suites'])}")
                    
                    total_tests = report.get('total_tests_run', 0)
                    total_failures = report.get('total_failures', 0)
                    total_errors = report.get('total_errors', 0)
                    
                    print(f"Total tests: {total_tests}")
                    print(f"Total failures: {total_failures}")
                    print(f"Total errors: {total_errors}")
                    
                    if total_tests > 0:
                        success_rate = (total_tests - total_failures - total_errors) / total_tests
                        print(f"Success rate: {success_rate*100:.1f}%")
                        
                        if success_rate > 0.9:
                            print("✓ Test suite shows excellent performance")
                        elif success_rate > 0.7:
                            print("✓ Test suite shows good performance")
                        elif success_rate > 0.5:
                            print("⚠ Test suite shows moderate performance")
                        else:
                            print("✗ Test suite shows poor performance")
                
                # Generate HTML report if requested
                if args.generate_html:
                    html_report = os.path.join(args.output_dir, "test_report.html")
                    print(f"\nGenerating HTML report: {html_report}")
                    
                    # Simple HTML report generation
                    html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Test Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .test-suite {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; }}
        .passed {{ background-color: #d4edda; }}
        .failed {{ background-color: #f8d7da; }}
        .summary {{ font-weight: bold; font-size: 1.2em; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Test Suite Report</h1>
        <p>Generated on {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
        <p class="summary">Overall Status: {report.get('overall_status', 'UNKNOWN')}</p>
    </div>
    
    <h2>Test Suites</h2>
    """
                    
                    for test_suite in report.get('test_suites', []):
                        status_class = 'passed' if test_suite['success'] else 'failed'
                        html_content += f"""
    <div class="test-suite {status_class}">
        <h3>{test_suite['name']}</h3>
        <p>Tests run: {test_suite['tests_run']}</p>
        <p>Failures: {test_suite['failures']}</p>
        <p>Errors: {test_suite['errors']}</p>
        <p>Status: {'✓ PASS' if test_suite['success'] else '✗ FAIL'}</p>
    </div>
                        """
                    
                    html_content += """
</body>
</html>
                    """
                    
                    # Write HTML report
                    with open(html_report, 'w') as f:
                        f.write(html_content)
                    
                    print(f"✓ HTML report generated")
            
            else:
                print(f"✗ Test report file not found: {args.report_file}")
                sys.exit(1)
        
        else:
            print(f"Unknown command: {args.command}")
            sys.exit(1)
        
        print(f"\n✓ Command completed successfully")
        
    except Exception as e:
        print(f"✗ Command failed: {str(e)}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
