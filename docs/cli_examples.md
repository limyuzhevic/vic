# NLM Command-Line Tool Examples
# =============================

# This directory contains examples for using the brain-tool command-line utility
# that comes with the NLM project for brain manipulation and configuration.

# =============================================================================
# Basic Usage Examples
# =============================================================================

# 1. Get brain status and statistics
# brain-tool --action=status
#
# Expected output:
# === Brain Status ===
# Regions: 1
# Total neurons: 500
# Total synapses: 2500
# Active neurons: 0
# Firing neurons (this step): 0
# Total spikes: 0
# Pending spike events: 0
# Average firing rate: 0.000000
# E/I ratio: infinity
#
# Working memory traces: 0
# Episodic memory episodes: 0
# Dopamine level: 0.000000
# Developmental stage: 0
#
# Region 1 (Region_1):
#   500 neurons, 2500 synapses, avg weight: 0.100000

# 2. Save brain state to file
# brain-tool --action=save --output=brain_checkpoint.chk
#
# Output:
# Saving brain to: brain_checkpoint.chk
# Brain state saved successfully (1024 bytes)

# 3. Load brain state from file
# brain-tool --action=load --input=previous_checkpoint.chk
#
# Output:
# Loading brain from: previous_checkpoint.chk
# Loaded 2500 synapses
# Brain state loaded successfully

# 4. Reset brain
# brain-tool --action=reset
#
# Output:
# Resetting brain...
# NLM Brain reset complete

# 5. Run simulation
# brain-tool --action=run --steps=1000 --verbose
#
# Output (verbose):
# Step | Spikes | Firing | Total Spikes | E/I Ratio
# -----|--------|--------|--------------|----------
#   0  |    5    |    2    |            5 |    1.000
# 100 |   23    |    7    |           23 |    1.050
# 200 |   45    |   12    |           45 |    1.100
# ...

# =============================================================================
# Configuration Examples
# =============================================================================

# 6. Load configuration from file and set runtime values
# brain-tool --config=advanced.cfg \
#           --set=neuron_count=1000 \
#           --set=stdp_ltp_weight=0.02 \
#           --set=stdp_tau=25.0 \
#           --action=status
#
# This loads from advanced.cfg, then overrides specific values, then shows status

# 7. Run simulation with custom configuration
# brain-tool --config=learning.cfg \
#           --steps=500 \
#           --action=run \
#           --verbose

# =============================================================================
# Configuration File Examples
# =============================================================================

# The brain-tool uses the enhanced NLM configuration system which supports:
# - Type-safe configuration values (integers, doubles, strings, booleans)
# - Source priority: Runtime > CommandLine > File > Default
# - Environment variable loading with prefix support
# - Configuration validation and error handling
# - Version compatibility checking

# Example config file (brain-tool.ini):
# [simulation]
# neuron_count = 1000
# region_count = 1
# simulation_timestep = 0.001
# random_seed = 42
#
# [plasticity]
# stdp_ltp_weight = 0.01
# stdp_ltd_weight = 0.012
# stdp_tau = 20.0
# synaptogenesis_rate = 0.0001
# pruning_rate = 0.00001
#
# [memory]
# replay_interval = 100
# consolidation_interval = 1000
#
# [neuromodulation]
# dopamine_sensitivity = 1.0
# curiosity_threshold = 0.3
# novelty_decay = 0.99

# =============================================================================
# Advanced Usage Examples
# =============================================================================

# 8. Batch processing with multiple configurations
# brain-tool --config=learning.cfg --action=run --steps=100
# brain-tool --config=advanced.cfg --action=save --output=batch1.chk
# brain-tool --config=advanced.cfg --action=load --input=batch1.chk
# brain-tool --action=status

# 9. Real-time monitoring
# brain-tool --action=run --steps=1000 --verbose | tee simulation.log
#
# You can then analyze the log file:
# grep "Firing neurons" simulation.log | awk '{sum+=$3} END {print "Average firing:", sum/NR}'

# =============================================================================
# Common Error Cases and Solutions
# =============================================================================

# "Error: No brain to save"
# Solution: Ensure brain is initialized before save action
# brain-tool --config=learning.cfg --action=save --output=brain.chk
#
# "Error: Failed to open checkpoint file"
# Solution: Check file permissions and path
# brain-tool --action=load --input=./nonexistent.chk
#
# "Configuration key not found"
# Solution: Set the key before using it
# brain-tool --set=neuron_count=2000 --action=run --steps=50

# =============================================================================
# Performance Monitoring Examples
# =============================================================================

# 10. Memory usage monitoring
# brain-tool --action=run --steps=100
# brain-tool --action=status | grep -i "working memory"
# brain-tool --action=save --output=memory_checkpoint.chk

# 11. Spike rate monitoring
# brain-tool --action=run --steps=1000 --verbose > spikes.log
# brain-tool --action=status | grep "Total spikes"

# =============================================================================
# Integration Examples
# =============================================================================

# 12. Automated experiment runner (example script):
# ```bash
#!/bin/bash

# Run multiple configurations
echo "Running learning configuration..."
brain-tool --config=learning.cfg --action=save --output=learn_start.chk
brain-tool --config=learning.cfg --action=run --steps=500
brain-tool --config=learning.cfg --action=save --output=learn_end.chk

echo "Running advanced configuration..."
brain-tool --config=advanced.cfg --action=save --output=advanced_start.chk
brain-tool --config=advanced.cfg --action=run --steps=500
brain-tool --config=advanced.cfg --action=save --output=advanced_end.chk

echo "All experiments completed!"
# ```

# 13. Real-time brain exploration:
# ```bash
# brain-tool --action=run --steps=1000 --verbose | while read line; do
#     echo "[$line]" >> live_monitor.log
#     # Process line in real-time
# done
# brain-tool --action=status >> final_status.log
# ```

# =============================================================================
# Configuration Validation Examples
# =============================================================================

# 14. Validate configuration before use
# brain-tool --config=learning.cfg --set=neuron_count=100 --action=run --steps=10
# brain-tool --set=stdp_ltp_weight=999.0 --action=save --output=test.chk
#
# The enhanced configuration system validates values and provides:
# - Type checking
# - Range validation
# - Source priority enforcement
# - Version compatibility checks