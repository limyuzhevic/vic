#!/usr/bin/env python3
"""
Memory Leak Detection Example

This example demonstrates memory leak detection and analysis for NLM neural simulations.
"""

import pynlm
from monitor import MemoryMonitor, MemoryLeakDetector

def memory_leak_detection_example():
    # Setup memory monitors
    memory_monitor = MemoryMonitor()
    leak_detector = MemoryLeakDetector()

    memory_monitor.start_tracking()
    leak_detector.start_detection()

    # Create multiple brains and simulate
    for i in range(100):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        for step in range(100):
            brain.step(step)

            # Track allocations and deallocations
            # Note: In a real implementation, you would get actual allocation data
            memory_monitor.track_allocation(1024, "step_memory")
            memory_monitor.track_deallocation(512, "step_memory")

    memory_monitor.stop_tracking()
    leak_detector.stop_detection()

    # Analyze memory usage
    total_memory = memory_monitor.get_total_memory_usage()
    allocated_memory = memory_monitor.get_allocated_memory()
    deallocated_memory = memory_monitor.get_deallocated_memory()
    current_usage = memory_monitor.get_current_memory_usage()
    peak_usage = memory_monitor.get_peak_memory_usage()

    print(f"Total memory usage: {total_memory} bytes")
    print(f"Allocated memory: {allocated_memory} bytes")
    print(f"Deallocated memory: {deallocated_memory} bytes")
    print(f"Current memory usage: {current_usage} bytes")
    print(f"Peak memory usage: {peak_usage} bytes")

    # Check for memory leaks
    memory_monitor.set_verbose_logging(True)
    leaks = memory_monitor.detect_memory_leaks()

    print(f"\nMemory leak detection results:")
    if leaks.empty():
        print("✅ No memory leaks detected")
    else:
        print(f"❌ {leaks.size()} potential memory leaks detected")
        for i, leak in enumerate(leaks.head(10)):
            print(f"  Leak {i+1}: {leak.size} bytes at {leak.source_file}:{leak.source_line}")

    # Get detailed leak report
    leak_report = leak_detector.generate_leak_report()
    print(f"\nDetailed leak report:")
    print(leak_report)

    # Export leak data
    leak_detector.export_leak_data("leak_data.json", "json")

    print("\nLeak data exported successfully")

if __name__ == "__main__":
    memory_leak_detection_example()
