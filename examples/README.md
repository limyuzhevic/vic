# NLM Enhanced System - Examples

This directory contains example configurations and scripts for using the Enhanced NLM system.

## Examples Overview

This folder provides practical examples for getting started with the Enhanced NLM system, from basic demonstrations to advanced configurations.

## Example 1: Basic Demo Configuration

**File:** `examples/basic_demo.cfg`

```bash
# Basic NLM Agent Demo Configuration
mode=demo
world-width=20
world-height=20
vision-width=16
vision-height=16
brain-regions=2
neurons-per-region=100
episodes=10
steps-per-episode=100
enable-dev=true
enable-curiosity=true
enable-reward=true
log-level=info
```

**Usage:**
```bash
nlm_agent --load-config examples/basic_demo.cfg
```

**What it demonstrates:**
- Basic agent exploration in a small world
- Simple neural network with 2 regions
- Memory system initialization
- Basic curiosity-driven exploration

## Example 2: Advanced Training Configuration

**File:** `examples/advanced_train.cfg`

```bash
# Advanced Training Configuration
mode=train
world-width=30
world-height=30
vision-width=20
vision-height=20
brain-regions=5
neurons-per-region=500
episodes=1000
steps-per-episode=2000
memory-capacity=1000
memory-decay=0.005
enable-dev=true
enable-curiosity=true
enable-reward=true
enable-structural-plasticity=true
log-level=debug
checkpoint-interval=100
visualization=true
output-format=html
output-file=training_dashboard.html
```

**Usage:**
```bash
nlm_agent --load-config examples/advanced_train.cfg
```

**What it demonstrates:**
- Complex neural network with 5 regions and 2500 total neurons
- Advanced memory management with custom capacity and decay
- Full learning system with all neuromodulators enabled
- Automated checkpoint saving and visualization output
- Detailed debugging information

## Example 3: Evaluation Configuration

**File:** `examples/eval_checkpoint.cfg`

```bash
# Evaluation Configuration for Trained Agent
mode=eval
load=trained_brain_phase6.dat
world-width=20
world-height=20
vision-width=16
vision-height=16
brain-regions=3
neurons-per-region=200
episodes=50
steps-per-episode=1000
enable-dev=false
enable-curiosity=false
enable-reward=false
log-level=info
performance-monitoring=true
metrics-file=evaluation_metrics.json
```

**Usage:**
```bash
# First train an agent
nlm_agent --mode train --episodes 1000 --steps-per-episode 2000

# Then evaluate it
nlm_agent --load-config examples/eval_checkpoint.cfg
```

**What it demonstrates:**
- Performance evaluation of trained agent
- No learning during evaluation (pure performance testing)
- Metrics collection and analysis
- Checkpoint loading and restoration

## Example 4: Debug Configuration

**File:** `examples/debug_session.cfg`

```bash
# Debug Session Configuration
mode=debug
world-width=15
world-height=15
vision-width=12
vision-height=12
brain-regions=1
neurons-per-region=50
episodes=5
steps-per-episode=50
enable-dev=true
enable-curiosity=true
enable-reward=true
log-level=trace
visualization=true
real-time-monitoring=true
viz-update-interval=500
memory-debug=true
neural-debug=true
behavior-debug=true
performance-debug=true
```

**Usage:**
```bash
nlm_agent --load-config examples/debug_session.cfg
```

**What it demonstrates:**
- Maximum debugging information
- Real-time monitoring and visualization
- Detailed memory and neural system analysis
- Performance profiling and optimization
- Comprehensive error tracking

## Example 5: Production Configuration

**File:** `examples/production.cfg`

```bash
# Production Configuration - Optimized for Performance
mode=train
world-width=50
world-height=50
vision-width=32
vision-height=32
brain-regions=8
neurons-per-region=1000
episodes=5000
steps-per-episode=5000
memory-capacity=5000
memory-decay=0.001
enable-dev=true
enable-curiosity=true
enable-reward=true
enable-structural-plasticropy=true
enable-meta-learning=true
log-level=warn
checkpoint-interval=1000
checkpoint-file=production_brain
visualization=false
performance-monitoring=true
metrics-interval=100
```

**Usage:**
```bash
nlm_agent --load-config examples/production.cfg
```

**What it demonstrates:**
- Large-scale production system
- Optimized for performance and stability
- Automated checkpointing and recovery
- Performance monitoring and metrics collection
- Minimal logging for production efficiency

## Example Scripts

### Script 1: Quick Setup Script (`scripts/quick_setup.sh`)

```bash
#!/bin/bash
# Quick setup script for NLM Enhanced system

set -e

echo "Setting up NLM Enhanced System..."

echo "Creating build directory..."
mkdir -p build
cd build

echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native" \
    -DNLM_ENABLE_VISUALIZATION=ON \
    -DNLM_ENABLE_TESTS=ON

echo "Building system..."
make -j$(nproc)

echo "Setup complete!"
echo "You can now run: ./nlm_agent --mode demo"
```

### Script 2: Training Script (`scripts/train_agent.sh`)

```bash
#!/bin/bash
# Train NLM agent with advanced configuration

MODEL_DIR="models"
CONFIG_FILE="examples/advanced_train.cfg"
CHECKPOINT_INTERVAL=500

mkdir -p $MODEL_DIR

echo "Starting training with configuration: $CONFIG_FILE"
echo "Checkpoints will be saved to: $MODEL_DIR"

echo "nlm_agent --load-config $CONFIG_FILE --checkpoint-interval $CHECKPOINT_INTERVAL"
nlm_agent --load-config $CONFIG_FILE --checkpoint-interval $CHECKPOINT_INTERVAL

echo "Training completed!"
echo "Final model saved to: $MODEL_DIR/"
```

### Script 3: Evaluation Script (`scripts/evaluate_agent.sh`)

```bash
#!/bash
# Evaluate trained NLM agent

MODEL_DIR="models"
EVAL_CONFIG="examples/eval_checkpoint.cfg"

if [ ! -d "$MODEL_DIR" ] || [ $(ls -A $MODEL_DIR) ]; then
    echo "Warning: No pre-trained models found in $MODEL_DIR"
    echo "Run the training script first to create models."
    exit 1
fi

echo "Evaluating agent with configuration: $EVAL_CONFIG"
echo "Available models:"
ls -la $MODEL_DIR/

echo "Enter model filename (without path):"
read model_file

if [ -f "$MODEL_DIR/$model_file" ]; then
    echo "Running evaluation with model: $model_file"
    nlm_agent --mode eval --load "$MODEL_DIR/$model_file" \
              --world-width 20 --world-height 20 \
              --brain-regions 3 --neurons-per-region 200 \
              --episodes 100 --steps-per-episode 1000 \
              --metrics-file eval_results.json
else
    echo "Error: Model file not found: $model_file"
    exit 1
fi

echo "Evaluation completed! Results saved to eval_results.json"
```

### Script 4: Visualization Script (`scripts/generate_visualization.sh`)

```bash
#!/bin/bash
# Generate visualization from existing data

DATA_FILE="neural_data.json"
HTML_OUTPUT="neural_dashboard.html"

if [ ! -f "$DATA_FILE" ]; then
    echo "Error: Neural data file not found: $DATA_FILE"
    echo "Run a training session first to generate data."
    exit 1
fi

echo "Generating visualization from: $DATA_FILE"
echo "Output will be: $HTML_OUTPUT"

nlm_agent --visualize-json "$DATA_FILE" "$HTML_OUTPUT"

echo "Visualization generated successfully!"
echo "Open $HTML_OUTPUT in your browser to view the dashboard."
```

## Example Output Files

### Neural Activity Data (`examples/sample_neural_data.json`)

Sample neural activity data for visualization testing:

```json
{
  "timestamp": 1234567890,
  "neural_activities": [0.1, 0.5, 0.9, 0.3, 0.7, ...],
  "synaptic_weights": [0.2, 0.4, 0.6, 0.8, ...],
  "neuromodulation_levels": [0.1, 0.05, 0.15, ...]
}
```

### Memory Statistics (`examples/memory_stats.txt`)

Sample memory system statistics:

```
=== Working Memory Statistics ===
Current Size: 450/1000
Utilization: 45.0%
Decay Rate: 0.01
Access Patterns: High recency bias

=== Episodic Memory Statistics ===
Total Episodes: 127
Recent Episodes (10): 10
Pattern Completion Rate: 78.5%

=== Semantic Memory Statistics ===
Total Facts: 89
Concept Clusters: 12
Unique Concepts: 23

=== Procedural Memory Statistics ===
Total Skills: 15
Average Proficiency: 0.72
Most Used Skills: move_forward (0.95), turn_left (0.87)

=== Associative Memory Statistics ===
Total Associations: 342
Average Strength: 0.64
Strongest Associations: 0.92
```

### Performance Metrics (`examples/performance_metrics.json`)

Sample performance metrics:

```json
{
  "total_reward": 1500.5,
  "average_step_reward": 0.75,
  "total_steps": 2000,
  "successful_actions": 1800,
  "exploration_rate": 0.15,
  "learning_rate": 0.08,
  "final_memory_usage": 0.82,
  "final_skill_proficiency": 0.91,
  "time_per_step": 0.025
}
```

## How to Use Examples

### 1. Run Basic Examples
```bash
# Run basic demo
./nlm_agent --mode demo --world-width 20 --world-height 20

# Run advanced training
./nlm_agent --mode train --episodes 100 --steps-per-episode 1000
```

### 2. Use Configuration Files
```bash
# Load example configuration
./nlm_agent --load-config examples/basic_demo.cfg
```

### 3. Run Scripts
```bash
# Make scripts executable
chmod +x scripts/*.sh

# Run training script
./scripts/train_agent.sh

# Run evaluation script
./scripts/evaluate_agent.sh
```

### 4. Analyze Generated Output
```bash
# View neural visualization
open examples/neural_dashboard.html

# Check memory statistics
less examples/memory_stats.txt

# Review performance metrics
cat examples/performance_metrics.json
```

## Best Practices

### 1. Configuration Management
- Use configuration files for consistent setups
- Version control your configuration files
- Document custom configurations

### 2. Data Management
- Regularly back up model checkpoints
- Organize output files systematically
- Use descriptive file names

### 3. Performance Optimization
- Profile your system regularly
- Monitor memory usage
- Optimize configuration for your hardware

### 4. Testing and Validation
- Run unit tests for new configurations
- Validate against expected behavior
- Document test results

## Troubleshooting Examples

### Problem: Configuration Not Loading
**Solution:**
```bash
# Check if file exists
ls -la examples/basic_demo.cfg

# Try running without config file first
./nlm_agent --mode demo

# Then add config back
./nlm_agent --load-config examples/basic_demo.cfg
```

### Problem: Visualization Not Working
**Solution:**
```bash
# Check dependencies
./nlm_agent --help | grep -i visualize

# Try text output instead
./nlm_agent --mode debug --log-level info --visualize-text > output.txt

# Check if HTML dependencies are available
which firefox
which google-chrome
```

### Problem: Memory Usage Too High
**Solution:**
```bash
# Reduce configuration parameters
./nlm_agent --mode train --brain-regions 2 --neurons-per-region 100

# Add memory limits
./nlm_agent --mode train --memory-capacity 500 --memory-decay 0.02
```

## Conclusion

These examples provide a comprehensive starting point for working with the Enhanced NLM system. They demonstrate:

1. **Basic usage** for getting started quickly
2. **Advanced configurations** for complex scenarios
3. **Debugging tools** for troubleshooting
4. **Performance optimization** techniques
5. **Best practices** for production use

Use these examples as templates for your own NLM applications, and feel free to modify them to meet your specific requirements.