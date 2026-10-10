# NLM (熙然) - Neural Learning Machine
# Phase 6: Advanced Features and User Interface
#
# This document describes advanced features, commands, and usage patterns
# for the NLM Phase 6 integrated artificial brain system.

# Advanced Usage Patterns

## 1. Experiment Management

### Batch Experiment Runs
```bash
# Run multiple experiments with different configurations
for neurons in 100 500 1000; do
    echo "Running experiment with $neurons neurons"
    ./nlm --neurons=$neurons --steps=1000 --checkpoint=experiment_${neurons}.bin
    mv nlm_checkpoint.bin experiment_${neurons}_final.bin
done
```

### Configuration Templates
```bash
# Create custom configuration file
cat > my_config.cfg << EOF
neuron_count = 750
region_count = 2
connection_probability = 0.2
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.01
enable_development = true
enable_replay = true
EOF

# Run with custom configuration
./nlm --config=my_config.cfg --checkpoint=custom_exp.bin
```

## 2. Performance Tuning

### High-Performance Settings
```bash
# For larger simulations with better performance
./nlm --neurons=2000 --steps=5000 \
     --region-count=4 \
     --timestep=0.0005 \
     --checkpoint=large_exp.bin
```

### Memory-Efficient Settings
```bash
# For systems with limited memory
./nlm --neurons=100 --steps=500 \
     --demo-mode \
     --no-development \
     --no-replay
```

## 3. Development and Testing

### Quick Integration Verification
```bash
# Fast verification of system integration
./nlm --demo-mode
```

### Phase 2 Compatibility
```bash
# Run legacy experiments for backward compatibility
./nlm --phase2
```

### Custom Experiment Parameters
```bash
# Run specific test scenarios
./nlm --neurons=500 --steps=2000 --region-count=1 \
     --checkpoint=controlled_exp.bin
```

## 4. Advanced Command Line Options

### Configuration File Management
```bash
# View configuration file
./nlm --config=configs/default.cfg --help

# Create new configuration template
./nlm --help  # Shows all options
```

### System Monitoring
```bash
# Enable verbose logging (requires log level configuration)
./nlm --neurons=500 --steps=1000
```

## 5. Automation Scripts

### Experiment Pipeline Script
```bash
#!/bin/bash
# experiment_pipeline.sh - Run a series of NLM experiments

SCRIPT_DIR=$(cd $(dirname $0); pwd)
NLM_EXEC="$SCRIPT_DIR/nlm"
CONFIG_DIR="$SCRIPT_DIR/configs"
RESULTS_DIR="$SCRIPT_DIR/results"

mkdir -p $RESULTS_DIR

# Run quick verification
echo "=== Quick Integration Verification ==="
$NLM_EXEC --demo-mode

# Run standard experiment
echo "=== Standard Experiment ==="
$NLM_EXEC --neurons=500 --steps=2000 \
         --checkpoint=$RESULTS_DIR/standard_exp.bin

# Run development experiment
echo "=== Development Experiment ==="
$NLM_EXEC --neurons=300 --steps=1500 \
         --no-development \
         --checkpoint=$RESULTS_DIR/no_dev_exp.bin

# Run replay experiment
echo "=== Replay Experiment ==="
$NLM_EXEC --neurons=400 --steps=2000 \
         --no-replay \
         --checkpoint=$RESULTS_DIR/no_replay_exp.bin

echo "=== All experiments completed ==="
echo "Results saved to: $RESULTS_DIR"
```

### Parameter Sweeps
```bash
#!/bin/bash
# parameter_sweep.sh - Run experiments with varying parameters

echo "Starting parameter sweep..."

# Neuron count sweep
for neurons in 100 250 500 1000; do
    echo "Testing with $neurons neurons"
    ./nlm --neurons=$neurons --steps=1000 \
          --checkpoint=neuron_sweep_${neurons}.bin
    echo "Completed: $neurons neurons"
done

# Region count sweep
echo "Testing region configurations..."
for regions in 1 2 4; do
    echo "Testing with $regions regions"
    ./nlm --neurons=500 --steps=1000 \
          --region-count=$regions \
          --checkpoint=region_sweep_${regions}.bin
    echo "Completed: $regions regions"
done

echo "Parameter sweep completed"
```

## 6. Integration with Other Tools

### Python Bindings
```python
# Example of using NLM results with Python analysis
import subprocess
import json
import time

# Run NLM experiment
result = subprocess.run([
    './nlm', '--neurons=500', '--steps=1000', 
    '--checkpoint=python_test.bin'
], capture_output=True, text=True)

if result.returncode == 0:
    print("NLM experiment completed successfully")
    print(result.stdout)
else:
    print("NLM experiment failed")
    print(result.stderr)
```

### Visualization Integration
```bash
# Future: Integration with visualization tools
# ./nlm --visualization=launch_browser
# ./nlm --generate_analysis_report=neural_dynamics.json
```

## 7. System Administration

### Build and Installation
```bash
# Build the project
cmake -B build
cmake --build build

# Run tests
make -C build check

# Install (future feature)
make -C build install
```

### Monitoring and Maintenance
```bash
# Check build configuration
make -C build config

# View build logs
make -C build VERBOSE=1

# Clean build artifacts
cmake -B build --clean-first
```

## 8. Troubleshooting

### Common Issues and Solutions

1. **Configuration file not found**
```bash
# Solution: Create configs/default.cfg
./nlm --config=configs/default.cfg
```

2. **Insufficient memory**
```bash
# Solution: Reduce neuron count or steps
./nlm --neurons=100 --steps=500 --demo-mode
```

3. **Permission denied**
```bash
# Solution: Check file permissions and run with appropriate access
chmod +x ./nlm
```

4. **Missing dependencies**
```bash
# Solution: Ensure all required libraries are installed
# Check build logs for missing dependencies
```

### Error Analysis
```bash
# Capture detailed error information
./nlm --neurons=500 --steps=1000 2>&1 | tee error_log.txt

# Check specific error patterns
grep -i "error\|warning\|failed" error_log.txt | head -20
```

## 9. Performance Optimization Tips

### For Large Simulations
- Use appropriate timestep values
- Enable checkpointing for long runs
- Monitor system resources during execution

### For Small Simulations
- Use demo mode for quick testing
- Reduce region count when appropriate
- Consider replay and development settings

### Memory Management
- Monitor memory usage during long runs
- Use smaller neuron counts for development testing
- Consider system limitations when scaling up

## 10. Future Enhancements

### Planned Features
- Web interface for experiment control
- Real-time visualization dashboard
- Automated experiment optimization
- Cloud integration for distributed computing
- Advanced analysis tools for neural data

### Research Applications
- Cognitive development studies
- Memory consolidation research
- Neuromodulation experiments
- Social learning simulations
- Adaptation and plasticity studies

# License and Attribution

## NLM Advanced Features License

The advanced features and command-line interfaces described in this document
are part of the NLM Phase 6 project and are licensed under the MIT License.

See LICENSE file for complete terms and conditions.

## Version Information

- Document Version: 1.0
- NLM Version: 0.1.0
- Phase: 6 Final Integration
- Last Updated: $(date)

## Contact and Support

For technical support or feature requests, please refer to the project documentation
and issue tracking systems.

---
*This document is part of the NLM Phase 6 integrated artificial brain project.*
