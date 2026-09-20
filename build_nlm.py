#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine
Comprehensive build and deployment script

This script provides:
1. CMake configuration and building
2. Testing automation
3. Performance benchmarking
4. Documentation generation
5. Clean deployment scripts
"""

import os
import sys
import subprocess
import json
import argparse
import time
from pathlib import Path
from datetime import datetime

def run_command(cmd, cwd=None, env=None):
    """Run a command and return output"""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(
            cmd, shell=True, cwd=cwd, env=env,
            capture_output=True, text=True, check=True
        )
        return result.stdout, result.stderr, result.returncode
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {cmd}")
        print(f"Error: {e.stderr}")
        return e.stdout, e.stderr, e.returncode

def main():
    parser = argparse.ArgumentParser(description="NLM build and deployment script")
    parser.add_argument("--build-type", default="Release", 
                       choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
                       help="Build type")
    parser.add_argument("--generator", default="Unix Makefiles",
                       help="CMake generator")
    parser.add_argument("--parallel", type=int, default=4,
                       help="Number of parallel build jobs")
    parser.add_argument("--clean", action="store_true",
                       help="Clean build directories")
    parser.add_argument("--test", action="store_true",
                       help="Run tests after building")
    parser.add_argument("--benchmark", action="store_true",
                       help="Run performance benchmarks")
    parser.add_argument("--install", action="store_true",
                       help="Install built artifacts")
    parser.add_argument("--help", action="store_true",
                       help="Show help message")
    
    args = parser.parse_args()
    
    # Project root directory
    project_root = Path(__file__).parent
    build_dir = project_root / "build"
    install_dir = project_root / "install"
    
    # Show help if requested
    if args.help:
        parser.print_help()
        return
    
    # Clean build if requested
    if args.clean and build_dir.exists():
        print("Cleaning build directory...")
        import shutil
        shutil.rmtree(build_dir)
    
    # Create build directory
    build_dir.mkdir(exist_ok=True)
    
    # Configure CMake
    cmake_cmd = f"cmake -G \"{args.generator}\" -DCMAKE_BUILD_TYPE={args.build_type} \
        -DCMAKE_INSTALL_PREFIX={install_dir} \
        -DBUILD_TESTS=ON \
        -DBUILD_BENCHMARKS=ON \
        {project_root}"
    
    stdout, stderr, returncode = run_command(cmake_cmd, cwd=build_dir)
    if returncode != 0:
        print("CMake configuration failed!")
        return 1
    
    # Build the project
    build_cmd = f"cmake --build . --config {args.build_type} --parallel {args.parallel}"
    
    stdout, stderr, returncode = run_command(build_cmd, cwd=build_dir)
    if returncode != 0:
        print("Build failed!")
        return 1
    
    print("Build successful!")
    
    # Run tests if requested
    if args.test:
        test_cmd = f"ctest --output-on-failure --parallel {args.parallel}"
        stdout, stderr, returncode = run_command(test_cmd, cwd=build_dir)
        if returncode != 0:
            print("Tests failed!")
            return 1
        print("All tests passed!")
    
    # Run benchmarks if requested
    if args.benchmark:
        benchmark_cmd = f".build/benchmark/nlm_benchmark"
        stdout, stderr, returncode = run_command(benchmark_cmd, cwd=build_dir)
        if returncode != 0:
            print("Benchmarks failed!")
            return 1
        print("Benchmarks completed!")
        
        # Save benchmark results
        results_file = project_root / "benchmark_results.json"
        with open(results_file, 'w') as f:
            json.dump({
                "timestamp": datetime.now().isoformat(),
                "build_type": args.build_type,
                "benchmark_results": stdout
            }, f, indent=2)
    
    # Install if requested
    if args.install:
        install_cmd = f"cmake --install . --prefix {install_dir}"
        stdout, stderr, returncode = run_command(install_cmd, cwd=build_dir)
        if returncode != 0:
            print("Installation failed!")
            return 1
        print("Installation successful!")
        
        # Create installation summary
        summary_file = install_dir / "installation_summary.txt"
        with open(summary_file, 'w') as f:
            f.write(f"NLM Installation Summary\n")
            f.write(f"===========================\n")
            f.write(f"Timestamp: {datetime.now().isoformat()}\n")
            f.write(f"Build type: {args.build_type}\n")
            f.write(f"Generator: {args.generator}\n")
            f.write(f"Parallel jobs: {args.parallel}\n")
            f.write(f"\n")
            f.write(f"Installed binaries:\n")
            
            bin_dir = install_dir / "bin"
            if bin_dir.exists():
                for exe in bin_dir.glob("*"):
                    if exe.is_file() and os.access(exe, os.X_OK):
                        f.write(f"  {exe.name}\n")
            
            lib_dir = install_dir / "lib"
            if lib_dir.exists():
                f.write(f"\nStatic libraries:\n")
                for lib in lib_dir.glob("*.a"):
                    f.write(f"  {lib.name}\n")
                f.write(f"\nShared libraries:\n")
                for lib in lib_dir.glob("*.so"):
                    f.write(f"  {lib.name}\n")
    
    print("\nBuild and deployment completed successfully!")
    
    # Show available executables
    print("\nAvailable executables:")
    exe_dir = build_dir / "bin"
    if exe_dir.exists():
        for exe in exe_dir.glob("*"):
            if exe.is_file() and os.access(exe, os.X_OK):
                print(f"  {exe.name}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
