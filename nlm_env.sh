#!/bin/bash

# NLM Environment Setup Script

# Source this script to setup the NLM development environment

echo "Setting up NLM development environment..."

# Set NLM home directory
NLM_HOME="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export NLM_HOME

# Add NLM to PATH
export PATH="$NLM_HOME:$PATH"

# Set up Python environment
if command -v python3 &> /dev/null; then
    echo "Python 3 found, setting up Python environment..."
    
    # Install Python dependencies if needed
    if [ -f "requirements.txt" ]; then
        echo "Installing Python dependencies..."
        pip3 install -r requirements.txt
    fi
    
    # Add Python bindings to path
    PYTHON_PATH="$NLM_HOME:$PYTHONPATH"
    export PYTHONPATH
else
    echo "Warning: Python 3 not found, Python bindings may not work"
fi

# Set up NLM-specific environment variables
export NLM_LOG_LEVEL="${NLM_LOG_LEVEL:-info}"
export NLM_CONFIG="${NLM_CONFIG:-}"

# Create common directories
mkdir -p "$NLM_HOME/checkpoints"
mkdir -p "$NLM_HOME/configs"
mkdir -p "$NLM_HOME/experiments"
mkdir -p "$NLM_HOME/output"
mkdir -p "$NLM_HOME/logs"

# Create a basic config if none exists
if [ ! -f "$NLM_HOME/configs/default.cfg" ]; then
    echo "Creating default configuration..."
    cat > "$NLM_HOME/configs/default.cfg" << EOF
# NLM Default Configuration
# Set simulation parameters
simulation_timestep = 0.001
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Enable/disable systems
enable_plasticity = true
enable_memory = true
enable_neuromodulation = true

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Logging and output
log_level = info
checkpoint_enabled = true
checkpoint_interval = 1000
EOF
    echo "Default configuration created at: $NLM_HOME/configs/default.cfg"
fi

# Create NLM CLI script
NLM_CLI_PATH="$NLM_HOME/bin/nlm-cli"
mkdir -p "$NLM_HOME/bin"

if [ ! -f "$NLM_CLI_PATH" ]; then
    echo "Creating NLM CLI script..."
    cat > "$NLM_CLI_PATH" << 'EOF'
#!/bin/bash

# NLM Command Line Interface Wrapper Script

# Set NLM home directory
NLM_HOME="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/.."

# Use Python CLI implementation
python3 "$NLM_HOME/nlm_cli.py" "$@"
EOF
    chmod +x "$NLM_CLI_PATH"
    echo "NLM CLI script created at: $NLM_CLI_PATH"
fi

# Create development workspace script
DEV_SCRIPT_PATH="$NLM_HOME/scripts/dev_workspace.sh"
mkdir -p "$NLM_HOME/scripts"

if [ ! -f "$DEV_SCRIPT_PATH" ]; then
    echo "Creating development workspace script..."
    cat > "$DEV_SCRIPT_PATH" << 'EOF'
#!/bin/bash

# NLM Development Workspace Script

echo "Setting up NLM development workspace..."

# Create workspace directory
WORKSPACE_DIR="$(pwd)/nlm_workspace"
mkdir -p "$WORKSPACE_DIR"

# Create standard directories
mkdir -p "$WORKSPACE_DIR/checkpoints"
mkdir -p "$WORKSPACE_DIR/configs"
mkdir -p "$WORKSPACE_DIR/experiments"
mkdir -p "$WORKSPACE_DIR/output"
mkdir -p "$WORKSPACE_DIR/logs"
mkdir -p "$WORKSPACE_DIR/scripts"
mkdir -p "$WORKSPACE_DIR/docs"

# Create basic config
cat > "$WORKSPACE_DIR/configs/workspace.cfg" << 'CONFIGEOF'
# NLM Workspace Configuration
simulation_timestep = 0.001
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15

enable_plasticity = true
enable_memory = true
enable_neuromodulation = true

stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

log_level = info
checkpoint_enabled = true
checkpoint_interval = 1000
CONFIGEOF

echo "Development workspace created at: $WORKSPACE_DIR"
echo "Configuration file: $WORKSPACE_DIR/configs/workspace.cfg"

# Show workspace contents
ls -la "$WORKSPACE_DIR"
EOF
    chmod +x "$DEV_SCRIPT_PATH"
    echo "Development workspace script created at: $DEV_SCRIPT_PATH"
fi

# Create quick test script
TEST_SCRIPT_PATH="$NLM_HOME/scripts/quick_test.sh"

if [ ! -f "$TEST_SCRIPT_PATH" ]; then
    echo "Creating quick test script..."
    cat > "$TEST_SCRIPT_PATH" << 'EOF'
#!/bin/bash

# NLM Quick Test Script

echo "Running NLM quick tests..."

# Test if NLM is available
if command -v nlm-cli &> /dev/null; then
    echo "Testing NLM CLI..."
    nlm-cli --version
    
    echo "Running Phase 2 demo..."
    nlm-cli demo phase2
    
    echo "Running learning experiment..."
    nlm-cli run learning --steps 100 --neuron-count 100
    
    echo "All tests passed!"
else
    echo "NLM CLI not found. Please install NLM first."
    exit 1
fi
EOF
    chmod +x "$TEST_SCRIPT_PATH"
    echo "Quick test script created at: $TEST_SCRIPT_PATH"
fi

echo ""
echo "NLM development environment setup complete!"
echo ""
echo "To use NLM:")
echo "  1. Source this script: source nlm_env.sh"
echo "  2. Run NLM CLI: nlm-cli demo phase2"
echo "  3. Run experiments: nlm-cli run learning --steps 1000"
echo ""
echo "Useful commands:")
echo "  - Development workspace: scripts/dev_workspace.sh"
echo "  - Quick test: scripts/quick_test.sh"
echo "  - Help: nlm-cli --help"
echo ""
echo "For more information, see the documentation in the repository.
