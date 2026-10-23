"""
NLM Examples - Collection of usage examples for the NLM Python bindings

This file provides an overview of all available examples and instructions for running them.
"""

EXAMPLES = {
    "basic_simulation.py": "Basic brain simulation without environment interaction",
    "agent_world_simulation.py": "Complete agent-environment interaction example",
    "plasticity_learning.py": "Learning and plasticity experiments",
    "advanced_configuration.py": "Advanced configuration and batch processing",
}

def print_examples():
    """Print all available examples"""
    print("=" * 60)
    print("NLM Python Examples - Available Examples")
    print("=" * 60)
    print()
    
    for example_file, description in EXAMPLES.items():
        print(f"• {example_file}")
        print(f"    {description}")
        print()
    
    print("=" * 60)
    print("To run an example, execute:")
    print("  python examples/example_name.py")
    print()
    print("Example usage:")
    print("  python examples/agent_world_simulation.py")
    print()
    print("All examples demonstrate different aspects of the NLM system:")
    print("  1. Basic neural computation")
    print("  2. Embodied intelligence with world interaction")
    print("  3. Learning through synaptic plasticity")
    print("  4. Advanced configuration and batch processing")
    print("=" * 60)

if __name__ == "__main__":
    print_examples()
