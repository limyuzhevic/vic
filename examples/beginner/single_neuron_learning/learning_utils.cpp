# Single Neuron Learning - Utility Functions

This file contains utility functions used throughout the single neuron learning implementation.

## Core Utilities

### 1. Statistical Analysis Functions

```cpp
// Calculate mean of a vector
float calculate_mean(const std::vector<float>& data);

// Calculate standard deviation
float calculate_std(const std::vector<float>& data, float mean);

// Find maximum absolute change
float find_max_change(const std::vector<float>& data);

// Calculate relative change
float calculate_relative_change(float initial, float final);
```

### 2. Data Processing Functions

```cpp
// Linear interpolation for smooth curves
std::vector<float> interpolate_data(const std::vector<float>& data, int window_size);

// Calculate moving average
std::vector<float> moving_average(const std::vector<float>& data, int window_size);

// Filter outliers using IQR method
std::vector<float> filter_outliers(const std::vector<float>& data);
```

### 3. Learning Metrics

```cpp
// Learning efficiency score
float calculate_learning_efficiency(float initial_response, float final_response, int time_steps);

// Adaptation index
float calculate_adaptation_index(const std::vector<float>& response_curve);

// Stability measure
float calculate_stability(const std::vector<float>& response_curve);
```

### 4. Neural Network Utilities

```cpp
// Get main neuron from brain
NeuronId get_main_neuron_id(const std::shared_ptr<Brain>& brain);

// Get main region from brain
RegionId get_main_region_id(const std::shared_ptr<Brain>& brain);

// Enable Hebbian plasticity in network
void enable_hebbian_plasticity(std::shared_ptr<Brain> brain);

// Get average synaptic weight
float get_average_synaptic_weight(const std::shared_ptr<Brain>& brain);
```

### 5. File I/O Utilities

```cpp
// Save learning data to CSV
bool save_learning_data(const std::string& filename, const LearningResults& results);

// Load learning data from CSV
bool load_learning_data(const std::string& filename, LearningResults& results);

// Save spike times
bool save_spike_times(const std::string& filename, const std::vector<double>& spike_times);

// Load spike times
bool load_spike_times(const std::string& filename, std::vector<double>& spike_times);
```

### 6. Visualization Utilities

```cpp
// Generate learning curve plot data
bool generate_learning_curve_data(const LearningResults& results, std::vector<double>& time_points, std::vector<double>& firing_rates);

// Generate synaptic weight distribution
bool generate_weight_distribution(const std::vector<double>& weights, std::vector<double>& bin_values, std::vector<double>& bin_counts);

// Generate spike raster plot data
bool generate_spike_raster(const std::vector<double>& spike_times, double duration, std::vector<int>& spike_counts);
```

### 7. Configuration Utilities

```cpp
// Load learning experiment configuration
bool load_learning_config(const std::string& filename, LearningConfig& config);

// Save learning experiment configuration
bool save_learning_config(const std::string& filename, const LearningConfig& config);

// Validate learning configuration
bool validate_learning_config(const LearningConfig& config);
```

### 8. Error Handling Utilities

```cpp
// Learning experiment error codes
enum class LearningErrorCode {
    SUCCESS = 0,
    INVALID_CONFIG = 1,
    NETWORK_CREATION_FAILED = 2,
    STIMULUS_PRESENTATION_FAILED = 3,
    DATA_ANALYSIS_FAILED = 4,
    FILE_IO_ERROR = 5,
    MEMORY_ALLOCATION_FAILED = 6,
    VALIDATION_FAILED = 7
};

// Get error message from error code
std::string get_error_message(LearningErrorCode error_code);

// Log learning experiment results
void log_learning_results(const LearningResults& results, LogLevel level = LogLevel::Info);
```

### 9. Performance Monitoring

```cpp
// Performance metrics for learning experiment
struct PerformanceMetrics {
    double cpu_time;           // CPU time in seconds
    double memory_usage;       // Memory usage in MB
    double simulation_speed;   // Simulation steps per second
    double learning_rate_achieved;  // Actual learning rate achieved
    int total_operations;      // Total computational operations
};

// Measure performance of learning experiment
PerformanceMetrics measure_performance(const LearningResults& results);

// Log performance metrics
void log_performance(const PerformanceMetrics& metrics);
```

### 10. Parallel Processing Utilities

```cpp
// Parallel learning simulation
void parallel_learning_simulation(std::shared_ptr<Brain> brain, const std::vector<double>& stimuli, int num_threads);

// Parallel data analysis
void parallel_data_analysis(const std::vector<LearningDataPoint>& data, std::vector<double>& results, int num_threads);

// Parallel file I/O
void parallel_file_io(const std::string& filename, const std::vector<LearningDataPoint>& data, bool write_mode);
```

## Usage Examples

### Basic Statistics

```cpp
#include "learning_utils.h"

// Calculate mean and standard deviation
std::vector<float> response_data = {0.5, 0.7, 0.9, 1.2, 1.5};
float mean = calculate_mean(response_data);
float std_dev = calculate_std(response_data, mean);

// Find maximum change
float max_change = find_max_change(response_data);
```

### Data Processing

```cpp
// Apply moving average to smooth learning curve
std::vector<float> smooth_data = moving_average(response_data, 3);

// Filter outliers
std::vector<float> filtered_data = filter_outliers(response_data);
```

### Learning Metrics

```cpp
// Calculate learning efficiency
float efficiency = calculate_learning_efficiency(0.5, 1.5, 100);

// Calculate adaptation index
float adaptation = calculate_adaptation_index(response_data);
```

### Neural Network Utilities

```cpp
// Get main neuron and region
NeuronId main_neuron = get_main_neuron_id(brain);
RegionId main_region = get_main_region_id(brain);

// Enable plasticity
enable_hebbian_plasticity(brain);

// Get synaptic weight
float avg_weight = get_average_synaptic_weight(brain);
```

### File I/O

```cpp
// Save results to file
LearningResults results = run_experiment();
save_learning_data("results.csv", results);

// Load results from file
LearningResults loaded_results;
load_learning_data("results.csv", loaded_results);
```

### Error Handling

```cpp
// Handle learning errors
LearningErrorCode error = LearningErrorCode::SUCCESS;
if (error != LearningErrorCode::SUCCESS) {
    std::string message = get_error_message(error);
    std::cerr << "Error: " << message << std::endl;
}
```

### Performance Monitoring

```cpp
// Measure and log performance
PerformanceMetrics metrics = measure_performance(results);
log_performance(metrics);
```

## Learning Integration

These utility functions are integrated throughout the single neuron learning system:

1. **LearningExperiment Class**: Uses statistics and data processing utilities
2. **Analysis Module**: Provides advanced learning metrics and statistical analysis
3. **Visualization Module**: Generates plot data and performance metrics
4. **Error Handling**: Provides comprehensive error management and logging

## Mathematical Background

The utility functions are based on standard mathematical and statistical principles:

- **Mean and Standard Deviation**: Fundamental statistics for learning analysis
- **Moving Average**: Signal processing for noise reduction
- **Outlier Detection**: Statistical methods for data quality assessment
- **Learning Metrics**: Domain-specific measures for neural learning
- **Error Handling**: Robust error management for production systems

## Future Enhancements

### Planned Additions

1. **Advanced Statistics**: Bayesian analysis, confidence intervals
2. **Machine Learning Integration**: Pattern recognition for learning analysis
3. **Parallel Computing**: GPU acceleration for large-scale simulations
4. **Distributed Computing**: Multi-machine learning experiments
5. **Real-time Monitoring**: Live performance tracking and visualization

### Research Extensions

- **Neural Dynamics Analysis**: Advanced time-series analysis
- **Plasticity Modeling**: Multi-factor learning rules
- **Network Topology**: Structural analysis of neural connectivity
- **Cross-frequency Coupling**: Analysis of oscillatory interactions
- **Critical Period Analysis**: Developmental learning stages

## Academic Integration

### Research Applications

These utility functions support various research scenarios:

1. **Learning Algorithm Development**: Statistical validation of learning rules
2. **Performance Optimization**: Computational efficiency analysis
3. **Data Quality Assessment**: Robustness testing of neural learning
4. **Reproducibility Studies**: Standardized analysis across experiments

### Experimental Design

The utilities can be adapted for different experimental conditions:

- **Varying Stimulus Patterns**: Statistical analysis of different input patterns
- **Network Size Effects**: Scalability analysis across different sizes
- **Parameter Optimization**: Automated parameter tuning using statistical methods
- **Cross-validation**: Robust learning assessment across multiple runs

## Performance Characteristics

### Computational Complexity

- **Statistics**: O(N) for basic statistics, O(N log N) for sorting
- **Data Processing**: O(N) for linear operations, O(N²) for matrix operations
- **Learning Metrics**: O(N) for basic metrics, O(N²) for advanced analysis
- **Visualization**: O(N log N) for plotting algorithms

### Memory Usage

- **Basic Statistics**: O(1) additional memory
- **Data Processing**: O(N) additional memory for intermediate results
- **Learning Metrics**: O(N) additional memory for analysis arrays
- **Visualization**: O(N) additional memory for plot data

### Optimization Features

1. **Memory Efficiency**: Vectorized operations and memory pooling
2. **CPU Optimization**: SIMD instructions for parallel processing
3. **Cache Optimization**: Data layout optimization for better cache performance
4. **Threading**: Multi-core support for parallel operations

## Integration with Single Neuron Learning

### Core Functionality

The utility functions provide essential support for the single neuron learning system:

1. **Data Analysis**: Statistical analysis of learning curves and synaptic changes
2. **Visualization**: Generation of plots and graphical representations
3. **Error Management**: Robust error handling and logging
4. **Performance Monitoring**: Real-time performance tracking
5. **Configuration Management**: Standardized configuration loading/saving

### Learning Experiment Workflow

1. **Initialization**: Configure learning parameters using configuration utilities
2. **Data Collection**: Present stimuli and collect neural responses
3. **Analysis**: Apply statistical and learning metrics using utility functions
4. **Visualization**: Generate plots and visual representations
5. **Results Storage**: Save results using file I/O utilities
6. **Performance Monitoring**: Track and optimize computational efficiency

## Configuration and Customization

### Utility Configuration

The utility functions can be configured through the learning experiment configuration:

```ini
[statistics]
mean_method = "arithmetic"
std_method = "sample"
outlier_threshold = 3.0

[data_processing]
moving_average_window = 3
filter_outliers = true

[learning_metrics]
efficiency_method = "relative_change"
adaptation_window = 50
stability_threshold = 0.1
```

### Customization Options

1. **Statistical Methods**: Choose different algorithms for mean, std, etc.
2. **Data Processing**: Adjust filtering and smoothing parameters
3. **Learning Metrics**: Customize learning assessment criteria
4. **Performance Optimization**: Enable/disable specific optimizations
5. **Output Format**: Choose file format for results

## Testing and Validation

### Unit Tests

The utility functions are thoroughly tested with:

1. **Statistical Functions**: Mean, std, max change calculations
2. **Data Processing**: Interpolation, filtering, smoothing
3. **Learning Metrics**: Efficiency, adaptation, stability measures
4. **File I/O**: CSV writing and reading operations
5. **Error Handling**: Exception handling and error reporting

### Integration Tests

1. **Learning Experiment Integration**: End-to-end testing of learning workflow
2. **Performance Integration**: Real-time performance monitoring
3. **Visualization Integration**: Complete plotting pipeline
4. **Configuration Integration**: Full configuration management
5. **Error Handling Integration**: Robust error reporting throughout system

## License and Attribution

```
Copyright (c) 2025 NLM Project - Single Neuron Learning Utilities

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files, to deal in the Software
without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
```

## Contact and Support

### For Technical Questions

- **Documentation**: See `README.md` and `HOW_TO_USE.md`
- **Community**: NLM forums and Discord channel
- **Issues**: Report bugs through GitHub issue tracker
- **Support**: Contact technical support team

### For Research Collaboration

- **Academic Partners**: Contact for collaborative research projects
- **Data Sharing**: Access utility function documentation and examples
- **Publications**: Reference utility functions in scholarly work
- **Validation**: Request independent verification of utility performance

### Support Resources

1. **Quick Start Guide**: `examples/beginner/single_neuron_learning/quick_start.md`
2. **API Documentation**: Generated by Doxygen, available in `docs/`
3. **Utilities Documentation**: This file and companion documentation
4. **Examples Gallery**: Additional utility usage in `examples/`

### Citing This Work

If you use these utility functions in your research or publications, please cite:

```bibtex
@misc{nlm_single_neuron_learning_utilities_2025,
  title={Single Neuron Learning Utility Functions for NLM},
  author={NLM Project Team},
  year={2025},
  url={https://github.com/nlm-project/nlm/examples/beginner/single_neuron_learning/utilities},
  note={Utility functions for neural learning analysis and visualization}
}
```

These utility functions represent a comprehensive toolkit for neural learning analysis, designed to support both educational and research applications in computational neuroscience.
