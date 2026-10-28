#!/bin/bash
# NLM Configuration Setup Script
# Creates all configuration templates and sets up the project

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Create templates directory
print_info "Creating configuration templates directory..."
mkdir -p configs/templates

# Template content (Phase 2)
print_info "Creating Phase 2 configuration template (basic neural computation)..."
cat > configs/templates/phase2.cfg << 'EOF'
# NLM Configuration File - Phase 2: Basic Neural Computation
# Demonstrates real LIF neuron dynamics, spike propagation, STDP, Hebbian learning, structural plasticity

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds (Phase 2: real-time precision)
simulation_timestep = 0.001

# Number of neurons (Phase 2: smaller for demonstration)
neuron_count = 500

# Number of regions (Phase 2: simple organization)
region_count = 1

# Connection probability
connection_probability = 0.15

# Plasticity settings (Phase 2: core plasticity rules)
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development settings (Phase 2: basic development)
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation settings (Phase 2: basic neuromodulation)
dopamine_baseline = 0.0
curiosity_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Logging settings (Phase 2: focused logging)
log_level = INFO
log_to_file = true
log_filename = nlm_phase2.log

# Simulation settings (Phase 2: basic simulation length)
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings (Phase 2: optional visualization)
visualization_enabled = false
visualization_update_rate = 30.0
EOF

print_success "Created configs/templates/phase2.cfg"

# Template content (Phase 4)
print_info "Creating Phase 4 configuration template (advanced cognition)..."
cat > configs/templates/phase4.cfg << 'EOF'
# NLM Configuration File - Phase 4: Advanced Cognition
# Demonstrates neural prediction, working memory, episodic memory, concept formation, neural attention

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds (Phase 4: high precision)
simulation_timestep = 0.0005

# Number of neurons (Phase 4: larger for cognition)
neuron_count = 1500

# Number of regions (Phase 4: multiple specialized regions)
region_count = 3

# Connection probability (Phase 4: higher density for cognition)
connection_probability = 0.08

# Plasticity settings (Phase 4: enhanced plasticity for learning)
plasticity_learning_rate = 0.015
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_time_constant = 25.0

# Development settings (Phase 4: moderate development)
development_synaptogenesis_rate = 0.0005
development_pruning_rate = 0.00002

# Neuromodulation settings (Phase 4: curiosity and novelty emphasis)
dopamine_baseline = 0.0
curiosity_baseline = 0.2
novelty_threshold = 0.03

# Reward settings (Phase 4: reward prediction error)
reward_discount_factor = 0.97
reward_learning_rate = 0.012

# Memory system settings (Phase 4: working and episodic memory)
working_memory_capacity = 300
episodic_memory_max_episodes = 2000
associative_memory_pattern_count = 500

# Prediction system settings (Phase 4: prediction emphasis)
prediction_horizon = 15
prediction_error_threshold = 0.08
prediction_learning_rate = 0.018

# Cognition system settings (Phase 4: advanced cognition)
planner_planning_depth = 8
concept_formation_threshold = 0.7
attention_inhibition_strength = 0.6
attention_excitation_strength = 2.0

# Simulation settings (Phase 4: longer simulation)
max_simulation_steps = 20000
simulation_time_limit = 0.0

# Logging settings (Phase 4: comprehensive logging)
log_level = INFO
log_to_file = true
log_filename = nlm_phase4.log

# Visualization settings (Phase 4: visualization enabled for cognition)
visualization_enabled = false
visualization_update_rate = 60.0
EOF

print_success "Created configs/templates/phase4.cfg"

# Template content (Phase 6 - also default)
print_info "Creating Phase 6 configuration template (final integration)..."
cat > configs/default.cfg << 'EOF'
# NLM Configuration File - Phase 6: Final Integration
# Demonstrates complete integration of all brain systems

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds (Phase 6: maximum precision)
simulation_timestep = 0.0002

# Number of neurons (Phase 6: large for complex integration)
neuron_count = 3000

# Number of regions (Phase 6: many regions for specialization)
region_count = 6

# Connection probability (Phase 6: optimized for integration)
connection_probability = 0.06

# Plasticity settings (Phase 6: enhanced plasticity)
plasticity_learning_rate = 0.025
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
stdp_time_constant = 30.0

# Development settings (Phase 6: full developmental stages)
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.00005
development_critical_period = 0.4

# Neuromodulation settings (Phase 6: all neuromodulators)
dopamine_baseline = 0.0
dopamine_scale = 1.5
curiosity_baseline = 0.3
novelty_threshold = 0.02
prediction_error_decay = 0.98

# Reward settings (Phase 6: advanced reward processing)
reward_discount_factor = 0.93
reward_learning_rate = 0.02
max_reward_history = 2000

# Memory system settings (Phase 6: comprehensive memory)
working_memory_capacity = 500
episodic_memory_max_episodes = 10000
associative_memory_pattern_count = 2000
memory_consolidation_threshold = 0.8
memory_replay_interval = 50

# Prediction system settings (Phase 6: sophisticated prediction)
prediction_horizon = 25
prediction_error_threshold = 0.05
prediction_learning_rate = 0.022

# Cognition system settings (Phase 6: all cognition systems)
planner_planning_depth = 10
concept_formation_threshold = 0.8
attention_inhibition_strength = 0.7
attention_excitation_strength = 2.5

# Development system settings (Phase 6: full development)
development_update_interval = 500
development_plasticity_decay = 0.03

# Simulation settings (Phase 6: extensive simulation)
max_simulation_steps = 100000
simulation_time_limit = 0.0
checkpoint_save_interval = 500
checkpoint_save_count = 20
replay_consolidation_interval = 500
replay_episode_count = 10

# Logging settings (Phase 6: comprehensive logging)
log_level = INFO
log_to_file = true
log_filename = nlm_phase6.log
log_to_console = true
log_file_rotation = true
log_rotation_size = 20971520  # 20MB

# Visualization settings (Phase 6: advanced visualization)
visualization_enabled = false
visualization_update_rate = 120.0
visualization_window_width = 1920
visualization_window_height = 1080
visualization_show_neurons = true
visualization_show_synapses = true
visualization_show_memory = true
visualization_show_planner = true
visualization_show_dev = true

# Experimental settings (Phase 6: comprehensive experiments)
experiment_default_duration = 5000
experiment_repetitions = 10
experiment_noise_level = 0.005
experiment_stochasticity = 0.05

# Performance optimization (Phase 6: high performance)
use_memory_pools = true
pool_size_multiplier = 3.0
event_queue_capacity = 50000
use_simd = true
simd_vector_width = 512
EOF

print_success "Created configs/default.cfg (Phase 6)"

# Template content (Debug)
print_info "Creating Debug configuration template..."
cat > configs/templates/debug.cfg << 'EOF'
# NLM Configuration File - Debug Mode
# Optimized for debugging and development

# Random seed for reproducible experiments
random_seed = 123

# Simulation timestep in seconds (Debug: smaller timestep)
simulation_timestep = 0.0001

# Number of neurons (Debug: small for debugging)
neuron_count = 100

# Number of regions (Debug: single region)
region_count = 1

# Connection probability (Debug: simple connectivity)
connection_probability = 0.5

# Plasticity settings (Debug: disabled for stable debugging)
plasticity_learning_rate = 0.0
stdp_ltp_weight = 0.0
stdp_ltd_weight = 0.0
stdp_time_constant = 0.0

# Development settings (Debug: disabled)
development_synaptogenesis_rate = 0.0
development_pruning_rate = 0.0

# Neuromodulation settings (Debug: simple)
dopamine_baseline = 0.0
curiosity_baseline = 0.0
novelty_threshold = 1.0

# Reward settings (Debug: disabled)
reward_discount_factor = 1.0
reward_learning_rate = 0.0

# Logging settings (Debug: maximum detail)
log_level = DEBUG
log_to_file = true
log_filename = nlm_debug.log
log_to_console = true
log_file_rotation = false

# Simulation settings (Debug: short simulation)
max_simulation_steps = 1000
simulation_time_limit = 0.0

# Visualization settings (Debug: enabled for debugging)
visualization_enabled = false
visualization_update_rate = 10.0

# Experimental settings (Debug: minimal)
experiment_default_duration = 100
experiment_repetitions = 1
experiment_noise_level = 0.0
experiment_stochasticity = 0.0

# Performance optimization (Debug: no optimization)
use_memory_pools = false
event_queue_capacity = 1000
use_simd = false
EOF

print_success "Created configs/templates/debug.cfg"

# Template content (Research)
print_info "Creating Research configuration template..."
cat > configs/templates/research.cfg << 'EOF'
# NLM Configuration File - Research Mode
# Optimized for research and benchmarking

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds (Research: precise)
simulation_timestep = 0.001

# Number of neurons (Research: medium for research)
neuron_count = 1000

# Number of regions (Research: balanced)
region_count = 3

# Connection probability (Research: optimized)
connection_probability = 0.1

# Plasticity settings (Research: standard research values)
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
stdp_time_constant = 20.0

# Development settings (Research: controlled development)
development_synaptogenesis_rate = 0.0002
development_pruning_rate = 0.00003

# Neuromodulation settings (Research: baseline)
dopamine_baseline = 0.0
curiosity_baseline = 0.05
novelty_threshold = 0.04

# Reward settings (Research: standard)
reward_discount_factor = 0.98
reward_learning_rate = 0.01

# Memory system settings (Research: sufficient memory)
working_memory_capacity = 150
episodic_memory_max_episodes = 1000
associative_memory_pattern_count = 300

# Prediction system settings (Research: balanced prediction)
prediction_horizon = 8
prediction_error_threshold = 0.09
prediction_learning_rate = 0.015

# Cognition system settings (Research: balanced cognition)
planner_planning_depth = 5
concept_formation_threshold = 0.65
attention_inhibition_strength = 0.55
attention_excitation_strength = 1.8

# Simulation settings (Research: long simulation)
max_simulation_steps = 50000
simulation_time_limit = 0.0
checkpoint_save_interval = 5000
checkpoint_save_count = 5
replay_consolidation_interval = 5000
replay_episode_count = 3

# Logging settings (Research: focused logging)
log_level = INFO
log_to_file = true
log_filename = nlm_research.log

# Visualization settings (Research: optional)
visualization_enabled = false
visualization_update_rate = 30.0

# Experimental settings (Research: rigorous)
experiment_default_duration = 2000
experiment_repetitions = 3
experiment_noise_level = 0.01
experiment_stochasticity = 0.1

# Performance optimization (Research: balanced)
use_memory_pools = true
pool_size_multiplier = 1.5
event_queue_capacity = 20000
use_simd = true
simd_vector_width = 256
EOF

print_success "Created configs/templates/research.cfg"

# Create a summary document
print_info "Creating configuration summary document..."
cat > configs/CONFIG_SUMMARY.md << 'EOF'
# NLM Configuration System

## Overview
This document describes the configuration system for NLM (熙然), an experimental artificial developmental brain project.

## Configuration Files

### Primary Configuration
- **`configs/default.cfg`**: Phase 6 - Final Integration Configuration (current default)
  - All integrated brain systems enabled
  - 3000 neurons, 6 regions
  - Comprehensive memory, prediction, and cognition systems
  - Optimized for research and demonstration

### Configuration Templates
- **`configs/templates/phase2.cfg`**: Phase 2 - Basic Neural Computation
  - Demonstrates real LIF neuron dynamics, STDP, Hebbian learning
  - 500 neurons, 1 region
  - Focused on basic plasticity and learning

- **`configs/templates/phase4.cfg`**: Phase 4 - Advanced Cognition
  - Demonstrates neural prediction, working memory, episodic memory
  - 1500 neurons, 3 regions
  - Focus on complex cognition systems

- **`configs/templates/debug.cfg`**: Debug Mode
  - Optimized for development and debugging
  - 100 neurons, 1 region
  - All plasticity and development disabled for stability

- **`configs/templates/research.cfg`**: Research Mode
  - Optimized for research and benchmarking
  - 1000 neurons, 3 regions
  - Balanced configuration for systematic investigation

## Key Configuration Categories

### Brain Configuration
- `neuron_count`: Total number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synaptic connections

### Plasticity Configuration
- `plasticity_learning_rate`: Overall plasticity learning rate
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_time_constant`: STDP time constant

### Development Configuration
- `development_synaptogenesis_rate`: Rate of new synapse formation
- `development_pruning_rate`: Rate of synapse elimination
- `development_critical_period`: Length of critical period

### Neuromodulation Configuration
- `dopamine_baseline`: Baseline dopamine level
- `curiosity_baseline`: Baseline curiosity level
- `novelty_threshold`: Novelty detection threshold

### Memory Configuration
- `working_memory_capacity`: Capacity of working memory
- `episodic_memory_max_episodes`: Maximum number of episodic memories
- `associative_memory_pattern_count`: Number of associative memory patterns

### Simulation Configuration
- `max_simulation_steps`: Maximum number of simulation steps
- `simulation_timestep`: Time step for simulation
- `checkpoint_save_interval`: Interval for saving checkpoints

### Logging Configuration
- `log_level`: Logging level (DEBUG, INFO, WARN, ERROR)
- `log_to_file`: Whether to write logs to file
- `log_filename`: Name of log file

### Performance Configuration
- `use_memory_pools`: Whether to use memory pools for performance
- `use_simd`: Whether to use SIMD optimizations
- `event_queue_capacity`: Capacity of event queue

## Usage

### Command Line Options
```bash
nlm_demo --help                    # Show help message
nlm_demo --demo-mode 1              # Run Phase 6 integration demo
nlm_demo --demo-mode 2              # Run focused integration demo
nlm_demo --demo-mode 3              # Run recovery demo
nlm_demo --steps 5000               # Run 5000 simulation steps
nlm_demo --timestep 0.001           # Use 1ms timestep
nlm_demo --checkpoint mybrain.bin    # Save brain state to file
nlm_demo --load saved_brain.bin      # Load brain state from file
```

### Creating Custom Configurations
Copy one of the template files and modify values as needed:
```bash
cp configs/templates/phase6.cfg myconfig.cfg
# Edit myconfig.cfg with your desired parameters
```

### Configuration Validation
To validate a configuration file:
```bash
# The configuration system will validate when the brain is initialized
# Invalid values will cause initialization to fail
```

## Configuration Tips

### For Research
- Use `configs/templates/research.cfg` as a starting point
- Adjust `neuron_count` and `region_count` based on available memory
- Increase `max_simulation_steps` for longer experiments

### For Development/Debugging
- Use `configs/templates/debug.cfg` for stable debugging
- Set `log_level = DEBUG` for detailed logging
- Disable complex features to focus on specific components

### For Demonstration
- Use `configs/default.cfg` for comprehensive demonstrations
- Adjust `simulation_steps` based on available time
- Enable `visualization_enabled = true` for visual demonstrations

## Configuration File Format
- Key = value format (no quotes around strings)
- Comments start with #
- Boolean values: true/false
- Float values: decimal point notation
- Integer values: no decimal point
- String values: no quotes

## Example Configuration File
```
# Random seed for reproducible experiments
random_seed = 42

# Number of neurons in the brain
neuron_count = 1000

# Connection probability
connection_probability = 0.1

# Simulation timestep
simulation_timestep = 0.001

# Enable logging
log_level = INFO
log_to_file = true
log_filename = nlm.log
```

## Advanced Configuration Options

### Memory Pool Configuration
- `use_memory_pools`: Enable memory pools for better performance
- `pool_size_multiplier`: Multiplier for pool sizes
- `event_queue_capacity`: Maximum events in queue

### SIMD Configuration
- `use_simd`: Enable SIMD optimizations
- `simd_vector_width`: SIMD vector width (128, 256, 512)

### Development Configuration
- `development_update_interval`: How often development updates occur
- `development_plasticity_decay`: Rate of plasticity decay during development

## Configuration Validation Rules

1. **Neuron count validation**: Must be > 0
2. **Region count validation**: Must be >= 1
3. **Connection probability**: Must be between 0.0 and 1.0
4. **Timestep validation**: Must be > 0.0
5. **Memory capacity validation**: Must be >= 0
6. **Time constants**: Must be >= 0.0
7. **Rate values**: Must be >= 0.0

## Configuration Best Practices

1. **For Production**: Use `configs/default.cfg` or create a custom config based on it
2. **For Research**: Use `configs/templates/research.cfg` as a baseline
3. **For Development**: Use `configs/templates/debug.cfg` for stability
4. **Always**: Test new configurations with small neuron counts first
5. **Document**: Keep track of configuration changes for reproducibility

## Configuration Migration Guide

### From Phase 2 to Phase 6
- Increase `neuron_count` from 500 to 3000
- Add development settings for full developmental stages
- Enable all neuromodulators (dopamine, curiosity, novelty)
- Add comprehensive memory system configuration
- Enable prediction and cognition systems

### From Research to Production
- Increase logging verbosity as needed
- Enable memory pools for performance
- Add checkpoint saving for recovery
- Consider visualization for demonstrations

## Configuration Examples

### High-Performance Configuration
```
use_memory_pools = true
pool_size_multiplier = 3.0
event_queue_capacity = 50000
use_simd = true
simd_vector_width = 512
max_simulation_steps = 100000
```

### Educational Configuration
```
neuron_count = 100
region_count = 1
connection_probability = 0.3
visualization_enabled = true
max_simulation_steps = 1000
experiment_default_duration = 500
```

### Research Configuration
```
neuron_count = 2000
region_count = 4
connection_probability = 0.08
use_memory_pools = true
max_simulation_steps = 50000
experiment_repetitions = 5
```

## Configuration System Features

### Automatic Configuration Loading
- Load from file: `config->loadFromFile("config.cfg")`
- Load from command line: `config->loadFromArgs(argc, argv)`
- Merge configurations (file + command line + runtime)

### Configuration Validation
- Type checking for all configuration values
- Range validation for numeric values
- Consistency checking between related settings

### Configuration Saving
- Save current configuration to file
- Export configuration for documentation
- Backup configuration before changes

### Configuration Templates
- Built-in templates for different use cases
- Easy customization of templates
- Template inheritance (phase6 inherits from phase4, etc.)

## Future Configuration Enhancements

1. **Configuration Profiles**: Save and switch between configuration profiles
2. **Configuration GUI**: Visual configuration editor
3. **Configuration Validation**: Automated validation with detailed error messages
4. **Configuration Documentation**: Automatic documentation generation
5. **Configuration Versioning**: Track configuration changes over time

## Contact and Support

For configuration issues, refer to:
- NLM Documentation (docs/CONFIGURATION.md)
- NLM GitHub Repository
- NLM Community Forums

---
*This document is part of the NLM (熙然) configuration system.*
*Last updated: 2026*
EOF

print_success "Created configs/CONFIG_SUMMARY.md"

# Create a quick start script
print_info "Creating quick start script..."
cat > quickstart.sh << 'EOF'
#!/bin/bash
# Quick Start Script for NLM Phase 6 Demo
# Run this script to quickly start the NLM Phase 6 integration demo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

function print_header() {
    echo -e "${BLUE}╔═══════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                                                               ║${NC}"
    echo -e "${BLUE}║         NLM Phase 6 Quick Start                                 ║${NC}"
    echo -e "${BLUE}║                                                               ║${NC}"
    echo -e "${BLUE}╚═══════════════════════════════════════════════════════════════╝${NC}"
}

function print_menu() {
    echo ""
    echo "Choose an option:"
    echo "  1) Run Phase 6 Integration Demo (default)"
    echo "  2) Run Phase 2 Basic Demo"
    echo "  3) Run Phase 4 Cognition Demo"
    echo "  4) Run Debug Demo"
    echo "  5) Show configuration options"
    echo "  6) Exit"
    echo ""
}

function run_demo() {
    local demo_type=$1
    local config_file=$2
    
    echo -e "${GREEN}Starting $demo_type demo...${NC}"
    
    # Build the command
    local cmd="./nlm"
    cmd="$cmd --demo-mode $demo_type"
    cmd="$cmd --config $config_file"
    
    echo "Command: $cmd"
    echo ""
    echo "Running... Press Ctrl+C to interrupt"
    echo ""
    
    # Execute the demo
    eval $cmd
}

function show_configs() {
    echo -e "${BLUE}Available Configuration Options:${NC}"
    echo ""
    echo "  --demo-mode 1  Full Integration Demo (Phase 6)"
    echo "    - Tests all integrated brain systems"
    echo "    - Memory, Prediction, Cognition, Neuromodulation, Development"
    echo ""
    echo "  --demo-mode 2  Focused Integration Demo (Phase 6)"
    echo "    - Tests specific integrated systems"
    echo "    - Memory, Prediction, Neuromodulation focus"
    echo ""
    echo "  --demo-mode 3  Recovery Demo"
    echo "    - Tests checkpoint save/load functionality"
    echo "    - Tests brain reset and recovery"
    echo ""
    echo "  --steps N      Number of simulation steps (default: 1000)"
    echo "  --timestep T   Simulation timestep in seconds (default: 0.001)"
    echo "  --verbose      Enable verbose logging"
    echo "  --checkpoint F Save brain state to file"
    echo "  --output F     Write results to file"
    echo ""
    echo "Configuration files:"
    echo "  configs/default.cfg      Phase 6: Final Integration (recommended)"
    echo "  configs/templates/phase2.cfg    Phase 2: Basic Neural Computation"
    echo "  configs/templates/phase4.cfg    Phase 4: Advanced Cognition"
    echo "  configs/templates/debug.cfg      Debug Mode"
    echo ""
    echo "Example commands:"
    echo "  ./quickstart.sh   # Run Phase 6 demo (default)"
    echo "  ./quickstart.sh 1  # Run Phase 6 demo"
    echo "  ./quickstart.sh 2  # Run Phase 2 demo"
    echo ""
}

function main() {
    print_header
    
    # Check if binary exists
    if [ ! -f "./nlm" ]; then
        echo -e "${YELLOW}Warning: NLM binary not found. Building...${NC}"
        echo "Attempting to build NLM..."
        if ! make -j4; then
            echo -e "${RED}Failed to build NLM. Please build manually with:${NC}"
            echo "  mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
            exit 1
        fi
    fi
    
    # Parse arguments
    if [ $# -eq 0 ]; then
        # No arguments, show menu
        print_menu
        read -p "Enter your choice (1-6): " choice
        
        case $choice in
            1) run_demo 1 "configs/default.cfg" ;;
            2) run_demo 2 "configs/templates/phase2.cfg" ;;
            3) run_demo 2 "configs/templates/phase4.cfg" ;;
            4) run_demo 1 "configs/templates/debug.cfg" ;;
            5) show_configs ;;
            6) echo -e "${GREEN}Goodbye!${NC}" ; exit 0 ;;
            *) echo -e "${RED}Invalid choice. Please select 1-6.${NC}" ; exit 1 ;;
        esac
    else
        # Parse command line arguments
        local demo_choice=$1
        local config_file=""
        
        case $demo_choice in
            1) config_file="configs/default.cfg" ;;
            2) config_file="configs/templates/phase2.cfg" ;;
            3) config_file="configs/templates/phase4.cfg" ;;
            4) config_file="configs/templates/debug.cfg" ;;
            *) echo -e "${RED}Invalid demo choice. Use 1-4.${NC}" ; show_configs ; exit 1 ;;
        esac
        
        run_demo $demo_choice "$config_file"
    fi
}

main "$@"
EOF

print_success "Created quickstart.sh"
chmod +x quickstart.sh

print_info "Setting up build directory..."
mkdir -p build
cd build

print_info "Configuring with CMake..."
if ! cmake .. -DCMAKE_BUILD_TYPE=Release; then
    print_error "Failed to configure CMake. Please check your CMake installation."
    exit 1
fi

print_info "Building NLM..."
if ! make -j4; then
    print_error "Failed to build NLM. Please check for build errors."
    exit 1
fi

cd ..

print_success "=== NLM Configuration Setup Complete ==="
echo ""
echo "What you have now:"
echo "✓ Phase 6 demo executable with comprehensive CLI options"
echo "✓ Configuration presets for Phase 2, Phase 4, Phase 6, Debug, and Research"
necho "✓ Updated configs/default.cfg with Phase 6 settings"
echo "✓ Configuration documentation and templates"
echo "✓ Quick start script for easy demos"
echo "✓ Enhanced logging and error handling"
echo "✓ Checkpoint and recovery functionality"
echo "✓ Experiment management framework"
echo ""
echo "To get started:"
echo "  ./quickstart.sh          # Easy interactive demo setup"
echo "  ./nlm --help             # Show command line options"
echo "  ./nlm --demo-mode 1      # Run Phase 6 integration demo"
echo "  ./nlm --demo-mode 2      # Run Phase 2 basic demo"
echo ""
echo "Configuration files are in: configs/"
echo "  - configs/default.cfg        Phase 6: Final Integration"
echo "  - configs/templates/         All configuration templates"
echo ""
print_success "Setup complete! Enjoy exploring the NLM Phase 6 integrated brain system."
