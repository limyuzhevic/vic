# NLM Educational Examples

This directory contains comprehensive educational examples for the NLM (Neural Learning Machine) Python bindings. These examples are designed to teach users how to:

- Basic NLM usage and API
- Configuration management and presets
- Learning experiments and plasticity
- Visualization and data export
- Agent-based simulations
- Advanced configuration techniques
- Performance testing and scalability

## Prerequisites

Before running these examples, ensure that:

1. NLM is properly built and installed
2. Python is installed (Python 3.8+)
3. All dependencies are installed
4. The `pynlm` module is available

## Installation and Setup

1. Build NLM from the workspace root:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. Install Python bindings (if not already installed):
   ```bash
   cd ..
   pip install -e .
   ```

3. Set up the Python environment:
   ```bash
   cd /workspace/app/examples
   export PYTHONPATH="$(pwd):$PYTHONPATH"
   ```

## Running Examples

### Running All Examples

To run all examples sequentially:

```bash
cd /workspace/app/examples
python3 educational_examples.py
```

### Running Specific Examples

To run a specific example, provide the example number (1-8):

```bash
cd /workspace/app/examples
python3 educational_examples.py 1  # Basic NLM Usage
python3 educational_examples.py 3  # Learning Experiment
python3 educational_examples.py 5  # Agent-Based Simulation
# ... and so on
```

### Interactive Mode

For an interactive demonstration, create an interactive script:

```bash
cd /workspace/app/examples
cat > interactive_demo.py << 'EOF'
import pynlm as nlm

# Quick demo
print("NLM Interactive Demo")
print("==================")

# Create basic brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run short simulation
for step in range(10):
    brain.step(step)

print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")

# Show configuration
print(f"\nConfiguration summary:")
print(f"  Neuron count: {brain.getConfig().get('neuron_count')}")
print(f"  Region count: {brain.getConfig().get('region_count')}")
print(f"  Simulation timestep: {brain.getConfig().get('simulation_timestep')}")
EOF

python3 interactive_demo.py
```

## Example 1: Basic NLM Usage

**File:** `educational_examples.py` (lines 45-103)

This example demonstrates the fundamental steps for creating and using an NLM brain:

1. Creating a configuration
2. Initializing a brain
3. Running a simulation
4. Observing results

**Key Concepts:**
- Basic brain initialization
- Simulation execution
- Data collection and statistics
- Configuration management

**Expected Output:**
- Brain creation and initialization messages
- Simulation statistics (neuron count, synapse count, spikes, firing rate)
- Configuration information

## Example 2: Configuration Presets

**File:** `educational_examples.py` (lines 105-142)

This example demonstrates the different configuration presets available in NLM:

1. Available presets: basic, learning, fast, research, demo
2. Preset descriptions and values
3. How to apply presets programmatically

**Key Concepts:**
- Configuration presets
- Preset descriptions
- Preset value inspection
- Preset application

**Expected Output:**
- List of all available presets
- Each preset's description
- Preset configuration values

## Example 3: Learning Experiment

**File:** `educational_examples.py` (lines 144-176)

This example demonstrates how to set up and run a learning experiment using NLM's plasticity mechanisms:

1. Creating a learning-focused configuration
2. Setting up the brain with plasticity enabled
3. Using the ExperimentSession context manager for automatic plasticity setup
4. Analyzing learning metrics and results

**Key Concepts:**
- Plasticity mechanisms
- Learning experiments
- STDP and Hebbian plasticity
- Learning metrics and analysis

**Expected Output:**
- Experiment setup messages
- Learning metrics (weight change, strengthened/weakened synapses)
- Learning detection results

## Example 4: Visualization and Data Export

**File:** `educational_examples.py` (lines 178-221)

This example demonstrates how to export simulation data for visualization and analysis:

1. Network statistics export
2. Spike data export
3. Activity data export
4. Weight data export
5. Comprehensive visualization creation

**Key Concepts:**
- Data export formats
- Visualization utilities
- Data analysis
- File format handling

**Expected Output:**
- Export completion messages
- File creation confirmations
- Visualization generation

## Example 5: Agent-Based Simulation

**File:** `educational_examples.py` (lines 223-279)

This example demonstrates how to create an agent-based simulation using NLM's agent brain interface and world system:

1. Creating brain and world components
2. Setting up agent brain interface
3. Running simulation loop with sensory input and motor output
4. Logging and monitoring agent behavior

**Key Concepts:**
- Agent architecture
- Sensorimotor loops
- World simulation
- Reward-based learning
- Action selection

**Expected Output:**
- Agent initialization messages
- Simulation step logs
- Action execution results
- Final statistics

## Example 6: Advanced Configuration and Validation

**File:** `educational_examples.py` (lines 281-319)

This example demonstrates advanced configuration techniques:

1. Creating configuration from Python dictionary
2. Applying presets to configurations
3. Configuration validation
4. Configuration statistics and analysis

**Key Concepts:**
- Advanced configuration
- Configuration validation
- Configuration comparison
- Preset management

**Expected Output:**
- Configuration setup messages
- Validation results
- Configuration statistics
- Analysis output

## Example 7: Multiple Scenario Comparison

**File:** `educational_examples.py` (lines 321-351)

This example demonstrates how to compare different simulation scenarios:

1. Running multiple scenarios with different configurations
2. Collecting and comparing statistics across scenarios
3. Performance comparison

**Key Concepts:**
- Scenario comparison
- Performance analysis
- Configuration comparison
- Results visualization

**Expected Output:**
- Scenario execution messages
- Comparison table
- Performance analysis

## Example 8: Performance and Scalability Testing

**File:** `educational_examples.py` (lines 353-392)

This example demonstrates how to test NLM performance with different configuration sizes:

1. Performance testing across different neuron counts
2. Timing measurements
3. Throughput analysis (spikes per second)
4. Scalability assessment

**Key Concepts:**
- Performance testing
- Scalability analysis
- Benchmark testing
- Resource utilization

**Expected Output:**
- Performance test results
- Timing measurements
- Throughput calculations
- Scalability analysis

## Additional Resources

### Documentation

For more detailed documentation, refer to:

1. **NLM Main README**: `/workspace/app/README.md`
2. **Architecture Documentation**: `/workspace/app/docs/`
3. **Phase 6 Audit**: `/workspace/app/docs/PHASE6_FINAL_AUDIT.md`
4. **API Documentation**: Generated from docstrings in the source code

### Related Projects

NLM includes several related projects and modules:

1. **NLM Core Library**: `/workspace/app/src/`
2. **Python Bindings**: `/workspace/app/python/`
3. **Configuration System**: `/workspace/app/src/core/Config/`
4. **Visualization Module**: `/workspace/app/src/visualization/`
5. **Test Suite**: `/workspace/app/tests/`

### Support and Troubleshooting

If you encounter issues while running the examples:

1. **Build Issues**: Ensure NLM is properly built with Python bindings
2. **Import Errors**: Check that the pynlm module is correctly installed
3. **Performance Issues**: Monitor system resources and consider using smaller configurations
4. **Data Export Issues**: Ensure output directories are writable

For issues, refer to the NLM troubleshooting documentation or contact the development team.

## Example Usage Script

For quick testing and demonstration, create a simple script:

```bash
cat > quick_test.py << 'EOF'
#!/usr/bin/env python3
"""
Quick test script for NLM Python bindings
"""
import sys
import os

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm as nlm
    
    print("Quick NLM Test")
    print("===============")
    
    # Create and test brain
    brain = nlm.createDefaultBrain()
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    success = brain.initialize()
    print(f"✓ Brain initialization: {'Success' if success else 'Failed'}")
    
    # Run short simulation
    for step in range(20):
        brain.step(step)
    
    print(f"✓ Simulation completed")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("\nAll tests passed!")
    
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
EOF

python3 quick_test.py
```

## Best Practices

1. **Always initialize brains** before running simulations
2. **Use context managers** for experiments when possible
3. **Validate configurations** before use
4. **Clean up resources** after simulations
5. **Monitor performance** for large-scale simulations
6. **Use appropriate presets** for your specific use case
7. **Handle errors gracefully** in production code
8. **Log simulation progress** for debugging
9. **Export data regularly** for analysis
10. **Use consistent configuration** across comparisons

## Future Enhancements

These examples can be extended with:

1. **Interactive visualizations** using web-based plots
2. **Real-time monitoring** dashboards
3. **Machine learning integration** for parameter optimization
4. **Multi-threaded simulations** for better performance
5. **GPU acceleration** for larger networks
6. **Distributed computing** for massive simulations
7. **Advanced visualization** with 3D rendering
8. **Automated experiment generation** and analysis
9. **Reproducibility** features for scientific research
10. **Documentation generation** from examples

## License

This educational examples package is licensed under the MIT License, the same as the NLM project itself.

For more information, refer to the main NLM LICENSE file.

---

## Acknowledgements

These examples were developed as part of the NLM (Neural Learning Machine) project, an experimental artificial developmental brain system.

Special thanks to the NLM development team for their contributions to the core architecture and API design.

For more information about NLM, visit the project documentation and repository.
