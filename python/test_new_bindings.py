#!/usr/bin/env python3
"""Test script for new performance monitoring, debugging, and analysis Python bindings"""

import sys
import os

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import the module
try:
    from pynlm import (
        # Performance monitoring
        TimingUtils, BenchmarkResult, MemoryTracker, Profiler, PerformanceMonitor,
        # Debugging
        DebugLogger, StateInspector, MemoryDumper, DebugHelper,
        # Analysis
        NeuralAnalyzer, ConnectivityAnalyzer, PerformanceAnalyzer, PatternAnalyzer,
        # Factory functions
        createPerformanceMonitor, createProfiler, createDebugLogger,
        createStateInspector, createMemoryDumper, createNeuralAnalyzer,
        createConnectivityAnalyzer, createPerformanceAnalyzer, createPatternAnalyzer,
        # Convenience functions
        enableDebugMode, disableDebugMode, setLogLevel, logDebug, logInfo, logWarning, logError
    )
    print("Successfully imported all new Python bindings!")
except ImportError as e:
    print(f"Failed to import new Python bindings: {e}")
    sys.exit(1)

# Test 1: Create and test PerformanceMonitor
print("\nTest 1: Creating PerformanceMonitor")
try:
    perf_monitor = createPerformanceMonitor()
    perf_monitor.start("Test Benchmark")
    import time
    time.sleep(0.01)  # Small delay to measure
    result = perf_monitor.stop()
    print(f"✓ PerformanceMonitor created successfully")
    print(f"  Benchmark: {result.name}")
    print(f"  Total time: {result.totalTime:.4f}s")
except Exception as e:
    print(f"✗ Failed PerformanceMonitor test: {e}")
    sys.exit(1)

# Test 2: Create and test Profiler
print("\nTest 2: Creating Profiler")
try:
    profiler = createProfiler()
    profiler.startProfile("test_function")
    time.sleep(0.01)
    profile_data = profiler.stopProfile("test_function")
    print(f"✓ Profiler created successfully")
    print(f"  Function: {profile_data.functionName}")
    print(f"  Total time: {profile_data.totalTime:.4f}s")
    print(f"  Call count: {profile_data.callCount}")
    
    # Generate report
    report = profiler.generateReport()
    print(f"  Report generated (length: {len(report)} chars)")
except Exception as e:
    print(f"✗ Failed Profiler test: {e}")
    sys.exit(1)

# Test 3: Create and test DebugLogger
print("\nTest 3: Creating DebugLogger")
try:
    debug_logger = createDebugLogger()
    debug_logger.setLevel(DebugLogger.Level.Info)
    debug_logger.info("This is an info message")
    debug_logger.warning("This is a warning message")
    debug_logger.error("This is an error message")
    print(f"✓ DebugLogger created successfully")
    print(f"  Level set to Info")
    
    log_entries = debug_logger.getLogEntries()
    print(f"  Log entries captured: {len(log_entries)}")
    
    # Generate report
    log_report = debug_logger.generateLogReport()
    print(f"  Log report generated (length: {len(log_report)} chars)")
except Exception as e:
    print(f"✗ Failed DebugLogger test: {e}")
    sys.exit(1)

# Test 4: Create and test StateInspector
print("\nTest 4: Creating StateInspector")
try:
    state_inspector = createStateInspector()
    state = state_inspector.inspectState(
        "adult", 1000, 800, 750, 0.85, 1.2, 5000, 10000, 100.5, 95.2, 10
    )
    print(f"✓ StateInspector created successfully")
    print(f"  Session ID: {state.sessionId}")
    print(f"  Timestamp: {state.timestamp}")
    print(f"  Neurons: {state.totalNeurons}")
    print(f"  Active: {state.activeNeurons}")
    print(f"  Firing rate: {state.averageFiringRate:.2f}")
except Exception as e:
    print(f"✗ Failed StateInspector test: {e}")
    sys.exit(1)

# Test 5: Create and test MemoryDumper
print("\nTest 5: Creating MemoryDumper")
try:
    memory_dumper = createMemoryDumper()
    memory_regions = memory_dumper.dumpMemoryRegions()
    print(f"✓ MemoryDumper created successfully")
    print(f"  Memory regions dumped: {len(memory_regions)}")
    for i, region in enumerate(memory_regions[:3]):  # Show first 3
        print(f"    Region {i+1}: {region.format()}")
    
    # Generate report
    mem_report = memory_dumper.generateMemoryReport(memory_regions)
    print(f"  Memory report generated (length: {len(mem_report)} chars)")
except Exception as e:
    print(f"✗ Failed MemoryDumper test: {e}")
    sys.exit(1)

# Test 6: Create and test DebugHelper
print("\nTest 6: Creating DebugHelper")
try:
    debug_helper = DebugHelper()
    call_stack = debug_helper.getCallStack()
    print(f"✓ DebugHelper created successfully")
    print(f"  Call stack size: {len(call_stack)}")
    
    # Test formatting functions
    mem_size = debug_helper.formatMemorySize(1024 * 1024 * 5)  # 5MB
    time_format = debug_helper.formatTime(125.5)  # 125.5 seconds
    print(f"  Memory formatting: {mem_size}")
    print(f"  Time formatting: {time_format}")
    
    # Test issue detection
    issues = debug_helper.checkForIssues("memory performance")
    print(f"  Issues detected: {len(issues)}")
    for issue in issues:
        print(f"    - {issue}")
except Exception as e:
    print(f"✗ Failed DebugHelper test: {e}")
    sys.exit(1)

# Test 7: Create and test NeuralAnalyzer
print("\nTest 7: Creating NeuralAnalyzer")
try:
    neural_analyzer = createNeuralAnalyzer()
    # Create sample spike trains
    spike_trains = [
        [0.1, 0.3, 0.6, 1.0, 1.5],  # Regular spikes
        [0.2, 0.5, 1.0, 1.8, 2.7],  # Irregular spikes
        [0.1, 0.4, 0.7, 1.2, 1.8]   # Medium irregular
    ]
    
    activities = []
    for train in spike_trains:
        activity = neural_analyzer.analyzeNeuronActivity(train)
        activities.append(activity)
    
    network_stats = neural_analyzer.analyzeNetwork(activities)
    print(f"✓ NeuralAnalyzer created successfully")
    print(f"  Neurons analyzed: {network_stats.totalNeurons}")
    print(f"  Active neurons: {network_stats.activeNeurons}")
    print(f"  Firing rate: {network_stats.averageFiringRate:.4f}")
    
    # Generate report
    neural_report = neural_analyzer.generateNeuralReport(network_stats)
    print(f"  Neural report generated (length: {len(neural_report)} chars)")
except Exception as e:
    print(f"✗ Failed NeuralAnalyzer test: {e}")
    sys.exit(1)

# Test 8: Create and test ConnectivityAnalyzer
print("\nTest 8: Creating ConnectivityAnalyzer")
try:
    connectivity_analyzer = createConnectivityAnalyzer()
    // Create sample adjacency matrix (3 neurons)
    adjacency_matrix = [
        [0.0, 0.8, 0.2],
        [0.3, 0.0, 0.6],
        [0.1, 0.4, 0.0]
    ]
    
    connectivity = connectivity_analyzer.analyzeConnectivity(adjacency_matrix)
    print(f"✓ ConnectivityAnalyzer created successfully")
    print(f"  Connections: {connectivity.totalConnections}")
    print(f"  Average weight: {connectivity.averageWeight:.4f}")
    print(f"  Connection density: {connectivity.connectionDensity:.4f}")
    
    # Test shortest path
    path = connectivity_analyzer.findShortestPath(adjacency_matrix, 0, 2)
    print(f"  Shortest path from 0 to 2: {path}")
    
    # Generate report
    conn_report = connectivity_analyzer.generateConnectivityReport(connectivity)
    print(f"  Connectivity report generated (length: {len(conn_report)} chars)")
except Exception as e:
    print(f"✗ Failed ConnectivityAnalyzer test: {e}")
    sys.exit(1)

# Test 9: Create and test PerformanceAnalyzer
print("\nTest 9: Creating PerformanceAnalyzer")
try:
    performance_analyzer = createPerformanceAnalyzer()
    metrics = performance_analyzer.analyzePerformance(
        cpuUsage=75.5,
        memoryUsageMB=512.3,
        networkIO=125.8,
        simulationSpeed=1500.0,
        activeNeurons=850,
        totalNeurons=1000,
        averageFiringRate=0.85,
        simulationTime=60.0
    )
    print(f"✓ PerformanceAnalyzer created successfully")
    print(f"  CPU Usage: {metrics.cpuUsage}%")
    print(f"  Memory Usage: {metrics.memoryUsageMB} MB")
    print(f"  Simulation Speed: {metrics.simulationSpeed} steps/s")
    print(f"  Efficiency: {metrics.efficiency:.2f}%")
    
    # Test performance comparison
    more_metrics = [
        metrics,
        performance_analyzer.analyzePerformance(
            80.0, 600.0, 100.0, 1200.0, 900, 1000, 0.9, 30.0
        )
    ]
    
    comparisons = performance_analyzer.comparePerformance(more_metrics)
    print(f"  Performance comparisons: {len(comparisons)}")
except Exception as e:
    print(f"✗ Failed PerformanceAnalyzer test: {e}")
    sys.exit(1)

# Test 10: Create and test PatternAnalyzer
print("\nTest 10: Creating PatternAnalyzer")
try:
    pattern_analyzer = createPatternAnalyzer()
    // Create sample time series
    time_series = [0.1, 0.3, 0.6, 1.0, 1.5, 1.8, 2.2, 2.5, 2.8, 3.2]
    
    patterns = pattern_analyzer.detectPatterns(time_series)
    print(f"✓ PatternAnalyzer created successfully")
    print(f"  Patterns detected: {len(patterns)}")
    for i, pattern in enumerate(patterns):
        print(f"    Pattern {i+1}: {pattern.patternType} (confidence: {pattern.confidence:.4f})")
    
    // Test neural pattern analysis
    spike_trains = [[0.1, 0.5, 1.0, 1.5, 2.0], [0.2, 0.7, 1.3, 1.8]]
    neural_patterns = pattern_analyzer.analyzeNeuralPatterns(spike_trains)
    print(f"  Neural patterns detected: {len(neural_patterns)}")
    
    // Generate report
    pattern_report = pattern_analyzer.generatePatternReport(patterns)
    print(f"  Pattern report generated (length: {len(pattern_report)} chars)")
except Exception as e:
    print(f"✗ Failed PatternAnalyzer test: {e}")
    sys.exit(1)

# Test 11: Test convenience functions
print("\nTest 11: Testing convenience functions")
try:
    enableDebugMode()
    print(f"✓ Debug mode enabled")
    
    logInfo("This is an info message from global logger")
    logWarning("This is a warning message from global logger")
    logError("This is an error message from global logger")
    print(f"✓ Convenience logging functions work")
    
    disableDebugMode()
    print(f"✓ Debug mode disabled")
except Exception as e:
    print(f"✗ Failed convenience functions test: {e}")
    sys.exit(1)

print("\n" + "="*60)
print("All new Python binding tests passed successfully!")
print("="*60)
print("\nSummary of new functionality:")
print("- Performance monitoring: TimingUtils, BenchmarkResult, MemoryTracker, Profiler, PerformanceMonitor")
print("- Debugging: DebugLogger, StateInspector, MemoryDumper, DebugHelper")
print("- Analysis: NeuralAnalyzer, ConnectivityAnalyzer, PerformanceAnalyzer, PatternAnalyzer")
print("- Enhanced debugging features: Global logger, log levels, session tracking")
print("\nThese tools provide comprehensive capabilities for:")
print("  • Performance monitoring and benchmarking")
print("  • Memory usage tracking and analysis")
print("  • Brain state inspection and debugging")
print("  • Neural activity pattern analysis")
print("  • Network connectivity analysis")
print("  • Performance comparison and optimization")
print("  • Comprehensive logging and issue detection")
