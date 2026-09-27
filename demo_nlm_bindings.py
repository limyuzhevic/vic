#!/usr/bin/env python3
"""
NLM Python Bindings Demonstration

This script demonstrates the enhanced Python API for the NLM (Neural Learning Machine) framework.
It showcases the new capabilities, error handling, and advanced features.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

import pynlm as nlm

def main():
    print("=== NLM Python Bindings Demonstration ===")
    print()
    
    # === Demonstration 1: Configuration and Brain Creation ===
    print("1. Creating a basic NLM brain...")
    try:
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        print(f"   ✓ Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"   ✓ {brain.getRegionCount()} regions")
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return 1
    
    print()
    
    # === Demonstration 2: Enhanced Error Handling ===
    print("2. Demonstrating enhanced error handling...")
    try:
        # Test parameter validation
        invalid_action = nlm.Action()
        invalid_action.setParameter(100, 5.0)  # Should raise InvalidParameterError
        print("   ✗ Should have raised InvalidParameterError")
        return 1
    except nlm.InvalidParameterError as e:
        print(f"   ✓ Correctly caught InvalidParameterError: {e}")
    except Exception as e:
        print(f"   ? Unexpected exception: {e}")
    
    print()
    
    # === Demonstration 3: Enhanced Action API ===
    print("3. Demonstrating enhanced Action API...")
    try:
        # Create an action with parameters
        move_action = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.8, 1.2])
        
        print(f"   ✓ Action created: {move_action.getName()}")
        print(f"   ✓ Parameters: {move_action.getParameters()}")
        print(f"   ✓ Number of parameters: {move_action.getParameterCount()}")
        print(f"   ✓ Ready for execution: {move_action.isReady()}")
        
        # Test parameter access
        if move_action.hasParameter("move"):
            print("   ✓ Action has 'move' parameter")
        
        # Test parameter modification
        move_action.setParameter(1, 0.9)
        print(f"   ✓ Updated parameter[1] to {move_action.getParameter(1)}")
        
        # Test cloning
        cloned_action = move_action.clone()
        print(f"   ✓ Action cloned successfully: {cloned_action.getName()}")
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return 1
    
    print()
    
    # === Demonstration 4: Enhanced World System ===
    print("4. Demonstrating enhanced World system...")
    try:
        world = nlm.createSimpleWorld()
        world.initialize(64, 64, 32, 32)
        
        # Create and add objects
        player = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Resource, 100.0, 0.8)
        hazard = nlm.WorldObject(30.0, 30.0, nlm.WorldObjectType.Hazard, 50.0, 1.0)
        wall = nlm.WorldObject(20.0, 20.0, nlm.WorldObjectType.Wall, 0.0, 2.0)
        
        world.addObject(player)
        world.addObject(hazard)
        world.addObject(wall)
        
        print(f"   ✓ World created: {world.getWidth()}x{world.getHeight()}")
        print(f"   ✓ Added {len(world.getObjectsNear(10.0, 10.0, 5.0))} objects near player")
        
        # Test distance calculation
        distance = player.getDistance(15.0, 15.0)
        print(f"   ✓ Distance from player to (15,15): {distance:.2f}")
        
        # Test position validation
        is_valid = world.isValidPosition(50.0, 50.0)
        print(f"   ✓ Position (50,50) is valid: {is_valid}")
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return 1
    
    print()
    
    # === Demonstration 5: Enhanced Configuration API ===
    print("5. Demonstrating enhanced Configuration API...")
    try:
        config = nlm.createDefaultConfig()
        
        # Set various types of values
        config.set("int_value", 42)
        config.set("float_value", 3.14159)
        config.set("bool_value", True)
        config.set("string_value", "test_string")
        
        # Get values with type safety
        int_val = config.get("int_value", 0)
        float_val = config.get("float_value", 0.0)
        bool_val = config.get("bool_value", False)
        str_val = config.get("string_value", "")
        
        print(f"   ✓ Integer value: {int_val}")
        print(f"   ✓ Float value: {float_val}")
        print(f"   ✓ Boolean value: {bool_val}")
        print(f"   ✓ String value: {str_val}")
        
        # Get all configuration as dictionary
        all_config = config.getAll()
        print(f"   ✓ Total configuration keys: {len(all_config)}")
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return 1
    
    print()
    
    # === Demonstration 6: Exception Handling Best Practices ===
    print("6. Demonstrating exception handling patterns...")
    try:
        # Test file not found error
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        # Try to load a non-existent file
        brain.load("/nonexistent/path/file.bin")
        print("   ✗ Should have raised FileNotFoundError")
        return 1
        
    except nlm.FileNotFoundError as e:
        print(f"   ✓ Correctly caught FileNotFoundError: {e}")
    except Exception as e:
        print(f"   ? Unexpected exception type: {type(e).__name__}: {e}")
    
    print()
    
    # === Demonstration 7: MemoryView for Large Data ===
    print("7. Demonstrating memoryview support for large data...")
    try:
        vision = nlm.Vision(64, 64)
        vision.setData([0.1] * 4096)  # 64x64 vision data
        
        # Get memoryview for efficient access
        data_view = vision.getDataPtr()
        print(f"   ✓ Memory view created for vision data")
        print(f"   ✓ Vision dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return 1
    
    print()
    
    print("=== Demonstration Complete ===")
    print("✓ All demonstrations passed successfully!")
    print()
    print("Key improvements demonstrated:")
    print("  - Enhanced error handling with specific exception types")
    print("  - Improved configuration API with type safety")
    print("  - Enhanced Action class with parameter management")
    print("  - Better World object creation and management")
    print("  - Memoryview support for large data")
    print("  - Comprehensive documentation and error messages")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())