#!/usr/bin/env python3
"""
NLM (熙然) Build Script

This script provides commands to build, test, and run the NLM project.
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def run_command(cmd, cwd=None, check=True):
    """Run a command and print output"""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(
            cmd, shell=True, cwd=cwd, check=check,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True
        )
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(f"STDERR: {result.stderr}")
        return result.returncode == 0
    except subprocess.CalledProcessError as e:
        print(f"Command failed with exit code {e.returncode}")
        print(f"STDOUT: {e.stdout}")
        print(f"STDERR: {e.stderr}")
        return False

def main():
    parser = argparse.ArgumentParser(description="NLM Build Script")
    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Build command
    build_parser = subparsers.add_parser("build", help="Build the project")
    build_parser.add_argument("--config", choices=["debug", "release"], default="release",
                             help="Build configuration")
    build_parser.add_argument("--generator", help="CMake generator (e.g., 'Ninja')")
    build_parser.add_argument("--build-dir", default="build", help="Build directory")

    # Test command
    test_parser = subparsers.add_parser("test", help="Run tests")
    test_parser.add_argument("--build-dir", default="build", help="Build directory")
    test_parser.add_argument("--verbose", action="store_true", help="Verbose output")

    # Run command
    run_parser = subparsers.add_parser("run", help="Run the main executable")
    run_parser.add_argument("--build-dir", default="build", help="Build directory")
    run_parser.add_argument("--config", help="Path to configuration file")

    # Clean command
    clean_parser = subparsers.add_parser("clean", help="Clean build artifacts")
    clean_parser.add_argument("--build-dir", default="build", help="Build directory")

    # All-in-one command
    install_parser = subparsers.add_parser("install", help="Build, test, and run")
    install_parser.add_argument("--config", choices=["debug", "release"], default="release",
                               help="Build configuration")
    install_parser.add_argument("--generator", help="CMake generator (e.g., 'Ninja')")
    install_parser.add_argument("--build-dir", default="build", help="Build directory")
    install_parser.add_argument("--config-file", help="Path to configuration file")

    # Python bindings command
    bindings_parser = subparsers.add_parser("bindings", help="Build Python bindings")
    bindings_parser.add_argument("--build-dir", default="build", help="Build directory")
    bindings_parser.add_argument("--module-name", default="pynlm", help="Module name")

    # Phase commands
    phase_parser = subparsers.add_parser("phase", help="Run specific phase tests")
    phase_parser.add_argument("phase", choices=["1", "2", "3", "4", "5", "6"],
                             help="Phase number to run")
    phase_parser.add_argument("--build-dir", default="build", help="Build directory")
    phase_parser.add_argument("--config", help="Path to configuration file")

    args = parser.parse_args()

    if not args.command:
        parser.print_help()
        return 1

    # Get repository root
    repo_root = Path(__file__).parent

    if args.command == "build":
        return build_project(args, repo_root)
    elif args.command == "test":
        return run_tests(args, repo_root)
    elif args.command == "run":
        return run_main(args, repo_root)
    elif args.command == "clean":
        return clean_build(args, repo_root)
    elif args.command == "install":
        return install_all(args, repo_root)
    elif args.command == "bindings":
        return build_bindings(args, repo_root)
    elif args.command == "phase":
        return run_phase(args, repo_root)

    return 0

def build_project(args, repo_root):
    """Build the project"""
    build_dir = repo_root / args.build_dir

    # Create build directory
    build_dir.mkdir(exist_ok=True)

    # Configure CMake
    cmake_cmd = [
        "cmake", "..",
        f"-DCMAKE_BUILD_TYPE={args.config.upper()}",
        "-DCMAKE_CXX_STANDARD=20",
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
    ]

    if args.generator:
        cmake_cmd.extend(["-G", args.generator])

    if not run_command(" ".join(cmake_cmd), cwd=build_dir):
        print("CMake configuration failed")
        return 1

    # Build
    if not run_command(f"make -j4", cwd=build_dir):
        print("Build failed")
        return 1

    print("Build completed successfully")
    return 0

def run_tests(args, repo_root):
    """Run tests"""
    build_dir = repo_root / args.build_dir

    # Check if tests target exists
    if not run_command("make help", cwd=build_dir):
        print("Available targets:")
        run_command("make help", cwd=build_dir, check=False)

    # Run all tests
    if not run_command("ctest --output-on-failure", cwd=build_dir):
        print("Tests failed")
        return 1

    print("Tests completed successfully")
    return 0

def run_main(args, repo_root):
    """Run the main executable"""
    build_dir = repo_root / args.build_dir

    # Find the executable
    exe_path = build_dir / "nlm"
    if not exe_path.exists():
        print(f"Executable not found: {exe_path}")
        print("Try building the project first")
        return 1

    # Run with config file if provided
    cmd = str(exe_path)
    if args.config:
        cmd += f" --config {args.config}"

    print(f"Running: {cmd}")
    return subprocess.call(cmd, shell=True)

def clean_build(args, repo_root):
    """Clean build artifacts"""
    build_dir = repo_root / args.build_dir

    if build_dir.exists():
        print(f"Removing {build_dir}")
        import shutil
        shutil.rmtree(build_dir)

    print("Clean completed")
    return 0

def install_all(args, repo_root):
    """Build, test, and run"""
    if build_project(args, repo_root) != 0:
        return 1

    if run_tests(args, repo_root) != 0:
        return 1

    return run_main(args, repo_root)

def build_bindings(args, repo_root):
    """Build Python bindings"""
    build_dir = repo_root / args.build_dir

    # Build the main project first
    if build_project(args, repo_root) != 0:
        return 1

    # Find Python bindings build directory
    pybind_dir = build_dir / "python"
    if not pybind_dir.exists():
        print("Python bindings directory not found")
        return 1

    # Build Python module
    if not run_command(f"pip install -e .", cwd=repo_root):
        print("Failed to install Python bindings")
        return 1

    print("Python bindings built successfully")
    return 0

def run_phase(args, repo_root):
    """Run a specific phase test"""
    build_dir = repo_root / args.build_dir

    # Find the phase executable
    phase_exe = build_dir / f"nlm_phase{args.phase}_demo"
    if not phase_exe.exists():
        print(f"Phase {args.phase} executable not found: {phase_exe}")
        print("Try building the project first")
        return 1

    # Run with config file if provided
    cmd = str(phase_exe)
    if args.config:
        cmd += f" --config {args.config}"

    print(f"Running: {cmd}")
    return subprocess.call(cmd, shell=True)

if __name__ == "__main__":
    sys.exit(main())
