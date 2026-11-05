#!/bin/bash
# NLM Setup and Quick Start Script
# This script provides the easiest way to set up and use the NLM advanced interface

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored messages
function print_message() {
    local type="$1"
    shift
    case "$type" in
        "success")
            echo -e "${GREEN}[SUCCESS]${NC} $*"
            ;;
        "warning")
            echo -e "${YELLOW}[WARNING]${NC} $*"
            ;;
        "error")
            echo -e "${RED}[ERROR]${NC} $*"
            ;;
        "info")
            echo -e "${GREEN}[INFO]${NC} $*"
            ;;
        *)
            echo -e "${GREEN}[MESSAGE]${NC} $*"
            ;;
    esac
}

function check_prerequisites() {
    print_message info "Checking prerequisites..."
    
    # Check if we're in the right directory
    if [ ! -f "/workspace/app/src/main.cpp" ]; then
        print_message error "Main NLM source files not found!"
        print_message info "Please ensure you're running this script from the NLM project root."
        exit 1
    fi
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        print_message error "Python 3 not found."
        print_message info "Please install Python 3 using your package manager."
        exit 1
    fi
    
    # Check Python version
    PYTHON_VERSION=$(python3 --version 2>&1 | awk '{print $2}')
    if [[ "$PYTHON_VERSION" != "3.*" ]]; then
        print_message error "Python 1 not found."
        print_message info "Please install Python 3 using your package manager."
        exit 1
    fi
    
    # Check if NLM CLI scripts exist
    if [ ! -f "/workspace/app/nlm_cli.py" ]; then
        print_message error "NLM CLI script not found!"
        print_message info "The improvements may not have been applied correctly."
        exit 1
    fi
    
    print_message success "All prerequisites checked and installed successfully."
}

function setup_directory_structure() {
    print_message info "Setting up directory structure..."
    
    # Create necessary directories
    mkdir -p /workspace/app/configs
    mkdir -p /workspace/app/checkpoints
    mkdir -p /workspace/app/experiments
    
    # Set appropriate permissions
    chmod 755 /workspace/app/configs
    chmod 755 /workspace/app/checkpoints
    chmod 755 /workspace/app/experiments
    
    print_message success "Directory structure created successfully."
}

function create_example_configs() {
    print_message info "Creating example configuration files..."
    
    # Create basic configuration template
    cat > /workspace/app/configs/basic_config.json << EOF
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.15,
  "development_enabled": true,
  "curiosity_enabled": true,
  "plasticity_modifier": 1.0
}
EOF
    
    # Create advanced configuration template
    cat > /workspace/app/configs/advanced_config.json << EOF
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.10,
  "development_enabled": true,
  "curiosity_enabled": true,
  "novelty_enabled": true,
  "prediction_enabled": true,
  "concept_enabled": true,
  "social_enabled": true,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "plasticity_modifier": 0.8
}
EOF
    
    # Create batch configuration template
    cat > /workspace/app/configs/batch_config.json << EOF
{
  "batch_config": {
    "experiment_count": 10,
    "seed_start": 1,
    "seed_end": 100,
    "parallel": true,
    "auto_save": true,
    "monitor_interval": 100
  },
  "experiment_template": {
    "neuron_count": 500,
    "region_count": 1,
    "seed": 42,
    "max_steps": 1000,
    "development_enabled": true,
    "curiosity_enabled": true
  }
}
EOF
    
    print_message success "Configuration templates created."
}

function make_scripts_executable() {
    print_message info "Making scripts executable..."
    
    chmod +x /workspace/app/nlm_cli.py
    chmod +x /workspace/app/nlm_wrapper.py
    
    print_message success "Scripts made executable."
}

function show_quick_start_examples() {
    print_message info "Showing quick start examples..."
    
    cat << EOF

========================================================
QUICK START EXAMPLES
========================================================

1. BASIC EXPERIMENT SETUP:
   cd /workspace/app
   ./nlm_wrapper.py configure brain create --neurons 500 --regions 1 --seed 42
   ./nlm_wrapper.py experiment run --name test1 --max-steps 1000

2. BATCH PROCESSING:
   ./nlm_wrapper.py batch run --count 10 --seed-start 1 --seed-end 100 --parallel

3. SYSTEM DIAGNOSTICS:
   ./nlm_wrapper.py diagnostic run --verbose

4. RESULT ANALYSIS:
   ./nlm_wrapper.py analysis run --input results.json --plot --export

5. CHECKPOINT MANAGEMENT:
   ./nlm_wrapper.py checkpoint create --label my_checkpoint
   ./nlm_wrapper.py checkpoint list

========================================================

EOF
}

function show_help_resources() {
    print_message info "Providing help resources..."
    
    cat << EOF

========================================================
HELP RESOURCES
========================================================

Documentation:
- README.md - Complete documentation
- README_QUICK_START.md - Quick start guide
- IMPROVEMENTS_SUMMARY.md - Detailed improvements summary
- CHANGES_BREAKDOWN.md - File-by-file changes

Help Commands:
- ./nlm_wrapper.py help
- ./nlm_wrapper.py help experiment
- ./nlm_wrapper.py help batch
- ./nlm_wrapper.py help diagnostic

Configuration Examples:
- /workspace/app/configs/basic_config.json
- /workspace/app/configs/advanced_config.json
- /workspace/app/configs/batch_config.json

========================================================

EOF
}

function main() {
    print_message info "NLM Advanced Interface Setup and Quick Start"
    print_message info "==============================================="
    print_message info ""
    print_message info "This script will set up the NLM advanced interface"
    print_message info "and provide quick start examples for getting started."
    print_message info ""
    
    # Check if script is running from correct directory
    if [ "$(pwd)" != "/workspace/app" ]; then
        print_message warning "Script is not running from /workspace/app directory."
        print_message info "Changing to project directory..."
        cd /workspace/app || {
            print_message error "Cannot change to /workspace/app directory."
            exit 1
        }
    fi
    
    # Run setup steps
    check_prerequisites
    setup_directory_structure
    create_example_configs
    make_scripts_executable
    
    print_message success "NLM Advanced Interface setup completed successfully!"
    print_message info ""
    
    show_quick_start_examples
    show_help_resources
    
    print_message success "Setup completed! You're ready to use NLM advanced features."
    print_message info ""
    print_message info "Next steps:"
    print_message info "1. Configure your first NLM brain"
    print_message info "2. Run a basic experiment"
    print_message info "3. Explore batch processing capabilities"
    print_message info "4. Set up automated workflows"
    print_message info ""
    print_message info "For detailed documentation, see README.md"
}

# Run main function
main "$@"
