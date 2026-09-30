#!/usr/bin/env python3
"""
Example: Demonstrating Improved NLM Python Bindings

This script showcases the enhanced features of the NLM Python bindings,
including better error handling, validation, and Pythonic interfaces.
"""

import pynlm as nlm
import time

def example_basic_usage():
    """Demonstrate basic usage with enhanced error handling."""
    print("=== Basic Usage Example ===")
    
    # Create components with validation
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=80, height=60, vision_width=16, vision_height=16)
    world.setAgentStart(40.0, 30.0)
    
    # Initialize with error checking
    try:
        brain.initialize()
        agent.initialize(world)
        print("✓ Environment initialized successfully")
    except Exception as e:
        print(f"✗ Initialization failed: {e}")
        return
    
    # Check agent status
    status = agent.getProcessingStatus()
    print(f"✓ Agent features enabled: {agent.getFeatureCount()}")
    print(f"  - Neuromodulation: {status['neuromodulation_level']:.3f}")
    print(f"  - Curiosity: {status['curiosity_level']:.3f}")
    print(f"  - Prediction error: {status['prediction_error']:.3f}")

def example_sensor_processing():
    """Demonstrate enhanced sensor processing with validation."""
    print("\n=== Sensor Processing Example ===")
    
    # Create test environment
    config, world, brain, agent = nlm.createTestEnvironment()
    
    # Create and configure sensory percept
    sensor_data = nlm.SensoryPercept()
    
    # Create vision data (gradient pattern)
    vision_width, vision_height = sensor_data.getVisionWidth(), sensor_data.getVisionHeight()
    vision_data = []
    for y in range(vision_height):
        for x in range(vision_width):
            # Create a simple gradient (0 to 1)
            value = (x + y) / (vision_width + vision_height)
            vision_data.append(value)
    
    sensor_data.setVision(vision_data)
    sensor_data.setTimestamp(time.time())
    
    print(f"✓ Vision data created: {len(vision_data)} pixels ({vision_width}x{vision_height})")
    
    # Access data with bounds checking
    try:
        center_x, center_y = vision_width // 2, vision_height // 2
        center_value = sensor_data.getVisionPixel(center_x, center_y)
        print(f"✓ Center pixel value: {center_value:.3f}")
    except Exception as e:
        print(f"✗ Error accessing pixel: {e}")
    
    # Process sensor data
    agent.processSensoryInput(sensor_data)
    print(f"✓ Sensor data processed")
    
    # Get processed status
    status = agent.getProcessingStatus()
    print(f"  - Novelty level: {status['novelty_level']:.3f}")
    print(f"  - Curiosity level: {status['curiosity_level']:.3f}")

def example_error_handling():
    """Demonstrate comprehensive error handling."""
    print("\n=== Error Handling Example ===")
    
    config = nlm.createDefaultConfig()
    
    # Example 1: Configuration key not found
    print("1. Testing configuration key validation:")
    try:
        value = config.get('nonexistent_key')
        print(f"   ✗ Unexpected success: {value}")
    except nlm.KeyError as e:
        print(f"   ✓ Caught KeyError: {e}")
    except Exception as e:
        print(f"   ✓ Caught exception: {type(e).__name__}: {e}")
    
    # Example 2: Invalid data types
    print("\n2. Testing type validation:")
    try:
        config.set('test_value', 'string')
        int_value = config.get_int('test_value')  # Should fail
        print(f"   ✗ Unexpected success: {int_value}")
    except Exception as e:
        print(f"   ✓ Caught type error: {type(e).__name__}")
    
    # Example 3: Invalid sensor access
    print("\n3. Testing bounds checking:")
    sensor_data = nlm.SensoryPercept()
    try:
        pixel = sensor_data.getVisionPixel(100, 100)  # Out of bounds
        print(f"   ✗ Unexpected success: {pixel}")
    except nlm.IndexError as e:
        print(f"   ✓ Caught IndexError: {e}")
    except Exception as e:
        print(f"   ✓ Caught exception: {type(e).__name__}")

def example_brain_analysis():
    """Demonstrate enhanced brain analysis capabilities."""
    print("\n=== Brain Analysis Example ===")
    
    # Create test environment
    config, world, brain, agent = nlm.createTestEnvironment()
    
    # Get comprehensive brain statistics
    print("Brain Statistics:")
    stats = brain.getActivityStats()
    for key, value in stats.items():
        print(f"  - {key}: {value}")
    
    # Get region information
    region_ids = brain.getRegionIds()
    print(f"\n✓ Brain regions: {len(region_ids)} regions")
    
    # Test feature processing
    print("\nAgent Features:")
    agent.enableAllFeatures()
    
    # Create some action and observe effects
    action = nlm.Action(nlm.ActionType.MoveForward)
    action.addParameter(1.0)
    action.addParameter(2.0)
    
    print(f"✓ Created action: {action.getName()}")
    print(f"  - Type: {'Movement' if action.isMovement() else 'Other'}")
    print(f"  - Has parameters: {action.hasParameters()}")
    print(f"  - Parameter count: {action.getParameterCount()}")

def example_processing_modes():
    """Demonstrate different processing modes."""
    print("\n=== Processing Modes Example ===")
    
    config, world, brain, agent = nlm.createTestEnvironment()
    
    # Test different processing modes
    modes = ["strict", "fast", "curious"]
    
    for mode in modes:
        print(f"\nTesting mode: '{mode}'")
        agent.setProcessingMode(mode)
        
        status = agent.getProcessingStatus()
        print(f"  - Features enabled: {agent.getFeatureCount()}")
        print(f"  - Neuromodulation: {status['neuromodulation_level']:.3f}")
        print(f"  - Curiosity: {status['curiosity_level']:.3f}")
    
    # Test feature management
    print("\nFeature Management:")
    print(f"  - Enabled features: {agent.getFeatureCount()}")
    
    agent.disableAllFeatures()
    print(f"  - After disable: {agent.getFeatureCount()}")
    
    agent.enableAllFeatures()
    print(f"  - After enable: {agent.getFeatureCount()}")

def example_world_interaction():
    """Demonstrate world interaction with enhanced validation."""
    print("\n=== World Interaction Example ===")
    
    world = nlm.createSimpleWorld()
    world.configure(width=80, height=60, vision_width=16, vision_height=16)
    
    # Add objects to world
    print("Adding objects to world:")
    world.addResource(10.0, 20.0, 100.0)
    world.addHazard(70.0, 50.0, 50.0)
    world.addWall(40.0, 0.0, 5.0)
    
    print("✓ Objects added successfully")
    
    # Test position validation
    print("\nTesting position validation:")
    test_positions = [(25.0, 25.0), (90.0, 30.0), (-10.0, 20.0)]
    
    for x, y in test_positions:
        if world.isValidPosition(x, y):
            distance = world.distanceTo(x, y)
            angle = world.angleTo(x, y)
            print(f"  ✓ ({x}, {y}) valid - distance: {distance:.1f}, angle: {angle:.2f}")
        else:
            print(f"  ✗ ({x}, {y}) invalid")

def main():
    """Run all examples."""
    print("NLM Python Bindings - Enhanced Features Demonstration")
    print("=" * 60)
    
    try:
        example_basic_usage()
        example_sensor_processing()
        example_error_handling()
        example_brain_analysis()
        example_processing_modes()
        example_world_interaction()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("\nKey Improvements Demonstrated:")
        print("• Enhanced error handling and validation")
        print("• Pythonic property access and type safety")
        print("• Comprehensive status monitoring")
        print("• Advanced sensor processing utilities")
        print("• Flexible processing modes")
        print("• Robust bounds checking")
        
    except Exception as e:
        print(f"\n✗ Example failed with error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
