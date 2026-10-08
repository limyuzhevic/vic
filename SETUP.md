# NLM Python Bindings - Setup Configuration

This file defines the package structure and dependencies for the NLM Python bindings.

# Core Python Package Structure
# ============================

# Package contents:
# src/pynlm/          # Main NLM module
#   __init__.py      # Main module entry point
#   core.py          # Core classes (NLMConfig, NLMBrain, etc.)
#   config.py        # Configuration management
#   simulation.py    # Simulation utilities
#   monitoring.py     # Performance monitoring
#   enums.py         # Enum definitions
#   benchmark.py     # Benchmarking tools
#   explorer.py      # Exploration tools
#   developer.py     # Development tools
#   __pycache__/      # Python bytecode cache
#
# README.md          # Documentation
# LICENSE            # License information
# pyproject.toml    # Package configuration
# src/pynlm.pyx     # C++ extension module (placeholder)
# build/            # Build directory
# dist/             # Distribution directory
#
# Additional directories:
# examples/         # Usage examples
# tests/            # Test suite
# benchmarks/       # Performance benchmarks
# docs/             # Documentation
#
# Configuration files:
# configs/default.cfg  # Default NLM configuration
# configs/development.cfg  # Development configuration
# configs/production.cfg   # Production configuration
#
# Build outputs:
# _pynlm.cpython-312-x86_64-linux-gnu.so  # Compiled extension module
# pynlm*.whl                                # Python wheel
# nlm*.tar.gz                               # Source distribution
#
# Installation:
# ============================
# pip install .
# pip install -e .               # Development mode
# pip install .[extended]         # With optional dependencies
# pip install .[dev]              # With development tools
#
# Building:
# ============================
# python -m build                     # Modern build system
# pip wheel . --wheel-dir dist        # Create wheel
# pip sdist . --dist-dir dist         # Create source distribution
# scikit-build-core                    # Advanced build options
#
# Testing:
# ============================
# pytest                              # Run all tests
# pytest tests/                      # Run core tests
# pytest benchmarks/                 # Run benchmarks
# pytest --cov=pynlm                # Run with coverage
#
# Running:
# ============================
# pynlm-info                          # Show installation info
# pynlm-run --steps 100               # Run simple simulation
# pynlm-benchmark --neurons 1000     # Run benchmark
# pynlm-explore --mode interactive    # Interactive exploration
#
# Documentation:
# ============================
# Build HTML documentation:
#   pip install sphinx
#   sphinx-build docs/ docs/_build/html
#
# Build PDF documentation:
#   pip install sphinx-rtd-theme
#   sphinx-build -b latex docs/ docs/_build/latex
#   pdflatex docs/_build/latex/nlm.tex
#
# Online documentation:
#   https://nlm.readthedocs.io
#
# License:
# ============================
# MIT License (see LICENSE file)
#
# Copyright (c) 2024 NLM Team
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Contribution Guidelines:
# ============================
#
# 1. Fork the repository
# 2. Create a feature branch
# 3. Implement your changes
# 4. Add tests for new functionality
# 5. Run existing tests to ensure no regressions
# 6. Submit a pull request
#
# Code style:
# ============================
#
# We follow PEP 8 style guidelines with some modifications:
# - Line length: 88 characters (Black compatible)
# - Import ordering: isort compatible
# - Documentation: Google style docstrings
# - Type hints: Full type annotations
# - Error handling: Comprehensive exception handling
#
# Performance considerations:
# ============================
#
# 1. Memory management: Use context managers for resource cleanup
# 2. Caching: Implement caching for expensive operations
# 3. Parallel processing: Use multiprocessing for CPU-intensive tasks
# 4. Streaming: Process large datasets in chunks
# 5. Native extensions: Use C++ for performance-critical code
#
# Testing strategy:
# ============================
#
# Unit tests:
#   - Test individual classes and functions
#   - Mock external dependencies
#   - Test edge cases and error conditions
#
# Integration tests:
#   - Test interactions between components
#   - Test complete simulation workflows
#   - Test configuration loading and saving
#
# Performance tests:
#   - Benchmark simulation speed
#   - Measure memory usage
#   - Test scalability with large models
#
# Example configuration files:
# ============================
#
# configs/default.cfg:
#   # Default configuration for NLM simulations
#   random_seed = 42
#   simulation_timestep = 0.001
#   neuron_count = 1000
#   region_count = 2
#   connection_probability = 0.1
#
# configs/development.cfg:
#   # Configuration for development and testing
#   random_seed = 123
#   simulation_timestep = 0.001
#   neuron_count = 100
#   region_count = 1
#   connection_probability = 0.05
#   log_level = DEBUG
#
# configs/production.cfg:
#   # Configuration for production simulations
#   random_seed = 456
#   simulation_timestep = 0.0005
#   neuron_count = 5000
#   region_count = 5
#   connection_probability = 0.15
#   log_level = INFO
#
# Example usage:
# ============================
#
# Basic usage:
#   import pynlm
#
#   # Create configuration
#   config = pynlm.create_default_config()
#   config.set("brain.neuron_count", 2000)
#   config.set("plasticity.stdp.enable", True)
#
#   # Create brain
#   brain = pynlm.create_brain(config)
#   
#   # Run simulation
#   for step in range(100):
#       brain.step(step)
#
#   print(f"Simulation complete! {brain.get_firing_neurons()} neurons firing")
#
# Advanced usage:
#   import pynlm
#
#   # Create advanced brain with all features enabled
#   brain = pynlm.create_advanced_brain(
#       neuron_count=5000,
#       regions=5,
#       connectivity=0.15
#   )
#
#   # Create world and agent
#   world = pynlm.SimpleWorld(width=30, height=30)
#   agent = pynlm.create_agent_brain(brain)
#   agent.initialize(world)
#
#   # Enable all subsystems
#   agent.enable_subsystems(
#       reward_modulation=True,
#       structural_plasticity=True,
#       development=True,
#       curiosity=True
#   )
#
#   # Run complete simulation
#   results = pynlm.run_simulation(
#       brain, world, agent, 
#       num_steps=1000, 
#       verbose=True
#   )
#
# Command line interface:
# ============================
#
#   pynlm-info                          # Show installation information
#   pynlm-run --steps 100               # Run simple simulation
#   pynlm-run --steps 1000 --world-size 30  # Run with world
#   pynlm-benchmark --neurons 1000      # Run benchmark
#   pynlm-benchmark --neurons 5000 --iterations 5  # Run with more iterations
#   pynlm-explore --mode interactive    # Open interactive explorer
#   pynlm-dev --config config.json --validate  # Validate configuration
#   pynlm-dev --config config.json --optimize  # Optimize configuration
#
# Building from source:
# ============================
#
# Prerequisites:
#   - Python 3.8 or higher
#   - pip
#   - scikit-build-core
#   - pybind11
#   - CMake 3.16 or higher
#
# Installation:
#   pip install scikit-build-core pybind11 numpy typing-extensions
#   pip install .
#
# Development installation:
#   pip install -e .[dev]
#
# Building with extended features:
#   pip install .[extended]
#
# Building with development tools:
#   pip install .[dev]
#
# Running tests:
# ============================
#
#   pytest                              # Run all tests
#   pytest tests/                      # Run core tests
#   pytest benchmarks/                 # Run benchmarks
#   pytest --cov=pynlm                # Run with coverage
#   pytest --benchmark-enable          # Run with benchmarking
#
# Building documentation:
# ============================
#
#   pip install sphinx sphinx-rtd-theme
#   sphinx-build docs/ docs/_build/html
#   open docs/_build/html/index.html
#
# Building PDF documentation:
#   sphinx-build -b latex docs/ docs/_build/latex
#   pdflatex docs/_build/latex/nlm.tex
#
# Testing the build:
# ============================
#
#   # Test import
#   python -c "import pynlm; print('NLM imported successfully')"
#
#   # Test basic functionality
#   python -c "
#       import pynlm
#       config = pynlm.create_default_config()
#       brain = pynlm.create_brain(config)
#       brain.step(0)
#       print(f'Test passed: {brain.get_firing_neurons()} neurons firing')
#   "
#
#   # Test configuration
#   python -c "
#       import pynlm
#       config = pynlm.NLMConfig()
#       config.set('brain.neuron_count', 100)
#       print(f'Configuration test passed: {config.get(\"brain.neuron_count\")}')
#   "
#
# Common errors and solutions:
# ============================
#
# Error: ModuleNotFoundError: No module named 'pynlm'
# Solution: Install the package with 'pip install .'
#
# Error: ImportError: cannot import name '_pynlm'
# Solution: Install the C++ extension with 'pip install .' or rebuild with 'python -m build'
#
# Error: AttributeError: module 'pynlm' has no attribute 'create_brain'
# Solution: Check if you're using the correct version of the module
#
# Error: ValueError: Configuration key 'brain.neuron_count' not found
# Solution: Set the configuration key before using it
#
# Error: TypeError: step() missing required argument: 'step_number'
# Solution: Provide the step number: brain.step(0)
#
# Performance tips:
# ============================
#
# 1. Use create_reduced_brain() for rapid prototyping
# 2. Use create_advanced_brain() for production simulations
# 3. Enable performance monitoring to identify bottlenecks
# 4. Use batch processing for multiple simulations
# 5. Implement caching for repeated operations
# 6. Use parallel processing for CPU-intensive tasks
#
# Scalability considerations:
# ============================
#
# 1. Neuron count: Keep under 10,000 for real-time applications
# 2. Region count: Balance between modularity and performance
# 3. Connection probability: Higher values increase computation time
# 4. Simulation timestep: Smaller values increase accuracy but slow down simulation
# 5. Memory: Monitor memory usage during long simulations
#
# Migration guide:
# ============================
#
# From v1.0.0 to v2.0.0:
#
# 1. API changes:
#    - New configuration system with type-safe access
#    - Enhanced brain simulation with performance monitoring
#    - New world and agent interfaces
#    - Additional utility functions and tools
#
# 2. Installation:
#    pip install pynlm==2.0.0
#
# 3. Migration code:
#    # Old code:
#    from pynlm import createBrain, createDefaultConfig
#
#    # New code:
#    import pynlm
#    config = pynlm.create_default_config()
#    brain = pynlm.create_brain(config)
#
# 4. Configuration:
#    # Old format: brain.neuron_count = 1000
#    # New format: config.set("brain.neuron_count", 1000)
#
# 5. Simulation:
#    # Old code:
#    for i in range(100):
#        brain.step(i)
#
#    # New code (with performance monitoring):
#    for step in range(100):
#        brain.step(step)
#        if step % 10 == 0:
#            print(f"Step {step}: {brain.get_firing_neurons()} neurons firing")
#
# For more information, visit: https://nlm.readthedocs.io
#
# Troubleshooting:
# ============================
#
# If you encounter issues, please:
#
# 1. Check the error message and traceback
# 2. Verify you have the latest version installed
# 3. Check your system requirements
# 4. Review the configuration file
# 5. Run tests to identify the issue
# 6. Create a minimal reproduction case
# 7. Check the documentation for solutions
#
# Common troubleshooting steps:
#
#   # Check installation
#   pip list | grep pynlm
#
#   # Test import
#   python -c "import pynlm; print(pynlm.__version__)"
#
#   # Run basic test
#   python -m pynlm.test.basic
#
#   # Check configuration
#   python -m pynlm.test.config
#
#   # Check performance
#   python -m pynlm.test.performance
#
# Support:
# ============================
#
# For support, please:
#
# 1. Check the documentation at https://nlm.readthedocs.io
# 2. Search existing issues in the repository
# 3. Create a new issue with detailed error information
# 4. Include your configuration and system information
# 5. Provide a minimal reproduction case
#
# We appreciate your feedback and contributions to improve the NLM Python bindings!
