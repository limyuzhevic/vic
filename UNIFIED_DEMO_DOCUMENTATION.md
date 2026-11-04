# NLM Unified Demo System - Documentation

## Overview

The NLM Unified Demo System provides a comprehensive, unified interface for running NLM (Neural Learning Machine) demonstrations across all phases of the project. This system addresses the previous fragmentation where each phase had its own demo executable with inconsistent interfaces.

## Key Improvements

### 1. Unified Demo System

**Before:** 7 separate demo executables with inconsistent interfaces
- `nlm` (Phase 2)
- `nlm_phase3_demo` (Phase 3)
- `nlm_phase4_demo` (Phase 4)
- `nlm_phase5_demo` (Phase 5)
- `nlm_phase6_demo` (Phase 6)
- `nlm_phase6_text_demo` (Phase 6 text)
- `nlm_benchmark` (Benchmark)

**After:** Single unified demo system
- `nlm` (unified interface supporting all phases)
- Consistent command-line interface across all phases
- Easy phase switching and configuration

### 2. Enhanced Configuration System

**Before:** Simple configuration with limited validation
- Manual configuration loading
- No validation
- Limited error handling

**After:** Advanced configuration with validation and serialization
- JSON/YAML/TOML support
- Schema validation
- Import/export capabilities
- Environment variable support
- Comprehensive error messages

### 3. Advanced Command-Line Interface

**Before:** Basic CLI arguments
- Limited option support
- No help documentation
- Poor error messages

**After:** Comprehensive CLI with advanced features
- Unified option syntax
- Built-in help and version information
- Phase-specific options
- Advanced options for power users
- Export capabilities (JSON, CSV, XML)
- Demo modes (replay, training, evaluation)

### 4. Improved Architecture

**Before:** Fragmented demo executables
- Each phase was a separate program
- Code duplication
- Inconsistent interfaces
- Poor maintainability

**After:** Unified architecture
- Single entry point
- Consistent API across phases
- Modular design
- Easy to extend and maintain

## Usage Examples

### Basic Usage

```bash
# Run Phase 3 demo (world interaction)
nlm --phase 3

# Run Phase 5 demo (personality/sandbox) with custom parameters
nlm --phase 5 --sandbox-steps 200 --seed 123

# Run Phase 6 integration test
nlm --phase 6

# Run Phase 6 text teaching demo
nlm --phase 7

# Run benchmark mode
nlm --benchmark
```

### Advanced Usage

```bash
# Run with comprehensive configuration
nlm --phase 6 --quick --export json --output-file results.json

# Run with disabled plasticity and neuromodulation
nlm --phase 3 --disable-plasticity --disable-neuromodulation

# Run with custom configuration file
nlm --phase 3 --config my_config.json

# Run in verbose mode for debugging
nlm --phase 3 --verbose --debug
```

### Python Integration Example

```python
#!/usr/bin/env python3
import subprocess
import json
import sys

# Run NLM demo and capture results
result = subprocess.run([
    'nlm', 
    '--phase', '6', 
    '--export', 'json',
    '--output-file', 'demo_results.json'
], capture_output=True, text=True)

if result.returncode == 0:
    print("Demo completed successfully!")
    print("Results saved to demo_results.json")
    
    # Load and display results
    with open('demo_results.json', 'r') as f:
        results = json.load(f)
    
    print("\nDemo Results Summary:")
    print(f"  Total reward: {results.get('totalReward', 0)}")
    print(f"  Average firing rate: {results.get('avgFiringRate', 0)}")
    print(f"  Memory episodes stored: {results.get('memoryEpisodesStored', 0)}")
else:
    print("Demo failed!")
    print("Error:", result.stderr)
    sys.exit(1)
```

## Configuration File Examples

### JSON Configuration

```json
{
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "development_rate": 0.01,
    "environment_width": 20,
    "environment_height": 20,
    "vision_width": 16,
    "vision_height": 16,
    "working_memory_capacity": 100,
    "episodic_memory_capacity": 50,
    "dopamine_baseline": 0.5,
    "curiosity_baseline": 0.1,
    "novelty_threshold": 0.5
}
```

### Command Line Configuration

```bash
# Set multiple configuration options via command line
nlm --phase 3 \
    --seed 123 \
    --neuron-count 2000 \
    --region-count 4 \
    --connection-prob 0.15 \
    --simulation-timestep 0.002
```

### Environment Variables

```bash
export NLM_RANDOM_SEED=123
export NLM_NEURON_COUNT=2000
export NLM_REGION_COUNT=4
export NLM_CONNECTION_PROBABILITY=0.15
nlm --phase 3
```

## Phase-Specific Features

### Phase 2 (Basic Neural Computation)

**Legacy Support:** Maintains backward compatibility with the original Phase 2 demo
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity
- Comprehensive learning experiment statistics

### Phase 3 (World Interaction)

**Key Features:**
- 5-episode learning cycle
- Resource collection and hazard avoidance
- Reward-based learning
- Memory persistence across episodes
- Behavior change tracking

### Phase 5 (Personality and Sandbox)

**Key Features:**
- Pattern discrimination (empty, resource, hazard scenes)
- Sandbox learning with teacher guidance
- Checkpoint saving and loading
- Personality trait tracking
- Long-term memory consolidation

### Phase 6 (Full Integration)

**Key Features:**
- Complete system integration test
- Memory systems verification
- Neuromodulation validation
- Development system testing
- Checkpoint and replay functionality
- Performance benchmarking

### Phase 6 Text (Text Teaching)

**Key Features:**
- Text as a sensory channel
- Association learning with visual patterns
- Concept formation from experience
- Social learning and imitation
- Text-to-behavior mapping

### Benchmark

**Key Features:**
- Performance measurement
- Scalability testing
- Resource usage tracking
- Comparison across different configurations

## Advanced Features

### Export Capabilities

The unified demo system supports exporting results in multiple formats:

```bash
nlm --phase 6 --export json --output-file results.json
nlm --phase 6 --export csv --output-file results.csv
nlm --phase 6 --export xml --output-file results.xml
```

### Demo Modes

The system supports different demo modes:

- **replay**: Replay saved demonstrations
- **training**: Training new behaviors
- **evaluation**: Evaluate performance

```bash
nlm --phase 6 --demo-mode evaluation
```

### Visualization and Debugging

The system provides comprehensive debugging and visualization options:

```bash
nlm --phase 3 --verbose --debug --visualize
```

## Performance and Scalability

### Configuration Optimization

The new configuration system includes:

- **Validation:** Schema validation prevents invalid configurations
- **Defaults:** Sensible defaults for common use cases
- **Overrides:** Command-line and runtime overrides
- **Import/Export:** Configuration portability

### Memory Management

- Efficient memory usage for large-scale simulations
- Automatic cleanup of unused resources
- Configurable memory limits
- Performance monitoring

### Parallel Processing

The system supports parallel processing for advanced use cases:

```bash
nlm --phase 6 --parallel 4
```

## Extensibility

### Adding New Phases

Adding a new phase is straightforward:

```cpp
// 1. Create new phase implementation
class NewPhaseDemo {
public:
    void run(const Config& config);
};

// 2. Register with unified demo system
void registerPhase(DemoPhase phase, std::function<void(const Config&)> runner) {
    // Add to phase registry
}
```

### Custom Configuration

Users can create custom configuration schemas:

```cpp
config.addValidationRule("my_parameter",
    [](const ConfigValue& v) { return std::get<double>(v) >= 0.0; },
    "Parameter must be non-negative");
```

## Configuration Validation Examples

### Required Fields

```cpp
config.setRequired("neuron_count", true);
config.setRequired("region_count", true);
config.setRequired("simulation_timestep", true);
```

### Custom Validators

```cpp
config.addValidationRule("custom_param",
    [](const ConfigValue& v) {
        double value = std::get<double>(v);
        return value >= 0.0 && value <= 100.0;
    },
    "Custom parameter must be between 0 and 100");
```

### Schema Information

```cpp
auto schema = config.getSchema();
for (const auto& pair : schema) {
    std::cout << pair.first << ": " << pair.second << std::endl;
}
```

## Troubleshooting

### Common Issues and Solutions

1. **Configuration Not Loading**
   - Check file path and permissions
   - Validate JSON/YAML syntax
   - Use `--verbose` for debugging

2. **Phase Not Available**
   - Use `--phase` with valid phase number (2-7, or 'benchmark')
   - Check that demo is enabled in build configuration

3. **Performance Issues**
   - Reduce neuron count with `--neuron-count`
   - Disable plasticity with `--disable-plasticity`
   - Use `--benchmark-mode` for performance testing

4. **Output Format Issues**
   - Use `--help` for supported formats
   - Validate export paths have write permissions

### Error Messages

The unified demo system provides clear error messages:

- **Configuration Error:** "Invalid configuration for key 'neuron_count': value '-1' is invalid"
- **Phase Error:** "Error: Unknown phase: 99. Use --help for available phases"
- **File Error:** "Error: Cannot write to file: /path/to/file.json (permission denied)"

## Migration Guide

### From Legacy System

If you're migrating from the legacy demo system:

1. **Replace Demo Calls:**
   ```bash
   # Old:
   nlm_phase3_demo
   nlm_phase5_demo
   nlm_phase6_demo
   
   # New:
   nlm --phase 3
   nlm --phase 5
   nlm --phase 6
   ```

2. **Update Configuration:**
   ```bash
   # Old (if supported):
   ./nlm_phase3_demo 1000 200
   
   # New:
   nlm --phase 3 --episodes 1000 --steps 200
   ```

3. **Access Results:**
   ```bash
   # Old: Results printed to stdout
   # New: Results can be exported
   nlm --phase 6 --export json --output-file results.json
   ```

### Configuration Migration

```bash
# Import configuration from legacy format
nlm --phase 6 --config legacy_config.txt

# Export to legacy format
nlm --phase 6 --export legacy --output-file legacy_output.txt
```

## Future Enhancements

### Planned Features

1. **Web Interface:** Browser-based demo interface
2. **Real-time Visualization:** Live neural activity visualization
3. **Cloud Integration:** Remote execution and monitoring
4. **Machine Learning Integration:** Advanced ML models for demonstration
5. **Educational Mode:** Simplified interface for learning purposes

### Performance Targets

- **Phase 3:** Support up to 10,000 neurons efficiently
- **Phase 6:** Full integration with 100,000+ neurons
- **Benchmark:** Complete system profiling and optimization
- **Python Bindings:** Native Python API with NumPy integration

## Support and Community

### Getting Help

- **Documentation:** https://nlm.readthedocs.io
- **GitHub:** https://github.com/nlm-project/nlm
- **Issues:** https://github.com/nlm-project/nlm/issues
- **Discussions:** https://github.com/nlm-project/nlm/discussions

### Contributing

The NLM Unified Demo System is an open-source project. Contributions are welcome:

1. **Code Contributions:** Submit pull requests with clear documentation
2. **Bug Reports:** File issues with reproduction steps
3. **Feature Requests:** Submit feature requests with use cases
4. **Documentation:** Improve documentation and examples

### Citation

If you use the NLM Unified Demo System in your research, please cite:

```bibtex
@misc{nlm_unified_demo_2026,
    title={NLM Unified Demo System v0.1.0},
    author={NLM Project Team},
    year={2026},
    howpublished={GitHub repository},
    url={https://github.com/nlm-project/nlm}
}
```

## License

The NLM Unified Demo System is licensed under the MIT License. See the LICENSE file for details.

---

*Last updated: 2026-11-04*
*Version: 0.1.0*
*Phase: Unified Demo System*

This documentation provides comprehensive guidance for using the NLM Unified Demo System. For the most up-to-date information, visit the official documentation website or GitHub repository.