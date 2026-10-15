# Create a simple command line interface for NLM Python bindings
import argparse
import sys
from pathlib import Path

# Add the python module to path
sys.path.insert(0, str(Path(__file__).parent / ".." / "python"))

import pynlm
def main():
    """Main CLI entry point for NLM Python bindings."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Python Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-demo                    # Run Phase 6 integration demo
  nlm-test --steps 100        # Test learning and plasticity
  nlm-test --neurons 50       # Test with smaller brain
  pynlm-demo                  # Alternative command name
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run Phase 6 integration demo")
    demo_parser.add_argument("--steps", type=int, default=2000, help="Number of simulation steps")
    demo_parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    
    # Test command
    test_parser = subparsers.add_parser("test", help="Test learning and plasticity")
    test_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    test_parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    
    # Show command
    subparsers.add_parser("show", help="Show help information")
    
    args = parser.parse_args()
    
    if args.command == "demo" or args.command is None:
        print("=== NLM Phase 6 Integration Demo ===")
        print(f"Running with {args.neurons} neurons for {args.steps} steps...")
        
        results = pynlm.run_phase6_demo(max_steps=args.steps)
        if results:
            print("\n=== Results ===")
            print(f"Total Reward: {results['total_reward']}")
            print(f"Average Firing Rate: {results['avg_firing_rate']}")
            print(f"Memory Episodes Stored: {results['memory_episodes_stored']}")
            print(f"Dopamine Level: {results['dopamine_level']}")
            
            if results['total_reward'] > 0:
                print("\n✓ SUCCESS: Integration successful!")
            else:
                print("\n✗ WARNING: Low reward - check integration")
        else:
            print("✗ FAILED: Demo could not run")
            sys.exit(1)
    
    elif args.command == "test":
        print("=== NLM Learning and Plasticity Test ===")
        print(f"Testing with {args.neurons} neurons for {args.steps} steps...")
        
        brain = pynlm.create_brain(neuron_count=args.neurons, timestep=0.001)
        if brain:
            results = pynlm.test_learning(brain, steps=args.steps)
            if results:
                print("\n=== Test Results ===")
                print(f"Total Spikes: {results['total_spikes']}")
                print(f"Total Neurons: {results['total_neurons']}")
                print(f"Total Synapses: {results['total_synapses']}")
                print(f"Active Neurons: {results['active_neurons']}")
                print(f"Firing Neurons: {results['firing_neurons']}")
                print(f"Average Firing Rate: {results['avg_firing_rate']}")
                print(f"E/I Ratio: {results['excitation_inhibition_ratio']}")
                
                if results['total_spikes'] > 0:
                    print("\n✓ SUCCESS: Network activity detected!")
                else:
                    print("\n! WARNING: No spikes detected - check configuration")
            else:
                print("✗ FAILED: Test could not run")
                sys.exit(1)
        else:
            print("✗ FAILED: Could not create brain")
            sys.exit(1)
    
    elif args.command == "show":
        parser.print_help()
if __name__ == "__main__":
    main()