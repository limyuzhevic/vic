#!/usr/bin/env bash
# Example CLI usage for NLM neural simulations

# Basic simulation
nlm simulation --neuron-count 1000 --max-steps 500

# Simulation with output file and JSON format
nlm simulation --neuron-count 2000 --max-steps 1000 --output-file results.json --output-format json

# Advanced simulation with profiling and validation
nlm simulation --neuron-count 500 --max-steps 2000 --enable-profiling --enable-validation --parallel 4

# Configuration management
nlm config --list
nlm config --config-file myconfig.cfg --validate
nlm config --output-file exported.cfg

# Debugging and profiling
nlm debug --attach --log-level debug
nlm debug --profile --output-file profile.json
nlm debug --validate --memory

# System validation
nlm validate --memory --threads --config --plugins

# Plugin management
nlm plugin --list
nlm plugin --load ./plugins
nlm plugin --info MyPlugin

# Performance analysis
nlm benchmark --neuron-count 1000 --steps 500 --output-file results.csv
nlm profile --benchmark --output-file perf.json
nlm profile --analyze --type performance

# System analysis
nlm analyze --type performance --output-format json
nlm analyze --type memory --output-file memory_stats.csv

# Interactive help and discovery
nlm help simulation
nlm help --list
nlm help --search "simulation"
nlm help --category simulation

# Configuration file examples
# configs/default.cfg
#neuron_count = 500
#max_steps = 1000
#timestep = 0.001
#output_format = "text"
#enable_profiling = false

# Environment variable examples
export NLM_LOG_LEVEL=info
export NLM_VERBOSE=true
export NLM_CONFIG_FILE=configs/myconfig.cfg
export NLM_THREADS=4
