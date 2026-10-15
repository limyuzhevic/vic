#!/usr/bin/env python3
"""
NLM Command Line Interface - Entry point for NLM CLI tools.

This script provides the main entry point for accessing NLM's command-line
interface through the Python package. It delegates to specific CLI modules
for different functionalities.
"""

import sys
import os

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def main():
    """Main entry point."""
    print("NLM Command Line Interface")
    print("=" * 50)
    print("Usage: nlm-command [OPTIONS]")
    print()
    print("Available commands:")
    print("  help           Show help information")
    print("  version        Show version information")
    print("  config         Configuration management")
    print("  brain          Brain operations")
    print("  agent          Agent operations")
    print("  world          World operations")
    print("  explore        Interactive exploration")
    print("  demo           Run demonstrations")
    print()
    
    if len(sys.argv) < 2:
        print("No command specified. Use 'help' for more information.")
        return 1
    
    command = sys.argv[1]
    
    # Map commands to functions
    commands = {
        "help": show_help,
        "version": show_version,
        "config": config_command,
        "brain": brain_command,
        "agent": agent_command,
        "world": world_command,
        "explore": explore_command,
        "demo": demo_command,
    }
    
    if command not in commands:
        print(f"Unknown command: {command}")
        print("Use 'help' for available commands.")
        return 1
    
    return commands[command]()


def show_help():
    """Show help information."""
    print("NLM Command Line Interface - Help")
    print("=" * 50)
    print("\nGeneral Commands:")
    print("  help           Show this help message")
    print("  version        Show version information")
    print("  help [topic]   Get help on specific topic")
    
    print("\nConfiguration Commands:")
    print("  config list                    List all configuration keys")
    print("  config get <key> [default]    Get configuration value")
    print("  config set <key> <value>      Set configuration value")
    print("  config validate                Validate configuration")
    print("  config reset                   Reset configuration")
    
    print("\nBrain Commands:")
    print("  brain create <config>          Create a new brain")
    print("  brain initialize              Initialize brain")
    print("  brain step <steps> [time]      Run brain steps")
    print("  brain reset                    Reset brain state")
    print("  brain status                   Show brain status")
    print("  brain save <file>             Save brain state")
    print("  brain load <file>             Load brain state")
    
    print("\nAgent Commands:")
    print("  agent create <brain>           Create an agent")
    print("  agent initialize <world>       Initialize agent with world")
    print("  agent process <percept>        Process sensory input")
    print("  agent decode                   Decode motor command")
    print("  agent reward <r> [p]          Apply reward modulation")
    print("  agent reset                    Reset agent")
    
    print("\nWorld Commands:")
    print("  world create                   Create a world")
    print("  world configure <w> <h> [vw] [vh] Configure world")
    print("  world reset                    Reset world")
    print("  world update <time>            Update world")
    print("  world percept                 Get sensory percept")
    print("  world apply <action> [time]   Apply motor command")
    print("  world status                   Show world status")
    
    print("\nExploration Commands:")
    print("  explore interactive [steps] [neurons] Start interactive exploration")
    print("  explore step <steps>           Run exploration steps")
    print("  explore explore <command>      Send exploration command")
    
    print("\nDemo Commands:")
    print("  demo basic [steps] [neurons]     Run basic demo")
    print("  demo agent [steps] [width] [height] Run agent demo")
    print("  demo memory [steps] [neurons]    Run memory demo")
    print("  demo plasticity [steps] [neurons] Run plasticity demo")
    print("  demo development [steps] [neurons] Run development demo")
    print("  demo prediction [steps] [neurons] Run prediction demo")
    print("  demo comprehensive [steps] [neurons] Run comprehensive demo")
    print("  demo list                        List all available demos")
    
    print("\nTest Commands:")
    print("  test all                        Run all tests")
    print("  test basic                     Run basic tests")
    print("  test explore                   Run exploration tests")
    print("  test demo                      Run demo tests")
    print("  test integration                Run integration tests")


def show_version():
    """Show version information."""
    print("NLM Command Line Interface")
    print("=" * 30)
    print("Version: 0.1.0")
    print("Build: Development")
    print("Python: 3.8+")
    print("Author: NLM Project")
    print()
    print("For more information, visit: https://github.com/nlm-project/nlm")


def config_command():
    """Configuration command handler."""
    if len(sys.argv) < 3:
        print("Configuration subcommand required")
        print("Use 'config list', 'config get', 'config set', etc.")
        return 1
    
    subcommand = sys.argv[2]
    
    # Import config functionality
    try:
        from pynlm.config.cli import config_cli
        return config_cli(sys.argv)
    except ImportError:
        print("Configuration CLI not available")
        return 1


def brain_command():
    """Brain command handler."""
    try:
        from pynlm.brain.cli import brain_cli
        return brain_cli(sys.argv)
    except ImportError:
        print("Brain CLI not available")
        return 1


def agent_command():
    """Agent command handler."""
    try:
        from pynlm.agent.cli import agent_cli
        return agent_cli(sys.argv)
    except ImportError:
        print("Agent CLI not available")
        return 1


def world_command():
    """World command handler."""
    try:
        from pynlm.world.cli import world_cli
        return world_cli(sys.argv)
    except ImportError:
        print("World CLI not available")
        return 1


def explore_command():
    """Explore command handler."""
    try:
        from pynlm.explore.cli import explore_cli
        return explore_cli(sys.argv)
    except ImportError:
        print("Explore CLI not available")
        return 1


def demo_command():
    """Demo command handler."""
    try:
        from pynlm.demo.cli import demo_cli
        return demo_cli(sys.argv)
    except ImportError:
        print("Demo CLI not available")
        return 1


if __name__ == "__main__":
    sys.exit(main())
