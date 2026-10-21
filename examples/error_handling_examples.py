"""
Error Handling Examples

This file demonstrates comprehensive error handling in NLM Python bindings
version 0.1.0, including custom exception types and best practices.
"""

import pynlm

def example_1_custom_exceptions():
    """Example 1: Using custom exception types."""
    print("Example 1: Custom Exception Types")
    print("-" * 35)
    
    print("Available custom exceptions:")
    print("• pynlm.NLMError - General NLM errors")
    print("• pynlm.InvalidArgumentError - Invalid arguments")
    print("• pynlm.ConfigurationError - Configuration issues")
    print("• pynlm.SimulationError - Simulation problems")
    print()
    
    # Demonstrate exception handling
    try:
        # This should raise a ConfigurationError
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 5)  # Too small
        pynlm.validate_config(config)
        print("✗ Unexpected: Validation passed")
        
    except pynlm.ConfigurationError as e:
        print(f"✓ Caught ConfigurationError: {str(e)[:60]}...")
    
    try:
        # This should raise an InvalidArgumentError
        pynlm.create_brain_with_preset("unknown_preset", 100)
        print("✗ Unexpected: Invalid preset accepted")
        
    except pynlm.InvalidArgumentError as e:
        print(f"✓ Caught InvalidArgumentError: {e}")
    
    try:
        # This should work fine
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print("✓ Successful brain creation")
        
    except pynlm.NLMError as e:
        print(f"✓ Caught NLMError: {e}")
    
    print()

def example_2_validation_workflow():
    """Example 2: Configuration validation workflow."""
    print("Example 2: Configuration Validation Workflow")
    print("-" * 45)
    
    def safe_config_creation():
        """Safely create and validate a configuration."""
        try:
            # Create base configuration
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 200)
            config.set("learning.rate", 0.01)
            config.set("simulation_timestep", 0.001)
            
            # Validate configuration
            if not pynlm.validate_config(config):
                return None, "Configuration validation failed"
            
            # Save configuration
            pynlm.save_config_to_file(config, "safe_config.json")
            
            return config, "Configuration created and saved successfully"
            
        except pynlm.ConfigurationError as e:
            return None, f"Configuration error: {e}"
        except Exception as e:
            return None, f"Unexpected error: {e}"
    
    # Test the safe configuration creation
    config, message = safe_config_creation()
    
    if config:
        print(f"✓ {message}")
        
        # Use the configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
    else:
        print(f"✗ {message}")
    
    # Clean up
    import os
    if os.path.exists("safe_config.json"):
        os.unlink("safe_config.json")
        print("✓ Cleaned up temporary file")
    
    print()

def example_3_simulation_error_handling():
    """Example 3: Simulation error handling."""
    print("Example 3: Simulation Error Handling")
    print("-" * 35)
    
    def safe_simulation_run(brain, steps):
        """Safely run a brain simulation with error handling."""
        try:
            if steps < 0:
                raise pynlm.InvalidArgumentError("Number of steps cannot be negative")
            
            if brain.getTotalNeuronCount() < 10:
                raise pynlm.SimulationError("Brain has too few neurons for meaningful simulation")
            
            # Run simulation
            for step in range(steps):
                brain.step(step)
                
                if step % 10 == 0:
                    print(f"  Progress: Step {step}, Spikes: {brain.getTotalSpikeCount()}")
            
            return True, f"Simulation completed successfully with {steps} steps"
            
        except pynlm.SimulationError as e:
            return False, f"Simulation error: {e}"
        except Exception as e:
            return False, f"Unexpected error: {e}"
    
    # Test with valid parameters
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain1.initialize()
    
    success, message = safe_simulation_run(brain1, 20)
    print(f"1. Valid simulation: {message}")
    
    # Test with negative steps
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2.initialize()
    
    success, message = safe_simulation_run(brain2, -5)
    print(f"2. Invalid steps: {message}")
    
    # Test with too few neurons
    brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain3.getConfig().set("neuron_count", 5)
    
    success, message = safe_simulation_run(brain3, 10)
    print(f"3. Too few neurons: {message}")
    
    print()

def example_4_agent_error_handling():
    """Example 4: Agent error handling."""
    print("Example 4: Agent Error Handling")
    print("-" * 30)
    
    def safe_agent_simulation(agent, world, steps):
        """Safely run an agent simulation with comprehensive error handling."""
        try:
            # Validate agent
            if not hasattr(agent, 'processSensoryInput'):
                raise pynlm.InvalidArgumentError("Invalid agent object")
            
            # Validate world
            if not hasattr(world, 'update') or not hasattr(world, 'getSensoryPercept'):
                raise pynlm.InvalidArgumentError("Invalid world object")
            
            if steps <= 0:
                raise pynlm.InvalidArgumentError("Number of steps must be positive")
            
            print(f"Starting agent simulation with {steps} steps...")
            
            for step in range(steps):
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                if not percept:
                    print(f"Warning: Empty percept at step {step}")
                    continue
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Get brain's step number
                brain = agent.getBrain()
                brain.step(step)
                
                # Decode motor command
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply learning if enabled
                if agent.isRewardModulationEnabled():
                    agent.applyRewardModulation(1.0, 0.5)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                if step % 10 == 0:
                    print(f"  Step {step}: Action={action.getType()}, "
                          f"Curiosity={agent.getCuriosityLevel():.2f}")
            
            return True, f"Agent simulation completed successfully"
            
        except pynlm.InvalidArgumentError as e:
            return False, f"Invalid argument: {e}"
        except pynlm.NLMError as e:
            return False, f"NLM error: {e}"
        except Exception as e:
            return False, f"Unexpected error: {e}"
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Run simulation with error handling
    success, message = safe_agent_simulation(agent, world, 25)
    print(f"Agent simulation: {message}")
    
    print()

def example_5_exception_chaining():
    """Example 5: Exception chaining and nested errors."""
    print("Example 5: Exception Chaining")
    print("-" * 25)
    
    def demonstrate_exception_chaining():
        """Demonstrate how exceptions can chain in complex operations."""
        
        try:
            # Step 1: Try to create a configuration
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 100)
            
            # Step 2: Validate configuration
            if not pynlm.validate_config(config):
                raise pynlm.ConfigurationError("Configuration validation failed")
            
            # Step 3: Save configuration
            pynlm.save_config_to_file(config, "example_config.json")
            
            # Step 4: Try to create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Step 5: Try to create agent
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 5, 5)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Step 6: Run simulation
            for step in range(5):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            print("✓ Complete workflow succeeded")
            
        except pynlm.ConfigurationError as e:
            print(f"✗ Configuration error: {e}")
        except pynlm.SimulationError as e:
            print(f"✗ Simulation error: {e}")
        except pynlm.InvalidArgumentError as e:
            print(f"✗ Invalid argument: {e}")
        except pynlm.NLMError as e:
            print(f"✗ NLM error: {e}")
        except Exception as e:
            print(f"✗ Unexpected error: {e}")
        finally:
            # Clean up
            import os
            if os.path.exists("example_config.json"):
                os.unlink("example_config.json")
                print("✓ Cleaned up temporary file")
    
    demonstrate_exception_chaining()
    print()

def example_6_error_recovery():
    """Example 6: Error recovery strategies."""
    print("Example 6: Error Recovery Strategies")
    print("-" * 35)
    
    def robust_config_loader(filepath):
        """Robust configuration loader with recovery options."""
        import json
        
        try:
            # Try to load using NLM's built-in loader
            config = pynlm.load_config_from_file(filepath)
            print("✓ Loaded using NLM's configuration loader")
            return config
            
        except pynlm.ConfigurationError:
            print("⚠ NLM loader failed, trying alternative method...")
            
            try:
                # Fallback: manual JSON loading
                with open(filepath, 'r') as f:
                    data = json.load(f)
                
                # Create NLM config and set values
                config = pynlm.createDefaultConfig()
                for key, value in data.items():
                    # Try to determine type and set accordingly
                    if isinstance(value, bool):
                        config.set(key, value)
                    elif isinstance(value, int):
                        config.set(key, value)
                    elif isinstance(value, float):
                        config.set(key, value)
                    elif isinstance(value, str):
                        config.set(key, value)
                    else:
                        print(f"  ⚠ Skipping unknown type for key '{key}'")
                
                print("✓ Fallback loading successful")
                return config
                
            except Exception as e:
                print(f"✗ Both loading methods failed: {e}")
                return None
    
    # Test error recovery
    print("Testing configuration loader with recovery...")
    
    # Create a valid config file
    with open("test_config.json", "w") as f:
        f.write('{"neuron_count": 200, "learning.rate": 0.01}')
    
    # Test with valid file
    config1 = robust_config_loader("test_config.json")
    if config1:
        print(f"✓ Successfully loaded: {config1.getOr('neuron_count', 'unknown')}")
    
    # Test with invalid file
    with open("invalid_config.json", "w") as f:
        f.write("invalid json content")
    
    config2 = robust_config_loader("invalid_config.json")
    if config2 is None:
        print("✓ Correctly handled invalid file")
    
    # Clean up
    import os
    for filename in ["test_config.json", "invalid_config.json"]:
        if os.path.exists(filename):
            os.unlink(filename)
    
    print()

if __name__ == "__main__":
    print("Error Handling Examples")
    print("=" * 50)
    print("Demonstrating comprehensive error handling in NLM 0.1.0")
    print()
    
    example_1_custom_exceptions()
    example_2_validation_workflow()
    example_3_simulation_error_handling()
    example_4_agent_error_handling()
    example_5_exception_chaining()
    example_6_error_recovery()
    
    print("All error handling examples completed successfully!")
    print()
    print("Key concepts covered:")
    print("• Custom exception types for different error categories")
    print("• Configuration validation and safe file I/O")
    print("• Simulation error handling with graceful degradation")
    print("• Agent error handling with comprehensive validation")
    print("• Exception chaining for complex workflows")
    print("• Error recovery and fallback strategies")
    print("• Best practices for robust Python code")
