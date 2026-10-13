"""Main entry point for NLM CLI."""

import sys
import pynlm

def print_version():
    """Print version information."""
    info = pynlm.getVersionInfo()
    print(f"NLM {info['version']}")
    print(f"Author: {info['author']}")
    print(f"Description: {info['description']}")
    print()

def print_help():
    """Print help information."""
    print("NLM - Neural Learning Machine")
    print("=" * 50)
    print()
    print("Usage: nlm <command> [options]")
    print()
    print("Commands:")
    print("  version              Show version information")
    print("  create              Create and initialize a brain")
    print("  demo                Run integration demo")
    print("  test                Run tests")
    print("  help                Show this help")
    print()

def main():
    """Main CLI entry point."""
    if len(sys.argv) < 2:
        print_help()
        return 1
    
    command = sys.argv[1]
    
    if command == "version":
        print_version()
        return 0
    
    elif command == "help":
        print_help()
        return 0
    
    elif command == "demo":
        from .demo import main as demo_main
        return demo_main()
    
    elif command == "test":
        from .test import main as test_main
        return test_main()
    
    elif command == "create":
        print("NLM Create Command")
        print("=" * 30)
        print("Creates a new brain with default configuration.")
        print()
        
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            print("✓ Brain created successfully!")
            print(f"  Total neurons: {brain.getTotalNeuronCount()}")
            print(f"  Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  Region count: {brain.getRegionCount()}")
            
            return 0
        except Exception as e:
            print(f"✗ Error creating brain: {e}")
            return 1
    
    else:
        print(f"Unknown command: {command}")
        print("Use 'nlm help' for available commands")
        return 1

if __name__ == "__main__":
    sys.exit(main())
