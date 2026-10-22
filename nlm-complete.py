#!/usr/bin/env python3
"""
NLM Complete User Interface
============================

Complete interface for NLM (Neural Learning Machine) users of all levels.

This provides:
- Beginner-friendly interface (easy usage)
- Standard interface (command-line tools)
- Advanced interface (power user tools)
- Interactive help and guidance
- Quick access to common operations

Usage:
  nlm-complete [options] <command>

Commands:
  quick        Quick start for beginners
  help         Interactive help and guidance
  tools        Access standard command-line tools
  advanced     Access advanced power user tools
  setup        Setup and configuration assistance
  info         System information and status
"""

import argparse
import subprocess
import sys
import os
from pathlib import Path

class NLMCompleteUI:
    def __init__(self):
        self.project_root = Path(__file__).parent
        
        # Initialize tools
        self.basic_tools = self.project_root / "nlm-tools.py"
        self.advanced_tools = self.project_root / "nlm-advanced.py"
        self.config_dir = self.project_root / "configs"
        self.docs_dir = self.project_root / "docs"
        
    def show_welcome(self):
        """Show welcome message and quick overview."""
        print("=" * 60)
        print("NLM Complete User Interface")
        print("Neural Learning Machine - Phase 6: Final Integration")
        print("=" * 60)
        print()
        print("Welcome to NLM! This is a complete interface for working with")
        print("the NLM (熙然) brain simulation framework.")
        print()
        print("NLM is an experimental artificial developmental brain that")
        print("learns, remembers, and makes decisions through interaction")
        print("with an environment.")
        print()
        
        # Show current status
        self.show_system_status()
        
        print()
        print("Getting Started:")
        print("  1. nlm-complete help                    - Get interactive help")
        print("  2. nlm-complete quick                  - Quick start tutorial")
        print("  3. nlm-config show                     - View configuration")
        print("  4. nlm                                - Run basic simulation")
        print("  5. nlm-advanced batch                  - Run multiple simulations")
        print()
    
    def show_system_status(self):
        """Show system status and information."""
        print("System Status:")
        print(f"  Project Root: {self.project_root}")
        print(f"  Configs: {self.config_dir.exists()}")
        print(f"  Documentation: {self.docs_dir.exists()}")
        print(f"  Basic Tools: {self.basic_tools.exists()}")
        print(f"  Advanced Tools: {self.advanced_tools.exists()}")
        
        # Check for key files
        main_cpp = self.project_root / "src" / "main.cpp"
        brain_hpp = self.project_root / "src" / "brain" / "Brain.hpp"
        config_file = self.config_dir / "default.cfg"
        
        print(f"  Main Brain Code: {main_cpp.exists()}")
        print(f"  Brain Header: {brain_hpp.exists()}")
        print(f"  Default Config: {config_file.exists()}")
        
        if config_file.exists():
            self.check_config_file()
    
    def check_config_file(self):
        """Check configuration file for issues."""
        try:
            with open(self.config_dir / "default.cfg", 'r') as f:
                content = f.read()
            
            # Check for duplicate lines (like the dopamine_baseline issue we fixed)
            lines = content.split('\n')
            seen = set()
            duplicates = []
            
            for i, line in enumerate(lines):
                stripped = line.strip()
                if stripped.startswith('dopamine_baseline ='):
                    if stripped in seen:
                        duplicates.append(i + 1)
                    else:
                        seen.add(stripped)
            
            if duplicates:
                print(f"  ⚠ Configuration issues found in default.cfg:")
                print(f"    - Duplicate dopamine_baseline entries at lines: {duplicates}")
                print(f"    - This has been fixed in the current version.")
            else:
                print(f"  ✓ Configuration file is clean")
                
        except Exception as e:
            print(f"  ✗ Error checking config file: {e}")
    
    def show_help(self):
        """Show interactive help."""
        print("NLM Complete User Interface - Help")
        print("=" * 40)
        print()
        
        help_sections = {
            "beginner": {
                "title": "Beginner Help",
                "content": """
For Beginners:
---------------

1. Quick Start:
   nlm-complete quick
   
2. Basic Usage:
   nlm                  # Run basic simulation
   nlm config show      # View configuration
   nlm config edit --key random_seed --value 123
   
3. Python API:
   import pynlm
   
   # Create a simple brain
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   
   for step in range(100):
       brain.step(step)
   
   print(f"Total neurons: {brain.getTotalNeuronCount()}")
   print(f"Total spikes: {brain.getTotalSpikeCount()}")
   
4. Saving and Loading:
   brain.save("my_brain.checkpoint")
   # ... later ...
   brain.load("my_brain.checkpoint")
   """
            },
            "standard": {
                "title": "Standard Tools",
                "content": """
Standard Command-Line Tools:
----------------------------

Available Commands:
  nlm-config      - Configuration management
    show         - Display configuration
    edit         - Edit configuration values
    generate     - Create configuration template
    
  nlm-inspect     - Brain and system inspection
    brain       - Inspect brain state from checkpoint
    system      - Check system status
    
  nlm-experiment  - Experiment control
    run         - Run experiment with specified config
    status      - Check experiment status
    
  nlm-monitor     - Real-time monitoring
    Basic monitoring of system metrics
    
  nlm-script      - Batch processing and scripting
    Run scripts with parameter support
"""
            },
            "advanced": {
                "title": "Advanced Tools",
                "content": """
Advanced Power User Tools:
--------------------------

Available Commands:
  nlm-advanced batch       - Run multiple simulations in batch mode
  nlm-advanced profile     - Profile simulation performance
  nlm-advanced analyze     - Analyze simulation data
  nlm-advanced compare     - Compare multiple simulation results
  nlm-advanced automate    - Execute automation scripts
  nlm-advanced monitor     - Advanced real-time monitoring
  nlm-advanced export      - Export data for external analysis
  nlm-advanced template    - Generate advanced configuration templates
"""
            },
            "technical": {
                "title": "Technical Information",
                "content": """
Technical Details:
-----------------

Project Structure:
  src/                    - Main C++ source code
  src/core/              - Core utilities
  src/brain/             - Neural components
  src/memory/           - Memory systems
  src/cognition/        - Cognitive functions
  src/sensory/          - Sensory processing
  src/motor/            - Motor system
  src/environment/      - Environment interface
  src/experiments/      - Experiment frameworks
  
  Python Bindings:
  - pynlm                - Python API for NLM
  - PyBind11             - C++ Python bindings
  
  Configuration:
  - configs/             - Configuration files
  - default.cfg          - Default configuration
  
  Documentation:
  - docs/                - Technical documentation
  - README.md            - Project overview
  - easy_usage.md        - Beginner guide
  - HOW_TO_USE.md        - Usage instructions
"""
            }
        }
        
        for section_key, section in help_sections.items():
            print(f"{section['title']}:")
            print(section['content'].strip())
            print()
    
    def show_quick_start(self):
        """Show quick start guide."""
        print("NLM Complete User Interface - Quick Start")
        print("=" * 45)
        print()
        print("This guide gets you started with NLM quickly.")
        print()
        print("Prerequisites:")
        print("  ✓ NLM project installed")
        print("  ✓ C++ compiler with C++20 support")
        print("  ✓ Python 3.8+")
        print()
        print("Step 1: Build NLM")
        print("  mkdir build && cd build")
        print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
        print("  make -j4")
        print()
        print("Step 2: Try Basic Simulation")
        print("  cd ..")
        print("  ./nlm")
        print()
        print("Step 3: Use Python API")
        print("  python3 -c \"import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Neurons:', brain.getTotalNeuronCount())\"")
        print()
        print("Step 4: Explore Further")
        print("  nlm-complete help     # Get more help")
        print("  nlm-config show      # View configuration")
        print("  nlm-inspect system   # Check system status")
        print()
    
    def show_setup_assistance(self):
        """Show setup assistance."""
        print("NLM Complete User Interface - Setup Assistance")
        print("=" * 50)
        print()
        print("This guide helps you set up and configure NLM.")
        print()
        print("Configuration Files:")
        self.show_config_files()
        print()
        print("Build Options:")
        print("  Debug Build:")
        print("    cmake .. -DCMAKE_BUILD_TYPE=Debug")
        print("    make -j4")
        print()
        print("  Release Build:")
        print("    cmake .. -DCMAKE_BUILD_TYPE=Release")
        print("    make -j4")
        print()
        print("Python Installation:")
        print("  pip install pynlm")
        print("  # or")
        print("  pip install -e .")
        print()
        print("Testing:")
        print("  ./nlm_phase6_demo      # Phase 6 integration demo")
        print("  nlm-config show       # Check configuration")
        print("  nlm-inspect system    # Verify system integrity")
        print()
    
    def show_config_files(self):
        """Show configuration files and their purposes."""
        config_files = [
            ("configs/default.cfg", "Default configuration for Phase 6"),
            ("configs/advanced.cfg", "Advanced configuration with all options"),
        ]
        
        for file_path, description in config_files:
            full_path = self.project_root / file_path
            exists = full_path.exists()
            print(f"  {'✓' if exists else '✗'} {file_path}")
            if exists:
                print(f"    {description}")
                self.show_config_content(file_path)
            print()
    
    def show_config_content(self, file_path):
        """Show relevant content from configuration file."""
        try:
            full_path = self.project_root / file_path
            with open(full_path, 'r') as f:
                lines = f.readlines()
            
            # Show key sections
            sections = ['[Basic Settings]', '[Memory Systems]', '[Neuromodulation]', '[Development]']
            
            for section in sections:
                for i, line in enumerate(lines):
                    if section in line:
                        print(f"    {section}:")
                        # Show next 3-5 lines
                        for j in range(i + 1, min(i + 6, len(lines))):
                            if lines[j].strip() and not lines[j].startswith('#'):
                                print(f"      {lines[j].strip()}")
                        break
        except Exception as e:
            print(f"    Error reading file: {e}")
    
    def main(self):
        """Main entry point for complete user interface."""
        parser = argparse.ArgumentParser(
            description="NLM Complete User Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog=__doc__
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Available commands")
        
        # Welcome command
        welcome_parser = subparsers.add_parser("welcome", help="Show welcome message")
        
        # Help command
        help_parser = subparsers.add_parser("help", help="Show interactive help")
        
        # Quick start command
        quick_parser = subparsers.add_parser("quick", help="Show quick start guide")
        
        # Setup command
        setup_parser = subparsers.add_parser("setup", help="Show setup assistance")
        
        # Info command
        info_parser = subparsers.add_parser("info", help="Show system information")
        
        # Tools command
        tools_parser = subparsers.add_parser("tools", help="Access standard tools")
        tools_parser.add_argument("tool_args", nargs="*", help="Arguments for tools")
        
        # Advanced command
        advanced_parser = subparsers.add_parser("advanced", help="Access advanced tools")
        advanced_parser.add_argument("tool_args", nargs="*", help="Arguments for advanced tools")
        
        # Parse arguments
        if len(sys.argv) == 1:
            self.show_welcome()
            return
        
        args = parser.parse_args()
        
        # Route to appropriate functions
        if args.command == "welcome":
            self.show_welcome()
        elif args.command == "help":
            self.show_help()
        elif args.command == "quick":
            self.show_quick_start()
        elif args.command == "setup":
            self.show_setup_assistance()
        elif args.command == "info":
            self.show_system_status()
        elif args.command == "tools":
            self.run_tools(args.tool_args)
        elif args.command == "advanced":
            self.run_advanced_tools(args.tool_args)
    
    def run_tools(self, args):
        """Run basic tools with given arguments."""
        if not args:
            print("Usage: nlm-complete tools <command> [options]")
            print("  Available commands: config, inspect, experiment, monitor, script")
            return
        
        cmd = ["python3", str(self.basic_tools)] + args
        print(f"Running: {' '.join(cmd)}")
        
        try:
            subprocess.run(cmd)
        except Exception as e:
            print(f"Error running tools: {e}")
    
    def run_advanced_tools(self, args):
        """Run advanced tools with given arguments."""
        if not args:
            print("Usage: nlm-complete advanced <command> [options]")
            print("  Available commands: batch, profile, analyze, compare, automate, monitor, export, template")
            return
        
        cmd = ["python3", str(self.advanced_tools)] + args
        print(f"Running: {' '.join(cmd)}")
        
        try:
            subprocess.run(cmd)
        except Exception as e:
            print(f"Error running advanced tools: {e}")

if __name__ == "__main__":
    ui = NLMCompleteUI()
    ui.main()