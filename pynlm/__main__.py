# NLM Command Line Interface
# Main entry point for the NLM Python module

from pynlm import run_demo, run_complete_simulation, get_system_info
import sys

def main():
    """Main entry point for the NLM CLI."""
    print("NLM (Neural Learning Machine) - Command Line Interface")
    print("Version 0.1.0")
    print()
    print("Available commands:")
    print("  demo          Run basic demonstration")
    print("  simulate      Run complete simulation")
    print("  info          Show system information")
    print()
    print("For more help, use: python -m pynlm --help")

if __name__ == "__main__":
    main()