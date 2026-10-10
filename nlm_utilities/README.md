"""
NLM Advanced Utilities Package

This package provides comprehensive utilities for advanced users of the NLM (Neural Learning Machine) neural simulation framework.

Overview:
---------
The NLM framework is a C++ neural simulation system with Python bindings. These utilities extend the framework with advanced functionality for performance monitoring, configuration management, debugging, export/import operations, and data analysis.

Features:
----------
1. Performance Monitoring Utilities
   - Real-time performance monitoring with timing statistics
   - Memory usage tracking and reporting
   - CPU usage analysis
   - Bottleneck identification

2. Advanced Configuration Utilities
   - Configuration comparison and diffing
   - Configuration validation and schema checking
   - Configuration backup and restore
   - Configuration migration tools

3. Debugging and Profiling Utilities
   - Neural activity visualization
   - Connection graph debugging
   - Spike tracing and analysis
   - Memory state inspection

4. Export/Import Utilities
   - Neural network export for analysis
   - Brain state export/import
   - Simulation recording and playback
   - Data analysis export formats

5. Advanced Analysis Tools
   - Network topology analysis
   - Plasticity rule effectiveness analysis
   - Learning curve analysis
   - Behavioral pattern analysis

Installation:
-------------
The utilities are designed to work with the existing NLM installation:

1. Clone the NLM repository
2. Build the NLM framework (C++)
3. Install Python bindings
4. Install this utilities package:

```bash
pip install -e ./nlm_utilities
```

Requirements:
-------------
- Python 3.8+
- NumPy
- Pandas
- Matplotlib
- SciPy
- NetworkX
- h5py
- PyYAML

Basic Usage:
------------
"""

import os
import sys
from pathlib import Path

def check_requirements():
    """Check if required packages are installed."""
    required_packages = [
        'numpy',
        'pandas',
        'matplotlib',
        'scipy',
        'networkx',
        'h5py',
        'yaml'
    ]
    
    missing = []
    for package in required_packages:
        try:
            __import__(package)
        except ImportError:
            missing.append(package)
            
    if missing:
        print(f"Missing required packages: {missing}")
        print("Install with: pip install", ' '.join(missing))
        return False
    return True

def show_examples():
    """Show example usage."""
    print("\nExample Usage:")
    print("-------------")
    
    print("\n1. Performance Monitoring:")
    print("""
from nlm_utilities.performance.performance_monitor import PerformanceMonitor

# Create performance monitor (connect to brain in real usage)
perf_monitor = PerformanceMonitor(brain=your_brain_object)
perf_monitor.start_monitoring()

# In simulation loop:
# perf_monitor.capture_metrics(step=current_step)

# Get statistics
stats = perf_monitor.get_statistics()
print(f"CPU Usage: {stats['avg_cpu_usage']:.1f}%")
print(f"Memory Usage: {stats['avg_memory_usage']:.1f}%")

# Generate report
report = perf_monitor.generate_report()
""")
    
    print("\n2. Configuration Utilities:")
    print("""
from nlm_utilities.configuration.config_utils import ConfigurationUtils

config_utils = ConfigurationUtils()

# Load and save configurations
config = config_utils.load_config("config.json")
config_utils.save_config(config, "new_config.json")

# Compare configurations
diff = config_utils.compare_configs(config1, config2)

# Validate with schema
validation = config_utils.validate_config(config, schema)

# Backup and restore
backup_path = config_utils.create_backup(config)
restored_config = config_utils.restore_backup(backup_path)
""")
    
    print("\n3. Debugging Utilities:")
    print("""
from nlm_utilities.debugging.debugger import Debugger

debugger = Debugger(brain=your_brain_object)
debugger.start_recording()

# In simulation loop:
# debugger.capture_debug_data(step=current_step)

debugger.stop_recording()

# Analyze activity
spikes = debugger.trace_spikes(neuron_id=123)
analysis = debugger.analyze_spike_patterns()

# Visualize
debugger.visualize_activity("activity_plot.png", neurons_subset=[1,2,3])
debugger.visualize_connection_graph("connectivity_graph.png")
""")
    
    print("\n4. Export Utilities:")
    print("""
from nlm_utilities.export.export_utils import ExportUtils

export_utils = ExportUtils(brain=your_brain_object)

# Start recording
export_utils.start_recording("my_simulation")

# In simulation loop:
# export_utils.capture_frame(step=current_step, simulation_time=time)

export_utils.stop_recording()

# Export recordings
export_utils.export_recording("simulation.json")
export_utils.export_recording("simulation.h5")

# Export network structure
network_path = export_utils.export_network("network_analysis.json")

# Export brain state
brain_state_path = export_utils.export_brain_state("brain_state.json")
""")
    
    print("\n5. Analysis Tools:")
    print("""
from nlm_utilities.analysis.analysis_tools import AnalysisTools

analysis_tools = AnalysisTools(brain=your_brain_object)

# Run complete analysis on recording
results = analysis_tools.run_complete_analysis("simulation.json")

# Export results
analysis_tools.export_analysis_results(results, "analysis_results.json")

# Individual analyzers
topology = analysis_tools.topology_analyzer.analyze_connectivity_matrix()
plasticity = analysis_tools.plasticity_analyzer.analyze_plasticity_effectiveness()
learning = analysis_tools.learning_curve_analyzer.analyze_performance_learning("simulation.json")
behavioral = analysis_tools.behavioral_pattern_analyzer.analyze_action_patterns("simulation.json")
""")

def show_api_reference():
    """Show API reference information."""
    print("\nAPI Reference:")
    print("-------------")
    
    print("\nPerformance Monitor:")
    print("- start_monitoring() / stop_monitoring()")
    print("- get_current_metrics() / get_metrics_history()")
    print("- get_statistics() / export_metrics()")
    print("- generate_report()")
    
    print("\nConfiguration Utils:")
    print("- load_config() / save_config()")
    print("- compare_configs()")
    print("- validate_config() / validate_against_schema()")
    print("- create_backup() / restore_backup()")
    print("- list_backups()")
    
    print("\nDebugger:")
    print("- start_recording() / stop_recording()")
    print("- capture_debug_data()")
    print("- trace_spikes() / analyze_spike_patterns()")
    print("- visualize_activity() / visualize_connection_graph()")
    print("- inspect_memory_state()")
    print("- export_debug_data() / import_debug_data()")
    print("- generate_debug_report()")
    
    print("\nExport Utils:")
    print("- start_recording() / stop_recording() / capture_frame()")
    print("- export_recording()")
    print("- export_network() / export_brain_state()")
    print("- import_brain_state()")
    
    print("\nAnalysis Tools:")
    print("- run_complete_analysis()")
    print("- export_analysis_results()")
    print("- Individual analyzers (topology, plasticity, learning, behavioral)")

def main():
    """Main help function."""
    if len(sys.argv) > 1:
        if sys.argv[1] == "check":
            if check_requirements():
                print("All requirements satisfied.")
            return
        elif sys.argv[1] == "examples":
            show_examples()
            return
        elif sys.argv[1] == "api":
            show_api_reference()
            return
            
    print("NLM Advanced Utilities")
    print("=" * 40)
    print("\nThis package provides advanced utilities for the NLM neural simulation framework.")
    print("\nUsage:")
    print("  python -m nlm_utilities.check          - Check requirements")
    print("  python -m nlm_utilities.examples      - Show usage examples")
    print("  python -m nlm_utilities.api           - Show API reference")
    print("\nFor installation and usage instructions, see the documentation.")

if __name__ == "__main__":
    main()