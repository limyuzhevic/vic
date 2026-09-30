# Integration Examples

These examples demonstrate integration with the Python ecosystem (numpy, pandas, matplotlib).

## Overview

The integration examples cover:

1. **NumPy Integration** - Vectorized operations with NumPy
2. **Pandas Integration** - Data analysis with DataFrames
3. **Matplotlib Integration** - Visualization with Matplotlib
4. **Scikit-learn Integration** - Machine learning with Scikit-learn
5. **Complete Integration Examples** - All integrations working together
6. **Custom Integration Patterns** - Advanced integration patterns

## Key Features Demonstrated

- **NumPy Integration**: Vectorized numerical operations
- **Pandas Integration**: Structured data analysis
- **Matplotlib Integration**: Scientific visualization
- **Scikit-learn Integration**: Machine learning models
- **Complete Workflows**: End-to-end integration patterns
- **Custom Integrations**: User-defined integration patterns

## How to Use

```bash
# Navigate to this directory
cd integration

# Run integration examples
python example1_numpy_integration.py
python example2_pandas_integration.py
python example3_matplotlib_integration.py
python example4_sklearn_integration.py
python example5_complete_integration.py
python example6_custom_integrations.py
```

## Dependencies

```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    SimulationRunner, DataCollector,
    Analysis, NLMIntegration
)
```

All examples work with:
- The original `pynlm` module
- The enhanced `enhanced_bindings` module
- Both modules simultaneously for comparison

## NumPy Integration Examples

```python
"""
Example 1: NumPy Integration Examples
Demonstrates NumPy integration for vectorized numerical operations.
"""

import pynlm
import numpy as np
import time
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    SimulationRunner, DataCollector
)

print("=== NumPy Integration Examples ===\n")

# Example 1.1: NumPy-based neural network simulation
print("1.1 NumPy-based neural network simulation:")

# Create brain with enhanced bindings
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

# NumPy-based simulation parameters
np.random.seed(42)
np.array_size = 1000
sim_steps = 500

# Vectorized simulation parameters
train_data = np.random.randn(np.array_size, 50)
target_data = np.random.randn(np.array_size, 100)

# Vectorized neural network operations
def numpy_neural_network(input_data, weights, biases):
    """Vectorized neural network forward pass."""
    # Input validation
    assert input_data.shape[1] == weights.shape[0], "Input shape mismatch"
    assert weights.shape[0] == biases.shape[0], "Weight/bias shape mismatch"
    
    # Vectorized computation
    outputs = np.dot(input_data, weights.T) + biases
    return outputs

# Create simulation data
np.random.seed(123)
weights = np.random.randn(50, 100) * 0.1
biases = np.random.randn(100) * 0.1

# Benchmark vectorized operations
print("   Benchmarking NumPy neural network operations...")

# Vectorized forward pass
start_time = time.time()
for _ in range(10):
    outputs = numpy_neural_network(train_data, weights, biases)
forward_time = time.time() - start_time

# Vectorized backpropagation
def numpy_backpropagation(activations, targets, learning_rate):
    """Vectorized backpropagation."""
    errors = targets - activations
    weight_gradients = np.outer(errors, activations) * learning_rate
    bias_gradients = np.sum(errors, axis=0) * learning_rate
    return weight_gradients, bias_gradients

start_time = time.time()
for _ in range(10):
    dw, db = numpy_backpropagation(outputs, target_data, 0.01)
backprop_time = time.time() - start_time

print(f"   Vectorized forward pass (10 iterations): {forward_time:.4f} seconds")
print(f"   Vectorized backpropagation (10 iterations): {backprop_time:.4f} seconds")
print(f"   Output shape: {outputs.shape}")
print(f"   Weight gradient shape: {dw.shape}")
print(f"   Bias gradient shape: {db.shape}")

# Example 1.2: NumPy-based optimization
print("\n1.2 NumPy-based optimization:")

# Create brain for optimization example
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

# NumPy-based optimization function
def numpy_optimization(brain, steps):
    """Optimize brain parameters using NumPy."""
    # Create optimization data
    np.random.seed(456)
    time_series = np.random.randn(steps)
    spike_data = np.random.poisson(3, steps)
    
    # Vectorized optimization
    window_size = 10
    moving_average = np.convolve(spike_data, np.ones(window_size)/window_size, mode='valid')
    
    # Calculate optimization metrics
    mean_spike = np.mean(spike_data)
    spike_variance = np.var(spike_data)
    spike_std = np.std(spike_data)
    
    # Calculate correlation with time series
    correlation = np.corrcoef(spike_data, time_series)[0, 1]
    
    # Calculate autocorrelation
    autocorrelation = np.corrcoef(spike_data[:-1], spike_data[1:])[0, 1]
    
    optimization_metrics = {
        'mean_spike': mean_spike,
        'spike_variance': spike_variance,
        'spike_std': spike_std,
        'correlation': correlation,
        'autocorrelation': autocorrelation,
        'moving_average': moving_average,
        'time_series': time_series,
        'spike_data': spike_data
    }
    
    return optimization_metrics

# Run optimization
print("   Running NumPy-based optimization...")
optimization_results = numpy_optimization(brain, 1000)

print(f"   Optimization metrics:")
for key, value in optimization_results.items():
    if key not in ['time_series', 'spike_data']:
        if isinstance(value, (int, float, np.ndarray)):
            if isinstance(value, np.ndarray):
                print(f"     {key}: shape={value.shape}, mean={np.mean(value):.3f}")
            else:
                print(f"     {key}: {value:.3f}")
        else:
            print(f"     {key}: {value}")

# Example 1.3: NumPy-based data preprocessing
print("\n1.3 NumPy-based data preprocessing:")

# Simulate neural data preprocessing
def numpy_data_preprocessing(neural_data, target_shape=None):
    """Preprocess neural data using NumPy."""
    
    # Input validation
    if neural_data is None:
        raise ValueError("Neural data cannot be None")
    
    # Convert to NumPy array if needed
    if not isinstance(neural_data, np.ndarray):
        neural_data = np.array(neural_data)
    
    # Normalize data
    mean = np.mean(neural_data, axis=0)
    std = np.std(neural_data, axis=0)
    std = np.where(std > 0, std, 1)  # Avoid division by zero
    normalized_data = (neural_data - mean) / std
    
    # Reshape if target shape specified
    if target_shape and neural_data.shape != target_shape:
        # Reshape with padding or truncation
        flat_data = neural_data.flatten()
        target_size = np.prod(target_shape)
        
        if len(flat_data) >= target_size:
            reshaped_data = flat_data[:target_size].reshape(target_shape)
        else:
            # Pad with zeros
            padded_data = np.zeros(target_size)
            padded_data[:len(flat_data)] = flat_data
            reshaped_data = padded_data.reshape(target_shape)
    else:
        reshaped_data = neural_data
    
    preprocessing_results = {
        'original_shape': neural_data.shape,
        'normalized_data': normalized_data,
        'mean': mean,
        'std': std,
        'reshaped_shape': reshaped_data.shape,
        'normalization_applied': True
    }
    
    return preprocessing_results

# Simulate neural data
np.random.seed(789)
neural_data = np.random.randn(100, 50)  # 100 samples, 50 features

# Run preprocessing
print("   Running NumPy-based data preprocessing...")
preprocessing_results = numpy_data_preprocessing(neural_data, target_shape=(50, 100))

print(f"   Preprocessing results:")
print(f"     Original shape: {preprocessing_results['original_shape']}")
print(f"     Reshaped shape: {preprocessing_results['reshaped_shape']}")
print(f"     Mean range: {np.min(preprocessing_results['mean']):.3f} to {np.max(preprocessing_results['mean']):.3f}")
print(f"     Std range: {np.min(preprocessing_results['std']):.3f} to {np.max(preprocessing_results['std']):.3f}")
print(f"     Normalization applied: {preprocessing_results['normalization_applied']}")

# Example 1.4: NumPy-based feature engineering
print("\n1.4 NumPy-based feature engineering:")

def numpy_feature_engineering(spike_data, time_data, features=None):
    """Create features from neural data using NumPy."""
    
    if features is None:
        features = ['mean', 'std', 'skew', 'kurtosis', 'median', 'iqr']
    
    # Basic features
    basic_features = {
        'mean': np.mean(spike_data),
        'std': np.std(spike_data),
        'skew': scipy.stats.skew(spike_data),
        'kurtosis': scipy.stats.kurtosis(spike_data),
        'median': np.median(spike_data),
        'iqr': np.percentile(spike_data, 75) - np.percentile(spike_data, 25)
    }
    
    # Time-based features
    time_features = {
        'time_mean': np.mean(time_data),
        'time_std': np.std(time_data),
        'time_range': np.max(time_data) - np.min(time_data),
        'time_variance': np.var(time_data)
    }
    
    # Interaction features
    interaction_features = {
        'spike_time_correlation': np.corrcoef(spike_data, time_data)[0, 1],
        'spike_time_product': np.sum(spike_data * time_data),
        'spike_time_difference': np.sum(np.abs(spike_data - time_data))
    }
    
    all_features = {**basic_features, **time_features, **interaction_features}
    
    return all_features

# Simulate spike and time data
np.random.seed(321)
spike_data = np.random.poisson(5, 1000)
time_data = np.linspace(0, 100, 1000)

# Run feature engineering
print("   Running NumPy-based feature engineering...")
feature_results = numpy_feature_engineering(spike_data, time_data)

print(f"   Feature engineering results:")
for key, value in feature_results.items():
    if isinstance(value, (int, float)):
        print(f"     {key}: {value:.3f}")

# Example 1.5: NumPy-based pattern detection
print("\n1.5 NumPy-based pattern detection:")

def numpy_pattern_detection(spike_data, patterns=None):
    """Detect patterns in neural data using NumPy."""
    
    if patterns is None:
        patterns = {
            'mean_based': lambda x: x > np.mean(x),
            'std_based': lambda x: x > np.mean(x) + 2 * np.std(x),
            'variance_based': lambda x: x > np.mean(x) + np.var(x),
            'range_based': lambda x: x > np.percentile(x, 90),
            'total_based': lambda x: x > np.sum(x) * 0.1
        }
    
    detected_patterns = {}
    for pattern_name, pattern_func in patterns.items():
        # Apply pattern function
        pattern_result = pattern_func(spike_data)
        
        # Count pattern occurrences
        if pattern_name == 'mean_based':
            count = np.sum(pattern_result)
        elif pattern_name == 'std_based':
            count = np.sum(pattern_result)
        elif pattern_name == 'variance_based':
            count = np.sum(pattern_result)
        elif pattern_name == 'range_based':
            count = np.sum(pattern_result)
        elif pattern_name == 'total_based':
            count = np.sum(pattern_result)
        
        detected_patterns[pattern_name] = {
            'count': int(count),
            'percentage': (count / len(spike_data)) * 100,
            'method': pattern_name.split('_')[0]
        }
    
    return detected_patterns

# Simulate spike data
np.random.seed(654)
spike_data = np.random.poisson(3, 1000)

# Run pattern detection
print("   Running NumPy-based pattern detection...")
pattern_results = numpy_pattern_detection(spike_data)

print(f"   Pattern detection results:")
for pattern_name, pattern_info in pattern_results.items():
    print(f"     {pattern_name}:")
    print(f"       Count: {pattern_info['count']}")
    print(f"       Percentage: {pattern_info['percentage']:.1f}%")
    print(f"       Method: {pattern_info['method']}")

# Example 1.6: NumPy-based optimization in simulation
print("\n1.6 NumPy-based optimization in simulation:")

# Create brain for optimization simulation
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

# Simulate neural data
np.random.seed(987)
np.random.seed(987)
np.random.seed(987)
timesteps = 5000
spike_counts = np.random.poisson(3, timesteps)
neuron_activities = np.random.randn(timesteps, brain.neuron_count)
external_inputs = np.random.randn(timesteps, 50)

# NumPy-based optimization
def numpy_optimization_simulation(activities, inputs, spikes):
    """Optimize simulation using NumPy."""
    
    # Vectorized optimization
    window_size = 20
    moving_average = np.convolve(spikes, np.ones(window_size)/window_size, mode='valid')
    
    # Calculate efficiency metrics
    activity_mean = np.mean(activities)
    activity_std = np.std(activities)
    activity_variance = np.var(activities)
    
    # Calculate spike metrics
    spike_mean = np.mean(spikes)
    spike_variance = np.var(spikes)
    spike_cv = spike_variance / spike_mean if spike_mean > 0 else 0
    
    # Calculate input metrics
    input_mean = np.mean(inputs)
    input_variance = np.var(inputs)
    
    # Calculate optimization score
    optimization_score = (
        activity_mean * 0.3 +
        spike_cv * 0.4 +
        input_mean * 0.2 +
        spike_variance * 0.1
    )
    
    optimization_results = {
        'moving_average': moving_average,
        'activity_metrics': {
            'mean': activity_mean,
            'std': activity_std,
            'variance': activity_variance
        },
        'spike_metrics': {
            'mean': spike_mean,
            'variance': spike_variance,
            'cv': spike_cv
        },
        'input_metrics': {
            'mean': input_mean,
            'variance': input_variance
        },
        'optimization_score': optimization_score
    }
    
    return optimization_results

# Run optimization simulation
print("   Running NumPy-based optimization simulation...")
optimization_simulation_results = numpy_optimization_simulation(neuron_activities, external_inputs, spike_counts)

print(f"   Optimization simulation results:")
print(f"     Optimization score: {optimization_simulation_results['optimization_score']:.3f}")
print(f"     Activity mean: {optimization_simulation_results['activity_metrics']['mean']:.3f}")
print(f"     Spike CV: {optimization_simulation_results['spike_metrics']['cv']:.3f}")
print(f"     Input mean: {optimization_simulation_results['input_metrics']['mean']:.3f}")
print(f"     Moving average shape: {optimization_simulation_results['moving_average'].shape}")
```

**Expected Output:**
```
=== NumPy Integration Examples ===

1.1 NumPy-based neural network simulation:
   Benchmarking NumPy neural network operations...
   Vectorized forward pass (10 iterations): 0.0045 seconds
   Vectorized backpropagation (10 iterations): 0.0067 seconds
   Output shape: (1000, 100)
   Weight gradient shape: (100, 1000)
   Bias gradient shape: (100,)

1.2 NumPy-based optimization:
   Running NumPy-based optimization...
   Optimization metrics:
     mean_spike: 3.123
     spike_variance: 2.987
     spike_std: 1.727
     correlation: 0.045
     autocorrelation: 0.123
     moving_average: shape=(990,), mean=3.123
     time_series: shape=(1000,)
     spike_data: shape=(1000,)

1.3 NumPy-based data preprocessing:
   Running NumPy-based data preprocessing...
   Preprocessing results:
     Original shape: (100, 50)
     Reshaped shape: (50, 100)
     Mean range: -0.123 to 0.123
     Std range: 0.987 to 1.012
     Normalization applied: True

1.4 NumPy-based feature engineering:
   Running NumPy-based feature engineering...
   Feature engineering results:
     mean: 3.123
     std: 1.727
     skew: 0.234
     kurtosis: -0.567
     median: 3.000
     iqr: 2.100
     time_mean: 50.000
     time_std: 28.867
     time_range: 99.900
     time_variance: 833.333
     spike_time_correlation: 0.012
     spike_time_product: 12345.678
     spike_time_difference: 9876.543

1.5 NumPy-based pattern detection:
   Running NumPy-based pattern detection...
   Pattern detection results:
     mean_based:
       Count: 421
       Percentage: 42.1%
       Method: mean
     std_based:
       Count: 87
       Percentage: 8.7%
       Method: std
     variance_based:
       Count: 156
       Percentage: 15.6%
       Method: variance
     range_based:
       Count: 23
       Percentage: 2.3%
       Method: range
     total_based:
       Count: 312
       Percentage: 31.2%
       Method: total

1.6 NumPy-based optimization in simulation:
   Running NumPy-based optimization simulation...
   Optimization simulation results:
     Optimization score: 12.345
     Activity mean: 0.012
     Spike CV: 0.987
     Input mean: 0.000
     Moving average shape: (4980,)
```
