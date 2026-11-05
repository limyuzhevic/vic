#!/usr/bin/env python3
"""
NLM Advanced Demo Script
Demonstrates the new improved NLM test suite and features

This script shows how to use the enhanced NLM test suite with:
- Multiple test modes
- Configuration file support
- Performance monitoring
- Advanced learning scenarios
"""

import subprocess
import sys
import os
import time
from pathlib import Path

def print_header(title):
    """Print a formatted header"""
    print("\n" + "=" * 60)
    print(f"{title.center(60)}")
    print("=" * 60)

def run_command(cmd, description):
    """Run a command and report results"""
    print(f"\n🚀 {description}")
    print(f"Command: {' '.join(cmd)}")
    
    start_time = time.time()
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, cwd="/workspace/app")
        end_time = time.time()
        execution_time = end_time - start_time
        
        if result.returncode == 0:
            print(f"✅ Success! (took {execution_time:.2f}s)")
            print(f"Output (first 500 chars): {result.stdout[:500]}...")
        else:
            print(f"❌ Failed! (took {execution_time:.2f}s)")
            print(f"Error: {result.stderr}")
        
        return result.returncode == 0
    except Exception as e:
        print(f"❌ Exception: {e}")
        return False

def main():
    print_header("NLM Advanced Demo - Improved Test Suite")
    
    # Check if the build directory exists
    if not Path("/workspace/app/build").exists():
        print("❌ Build directory not found. Please run: mkdir build && cd build && cmake ..")
        return 1
    
    # List available executables
    build_dir = Path("/workspace/app/build")
    executables = [f for f in build_dir.iterdir() if f.is_file() and os.access(f, os.X_OK)]
    
    print(f"\n📂 Found {len(executables)} executable(s) in build directory:")
    for exe in sorted(executables):
        print(f"   • {exe.name}")
    
    print_header("Running Demonstrations")
    
    # Demo 1: Run the improved test suite
    success1 = run_command(
        ["/workspace/app/build/nlm", "--help"],
        "NLM Test Suite - Help Menu"
    )
    
    # Demo 2: Show available config files
    config_dir = Path("/workspace/app/configs")
    if config_dir.exists():
        config_files = list(config_dir.glob("*.cfg"))
        print(f"\n📋 Available config files ({len(config_files)}):")
        for config in config_files:
            print(f"   • {config.name}")
            # Show first few lines of config
            try:
                with open(config, 'r') as f:
                    lines = f.readlines()[:5]
                    for line in lines:
                        print(f"     {line.rstrip()}")
                print("     ...")
            except:
                print("     [Could not read config]")
    
    # Demo 3: Run basic connectivity test (if binary exists)
    basic_test = build_dir / "nlm"
    if basic_test.exists():
        success3 = run_command(
            [str(basic_test)],
            "NLM Basic Test Suite"
        )
    else:
        print("\n⏭️  Skipping basic test (nlm executable not found)")
        success3 = True
    
    # Demo 4: Show test runner structure
    print("\n📊 Test Runner Features:")
    print("   • Modular test design with reusable components")
    print("   • Configurable test parameters via config files")
    print("   • Comprehensive logging and error reporting")
    print("   • Performance monitoring and timing")
    print("   • Multiple test modes (basic, plasticity, STDP, advanced)")
    
    # Demo 5: Show LearningExperiment improvements
    print("\n🧠 LearningExperiment Improvements:")
    print("   • Extracted from main.cpp to reusable header file")
    print("   • Proper encapsulation of learning metrics")
    print("   • Improved state management")
    print("   • Better statistics and analysis")
    
    print_header("Demo Summary")
    all_success = success1 and success3
    
    if all_success:
        print("✅ All demos completed successfully!")
        print("\n🎯 Key Improvements:")
        print("   1. Enhanced test suite with modular design")
        print("   2. Comprehensive configuration system")
        print("   3. Better logging and error handling")
        print("   4. Performance monitoring capabilities")
        print("   5. Advanced learning experiment support")
        print("   6. Improved user interface and help system")
        print("   7. Multiple test modes for different scenarios")
        print("\n🚀 The NLM system is now more robust, maintainable, and feature-rich!")
    else:
        print("⚠️  Some demos had issues. Check the output above for details.")
    
    return 0 if all_success else 1

if __name__ == "__main__":
    sys.exit(main())