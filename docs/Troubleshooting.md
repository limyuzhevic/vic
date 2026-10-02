# NLM Troubleshooting Guide

## Overview

The NLM troubleshooting guide provides solutions to common issues encountered during installation, configuration, and operation of the neural simulation framework. It covers error messages, configuration problems, performance issues, and integration challenges.

## Common Issues and Solutions

### 1. Installation Issues

#### CMake Cannot Find Required Components

**Problem:**
```
CMake Error: CMake cannot find package "Python3" required by "pybind11"
```

**Solution:**
```bash
# Install Python development packages
apt-get install python3-dev python3-pip

# Or use conda
conda install python=3.8

# Re-run CMake
cd build
cmake .. -DPython_EXECUTABLE=$(which python3)
```

**Alternative:** Use system Python
```bash
cmake .. -DPython_EXECUTABLE=/usr/bin/python3
```

#### Compilation Errors

**Problem:**
```
error: ‘std::isnan’ is not a member of ‘std’
```

**Solution:**
```bash
# Add required compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-std=c++20 -D_GLIBCXX_USE_CXX11_ABI=1"

# Or update to newer GCC
g++ --version  # Check version
# Use g++ 10+ for proper std::isnan support
```

#### Python Module Import Error

**Problem:**
```
ImportError: No module named 'pynlm'
```

**Solution:**
```bash
# Install from source
pip install .

# Or install with verbose output
pip install -v .

# Check if build dependencies are met
pip install pybind11 scikit-build-core numpy
```

### 2. Configuration Issues

#### Invalid Configuration File

**Problem:**
```
Error: Configuration file has invalid JSON/YAML
```

**Solution:**
```bash
# Use validation script
python scripts/validate_config.py config.json

# Fix the configuration file
# Example invalid JSON:
{"brain": {"neuron_count": 1000}}  # Missing closing brace

# Correct JSON:
{
    "brain": {"neuron_count": 1000}
}
```

**Configuration Validation Scripts:**
```python
#!/usr/bin/env python3
# scripts/validate_config.py

import argparse
import json
import yaml
import sys
from pathlib import Path

def validate_json(filepath):
    with open(filepath, 'r') as f:
        try:
            json.load(f)
            print(f"✓ {filepath} is valid JSON")
            return True
        except json.JSONDecodeError as e:
            print(f"✗ {filepath} is invalid JSON: {e}")
            return False

def validate_yaml(filepath):
    with open(filepath, 'r') as f:
        try:
            yaml.safe_load(f)
            print(f"✓ {filepath} is valid YAML")
            return True
        except yaml.YAMLError as e:
            print(f"✗ {filepath} is invalid YAML: {e}")
            return False

def validate_ini(filepath):
    # Basic INI validation
    with open(filepath, 'r') as f:
        content = f.read()
        has_section = '[' in content and ']' in content
        if has_section:
            print(f"✓ {filepath} is valid INI format")
            return True
        else:
            print(f"✗ {filepath} is not valid INI format")
            return False

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Validate configuration file")
    parser.add_argument("filepath", help="Path to configuration file")
    
    args = parser.parse_args()
    filepath = Path(args.filepath)
    
    if not filepath.exists():
        print(f"✗ File '{filepath}' does not exist")
        sys.exit(1)
    
    suffix = filepath.suffix.lower()
    success = False
    
    if suffix == '.json':
        success = validate_json(filepath)
    elif suffix in ['.yaml', '.yml']:
        success = validate_yaml(filepath)
    elif suffix in ['.cfg', '.ini']:
        success = validate_ini(filepath)
    else:
        print(f"✗ Unsupported file format: {suffix}")
        print("Supported formats: .json, .yaml, .yml, .cfg, .ini")
        sys.exit(1)
    
    sys.exit(0 if success else 1)
```

#### Missing Required Configuration Parameters

**Problem:**
```
Error: Missing required parameter 'brain.neuron_count'
```

**Solution:**
```bash
# Use default configuration
nlm config load default.json

# Or create minimal valid configuration
{
    "brain": {
        "neuron_count": 100,
        "connection_probability": 0.1
    },
    "environment": {
        "name": "GridWorld"
    }
}
```

**Available Configuration Parameters:**
```bash
# List all available parameters
nlm config show | grep -E "^\s*[a-z\.]+"

# Or check schema
python scripts/check_config_schema.py
```

### 3. Runtime Issues

#### Memory Allocation Errors

**Problem:**
```
std::bad_alloc
```

**Solution:**
```bash
# Check system memory
free -h

# Reduce simulation size
nlm config set brain.neuron_count 500

# Use memory-efficient mode
nlm --memory-efficient-mode

# Close other applications
```

#### Simulation Crashes

**Problem:**
```
Segmentation fault (core dumped)
```

**Solution:**
```bash
# Run with debugging
nlm --debug --log-level DEBUG

# Check for memory errors
valgrind --leak-check=full ./nlm_simulation

# Verify configuration
nlm config validate config.json

# Check version compatibility
nlm --version
```

#### Performance Issues

**Problem:**
```
Simulation taking too long or CPU usage too high
```

**Solution:**
```bash
# Monitor performance
nlm --performance-monitor

# Reduce simulation size
nlm config set brain.neuron_count 1000

# Optimize configuration
nlm config set --brain.optimization.enable true

# Check system resources
top -p $(pgrep nlm_simulation)
```

### 4. Integration Issues

#### Agent-Brain Interface Problems

**Problem:**
```
Error: Cannot connect agent to brain
```

**Solution:**
```bash
# Check if brain is initialized
nlm brain status my_brain

# Initialize brain first
nlm brain initialize my_brain

# Verify agent configuration
nlm config show agent

# Create agent properly
nlm agent create my_agent my_brain --world my_world
```

**Common Integration Issues:**
```python
# Issue 1: Agent tries to process sensory input before initialization
world = pynlm.createSimpleWorld()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# ❌ WRONG: Call this after world initialization
world.reset()  # ❌ Wrong order
agent.initialize(world)  # ❌ Wrong order

# ✅ CORRECT: Initialize world first
world.configure(20, 20, 8, 8)
world.setAgentStart(10, 10)
world.reset()  # ✅ Correct order
agent.initialize(world)  # ✅ Correct order
```

#### World-Environment Mismatches

**Problem:**
```
Error: Vision dimensions larger than world dimensions
```

**Solution:**
```bash
# Fix vision configuration
nlm config set environment.vision_width 8
nlm config set environment.vision_height 8

# Or recreate world
nlm world create my_world --vision-width 8 --vision-height 8
```

### 5. Phase-Specific Issues

#### Phase 3 (World Interaction) Issues

**Problem:**
```
Error: Agent not responding to world changes
```

**Solution:**
```bash
# Check world configuration
nlm config show environment

# Verify action system
nlm brain status my_brain

# Enable motor output
nlm brain enable-motor-output
```

#### Phase 4 (Cognitive) Issues

**Problem:**
```
Error: Prediction system not working
```

**Solution:**
```bash
# Check prediction configuration
nlm config show prediction

# Verify prediction system is enabled
nlm brain enable-prediction-system

# Check for required components
nlm brain check-prediction-dependencies
```

#### Phase 6 (Final Integration) Issues

**Problem:**
```
Error: Memory systems not integrated
```

**Solution:**
```bash
# Verify Phase 6 configuration
nlm config show phase6.memory-integration

# Check memory system status
nlm brain status --memory-systems

# Re-enable memory integration
nlm brain enable-memory-integration
```

### 6. Debugging Techniques

#### Enable Debug Logging

**Solution:**
```bash
# Enable verbose debugging
nlm --debug --log-level DEBUG --log-file debug.log

# Or use environment variable
export NLM_LOG_LEVEL=DEBUG
export NLM_LOG_FILE=debug.log
nlm run --debug
```

**Sample Debug Output:**
```
2026-01-15 10:30:45.123 DEBUG [brain] Creating 1000 neurons
2026-01-15 10:30:45.456 INFO  [brain] Brain initialized successfully
2026-01-15 10:30:45.789 DEBUG [agent] Creating agent-brain interface
2026-01-15 10:30:46.012 DEBUG [agent] Connecting to world
2026-01-15 10:30:46.345 INFO  [simulation] Starting simulation
2026-01-15 10:30:46.789 DEBUG [neuron] Neuron 42 fired at t=0.456ms
2026-01-15 10:30:47.123 DEBUG [synapse] Synapses updating: +0.012
```

#### Memory Debugging

**Solution:**
```bash
# Check memory usage
nml --memory-check

# Use valgrind for memory leak detection
valgrind --leak-check=full --show-leak-kinds=all ./nlm_simulation

# Monitor memory with top
watch -n 1 'ps aux | grep nlm_simulation'
```

#### Performance Debugging

**Solution:**
```bash
# Profile with GNU time
/usr/bin/time -v ./nlm_simulation --steps 1000

# Use Intel VTune (if available)
amplxe-cl -collect hotspots --result-dir vtune_results ./nlm_simulation

# Use perf (Linux)
perf record -g ./nlm_simulation
perf report
```

### 7. Specific Error Messages and Solutions

#### Error 1: Configuration File Not Found

**Error:**
```
Error: Configuration file '/path/to/config.json' not found
```

**Solution:**
```bash
# Check if file exists
ls -la /path/to/config.json

# Create from default
nlm config create-default > config.json

# Use absolute path
nlm --config /absolute/path/to/config.json
```

#### Error 2: Invalid Neuron Count

**Error:**
```
Error: Invalid neuron count: 0. Brain requires at least 10 neurons
```

**Solution:**
```bash
# Set minimum valid count
nlm config set brain.neuron_count 10

# Or set reasonable count
nlm config set brain.neuron_count 1000
```

#### Error 3: Connection Probability Out of Range

**Error:**
```
Error: Connection probability must be between 0.0 and 1.0
```

**Solution:**
```bash
# Set valid probability
nlm config set brain.connection_probability 0.1
```

#### Error 4: STDP Learning Rate Too Large

**Error:**
```
Error: STDP learning rate too large. Maximum allowed: 0.01
```

**Solution:**
```bash
# Reduce learning rate
nlm config set plasticity.stdp.learning_rate 0.001

# Or disable STDP
nlm config set plasticity.stdp.enable false
```

#### Error 5: Developmental Stage Not Valid

**Error:**
```
Error: Invalid developmental stage 'InvalidStage'
```

**Solution:**
```bash
# List valid stages
nlm config show development.stages

# Set valid stage
nlm config set development.stage "CriticalPeriod"
```

### 8. Recovery Procedures

#### Recover from Configuration Error

**Procedure:**
```bash
# 1. Backup current configuration
nlm brain save current_brain backup.json

# 2. Restore from default
nlm config load default.json

# 3. Apply custom settings
nlm config set brain.neuron_count 500
nlm config set environment.width 20

# 4. Validate
nlm config validate
```

#### Recover from Simulation Crash

**Procedure:**
```bash
# 1. Check for checkpoint
if [ -f "checkpoint.json" ]; then
    nlm brain load crashed_brain checkpoint.json
fi

# 2. Restart simulation with shorter duration
nlm simulation run crashed_brain --steps 100

# 3. Monitor for issues
nml --log-file crash_analysis.log
```

#### Recover from Memory Error

**Procedure:**
```bash
# 1. Check system memory
free -h

# 2. Kill NLM processes if necessary
pkill -f nlm_simulation

# 3. Free up memory
# Close other applications
# Clear browser cache
# Use swap if available

# 4. Restart with reduced memory usage
nlm --memory-efficient-mode --neurons 500
```

### 9. Automation Scripts

#### Automated Recovery Script
```bash
#!/bin/bash
# scripts/auto_recover.sh

SCRIPT_NAME="NLM Auto Recovery"
LOG_FILE="auto_recover.log"
BACKUP_DIR="backups"

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG_FILE"
}

backup_and_recover() {
    log "Starting auto-recovery process..."
    
    # Create backup directory
    mkdir -p "$BACKUP_DIR"
    
    # Backup current state
    BACKUP_FILE="$BACKUP_DIR/backup_$(date +%Y%m%d_%H%M%S).json"
    nlm brain save current_brain "$BACKUP_FILE" 2>> "$LOG_FILE"
    
    # Try to validate configuration
    if ! nlm config validate 2>> "$LOG_FILE"; then
        log "Configuration validation failed. Restoring defaults..."
        nlm config load default.json 2>> "$LOG_FILE"
    fi
    
    # Try to restart with reduced settings
    if ! nlm simulation run current_brain --steps 100 2>> "$LOG_FILE"; then
        log "Simulation failed. Trying minimal configuration..."
        nlm config set brain.neuron_count 10
        nlm config set environment.width 5
        nlm config set environment.height 5
        
        # Try again with minimal settings
        if ! nlm simulation run current_brain --steps 50 2>> "$LOG_FILE"; then
            log "Recovery failed. Manual intervention required."
            exit 1
        fi
    fi
    
    log "Auto-recovery completed successfully"
}

# Run recovery
backup_and_recover
```

#### Health Check Script
```bash
#!/bin/bash
# scripts/health_check.sh

SCRIPT_NAME="NLM Health Check"
LOG_FILE="health_check.log"

health_check() {
    echo "Starting NLM health check..." | tee "$LOG_FILE"
    
    # Check if NLM processes are running
    if ps aux | grep -v grep | grep -q nlm_simulation; then
        echo "✓ NLM simulation is running" | tee -a "$LOG_FILE"
    else
        echo "✗ NLM simulation is not running" | tee -a "$LOG_FILE"
        return 1
    fi
    
    # Check memory usage
    MEMORY_USAGE=$(free -m | grep '^Mem:' | awk '{print $3/$2 * 100.0}')
    if (( $(echo "$MEMORY_USAGE > 90" | bc -l) )); then
        echo "✗ Memory usage too high: ${MEMORY_USAGE}%" | tee -a "$LOG_FILE"
        return 1
    else
        echo "✓ Memory usage normal: ${MEMORY_USAGE}%" | tee -a "$LOG_FILE"
    fi
    
    # Check disk space
    DISK_USAGE=$(df . | tail -1 | awk '{print $5}' | sed 's/%//')
    if [ "$DISK_USAGE" -gt 90 ]; then
        echo "✗ Disk usage too high: ${DISK_USAGE}%" | tee -a "$LOG_FILE"
        return 1
    else
        echo "✓ Disk usage normal: ${DISK_USAGE}%" | tee -a "$LOG_FILE"
    fi
    
    # Check configuration
    if nlm config validate 2>> "$LOG_FILE"; then
        echo "✓ Configuration is valid" | tee -a "$LOG_FILE"
    else
        echo "✗ Configuration is invalid" | tee -a "$LOG_FILE"
        return 1
    fi
    
    echo "Health check completed successfully" | tee -a "$LOG_FILE"
    return 0
}

# Run health check
health_check
exit $?
```

## Related Documentation

- See [ARCHITECTURE.md](ARCHITECTURE.md) for system design
- See [Configuration.md](Configuration.md) for configuration options
- See [API-Guide.md](API-Guide.md) for programmatic interface
- See [Development-Guide.md](Development-Guide.md) for development practices
- See [Performance-Guide.md](Performance-Guide.md) for performance optimization

## Troubleshooting Resources

### Online Resources

- **NLM Issue Tracker**: [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- **Community Forum**: [Discourse](https://forum.nlm-project.org)
- **Slack Channel**: #nlm-support
- **Documentation**: [NLM Docs](https://docs.nlm-project.org)
- **API Reference**: [Doxygen](docs/api/html/index.html)

### Support Contact

**For Troubleshooting Assistance:**
- **Email**: support@nlm-project.org
- **Slack**: #troubleshooting-channel
- **GitHub**: [Submit Issue](https://github.com/nlm-project/nlm/issues/new)

**For Development Questions:**
- **Email**: dev-support@nlm-project.org
- **GitHub**: [Development Forum](https://github.com/nlm-project/nlm/discussions)

### Troubleshooting Tools

**Command Line Tools:**
```bash
# Check NLM version
nlm --version

# List all available commands
nlm --help

# Show help for specific command
nlm <command> --help

# Check configuration
nlm config show

# Validate configuration
nlm config validate

# Check system status
nlm system status
```

**Python Tools:**
```python
import pynlm

# Create debug brain
config = pynlm.create_debug_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run with debugging
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.get_firing_neuron_count()} firing")
```

## End of Troubleshooting Guide

This troubleshooting guide provides solutions to common NLM issues. For additional assistance, please:

1. **Check the documentation** for detailed solutions
2. **Review error messages** carefully for root causes
3. **Test with smaller configurations** to isolate issues
4. **Use debugging tools** to gather detailed information
5. **Contact support** for unresolved issues

_If you encounter issues not covered in this guide, please contribute to its improvement by submitting additional troubleshooting tips and solutions._