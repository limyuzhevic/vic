# NLM Python Examples

This directory contains comprehensive examples for using the enhanced NLM Python bindings.

## Overview

These examples demonstrate the new features and capabilities of the enhanced NLM Python bindings, including:

- Context managers and resource management
- Advanced statistical analysis methods
- Configuration validation and management
- Performance benchmarking
- Agent simulation with neuromodulation
- Data export and reporting

## Example Files

### basic_examples.py
Basic usage examples for beginners.

### advanced_features.py
Demonstrates advanced features like context managers, benchmarking, and analysis.

### data_analysis.py
Shows how to perform statistical analysis on neural activity.

### agent_simulation.py
Complete agent simulation examples with different scenarios.

### configuration_management.py
Configuration validation, saving, and management utilities.

### performance.py
Performance testing and benchmarking examples.

## How to Run Examples

```bash
python basic_examples.py
python advanced_features.py
python data_analysis.py
python agent_simulation.py
python configuration_management.py
python performance.py
```

## Key Features Demonstrated

1. **Enhanced Python API**
   - Type hints and modern Python features
   - Context managers for resource management
   - Convenience methods for common operations

2. **Statistical Analysis**
   - Activity pattern analysis
   - Network metrics computation
   - Synchrony and burst detection
   - Frequency spectrum analysis

3. **Configuration Management**
   - Configuration validation
   - File I/O operations
   - Parameter validation

4. **Agent Simulation**
   - Complete agent lifecycle
   - Neuromodulation support
   - Sensorimotor loop

5. **Performance Monitoring**
   - Benchmarking
   - Performance metrics
   - Analysis report generation

6. **Error Handling**
   - Custom exception handling
   - Validation errors
   - Graceful error recovery

## Prerequisites

```bash
pip install pynlm numpy matplotlib pandas
```

## Example Usage

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Use context manager for automatic resource management
with brain:
    # Run simulation with monitoring
    metrics = brain.benchmarkPerformance([100, 500, 1000])
    print(f"Performance metrics: {metrics}")

# Export analysis report
brain.exportAnalysisReport("brain_analysis.txt")
```

## Integration with Python Ecosystem

The enhanced NLM bindings integrate seamlessly with:

- **NumPy**: For numerical operations and array handling
- **Matplotlib**: For visualization and plotting
- **Pandas**: For data analysis and reporting
- **Jupyter Notebooks**: For interactive exploration

## Testing Examples

Run the examples to see the enhanced features in action:

```bash
for example in basic_examples advanced_features data_analysis agent_simulation; do
    echo "Running $example..."
    python python/examples/$example.py
    echo "=== $example completed ==="
    echo ""
done
```

## Additional Resources

- [NLM Documentation](../docs/README.md)
- [Python API Reference](../docs/ARCHITECTURE.md#python-bindings)
- [Configuration Guide](../HOW_TO_USE.md#part-3-using-the-python-library)
- [Troubleshooting Guide](../HOW_TO_USE.md#troubleshooting)

## License

MIT License

Copyright (c) 2026 NLM Project Authors

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.