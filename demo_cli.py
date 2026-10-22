#!/usr/bin/env python3
"""
NLM CLI Demonstration Script

Demonstrates the functionality of the NLM CLI tool.
This script shows basic usage and commands.
"""

import sys
import os
from pathlib import Path

# Add the project root to Python path
sys.path.insert(0, str(Path(__file__).parent))

from cli_utils import load_config, create_brain, run_simulation, format_stats, NLMCLIError

def demo_basic_usage():
    """Demonstrate basic CLI usage"""
    print("=== NLM CLI Demonstration ===\n")
    
    print("1. Creating a simple brain with default configuration...")
    try:
        brain = create_brain()
        print("   ✓ Brain created successfully")
    except Exception as e:
        print(f"   ✗ Error creating brain: {e}")
        return
    
    print("\n2. Showing brain statistics...")
    try:
        stats = format_stats(brain)
        print(stats)
    except Exception as e:
        print(f"   ✗ Error showing stats: {e}")
    
    print("\n3. Running a short simulation (10 steps)...")
    try:
        results = run_simulation(brain, 10, verbose=False)
        print(f"   ✓ Simulation completed:")
        print(f"     - Steps: {results['steps']}")
        print(f"     - Spikes generated: {results['spike_increase']}")
        print(f"     - Average firing rate: {results['average_firing_rate']:.3f}")
    except Exception as e:
        print(f"   ✗ Error running simulation: {e}")
    
    print("\n4. Demonstrating configuration file handling...")
    try:
        # Try to load a config file
        config = load_config("configs/default.cfg")
        print(f"   ✓ Loaded config with {len(config)} key-value pairs")
        print(f"   Sample config: {dict(list(config.items())[:3])}")
    except NLMCLIError as e:
        print(f"   ! Config file not found (expected): {e}")
    except Exception as e:
        print(f"   ✗ Error loading config: {e}")
    
    print("\n=== Demonstration Complete ===")
    print("\nNext steps:")
    print("  python nlm_cli.py run --steps 100")
    print("  python nlm_cli.py config show")
    print("  python nlm_cli.py stats")
    print("  python nlm_cli.py connectivity")
    print("  python nlm_cli.py --help")

def demo_interactive():
    """Demonstrate interactive mode"""
    print("\n=== Interactive Mode Demonstration ===\n")
    
    try:
        # Import and run interactive command
        from cli_commands import InteractiveCommand
        
        # Create a mock args object
        class MockArgs:
            config = "configs/default.cfg"
        
        # Create command instance
        command = InteractiveCommand(MockArgs(), debug=False)
        
        print("Starting interactive mode...")
        print("Commands available: status, stats, step <N>, spikes, firing, help")
        print("Press Ctrl+C to exit")
        
        # Note: In a real demo, we would need to handle the interactive loop
        print("\n(Note: Full interactive demo requires user input)")
        
    except ImportError as e:
        print(f"   ✗ Could not import interactive command: {e}")
    except Exception as e:
        print(f"   ✗ Error in interactive demo: {e}")

def demo_configuration_commands():
    """Demonstrate configuration commands"""
    print("\n=== Configuration Commands Demo ===\n")
    
    print("1. Default configuration:")
    try:
        from cli_utils import validate_config
        
        default_config = {
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.1,
            "simulation_timestep": 0.001,
            "random_seed": 42
        }
        
        validated = validate_config(default_config)
        print(f"   ✓ Validated config:")
        for key, value in validated.items():
            print(f"     {key}: {value}")
    
    except Exception as e:
        print(f"   ✗ Error: {e}")
    
    print("\n2. JSON configuration support:")
    try:
        import json
        json_config = {
            "simulation": {
                "steps": 1000,
                "timestep": 0.001
            },
            "brain": {
                "neurons": 1000,
                "regions": 2
            }
        }
        
        # Demonstrate YAML-like formatting (basic)
        print("   ✓ Config can be saved in JSON/YAML format")
        print(f"   Example JSON keys: {list(json_config.keys())}")
    
    except Exception as e:
        print(f"   ✗ Error: {e}")

def demo_error_handling():
    """Demonstrate error handling"""
    print("\n=== Error Handling Demo ===\n")
    
    print("1. Invalid configuration:")
    try:
        from cli_utils import validate_config
        
        invalid_config = {
            "neuron_count": -1,  # Invalid: negative
            "connection_probability": 1.5  # Invalid: > 1
        }
        
        validated = validate_config(invalid_config)
        print("   ✗ Should have raised error for invalid config")
    
    except NLMCLIError as e:
        print(f"   ✓ Caught expected error: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")
    
    print("\n2. Missing configuration file:")
    try:
        load_config("nonexistent_file.txt")
        print("   ✗ Should have raised error for missing file")
    except NLMCLIError as e:
        print(f"   ✓ Caught expected error: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected error: {e}")

def main():
    """Main demonstration function"""
    print_banner()
    
    # Check if pynlm is available
    try:
        import pynlm
        if not pynlm:
            print("ERROR: pynlm module is not available")
            sys.exit(1)
    except ImportError as e:
        print(f"ERROR: Could not import pynlm: {e}")
        print("Make sure NLM is properly installed: pip install -e .")
        sys.exit(1)
    
    # Run demonstrations
    demo_basic_usage()
    demo_configuration_commands()
    demo_error_handling()
    
    print("\n=== All Demos Complete ===")
    print("\nThe NLM CLI tool is ready for use!")
    print("\nTo use the CLI tool:")
    print("  python nlm_cli.py --help")
    print("  python nlm_cli.py run --steps 100")
    print("  python nlm_cli.py config show")

def print_banner():
    """Print the NLM CLI banner"""
    print("╔══════════════════════════════════════════════════════════════╗")
    print("║                                                              ║")
    print("║     NLM — 熙然                                                ║")
    print("║     Neural Learning Machine                                   ║")
    print("║                                                              ║")
    print("║     CLI Demonstration                                         ║")
    print("║                                                              ║")
    print("╚══════════════════════════════════════════════════════════════╝")
    print()

if __name__ == "__main__":
    main()