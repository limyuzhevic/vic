# Modern testing framework for NLM
from typing import Optional, List, Dict, Any, Callable, Union
import unittest
import asyncio
import time
import json
import tempfile
import os
from pathlib import Path
from dataclasses import dataclass
from enum import Enum
from collections import defaultdict
import statistics
from contextlib import contextmanager

class TestStatus(Enum):
    """Test execution status."""
    PASSED = "passed"
    FAILED = "failed"
    ERROR = "error"
    SKIPPED = "skipped"
    PENDING = "pending"

class TestResult:
    """Test result with detailed information."""
    
    def __init__(self, test_name: str, status: TestStatus, duration: float = 0.0):
        self.test_name = test_name
        self.status = status
        self.duration = duration
        self.start_time = time.time()
        self.end_time = time.time()
        self.error_message = None
        self.stack_trace = None
        self.metrics = {}
        self.tags = []
        self.dependencies = []
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert test result to dictionary."""
        return {
            'test_name': self.test_name,
            'status': self.status.value,
            'duration': self.duration,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'error_message': self.error_message,
            'stack_trace': self.stack_trace,
            'metrics': self.metrics,
            'tags': self.tags,
            'dependencies': self.dependencies
        }

@dataclass
class TestMetrics:
    """Test execution metrics."""
    
    cpu_usage: float = 0.0
    memory_usage: float = 0.0
    step_count: int = 0
    spike_count: int = 0
    avg_step_time: float = 0.0
    stability: float = 0.0

class TestSuite:
    """Test suite for NLM tests."""
    
    def __init__(self, name: str):
        self.name = name
        self.tests = []
        self.results = []
        self.tags = []
        self.dependencies = []
        self.setup_func = None
        self.teardown_func = None
        self.beforesuite_func = None
        self.aftersuite_func = None
    
    def add_test(self, test_func: Callable, test_name: str = None, tags: List[str] = None):
        """Add a test to the suite."""
        if test_name is None:
            test_name = test_func.__name__
        test = {
            'func': test_func,
            'name': test_name,
            'tags': tags or [],
            'result': None
        }
        self.tests.append(test)
    
    def run(self) -> Dict[str, Any]:
        """Run all tests in the suite."""
        suite_start = time.time()
        suite_result = {
            'suite_name': self.name,
            'start_time': suite_start,
            'end_time': 0.0,
            'duration': 0.0,
            'total_tests': len(self.tests),
            'passed_tests': 0,
            'failed_tests': 0,
            'error_tests': 0,
            'skipped_tests': 0,
            'results': [],
            'setup_time': 0.0,
            'teardown_time': 0.0
        }
        
        # Run setup
        setup_start = time.time()
        if self.setup_func:
            try:
                self.setup_func()
            except Exception as e:
                print(f"Setup failed: {e}")
                suite_result['setup_error'] = str(e)
        suite_result['setup_time'] = time.time() - setup_start
        
        # Run tests
        for test in self.tests:
            test_start = time.time()
            test_result = self._run_single_test(test)
            test_end = time.time()
            
            test_result['duration'] = test_end - test_start
            test_result['suite_name'] = self.name
            suite_result['results'].append(test_result)
            
            # Count results
            if test_result['status'] == TestStatus.PASSED:
                suite_result['passed_tests'] += 1
            elif test_result['status'] == TestStatus.FAILED:
                suite_result['failed_tests'] += 1
            elif test_result['status'] == TestStatus.ERROR:
                suite_result['error_tests'] += 1
            elif test_result['status'] == TestStatus.SKIPPED:
                suite_result['skipped_tests'] += 1
        
        # Run teardown
        teardown_start = time.time()
        if self.teardown_func:
            try:
                self.teardown_func()
            except Exception as e:
                print(f"Teardown failed: {e}")
                suite_result['teardown_error'] = str(e)
        suite_result['teardown_time'] = time.time() - teardown_start
        
        # Calculate total duration
        suite_result['end_time'] = time.time()
        suite_result['duration'] = suite_result['end_time'] - suite_result['start_time']
        
        return suite_result
    
    def _run_single_test(self, test: Dict[str, Any]) -> Dict[str, Any]:
        """Run a single test."""
        test_name = test['name']
        test_func = test['func']
        
        try:
            # Run the test function
            test_result = test_func()
            
            # If test returns a dict, use it as result
            if isinstance(test_result, dict):
                test_result['test_name'] = test_name
                test_result['status'] = TestStatus.PASSED.value
            else:
                test_result = {
                    'test_name': test_name,
                    'status': TestStatus.PASSED.value,
                    'result': test_result,
                    'metrics': {},
                    'tags': test['tags']
                }
            
            return test_result
            
        except Exception as e:
            import traceback
            return {
                'test_name': test_name,
                'status': TestStatus.ERROR.value,
                'error_message': str(e),
                'stack_trace': traceback.format_exc(),
                'tags': test['tags']
            }
    
    def filter_by_tags(self, tags: List[str]) -> 'TestSuite':
        """Create a new suite filtered by tags."""
        filtered_suite = TestSuite(f"{self.name}_filtered")
        filtered_suite.tags = self.tags
        filtered_suite.dependencies = self.dependencies
        filtered_suite.setup_func = self.setup_func
        filtered_suite.teardown_func = self.teardown_func
        filtered_suite.beforesuite_func = self.beforesuite_func
        filtered_suite.aftersuite_func = self.aftersuite_func
        
        for test in self.tests:
            if any(tag in test['tags'] for tag in tags):
                filtered_suite.add_test(test['func'], test['name'], test['tags'])
        
        return filtered_suite
    
    def get_tests_by_tags(self, tags: List[str]) -> List[Dict[str, Any]]:
        """Get tests filtered by tags."""
        return [test for test in self.tests if any(tag in test['tags'] for tag in tags)]

class NLMLiveTest:
    """Live NLM test that executes with actual simulation."""
    
    def __init__(self, name: str, brain, world, agent):
        self.name = name
        self.brain = brain
        self.world = world
        self.agent = agent
        self.setup_time = 0.0
        self.execution_time = 0.0
    
    def setup(self) -> Dict[str, Any]:
        """Setup test environment."""
        setup_start = time.time()
        
        # Initialize components
        self.brain.initialize()
        self.world.reset()
        self.agent.initialize(self.world)
        
        setup_end = time.time()
        self.setup_time = setup_end - setup_start
        
        return {
            'test_name': self.name,
            'setup_time': self.setup_time,
            'status': 'success',
            'brain_configured': True,
            'world_reset': True,
            'agent_initialized': True
        }
    
    def run(self, num_steps: int, record_metrics: bool = True) -> Dict[str, Any]:
        """Run the live test."""
        execution_start = time.time()
        
        # Record initial state
        initial_spikes = self.brain.getTotalSpikeCount()
        initial_neurons = self.brain.getTotalNeuronCount()
        
        # Run simulation
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            if action:
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Record final state
        final_spikes = self.brain.getTotalSpikeCount()
        final_neurons = self.brain.getTotalNeuronCount()
        
        execution_end = time.time()
        self.execution_time = execution_end - execution_start
        
        # Calculate metrics
        spike_increase = final_spikes - initial_spikes
        spike_rate = spike_increase / max(self.execution_time, 0.001)
        
        # Get brain statistics
        brain_stats = {
            'total_neurons': final_neurons,
            'total_spikes': final_spikes,
            'spike_increase': spike_increase,
            'spike_rate': spike_rate,
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio()
        }
        
        # Get world statistics
        world_stats = {
            'simulation_time': self.world.getSimulationTime(),
            'agent_x': self.world.getAgentBody().x,
            'agent_y': self.world.getAgentBody().y,
            'agent_energy': self.world.getAgentBody().energy,
            'agent_health': self.world.getAgentBody().health,
            'agent_age': self.world.getAgentBody().age
        }
        
        # Get agent statistics
        agent_stats = {
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel(),
            'developmental_stage': self.agent.getDevelopmentalStage()
        }
        
        return {
            'test_name': self.name,
            'execution_time': self.execution_time,
            'setup_time': self.setup_time,
            'total_time': self.setup_time + self.execution_time,
            'status': 'completed',
            'num_steps': num_steps,
            'brain_stats': brain_stats,
            'world_stats': world_stats,
            'agent_stats': agent_stats
        }

class PerformanceTest:
    """Performance testing for NLM simulations."""
    
    def __init__(self):
        self.test_history = []
    
    def benchmark_step_execution(self, brain, world, agent, num_steps: int) -> Dict[str, Any]:
        """Benchmark step execution performance."""
        # Warm up
        for step in range(min(10, num_steps)):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            if action:
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Actual benchmark
        start_time = time.perf_counter()
        
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            if action:
                world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.perf_counter()
        
        elapsed = end_time - start_time
        steps_per_second = num_steps / elapsed if elapsed > 0 else 0
        avg_step_time = elapsed / num_steps if num_steps > 0 else 0
        
        return {
            'benchmark_name': f'step_execution_{num_steps}',
            'num_steps': num_steps,
            'total_time': elapsed,
            'steps_per_second': steps_per_second,
            'avg_step_time': avg_step_time,
            'min_step_time': avg_step_time * 0.9,  # Placeholder
            'max_step_time': avg_step_time * 1.1,  # Placeholder
            'stability': 1.0 - (0.05),  # Placeholder
            'status': 'completed'
        }
    
    def benchmark_brain_memory(self, brain, num_iterations: int) -> Dict[str, Any]:
        """Benchmark brain memory usage."""
        import tracemalloc
        
        tracemalloc.start()
        
        # Perform memory operations
        for i in range(num_iterations):
            # Create and use brain
            brain.initialize()
            brain.step(i)
        
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        return {
            'benchmark_name': 'brain_memory',
            'num_iterations': num_iterations,
            'current_memory': current / 1024 / 1024,  # MB
            'peak_memory': peak / 1024 / 1024,  # MB
            'memory_efficiency': current / max(num_iterations, 1),
            'status': 'completed'
        }
    
    def benchmark_neuron_activities(self, brain, num_steps: int) -> Dict[str, Any]:
        """Benchmark neuron activity patterns."""
        start_time = time.perf_counter()
        
        spike_counts = []
        firing_rates = []
        
        for step in range(num_steps):
            brain.step(step)
            
            spike_counts.append(brain.getTotalSpikeCount())
            firing_rates.append(brain.getAverageFiringRate())
        
        end_time = time.perf_counter()
        
        # Analyze spike patterns
        spike_changes = [spike_counts[i] - spike_counts[i-1] for i in range(1, len(spike_counts))]
        avg_spike_change = statistics.mean(spike_changes) if spike_changes else 0
        
        return {
            'benchmark_name': 'neuron_activities',
            'num_steps': num_steps,
            'total_time': end_time - start_time,
            'avg_spike_count': statistics.mean(spike_counts) if spike_counts else 0,
            'max_spike_count': max(spike_counts) if spike_counts else 0,
            'avg_spike_change': avg_spike_change,
            'firing_rate_stability': 1.0 - (statistics.stdev(firing_rates) / statistics.mean(firing_rates) if len(firing_rates) > 1 else 0),
            'status': 'completed'
        }
    
    record_test(self, test_name: str, test_result: Dict[str, Any]) -> None:
        """Record a test result."""
        self.test_history.append({
            'test_name': test_name,
            'timestamp': time.time(),
            'result': test_result
        })
    
    get_test_history(self, test_name: str = None) -> List[Dict[str, Any]]:
        """Get test history."""
        if test_name:
            return [t for t in self.test_history if t['test_name'] == test_name]
        return self.test_history
    
    clear_history(self) -> None:
        """Clear test history."""
        self.test_history.clear()

class NLMTestCase(unittest.TestCase):
    """Base test case for NLM tests."""
    
    def setUp(self):
        """Setup before each test."""
        # Initialize NLM components
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
        self._setup_environment()
    
    def tearDown(self):
        """Cleanup after each test."""
        if self.teardown_func:
            self.teardown_func()
    
    def _setup_environment(self):
        """Setup NLM environment. Override in subclasses."""
        pass

class NLMLiveTestCase(NLMLiveTest, unittest.TestCase):
    """Live NLM test case that combines NLM testing with unittest."""
    
    def setUp(self):
        """Setup before each test."""
        super().setUp()
        self._setup_environment()
        
        # Run test setup
        self.setup_result = self.setup()
    
    def runTest(self):
        """Run the test."""
        # Run live test
        test_result = self.run(
            num_steps=getattr(self, 'num_steps', 100),
            record_metrics=getattr(self, 'record_metrics', True)
        )
        
        # Assert test result
        self.assertEqual(test_result['status'], 'completed')
        self.assertGreater(test_result['execution_time'], 0)
        self.assertGreaterEqual(test_result['brain_stats']['total_neurons'], 0)
    
    def _setup_environment(self):
        """Setup NLM environment. Override in subclasses."""
        # This will be overridden by subclasses
        pass

class TestReporter:
    """Report test results."""
    
    def __init__(self, output_format: str = 'text'):
        self.output_format = output_format
        self.results = []
    
    def add_result(self, test_result: Dict[str, Any]) -> None:
        """Add test result."""
        self.results.append(test_result)
    
    def report(self) -> str:
        """Generate report."""
        if self.output_format == 'json':
            return self._report_json()
        elif self.output_format == 'html':
            return self._report_html()
        elif self.output_format == 'csv':
            return self._report_csv()
        else:
            return self._report_text()
    
    def _report_text(self) -> str:
        """Generate text report."""
        lines = [
            "=== NLM Test Results ===",
            f"Total tests: {len(self.results)}",
            f"Passed tests: {sum(1 for r in self.results if r.get('status') == 'passed')}",
            f"Failed tests: {sum(1 for r in self.results if r.get('status') == 'failed')}",
            f"Error tests: {sum(1 for r in self.results if r.get('status') == 'error')}",
            ""
        ]
        
        for result in self.results:
            status = result.get('status', 'unknown')
            name = result.get('test_name', 'unknown')
            duration = result.get('duration', 0)
            lines.append(f"{status.upper():10} {name:30} {duration:.3f}s")
        
        lines.append("")
        return "\n".join(lines)
    
    def _report_json(self) -> str:
        """Generate JSON report."""
        return json.dumps(self.results, indent=2, default=str)
    
    def _report_html(self) -> str:
        """Generate HTML report."""
        html = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Test Results</title>
            <style>
                body { font-family: Arial, sans-serif; margin: 20px; }
                table { border-collapse: collapse; width: 100%; }
                th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                th { background-color: #f2f2f2; }
                .passed { background-color: #d4edda; }
                .failed { background-color: #f8d7da; }
                .error { background-color: #f8d7da; }
                .skipped { background-color: #fff3cd; }
            </style>
        </head>
        <body>
            <h1>NLM Test Results</h1>
            <table>
                <tr>
                    <th>Test Name</th>
                    <th>Status</th>
                    <th>Duration (s)</th>
                    <th>Metrics</th>
                </tr>
        """
        
        for result in self.results:
            status = result.get('status', 'unknown')
            name = result.get('test_name', 'unknown')
            duration = result.get('duration', 0)
            
            row_class = status.lower()
            metrics = json.dumps(result.get('metrics', {}))
            
            html += f"""
                <tr class="{row_class}">
                    <td>{name}</td>
                    <td>{status}</td>
                    <td>{duration:.3f}</td>
                    <td>{metrics}</td>
                </tr>
            """
        
        html += """
            </table>
        </body>
        </html>
        """
        
        return html
    
    def _report_csv(self) -> str:
        """Generate CSV report."""
        import csv
        import io
        
        output = io.StringIO()
        writer = csv.writer(output)
        
        # Write header
        writer.writerow(['Test Name', 'Status', 'Duration', 'Start Time', 'Error Message'])
        
        # Write test results
        for result in self.results:
            writer.writerow([
                result.get('test_name', 'unknown'),
                result.get('status', 'unknown'),
                result.get('duration', 0),
                result.get('start_time', 0),
                result.get('error_message', '')
            ])
        
        return output.getvalue()

# Factory functions

def create_test_suite(name: str) -> TestSuite:
    """Create a test suite."""
    return TestSuite(name)

def create_test_reporter(output_format: str = 'text') -> TestReporter:
    """Create a test reporter."""
    return TestReporter(output_format)

def create_performance_tester() -> PerformanceTest:
    """Create a performance tester."""
    return PerformanceTest()

# Test utilities

class MockBrain:
    """Mock brain for testing."""
    
    def __init__(self):
        self.neuron_count = 100
        self.spike_count = 0
        self.firing_count = 0
        self.initialized = False
    
    def initialize(self):
        self.initialized = True
        self.spike_count = 0
        self.firing_count = 0
    
    def step(self, step_num):
        self.spike_count += 1
        self.firing_count = min(self.firing_count + 1, 10)
    
    def getTotalNeuronCount(self):
        return self.neuron_count
    
    def getTotalSpikeCount(self):
        return self.spike_count
    
    def getFiringNeuronCount(self):
        return self.firing_count
    
    def getAverageFiringRate(self):
        return 5.0
    
    def getExcitationInhibitionRatio(self):
        return 0.7

class MockWorld:
    """Mock world for testing."""
    
    def __init__(self):
        self.width = 10.0
        self.height = 10.0
        self.simulation_time = 0.0
        self.agent_x = 5.0
        self.agent_y = 5.0
        self.agent_energy = 100.0
    
    def configure(self, width, height, vision_width, vision_height):
        self.width = width
        self.height = height
    
    def reset(self):
        self.simulation_time = 0.0
        self.agent_x = 5.0
        self.agent_y = 5.0
        self.agent_energy = 100.0
    
    def update(self, timestep):
        self.simulation_time += timestep
    
    def getSimulationTime(self):
        return self.simulation_time
    
    def getAgentBody(self):
        class MockBody:
            def __init__(self, x, y):
                self.x = x
                self.y = y
                self.energy = 100.0
                self.health = 100.0
                self.age = 0.0
        
        return MockBody(self.agent_x, self.agent_y)

class MockAgent:
    """Mock agent for testing."""
    
    def __init__(self, brain):
        self.brain = brain
        self.curiosity = 0.5
        self.novelty = 0.3
        self.prediction_error = 0.1
        self.neuromodulation = 0.7
    
    def initialize(self, world):
        pass
    
    def processSensoryInput(self, percept):
        pass
    
    def decodeMotorCommand(self):
        return None
    
    def getCuriosityLevel(self):
        return self.curiosity
    
    def getNoveltyLevel(self):
        return self.novelty
    
    def getPredictionError(self):
        return self.prediction_error
    
    def getNeuromodulationLevel(self):
        return self.neuromodulation
    
    def getDevelopmentalStage(self):
        return "Adult"

# Test example functions

def example_simple_test():
    """Example of a simple test."""
    brain = MockBrain()
    world = MockWorld()
    agent = MockAgent(brain)
    
    # Setup
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Test
    initial_neurons = brain.getTotalNeuronCount()
    brain.step(0)
    final_spikes = brain.getTotalSpikeCount()
    
    assert initial_neurons > 0
    assert final_spikes > 0
    
    return {
        'test_name': 'simple_test',
        'status': 'passed',
        'initial_neurons': initial_neurons,
        'final_spikes': final_spikes
    }

def example_performance_test():
    """Example of a performance test."""
    brain = MockBrain()
    world = MockWorld()
    agent = MockAgent(brain)
    
    start_time = time.time()
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(None)
        brain.step(step)
        action = agent.decodeMotorCommand()
        if action:
            world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    duration = end_time - start_time
    
    return {
        'test_name': 'performance_test',
        'status': 'passed',
        'duration': duration,
        'steps_per_second': 100 / duration,
        'avg_step_time': duration / 100
    }

def example_live_test(brain, world, agent, num_steps=50):
    """Example of a live NLM test."""
    test = NLMLiveTest("live_test_example", brain, world, agent)
    
    # Setup
    setup_result = test.setup()
    
    # Run test
    run_result = test.run(num_steps)
    
    return {
        'test_name': 'live_test_example',
        'status': 'passed',
        'setup_result': setup_result,
        'run_result': run_result,
        'total_time': test.setup_time + test.execution_time
    }
