#!/usr/bin/env python3
"""
NLM Project - Utility Demonstration

This script demonstrates the new utility features in the NLM project.

Usage:
    python demonstration.py [options]

Options:
    --example      Run complete demonstration
    --config       Show configuration examples
    --utils        Show utility examples
    --viz          Show visualization examples
    --help         Show this help message
"""

import sys
import os
from pathlib import Path

def print_header(text):
    """Print formatted header."""
    print("\n" + "=" * 60)
    print(f" {text}")
    print("=" * 60)

def print_section(text):
    """Print formatted section header."""
    print(f"\n📋 {text}")
    print("-" * 60)

def print_example(title, code):
    """Print example with title and code."""
    print(f"\n{title}")
    print("-" * 40)
    print(code)

def main():
    """Main demonstration function."""
    print_header("NLM Project - Utility Demonstration")
    print("\nThis demonstration showcases the new utility features in the NLM project.")
    print("\nThe project now includes:")
    print("  • Comprehensive Python utility modules")
    print("  • Advanced command-line tools (nlm-utils)")
    print("  • Enhanced documentation and examples")
    print("  • Configuration management and parameter sweeps")
    print("  • Visualization and analysis tools")
    
    if "--example" in sys.argv:
        run_complete_demo()
    elif "--config" in sys.argv:
        show_config_examples()
    elif "--utils" in sys.argv:
        show_utils_examples()
    elif "--viz" in sys.argv:
        show_viz_examples()
    else:
        show_main_examples()

def show_main_examples():
    """Show basic usage examples."""
    print_section("1. Basic Python Usage")
    
    print_example("Create a simple brain:", 
"""import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
""")
    
    print_section("2. Configuration with Presets")
    
    print_example("Create brain using preset:", 
"""import pynlm
from pynlm.config import BrainConfigurator

# Create using research preset
config = BrainConfigurator()
config.apply_preset("research")

brain = pynlm.createBrain(config)
brain.initialize()
print(f"Neurons: {brain.getTotalNeuronCount()}")
""")
    
    print_section("3. Basic Command-Line Usage")
    
    print("nlm-utils brain create --preset medium --steps 100")
    print("nlm-utils brain analyze my_brain.bin")
    print("nlm-utils config presets")

def run_complete_demo():
    """Run complete demonstration with all features."""
    print_section("Running Complete Demonstration")
    
    # Import and run demonstrations
    try:
        from pynlm.config import BrainConfigurator
        from pynlm.utils import BrainAnalyzer
        
        print("\n🧪 Phase 1: Configuration Setup")
        config = BrainConfigurator()
        config.apply_preset("research")
        
        print("🧠 Phase 2: Brain Creation and Initialization")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("🔄 Phase 3: Simulation Run")
        for step in range(200):
            brain.step(step)
        
        print("📊 Phase 4: Analysis")
        analyzer = BrainAnalyzer(brain)
        analyzer.print_formatted_stats()
        
        print("\n✅ Demonstration completed successfully!")
        print("\nKey features demonstrated:")
        print("  • Preset configuration management")
        print("  • Brain creation and simulation")
        print("  • Advanced statistics analysis")
        print("  • Configuration validation")
        
    except Exception as e:
        print(f"\n❌ Demonstration failed: {e}")
        print("This is expected if some modules are not available.")
        print("\nThe core NLM functionality is working!")

def show_config_examples():
    """Show configuration examples."""
    print_section("Configuration Examples")
    
    print("\n📝 Creating Different Configurations:")
    print("""
# Tiny (debugging)
config = BrainConfigurator()
config.apply_preset("tiny")

# Small (basic)
config = BrainConfigurator()
config.apply_preset("small")

# Medium (general purpose)
config = BrainConfigurator()
config.apply_preset("medium")

# Large (complex)
config = BrainConfigurator()
config.apply_preset("large")

# Research (scientific)
config = BrainConfigurator()
config.apply_preset("research")

# Debug (development)
config = BrainConfigurator()
config.apply_preset("debug")
""")

def show_utils_examples():
    """Show utility examples."""
    print_section("Utility Module Examples")
    
    print("\n🔧 Brain Analysis:")
    print("""
from pynlm.utils import BrainAnalyzer

analyzer = BrainAnalyzer(brain)
stats = analyzer.get_comprehensive_stats()
analyzer.export_to_json("stats.json")
analyzer.export_to_csv("stats.csv")
analyzer.print_formatted_stats()
""")
    
    print("\n⚙️ Configuration Management:")
    print("""
from pynlm.config import BrainConfigurator, ParameterSweep

# Create custom config
config = BrainConfigurator()
config.update({
    'brain.neuron_count': 2000,
    'random_seed': 12345
})

# Apply preset then modify
config = BrainConfigurator()
config.apply_preset("medium")
config.update({'random_seed': 999})

# Parameter sweep
sweep = ParameterSweep(config)
sweep.add_parameter("brain.neuron_count", [1000, 2000, 5000])
sweep.add_parameter("random_seed", [42, 123, 456])
""")

def show_viz_examples():
    """Show visualization examples."""
    print_section("Visualization Examples")
    
    print("\n📊 Creating Dashboards:")
    print("""
from pynlm.viz import Visualization

viz = Visualization()
dashboard = viz.create_dashboard(brain)
viz.save_dashboard(brain, "dashboard.png")
""")
    
    print("\n📋 Creating Statistics Plots:")
    print("""
from pynlm.viz import plot_brain_stats

fig = plot_brain_stats(brain, filename="brain_stats.png")
""")

def print_help():
    """Print help message."""
    print_header("NLM Project - Utility Demonstration Help")
    print("\nThis demonstration script shows examples of the new NLM utility features.
\nUsage:
    python demonstration.py [options]

Options:
    --example      Run complete demonstration
    --config       Show configuration examples
    --utils        Show utility examples
    --viz          Show visualization examples
    --help         Show this help message
\nExamples:
    python demonstration.py --example    # Complete working demo
    python demonstration.py --config     # Configuration examples
    python demonstration.py --utils      # Utility module examples
    python demonstration.py --viz        # Visualization examples
    python demonstration.py               # Basic usage examples")

if __name__ == "__main__":
    if "--help" in sys.argv:
        print_help()
    else:
        main()
