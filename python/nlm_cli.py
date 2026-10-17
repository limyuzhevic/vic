import os
import sys
import argparse

def main():
    """Main entry point for the nlm command line tool."""
    parser = argparse.ArgumentParser(description="NLM (Neural Learning Machine) Python Interface")
    parser.add_argument('--version', action='store_true', help='Show version information')
    parser.add_argument('--examples', action='store_true', help='Run examples')
    parser.add_argument('--help', '-h', action='store_true', help='Show help message')
    
    args = parser.parse_args()
    
    if args.version:
        print("NLM Python Interface v1.0.0")
        print("Pythonic wrapper for NLM C++ neural simulation framework")
        return
    
    if args.examples:
        print("Running NLM examples...")
        try:
            # Import and run examples
            from examples import main as run_examples
            run_examples()
        except Exception as e:
            print(f"Error running examples: {e}")
            return
        return
    
    if args.help:
        parser.print_help()
        return
    
    # Default behavior: show help
    parser.print_help()

if __name__ == '__main__':
    main()