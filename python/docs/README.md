# NLM Python Bindings Enhanced Documentation
# ============================================
#
# This document provides comprehensive documentation for the enhanced Python bindings
# of the NLM (Neural Learning Machine) simulation framework.
#
# The enhanced bindings provide:
# - Modern Python API with type hints and context managers
# - NumPy integration for scientific computing
# - Matplotlib and Plotly visualization
# - Performance profiling and benchmarking
# - Checkpoint/restore capabilities
# - Learning and development examples
# - Neuromodulation examples
# - Advanced world interaction
#

# Table of Contents
# -----------------
# 1. Overview
# 2. Installation
# 3. Basic Usage
# 4. Advanced API
# 5. Examples
# 6. Visualization
# 7. Performance
# 8. API Reference
# 9. Migration Guide
#

# 1. Overview
# -----------
# The NLM Python bindings provide access to the NLM C++ neural simulation framework
# through an enhanced Python API that integrates with the scientific Python ecosystem.
#
# Key features:
# - Type hints and autocomplete support
# - Context managers for resource management
# - NumPy integration for sensor data
# - Matplotlib and Plotly visualization
# - Performance profiling and benchmarking
# - Checkpoint/restore for reproducibility
# - Learning and development examples
#
# 2. Installation
# ---------------
# The enhanced Python bindings are installed as a Python package:
#
# ```bash
# pip install pynlm
# ```
#
# The package includes the compiled C++ extensions and example notebooks.
#
# 3. Basic Usage
# ----------------
# Basic usage of the NLM Python bindings:
#
# ```python
# from pynlm import NeuralSystem, SensorData
# import numpy as np
#
# # Create neural system
# brain = NeuralSystem()
#
# # Create world
# world = brain.create_world(width=100.0, height=100.0)
#
# # Create agent
# agent = brain.create_agent(0.0, 0.0)
#
# # Run episode
# episode, total_reward = brain.run_episode(max_steps=1000)
#
# print(f"Total reward: {total_reward}")
# ```
#
# 4. Advanced API
# ------------------
# The enhanced API provides:
#
# - NeuralSystem: High-level interface with context managers
# - DataManager: Data collection and analysis utilities
# - PerformanceProfiler: Performance profiling
# - Visualizer: Visualization tools
# - LearningExamples: Learning and memory examples
# - DevelopmentExamples: Development system examples
# - NeuromodulationExamples: Neuromodulation examples
# - EnvironmentExamples: Complex environment examples
# - PerformanceExamples: Performance and monitoring examples
# - CheckpointExamples: Checkpoint/restore examples
#
# 5. Examples
# -------------
# The examples directory contains:
#
# - basic_examples/: Basic usage examples
# - learning_examples/: Learning and memory examples
# - development_examples/: Development system examples
# - neuromodulation_examples/: Neuromodulation examples
# - environment_examples/: Complex environment examples
# - performance_examples/: Performance and monitoring examples
# - checkpoint_examples/: Checkpoint/restore examples
#
# 6. Visualization
# ----------------
# The enhanced bindings provide visualization capabilities:
#
# - Metrics plotting: Plot brain metrics over time
# - Connectivity visualization: Visualize brain connectivity
# - World dynamics: Visualize world state and agent behavior
# - Interactive plots: Use Plotly for interactive visualizations
#
# Example:
#
# ```python
# from pynlm import Visualizer
#
# visualizer = Visualizer(backend='matplotlib')
# visualizer.plot_metrics(metrics, 'brain_metrics.png')
# ```
#
# 7. Performance
# ---------------
# The enhanced bindings provide performance profiling:
#
# - PerformanceProfiler: Profile CPU and memory usage
# - Metrics collection: Collect brain performance metrics
# - Benchmarking: Run performance benchmarks
#
# Example:
#
# ```python
# from pynlm import PerformanceProfiler
#
# profiler = PerformanceProfiler()
# profiler.start_profiling()
#
# # Run simulation
# brain.run_episode(max_steps=1000)
#
# profiler.end_profiling()
# print(f"Average time: {profiler.avg_time:.4f}s")
# print(f"Average memory: {profiler.avg_memory:.2f} MB")
# ```
#
# 8. API Reference
# ------------------
# The enhanced API provides comprehensive documentation with type hints.
#
# Key classes:
#
# - NeuralSystem: High-level interface
# - BrainSession: Context manager for brain sessions
# - WorldSession: Context manager for world sessions
# - SensorData: Container for sensor data
# - BrainMetrics: Brain performance metrics
# - DataManager: Data collection and analysis
# - PerformanceProfiler: Performance profiling
# - Visualizer: Visualization tools
#
# Helper functions:
#
# - configure_logging: Configure logging
# - set_config: Set global configuration
# - get_config: Get global configuration
#
# Exception classes:
#
# - NLMError: Base exception
# - ValidationError: Configuration validation error
# - ConfigurationError: Configuration error
# - DeviceError: Device-related error
#
# 9. Migration Guide
# -------------------
# The enhanced bindings maintain backward compatibility with the original bindings.
#
# Migration from original bindings:
#
# 1. Import enhanced bindings instead of original:
#
# ```python
# # Original
# from pynlm import Brain, SimpleWorld
#
# # Enhanced
# from pynlm import NeuralSystem, Brain
# ```
#
# 2. Use NeuralSystem for high-level operations:
#
# ```python
# # Original
# brain = createBrain(config)
# world = createSimpleWorld()
#
# # Enhanced
# brain = NeuralSystem()
# world = brain.create_world()
# ```
#
# 3. Use context managers for resource management:
#
# ```python
# # Original - manual management
# brain.initialize()
#
# # Enhanced - context manager
# with BrainSession(brain):
#     # Use brain
#     pass
# ```
#
# 4. Use SensorData for sensor input:
#
# ```python
# # Original - manual data handling
# vision = vision_sensor.getData()
#
# # Enhanced - structured data
# sensor_data = SensorData.from_vision(vision_data)
# ```
#
# Benefits of enhanced bindings:
#
# - Type hints for better IDE support
# - Context managers for resource management
# - NumPy integration for scientific computing
# - Visualization capabilities
# - Performance profiling
# - Better error handling
# - Comprehensive documentation
#
# The enhanced bindings provide a more Pythonic and feature-rich interface
# while maintaining full backward compatibility.
