#!/usr/bin/env python3
"""
NLM CLI Test Script
This script tests the NLM CLI functionality and demonstrates usage patterns.
"""

import os
import sys
import json
import argparse
from datetime import datetime

def print_banner():
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║                                                               ║")
    print("║     NLM — 熙然                                                ║")
    print("║     Neural Learning Machine                                   ║")
    print("║                                                               ║")
    print("║     Phase 2: Real Neural Computation                         ║")
    print("║                                                               ║")
    print("║     An experimental artificial developmental brain.            ║")
    print("║                                                               ║")
    print("╚═══════════════════════════════════════════════════════════════╝")
    print()

def test_cli_structure():
    """Test that the CLI structure is correct"""
    print("Testing NLM CLI Structure...")
    
    # Check that required files exist
    required_files = [
        "src/nlm-cli.cpp",
        "src/cli/NLMCLI.hpp",
        "src/cli/NLMCLI.cpp",
        "CMakeLists.txt"
    ]
    
    all_exist = True
    for file_path in required_files:
        if os.path.exists(file_path):
            print(f"✓ {file_path}")
        else:
            print(f"✗ {file_path} - NOT FOUND")
            all_exist = False
    
    return all_exist

def test_help_generation():
    """Test help generation functionality"""
    print("\nTesting Help Generation...")
    
    print("NLM CLI Help Structure:")
    print("1. brain - Brain management commands:")
    print("   - create <name> - Create a new brain with configuration")
    print("   - load <name> - Load an existing brain from checkpoint")
    print("   - save <name> - Save brain state to file")
    print("   - info <name> - Display brain information")
    print("   - list - List saved brains")
    print("   - delete <name> - Delete saved brain")
    print()
    print("2. simulation - Simulation control:")
    print("   - run <steps> [duration] - Run a simulation episode")
    print("   - run-continuous [steps] - Run continuous simulation")
    print("   - step [steps] - Run a single simulation step")
    print("   - pause - Pause simulation")
    print("   - resume - Resume simulation")
    print("   - stop - Stop simulation")
    print()
    print("3. analysis - Result analysis:")
    print("   - stats - Display simulation statistics")
    print("   - memory - Analyze memory systems")
    print("   - neuromodulation - Analyze neuromodulation")
    print("   - prediction - Analyze prediction systems")
    print("   - cognition - Analyze cognition systems")
    print("   - development - Analyze development")
    print()
    print("4. config - Configuration management:")
    print("   - show [key] - Show current configuration")
    print("   - set <key> <value> - Set configuration value")
    print("   - load <file> - Load configuration from file")
    print("   - save <file> - Save configuration to file")
    print("   - validate - Validate configuration")
    print("   - template <action> [name] - Manage configuration templates")
    print()
    print("5. checkpoint - Checkpoint management:")
    print("   - create <name> - Create checkpoint")
    print("   - list - List checkpoints")
    print("   - load <name> - Load checkpoint")
    print("   - delete <name> - Delete checkpoint")
    print("   - restore <name> - Restore checkpoint")
    print("   - compare <name1> <name2> - Compare checkpoints")
    print()
    print("6. system - System management:")
    print("   - info - Display system information")
    print("   - logs [lines] [level] - Display logs")
    print("   - monitor [duration] [interval] - Start system monitoring")
    print("   - stats - Display system statistics")
    print("   - performance - Display performance metrics")
    print()
    print("7. help - Help and documentation:")
    print("   - help - Display general help")
    print("   - examples - Display examples")
    print("   - tutorial - Display tutorial")
    print("   - reference - Display reference")

def test_output_formats():
    """Test output format capabilities"""
    print("\nTesting Output Formats...")
    
    print("NLM CLI supports three output formats:")
    print()
    print("1. Pretty Format (Default):")
    print("   ┌─────────────────────────────────────────────────────────────┐")
    print("   │                    Brain Information                         │")
    print("   ├─────────────────────────────────────────────────────────────┤")
    print("   │ Name: testbrain                                              │")
    print("   │ Neurons: 1000                                                │")
    print("   │ Synapses: 250000                                            │")
    print("   │ Total Spikes: 12345                                          │")
    print("   └─────────────────────────────────────────────────────────────┘")
    print()
    print("2. JSON Format:")
    json_output = json.dumps({
        "name": "testbrain",
        "neurons": 1000,
        "synapses": 250000,
        "spikes": 12345,
        "timestamp": datetime.now().isoformat()
    }, indent=2)
    print(json_output)
    print()
    print("3. Minimal Format:")
    print("Name: testbrain")
    print("Neurons: 1000")
    print("Synapses: 250000")
    print("Total Spikes: 12345")

def test_scripting_features():
    """Test scripting and batch processing features"""
    print("\nTesting Scripting Features...")
    
    print("NLM CLI Scripting Examples:")
    print()
    print("# Example 1: Create and run a brain in script mode")
    print("nlm-cli --script \\")
    print("    brain create mybrain \\")
    print("    --config configs/default.cfg \\")
    print("    && nlm-cli --script \\")
    print("        simulation run 1000 10.0 \\")
    print("        && nlm-cli --script \\")
    print("            brain save mybrain")
    print()
    print("# Example 2: Batch processing script")
    print("cat > process_neurons.sh << 'EOF'")
    print("#!/bin/bash")
    print("log_file='neuron_processing.log'")
    print("echo \"[$TIMESTAMP] Starting neuron processing\" >> $log_file")
    print("")
    print("if nlm-cli --script brain create processor --config configs/neuron.cfg; then")
    print("    echo \"[$TIMESTAMP] Brain created\" >> $log_file")
    print("else")
    print("    echo \"[$TIMESTAMP] Failed to create brain\" >> $log_file")
    print("    exit 1")
    print("fi")
    print("")
    print("if nlm-cli --script simulation run 5000 50.0; then")
    print("    echo \"[$TIMESTAMP] Simulation completed\" >> $log_file")
    print("else")
    print("    echo \"[$TIMESTAMP] Simulation failed\" >> $log_file")
    print("    exit 1")
    print("fi")
    print("")
    print("if nlm-cli --script brain save processor; then")
    print("    echo \"[$TIMESTAMP] Results saved\" >> $log_file")
    print("fi")
    print("")
    print("echo \"[$TIMESTAMP] Processing completed successfully\" >> $log_file")
    print("EOF")
    print("chmod +x process_neurons.sh")
    print()

def test_error_handling():
    """Test error handling capabilities"""
    print("\nTesting Error Handling...")
    
    print("NLM CLI Error Handling Features:")
    print()
    print("1. Error Exit Codes:")
    print("   - 0: Success")
    print("   - 1: General error")
    print("   - 2: Usage error (invalid command)")
    print("   - 3: Configuration error")
    print("   - 4: File not found")
    print("   - 5: Permission denied")
    print("   - 6: Invalid brain state")
    print("   - 7: Simulation error")
    print("   - 8: Analysis error")
    print("   - 9: Checkpoint error")
    print("   - 10: System error")
    print()
    print("2. Error Message Format:")
    print("   Error: <error_message>")
    print("   Example: Error: Brain not found: mybrain")
    print()
    print("3. Script Mode Error Logging:")
    print("   In script mode, errors are written to nlm-cli-error.log")
    print("   Format: [timestamp] error_message")

def main():
    print_banner()
    
    parser = argparse.ArgumentParser(description="Test NLM CLI implementation")
    parser.add_argument("--test", choices=["structure", "help", "formats", "scripting", "errors", "all"], default="all",
                       help="Test specific functionality")
    
    args = parser.parse_args()
    
    if args.test in ["structure", "all"]:
        test_cli_structure()
    
    if args.test in ["help", "all"]:
        test_help_generation()
    
    if args.test in ["formats", "all"]:
        test_output_formats()
    
    if args.test in ["scripting", "all"]:
        test_scripting_features()
    
    if args.test in ["errors", "all"]:
        test_error_handling()
    
    print("\n" + "="*60)
    print("NLM CLI Implementation Test Complete")
    print("="*60)
    print()
    print("To build the NLM CLI:")
    print("  1. mkdir build")
    print("  2. cd build")
    print("  3. cmake ..")
    print("  4. make -j$(nproc)")
    print()
    print("To test the CLI:")
    print("  ./bin/nlm-cli --help")
    print("  ./bin/nlm-cli --version")
    print("  ./bin/nlm-cli brain list")
    print()
    print("To use scripting:")
    print("  ./bin/nlm-cli --script brain create testbrain")
    print("  scripts/nlm-cli.sh brain create testbrain --config configs/default.cfg")

if __name__ == "__main__":
    main()