# NLM Python Bindings - Performance Benchmarks and Best Practices Guide

This guide provides performance benchmarks, best practices, and optimization techniques for the NLM Python bindings.

## Performance Benchmarks

### 1. Sensory Input Processing Benchmarks

#### Vision Processing Benchmark

```python
import pynlm
import time
import statistics
from memory_profiler import memory_usage

def benchmark_vision_processing(width, height, iterations=100):
    """Benchmark vision processing performance."""
    print(f"=== Vision Processing Benchmark: {width}x{height} ===")
    
    # Setup
    vision = pynlm.createVision(width, height, 3)
    processor = pynlm.VisionProcessor()
    
    # Generate test data
    data = [0.0] * (width * height * 3)
    for i in range(iterations):
        # Add some variation to simulate real data
        for j in range(len(data)):
            data[j] = (i + j + (j // 100)) % 255 / 255.0
        
        vision.setData(data)
    
    # Memory usage before processing
    mem_before = memory_usage(-1, interval=0.1, timeout=1)[0]
    
    # Processing benchmark
    times = []
    for i in range(iterations):
        start_time = time.perf_counter()
        vision.setData(data)
        processor.process(vision)
        features = processor.getFeatures()
        end_time = time.perf_counter()
        
        times.append((end_time - start_time) * 1000)  # Convert to milliseconds
    
    # Memory usage after processing
    mem_after = memory_usage(-1, interval=0.1, timeout=1)[0]
    mem_used = mem_after - mem_before
    
    # Calculate statistics
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    std_time = statistics.stdev(times) if len(times) > 1 else 0
    throughput = 1000 / avg_time * iterations  # images per second
    
    print(f"  Average processing time: {avg_time:.2f} ms")
    print(f"  Min processing time: {min_time:.2f} ms")
    print(f"  Max processing time: {max_time:.2f} ms")
    print(f"  Standard deviation: {std_time:.2f} ms")
    print(f"  Throughput: {throughput:.2f} images/second")
    print(f"  Memory used: {mem_used:.1f} MB")
    print(f"  Features per processing: {len(features) if features else 0}")
    
    return {
        'avg_time_ms': avg_time,
        'min_time_ms': min_time,
        'max_time_ms': max_time,
        'std_time_ms': std_time,
        'throughput_fps': throughput / 1000,
        'memory_used_mb': mem_used,
        'features_count': len(features) if features else 0
    }

def benchmark_vision_scalability():
    """Benchmark vision processing at different resolutions."""
    print("=== Vision Processing Scalability Benchmark ===")
    
    resolutions = [
        (32, 32, "Small (32x32)"),
        (64, 64, "Medium (64x64)"),
        (128, 128, "Large (128x128)"),
        (256, 256, "Very Large (256x256)"),
    ]
    
    results = []
    
    for width, height, description in resolutions:
        print(f"\nTesting {description}:")
        result = benchmark_vision_processing(width, height, iterations=50)
        result['resolution'] = (width, height)
        result['description'] = description
        results.append(result)
    
    # Summary
    print("\n=== Scalability Summary ===")
    for result in results:
        print(f"{result['description']}: {result['avg_time_ms']:.2f} ms, "
              f"{result['throughput_fps']:.2f} FPS")
    
    return results
```

#### Audio Processing Benchmark

```python
import pynlm
import time
import statistics
import math

def benchmark_audio_processing(duration_ms, iterations=50):
    """Benchmark audio processing performance."""
    print(f"=== Audio Processing Benchmark: {duration_ms}ms ===")
    
    sample_rate = 44100
    num_samples = (sample_rate * duration_ms) / 1000
    
    # Setup
    audio = pynlm.createAudio(sample_rate, num_samples)
    processor = pynlm.AudioProcessor()
    
    # Generate test data (sine waves)
    for i in range(iterations):
        data = audio.getData()
        for j in range(len(data)):
            t = j / sample_rate
            # Mix multiple frequencies
            data[j] = 0.3 * math.sin(2 * math.pi * 440 * t) + \
                     0.2 * math.sin(2 * math.pi * 880 * t) + \
                     0.1 * math.sin(2 * math.pi * 1320 * t)
        
        audio.setData(data)
    
    # Processing benchmark
    times = []
    for i in range(iterations):
        start_time = time.perf_counter()
        audio.setData(audio.getData())  # Simulate new data
        processor.process(audio)
        features = processor.getFeatures()
        end_time = time.perf_counter()
        
        times.append((end_time - start_time) * 1000)  # Convert to milliseconds
    
    # Calculate statistics
    avg_time = statistics.mean(times)
    min_time = min(times)
    max_time = max(times)
    std_time = statistics.stdev(times) if len(times) > 1 else 0
    throughput = 1000 / avg_time * iterations  # clips per second
    
    print(f"  Average processing time: {avg_time:.2f} ms")
    print(f"  Min processing time: {min_time:.2f} ms")
    print(f"  Max processing time: {max_time:.2f} ms")
    print(f"  Standard deviation: {std_time:.2f} ms")
    print(f"  Throughput: {throughput:.2f} clips/second")
    print(f"  Features per processing: {len(features) if features else 0}")
    
    return {
        'avg_time_ms': avg_time,
        'min_time_ms': min_time,
        'max_time_ms': max_time,
        'std_time_ms': std_time,
        'throughput_fps': throughput / 1000,
        'samples_per_clip': num_samples
    }

def benchmark_audio_scalability():
    """Benchmark audio processing at different durations."""
    print("=== Audio Processing Scalability Benchmark ===")
    
    durations = [
        (100, "Short (100ms)"),
        (500, "Medium (500ms)"),
        (1000, "Long (1s)"),
        (2000, "Very Long (2s)"),
    ]
    
    results = []
    
    for duration_ms, description in durations:
        print(f"\nTesting {description}:")
        result = benchmark_audio_processing(duration_ms, iterations=20)
        result['duration_ms'] = duration_ms
        result['description'] = description
        results.append(result)
    
    # Summary
    print("\n=== Scalability Summary ===")
    for result in results:
        print(f"{result['description']}: {result['avg_time_ms']:.2f} ms, "
              f"{result['throughput_fps']:.2f} FPS")
    
    return results
```

### 2. Batch Processing Benchmarks

#### Batch Vision Processing Benchmark

```python
import pynlm
import time
import statistics
from memory_profiler import memory_usage

def benchmark_batch_vision_processing(width, height, batch_sizes=[1, 10, 50, 100]):
    """Benchmark batch vision processing performance."""
    print(f"=== Batch Vision Processing Benchmark: {width}x{height} ===")
    
    # Create test data
    single_vision = pynlm.createVision(width, height, 3)
    data = [0.0] * (width * height * 3)
    for i in range(len(data)):
        data[i] = (i % 255) / 255.0
    
    processor = pynlm.VisionProcessor()
    
    results = []
    
    for batch_size in batch_sizes:
        print(f"\nTesting batch size {batch_size}:")
        
        # Create batch of visions
        visions = []
        for i in range(batch_size):
            vision = pynlm.createVision(width, height, 3)
            # Use different data for each vision
            batch_data = data.copy()
            for j in range(len(batch_data)):
                batch_data[j] = (i + j) % 255 / 255.0
            vision.setData(batch_data)
            visions.append(vision)
        
        # Memory usage before processing
        mem_before = memory_usage(-1, interval=0.1, timeout=1)[0]
        
        # Processing benchmark
        start_time = time.perf_counter()
        
        # Use the batch processing utility
        pynlm.batchProcessVision(visions, processor)
        
        end_time = time.perf_counter()
        
        # Memory usage after processing
        mem_after = memory_usage(-1, interval=0.1, timeout=1)[0]
        mem_used = mem_after - mem_before
        
        # Calculate metrics
        total_time = (end_time - start_time) * 1000  # Convert to milliseconds
        avg_time_per_vision = total_time / batch_size
        throughput = batch_size / (total_time / 1000)  # visions per second
        
        print(f"  Total batch time: {total_time:.2f} ms")
        print(f"  Average per vision: {avg_time_per_vision:.2f} ms")
        print(f"  Throughput: {throughput:.2f} visions/second")
        print(f"  Memory used: {mem_used:.1f} MB")
        print(f"  Memory per vision: {mem_used/batch_size:.3f} MB")
        
        results.append({
            'batch_size': batch_size,
            'total_time_ms': total_time,
            'avg_time_per_vision_ms': avg_time_per_vision,
            'throughput_fps': throughput / 1000,
            'memory_used_mb': mem_used,
            'memory_per_vision_mb': mem_used / batch_size
        })
    
    # Summary
    print("\n=== Batch Processing Summary ===")
    print("Batch Size | Avg Time/Vision | Throughput | Memory/Vision")
    print("-" * 60)
    for result in results:
        print(f"{result['batch_size']:9d} | {result['avg_time_per_vision_ms']:15.2f} ms | "
              f"{result['throughput_fps']:10.2f} FPS | {result['memory_per_vision_mb']:12.3f} MB")
    
    return results
```

#### Batch Audio Processing Benchmark

```python
import pynlm
import time
import statistics

def benchmark_batch_audio_processing(duration_ms, batch_sizes=[1, 5, 10, 20]):
    """Benchmark batch audio processing performance."""
    print(f"=== Batch Audio Processing Benchmark: {duration_ms}ms ===")
    
    sample_rate = 22050
    processor = pynlm.AudioProcessor()
    
    results = []
    
    for batch_size in batch_sizes:
        print(f"\nTesting batch size {batch_size}:")
        
        # Create batch of audios
        audios = []
        for i in range(batch_size):
            audio = pynlm.createAudio(sample_rate, int(sample_rate * duration_ms / 1000))
            data = audio.getData()
            for j in range(len(data)):
                t = j / sample_rate
                data[j] = 0.3 * math.sin(2 * math.pi * (440 + i * 50) * t)
            audio.setData(data)
            audios.append(audio)
        
        # Processing benchmark
        start_time = time.perf_counter()
        
        # Use the batch processing utility
        pynlm.batchProcessAudio(audios, processor)
        
        end_time = time.perf_counter()
        
        # Calculate metrics
        total_time = (end_time - start_time) * 1000  # Convert to milliseconds
        avg_time_per_audio = total_time / batch_size
        throughput = batch_size / (total_time / 1000)  # clips per second
        
        print(f"  Total batch time: {total_time:.2f} ms")
        print(f"  Average per audio: {avg_time_per_audio:.2f} ms")
        print(f"  Throughput: {throughput:.2f} clips/second")
        
        results.append({
            'batch_size': batch_size,
            'total_time_ms': total_time,
            'avg_time_per_audio_ms': avg_time_per_audio,
            'throughput_fps': throughput / 1000,
            'samples_per_audio': len(audios[0]) if audios else 0
        })
    
    # Summary
    print("\n=== Batch Processing Summary ===")
    print("Batch Size | Avg Time/Audio | Throughput | Samples/Audio")
    print("-" * 60)
    for result in results:
        print(f"{result['batch_size']:9d} | {result['avg_time_per_audio_ms']:15.2f} ms | "
              f"{result['throughput_fps']:10.2f} FPS | {result['samples_per_audio']:13d}")
    
    return results
```

### 3. End-to-End System Benchmarks

#### Complete Agent Simulation Benchmark

```python
import pynlm
import time
import statistics
import psutil
import os

def get_memory_usage():
    """Get current memory usage in MB."""
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024

def benchmark_agent_simulation(num_steps=1000, num_runs=5):
    """Benchmark complete agent simulation performance."""
    print(f"=== Complete Agent Simulation Benchmark: {num_steps} steps, {num_runs} runs ===")
    
    # Configure agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Benchmark individual runs
    all_times = []
    all_memory_usages = []
    
    for run in range(num_runs):
        print(f"\nRun {run + 1}/{num_runs}:")
        
        # Reset state
        brain.reset()
        agent.reset()
        world.reset()
        
        # Measure memory before simulation
        mem_before = get_memory_usage()
        
        # Run simulation
        start_time = time.perf_counter()
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Measure time and memory
        end_time = time.perf_counter()
        mem_after = get_memory_usage()
        
        simulation_time = (end_time - start_time) * 1000  # Convert to milliseconds
        memory_used = mem_after - mem_before
        
        all_times.append(simulation_time)
        all_memory_usages.append(memory_used)
        
        print(f"  Simulation time: {simulation_time:.2f} ms")
        print(f"  Memory used: {memory_used:.1f} MB")
        print(f"  Steps per second: {num_steps / (simulation_time / 1000):.2f}")
        print(f"  Memory per step: {memory_used/num_steps:.3f} MB")
    
    # Calculate statistics
    avg_time = statistics.mean(all_times)
    min_time = min(all_times)
    max_time = max(all_times)
    std_time = statistics.stdev(all_times) if len(all_times) > 1 else 0
    avg_memory = statistics.mean(all_memory_usages)
    
    print(f"\n=== Simulation Benchmark Summary ===")
    print(f"Average simulation time: {avg_time:.2f} ms")
    print(f"Min simulation time: {min_time:.2f} ms")
    print(f"Max simulation time: {max_time:.2f} ms")
    print(f"Standard deviation: {std_time:.2f} ms")
    print(f"Average memory usage: {avg_memory:.1f} MB")
    print(f"Performance consistency: {(1 - std_time/avg_time)*100:.1f}%")
    
    return {
        'avg_time_ms': avg_time,
        'min_time_ms': min_time,
        'max_time_ms': max_time,
        'std_time_ms': std_time,
        'avg_memory_mb': avg_memory,
        'throughput_steps_per_sec': num_steps / (avg_time / 1000),
        'consistency_percent': (1 - std_time/avg_time) * 100
    }
```

## Best Practices

### 1. Memory Management Best Practices

#### Memory Allocation and Deallocation

```python
import pynlm
import time

def memory_efficient_vision_processing(width, height, num_iterations):
    """Demonstrate memory-efficient vision processing."""
    print("=== Memory-Efficient Vision Processing ===")
    
    # Strategy 1: Reuse objects instead of creating new ones
    print("Strategy 1: Object reuse")
    vision = pynlm.createVision(width, height, 3)
    processor = pynlm.VisionProcessor()
    
    # Reuse the same vision object
    for i in range(num_iterations):
        # Create data in-place
        data = vision.getData()
        for j in range(len(data)):
            data[j] = (i + j) % 255 / 255.0
        
        vision.setData(data)
        processor.process(vision)
        features = processor.getFeatures()
        
        # Use features here...
    
    # Strategy 2: Batch processing to reduce overhead
    print("Strategy 2: Batch processing")
    visions = []
    
    # Create all visions first
    for i in range(100):
        vision = pynlm.createVision(width, height, 3)
        data = [0.0] * (width * height * 3)
        for j in range(len(data)):
            data[j] = (i + j) % 255 / 255.0
        vision.setData(data)
        visions.append(vision)
    
    # Process all at once
    processor = pynlm.VisionProcessor()
    start_time = time.time()
    pynlm.batchProcessVision(visions, processor)
    end_time = time.time()
    
    print(f"  Batch processing time: {end_time - start_time:.3f} seconds")
    
    # Strategy 3: Explicit cleanup
    print("Strategy 3: Explicit cleanup")
    visions.clear()  # Clear references to allow garbage collection
    
    # Force garbage collection (if needed)
    import gc
    gc.collect()

def memory_monitoring_agent(agent, num_steps):
    """Agent with memory monitoring."""
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    
    def get_memory_mb():
        return process.memory_info().rss / 1024 / 1024
    
    baseline_memory = get_memory_mb()
    max_memory = baseline_memory
    
    print(f"Baseline memory: {baseline_memory:.1f} MB")
    
    for step in range(num_steps):
        # Run simulation step
        world = agent.world
        brain = agent.brain
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Monitor memory
        current_memory = get_memory_mb()
        max_memory = max(max_memory, current_memory)
        
        memory_increase = current_memory - baseline_memory
        
        if step % 100 == 0:
            print(f"Step {step}: Memory: {current_memory:.1f} MB "
                  f"(increase: {memory_increase:.1f} MB, max: {max_memory:.1f} MB)")
        
        # Check for memory issues
        if memory_increase > 100:  # More than 100MB increase
            print(f"Warning: Significant memory increase at step {step}")
            print("Consider reducing batch size or reusing objects")
    
    total_memory_increase = max_memory - baseline_memory
    print(f"\nMemory summary:")
    print(f"  Baseline: {baseline_memory:.1f} MB")
    print(f"  Peak: {max_memory:.1f} MB")
    print(f"  Total increase: {total_memory_increase:.1f} MB")
    print(f"  Average per step: {total_memory_increase/num_steps:.3f} MB")
```

#### Data Structure Best Practices

```python
import pynlm

def efficient_data_storage():
    """Demonstrate efficient data storage patterns."""
    print("=== Efficient Data Storage Patterns ===")
    
    # Pattern 1: Use vectors for homogeneous data
    print("Pattern 1: Vector-based storage")
    
    # Store vision data in a vector
    vision_data = [0.0] * (64 * 64 * 3)
    for i in range(len(vision_data)):
        vision_data[i] = i % 255 / 255.0
    
    # Create vision from vector
    vision = pynlm.createVision(64, 64, 3)
    vision.setData(vision_data)
    
    # Pattern 2: Use lists for heterogeneous data
    print("Pattern 2: List-based storage")
    
    # Store multiple vision samples
    vision_samples = []
    for i in range(10):
        sample_vision = pynlm.createVision(32, 32, 3)
        data = [0.0] * (32 * 32 * 3)
        for j in range(len(data)):
            data[j] = (i + j) % 255 / 255.0
        sample_vision.setData(data)
        vision_samples.append(sample_vision)
    
    # Process batch
    processor = pynlm.VisionProcessor()
    pynlm.batchProcessVision(vision_samples, processor)
    
    # Pattern 3: Use tuples for immutable data
    print("Pattern 3: Tuple-based parameters")
    
    # Store parameters as tuples
    processing_params = [
        (64, 64, "low_resolution"),
        (128, 128, "medium_resolution"),
        (256, 256, "high_resolution")
    ]
    
    for width, height, description in processing_params:
        vision = pynlm.createVision(width, height, 3)
        print(f"  Created {description} vision ({width}x{height})")
    
    # Pattern 4: Use dictionaries for metadata
    print("Pattern 4: Dictionary-based metadata")
    
    sample_metadata = {
        'source': 'camera',
        'timestamp': 1234567890,
        'quality': 'high',
        'format': 'rgb',
        'width': 128,
        'height': 128,
        'channels': 3
    }
    
    # Create vision with metadata
    vision = pynlm.createVision(128, 128, 3)
    # You can attach metadata to the vision object if the API supports it
    
    print("  Metadata stored for vision processing")
```

### 2. Processing Optimization Best Practices

#### Optimization Strategies

```python
import pynlm
import time

def optimize_vision_processing():
    """Demonstrate vision processing optimization strategies."""
    print("=== Vision Processing Optimization ===")
    
    width, height = 128, 128
    iterations = 1000
    
    # Strategy 1: Cache frequently used data
    print("Strategy 1: Data caching")
    
    # Pre-compute data patterns
    base_pattern = [0.0] * (width * height * 3)
    for i in range(len(base_pattern)):
        base_pattern[i] = (i % 255) / 255.0
    
    vision = pynlm.createVision(width, height, 3)
    processor = pynlm.VisionProcessor()
    
    cache_hits = 0
    cache_misses = 0
    
    for i in range(iterations):
        # Check cache (simple cache simulation)
        if i % 10 == 0:  # Cache every 10th iteration
            vision.setData(base_pattern)
            cache_hits += 1
        else:
            # Generate new data with cache miss
            data = base_pattern.copy()
            for j in range(len(data)):
                data[j] = (i + j) % 255 / 255.0
            vision.setData(data)
            cache_misses += 1
        
        processor.process(vision)
        features = processor.getFeatures()
    
    print(f"  Cache hits: {cache_hits}")
    print(f"  Cache misses: {cache_misses}")
    print(f"  Cache efficiency: {cache_hits/(cache_hits + cache_misses)*100:.1f}%")
    
    # Strategy 2: Minimize object creation
    print("Strategy 2: Minimize object creation")
    
    # Create processor once and reuse
    processor = pynlm.VisionProcessor()
    
    # Reuse vision object
    vision = pynlm.createVision(width, height, 3)
    
    start_time = time.time()
    for i in range(iterations):
        # Reuse vision and processor
        data = vision.getData()
        for j in range(len(data)):
            data[j] = (i + j) % 255 / 255.0
        vision.setData(data)
        processor.process(vision)
        features = processor.getFeatures()
    end_time = time.time()
    
    print(f"  Time with object reuse: {end_time - start_time:.3f} seconds")
    
    # Strategy 3: Parallel processing (if supported)
    print("Strategy 3: Parallel processing simulation")
    
    # Simulate parallel processing by processing in chunks
    chunk_size = 100
    num_chunks = iterations // chunk_size
    
    start_time = time.time()
    for chunk in range(num_chunks):
        # Process chunk
        chunk_start = chunk * chunk_size
        for i in range(chunk_start, chunk_start + chunk_size):
            vision = pynlm.createVision(width, height, 3)
            data = [0.0] * (width * height * 3)
            for j in range(len(data)):
                data[j] = (i + j) % 255 / 255.0
            vision.setData(data)
            processor.process(vision)
            features = processor.getFeatures()
    end_time = time.time()
    
    print(f"  Time with chunked processing: {end_time - start_time:.3f} seconds")
    
    # Strategy 4: Early termination and conditional processing
    print("Strategy 4: Conditional processing")
    
    threshold = 0.5
    processed_count = 0
    skipped_count = 0
    
    for i in range(iterations):
        vision = pynlm.createVision(width, height, 3)
        data = [0.0] * (width * height * 3)
        for j in range(len(data)):
            data[j] = (i + j) % 255 / 255.0
        
        # Check if processing is needed
        brightness = sum(data) / len(data)
        if brightness > threshold:
            vision.setData(data)
            processor.process(vision)
            features = processor.getFeatures()
            processed_count += 1
        else:
            # Skip processing for low brightness
            skipped_count += 1
    
    print(f"  Processed: {processed_count}")
    print(f"  Skipped: {skipped_count}")
    print(f"  Skipping efficiency: {skipped_count/iterations*100:.1f}%")
```

#### Error Handling Best Practices

```python
import pynlm

def robust_vision_processing():
    """Demonstrate robust vision processing with error handling."""
    print("=== Robust Vision Processing ===")
    
    def safe_vision_creation(width, height, data=None):
        """Safely create a vision input."""
        try:
            if data is not None:
                # Validate data
                if not pynlm.validateVisionData(data, width, height):
                    raise ValueError(f"Invalid vision data for {width}x{height}")
                
                vision = pynlm.createVision(width, height, 3, data)
                print(f"  Created vision with validation: {width}x{height}")
                return vision
            else:
                vision = pynlm.createVision(width, height, 3)
                print(f"  Created vision without data: {width}x{height}")
                return vision
        except Exception as e:
            print(f"  Error creating vision: {e}")
            return None
    
    def safe_processing(vision, processor):
        """Safely process vision data."""
        try:
            if vision is None:
                raise ValueError("Vision is None")
            
            if processor is None:
                raise ValueError("Processor is None")
            
            processor.process(vision)
            features = processor.getFeatures()
            
            if features is None:
                raise ValueError("Processing returned no features")
            
            print(f"  Successfully processed vision, got {len(features)} features")
            return features
        except Exception as e:
            print(f"  Error processing vision: {e}")
            return None
    
    # Test with valid data
    print("Test 1: Valid data")
    valid_data = [0.0] * (64 * 64 * 3)
    for i in range(len(valid_data)):
        valid_data[i] = i % 255 / 255.0
    
    vision = safe_vision_creation(64, 64, valid_data)
    processor = pynlm.VisionProcessor()
    features = safe_processing(vision, processor)
    
    # Test with invalid data
    print("\nTest 2: Invalid data")
    invalid_data = [0.0] * 100  # Too small for 64x64x3
    vision = safe_vision_creation(64, 64, invalid_data)
    
    # Test with None
    print("\nTest 3: None input")
    vision = safe_vision_creation(64, 64, None)
    if vision:
        features = safe_processing(vision, processor)

def fault_tolerance_agent(agent, num_steps):
    """Agent with fault tolerance mechanisms."""
    print("=== Fault-Tolerant Agent ===")
    
    def safe_step(agent, step):
        """Perform a single agent step with error handling."""
        try:
            world = agent.world
            brain = agent.brain
            
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            if percept is None:
                raise ValueError("Percept is None")
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            if action is None:
                raise ValueError("Action is None")
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            return True, None
            
        except Exception as e:
            print(f"  Error at step {step}: {e}")
            
            # Try to recover
            try:
                print(f"  Attempting recovery...")
                # Reset to safe state
                brain.reset()
                agent.reset()
                world.reset()
                return False, "Recovered"
            except Exception as recovery_error:
                print(f"  Recovery failed: {recovery_error}")
                return False, "Failed to recover"
    
    # Run simulation with fault tolerance
    for step in range(num_steps):
        success, recovery_msg = safe_step(agent, step)
        
        if not success:
            print(f"  Step {step} had issues: {recovery_msg}")
            # Continue to next step
    
    print("  Simulation completed with fault tolerance")
```

### 3. Performance Monitoring Best Practices

#### Real-time Monitoring

```python
import pynlm
import time
import statistics
from memory_profiler import memory_usage
import psutil
import os

class PerformanceMonitor:
    """Real-time performance monitoring for NLM applications."""
    
    def __init__(self, name="NLM Application"):
        self.name = name
        self.start_time = time.time()
        self.step_count = 0
        self.times = []
        self.memory_usages = []
        self.process = psutil.Process(os.getpid())
        
        print(f"Initializing performance monitor: {name}")
    
    def record_step(self, step_time=None, memory_usage=None):
        """Record performance metrics for a step."""
        current_time = time.time()
        self.step_count += 1
        
        if step_time is None:
            step_time = current_time - self.start_time
        
        if memory_usage is None:
            memory_usage = self.get_memory_mb()
        
        self.times.append(step_time)
        self.memory_usages.append(memory_usage)
        
        if self.step_count % 100 == 0:
            self.print_summary()
    
    def get_memory_mb(self):
        """Get current memory usage in MB."""
        return self.process.memory_info().rss / 1024 / 1024
    
    def print_summary(self):
        """Print current performance summary."""
        if not self.times:
            return
        
        current_memory = self.get_memory_mb()
        avg_time = statistics.mean(self.times[-100:]) if len(self.times) >= 100 else statistics.mean(self.times)
        min_time = min(self.times[-100:]) if len(self.times) >= 100 else min(self.times)
        max_time = max(self.times[-100:]) if len(self.times) >= 100 else max(self.times)
        
        print(f"  Step {self.step_count}: Time: {avg_time*1000:.1f}ms "
              f"| Memory: {current_memory:.1f}MB "
              f"| Min: {min_time*1000:.1f}ms "
              f"| Max: {max_time*1000:.1f}ms "
              f"| Throughput: {1/avg_time:.2f} steps/sec")
    
    def get_performance_report(self):
        """Get comprehensive performance report."""
        if not self.times:
            return {"error": "No performance data recorded"}
        
        report = {
            "application": self.name,
            "total_steps": self.step_count,
            "total_time_s": self.times[-1],
            "avg_time_ms": statistics.mean(self.times) * 1000,
            "min_time_ms": min(self.times) * 1000,
            "max_time_ms": max(self.times) * 1000,
            "std_time_ms": statistics.stdev(self.times) * 1000 if len(self.times) > 1 else 0,
            "throughput_steps_per_sec": len(self.times) / self.times[-1] if self.times[-1] > 0 else 0,
            "memory_usages_mb": self.memory_usages,
            "avg_memory_mb": statistics.mean(self.memory_usages) if self.memory_usages else 0,
            "max_memory_mb": max(self.memory_usages) if self.memory_usages else 0,
            "memory_increase_mb": (max(self.memory_usages) if self.memory_usages else 0) - 
                                 (self.memory_usages[0] if self.memory_usages else 0)
        }
        
        return report
    
    def print_final_report(self):
        """Print final performance report."""
        report = self.get_performance_report()
        
        print(f"\n=== Performance Report: {self.name} ===")
        print(f"Total steps: {report['total_steps']}")
        print(f"Total time: {report['total_time_s']:.2f} seconds")
        print(f"Average step time: {report['avg_time_ms']:.2f} ms")
        print(f"Min step time: {report['min_time_ms']:.2f} ms")
        print(f"Max step time: {report['max_time_ms']:.2f} ms")
        print(f"Performance consistency: {(1 - report['std_time_ms']/report['avg_time_ms'])*100:.1f}%")
        print(f"Average memory: {report['avg_memory_mb']:.1f} MB")
        print(f"Peak memory: {report['max_memory_mb']:.1f} MB")
        print(f"Memory increase: {report['memory_increase_mb']:.1f} MB")
        print(f"Throughput: {report['throughput_steps_per_sec']:.2f} steps/second")

def monitor_vision_processing_performance(width, height, num_steps):
    """Monitor vision processing performance in detail."""
    print(f"=== Vision Processing Performance Monitor: {width}x{height} ===")
    
    monitor = PerformanceMonitor(f"Vision Processing {width}x{height}")
    
    vision = pynlm.createVision(width, height, 3)
    processor = pynlm.VisionProcessor()
    
    for step in range(num_steps):
        # Generate test data
        data = [0.0] * (width * height * 3)
        for i in range(len(data)):
            data[i] = (step + i) % 255 / 255.0
        
        # Measure performance
        step_start = time.perf_counter()
        
        vision.setData(data)
        processor.process(vision)
        features = processor.getFeatures()
        
        step_end = time.perf_counter()
        step_time = step_end - step_start
        
        memory_usage = monitor.get_memory_mb()
        
        monitor.record_step(step_time, memory_usage)
        
        if step % 500 == 0:
            print(f"  Progress: {step}/{num_steps} steps")
    
    monitor.print_final_report()
    return monitor.get_performance_report()
```

### 4. Profiling Best Practices

#### Basic Profiling

```python
import pynlm
import cProfile
import pstats
import io
from memory_profiler import profile

def profile_vision_processing(width, height, num_iterations=100):
    """Profile vision processing performance."""
    print(f"=== Profiling Vision Processing: {width}x{height} ===")
    
    vision = pynlm.createVision(width, height, 3)
    processor = pynlm.VisionProcessor()
    
    # Generate test data
    data = [0.0] * (width * height * 3)
    for i in range(len(data)):
        data[i] = i % 255 / 255.0
    
    @profile
    def processing_function():
        for i in range(num_iterations):
            vision.setData(data)
            processor.process(vision)
            features = processor.getFeatures()
    
    # Run profiler
    profiler = cProfile.Profile()
    profiler.enable()
    
    processing_function()
    
    profiler.disable()
    
    # Print profiling results
    s = io.StringIO()
    ps = pstats.Stats(profiler, stream=s).sort_stats('cumulative')
    ps.print_stats()
    
    print(s.getvalue())
    
    return processing_function

@profile
def profile_agent_simulation(num_steps=1000):
    """Profile agent simulation."""
    print(f"=== Profiling Agent Simulation: {num_steps} steps ===")
    
    # Configure agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    print("  Simulation completed")
```

### 5. Benchmarking Best Practices

#### Standardized Benchmarking

```python
import pynlm
import time
import statistics
from datetime import datetime
import json

class BenchmarkSuite:
    """Standardized benchmarking suite for NLM applications."""
    
    def __init__(self, name):
        self.name = name
        self.results = {}
        self.timestamp = datetime.now().isoformat()
    
    def run_benchmark(self, benchmark_name, benchmark_func, **kwargs):
        """Run a single benchmark and record results."""
        print(f"\nRunning benchmark: {benchmark_name}")
        
        try:
            start_time = time.time()
            
            # Run benchmark
            result = benchmark_func(**kwargs)
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Record results
            self.results[benchmark_name] = {
                'timestamp': self.timestamp,
                'duration_s': duration,
                'result': result,
                'kwargs': kwargs
            }
            
            print(f"  Completed in {duration:.2f} seconds")
            return result
            
        except Exception as e:
            print(f"  Benchmark failed: {e}")
            self.results[benchmark_name] = {
                'timestamp': self.timestamp,
                'error': str(e),
                'kwargs': kwargs
            }
            return None
    
    def save_results(self, filepath):
        """Save benchmark results to file."""
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2, default=str)
        
        print(f"Results saved to: {filepath}")
    
    def print_summary(self):
        """Print benchmark summary."""
        print(f"\n=== Benchmark Summary: {self.name} ===")
        print(f"Total benchmarks: {len(self.results)}")
        print(f"Completed: {sum(1 for r in self.results.values() if 'error' not in r)}")
        print(f"Failed: {sum(1 for r in self.results.values() if 'error' in r)}")
        
        for benchmark_name, result in self.results.items():
            if 'error' not in result:
                duration = result['duration_s']
                print(f"  {benchmark_name}: {duration:.2f}s")
            else:
                print(f"  {benchmark_name}: FAILED - {result['error']}")

def create_standard_vision_benchmark(width, height, num_iterations=100):
    """Create standardized vision processing benchmark."""
    print(f"Vision Processing Benchmark: {width}x{height}, {num_iterations} iterations")
    
    def benchmark():
        vision = pynlm.createVision(width, height, 3)
        processor = pynlm.VisionProcessor()
        
        # Generate test data
        data = [0.0] * (width * height * 3)
        for i in range(len(data)):
            data[i] = i % 255 / 255.0
        
        # Processing
        times = []
        for i in range(num_iterations):
            start_time = time.perf_counter()
            vision.setData(data)
            processor.process(vision)
            features = processor.getFeatures()
            end_time = time.perf_counter()
            
            times.append((end_time - start_time) * 1000)  # Convert to ms
        
        return {
            'width': width,
            'height': height,
            'iterations': num_iterations,
            'avg_time_ms': statistics.mean(times),
            'min_time_ms': min(times),
            'max_time_ms': max(times),
            'std_time_ms': statistics.stdev(times) if len(times) > 1 else 0,
            'throughput_fps': num_iterations / (statistics.mean(times) / 1000)
        }
    
    return benchmark

def create_standard_agent_benchmark(num_steps=1000, num_runs=5):
    """Create standardized agent simulation benchmark."""
    print(f"Agent Simulation Benchmark: {num_steps} steps, {num_runs} runs")
    
    def benchmark():
        all_times = []
        
        for run in range(num_runs):
            # Configure agent
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            agent.initialize(world)
            
            # Enable all features
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            start_time = time.time()
            
            for step in range(num_steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            end_time = time.time()
            all_times.append((end_time - start_time) * 1000)  # Convert to ms
        
        return {
            'steps': num_steps,
            'runs': num_runs,
            'avg_time_ms': statistics.mean(all_times),
            'min_time_ms': min(all_times),
            'max_time_ms': max(all_times),
            'std_time_ms': statistics.stdev(all_times) if len(all_times) > 1 else 0,
            'throughput_steps_per_sec': num_steps * num_runs / (statistics.mean(all_times) / 1000)
        }
    
    return benchmark

# Example usage
if __name__ == "__main__":
    # Create benchmark suite
    suite = BenchmarkSuite("NLM Python Bindings")
    
    # Add benchmarks
    suite.run_benchmark("Vision Processing 64x64", 
                       create_standard_vision_benchmark(64, 64, 100))
    
    suite.run_benchmark("Vision Processing 128x128", 
                       create_standard_vision_benchmark(128, 128, 50))
    
    suite.run_benchmark("Agent Simulation", 
                       create_standard_agent_benchmark(1000, 3))
    
    # Save and print results
    suite.save_results("benchmark_results.json")
    suite.print_summary()
```

## Advanced Optimization Techniques

### 1. SIMD Optimization (if available)

```python
import pynlm
import numpy as np

def vectorized_vision_processing(visions, processor):
    """Optimized vision processing using vectorized operations."""
    print("=== Vectorized Vision Processing ===")
    
    # If numpy is available, use vectorized operations
    if 'numpy' in globals():
        print("Using numpy for vectorized operations")
        
        # Stack all vision data into a single numpy array
        all_data = np.stack([vision.getData() for vision in visions])
        
        # Vectorized processing
        batch_size, height, width, channels = all_data.shape
        
        # Reshape for processing
        flat_data = all_data.reshape(batch_size, -1)
        
        # Apply vectorized transformations
        # This is a simplified example - real implementation would depend on processor
        processed_features = np.mean(flat_data, axis=1)
        
        # Reshape back
        features = processed_features.reshape(batch_size, -1)
        
        # Process each vision individually for compatibility
        # (In practice, you'd modify the processor to handle batch input)
        for i, vision in enumerate(visions):
            processor.process(vision)
            
        print(f"  Processed {len(visions)} visions using vectorized operations")
    else:
        print("NumPy not available, falling back to standard processing")
        pynlm.batchProcessVision(visions, processor)
```

### 2. Multithreading (if supported)

```python
import pynlm
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

def parallel_vision_processing(visions, processor, max_workers=None):
    """Parallel vision processing using multithreading."""
    print("=== Parallel Vision Processing ===")
    
    if max_workers is None:
        max_workers = min(len(visions), 4)  # Limit to 4 workers
    
    print(f"Using {max_workers} workers for {len(visions)} visions")
    
    def process_single_vision(vision):
        """Process a single vision."""
        processor.process(vision)
        return processor.getFeatures()
    
    # Use ThreadPoolExecutor for parallel processing
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        # Submit all tasks
        futures = [executor.submit(process_single_vision, vision) 
                  for vision in visions]
        
        # Collect results as they complete
        results = []
        start_time = time.time()
        
        for future in as_completed(futures):
            try:
                features = future.result()
                results.append(features)
            except Exception as e:
                print(f"  Error in processing: {e}")
        
        end_time = time.time()
    
    total_time = end_time - start_time
    throughput = len(visions) / total_time
    
    print(f"  Total time: {total_time:.3f} seconds")
    print(f"  Throughput: {throughput:.2f} visions/second")
    print(f"  Average per vision: {total_time/len(visions)*1000:.2f} ms")
    
    return results

def parallel_batch_processing(data_bundles, processing_function, max_workers=None):
    """Parallel processing of data bundles."""
    print("=== Parallel Batch Processing ===")
    
    if max_workers is None:
        max_workers = min(len(data_bundles), 4)
    
    print(f"Processing {len(data_bundles)} data bundles with {max_workers} workers")
    
    def process_bundle(bundle):
        """Process a single bundle."""
        return processing_function(bundle)
    
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        # Submit all tasks
        futures = [executor.submit(process_bundle, bundle) 
                  for bundle in data_bundles]
        
        # Collect results
        results = []
        start_time = time.time()
        
        for future in as_completed(futures):
            try:
                result = future.result()
                results.append(result)
            except Exception as e:
                print(f"  Error processing bundle: {e}")
        
        end_time = time.time()
    
    total_time = end_time - start_time
    throughput = len(data_bundles) / total_time
    
    print(f"  Total time: {total_time:.3f} seconds")
    print(f"  Throughput: {throughput:.2f} bundles/second")
    
    return results
```

### 3. GPU Acceleration (if available)

```python
import pynlm
import torch
import numpy as np

def gpu_vision_processing(visions, processor):
    """Vision processing using GPU acceleration if available."""
    print("=== GPU Vision Processing ===")
    
    if not torch.cuda.is_available():
        print("CUDA not available, falling back to CPU processing")
        pynlm.batchProcessVision(visions, processor)
        return []
    
    print("Using GPU for vision processing")
    
    # Convert vision data to PyTorch tensors
    vision_tensors = []
    for vision in visions:
        data = vision.getData()
        # Convert to tensor and move to GPU
        tensor = torch.tensor(data, dtype=torch.float32).cuda()
        vision_tensors.append(tensor)
    
    # Stack tensors into a batch
    batch_tensor = torch.stack(vision_tensors)  # Shape: (batch_size, height, width, channels)
    
    # Process on GPU (simplified example - real implementation would depend on processor)
    start_time = time.time()
    
    # Example processing: Apply a simple GPU operation
    with torch.no_grad():
        # Reshape for processing
        batch_size, height, width, channels = batch_tensor.shape
        flat_tensor = batch_tensor.reshape(batch_size, -1)
        
        # Apply processing (example: simple normalization)
        processed = (flat_tensor - flat_tensor.mean(dim=1, keepdim=True)) / (flat_tensor.std(dim=1, keepdim=True) + 1e-8)
        
        # Reshape back
        features = processed.reshape(batch_size, height, width, channels)
    
    end_time = time.time()
    
    # Process each vision individually (for API compatibility)
    # In practice, you'd modify the processor to handle GPU tensors
    results = []
    for i, vision in enumerate(visions):
        # Copy features back to CPU
        cpu_features = features[i].cpu().numpy()
        vision.setData(cpu_features.tolist())
        processor.process(vision)
        results.append(processor.getFeatures())
    
    total_time = end_time - start_time
    throughput = len(visions) / total_time
    
    print(f"  GPU processing time: {total_time:.3f} seconds")
    print(f"  Throughput: {throughput:.2f} visions/second")
    print(f"  GPU memory used: {torch.cuda.memory_allocated() / 1024**2:.1f} MB")
    
    return results
```

## Summary and Further Reading

### Key Takeaways

1. **Memory Management**: Reuse objects, use batch processing, and monitor memory usage
2. **Error Handling**: Implement robust error handling and fault tolerance
3. **Performance Monitoring**: Use monitoring tools to track and optimize performance
4. **Profiling**: Profile your applications to identify bottlenecks
5. **Optimization**: Use parallel processing and GPU acceleration when available

### Further Reading

1. **NLM Documentation**: Read the main NLM documentation for system architecture
2. **Python Performance**: Learn more about Python performance optimization
3. **Parallel Computing**: Study parallel computing patterns and best practices
4. **GPU Programming**: Explore GPU acceleration techniques
5. **Memory Management**: Learn more about memory management in Python and C++

This guide provides comprehensive performance benchmarks, best practices, and optimization techniques for the NLM Python bindings. Use these guidelines to build high-performance, memory-efficient, and robust NLM applications.