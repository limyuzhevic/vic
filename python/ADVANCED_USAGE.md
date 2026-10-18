# NLM Python Bindings - Advanced Usage Guide

This guide covers advanced usage patterns and features specific to the NLM Python bindings.

## Advanced Sensory Input Manipulation

### 1. Vision Data Manipulation

```python
import pynlm

# Create a vision input with custom dimensions
vision = pynlm.createVision(width=128, height=128, channels=3)

# Create vision with pre-filled data
image_data = [0.0] * (128 * 128 * 3)  # Black image
for i in range(0, len(image_data), 3):
    # Create a simple pattern (bright center)
    x = (i // 3) % 128
    y = (i // 3) // 128
    dist = ((x - 64) ** 2 + (y - 64) ** 2) ** 0.5
    brightness = max(0, 1.0 - dist / 40)
    image_data[i] = brightness     # R
    image_data[i + 1] = brightness # G
    image_data[i + 2] = brightness # B

vision = pynlm.createVision(128, 128, 3, image_data)

# Apply noise to vision data
import random
for i in range(len(image_data)):
    image_data[i] += (random.random() - 0.5) * 0.1
vision.setData(image_data)

# Resize vision
vision.resize(64, 64)

# Get processed vision features
processor = pynlm.VisionProcessor()
processor.process(vision)
features = processor.getFeatures()
print(f"Feature dimensions: {processor.getFeatureDimensions()}")
```

### 2. Audio Signal Processing

```python
import pynlm
import math

# Create audio with sine wave
sample_rate = 44100
duration_ms = 2000
audio = pynlm.createDefaultAudio(sample_rate, duration_ms)

data = audio.getData()
for i in range(len(data)):
    t = i / sample_rate
    data[i] = 0.5 * math.sin(2 * math.pi * 440 * t)  # A4 note

audio.setData(data)

# Apply amplitude modulation
for i in range(len(data)):
    data[i] *= 0.5 * (1 + math.sin(2 * math.pi * 5 * i / sample_rate))  # 5Hz modulation

# Create an audio filter
def bandpass_filter(value, low_cut=1000, high_cut=5000, sample_rate=44100):
    if value == 0:
        return 0
    # Simple implementation
    return value

audio.applyFilter(bandpass_filter)

# Amplify audio
audio.amplify(2.0)

# Get audio features
processor = pynlm.AudioProcessor()
processor.process(audio)
audio_features = processor.getFeatures()
```

### 3. Advanced Internal Signals

```python
import pynlm
import numpy as np

# Create internal signals from Python list
signals = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
internal = pynlm.createInternalSignals(signals)

# Add more signals dynamically
internal.addSignal(1.5)
internal.addSignal(-0.5)

# Process signals
processor = pynlm.InternalSignalsProcessor()
processor.process(internal)

processed = processor.getProcessedSignals()
homeostatic = processor.getHomeostaticSignals()
print(f"Processed signals: {processed}")
print(f"Homeostatic signals: {homeostatic}")
```

## Batch Processing Utilities

### 1. Batch Vision Processing

```python
import pynlm
import time

# Create multiple vision inputs
visions = []
for i in range(10):
    vision = pynlm.createDefaultVision(64, 64)
    # Fill with different patterns
    data = [0.0] * (64 * 64 * 3)
    for j in range(len(data)):
        data[j] = (i + j) % 255 / 255.0
    vision.setData(data)
    visions.append(vision)

# Process all visions with a single processor
processor = pynlm.VisionProcessor()
start_time = time.time()

# This uses the batch processing utility
pynlm.batchProcessVision(visions, processor)

end_time = time.time()
print(f"Processed {len(visions)} visions in {end_time - start_time:.3f} seconds")

# Collect results
all_features = []
for vision in visions:
    # Note: In a real scenario, you'd need to process each one individually
    # This is a simplified example
    pass
```

### 2. Batch Audio Processing

```python
import pynlm
import time

# Create multiple audio clips
audios = []
for i in range(5):
    audio = pynlm.createDefaultAudio(22050, 500)  # 0.5 seconds each
    data = audio.getData()
    for j in range(len(data)):
        t = j / 22050
        data[j] = 0.3 * math.sin(2 * math.pi * (440 + i * 50) * t)
    audio.setData(data)
    audios.append(audio)

# Process all audios
processor = pynlm.AudioProcessor()
start_time = time.time()

pynlm.batchProcessAudio(audios, processor)

end_time = time.time()
print(f"Processed {len(audios)} audio clips in {end_time - start_time:.3f} seconds")
```

## Error Handling and Validation

### 1. Data Validation

```python
import pynlm

# Validate vision data
vision_data = [0.0] * (128 * 128 * 3)
try:
    if pynlm.validateVisionData(vision_data, 128, 128):
        print("Vision data validation passed")
    vision = pynlm.createVision(128, 128, 3, vision_data)
except Exception as e:
    print(f"Validation error: {e}")

# Validate audio data
audio_data = [0.0] * (44100 * 1000)  # 1 second at 44.1kHz
try:
    if pynlm.validateAudioData(audio_data, 44100):
        print("Audio data validation passed")
    audio = pynlm.createAudio(44100, 1000, audio_data)
except Exception as e:
    print(f"Validation error: {e}")
```

### 2. Graceful Error Recovery

```python
import pynlm

def safe_vision_creation(width, height, data=None):
    """Safely create a vision input with error handling."""
    try:
        if data is not None:
            if pynlm.validateVisionData(data, width, height):
                return pynlm.createVision(width, height, 3, data)
            else:
                print(f"Warning: Vision data invalid for {width}x{height}")
                return None
        else:
            return pynlm.createVision(width, height)
    except Exception as e:
        print(f"Error creating vision: {e}")
        return None

def safe_audio_creation(sample_rate, duration_ms, data=None):
    """Safely create an audio input with error handling."""
    try:
        if data is not None:
            if pynlm.validateAudioData(data, sample_rate, duration_ms):
                return pynlm.createAudio(sample_rate, duration_ms, data)
            else:
                print(f"Warning: Audio data invalid for {sample_rate}Hz, {duration_ms}ms")
                return None
        else:
            return pynlm.createDefaultAudio(sample_rate, duration_ms)
    except Exception as e:
        print(f"Error creating audio: {e}")
        return None
```

## Performance Monitoring

### 1. Memory Usage Monitoring

```python
import pynlm

# Get current memory usage
memory_info = pynlm.getMemoryUsage()
print(f"Memory status: {memory_info}")

# Monitor memory during batch operations
def monitor_memory_operation(operation_name, operation_func):
    """Monitor memory usage during an operation."""
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    mem_before = process.memory_info().rss / 1024 / 1024  # MB
    
    result = operation_func()
    
    mem_after = process.memory_info().rss / 1024 / 1024  # MB
    mem_used = mem_after - mem_before
    
    print(f"{operation_name}:")
    print(f"  Memory before: {mem_before:.1f} MB")
    print(f"  Memory after: {mem_after:.1f} MB")
    print(f"  Memory used: {mem_used:.1f} MB")
    
    return result
```

### 2. Performance Benchmarking

```python
import pynlm
import time

def benchmark_vision_processing(width, height, num_samples):
    """Benchmark vision processing performance."""
    print(f"Benchmarking vision processing ({width}x{height}, {num_samples} samples)...")
    
    # Create test data
    total_time = 0
    
    for i in range(num_samples):
        vision = pynlm.createDefaultVision(width, height)
        # Add some processing time
        data = vision.getData()
        for j in range(len(data)):
            data[j] = (i + j) % 255 / 255.0
        vision.setData(data)
        
        # Create and use processor
        processor = pynlm.VisionProcessor()
        start = time.time()
        processor.process(vision)
        end = time.time()
        
        total_time += (end - start)
    
    avg_time = total_time / num_samples
    print(f"  Average processing time: {avg_time * 1000:.2f} ms per image")
    print(f"  Throughput: {1 / avg_time:.2f} images/second")
    
    return avg_time

def benchmark_audio_processing(sample_rate, duration_ms, num_samples):
    """Benchmark audio processing performance."""
    print(f"Benchmarking audio processing ({sample_rate}Hz, {duration_ms}ms, {num_samples} samples)...")
    
    total_time = 0
    
    for i in range(num_samples):
        audio = pynlm.createDefaultAudio(sample_rate, duration_ms)
        data = audio.getData()
        
        # Simple processing
        start = time.time()
        for j in range(len(data)):
            t = j / sample_rate
            data[j] = 0.5 * math.sin(2 * math.pi * 440 * t)
        end = time.time()
        
        total_time += (end - start)
    
    avg_time = total_time / num_samples
    print(f"  Average processing time: {avg_time * 1000:.2f} ms per clip")
    print(f"  Throughput: {1 / avg_time:.2f} clips/second")
    
    return avg_time
```

## Configuration and Integration

### 1. Custom Configuration Loader

```python
import pynlm
import json

class NLMConfigurator:
    """Helper class for managing NLM configurations and settings."""
    
    @staticmethod
    def load_from_json(filepath):
        """Load NLM configuration from JSON file."""
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        config = pynlm.createDefaultConfig()
        
        # Apply configuration settings
        if 'brain' in config_data:
            brain_config = config_data['brain']
            if 'neuron_count' in brain_config:
                config.set("brain.neuron_count", brain_config['neuron_count'])
            if 'synapse_density' in brain_config:
                config.set("brain.synapse_density", brain_config['synapse_density'])
        
        return config
    
    @staticmethod
    def create_experiment_config(base_config, experiment_params):
        """Create configuration for specific experiments."""
        config = pynlm.createDefaultConfig()
        
        # Copy base settings
        for key in base_config.getKeys():
            config.set(key, base_config.get(key))
        
        # Apply experiment-specific settings
        for key, value in experiment_params.items():
            config.set(key, value)
        
        return config

# Example usage
experiment_config = {
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.5,
}

config = NLMConfigurator.create_experiment_config(
    pynlm.createDefaultConfig(),
    experiment_config
)
```

### 2. Advanced Agent Configuration

```python
import pynlm

class AdvancedAgent:
    """Advanced agent with multiple sensory modalities and processing pipelines."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.agent.initialize(self.world)
        
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Initialize processors
        self.vision_processor = pynlm.VisionProcessor()
        self.audio_processor = pynlm.AudioProcessor()
        self.internal_processor = pynlm.InternalSignalsProcessor()
        
        # Processing buffers
        self.vision_buffer = []
        self.audio_buffer = []
        self.internal_buffer = []
    
    def add_vision_sample(self, vision):
        """Add a vision sample for batch processing."""
        self.vision_buffer.append(vision)
    
    def add_audio_sample(self, audio):
        """Add an audio sample for batch processing."""
        self.audio_buffer.append(audio)
    
    def add_internal_sample(self, signals):
        """Add internal signals for batch processing."""
        self.internal_buffer.append(signals)
    
    def process_batch(self):
        """Process all buffered samples using batch utilities."""
        # Process vision batch
        if self.vision_buffer:
            pynlm.batchProcessVision(self.vision_buffer, self.vision_processor)
            self.vision_buffer.clear()
        
        # Process audio batch
        if self.audio_buffer:
            pynlm.batchProcessAudio(self.audio_buffer, self.audio_processor)
            self.audio_buffer.clear()
        
        # Process internal signals batch
        if self.internal_buffer:
            pynlm.batchProcessInternalSignals(self.internal_buffer, self.internal_processor)
            self.internal_buffer.clear()
        
        return {
            'vision_features': self.vision_processor.getFeatures() if self.vision_buffer else [],
            'audio_features': self.audio_processor.getFeatures() if self.audio_buffer else [],
            'internal_features': self.internal_processor.getProcessedSignals() if self.internal_buffer else []
        }
    
    def run_simulation(self, num_steps):
        """Run a complete simulation."""
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            # Extract reward from percept if available
            if percept.getInternal():
                reward = percept.getInternal()[0]
            
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Print progress periodically
            if step % 100 == 0:
                print(f"Step {step}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {self.agent.getNoveltyLevel():.3f}")
        
        return self.brain

# Advanced agent example
advanced_agent = AdvancedAgent()
result_brain = advanced_agent.run_simulation(1000)
print(f"Simulation complete. Final firing rate: {result_brain.getAverageFiringRate()}")
```

## Troubleshooting and Best Practices

### 1. Common Issues and Solutions

```python
# Problem: Brain not responding to sensory input
def check_sensory_connection(brain, world):
    """Check if sensory input is properly connected."""
    percept = world.getSensoryPercept()
    
    # Check if percept has valid data
    if percept.getVision():
        vision_data = percept.getVision().getData()
        if not vision_data or len(vision_data) == 0:
            print("Warning: Vision data is empty")
    
    if percept.getAudio():
        audio_data = percept.getAudio().getData()
        if not audio_data or len(audio_data) == 0:
            print("Warning: Audio data is empty")
    
    if percept.getInternal():
        internal_data = percept.getInternal()
        if not internal_data or len(internal_data) == 0:
            print("Warning: Internal signals are empty")

# Problem: Memory issues with large batch processing
def safe_batch_processing(samples, processor, max_memory_mb=500):
    """Safely process samples with memory monitoring."""
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    # Estimate memory usage
    estimated_memory = len(samples) * 10  # Rough estimate per sample in MB
    
    if estimated_memory > max_memory_mb:
        print(f"Warning: Estimated memory usage ({estimated_memory} MB) exceeds limit ({max_memory_mb} MB)")
        print("Consider reducing batch size or processing in smaller chunks")
        
        # Process in chunks
        chunk_size = max_memory_mb // 10
        for i in range(0, len(samples), chunk_size):
            chunk = samples[i:i + chunk_size]
            processor.process(*chunk)
            print(f"Processed chunk {i//chunk_size + 1}/{(len(samples) + chunk_size - 1)//chunk_size}")
    else:
        # Process all at once
        processor.process(*samples)
```

### 2. Performance Optimization Tips

```python
# Tip 1: Reuse processors instead of creating new ones
processor = pynlm.VisionProcessor()
for vision in visions:
    processor.process(vision)  # Reuse the same processor

# Tip 2: Pre-allocate data structures
for i in range(len(visions)):
    vision = visions[i]
    # Allocate once and reuse
    data = vision.getData()
    # ... process data

# Tip 3: Use smaller batch sizes for real-time applications
for vision in batch_of_visions:
    processor.process(vision)  # Process one at a time

# Tip 4: Monitor and log performance
import time
def timed_operation(operation, name):
    start = time.time()
    result = operation()
    end = time.time()
    print(f"{name}: {end - start:.3f} seconds")
    return result

# Use it
result = timed_operation(lambda: processor.process(vision), "Vision processing")

# Tip 5: Use synchronous processing for critical paths
def synchronous_processing(samples, processor):
    """Process samples synchronously for consistent timing."""
    results = []
    for sample in samples:
        processor.process(sample)
        results.append(processor.getFeatures())
    return results
```

## Extending the Python API

### 1. Adding Custom Processors

```python
import pynlm

class CustomVisionProcessor:
    """Custom vision processor that extends the built-in capabilities."""
    
    def __init__(self):
        self.processor = pynlm.VisionProcessor()
        self.custom_features = []
    
    def process(self, vision):
        """Process vision with custom enhancements."""
        # First, use the built-in processor
        self.processor.process(vision)
        
        # Add custom processing
        features = self.processor.getFeatures()
        
        # Apply custom transformations
        enhanced_features = self._enhance_features(features)
        self.custom_features.append(enhanced_features)
        
        return enhanced_features
    
    def _enhance_features(self, features):
        """Apply custom feature enhancement."""
        # Example: Apply a simple feature extraction
        if not features:
            return []
        
        # Extract simple statistics
        mean_val = sum(features) / len(features)
        std_val = (sum((x - mean_val) ** 2 for x in features) / len(features)) ** 0.5
        
        return {
            'mean': mean_val,
            'std': std_val,
            'min': min(features),
            'max': max(features),
            'range': max(features) - min(features)
        }
    
    def get_all_features(self):
        """Get all processed features."""
        return self.custom_features
```

### 2. Adding Custom Data Types

```python
import pynlm

class MultiModalSensoryData:
    """Custom multi-modal sensory data container."""
    
    def __init__(self, vision=None, audio=None, internal=None, timestamp=None):
        self.vision = vision
        self.audio = audio
        self.internal = internal
        self.timestamp = timestamp or time.time()
    
    def add_to_buffer(self, agent):
        """Add this sensory data to an agent's buffers."""
        if self.vision:
            agent.add_vision_sample(self.vision)
        
        if self.audio:
            agent.add_audio_sample(self.audio)
        
        if self.internal:
            agent.add_internal_sample(self.internal)
    
    def process(self, agent):
        """Process this multi-modal data through the agent."""
        # Add to buffers
        self.add_to_agent(agent)
        
        # Process batch
        return agent.process_batch()

# Example usage
multimodal_data = MultiModalSensoryData(
    vision=pynlm.createVision(64, 64),
    audio=pynlm.createAudio(22050, 500),
    internal=pynlm.createInternalSignals([0.5, 0.6, 0.7])
)

# Process through advanced agent
agent = AdvancedAgent()
result = multimodal_data.process(agent)
print(f"Processed multimodal data: {result}")
```

## Next Steps

1. **Explore Phase 6 experiments**: Run the comprehensive Phase 6 integration demo to see all systems working together
2. **Read architecture documentation**: Understand the system design and how to extend it
3. **Performance tuning**: Profile your applications and optimize for your specific use case
4. **Contribute to the community**: Share your extensions and improvements with others

The Python bindings for NLM provide a powerful interface to the underlying neural simulation framework, with extensive support for sensory processing, neural control, and agent-based learning. Use these advanced patterns to build sophisticated brain-inspired applications.