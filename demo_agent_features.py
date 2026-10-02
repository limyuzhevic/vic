#!/usr/bin/env python3
"""
Example script demonstrating the enhanced AgentBrain features.

This script shows how to use the new agent commands and capabilities
in the NLM (Neural Learning Machine) system.
"""

import subprocess
import sys
import os
import time

def run_command(cmd):
    """Run a command and return output."""
    print(f"\n> {cmd}")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if result.stdout:
        print(result.stdout)
    if result.stderr:
        print(f"Error: {result.stderr}")
    return result.returncode == 0

def main():
    print("=== NLM Agent Brain Advanced Features Demo ===\n")
    
    # Check if the NLM executable exists
    if not os.path.exists("./nlm"):
        print("Error: NLM executable not found. Please build the project first.")
        return 1
    
    print("1. Running basic help to show available options:\n")
    run_command("./nlm --help")
    
    print("\n2. Running with agent mode and help:\n")
    run_command("./nlm mode=agent help")
    
    print("\n3. Running with agent mode and agent help:\n")
    run_command("./nlm mode=agent help agent")
    
    print("\n4. Running with agent mode and status:\n")
    run_command("./nlm mode=agent status")
    
    print("\n5. Testing agent command processing:\n")
    # Create a temporary test configuration
    with open("/tmp/agent_test.cfg", "w") as f:
        f.write("""[agent_brain]
agent_learning_enabled = true
agent_learning_rate = 0.1
agent_exploration_rate = 0.15
agent_performance_monitoring = true
agent_curiosity_threshold = 0.3
agent_adaptation_speed = 0.02
""")
    
    print("   Created test configuration")
    print("   (Run ./nlm mode=agent --config=/tmp/agent_test.cfg for full test)")
    
    print("\n=== Demo Complete ===\n")
    print("The enhanced AgentBrain provides:")
    print("  • Advanced agent control (start, stop, pause, resume)")
    print("  • Learning configuration (enable/disable, rate adjustment)")
    print("  • Behavior modification (exploration/exploitation)")
    print("  • State management (checkpoint, restore, save/load)")
    print("  • Performance monitoring and optimization")
    print("  • Comprehensive command line interface")
    print("  • Full backward compatibility")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())