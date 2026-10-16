#!/usr/bin/env python3
"""
Example usage of the improved NLM Python bindings

This script demonstrates the new convenience methods and improved API
introduced in the NLM Python bindings.
"""

def example_config_usage():
    """Demonstrate improved Config class API"""
    print("=== Config Class Examples ===")
    
    # Create a default config
    config = nlm.createDefaultConfig()
    print("Created default config")
    
    # Set values
    config.set("learning_rate", 0.05)
    config.set("population_size", 500)
    config.set("enable_plasticity", True)
    
    # Use convenience getter methods
    lr = config.getFloat("learning_rate")
    pop_size = config.getInt("population_size")
    plasticity = config.getBool("enable_plasticity")
    
    print(f"Learning rate: {lr}")
    print(f"Population size: {pop_size}")
    print(f"Plasticity enabled: {plasticity}")
    
    # Use type-specific getters for common parameters
    learning_rate = config.getLearningRate()
    decay_rate = config.getDecayRate()
    connection_density = config.getConnectionDensity()
    
    print(f"Learning rate: {learning_rate}")
    print(f"Decay rate: {decay_rate}")
    print(f"Connection density: {connection_density}")
    
    # Check if key exists
    if config.has("custom_param"):
        value = config.getString("custom_param")
        print(f"Custom param: {value}")
    else:
        print("Custom param not found")

def example_vision_usage():
    """Demonstrate improved Vision class API"""
    print("\n=== Vision Class Examples ===")
    
    # Create a vision sensor
    vision = nlm.createVisionSensor(84, 84, 3)
    
    # Set some data (simulated image data)
    import random
    data = [random.random() for _ in range(84 * 84 * 3)]
    vision.setData(data)
    
    # Get image properties
    print(f"Width: {vision.getWidth()}")
    print(f"Height: {vision.getHeight()}")
    print(f"Channels: {vision.getChannels()}")
    
    # Use vision processing methods
    # Note: These methods are implemented in C++ but demonstrated here
    print("Vision processing methods available:")
    print("- getChannelData(channel)")
    print("- applyFilter(kernel)")
    print("- normalize(target_mean, target_stddev)")
    print("- scale(factor)")
    print("- resize(new_width, new_height)")

def example_audio_usage():
    """Demonstrate improved Audio class API"""
    print("\n=== Audio Class Examples ===")
    
    # Create an audio sensor
    audio = nlm.createAudioSensor(44100, 44100)  # 1 second at 44.1kHz
    
    # Set some audio data
    import random
    data = [random.random() * 2 - 1 for _ in range(44100)]
    audio.setData(data)
    
    # Get audio properties
    print(f"Sample rate: {audio.getSampleRate()}")
    print(f"Number of samples: {audio.getNumSamples()}")
    print(f"Duration: {audio.getDuration():.2f} seconds")
    
    # Apply gain
    audio.applyGain(-6.0)  # -6 dB
    print("Applied -6 dB gain")
    
    # Get frequency content
    freqs = audio.getFrequency()
    print(f"Frequency analysis available (first 10 bins): {freqs[:10]}")
    
    # Set sample rate
    audio.setSampleRate(48000)
    print(f"New sample rate: {audio.getSampleRate()}")

def example_action_usage():
    """Demonstrate improved Action class API"""
    print("\n=== Action Class Examples ===")
    
    # Create actions using the factory function
    move_action = nlm.createAction(nlm.ActionType.MoveForward, [0.5, 0.0])
    print(f"Move action name: {move_action.getName()}")
    print(f"Move action type: {move_action.getType()}")
    print(f"Move action parameters: {move_action.getParameters()}")
    
    # Use string representation methods
    action_str = move_action.toString()
    print(f"Action string representation: {action_str}")
    
    # Parse action from string
    parsed_action = nlm.Action.fromString(action_str)
    if parsed_action:
        print(f"Parsed action type: {parsed_action.getType()}")
        print(f"Parsed action parameters: {parsed_action.getParameters()}")
    
    # Create more actions using factory functions
    move_action = nlm.createMoveAction(1.0, 0.5)
    look_action = nlm.createLookAction(0.5, -0.3)
    
    print(f"Created movement action: {move_action.toString()}")
    print(f"Created look action: {look_action.toString()}")

def example_utility_functions():
    """Demonstrate utility functions"""
    print("\n=== Utility Functions Examples ===")
    
    import random
    
    # Create sample data
    data = [random.random() for _ in range(10)]
    print(f"Original data: {data}")
    
    # Normalize data
    normalized = nlm.normalizeData(data, 0.0, 1.0)
    print(f"Normalized data: {normalized}")
    
    # Scale data
    scaled = nlm.scaleData(data, 2.0)
    print(f"Scaled data (factor 2.0): {scaled}")
    
    # Clip data
    clipped = nlm.clipData(data, 0.3, 0.7)
    print(f"Clipped data [0.3, 0.7]: {clipped}")
    
    # Create neural IDs
    neuron_id = nlm.getRandomNeuronId()
    print(f"Random neuron ID: {neuron_id}")
    
    synapse_id = nlm.getRandomSynapseId()
    print(f"Random synapse ID: {synapse_id}")
    
    region_id = nlm.getRandomRegionId()
    print(f"Random region ID: {region_id}")
    
    # Type conversion utilities
    neuron_id_int = nlm.neuronIdToInt(neuron_id)
    print(f"Neuron ID as int: {neuron_id_int}")
    
    restored_neuron_id = nlm.intToNeuronId(neuron_id_int)
    print(f"Restored neuron ID: {restored_neuron_id}")
    print(f"IDs match: {neuron_id == restored_neuron_id}")

def example_factory_functions():
    """Demonstrate specialized factory functions"""
    print("\n=== Factory Functions Examples ===")
    
    # Create a simple robot brain
    robot_brain = nlm.createSimpleRobotBrain()
    print("Created simple robot brain with basic settings")
    
    # Create a learning agent world
    world = nlm.createLearningAgentWorld()
    print("Created learning agent world (84x84 vision)")
    
    # Create sensory inputs
    vision_sensor = nlm.createVisionSensor(224, 224, 3)
    print("Created vision sensor (224x224 RGB)")
    
    audio_sensor = nlm.createAudioSensor(16000, 16000)  # 1 second at 16kHz
    print("Created audio sensor (16kHz, 1 second)")

if __name__ == "__main__":
    print("NLM Python Bindings - API Examples")
    print("=" * 50)
    
    # Note: These examples assume the pynlm module has been built and imported
    try:
        import nlm
        print("Successfully imported nlm module")
        
        example_config_usage()
        example_vision_usage()
        example_audio_usage()
        example_action_usage()
        example_utility_functions()
        example_factory_functions()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        
    except ImportError as e:
        print(f"Error importing nlm module: {e}")
        print("Please build the Python bindings first:")
        print("  pip install -e .")
        print("or")
        print("  python -m pip install scikit-build-core pybind11")
        print("Then run: python setup.py build_ext --inplace")
        
    except Exception as e:
        print(f"Error during examples: {e}")
        import traceback
        traceback.print_exc()
