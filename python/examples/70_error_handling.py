"""NLM Error Handling - Robust error handling examples"""
import pynlm

def demo_error_handling():
    """Demonstrate improved error handling."""
    print("=== Error Handling Demo ===\n")
    
    # 1. Configuration error handling
    print("1. Configuration validation...")
    try:
        config = pynlm.createDefaultConfig()
        # Set invalid parameters
        config.set("neuron_count", -100)  # Invalid negative count
        config.set("region_count", "invalid")  # Invalid type
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        brain.step(0)
        
        print("   Note: Some invalid parameters may be silently accepted")
        print("   In production code, additional validation is recommended")
        
    except Exception as e:
        print(f"   Configuration error caught: {e}")
    
    print()
    
    # 2. Brain operation error handling
    print("2. Brain operation error handling...")
    try:
        # Create brain without initialization
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # This should work (step without initialization is allowed)
        brain.step(0)
        print("   ✓ Step without initialization successful")
        
        # Now initialize and try again
        brain.initialize()
        brain.step(1)
        print("   ✓ Step after initialization successful")
        
    except Exception as e:
        print(f"   Brain operation error: {e}")
    
    print()
    
    # 3. Agent setup error handling
    print("3. Agent setup error handling...")
    try:
        brain = pynlm.createQuickBrain()
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("   ✓ Agent setup and initialization successful")
        
        # Try to process without world reset
        agent.processSensoryInput(world.getSensoryPercept())
        print("   ✓ Sensory processing successful")
        
    except Exception as e:
        print(f"   Agent setup error: {e}")
    
    print()
    
    # 4. Exception types
    print("4. Exception type handling...")
    exception_types = [
        ("RuntimeError", pynlm.RuntimeError),
    ]
    
    for name, exc_type in exception_types:
        print(f"   Available exception type: {name}")
    
    print()
    
    # 5. Best practices
    print("5. Error handling best practices...")
    print("   ✓ Always check configuration validity")
    print("   ✓ Validate brain state before operations")
    print("   ✓ Handle agent setup errors")
    print("   ✓ Use try-except blocks for critical operations")
    print("   ✓ Provide meaningful error messages")
    
    print("\n=== Error handling complete ===")

demo_error_handling()
