#!/usr/bin/env python3
"""
Demonstration script showing the enhanced NLM Python API

This script demonstrates the new Pythonic interface for NLM (Neural Learning Machine)
using the enhanced bindings with better error handling, convenience functions,
and Pythonic design patterns.
"""

import pynlm_enhanced as nlm
import sys

def demo_basic_usage():
    """Demonstrate basic usage of the enhanced NLM API"""
    print("=== Basic Usage Demo ===")
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    print(f"Created config: {config}")
    
    # Create brain
    brain = nlm.createBrain(config)
    print(f"Created brain: {brain}")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized successfully")
    
    # Perform some simulation steps
    for i in range(10):
        brain.step(i)
    
    # Get statistics
    print(f"Statistics: {nlm.formatStats(brain.getBrain())}")
    print()

def demo_convenience_functions():
    """Demonstrate convenience functions"""
    print("=== Convenience Functions Demo ===")
    
    # Create random seed for reproducibility
    seed = nlm.createRandomSeed()
    print(f"Generated random seed: {seed}")
    
    # Create simple world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(5, 5)
    world.setRandomSeed(seed)
    
    print(f"Created world: {world}")
    
    # Create agent brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    agent = nlm.createAgentBrain(brain)
    
    print(f"Created agent: {agent}")
    
    # Run a simple simulation
    for step in range(5):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Simulation complete after 5 steps")
    print(f"Final position: {world.getAgentBody().x}, {world.getAgentBody().y}")
    print()

def demo_error_handling():
    """Demonstrate improved error handling"""
    print("=== Error Handling Demo ===")
    
    try:
        # Try to create config with invalid operations
        config = nlm.createDefaultConfig()
        
        # This should raise a ConfigurationError
        config.set("invalid.key", 12345)  # Valid for legacy API
        print("This should not print - error should have been raised")
        
    except Exception as e:
        print(f"Caught exception: {type(e).__name__}: {e}")
    
    try:
        # Try operations on null brain
        brain = None
        if brain is None:
            raise nlm.SimulationError("Brain cannot be null")
            
    except nlm.SimulationError as e:
        print(f"Caught simulation error: {e}")
    
    try:
        # Try to decode motor command without initialization
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        # Don't initialize brain
        action = brain.produceAction()  # This should work if brain doesn't need initialization
        print(f"Action: {action}")
        
    except Exception as e:
        print(f"Caught exception during brain operation: {type(e).__name__}: {e}")
    
    print()

def demo_context_manager_pattern():
    """Demonstrate context manager patterns"""
    print("=== Context Manager Pattern Demo ===")
    
    # While NLM doesn't have built-in context managers, we can demonstrate
    # good Python patterns for resource management
    
    class BrainSession:
        """Example context manager for brain sessions"""
        def __init__(self):
            self.brain = nlm.createBrain(nlm.createDefaultConfig())
            self.world = nlm.createSimpleWorld()
            self.agent = nlm.createAgentBrain(self.brain)
        
        def __enter__(self):
            self.brain.initialize()
            self.world.configure(10, 10, 8, 8)
            self.world.reset()
            self.world.setAgentStart(2, 2)
            return self
        
        def __exit__(self, exc_type, exc_val, exc_tb):
            # Cleanup resources
            pass
        
        def run_steps(self, steps):
            for i in range(steps):
                self.world.update(0.1)
                self.agent.processSensoryInput(self.world.getSensoryPercept())
                self.brain.step(i)
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    # Use the context manager
    with BrainSession() as session:
        session.run_steps(5)
        print(f"Context manager completed. Final position: {session.world.getAgentBody().x}, {session.world.getAgentBody().y}")
    
    print()

def demo_property_based_api():
    """Demonstrate property-based API usage"""
    print("=== Property-Based API Demo ===")
    
    config = nlm.createDefaultConfig()
    
    # Set various properties using the enhanced API
    config.set("learning_rate", 0.01)
    config.set("hidden_size", 128)
    config.set("activation", "relu")
    config.set("dropout", 0.1)
    config.set("batch_size", [32, 64, 128])
    
    # Get properties with defaults
    learning_rate = config.getOr("learning_rate", 0.001)
    hidden_size = config.getOr("hidden_size", 64)
    activation = config.getOr("activation", "sigmoid")
    dropout = config.getOr("dropout", 0.0)
    batch_size = config.getOr("batch_size", [16])
    
    print(f"Learning rate: {learning_rate}")
    print(f"Hidden size: {hidden_size}")
    print(f"Activation: {activation}")
    print(f"Dropout: {dropout}")
    print(f"Batch sizes: {batch_size}")
    
    # Check if properties exist
    print(f"Has learning_rate: {config.has('learning_rate')}")
    print(f"Has non_existent: {config.has('non_existent')}")
    
    # Get all keys
    all_keys = config.getKeys()
    print(f"All config keys: {all_keys}")
    
    print()

def demo_validation():
    """Demonstrate configuration validation"""
    print("=== Validation Demo ===")
    
    config = nlm.createDefaultConfig()
    
    # Validate config (would be called in real usage)
    is_valid = nlm.validateConfig(config.getConfig())
    print(f"Default config valid: {is_valid}")
    
    # Modify config
    config.set("model_name", "nlm_test")
    config.set("epochs", 100)
    config.set("optimizer", "adam")
    
    is_valid = nlm.validateConfig(config.getConfig())
    print(f"Modified config valid: {is_valid}")
    
    print()

def demo_advanced_features():
    """Demonstrate advanced features"""
    print("=== Advanced Features Demo ===")
    
    # Create simulation components
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    # Initialize
    brain.initialize()
    world.configure(15, 15, 10, 10)
    world.reset()
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run a more complex simulation
    print("Running advanced simulation with all features enabled...")
    for step in range(20):
        world.update(0.05)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation based on performance
        body = world.getAgentBody()
        reward = body.energy > 50.0 ? 1.0 : 0.0  # Simple reward logic
        agent.applyRewardModulation(reward, 0.5)
        
        # Update development
        agent.updateDevelopment(0.05)
        
        if step % 5 == 0:
            print(f"Step {step}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
                  f"Energy: {world.getAgentBody().energy:.1f}, "
                  f"Development Stage: {agent.getDevelopmentalStage()}")
    
    print(f"Advanced simulation complete!")
    print(f"Final statistics: {nlm.formatStats(brain.getBrain())}")
    print()

def main():
    """Run all demonstrations"""
    print("Enhanced NLM Python API Demonstration")
    print("=" * 50)
    print()
    
    demo_basic_usage()
    demo_convenience_functions()
    demo_error_handling()
    demo_context_manager_pattern()
    demo_property_based_api()
    demo_validation()
    demo_advanced_features()
    
    print("=== All demonstrations completed ===")
    print()
    print("Key improvements in this enhanced API:")
    print("1. Pythonic wrapper classes (ConfigWrapper, BrainWrapper, etc.)")
    print("2. Better error handling with specific exception types")
    print("3. Convenience functions and factory methods")
    print("4. Enhanced documentation and examples")
    print("5. Simulation orchestration helpers")
    print("6. Configuration validation utilities")
    print("7. Property-based API for easier configuration")

if __name__ == "__main__":
    main()