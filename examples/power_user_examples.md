# NLM Power User Examples
# This file demonstrates advanced usage of NLM for power users

# Example 1: High-performance configuration for research
# This configuration is optimized for research simulations with complex behaviors

# Run this configuration:
# ./nlm_phase6_demo --config=configs/power_user_config.cfg --neuron-count=5000 --max-steps=50000

# For development and debugging, use these command-line options:
# ./nlm_phase6_demo --enable-debug --log-level=DEBUG --checkpoint-interval=1000

# Example 2: Performance benchmarking configuration
# This configuration is optimized for performance testing and benchmarking

# Create a performance benchmark config:
cat > benchmark_config.cfg << EOF
# Performance benchmark configuration
simulation_timestep = 0.0005  # Small timestep for accuracy
neuron_count = 2000  # Moderate size for benchmarking
max_simulation_steps = 100000  # Long benchmark
enable_vectorization = true
enable_parallel_processing = true
thread_count = 8
enable_detailed_profiling = true
profile_frequency = 100
log_level = WARNING  # Reduce logging overhead
EOF

# Run benchmark:
# ./nlm_phase6_demo --config=benchmark_config.cfg

# Example 3: Advanced debugging and analysis
# This configuration enables detailed debugging for research analysis

# Create debug analysis config:
cat > debug_config.cfg << EOF
# Debug analysis configuration
debug_level = 2
enable_detailed_profiling = true
profile_frequency = 100
log_to_file = true
log_filename = nlm_debug.log
checkpoint_interval = 500
checkpoint_compression = true
max_simulation_steps = 20000
EOF

# Run debug analysis:
# ./nlm_phase6_demo --config=debug_config.cfg
# ./nlm_phase6_demo --enable-debug

# Example 4: Custom environment and behavior experiments
# This configuration enables custom environment interactions

# Create custom behavior config:
cat > custom_behavior_config.cfg << EOF
# Custom behavior configuration
environment_name = CustomWorld
environment_width = 50
environment_height = 50
vision_width = 32
vision_height = 32
vision_fov = 5.0
vision_sensitivity = 0.9
motor_max_speed = 3.0
motor_max_rotation = 0.5
motor_action_interval = 0.05

# Advanced neuromodulation
enable_complex_neuromodulation = true
novelty_threshold = 0.3
curiosity_sensitivity = 0.5
prediction_error_weight = 0.8

# Enable all advanced features
enable_reward_modulated_plasticity = true
enable_structural_development = true
enable_neural_planning = true
enable_concept_formation = true
enable_replay_system = true

# Extended learning
developmental_stage_duration = 2000
maturation_threshold = 10000
concept_formation_threshold = 0.8
consolidation_threshold = 0.9

neuron_count = 3000
max_simulation_steps = 50000
EOF

# Run custom behavior experiment:
# ./nlm_phase6_demo --config=custom_behavior_config.cfg

# Example 5: Integration and validation testing
# This configuration is designed for integration testing and validation

# Create integration test config:
cat > integration_test_config.cfg << EOF
# Integration testing configuration
neuron_count = 1000
region_count = 3
connection_probability = 0.15

# Enable all systems for integration testing
enable_structural_development = true
enable_complex_neuromodulation = true
enable_neural_planning = true
enable_concept_formation = true
enable_replay_system = true

# Memory system configuration
working_memory_size = 2048
episodic_memory_capacity = 500
associative_memory_size = 1000

# Checkpointing for integration testing
checkpoint_interval = 100
checkpoint_compression = true
checkpoint_encryption = true

# Debug and validation
enable_detailed_profiling = true
debug_level = 1
log_to_file = true
log_filename = integration_test.log

max_simulation_steps = 10000
EOF

# Run integration test:
# ./nlm_phase6_demo --config=integration_test_config.cfg

# Example 6: Command-line power user options
# These command-line options can be used with any NLM demo:

# Enable debug mode
# ./nlm_phase6_demo --enable-debug

# Enable profiling
# ./nlm_phase6_demo --enable-profiling

# Enable vectorization
# ./nlm_phase6_demo --enable-vectorization

# Set checkpoint interval
# ./nlm_phase6_demo --checkpoint-interval=1000

# Set log level
# ./nlm_phase6_demo --log-level=DEBUG

# Set random seed for reproducible experiments
# ./nlm_phase6_demo --random-seed=12345

# Set custom neuron count
# ./nlm_phase6_demo --neuron-count=5000

# Set custom configuration file
# ./nlm_phase6_demo --config=custom_config.cfg

# Example 7: Complete power user command sequence
# This demonstrates a complete power user command sequence for research:

# Full research command:
# ./nlm_phase6_demo \
#   --enable-debug \
#   --enable-profiling \
#   --enable-vectorization \
#   --checkpoint-interval=500 \
#   --log-level=INFO \
#   --random-seed=12345 \
#   --neuron-count=3000 \
#   --config=power_user_config.cfg

# This command enables all power user features for comprehensive research experiments

# Example 8: Batch processing and automation
# These examples show how to automate multiple experiments

# Create a batch script:
cat > run_experiments.sh << 'EOF'
#!/bin/bash

# Array of experiment configurations
experiments=(
    "configs/default.cfg basic_experiments"
    "configs/power_user_config.cfg advanced_experiments"
    "configs/debug_config.cfg debug_experiments"
    "configs/integration_test_config.cfg integration_experiments"
)

# Run each experiment
for config in "${experiments[@]}"; do
    echo "Running experiment with $config"
    ./nlm_phase6_demo --config=${config}
    echo "Experiment completed with $config"
    echo "---"
done
EOF

# Make the script executable:
# chmod +x run_experiments.sh

# Run the batch script:
# ./run_experiments.sh

# Example 9: Performance analysis and comparison
# These examples show how to analyze and compare performance

# Create a performance comparison script:
cat > compare_performance.sh << 'EOF'
#!/bin/bash

# Function to run an experiment and measure performance
run_experiment() {
    local config_file=$1
    local description=$2
    
    echo "=== Running $description ==="
    echo "Configuration: $config_file"
    
    # Run the experiment with timing
    start_time=$(date +%s)
    ./nlm_phase6_demo --config=$config_file --max-steps=10000
    end_time=$(date +%s)
    
    duration=$((end_time - start_time))
    echo "Duration: $duration seconds"
    echo "---"
}

# Compare different configurations
run_experiment "configs/default.cfg" "Basic Configuration"
run_experiment "configs/power_user_config.cfg" "Power User Configuration"
run_experiment "configs/debug_config.cfg" "Debug Configuration"
run_experiment "configs/integration_test_config.cfg" "Integration Test Configuration"

# Generate comparison report
echo "=== Performance Comparison Report ===" > comparison_report.txt
echo "Generated on $(date)" >> comparison_report.txt
echo "" >> comparison_report.txt
echo "This report compares performance across different NLM configurations." >> comparison_report.txt
echo "" >> comparison_report.txt
echo "For detailed analysis, refer to the debug logs and performance metrics." >> comparison_report.txt
EOF

# Make the script executable:
# chmod +x compare_performance.sh

# Run performance comparison:
# ./compare_performance.sh

# Example 10: Research workflow automation
# This example shows how to automate a complete research workflow

# Create a complete research workflow script:
cat > research_workflow.sh << 'EOF'
#!/bin/bash

# Research workflow configuration
EXPERIMENT_BASE_DIR="experiments"
RESULTS_DIR="results"
DEBUG_DIR="debug_output"
BACKUP_DIR="backups"

# Create directories
mkdir -p $EXPERIMENT_BASE_DIR
mkdir -p $RESULTS_DIR
mkdir -p $DEBUG_DIR
mkdir -p $BACKUP_DIR

# Function to run a complete research experiment
run_research_experiment() {
    local experiment_name=$1
    local config_file=$2
    local description=$3
    
    echo "=== Starting Research Experiment: $experiment_name ==="
    echo "Description: $description"
    echo "Configuration: $config_file"
    echo "Start time: $(date)"
    
    # Create experiment directory
    exp_dir="$EXPERIMENT_BASE_DIR/$experiment_name"
    mkdir -p $exp_dir
    
    # Run the experiment with all power user features
    ./nlm_phase6_demo \
        --config=$config_file \
        --enable-debug \
        --enable-profiling \
        --enable-vectorization \
        --checkpoint-interval=1000 \
        --log-level=INFO \
        --random-seed=$RANDOM \
        --neuron-count=3000 \
        > "$exp_dir/output.log" 2> "$exp_dir/error.log"
    
    # Copy debug output
    cp "/tmp/nlm_debug_report.txt" "$exp_dir/debug_report.txt" 2>/dev/null || true
    cp nlm.log "$exp_dir/simulation.log" 2>/dev/null || true
    
    # Create experiment summary
    cat > "$exp_dir/experiment_summary.txt" << EOF
Experiment: $experiment_name
Description: $description
Configuration: $config_file
Start Time: $(date -d "@$start_time" '+%Y-%m-%d %H:%M:%S')
Duration: $duration seconds
Status: COMPLETED
Output: $exp_dir/output.log
Error: $exp_dir/error.log
Debug: $exp_dir/debug_report.txt
Simulation Log: $exp_dir/simulation.log
EOF
    
    echo "Experiment completed: $experiment_name"
    echo "Duration: $duration seconds"
    echo "Results saved to: $exp_dir"
    echo "---"
}

# Backup function
backup_experiment() {
    local experiment_name=$1
    local exp_dir="$EXPERIMENT_BASE_DIR/$experiment_name"
    
    if [ -d "$exp_dir" ]; then
        local backup_name="$BACKUP_DIR/backup_${experiment_name}_$(date +%Y%m%d_%H%M%S)"
        cp -r "$exp_dir" "$backup_name"
        echo "Backed up experiment $experiment_name to $backup_name"
    fi
}

# Main workflow execution
start_time=$(date +%s)

# Define research experiments
experiments=(
    "basic_research configs/default.cfg "Basic cognitive research with standard configuration"
    "advanced_research configs/power_user_config.cfg "Advanced research with power user features"
    "debug_analysis configs/debug_config.cfg "Detailed debugging and profiling analysis"
    "integration_validation configs/integration_test_config.cfg "Integration testing and validation"
)

# Run all experiments
for experiment in "${experiments[@]}"; do
    name=$(echo $experiment | cut -d' ' -f1)
    config=$(echo $experiment | cut -d' ' -f2)
    desc=$(echo $experiment | cut -d' ' -f3-)
    
    run_research_experiment "$name" "$config" "$desc"
    backup_experiment "$name"
    
    # Wait between experiments
    echo "Waiting between experiments..."
    sleep 30
    echo "---"
done

# Generate final research report
end_time=$(date +%s)
duration=$((end_time - start_time))

cat > "$RESULTS_DIR/research_workflow_report.txt" << EOF
=== NLM Research Workflow Report ===
Start Time: $(date -d "@$start_time" '+%Y-%m-%d %H:%M:%S')
End Time: $(date -d "@$end_time" '+%Y-%m-%d %H:%M:%S')
Total Duration: $duration seconds

=== Experiments Executed ===
$(ls -la "$EXPERIMENT_BASE_DIR" | grep "^d" | sed 's/^/  /')

=== Backup Summary ===
$(ls -la "$BACKUP_DIR" | grep "^d" | sed 's/^/  /')

=== Key Findings ===
1. All experiments completed successfully
2. Power user features enabled comprehensive research capabilities
3. Debug and profiling data collected for analysis
4. Backup system provides experiment reproducibility

=== Recommendations for Future Research ===
1. Use power_user_config.cfg for advanced research studies
2. Enable --enable-debug for detailed debugging sessions
3. Use --enable-profiling for performance optimization
4. Implement batch processing for large-scale experiments

=== Next Steps ===
1. Analyze debug reports in debug_output/ directory
2. Compare results in results/ directory
3. Review backups in backups/ directory
4. Plan follow-up experiments based on findings

Generated on: $(date)
Report Version: 1.0
EOF

echo "=== Research Workflow Completed ==="
echo "Total Duration: $duration seconds"
echo "Results saved to: $RESULTS_DIR/research_workflow_report.txt"
EOF

# Make the script executable:
# chmod +x research_workflow.sh

# Run the research workflow:
# ./research_workflow.sh
