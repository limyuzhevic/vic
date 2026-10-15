# NLM - Debugging and Troubleshooting Guide

## Overview

This comprehensive guide provides troubleshooting strategies and debugging techniques for NLM (Neural Learning Machine) research and simulations. It covers common issues, debugging approaches, and resolution strategies for the enhanced Python API and NLM framework.

## Debugging Philosophy

### Core Principles

Effective debugging in NLM follows these key principles:

1. **Systematic Approach**: Follow a logical, step-by-step debugging process
2. **Reproducibility**: Ensure issues can be reproduced consistently
3. **Isolation**: Isolate problems to specific components or modules
4. **Documentation**: Document issues, solutions, and lessons learned
5. **Prevention**: Implement preventive measures to avoid future issues

### Debugging Process

```
┌─────────────────────────────────────────────────────────────────┐
│                    DEBUGGING PROCESS                             │
├─────────────────────────────────────────────────────────────────┤
│ 1. PROBLEM IDENTIFICATION                                         │
│    ├── Define the issue clearly                                   │
│    ├── Document symptoms                                           │
│    ├── Identify when it occurs                                    │
│    └── Gather relevant information                                 │
├─────────────────────────────────────────────────────────────────┤
│ 2. PROBLEM ISOLATION                                             │
│    ├── Reproduce the issue                                        │
│    ├── Test hypotheses                                            │
│    ├── Isolate affected components                                 │
│    └── Determine scope of the problem                             │
├─────────────────────────────────────────────────────────────────┤
│ 3. ROOT CAUSE ANALYSIS                                          │
│    ├── Examine code and logs                                      │
│    ├── Check configuration settings                                │
│    ├── Review environmental factors                               │
│    └── Test specific hypotheses                                    │
├─────────────────────────────────────────────────────────────────┤
│ 4. SOLUTION IMPLEMENTATION                                      │
│    ├── Develop and test fixes                                     │
│    ├── Validate the solution                                      │
│    └── Document the resolution                                     │
├─────────────────────────────────────────────────────────────────┤
│ 5. VERIFICATION AND PREVENTION                                  │
│    ├── Test thoroughly                                           │
│    ├── Implement preventive measures                             │
│    ├── Update documentation                                       │
│    └── Monitor for recurrence                                     │
└─────────────────────────────────────────────────────────────────┘
```

## Common Debugging Issues

### 1. Basic Runtime Errors

**Symptoms**:
- Import errors
- Type errors
- Attribute errors
- Name errors

**Typical Solutions**:
```python
# Import errors
try:
    import pynlm
    print("✓ NLM imported successfully")
except ImportError as e:
    print(f"✗ NLM import failed: {e}")
    print("Install with: pip install pynlm")

# Type errors
try:
    config = pynlm.createResearchConfig()
    config.set("brain.neuron_count", "1000")  # Wrong type
    print("Config set successfully")
except Exception as e:
    print(f"Type error: {e}")
    config.set("brain.neuron_count", 1000)  # Correct type

# Attribute errors
try:
    brain = pynlm.createBrain(config)
    brain.initialize()  # Auto-initialized in new API
    print("Brain initialized")
except AttributeError as e:
    print(f"Attribute error: {e}")
    print("Use context manager: with pynlm.createBrain(config) as brain:")
```

### 2. Configuration Issues

**Symptoms**:
- Configuration validation failures
- Missing required parameters
- Invalid parameter values
- Configuration loading errors

**Typical Solutions**:
```python
import pynlm

def debug_configuration(config):
    """Debug configuration issues."""
    
    print("Configuration Debug Report")
    print("=" * 40)
    
    # Check for required keys
    required_keys = ["brain.neuron_count", "brain.synapse_density"]
    
    print("\nRequired keys check:")
    for key in required_keys:
        if config.has(key):
            value = config.get(key)
            print(f"  ✓ {key}: {value}")
        else:
            print(f"  ✗ {key}: MISSING")
    
    # Check parameter ranges
    print("\nParameter range checks:")
    
    # Neuron count
    if config.has("brain.neuron_count"):
        neuron_count = config.get("brain.neuron_count")
        if 100 <= neuron_count <= 100000:
            print(f"  ✓ brain.neuron_count ({neuron_count}): OK")
        else:
            print(f"  ✗ brain.neuron_count ({neuron_count}): OUT OF RANGE")
    
    # Synapse density
    if config.has("brain.synapse_density"):
        density = config.get("brain.synapse_density")
        if 0.0 <= density <= 1.0:
            print(f"  ✓ brain.synapse_density ({density}): OK")
        else:
            print(f"  ✗ brain.synapse_density ({density}): OUT OF RANGE")
    
    # Validate entire configuration
    print("\nConfiguration validation:")
    errors, warnings = pynlm.validate_config(config)
    
    if errors:
        print("  Errors:")
        for error in errors:
            print(f"    - {error}")
    
    if warnings:
        print("  Warnings:")
        for warning in warnings:
            print(f"    - {warning}")
    
    if not errors and not warnings:
        print("  ✓ Configuration is valid!")

# Usage
config = pynlm.createResearchConfig()
debug_configuration(config)
```

### 3. Performance Issues

**Symptoms**:
- Slow simulation performance
- High memory usage
- System crashes
- Bottlenecks in specific operations

**Typical Solutions**:
```python
import pynlm
import time
import psutil
import os

class PerformanceDebugger:
    """Debug performance issues in NLM simulations."""
    
    def __init__(self):
        self.process = psutil.Process(os.getpid())
    
    def monitor_memory_usage(self) -> Dict[str, Any]:
        """Monitor memory usage during simulation."""
        
        memory_info = self.process.memory_info()
        memory_percent = self.process.memory_percent()
        
        return {
            'rss_mb': memory_info.rss / (1024 * 1024),
            'vms_mb': memory_info.vms / (1024 * 1024),
            'memory_percent': memory_percent,
            'cpu_percent': self.process.cpu_percent()
        }
    
    def profile_simulation(self, config: pynlm.Config, steps: int, 
                          chunk_size: int = 1000) -> Dict[str, Any]:
        """Profile simulation performance."""
        
        print(f"Profiling simulation: {steps} steps")
        
        # Initial memory
        initial_memory = self.monitor_memory_usage()
        
        # Run simulation in chunks
        total_time = 0
        chunk_times = []
        memory_usage = []
        
        for chunk in range(0, steps, chunk_size):
            chunk_steps = min(chunk_size, steps - chunk)
            
            # Start timing
            start_time = time.time()
            
            # Run chunk
            with pynlm.createBrain(config) as brain:
                brain.runSteps(chunk_steps)
                
                # Record metrics
                stats = brain.getFiringStatistics()
                
                # Measure memory
                current_memory = self.monitor_memory_usage()
                memory_usage.append(current_memory)
                
                # Calculate timing
                chunk_time = time.time() - start_time
                chunk_times.append(chunk_time)
                total_time += chunk_time
                
                # Print progress
                print(f"  Chunk {chunk//chunk_size + 1}: {chunk_steps} steps, "
                      f"{chunk_time:.2f}s, {stats['firing_percentage']:.1f}% firing")
        
        # Calculate performance metrics
        avg_time_per_step = total_time / steps
        steps_per_second = steps / total_time
        avg_memory_mb = sum(m['rss_mb'] for m in memory_usage) / len(memory_usage)
        
        performance = {
            'total_time_seconds': total_time,
            'steps_per_second': steps_per_second,
            'avg_time_per_step_ms': avg_time_per_step * 1000,
            'avg_memory_mb': avg_memory_mb,
            'peak_memory_mb': max(m['rss_mb'] for m in memory_usage),
            'chunk_times': chunk_times,
            'memory_usage': memory_usage
        }
        
        print(f"\nPerformance Summary:")
        print(f"  Total time: {performance['total_time_seconds']:.2f}s")
        print(f"  Steps per second: {performance['steps_per_second']:.0f}")
        print(f"  Average time per step: {performance['avg_time_per_step_ms']:.2f}ms")
        print(f"  Average memory usage: {performance['avg_memory_mb']:.1f}MB")
        print(f"  Peak memory usage: {performance['peak_memory_mb']:.1f}MB")
        
        return performance

# Usage example
perf_debugger = PerformanceDebugger()

# Create configuration
config = pynlm.createResearchConfig()
config.set("brain.neuron_count", 5000)

# Profile simulation
performance = perf_debugger.profile_simulation(config, steps=5000)

# Analyze performance
if performance['avg_time_per_step_ms'] > 100:  # >100ms per step
    print("\n⚠️  WARNING: Slow performance detected!")
    print("  Possible causes:")
    print("  - Large network size for current hardware")
    print("  - Inefficient configuration")
    print("  - Memory constraints")
    
    # Recommendations
    print("\nRecommendations:")
    if config.get("brain.neuron_count") > 10000:
        print("  - Reduce network size")
    if performance['peak_memory_mb'] > 4000:
        print("  - Reduce memory usage")
    print("  - Consider using sparse connectivity")
    print("  - Enable parallel processing if available")
```

### 4. Simulation Issues

**Symptoms**:
- Brains not firing
- Agents not moving
- World updates not working
- Unexpected behavior

**Typical Solutions**:
```python
def debug_simulation(brain, world, agent, steps=100):
    """Debug simulation issues."""
    
    print("Simulation Debug Report")
    print("=" * 40)
    
    # Check brain initialization
    print("\n1. Brain Status:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Check if brain is initialized
    if brain.getTotalNeuronCount() == 0:
        print("   ⚠️  WARNING: Brain has no neurons!")
    elif brain.getFiringNeuronCount() == 0:
        print("   ⚠️  WARNING: No neurons currently firing!")
    elif brain.getTotalSpikeCount() == 0:
        print("   ⚠️  WARNING: No spikes recorded!")
    
    # Run short simulation
    print(f"\n2. Running {steps}-step simulation:")
    for step in range(min(steps, 50)):  # Limit for debugging
        brain.step(step)
        
        if step % 10 == 0:
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            rate = brain.getAverageFiringRate()
            print(f"   Step {step}: {firing} firing, {spikes} spikes, {rate:.2f} Hz")
    
    # Check world status
    print(f"\n3. World Status:")
    print(f"   - Width: {world.getWidth()} cells")
    print(f"   - Height: {world.getHeight()} cells")
    print(f"   - Simulation time: {world.getSimulationTime()}")
    
    # Check agent status
    print(f"\n4. Agent Status:")
    print(f"   - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"   - Curiosity: {agent.isCuriosityEnabled()}")
    print(f"   - Development: {agent.isDevelopmentEnabled()}")
    print(f"   - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   - Novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Check for common issues
    print(f"\n5. Issue Analysis:")
    
    # Check if brain needs input
    if brain.getFiringNeuronCount() == 0 and brain.getTotalSpikeCount() == 0:
        print("   ⚠️  NEURAL ACTIVITY ISSUE:")
        print("     - Brain may need sensory input")
        print("     - Check if agent is processing sensory data")
        print("     - Verify world updates are happening")
        print("     - Consider enabling neuromodulation")
    
    # Check agent movement
    if world.getAgentPosition() is None:
        print("   ⚠️  AGENT POSITION ISSUE:")
        print("     - Agent position not set")
        print("     - Check world.reset() was called")
        print("     - Verify agent.initialize(world) was called")
    
    # Check world updates
    percept = world.getSensoryPercept()
    if percept is None:
        print("   ⚠️  WORLD PERCEPTION ISSUE:")
        print("     - No sensory input available")
        print("     - Check world.update() is being called")
        print("     - Verify world configuration")
    
    # Check agent processing
    if agent.getCuriosityLevel() == 0 and agent.getNoveltyLevel() == 0:
        print("   ⚠️  AGENT PROCESSING ISSUE:")
        print("     - Agent may need to process sensory input")
        print("     - Check agent.decodeMotorCommand()")
        print("     - Verify agent.applyRewardModulation() is working")
```

### 5. Environment Issues

**Symptoms**:
- Import errors
- Missing dependencies
- System resource constraints
- Installation problems

**Typical Solutions**:
```python
import sys
import platform
import subprocess
import pkgutil

def debug_environment():
    """Debug environment setup issues."""
    
    print("Environment Debug Report")
    print("=" * 40)
    
    # Check Python version
    print(f"\n1. Python Version:")
    print(f"   Current: {sys.version}")
    print(f"   Recommended: >= 3.8")
    
    if sys.version_info < (3, 8):
        print("   ⚠️  WARNING: Python version may cause compatibility issues")
    
    # Check platform
    print(f"\n2. Platform Information:")
    print(f"   System: {platform.system()}")
    print(f"   Release: {platform.release()}")
    print(f"   Version: {platform.version()}")
    
    # Check architecture
    print(f"\n3. Architecture:")
    print(f"   Processor: {platform.processor()}")
    print(f"   Architecture: {platform.architecture()[0]}")
    print(f"   Machine: {platform.machine()}")
    
    # Check NLM installation
    print(f"\n4. NLM Installation:")
    try:
        import pynlm
        print(f"   ✓ NLM imported successfully")
        print(f"   Version: {getattr(pynlm, '__version__', 'Unknown')}")
        
        # Check NLM components
        config = pynlm.createDefaultConfig()
        print(f"   ✓ Configuration creation: OK")
        
        brain = pynlm.createBrain(config)
        print(f"   ✓ Brain creation: OK")
        
        world = pynlm.createSimpleWorld()
        print(f"   ✓ World creation: OK")
        
        agent = pynlm.createAgentBrain(brain)
        print(f"   ✓ Agent creation: OK")
        
    except ImportError as e:
        print(f"   ✗ NLM import failed: {e}")
        print(f"   Solution: pip install pynlm")
    except Exception as e:
        print(f"   ✗ NLM test failed: {e}")
    
    # Check optional dependencies
    print(f"\n5. Optional Dependencies:")
    optional_deps = {
        'matplotlib': 'plotting and visualization',
        'numpy': 'numerical computations',
        'pandas': 'data analysis',
        'scipy': 'statistical analysis',
        'jupyter': 'notebook interface'
    }
    
    for dep, purpose in optional_deps.items():
        try:
            __import__(dep)
            print(f"   ✓ {dep}: {purpose}")
        except ImportError:
            print(f"   ✗ {dep}: {purpose} (not installed)")
    
    # Check system resources
    print(f"\n6. System Resources:")
    try:
        import psutil
        
        memory = psutil.virtual_memory()
        print(f"   Memory: {memory.total / (1024**3):.1f} GB total")
        print(f"           {memory.available / (1024**3):.1f} GB available")
        
        if memory.available < 2 * (1024**3):
            print(f"   ⚠️  WARNING: Low memory available")
        
        disk = psutil.disk_usage('/')
        print(f"   Disk Space: {disk.total / (1024**3):.1f} GB total")
        print(f"              {disk.free / (1024**3):.1f} GB free")
        
        if disk.free < 5 * (1024**3):
            print(f"   ⚠️  WARNING: Low disk space")
        
        cpu_count = psutil.cpu_count()
        print(f"   CPU Cores: {cpu_count}")
        
    except ImportError:
        print("   ✗ psutil not available for system resource monitoring")
    
    # Check build dependencies
    print(f"\n7. Build Dependencies:")
    required_deps = [
        'cmake >= 3.16',
        'C++20 compiler',
        'pybind11 >= 2.11.0',
        'scikit-build-core'
    ]
    
    for dep in required_deps:
        print(f"   - {dep}")
    
    print(f"\n   Note: Build dependencies are required for building from source.")
    print(f"   Pre-built packages may have different requirements.")

# Usage
if __name__ == "__main__":
    debug_environment()
```

## Advanced Debugging Techniques

### 1. Logging and Debugging Output

**Comprehensive Logging Setup**:
```python
import pynlm
import logging
import time
import sys
from datetime import datetime

class NLMSDebugger:
    """Advanced debugging and logging for NLM simulations."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.setup_logging()
        self.simulation_data = []
        self.error_log = []
    
    def setup_logging(self):
        """Setup comprehensive logging system."""
        
        # Create logger
        self.logger = logging.getLogger(f"NLM_Debug_{self.experiment_name}")
        self.logger.setLevel(logging.DEBUG)
        
        # Create console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        
        # Create file handler
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        file_handler = logging.FileHandler(
            f"debug_log_{timestamp}.txt",
            mode='w'
        )
        file_handler.setLevel(logging.DEBUG)
        
        # Create formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        
        # Set formatters and handlers
        console_handler.setFormatter(formatter)
        file_handler.setFormatter(formatter)
        
        self.logger.addHandler(console_handler)
        self.logger.addHandler(file_handler)
        
        # Suppress noisy loggers
        logging.getLogger('matplotlib').setLevel(logging.WARNING)
        logging.getLogger('numpy').setLevel(logging.WARNING)
    
    def log_simulation_step(self, step: int, brain, world, agent, 
                          timestep: float = 0.1) -> None:
        """Log detailed information about each simulation step."""
        
        # Collect metrics
        brain_stats = brain.getFiringStatistics()
        
        step_data = {
            'timestamp': time.time(),
            'step': step,
            'timestep': timestep,
            'brain_stats': {
                'total_neuron_count': brain.getTotalNeuronCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'total_spike_count': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'activity_percentage': brain_stats['activity_percentage'],
                'firing_percentage': brain_stats['firing_percentage'],
                'e_i_ratio': brain_stats['e_i_ratio'],
                'spikes_per_neuron': brain_stats['spikes_per_neuron']
            },
            'world_stats': {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
                'agent_position': world.getAgentPosition(),
                'sensory_percept': world.getSensoryPercept()
            },
            'agent_stats': {
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'reward_modulation_enabled': agent.isRewardModulationEnabled(),
                'curiosity_enabled': agent.isCuriosityEnabled(),
                'development_enabled': agent.isDevelopmentEnabled(),
                'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled(),
                'novelty_detection_enabled': agent.isNoveltyDetectionEnabled()
            }
        }
        
        # Log to file
        self.logger.info(f"Step {step}: "
                        f"Neurons={step_data['brain_stats']['total_neuron_count']}, "
                        f"Firing={step_data['brain_stats']['firing_neuron_count']}, "
                        f"Spikes={step_data['brain_stats']['total_spike_count']}, "
                        f"Rate={step_data['brain_stats']['average_firing_rate']:.2f} Hz, "
                        f"Activity={step_data['brain_stats']['activity_percentage']:.1f}%, "
                        f"Curiosity={step_data['agent_stats']['curiosity_level']:.3f}")
        
        # Store for later analysis
        self.simulation_data.append(step_data)
        
        # Log warnings
        if step_data['brain_stats']['firing_neuron_count'] == 0:
            self.logger.warning(f"Step {step}: No neurons currently firing!")
        
        if step_data['agent_stats']['curiosity_level'] == 0:
            self.logger.warning(f"Step {step}: Agent curiosity level is zero!")
        
        if step_data['world_stats']['sensory_percept'] is None:
            self.logger.warning(f"Step {step}: No sensory percept available!")
    
    def log_error(self, error_type: str, error_message: str, 
                  context: Dict = None) -> None:
        """Log error with context information."""
        
        error_info = {
            'timestamp': time.time(),
            'error_type': error_type,
            'error_message': error_message,
            'context': context or {}
        }
        
        self.error_log.append(error_info)
        
        self.logger.error(f"ERROR [{error_type}]: {error_message}")
        if context:
            self.logger.error(f"Context: {context}")
    
    def generate_debug_report(self) -> Dict[str, Any]:
        """Generate comprehensive debug report."""
        
        if not self.simulation_data:
            return {'error': 'No simulation data available'}
        
        report = {
            'experiment_name': self.experiment_name,
            'total_steps': len(self.simulation_data),
            'start_time': self.simulation_data[0]['timestamp'] if self.simulation_data else None,
            'end_time': self.simulation_data[-1]['timestamp'] if self.simulation_data else None,
            'duration_seconds': (
                self.simulation_data[-1]['timestamp'] - self.simulation_data[0]['timestamp']
                if self.simulation_data else 0
            ),
            'brain_statistics': self._aggregate_brain_stats(),
            'world_statistics': self._aggregate_world_stats(),
            'agent_statistics': self._aggregate_agent_stats(),
            'error_summary': self._generate_error_summary(),
            'warnings': self._generate_warnings()
        }
        
        return report
    
    def _aggregate_brain_stats(self) -> Dict[str, Any]:
        """Aggregate brain statistics from simulation data."""
        
        if not self.simulation_data:
            return {}
        
        brain_stats = self.simulation_data[0]['brain_stats']
        keys = brain_stats.keys()
        
        aggregated = {}
        
        for key in keys:
            values = [step_data['brain_stats'][key] for step_data in self.simulation_data]
            
            if isinstance(values[0], (int, float)):
                aggregated[f'{key}_mean'] = np.mean(values)
                aggregated[f'{key}_std'] = np.std(values)
                aggregated[f'{key}_min'] = np.min(values)
                aggregated[f'{key}_max'] = np.max(values)
                aggregated[f'{key}_median'] = np.median(values)
                aggregated[f'{key}_percentile_25'] = np.percentile(values, 25)
                aggregated[f'{key}_percentile_75'] = np.percentile(values, 75)
        
        return aggregated
    
    def _aggregate_world_stats(self) -> Dict[str, Any]:
        """Aggregate world statistics from simulation data."""
        
        if not self.simulation_data:
            return {}
        
        world_stats = self.simulation_data[0]['world_stats']
        keys = world_stats.keys()
        
        aggregated = {}
        
        for key in keys:
            if key == 'sensory_percept':
                continue  # Skip complex objects
                
            values = [step_data['world_stats'][key] for step_data in self.simulation_data]
            
            if isinstance(values[0], (int, float)):
                aggregated[f'{key}_mean'] = np.mean(values)
                aggregated[f'{key}_std'] = np.std(values)
        
        return aggregated
    
    def _aggregate_agent_stats(self) -> Dict[str, Any]:
        """Aggregate agent statistics from simulation data."""
        
        if not self.simulation_data:
            return {}
        
        agent_stats = self.simulation_data[0]['agent_stats']
        keys = agent_stats.keys()
        
        aggregated = {}
        
        for key in keys:
            values = [step_data['agent_stats'][key] for step_data in self.simulation_data]
            
            if isinstance(values[0], (int, float)):
                aggregated[f'{key}_mean'] = np.mean(values)
                aggregated[f'{key}_std'] = np.std(values)
                aggregated[f'{key}_min'] = np.min(values)
                aggregated[f'{key}_max'] = np.max(values)
        
        return aggregated
    
    def _generate_error_summary(self) -> Dict[str, Any]:
        """Generate error summary."""
        
        if not self.error_log:
            return {'total_errors': 0, 'error_types': []}
        
        error_types = {}
        
        for error in self.error_log:
            error_type = error['error_type']
            if error_type not in error_types:
                error_types[error_type] = 0
            error_types[error_type] += 1
        
        return {
            'total_errors': len(self.error_log),
            'error_types': error_types
        }
    
    def _generate_warnings(self) -> List[str]:
        """Generate warnings based on simulation data."""
        
        warnings = []
        
        if not self.simulation_data:
            return warnings
        
        # Check for no firing neurons
        if all(step['brain_stats']['firing_neuron_count'] == 0 
               for step in self.simulation_data):
            warnings.append("No neurons were firing throughout the simulation")
        
        # Check for zero spikes
        if all(step['brain_stats']['total_spike_count'] == 0 
               for step in self.simulation_data):
            warnings.append("No spikes were recorded throughout the simulation")
        
        # Check for zero curiosity
        if all(step['agent_stats']['curiosity_level'] == 0 
               for step in self.simulation_data):
            warnings.append("Agent curiosity level remained at zero throughout simulation")
        
        # Check for long periods of no activity
        low_activity_steps = [step for step in self.simulation_data 
                             if step['brain_stats']['activity_percentage'] < 1.0]
        if len(low_activity_steps) > len(self.simulation_data) * 0.8:
            warnings.append("Low activity levels for most of the simulation")
        
        return warnings
    
    def export_debug_data(self, filename: str = None) -> str:
        """Export debug data to file."""
        
        if filename is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"debug_data_{timestamp}.json"
        
        debug_data = {
            'experiment_name': self.experiment_name,
            'simulation_data': self.simulation_data,
            'error_log': self.error_log,
            'generated_at': time.time()
        }
        
        with open(filename, 'w') as f:
            json.dump(debug_data, f, indent=2, default=str)
        
        print(f"Debug data exported to {filename}")
        return filename

# Usage example
experiment_name = "Comprehensive Debug Example"
debugger = NLMSDebugger(experiment_name)

# Setup logging
print(f"Debug logging initialized for: {experiment_name}")

# Simulate debugging
try:
    # Create experiment components
    config = pynlm.createResearchConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Run simulation with logging
    for step in range(50):
        # Log each step
        debugger.log_simulation_step(step, brain, world, agent)
        
        # Simulate the step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log intentional errors for demonstration
        if step == 25:
            debugger.log_error("TEST_ERROR", "Simulated test error", 
                             {'step': step, 'component': 'agent'})
        
        if step % 10 == 0:
            debugger.log_simulation_step(step, brain, world, agent)
    
    # Generate debug report
    debug_report = debugger.generate_debug_report()
    
    print(f"\nDebug Report Summary:")
    print(f"  Total steps logged: {debug_report['total_steps']}")
    print(f"  Brain neurons: {debug_report['brain_statistics'].get('total_neuron_count_mean', 'N/A')}")
    print(f"  Total errors: {debug_report['error_summary']['total_errors']}")
    
    # Export debug data
    export_file = debugger.export_debug_data()
    
except Exception as e:
    debugger.log_error("SIMULATION_ERROR", str(e), {'step': 'initialization'})
    print(f"Debug session completed with errors. Check log files.")
```

### 2. Remote Debugging

**Remote Debugging Setup**:
```python
import pynlm
import socket
import threading
import time
import json

class RemoteDebugger:
    """Remote debugging capabilities for NLM simulations."""
    
    def __init__(self, host='localhost', port=9999):
        self.host = host
        self.port = port
        self.server_socket = None
        self.client_socket = None
        self.debugger_thread = None
        self.running = False
    
    def start_server(self):
        """Start remote debugging server."""
        
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(1)
        
        print(f"Remote debugger server started on {self.host}:{self.port}")
        print("Waiting for client connection...")
        
        self.client_socket, client_address = self.server_socket.accept()
        print(f"Client connected from {client_address}")
        
        self.running = True
        self.debugger_thread = threading.Thread(target=self._debugger_loop)
        self.debugger_thread.start()
    
    def _debugger_loop(self):
        """Main debugging loop."""
        
        while self.running:
            try:
                # Receive command from client
                data = self.client_socket.recv(1024).decode('utf-8')
                
                if not data:
                    break
                
                # Parse command
                command = json.loads(data)
                self._handle_command(command)
                
            except (ConnectionError, json.JSONDecodeError) as e:
                print(f"Debugger error: {e}")
                break
    
    def _handle_command(self, command: Dict[str, Any]):
        """Handle client commands."""
        
        command_type = command.get('type', '')
        
        if command_type == 'get_simulation_state':
            self._send_simulation_state(command.get('simulation_data'))
        
        elif command_type == 'set_debug_level':
            self._set_debug_level(command.get('level', 'INFO'))
        
        elif command_type == 'get_performance_metrics':
            self._send_performance_metrics(command.get('performance_data'))
        
        elif command_type == 'request_screenshot':
            self._take_screenshot()
        
        elif command_type == 'execute_command':
            self._execute_remote_command(command.get('command'))
        
        elif command_type == 'terminate':
            self._terminate()
    
    def _send_simulation_state(self, simulation_data: Dict):
        """Send simulation state to client."""
        
        state = {
            'type': 'simulation_state',
            'data': simulation_data,
            'timestamp': time.time()
        }
        
        self._send_data(state)
    
    def _set_debug_level(self, level: str):
        """Set debug logging level."""
        
        level = level.upper()
        if level in ['DEBUG', 'INFO', 'WARNING', 'ERROR']:
            self.debug_level = level
            print(f"Debug level set to: {level}")
    
    def _send_performance_metrics(self, performance_data: Dict):
        """Send performance metrics to client."""
        
        metrics = {
            'type': 'performance_metrics',
            'data': performance_data,
            'timestamp': time.time()
        }
        
        self._send_data(metrics)
    
    def _take_screenshot(self):
        """Take screenshot of simulation window."""
        
        try:
            import matplotlib.pyplot as plt
            import matplotlib
            matplotlib.use('Agg')  # Non-interactive backend
            
            # Create visualization
            plt.figure(figsize=(12, 8))
            
            # Plot simulation data
            plt.subplot(2, 2, 1)
            plt.plot([step['step'] for step in self.simulation_data],
                    [step['brain_stats']['average_firing_rate'] for step in self.simulation_data])
            plt.title('Average Firing Rate')
            plt.xlabel('Step')
            plt.ylabel('Hz')
            
            # Plot activity percentage
            plt.subplot(2, 2, 2)
            plt.plot([step['step'] for step in self.simulation_data],
                    [step['brain_stats']['activity_percentage'] for step in self.simulation_data])
            plt.title('Activity Percentage')
            plt.xlabel('Step')
            plt.ylabel('%')
            
            # Plot curiosity level
            plt.subplot(2, 2, 3)
            plt.plot([step['step'] for step in self.simulation_data],
                    [step['agent_stats']['curiosity_level'] for step in self.simulation_data])
            plt.title('Curiosity Level')
            plt.xlabel('Step')
            plt.ylabel('Level')
            
            plt.tight_layout()
            plt.savefig('debug_screenshot.png', dpi=150)
            plt.close()
            
            print("Screenshot saved as debug_screenshot.png")
            
        except Exception as e:
            print(f"Failed to take screenshot: {e}")
    
    def _execute_remote_command(self, command: str):
        """Execute remote command."""
        
        try:
            result = eval(command)  # Execute in safe context
            response = {
                'type': 'command_result',
                'command': command,
                'result': result,
                'success': True
            }
        except Exception as e:
            response = {
                'type': 'command_result',
                'command': command,
                'error': str(e),
                'success': False
            }
        
        self._send_data(response)
    
    def _send_data(self, data: Dict):
        """Send data to client."""
        
        try:
            json_data = json.dumps(data)
            self.client_socket.send(json_data.encode('utf-8'))
        except Exception as e:
            print(f"Failed to send data: {e}")
    
    def _terminate(self):
        """Terminate debugging session."""
        
        print("Terminating debugging session...")
        self.running = False
        
        if self.debugger_thread:
            self.debugger_thread.join(timeout=5)
        
        if self.client_socket:
            self.client_socket.close()
        
        if self.server_socket:
            self.server_socket.close()
    
    def connect_to_server(self, host: str, port: int):
        """Connect to remote debugging server."""
        
        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((host, port))
            
            print(f"Connected to remote debugger at {host}:{port}")
            
            # Start listening for commands
            self.debugger_thread = threading.Thread(target=self._listen_for_commands)
            self.debugger_thread.start()
            
            return True
            
        except Exception as e:
            print(f"Failed to connect to server: {e}")
            return False
    
    def _listen_for_commands(self):
        """Listen for commands from server."""
        
        while self.running:
            try:
                data = self.client_socket.recv(1024).decode('utf-8')
                
                if not data:
                    break
                
                command = json.loads(data)
                self._handle_command(command)
                
            except (ConnectionError, json.JSONDecodeError) as e:
                print(f"Connection error: {e}")
                break

# Usage example
if __name__ == "__main__":
    print("NLM Remote Debugging Example")
    print("=" * 50)
    
    # Example 1: Start debugging server
    print("\n1. Starting debugging server...")
    debugger = RemoteDebugger(host='localhost', port=9999)
    debugger.start_server()
    
    # Example 2: Connect to server
    print("\n2. Connecting to debugging server...")
    client_debugger = RemoteDebugger()
    if client_debugger.connect_to_server('localhost', 9999):
        print("✓ Connected successfully")
        
        # Send commands to server
        command = {
            'type': 'get_simulation_state',
            'simulation_data': {
                'step': 0,
                'firing_rate': 10.5,
                'activity': 0.8
            }
        }
        
        # The server would receive this command
        print("✓ Command sent to server")
    
    # Clean up
    print("\nCleaning up...")
    if 'debugger' in locals():
        debugger._terminate()
    if 'client_debugger' in locals():
        client_debugger._terminate()
```

## Debugging Tools and Utilities

### 1. NLM Debug Assistant

**Interactive Debugging Assistant**:
```python
import pynlm
import json
from datetime import datetime

class NLMDebugAssistant:
    """Interactive debugging assistant for NLM simulations."""
    
    def __init__(self):
        self.debug_history = []
        self.session_start = datetime.now()
    
    def analyze_simulation(self, brain, world, agent, steps=100) -> Dict[str, Any]:
        """Analyze simulation and provide debugging insights."""
        
        analysis = {
            'timestamp': datetime.now().isoformat(),
            'simulation_parameters': {
                'steps': steps,
                'brain_type': type(brain).__name__,
                'world_type': type(world).__name__,
                'agent_type': type(agent).__name__
            },
            'initial_state': self._capture_state(brain, world, agent),
            'simulation_data': [],
            'issues_found': [],
            'recommendations': []
        }
        
        # Run simulation with analysis
        for step in range(steps):
            # Capture state before step
            state_before = self._capture_state(brain, world, agent)
            
            # Perform step
            try:
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Capture state after step
                state_after = self._capture_state(brain, world, agent)
                
                simulation_step = {
                    'step': step,
                    'state_before': state_before,
                    'state_after': state_after,
                    'changes': self._calculate_changes(state_before, state_after)
                }
                
                analysis['simulation_data'].append(simulation_step)
                
                # Check for issues
                issues = self._detect_issues(state_before, state_after)
                if issues:
                    analysis['issues_found'].extend(issues)
                
            except Exception as e:
                analysis['issues_found'].append({
                    'type': 'exception',
                    'message': str(e),
                    'step': step
                })
                break
        
        # Generate recommendations
        analysis['recommendations'] = self._generate_recommendations(analysis)
        
        return analysis
    
    def _capture_state(self, brain, world, agent) -> Dict[str, Any]:
        """Capture current simulation state."""
        return {
            'brain': {
                'neuron_count': brain.getTotalNeuronCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'activity_percentage': brain.getFiringStatistics()['activity_percentage']
            },
            'world': {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime(),
                'agent_position': world.getAgentPosition()
            },
            'agent': {
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'reward_modulation': agent.isRewardModulationEnabled(),
                'curiosity_enabled': agent.isCuriosityEnabled(),
                'development_enabled': agent.isDevelopmentEnabled()
            }
        }
    
    def _calculate_changes(self, before: Dict, after: Dict) -> Dict[str, Any]:
        """Calculate changes between states."""
        
        changes = {}
        
        for key in before:
            if isinstance(before[key], (int, float)) and isinstance(after[key], (int, float)):
                changes[key] = after[key] - before[key]
            elif key in ['agent_position']:
                # For complex objects, just note if they changed
                if before[key] != after[key]:
                    changes[key] = 'changed'
        
        return changes
    
    def _detect_issues(self, before: Dict, after: Dict) -> List[Dict[str, Any]]:
        """Detect potential issues from state changes."""
        
        issues = []
        
        # Check for zero activity
        if after['brain']['firing_neuron_count'] == 0 and before['brain']['firing_neuron_count'] > 0:
            issues.append({
                'type': 'activity_loss',
                'severity': 'medium',
                'description': f"Neurons stopped firing (from {before['brain']['firing_neuron_count']} to {after['brain']['firing_neuron_count']})"
            })
        
        # Check for activity increase
        if after['brain']['firing_neuron_count'] > before['brain']['firing_neuron_count'] * 2:
            issues.append({
                'type': 'activity_spike',
                'severity': 'low',
                'description': f"Unexpected spike in firing neurons (from {before['brain']['firing_neuron_count']} to {after['brain']['firing_neuron_count']})"
            })
        
        # Check for zero curiosity
        if after['agent']['curiosity_level'] == 0 and before['agent']['curiosity_level'] > 0:
            issues.append({
                'type': 'curiosity_loss',
                'severity': 'high',
                'description': f"Agent curiosity dropped from {before['agent']['curiosity_level']:.3f} to {after['agent']['curiosity_level']:.3f}"
            })
        
        # Check for missing world updates
        if before['world']['simulation_time'] == after['world']['simulation_time']:
            issues.append({
                'type': 'world_stagnation',
                'severity': 'medium',
                'description': 'World simulation time did not advance'
            })
        
        return issues
    
    def _generate_recommendations(self, analysis: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Generate debugging recommendations."""
        
        recommendations = []
        
        # Analyze issues
        issues_by_type = {}
        for issue in analysis['issues_found']:
            issue_type = issue['type']
            if issue_type not in issues_by_type:
                issues_by_type[issue_type] = 0
            issues_by_type[issue_type] += 1
        
        # Generate recommendations based on issues
        if 'activity_loss' in issues_by_type:
            recommendations.append({
                'priority': 'high',
                'issue': 'Neural activity loss',
                'suggestion': 'Check sensory input pipeline. Ensure agent is processing world percepts and feeding them to brain.',
                'steps': [
                    'Verify world.update() is being called',
                    'Check agent.processSensoryInput() is working',
                    'Ensure brain.receiveSensoryInput() is implemented',
                    'Consider enabling neuromodulation for activity regulation'
                ]
            })
        
        if 'curiosity_loss' in issues_by_type:
            recommendations.append({
                'priority': 'medium',
                'issue': 'Curiosity decay',
                'suggestion': 'Investigate novelty detection and curiosity mechanisms.',
                'steps': [
                    'Check novelty calculation accuracy',
                    'Verify curiosity update mechanism',
                    'Consider adjusting curiosity decay parameters',
                    'Add curiosity monitoring for debugging'
                ]
            })
        
        if 'world_stagnation' in issues_by_type:
            recommendations.append({
                'priority': 'high',
                'issue': 'World simulation stagnation',
                'suggestion': 'World time is not advancing properly.',
                'steps': [
                    'Verify world.update() parameters',
                    'Check timestep settings',
                    'Ensure no infinite loops in world processing',
                    'Validate world configuration'
                ]
            })
        
        # Add general recommendations
        recommendations.append({
            'priority': 'low',
            'issue': 'Performance monitoring',
            'suggestion': 'Implement continuous performance monitoring for better debugging.',
            'steps': [
                'Add logging for key metrics',
                'Implement performance profiling',
                'Set up automated alerts for anomalies'
            ]
        })
        
        return recommendations
    
    def export_analysis(self, analysis: Dict[str, Any], filename: str = None) -> str:
        """Export analysis results."""
        
        if filename is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"debug_analysis_{timestamp}.json"
        
        with open(filename, 'w') as f:
            json.dump(analysis, f, indent=2, default=str)
        
        print(f"Analysis exported to {filename}")
        return filename
    
    def print_summary(self, analysis: Dict[str, Any]):
        """Print analysis summary."""
        
        print(f"\n{'=' * 60}")
        print(f"NLM DEBUG ANALYSIS SUMMARY - {analysis['timestamp']}")
        print(f"{'=' * 60}")
        
        print(f"\nSimulation Parameters:")
        params = analysis['simulation_parameters']
        print(f"  Steps: {params['steps']}")
        print(f"  Brain type: {params['brain_type']}")
        print(f"  World type: {params['world_type']}")
        print(f"  Agent type: {params['agent_type']}")
        
        print(f"\nInitial State:")
        init = analysis['initial_state']
        print(f"  Brain neurons: {init['brain']['neuron_count']}")
        print(f"  Firing neurons: {init['brain']['firing_neuron_count']}")
        print(f"  Activity: {init['brain']['activity_percentage']:.1f}%")
        print(f"  Curiosity: {init['agent']['curiosity_level']:.3f}")
        
        print(f"\nAnalysis Results:")
        print(f"  Total steps analyzed: {len(analysis['simulation_data'])}")
        print(f"  Issues found: {len(analysis['issues_found'])}")
        
        if analysis['issues_found']:
            print(f"  Issue types: {', '.join(set(issue['type'] for issue in analysis['issues_found']))}")
        
        print(f"  Recommendations: {len(analysis['recommendations'])}")
        
        if analysis['issues_found']:
            print(f"\n{'!' * 60}")
            print(f"ISSUES FOUND - IMMEDIATE ATTENTION RECOMMENDED")
            print(f"{'!' * 60}")
            
            for issue in analysis['issues_found']:
                print(f"\n  Issue: {issue['type']}")
                print(f"  Severity: {issue.get('severity', 'unknown')}")
                print(f"  Description: {issue.get('description', 'N/A')}")
        
        if analysis['recommendations']:
            print(f"\n{'RECOMMENDATIONS' * 20}")
            
            for rec in analysis['recommendations']:
                print(f"\n  Priority: {rec['priority'].upper()} - {rec['issue']}")
                print(f"    Suggestion: {rec['suggestion']}")
                print(f"    Steps:")
                for i, step in enumerate(rec['steps'], 1):
                    print(f"      {i}. {step}")

# Usage example
assistant = NLMDebugAssistant()

print("NLM Debug Assistant - Interactive Debugging")
print("=" * 60)

# Create experiment components
config = pynlm.createResearchConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Run analysis
analysis = assistant.analyze_simulation(brain, world, agent, steps=50)

# Print summary
assistant.print_summary(analysis)

# Export analysis
export_file = assistant.export_analysis(analysis)

print(f"\n{'=' * 60}")
print(f"Debugging session complete. Results saved to {export_file}")
print(f"{'=' * 60}")
```

### 2. Debug Script Template

**Complete Debugging Script Template**:
```bash
#!/usr/bin/env python3
"""
NLM Experiment Debugging Script

This script provides comprehensive debugging capabilities for NLM experiments.
"""

import argparse
import json
import logging
import os
import sys
import time
from datetime import datetime
from pathlib import Path

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from nlm_debugger import NLMSDebugger, NLMDebugAssistant, RemoteDebugger

class NLMSDebugScript:
    """Main debugging script for NLM experiments."""
    
    def __init__(self, args):
        self.args = args
        self.setup_logging()
        self.debugger = None
        self.assistant = None
        self.remote_debugger = None
    
    def setup_logging(self):
        """Setup logging for debugging script."""
        
        # Create log directory
        log_dir = Path("logs")
        log_dir.mkdir(exist_ok=True)
        
        # Setup logger
        logging.basicConfig(
            level=logging.DEBUG,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler(
                    log_dir / f"debug_script_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
                ),
                logging.StreamHandler(sys.stdout)
            ]
        )
        
        self.logger = logging.getLogger(__name__)
    
    def run_basic_debug(self):
        """Run basic debugging of experiment."""
        
        self.logger.info("Starting basic debugging...")
        
        try:
            # Load experiment configuration
            with open(self.args.config_file, 'r') as f:
                config_data = json.load(f)
            
            # Create experiment components
            config = pynlm.createResearchConfig()
            
            # Apply configuration from file
            if 'brain.neuron_count' in config_data:
                config.set("brain.neuron_count", config_data['brain.neuron_count'])
            
            if 'brain.synapse_density' in config_data:
                config.set("brain.synapse_density", config_data['brain.synapse_density'])
            
            # Initialize components
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            self.logger.info("Experiment components initialized successfully")
            
            # Run simulation with debugging
            steps = self.args.steps or 100
            
            for step in range(steps):
                # Log step
                self.logger.info(f"Step {step}/{steps}")
                
                # Run simulation
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                if percept is None:
                    self.logger.warning(f"Step {step}: No sensory percept available")
                
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Log key metrics
                if step % 10 == 0:
                    stats = brain.getFiringStatistics()
                    self.logger.info(f"  Neurons: {brain.getFiringNeuronCount()}, "
                                   f"Spikes: {stats['total_spikes']}, "
                                   f"Firing: {stats['firing_percentage']:.1f}%")
            
            self.logger.info("Basic debugging completed successfully")
            
        except Exception as e:
            self.logger.error(f"Basic debugging failed: {e}")
            raise
    
    def run_advanced_debug(self):
        """Run advanced debugging with comprehensive analysis."""
        
        self.logger.info("Starting advanced debugging...")
        
        try:
            # Initialize debug assistant
            self.assistant = NLMDebugAssistant()
            
            # Create experiment components
            config = pynlm.createResearchConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            # Run analysis
            analysis = self.assistant.analyze_simulation(
                brain, world, agent, 
                steps=self.args.steps or 100
            )
            
            # Print summary
            self.assistant.print_summary(analysis)
            
            # Export analysis
            if self.args.export:
                export_file = self.assistant.export_analysis(
                    analysis, f"analysis_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
                )
                self.logger.info(f"Analysis exported to {export_file}")
            
            self.logger.info("Advanced debugging completed successfully")
            
        except Exception as e:
            self.logger.error(f"Advanced debugging failed: {e}")
            raise
    
    def run_remote_debug(self):
        """Run remote debugging session."""
        
        self.logger.info("Starting remote debugging...")
        
        try:
            # Initialize remote debugger
            self.remote_debugger = RemoteDebugger(
                host=self.args.host or 'localhost',
                port=self.args.port or 9999
            )
            
            # Start server
            if self.args.server:
                self.remote_debugger.start_server()
                self.logger.info("Remote debugger server started")
            
            # Connect to server
            if self.args.connect:
                connected = self.remote_debugger.connect_to_server(
                    self.args.host or 'localhost',
                    self.args.port or 9999
                )
                
                if connected:
                    self.logger.info("Connected to remote debugger server")
                    
                    # Wait for commands
                    input("Press Enter to continue...")
                
                self.remote_debugger._terminate()
            
            self.logger.info("Remote debugging completed")
            
        except Exception as e:
            self.logger.error(f"Remote debugging failed: {e}")
            raise
    
    def run_validation(self):
        """Run experiment validation."""
        
        self.logger.info("Starting experiment validation...")
        
        # Validate experiment directory
        experiment_dir = Path(self.args.experiment_dir)
        
        if not experiment_dir.exists():
            raise FileNotFoundError(f"Experiment directory not found: {experiment_dir}")
        
        # Check for essential files
        essential_files = [
            'config.json',
            'analysis.py',
            'results.csv'
        ]
        
        missing_files = []
        for file in essential_files:
            if not (experiment_dir / file).exists():
                missing_files.append(file)
        
        if missing_files:
            self.logger.warning(f"Missing essential files: {missing_files}")
        else:
            self.logger.info("All essential files present")
        
        # Validate configuration
        config_file = experiment_dir / 'config.json'
        if config_file.exists():
            try:
                with open(config_file, 'r') as f:
                    config = json.load(f)
                
                # Validate required keys
                required_keys = ['brain.neuron_count', 'brain.synapse_density']
                
                for key in required_keys:
                    if key not in config:
                        self.logger.warning(f"Missing configuration key: {key}")
                    
            except json.JSONDecodeError as e:
                self.logger.error(f"Invalid JSON in config file: {e}")
            except Exception as e:
                self.logger.error(f"Error reading config file: {e}")
        
        # Validate results
        results_file = experiment_dir / 'results.csv'
        if results_file.exists():
            try:
                import pandas as pd
                results = pd.read_csv(results_file)
                self.logger.info(f"Results file contains {len(results)} rows")
                
                # Check for essential columns
                required_columns = ['step', 'firing_percentage']
                missing_columns = [col for col in required_columns if col not in results.columns]
                
                if missing_columns:
                    self.logger.warning(f"Results missing columns: {missing_columns}")
                    
            except Exception as e:
                self.logger.error(f"Error reading results file: {e}")
        
        self.logger.info("Experiment validation completed")
    
    def run_all_debugging(self):
        """Run comprehensive debugging session."""
        
        self.logger.info("Starting comprehensive debugging session...")
        
        try:
            # Initialize debugger
            self.debugger = NLMSDebugger(
                experiment_name=self.args.experiment_name or "comprehensive_debug"
            )
            
            # Create experiment components
            config = pynlm.createResearchConfig()
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            # Run simulation with detailed logging
            steps = self.args.steps or 100
            
            for step in range(steps):
                # Log simulation
                self.debugger.log_simulation_step(step, brain, world, agent)
                
                # Run simulation
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Log errors periodically
                if step % 25 == 0:
                    self.debugger.log_error(
                        'SIMULATION_ERROR',
                        f'Step {step}: Simulation completed successfully',
                        {'step': step, 'status': 'ok'}
                    )
            
            # Generate debug report
            debug_report = self.debugger.generate_debug_report()
            
            self.logger.info(f"Debug report generated: {json.dumps(debug_report, indent=2)}")
            
            # Export debug data
            if self.args.export:
                export_file = self.debugger.export_debug_data(
                    f"debug_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
                )
                self.logger.info(f"Debug data exported to {export_file}")
            
            # Print summary
            self._print_debug_summary(debug_report)
            
            self.logger.info("Comprehensive debugging completed successfully")
            
        except Exception as e:
            self.logger.error(f"Comprehensive debugging failed: {e}")
            raise
    
    def _print_debug_summary(self, debug_report: Dict[str, Any]):
        """Print debug session summary."""
        
        print(f"\n{'=' * 80}")
        print(f"NLM DEBUG SESSION SUMMARY")
        print(f"{'=' * 80}")
        
        print(f"\nSession Information:")
        print(f"  Duration: {debug_report.get('duration_seconds', 'N/A')} seconds")
        print(f"  Total steps: {debug_report.get('total_steps', 'N/A')}")
        print(f"  Simulation time: {debug_report.get('start_time', 'N/A')}")
        
        if 'brain_statistics' in debug_report:
            brain_stats = debug_report['brain_statistics']
            print(f"\nBrain Statistics:")
            for key, value in brain_stats.items():
                if isinstance(value, (int, float)):
                    print(f"  {key}: {value:.4f}")
        
        if 'error_summary' in debug_report:
            error_summary = debug_report['error_summary']
            print(f"\nError Summary:")
            print(f"  Total errors: {error_summary.get('total_errors', 0)}")
            
            if 'error_types' in error_summary and error_summary['error_types']:
                print(f"  Error types: {error_summary['error_types']}")
        
        if 'warnings' in debug_report:
            warnings = debug_report['warnings']
            if warnings:
                print(f"\n{'!' * 80}")
                print(f"WARNINGS FOUND - REVIEW RECOMMENDED")
                print(f"{'!' * 80}")
                for warning in warnings:
                    print(f"  - {warning}")
        
        print(f"\n{'=' * 80}")
    
    def run(self):
        """Run debugging based on command line arguments."""
        
        if self.args.debug_type == 'basic':
            self.run_basic_debug()
        
        elif self.args.debug_type == 'advanced':
            self.run_advanced_debug()
        
        elif self.args.debug_type == 'remote':
            self.run_remote_debug()
        
        elif self.args.debug_type == 'validation':
            self.run_validation()
        
        elif self.args.debug_type == 'all':
            self.run_all_debugging()
        
        else:
            raise ValueError(f"Unknown debug type: {self.args.debug_type}")

def main():
    """Main function."""
    
    parser = argparse.ArgumentParser(
        description="NLM Experiment Debugging Script",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Debugging type
    parser.add_argument(
        '--debug-type', '-t',
        choices=['basic', 'advanced', 'remote', 'validation', 'all'],
        default='all',
        help='Type of debugging to perform'
    )
    
    # Experiment configuration
    parser.add_argument(
        '--config-file', '-c',
        default='config.json',
        help='Path to experiment configuration file'
    )
    
    # Debugging parameters
    parser.add_argument(
        '--steps', '-s',
        type=int,
        default=100,
        help='Number of simulation steps'
    )
    
    # Remote debugging parameters
    parser.add_argument(
        '--host', '-H',
        default='localhost',
        help='Remote debugger host'
    )
    
    parser.add_argument(
        '--port', '-P',
        type=int,
        default=9999,
        help='Remote debugger port'
    )
    
    # File export options
    parser.add_argument(
        '--export', '-e',
        action='store_true',
        help='Export debug data to file'
    )
    
    # Server options
    parser.add_argument(
        '--server', '-S',
        action='store_true',
        help='Start debugging server'
    )
    
    parser.add_argument(
        '--connect', '-C',
        action='store_true',
        help='Connect to debugging server'
    )
    
    # Experiment information
    parser.add_argument(
        '--experiment-name', '-n',
        default='nlm_debug_session',
        help='Name for debugging session'
    )
    
    # Validation options
    parser.add_argument(
        '--experiment-dir', '-E',
        default='experiment',
        help='Path to experiment directory for validation'
    )
    
    args = parser.parse_args()
    
    # Create and run debug script
    debug_script = NLMSDebugScript(args)
    debug_script.run()

if __name__ == '__main__':
    main()
```

## Conclusion

This comprehensive Debugging and Troubleshooting Guide provides extensive resources for debugging NLM (Neural Learning Machine) experiments and simulations. It covers:

1. **Common Debugging Issues**: Basic runtime errors, configuration problems, performance issues, and simulation problems
2. **Advanced Debugging Techniques**: Logging, remote debugging, and interactive debugging
3. **Debugging Tools and Utilities**: Debug assistant, remote debugger, and comprehensive script templates
4. **Problem-Solving Strategies**: Systematic approach, isolation techniques, and root cause analysis

The guide emphasizes:

- **Proactive Debugging**: Identifying and preventing issues before they occur
- **Comprehensive Documentation**: Detailed logging and reporting of debugging activities
- **Systematic Approach**: Logical, step-by-step debugging process
- **Tool Integration**: Leveraging specialized debugging tools and utilities
- **Reproducibility**: Ensuring debugging results can be reproduced and verified

Key features include:

- **Interactive Debug Assistant**: Comprehensive analysis and recommendation system
- **Remote Debugging**: Network-based debugging capabilities
- **Performance Monitoring**: Detailed performance tracking and analysis
- **Error Handling**: Comprehensive error detection and reporting
- **Validation Tools**: Experiment validation and quality assurance

The guide serves as a complete reference for debugging NLM experiments, from basic issue resolution to advanced debugging scenarios. It provides practical examples, code templates, and best practices for effective debugging in NLM research environments.

-----
*End of Debugging and Troubleshooting Guide*