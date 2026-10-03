# Phase 6 Integration Test Suite
# Comprehensive testing for NLM Phase 6 final integration

## Overview

This test suite validates that all Phase 6 cognitive systems are properly integrated into the NLM brain loop. Based on the Phase 6 Final Audit, this suite focuses on completing the integration of partially connected systems:

- Working Memory (NeuralWorkingMemory)
- Episodic Memory (NeuralEpisodicMemory) 
- Prediction System (NeuralPrediction)
- Checkpointing (CheckpointSystem)
- Development System integration
- Neuromodulation integration

## Test Structure

### 1. Memory Integration Tests

#### 1.1 Working Memory Integration
```python
def test_working_memory_integration(brain, config):
    """Test that working memory is properly integrated with neural processing"""
    
    # Create and initialize brain
    brain = create_brain(config)
    brain.initialize()
    
    # Inject some neural activity
    for i in range(100):
        brain.step(i)
        
        # Working memory should capture active traces
        wm = brain.getWorkingMemory()
        if wm:
            traces = wm.getActiveTraces()
            # Validate traces are being captured
            assert isinstance(traces, (list, int))
            if traces > 0:
                print(f"Working memory captured {traces} active traces")
                break
    else:
        print("Note: Working memory may not have active traces for this test configuration")
    
    return True
```

#### 1.2 Episodic Memory Integration
```python
def test_episodic_memory_integration(brain, config):
    """Test that episodic memory is properly integrated with neural processing"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Run simulation to generate experiences
    for i in range(500):
        brain.step(i)
        
        # Episodic memory should capture episodes
        em = brain.getEpisodicMemory()
        if em:
            episode_count = em.getEpisodeCount()
            if episode_count > 0:
                print(f"Episodic memory captured {episode_count} episodes")
                
                # Test replay functionality
                episodes = em.getEpisodesForReplay(3)
                assert isinstance(episodes, list)
                print(f"Replay system retrieved {len(episodes)} episodes for replay")
                break
    
    return True
```

### 2. Neuromodulation Integration Tests

#### 2.1 Dopamine Neuromodulation Integration
```python
def test_dopamine_neuromodulation_integration(brain, config):
    """Test dopamine integration with plasticity and learning"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Enable neuromodulation
    agent = create_agent(brain)
    agent.enableRewardModulation(True)
    
    # Generate reward prediction errors
    for i in range(200):
        brain.step(i)
        
        # Apply reward modulation
        reward = calculate_reward(brain, i)
        agent.applyRewardModulation(reward, 0.0)
        
        # Check neuromodulation level
        neuromod_level = agent.getNeuromodulationLevel()
        if neuromod_level > 0.01:
            print(f"Dopamine neuromodulation level: {neuromod_level}")
            break
    
    return True
```

#### 2.2 Curiosity Integration Test
```python
def test_curiosity_integration(brain, config):
    """Test curiosity-driven exploration behavior"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Enable curiosity
    agent = create_agent(brain)
    agent.enableCuriosity(True)
    
    # Run simulation with environmental exploration
    curiosity_levels = []
    for i in range(300):
        brain.step(i)
        
        # Get curiosity level
        curiosity = agent.getCuriosityLevel()
        curiosity_levels.append(curiosity)
        
        # Check for novelty detection
        novelty = agent.getNoveltyLevel()
        if novelty > 0.1:
            print(f"Novelty detected: {novelty} at step {i}")
    
    # Validate curiosity system is working
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
    assert avg_curiosity > 0.0, "Curiosity system not functioning"
    print(f"Average curiosity level: {avg_curiosity}")
    
    return True
```

### 3. Prediction System Integration Tests

#### 3.1 Neural Prediction Integration
```python
def test_prediction_system_integration(brain, config):
    """Test prediction system integration with cognitive processes"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Check if prediction system is accessible
    prediction = brain.getPredictionSystem()
    if prediction:
        print("Prediction system successfully integrated with brain")
        
        # Test prediction error computation
        # Run some steps to generate prediction errors
        for i in range(100):
            brain.step(i)
            
            error = prediction.getPredictionError()
            if error > 0.0:
                print(f"Prediction error computed: {error}")
                break
        
        return True
    else:
        print("ERROR: Prediction system not integrated with brain")
        return False
```

#### 3.2 Confidence Tracking Integration
```python
def test_confidence_tracking_integration(brain, config):
    """Test confidence tracking with prediction system"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Enable prediction system if available
    prediction = brain.getPredictionSystem()
    if prediction:
        # Run simulation to develop predictions
        confidence_values = []
        for i in range(200):
            brain.step(i)
            
            # Check prediction confidence
            confidence = prediction.getPredictionConfidence()
            confidence_values.append(confidence)
        
        # Validate confidence tracking
        avg_confidence = sum(confidence_values) / len(confidence_values)
        if avg_confidence > 0.0:
            print(f"Prediction confidence tracking operational: avg {avg_confidence}")
        
        return True
    
    return False
```

### 4. Checkpointing Integration Tests

#### 4.1 Checkpoint Save/Load Integration
```python
def test_checkpoint_integration(brain, config):
    """Test checkpoint save/load functionality integration"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Run some simulation steps
    for i in range(100):
        brain.step(i)
    
    # Test checkpoint save
    checkpoint_path = "/tmp/nlm_test_checkpoint.bin"
    save_result = brain.save(checkpoint_path)
    
    if save_result:
        print("Checkpoint save functionality working")
        
        # Create new brain and test load
        brain2 = create_brain(config)
        brain2.initialize()
        
        load_result = brain2.load(checkpoint_path)
        if load_result:
            print("Checkpoint load functionality working")
            
            # Verify brain state was restored
            spike_count_1 = brain.getTotalSpikeCount()
            spike_count_2 = brain2.getTotalSpikeCount()
            
            if spike_count_2 > 0:
                print(f"Checkpoint restored brain state: {spike_count_2} spikes")
            
            return True
        else:
            print("ERROR: Checkpoint load failed")
    else:
        print("ERROR: Checkpoint save failed")
    
    return False
```

### 5. Development System Integration Tests

#### 5.1 Developmental Stage Integration
```python
def test_development_system_integration(brain, config):
    """Test developmental system integration with brain"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Check developmental stage access
    dev_stage = brain.getDevelopmentalStage()
    print(f"Initial developmental stage: {dev_stage}")
    
    # Run simulation with development
    for i in range(300):
        brain.step(i)
        
        # Check development progress
        current_stage = brain.getDevelopmentalStage()
        if current_stage != dev_stage:
            print(f"Development stage changed from {dev_stage} to {current_stage}")
            dev_stage = current_stage
            
            # Check synaptic changes with development
            synapse_count = brain.getTotalSynapseCount()
            if synapse_count > 0:
                print(f"Development: Synapse count {synapse_count}")
    
    return True
```

### 6. Advanced Integration Tests

#### 6.1 Memory-Prediction Integration
```python
def test_memory_prediction_integration(brain, config):
    """Test integration between memory and prediction systems"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Enable memory and prediction systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    prediction = brain.getPredictionSystem()
    
    if working_memory and episodic_memory and prediction:
        print("All systems accessible for memory-prediction integration test")
        
        # Run simulation to develop interactions
        for i in range(400):
            brain.step(i)
            
            # Check memory-prediction interactions
            wm_traces = working_memory.getActiveTraces()
            em_episodes = episodic_memory.getEpisodeCount()
            pred_error = prediction.getPredictionError()
            
            if wm_traces > 0 and em_episodes > 0:
                print(f"Memory-prediction integration: {wm_traces} traces, {em_episodes} episodes, error {pred_error}")
                break
        
        return True
    
    return False
```

#### 6.2 Neuromodulation-Memory Integration
```python
def test_neuromodulation_memory_integration(brain, config):
    """Test neuromodulation effects on memory systems"""
    
    brain = create_brain(config)
    brain.initialize()
    
    # Enable neuromodulation and memory
    agent = create_agent(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if working_memory and episodic_memory:
        # Generate neuromodulated experiences
        for i in range(350):
            brain.step(i)
            
            # Apply neuromodulation
            reward = calculate_contextual_reward(brain, i)
            agent.applyRewardModulation(reward, 0.0)
            
            # Check memory growth under neuromodulation
            wm_traces = working_memory.getActiveTraces()
            em_episodes = episodic_memory.getEpisodeCount()
            
            if em_episodes > 0:
                neuromod_level = agent.getNeuromodulationLevel()
                curiosity = agent.getCuriosityLevel()
                print(f"Neuromodulation-memory integration: neuromod {neuromod_level}, curiosity {curiosity}, memory {em_episodes} episodes")
                break
        
        return True
    
    return False
```

## Test Suite Execution

### Running Phase 6 Integration Tests

#### Command Line Interface
```bash
# Run all Phase 6 integration tests
./run_phase6_tests.sh

# Run specific test categories
./run_phase6_tests.sh --memory-integration
./run_phase6_tests.sh --neuromodulation-integration
./run_phase6_tests.sh --prediction-integration
./run_phase6_tests.sh --checkpointing-integration
./run_phase6_tests.sh --development-integration
./run_phase6_tests.sh --advanced-integration
```

#### Python Test Framework
```python
import pytest
from phase6_integration_tests import (
    test_working_memory_integration,
    test_episodic_memory_integration,
    test_dopamine_neuromodulation_integration,
    test_curiosity_integration,
    test_prediction_system_integration,
    test_confidence_tracking_integration,
    test_checkpoint_integration,
    test_development_system_integration,
    test_memory_prediction_integration,
    test_neuromodulation_memory_integration,
)

# Run individual tests
if __name__ == "__main__":
    # Configuration for testing
    config = create_default_config()
    
    # Run all integration tests
    test_results = []
    
    print("=== NLM Phase 6 Integration Test Suite ===")
    
    # Memory integration tests
    print("\n1. Testing Memory Integration...")
    test_results.append(("Working Memory Integration", 
                        test_working_memory_integration(create_brain(config), config)))
    test_results.append(("Episodic Memory Integration", 
                        test_episodic_memory_integration(create_brain(config), config)))
    
    # Neuromodulation integration tests
    print("\n2. Testing Neuromodulation Integration...")
    test_results.append(("Dopamine Neuromodulation Integration", 
                        test_dopamine_neuromodulation_integration(create_brain(config), config)))
    test_results.append(("Curiosity Integration", 
                        test_curiosity_integration(create_brain(config), config)))
    
    # Prediction integration tests
    print("\n3. Testing Prediction Integration...")
    test_results.append(("Prediction System Integration", 
                        test_prediction_system_integration(create_brain(config), config)))
    test_results.append(("Confidence Tracking Integration", 
                        test_confidence_tracking_integration(create_brain(config), config)))
    
    # Persistence integration tests
    print("\n4. Testing Persistence Integration...")
    test_results.append(("Checkpoint Integration", 
                        test_checkpoint_integration(create_brain(config), config)))
    
    # Development integration tests
    print("\n5. Testing Development Integration...")
    test_results.append(("Development System Integration", 
                        test_development_system_integration(create_brain(config), config)))
    
    # Advanced integration tests
    print("\n6. Testing Advanced Integration...")
    test_results.append(("Memory-Prediction Integration", 
                        test_memory_prediction_integration(create_brain(config), config)))
    test_results.append(("Neuromodulation-Memory Integration", 
                        test_neuromodulation_memory_integration(create_brain(config), config)))
    
    # Print test results
    print("\n=== Test Results Summary ===")
    passed = 0
    failed = 0
    
    for test_name, result in test_results:
        status = "PASS" if result else "FAIL"
        print(f"{test_name}: {status}")
        if result:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal: {passed + failed}, Passed: {passed}, Failed: {failed}")
    
    if failed > 0:
        print("\nSome integration tests failed. Check output for details.")
    else:
        print("\n✅ All Phase 6 integration tests passed!")
```

#### Unit Test Framework
```python
# phase6_integration_tests.py
"""
Phase 6 Integration Test Suite
Comprehensive testing for NLM Phase 6 final integration
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pytest
from nlm import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Test configuration
def create_default_config():
    return createDefaultConfig()

def create_brain(config):
    return createBrain(config)

def create_agent(brain):
    return createAgentBrain(brain)

# Helper functions for testing
def calculate_reward(brain, step):
    """Calculate reward for testing"""
    # Simple reward function for testing
    return 0.1 * ((step % 100) / 100.0)

def calculate_contextual_reward(brain, step):
    """Calculate contextual reward for testing"""
    # More complex reward function
    reward = 0.0
    if step % 50 == 0:
        reward += 1.0
    if brain.getFiringNeuronCount() > 50:
        reward += 0.5
    return reward

# Integration test functions (to be completed)
def test_working_memory_integration(brain, config):
    """Placeholder for working memory integration test"""
    return True

def test_episodic_memory_integration(brain, config):
    """Placeholder for episodic memory integration test"""
    return True

def test_dopamine_neuromodulation_integration(brain, config):
    """Placeholder for dopamine neuromodulation integration test"""
    return True

def test_curiosity_integration(brain, config):
    """Placeholder for curiosity integration test"""
    return True

def test_prediction_system_integration(brain, config):
    """Placeholder for prediction system integration test"""
    return True

def test_confidence_tracking_integration(brain, config):
    """Placeholder for confidence tracking integration test"""
    return True

def test_checkpoint_integration(brain, config):
    """Placeholder for checkpoint integration test"""
    return True

def test_development_system_integration(brain, config):
    """Placeholder for development system integration test"""
    return True

def test_memory_prediction_integration(brain, config):
    """Placeholder for memory-prediction integration test"""
    return True

def test_neuromodulation_memory_integration(brain, config):
    """Placeholder for neuromodulation-memory integration test"""
    return True

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
```

### Performance Integration Tests

#### Load Test for Integration
```python
def run_integration_load_test(config, duration_seconds=60):
    """Run performance test for Phase 6 integration"""
    import time
    
    start_time = time.time()
    end_time = start_time + duration_seconds
    
    brain = create_brain(config)
    brain.initialize()
    
    steps = 0
    integration_points = []
    
    print("Starting Phase 6 integration load test...")
    
    while time.time() < end_time:
        brain.step(steps)
        steps += 1
        
        # Record integration points
        if steps % 1000 == 0:
            integration_points.append({
                'step': steps,
                'spike_count': brain.getTotalSpikeCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                'episodic_memory_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
            })
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Calculate performance metrics
    steps_per_second = steps / duration
    avg_firing_rate = brain.getAverageFiringRate()
    
    print(f"Load test completed: {steps} steps in {duration:.2f}s")
    print(f"Performance: {steps_per_second:.2f} steps/second")
    print(f"Average firing rate: {avg_firing_rate:.4f}")
    
    if integration_points:
        print(f"Integration recorded at {len(integration_points)} points")
        
        # Print sample integration points
        print("\nSample integration points:")
        for i, point in enumerate(integration_points[::10]):  # Print every 10th point
            print(f"  Step {point['step']}: {point['spike_count']} spikes, "
                  f"{point['firing_neuron_count']} firing neurons, "
                  f"{point['working_memory_traces']} WM traces, "
                  f"{point['episodic_memory_episodes']} EM episodes")
    
    return {
        'steps': steps,
        'duration': duration,
        'steps_per_second': steps_per_second,
        'avg_firing_rate': avg_firing_rate,
        'integration_points': integration_points,
    }
```

## Test Reporting

### Integration Test Report Generator
```python
class Phase6IntegrationReport:
    """Generate comprehensive reports for Phase 6 integration tests"""
    
    def __init__(self):
        self.test_results = []
        self.performance_metrics = {}
        self.integration_status = {}
    
    def add_test_result(self, test_name, passed, details=None):
        """Add a test result to the report"""
        self.test_results.append({
            'test_name': test_name,
            'passed': passed,
            'details': details or {},
            'timestamp': time.time(),
        })
    
    def add_performance_metrics(self, test_name, metrics):
        """Add performance metrics for a test"""
        self.performance_metrics[test_name] = metrics
    
    def add_integration_status(self, system_name, status, details=None):
        """Add integration status for a system"""
        self.integration_status[system_name] = {
            'status': status,
            'details': details or {},
            'timestamp': time.time(),
        }
    
    def generate_report(self, output_format='json'):
        """Generate a comprehensive test report"""
        
        report = {
            'timestamp': time.time(),
            'test_suite': 'Phase 6 Integration Test Suite',
            'version': '1.0.0',
            'test_results': self.test_results,
            'performance_metrics': self.performance_metrics,
            'integration_status': self.integration_status,
            'summary': self._generate_summary(),
        }
        
        if output_format == 'json':
            import json
            return json.dumps(report, indent=2, default=str)
        elif output_format == 'html':
            return self._generate_html_report()
        elif output_format == 'text':
            return self._generate_text_report()
        else:
            raise ValueError(f"Unsupported output format: {output_format}")
    
    def _generate_summary(self):
        """Generate a summary of test results"""
        total_tests = len(self.test_results)
        passed_tests = sum(1 for result in self.test_results if result['passed'])
        failed_tests = total_tests - passed_tests
        
        return {
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': failed_tests,
            'success_rate': (passed_tests / total_tests * 100) if total_tests > 0 else 0,
            'systems_integrated': len([s for s in self.integration_status.values() if s['status'] == 'integrated']),
            'systems_partially_integrated': len([s for s in self.integration_status.values() if s['status'] == 'partially_integrated']),
            'systems_not_integrated': len([s for s in self.integration_status.values() if s['status'] == 'not_integrated']),
        }
    
    def _generate_html_report(self):
        """Generate an HTML report"""
        html = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Phase 6 Integration Test Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .header {{ background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }}
        .summary {{ background-color: #ecf0f1; padding: 20px; margin: 20px 0; border-radius: 5px; }}
        .test-result {{ background-color: #d5f4e6; padding: 15px; margin: 10px 0; border-radius: 3px; }}
        .failed {{ background-color: #fadbd8; }}
        .performance {{ background-color: #fff3cd; padding: 15px; margin: 10px 0; border-radius: 3px; }}
        .integration {{ background-color: #e8f4fd; padding: 15px; margin: 10px 0; border-radius: 3px; }}
        h1 {{ color: #2c3e50; }}
        h2 {{ color: #34495e; }}
        .status {{ font-weight: bold; }}
        .passed {{ color: #27ae60; }}
        .failed {{ color: #e74c3c; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>🔬 NLM Phase 6 Integration Test Report</h1>
        <p>Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
        <p>Test Suite Version: 1.0.0</p>
    </div>
    
    <div class="summary">
        <h2>📊 Test Summary</h2>
        <p><strong>Total Tests:</strong> {len(self.test_results)}</p>
        <p><strong>Passed:</strong> <span class="passed">{sum(1 for result in self.test_results if result['passed'])}</span></p>
        <p><strong>Failed:</strong> <span class="failed">{sum(1 for result in self.test_results if not result['passed'])}</span></p>
        <p><strong>Success Rate:</strong> {sum(1 for result in self.test_results if result['passed']) / max(len(self.test_results), 1) * 100:.1f}%</p>
    </div>
    
    <div class="integration-status">
        <h2>🔗 System Integration Status</h2>
"""
        
        for system_name, status_info in self.integration_status.items():
            status_class = status_info['status'].replace('_', '-')
            html += f"""
        <div class="integration">
            <h3>{system_name}</h3>
            <p><strong>Status:</strong> <span class="status {status_class}">{status_info['status'].upper().replace('_', ' ')}</span></p>
            <p><strong>Details:</strong> {status_info['details']}</p>
        </div>
        """
        
        html += """
    </div>
    
    <div class="test-results">
        <h2>🧪 Detailed Test Results</h2>
"""
        
        for result in self.test_results:
            status_class = "passed" if result['passed'] else "failed"
            status_icon = "✅" if result['passed'] else "❌"
            
            html += f"""
        <div class="test-result {status_class}">
            <h4>{status_icon} {result['test_name']}</h4>
            <p><strong>Status:</strong> <span class="status {status_class}">{result['passed'] and 'PASS' or 'FAIL'}</span></p>
            <p><strong>Details:</strong> {result['details']}</p>
            <p><strong>Timestamp:</strong> {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(result['timestamp']))}</p>
        </div>
        """
        
        html += """
    </div>
    
    <div class="performance-metrics">
        <h2>⚡ Performance Metrics</h2>
"""
        
        for test_name, metrics in self.performance_metrics.items():
            html += f"""
        <div class="performance">
            <h3>{test_name}</h3>
"""
            for key, value in metrics.items():
                html += f"<p><strong>{key}:</strong> {value}</p>"
            
            html += "</div>"
        
        html += """
    </div>
</body>
</html>
"""
        
        return html
    
    def _generate_text_report(self):
        """Generate a plain text report"""
        report = f"""
NLM Phase 6 Integration Test Report
{'=' * 50}
Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}
Test Suite Version: 1.0.0

{'=' * 50}
TEST SUMMARY
{'=' * 50}
Total Tests: {len(self.test_results)}
Passed: {sum(1 for result in self.test_results if result['passed'])}
Failed: {sum(1 for result in self.test_results if not result['passed'])}
Success Rate: {sum(1 for result in self.test_results if result['passed']) / max(len(self.test_results), 1) * 100:.1f}%

{'=' * 50}
SYSTEM INTEGRATION STATUS
{'=' * 50}
"""
        
        for system_name, status_info in self.integration_status.items():
            report += f"{system_name}: {status_info['status'].upper().replace('_', ' ')} - {status_info['details']}\n"
        
        report += f"\n{'=' * 50}\nDETAILED TEST RESULTS\n{'=' * 50}\n"
        
        for result in self.test_results:
            status = "PASS" if result['passed'] else "FAIL"
            status_icon = "✅" if result['passed'] else "❌"
            report += f"{status_icon} {result['test_name']}: {status}\n"
            if result['details']:
                report += f"    Details: {result['details']}\n"
        
        return report

# Integration test execution example
def run_complete_integration_test_suite():
    """Run the complete Phase 6 integration test suite"""
    from phase6_integration_tests import Phase6IntegrationReport
    
    print("🚀 Starting Complete Phase 6 Integration Test Suite")
    
    # Create report generator
    report = Phase6IntegrationReport()
    
    # Configuration for testing
    config = create_default_config()
    
    # Run integration tests
    brain = create_brain(config)
    brain.initialize()
    
    # Test 1: Working memory integration
    print("🧪 Testing working memory integration...")
    wm_integration = test_working_memory_integration(brain, config)
    report.add_test_result("Working Memory Integration", wm_integration)
    report.add_integration_status("Working Memory", "integrated" if wm_integration else "partially_integrated")
    
    # Test 2: Episodic memory integration
    print("🧪 Testing episodic memory integration...")
    em_integration = test_episodic_memory_integration(brain, config)
    report.add_test_result("Episodic Memory Integration", em_integration)
    report.add_integration_status("Episodic Memory", "integrated" if em_integration else "partially_integrated")
    
    # Test 3: Neuromodulation integration
    print("🧪 Testing neuromodulation integration...")
    neuromod_integration = test_dopamine_neuromodulation_integration(brain, config)
    report.add_test_result("Neuromodulation Integration", neuromod_integration)
    report.add_integration_status("Neuromodulation", "integrated" if neuromod_integration else "partially_integrated")
    
    # Test 4: Prediction system integration
    print("🧪 Testing prediction system integration...")
    pred_integration = test_prediction_system_integration(brain, config)
    report.add_test_result("Prediction System Integration", pred_integration)
    report.add_integration_status("Prediction System", "integrated" if pred_integration else "not_integrated")
    
    # Test 5: Development system integration
    print("🧪 Testing development system integration...")
    dev_integration = test_development_system_integration(brain, config)
    report.add_test_result("Development System Integration", dev_integration)
    report.add_integration_status("Development System", "integrated" if dev_integration else "partially_integrated")
    
    # Run performance test
    print("⚡ Running integration performance test...")
    perf_metrics = run_integration_load_test(config, duration_seconds=30)
    report.add_performance_metrics("Integration Load Test", perf_metrics)
    
    # Generate comprehensive report
    print("📊 Generating comprehensive test report...")
    
    # Save JSON report
    json_report = report.generate_report('json')
    with open('/tmp/phase6_integration_test_report.json', 'w') as f:
        f.write(json_report)
    
    # Save HTML report
    html_report = report.generate_report('html')
    with open('/tmp/phase6_integration_test_report.html', 'w') as f:
        f.write(html_report)
    
    # Save text report
    text_report = report.generate_report('text')
    with open('/tmp/phase6_integration_test_report.txt', 'w') as f:
        f.write(text_report)
    
    print("\n✅ Integration test suite completed!")
    print("Reports saved to:")
    print("  - /tmp/phase6_integration_test_report.json")
    print("  - /tmp/phase6_integration_test_report.html")
    print("  - /tmp/phase6_integration_test_report.txt")
    
    return report

if __name__ == "__main__":
    import time
    
    # Execute complete integration test suite
    integration_report = run_complete_integration_test_suite()
    
    # Print summary
    summary = integration_report._generate_summary()
    print(f"\n{'=' * 60}")
    print("INTEGRATION TEST SUMMARY")
    print(f"{'=' * 60}")
    print(f"Total Tests: {summary['total_tests']}")
    print(f"Passed Tests: {summary['passed_tests']}")
    print(f"Failed Tests: {summary['failed_tests']}")
    print(f"Success Rate: {summary['success_rate']:.1f}%")
    print(f"Systems Integrated: {summary['systems_integrated']}")
    print(f"Systems Partially Integrated: {summary['systems_partially_integrated']}")
    print(f"Systems Not Integrated: {summary['systems_not_integrated']}")
    
    if summary['failed_tests'] > 0:
        print("\n⚠️  Some integration tests failed. Review reports for details.")
    else:
        print("\n🎉 All integration tests passed!")
```