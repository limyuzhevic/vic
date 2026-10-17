# NLM — Troubleshooting Guide

This document provides comprehensive troubleshooting guide for the NLM codebase, covering common issues, their solutions, and diagnostic procedures.

---

## Overview

This troubleshooting guide provides solutions for common issues encountered when using the NLM codebase. It covers installation, configuration, simulation, and performance issues.

## Common Issues and Solutions

### 1. Installation Issues

#### Issue: Build Fails

**Symptoms:**
- CMake configuration fails
- Compiler errors
- Missing dependencies

**Solutions:**
```bash
# Check CMake version
cmake --version

# Install dependencies
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git python3-pip

# Install Python dependencies
pip install scikit-build-core pybind11 pytest numpy

# Rebuild
rm -rf build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Error Code:** Compilation error

#### Issue: Python Bindings Not Found

**Symptoms:**
- ImportError: No module named 'pynlm'
- Python module not found

**Solutions:**
```bash
# Install Python bindings
pip install --force-reinstall .

# Install in development mode
pip install -e .

# Check installation
python -c "import pynlm; print(pynlm.__version__)"
```

**Error Code:** ImportError

#### Issue: Missing Dependencies

**Symptoms:**
- Missing shared libraries
- Missing Python packages
- Missing system dependencies

**Solutions:**
```bash
# Check for missing libraries
ldd ./nlm

# Install system dependencies
# Ubuntu/Debian
sudo apt-get install libpython3-dev libblas-dev liblapack-dev

# Install Python dependencies
pip install numpy scipy matplotlib
```

**Error Code:** MissingDependency

### 2. Configuration Issues

#### Issue: Configuration File Not Found

**Symptoms:**
- Error message: "Configuration file not found: config.json"
- Application fails to start
- Default configuration used

**Solutions:**
```bash
# Create the configuration file
cat > config.json << EOF
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  }
}
EOF

# Use default configuration
./nlm --preset=basic

# Use environment variables
export NLM_brain_neuron_count=1000
export NLM_brain_synapse_density=0.1
./nlm --no-env
```

**Error Code:** FileError

#### Issue: Invalid Configuration Values

**Symptoms:**
- Error message: "Invalid configuration value for key: brain.neuron_count"
- Configuration validation fails
- Application crashes

**Solutions:**
```bash
# Check configuration schema
./nlm --validate-config --schema=production_schema.json

# Fix the configuration value
cat > config.json << EOF
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  }
}
EOF

# Or use a preset that has valid values
./nlm --preset=production
```

**Error Code:** ValidationError

#### Issue: Configuration Validation Fails

**Symptoms:**
- Error message: "Configuration validation failed"
- Schema validation errors
- Type validation errors

**Solutions:**
```bash
# Check specific validation errors
./nlm --validate-config --config=problematic_config.json

# Fix configuration based on error messages
# Check schema documentation
# Fix invalid values
# Add missing required keys
```

**Error Code:** ValidationError

#### Issue: File Permission Error

**Symptoms:**
- Error message: "Permission denied: config.json"
- Cannot read/write configuration files
- File access denied

**Solutions:**
```bash
# Check file permissions
ls -la config.json

# Fix permissions
chmod 644 config.json

# Use a different directory
./nlm --config=./configs/config.json

# Use environment variables instead
export NLM_config_content="neuron_count=1000"
```

**Error Code:** FileError

### 3. Simulation Issues

#### Issue: Brain Initialization Fails

**Symptoms:**
- Error message: "Failed to initialize brain"
- Brain doesn't fire any neurons
- Memory allocation errors

**Solutions:**
```bash
# Check configuration
./nlm --validate-config --config=problematic_config.json

# Use a known good preset
./nlm --preset=basic

# Check system resources
free -h
df -h

# Check logs for specific error messages
./nlm --info --verbose
```

**Error Code:** InitializationError

#### Issue: Agent Not Learning

**Symptoms:**
- Low curiosity level
- No reward modulation
- Static behavior

**Solutions:**
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Check learning statistics
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity: {agent.getCuriosityLevel()}")
        print(f"  Novelty: {agent.getNoveltyLevel()}")
        print(f"  Prediction error: {agent.getPredictionError()}")
```

**Error Code:** LearningError

#### Issue: Simulation Runs Slowly

**Symptoms:**
- Slow simulation performance
- Low firing rates
- High step times
- Memory usage high

**Solutions:**
```bash
# Use production preset for better performance
./nlm --preset=production

# Enable performance optimizations
./nlm --config=optimized_config.json

# Monitor performance
./nlm --info --performance

# Check configuration bottlenecks
cat performance_report.json
```

**Error Code:** PerformanceError

#### Issue: Memory Usage Too High

**Symptoms:**
- High memory consumption
- Possible segmentation fault
- Slow simulation performance

**Solutions:**
```bash
# Reduce configuration complexity
./nlm --preset=basic

# Enable performance optimizations
./nlm --config=performance_config.json

# Monitor memory usage
./nlm --info --memory

# Use smaller configuration
cat > config.json << EOF
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  }
}
EOF
```

**Error Code:** MemoryError

#### Issue: Phase 6 Demo Fails

**Symptoms:**
- Phase 6 demo fails to complete
- Missing systems or connections
- Integration errors

**Solutions:**
```bash
# Check Phase 6 specific logs
./nlm --run-phase-6 --verbose

# Use individual system tests
./nlm --run-system-tests

# Check system connections
./nlm --check-connections

# Verify all systems are properly initialized
./nlm --validate-all-systems
```

**Error Code:** IntegrationError

### 4. Performance Issues

#### Issue: Simulation Time Too Long

**Symptoms:**
- Simulation takes too long
- Low performance
- High CPU usage

**Solutions:**
```bash
# Use production preset
./nlm --preset=production

# Enable performance optimizations
./nlm --config=performance_config.json

# Monitor performance
./nlm --info --performance

# Check system resources
./nlm --monitor --resources
```

**Error Code:** PerformanceError

#### Issue: Memory Usage Too High

**Symptoms:**
- High memory consumption
- Possible segmentation fault
- Slow simulation performance

**Solutions:**
```bash
# Reduce configuration complexity
./nlm --preset=basic

# Enable memory optimization
./nlm --config=optimized_config.json

# Monitor memory usage
./nlm --info --memory

# Use smaller configuration
cat > config.json << EOF
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1
  }
}
EOF
```

**Error Code:** MemoryError

#### Issue: CPU Usage Too High

**Symptoms:**
- High CPU usage
- System slow
- Simulation slow

**Solutions:**
```bash
# Use performance preset
./nlm --preset=production

# Enable parallel processing
./nlm --config=performance_config.json

# Monitor CPU usage
./nlm --monitor --cpu

# Check system resources
./nlm --monitor --resources
```

**Error Code:** PerformanceError

### 5. Agent Issues

#### Issue: Agent Not Moving

**Symptoms:**
- Agent doesn't move
- Static position
- No actions taken

**Solutions:**
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

# Enable movement
agent.enableMovement(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Print agent position
    if step % 20 == 0:
        pos = world.getAgentBody()
        print(f"Step {step}: Agent at ({pos.getX()}, {pos.getY()})")
```

**Error Code:** AgentError

#### Issue: Agent Not Learning from Rewards

**Symptoms:**
- Low reward modulation
- No behavior change
- Static performance

**Solutions:**
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

# Enable reward modulation
agent.enableRewardModulation(True)

# Enable curiosity
agent.enableCuriosity(True)

# Enable development
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Get reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    
    # Apply reward modulation
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print learning statistics
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Reward: {reward}")
        print(f"  Curiosity: {agent.getCuriosityLevel()}")
        print(f"  Novelty: {agent.getNoveltyLevel()}")
        print(f"  Prediction error: {agent.getPredictionError()}")
```

**Error Code:** LearningError

### 6. CLI Issues

#### Issue: Command Not Found

**Symptoms:**
- Command not found error
- Help not showing
- Invalid command

**Solutions:**
```bash
# Display available commands
./nlm --help

# List all available subcommands
./nlm --run-phase-6 --help
./nlm --validate-config --help
./nlm --export-config --help
./nlm --info --help
./nlm --compare-configs --help
./nlm --list-presets --help
./nlm --list-templates --help
```

**Error Code:** CommandError

#### Issue: Invalid Command Arguments

**Symptoms:**
- Invalid command arguments
- Missing required arguments
- Invalid argument values

**Solutions:**
```bash
# Check help for valid arguments
./nlm --run-phase-6 --help

# Use valid arguments
./nlm --run-phase-6 --config=config.json
./nlm --validate-config --config=production.cfg
./nlm --export-config --format=json --file=output.json
```

**Error Code:** ArgumentError

#### Issue: Configuration Commands Fail

**Symptoms:**
- Export configuration fails
- Validate configuration fails
- List presets fails

**Solutions:**
```bash
# Check configuration system
./nlm --validate-config

# List presets
./nlm --list-presets

# List templates
./nlm --list-templates

# Check system status
./nlm --check-status
```

**Error Code:** ConfigError

### 7. File System Issues

#### Issue: Checkpoint File Not Found

**Symptoms:**
- Error message: "Checkpoint file not found"
- Cannot load checkpoint
- Simulation interruption

**Solutions:**
```bash
# Check checkpoint files
ls -la checkpoint*.bin

# Create checkpoint
cat > create_checkpoint.py << EOF
import pynlm
import sys

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few steps
for i in range(100):
    brain.step(i)

# Save checkpoint
brain.save("checkpoint_step_100.bin")
print("Checkpoint created")
EOF

python create_checkpoint.py
```

**Error Code:** FileError

#### Issue: Backup File Not Found

**Symptoms:**
- Error message: "Backup file not found"
- Cannot restore from backup
- Configuration loss

**Solutions:**
```bash
# Check backup files
ls -la backup*.json

# Create backup
cp config.json backup_config.json
```

**Error Code:** FileError

#### Issue: Log File Not Found

**Symptoms:**
- Error message: "Log file not found"
- Cannot read logs
- No error logging

**Solutions:**
```bash
# Check log files
ls -la log*.txt

# Create log file
touch debug.log
```

**Error Code:** FileError

---

## Error Reference

### Common Error Codes

| Error Type | Error Code | Description |
|------------|------------|-------------|
| `InitializationError` | `INIT_BRAIN_FAILED` | Failed to initialize brain |
| `InitializationError` | `INIT_WORLD_FAILED` | Failed to initialize world |
| `FileError` | `FILE_NOT_FOUND` | Configuration file not found |
| `ValidationError` | `VALIDATION_FAILED` | Configuration validation failed |
| `ConfigError` | `CONFIG_KEY_NOT_FOUND` | Configuration key not found |
| `ConfigError` | `CONFIG_INVALID_TYPE` | Invalid configuration value type |
| `LearningError` | `LEARNING_NOT_ENABLED` | Learning feature not enabled |
| `PerformanceError` | `PERFORMANCE_THRESHOLD_EXCEEDED` | Performance threshold exceeded |
| `MemoryError` | `MEMORY_EXHAUSTED` | Memory exhausted |
| `AgentError` | `AGENT_NOT_INITIALIZED` | Agent not initialized |
| `CommandError` | `COMMAND_NOT_FOUND` | Command not found |

### Error Resolution Steps

1. **Identify the Error:**
   - Check error messages for specific error codes
   - Look for stack traces in logs
   - Note the exact error type and message

2. **Find the Cause:**
   - Check configuration files
   - Verify file permissions
   - Check system resources

3. **Apply the Solution:**
   - Fix configuration files
   - Adjust permissions
   - Monitor system resources

4. **Test the Fix:**
   - Run the application again
   - Check for additional errors
   - Monitor performance

## System Requirements

### Minimum Requirements

- **CPU:** Quad-core processor (8+ cores recommended)
- **RAM:** 8 GB minimum (16 GB+ recommended)
- **Storage:** 1 GB free space
- **Operating System:** Linux, macOS, or Windows 10+

### Performance Requirements

- **Memory:** 2-4 GB for basic simulations
- **Processing:** 1+ CPU cores for complex simulations
- **Disk I/O:** Fast storage for checkpoint operations

## Dependency Information

### Python Dependencies

```python
import pynlm

# All dependencies are bundled with pynlm
# No additional Python packages required
# Dependencies include:
# - numpy (for numerical computations)
# - pybind11 (for Python bindings)
```

### System Dependencies

- **C++ Compiler:** GCC 10+, Clang 12+, or MSVC 2019+
- **CMake:** Version 3.16 or higher
- **Python:** 3.8+ (for Python bindings)
- **pybind11:** Version 2.11.0+ (for Python bindings)

## Monitoring and Diagnostics

### Monitoring Commands

```bash
# Monitor system resources
./nlm --monitor --resources

# Monitor simulation performance
./nlm --monitor --performance

# Monitor memory usage
./nlm --monitor --memory

# Monitor file system usage
./nlm --monitor --disk
```

### Diagnostic Commands

```bash
# Check system configuration
./nlm --check-config

# Check system connections
./nlm --check-connections

# Check system status
./nlm --check-status

# Check system health
./nlm --check-health
```

## Support and Resources

### Getting Help

1. **Documentation:**
   - This troubleshooting guide
   - User guide (NLMUserGuide.md)
   - API documentation (NLMApiReference.md)
   - Configuration documentation (NLMConfigurationReference.md)
   - Examples (NLMExamples.md)
   - Migration guide (NLMMigrationGuide.md)
   - Best practices (NLMBestPractices.md)

2. **Community Resources:**
   - GitHub repository: https://github.com/nlm-project/nlm
   - Discussion forums: Available on GitHub
   - Issue tracker: Available on GitHub

3. **Technical Support:**
   - Contact the development team
   - Available for enterprise customers

### Support Channels

- **Bug Reports:** Submit issues on GitHub
- **Feature Requests:** Submit issues on GitHub
- **Technical Support:** Available through commercial support
- **Documentation Updates:** Available on GitHub

### Emergency Support

If you encounter critical issues:

1. **Immediate Actions:**
   - Check documentation
   - Review error messages
   - Check system resources
   - Verify configuration files

2. **Contact Support:**
   - Use GitHub issue tracker
   - Contact development team directly
   - Use commercial support channels

3. **Documentation:**
   - This troubleshooting guide
   - All other documentation files

---

## References

1. **NLMUserGuide.md** - User guide and tutorials
2. **NLMApiReference.md** - API documentation
3. **NLMConfigurationReference.md** - Configuration documentation
4. **NLMExamples.md** - Additional examples
5. **NLMMigrationGuide.md** - Migration guide
6. **NLMBestPractices.md** - Best practices

---

*Last Updated: Phase 6 Completion*
*Version: 1.0.0*