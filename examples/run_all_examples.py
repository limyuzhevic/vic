#!/usr/bin/env python3
"""
NLM Example Scripts

Complete set of working example scripts demonstrating NLM's enhanced Python API.

These examples cover:
1. Basic brain simulation
2. Agent-world interaction
3. Memory systems (working memory, episodic memory)
4. Developmental processes
5. Prediction systems
6. Advanced agent features
7. Batch processing
8. Configuration management
9. Event-driven programming
10. Performance monitoring

Each example is designed to be educational and demonstrate specific
aspects of the NLM framework.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'pynlm_enhanced'))

from examples import (
    basic_brain_example,
    agent_world_example,
    memory_system_example,
    development_example,
    prediction_example,
    complete_agent_example,
    batch_processing_example,
    configuration_example,
    event_driven_example,
    performance_monitoring_example,
)

def print_banner():
    """Print example script banner."""
    print("=" * 70)
    print("NLM Enhanced Python API Examples")
    print("Neural Learning Machine - Experimental Brain Simulator")
    print("=" * 70)
    print()

def print_usage():
    """Print usage information."""
    print("Usage:")
    print("  python examples/run_all_examples.py                    # Run all examples")
    print("  python examples/basic_brain_example.py                # Run basic brain example")
    print("  python examples/agent_world_example.py                # Run agent-world example")
    print("  python examples/memory_system_example.py              # Run memory system example")
    print("  python examples/development_example.py                 # Run development example")
    print("  python examples/prediction_example.py                  # Run prediction example")
    print("  python examples/complete_agent_example.py              # Run complete agent example")
    print("  python examples/batch_processing_example.py            # Run batch processing example")
    print("  python examples/configuration_example.py                # Run configuration example")
    print("  python examples/event_driven_example.py                 # Run event-driven example")
    print("  python examples/performance_monitoring_example.py      # Run performance monitoring example")
    print()

def main():
    """Main function."""
    if len(sys.argv) == 1:
        # Run all examples
        print_banner()
        print("Running all NLM examples...\n")
        run_all_examples()
    
    elif len(sys.argv) == 2:
        # Run specific example
        example_name = sys.argv[1]\n        example_map = {
            'basic_brain_example': basic_brain_example,
            'agent_world_example': agent_world_example,
            'memory_system_example': memory_system_example,
            'development_example': development_example,
            'prediction_example': prediction_example,
            'complete_agent_example': complete_agent_example,
            'batch_processing_example': batch_processing_example,
            'configuration_example': configuration_example,
            'event_driven_example': event_driven_example,
            'performance_monitoring_example': performance_monitoring_example,
        }
        
        if example_name in example_map:
            print_banner()
            print(f"Running: {example_name}\n")
            example_map[example_name]()
        else:
            print(f"Unknown example: {example_name}")
            print_usage()
            sys.exit(1)
    
    else:
        print_usage()
        sys.exit(1)

if __name__ == "__main__":
    main()