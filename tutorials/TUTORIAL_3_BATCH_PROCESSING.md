# Tutorial 3: Batch Processing

## Overview

This tutorial introduces you to NLM (Neural Learning Machine) batch processing capabilities. It covers:

- Efficient batch processing for large-scale simulations
- Memory optimization through memory pooling
- Performance profiling and analysis
- Thread-safe parallel processing
- Progress tracking and monitoring
- Checkpointing and recovery

This tutorial is designed for users who want to achieve maximum performance from their NLM simulations.

## Prerequisites

Before starting this tutorial, you should:

1. **Be familiar with Tutorial 1 and 2**: Basic brain creation and advanced configuration
2. **Understand performance optimization concepts**: Memory usage, CPU performance, I/O optimization
3. **Have NLM installed**: As per the main installation instructions
4. **Consider system resources**: Ensure adequate memory and CPU for large simulations

## What You'll Learn

By the end of this tutorial, you will:

- Understand NLM batch processing architecture and components
- Create batch-processed brains with performance optimization
- Monitor and analyze batch processing performance
- Implement checkpointing for long-running simulations
- Use progress tracking and monitoring
- Optimize configuration for batch processing

## Setting Up Your Environment

The same as Tutorial 1. Ensure NLM is installed:

```bash
pip install pynlm
```

## NLM Batch Processing System

### Overview

NLM's batch processing system provides comprehensive batch processing capabilities for neural simulations, enabling you to:

1. **Process Multiple Steps Efficiently**: Process thousands or millions of simulation steps with optimized algorithms
2. **Optimize Memory Usage**: Use memory pooling and efficient data structures to reduce memory overhead
3. **Leverage Parallel Processing**: Use multithreading for CPU-bound tasks
4. **Track Progress**: Monitor progress with callbacks and progress bars
5. **Implement Checkpointing**: Save and restore simulation state for long-running simulations
6. **Analyze Performance**: Profile and analyze batch processing performance

### Batch Processing Architecture

```
NLM Batch Processing Architecture

┌─────────────────────────────────────────────────────────┐
│                    Batch Processing                   │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐            │
│  │ BatchStepProcessor │  │  BatchBrain      │            │
│  │  (Core Engine)    │  │  (High-Level)    │            │
│  └─────────────────┘  └─────────────────┘            │
│         │                      │                   │
│   ┌─────▼─────┐        ┌─────────▼─────────┐           │
│   │ MemoryPool │        │  ProgressTracking │           │
│   │ (Pooling)  │        │  (Monitoring)     │           │
│   └───────────┘        └───────────────────┘           │
│                      │                             │
│   ┌─────────────────┐  ┌─────────▼────────┐          │
│   │  SIMDVectorization  │  │  Checkpointing  │          │
│   │ (Vectorized)   │  │ (Save/Restore)  │          │
│   └─────────────────┘  └─────────────────┘          │
│                      │                             │
│   ┌─────────────────┐  ┌─────────────────┐            │
│   │ ThreadPool      │  │  PerformanceProfiler  │       │
│   │ (Multithreading)│  │  (Analysis)     │           │
│   └─────────────────┘  └─────────────────┘          │
└─────────────────────────────────────────────────────────┘
```

## Batch Processing Components

### 1. BatchStepProcessor

Core batch processing engine with memory optimization:

```cpp
class BatchStepProcessor {
public:
    BatchStepProcessor(const BatchConfig& config);
    ~BatchStepProcessor();
    
    bool processBatch(int startStep, int endStep);
    bool processBatchWithTime(double timeLimit);
    bool processBatchWithEarlyTermination(float threshold);
    bool processBatchWithCheckpointing(const std::string& checkpointDir, int checkpointInterval);
    
    BatchMetrics getMetrics() const;
    void setProgressCallback(std::function<void(float)> callback);
    void setCompletionCallback(std::function<void(bool, const BatchMetrics&)> callback);
    
private:
    BatchConfig config_;
    MemoryPool memoryPool_;
    SIMDVectorization simd_;
    ThreadPool threadPool_;
    ProgressTracker progressTracker_;
    CheckpointManager checkpointManager_;
};
```

**Key Features**:
- **Memory Pooling**: Pre-allocated object pools for efficient allocation/deallocation
- **SIMD Vectorization**: Vectorized operations for parallel processing
- **Thread Safety**: Thread-safe operations with proper synchronization
- **Progress Tracking**: Real-time progress monitoring and callbacks
- **Checkpoint Support**: Save/restore state for long-running batches

### 2. BatchBrain

High-level interface for batch processing:

```cpp
class BatchBrain {
public:
    BatchBrain(const BatchConfig& config);
    ~BatchBrain();
    
    bool processBatch(int startStep, int endStep);
    bool processBatchWithCheckpointing(const std::string& checkpointDir, int checkpointInterval);
    bool processBatchAsync(int startStep, int endStep);
    
    BatchMetrics getMetrics() const;
    void enableSIMD(bool enable);
    void enableMultithreading(bool enable);
    void enableMemoryPool(bool enable);
    
private:
    BatchConfig config_;
    std::unique_ptr<BatchStepProcessor> processor_;
    ProgressCallback progressCallback_;
    CompletionCallback completionCallback_;
};
```

**Key Features**:
- **Simplified API**: Easy-to-use batch processing interface
- **Performance Monitoring**: Statistics and metrics collection
- **Thread Safety**: Thread-safe batch operations
- **Graceful Degradation**: Optional optimization for smaller batches

### 3. PerformanceProfiler

Performance profiling and analysis:

```cpp
class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& name);
    ~PerformanceProfiler();
    
    void setup();
    void teardown();
    
    void recordMetric(const std::string& name, double value);
    PerformanceReport generateReport();
    
    void analyzeReport(const PerformanceReport& report);
    
private:
    std::string name_;
    std::map<std::string, std::vector<double>> metrics_;
    double startTime_;
    double endTime_;
};
```

**Key Features**:
- **Multi-Stage Profiling**: Setup, execution, and teardown phases
- **Component-Level Profiling**: Profile individual components separately
- **Metric Collection**: Collect and store performance metrics
- **Report Generation**: Generate performance reports

### 4. MemoryProfiler

Memory profiling and analysis:

```cpp
class MemoryProfiler {
public:
    MemoryProfiler(const std::string& name);
    ~MemoryProfiler();
    
    void setup();
    void teardown();
    
    void recordMemoryUsage();
    MemoryReport generateReport();
    
    void analyzeReport(const MemoryReport& report);
    
private:
    std::string name_;
    std::vector<MemorySample> memorySamples_;
    double startTime_;
    double endTime_;
};
```

**Key Features**:
- **Allocation Tracking**: Track all memory allocations and deallocations
- **Leak Detection**: Detect memory leaks
- **Memory Usage Analysis**: Analyze memory usage patterns
- **Fragmentation Analysis**: Analyze memory fragmentation

## Batch Processing Configuration

### Batch Configuration

```cpp
struct BatchConfig {
    size_t batchSize = 1000;           // Default batch size
    size_t maxBatchSize = 10000;       // Maximum allowed batch size
    bool enableSIMD = true;            // Enable SIMD vectorization
    bool enableMultithreading = true;  // Enable parallel processing
    bool enableMemoryPool = true;      // Enable memory pooling
    float earlyTerminationThreshold = 0.95f;  // Early termination threshold
    std::function<bool()> shouldTerminate;    // Termination callback
    std::function<void(float)> progressCallback;  // Progress reporting
    std::function<void(bool, const BatchMetrics&)> completionCallback;  // Completion reporting
};
```

### Performance Metrics

```cpp
struct BatchMetrics {
    SimulationStep stepsProcessed;
    SimulationStep totalSteps;
    double startTime;
    double endTime;
    double cpuTime;
    double wallTime;
    size_t neuronsProcessed;
    size_t spikesGenerated;
    size_t synapticEvents;
    double throughput;
    double memoryUsage;
    float progress;
    double estimatedTimeRemaining;
    bool completedSuccessfully;
    bool terminatedEarly;
    size_t checkpointCount;
};
```

## Creating a Batch-Processed Brain

### Basic Batch Processing

```python
import pynlm
from performance import BatchBrain

print("=== NLM Tutorial 3: Batch Processing ===")

# 1. Create batch brain configuration
print("\n1. Creating batch brain configuration...")
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

batch_config = {
    'batch_size': 1000,
    'enable_SIMD': True,
    'enable_multithreading': True,
    'enable_memory_pool': True,
    'early_termination_threshold': 0.95
}

batch_brain = BatchBrain(config, batch_config)
print("   Batch brain created successfully!")
print(f"   Batch size: {batch_config['batch_size']}")
print(f"   SIMD enabled: {batch_config['enable_SIMD']}")
print(f"   Multithreading enabled: {batch_config['enable_multithreading']}")
print(f"   Memory pool enabled: {batch_config['enable_memory_pool']}")

# 2. Set up progress monitoring
print("\n2. Setting up progress monitoring...")

def progress_callback(progress):
    if progress * 100 > 25 and progress * 100 <= 100:
        print(f"   Progress: {progress * 100:.1f}%")

batch_brain.setProgressCallback(progress_callback)

def completion_callback(success, metrics):
    if success:
        print(f"   Batch completed successfully!")
        print(f"   Processed {metrics['steps_processed']} steps in {metrics['wall_time']:.2f}s")
        print(f"   Throughput: {metrics['throughput']:.0f} steps/s")
    else:
        print(f"   Batch failed!")

batch_brain.setCompletionCallback(completion_callback)

# 3. Run batch processing
print("\n3. Running batch processing...")
print("   Processing 1000 simulation steps...")

result = batch_brain.processBatch(0, 999)

print("   ✓ Batch processing completed!")

# 4. Get performance metrics
print("\n4. Getting performance metrics...")
metrics = batch_brain.getMetrics()

print(f"   Steps processed: {metrics['steps_processed']}")
print(f"   Processing time: {metrics['wall_time']:.2f}s")
print(f"   Throughput: {metrics['throughput']:.0f} steps/s")
print(f"   Memory usage: {metrics['memory_usage']:.2f} MB")
print(f"   Progress: {metrics['progress'] * 100:.1f}%")

# 5. Save batch configuration
print("\n5. Saving batch configuration...")
batch_brain.save("batch_config.json")
print("   Batch configuration saved to 'batch_config.json'")

# 6. Load batch configuration
print("\n6. Loading batch configuration...")
loaded_batch_brain = BatchBrain.load("batch_config.json")
print("   Batch configuration loaded successfully!")

print("\n=== Tutorial 3 Complete! ===")
```

### C++ Example

```cpp
#include <iostream>
#include "performance/BatchBrain.hpp"

int main() {
    std::cout << "=== NLM Tutorial 3: Batch Processing ===" << std::endl;
    
    // 1. Create batch brain configuration
    std::cout << "\n1. Creating batch brain configuration..." << std::endl;
    nlm::Config config;
    config.setInt("neuron_count", 5000);
    
    BatchBrain::BatchConfig batchConfig;
    batchConfig.batchSize = 1000;
    batchConfig.enableSIMD = true;
    batchConfig.enableMultithreading = true;
    batchConfig.enableMemoryPool = true;
    batchConfig.earlyTerminationThreshold = 0.95;
    
    auto batchBrain = BatchBrain(batchConfig);
    std::cout << "   Batch brain created successfully!" << std::endl;
    std::cout << "   Batch size: " << batchConfig.batchSize << std::endl;
    std::cout << "   SIMD enabled: " << (batchConfig.enableSIMD ? "true" : "false") << std::endl;
    std::cout << "   Multithreading enabled: " << (batchConfig.enableMultithreading ? "true" : "false") << std::endl;
    std::cout << "   Memory pool enabled: " << (batchConfig.enableMemoryPool ? "true" : "false") << std::endl;
    
    // 2. Set up progress monitoring
    std::cout << "\n2. Setting up progress monitoring..." << std::endl;
    
    auto progressCallback = [](float progress) {
        if (progress * 100 > 25 && progress * 100 <= 100) {
            std::cout << "   Progress: " << std::fixed << std::setprecision(1) 
                      << (progress * 100) << "%" << std::endl;
        }
    };
    
    auto completionCallback = [](bool success, const BatchBrain::BatchMetrics& metrics) {
        if (success) {
            std::cout << "   Batch completed successfully!" << std::endl;
            std::cout << "   Processed " << metrics.stepsProcessed 
                      << " steps in " << metrics.wallTime << "s" << std::endl;
            std::cout << "   Throughput: " << metrics.throughput << " steps/s" << std::endl;
        } else {
            std::cout << "   Batch failed!" << std::endl;
        }
    };
    
    batchBrain.setProgressCallback(progressCallback);
    batchBrain.setCompletionCallback(completionCallback);
    
    // 3. Run batch processing
    std::cout << "\n3. Running batch processing..." << std::endl;
    std::cout << "   Processing 1000 simulation steps..." << std::endl;
    
    bool success = batchBrain.processBatch(0, 999);
    
    std::cout << "   ✓ Batch processing completed!" << std::endl;
    
    // 4. Get performance metrics
    std::cout << "\n4. Getting performance metrics..." << std::endl;
    auto metrics = batchBrain.getMetrics();
    
    std::cout << "   Steps processed: " << metrics.stepsProcessed << std::endl;
    std::cout << "   Processing time: " << metrics.wallTime << "s" << std::endl;
    std::cout << "   Throughput: " << metrics.throughput << " steps/s" << std::endl;
    std::cout << "   Memory usage: " << metrics.memoryUsage << " MB" << std::endl;
    std::cout << "   Progress: " << metrics.progress * 100 << "%" << std::endl;
    
    // 5. Save batch configuration
    std::cout << "\n5. Saving batch configuration..." << std::endl;
    batchBrain.save("batch_config.json");
    std::cout << "   Batch configuration saved to 'batch_config.json'" << std::endl;
    
    // 6. Load batch configuration
    std::cout << "\n6. Loading batch configuration..." << std::endl;
    auto loadedBatchBrain = BatchBrain::load("batch_config.json");
    std::cout << "   Batch configuration loaded successfully!" << std::endl;
    
    std::cout << "\n=== Tutorial 3 Complete! ===" << std::endl;
    
    return 0;
}
```

## Advanced Batch Processing

### Batch Processing with Checkpoints

```python
import pynlm
from performance import BatchBrain

print("=== NLM Tutorial 3: Advanced Batch Processing ===")

# 1. Create batch brain with checkpointing
print("\n1. Creating batch brain with checkpointing...")
config = pynlm.createDefaultConfig()
config.set("neuron_count", 10000)

batch_config = {
    'batch_size': 1000,
    'enable_SIMD': True,
    'enable_multithreading': True,
    'enable_memory_pool': True,
    'checkpoint_dir': '/tmp/checkpoints',
    'checkpoint_interval': 100
}

batch_brain = BatchBrain(config, batch_config)
print("   Batch brain with checkpointing created successfully!")

# 2. Run batch processing with checkpoints
print("\n2. Running batch processing with checkpoints...")
print("   Processing 10000 simulation steps with checkpointing...")

# Create checkpoint directory
import os
os.makedirs('/tmp/checkpoints', exist_ok=True)

result = batch_brain.processBatchWithCheckpointing(0, 9999, '/tmp/checkpoints', 100)

print("   ✓ Batch processing with checkpointing completed!")

# 3. List checkpoints
print("\n3. Checking checkpoints...")
checkpoints = batch_brain.listCheckpoints()
print(f"   Checkpoints created: {len(checkpoints)}")
for checkpoint in checkpoints:
    print(f"     - {checkpoint}")

# 4. Restore from checkpoint
print("\n4. Restoring from checkpoint...")
restored_batch_brain = batch_brain.restoreFromCheckpoint(checkpoints[0])
print("   Batch processing restored from checkpoint successfully!")

print("\n=== Advanced Batch Processing Complete! ===")
```

### Batch Processing with Early Termination

```python
import pynlm
from performance import BatchBrain

print("=== NLM Tutorial 3: Batch Processing with Early Termination ===")

# 1. Create batch brain with early termination
print("\n1. Creating batch brain with early termination...")
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

batch_config = {
    'batch_size': 1000,
    'enable_SIMD': True,
    'enable_multithreading': True,
    'enable_memory_pool': True,
    'early_termination_threshold': 0.8  # Early terminate at 80%
}

batch_brain = BatchBrain(config, batch_config)
print("   Batch brain with early termination created successfully!")

# 2. Set up early termination condition
print("\n2. Setting up early termination condition...")

def should_terminate():
    # Custom termination condition: stop if processing time exceeds 10 seconds
    import time
    return time.time() - start_time > 10.0

start_time = time.time()
batch_brain.setShouldTerminate(should_terminate)

# 3. Run batch processing with early termination
print("\n3. Running batch processing with early termination...")
print("   Processing 10000 simulation steps with early termination...")

result = batch_brain.processBatchWithEarlyTermination(0, 9999, should_terminate)

print("   ✓ Batch processing with early termination completed!")

# 4. Check termination status
print("\n4. Checking termination status...")
print(f"   Terminated early: {result['terminated_early']}")
print(f"   Steps processed: {result['steps_processed']}")
print(f"   Processing time: {result['processing_time']:.2f}s")

print("\n=== Batch Processing with Early Termination Complete! ===")
```

## Performance Analysis

### Performance Profiling

```python
import pynlm
from performance import PerformanceProfiler

print("=== NLM Tutorial 3: Performance Analysis ===")

# 1. Create performance profiler
print("\n1. Creating performance profiler...")
profiler = PerformanceProfiler("batch_performance")
profiler.setup()

# 2. Create batch brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

batch_config = {
    'batch_size': 1000,
    'enable_SIMD': True,
    'enable_multithreading': True,
    'enable_memory_pool': True,
}

batch_brain = BatchBrain(config, batch_config)

# 3. Run batch processing with profiling
print("\n2. Running batch processing with profiling...")
print("   Processing 10000 simulation steps...")

start_time = time.time()
for step in range(10000):
    batch_brain.step(step)
end_time = time.time()

profiler.teardown()

# 4. Generate performance report
print("\n3. Generating performance report...")
report = profiler.generateReport()

# 5. Analyze performance report
print("\n4. Analyzing performance report...")
profiler.analyzeReport(report)

# 6. Print performance analysis
print("\n5. Performance Analysis:")
print(f"   Duration: {report['duration']:.2f}s")
print(f"   Throughput: {report['throughput']:.0f} steps/s")
print(f"   Memory efficiency: {report['memory_efficiency']:.2f} steps/MB")
print(f"   Performance rating: {report['performance_rating']:.1f}/100")

print("\n=== Performance Analysis Complete! ===")
```

### Memory Analysis

```python
import pynlm
from performance import MemoryProfiler

print("=== NLM Tutorial 3: Memory Analysis ===")

# 1. Create memory profiler
print("\n1. Creating memory profiler...")
profiler = MemoryProfiler("batch_memory")
profiler.setup()

# 2. Create batch brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

batch_config = {
    'batch_size': 1000,
    'enable_SIMD': True,
    'enable_multithreading': True,
    'enable_memory_pool': True,
}

batch_brain = BatchBrain(config, batch_config)

# 3. Run batch processing with memory profiling
print("\n2. Running batch processing with memory profiling...")
print("   Processing 10000 simulation steps...")

for step in range(10000):
    batch_brain.step(step)
    if step % 1000 == 0:
        print(f"   Step {step}: Memory usage tracking...")

profiler.teardown()

# 4. Generate memory report
print("\n3. Generating memory report...")
report = profiler.generateReport()

# 5. Analyze memory report
print("\n4. Analyzing memory report...")
profiler.analyzeReport(report)

# 6. Print memory analysis
print("\n5. Memory Analysis:")
print(f"   Peak memory: {report['peak_memory']:.2f} MB")
print(f"   Average memory: {report['average_memory']:.2f} MB")
print(f"   Memory efficiency: {report['memory_efficiency']:.2f} neurons/MB")
print(f"   Memory rating: {report['memory_rating']:.1f}/100")

if report['memory_leaks_detected']:
    print("\n   Memory leaks detected:")
    for leak in report['memory_leaks']:
        print(f"     - {leak}")

print("\n=== Memory Analysis Complete! ===")
```

## Batch Processing Best Practices

### 1. Use Appropriate Batch Size
```python
# Test different batch sizes
sizes = [100, 500, 1000, 5000, 10000]

for size in sizes:
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 10000)
    
    batch_config = {
        'batch_size': size,
        'enable_SIMD': True,
        'enable_multithreading': True,
        'enable_memory_pool': True,
    }
    
    brain = BatchBrain(config, batch_config)
    start_time = time.time()
    brain.processBatch(0, size - 1)
    end_time = time.time()
    
    steps_per_second = size / (end_time - start_time)
    print(f"Batch size {size}: {steps_per_second:.0f} steps/s")
```

### 2. Optimize Memory Usage
```python
# Use memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)  # Smaller brain
config.set("brain.synapse_density", 0.05)  # Lower synapse density

batch_config = {
    'batch_size': 1000,
    'enable_SIMD': True,
    'enable_multithreading': True,
    'enable_memory_pool': True,
}

brain = BatchBrain(config, batch_config)
```

### 3. Enable Performance Optimizations
```python
# Enable all performance optimizations
batch_config = {
    'batch_size': 1000,
    'enable_SIMD': True,        # SIMD vectorization
    'enable_multithreading': True,  # Multithreading
    'enable_memory_pool': True,     # Memory pooling
    'early_termination_threshold': 0.95,  # Early termination
}
```

### 4. Monitor Progress
```python
# Set up progress monitoring
def progress_callback(progress):
    if progress * 100 > 0:
        print(f"Progress: {progress * 100:.1f}%")

batch_brain.setProgressCallback(progress_callback)
```

### 5. Use Checkpoints for Long Simulations
```python
# For long simulations, use checkpointing
batch_config = {
    'batch_size': 1000,
    'checkpoint_dir': '/tmp/checkpoints',
    'checkpoint_interval': 1000,
}

brain = BatchBrain(config, batch_config)
brain.processBatch(0, 100000)  # 100,000 steps
```

## Batch Processing Tools

### Batch Processing Scripts

```bash
#!/bin/bash
# run_batch_processing.sh - Run NLM batch processing

# Check if NLM is installed
if ! command -v nlm &> /dev/null; then
    echo "NLM is not installed. Installing..."
    pip install pynlm
fi

# Create batch processing configuration
mkdir -p config

cat > config/batch_config.json << EOF
{
  "batch": {
    "enabled": true,
    "batch_size": 1000,
    "SIMD_enabled": true,
    "multithreading_enabled": true,
    "memory_pool_enabled": true
  },
  "brain": {
    "neuron_count": 10000,
    "synapse_density": 0.1
  }
}
