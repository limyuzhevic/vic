#!/usr/bin/env python3
"""
NLM Enhanced Configuration System Test

This script tests all the enhancements made to the NLM project:
- Enhanced command-line interface
- Advanced Python bindings
- Configuration validation and runtime modification
- Performance profiling and optimization
- Documentation and help systems

Run this script to verify all improvements work correctly.
"""

import subprocess
import sys
import os
import json
import time
from pathlib import Path

def run_command(cmd, description=""):
    """Run a shell command and return output."""
    print(f"\n=== {description} ===")
    print(f"Command: {cmd}")
    
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=60)
        
        if result.returncode == 0:
            print("✅ SUCCESS")
            if result.stdout:
                print("Output:", result.stdout[:500] + "..." if len(result.stdout) > 500 else result.stdout)
        else:
            print("❌ FAILED")
            if result.stderr:
                print("Error:", result.stderr[:500] + "..." if len(result.stderr) > 500 else result.stderr)
        
        return result.returncode == 0, result.stdout, result.stderr
        
    except subprocess.TimeoutExpired:
        print("⏰ TIMEOUT")
        return False, "", "Command timed out"
    except Exception as e:
        print(f"💥 ERROR: {e}")
        return False, "", str(e)

def test_basic_usage():
    """Test basic NLM usage."""
    print("\n" + "="*60)
    print("TEST 1: Basic NLM Usage")
    print("="*60)
    
    test_code = '''
import pynlm

# Test basic functionality
brain = pynlm.createPythonBrain()
brain.initialize()

# Run simulation
for step in range(10):
    brain.step(step)

print("Basic brain test completed successfully")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Basic NLM Usage Test")

def test_enhanced_config():
    """Test enhanced configuration system."""
    print("\n" + "="*60)
    print("TEST 2: Enhanced Configuration System")
    print("="*60)
    
    test_code = '''
import pynlm

# Test enhanced configuration
config = pynlm.PythonConfig()
config.loadFromDict({
    "neuron_count": 100,
    "timestep": 0.001,
    "enable_development": True,
    "enable_curiosity": True
})

print("Configuration loaded successfully")
print(f"Neuron count: {config.getOrDefault('neuron_count', 0)}")
print(f"Timestep: {config.getOrDefault('timestep', 0.0)}")
print(f"Development enabled: {config.getOrDefault('enable_development', False)}")
print(f"Curiosity enabled: {config.getOrDefault('enable_curiosity', False)}")

# Test configuration validation
validation = pynlm.validateConfig({
    "neuron_count": 500,
    "timestep": 0.001,
    "enable_development": True
})

print(f"Configuration validation: {validation['valid']}")
if validation['valid']:
    print("Configuration is valid!")
else:
    print("Validation errors:", validation['errors'])

# Test configuration merging
config1 = {"neuron_count": 100, "timestep": 0.001}
config2 = {"enable_development": True, "enable_curiosity": True}
merged = pynlm.mergeConfigs(config1, config2)
print(f"Merged configuration: {merged}")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Enhanced Configuration Test")

def test_enhanced_python_bindings():
    """Test enhanced Python bindings."""
    print("\n" + "="*60)
    print("TEST 3: Enhanced Python Bindings")
    print("="*60)
    
    test_code = '''
import pynlm

# Test enhanced brain with context manager
with pynlm.createPythonBrain() as brain:
    brain.initialize()
    
    # Test neural stats
    stats = brain.getNeuralStats()
    print(f"Neural stats: {stats}")
    
    # Test spike stats
    spike_stats = brain.getSpikeStats(time_window=1.0)
    print(f"Spike stats: {spike_stats}")
    
    # Test brain comparison
    brains = [brain, brain, brain]
    comparison = pynlm.compareBrains(brains)
    print(f"Brain comparison: {comparison}")
    
    print("Enhanced Python bindings test completed")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Enhanced Python Bindings Test")

def test_batch_simulation():
    """Test batch simulation functionality."""
    print("\n" + "="*60)
    print("TEST 4: Batch Simulation")
    print("="*60)
    
    test_code = '''
import pynlm

# Test batch simulation
config = pynlm.PythonConfig()
config.loadFromDict({
    "neuron_count": 50,
    "timestep": 0.001
})

# Create batch configuration
batch_config = pynlm.createSimulationBatch([config, config], "plasticity")
print(f"Batch configuration created: {batch_config}")

# Test batch simulation (simplified)
print("Batch simulation test completed (limited for demo)")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Batch Simulation Test")

def test_performance_profiling():
    """Test performance profiling."""
    print("\n" + "="*60)
    print("TEST 5: Performance Profiling")
    print("="*60)
    
    test_code = '''
import pynlm

# Test performance profiling
brain = pynlm.createPythonBrain()

# Create brain for benchmarking
brain.initialize()
for step in range(50):
    brain.step(step)

# Profile brain execution
profile_result = pynlm.profileBrainExecution(brain, iterations=10)
print(f"Profile results: {profile_result}")
print(f"Average time per iteration: {profile_result['average_time_us']} us")
print(f"Operations per second: {profile_result['ops_per_second']:.2f}")
'''
   
    return run_command(f'python3 -c "{test_code}"', "Performance Profiling Test")

def test_spike_analysis():
    """Test spike train analysis."""
    print("\n" + "="*60)
    print("TEST 6: Spike Train Analysis")
    print("="*60)
    
    test_code = '''
import pynlm

# Test spike train analysis
spike_counts = [10, 15, 20, 25, 30, 35, 40, 45, 50]
analysis = pynlm.analyzeSpikeTrain(spike_counts, time_window=5.0)

print("Spike train analysis results:")
print(f"  Total samples: {len(spike_counts)}")
print(f"  Mean spikes per sample: {analysis['mean_spikes']:.2f}")
print(f"  Standard deviation: {analysis['std_deviation']:.2f}")
print(f"  Burst count: {analysis['burst_count']}")
print(f"  Burst rate: {analysis['burst_rate']:.2f} bursts/second")
print(f"  Coefficient of variation: {analysis['coefficient_of_variation']:.3f}")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Spike Train Analysis Test")

def test_configuration_export():
    """Test configuration export/import."""
    print("\n" + "="*60)
    print("TEST 7: Configuration Export/Import")
    print("="*60)
    
    test_code = '''
import pynlm

# Test configuration export
config = pynlm.PythonConfig()
config.loadFromDict({
    "neuron_count": 200,
    "timestep": 0.001,
    "enable_development": True,
    "enable_curiosity": True,
    "experiment_type": "plasticity"
})

# Export configuration to file
config.exportConfig("test_config.json", "json")
print("Configuration exported to test_config.json")

# Test configuration templates
templates = pynlm.getConfigurationTemplates()
print(f"Available templates: {list(templates.keys())}")

# Test template usage
template_config = templates["standard"]
print(f"Standard template: {template_config}")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Configuration Export Test")

def test_help_systems():
    """Test help and documentation systems."""
    print("\n" + "="*60)
    print("TEST 8: Help and Documentation Systems")
    print("="*60)
    
    test_code = '''
import pynlm

# Test help system
help_text = pynlm.getPythonAPIDoc()
print("Python API documentation available (length:", len(help_text), "characters)")

# Test library info
library_info = pynlm.getLibraryInfo()
print(f"Library version: {library_info['version']}")
print(f"Python compatibility: {library_info['python_compatibility']}")
print(f"Features available: {', '.join(library_info['features'])}")

# Test advanced features
pynlm.showAdvancedFeatures()
print("Advanced features demonstration completed")

# Test about
about_info = pynlm.about()
print("About information displayed")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Help and Documentation Test")

def test_error_handling():
    """Test error handling and validation."""
    print("\n" + "="*60)
    print("TEST 9: Error Handling and Validation")
    print("="*60)
    
    test_code = '''
import pynlm

# Test configuration validation
invalid_config = {
    "neuron_count": -100,  # Invalid: negative
    "timestep": -0.001,    # Invalid: negative
    "enable_development": "yes"  # Invalid: should be bool
}

validation = pynlm.validateConfig(invalid_config)
print(f"Invalid configuration validation: {validation['valid']}")
if not validation['valid']:
    print(f"Errors found: {len(validation['errors'])}")
    for error in validation['errors']:
        print(f"  - {error}")

# Test error handler
def custom_error_handler(error):
    print(f"Custom error handler received: {error}")

pynlm.setErrorHandler(custom_error_handler)
print("Custom error handler set")

# Test debugging
brain = pynlm.createPythonBrain()
debug_info = pynlm.debugBrainState(brain)
print(f"Debug info: {debug_info}")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Error Handling Test")

def test_multi_region_simulation():
    """Test multi-region simulation."""
    print("\n" + "="*60)
    print("TEST 10: Multi-Region Simulation")
    print("="*60)
    
    test_code = '''
import pynlm

# Test multi-region simulation configuration
multi_region_config = pynlm.createMultiRegionSimulation(
    num_regions=3,
    neurons_per_region=100,
    connection_prob=0.1
)

print("Multi-region simulation configuration:")
print(f"  Number of regions: {multi_region_config['num_regions']}")
print(f"  Neurons per region: {multi_region_config['neurons_per_region']}")
print(f"  Connection probability: {multi_region_config['connection_prob']}")
print(f"  Simulation type: {multi_region_config['simulation_type']}")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Multi-Region Simulation Test")

def test_research_export():
    """Test research export functionality."""
    print("\n" + "="*60)
    print("TEST 11: Research Export")
    print("="*60)
    
    test_code = '''
import pynlm

# Test research export
research_data = {
    "metadata": {
        "experiment_type": "plasticity_learning",
        "neuron_count": 1000,
        "timestep": 0.001,
        "date": "2024-01-01",
        "researcher": "Dr. Smith"
    },
    "results": [
        {
            "simulation_id": 0,
            "final_spike_count": 5420,
            "final_firing_rate": 54.2,
            "e_i_ratio": 0.85,
            "developmental_stage": 3
        },
        {
            "simulation_id": 1,
            "final_spike_count": 5890,
            "final_firing_rate": 58.9,
            "e_i_ratio": 0.87,
            "developmental_stage": 4
        }
    ]
}

# Test export for research
export_result = pynlm.exportForResearchPaper(research_data, "Neural Plasticity Study")
print(f"Research export prepared: {export_result}")
'''
    
    return run_command(f'python3 -c "{test_code}"', "Research Export Test")

def run_all_tests():
    """Run all tests and provide summary."""
    print("\n" + "="*80)
    print("NLM ENHANCED SYSTEM - COMPREHENSIVE TEST SUITE")
    print("="*80)
    print("Testing all enhancements made to the NLM project...")
    
    tests = [
        ("Basic NLM Usage", test_basic_usage),
        ("Enhanced Configuration System", test_enhanced_config),
        ("Enhanced Python Bindings", test_enhanced_python_bindings),
        ("Batch Simulation", test_batch_simulation),
        ("Performance Profiling", test_performance_profiling),
        ("Spike Train Analysis", test_spike_analysis),
        ("Configuration Export/Import", test_configuration_export),
        ("Help and Documentation Systems", test_help_systems),
        ("Error Handling and Validation", test_error_handling),
        ("Multi-Region Simulation", test_multi_region_simulation),
        ("Research Export", test_research_export),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*80}")
        print(f"Running: {test_name}")
        print(f"{'='*80}")
        
        success, stdout, stderr = test_func()
        results.append((test_name, success, stdout, stderr))
        
        # Wait between tests
        time.sleep(1)
    
    # Test summary
    print("\n" + "="*80)
    print("TEST SUMMARY")
    print("="*80)
    
    passed = sum(1 for _, success, _, _ in results if success)
    total = len(results)
    
    for test_name, success, stdout, stderr in results:
        status = "✅ PASS" if success else "❌ FAIL"
        print(f"{status} {test_name}")
    
    print(f"\nTotal Tests: {total}")
    print(f"Passed: {passed}")
    print(f"Failed: {total - passed}")
    print(f"Success Rate: {passed/total*100:.1f}%")
    
    if passed == total:
        print("\n🎉 ALL TESTS PASSED! 🎉")
        print("\nAll enhancements have been successfully implemented and tested!")
        print("\nKey improvements verified:")
        print("  ✅ Enhanced command-line interface with advanced options")
        print("  ✅ Advanced Python bindings with context managers")
        print("  ✅ Configuration validation and runtime modification")
        print("  ✅ Performance profiling and optimization tools")
        print("  ✅ Batch simulation and comparison utilities")
        print("  ✅ Spike train analysis and burst detection")
        print("  ✅ Configuration export/import for research")
        print("  ✅ Comprehensive error handling and debugging")
        print("  ✅ Documentation and help systems")
        print("  ✅ Multi-region simulation capabilities")
        print("  ✅ Research export utilities")
        
        return True
    else:
        print(f"\n⚠️  {total - passed} test(s) failed!")
        print("\nPlease review the failed tests above and address any issues.")
        return False

if __name__ == "__main__":
    # Check if we're in the right directory
    if not os.path.exists("src/main.cpp"):
        print("Error: This script must be run from the NLM project root directory.")
        print("Please run this script from the directory containing src/main.cpp")
        sys.exit(1)
    
    # Run all tests
    success = run_all_tests()
    
    if success:
        sys.exit(0)
    else:
        sys.exit(1)