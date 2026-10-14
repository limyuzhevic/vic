#!/usr/bin/env python3
# NLM Phase 6 Demo Launcher
# Convenient Python script to run different Phase 6 demonstrations

import subprocess
import sys
import os
import argparse

def print_banner():
    print("=" * 70)
    print("NLM Phase 6: Complete Artificial Brain - Demo Launcher")
    print("=" * 70)
    print()

def print_help():
    print("Usage: nlm_demo.py [options]")
    print()
    print("Options:")
    print("  --quick            Run quick integration test (100 steps)")
    print("  --full             Run full integration test (2000 steps)")
    print("  --demo [type]      Run demonstration: 'quick' or 'full'")
    print("  --checkpoints      Test checkpoint save/load functionality")
    print("  --replay           Test memory replay functionality")
    print("  --status           Show system status")
    print("  --build            Build the project")
    print("  --help             Show this help message")
    print()
    print("Examples:")
    print("  python nlm_demo.py --demo full")
    print("  python nlm_demo.py --quick")
    print("  python nlm_demo.py --checkpoints")
    print()

def run_command(cmd, description):
    print(f"\n{'='*70}")
    print(f"Running: {description}")
    print(f"Command: {cmd}")
    print(f"{'='*70}")
    
    try:
        result = subprocess.run(cmd, shell=True, check=True)
        print(f"✓ {description} completed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ {description} failed with error code {e.returncode}")
        return False
    except FileNotFoundError:
        print(f"✗ Command not found: {cmd}")
        return False

def build_project():
    print("Building NLM Phase 6...")
    
    # Create build directory
    if not os.path.exists("build"):
        os.makedirs("build")
    
    # Configure with CMake
    cmake_cmd = "cmake .. -DCMAKE_BUILD_TYPE=Release"
    if not run_command(cmake_cmd, "Configure CMake"):
        return False
    
    # Build
    build_cmd = "make -j$(nproc)"
    if not run_command(build_cmd, "Build NLM"):
        return False
    
    print("\nBuild complete!")
    print("Generated executables:")
    print("  build/nlm                 - Main Phase 6 integration")
    print("  build/nlm_phase3_demo     - Phase 3 demonstration")
    print("  build/nlm_phase4_demo     - Phase 4 demonstration")
    print("  build/nlm_phase6_demo     - Phase 6 demonstration")
    
    return True

def run_demo(demo_type="full"):
    if demo_type not in ["quick", "full"]:
        print(f"Error: Invalid demo type '{demo_type}'. Use 'quick' or 'full'.")
        return False
    
    steps = 100 if demo_type == "quick" else 2000
    brain_count = 500 if demo_type == "quick" else 1000
    
    cmd = f"./build/nlm --demo {demo_type}"
    return run_command(cmd, f"Run NLM Phase 6 {demo_type} demonstration ({steps} steps, {brain_count} neurons)")

def run_quick_test():
    cmd = "./build/nlm --quick"
    return run_command(cmd, "Run NLM Phase 6 quick integration test (100 steps)")

def run_full_test():
    cmd = "./build/nlm --full"
    return run_command(cmd, "Run NLM Phase 6 full integration test (2000 steps)")

def test_checkpoints():
    cmd = "./build/nlm --checkpoints"
    return run_command(cmd, "Test checkpoint save/load functionality")

def test_replay():
    cmd = "./build/nlm --replay"
    return run_command(cmd, "Test memory replay functionality")

def show_status():
    print("\nNLM Phase 6 System Status")
    print("=" * 70)
    
    # Check if build directory exists
    if not os.path.exists("build"):
        print("❌ Build directory not found. Run './build/nlm_demo.sh build' first.")
        return False
    
    # Check for executables
    executables = [
        "nlm",
        "nlm_phase3_demo", 
        "nlm_phase4_demo",
        "nlm_phase6_demo"
    ]
    
    print("\nExecutables found:")
    for exe in executables:
        exe_path = f"build/{exe}"
        if os.path.exists(exe_path):
            print(f"  ✅ {exe}")
        else:
            print(f"  ❌ {exe} (not found)")
    
    # Check if executables are executable
    print("\nExecutable permissions:")
    for exe in executables:
        exe_path = f"build/{exe}"
        if os.path.exists(exe_path):
            if os.access(exe_path, os.X_OK):
                print(f"  ✅ {exe} - executable")
            else:
                print(f"  ⚠️  {exe} - exists but not executable")
        else:
            print(f"  ❌ {exe} - not found")
    
    return True

def main():
    parser = argparse.ArgumentParser(
        description="NLM Phase 6 Demo Launcher",
        add_help=False
    )
    
    parser.add_argument(
        "--help", "-h",
        action="store_true",
        help="Show this help message"
    )
    
    parser.add_argument(
        "--demo",
        choices=["quick", "full"],
        help="Run demonstration: 'quick' or 'full'"
    )
    
    parser.add_argument(
        "--quick",
        action="store_true",
        help="Run quick integration test (100 steps)"
    )
    
    parser.add_argument(
        "--full",
        action="store_true",
        help="Run full integration test (2000 steps)"
    )
    
    parser.add_argument(
        "--checkpoints",
        action="store_true",
        help="Test checkpoint save/load functionality"
    )
    
    parser.add_argument(
        "--replay",
        action="store_true",
        help="Test memory replay functionality"
    )
    
    parser.add_argument(
        "--status",
        action="store_true",
        help="Show system status"
    )
    
    parser.add_argument(
        "--build",
        action="store_true",
        help="Build the project"
    )
    
    args = parser.parse_args()
    
    # Print banner
    print_banner()
    
    # Handle commands
    if args.help:
        print_help()
        return 0
    
    if args.build:
        return 0 if build_project() else 1
    
    if args.status:
        return 0 if show_status() else 1
    
    # Default behavior if no specific command given
    if args.demo:
        return 0 if run_demo(args.demo) else 1
    
    if args.quick:
        return 0 if run_quick_test() else 1
    
    if args.full:
        return 0 if run_full_test() else 1
    
    if args.checkpoints:
        return 0 if test_checkpoints() else 1
    
    if args.replay:
        return 0 if test_replay() else 1
    
    # If no arguments given, show help
    print("No arguments provided. Showing help...")
    print_help()
    return 0

if __name__ == "__main__":
    sys.exit(main())
