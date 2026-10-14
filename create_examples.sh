#!/bin/bash
# Example: Basic NLM Usage

echo "=== NLM Basic Usage Example ==="
echo "This script demonstrates how to use NLM Phase 6 with different configurations."
echo ""

# Create a temporary directory for examples
mkdir -p examples/basic

cat > examples/basic/README.md << 'EOF'
# NLM Basic Usage Examples

This directory contains examples demonstrating various usage patterns for NLM Phase 6.

## Basic Configuration Example

Run NLM with default settings:
```bash
./nlm
```

This will run a comprehensive integration test with default configuration.

## Custom Configuration Example

Create a custom configuration file:

```bash
# In a new file: custom.cfg
cat > custom.cfg << EOF
# Custom settings for a specific experiment
neuron_count = 500
region_count = 3
connection_probability = 0.15
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.035
reward_prediction_error_weight = 0.2
working_memory_capacity = 150
EOF

# Run with custom configuration
./nlm --config custom.cfg
```

## Performance Configuration Example

Create a high-performance configuration:

```bash
cat > perf.cfg << EOF
# Performance-optimized settings
spike_queue_size = 20000
neuron_processing_batch_size = 200
plasticity_update_frequency = 0.9
working_memory_capacity = 200
episodic_memory_max_episodes = 2000
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.03
EOF

# Run with performance configuration
./nlm --config perf.cfg
```

## Neuromodulation Example

Configure for neuromodulation experiments:

```bash
cat > neuromod.cfg << EOF
# Neuromodulation settings
dopamine_baseline = 0.05
curiosity_baseline = 0.02
novelty_threshold = 0.4
reward_prediction_error_weight = 0.25
reward_modulation_tau = 150.0

# Memory with neuromodulation
working_memory_capacity = 180
episodic_memory_max_episodes = 1500

# Triplet STDP for complex learning
pre_trace_time_constant = 25.0
post_trace_time_constant = 25.0
EOF

# Run neuromodulation experiment
./nlm --config neuromod.cfg --log-file neuromod_experiment.log
```

## Checkpoint Example

Save and load checkpoints:

```bash
# Run for a while to generate checkpoints
./nlm --config perf.cfg --save-interval 500 --log-file run.log

# Load checkpoint after 1000 steps
./nlm --config perf.cfg --load-checkpoint checkpoints/snapshot_1000.bin
```

## Help and Documentation

View help information:
```bash
./nlm --help
```

View version information:
```bash
./nlm --version
```

## Configuration File Reference

See README.md in the main directory for a complete list of configuration options.

## Troubleshooting

If you encounter issues:

1. Check the log file for errors:
   ```bash
   tail -f nlm.log
   ```

2. Run with default settings to isolate issues:
   ```bash
   ./nlm
   ```

3. Check available memory:
   ```bash
   free -h
   ```

4. Check disk space:
   ```bash
   df -h .
   ```

## Performance Tips

- For larger networks, use the performance configuration
- Monitor memory usage with: `--monitor-memory`
- Enable visualization with: `--enable-visualization`
- Save checkpoints regularly to avoid data loss

EOF

echo "Basic examples created in examples/basic/"
echo ""
echo "=== Example Usage Complete ==="
