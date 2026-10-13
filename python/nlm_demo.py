# Simple NLM Python Demo

# This demo shows basic usage of the NLM Python bindings
# It can be run as: python nlm_demo.py

import pynlm

def demo_basic_usage():
    """Demonstrate basic NLM usage."""
    print("=== NLM Basic Usage Demo ===")
    
    # Create a default configuration
    config = pynlm.Config()
    print(f"Created config: {config}")
    
    # Set some configuration values
    config["neuron_count"] = 500
    config["learning_rate"] = 0.01
    config["enable_plasticity"] = True
    config["enable_development"] = True
    
    print(f"Config has neuron_count: {'neuron_count' in config}")
    print(f"Neuron count: {config['neuron_count']}")
    print(f"Learning rate: {config.get_or('learning_rate', 0.001)}")
    
    # Create a neural brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    print("Brain initialized successfully")
    
    # Run a few simulation steps
    for i in range(10):
        brain.step(i, i * 0.001)
        
    print(f"Simulation completed. Total spikes: {brain.get_total_spike_count()}")
    print(f"Average firing rate: {brain.get_average_firing_rate():.3f}")
    print()

def demo_agent_simulation():
    """Demonstrate agent-based simulation."""
    print("=== Agent Simulation Demo ===")
    
    # Create configuration
    config = pynlm.Config()
    config["neuron_count"] = 300
    config["enable_curiosity"] = True
    
    # Create brain and world
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    world = pynlm.create_simple_world()
    world.configure(100, 100, 20, 20)
    world.setAgentStart(50, 50)
    
    # Create and initialize agent brain
    agent_brain = pynlm.create_agent_brain(brain)
    agent_brain.initialize(world)
    agent_brain.enableDevelopment(True)
    
    # Run agent simulation
    for step in range(50):
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Get action
        action = brain.produceAction()
        print(f"Step {step}: Action = {action.getName()}")
        
        # Apply action
        world.applyMotorCommand(action, step * 0.001)
    
    # Get agent state
    body = world.getAgentBody()
    print(f"Agent final position: ({body.x:.1f}, {body.y:.1f})")
    print(f"Agent final energy: {body.energy:.1f}")
    print(f"Agent final curiosity: {agent_brain.get_curiosity_level():.3f}")
    print()

def demo_configuration_management():
    """Demonstrate configuration management features."""
    print("=== Configuration Management Demo ===")
    
    # Create and populate configuration
    config = pynlm.Config()
    config["simulation"] = {
        "timestep": 0.001,
        "neuron_count": 1000,
        "connection_probability": 0.15
    }
    config["plasticity"] = {
        "stdp_enabled": True,
        "hebbian_enabled": True,
        "structural_enabled": True
    }
    
    # Use configuration operators
    print(f"Neuron count: {config['simulation']['neuron_count']}")
    print(f"STDP enabled: {config['plasticity']['stdp_enabled']}")
    
    # Iterate over configuration keys
    print("Configuration keys:")
    for key in config:
        print(f"  {key}")
    
    # Update configuration
    config.update({
        "new_parameter": "value",
        "another_parameter": 42
    })
    
    print(f"Has new_parameter: {'new_parameter' in config}")
    print()

def demo_error_handling():
    """Demonstrate error handling."""
    print("=== Error Handling Demo ===")
    
    config = pynlm.Config()
    
    # Test validation
    try:
        config.validate(["missing_key"])
        print("ERROR: Should have raised NLMError")
    except pynlm.NLMError as e:
        print(f"✓ Caught validation error: {type(e).__name__}")
    
    # Test type validation
    config["int_key"] = 42
    try:
        config.get_with_validation("int_key", "string")
        print("ERROR: Should have raised NLMError")
    except pynlm.NLMError as e:
        print(f"✓ Caught type validation error: {type(e).__name__}")
    
    # Test file operation
    try:
        config.load_from_file("nonexistent_file.json")
        print("ERROR: Should have raised NLMError")
    except pynlm.NLMError as e:
        print(f"✓ Caught file operation error: {type(e).__name__}")
    
    print()

def demo_performance_tips():
    """Demonstrate performance tips."""
    print("=== Performance Tips Demo ===")
    
    import time
    
    # Batch operations for better performance
    config = pynlm.Config()
    
    start_time = time.time()
    
    # Batch update
    config.update({
        "param_1": "value1",
        "param_2": "value2",
        "param_3": "value3",
        "param_4": "value4",
        "param_5": "value5"
    })
    
    batch_time = time.time() - start_time
    print(f"✓ Batch update: {batch_time*1000:.2f} ms")
    
    # Multiple accesses (inefficient)
    start_time = time.time()
    for i in range(100):
        _ = config.get_or("param_1", "default")
    
    single_access_time = time.time() - start_time
    print(f"✓ Single access 100 times: {single_access_time*1000:.2f} ms")
    
    # Clear configuration
    config.clear()
    print("✓ Configuration cleared")
    print()

def main():
    """Run all demos."""
    print("NLM Python Bindings - Demo Suite")
    print("=" * 50)
    
    demo_basic_usage()
    demo_agent_simulation()
    demo_configuration_management()
    demo_error_handling()
    demo_performance_tips()
    
    print("=" * 50)
    print("All demos completed successfully!")
    print("\nThe NLM Python bindings provide a comprehensive interface")
    print("for neural simulation with Pythonic APIs and extensive")
    print("documentation and examples.")

if __name__ == "__main__":
    main()
