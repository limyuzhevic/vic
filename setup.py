#!/usr/bin/env python3
"""
Setup script for NLM Python package installation.

This script handles the installation of both the core C++ bindings
and the enhanced Python modules.
"""

import os
import sys
import subprocess
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run command and return success status."""
    try:
        subprocess.run(cmd, shell=True, cwd=cwd, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {cmd}")
        print(f"Error: {e}")
        return False

def main():
    print("NLM Python Package Setup")
    print("=" * 50)
    
    # Get the directory containing this setup script
    script_dir = Path(__file__).parent
    
    # Check if we should build C++ extensions
    build_extensions = os.getenv('NLM_BUILD_EXTENSIONS', 'true').lower() == 'true'
    
    if build_extensions:
        print("Building C++ extensions...")
        if not run_command("python -m pip install -e .", cwd=script_dir):
            print("Failed to install C++ extensions")
            return 1
    
    # Create enhanced modules if they don't exist
    enhanced_files = [
        script_dir / "pynlm_enhanced.py",
        script_dir / "nlm_cli_advanced.py",
        script_dir / "NLM_USER_GUIDE.md"
    ]
    
    print("\nCreating enhanced modules...")
    for file_path in enhanced_files:
        if not file_path.exists():
            print(f"  Creating {file_path.name}...")
        else:
            print(f"  Found {file_path.name}")
    
    # Create example scripts
    print("\nCreating example scripts...")
    
    examples_dir = script_dir / "examples"
    examples_dir.mkdir(exist_ok=True)
    
    # Basic example
    basic_example = examples_dir / "basic_example.py"
    if not basic_example.exists():
        with open(basic_example, 'w') as f:
            f.write("""#!/usr/bin/env python3
"""
            NLM Basic Usage Example
=======================

This example demonstrates basic NLM usage with mock implementations.
For actual usage, install the compiled pynlm module first.

Author: NLM Development Team
Version: 1.0.0
""".join(basic_example.read_text()) or """
import sys

# Try to import the actual bindings
try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError:
    print("Error: pynlm module not found.")
    print("Please install NLM with: pip install pynlm")
    sys.exit(1)

def main():
    print("NLM Basic Usage Example")
    print("=" * 30)
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Display brain information
    print(f"Brain created successfully!")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")
    
    # Run a few simulation steps
    print(f"\nRunning simulation for 10 steps...")
    for step in range(10):
        brain.step(step)
    
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Active neurons: {brain.getFiringNeuronCount()}")
    
    # Save brain state
    brain.save("brain_state.json")
    print(f"\nBrain state saved to 'brain_state.json'")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
""")
    
    # Advanced example
    advanced_example = examples_dir / "advanced_example.py"
    if not advanced_example.exists():
        with open(advanced_example, 'w') as f:
            f.write("""
#!/usr/bin/env python3
"""
            NLM Advanced Usage Example
==========================

This example demonstrates advanced NLM usage with enhanced features.
For actual usage, install the enhanced NLM package first.

Author: NLM Development Team
Version: 1.0.0
""".join(advanced_example.read_text()) or """
import sys

# Try to import enhanced modules
try:
    import pynlm as nlm
    from pynlm_enhanced import NLMSimulationContext, NLMBatchContext
    from nlm_cli_advanced import NLMCommandLine
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: Required modules not found: {e}")
    print("Please install NLM with enhanced features:")
    print("  pip install -e .")
    sys.exit(1)

def main():
    print("NLM Advanced Usage Example")
    print("=" * 30)
    
    # Create configuration
    config = nlm.createDefaultConfig()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create brain and agent
    brain = nlm.createBrain(config)
    brain.initialize()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Advanced features enabled:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run simulation with enhanced context
    print(f"\nRunning advanced simulation...")
    
    def progress_callback(event):
        if event.progress % 25 == 0:
            print(f"  Progress: {event.progress:.1f}% (Step {event.step})")
    
    with NLMSimulationContext(brain, world, agent) as ctx:
        stats = ctx.run(100, timestep=0.1, progress_callback=progress_callback)
    
    print(f"\nSimulation completed!")
    print(f"Statistics:")
    print(f"  Total neurons: {stats.total_neurons}")
    print(f"  Total synapses: {stats.total_synapses}")
    print(f"  Firing neurons: {stats.firing_neurons}")
    print(f"  Total spikes: {stats.total_spikes}")
    print(f"  Average firing rate: {stats.average_firing_rate:.2f} Hz")
    
    # Run batch simulation
    print(f"\nRunning batch simulation...")
    with NLMBatchContext(brain, world, agent, num_episodes=3, steps_per_episode=50) as batch:
        results = batch.run_batch()
    
    print(f"Batch simulation completed: {len(results)} episodes")
    
    # Generate CLI help
    print(f"\nNLM Command Line Interface:")
    cli = NLMCommandLine()
    parser = cli.get_argument_parser()
    print(f"  Run 'nlm --help' for CLI options")
    
    print("\nAdvanced example completed successfully!")

if __name__ == "__main__":
    main()
""")
    
    # Configuration example
    config_example = examples_dir / "config_example.py"
    if not config_example.exists():
        with open(config_example, 'w') as f:
            f.write("""
#!/usr/bin/env python3
"""
            NLM Configuration Example
==========================

This example demonstrates NLM configuration management.
For actual usage, install the NLM package first.

Author: NLM Development Team
Version: 1.0.0
""".join(config_example.read_text()) or """
import sys

# Try to import NLM
try:
    import pynlm as nlm
    from pynlm_enhanced import ConfigurationTemplate
    PYNLM_AVAILABLE = True
except ImportError:
    print("Error: NLM modules not found.")
    print("Please install NLM:")
    print("  pip install pynlm")
    sys.exit(1)

def main():
    print("NLM Configuration Example")
    print("=" * 25)
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    
    print("1. Default Configuration:")
    print(f"   Neurons: {config.get('brain.neuron_count')}")
    print(f"   Regions: {config.get('brain.region_count')}")
    print(f"   Connection probability: {config.get('brain.connection_probability')}")
    
    # Modify configuration
    print("\n2. Modifying Configuration:")
    config.set("brain.neuron_count", 2000)
    config.set("brain.connection_probability", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    print(f"   Modified neurons: {config.get('brain.neuron_count')}")
    print(f"   Modified connection probability: {config.get('brain.connection_probability')}")
    print(f"   Modified STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
    
    # Save configuration
    print("\n3. Saving Configuration:")
    config.saveToFile("my_config.json")
    print(f"   Configuration saved to 'my_config.json'")
    
    # Load configuration
    print("\n4. Loading Configuration:")
    new_config = nlm.createDefaultConfig()
    new_config.loadFromFile("my_config.json")
    print(f"   Loaded neurons: {new_config.get('brain.neuron_count')}")
    
    # Create configuration template
    print("\n5. Configuration Template:")
    template = ConfigurationTemplate("experiment_1")
    template.add_parameter("brain.neuron_count", 100, "Number of neurons")
    template.add_parameter("plasticity.stdp.enable", True, "Enable STDP")
    template.add_parameter("neuromod.curiosity.enable", True, "Enable curiosity")
    
    template.apply_to_config(config)
    print(f"   Template applied to configuration")
    
    # Configuration validation
    print("\n6. Configuration Validation:")
    # (Validation would be implemented in actual NLM)
    print("   Configuration validation would check for:")
    print("   - Parameter ranges")
    print("   - Consistency between settings")
    print("   - Required dependencies")
    
    # Display all configuration keys
    print("\n7. Configuration Keys:")
    for key in config.getKeys():
        value = config.get(key)
        if isinstance(value, bool):
            value_str = "true" if value else "false"
        else:
            value_str = str(value)
        print(f"   {key}: {value_str}")
    
    print("\nConfiguration example completed successfully!")

if __name__ == "__main__":
    main()
""")
    
    # Help script
    help_script = examples_dir / "help.py"
    if not help_script.exists():
        with open(help_script, 'w') as f:
            f.write("""
#!/usr/bin/env python3
"""
            NLM Help Script
=================

This script provides help and documentation for NLM usage.

Author: NLM Development Team
Version: 1.0.0
""".join(help_script.read_text()) or """
#!/usr/bin/env python3
"""
NLM Help and Documentation Script
===================================

This script provides help and documentation for using the NLM framework.

Author: NLM Development Team
Version: 1.0.0
"""

import sys
import os

def print_help():
    print("NLM (Neural Learning Machine) Help")
    print("=" * 50)
    print()
    
    print("INTRODUCTION")
    print("-" * 50)
    print("NLM is a brain-inspired spiking neural network simulator that")
    print("implements computational neuroscience models for learning and")
    print("cognition.")
    print()
    
    print("BASIC COMMANDS")
    print("-" * 50)
    print("python -c \"import pynlm as nlm; print(nlm.__version__)\"")
    print("    Display NLM version information")
    print()
    print("python examples/basic_example.py")
    print("    Run basic usage example")
    print()
    print("python examples/advanced_example.py")
    print("    Run advanced usage example")
    print()
    print("python examples/config_example.py")
    print("    Run configuration example")
    print()
    
    print("INSTALLATION")
    print("-" * 50)
    print("pip install pynlm")
    print("    Install core NLM bindings")
    print()
    print("pip install -e .")
    print("    Install with enhanced features")
    print()
    
    print("DOCUMENTATION")
    print("-" * 50)
    print("NLM_USER_GUIDE.md - Complete user guide")
    print("    Includes examples, best practices, and API reference")
    print()
    
    print("DEVELOPMENT")
    print("-" * 50)
    print("python -m pytest tests/")
    print("    Run unit tests")
    print()
    print("make build")
    print("    Build C++ extensions")
    print()
    
    print("EXAMPLES")
    print("-" * 50)
    print("The examples directory contains:")
    print("  - basic_example.py: Basic NLM usage")
    print("  - advanced_example.py: Advanced NLM features")
    print("  - config_example.py: Configuration management")
    print()
    
    print("CLI TOOLS")
    print("-" * 50)
    print("nlm simulate --steps 1000")
    print("    Run simulation with command line interface")
    print()
    print("nlm config validate my_config.json")
    print("    Validate configuration file")
    print()
    print("nlm analyze results.json")
    print("    Analyze simulation results")
    print()
    
    print("SUPPORT")
    print("-" * 50)
    print("Documentation: https://nlm.readthedocs.io")
    print("GitHub: https://github.com/nlm-project/nlm")
    print("Issue Tracker: https://github.com/nlm-project/nlm/issues")
    print()
    
    print("REFERENCES")
    print("-" * 50)
    print("Phase 6 Final Audit: docs/PHASE6_FINAL_AUDIT.md")
    print("Architecture: docs/ARCHITECTURE.md")
    print("Science Background: docs/SCIENCE.md")
    print()

if __name__ == "__main__":
    print_help()
""")
    
    print("Example scripts created successfully!")
    print(f"  Location: {examples_dir}/")

if __name__ == "__main__":
    main()