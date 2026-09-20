# NLM Build Configuration Documentation
# Comprehensive documentation for NLM build configuration options

# ==============================================================================
# CMake Configuration Documentation
# ==============================================================================

# NLM Build System Configuration
# ==============================

# This document describes all available configuration options for the NLM build system.

# ==============================================================================
# General Configuration
# ==============================================================================

## CMAKE_BUILD_TYPE
# --------------------
# Available build types:
# - Debug: Full debugging symbols, no optimizations
# - Release: Full optimizations, no debugging symbols
# - RelWithDebInfo: Optimizations + debugging symbols
# - MinSizeRel: Optimizations for minimum binary size

# Default: Release

# ==============================================================================
# Feature Options
# ==============================================================================

## NLM_ENABLE_SIMD
# -----------------
# Enable SIMD (Single Instruction, Multiple Data) optimizations
# Default: ON
# Options: ON, OFF

## NLM_ENABLE_OPENMP
# -----------------
# Enable OpenMP parallelization
# Default: ON
# Options: ON, OFF

## NLM_ENABLE_CUDA
# ----------------
# Enable CUDA acceleration (requires CUDA toolkit)
# Default: OFF
# Options: ON, OFF

## NLM_ENABLE_MPI
# ----------------
# Enable MPI parallelization
# Default: OFF
# Options: ON, OFF

## NLM_ENABLE_PROFILING
# -------------------
# Enable profiling support (gprof, perf)
# Default: OFF
# Options: ON, OFF

## NLM_ENABLE_COVERAGE
# --------------------
# Enable code coverage reporting
# Default: OFF
# Options: ON, OFF

## NLM_ENABLE_SANITIZERS
# ----------------------
# Enable memory error sanitizers
# Default: OFF
# Options: ON, OFF

## NLM_ENABLE_ASAN
# -----------------
# Enable AddressSanitizer for memory error detection
# Default: OFF
# Options: ON, OFF

## NLM_ENABLE_UBSAN
# ------------------
# Enable UndefinedBehaviorSanitizer for undefined behavior detection
# Default: OFF
# Options: ON, OFF

# ==============================================================================
# Performance Options
# ==============================================================================

## NLM_BUILD_OPTIMIZATION_LEVEL
# -----------------------------
# Optimization level (0-3)
# Default: 2
# Options: 0 (none), 1 (minimal), 2 (balanced), 3 (aggressive)

## NLM_BUILD_WARNINGS_AS_ERRORS
# -------------------------------
# Treat compiler warnings as errors
# Default: OFF
# Options: ON, OFF

## NLM_BUILD_VERBOSE_LOGGING
# --------------------------
# Enable verbose build logging
# Default: OFF
# Options: ON, OFF

# ==============================================================================
# Build Templates
# ==============================================================================

## BUILD_TEMPLATE
# ----------------
# Build template for different scenarios:
# - development: For active development with debugging
# - testing: For continuous integration and testing
# - production: For final release builds
# - performance: For high-performance computing

# Default: Not set (uses general configuration)

# ==============================================================================
# Platform Detection
# ==============================================================================

# Platform detection is automatic:
# - Linux: For Linux systems
# - Windows: For Windows systems
# - MacOS: For macOS systems

# ==============================================================================
# Configuration File Generation
# ==============================================================================

# NLM generates the following configuration files:
# - config/nlm_build_config.h: Build-time configuration header
# - config/nlm_features.h: Feature flags header
# - config/build_summary.txt: Build configuration summary

# ==============================================================================
# Python Configuration Documentation
# ==============================================================================

# ==============================================================================
# Python Bindings Configuration
# ==============================================================================

## Module Configuration
# --------------------

### module_name
# Python module name
# Default: "nlm"

### python_module
# Underlying Python package name
# Default: "pynlm"

### extension_name
# C++ extension name
# Default: "nlm"

### enable_pybind11
# Enable pybind11 bindings
# Default: True
# Options: True, False

### enable_python_tests
# Enable Python tests
# Default: True
# Options: True, False

### enable_python_examples
# Enable Python examples
# Default: True
# Options: True, False

### enable_python_docs
# Enable Python documentation
# Default: False
# Options: True, False

### python_version_min
# Minimum Python version
# Default: "3.8"

### python_version_max
# Maximum Python version
# Default: "3.12"

### threading_model
# Threading model for bindings
# Default: "dynamic"
# Options: "static", "dynamic", "auto"

### gil_state_management
# Global Interpreter Lock state management
# Default: "enabled"
# Options: "enabled", "disabled", "permissive"

### numpy_support
# Enable NumPy support
# Default: True
# Options: True, False

### scipy_support
# Enable SciPy support
# Default: False
# Options: True, False

# ==============================================================================
# Optimization Configuration
# ==============================================================================

### enable_simd
# Enable SIMD optimizations
# Default: True
# Options: True, False

### enable_openmp
# Enable OpenMP parallelization
# Default: True
# Options: True, False

### enable_cuda
# Enable CUDA acceleration
# Default: False
# Options: True, False

### enable_mpi
# Enable MPI parallelization
# Default: False
# Options: True, False

### optimization_level
# Optimization level (0-3)
# Default: "2"
# Options: "0", "1", "2", "3"

### warnings_as_errors
# Treat warnings as errors
# Default: False
# Options: True, False

### verbose_logging
# Enable verbose logging
# Default: False
# Options: True, False

### profile_bindings
# Enable profiling for bindings
# Default: False
# Options: True, False

### coverage_enabled
# Enable code coverage
# Default: False
# Options: True, False

### sanitizers_enabled
# Enable memory sanitizers
# Default: False
# Options: True, False

### asan_enabled
# Enable AddressSanitizer
# Default: False
# Options: True, False

### ubsan_enabled
# Enable UndefinedBehaviorSanitizer
# Default: False
# Options: True, False

# ==============================================================================
# Platform Configuration
# ==============================================================================

### system
# Detected system
# Example: "Linux", "Windows", "Darwin"

### architecture
# Detected architecture
# Example: "x86_64", "aarch64"

### python_implementation
# Python implementation
# Example: "CPython", "PyPy"

### python_version
# Python version string
# Example: "3.8.10"

### compiler
# Detected compiler
# Example: "gcc", "clang", "msvc"

# ==============================================================================
# Path Configuration
# ==============================================================================

### build_dir
# Build directory path
# Default: "build"

### install_dir
# Installation directory
# Default: "python_install"

### include_dir
# Include directory path
# Default: "python/include"

### library_dir
# Library directory path
# Default: "lib"

### config_file
# Configuration file name
# Default: "python_config.json"

# ==============================================================================
# Runtime Configuration
# ==============================================================================

### max_threads
# Maximum number of threads
# Default: CPU core count

### memory_limit_mb
# Memory limit in MB
# Default: 1024

### simulation_step
# Simulation timestep
# Default: 0.001

### random_seed
# Random seed for reproducibility
# Default: 42

### enable_logging
# Enable runtime logging
# Default: True
# Options: True, False

### log_level
# Log level
# Default: "INFO"
# Options: "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"

### log_file
# Log file name
# Default: "nlm_python.log"

# ==============================================================================
# Testing Configuration
# ==============================================================================

### test_paths
# Test paths
# Default: ["tests", "python/tests"]

### pytest_options
# pytest options
# Default: ["-v", "--tb=short"]

### coverage_threshold
# Code coverage threshold percentage
# Default: 80.0

### benchmark_enabled
# Enable benchmarking
# Default: False
# Options: True, False

# ==============================================================================
# Development Configuration
# ==============================================================================

### enable_linting
# Enable code linting
# Default: True
# Options: True, False

### enable_type_checking
# Enable type checking
# Default: True
# Options: True, False

### black_config
# Black formatter configuration

### isort_config
# isort configuration

### mypy_config
# mypy configuration

# ==============================================================================
# Configuration File Format
# ==============================================================================

# Configuration files use JSON format with the following structure:
# {
#   "bindings": {...},
#   "optimization": {...},
#   "platform": {...},
#   "paths": {...},
#   "runtime": {...},
#   "testing": {...},
#   "development": {...}
# }

# ==============================================================================
# Usage Examples
# ==============================================================================

# Example 1: Create custom configuration
# cat > nlm_config.json << EOF
# {
#   "bindings": {
#     "enable_cuda": true,
#     "enable_numpy": true
#   },
#   "optimization": {
#     "optimization_level": "3",
#     "enable_openmp": false
#   }
# }
# EOF

# Example 2: Load configuration and modify
# python -c "
# from python_config import load_config, set_config
# config = load_config('nlm_config.json')
# set_config('bindings', 'enable_cuda', True)
# print(config['bindings']['enable_cuda'])
# "

# Example 3: Generate build arguments
# python -c "
# from python_config import load_config, get_build_args
# config = load_config('nlm_config.json')
# args = get_build_args()
# print(' '.join(args))
# "

# Example 4: Validate configuration
# python -c "
# from python_config import load_config, validate_config
# config = load_config('nlm_config.json')
# if validate_config():
#     print('Configuration is valid')
# else:
#     print('Configuration is invalid')
# "

# ==============================================================================
# Configuration File Templates
# ==============================================================================

# Configuration file templates are available in the configs/ directory.

# ==============================================================================
# Troubleshooting
# ==============================================================================

# Configuration validation errors:
# - "Unknown configuration section: <section>" - Add section to _DEFAULT_CONFIG
# - "Unknown configuration key: <key> in section <section>" - Add key to _DEFAULT_CONFIG
# - Invalid log level: Use one of "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"
# - Invalid threading model: Use one of "static", "dynamic", "auto"
# - Invalid GIL state management: Use one of "enabled", "disabled", "permissive"
# - Invalid optimization level: Use one of "0", "1", "2", "3"

# Build system validation errors:
# - CUDA requested but CUDA toolkit not found
# - MPI requested but MPI not found
# - Optimization level must be between 0 and 3