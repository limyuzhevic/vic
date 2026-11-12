#!/usr/bin/env bash
# NLM Project - Installation Script

# This script installs the NLM project with all utility modules
# and configuration options.

set -e  # Exit on any error

echo "🚀 NLM Project - Installation Script"
echo "====================================="

# Check if we're in the right directory
if [ ! -f "setup.py" ]; then
    echo "❌ Error: setup.py not found. Please run this script from the project root."
    exit 1
fi

echo "✅ Project structure looks good."

# Check Python version
python_version=$(python3 --version 2>&1)
echo "🐍 Python version: $python_version"

# Check pip version
pip_version=$(pip3 --version 2>&1)
echo "📦 pip version: $pip_version"

# Create virtual environment if requested
if [[ "$1" == "--venv" ]]; then
    venv_name="nlm_env"
    echo "🔧 Creating virtual environment: $venv_name"
    python3 -m venv "$venv_name"
    source "$venv_name/bin/activate"
    echo "✅ Virtual environment activated."
fi

# Upgrade pip
echo "⬆️ Upgrading pip..."
pip3 install --upgrade pip

# Install core dependencies
echo "📥 Installing core dependencies..."
pip3 install numpy matplotlib PyYAML scikit-build-core pybind11 pytest

# Install pynlm in development mode
echo "🔧 Installing pynlm in development mode..."
pip3 install -e .

# Install additional utilities
echo "🛠️ Installing additional utility modules..."
pip3 install pandas seaborn scikit-learn

# Create examples directory structure
mkdir -p examples
mkdir -p examples/basic
mkdir -p examples/advanced
mkdir -p examples/experiments

echo "📝 Creating example files..."

# Create basic example
if [ ! -f "examples/basic_brain.py" ]; then
    cat > examples/basic_brain.py << 'EOF'
import pynlm

"""
Basic NLM brain example.

This example shows the simplest way to create and run a brain simulation.
"""

# Step 1: Create default configuration
config = pynlm.createDefaultConfig()

# Step 2: Create brain
brain = pynlm.createBrain(config)

# Step 3: Initialize brain
brain.initialize()

# Step 4: Run simulation
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print("Running simulation for 100 steps...")

for step in range(100):
    brain.step(step)

print(f"Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
EOF
fi

# Create advanced example
if [ ! -f "examples/agent_simulation.py" ]; then
    cat > examples/agent_simulation.py << 'EOF'
import pynlm
from pynlm.utils import BrainAnalyzer, WorldUtils
from pynlm.config import BrainConfigurator

"""
Agent simulation example.

This example shows how to create a complete agent simulation
with brain, world, and agent integration.
"""

def run_agent_simulation(steps=500):
    """Run complete agent simulation."""
    # Step 1: Configure brain
    config = BrainConfigurator()
    config.apply_preset("medium")

    # Step 2: Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Step 3: Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()

    # Step 4: Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)

    # Step 5: Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)

    # Step 6: Run simulation
    for step in range(steps):
        # Update world
        world.update(0.1)

        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())

        # Brain computation
        brain.step(step)

        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

        # Apply reward modulation
        agent.applyRewardModulation(
            world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0,
            0.0
        )

        # Update development
        agent.updateDevelopment(0.1)

        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

    return brain, agent, world

# Run the simulation
if __name__ == "__main__":
    print("Starting agent simulation...")
    brain, agent, world = run_agent_simulation(200)

    print("\nSimulation completed!")
    print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

    # Analyze results
    analyzer = BrainAnalyzer(brain)
    analyzer.print_formatted_stats()
EOF
fi

# Create configuration example
if [ ! -f "examples/config_example.py" ]; then
    cat > examples/config_example.py << 'EOF'
import pynlm
from pynlm.config import BrainConfigurator, ParameterSweep

"""
Configuration management example.

This example shows different ways to configure NLM brains.
"""

def create_from_preset(preset_name):
    """Create brain using built-in preset."""
    config = BrainConfigurator()
    config.apply_preset(preset_name)
    return config

def create_custom():
    """Create brain with custom settings."""
    config = BrainConfigurator()
    config.update({
        'brain.neuron_count': 3000,
        'brain.synapse_density': 0.15,
        'random_seed': 12345,
        'plasticity.stdp.learning_rate': 0.02,
    })
    return config

def run_parameter_sweep():
    """Run parameter sweep."""
    base_config = BrainConfigurator().apply_preset("medium")
    sweep = ParameterSweep(base_config)

    sweep.add_parameter("brain.neuron_count", [1000, 2000, 5000])
    sweep.add_parameter("random_seed", [42, 123, 456])
    sweep.add_parameter("plasticity.stdp.learning_rate", [0.001, 0.01, 0.1])

    configurations = sweep.generate_configurations()
    print(f"Generated {len(configurations)} configurations")
    return configurations

if __name__ == "__main__":
    print("Configuration examples:")
    print("\n1. Using preset:")
    config1 = create_from_preset("research")
    config1.print_summary()

    print("\n2. Custom configuration:")
    config2 = create_custom()
    config2.print_summary()

    print("\n3. Parameter sweep:")
    configs = run_parameter_sweep()
EOF
fi

# Create visualization example
if [ ! -f "examples/viz_example.py" ]; then
    cat > examples/viz_example.py << 'EOF'
import pynlm
from pynlm.viz import Visualization, create_quick_dashboard, plot_brain_stats

"""
Visualization example.

This example shows how to create visualizations and dashboards.
"""

def create_basic_visualization(brain):
    """Create basic visualization."""
    viz = Visualization()

    # Create dashboard
    dashboard_file = viz.save_dashboard(brain, "basic_dashboard.png")
    print(f"Dashboard saved to: {dashboard_file}")

    # Create stats plot
    stats_file = "basic_stats.png"
    fig = plot_brain_stats(brain, filename=stats_file)
    print(f"Stats plot saved to: {stats_file}")

def create_advanced_dashboard(brain, world=None, agent=None):
    """Create advanced dashboard with multiple components."""
    viz = Visualization()

    # Create comprehensive dashboard
    dashboard_file = viz.save_dashboard(
        brain, world, agent, "advanced_dashboard.png"
    )
    print(f"Advanced dashboard saved to: {dashboard_file}")

if __name__ == "__main__":
    # Create and run a simple brain for demonstration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)

    brain = pynlm.createBrain(config)
    brain.initialize()

    for step in range(100):
        brain.step(step)

    print("Creating visualizations...")
    create_basic_visualization(brain)
    print("\nVisualization examples completed!")
EOF
fi

# Create command-line examples directory
mkdir -p examples/command_line

if [ ! -f "examples/command_line/README.md" ]; then
    cat > examples/command_line/README.md << 'EOF'
# Command-Line Examples

This directory contains examples for using the `nlm-utils` command-line tool.

## Available Commands

### brain commands
- `nlm-utils brain create` - Create a new brain
- `nlm-utils brain analyze` - Analyze brain statistics
- `nlm-utils brain save` - Save brain state
- `nlm-utils brain load` - Load brain state
- `nlm-utils brain export` - Export brain data
- `nlm-utils brain compare` - Compare brains

### config commands
- `nlm-utils config presets` - List configuration presets
- `nlm-utils config create` - Create configuration
- `nlm-utils config save` - Save configuration
- `nlm-utils config load` - Load configuration
- `nlm-utils config validate` - Validate configuration
- `nlm-utils config compare` - Compare configurations

### world commands
- `nlm-utils world create` - Create simple world
- `nlm-utils world configure` - Configure world parameters
- `nlm-utils world reset` - Reset world state
- `nlm-utils world analyze` - Analyze world

### agent commands
- `nlm-utils agent create` - Create agent
- `nlm-utils agent initialize` - Initialize agent
- `nlm-utils agent process` - Process sensory input
- `nlm-utils agent decode` - Decode motor command
- `nlm-utils agent train` - Train agent

### experiment commands
- `nlm-utils experiment create` - Create experiment
- `nlm-utils experiment run` - Run experiment
- `nlm-utils experiment add-trial` - Add trial to experiment
- `nlm-utils experiment summary` - Show experiment summary
- `nlm-utils experiment results` - Export experiment results

### viz commands
- `nlm-utils viz dashboard` - Create dashboard
- `nlm-utils viz stats` - Create statistics plot
- `nlm-utils viz network` - Plot network topology
- `nlm-utils viz development` - Plot developmental stages
- `nlm-utils viz spikes` - Plot spike activity

### batch commands
- `nlm-utils batch create` - Create multiple brains
- `nlm-utils batch process` - Process multiple brains
- `nlm-utils batch analyze` - Analyze multiple brains
- `nlm-utils batch export` - Export multiple brains
EOF
fi

echo "✅ Example files created."

# Create documentation
mkdir -p docs/examples

if [ ! -f "docs/examples/README.md" ]; then
    cat > docs/examples/README.md << 'EOF'
# NLM Examples Documentation

This directory contains examples for using the NLM project.

## Basic Examples

- `examples/basic_brain.py`: Simple brain creation and usage
- `examples/agent_simulation.py`: Complete agent simulation
- `examples/config_example.py`: Configuration management examples
- `examples/viz_example.py`: Visualization examples

## Command-Line Examples

- `examples/command_line/README.md`: Command-line tool usage examples

## Getting Started

### For Beginners

1. Run `examples/basic_brain.py` to see basic brain functionality
2. Run `examples/agent_simulation.py` to see complete agent simulation

### For Advanced Users

1. Use `examples/config_example.py` to learn configuration management
2. Use `examples/viz_example.py` to learn visualization capabilities
3. Refer to `examples/command_line/README.md` for command-line tool usage

## Example Outputs

Running the examples will produce:
- Brain statistics output
- Visualization files (PNG format)
- Configuration files (JSON/YAML formats)
- Analysis results
"EOF
fi

echo "✅ Documentation created."

# Set executable permissions
echo "🔧 Setting executable permissions..."
chmod +x nlm-utils
chmod +x nlm

# Create requirements.txt for development
echo "📝 Creating requirements.txt..."
cat > requirements.txt << 'EOF'
# Core NLM dependencies
numpy>=1.20
matplotlib>=3.3
PyYAML>=5.3

# Development dependencies
pytest>=7.0
scikit-build-core>=0.5.0
pybind11>=2.11.0

# Optional utilities
pandas>=1.3
seaborn>=0.11
scikit-learn>=1.0
EOF

# Install as editable package
echo "📥 Finalizing installation..."
pip3 install -e . --quiet

echo "✅ Installation completed successfully!"

echo "\n🎉 Installation Summary:"
echo "   • Core NLM Python bindings installed"
echo "   • Utility modules (utils, viz, config) available"
echo "   • Command-line tools (nlm-utils, nlm) added"
echo "   • Examples and documentation created"
echo "   • Development dependencies installed"

echo "\n🚀 Next Steps:"
echo "1. Run examples: python examples/basic_brain.py"
echo "2. Try command-line tool: nlm-utils --help"
echo "3. See documentation: README.md, README_UTILITIES.md"
echo "4. Explore examples in examples/ directory"

echo "\n🎯 The NLM project is now fully installed and ready for use!"
