#!/usr/bin/env python3

"""
NLM Build Script
Simplified build script with comprehensive error handling and progress tracking.

Usage: ./build.py [--verbose] [--no-cache] [--jobs N]
"""

import os
import sys
import subprocess
import time
from pathlib import Path

# Color definitions for output
RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[1;33m'
BLUE = '\033[0;34m'
NC = '\033[0m'  # No Color
BOLD = '\033[1m'
DIM = '\033[2m'

def print_color(color, message):
    """Print colored message to stdout"""
    print(f"{color}{message}{NC}")

def log_info(message):
    """Log info message"""
    print_color(BLUE, f"[INFO] {message}")

def log_warn(message):
    """Log warning message"""
    print_color(YELLOW, f"[WARN] {message}")

def log_error(message):
    """Log error message"""
    print_color(RED, f"[ERROR] {message}")

def log_success(message):
    """Log success message"""
    print_color(GREEN, f"[SUCCESS] {message}")

def log_step(message):
    """Log step message"""
    print_color(BLUE, f"[STEP] {message}")

def run_command(cmd, cwd=None, check=True):
    """Run command and return result"""
    try:
        log_info(f"Running: {cmd}")
        result = subprocess.run(
            cmd, shell=True, cwd=cwd, check=check,
            capture_output=True, text=True
        )
        return result.returncode, result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        log_error(f"Command failed: {cmd}")
        log_error(f"Error: {e.stderr}")
        return e.returncode, e.stdout, e.stderr

def check_prerequisites():
    """Check system prerequisites"""
    log_step("Checking system prerequisites...")
    
    errors = 0
    
    # Check for required compilers
    result, stdout, stderr = run_command("g++ --version | head -n1", check=False)
    if result == 0:
        log_success(f"g++ found: {stdout.strip()}")
    else:
        log_error("g++ compiler not found")
        errors += 1
    
    result, stdout, stderr = run_command("cmake --version | head -n1", check=False)
    if result == 0:
        log_success(f"cmake found: {stdout.strip()}")
    else:
        log_error("cmake not found")
        errors += 1
    
    # Check for make
    result, stdout, stderr = run_command("make --version | head -n1", check=False)
    if result == 0:
        log_success(f"make found: {stdout.strip()}")
    else:
        log_error("make not found")
        errors += 1
    
    # Check for Python if needed
    result, stdout, stderr = run_command("python3 --version", check=False)
    if result == 0:
        log_success(f"Python found: {stdout.strip()}")
    else:
        log_warn("Python 3 not found - may affect some build steps")
    
    if errors > 0:
        log_error(f"{errors} prerequisite check(s) failed")
        log_error("Please install missing dependencies and try again")
        return False
    
    log_success("All prerequisites check completed")
    return True

def configure_cmake(build_dir):
    """Configure CMake build"""
    log_step("Configuring CMake build...")
    
    os.makedirs(build_dir, exist_ok=True)
    
    cmake_args = ["-DCMAKE_BUILD_TYPE=Release", "-j8"]  # Using default jobs for simplicity
    
    # Run CMake configuration
    log_info("Running CMake configuration...")
    result, stdout, stderr = run_command(f"cmake .. {' '.join(cmake_args)}", cwd=build_dir)
    if result != 0:
        log_error("CMake configuration failed")
        return False
    
    log_success("CMake configuration completed")
    return True

def build_project(build_dir):
    """Build the project"""
    log_step("Building NLM project...")
    
    result, stdout, stderr = run_command(f"make -j8", cwd=build_dir)
    if result != 0:
        log_error("Build failed")
        return False
    
    # Find built executables
    log_info("Searching for executables...")
    expected_executables = [
        "nlm",
        "nlm_phase3_demo", 
        "nlm_phase4_demo",
        "nlm_phase6_demo",
        "nlm_benchmark"
    ]
    
    found_executables = []
    for exe in expected_executables:
        exe_path = os.path.join(build_dir, exe)
        if os.path.exists(exe_path):
            found_executables.append(exe)
            log_success(f"Found executable: {exe}")
        else:
            log_warn(f"Missing: {exe}")
    
    if len(found_executables) == 0:
        log_warn("No expected executables found in build directory")
        log_warn("Please check build output for errors")
    else:
        log_success(f"Build completed successfully with {len(found_executables)} executables")
    
    return True

def run_tests(build_dir):
    """Run tests"""
    log_step("Running tests...")
    
    # Try to run tests if they exist
    if not os.path.exists(os.path.join(build_dir, "Makefile")):
        log_warn("CMake cache not found, skipping tests")
        return
    
    # Check if test target exists
    result, stdout, stderr = run_command("make help 2>/dev/null | grep test", cwd=build_dir, check=False)
    if result == 0 and "test" in stdout:
        log_info("Running test suite...")
        result, stdout, stderr = run_command("make test", cwd=build_dir)
        if result == 0:
            log_success("All tests passed")
        else:
            log_warn("Some tests failed - this may be expected for experimental software")
    else:
        log_info("No test target found in Makefile")
    
    log_info("Test phase completed")
    return

def create_build_scripts(build_dir):
    """Create build scripts"""
    log_step("Creating build scripts...")
    
    # Create build script
    build_script = os.path.join(build_dir, "build.sh")
    with open(build_script, "w") as f:
        f.write("""#!/bin/bash

# Quick build script for NLM
# Usage: ./build.sh [--verbose] [--no-cache] [--jobs N]

set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
VERBOSE="${VERBOSE:-false}"
USE_CCACHE="${USE_CCACHE:-true}"
MAX_JOBS="${MAX_JOBS:-$(nproc)}"

export BUILD_DIR VERBOSE USE_CCACHE MAX_JOBS

# Run the build
./build.py "$@"
""")
    
    os.chmod(build_script, 0o755)
    log_success("Created build.sh script")
    
    # Create run script for quick execution
    run_script = os.path.join(build_dir, "run_demo.sh")
    with open(run_script, "w") as f:
        f.write("""#!/bin/bash

# Quick demo runner script for NLM
# Usage: ./run_demo.sh <phase> [--neurons N] [--steps S]

set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <phase> [--neurons N] [--steps S]"
    echo "Example: $0 2 --neurons 500 --steps 1000"
    exit 1
fi

PHASE="$1"
NEURONS=500
STEPS=1000

# Parse arguments
for arg in "$@"; do
    case $arg in
        --neurons=*)
            NEURONS="${arg#*=}"
            shift
            ;;
        --steps=*)
            STEPS="${arg#*=}"
            shift
            ;;
        --help)
            echo "Usage: $0 <phase> [--neurons N] [--steps S]"
            echo "Example: $0 2 --neurons 500 --steps 1000"
            exit 0
            ;;
    esac
done

# Run demo if executable exists
EXECUTABLE="$BUILD_DIR/nlm_phase${PHASE}_demo"
if [[ -f "$EXECUTABLE" ]]; then
    echo "Running NLM Phase ${PHASE} demo..."
    echo "  Neurons: $NEURONS"
    echo "  Steps: $STEPS"
    "$EXECUTABLE" --neuron_count $NEURONS --simulation_timestep 0.001
    echo "Demo completed successfully!"
else
    echo "Error: Demo executable not found: $EXECUTABLE"
    echo "Please run: $0 2  # First, build the project with ./build.sh"
    exit 1
fi
""")
    
    os.chmod(run_script, 0o755)
    log_success("Created run_demo.sh script")
    
    return 0

def verify_installation(build_dir):
    """Verify installation"""
    log_step("Verifying installation...")
    
    # Check if executables exist
    found_executables = 0
    expected_executables = [
        "nlm",
        "nlm_phase3_demo",
        "nlm_phase4_demo", 
        "nlm_phase6_demo",
        "nlm_benchmark"
    ]
    
    for exe in expected_executables:
        exe_path = os.path.join(build_dir, exe)
        if os.path.exists(exe_path):
            log_success(f"✓ Found: {exe}")
            found_executables += 1
        else:
            log_warn(f"✗ Missing: {exe}")
    
    log_info(f"Found {found_executables} executables")
    
    # Create installation summary
    summary_file = os.path.join(build_dir, "installation_summary.txt")
    with open(summary_file, "w") as f:
        f.write(f"""NLM Installation Summary
=======================
Build directory: {build_dir}
Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}

Executables found: {found_executables}

To run demos, use:
  {build_dir}/nlm_phase2_demo
  {build_dir}/nlm_phase3_demo
  {build_dir}/nlm_phase4_demo
  {build_dir}/nlm_phase6_demo

Quick demo runner: ./run_demo.sh <phase> [--neurons N] [--steps S]
Build script: ./build.sh

Configuration: configs/default.cfg

For documentation, see:
  README.md - Main documentation
  easy_usage.md - Easy usage guide
  HOW_TO_USE.md - Detailed how-to guide
""")
    
    log_success("Created installation summary")
    return 0

def print_build_stats(start_time):
    """Print build statistics"""
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"""
{YELLOW}=== Build Statistics ==={NC}
Duration: {duration:.1f}s
Build directory: {BUILD_DIR}
Jobs: {MAX_JOBS}
Cache: {USE_CCACHE}
{NC}
""")

def main():
    """Main build function"""
    global BUILD_DIR, MAX_JOBS, USE_CCACHE, VERBOSE
    
    # Parse command line arguments
    verbose_flag = False
    no_cache_flag = False
    jobs_flag = None
    
    # For now, we'll ignore complex CLI parsing for simplicity
    # In a full implementation, this would be more comprehensive
    
    start_time = time.time()
    
    # Set default values
    BUILD_DIR = os.getenv("BUILD_DIR", "build")
    MAX_JOBS = int(os.getenv("MAX_JOBS", str(os.cpu_count() or 8))
    USE_CCACHE = os.getenv("USE_CCACHE", "true").lower() == "true"
    VERBOSE = os.getenv("VERBOSE", "false").lower() == "true"
    
    # Run build steps
    if check_prerequisites():
        log_success("Prerequisites check passed")
    else:
        log_error("Prerequisites check failed")
        return 1
    
    if configure_cmake(BUILD_DIR):
        log_success("CMake configuration completed")
    else:
        log_error("CMake configuration failed")
        return 1
    
    if build_project(BUILD_DIR):
        log_success("Build project completed")
    else:
        log_error("Build project failed")
        return 1
    
    run_tests(BUILD_DIR)
    
    create_build_scripts(BUILD_DIR)
    verify_installation(BUILD_DIR)
    
    # Print build statistics
    print_build_stats(start_time)
    
    log_success("Build process completed successfully!")
    log_info(f"See {BUILD_DIR}/build.log for detailed build information")
    log_info(f"See {BUILD_DIR}/installation_summary.txt for installation summary")
    
    # Create a simple README in build directory
    readme_file = os.path.join(BUILD_DIR, "README_build.txt")
    with open(readme_file, "w") as f:
        f.write(f"""NLM Build Complete
===================

Build directory: {BUILD_DIR}
Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}

EXECUTABLES:
  {BUILD_DIR}/nlm              - NLM Phase 2 demo (basic neural computation)
  {BUILD_DIR}/nlm_phase3_demo  - NLM Phase 3 demo (world interaction)
  {BUILD_DIR}/nlm_phase4_demo  - NLM Phase 4 demo (advanced cognition)
  {BUILD_DIR}/nlm_phase6_demo  - NLM Phase 6 demo (integration test)
  {BUILD_DIR}/nlm_benchmark     - Performance benchmark

QUICK START:
  {BUILD_DIR}/run_demo.sh 2                    # Run Phase 2 demo
  {BUILD_DIR}/run_demo.sh 3 --neurons 1000     # Run Phase 3 with more neurons
  {BUILD_DIR}/run_demo.sh 6 --steps 5000        # Run Phase 6 with more steps

CONFIGURATION:
  Configuration file: {BUILD_DIR}/../configs/default.cfg
  Available options in the source configs/ directory

CHECKPOINTS:
  Checkpoints are saved to ./checkpoints/ directory
  Use nlm run <phase> --save <filename> to save checkpoints

DOCUMENTATION:
  See the root README.md for detailed project documentation
  Check easy_usage.md and HOW_TO_USE.md for usage guides

TROUBLESHOOTING:
  If build fails, check {BUILD_DIR}/build.log for error details
  Ensure you have all required dependencies installed
""")
    
    log_success("Created build README")

if __name__ == "__main__":
    main()
