#!/usr/bin/env python3
"""
NLM Experiment Management Quick Start Guide
===========================================

This guide provides a quick overview of the experiment management system
for the NLM (Neural Learning Machine) project.
"""

from .experiment import (
    createExperimentManager,
    createBatchProcessor,
    createParameterSweep,
    createExperimentTemplate,
    createExperimentResult,
    createResultAnalyzer,
    getExperimentSystemStatus,
)

print("NLM Experiment Management System")
print("=" * 50)

# Get system status
status = getExperimentSystemStatus()
print(status)

# Create experiment manager
experiment_manager = createExperimentManager()
print("✓ Experiment manager created")

# Create batch processor
batch_processor = createBatchProcessor()
print("✓ Batch processor created")

# Create parameter sweep
parameter_sweep = createParameterSweep([
    {'neuron_count': [500, 1000, 2000]},
    {'connection_probability': [0.05, 0.1, 0.2]}
])
print("✓ Parameter sweep created")

# Create experiment template
experiment_template = createExperimentTemplate("test_template")
print("✓ Experiment template created")

# Create experiment result
experiment_result = createExperimentResult("test_experiment")
print("✓ Experiment result created")

# Create result analyzer
result_analyzer = createResultAnalyzer()
print("✓ Result analyzer created")

print("\nExperiment Management System Ready!")
print("\nFor detailed usage, see:")
print("- EXPERIMENT_MANAGEMENT_OVERVIEW.md")
print("- nlm_cli.py")
print("- python/experiment/ directory")
