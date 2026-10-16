#!/usr/bin/env python3
"""Demonstrate working with sensory inputs.

This example shows how NLM brains process different types of sensory data:
- Vision (camera-like input)
- Audio (sound input)
- Internal signals (internal state)
"""

import pynlm
import sys
import math

def create_synthetic_vision_data(width, height, channels):
    """Create synthetic vision data for testing."""
    data = []
    for h in range(height):
        for w in range(width):
            # Create a simple pattern: center bright, edges dark
            distance = math.sqrt((w - width/2)**2 + (h - height/2)**2)
            intensity = max(0, 1.0 - distance / (min(width, height) / 2))
            for c in range(channels):
                data.append(intensity)
    return data

def create_synthetic_audio_data(sample_rate, num_samples):
    """Create synthetic audio data (sine waves)."""
    data = []
    for i in range(num_samples):
        # Mix of sine waves at different frequencies
        t = i / sample_rate
        sample = 0.5 * math.sin(2 * math.pi * 440 * t) +  # A4 note
                         0.3 * math.sin(2 * math.pi * 880 * t)  # A5 note
        data.append(sample)
    return data

def main():
    print("=== NLM Sensory Input Demo ===\n")
    
    # 1. Setup brain and world
    print("Step 1: Setting up simulation environment")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("✓ Environment ready")
    print()
    
    # 2. Vision sensory demonstration
    print("Step 2: Vision sensory processing")
    
    vision = pynlm.Vision(world.getWidth(), world.getHeight(), 3)
    vision_data = create_synthetic_vision_data(
        world.getWidth(), world.getHeight(), 3
    )
    vision.setData(vision_data)
    
    print(f"  Vision dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    print(f"  Data size: {len(vision.getData())} elements")
    print(f"  Average intensity: {sum(vision.getData())/len(vision.getData()):.3f}")
    
    # Process vision through agent
    agent.processSensoryInput(vision)
    print("  ✓ Vision processed by agent")
    print()
    
    # 3. Audio sensory demonstration
    print("Step 3: Audio sensory processing")
    
    audio = pynlm.Audio(44100, 1024)  # CD quality sample rate
    audio_data = create_synthetic_audio_data(44100, 1024)
    audio.setData(audio_data)
    audio.setSampleRate(44100)
    
    print(f"  Audio sample rate: {audio.getSampleRate()} Hz")
    print(f"  Duration: {len(audio.getData()) / 44100:.2f} seconds")
    print(f"  Average amplitude: {sum(audio.getData())/len(audio.getData()):.3f}")
    
    # Process audio through agent
    agent.processSensoryInput(audio)
    print("  ✓ Audio processed by agent")
    print()
    
    # 4. Internal signals demonstration
    print("Step 4: Internal signals processing")
    
    internal = pynlm.InternalSignals()
    
    # Add various internal signals
    signals = [0.2, -0.5, 0.8, -0.3, 0.1]
    for i, signal in enumerate(signals):
        internal.addSignal(signal)
        print(f"  Added signal {i}: {signal}")
    
    print(f"  Total signals: {len(internal.getData())}")
    print(f"  Signal range: [{min(internal.getData()):.2f}, {max(internal.getData()):.2f}]")
    
    # Process internal signals
    agent.processSensoryInput(internal)
    print("  ✓ Internal signals processed by agent")
    print()
    
    # 5. Combined sensory processing
    print("Step 5: Combined sensory processing")
    
    # Simulate world update with all sensory modalities
    for step in range(20):
        world.update(0.1)
        
        # Get all sensory inputs
        percept = world.getSensoryPercept()
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Brain processes sensory input
        brain.step(step)
        
        # Show what's being processed
        if step % 5 == 0:
            body = world.getAgentBody()
            print(f"  Step {step}: Pos({body.x:.1f},{body.y:.1f}), "
                  f"Firing:{brain.getFiringNeuronCount()}, "
                  f"FiringRate:{brain.getAverageFiringRate():.2f}Hz")
    
    print()
    print("=== Sensory Input Demo Complete ===")

if __name__ == "__main__":
    try:
        main()
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Make sure NLM Python bindings are installed.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running sensory demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
