"""
Example usage of NLM advanced utilities for NLM neural simulation framework.

This file demonstrates how to use the various utility modules created for advanced users.
"""

import sys
import os

# Add the project root to the path so we can import nlm
project_root = os.path.dirname(os.path.abspath(__file__))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

try:
    from nlm_utilities.performance.performance_monitor import PerformanceMonitor
    from nlm_utilities.configuration.config_utils import ConfigurationUtils
    from nlm_utilities.debugging.debugger import Debugger
    from nlm_utilities.export.export_utils import ExportUtils
    from nlm_utilities.analysis.analysis_tools import AnalysisTools
    print("Successfully imported all NLM utilities")
except ImportError as e:
    print(f"Warning: Some utilities could not be imported: {e}")
    # Create placeholder classes for demonstration
    class PerformanceMonitor: pass
    class ConfigurationUtils: pass
    class Debugger: pass
    class ExportUtils: pass
    class AnalysisTools: pass

def example_performance_monitoring():
    """Example: Performance monitoring utility."""
    print("\n=== Performance Monitoring Example ===")
    
    # In a real scenario, this would be connected to an actual brain object
    # brain = create_brain()
    
    # Initialize performance monitor
    perf_monitor = PerformanceMonitor(brain=None)
    
    # Start monitoring
    perf_monitor.start_monitoring()
    
    # Simulate some work
    for i in range(5):
        # In real usage, this would be called in the simulation loop
        pass
        # perf_monitor.capture_metrics(step=i)
        
    # Stop monitoring
    perf_monitor.stop_monitoring()
    
    # Get performance statistics
    stats = perf_monitor.get_statistics()
    print(f"Performance stats: {stats}")
    
    # Generate report
    report = perf_monitor.generate_report()
    print("Performance Report:")
    print(report)
    
def example_configuration_utilities():
    """Example: Configuration utilities."""
    print("\n=== Configuration Utilities Example ===")
    
    config_utils = ConfigurationUtils()
    
    # Example configuration
    config1 = {
        'brain': {
            'region_count': 10,
            'neuron_count': 1000
        },
        'network': {
            'connectivity': 'auto',
            'plasticity': 'enabled'
        },
        'simulation': {
            'timestep': 0.1,
            'duration': 100.0
        }
    }
    
    config2 = {
        'brain': {
            'region_count': 10,
            'neuron_count': 1000
        },
        'network': {
            'connectivity': 'auto',
            'plasticity': 'disabled'  # Changed from config1
        },
        'simulation': {
            'timestep': 0.1,
            'duration': 100.0
        }
    }
    
    # Compare configurations
    comparison = config_utils.compare_configs(config1, config2)
    print(f"Configurations identical: {comparison['identical']}")
    print(f"Difference hash: {comparison.get('hash1', 'N/A')} vs {comparison.get('hash2', 'N/A')}")
    
    # Validate configuration
    validation = config_utils.validate_config(config1)
    print(f"Config valid: {validation['valid']}")
    if validation['errors']:
        print(f"Errors: {validation['errors']}")
    if validation['warnings']:
        print(f"Warnings: {validation['warnings']}")
    
    # Create backup
    backup_path = config_utils.create_backup(config1, "example_backup")
    print(f"Backup created: {backup_path}")
    
def example_debugging_utilities():
    """Example: Debugging utilities."""
    print("\n=== Debugging Utilities Example ===")
    
    debugger = Debugger(brain=None)
    
    # Start recording
    debugger.start_recording()
    
    # Capture debug data (simulated)
    for step in range(3):
        debugger.capture_debug_data(step)
        
    # Stop recording
    debugger.stop_recording()
    
    # Analyze spike patterns
    spikes = debugger.trace_spikes(neuron_id=1)
    print(f"Traced {len(spikes)} spikes")
    
    # Analyze spike patterns
    analysis = debugger.analyze_spike_patterns()
    print(f"Spike analysis: {analysis}")
    
    # Generate debug report
    report = debugger.generate_debug_report()
    print("Debug Report (first 500 chars):")
    print(report[:500] + "...")
    
def example_export_utilities():
    """Example: Export utilities."""
    print("\n=== Export Utilities Example ===")
    
    export_utils = ExportUtils(brain=None)
    
    # Start recording
    export_utils.start_recording("example_simulation")
    
    # Capture some frames
    for step in range(3):
        export_utils.capture_frame(step, step * 0.1)
        
    # Stop recording
    export_utils.stop_recording()
    
    # Export to different formats
    json_path = export_utils.export_recording(format='json')
    print(f"Exported recording to JSON: {json_path}")
    
    # In a real scenario, you would also export network structure
    # network_path = export_utils.export_network("network.json")
    # brain_state_path = export_utils.export_brain_state("brain_state.json")
    
def example_analysis_utilities():
    """Example: Analysis utilities."""
    print("\n=== Analysis Utilities Example ===")
    
    analysis_tools = AnalysisTools(brain=None)
    
    # This would require an actual recording file
    recording_file = "example_recording.json"
    
    # Check if file exists
    if os.path.exists(recording_file):
        # Run complete analysis
        results = analysis_tools.run_complete_analysis(recording_file)
        
        print(f"Analysis completed with {len(results)} result categories")
        
        # Export analysis results
        output_file = "analysis_results.json"
        analysis_tools.export_analysis_results(results, output_file, format='json')
        print(f"Analysis results exported to: {output_file}")
        
        # Print summary
        print("\nAnalysis Summary:")
        for category, data in results.items():
            if isinstance(data, dict) and 'error' not in data:
                print(f"  {category}: Success")
            else:
                print(f"  {category}: Error or empty")
    else:
        print(f"Recording file {recording_file} not found - skipping analysis example")
        
def example_configuration_validation():
    """Example: Configuration validation with schemas."""
    print("\n=== Configuration Validation Example ===")
    
    config_utils = ConfigurationUtils()
    
    # Define a simple JSON schema
    schema = {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "type": "object",
        "properties": {
            "brain": {
                "type": "object",
                "properties": {
                    "region_count": {"type": "integer", "minimum": 1},
                    "neuron_count": {"type": "integer", "minimum": 1}
                },
                "required": ["region_count", "neuron_count"]
            },
            "network": {
                "type": "object",
                "properties": {
                    "connectivity": {"type": "string"},
                    "plasticity": {"type": "string"}
                }
            }
        },
        "required": ["brain", "network"]
    }
    
    # Add schema
    config_utils.add_schema("basic", schema)
    
    # Test configuration
    config = {
        "brain": {"region_count": 5, "neuron_count": 100},
        "network": {"connectivity": "auto", "plasticity": "enabled"}
    }
    
    # Validate against schema
    validation = config_utils.validate_against_schema(config, "basic")
    print(f"Schema validation result: {validation['valid']}")
    
    # Test invalid configuration
    invalid_config = {
        "brain": {"region_count": 0, "neuron_count": 100},  # region_count should be >= 1
        "network": {"connectivity": "auto"}
    }
    
    invalid_validation = config_utils.validate_against_schema(invalid_config, "basic")
    print(f"Invalid config validation: {invalid_validation['valid']}")
    if invalid_validation['errors']:
        print(f"Errors: {invalid_validation['errors']}")

def main():
    """Run all examples."""
    print("NLM Advanced Utilities - Examples")
    print("=" * 50)
    
    example_performance_monitoring()
    example_configuration_utilities()
    example_debugging_utilities()
    example_export_utilities()
    example_analysis_utilities()
    example_configuration_validation()
    
    print("\n" + "=" * 50)
    print("All examples completed!")
    print("\nNote: This is a demonstration script. For real usage, you would:")
    print("1. Create an actual NLM brain object")
    print("2. Connect it to the utilities")
    print("3. Run real neural simulations")
    print("4. Use the utilities to monitor, analyze, and export data")

if __name__ == "__main__":
    main()