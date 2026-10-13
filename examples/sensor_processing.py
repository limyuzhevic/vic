import pynlm
import random

def main():
    """Sensor processing example demonstrating vision and audio sensors."""
    print("=== Sensor Processing Example ===")
    
    # === VISION SENSOR DEMONSTRATION ===
    print("\n1. Vision Sensor Testing:")
    
    # Create vision sensor with different resolutions
    resolutions = [
        (160, 120, 3),    # Standard
        (320, 240, 3),    # High definition
        (640, 480, 1),    # Grayscale
    ]
    
    for width, height, channels in resolutions:
        print(f"\n   Creating vision sensor: {width}x{height}x{channels}")
        
        vision = pynlm.Vision(width, height, channels)
        
        # Generate realistic vision data (simulating different scenarios)
        vision_data = []
        for _ in range(width * height * channels):
            if channels == 1:
                # Grayscale (simulating edge detection)
                vision_data.append(random.uniform(0, 1))
            else:
                # RGB (simulating natural images)
                vision_data.append(random.uniform(0, 1))
        
        vision.setData(vision_data)
        
        print(f"     - Width: {vision.getWidth()}")
        print(f"     - Height: {vision.getHeight()}")
        print(f"     - Channels: {vision.getChannels()}")
        print(f"     - Data points: {len(vision_data)}")
        
        # Test vision shape
        shape = vision.getShape() if hasattr(vision, 'getShape') else "N/A"
        print(f"     - Shape: {shape}")
    
    # === AUDIO SENSOR DEMONSTRATION ===
    print("\n2. Audio Sensor Testing:")
    
    # Create audio sensor with different configurations
    audio_configs = [
        (44100, 22050),  # 0.5 seconds at CD quality
        (16000, 8000),   # 0.5 seconds at telephony quality
        (8000, 4000),    # 0.5 seconds at low quality
    ]
    
    for sample_rate, num_samples in audio_configs:
        print(f"\n   Creating audio sensor: {sample_rate}Hz, {num_samples} samples")
        
        audio = pynlm.Audio(sample_rate, num_samples)
        
        # Generate realistic audio data (simulating different sound types)
        audio_data = []
        for _ in range(num_samples):
            # Simulate different types of audio: speech, music, noise
            audio_type = random.choice(['speech', 'music', 'noise'])
            
            if audio_type == 'speech':
                # Simulate speech (more sinusoidal with some noise)
                audio_data.append(random.uniform(-0.5, 0.5) * 0.8 + random.uniform(-0.1, 0.1))
            elif audio_type == 'music':
                # Simulate music (more complex patterns)
                audio_data.append(random.uniform(-1, 1) * 0.9 + random.uniform(-0.2, 0.2))
            else:
                # Simulate noise
                audio_data.append(random.uniform(-1, 1) * 0.7)
        
        audio.setData(audio_data)
        
        print(f"     - Sample rate: {audio.getSampleRate()} Hz")
        print(f"     - Number of samples: {audio.getNumSamples()}")
        print(f"     - Duration: {audio.getDuration():.3f} seconds")
        
        # Calculate audio characteristics
        avg_amplitude = sum(abs(x) for x in audio_data) / len(audio_data)
        max_amplitude = max(abs(x) for x in audio_data)
        print(f"     - Average amplitude: {avg_amplitude:.4f}")
        print(f"     - Max amplitude: {max_amplitude:.4f}")
    
    # === SENSORY PERCEPT INTEGRATION ===
    print("\n3. Sensory Percept Integration:")
    
    # Create sensory percept with multiple modalities
    percept = pynlm.SensoryPercept()
    
    # Add vision data
    vision = pynlm.Vision(80, 60, 3)
    vision_data = [random.uniform(0, 1) for _ in range(80 * 60 * 3)]
    vision.setData(vision_data)
    percept.setVision(vision)
    
    print(f"   Vision added: {vision.getWidth()}x{vision.getHeight()}")
    
    # Add audio data
    audio = pynlm.Audio(16000, 8000)
    audio_data = [random.uniform(-1, 1) for _ in range(8000)]
    audio.setData(audio_data)
    percept.setAudio(audio)
    
    print(f"   Audio added: {audio.getDuration():.2f} seconds")
    
    # Add internal signals (simulating internal states)
    internal_signals = pynlm.InternalSignals()
    for _ in range(5):
        internal_signals.addSignal(random.uniform(-0.5, 0.5))
    
    percept.setInternal(internal_signals)
    print(f"   Internal signals added: {internal_signals.getSignalCount()}")
    
    # Get all sensory signals
    all_signals = percept.getAllSignals()
    print(f"   Total sensory channels: {len(all_signals)}")
    
    # === BRAIN INTERACTION ===
    print("\n4. Brain-Sensor Integration:")
    
    # Create brain and test sensor processing
    brain = pynlm.create_brain_from_template("simple")
    brain.initialize()
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Process multiple sensory inputs
    for step in range(10):
        # Generate new sensory input each step
        new_vision = pynlm.Vision(40, 30, 3)
        new_vision_data = [random.uniform(0, 1) for _ in range(40 * 30 * 3)]
        new_vision.setData(new_vision_data)
        
        new_audio = pynlm.Audio(8000, 4000)
        new_audio_data = [random.uniform(-1, 1) for _ in range(4000)]
        new_audio.setData(new_audio_data)
        
        # Create percept and process
        new_percept = pynlm.SensoryPercept()
        new_percept.setVision(new_vision)
        new_percept.setAudio(new_audio)
        
        brain.receiveSensoryInput(new_percept)
        brain.step(step, step * 0.001)
        
        if step % 5 == 0:
            stats = pynlm.compute_neural_stats(brain)
            print(f"     Step {step}: {stats['firing_neurons']}/{stats['total_neurons']} neurons firing")
    
    # === SENSOR DATA PROCESSING ===
    print("\n5. Sensor Data Processing:")
    
    # Demonstrate data conversion utilities
    print("   Converting sensor data for processing...")
    
    # Process vision data
    vision_data = vision.getData()
    processed_vision = [max(0, min(1, x * 2 - 1)) for x in vision_data]  # Simple contrast enhancement
    print(f"     Vision data processed: {len(processed_vision)} points")
    
    # Process audio data
    audio_data = audio.getData()
    # Apply simple noise reduction (moving average)
    window_size = 5
    if len(audio_data) >= window_size:
        smoothed_audio = []
        for i in range(len(audio_data) - window_size + 1):
            window = audio_data[i:i + window_size]
            smoothed = sum(window) / len(window)
            smoothed_audio.append(smoothed)
        print(f"     Audio data smoothed: {len(smoothed_audio)} points")
    
    # === SENSOR SYNTHESIS ===
    print("\n6. Sensor Data Synthesis:")
    
    # Create synthetic sensory data for testing
    print("   Creating synthetic sensory data...")
    
    # Simulated visual scene: object in center
    scene_width, scene_height = 100, 100
    synthetic_vision = pynlm.Vision(scene_width, scene_height, 3)
    
    scene_data = []
    center_x, center_y = scene_width // 2, scene_height // 2
    radius = min(center_x, center_y) - 5
    
    for y in range(scene_height):
        for x in range(scene_width):
            # Calculate distance from center
            dx = x - center_x
            dy = y - center_y
            distance = (dx*dx + dy*dy) ** 0.5
            
            if distance < radius:
                # Bright center
                scene_data.extend([1.0, 1.0, 1.0])
            else:
                # Darker background
                brightness = max(0, 1 - distance / radius) * 0.3
                scene_data.extend([brightness, brightness, brightness])
    
    synthetic_vision.setData(scene_data)
    print(f"     Synthetic vision created: {scene_width}x{scene_height} with center object")
    
    # Simulated audio: tone burst
    tone_duration = 0.2  # seconds
    tone_samples = int(8000 * tone_duration)  # 8kHz sample rate
    tone_data = []
    
    for i in range(tone_samples):
        # Generate simple sine wave
        t = i / 8000.0
        sine_wave = 0.5 * (1 + (math.sin(2 * 3.14159 * 440 * t)))  # 440Hz tone
        tone_data.append(sine_wave)
    
    synthetic_audio = pynlm.Audio(8000, tone_samples)
    synthetic_audio.setData(tone_data)
    
    print(f"     Synthetic audio created: {synthetic_audio.getDuration():.2f}s tone at 440Hz")
    
    print("\n=== Sensor Processing Example Complete ===")
    print("\nSensor processing features demonstrated:")
    print("  ✓ Vision sensor creation and data handling")
    print("  ✓ Audio sensor creation and processing")
    print("  ✓ Sensory percept integration")
    print("  ✓ Brain-sensory interaction")
    print("  ✓ Data conversion and processing")
    print("  ✓ Synthetic sensor data generation")

if __name__ == "__main__":
    import math
    main()