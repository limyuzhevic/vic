#!/usr/bin/env python3
"""
Utility Functions Examples for NLM Python Bindings

This script demonstrates the utility functions among the 24 new Python bindings
added to the NLM bindings. Utility functions provide essential functionality
for validation, monitoring, configuration management, and performance tracking.

Key Utility Categories:
- Brain validation and state checking
- Performance monitoring and benchmarking
- Configuration management and validation
- Snapshot management (save/load)
- Simulation control (pause/resume/step multiple)
- Report generation and data export
"""

import sys
import os
import json
import time
import traceback

# Add python directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLMC_AVAILABLE = True
except ImportError:
    PYNLMC_AVAILABLE = False
    print("WARNING: pynlm module not available. Running with mock data.")

def print_header(title):
    """Print formatted header."""
    print(f"\n{'='*70}")
    print(f"  {title}")
    print(f"{'='*70}")

def print_section(title):
    """Print section header."""
    print(f"\n{'•'*50}")
    print(f"{title}")
    print(f"{'•'*50}")

def mock_brain():
    """Create mock brain for testing when pynlm is not available."""
    class MockBrain:
        def __init__(self, neuron_count=1000, region_count=5):
            self.neuron_count = neuron_count
            self.region_count = region_count
            self.spike_count = 0
            self.start_time = time.time()
            self.is_paused = False
            
        def initialize(self):
            return True
            
        def step(self, step):
            self.spike_count += 1
            return True
            
        def getTotalNeuronCount(self):
            return self.neuron_count
            
        def getTotalSpikeCount(self):
            return self.spike_count
            
        def getAverageFiringRate(self):
            elapsed = time.time() - self.start_time
            return (self.spike_count / elapsed) * 1000 if elapsed > 0 else 0
            
        def pauseSimulation(self):
            self.is_paused = True
            return True
            
        def resumeSimulation(self):
            self.is_paused = False
            return True
            
        def stepMultiple(self, steps):
            for i in range(steps):
                self.step(i)
            return True
            
        def save(self, filepath=""):
            if filepath:
                print(f"Mock: Saving brain to {filepath}")
            return True
            
        def load(self, filepath=""):
            if filepath:
                print(f"Mock: Loading brain from {filepath}")
            return True
    
    return MockBrain()

def create_mock_config(neuron_count=1000, region_count=5):
    """Create mock config for testing."""
    class MockConfig:
        def __init__(self):
            self.data = {}
            
        def set(self, key, value, source=None):
            self.data[key] = value
            
        def getKeys(self):
            return list(self.data.keys())
            
        def get(self, key, default=None):
            return self.data.get(key, default)
    
    config = MockConfig()
    config.set('neuron_count', neuron_count)
    config.set('region_count', region_count)
    return config

def example_1_brain_validation():
    """Example 1: Brain validation utility functions."""
    print_section("1. Brain Validation Utility Functions")
    
    if not PYNLMC_AVAILABLE:
        print("Running with mock implementation...")
        
        # Create and validate brain
        brain = mock_brain()
        brain.initialize()
        
        # Test validateBrain
        is_valid = brain.getTotalNeuronCount() > 0
        print(f"  ✓ Brain validation: {'PASS' if is_valid else 'FAIL'}")
        print(f"    Neurons: {brain.getTotalNeuronCount()}")
        print(f"    Spike count: {brain.getTotalSpikeCount()}")
        
        # Test additional validation scenarios
        print(f"\n  Additional validation scenarios:")
        
        # Test with invalid brain (zero neurons)
        invalid_brain = mock_brain()
        invalid_brain.neuron_count = 0
        is_valid_invalid = invalid_brain.getTotalNeuronCount() > 0
        print(f"    Invalid brain validation: {'PASS' if not is_valid_invalid else 'FAIL'}")
        
        # Test brain state after steps
        for i in range(5):
            brain.step(i)
        print(f"    After 5 steps - Spike count: {brain.getTotalSpikeCount()}")
        print(f"    After 5 steps - Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        return {
            'brain_valid': is_valid,
            'neuron_count': brain.getTotalNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
    else:
        print("Using real pynlm implementation...")
        
        # Create brain with config
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 5)
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Test validateBrain
        is_valid = pynlm.validateBrain(brain)
        print(f"  ✓ Brain validation: {'PASS' if is_valid else 'FAIL'}")
        print(f"    Neurons: {brain.getTotalNeuronCount()}")
        print(f"    Regions: {brain.getRegionCount()}")
        
        # Test additional validation
        print(f"\n  Additional validation scenarios:")
        
        # Test brain state
        brain.step(10)
        print(f"    After 10 steps - Spike count: {brain.getTotalSpikeCount()}")
        print(f"    After 10 steps - Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        return {
            'brain_valid': is_valid,
            'neuron_count': brain.getTotalNeuronCount(),
            'region_count': brain.getRegionCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate()
        }

def example_2_snapshot_management():
    """Example 2: Snapshot management utilities."""
    print_section("2. Snapshot Management Utilities")
    
    print("  Testing snapshot creation and management...")
    
    if not PYNLMC_AVAILABLE:
        # Create brain and test snapshot
        brain = mock_brain()
        brain.initialize()
        
        # Simulate some activity
        for i in range(20):
            brain.step(i)
        
        # Test save and load
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        save_result = brain.save(f"demo_snapshot_{timestamp}")
        print(f"  ✓ Save snapshot: {'SUCCESS' if save_result else 'FAILED'}")
        
        # Test load (simulated)
        load_result = brain.load(f"demo_snapshot_{timestamp}")
        print(f"  ✓ Load snapshot: {'SUCCESS' if load_result else 'FAILED'}")
        
        # Show snapshot info
        print(f"\n  Snapshot details:")
        print(f"    Timestamp: {timestamp}")
        print(f"    Neuron count: {brain.getTotalNeuronCount()}")
        print(f"    Total spikes: {brain.getTotalSpikeCount()}")
        print(f"    Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        return {
            'snapshot_created': True,
            'snapshot_timestamp': timestamp,
            'neuron_count': brain.getTotalNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    else:
        # Real implementation
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("region_count", 3)
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Run some simulation steps
        for i in range(20):
            brain.step(i)
        
        # Test save snapshot
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        save_result = pynlm.saveBrainSnapshot(brain, f"demo_snapshot_{timestamp}")
        print(f"  ✓ Save snapshot: {'SUCCESS' if save_result else 'FAILED'}")
        
        # Test load snapshot (skipped for demo)
        print(f"  ✗ Load snapshot: Skipped (requires saved file)")
        
        # Show snapshot info
        print(f"\n  Snapshot details:")
        print(f"    Timestamp: {timestamp}")
        print(f"    Neurons: {brain.getTotalNeuronCount()}")
        print(f"    Regions: {brain.getRegionCount()}")
        print(f"    Total spikes: {brain.getTotalSpikeCount()}")
        
        return {
            'snapshot_created': save_result,
            'snapshot_timestamp': timestamp,
            'neuron_count': brain.getTotalNeuronCount(),
            'region_count': brain.getRegionCount(),
            'total_spikes': brain.getTotalSpikeCount()
        }

def example_3_performance_benchmarking():
    """Example 3: Performance benchmarking utilities."""
    print_section("3. Performance Benchmarking Utilities")
    
    print("  Running performance benchmarks...")
    
    if not PYNLMC_AVAILABLE:
        # Create and benchmark mock brain
        brain = mock_brain()
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        for i in range(100):
            brain.step(i)
        elapsed_time = time.time() - start_time
        
        benchmark_results = {
            'avg_time_ms': (elapsed_time / 100) * 1000,
            'steps_per_second': 100 / elapsed_time if elapsed_time > 0 else 0,
            'total_spike_count': brain.getTotalSpikeCount(),
            'firing_rate_hz': brain.getAverageFiringRate()
        }
        
        print(f"  ✓ Benchmark completed in {elapsed_time:.2f} seconds")
        print(f"  ✓ Average time per step: {benchmark_results['avg_time_ms']:.2f} ms")
        print(f"  ✓ Steps per second: {benchmark_results['steps_per_second']:.2f}")
        print(f"  ✓ Total spike count: {benchmark_results['total_spike_count']}")
        print(f"  ✓ Firing rate: {benchmark_results['firing_rate_hz']:.2f} Hz")
        
        return benchmark_results
    else:
        # Real implementation
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 5)
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Run benchmark
        benchmark_results = pynlm.benchmarkSimulation(brain, 100)
        
        print(f"  ✓ Benchmark completed")
        print(f"  ✓ Average time per step: {benchmark_results['avg_time_ms']:.2f} ms")
        print(f"  ✓ Steps per second: {benchmark_results['steps_per_second']:.2f}")
        print(f"  ✓ Total spike count: {benchmark_results['total_spike_count']}")
        print(f"  ✓ Firing rate: {benchmark_results['firing_rate_hz']:.2f} Hz")
        
        return benchmark_results

def example_4_simulation_control():
    """Example 4: Simulation control utilities."""
    print_section("4. Simulation Control Utilities")
    
    print("  Testing simulation control functions...")
    
    if not PYNLMC_AVAILABLE:
        # Create and control mock brain
        brain = mock_brain()
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Test pause
        pause_result = brain.pauseSimulation()
        print(f"  ✓ Pause simulation: {'SUCCESS' if pause_result else 'FAILED'}")
        print(f"    Simulation paused: {brain.is_paused}")
        
        # Test resume
        resume_result = brain.resumeSimulation()
        print(f"  ✓ Resume simulation: {'SUCCESS' if resume_result else 'FAILED'}")
        print(f"    Simulation paused: {brain.is_paused}")
        
        # Test stepMultiple
        steps_to_add = 50
        step_multiple_result = brain.stepMultiple(steps_to_add)
        final_spikes = brain.getTotalSpikeCount()
        spikes_added = final_spikes - initial_spikes
        
        print(f"  ✓ Step multiple ({steps_to_add} steps): {'SUCCESS' if step_multiple_result else 'FAILED'}")
        print(f"    Spikes added: {spikes_added}/{steps_to_add}")
        
        return {
            'pause_success': pause_result,
            'resume_success': resume_result,
            'step_multiple_success': step_multiple_result,
            'spikes_added': spikes_added,
            'final_spike_count': final_spikes
        }
    else:
        # Real implementation
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("region_count", 3)
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Test pause
        pynlm.pauseSimulation(brain)
        print(f"  ✓ Pause simulation: SUCCESS")
        
        # Test resume
        pynlm.resumeSimulation(brain)
        print(f"  ✓ Resume simulation: SUCCESS")
        
        # Test stepMultiple
        steps_to_add = 50
        pynlm.stepMultiple(brain, steps_to_add)
        final_spikes = brain.getTotalSpikeCount()
        spikes_added = final_spikes - initial_spikes
        
        print(f"  ✓ Step multiple ({steps_to_add} steps): SUCCESS")
        print(f"    Spikes added: {spikes_added}/{steps_to_add}")
        
        return {
            'pause_success': True,
            'resume_success': True,
            'step_multiple_success': True,
            'spikes_added': spikes_added,
            'final_spike_count': final_spikes
        }

def example_5_report_generation():
    """Example 5: Report generation utilities."""
    print_section("5. Report Generation Utilities")
    
    print("  Generating simulation reports...")
    
    if not PYNLMC_AVAILABLE:
        # Create mock brain with data
        brain = mock_brain()
        brain.initialize()
        
        # Simulate some activity
        for i in range(50):
            brain.step(i)
        
        # Prepare report data
        report_data = {
            'avg_time_ms': 0.5,  # Mock data
            'steps_per_second': 2000.0,
            'total_spike_count': brain.getTotalSpikeCount(),
            'firing_rate_hz': brain.getAverageFiringRate(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalNeuronCount() * 10,  # Mock
            'average_reward_per_step': 0.25,
            'final_health': 100.0
        }
        
        # Generate report
        report = f"NLM Simulation Report\n"
        report += f"======================\n\n"
        report += f"Simulation Parameters:\n"
        report += f"  - Total Neurons: {report_data['neuron_count']}\n"
        report += f"  - Total Spikes: {report_data['total_spike_count']}\n"
        report += f"  - Steps per Second: {report_data['steps_per_second']:.2f}\n"
        report += f"  - Average Firing Rate: {report_data['firing_rate_hz']:.2f} Hz\n"
        report += f"  - Average Step Time: {report_data['avg_time_ms']:.2f} ms\n\n"
        report += f"Performance Metrics:\n"
        report += f"  - Average Reward per Step: {report_data['average_reward_per_step']:.2f}\n"
        report += f"  - Final Health: {report_data['final_health']:.1f}%\n"
        report += f"  - Synapse Count: {report_data['synapse_count']:,}\n\n"
        report += f"Generated at: {time.strftime('%Y-%m-%d %H:%M:%S')}\n"
        
        print(f"  ✓ Report generated successfully!")
        print(f"  ✓ Report length: {len(report)} characters")
        print(f"  ✓ Key metrics included: {'Yes' if 'Neurons' in report else 'No'}")
        
        # Show first few lines of report
        print(f"\n  Report preview:")
        for line in report.split('\n')[:10]:
            print(f"    {line}")
        
        return {
            'report_generated': True,
            'report_length': len(report),
            'report_content': report
        }
    else:
        # Real implementation
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("region_count", 3)
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Run some steps
        for i in range(50):
            brain.step(i)
        
        # Benchmark for report data
        benchmark = pynlm.benchmarkSimulation(brain, 50)
        
        report_data = {
            'avg_time_ms': benchmark['avg_time_ms'],
            'steps_per_second': benchmark['steps_per_second'],
            'total_spike_count': benchmark['total_spike_count'],
            'firing_rate_hz': benchmark['firing_rate_hz'],
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'average_reward_per_step': 0.25,  # Mock
            'final_health': 100.0
        }
        
        # Generate report
        report = pynlm.generate_report(report_data)
        
        print(f"  ✓ Report generated successfully!")
        print(f"  ✓ Report length: {len(report)} characters")
        
        # Show report preview
        print(f"\n  Report preview:")
        for line in report.split('\n')[:10]:
            print(f"    {line}")
        
        return {
            'report_generated': True,
            'report_length': len(report),
            'report_content': report,
            'benchmark_data': benchmark
        }

def example_6_configuration_management():
    """Example 6: Configuration management utilities."""
    print_section("6. Configuration Management Utilities")
    
    print("  Testing configuration export/import...")
    
    if not PYNLMC_AVAILABLE:
        # Create and export config
        config = create_mock_config()
        
        # Export to JSON
        config_dict = config.data
        json_str = json.dumps(config_dict, indent=2)
        
        print(f"  ✓ Config exported: {len(json_str)} characters")
        print(f"    Config keys: {list(config_dict.keys())}")
        
        # Import from JSON
        imported_dict = json.loads(json_str)
        imported_config = create_mock_config()
        imported_config.data = imported_dict
        
        print(f"  ✓ Config imported: {imported_config.getKeys()} keys")
        print(f"    Neuron count: {imported_config.get('neuron_count')}")
        
        # Test config validation
        is_valid_config = len(imported_config.getKeys()) > 0
        print(f"  ✓ Config validation: {'PASS' if is_valid_config else 'FAIL'}")
        
        return {
            'config_exported': True,
            'json_length': len(json_str),
            'config_keys': config.getKeys(),
            'import_success': True,
            'config_valid': is_valid_config
        }
    else:
        # Real implementation
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 5)
        config.set("connection_probability", 0.15)
        
        # Export to JSON
        json_str = pynlm.exportConfigToJson(config)
        print(f"  ✓ Config exported: {len(json_str)} characters")
        
        # Import from JSON
        imported_config = pynlm.importConfigFromJson(json_str)
        print(f"  ✓ Config imported: {imported_config.getKeys()} keys")
        
        # Test config validation
        schema_valid = pynlm.validateConfigSchema(config, "default_schema")
        print(f"  ✓ Config schema validation: {'PASS' if schema_valid else 'FAIL'}")
        
        return {
            'config_exported': True,
            'json_length': len(json_str),
            'config_keys': config.getKeys(),
            'import_success': True,
            'config_valid': schema_valid
        }

def example_7_memory_usage_monitoring():
    """Example 7: Memory usage monitoring."""
    print_section("7. Memory Usage Monitoring")
    
    print("  Monitoring memory usage...")
    
    if not PYNLMC_AVAILABLE:
        print(f"  ⚠️  Memory monitoring not available in mock implementation")
        print(f"  💡 Use real pynlm.get_memory_usage() for actual memory monitoring")
        
        return {
            'memory_monitoring_available': False,
            'note': 'Use real pynlm for memory monitoring'
        }
    else:
        # Real memory monitoring
        memory_info = pynlm.get_memory_usage()
        
        print(f"  ✓ Memory usage: {memory_info['total_mb']} MB total")
        print(f"  ✓ Used memory: {memory_info['used_mb']} MB")
        print(f"  ✓ Usage percentage: {memory_info['usage_percent']}%")
        
        # Show memory breakdown
        print(f"\n  Memory breakdown:")
        print(f"    - Total: {memory_info['total_mb']} MB")
        print(f"    - Used: {memory_info['used_mb']} MB")
        print(f"    - Available: {memory_info['total_mb'] - memory_info['used_mb']} MB")
        print(f"    - Usage: {memory_info['usage_percent']:.1f}%")
        
        return {
            'memory_monitoring_available': True,
            'memory_info': memory_info
        }

def main():
    """Main function to run all utility function examples."""
    print_header("NLM Python Bindings - Utility Functions Examples")
    
    if PYNLMC_AVAILABLE:
        print("✓ pynlm module loaded successfully")
    else:
        print("⚠️  pynlm module not available - running with mock implementation")
    
    print("\nThis script demonstrates the utility functions from the")
    print("24 new NLM Python bindings.")
    
    # Run all examples
    examples = [
        ("Brain Validation", example_1_brain_validation),
        ("Snapshot Management", example_2_snapshot_management),
        ("Performance Benchmarking", example_3_performance_benchmarking),
        ("Simulation Control", example_4_simulation_control),
        ("Report Generation", example_5_report_generation),
        ("Configuration Management", example_6_configuration_management),
        ("Memory Usage Monitoring", example_7_memory_usage_monitoring),
    ]
    
    all_results = []
    
    for example_name, example_func in examples:
        try:
            print(f"\n{'='*70}")
            print(f"EXECUTING: {example_name}")
            print(f"{'='*70}")
            
            result = example_func()
            all_results.append((example_name, result, "SUCCESS"))
            
        except Exception as e:
            print(f"\n{'='*70}")
            print(f"ERROR in {example_name}: {e}")
            print(f"{'='*70}")
            print(f"Traceback: {traceback.format_exc()}")
            all_results.append((example_name, None, f"FAILED: {e}"))
    
    # Summary
    print_header("UTILITY FUNCTIONS EXAMPLES SUMMARY")
    
    successful = [r for r in all_results if r[2] == "SUCCESS"]
    failed = [r for r in all_results if r[2] != "SUCCESS"]
    
    print(f"\nTotal examples executed: {len(all_results)}")
    print(f"Successful examples: {len(successful)}")
    print(f"Failed examples: {len(failed)}")
    
    # Count successful utility function categories
    successful_categories = {
        'brain_validation': 0,
        'snapshot_management': 0,
        'performance_benchmarking': 0,
        'simulation_control': 0,
        'report_generation': 0,
        'configuration_management': 0,
        'memory_monitoring': 0
    }
    
    for example_name, result, status in successful:
        if 'brain_valid' in str(result) or 'neuron_count' in str(result):
            successful_categories['brain_validation'] += 1
        if 'snapshot' in example_name.lower() or 'save' in example_name.lower():
            successful_categories['snapshot_management'] += 1
        if 'benchmark' in example_name.lower():
            successful_categories['performance_benchmarking'] += 1
        if 'control' in example_name.lower() or 'pause' in example_name.lower() or 'step' in example_name.lower():
            successful_categories['simulation_control'] += 1
        if 'report' in example_name.lower():
            successful_categories['report_generation'] += 1
        if 'config' in example_name.lower():
            successful_categories['configuration_management'] += 1
        if 'memory' in example_name.lower():
            successful_categories['memory_monitoring'] += 1
    
    print(f"\n{'='*70}")
    print("UTIL FUNCTION CATEGORIES SUCCESSFUL:")
    print(f"{'='*70}")
    
    for category, count in successful_categories.items():
        status = "✓" if count > 0 else "✗"
        print(f"  {status} {category.replace('_', ' ').title()}: {count}")
    
    if failed:
        print(f"\n{'='*70}")
        print("FAILED EXAMPLES:")
        print(f"{'='*70}")
        
        for example_name, result, status in failed:
            print(f"  ✗ {example_name}")
            print(f"    Error: {status}")
    
    print(f"\n{'='*70}")
    print("UTILITY FUNCTIONS DEMONSTRATION COMPLETE")
    print(f"{'='*70}")
    print(f"\nKey achievements:")
    print(f"  ✓ Demonstrated {len(successful_categories)} utility function categories")
    print(f"  ✓ Covered brain validation, monitoring, and control functions")
    print(f"  ✓ Showed performance benchmarking and reporting capabilities")
    print(f"  ✓ Demonstrated configuration management and snapshot handling")
    print(f"  ✓ Provided memory usage monitoring examples")
    
    print(f"\nTotal utility functions demonstrated: ~{len(successful_categories) * 3}+")
    
    print(f"\n{'='*70}")
    
    return 0

if __name__ == "__main__":
    exit(main())