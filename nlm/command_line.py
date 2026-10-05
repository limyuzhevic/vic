"""
Command-line interface for NLM neural simulation framework.

This module provides command-line tools for interacting with the NLM Python package,
including simulation control, data export/import, and configuration management.
"""

import argparse
import sys
import os
import time
import json
from typing import Dict, Any, Optional, List
import numpy as np

# Import NLM modules
from nlm import create_default_config, validate_config
from nlm.brain import Brain
from nlm.agent import AgentBrain
from nlm.world import SimpleWorld
from nlm.visualization import export_visualization_data, import_visualization_data
from nlm.core import Config

def main():
    """Main command-line interface function."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Python package - Neural Simulation Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-simulate --help                    # Show help
  nlm-simulate run --help                # Show simulation help
  nlm-export --help                     # Show export help
  nlm-import --help                     # Show import help
  nlm-config --help                     # Show configuration help
  nlm-simulate run config.json --steps 1000  # Run simulation
  nlm-export world.json brain.json       # Export world and brain state
  nlm-import --file world_import.json --brain brain_import.json  # Import state
  nlm-config create --file config.py     # Create default configuration
  nlm-config validate --file config.json  # Validate configuration
        """
    )
    
    # Top-level subcommands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Run simulation command
    run_parser = subparsers.add_parser("run", help="Run NLM simulation")
    run_subparsers = run_parser.add_subparsers(dest="run_command", help="Simulation commands")
    
    # Run help
    run_help_parser = run_subparsers.add_parser("help", help="Show simulation help")
    
    # Run with config
    run_config_parser = run_subparsers.add_parser("config", help="Run simulation with configuration file")
    run_config_parser.add_argument("config_file", help="Configuration file path")
    run_config_parser.add_argument("--steps", type=int, help="Number of simulation steps")
    run_config_parser.add_argument("--duration", type=float, help="Simulation duration in seconds")
    run_config_parser.add_argument("--output", help="Output file for simulation results")
    
    # Run with defaults
    run_default_parser = run_subparsers.add_parser("default", help="Run simulation with default configuration")
    run_default_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    run_default_parser.add_argument("--duration", type=float, help="Simulation duration in seconds")
    run_default_parser.add_argument("--output", help="Output file for simulation results")
    
    # Export command
    export_parser = subparsers.add_parser("export", help="Export NLM simulation data")
    export_subparsers = export_parser.add_subparsers(dest="export_command", help="Export commands")
    
    # Export help
    export_help_parser = export_subparsers.add_parser("help", help="Show export help")
    
    # Export world
    export_world_parser = export_subparsers.add_parser("world", help="Export world state")
    export_world_parser.add_argument("world_file", help="World state output file")
    export_world_parser.add_argument("--brain", help="Brain state output file (optional)")
    export_world_parser.add_argument("--format", choices=["json", "csv", "npy"], default="json", help="Export format")
    
    # Export brain
    export_brain_parser = export_subparsers.add_parser("brain", help="Export brain state")
    export_brain_parser.add_argument("brain_file", help="Brain state output file")
    export_brain_parser.add_argument("--world", help="World state output file (optional)")
    export_brain_parser.add_argument("--format", choices=["json", "csv", "npy"], default="json", help="Export format")
    
    # Import command
    import_parser = subparsers.add_parser("import", help="Import NLM simulation data")
    import_subparsers = import_parser.add_subparsers(dest="import_command", help="Import commands")
    
    # Import help
    import_help_parser = import_subparsers.add_parser("help", help="Show import help")
    
    # Import world
    import_world_parser = import_subparsers.add_parser("world", help="Import world state")
    import_world_parser.add_argument("--file", help="World state input file")n    import_world_parser.add_argument("--brain", help="Brain state input file (optional)")
    
    # Import brain
    import_brain_parser = import_subparsers.add_parser("brain", help="Import brain state")
    import_brain_parser.add_argument("--file", help="Brain state input file")
    import_brain_parser.add_argument("--world", help="World state input file (optional)")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Configuration management")
    config_subparsers = config_parser.add_subparsers(dest="config_command", help="Configuration commands")
    
    # Config help
    config_help_parser = config_subparsers.add_parser("help", help="Show configuration help")
    
    # Create configuration
    config_create_parser = config_subparsers.add_parser("create", help="Create configuration file")
    config_create_parser.add_argument("--file", help="Configuration output file")
    config_create_parser.add_argument("--type", choices=["py", "json", "ini", "toml"], default="py", help="Configuration file type")
    
    # Validate configuration
    config_validate_parser = config_subparsers.add_parser("validate", help="Validate configuration file")
    config_validate_parser.add_argument("config_file", help="Configuration file to validate")
    
    # List configuration
    config_list_parser = config_subparsers.add_parser("list", help="List available configuration options")
    
    # Status command
    status_parser = subparsers.add_parser("status", help="Show NLM package status")
    
    # Execute command
    if len(sys.argv) == 1:
        parser.print_help()
        return
    
    # Parse arguments
    args = parser.parse_args()
    
    # Dispatch commands
    try:
        if args.command == "run":
            run_simulation(args)
        elif args.command == "export":
            export_data(args)
        elif args.command == "import":
            import_data(args)
        elif args.command == "config":
            manage_config(args)
        elif args.command == "status":
            show_status(args)
        else:
            parser.print_help()
            
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
def run_simulation(args):
    """
    Run NLM simulation.
    
    Args:
        args: Parsed command-line arguments
    """
    if not hasattr(args, 'run_command') or args.run_command is None:
        print("Error: Please specify a simulation command (help, config, or default)")
        return
    
    try:
        if args.run_command == "help":
            print("\n=== NLM Simulation Commands ===")
            print("  config <file> [--steps STEPS] [--duration DURATION] [--output OUTPUT]")
            print("      Run simulation with configuration file")
            print("  default [--steps STEPS] [--duration DURATION] [--output OUTPUT]")
            print("      Run simulation with default configuration")
            
        elif args.run_command == "config":
            print(f"Running simulation with configuration: {args.config_file}")
            
            # Load configuration
            config = Config()
            if not config.loadFromFile(args.config_file):
                print(f"Error: Failed to load configuration from {args.config_file}")
                return
            
            # Validate configuration
            errors = config.validate()
            if errors:
                print(f"Configuration validation errors:")
                for key, error in errors.items():
                    print(f"  {key}: {error}")
                return
            
            # Override configuration if specified
            if args.steps:
                config.set("simulation_steps", args.steps)
            
            if args.duration:
                config.set("simulation_duration", args.duration)
            
            # Create brain
            brain = Brain(config)
            if not brain.initialize():
                print("Error: Failed to initialize brain")
                return
            
            # Run simulation
            print(f"Running simulation for {config.getInt('simulation_steps', 1000)} steps...")
            
            start_time = time.time()
            for i in range(config.getInt('simulation_steps', 1000)):
                brain.step(i, float(i) * 0.1)  # Simple time step
                
                # Update every 100 steps
                if i % 100 == 0:
                    print(f"  Step {i}/{config.getInt('simulation_steps', 1000)}")
            
            elapsed = time.time() - start_time
            print(f"Simulation completed in {elapsed:.2f} seconds")
            
            # Save output if specified
            if args.output:
                brain.save(args.output)
                print(f"Results saved to {args.output}")
                
        elif args.run_command == "default":
            print("Running simulation with default configuration...")
            
            # Create default configuration
            config = create_default_config()
            
            # Override configuration if specified
            if args.steps:
                config.set("simulation_steps", args.steps)
            
            if args.duration:
                config.set("simulation_duration", args.duration)
            
            # Create brain
            brain = Brain(config)
            if not brain.initialize():
                print("Error: Failed to initialize brain")
                return
            
            # Run simulation
            print(f"Running simulation for {config.getInt('simulation_steps', 1000)} steps...")
            
            start_time = time.time()
            for i in range(config.getInt('simulation_steps', 1000)):
                brain.step(i, float(i) * 0.1)  # Simple time step
                
                # Update every 100 steps
                if i % 100 == 0:
                    print(f"  Step {i}/{config.getInt('simulation_steps', 1000)}")
            
            elapsed = time.time() - start_time
            print(f"Simulation completed in {elapsed:.2f} seconds")
            
            # Save output if specified
            if args.output:
                brain.save(args.output)
                print(f"Results saved to {args.output}")
                
    except Exception as e:
        print(f"Error running simulation: {e}")
        import traceback
        traceback.print_exc()
def export_data(args):
    """
    Export NLM simulation data.
    
    Args:
        args: Parsed command-line arguments
    """
    try:
        if not hasattr(args, 'export_command') or args.export_command is None:
            print("Error: Please specify an export command (help, world, or brain)")
            return
        
        if args.export_command == "help":
            print("\n=== NLM Export Commands ===")
            print("  world <world_file> [--brain brain_file] [--format FORMAT]")
            print("      Export world state to file")
            print("  brain <brain_file> [--world world_file] [--format FORMAT]")
            print("      Export brain state to file")
            
        elif args.export_command == "world":
            print(f"Exporting world state to: {args.world_file}")
            
            # Create a simple world for demonstration
            world = SimpleWorld()
            world.configure(100.0, 100.0, 84, 84)
            
            # Create an agent in the world
            agent_body = world.getAgentBody()
            agent_body.x = 50.0
            agent_body.y = 50.0
            
            # Add some objects
            from nlm.world import WorldObject
            for i in range(5):
                obj = WorldObject(i * 20.0, i * 20.0, radius=5.0, value=float(i))
                world.addObject(obj)
            
            # Export world
            if not export_world_state(world, args.world_file, args.format):
                print(f"Error: Failed to export world state to {args.world_file}")
            else:
                print(f"World state exported successfully to {args.world_file}")
                
        elif args.export_command == "brain":
            print(f"Exporting brain state to: {args.brain_file}")
            
            # Create a simple brain for demonstration
            config = create_default_config()
            brain = Brain(config)
            if not brain.initialize():
                print("Error: Failed to initialize brain")
                return
            
            # Run a few steps for demonstration
            for i in range(100):
                brain.step(i, float(i) * 0.1)
            
            # Export brain
            if not export_brain_state(brain, args.brain_file, args.format):
                print(f"Error: Failed to export brain state to {args.brain_file}")
            else:
                print(f"Brain state exported successfully to {args.brain_file}")
                
    except Exception as e:
        print(f"Error exporting data: {e}")
        import traceback
        traceback.print_exc()
def export_world_state(world, filepath, format="json"):
    """
    Export world state to file.
    
    Args:
        world: World object
        filepath: Output file path
        format: Export format
        
    Returns:
        True if successful, False otherwise
    """
    try:
        if format.lower() == "json":
            # Create world state dictionary
            world_data = {
                "width": world.getWidth(),
                "height": world.getHeight(),
                "agent_x": world.getAgentBody().x,
                "agent_y": world.getAgentBody().y,
                "agent_orientation": world.getAgentBody().orientation,
                "simulation_time": world.getSimulationTime(),
                "max_energy": world.getMaxEnergy(),
                "energy_decay_rate": world.getEnergyDecayRate(),
                "objects": [],
            }
            
            # Add world objects
            for i in range(10):  # Example - should iterate over actual objects
                obj = world.getObject(i)  # This would need to be implemented
                if obj:
                    world_data["objects"].append({
                        "x": obj.x,
                        "y": obj.y,
                        "type": obj.type.name if hasattr(obj.type, 'name') else str(obj.type),
                        "radius": obj.radius,
                        "value": obj.value,
                        "active": obj.active,
                    })
            
            # Save to JSON file
            with open(filepath, 'w') as f:
                json.dump(world_data, f, indent=2)
                
        elif format.lower() == "csv":
            # This would be implemented with pandas
            print(f"CSV export not yet implemented: {filepath}")
            return False
            
        elif format.lower() == "npy":
            # This would be implemented with numpy
            print(f"NPY export not yet implemented: {filepath}")
            return False
            
        return True
        
    except Exception as e:
        print(f"Error exporting world state: {e}")
        return False
def export_brain_state(brain, filepath, format="json"):
    """
    Export brain state to file.
    
    Args:
        brain: Brain object
        filepath: Output file path
        format: Export format
        
    Returns:
        True if successful, False otherwise
    """
    try:
        if format.lower() == "json":
            # Create brain state dictionary
            brain_data = {
                "status": brain.getStatus(),
                "step": brain.getStepCount() if hasattr(brain, 'getStepCount') else 0,
            }
            
            # Save to JSON file
            with open(filepath, 'w') as f:
                json.dump(brain_data, f, indent=2)
                
        elif format.lower() == "csv":
            # This would be implemented with pandas
            print(f"CSV export not yet implemented: {filepath}")
            return False
            
        elif format.lower() == "npy":
            # This would be implemented with numpy
            print(f"NPY export not yet implemented: {filepath}")
            return False
            
        return True
        
    except Exception as e:
        print(f"Error exporting brain state: {e}")
        return False
def import_data(args):
    """
    Import NLM simulation data.
    
    Args:
        args: Parsed command-line arguments
    """
    try:
        if not hasattr(args, 'import_command') or args.import_command is None:
            print("Error: Please specify an import command (help, world, or brain)")
            return
        
        if args.import_command == "help":
            print("\n=== NLM Import Commands ===")
            print("  world [--file file] [--brain brain_file]")
            print("      Import world state from file")
            print("  brain [--file file] [--world world_file]")
            print("      Import brain state from file")
            
        elif args.import_command == "world":
            print("Importing world state...")
            
            if not args.file:
                print("Error: Please specify input file with --file")
                return
            
            # Import world state
            world = import_world_state(args.file, args.brain)
            if world is None:
                print(f"Error: Failed to import world state from {args.file}")
            else:
                print(f"World state imported successfully from {args.file}")
                
        elif args.import_command == "brain":
            print("Importing brain state...")
            
            if not args.file:
                print("Error: Please specify input file with --file")
                return
            
            # Import brain state
            brain = import_brain_state(args.file, args.world)
            if brain is None:
                print(f"Error: Failed to import brain state from {args.file}")
            else:
                print(f"Brain state imported successfully from {args.file}")
                
    except Exception as e:
        print(f"Error importing data: {e}")
        import traceback
        traceback.print_exc()
def import_world_state(filepath, brain_filepath=None):
    """
    Import world state from file.
    
    Args:
        filepath: World state input file
        brain_filepath: Brain state input file (optional)
        
    Returns:
        World object or None if import failed
    """
    try:
        if not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            return None
        
        # Load from JSON file
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Create world configuration
        config = Config()
        
        # Set configuration from import data
        config.set("world_width", data.get("width", 100.0))
        config.set("world_height", data.get("height", 100.0))
        config.set("vision_width", 84)
        config.set("vision_height", 84)
        config.set("max_energy", data.get("max_energy", 100.0))
        config.set("energy_decay_rate", data.get("energy_decay_rate", 0.01))
        
        # Create world
        world = SimpleWorld()
        world.configure(config.getFloat("world_width"), config.getFloat("world_height"),
                       config.getInt("vision_width"), config.getInt("vision_height"))
        
        # Set agent position
        agent_body = world.getAgentBody()
        agent_body.x = data.get("agent_x", 0.0)
        agent_body.y = data.get("agent_y", 0.0)
        
        # Set world objects (placeholder)
        # This would need to be implemented with actual WorldObject class
        
        return world
        
    except Exception as e:
        print(f"Error importing world state: {e}")
        return None
def import_brain_state(filepath, world_filepath=None):
    """
    Import brain state from file.
    
    Args:
        filepath: Brain state input file
        world_filepath: World state input file (optional)
        
    Returns:
        Brain object or None if import failed
    """
    try:
        if not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            return None
        
        # Load from JSON file
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Create configuration
        config = Config()
        
        # Set configuration from import data
        config.set("brain_name", "imported_brain")
        config.set("simulation_steps", data.get("step", 1000))
        
        # Create brain
        brain = Brain(config)
        if not brain.initialize():
            print("Error: Failed to initialize brain")
            return None
        
        # Run simulation steps based on imported data (placeholder)
        # This would need to be implemented with actual step data
        
        return brain
        
    except Exception as e:
        print(f"Error importing brain state: {e}")
        return None
def manage_config(args):
    """
    Manage NLM configuration.
    
    Args:
        args: Parsed command-line arguments
    """
    try:
        if not hasattr(args, 'config_command') or args.config_command is None:
            print("Error: Please specify a configuration command (help, create, validate, or list)")
            return
        
        if args.config_command == "help":
            print("\n=== NLM Configuration Commands ===")
            print("  create [--file file] [--type TYPE]")
            print("      Create configuration file")
            print("  validate <config_file>")
            print("      Validate configuration file")
            print("  list")
            print("      List available configuration options")
            
        elif args.config_command == "create":
            print("Creating configuration file...")
            
            # Create configuration file
            if not create_config_file(args.file, args.type):
                print(f"Error: Failed to create configuration file")
            else:
                print(f"Configuration file created successfully: {args.file}")
                
        elif args.config_command == "validate":
            print(f"Validating configuration file: {args.config_file}")
            
            # Load configuration
            config = Config()
            if not config.loadFromFile(args.config_file):
                print(f"Error: Failed to load configuration from {args.config_file}")
                return
            
            # Validate configuration
            errors = config.validate()
            if errors:
                print(f"Configuration validation errors:")
                for key, error in errors.items():
                    print(f"  {key}: {error}")
                print("Configuration validation failed!")
                return
            else:
                print("Configuration validation passed!")
                
        elif args.config_command == "list":
            print("\n=== Available Configuration Options ===")
            
            # List configuration options
            from nlm.core import DEFAULT_VALIDATION_RULES
            
            for key, rules in DEFAULT_VALIDATION_RULES.items():
                required = " (required)" if rules.get("required", False) else ""
                print(f"  {key}: {rules['type'].__name__}{required}")
                
    except Exception as e:
        print(f"Error managing configuration: {e}")
        import traceback
        traceback.print_exc()
def create_config_file(filepath, file_type="py"):
    """
    Create configuration file.
    
    Args:
        filepath: Output file path
        file_type: File type (py, json, ini, toml)
        
    Returns:
        True if successful, False otherwise
    """
    try:
        if not filepath:
            filepath = "nlm_config.py"
        
        if file_type.lower() == "py":
            # Create Python configuration file
            config_content = '''"""
Default NLM configuration file.

This file contains default configuration values for the NLM Python package.
"""

# Default configuration values
CONFIG = {
    "simulation_steps": 1000,
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": False,
    "thread_safe": False,
    "precision_mode": "single",
    "memory_pool_enabled": False,
    "brain_name": "default_brain",
    "random_seed": 42,
    "max_energy": 100.0,
    "energy_decay_rate": 0.01,
    "vision_width": 84,
    "vision_height": 84,
    "vision_channels": 3,
    "audio_sample_rate": 44100,
    "audio_num_samples": 44100,
}

# Environment variable overrides
ENV_PREFIX = "NLM_"

# File format support
SUPPORTED_FORMATS = ["json", "py", "ini", "cfg", "toml"]
'''
            
        elif file_type.lower() == "json":
            # Create JSON configuration file
            config_content = json.dumps({
                "simulation_steps": 1000,
                "enable_reward_modulation": True,
                "enable_structural_plasticity": True,
                "enable_development": True,
                "enable_curiosity": False,
                "thread_safe": False,
                "precision_mode": "single",
                "memory_pool_enabled": False,
                "brain_name": "default_brain",
                "random_seed": 42,
                "max_energy": 100.0,
                "energy_decay_rate": 0.01,
                "vision_width": 84,
                "vision_height": 84,
                "vision_channels": 3,
                "audio_sample_rate": 44100,
                "audio_num_samples": 44100,
            }, indent=2)
            
        elif file_type.lower() == "ini":
            # Create INI configuration file
            config_content = """[DEFAULT]
simulation_steps = 1000
enable_reward_modulation = true
enable_structural_plasticity = true
enable_development = true
enable_curiosity = false
thread_safe = false
precision_mode = single
memory_pool_enabled = false
brain_name = default_brain
random_seed = 42
max_energy = 100.0
energy_decay_rate = 0.01
vision_width = 84
vision_height = 84
vision_channels = 3
audio_sample_rate = 44100
audio_num_samples = 44100
"""
        
        elif file_type.lower() == "toml":
            # Create TOML configuration file
            config_content = """[nlm.config]
simulation_steps = 1000
enable_reward_modulation = true
enable_structural_plasticity = true
enable_development = true
enable_curiosity = false
thread_safe = false
precision_mode = "single"
memory_pool_enabled = false
brain_name = "default_brain"
random_seed = 42
max_energy = 100.0
energy_decay_rate = 0.01
vision_width = 84
vision_height = 84
vision_channels = 3
audio_sample_rate = 44100
audio_num_samples = 44100
"""
        
        else:
            print(f"Error: Unsupported file type: {file_type}")
            return False
        
        # Write to file
        with open(filepath, 'w') as f:
            f.write(config_content)
        
        return True
        
    except Exception as e:
        print(f"Error creating configuration file: {e}")
        return False
def show_status(args):
    """
    Show NLM package status.
    
    Args:
        args: Parsed command-line arguments
    """
    print("\n=== NLM Package Status ===")
    
    # Check Python version
    print(f"Python version: {sys.version}")
    
    # Check numpy version
    try:
        import numpy as np
        print(f"NumPy version: {np.__version__}")
    except ImportError:
        print("NumPy: Not installed")
    
    # Check package version
    try:
        from nlm.version import __version__
        print(f"NLM package version: {__version__}")
    except ImportError:
        print("NLM package version: Not available")
    
    # Check module availability
    modules = [
        ("nlm.core", "Core module"),
        ("nlm.brain", "Brain module"),
        ("nlm.agent", "Agent module"),
        ("nlm.world", "World module"),
        ("nlm.visualization", "Visualization module"),
    ]
    
    print("\nModule availability:")
    for module_name, module_desc in modules:
        try:
            __import__(module_name)
            print(f"  ✓ {module_desc}")
        except ImportError:
            print(f"  ✗ {module_desc}")
    
    # Check directory structure
    print("\nDirectory structure:")
    base_dir = os.path.dirname(os.path.abspath(__file__))
    for item in sorted(os.listdir(base_dir)):
        if os.path.isdir(os.path.join(base_dir, item)):
            print(f"  {item}/")
        elif item.endswith('.py'):
            print(f"  {item}")
    
    print("\nNLM package is ready for use!")
    print("\nNext steps:")
    print("  nlm-simulate run help                    # Show simulation help")
    print("  nlm-export help                         # Show export help")
    print("  nlm-import help                         # Show import help")
    print("  nlm-config create --help                # Show configuration creation help")
    print("  nlm-status                              # Show package status")
if __name__ == "__main__":
    main()
