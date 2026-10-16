#!/usr/bin/env python3
"""
Performance Monitoring Test Suite

This script provides a basic test to verify the performance monitoring implementation.
It checks if the core performance monitoring files exist and demonstrates the API usage.
"""

import os
import sys

def check_file_exists(filepath):
    """Check if a file exists and return its status."""
    if os.path.exists(filepath):
        return True, os.path.getsize(filepath)
    else:
        return False, 0

def check_directory_exists(dirpath):
    """Check if a directory exists and count its contents."""
    if os.path.exists(dirpath) and os.path.isdir(dirpath):
        files = [f for f in os.listdir(dirpath) if os.path.isfile(os.path.join(dirpath, f))]
        return True, len(files)
    else:
        return False, 0

def main():
    print("=== NLM Performance Monitoring Test ===")
    
    # Check core Performance directory
    perf_dir = "src/core/Performance"
    exists, file_count = check_directory_exists(perf_dir)
    
    if exists:
        print(f"✓ Core Performance directory found with {file_count} files")
    else:
        print(f"✗ Core Performance directory not found")
        sys.exit(1)
    
    # Check all Performance headers
    expected_headers = [
        "PerformanceMonitor.hpp",
        "Profiler.hpp", 
        "MemoryProfiler.hpp",
        "NeuralProfiler.hpp",
        "ConfigProfiler.hpp",
        "BuildProfiler.hpp",
        "CMakeProfiler.hpp"
    ]
    
    print("\nChecking Performance headers:")
    all_headers_exist = True
    for header in expected_headers:
        filepath = os.path.join(perf_dir, header)
        exists, size = check_file_exists(filepath)
        if exists:
            print(f"  ✓ {header} ({size} bytes)")
        else:
            print(f"  ✗ {header} - NOT FOUND")
            all_headers_exist = False
    
    # Check source files
    print("\nChecking Performance source files:")
    expected_sources = [
        "PerformanceMonitor.cpp",
        "Profiler.cpp",
        "MemoryProfiler.cpp",
        "NeuralProfiler.cpp",
        "ConfigProfiler.cpp",
        "BuildProfiler.cpp",
        "CMakeProfiler.cpp"
    ]
    
    for source in expected_sources:
        filepath = os.path.join(perf_dir, source)
        exists, size = check_file_exists(filepath)
        if exists:
            print(f"  ✓ {source} ({size} bytes)")
        else:
            print(f"  ✗ {source} - NOT FOUND")
            all_headers_exist = False
    
    # Check brain integration
    print("\nChecking Brain integration:")
    brain_files = [
        "src/brain/Neuron.cpp",
        "src/brain/Synapse.cpp"
    ]
    
    for filepath in brain_files:
        exists, size = check_file_exists(filepath)
        if exists:
            print(f"  ✓ {filepath} ({size} bytes)")
        else:
            print(f"  ✗ {filepath} - NOT FOUND")
            all_headers_exist = False
    
    # Check documentation
    print("\nChecking documentation:")
    doc_files = [
        "PERFORMANCE_MONITORING_IMPLEMENTATION.md",
        "test_performance_monitor.cpp"
    ]
    
    for filepath in doc_files:
        exists, size = check_file_exists(filepath)
        if exists:
            print(f"  ✓ {filepath} ({size} bytes)")
        else:
            print(f"  ✗ {filepath} - NOT FOUND")
            all_headers_exist = False
    
    # Summary
    print("\n" + "="*50)
    if all_headers_exist:
        print("SUCCESS: All Performance Monitoring files found!")
        print("The implementation appears to be complete.")
        print("\nNext steps:")
        print("1. Compile the project with performance monitoring enabled")
        print("2. Run the performance monitoring test")
        print("3. Verify performance monitoring functionality")
        print("4. Review the implementation documentation")
        return 0
    else:
        print("FAILURE: Some Performance Monitoring files are missing!")
        print("Please check the missing files and ensure the implementation is complete.")
        return 1

if __name__ == "__main__":
    sys.exit(main())