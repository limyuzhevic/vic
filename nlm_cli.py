#!/usr/bin/env python3
"""
NLM Command Line Interface

This script provides a comprehensive command-line interface for working with the
Neural Learning Machine (NLM) project. It supports:

1. Building and compiling the project
2. Running different phase demonstrations
3. Managing Python bindings and dependencies
4. Running tests and benchmarks
5. Configuration management
6. Visualization and analysis

Usage:
    nlm_cli.py <command> [options]

Commands:
    build          Build the C++ project
    build-python    Build Python bindings
    test           Run tests
    demo           Run demonstrations
    benchmark      Run benchmarks
    config         Configuration management
    help           Show help information
"""

import argparse
import subprocess
import sys
import os
import json
import time
from pathlib import Path
from typing import Optional, List

def run_command(cmd: List[str], cwd: Optional[str] = None) -> bool:
    """Run a command and return success status."""
    try:
        print(f"Running: {' '.join(cmd)}")
        result = subprocess.run(cmd, cwd=cwd, check=True, capture_output=True, text=True)
        print("Success:", result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
        print(f"Stdout: {e.stdout}")
        print(f"Stderr: {e.stderr}")
        return False

def build_cpp():
    """Build the C++ project."""
    print("Building NLM C++ project...")
    
    # Create build directory
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    
    # Configure with CMake
    if not run_command(["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"], cwd=str(build_dir)):
        return False
    
    # Build
    if not run_command(["make", "-j4"], cwd=str(build_dir)):
        return False
    
    print("C++ project built successfully!")
    print(f"Binaries available in: {build_dir.absolute()}")
    
    # List available binaries
    bin_dir = build_dir / "bin"
    if bin_dir.exists():
        print("\nAvailable binaries:")
        for exe in bin_dir.glob("*"):
            if exe.is_file() and os.access(exe, os.X_OK):
                print(f"  - {exe.name}")
    
    return True

def build_python():
    """Build Python bindings."""
    print("Building Python bindings...")
    
    # Check if we have pip and setuptools
    if not run_command([sys.executable, "-m", "pip", "install", "-U", "pip", "setuptools"]):
        return False
    
    # Install build dependencies
    if not run_command([sys.executable, "-m", "pip", "install", "pybind11", "scikit-build-core", "numpy"]):
        return False
    
    # Build and install
    if not run_command([sys.executable, "-m", "pip", "install", "."]):
        return False
    
    print("Python bindings built successfully!")
    
    # Test import
    try:
        import pynlm
        print(f"Python module version: {pynlm.__version__}")
        print("Python bindings are working correctly!")
    except ImportError as e:
        print(f"Failed to import pynlm: {e}")
        return False
    
    return True

def run_tests():
    """Run NLM tests."""
    print("Running NLM tests...")
    
    build_dir = Path("build")
    if not build_dir.exists():
        print("Build directory not found. Building first...")
        if not build_cpp():
            return False
    
    # Run C++ tests
    if not run_command(["ctest", "--output-on-failure"], cwd=str(build_dir)):
        return False
    
    # Run Python tests if available
    test_script = Path("python_test.py")
    if test_script.exists():
        if not run_command([sys.executable, str(test_script)]):
            return False
    
    print("All tests passed!")
    return True

def run_demo(phase: str, demo_name: Optional[str] = None):
    """Run a demonstration."""
    print(f"Running {phase} demo...")
    
    build_dir = Path("build")
    if not build_dir.exists():
        print("Build directory not found. Building first...")
        if not build_cpp():
            return False
    
    # Map phase names to executables
    demo_executables = {
        "phase2": "nlm",
        "phase3": "nlm_phase3_demo", 
        "phase4": "nlm_phase4_demo",
        "phase5": "nlm_phase5_demo",
        "phase6": "nlm_phase6_demo",
    }
    
    executable_name = demo_executables.get(phase.lower())
    if not executable_name:
        print(f"Unknown phase: {phase}")
        print(f"Available phases: {', '.join(demo_executables.keys())}")
        return False
    
    # Check if executable exists
    executable_path = build_dir / executable_name
    if not executable_path.exists():
        print(f"Executable not found: {executable_path}")
        print(f"Available files in build directory:")
        for f in build_dir.iterdir():
            if f.is_file() and os.access(f, os.X_OK):
                print(f"  - {f.name}")
        return False
    
    # Run demo with appropriate arguments
    cmd = [str(executable_path)]
    
    print(f"Executing: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    print(result.stdout)
    if result.stderr:
        print("Errors:", result.stderr)
    
    return result.returncode == 0

def run_benchmark(benchmark_type: str = "default"):
    """Run benchmarks."""
    print(f"Running {benchmark_type} benchmarks...")
    
    build_dir = Path("build")
    if not build_dir.exists():
        print("Build directory not found. Building first...")
        if not build_cpp():
            return False
    
    # Find benchmark executable
    benchmark_exec = build_dir / "benchmark"
    if not benchmark_exec.exists():
        print("Benchmark executable not found. Creating a simple benchmark...")
        return create_benchmark_script()
    
    # Run benchmark
    if not run_command([str(benchmark_exec), "--type", benchmark_type]):
        return False
    
    return True

def create_benchmark_script():
    """Create a simple benchmark script."""
    print("Creating benchmark script...")
    
    benchmark_script = Path("benchmark_script.py")
    
    script_content = '''#!/usr/bin/env python3
"""
Simple NLM benchmark script.
"""

import pynlm
import time
import statistics
from typing import List

def benchmark_brain_creation(num_runs: int = 100) -> float:
    """Benchmark brain creation time."""
    times = []
    
    for _ in range(num_runs):
        start = time.time()
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        end = time.time()
        
        times.append(end - start)
    
    return statistics.mean(times)

def benchmark_step_processing(brain, steps: int = 1000) -> float:
    """Benchmark step processing time."""
    start = time.time()
    
    for i in range(steps):
        brain.step(i, i * 0.001)
    
    end = time.time()
    return (end - start) / steps

def benchmark_memory_usage(brain, steps: int = 1000) -> float:
    """Benchmark memory usage change."""
    import tracemalloc
    
    tracemalloc.start()
    
    for i in range(steps):
        brain.step(i, i * 0.001)
    
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    return peak / (1024 * 1024)  # MB

def main():
    print("=== NLM Benchmark ===\n")
    
    # Create test brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses\n")
    
    # Run benchmarks
    print("Running benchmarks...")
    
    brain_creation_time = benchmark_brain_creation(50)
    print(f"Brain creation time: {brain_creation_time*1000:.2f} ms (average)")
    
    step_time = benchmark_step_processing(brain, 500)
    print(f"Step processing time: {step_time*1000:.4f} ms per step")
    
    memory_usage = benchmark_memory_usage(brain, 200)
    print(f"Memory usage: {memory_usage:.2f} MB")
    
    # Performance metrics
    steps_per_second = 1.0 / step_time
    print(f"Processing rate: {steps_per_second:.0f} steps/second")
    
    print("\n=== Benchmark Complete ===")

if __name__ == "__main__":
    main()
'''
    
    with open(benchmark_script, 'w') as f:
        f.write(script_content)
    
    print(f"Benchmark script created: {benchmark_script}")
    
    # Make executable
    benchmark_script.chmod(0o755)
    
    # Run it
    return run_command([sys.executable, str(benchmark_script)])

def manage_config(action: str, key: Optional[str] = None, value: Optional[str] = None):
    """Manage configuration."""
    config_file = Path("config.json")
    
    if action == "get":
        if config_file.exists():
            with open(config_file, 'r') as f:
                config = json.load(f)
            
            if key:
                print(f"{key}: {config.get(key, 'Not found')}")
            else:
                print("Configuration:")
                for k, v in config.items():
                    print(f"  {k}: {v}")
        else:
            print("Configuration file not found")
    
    elif action == "set":
        if not key or not value:
            print("Key and value required for set action")
            return False
        
        # Load existing config
        config = {}
        if config_file.exists():
            with open(config_file, 'r') as f:
                config = json.load(f)
        
        # Set value (try to convert to appropriate type)
        try:
            # Try to convert to int
            config[key] = int(value)
        except ValueError:
            try:
                # Try to convert to float
                config[key] = float(value)
            except ValueError:
                # Keep as string
                config[key] = value
        
        # Save config
        with open(config_file, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Set {key} = {value}")
    
    elif action == "create":
        # Create default config
        config = {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "brain.connection_probability": 0.15,
            "simulation.timestep": 0.001,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
        }
        
        with open(config_file, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Created default configuration: {config_file}")
    
    return True

def show_help():
    """Show help information."""
    help_text = """
NLM Command Line Interface
==========================

This script provides a comprehensive command-line interface for working with
the Neural Learning Machine (NLM) project.

Commands:

build
    Build the C++ project
    Usage: nlm_cli.py build

build-python
    Build Python bindings
    Usage: nlm_cli.py build-python

test
    Run NLM tests
    Usage: nlm_cli.py test

demo [phase] [demo_name]
    Run a demonstration
    Phases: phase2, phase3, phase4, phase5, phase6
    Usage: nlm_cli.py demo phase3

benchmark [type]
    Run benchmarks
    Types: default, comprehensive, memory, speed
    Usage: nlm_cli.py benchmark speed

config [action] [key] [value]
    Configuration management
    Actions: get, set, create
    Usage: nlm_cli.py config get brain.neuron_count
           nlm_cli.py config set brain.neuron_count 2000
           nlm_cli.py config create

help
    Show this help message

Examples:
    # Build the project
    nlm_cli.py build

    # Run Phase 3 demonstration
    nlm_cli.py demo phase3

    # Run benchmarks
    nlm_cli.py benchmark speed

    # Check configuration
    nlm_cli.py config get brain.neuron_count
    nlm_cli.py config create

Notes:
- All build operations will create a 'build' directory
- Python bindings are installed to the system or virtual environment
- Demos require the C++ project to be built first
- Configuration is stored in config.json

For more information, visit: https://github.com/nlm-project/nlm
    """
    
    print(help_text)

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "command",
        choices=["build", "build-python", "test", "demo", "benchmark", "config", "help"],
        help="Command to execute"
    )
    
    parser.add_argument(
        "args",
        nargs="*",
        help="Additional command arguments"
    )
    
    args = parser.parse_args()
    
    # Command dispatch
    if args.command == "help":
        show_help()
    
    elif args.command == "build":
        success = build_cpp()
        sys.exit(0 if success else 1)
    
    elif args.command == "build-python":
        success = build_python()
        sys.exit(0 if success else 1)
    
    elif args.command == "test":
        success = run_tests()
        sys.exit(0 if success else 1)
    
    elif args.command == "demo":
        phase = args.args[0] if args.args else None
        success = run_demo(phase)
        sys.exit(0 if success else 1)
    
    elif args.command == "benchmark":
        benchmark_type = args.args[0] if args.args else "default"
        success = run_benchmark(benchmark_type)
        sys.exit(0 if success else 1)
    
    elif args.command == "config":
        action = args.args[0] if len(args.args) > 0 else None
        key = args.args[1] if len(args.args) > 1 else None
        value = args.args[2] if len(args.args) > 2 else None
        
        success = manage_config(action, key, value)
        sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
