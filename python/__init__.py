# NLM Python Module

This module provides Python bindings for the NLM (Neural Learning Machine) framework, including comprehensive memory management capabilities.

## Overview

This module provides:
- Core NLM classes (Brain, Config, etc.)
- Memory management utilities (MemoryTracker, MemoryDebug, MemoryProfiler)
- Simulation and world management
- Integration with neural computation components

## Installation

The module is built as a Python extension using pybind11. It requires the NLM framework to be compiled and installed.

## Usage Examples

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createQuickBrain("basic")

# Get memory management instances
memory_tracker = pynlm.MemoryTracker()
memory_debug = pynlm.MemoryDebug()
memory_profiler = pynlm.MemoryProfiler()

# Enable memory management
memory_tracker.enable()
memory_debug.enable()
memory_profiler.enable()

# Start simulation
memory_tracker.startStep(0, 0.0)
memory_debug.startStep(0, 0.0)
memory_profiler.startStep(0, 0.0)

# Perform simulation
# ... simulation code ...

# Generate reports
report = memory_tracker.generateReport()
print(report)

# Save reports
memory_tracker.saveReport("simulation_report.txt")
memory_debug.saveReport("debug_report.txt")
memory_profiler.saveProfileReport("profiler_report.txt")

# End simulation
memory_tracker.endStep(0, 1.0)
memory_debug.endStep(0, 1.0)
memory_profiler.endStep(0, 1.0)
```

### Memory Debugging

```python
import pynlm

# Create memory debugger
memory_debug = pynlm.MemoryDebug()
memory_debug.enable()

# Configure debugging
config = pynlm.MemoryDebugConfig()
config.enableStackTrace = True
config.enablePatternTracking = True
config.enableCorruptionDetection = True
config.enableAccessTracking = True
config.maxAccessHistorySize = 10000

memory_debug.setConfig(config)

# Debug memory blocks
debug_info = memory_debug.getBlockDebugInfo(address)
if debug_info.isValid:
    print(f"Block valid: {debug_info.isValid}")
    print(f"Block allocated: {debug_info.isAllocated}")
    print(f"Size: {debug_info.size} bytes")
    print(f"Allocation site: {debug_info.allocationSite}")

# Check for corruption
if memory_debug.isBlockCorrupted(address):
    print("Memory block is corrupted!")

# Find memory issues
leaks = memory_debug.findUnfreedBlocks()
print(f"Found {len(leaks)} potential memory leaks")

# Get debug recommendations
recommendations = memory_debug.getDebugRecommendations()
for rec in recommendations:
    print(f"Debug recommendation: {rec}")

# Generate debug report
debug_report = memory_debug.generateReport()
print(debug_report)
```

### Memory Profiling

```python
import pynlm

# Create memory profiler
memory_profiler = pynlm.MemoryProfiler()
memory_profiler.enable()

# Configure profiling
memory_profiler.setProfileType(pynlm.ProfileType.AllocationProfile)
memory_profiler.setProfileInterval(1.0)
memory_profiler.setPressureThreshold(0.8)

# Start profiling
memory_profiler.startProfile("Neural Simulation")

# Perform simulation with profiling
for step in range(100):
    memory_profiler.startStep(step, step * 0.1)
    
    # ... simulation code ...
    
    # Record memory accesses
    memory_profiler.recordAccess(address, pynlm.MemoryLevel.RAM, thread_id, True, False, latency, size, "simulation.py")
    
    # Record allocations
    memory_profiler.recordAllocation(address, size, "simulation.py", "Stack trace", thread_id)
    
    # End step
    memory_profiler.endStep(step, (step + 1) * 0.1)

# End profiling
memory_profiler.endProfile("Neural Simulation")

# Get current profile
current_profile = memory_profiler.getCurrentProfile()
print(f"Profile session: {current_profile.sessionName}")
print(f"Steps recorded: {current_profile.stepCount}")
print(f"Memory accesses: {len(current_profile.accesses)}")

# Get optimization recommendations
recommendations = memory_profiler.getOptimizationRecommendations()
for rec in recommendations:
    print(f"Optimization: {rec.optimizationType}")
    print(f"  Gain: {rec.potentialGain}%")
    print(f"  Description: {rec.description}")
    print(f"  Auto-fixable: {rec.isAutoFixable}")

# Generate comprehensive report
report = memory_profiler.generateProfileReport()
print(report)

# Save reports
memory_profiler.saveProfileReport("neural_simulation_profile.txt")
memory_profiler.saveOptimizationReport("neural_simulation_optimization.txt")
```

### Advanced Memory Management

```python
import pynlm

# Create comprehensive memory management system
tracker = pynlm.MemoryTracker()
debugger = pynlm.MemoryDebug()
profiler = pynlm.MemoryProfiler()

# Enable all systems
tracker.enable()
debugger.enable()
profiler.enable()

# Integrate systems
profiler.integrateWithMemoryTracker(tracker)
profiler.integrateWithMemoryDebug(debugger)

# Configure systems
tracker.setMaxHistorySize(100000)
profiler.setProfileInterval(0.5)
profiler.setPressureThreshold(0.7)

# Start comprehensive simulation
step = 0
time = 0.0

tracker.startStep(step, time)
debugger.startStep(step, time)
profiler.startStep(step, time)

# Run simulation with memory management
for i in range(50):
    # ... simulation code ...
    
    # Update memory pressure
    pressure = pynlm.MemoryPressure()
    pressure.currentPressure = (i + 1) / 200.0  # Varying pressure
    pressure.pressureThreshold = 0.7
    pressure.totalMemory = 16 * 1024 * 1024 * 1024  # 16GB
    pressure.availableMemory = pressure.totalMemory * (1.0 - pressure.currentPressure)
    profiler.updateMemoryPressure(pressure)
    
    # Increment simulation
    step += 1
    time += 0.1
    
    # End step
    tracker.endStep(step, time)
    debugger.endStep(step, time)
    profiler.endStep(step, time)

# Generate comprehensive integration report
print("=== Memory Management Integration Report ===")

# Tracker report
tracker_report = tracker.generateReport()
print("Tracker Report:")
print(tracker_report[:500] + "..." if len(tracker_report) > 500 else tracker_report)

# Debug report
debug_report = debugger.generateReport()
print("\nDebug Report:")
print(debug_report[:500] + "..." if len(debug_report) > 500 else debug_report)

# Profiler report
profiler_report = profiler.generateProfileReport()
print("\nProfiler Report:")
print(profiler_report[:500] + "..." if len(profiler_report) > 500 else profiler_report)

# Save all reports
tracker.saveReport("integration_tracker.txt")
debugger.saveReport("integration_debugger.txt")
profiler.saveProfileReport("integration_profiler.txt")

print("\nReports saved successfully!")
```

## Available Classes

### Core Classes

#### NeuronId
- Represents neuron identifiers
- Used throughout the framework for neuron references

#### SynapseId
- Represents synapse identifiers
- Used for synapse references and connections

#### RegionId
- Represents brain region identifiers
- Used for regional organization

#### PopulationId
- Represents neuron population identifiers
- Used for population-based operations

### Memory Management Classes

#### MemoryTracker
- Core memory tracking and monitoring
- Real-time memory usage monitoring
- Memory allocation/deallocation tracking
- Memory efficiency analysis
- Memory leak detection
- Memory bandwidth and cache efficiency

#### MemoryDebug
- Memory debugging utilities
- Memory corruption detection
- Memory block lifecycle monitoring
- Memory state inspection
- Memory leak detection
- Cross-platform debugging support

#### MemoryProfiler
- Memory profiling framework
- Memory hierarchy analysis
- Memory pressure monitoring
- Memory optimization recommendations
- Integration with NLM systems

### Configuration Classes

#### Config
- Enhanced configuration class
- JSON and key=value format support
- Schema validation
- Preset configurations (basic, experimental, production, debug)
- Built-in quick access methods

### Memory Type Enumerations

#### AllocationType
- Standard
- Pool
- ThreadLocal
- SIMD
- Event
- Simulation

#### MemoryCorruptionType
- BufferOverflow
- BufferUnderflow
- DoubleFree
- UseAfterFree
- MemoryLeak
- InvalidPointer
- StackCorruption
- HeapCorruption
- AlignmentError
- PoolCorruption

#### MemoryLevel
- L1_CACHE
- L2_CACHE
- L3_CACHE
- RAM
- DISK

#### ProfileType
- AllocationProfile
- DeallocationProfile
- AccessProfile
- PoolProfile
- ThreadProfile

### Memory Data Structures

#### AllocationRecord
- Timestamp of allocation
- Memory address
- Allocation size
- Allocation site
- Stack trace
- Allocation type
- Thread ID

#### DeallocationRecord
- Timestamp of deallocation
- Memory address
- Deallocation site
- Thread ID
- Was tracked flag

#### MemoryBlockInfo
- Start address
- Size
- Type
- Description
- Thread ID
- Associated allocations
- Last access time

#### MemoryEfficiencyMetrics
- Allocation efficiency
- Fragmentation ratio
- Total allocated/deallocated
- Peak/current memory usage
- Active allocations
- Average allocation size
- Bandwidth utilization
- Cache efficiency

#### MemoryLeakDetection
- Leak type
- Address
- Size
- Allocation site
- Stack trace
- Thread ID
- Time since allocation
- Confidence score

#### MemoryAccessInfo
- Access timestamp
- Memory address
- Memory level
- Thread ID
- Read/write access
- Instruction access
- Access latency
- Access size
- Call site

#### AllocationProfile
- Allocation timestamp
- Memory address
- Allocation size
- Allocation site
- Stack trace
- Thread ID
- Future accesses
- Expected lifetime
- Expected memory level
- Pool allocation flag
- Pool name

#### DeallocationProfile
- Deallocation timestamp
- Memory address
- Deallocation site
- Thread ID
- Allocation profile

#### MemoryPoolProfile
- Pool name
- Total capacity
- Current usage
- Peak usage
- Allocation count
- Deallocation count
- Block sizes
- Efficiency
- Time series
- Tuning recommendations

#### MemoryHierarchyStats
- Memory level
- Total size
- Used size
- Available size
- Hit count
- Miss count
- Hit rate
- Average latency
- Size distribution

#### MemoryPressure
- Current pressure
- Pressure threshold
- Available memory
- Total memory
- Pressure increase rate
- Pressure history
- Alerts
- Critical flag

#### MemoryOptimization
- Optimization type
- Potential gain
- Description
- Auto-fixable flag
- Implementation plan
- Implementation cost

#### ProfileSession
- Session name
- Start time
- End time
- Step count
- Memory accesses
- Allocations
- Deallocations
- Pool profiles
- Hierarchy stats
- Memory pressure

## Module Functions

### Core Functions

#### createDefaultConfig()
Creates a default configuration.

#### createQuickBrain(configName = "basic")
Creates a brain with predefined configuration.

#### loadBrainCheckpoint(filepath, config = None)
Loads a brain checkpoint.

### Memory Management Functions

#### MemoryTracker functions
- enable(enable = True)
- isEnabled()
- trackAllocation(address, size, allocationSite, type, stackTrace)
- trackDeallocation(address, deallocationSite)
- getCurrentMemoryUsage()
- getTotalMemoryAllocated()
- getTotalMemoryDeallocated()
- getEfficiencyMetrics()
- detectMemoryLeaks()
- startStep(step, time)
- endStep(step, time)
- generateReport()
- saveReport(filename)

#### MemoryDebug functions
- enable(enable = True)
- isEnabled()
- setConfig(config)
- getConfig()
- debugBlock(address, size, site)
- trackAllocation(address, size, site, threadId = 0)
- trackDeallocation(address, site, threadId = 0)
- getBlockDebugInfo(address)
- isBlockCorrupted(address)
- detectCorruption(address, size)
- findUnfreedBlocks()
- findDoubleFrees()
- findBufferOverflows()
- findUseAfterFree()
- generateReport()
- saveReport(filename)
- getDebugRecommendations()
- inspectMemoryState()
- analyzeMemoryHealth()
- startStep(step, time)
- endStep(step, time)

#### MemoryProfiler functions
- enable(enable = True)
- isEnabled()
- setProfileType(type)
- setProfileInterval(interval)
- setPressureThreshold(threshold)
- recordAccess(address, level, threadId, isRead, isInstruction, latency, size, callSite)
- recordAllocation(address, size, site, stackTrace, threadId)
- recordDeallocation(address, site, threadId)
- recordPoolUsage(poolName, allocated, available)
- recordPoolAllocation(poolName, size)
- recordPoolDeallocation(poolName, size)
- updateMemoryHierarchy(stats)
- updateMemoryPressure(pressure)
- startProfile(sessionName)
- endProfile(sessionName)
- getCurrentProfile()
- getProfileHistory()
- getOptimizationRecommendations()
- generateProfileReport()
- saveProfileReport(filename)
- saveOptimizationReport(filename)
- getPerformanceAlerts()
- startStep(step, time)
- endStep(step, time)

## Configuration Options

### MemoryTracker Configuration
- enable: Enable/disable memory tracking
- maxHistorySize: Maximum history size
- samplingRate: Sampling rate (0.0-1.0)

### MemoryDebug Configuration
- enableStackTrace: Enable stack trace capture
- enablePatternTracking: Enable pattern tracking
- enableCorruptionDetection: Enable corruption detection
- enableAccessTracking: Enable access tracking
- enableSizeTracking: Enable size tracking
- corruptionDetectionThreshold: Corruption detection threshold
- maxAccessHistorySize: Maximum access history size
- debugOutputFile: Debug output file

### MemoryProfiler Configuration
- enable: Enable/disable profiling
- profileType: Profile type
- profileInterval: Profile interval (seconds)
- pressureThreshold: Pressure threshold (0.0-1.0)

## Performance Considerations

### Zero-Overhead Operation
- When disabled, all memory management systems have minimal performance impact
- Profiling can be selectively enabled
- Integration with existing performance monitoring

### Memory Overhead
- MemoryTracker: Minimal when disabled
- MemoryDebug: Additional usage for tracking
- MemoryProfiler: Additional usage for profiling data

### Thread Safety
- All memory management classes are thread-safe
- Use of mutexes and atomic operations
- Integration with existing thread-safe infrastructure

## Error Handling

The module raises the following exceptions:
- RuntimeError: For runtime errors
- ValueError: For invalid parameter values
- KeyError: For missing keys
- TypeError: For type errors

## Compatibility

### Python Versions
- Python 3.6+
- Python 3.7+ (recommended)

### Dependencies
- pybind11 >= 2.6.0
- NLM framework

## Building

### Requirements
- CMake 3.10+
- C++11 compiler
- pybind11
- NLM framework

### Build Instructions
```bash
mkdir build
cd build
cmake ..
make
```

### Install
```bash
sudo make install
```

## Testing

### Unit Tests
```bash
make test
```

### Python Tests
```bash
pytest tests/
```

## Troubleshooting

### Common Issues

1. **Module not found**
   - Ensure the module is properly installed
   - Check Python path
   - Verify dependencies

2. **Import errors**
   - Ensure pybind11 is properly installed
   - Check for compilation errors
   - Verify module structure

3. **Memory management issues**
   - Check configuration
   - Verify integration
   - Monitor performance

4. **Memory leaks**
   - Use memory debugging tools
   - Check for unfreed allocations
   - Review memory management code

## Support

For support, visit the NLM framework documentation or contact the NLM team.

## License

This module is part of the NLM (Neural Learning Machine) framework.
See LICENSE file for details.

## Author

NLM Framework Team

## Version

1.0.0

Generated: 2026-09-25