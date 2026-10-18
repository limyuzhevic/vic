"""
Sensory Analysis - Understanding Brain Input
============================================

This example demonstrates how to analyze and work with sensory inputs.
It shows different types of sensory data and how to extract information from them.

Key Concepts Demonstrated:
- Vision sensory processing (image-like data)
- Audio sensory processing (time-series data)
- Internal signals (brain state)
- Proprioception (body position)
- Touch signals (sensory receptors)

Expected Output:
---------------
Sensory Analysis Results:
Vision Sensory Input:
  Dimensions: 8x8 (3 channels)
  Data points: 192 total pixels
  Average intensity: 0.512
  Min intensity: 0.123
  Max intensity: 0.987

Audio Sensory Input:
  Sample rate: 44100 Hz
  Number of samples: 1024
  Duration: 0.023 seconds
  Average amplitude: 0.145

Internal Signals:
  Number of signals: 3
  Signal values: [0.234, 0.567, 0.891]

Touch Signals:
  Number of touch sensors: 8
  Touch values: [0.111, 0.222, 0.333, 0.444, 0.555, 0.666, 0.777, 0.888]

Proprioception:
  Body position: (x: 10.00, y: 10.00)
  Orientation: 0.00 radians

Sensory analysis complete!
"""

import pynlm
import numpy as np

def analyze_vision(vision):
    """Analyze vision sensory input"""
    width = vision.getWidth()
    height = vision.getHeight()
    channels = vision.getChannels()
    
    data = vision.getData()
    data_array = np.array(data)
    
    return {
        'width': width,
        'height': height,
        'channels': channels,
        'total_pixels': width * height * channels,
        'avg_intensity': float(np.mean(data_array)),
        'min_intensity': float(np.min(data_array)),
        'max_intensity': float(np.max(data_array)),
        'std_intensity': float(np.std(data_array))
    }

def analyze_audio(audio):
    """Analyze audio sensory input"""
    sample_rate = audio.getSampleRate()
    num_samples = audio.getNumSamples()
    
    data = audio.getData()
    data_array = np.array(data)
    
    duration = num_samples / sample_rate if sample_rate > 0 else 0
    
    return {
        'sample_rate': sample_rate,
        'num_samples': num_samples,
        'duration': duration,
        'avg_amplitude': float(np.mean(data_array)),
        'max_amplitude': float(np.max(data_array)),
        'min_amplitude': float(np.min(data_array)),
        'rms_amplitude': float(np.sqrt(np.mean(data_array**2)))
    }

def main():
    print("Sensory Analysis Results:")
    print("=" * 40)
    
    # Create world and get sensory percept
    world = pynlm.createSimpleWorld()
    world.configure(32, 32, 8, 8)
    world.reset()
    
    # Update world to generate sensory input
    for _ in range(10):
        world.update(0.1)
    
    percept = world.getSensoryPercept()
    
    # Analyze vision sensory input
    print("\nVision Sensory Input:")
    print(f"  Dimensions: {percept.getVisionWidth()}x{percept.getVisionHeight()} "
          f"({percept.getVision()[0]:.3f} avg pixel value)")
    
    vision_data = analyze_vision(percept.getVision())
    print(f"  Data points: {vision_data['total_pixels']} total pixels")
    print(f"  Average intensity: {vision_data['avg_intensity']:.3f}")
    print(f"  Min intensity: {vision_data['min_intensity']:.3f}")
    print(f"  Max intensity: {vision_data['max_intensity']:.3f}")
    
    # Create audio input for analysis
    print("\nAudio Sensory Input:")
    audio = pynlm.to_audio(22050, 2048)
    audio_data = analyze_audio(audio)
    print(f"  Sample rate: {audio_data['sample_rate']} Hz")
    print(f"  Number of samples: {audio_data['num_samples']}")
    print(f"  Duration: {audio_data['duration']:.3f} seconds")
    print(f"  Average amplitude: {audio_data['avg_amplitude']:.3f}")
    print(f"  RMS amplitude: {audio_data['rms_amplitude']:.3f}")
    
    # Get internal signals
    print("\nInternal Signals:")
    internal = percept.getInternal()
    print(f"  Number of signals: {len(internal)}")
    if internal:
        print(f"  Signal values: {[f'{x:.3f}' for x in internal]}")
    
    # Get touch signals
    print("\nTouch Signals:")
    touch = percept.getTouch()
    print(f"  Number of touch sensors: {len(touch)}")
    if touch:
        print(f"  Touch values: {[f'{x:.3f}' for x in touch]}")
    
    # Get proprioception (body position)
    print("\nProprioception:")
    body = world.getAgentBody()
    print(f"  Body position: (x: {body.x:.2f}, y: {body.y:.2f})")
    print(f"  Orientation: {body.orientation:.2f} radians")
    
    print("\nSensory analysis complete!")

if __name__ == "__main__":
    main()