#!/usr/bin/env bash
# NLM Phase 6 Usage Examples
# Examples for using the improved NLM Phase 6 command-line interface

# Create examples directory
mkdir -p examples

# Example 1: Quick Start (Easy Mode)
cat > examples/example_1_easy_mode.sh << 'EOF'
#!/bin/bash
# Example 1: Quick Start - Easy Mode
# This example shows the simplest way to run NLM Phase 6

echo "=== NLM Phase 6 Quick Start (Easy Mode) ==="
echo "This runs a basic demonstration with recommended settings."
echo ""

# Run easy mode demo
./nlm --mode easy

echo ""
echo "=== Completed Easy Mode Demo ==="
echo "The brain has been initialized and tested with default settings."
echo "All systems are now functional and ready for use."
EOF

# Example 2: Focused Integration Test (Demo Mode)
cat > examples/example_2_demo_mode.sh << 'EOF'
#!/bin/bash
# Example 2: Focused Integration Test - Demo Mode
# This example shows testing specific brain system integrations

echo "=== NLM Phase 6 Demo Mode (Memory Integration) ==="
echo "This tests the memory system integration specifically."
echo ""

# Run memory integration test with data export
./nlm --mode demo --demo memory --enable-csv

echo ""
echo "=== Completed Memory Integration Test ==="
echo "Memory systems have been verified and tested."
echo "Results have been saved to CSV format for analysis."
EOF

# Example 3: Advanced Research Experiment (Expert Mode)
cat > examples/example_3_expert_mode.sh << 'EOF'
#!/bin/bash
# Example 3: Advanced Research Experiment - Expert Mode
# This example shows advanced usage with custom parameters

echo "=== NLM Phase 6 Expert Mode Research Experiment ==="
echo "This runs a large-scale experiment with custom parameters."
echo ""

# Run expert mode experiment
./nlm --mode expert \
  --neurons 2000 \
  --steps 20000 \
  --timestep 0.001 \
  --enable-csv \
  --enable-json \
  --generate-report detailed

echo ""
echo "=== Completed Expert Mode Experiment ==="
echo "Large-scale experiment completed successfully."
echo "Data exported in CSV and JSON formats."
echo "Detailed report generated."
EOF

# Example 4: Custom Configuration (Custom Mode)
cat > examples/example_4_custom_mode.sh << 'EOF'
#!/bin/bash
# Example 4: Custom Configuration - Custom Mode
# This example shows using custom configuration files

echo "=== NLM Phase 6 Custom Configuration Mode ==="
echo "This loads a custom configuration file and applies overrides."
echo ""

# Create a sample configuration file
cat > custom_config.cfg << CONFIGEOF
# Custom NLM configuration
neuron_count = 1500
region_count = 2
connection_probability = 0.15
random_seed = 123
simulation_timestep = 0.002
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
CONFIGEOF

# Run custom mode with configuration
./nlm --mode custom --config custom_config.cfg

echo ""
echo "=== Completed Custom Configuration ==="
echo "Custom configuration loaded and applied successfully."
echo "Brain initialized with specified parameters."
EOF

# Example 5: Scripted Batch Processing
cat > examples/example_5_batch_processing.sh << 'EOF'
#!/bin/bash
# Example 5: Scripted Batch Processing
# This example shows running multiple experiments using scripts

echo "=== NLM Phase 6 Batch Processing ==="
echo "This runs multiple experiments with different parameters."
echo ""

# Create experiment script template
cat > experiment_template.txt << SCRIPTEOF
# Experiment template for NLM Phase 6
set neurons=1000
set steps=5000
set timestep=0.001

enable checkpointing
disable replay

generate report brief
SCRIPTEOF

# Run multiple experiments
echo "Running Experiment 1..."
./nlm --mode expert --script experiment_template.txt

echo ""
echo "Running Experiment 2..."
# Modify script for second experiment
sed -i 's/neurons=1000/neurons=1500/' experiment_template.txt
sed -i 's/steps=5000/steps=8000/' experiment_template.txt
./nlm --mode expert --script experiment_template.txt

echo ""
echo "Running Experiment 3..."
# Modify script for third experiment
sed -i 's/neurons=1500/neurons=2000/' experiment_template.txt
sed -i 's/steps=8000/steps=12000/' experiment_template.txt
./nlm --mode expert --script experiment_template.txt

echo ""
echo "=== Completed Batch Processing ==="
echo "Three experiments completed with different parameters."
echo "Each experiment ran with script-based configuration."
SCRIPTEOF

# Make examples executable
chmod +x examples/example_*.sh

# Create a main usage README
cat > examples/README.md << 'EOF'
# NLM Phase 6 Usage Examples

This directory contains example scripts for using NLM Phase 6 command-line interface.

## Examples Available

### 1. Quick Start (Easy Mode)
- **File**: `example_1_easy_mode.sh`
- **Purpose**: Simple demonstration with default settings
- **Command**: `./example_1_easy_mode.sh`

### 2. Focused Integration Test (Demo Mode)
- **File**: `example_2_demo_mode.sh`
- **Purpose**: Test specific brain system integration
- **Command**: `./example_2_demo_mode.sh`

### 3. Advanced Research Experiment (Expert Mode)
- **File**: `example_3_expert_mode.sh`
- **Purpose**: Large-scale experiment with custom parameters
- **Command**: `./example_3_expert_mode.sh`

### 4. Custom Configuration (Custom Mode)
- **File**: `example_4_custom_mode.sh`
- **Purpose**: Load and use custom configuration files
- **Command**: `./example_4_custom_mode.sh`

### 5. Scripted Batch Processing
- **File**: `example_5_batch_processing.sh`
- **Purpose**: Run multiple experiments with scripts
- **Command**: `./example_5_batch_processing.sh`

## Additional Files

- `nlm_expert_script.txt`: Example script file for expert mode

## How to Use Examples

1. Make scripts executable:
   ```bash
   chmod +x examples/example_*.sh
   ```

2. Run any example:
   ```bash
   ./examples/example_1_easy_mode.sh
   ```

3. Modify scripts for your specific needs

## Prerequisites

- NLM Phase 6 compiled and executable
- Required configuration files in `configs/` directory
- Sufficient system resources for your chosen parameters
EOF

echo "Created NLM Phase 6 usage examples in examples/ directory"
