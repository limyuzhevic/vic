#!/usr/bin/env bash
# Advanced NLM Configuration Examples
# Examples of the new JSON/YAML configuration system and CLI options

echo "=== NLM Configuration Examples ==="
echo ""
echo "This script shows examples of the new configuration system"
echo "and advanced CLI options introduced in the NLM improvements."
echo ""

# Create examples directory
mkdir -p examples

# Example 1: JSON Configuration
echo "1. JSON Configuration (recommended)"
cat > examples/config.json << 'EOF'
{
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "random_seed": 12345,
    "region_count": 2,
    "connection_probability": 0.15,
    
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    
    "hebbian_learning_rate": 0.01,
    "plasticity_modulation": 1.0,
    
    "dopamine_level": 0.5,
    "curiosity_factor": 0.3,
    "novelty_threshold": 0.2,
    
    "working_memory_capacity": 100,
    "episodic_memory_max_episodes": 1000,
    "prediction_horizon": 10,
    
    "checkpoint_dir": "./checkpoints",
    "replay_interval": 100,
    "consolidation_interval": 1000
}
EOF

echo "   File: examples/config.json"
echo "   Description: JSON configuration with all available options"
echo ""

# Example 2: YAML Configuration
echo "2. YAML Configuration (alternative format)"
cat > examples/config.yaml << 'EOF'
neuron-count: 1000
simulation-timestep: 0.001
random-seed: 12345
region-count: 2
connection-probability: 0.15

stdp-ltp-weight: 0.02
stdp-ltd-weight: 0.015
stdp-tau: 20.0

hebbian-learning-rate: 0.01
plasticity-modulation: 1.0

dopamine-level: 0.5
curiosity-factor: 0.3
novelty-threshold: 0.2

working-memory-capacity: 100
episodic-memory-max-episodes: 1000
prediction-horizon: 10

checkpoint-dir: ./checkpoints
replay-interval: 100
consolidation-interval: 1000
EOF

echo "   File: examples/config.yaml"
echo "   Description: YAML configuration with kebab-case keys"
echo ""

# Example 3: Command line with advanced options
echo "3. Advanced CLI Options"
echo ""
echo "# Run with custom configuration and advanced options"
echo "./build/nlm phase6 \\"
echo "  --config examples/config.json \\"
echo "  --stdp-weight-factors 0.02 0.015 \\"
echo "  --hebbian-learning-rate 0.05 \\"
echo "  --plasticity-mod 1.2 \\"
echo "  --dopamine-level 0.7 \\"
echo "  --curiosity-factor 0.4 \\"
echo "  --verbose \\"
echo "  --save-brain"
echo ""

# Example 4: Performance benchmarking
echo "4. Performance Benchmarking"
echo ""
echo "# Run benchmark with performance profiling"
echo "./build/nlm benchmark \\"
echo "  --neuron-count 2000 \\"
echo "  --timestep 0.0005 \\"
echo "  --profile \\"
echo "  --output benchmark_results.json"
echo ""

# Example 5: Machine learning hyperparameter optimization
echo "5. Hyperparameter Optimization"
echo ""
echo "# Test different STDP parameters"
echo "./build/nlm phase4 \\"
echo "  --stdp-weight-factors 0.015 0.012 \\"
echo "  --hebbian-learning-rate 0.008 \\"
echo "  --plasticity-mod 0.8 \\"
echo "  --curiosity-factor 0.2"
echo ""

# Example 6: Learning with saved brain state
echo "6. Resume Learning from Saved State"
echo ""
echo "# Load a saved brain and continue learning"
echo "./build/nlm phase3 \\"
echo "  --load-brain saved_brain.bin \\"
echo "  --random-seed 12345 \\"
echo "  --novelty-threshold 0.15 \\"
echo "  --curiosity-factor 0.25"
echo ""

# Example 7: Real-time monitoring
echo "7. Real-time Monitoring and Debugging"
echo ""
echo "# Run with debug mode for detailed analysis"
echo "./build/nlm phase6 \\"
echo "  --debug \\"
echo "  --verbose \\"
echo "  --output debug_log.txt"
echo ""

echo "=== Configuration Format Examples ==="
echo ""
echo "JSON keys use snake_case (recommended):"
echo "  { \"neuron_count\": 1000, \"simulation_timestep\": 0.001 }"
echo ""
echo "YAML keys use kebab-case (alternative):"
echo "  neuron-count: 1000"
echo "  simulation-timestep: 0.001"
echo ""
echo "Command line syntax:"
echo "  --option value (equals sign)"
echo "  --option value (space separated)"
echo ""

echo "=== Available Configuration Options ==="
echo ""
echo "Core Settings:"
echo "  neuron-count, simulation-timestep, random-seed, region-count"
echo "  connection-probability, checkpoint-dir"
echo ""
echo "Plasticity Rules:"
echo "  stdp-ltp-weight, stdp-ltd-weight, stdp-tau"
echo "  hebbian-learning-rate, plasticity-modulation"
echo ""
echo "Neuromodulation:"
echo "  dopamine-level, curiosity-factor, novelty-threshold"
echo "  reward-function, action-selection-policy"
echo ""
echo "Memory Systems:"
echo "  working-memory-capacity, episodic-memory-max-episodes"
echo "  prediction-horizon, replay-interval"
echo ""
echo "=== Demo Types ==="
echo ""
echo "phase2    Phase 2: Real Neural Computation (LIF neurons, STDP)"
echo "phase3    Phase 3: World Interaction (sensory/motor)"
echo "phase4    Phase 4: Cognitive Integration (memory, prediction)"
echo "phase6    Phase 6: Full Integration (all systems)"
echo "benchmark  Performance benchmarking and profiling"
echo ""

echo "=== Quick Usage Examples ==="
echo ""
echo "# Quick demo with minimal output"
echo "./build/nlm phase2 --quiet --duration 1.0"
echo ""
echo "# Advanced demo with full monitoring"
echo "./build/nlm phase6 --debug --profile --verbose"
echo ""
echo "# Save brain for later use"
echo "./build/nlm phase4 --save-brain --output brain_state.bin"
echo ""
echo "# Load and continue learning"
echo "./build/nlm phase3 --load-brain brain_state.bin --random-seed 123"
echo ""

echo "=== Performance Tips ==="
echo ""
echo "For faster demos:"
echo "  --neuron-count 500 (default)"
echo "  --timestep 0.001 (default)"
echo "  --quiet (reduce output)"
echo ""
echo "For detailed analysis:"
echo "  --verbose (show progress)"
echo "  --debug (debug mode)"
echo "  --profile (performance metrics)"
echo ""
echo "=== Error Handling ==="
echo ""
echo "The improved system provides:"
echo "- Better error messages for invalid configuration"
echo "- Graceful handling of missing files"
echo "- Validation of numeric ranges"
echo "- Clear help output with examples"
echo ""
echo "=== Get Help ==="
echo ""
echo "./build/nlm --help                    # Show all options"
echo "./build/nlm --version                # Version information"
echo ""
echo "=== File Locations ==="
echo ""
echo "- Binaries: ./build/nlm"
echo "- Examples: examples/"
echo "- Checkpoints: ./checkpoints/"
echo "- Logs: ./logs/"
echo ""
echo "Happy neural simulation!"
