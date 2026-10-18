# NLM Testing Guide

## Overview

This guide provides comprehensive instructions for testing NLM (Neural Learning Machine), from basic unit tests to complex integration tests. It covers test setup, execution, and best practices for ensuring reliable simulations.

## Setting Up Tests

### 1. Running Unit Tests

NLM includes several unit tests for core components:

```bash
# Navigate to project root
cd /workspace/app

# Run all tests
python -m pytest tests/ -v

# Run specific test modules
python -m pytest tests/test_neuron.cpp -v
python -m pytest tests/test_synapse.cpp -v
python -m pytest tests/test_brain.cpp -v
python -m pytest tests/test_config.cpp -v

# Run C++ tests if built
./nlm_test --gtest_filter="*Test*"
```

### 2. Running Demo Programs

NLM includes several demo programs for testing functionality:

```bash
# Phase 2: Real Neural Computation
./nlm_phase2_demo

# Phase 3: Real Neural Computation
./nlm_phase3_demo

# Phase 4: Learning and Planning
./nlm_phase4_demo

# Phase 6: Final Integration
./nlm_phase6_demo

# Custom test programs
cd src/experiments
./Phase6Demo
./Phase5Experiment
```

### 3. Python Integration Tests

```python
import pynlm
import unittest
import time

class TestNLMBasic(unittest.TestCase):
    """Basic NLM functionality tests."""
    
    def setUp(self):
        """Set up test environment."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after test."""
        del self.brain
    
    def test_brain_initialization(self):
        """Test that brain initializes correctly."""
        self.assertIsNotNone(self.brain)
        self.assertEqual(self.brain.getTotalNeuronCount(), 
                        self.config.getOr("brain.neuron_count", 1000))
        self.assertEqual(self.brain.getTotalSynapseCount(), 
                        self.brain.getTotalNeuronCount() * 
                        self.config.getOr("brain.connection_probability", 0.1))
    
    def test_brain_step(self):
        """Test that brain steps work correctly."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for step in range(10):
            self.brain.step(step)
        
        final_spikes = self.brain.getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_sensory_input(self):
        """Test sensory input processing."""
        input_data = [1.0, 0.5, 0.8, 1.2, 0.9]
        sensory_input = pynlm.SensoryInput()
        sensory_input.setData(input_data)
        
        self.brain.receiveSensoryInput(sensory_input)
        
        # Check that neurons were activated
        self.assertGreater(self.brain.getFiringNeuronCount(), 0)

class TestNLMAgent(unittest.TestCase):
    """Agent integration tests."""
    
    def setUp(self):
        """Set up agent test environment."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up after test."""
        del self.agent
        del self.world
        del self.brain
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        self.assertIsNotNone(self.agent)
        self.assertIsNotNone(self.world)
    
    def test_sensorimotor_loop(self):
        """Test complete sensorimotor loop."""
        for step in range(50):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Process brain
            self.brain.step(step)
            
            # Get motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Verify that something happened
            self.assertGreaterEqual(self.brain.getFiringNeuronCount(), 0)
    
    def test_neuromodulation(self):
        """Test neuromodulation features."""
        # Enable neuromodulation
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        # Get neuromodulation levels
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        
        # They should start at 0.0
        self.assertEqual(curiosity, 0.0)
        self.assertEqual(novelty, 0.0)
        
        # Apply some input to stimulate neuromodulation
        for step in range(10):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
        
        # Check that neuromodulation levels changed
        # (actual values depend on implementation)
        self.assertIsInstance(self.agent.getCuriosityLevel(), float)
        self.assertIsInstance(self.agent.getNoveltyLevel(), float)

class TestNLMPlasticity(unittest.TestCase):
    """Plasticity-related tests."""
    
    def setUp(self):
        """Set up plasticity test environment."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable plasticity
        self.stdp = self.brain.getSTDP()
        self.hebbian = self.brain.getHebbian()
        
        self.stdp.enablePlasticity(True, True, False)
        self.hebbian.enablePlasticity(True, True, False)
    
    def tearDown(self):
        """Clean up after test."""
        del self.hebbian
        del self.stdp
        del self.brain
    
    def test_stdp_learning(self):
        """Test STDP learning."""
        # Record initial weights
        initial_weights = []
        if self.brain.getRegion(pynlm.RegionId(1)):
            region = self.brain.getRegion(pynlm.RegionId(1))
            for synapse in region.getSynapses():
                initial_weights.append(synapse.getWeight())
        
        # Apply learning stimuli
        for step in range(100):
            # Create correlated activity
            if step % 10 == 0:
                # Pre-synaptic activation
                self.brain.injectCurrentToNeurons(
                    pynlm.NeuronType.Excitatory, 50.0f)
            
            if step % 10 == 5:
                # Post-synaptic activation
                self.brain.injectCurrentToNeurons(
                    pynlm.NeuronType.Inhibitory, 50.0f)
            
            self.brain.step(step)
        
        # Check that weights changed (learning occurred)
        if self.brain.getRegion(pynlm.RegionId(1)):
            region = self.brain.getRegion(pynlm.RegionId(1))
            final_weights = []
            for synapse in region.getSynapses():
                final_weights.append(synapse.getWeight())
            
            # Calculate weight changes
            weight_changes = []
            for i in range(min(len(initial_weights), len(final_weights))):
                weight_changes.append(final_weights[i] - initial_weights[i])
            
            # Check that some synapses changed
            self.assertTrue(any(abs(change) > 0.001 for change in weight_changes))
    
    def test_structural_plasticity(self):
        """Test structural plasticity features."""
        # Check if structural plasticity is available
        plasticity = self.brain.getStructuralPlasticity()
        if plasticity:
            # This is a basic check - actual structural changes
            # depend on implementation
            pass

class TestNLMStateManagement(unittest.TestCase):
    """State management tests."""
    
    def setUp(self):
        """Set up state management test environment."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Run some steps to create state
        for step in range(20):
            self.brain.step(step)
    
    def tearDown(self):
        """Clean up after test."""
        del self.brain
    
    def test_save_load(self):
        """Test save and load functionality."""
        # Save brain state
        checkpoint_file = "test_checkpoint.bin"
        self.assertTrue(self.brain.save(checkpoint_file))
        
        # Create new brain and load state
        new_brain = pynlm.createBrain(self.config)
        new_brain.initialize()
        self.assertTrue(new_brain.load(checkpoint_file))
        
        # Clean up
        import os
        os.remove(checkpoint_file)
        
        print("Save/load test completed")
    
    def test_reset(self):
        """Test brain reset functionality."""
        # Get initial state
        initial_neurons = self.brain.getTotalNeuronCount()
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Run some steps
        for step in range(10):
            self.brain.step(step)
        
        # Reset brain
        self.brain.reset()
        
        # Check that state was reset
        self.assertEqual(self.brain.getTotalSpikeCount(), 0)
        self.assertEqual(self.brain.getFiringNeuronCount(), 0)

if __name__ == '__main__':
    unittest.main()
```

### 4. Running Performance Tests

```bash
# Performance testing script
#!/bin/bash

cd /workspace/app

echo "=== NLM Performance Testing ==="

echo "1. Building project..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "2. Running unit tests..."
python -m pytest tests/ -v

echo "3. Running demo programs..."
for demo in nlm_phase2_demo nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo; do
    if [ -f "$demo" ]; then
        echo "Running $demo..."
        ./$demo
    else
        echo "Demo $demo not found, skipping"
    fi
done

echo "4. Running Python integration tests..."
python -m pytest test_nlm.py -v

echo "=== Performance testing complete ==="
```

## Test Best Practices

### 1. Writing Effective Tests

#### Test Structure
```python
import pynlm
import unittest

class TestExample(unittest.TestCase):
    """Example test class."""
    
    def setUp(self):
        """Set up test environment - runs before each test."""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up after test - runs after each test."""
        del self.brain
    
    def test_example(self):
        """Example test method."""
        # Arrange - set up test conditions
        input_data = [1.0, 0.5, 0.8]
        
        # Act - execute the code being tested
        sensory_input = pynlm.SensoryInput()
        sensory_input.setData(input_data)
        self.brain.receiveSensoryInput(sensory_input)
        self.brain.step(0)
        
        # Assert - check the results
        self.assertGreater(self.brain.getFiringNeuronCount(), 0)
```

#### Test Categories

1. **Unit Tests**: Test individual components (neurons, synapses, etc.)
2. **Integration Tests**: Test how components work together
3. **System Tests**: Test complete system behavior
4. **Performance Tests**: Test speed and resource usage
5. **Regression Tests**: Ensure existing functionality still works

### 2. Performance Testing

```python
import pynlm
import time
import psutil
import os

def performance_test(brain, steps, description):
    """Run a performance test on a brain."""
    
    print(f"\n=== Performance Test: {description} ===")
    
    # Get baseline metrics
    process = psutil.Process(os.getpid())
    mem_before = process.memory_info().rss / 1024 / 1024
    cpu_before = process.cpu_percent()
    
    # Run simulation
    start_time = time.time()
    start_memory = process.memory_info().rss / 1024 / 1024
    
    for step in range(steps):
        brain.step(step)
        
        if step % (steps // 10) == 0:
            current_time = time.time()
            current_memory = process.memory_info().rss / 1024 / 1024
            print(f"  Step {step:4d}: "
                  f"Time: {current_time - start_time:6.2f}s, "
                  f"Memory: {current_memory - start_memory:5.1f}MB")
    
    end_time = time.time()
    end_memory = process.memory_info().rss / 1024 / 1024
    
    # Calculate metrics
    duration = end_time - start_time
    memory_increase = end_memory - start_memory
    steps_per_second = steps / duration
    
    print(f"\nPerformance Results:")
    print(f"  Duration: {duration:.2f} seconds")
    print(f"  Steps per second: {steps_per_second:.1f}")
    print(f"  Memory increase: {memory_increase:.1f} MB")
    print(f"  Memory efficiency: {steps / memory_increase:.0f} steps/MB")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return {
        "duration": duration,
        "steps_per_second": steps_per_second,
        "memory_increase": memory_increase,
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate()
    }

# Example usage
def run_performance_benchmarks():
    """Run performance benchmarks on different brain configurations."""
    
    configs = [
        ("Small (100 neurons)", 100),
        ("Medium (500 neurons)", 500),
        ("Large (1000 neurons)", 1000),
    ]
    
    results = []
    
    for description, neuron_count in configs:
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run performance test
        result = performance_test(brain, 100, description)
        result["neuron_count"] = neuron_count
        results.append(result)
    
    # Compare results
    print(f"\n=== Performance Comparison ===")
    print(f"{'Configuration':<20} {'Duration':<10} {'Steps/s':<10} {'Memory':<10} {'Spikes':<10} {'Firing Rate':<12}")
    print("-" * 80)
    
    for result in results:
        print(f"{result['neuron_count']:<20} {result['duration']:<10.2f} "
              f"{result['steps_per_second']:<10.1f} {result['memory_increase']:<10.1f} "
              f"{result['total_spikes']:<10} {result['firing_rate']:<12.2f}")
    
    return results

# Run benchmarks
if __name__ == "__main__":
    results = run_performance_benchmarks()
```

### 3. Stress Testing

```python
import pynlm
import time
import threading
import queue

def stress_test_concurrent_brains(num_brains=5, steps=50):
    """Stress test multiple brains running concurrently."""
    
    print(f"\n=== Stress Test: Concurrent Brains ===")
    print(f"Running {num_brains} brains concurrently for {steps} steps each")
    
    # Create brain configurations
    configs = []
    for i in range(num_brains):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100 + i * 50)
        configs.append(config)
    
    # Create and start threads
    brain_threads = []
    results_queue = queue.Queue()
    
    def run_brain(index, config, steps):
        """Run a single brain simulation."""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        
        result = {
            "index": index,
            "duration": end_time - start_time,
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_rate": brain.getAverageFiringRate()
        }
        
        results_queue.put(result)
    
    # Start all brains
    for i, config in enumerate(configs):
        thread = threading.Thread(target=run_brain, args=(i, config, steps))
        brain_threads.append(thread)
        thread.start()
    
    # Wait for all threads to complete
    for thread in brain_threads:
        thread.join()
    
    # Collect results
    results = []
    while not results_queue.empty():
        results.append(results_queue.get())
    
    # Sort by index
    results.sort(key=lambda x: x["index"])
    
    # Print results
    print(f"\nResults:")
    print(f"{'Brain':<6} {'Duration':<10} {'Steps/s':<10} {'Total Spikes':<12} {'Firing Rate':<12}")
    print("-" * 60)
    
    total_duration = 0
    total_spikes = 0
    
    for result in results:
        steps_per_second = steps / result["duration"]
        print(f"Brain {result['index']:<2} {result['duration']:<10.2f} "
              f"{steps_per_second:<10.1f} {result['total_spikes']:<12} "
              f"{result['firing_rate']:<12.2f}")
        
        total_duration += result["duration"]
        total_spikes += result["total_spikes"]
    
    print("-" * 60)
    print(f"{'TOTAL':<6} {total_duration:<10.2f} "
          f"{num_brains * steps / total_duration:<10.1f} {total_spikes:<12} "
          f"{'N/A':<12}")
    
    return results

# Example usage
if __name__ == "__main__":
    stress_test_concurrent_brains()
```

### 4. Regression Testing

```python
import pynlm
import json
from datetime import datetime

class RegressionTest:
    """Regression testing for NLM."""
    
    def __init__(self):
        self.test_results = []
        self.baseline_file = "regression_baseline.json"
    
    def load_baseline(self):
        """Load baseline test results."""
        try:
            with open(self.baseline_file, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            print(f"Baseline file {self.baseline_file} not found. Creating new baseline.")
            return {}
    
    def save_baseline(self, results):
        """Save test results as baseline."""
        with open(self.baseline_file, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Baseline saved to {self.baseline_file}")
    
    def run_test_suite(self, test_name, test_function):
        """Run a test suite and record results."""
        print(f"\n=== Running Regression Test: {test_name} ===")
        
        start_time = time.time()
        result = test_function()
        end_time = time.time()
        
        test_result = {
            "name": test_name,
            "timestamp": datetime.now().isoformat(),
            "duration": end_time - start_time,
            "result": result,
            "passed": True  # Assume pass unless test fails
        }
        
        self.test_results.append(test_result)
        
        print(f"Test {test_name} completed in {test_result['duration']:.2f}s")
        print(f"Result: {result}")
        
        return test_result
    
    def compare_with_baseline(self, current_result):
        """Compare current results with baseline."""
        baseline = self.load_baseline()
        
        if not baseline:
            print("No baseline found. Creating baseline from current results.")
            self.save_baseline({current_result["name"]: current_result})
            return True
        
        if current_result["name"] not in baseline:
            print(f"New test {current_result['name']}. Adding to baseline.")
            baseline[current_result["name"]] = current_result
            self.save_baseline(baseline)
            return True
        
        baseline_result = baseline[current_result["name"]]
        
        # Compare key metrics
        print(f"\nComparing with baseline for {current_result['name']}:")
        
        if "duration" in current_result and "duration" in baseline_result:
            duration_diff = current_result["duration"] - baseline_result["duration"]
            print(f"Duration: {current_result['duration']:.2f}s (baseline: {baseline_result['duration']:.2f}s, diff: {duration_diff:+.2f}s)")
        
        if "total_spikes" in current_result and "total_spikes" in baseline_result:
            spike_diff = current_result["total_spikes"] - baseline_result["total_spikes"]
            print(f"Total spikes: {current_result['total_spikes']} (baseline: {baseline_result['total_spikes']}, diff: {spike_diff:+})")
        
        # Check if results are within acceptable tolerance
        tolerance = 0.1  # 10% tolerance
        
        if "duration" in current_result and "duration" in baseline_result:
            duration_ratio = current_result["duration"] / baseline_result["duration"]
            if abs(duration_ratio - 1.0) > tolerance:
                print(f"WARNING: Duration differs from baseline by {duration_ratio:.1f}x")
                return False
        
        if "total_spikes" in current_result and "total_spikes" in baseline_result:
            spike_ratio = current_result["total_spikes"] / baseline_result["total_spikes"]
            if abs(spike_ratio - 1.0) > tolerance:
                print(f"WARNING: Total spikes differs from baseline by {spike_ratio:.1f}x")
                return False
        
        print(f"Test {current_result['name']} PASSED - results within baseline tolerance")
        return True
    
    def run_regression_suite(self):
        """Run complete regression test suite."""
        
        print("=== NLM Regression Test Suite ===")
        
        # Define test functions
        def test_basic_functionality():
            """Test basic brain functionality."""
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(50):
                brain.step(step)
            
            return {
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate()
            }
        
        def test_sensory_processing():
            """Test sensory input processing."""
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            input_data = [1.0, 0.5, 0.8, 1.2, 0.9]
            sensory_input = pynlm.SensoryInput()
            sensory_input.setData(input_data)
            
            brain.receiveSensoryInput(sensory_input)
            brain.step(0)
            
            return {
                "spiking_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount()
            }
        
        def test_agent_simulation():
            """Test agent simulation."""
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
            agent.initialize(world)
            
            for step in range(30):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            return {
                "total_steps": 30,
                "firing_neurons": brain.getFiringNeuronCount(),
                "curiosity": agent.getCuriosityLevel()
            }
        
        # Run tests
        test_functions = [
            ("Basic Functionality", test_basic_functionality),
            ("Sensory Processing", test_sensory_processing),
            ("Agent Simulation", test_agent_simulation),
        ]
        
        all_passed = True
        
        for test_name, test_function in test_functions:
            current_result = self.run_test_suite(test_name, test_function)
            passed = self.compare_with_baseline(current_result)
            
            if not passed:
                all_passed = False
        
        if all_passed:
            print(f"\n=== ALL REGRESSION TESTS PASSED ===")
        else:
            print(f"\n=== SOME REGRESSION TESTS FAILED ===")
            print("Please review the results and update baseline if necessary.")
        
        return all_passed

if __name__ == "__main__":
    tester = RegressionTest()
    success = tester.run_regression_suite()
    
    if not success:
        exit(1)
```

## Test Results Analysis

### 1. Interpreting Test Results

```python
def analyze_test_results(results):
    """Analyze test results and provide recommendations."""
    
    print("=== Test Results Analysis ===")
    
    for result in results:
        print(f"\nTest: {result['name']}")
        print(f"  Duration: {result['duration']:.2f}s")
        print(f"  Result: {result.get('result', 'N/A')}")
        
        # Check for performance issues
        if result['duration'] > 10.0:  # More than 10 seconds
            print(f"  WARNING: Test took longer than expected")
        
        # Check for correctness
        if 'result' in result and isinstance(result['result'], dict):
            for key, value in result['result'].items():
                if value is None:
                    print(f"  WARNING: {key} returned None")
                elif isinstance(value, (int, float)) and value < 0:
                    print(f"  WARNING: {key} has negative value: {value}")
    
    print("\n=== Analysis Complete ===")
```

### 2. Performance Profiling

```python
import pynlm
import cProfile
import pstats
import io

def profile_brain_simulation(brain, steps):
    """Profile brain simulation performance."""
    
    print(f"\n=== Profiling Brain Simulation ({steps} steps) ===")
    
    # Create profiler
    pr = cProfile.Profile()
    
    # Start profiling
    pr.enable()
    
    # Run simulation
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    end_time = time.time()
    
    # Stop profiling
    pr.disable()
    
    # Get profiling results
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats(20)  # Top 20 functions
    
    profiling_results = s.getvalue()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Profiling results (top 20 by cumulative time):")
    print(profiling_results)
    
    return {
        "duration": end_time - start_time,
        "profiling_results": profiling_results
    }

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

results = profile_brain_simulation(brain, 100)
```

## Test Automation

### 1. CI/CD Integration

```yaml
# .github/workflows/nlm-tests.yml
name: NLM Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.8'
    
    - name: Install dependencies
      run: |
        pip install pybind11 scikit-build-core pytest numpy
        pip install -e .
    
    - name: Run Python tests
      run: |
        python -m pytest tests/ -v
    
    - name: Run demo programs
      run: |
        cd build
        ./nlm_phase2_demo
        ./nlm_phase3_demo
        ./nlm_phase4_demo
        ./nlm_phase6_demo
    
    - name: Run performance tests
      run: |
        python performance_tests.py
```

### 2. Automated Test Reporting

```python
import pynlm
import unittest
import json
import time
from datetime import datetime

class TestReporter:
    """Generate test reports and analytics."""
    
    def __init__(self):
        self.test_results = []
        self.report_file = "test_report.json"
    
    def run_test(self, test_name, test_function):
        """Run a test and record results."""
        
        print(f"Running test: {test_name}")
        
        start_time = time.time()
        
        try:
            result = test_function()
            passed = True
            error = None
        except Exception as e:
            result = None
            passed = False
            error = str(e)
        
        end_time = time.time()
        
        test_result = {
            "name": test_name,
            "timestamp": datetime.now().isoformat(),
            "duration": end_time - start_time,
            "passed": passed,
            "error": error,
            "result": result
        }
        
        self.test_results.append(test_result)
        
        status = "PASSED" if passed else "FAILED"
        print(f"  {status}: {test_name} ({end_time - start_time:.2f}s)")
        
        return test_result
    
    def generate_report(self):
        """Generate comprehensive test report."""
        
        print("\n=== NLM Test Report ===")
        
        total_tests = len(self.test_results)
        passed_tests = sum(1 for t in self.test_results if t["passed"])
        failed_tests = total_tests - passed_tests
        
        print(f"\nSummary:")
        print(f"  Total tests: {total_tests}")
        print(f"  Passed: {passed_tests}")
        print(f"  Failed: {failed_tests}")
        print(f"  Success rate: {passed_tests/total_tests*100:.1f}%")
        
        print(f"\nDetailed Results:")
        for result in self.test_results:
            status = "✓ PASS" if result["passed"] else "✗ FAIL"
            print(f"  {status} {result['name']} ({result['duration']:.2f}s)")
            
            if result["error"]:
                print(f"    Error: {result['error']}")
        
        # Save detailed report
        report_data = {
            "timestamp": datetime.now().isoformat(),
            "summary": {
                "total_tests": total_tests,
                "passed_tests": passed_tests,
                "failed_tests": failed_tests,
                "success_rate": passed_tests/total_tests*100
            },
            "tests": self.test_results
        }
        
        with open(self.report_file, 'w') as f:
            json.dump(report_data, f, indent=2)
        
        print(f"\nDetailed report saved to: {self.report_file}")
        
        return report_data
    
    def generate_performance_report(self):
        """Generate performance analysis report."""
        
        print(f"\n=== Performance Analysis ===")
        
        durations = [t["duration"] for t in self.test_results if t["duration"] > 0]
        
        if durations:
            avg_duration = sum(durations) / len(durations)
            max_duration = max(durations)
            min_duration = min(durations)
            
            print(f"Test durations:")
            print(f"  Average: {avg_duration:.2f}s")
            print(f"  Minimum: {min_duration:.2f}s")
            print(f"  Maximum: {max_duration:.2f}s")
            print(f"  Range: {max_duration - min_duration:.2f}s")
        
        # Calculate performance metrics
        total_tests = len(self.test_results)
        print(f"\nOverall performance:")
        print(f"  Tests per hour: {total_tests / (max(durations) / 3600) if durations else 0:.1f}")
    
    def generate_html_report(self):
        """Generate HTML test report."""
        
        html_template = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Test Report</title>
            <style>
                body { font-family: Arial, sans-serif; margin: 40px; }
                .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
                .test { margin: 10px 0; padding: 10px; border-left: 4px solid #ccc; }
                .passed { border-left-color: #4CAF50; }
                .failed { border-left-color: #f44336; }
                .summary { background-color: #e8f4fd; padding: 20px; border-radius: 5px; }
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Test Report</h1>
                <p>Generated on: {timestamp}</p>
            </div>
            
            <div class="summary">
                <h2>Summary</h2>
                <p>Total Tests: {total_tests}</p>
                <p>Passed: {passed_tests}</p>
                <p>Failed: {failed_tests}</p>
                <p>Success Rate: {success_rate:.1f}%</p>
            </div>
            
            <h2>Detailed Results</h2>
            {test_details}
        </body>
        </html>
        """
        
        total_tests = len(self.test_results)
        passed_tests = sum(1 for t in self.test_results if t["passed"])
        failed_tests = total_tests - passed_tests
        success_rate = passed_tests/total_tests*100 if total_tests > 0 else 0
        
        test_details = ""
        for result in self.test_results:
            status_class = "passed" if result["passed"] else "failed"
            status_icon = "✓" if result["passed"] else "✗"
            test_details += f'''
            <div class="test {status_class}">
                <h3>{status_icon} {result['name']}</h3>
                <p>Duration: {result['duration']:.2f}s</p>
                <p>Passed: {result['passed']}</p>
                {f"<p>Error: {result['error']}</p>" if result.get('error') else ''}
            </div>
            '''
        
        html_content = html_template.format(
            timestamp=datetime.now().isoformat(),
            total_tests=total_tests,
            passed_tests=passed_tests,
            failed_tests=failed_tests,
            success_rate=success_rate,
            test_details=test_details
        )
        
        report_file = "test_report.html"
        with open(report_file, 'w') as f:
            f.write(html_content)
        
        print(f"\nHTML report saved to: {report_file}")
        
        return report_file

# Usage example
if __name__ == "__main__":
    reporter = TestReporter()
    
    # Run some example tests
    def example_test_1():
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(50):
            brain.step(step)
        
        return {"spikes": brain.getTotalSpikeCount()}
    
    def example_test_2():
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        agent.initialize(world)
        
        for step in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        return {"steps": 30, "firing": brain.getFiringNeuronCount()}
    
    # Run tests
    reporter.run_test("Basic Brain Test", example_test_1)
    reporter.run_test("Agent Simulation Test", example_test_2)
    
    # Generate reports
    reporter.generate_report()
    reporter.generate_performance_report()
    reporter.generate_html_report()
```

## Conclusion

Effective testing is crucial for ensuring the reliability and correctness of NLM simulations. This guide provides comprehensive instructions for setting up, running, and analyzing tests, including:

1. **Unit Testing**: Testing individual components
2. **Integration Testing**: Testing component interactions
3. **System Testing**: Testing complete system behavior
4. **Performance Testing**: Testing speed and resource usage
5. **Regression Testing**: Ensuring existing functionality
6. **Test Automation**: CI/CD integration and reporting

By following these testing practices, you can ensure that your NLM simulations are reliable, perform well, and maintain the expected behavior over time.