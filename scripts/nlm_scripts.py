#!/usr/bin/env python3
"""
NLM Script Loader/Saver
Script management and execution for Neural Learning Machine

This script provides utilities for:
- Loading and saving NLM configurations
- Creating and managing experiment scripts
- Batch processing multiple configurations
- Script validation and testing
"""

import os
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, Any, List, Optional

class NLMScriptManager:
    def __init__(self, scripts_dir: str = "scripts"):
        self.scripts_dir = Path(scripts_dir)
        self.scripts_dir.mkdir(exist_ok=True)
        
        # Script templates
        self.templates = {
            "basic": """#!/usr/bin/env python3
import pynlm

# Basic NLM script template
def run_basic_simulation():
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally configure specific parameters
    # config.set("brain.neuron_count", 1000)
    # config.set("brain.synapse_density", 0.1)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Simulation complete!")
    return brain

if __name__ == "__main__":
    run_basic_simulation()
""",

            "advanced": """#!/usr/bin/env python3
import pynlm
import time

def run_advanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
    return brain, world, agent

if __name__ == "__main__":
    run_advanced_agent_simulation()
""",

            "performance_test": """#!/usr/bin/env python3
import pynlm
import time
import statistics

def benchmark_configuration(config, description, steps=1000):
    """Benchmark a specific configuration."""
    print(f"\nBenchmarking: {description}")
    print(f"Configuration: neuron_count={config.getOr('neuron_count', 1000)}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm-up
    for i in range(100):
        brain.step(i)
    
    # Benchmark
    start_time = time.time()
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"  Time: {total_time:.2f}s")
    print(f"  Steps/sec: {steps / total_time:.0f}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    return {
        'time': total_time,
        'steps_per_sec': steps / total_time,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount()
    }

def run_performance_comparison():
    """Compare performance across different configurations."""
    
    # Different configurations to test
    configs = [
        pynlm.createDefaultConfig(),
        pynlm.createDefaultConfig().set("brain.neuron_count", 500),
        pynlm.createDefaultConfig().set("brain.neuron_count", 2000),
        pynlm.createDefaultConfig().set("brain.synapse_density", 0.01),
        pynlm.createDefaultConfig().set("brain.synapse_density", 0.2)
    ]
    
    descriptions = [
        "Default configuration",
        "500 neurons",
        "2000 neurons",
        "Low synapse density",
        "High synapse density"
    ]
    
    print("NLM Performance Comparison")
    print("=" * 50)
    
    results = []
    for config, description in zip(configs, descriptions):
        result = benchmark_configuration(config, description)
        results.append(result)
    
    print("\nPerformance Summary:")
    print("=" * 30)
    for i, (result, description) in enumerate(zip(results, descriptions)):
        print(f"{i+1}. {description:25s} {result['steps_per_sec']:10.0f} steps/sec")

if __name__ == "__main__":
    run_performance_comparison()
""",

            "validation_test": """#!/usr/bin/env python3
import pynlm
import sys

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("Testing basic NLM functionality...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic properties
    assert brain.getTotalNeuronCount() > 0, "Should have neurons"
    assert brain.getTotalSynapseCount() >= 0, "Should have synapses"
    
    # Test basic operations
    initial_spikes = brain.getTotalSpikeCount()
    for i in range(10):
        brain.step(i)
    
    # Verify brain state changed
    assert brain.getTotalSpikeCount() >= initial_spikes, "Should accumulate spikes"
    
    print("✓ Basic functionality test passed!")

def test_agent_world_interaction():
    """Test agent-world interaction."""
    print("Testing agent-world interaction...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent.initialize(world)
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test motor output
    brain.step(0)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify interaction
    assert action is not None, "Should produce action"
    
    print("✓ Agent-world interaction test passed!")

def test_configuration_management():
    """Test configuration management."""
    print("Testing configuration management...")
    
    # Create default config
    config = pynlm.createDefaultConfig()
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    
    # Create brain with new config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify configuration was applied
    assert brain.getTotalNeuronCount() == 2000, "Neuron count should be 2000"
    
    # Test file I/O
    config.saveToFile("test_config.json")
    
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("test_config.json")
    
    assert config2.getOr("brain.neuron_count", 0) == 2000, "Config should load correctly"
    
    # Clean up
    import os
    if os.path.exists("test_config.json"):
        os.remove("test_config.json")
    
    print("✓ Configuration management test passed!")

def run_all_tests():
    """Run all validation tests."""
    print("Running NLM Validation Tests")
    print("=" * 40)
    
    try:
        test_basic_functionality()
        test_agent_world_interaction()
        test_configuration_management()
        
        print("\n✅ All tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed: {e}")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
"""
        }

    def create_script(self, name: str, template: str = "basic"):
        """Create a new script from template."""
        if template not in self.templates:
            print(f"Unknown template: {template}")
            return False
        
        script_content = self.templates[template]
        script_path = self.scripts_dir / f"{name}.py"
        
        with open(script_path, 'w') as f:
            f.write(script_content)
        
        # Make executable
        os.chmod(script_path, 0o755)
        
        print(f"✓ Created script: {script_path}")
        return True

    def load_script(self, name: str):
        """Load script content from file."""
        script_path = self.scripts_dir / f"{name}.py"
        if not script_path.exists():
            print(f"Script not found: {script_path}")
            return None
        
        with open(script_path, 'r') as f:
            return f.read()

    def list_scripts(self):
        """List available scripts."""
        scripts = []
        for file_path in self.scripts_dir.glob("*.py"):
            scripts.append(file_path.stem)
        
        return sorted(scripts)

    def batch_load_configs(self, config_dir: str):
        """Load multiple configuration files."""
        configs = {}
        config_dir_path = Path(config_dir)
        
        if not config_dir_path.exists():
            print(f"Config directory not found: {config_dir}")
            return configs
        
        for file_path in config_dir_path.glob("*.json"):
            name = file_path.stem
            try:
                with open(file_path, 'r') as f:
                    configs[name] = json.load(f)
                print(f"✓ Loaded config: {name}")
            except Exception as e:
                print(f"✗ Error loading {name}: {e}")
        
        return configs

    def generate_config_from_script(self, script_name: str):
        """Generate configuration file from script analysis."""
        script_content = self.load_script(script_name)
        if not script_content:
            return
        
        # Extract configuration settings from script (simple regex)
        import re
        
        config_settings = {}
        
        # Look for config.set calls
        config_set_pattern = r'config\.set\(["\']([^"\']+)["\']\s*,\s*([^)]+)\)'
        matches = re.findall(config_set_pattern, script_content)
        
        for key, value in matches:
            # Clean up value
            value = value.strip()
            if value.startswith("'") and value.endswith("'"):
                value = value[1:-1]
            elif value.startswith('"') and value.endswith('"'):
                value = value[1:-1]
            
            config_settings[key] = value
        
        # Generate config file
        if config_settings:
            filename = f"{script_name}_config.json"
            with open(self.scripts_dir / filename, 'w') as f:
                json.dump(config_settings, f, indent=2)
            
            print(f"✓ Generated config file: {filename}")

class ScriptManager:
    def __init__(self, manager: NLMScriptManager):
        self.manager = manager

    def create(self, args: argparse.Namespace):
        """Create a new script."""
        if not self.manager.create_script(args.name, args.template):
            return False

    def list(self):
        """List available scripts."""
        scripts = self.manager.list_scripts()
        print("Available scripts:")
        for script in scripts:
            print(f"  - {script}")

    def load(self, args: argparse.Namespace):
        """Load script content."""
        content = self.manager.load_script(args.script)
        if content:
            print(f"\n--- {args.script} ---")
            print(content)

    def generate_config(self, args: argparse.Namespace):
        """Generate configuration from script."""
        self.manager.generate_config_from_script(args.script)

def main():
    parser = argparse.ArgumentParser(
        description="NLM Script Manager",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Create script
    create_parser = subparsers.add_parser("create", help="Create new script")
    create_parser.add_argument("name", help="Script name")
    create_parser.add_argument("template", nargs="?", default="basic",
                              choices=["basic", "advanced", "performance_test", "validation_test"],
                              help="Script template")

    # List scripts
    subparsers.add_parser("list", help="List available scripts")

    # Load script
    load_parser = subparsers.add_parser("load", help="Load script content")
    load_parser.add_argument("script", help="Script name")

    # Generate config
    generate_parser = subparsers.add_parser("generate-config", help="Generate config from script")
    generate_parser.add_argument("script", help="Script name")

    # Batch load configs
    batch_parser = subparsers.add_parser("batch-load", help="Load multiple configuration files")
    batch_parser.add_argument("config_dir", help="Configuration directory")

    args = parser.parse_args()

    manager = NLMScriptManager()
    script_manager = ScriptManager(manager)

    if args.command == "create":
        script_manager.create(args)
    elif args.command == "list":
        script_manager.list()
    elif args.command == "load":
        script_manager.load(args)
    elif args.command == "generate-config":
        script_manager.generate_config(args)
    elif args.command == "batch-load":
        manager.batch_load_configs(args.config_dir)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
