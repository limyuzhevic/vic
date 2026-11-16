# Advanced Usage Examples

## Using nlm-config for Configuration Management

### Create a Complete Brain Configuration

This example shows how to create a comprehensive brain configuration using the nlm-config tool:

```bash
# Create a configuration file for a learning experiment
cat > learning_experiment.nml << EOF
brain:
  name: "LearningExperiment"
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.15
  simulation:
    timestep: 0.001
    max_steps: 5000
    warmup_steps: 500
  learning:
    enable_stdp: true
    enable_hebbian: true
    enable_reward: true
    stdp_ltp_weight: 0.02
    stdp_ltd_weight: 0.015
    hebbian_strength: 0.1
  neuromodulation:
    enable_curiosity: true
    enable_novelty: true
    dopamine_scale: 1.0
  development:
    enable: true
    critical_period: 1000
    maturation_rate: 0.01
  memory:
    enable_working: true
    enable_episodic: true
    enable_associative: true
    memory_window: 100
  environment:
    width: 20
    height: 20
    vision_width: 8
    vision_height: 8
EOF

# Load the configuration
nlm-config load learning_experiment.nml

# List all configuration parameters
nlm-config list

# Get specific values
nlm-config get brain.neuron_count
nlm-config get simulation.max_steps
nlm-config get learning.enable_stdp
```

### Configuration Templates

The following templates can be used as starting points:

#### Simple Brain Template
```bash
nlm-config create --brain.name="SimpleBrain" --brain.neuron_count=100 \
    --brain.region_count=1 --simulation.timestep=0.001 \
    --simulation.max_steps=1000 --learning.enable_stdp=true \
    --learning.enable_hebbian=false --output simple_brain.nml
```

#### Learning Template
```bash
nlm-config create --brain.name="LearningBrain" --brain.neuron_count=500 \
    --brain.region_count=1 --simulation.timestep=0.001 \
    --simulation.max_steps=5000 --learning.enable_stdp=true \
    --learning.enable_hebbian=true --learning.enable_reward=true \
    --neuromodulation.enable_curiosity=true --neuromodulation.enable_novelty=true \
    --development.enable=true --output learning_brain.nml
```

## Using nlm-batch for Automation

### Batch Processing Setup

This example shows how to set up a comprehensive batch experiment:

```bash
# Create a batch configuration file
cat > experiment_scenarios.txt << EOF
scenario_name,config_file,output_dir,parallel,steps,neurons
cold_start,cold_start_config.nml,cold_start_results,4,1000,500
cold_start_warm,cold_start_warm_config.nml,cold_start_warm_results,4,2000,500
warm_start,warm_start_config.nml,warm_start_results,4,3000,500
hybrid,h0,h0_results,2,4000,500
EOF

# Preview batch operations
nlm-batch --config experiment_scenarios.txt --preview

# Run batch processing
nlm-batch --config experiment_scenarios.txt --output batch_results \
    --parallel 4 --dry-run

# Execute actual batch processing
nlm-batch --config experiment_scenarios.txt --output batch_results \
    --parallel 4 --verbose
```

### Monitoring Batch Progress

To monitor your batch experiments:

```bash
# Monitor running processes
watch -n 10 "ps aux | grep nlm-batch | grep -v grep"

# Check results progress
nlm-batch --config experiment_scenarios.txt --status

# Resume failed experiments
nlm-batch --config experiment_scenarios.txt --resume

# Cancel specific experiments
nlm-batch --config experiment_scenarios.txt --cancel experiment1
```

## Using nlm-analysis for Results Analysis

### Basic Analysis

```bash
# Analyze simulation results
nlm-analysis --input batch_results/cold_start_results/ \
    --report cold_start_analysis.txt --format:detailed

# Extract specific metrics
nlm-analysis --input batch_results/cold_start_results/ \
    --metrics:learning_rate,curiosity_level,dopamine_level \
    --threshold:0.01 --output metrics_report.csv

# Generate comprehensive report
nlm-analysis --input batch_results/ --report comprehensive_report.html \
    --format:html --template:detailed --include:charts,graphs,stats
```

### Comparative Analysis

```bash
# Compare multiple experiments
nlm-analysis --compare batch_results/cold_start_results/ \
    batch_results/warm_start_results/ --output comparison_report.txt

# Statistical analysis
nlm-analysis --input batch_results/ --statistical \
    --confidence:0.95 --output statistical_analysis.txt

# Trend analysis
nlm-analysis --input batch_results/ --trend analysis_trends.txt \
    --window:100 --metrics:firing_rate,learning_progress
```

## Using nlm-checkpoint for State Management

### Checkpoint Operations

```bash
# Save brain state at specific simulation steps
nlm-checkpoint save experiment1/checkpoints/checkpoint_1000.bin \
    --input experiment1/sim_state_1000 --compress

nlm-checkpoint save experiment1/checkpoints/checkpoint_5000.bin \
    --input experiment1/sim_state_5000 --validate

# List and manage checkpoints
nlm-checkpoint list experiment1/checkpoints/

# Get checkpoint information
nlm-checkpoint info experiment1/checkpoints/checkpoint_1000.bin

# Restore from checkpoint
nlm-checkpoint load experiment1/checkpoints/checkpoint_1000.bin \
    --output restored_brain_state

# Delete old checkpoints
nlm-checkpoint delete experiment1/checkpoints/ --older-than 24h

# Create checkpoint snapshots
nlm-checkpoint snapshot experiment1/ --interval:1000 --max:50
```

### Checkpoint Validation

```bash
# Validate checkpoint integrity
nlm-checkpoint validate experiment1/checkpoints/checkpoint_1000.bin

# Test checkpoint restoration
nlm-checkpoint restore test_checkpoint.bin --test

# Compare checkpoints
nlm-checkpoint compare experiment1/checkpoints/checkpoint_1000.bin \
    experiment1/checkpoints/checkpoint_1001.bin --diff output.diff
```

## Using nlm-profile for Performance Optimization

### Profiling Experiments

```bash
# Profile single experiment
nlm-profile --config experiment_config.nml --output profile_1.json \
    --detailed --memusage --cpu

# Profile multiple experiments
nlm-profile --config configs/*.nml --output profiles/ --compare

# Performance comparison
nlm-profile --compare batch_results/experiment1/ batch_results/experiment2/ \
    --output performance_comparison.txt
```

### Optimization Recommendations

```bash
# Get optimization suggestions
nlm-profile --config experiment_config.nml --optimization-suggestions

# Identify performance bottlenecks
nlm-profile --config experiment_config.nml --bottleneck-analysis

# Generate optimization report
nlm-profile --config experiment_config.nml --output optimization_report.md
```

## Complete Workflow Example

This example demonstrates a complete workflow using all advanced tools:

```bash
# 1. Set up configuration
cat > complete_config.nml << EOF
brain:
  name: "CompleteExperiment"
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.15
  simulation:
    timestep: 0.001
    max_steps: 10000
  learning:
    enable_stdp: true
    enable_hebbian: true
EOF

# 2. Run the experiment
nlm-batch --config complete_config.nml --output complete_results \
    --parallel 4 --verbose

# 3. Save checkpoints
nlm-checkpoint snapshot complete_results/ --interval:1000 --max:20

# 4. Analyze results
nlm-analysis --input complete_results/ --report complete_analysis.html

# 5. Profile performance
nlm-profile --config complete_config.nml --output complete_profile.json

# rdsl-checkpoint compare complete_results/checkpoints/checkpoint_1000.bin \
    complete_results/checkpoints/checkpoint_2000.bin

# 7. Generate comprehensive report
nlm-analysis --compare complete_results/checkpoints/checkpoint_1000.bin \
    complete_results/checkpoints/checkpoint_2000.bin --output final_report.txt

# 8. Create visualization
nlm-visualize --input complete_results/checkpoints/checkpoint_2000.bin \
    --output experiment_visualization.html --type:comprehensive
```

## Error Handling and Troubleshooting

### Common Issues and Solutions

**Configuration Errors**
```bash
# Invalid configuration file
nlm-config load invalid_config.nml --fix

# Missing required parameters
nlm-config create --brain.name="Test" --brain.neuron_count=500 \
    --simulation.timestep=0.001 --output valid_config.nml
```

**Batch Processing Errors**
```bash
# Failed simulation
nlm-batch --config config.txt --status
nlm-batch --config config.txt --resume

# Disk space issues
nlm-batch --config config.txt --max-disk-space 2GB
```

**Analysis Errors**
```bash
# Invalid input directory
nlm-analysis --input invalid_dir/ --auto-fix

# Missing metrics
nlm-analysis --input results/ --add-metric learning_rate
```

**Performance Profiling Errors**
```bash
# Insufficient permissions
nlm-profile --config config.txt --run-as-root

# Missing profiling tools
nlm-profile --config config.txt --install-tools
```

## Advanced Tool Scripts

The following shell scripts demonstrate advanced usage patterns:

### create_experiment.sh
```bash
#!/bin/bash
# Script to create a complete experiment configuration

EXPERIMENT_NAME="${1}"
NEURON_COUNT="${2:-1000}"
MAX_STEPS="${3:-5000}"

# Create experiment directory
mkdir -p experiments/${EXPERIMENT_NAME}

# Generate configuration
nlm-config create --brain.name="${EXPERIMENT_NAME}" \
    --brain.neuron_count=${NEURON_COUNT} \
    --brain.region_count=1 --simulation.timestep=0.001 \
    --simulation.max_steps=${MAX_STEPS} \
    --learning.enable_stdp=true --learning.enable_hebbian=true \
    --output experiments/${EXPERIMENT_NAME}/config.nml

# Create batch file
nml-batch create batch_experiments.txt \
    experiments/${EXPERIMENT_NAME}/config.nml \
    experiments/${EXPERIMENT_NAME}/results

# Run experiment
nlm-batch --config batch_experiments.txt --output results \
    --parallel 4

# Save checkpoints
nlm-checkpoint snapshot experiments/${EXPERIMENT_NAME}/results \
    --interval:1000 --max:20

# Analyze results
nlm-analysis --input experiments/${EXPERIMENT_NAME}/results \
    --report experiments/${EXPERIMENT_NAME}/analysis.html

echo "Experiment ${EXPERIMENT_NAME} completed successfully!"
```

### analyze_batch.sh
```bash
#!/bin/bash
# Script to analyze a batch of experiments

BATCH_DIR="${1}"
OUTPUT_FILE="${2:-batch_analysis.txt}"

# Compare all experiments in batch
nlm-analysis --input ${BATCH_DIR}/ --compare \
    --output ${OUTPUT_FILE}

# Generate statistical analysis
nlm-analysis --input ${BATCH_DIR}/ --statistical \
    --confidence:0.95 --output ${BATCH_DIR}/stats.txt

# Create performance comparison
nlm-profile --compare ${BATCH_DIR}/*/ --output ${BATCH_DIR}/performance.txt

echo "Batch analysis completed: ${OUTPUT_FILE}"
```

## Conclusion

The advanced NLM tools provide a comprehensive set of capabilities for:

- **Configuration Management**: Creating and managing complex brain configurations
- **Batch Processing**: Automating multiple simulation runs
- **Results Analysis**: Comprehensive analysis of simulation outcomes
- **State Management**: Saving and loading brain states efficiently
- **Performance Optimization**: Profiling and optimizing simulation performance

These tools make it easy to run large-scale experiments, analyze results systematically, and optimize brain configurations for specific tasks.

By mastering these advanced tools, you can:

- Run complex experiments with minimal manual intervention
- Analyze results in multiple formats and detail levels
- Optimize brain configurations for better performance
- Maintain reproducible experiments with checkpointing
- Compare different approaches and configurations systematically

The tools are designed to work together seamlessly, providing a complete workflow from configuration through analysis to optimization.
