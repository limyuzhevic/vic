#!/usr/bin/env python3
"""
Demonstration of Enhanced Python Bindings for NLM

This script demonstrates the new Pythonic patterns and features
introduced in the enhanced pynlm bindings.
"""

import pynlm
import sys

def demonstrate_action_improvements():
    """Demonstrate Action class improvements with factory methods and builder pattern."""
    print("=== Action Class Improvements ===")
    
    # Pythonic factory methods
    action1 = pynlm.Action.move_forward()
    print(f"Move forward action: {action1}")
    
    action2 = pynlm.Action.turn_left()
    print(f"Turn left action: {action2}")
    
    # Builder pattern with fluent interface
    action3 = pynlm.Action.turn_right().with_parameters([0.5, 0.3, 0.1])
    print(f"Turn right with parameters: type={action3.get_type()}, params={action3.get_parameters()}")
    
    # Static factory methods
    action4 = pynlm.Action.custom([1.0, 2.0, 3.0])
    print(f"Custom action: {action4}")
    
    # Alternative constructors
    action5 = pynlm.Action(pynlm.ActionType.Look)
    print(f"Look action: {action5}")
    
    print()

def demonstrate_vision_improvements():
    """Demonstrate Vision class improvements with factory methods."""
    print("=== Vision Class Improvements ===")
    
    # Pythonic factory methods
    vision1 = pynlm.Vision.from_image(640, 480)
    print(f"Vision 640x480 RGB: {vision1.get_width()}x{vision1.get_height()}x{vision1.get_channels()}")
    
    vision2 = pynlm.Vision.from_image(320, 240, 1)  # Grayscale
    print(f"Vision 320x240 grayscale: {vision2.is_grayscale()}")
    
    vision3 = pynlm.Vision.rgb(800, 600)  # RGB factory
    print(f"Vision RGB: {vision3.is_rgb()}")
    
    vision4 = pynlm.Vision.rgba(1024, 768)  # RGBA factory
    print(f"Vision RGBA: {vision4.is_rgba()}")
    
    # Enhanced property access
    if vision1.is_rgb():
        print("Vision is RGB - good for color images")
    
    print()

def demonstrate_audio_improvements():
    """Demonstrate Audio class improvements."""
    print("=== Audio Class Improvements ===")
    
    # Pythonic factory methods
    audio1 = pynlm.Audio.from_samples(44100, 44100)  # 1 second at 44.1kHz
    print(f"Audio: {audio1.get_sample_rate()} Hz, {audio1.get_duration():.2f} seconds")
    
    # Alternative constructors
    audio2 = pynlm.Audio.create_default_audio(2.5, 48000.0)
    print(f"Audio 2.5 seconds at 48kHz: {audio2.get_num_samples()} samples")
    
    print()

def demonstrate_enhanced_factory_methods():
    """Demonstrate enhanced Pythonic factory methods."""
    print("=== Enhanced Factory Methods ===")
    
    # Create brain from configuration
    config = pynlm.createDefaultConfig()
    print(f"Created default config: {config}")
    
    # Create brain from file (if file exists)
    try:
        brain = pynlm.createBrainFromFile("test_config.json")
        print(f"Created brain from file: {brain}")
    except Exception as e:
        print(f"File creation (expected to fail if file doesn't exist): {e}")
    
    # Create brain from arguments
    brain = pynlm.createBrainFromArgs(2, ["prog", "--neurons=100"])
    print(f"Created brain from arguments: {brain}")
    
    # Create agent brain from config
    agent_brain = pynlm.createAgentBrainFromConfig(config)
    print(f"Created agent brain from config: {agent_brain}")
    
    # Create simple world with size
    world = pynlm.createSimpleWorldWithSize(20, 20, 8, 8)
    print(f"Created world 20x20 with vision 8x8: {world.getWidth()}x{world.getHeight()}")
    
    print()

def demonstrate_error_handling():
    """Demonstrate enhanced error handling and validation."""
    print("=== Error Handling and Validation ===")
    
    try:
        # Test validation in Vision
        data = [1.0, 2.0, 3.0]  # Too small for 640x480 RGB (921600 elements)
        vision = pynlm.Vision.from_data(data, 640, 480, 3)
        vision.setData(data)  # This should throw an error
    except Exception as e:
        print(f"Vision validation error (expected): {type(e).__name__}: {e}")
    
    try:
        # Test validation in Audio
        audio_data = [1.0, 2.0]  # Too small for 44100 samples
        audio = pynlm.Audio.from_samples_with_data(44100, 44100, audio_data)
        audio.setData(audio_data)  # This should throw an error
    except Exception as e:
        print(f"Audio validation error (expected): {type(e).__name__}: {e}")
    
    print()

def demonstrate_context_managers():
    """Demonstrate context managers for resource cleanup."""
    print("=== Context Managers ===")
    
    # Brain context manager
    try:
        with pynlm.Brain(pynlm.createDefaultConfig()) as brain:
            brain.initialize()
            brain.step(0)
            print(f"Brain context initialized and stepped successfully")
            print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"Brain context error: {e}")
    
    # World context manager
    try:
        with pynlm.SimpleWorld() as world:
            world.configure(10, 10, 8, 8)
            world.reset()
            world.update(0.1)
            print(f"World context initialized and updated successfully")
    except Exception as e:
        print(f"World context error: {e}")
    
    print()

def demonstrate_enhanced_api():
    """Demonstrate enhanced API features."""
    print("=== Enhanced API Features ===")
    
    # Create some objects
    action = pynlm.Action.move_forward()
    vision = pynlm.Vision.from_image(100, 100)
    audio = pynlm.Audio.from_samples(16000, 16000)
    
    # Type conversion utilities
    action_str = pynlm.to_string(action)
    print(f"Action as string: {action_str}")
    
    vision_data = pynlm.to_vector(vision)
    print(f"Vision data length: {len(vision_data)}")
    
    audio_data = pynlm.to_vector(audio)
    print(f"Audio data length: {len(audio_data)}")
    
    # Action comparison
    action2 = pynlm.Action.move_forward()
    are_equal = pynlm.compare_actions(action, action2)
    print(f"Two move_forward actions equal: {are_equal}")
    
    # Get action type names
    action_names = pynlm.get_action_type_names()
    print(f"Action types: {action_names}")
    
    print()

def demonstrate_migration_guide():
    """Show how to migrate from old API to new API."""
    print("=== Migration Guide (Old API -> New API) ===")
    print()
    print("OLD API:")
    print("  action = pynlm.Action(pynlm.ActionType.MoveForward)")
    print("  vision = pynlm.Vision(640, 480, 3)")
    print("  audio = pynlm.Audio(44100, 44100)")
    print()
    print("NEW API:")
    print("  action = pynlm.Action.move_forward()")
    print("  vision = pynlm.Vision.from_image(640, 480)")
    print("  audio = pynlm.Audio.from_samples(44100, 44100)")
    print()
    print("Benefits of new API:")
    print("  - More Pythonic and readable")
    print("  - Builder pattern for fluent interface")
    print("  - Better validation and error handling")
    print("  - Smart pointer integration")
    print("  - Context managers for resource cleanup")
    print("  - Enhanced factory methods")
    print()

def main():
    """Run all demonstrations."""
    print("Enhanced Python Bindings for NLM - Demonstration")
    print("=" * 50)
    print()
    
    demonstrate_action_improvements()
    demonstrate_vision_improvements()
    demonstrate_audio_improvements()
    demonstrate_enhanced_factory_methods()
    demonstrate_error_handling()
    demonstrate_context_managers()
    demonstrate_enhanced_api()
    demonstrate_migration_guide()
    
    print("=" * 50)
    print("All demonstrations completed!")
    print()
    print("For more detailed documentation, see the migration guide")
    print("in the repository documentation.")

if __name__ == "__main__":
    main()
