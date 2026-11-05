"""
Enhanced NumPy Integration for NLM Python API

This module provides enhanced NumPy support for the NLM Python bindings.
It includes:

1. Enhanced Vision and Audio classes with NumPy array support
2. Convenience functions for loading data from NumPy arrays
3. Configuration loading from NumPy arrays
4. Type conversion utilities

Usage:
    from pynlm import Vision, Audio, loadImageFromNumPy, loadAudioFromNumPy
    import numpy as np
    
    # Create vision from NumPy array
    image_data = np.random.rand(224, 224, 3).astype(np.float32)
    vision = loadImageFromNumPy(image_data, 224, 224, 3)
    
    # Create audio from NumPy array
    audio_data = np.random.rand(44100).astype(np.float32)
    audio = loadAudioFromNumPy(audio_data, 44100)
    
    # Access data as NumPy arrays
    vision_array = vision.toNumPyArray()
    audio_array = audio.toNumPyArray()
"""

# This is a standalone example file that demonstrates NumPy integration
# with the NLM Python API. It should be used as a reference for users
# who want to use NumPy arrays with NLM.

import numpy as np
from pynlm import Vision, Audio, loadImageFromNumPy, loadAudioFromNumPy

def demonstrate_numpy_integration():
    """
    Demonstrate how to use NumPy arrays with NLM
    """
    print("=== NLM NumPy Integration Demo ===")
    
    # Create sample data using NumPy
    print("1. Creating sample image data with NumPy...")
    image_height, image_width, image_channels = 64, 64, 3
    image_data = np.random.randn(image_height, image_width, image_channels).astype(np.float32)
    print(f"   Created image data with shape: {image_data.shape}")
    
    # Create vision from NumPy array
    print("2. Creating Vision from NumPy array...")
    vision = loadImageFromNumPy(image_data, image_width, image_height, image_channels)
    print(f"   Vision created with dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Convert back to NumPy
    print("3. Converting vision data back to NumPy...")
    vision_array = vision.toNumPyArray()
    print(f"   Vision array shape: {vision_array.shape}")
    print(f"   Vision array dtype: {vision_array.dtype}")
    
    # Create audio data
    print("4. Creating sample audio data with NumPy...")
    sample_rate = 22050
    duration = 1.0  # 1 second
    audio_samples = int(sample_rate * duration)
    audio_data = np.random.randn(audio_samples).astype(np.float32) * 0.1
    print(f"   Created audio data with shape: {audio_data.shape}")
    
    # Create audio from NumPy array
    print("5. Creating Audio from NumPy array...")
    audio = loadAudioFromNumPy(audio_data, sample_rate)
    print(f"   Audio created with {audio.getNumSamples()} samples at {audio.getSampleRate()} Hz")
    
    # Convert back to NumPy
    print("6. Converting audio data back to NumPy...")
    audio_array = audio.toNumPyArray()
    print(f"   Audio array shape: {audio_array.shape}")
    print(f"   Audio array dtype: {audio_array.dtype}")
    
    # Create configuration from NumPy
    print("7. Creating configuration from NumPy array...")
    config_data = np.array([1000, 5, 0.1], dtype=np.float64)  # [neuron_count, region_count, connection_probability]
    from pynlm import loadConfigFromNumPy
    config = loadConfigFromNumPy(config_data)
    print(f"   Configuration loaded:")
    print(f"     - Neuron count: {config.get('neuron_count')}")
    print(f"     - Region count: {config.get('region_count')}")
    print(f"     - Connection probability: {config.get('connection_probability')}")
    
    # Demonstrate batch operations
    print("8. Demonstrating batch operations...")
    num_images = 5
    batch_images = np.random.randn(num_images, image_height, image_width, image_channels).astype(np.float32)
    print(f"   Created batch of {num_images} images with shape: {batch_images.shape}")
    
    # Process batch (simplified example)
    for i in range(num_images):
        vision = loadImageFromNumPy(batch_images[i], image_width, image_height, image_channels)
        # Process each vision...
        if i == 0:
            print(f"   Processed batch image {i+1}")
    
    print("9. NumPy integration demo completed successfully!")
    print("\nKey benefits of NumPy integration:")
    print("  - Fast vectorized operations with NumPy")
    print("  - Easy data manipulation and processing")
    print("  - Seamless conversion between NumPy arrays and NLM objects")
    print("  - Support for batch processing")
    print("  - Integration with existing NumPy-based workflows")

if __name__ == "__main__":
    demonstrate_numpy_integration()
