# NLM Migration Guide

## Overview

This migration guide provides comprehensive information for users transitioning from previous versions of NLM to the latest release. It covers breaking changes, new features, migration strategies, and best practices for upgrading.

## Migration Overview

NLM has undergone significant improvements in recent versions, including:

### 1. Configuration System Refactoring
- **Before**: Simple key-value configuration system
- **After**: Type-safe, schema-based configuration with validation
- **Impact**: Breaking changes to configuration API

### 2. Enhanced Python Bindings
- **Before**: Basic Python bindings
- **After**: Comprehensive Python API with advanced features
- **Impact**: New API patterns and features

### 3. New Logging System
- **Before**: Basic logging
- **After**: Modular, component-based logging system
- **Impact**: New logging API and features

### 4. Batch Processing
- **Before**: Single-step processing only
- **After**: Efficient batch processing with memory optimization
- **Impact**: New performance features

### 5. Performance Profiling
- **Before**: Basic performance monitoring
- **After**: Comprehensive performance profiling tools
- **Impact**: New performance analysis capabilities

## Migration Path

### Phase 1: Assess Current Setup

#### Check Your Current Configuration
```bash
# Check if you're using the old configuration system
if [ -f "config.cfg" ]; then
    echo "Found legacy config file: config.cfg"
fi

# Check if you're using old Python API
grep -r "pynlm" your_project/ | grep -v "docs/" | grep -v "tests/"
```

#### Identify Dependencies
```python
# Check for old imports
import old_nlm  # Not available
from nlm import Brain  # Old import
import nlm.config  # Old module
```

#### Review Your Code
```bash
# Search for old API usage
grep -r "createBrain(" your_project/ --include="*.py"
grep -r "set(" your_project/ --include="*.py" | grep -v "config.set"
grep -r "get(" your_project/ --include="*.py" | grep -v "config.get"
```

### Phase 2: Plan Migration

#### 1. Configuration Migration

**Old Configuration API:**
```python
# Old way (no longer supported)
config = OldConfig()
config.setString("neuron_count", 1000)
config.setDouble("learning_rate", 0.01)
config.setBool("enable_curiosity", True)

neuron_count = config.getString("neuron_count")
```

**New Configuration API:**
```python
# New way (recommended)
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("learning_rate", 0.01)
config.set("enable_curiosity", True)

# Get values
neuron_count = config.getInt("neuron_count")
```

#### 2. Brain Class Migration

**Old Brain API:**
```python
# Old way (deprecated)
brain = OldBrain()
brain.initialize()

for step in range(1000):
    brain.step(step)

firing_neurons = brain.getFiringNeuronCount()
```

**New Brain API:**
```python
# New way (recommended)
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
firing_neurons = brain.getFiringNeuronCount()
```

#### 3. Agent Migration

**Old Agent API:**
```python
# Old way (deprecated)
agent = OldAgent(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

action = agent.decodeMotorCommand()
```

**New Agent API:**
```python
# New way (recommended)
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Get action
action = agent.decodeMotorCommand()
```

#### 4. World Migration

**Old World API:**
```python
# Old way (deprecated)
world = OldWorld()
world.configure(20, 20)
world.reset()
world.update(0.1)

percept = world.getSensoryPercept()
world.applyMotorCommand(action, time)
```

**New World API:**
```python
# New way (recommended)
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.update(0.1)

# Get percept and apply action
percept = world.getSensoryPercept()
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

### Phase 3: Implementation

#### 3.1 Configuration Migration

**Step 1: Update Configuration Files**
```python
# Old config file (config.cfg)
# format: key = value
neuron_count = 1000
learning_rate = 0.01
enable_curiosity = true

# New config file (config.json)
{
  "neuron_count": 1000,
  "learning_rate": 0.01,
  "enable_curiosity": true
}
```

**Step 2: Update Configuration Loading**
```python
# Old way
config = OldConfig()
config.loadFromFile("config.cfg")

# New way
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
```

#### 3.2 Brain Migration

**Step 1: Update Brain Creation**
```python
# Old way
brain = OldBrain()
brain.initialize()

# New way
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("learning_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()
```

**Step 2: Update Brain Usage**
```python
# Old way
for step in range(1000):
    brain.step(step)

# New way
for step in range(1000):
    brain.step(step)
```

#### 3.3 Agent Migration

**Step 1: Update Agent Creation**
```python
# Old way
agent = OldAgent(brain)
agent.initialize(world)

# New way
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**Step 2: Update Agent Configuration**
```python
# Old way
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# New way
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

#### 3.4 World Migration

**Step 1: Update World Creation**
```python
# Old way
world = OldWorld()
world.configure(20, 20)

# New way
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

**Step 2: Update World Usage**
```python
# Old way
world.update(0.1)
percept = world.getSensoryPercept()
world.applyMotorCommand(action, time)

# New way
world.update(0.1)
percept = world.getSensoryPercept()
world.applyMotorCommand(action, world.getSimulationTime())
```

### Phase 4: Testing and Validation

#### 4.1 Unit Testing

**Update Unit Tests for New API**
```python
import unittest
import pynlm

class TestNMMigration(unittest.TestCase):
    def setUp(self):
        """Set up test."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 100)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up test."""
        del self.agent, self.brain, self.world
    
    def testBrainCreation(self):
        """Test brain creation."""
        self.assertIsNotNone(self.brain)
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
    
    def testAgentCreation(self):
        """Test agent creation."""
        self.assertIsNotNone(self.agent)
    
    def testSimulation(self):
        """Test simulation."""
        for step in range(10):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Verify simulation completed
        self.assertTrue(self.world.getSimulationTime() > 0)
```

#### 4.2 Integration Testing

**Update Integration Tests**
```bash
# Run updated tests
python -m pytest tests/ -v

# Check for any test failures
python -m pytest tests/ --tb=short
```

#### 4.3 Performance Testing

**Update Performance Tests**
```python
import pynlm
import time

def testPerformance():
    """Test performance with new API."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(1000):
        brain.step(step)
    end_time = time.time()
    
    print(f"Performance: {1000/(end_time - start_time):.0f} steps/second")
```

### Phase 5: Optimization

#### 5.1 Performance Optimization

**Use New Batch Processing Features**
```python
import pynlm
from performance import BatchBrain

# Create batch brain with performance optimizations
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

batch_brain = BatchBrain(config)
batch_brain.enableSIMD(true)
batch_brain.enableMultithreading(true)
batch_brain.enableMemoryPool(true)

# Process batch efficiently
batch_brain.processBatch(0, 9999)
```

**Use New Performance Profiling**
```python
import pynlm
from performance import PerformanceProfiler

# Profile performance
profiler = PerformanceProfiler("simulation_profile")
profiler.setup()

# Run simulation
runSimulation()

profiler.teardown()
report = profiler.generateReport()

# Analyze report
analyzePerformanceReport(report)
```

#### 5.2 Memory Optimization

**Use Memory-Efficient Configuration**
```python
import pynlm

# Create memory-efficient brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("brain.synapse_density", 0.05)  # Lower synapse density

brain = pynlm.createBrain(config)
brain.initialize()
```

**Use Memory Pooling**
```python
import pynlm
from performance import MemoryPool

# Use memory pool for efficient memory management
memory_pool = MemoryPool()
memory_pool.initialize()

# Create brain with memory pool
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config, memory_pool=memory_pool)
brain.initialize()
```

### Phase 6: Deployment

#### 6.1 Production Deployment

**Deploy with New Configuration**
```bash
# Deploy with new configuration system
./nlm --config config.json

# Deploy with new logging system
./nlm --logging-config logging_config.json

# Deploy with new performance monitoring
./nlm --performance-profile performance_config.json
```

**Deploy with New Python Bindings**
```python
# Deploy with new Python bindings
import pynlm

# Create and run simulation
simulation = pynlm.createSimulation(
    config=pynlm.createDefaultConfig(),
    world=pynlm.createSimpleWorld(),
    agent=pynlm.createAgentBrain(brain)
)

simulation.run()
```

#### 6.2 Monitoring and Maintenance

**Set Up Monitoring**
```python
import pynlm
from performance import PerformanceMonitor

# Set up performance monitoring
monitor = PerformanceMonitor("production_simulation")
monitor.startMonitoring()

# Run simulation with monitoring
runProductionSimulation()

# Stop monitoring
monitor.stopMonitoring()

# Generate report
report = monitor.generateReport()
monitor.saveReport("production_report.html")
```

## Common Migration Issues and Solutions

### 1. Configuration Validation Errors

**Issue**: Configuration validation fails with new schema.

**Solution**:
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()

# Try different configuration options
config.set("neuron_count", 100)  # Smaller value
config.set("learning_rate", 0.01)  # Valid range
config.set("enable_curiosity", True)  # Valid boolean

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Type Mismatch Errors

**Issue**: Type mismatch when getting configuration values.

**Solution**:
```python
import pynlm

config = pynlm.createDefaultConfig()

# Use correct type getter
neuron_count = config.getInt("neuron_count")  # Instead of getString
learning_rate = config.getDouble("learning_rate")  # Instead of getFloat
enable_curiosity = config.getBool("enable_curiosity")  # Instead of getBoolean
```

### 3. Missing Configuration Keys

**Issue**: Required configuration keys are missing.

**Solution**:
```python
import pynlm

config = pynlm.createDefaultConfig()

# Set all required keys
config.set("neuron_count", 1000)  # Required
config.set("learning_rate", 0.01)  # Required
config.set("enable_curiosity", True)  # Required

# Optional keys have defaults
config.set("synapse_density", 0.1)  # Optional
```

### 4. Performance Issues

**Issue**: Performance degradation after migration.

**Solution**:
```python
import pynlm
from performance import BatchBrain

# Use batch processing for better performance
config = pynlm.createDefaultConfig()
batch_brain = BatchBrain(config)

# Enable performance optimizations
batch_brain.enableSIMD(true)
batch_brain.enableMultithreading(true)
batch_brain.enableMemoryPool(true)

# Process batch efficiently
batch_brain.processBatch(0, 9999)
```

### 5. Memory Issues

**Issue**: Memory usage increases after migration.

**Solution**:
```python
import pynlm

# Use memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)  # Smaller brain
config.set("brain.synapse_density", 0.05)  # Lower synapse density

brain = pynlm.createBrain(config)
brain.initialize()
```

### 6. Logging Issues

**Issue**: Logging not working after migration.

**Solution**:
```python
import pynlm

# Enable logging
config = pynlm.createDefaultConfig()
config.set("logging.level", "INFO")

brain = pynlm.createBrain(config)
brain.initialize()

# Check if logging is working
if brain.getLogger():
    print("Logging is working!")
else:
    print("Logging is not working.")
```

## Migration Checklist

### Pre-Migration Checklist
- [ ] Assess current codebase for old API usage
- [ ] Identify all configuration files
- [ ] Create migration plan
- [ ] Test migration in development environment
- [ ] Update documentation
- [ ] Train team on new API

### Migration Checklist
- [ ] Update configuration files
- [ ] Update Brain class usage
- [ ] Update Agent class usage
- [ ] Update World class usage
- [ ] Run unit tests
- [ ] Run integration tests
- [ ] Run performance tests
- [ ] Update documentation
- [ ] Train end users

### Post-Migration Checklist
- [ ] Verify all functionality works
- [ ] Check performance metrics
- [ ] Monitor memory usage
- [ ] Validate configuration
- [ ] Update monitoring and alerting
- [ ] Document migration process

## Version-Specific Migration

### Migration from Version 5.x to 6.0

**Breaking Changes in 6.0**:
1. Configuration system refactored (type-safe API)
2. Python bindings enhanced (new API)
3. Logging system modularized (new API)
4. Batch processing added (new features)
5. Performance profiling enhanced (new tools)

**Migration Steps**:
```bash
# 1. Update configuration files
# Old: config.cfg (key = value format)
# New: config.json (JSON format)

# 2. Update Python code
# Old: import nlm; nlm.createBrain()
# New: import pynlm; pynlm.createBrain()

# 3. Update C++ code
# Old: Brain brain(); brain.initialize()
# New: auto brain = nlm::createBrain(); brain->initialize()

# 4. Update documentation
# Update all documentation to reflect new API
```

### Migration from Version 4.x to 5.x

**Breaking Changes in 5.0**:
1. Configuration system refactored
2. Python bindings enhanced
3. Logging system added

**Migration Steps**:
```bash
# 1. Update configuration system
# Old: Simple key-value store
# New: Type-safe schema-based system

# 2. Update Python bindings
# Old: Basic Python bindings
# New: Comprehensive Python API

# 3. Update logging
# Old: Basic logging
# New: Modular logging system
```

## Support and Resources

### 1. Documentation
- **Documentation**: docs/ (all documentation files)
- **API Reference**: docs/PYTHON_BINDINGS.md
- **Development Guide**: docs/COMPREHENSIVE_DEVELOPMENT_GUIDE.md
- **Getting Started**: docs/GETTING_STARTED.md

### 2. Community
- **GitHub**: https://github.com/your-org/nlm
- **Issues**: https://github.com/your-org/nlm/issues
- **Discussions**: https://github.com/your-org/nlm/discussions
- **Slack/Discord**: Community channels

### 3. Support
- **Email**: support@your-org.com
- **Phone**: +1-800-NLM-HELP
- **Live Chat**: Available during business hours

### 4. Training
- **Webinars**: Regular webinars on NLM features
- **Tutorials**: Online tutorials and examples
- **Workshops**: In-person workshops
- **Documentation**: Comprehensive documentation

## Conclusion

Migration from older versions of NLM to the latest version requires careful planning and execution. This guide provides comprehensive information for migrating your codebase, ensuring a smooth transition to the new features and improvements.

Key points:
1. **Plan ahead**: Assess current setup and create migration plan
2. **Test thoroughly**: Run tests in development environment
3. **Document changes**: Update all documentation and comments
4. **Train team**: Ensure team understands new API
5. **Monitor performance**: Track performance after migration
6. **Provide support**: Have support channels ready for users

By following this migration guide, you can successfully upgrade to the latest version of NLM and take advantage of all new features and improvements.
