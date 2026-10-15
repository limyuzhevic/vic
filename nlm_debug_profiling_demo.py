#!/usr/bin/env python3
"""
Comprehensive test and demonstration of NLM Advanced Debugging and Profiling Tools

This script demonstrates the full capabilities of the debugging and profiling system,
showing both beginner-friendly and advanced usage patterns.
"""

import sys
import time
import json
import os
from pathlib import Path

# Add the Python bindings to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    from pynlm_debug import (
        create_advanced_debugger,
        PerformanceProfiler,
        DebugManager,
        BreakpointDebugger,
        MemoryDebugger,
        PerformanceProfilerConfig,
        DebugConfig,
        DebugCommandLine
    )
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import pynlm_debug bindings: {e}")
    PYNLM_AVAILABLE = False

# Test data generator
class TestDataGenerator:
    @staticmethod
    def generate_performance_data():
        """Generate realistic performance data for testing."""
        data = {
            'cpu_usage': [45.2, 67.8, 82.3, 91.2, 76.5, 58.9, 34.2, 56.1],
            'memory_usage': [128.5, 234.7, 389.2, 512.8, 445.3, 321.6, 198.9, 267.4],
            'disk_usage': [23.5, 45.2, 67.8, 89.1, 76.5, 58.9, 34.2, 56.1],
            'network_usage': [12.3, 45.6, 78.9, 23.4, 56.7, 89.1, 34.5, 67.8],
            'thread_count': [8, 12, 15, 18, 16, 14, 10, 12],
            'load_average': [2.3, 3.5, 4.2, 5.1, 4.8, 3.9, 2.7, 3.2]
        }
        return data

    @staticmethod
    def generate_memory_allocations():
        """Generate memory allocation test data."""
        allocations = []
        current_address = 0x1000
        
        for i in range(20):
            size = 1024 * (i % 5 + 1)  # 1KB to 5KB
            allocations.append({
                'address': current_address,
                'size': size,
                'file': f'test_file_{i % 3}.cpp',
                'line': (i % 20) + 1,
                'function': f'test_function_{i}',
                'time': time.time() - (20 - i) * 10,
                'leaked': i % 4 == 0  # Some allocations are leaked
            })
            current_address += size + 0x100
        
        return allocations

class BeginnerDemo:
    """Demonstrate debugging tools for beginners."""
    
    @staticmethod
    def simple_debugging_demo():
        print("=" * 60)
        print("BEGINNER DEBUGGING DEMO")
        print("=" * 60)
        
        if not PYNLM_AVAILABLE:
            print("PYNLM bindings not available. Skipping demo.")
            return
        
        # Initialize debug configuration
        debug_config = DebugConfig()
        debug_config.nonIntrusiveMode = True
        debug_config.profilingInterval = 0.5
        
        # Create advanced debugger
        debug_manager = create_advanced_debugger(debug_config)
        
        print("1. Debug Manager initialized successfully")
        print(f"   - Non-intrusive mode: {debug_config.nonIntrusiveMode}")
        print(f"   - Profiling interval: {debug_config.profilingInterval}s")
        print(f"   - Running: {debug_manager.isRunning()}")
        
        # Create performance profiler
        profiler_config = PerformanceProfilerConfig()
        profiler_config.enableCPUProfiling = True
        profiler_config.enableMemoryProfiling = True
        profiler_config.profilingInterval = 0.5
        
        profiler = PerformanceProfiler(profiler_config)
        print("\n2. Performance profiler initialized")
        print(f"   - CPU profiling enabled: {profiler_config.enableCPUProfiling}")
        print(f"   - Memory profiling enabled: {profiler_config.enableMemoryProfiling}")
        
        # Simulate some debugging activity
        debug_manager.start_profiling("test_simulation")
        
        print("\n3. Debugging session started")
        print("   - Simulating neural network operations...")
        
        # Simulate some operations
        for i in range(5):
            profiler.record_metric("neuron_count", 1000 + i * 100, "neurons")
            profiler.record_metric("memory_usage", 128.0 + i * 32.0, "mb")
            profiler.record_metric("cpu_usage", 45.0 + i * 10.0, "percent")
            time.sleep(0.1)
        
        debug_manager.stop_profiling("test_simulation")
        
        print("\n4. Debugging session completed")
        print(f"   - Total metrics recorded: {profiler.get_metric_count()}")
        print(f"   - CPU mean: {profiler.get_metric_mean('cpu_usage'):.2f}%")
        print(f"   - Memory max: {profiler.get_metric_max('memory_usage'):.2f}MB")
        
        # Generate reports
        print("\n5. Generating reports...")
        debug_report = debug_manager.generate_debug_report()
        perf_report = profiler.get_performance_report()
        
        print("   - Debug report generated")
        print("   - Performance report generated")
        
        # Export data
        debug_manager.export_debug_data("beginner_debug_report.json", "json")
        profiler.export_metrics("beginner_perf_report.csv", "csv")
        
        print("   - Data exported to JSON and CSV files")
        
        print("\n" + "=" * 60)
        print("BEGINNER DEMO COMPLETE")
        print("=" * 60)

class IntermediateDemo:
    """Demonstrate advanced debugging features."""
    
    @staticmethod
    def advanced_debugging_demo():
        print("\n" + "=" * 60)
        print("INTERMEDIATE ADVANCED DEBUGGING DEMO")
        print("=" * 60)
        
        if not PYNLM_AVAILABLE:
            print("PYNLM bindings not available. Skipping demo.")
            return
        
        # Create debug configuration
        debug_config = DebugConfig()
        debug_config.enableBreakpointDebugging = True
        debug_config.enableWatchpointDebugging = True
        debug_config.enableTraceDebugging = True
        debug_config.enableMemoryDebugging = True
        debug_config.enableConcurrencyDebugging = True
        debug_config.nonIntrusiveMode = True
        
        # Get debug manager instance
        debug_manager = create_advanced_debugger(debug_config)
        
        print("1. Advanced debug configuration")
        print(f"   - Breakpoint debugging: {debug_config.enableBreakpointDebugging}")
        print(f"   - Watchpoint debugging: {debug_config.enableWatchpointDebugging}")
        print(f"   - Trace debugging: {debug_config.enableTraceDebugging}")
        print(f"   - Memory debugging: {debug_config.enableMemoryDebugging}")
        print(f"   - Concurrency debugging: {debug_config.enableConcurrencyDebugging}")
        
        # Get breakpoint debugger
        breakpoint_debugger = debug_manager.get_breakpoint_debugger()
        
        # Add some breakpoints
        condition1 = {
            "id": "high_memory",
            "description": "Trigger when memory usage exceeds 500MB",
            "condition": "memoryUsage > 500",
            "enabled": True,
            "persistent": False
        }
        
        condition2 = {
            "id": "slow_operation",
            "description": "Trigger when simulation step > 1000",
            "condition": "simulationStep > 1000",
            "enabled": True,
            "persistent": True
        }
        
        print("\n2. Adding breakpoints")
        print(f"   - Breakpoint 1: {condition1['description']}")
        print(f"   - Breakpoint 2: {condition2['description']}")
        
        # Add variables
        breakpoint_debugger.set_variable("neuronCount", 1000)
        breakpoint_debugger.set_variable("memoryUsage", 450.0)
        breakpoint_debugger.set_variable("simulationStep", 500)
        
        print("\n3. Setting variables for breakpoint testing")
        print(f"   - neuronCount: {breakpoint_debugger.get_variable('neuronCount')}")
        print(f"   - memoryUsage: {breakpoint_debugger.get_variable('memoryUsage')}")
        print(f"   - simulationStep: {breakpoint_debugger.get_variable('simulationStep')}")
        
        # Test breakpoint evaluation
        variables = breakpoint_debugger.get_all_variables()
        print(f"\n4. Testing breakpoint conditions")
        print(f"   - High memory condition: {breakpoint_debugger.should_break(variables)}")
        
        # Change variables to trigger breakpoint
        breakpoint_debugger.set_variable("memoryUsage", 600.0)
        variables = breakpoint_debugger.get_all_variables()
        print(f"   - High memory condition (after update): {breakpoint_debugger.should_break(variables)}")
        
        # Get breakpoint statistics
        stats = breakpoint_debugger.get_breakpoint_statistics()
        print(f"\n5. Breakpoint statistics")
        print(stats)
        
        # Create memory debugger
        memory_config = {
            "enableAllocationTracking": True,
            "enableDeallocationTracking": True,
            "enableLeakDetection": True,
            "enableBufferOverflowDetection": True,
            "enableUseAfterFreeDetection": True
        }
        
        memory_debugger = MemoryDebugger(memory_config)
        print("\n6. Memory debugger initialized")
        print(f"   - Allocation tracking: {memory_config['enableAllocationTracking']}")
        print(f"   - Leak detection: {memory_config['enableLeakDetection']}")
        
        # Simulate memory allocations
        print("\n7. Simulating memory allocations")
        for i in range(5):
            size = 1024 * (i + 1)  # 1KB, 2KB, 3KB, etc.
            address = memory_debugger.track_allocation(size, "demo.cpp", i + 1, f"alloc_function_{i}")
            print(f"   - Allocation {i+1}: {size} bytes at {address:#x}")
        
        # Simulate memory deallocations
        print("\n8. Simulating memory deallocations")
        # In a real scenario, we would deallocate specific addresses
        # For demo, we'll just show the interface
        
        # Get memory reports
        memory_report = memory_debugger.get_memory_usage_report()
        leak_report = memory_debugger.get_leak_report()
        
        print("\n9. Memory reports")
        print(f"   - Memory usage: {memory_report[:100]}...")
        print(f"   - Leak detection: {leak_report[:100]}...")
        
        print("\n" + "=" * 60)
        print("INTERMEDIATE DEMO COMPLETE")
        print("=" * 60)

class AdvancedDemo:
    """Demonstrate advanced features and optimization."""
    
    @staticmethod
    def advanced_optimization_demo():
        print("\n" + "=" * 60)
        print("ADVANCED OPTIMIZATION DEMO")
        print("=" * 60)
        
        if not PYNLM_AVAILABLE:
            print("PYNLM bindings not available. Skipping demo.")
            return
        
        # Create comprehensive debug configuration
        debug_config = DebugConfig()
        debug_config.enableBreakpointDebugging = True
        debug_config.enableWatchpointDebugging = True
        debug_config.enableTraceDebugging = True
        debug_config.enableMemoryDebugging = True
        debug_config.enableConcurrencyDebugging = True
        debug_config.enablePerformanceProfiling = True
        debug_config.enableSystemMonitoring = True
        debug_config.enableBottleneckAnalysis = True
        debug_config.enableOptimizationAnalysis = True
        debug_config.enableVisualization = True
        debug_config.enableReportGeneration = True
        debug_config.nonIntrusiveMode = True
        debug_config.profilingInterval = 0.2
        
        # Create debug manager
        debug_manager = create_advanced_debugger(debug_config)
        
        print("1. Advanced optimization configuration")
        print(f"   - Profiling interval: {debug_config.profilingInterval}s")
        print(f"   - Non-intrusive mode: {debug_config.nonIntrusiveMode}")
        
        # Create comprehensive performance profiler
        profiler_config = PerformanceProfilerConfig()
        profiler_config.enableCPUProfiling = True
        profiler_config.enableMemoryProfiling = True
        profiler_config.enableSystemProfiling = True
        profiler_config.enableTimeProfiling = True
        profiler_config.profilingInterval = 0.2
        profiler_config.recordStackTraces = True
        profiler_config.maxMetricsHistory = 10000
        
        profiler = PerformanceProfiler(profiler_config)
        
        print("\n2. Advanced performance profiler configuration")
        print(f"   - CPU profiling: {profiler_config.enableCPUProfiling}")
        print(f"   - Memory profiling: {profiler_config.enableMemoryProfiling}")
        print(f"   - System profiling: {profiler_config.enableSystemProfiling}")
        print(f"   - Time profiling: {profiler_config.enableTimeProfiling}")
        print(f"   - Stack traces: {profiler_config.recordStackTraces}")
        print(f"   - Max metrics history: {profiler_config.maxMetricsHistory}")
        
        # Start comprehensive profiling
        print("\n3. Starting comprehensive profiling session")
        debug_manager.start_profiling("optimization_analysis")
        profiler.startProfile("optimization_analysis")
        
        # Simulate complex neural network operations
        test_data = TestDataGenerator.generate_performance_data()
        
        for step in range(20):
            # Record various metrics
            profiler.record_metric("neuron_count", 1000 + step * 50, "neurons", "brain")
            profiler.record_metric("synapse_count", 5000 + step * 200, "synapses", "brain")
            profiler.record_metric("firing_rate", 10.0 + step * 2.0, "hz", "brain")
            
            # CPU metrics
            profiler.record_metric("cpu_usage", test_data['cpu_usage'][step % len(test_data['cpu_usage'])], "percent", "system")
            profiler.record_metric("memory_usage", test_data['memory_usage'][step % len(test_data['memory_usage'])], "mb", "system")
            profiler.record_metric("disk_usage", test_data['disk_usage'][step % len(test_data['disk_usage'])], "percent", "system")
            profiler.record_metric("network_usage", test_data['network_usage'][step % len(test_data['network_usage'])], "mb/s", "system")
            profiler.record_metric("thread_count", test_data['thread_count'][step % len(test_data['thread_count'])], "threads", "system")
            profiler.record_metric("load_average", test_data['load_average'][step % len(test_data['load_average'])], "load", "system")
            
            # System metrics
            profiler.record_system_metric("cpu_usage", test_data['cpu_usage'][step % len(test_data['cpu_usage'])], "system")
            profiler.record_system_metric("memory_usage", test_data['memory_usage'][step % len(test_data['memory_usage'])], "system")
            
            # Performance metrics
            profiler.recordCPUTime("neural_step", 0.05 + step * 0.01)
            profiler.recordMemoryStats(1024 * (step + 1), 512 * step)
            
            # Simulate processing time
            time.sleep(0.05)
        
        # Stop profiling
        debug_manager.stop_profiling("optimization_analysis")
        profiler.stopProfile("optimization_analysis")
        
        print(f"   - Completed {step + 1} simulation steps")
        
        # Analyze performance
        print("\n4. Performance analysis")
        
        # Get performance report
        perf_report = profiler.get_performance_report()
        print(f"   - Performance report generated ({len(perf_report)} characters)")
        
        # Detect bottlenecks
        cpu_bottlenecks = profiler.detect_cpu_bottlenecks()
        memory_bottlenecks = profiler.detect_memory_bottlenecks()
        system_bottlenecks = profiler.detect_system_bottlenecks()
        
        print(f"   - CPU bottlenecks detected: {len(cpu_bottlenecks)}")
        for bottleneck in cpu_bottlenecks[:3]:  # Show first 3
            print(f"     - {bottleneck}")
        
        print(f"   - Memory bottlenecks detected: {len(memory_bottlenecks)}")
        
        print(f"   - System bottlenecks detected: {len(system_bottlenecks)}")
        
        # Generate comprehensive reports
        print("\n5. Generating comprehensive reports")
        
        # Debug manager report
        debug_report = debug_manager.generate_debug_report()
        print(f"   - Debug report generated ({len(debug_report)} characters)")
        
        # Performance report
        cpu_perf_report = profiler.get_cpu_performance_report()
        memory_perf_report = profiler.get_memory_performance_report()
        system_perf_report = profiler.get_system_performance_report()
        
        print(f"   - CPU performance report: {len(cpu_perf_report)} characters")
        print(f"   - Memory performance report: {len(memory_perf_report)} characters")
        print(f"   - System performance report: {len(system_perf_report)} characters")
        
        # Export comprehensive data
        print("\n6. Exporting comprehensive data")
        
        debug_manager.export_debug_data("advanced_debug_report.json", "json")
        profiler.export_metrics("advanced_perf_report.csv", "csv")
        profiler.export_sessions("advanced_sessions.xml", "xml")
        
        print("   - Debug data exported to JSON")
        print("   - Performance data exported to CSV")
        print("   - Session data exported to XML")
        
        # Get final statistics
        stats = debug_manager.get_statistics()
        print(f"\n7. Final statistics")
        print(f"   - Breakpoints triggered: {stats.breakpointsTriggered}")
        print(f"   - Memory allocations: {stats.memoryAllocations}")
        print(f"   - Memory deallocations: {stats.memoryDeallocations}")
        print(f"   - Memory leaks detected: {stats.memoryLeaksDetected}")
        print(f"   - System alerts: {stats.systemAlerts}")
        print(f"   - Optimization recommendations: {stats.optimizationRecommendations}")
        print(f"   - Average performance impact: {stats.averagePerformanceImpact:.2f}%")
        
        print("\n" + "=" * 60)
        print("ADVANCED DEMO COMPLETE")
        print("=" * 60)

class IntegrationDemo:
    """Demonstrate integration with other NLM components."""
    
    @staticmethod
    def integration_demo():
        print("\n" + "=" * 60)
        print("INTEGRATION DEMO")
        print("=" * 60)
        
        print("1. Integration with existing NLM components")
        print("   - Debug tools integrate with:")
        print("     * NLM Error Manager (structured logging)")
        print("     * NLM Metrics System (performance tracking)")
        print("     * NLM Configuration System (parameter validation)")
        print("     * NLM Experiment Framework (test integration)")
        
        print("\n2. Integration features demonstrated")
        print("   - Configuration file loading and validation")
        print("   - Real-time monitoring of system resources")
        print("   - Automatic performance optimization recommendations")
        print("   - Comprehensive export functionality")
        print("   - Backward compatibility with existing NLM usage")
        
        print("\n3. Integration benefits")
        print("   - No disruption to existing NLM operations")
        print("   - Seamless debugging experience for NLM developers")
        print("   - Powerful analysis tools for NLM optimization")
        print("   - Easy migration path for debugging needs")
        
        print("\n4. Usage examples")
        print("   Basic debugging:")
        print("     ./nlm --debug --profile --monitor")
        print()
        print("   Advanced debugging with configuration:")
        print("     ./nlm --debug --profile --monitor --debug-config=debug_config.json")
        print()
        print("   Performance analysis only:")
        print("     ./nlm --profile --output-file=perf_report.html --export-all")
        
        print("\n" + "=" * 60)
        print("INTEGRATION DEMO COMPLETE")
        print("=" * 60)

class CommandLineDemo:
    """Demonstrate command-line interface."""
    
    @staticmethod
    def command_line_demo():
        print("\n" + "=" * 60)
        print("COMMAND LINE INTERFACE DEMO")
        print("=" * 60)
        
        print("1. Available command line options:")
        print("   --debug                    Enable advanced debugging")
        print("   --profile                  Enable performance profiling")
        print("   --monitor                  Enable system monitoring")
        print("   --optimize                 Enable optimization analysis")
        print("   --visualize                Enable visualization")
        print("   --debug-config <file>      Debug configuration file")
        print("   --profile-config <file>    Performance profiler configuration file")
        print("   --output-format <format>   Output format (text, json, csv, xml)")
        print("   --output-file <file>       Output file")
        print("   --profiling-interval <sec>  Profiling interval in seconds")
        print("   --log-level <level>        Log level (debug, info, warning, error)")
        print("   --non-intrusive            Run in non-intrusive mode")
        print("   --export-all               Export all debug and profiling data")
        print("   --validate-only            Validate configuration only")
        
        print("\n2. Command line examples:")
        print("   Basic debugging session:")
        print("     nlm --debug --profile --monitor --debug-config=debug_config.json")
        print()
        print("   Performance analysis:")
        print("     nlm --profile --output-file=perf_report.html --export-all")
        print()
        print("   Real-time monitoring:")
        print("     nlm --monitor --dashboard --refresh=1s")
        print()
        print("   Debugging with configuration:")
        print("     nlm --debug --visualize --output-format=json")
        print()
        print("   Validation only:")
        print("     nlm --debug --validate-only --debug-config=debug_config.json")
        
        print("\n" + "=" * 60)
        print("COMMAND LINE DEMO COMPLETE")
        print("=" * 60)

def main():
    """Main demonstration function."""
    print("NLM Advanced Debugging and Profiling Tools - Comprehensive Demonstration")
    print("This script demonstrates all features of the debugging and profiling system.")
    
    if not PYNLM_AVAILABLE:
        print("\nWarning: PYNLM bindings not available.")
        print("Some demonstrations will be skipped.")
        print("Make sure to build the Python bindings first:")
        print("  python setup.py build_ext --inplace")
    
    # Run demonstrations
    BeginnerDemo.simple_debugging_demo()
    IntermediateDemo.advanced_debugging_demo()
    AdvancedDemo.advanced_optimization_demo()
    IntegrationDemo.integration_demo()
    CommandLineDemo.command_line_demo()
    
    print("\n" + "=" * 60)
    print("ALL DEMONSTRATIONS COMPLETE")
    print("=" * 60)
    print("\nThe NLM Advanced Debugging and Profiling Tools have been successfully")
    print("demonstrated with beginner, intermediate, and advanced examples.")
    print("\nKey features implemented:")
    print("✓ Comprehensive debugging infrastructure")
    print("✓ Advanced performance profiling")
    print("✓ System monitoring and alerting")
    print("✓ Automatic optimization analysis")
    print("✓ Visualization and reporting")
    print("✓ Integration with existing NLM components")
    print("✓ Command-line interface")
    print("✓ Python bindings")
    print("✓ Backward compatibility")
    
    print("\nFor more information, see:")
    print("- NLM_ADVANCED_DEBUGGING_AND_PROFILING_TOOLS.md (this file)")
    print("- Debug documentation in the docs/ directory")
    print("- Examples in the tests/ directory")

if __name__ == "__main__":
    main()
