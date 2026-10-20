#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools - Usage Example

This is a simple example showing how to use the NLM visualization and debugging tools.
It demonstrates basic functionality and can be used as a starting point for users.
"""

import time
import sys
import os

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM visualization tools
try:
    from nlm_visualization_tools import (
        VisualizationInterface,
        DebugInterface,
        VisualizationMode,
        DebugMode,
        OutputFormat,
        NLMVisualizationTools
    )
    NLM_AVAILABLE = True
    print("✓ NLM visualization tools available")
except ImportError as e:
    print(f"⚠ Warning: NLM tools not available: {e}")
    print("Using basic demonstration with Python fallback")
    NLM_AVAILABLE = False

def example_basic_usage():
    """Example 1: Basic usage"""
    print("\n" + "=" * 60)
    print("EXAMPLE 1: Basic Usage")
    print("=" * 60)
    
    if NLM_AVAILABLE:
        # Initialize NLM tools
        tools = NLMVisualizationTools()
        if not tools.initialize():
            print("Failed to initialize NLM tools")
            return
        
        # Get interfaces
        viz = tools.get_visualization_interface()
        debug = tools.get_debug_interface()
        
        # Configure interfaces
        viz.set_update_rate(30.0)
        viz.set_visualization_mode(VisualizationMode.REAL_TIME)
        
        debug.set_debug_mode(DebugMode.DETAILED)
        
        print("✓ NLM tools initialized")
        print("  - Visualization interface configured")
        print("  - Debug interface configured")
        
        # Cleanup
        tools.close()
        print("✓ Tools closed")
    
    else:
        print("Basic demonstration (Python fallback only)")
        print("  - Visualization and debugging interfaces available")
        print("  - Basic functionality: time series, debug info, monitoring hooks")
        print("  - Note: C++ library integration not available in this environment")

def example_time_series_monitoring():
    """Example 2: Time series monitoring"""
    print("\n" + "=" * 60)
    print("EXAMPLE 2: Time Series Monitoring")
    print("=" * 60)
    
    if NLM_AVAILABLE:
        tools = NLMVisualizationTools()
        tools.initialize()
        
        viz = tools.get_visualization_interface()
        
        # Add time series data
        print("Adding time series data...")
        for i in range(10):
            viz.add_time_series_metric("simulation_step", float(i))
            viz.add_time_series_metric("firing_rate", 10.0 + (i * 0.5))
            viz.add_time_series_metric("memory_usage", 50.0 + (i * 2.0))
            time.sleep(0.1)  # Simulate data collection
        
        # Retrieve and display data
        steps_data = viz.get_time_series_data("simulation_step")
        firing_data = viz.get_time_series_data("firing_rate")
        memory_data = viz.get_time_series_data("memory_usage")
        
        print(f"✓ Retrieved time series data:")
        print(f"  - Steps: {len(steps_data)} data points")
        print(f"  - Firing rate: {len(firing_data)} data points")
        print(f"  - Memory usage: {len(memory_data)} data points")
        
        # Show sample data
        print(f"\nSample Data:")
        print(f"  Step {steps_data[0].value}: Firing rate = {firing_data[0].value:.1f} Hz")
        print(f"  Step {steps_data[-1].value}: Memory usage = {memory_data[-1].value:.1f}%")
        
        tools.close()
    
    else:
        print("Time series monitoring demonstration")
        print("  - Add time series metrics")
        print("  - Retrieve data by metric name")
        print("  - Analyze trends and patterns")
        print("  - Export data for further analysis")

def example_debug_monitoring():
    """Example 3: Debug monitoring"""
    print("\n" + "=" * 60)
    print("EXAMPLE 3: Debug Monitoring")
    print("=" * 60)
    
    if NLM_AVAILABLE:
        tools = NLMVisualizationTools()
        tools.initialize()
        
        viz = tools.get_visualization_interface()
        debug = tools.get_debug_interface()
        
        # Configure debug interface
        debug.set_debug_mode(DebugMode.BASIC)
        debug.enable_error_detection(True)
        
        print("Adding debug information...")
        
        # Add debug info for different components
        from nlm_visualization_tools import DebugInfo
        
        debug_infos = [
            DebugInfo(
                component_name="BrainVisualization",
                current_state="Active",
                warnings=["High firing rate detected"],
                errors=[],
                last_update_time=time.time(),
                is_healthy=True
            ),
            DebugInfo(
                component_name="PerformanceMonitor",
                current_state="Warning",
                warnings=["High CPU usage"],
                errors=[],
                last_update_time=time.time(),
                is_healthy=False
            ),
            DebugInfo(
                component_name="MemoryManager",
                current_state="Error",
                warnings=[],
                errors=["Memory leak detected"],
                last_update_time=time.time(),
                is_healthy=False
            )
        ]
        
        for debug_info in debug_infos:
            viz.add_debug_info(debug_info)
        
        # Retrieve debug information
        all_debug_infos = viz.get_debug_info()
        
        print(f"✓ Retrieved debug information:")
        print(f"  - Total debug entries: {len(all_debug_infos)}")
        
        # Analyze debug info
        healthy_count = sum(1 for info in all_debug_infos if info.is_healthy)
        warning_count = sum(1 for info in all_debug_infos if info.warnings and not info.is_healthy)
        error_count = sum(1 for info in all_debug_infos if info.errors)
        
        print(f"\nDebug Analysis:")
        print(f"  - Healthy components: {healthy_count}")
        print(f"  - Components with warnings: {warning_count}")
        print(f"  - Components with errors: {error_count}")
        
        # Generate debug report
        report = debug.generate_debug_report()
        print(f"\n✓ Debug report generated")
        
        tools.close()
    
    else:
        print("Debug monitoring demonstration")
        print("  - Add debug information for system components")
        print("  - Track system health and warnings")
        print("  - Analyze error patterns")
        print("  - Generate debug reports")
        print("  - Monitor system state over time")

def example_monitoring_hooks():
    """Example 4: Monitoring hooks"""
    print("\n" + "=" * 60)
    print("EXAMPLE 4: Monitoring Hooks")
    print("=" * 60)
    
    if NLM_AVAILABLE:
        tools = NLMVisualizationTools()
        tools.initialize()
        
        viz = tools.get_visualization_interface()
        
        # Define custom callback functions
        def custom_performance_callback(metrics):
            print(f"Performance Callback: CPU={metrics.cpu_usage:.1f}%, Memory={metrics.memory_usage:.1f}%")
        
        def custom_memory_callback(allocations):
            leaks = [a for a in allocations if getattr(a, 'is_leaked', False)]
            if leaks:
                print(f"Memory Callback: {len(leaks)} potential memory leaks")
        
        def custom_neural_callback(neural_metrics):
            print(f"Neural Callback: {neural_metrics.active_neuron_count} active neurons, {neural_metrics.average_firing_rate:.1f} Hz")
        
        # Create monitoring hooks
        from nlm_visualization_tools import MonitoringHook
        
        hooks = [
            MonitoringHook(
                hook_name="performance_monitor",
                callback=custom_performance_callback,
                last_execution=time.time(),
                execution_interval=1.0,
                is_active=True,
                hook_type="periodic"
            ),
            MonitoringHook(
                hook_name="memory_monitor",
                callback=custom_memory_callback,
                last_execution=time.time(),
                execution_interval=2.0,
                is_active=True,
                hook_type="periodic"
            ),
            MonitoringHook(
                hook_name="neural_monitor",
                callback=custom_neural_callback,
                last_execution=time.time(),
                execution_interval=1.5,
                is_active=True,
                hook_type="periodic"
            )
        ]
        
        # Add hooks
        for hook in hooks:
            viz.add_monitoring_hook(hook)
        
        # Retrieve and display hooks
        all_hooks = viz.get_monitoring_hooks()
        
        print(f"✓ Added {len(hooks)} monitoring hooks")
        print(f"  - Total active hooks: {len(all_hooks)}")
        
        # Display hook information
        print(f"\nMonitoring Hooks:")
        for hook in all_hooks:
            print(f"  - {hook.hook_name}: {hook.hook_type} (active: {hook.is_active})")
        
        # Execute hooks manually
        print(f"\nExecuting hooks manually:")
        for hook in all_hooks:
            if hook.is_active:
                print(f"  Executing {hook.hook_name}...")
        
        tools.close()
    
    else:
        print("Monitoring hooks demonstration")
        print("  - Define custom callback functions")
        print("  - Create monitoring hooks with intervals")
        print("  - Add hooks to visualization interface")
        print("  - Retrieve and manage hooks")
        print("  - Execute hooks manually or automatically")

def example_data_export():
    """Example 5: Data export"""
    print("\n" + "=" * 60)
    print("EXAMPLE 5: Data Export")
    print("=" * 60)
    
    if NLM_AVAILABLE:
        tools = NLMVisualizationTools()
        tools.initialize()
        
        viz = tools.get_visualization_interface()
        debug = tools.get_debug_interface()
        
        # Configure for export
        viz.set_visualization_mode(VisualizationMode.STATIC)
        
        # Add some sample data
        viz.add_time_series_metric("step", 1.0)
        viz.add_time_series_metric("step", 2.0)
        viz.add_time_series_metric("firing_rate", 12.5)
        
        from nlm_visualization_tools import DebugInfo
        viz.add_debug_info(DebugInfo(
            component_name="SampleComponent",
            current_state="Active",
            warnings=[],
            errors=[],
            last_update_time=time.time(),
            is_healthy=True
        ))
        
        # Export data
        output_file = "example_export.json"
        tools.export_data(output_file, OutputFormat.JSON)
        
        print(f"✓ Data exported to: {output_file}")
        
        # Display file info
        if os.path.exists(output_file):
            file_size = os.path.getsize(output_file)
            print(f"  - File size: {file_size} bytes")
        
        # Read and display exported data
        try:
            with open(output_file, 'r') as f:
                exported_data = json.load(f)
            
            print(f"\nExported Data Summary:")
            print(f"  - Keys in exported data: {list(exported_data.keys())}")
            
            if 'time_series_data' in exported_data:
                print(f"  - Time series entries: {len(exported_data['time_series_data'])}")
            
            if 'debug_info' in exported_data:
                print(f"  - Debug info entries: {len(exported_data['debug_info'])}")
                
        except Exception as e:
            print(f"  - Could not read exported data: {e}")
        
        tools.close()
        
        # Clean up
        if os.path.exists(output_file):
            os.remove(output_file)
            print(f"✓ Cleaned up: {output_file}")
    
    else:
        print("Data export demonstration")
        print("  - Configure export settings")
        print("  - Collect data from visualization and debugging")
        print("  - Export to JSON, CSV, or other formats")
        print("  - Import data for analysis")
        print("  - Generate reports and summaries")

def main():
    """Main function"""
    print("NLM Visualization and Debugging Tools - Usage Example")
    print("=" * 60)
    print("\nThis example demonstrates the basic usage of the NLM")
    print("visualization and debugging tools. It shows:")
    print("  1. Basic initialization and configuration")
    print("  2. Time series monitoring")
    print("  3. Debug monitoring")
    print("  4. Monitoring hooks")
    print("  5. Data export")
    
    try:
        # Example 1: Basic usage
        example_basic_usage()
        
        # Example 2: Time series monitoring
        example_time_series_monitoring()
        
        # Example 3: Debug monitoring
        example_debug_monitoring()
        
        # Example 4: Monitoring hooks
        example_monitoring_hooks()
        
        # Example 5: Data export
        example_data_export()
        
        print("\n" + "=" * 60)
        print("EXAMPLES COMPLETED SUCCESSFULLY")
        print("=" * 60)
        print("\nThe NLM visualization and debugging tools provide a")
        print("comprehensive framework for monitoring, analyzing, and")
        print("debugging neural simulations.")
        print("\nKey features available:")
        print("  • Real-time visualization and debugging")
        print("  • Performance monitoring and profiling")
        print("  • Memory leak detection")
        print("  • Error detection and analysis")
        print("  • Interactive debugging interface")
        print("  • Data export and reporting")
        print("  • Monitoring hooks and callbacks")
        
        return 0
        
    except Exception as e:
        print(f"\nError during examples: {e}")
        return 1

if __name__ == "__main__":
    exit_code = main()
    exit(exit_code)
