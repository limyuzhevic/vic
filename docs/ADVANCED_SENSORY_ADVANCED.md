# SensoryInputAdvanced Documentation

## Overview

This document provides comprehensive documentation for the SensoryInputAdvanced class, which offers advanced Python APIs for the SensoryInput class, including sensory statistics, input quality, and signal processing APIs.

## Introduction

The SensoryInputAdvanced class provides enhanced access to SensoryInput functionality through Python-friendly APIs, including comprehensive sensory statistics, input quality assessment, and signal processing capabilities.

## Key Features

### Sensory Statistics
- **Sensory Input Analysis**: Analyze sensory input data and characteristics
- **Input Quality Assessment**: Assess quality of sensory input
- **Signal-to-Noise Ratio**: Calculate signal-to-noise ratio
- **Sensory Load**: Measure sensory processing load

### Vision APIs
- **Visual Field Statistics**: Analyze visual field characteristics
- **Visual Acuity**: Measure visual acuity
- **Visual Sensitivity**: Measure visual sensitivity
- **Visual Processing Speed**: Measure visual processing speed

### Audio APIs
- **Audio Statistics**: Analyze audio characteristics
- **Sound Clarity**: Measure sound clarity
- **Audio Frequency Range**: Analyze audio frequency range
- **Audio Processing Speed**: Measure audio processing speed

### Internal Signals APIs
- **Internal Signals Statistics**: Analyze internal signals
- **Signal Strength**: Measure signal strength
- **Signal Reliability**: Measure signal reliability
- **Internal Processing Speed**: Measure internal processing speed

### Percept Operations
- **Percept Creation**: Create sensory percepts from components
- **Signal Retrieval**: Get all signals from percepts
- **Data Management**: Manage sensory input data

## API Reference

### Constructor
```python
def __init__(self, sensory_input):
    """Initialize SensoryInputAdvanced with a SensoryInput object
    
    Args:
        sensory_input: The SensoryInput object to wrap
    """
```

### Basic Sensory Input Operations

#### Core Operations

**`getType()`**
```python
def getType(self):
    """Get sensory input type
    
    Returns:
        str: Sensory input type
    """
```

**`getData()`**
```python
def getData(self):
    """Get the raw data as a vector
    
    Returns:
        list: Raw sensory data vector
    """
```

**`getDimensions()`**
```python
def getDimensions(self):
    """Get the dimensionality
    
    Returns:
        int: Dimensionality of sensory data
    """
```

**`getTimestamp()`**
```python
def getTimestamp(self):
    """Get the timestamp
    
    Returns:
        float: Timestamp
    """
```

**`setTimestamp(timestamp)`**
```python
def setTimestamp(self, timestamp):
    """Set the timestamp
    
    Args:
        timestamp (float): Timestamp to set
    """
```

### Sensory APIs

#### Sensory Statistics

**`getSensoryStats()`**
```python
def getSensoryStats(self):
    """Get sensory statistics
    
    Returns:
        dict: Dictionary with sensory statistics:
            - type (str): Sensory input type
            - dimensions (int): Dimensionality
            - data_size (int): Size of data vector
            - timestamp (float): Timestamp
            - data_range (dict): Data range (min, max, mean)
            - quality_score (float): Quality score (0.0 to 1.0)
            - signal_noise_ratio (float): Signal-to-noise ratio
            - sensory_load (float): Sensory load (0.0 to 1.0)
    """
```

**`getInputQuality()`**
```python
def getInputQuality(self):
    """Get input quality
    
    Returns:
        float: Input quality (0.0 to 1.0)
    """
```

**`getSignalNoiseRatio()`**
```python
def getSignalNoiseRatio(self):
    """Get signal-to-noise ratio
    
    Returns:
        float: Signal-to-noise ratio
    """
```

**`getSensoryLoad()`**
```python
def getSensoryLoad(self):
    """Get sensory load
    
    Returns:
        float: Sensory load (0.0 to 1.0)
    """
```

### Vision APIs

#### Visual Field Statistics

**`getVisualFieldStats()`**
```python
def getVisualFieldStats(self):
    """Get visual field statistics
    
    Returns:
        dict: Dictionary with visual field statistics:
            - field_of_view (float): Field of view (degrees)
            - resolution (dict): Resolution (width, height)
            - bit_depth (int): Bit depth
            - frame_rate (float): Frame rate (Hz)
            - pixel_count (int): Pixel count
            - visual_acuity (float): Visual acuity (0.0 to 1.0)
            - visual_sensitivity (float): Visual sensitivity (0.0 to 1.0)
            - visual_processing_speed (float): Visual processing speed (milliseconds per frame)
    """
```

**`getVisualAcuity()`**
```python
def getVisualAcuity(self):
    """Get visual acuity
    
    Returns:
        float: Visual acuity (0.0 to 1.0)
    """
```

**`getVisualSensitivity()`**
```python
def getVisualSensitivity(self):
    """Get visual sensitivity
    
    Returns:
        float: Visual sensitivity (0.0 to 1.0)
    """
```

**`getVisualProcessingSpeed()`**
```python
def getVisualProcessingSpeed(self):
    """Get visual processing speed
    
    Returns:
        float: Visual processing speed (milliseconds per frame)
    """
```

### Audio APIs

#### Audio Statistics

**`getAudioStats()`**
```python
def getAudioStats(self):
    """Get audio statistics
    
    Returns:
        dict: Dictionary with audio statistics:
            - sample_rate (int): Sample rate (Hz)
            - channels (int): Number of channels
            - bit_depth (int): Bit depth
            - duration (float): Duration (seconds)
            - sample_count (int): Sample count
            - frequency_range (dict): Frequency range (min, max)
            - sound_clarity (float): Sound clarity (0.0 to 1.0)
            - audio_processing_speed (float): Audio processing speed (milliseconds per buffer)
    """
```

**`getSoundClarity()`**
```python
def getSoundClarity(self):
    """Get sound clarity
    
    Returns:
        float: Sound clarity (0.0 to 1.0)
    """
```

**`getAudioFrequencyRange()`**
```python
def getAudioFrequencyRange(self):
    """Get audio frequency range
    
    Returns:
        dict: Dictionary with frequency range:
            - min (float): Minimum frequency (Hz)
            - max (float): Maximum frequency (Hz)
    """
```

**`getAudioProcessingSpeed()`**
```python
def getAudioProcessingSpeed(self):
    """Get audio processing speed
    
    Returns:
        float: Audio processing speed (milliseconds per buffer)
    """
```

### Internal Signals APIs

#### Internal Signals Statistics

**`getInternalStats()`**
```python
def getInternalStats(self):
    """Get internal signals statistics
    
    Returns:
        dict: Dictionary with internal signals statistics:
            - signal_count (int): Number of signals
            - signal_range (dict): Signal range (min, max)
            - update_rate (float): Update rate (Hz)
            - signal_strength (float): Signal strength (0.0 to 1.0)
            - signal_reliability (float): Signal reliability (0.0 to 1.0)
            - internal_processing_speed (float): Internal processing speed (Hz)
    """
```

**`getSignalStrength()`**
```python
def getSignalStrength(self):
    """Get signal strength
    
    Returns:
        float: Signal strength (0.0 to 1.0)
    """
```

**`getSignalReliability()`**
```python
def getSignalReliability(self):
    """Get signal reliability
    
    Returns:
        float: Signal reliability (0.0 to 1.0)
    """
```

**`getInternalProcessingSpeed()`**
```python
def getInternalProcessingSpeed(self):
    """Get internal processing speed
    
    Returns:
        float: Internal processing speed (Hz)
    """
```

### Percept Operations

#### Percept Creation

**`createPercept(vision=None, touch=None, internal=None, proprioception=None, audio=None)`**
```python
def createPercept(self, vision=None, touch=None, internal=None, proprioception=None, audio=None):
    """Create sensory percept from components
    
    Args:
        vision: Vision component
        touch: Touch component
        internal: Internal signals component
        proprioception: Proprioception component
        audio: Audio component
    
    Returns:
        SensoryPercept: Created sensory percept
    """
```

**`getAllSignals(percpt)`**
```python
def getAllSignals(self, percept):
    """Get all signals from percept
    
    Args:
        percept: Sensory percept
    
    Returns:
        list: List of all signals from percept
    """
```

### Python Integration APIs

#### Version and Information APIs

**`getPythonBindingsVersion()`**
```python
def getPythonBindingsVersion(self):
    """Get Python bindings version
    
    Returns:
        str: Python bindings version
    """
```

**`getPythonBindingsInfo()`**
```python
def getPythonBindingsInfo(self):
    """Get Python bindings information
    
    Returns:
        dict: Python bindings information including version, platform, python version, and features
    """
```

### Resource Monitoring APIs

#### Memory Usage APIs

**`getMemoryUsage()`**
```python
def getMemoryUsage(self):
    """Get memory usage
    
    Returns:
        int: Memory usage in bytes
    """
```

### Export APIs

#### Sensory Data Export

**`exportSensoryData(filepath)`**
```python
def exportSensoryData(self, filepath):
    """Export sensory data to file
    
    Args:
        filepath (str): Path to export file
    
    Returns:
        str: Export result message
    """
```

**`importSensoryData(filepath)`**
```python
def importSensoryData(self, filepath):
    """Import sensory data from file
    
    Args:
        filepath (str): Path to import file
    
    Returns:
        str: Import result message
    """
```

## Usage Examples

### Basic Sensory Input Usage

```python
from nlm import SensoryInputAdvanced

# Create and initialize sensory input
sensory = SensoryInputAdvanced(sensory_input)

# Get sensory input information
print(f"Input Type: {sensory.getType()}")
print(f"Dimensions: {sensory.getDimensions()}")
print(f"Timestamp: {sensory.getTimestamp()}")
```

### Sensory Statistics Usage

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get sensory statistics
stats = sensory.getSensoryStats()
print("Sensory Statistics:")
print(f"  Type: {stats['type']}")
print(f"  Dimensions: {stats['dimensions']}")
print(f"  Data Size: {stats['data_size']}")
print(f"  Quality Score: {stats['quality_score']:.3f}")
print(f"  Signal-to-Noise Ratio: {stats['signal_noise_ratio']:.3f}")
print(f"  Sensory Load: {stats['sensory_load']:.3f}")
```

### Vision APIs Usage

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get visual field statistics
visual_stats = sensory.getVisualFieldStats()
print("Visual Field Statistics:")
print(f"  Field of View: {visual_stats['field_of_view']:.3f} degrees")
print(f"  Resolution: {visual_stats['resolution']['width']}x{visual_stats['resolution']['height']}")
print(f"  Frame Rate: {visual_stats['frame_rate']:.3f} Hz")
print(f"  Visual Acuity: {visual_stats['visual_acuity']:.3f}")
print(f"  Visual Sensitivity: {visual_stats['visual_sensitivity']:.3f}")
```

### Audio APIs Usage

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get audio statistics
audio_stats = sensory.getAudioStats()
print("Audio Statistics:")
print(f"  Sample Rate: {audio_stats['sample_rate']:.3f} Hz")
print(f"  Channels: {audio_stats['channels']}")
print(f"  Bit Depth: {audio_stats['bit_depth']}")
print(f"  Duration: {audio_stats['duration']:.3f} s")
print(f"  Sound Clarity: {audio_stats['sound_clarity']:.3f}")

# Get audio frequency range
freq_range = sensory.getAudioFrequencyRange()
print(f"  Frequency Range: {freq_range['min']:.3f} - {freq_range['max']:.3f} Hz")
```

### Internal Signals APIs Usage

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get internal signals statistics
internal_stats = sensory.getInternalStats()
print("Internal Signals Statistics:")
print(f"  Signal Count: {internal_stats['signal_count']}")
print(f"  Signal Range: {internal_stats['signal_range']}")
print(f"  Update Rate: {internal_stats['update_rate']:.3f} Hz")
print(f"  Signal Strength: {internal_stats['signal_strength']:.3f}")
print(f"  Signal Reliability: {internal_stats['signal_reliability']:.3f}")
```

### Percept Operations Usage

```python
from nlm import SensoryInputAdvanced

sensory = SensoryInputAdvanced(sensory_input)

# Create percept from components
vision = {'type': 'vision', 'data': [0.1, 0.2, 0.3]}
internal = {'type': 'internal', 'data': [0.4, 0.5, 0.6]}
percept = sensory.createPercept(vision=vision, internal=internal)

# Get all signals from percept
all_signals = sensory.getAllSignals(percept)
print(f"All Signals from Percept: {all_signals}")
```

### Quality Assessment Usage

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get input quality
quality = sensory.getInputQuality()
print(f"Input Quality: {quality:.3f}")

# Get signal-to-noise ratio
snr = sensory.getSignalNoiseRatio()
print(f"Signal-to-Noise Ratio: {snr:.3f}")

# Get sensory load
load = sensory.getSensoryLoad()
print(f"Sensory Load: {load:.3f}")

# Assess input quality
if quality > 0.8:
    print("Input quality is excellent.")
elif quality > 0.6:
    print("Input quality is good.")
elif quality > 0.4:
    print("Input quality is acceptable.")
else:
    print("Input quality is poor. Consider improving input quality.")
```

### Signal Analysis Usage

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get signal-to-noise ratio
snr = sensory.getSignalNoiseRatio()
if snr > 10.0:
    print("Signal quality is excellent.")
elif snr > 5.0:
    print("Signal quality is good.")
elif snr > 1.0:
    print("Signal quality is acceptable.")
else:
    print("Signal quality is poor. Consider improving signal quality.")

# Get sensory load
load = sensory.getSensoryLoad()
print(f"Sensory Load: {load:.3f}")

if load > 0.8:
    print("Sensory load is high. Consider reducing complexity.")
elif load > 0.6:
    print("Sensory load is moderate.")
else:
    print("Sensory load is low.")
```

## Performance Tips

### 1. Sensory Input Optimization

```python
sensory = SensoryInputAdvanced(sensory_input)

# Optimize sensory input quality
quality = sensory.getInputQuality()
if quality < 0.8:
    print("Input quality needs improvement.")

# Assess signal-to-noise ratio
snr = sensory.getSignalNoiseRatio()
if snr < 10.0:
    print("Signal-to-noise ratio needs improvement.")

# Assess sensory load
load = sensory.getSensoryLoad()
if load > 0.8:
    print("Sensory load is high. Consider reducing complexity.")
```

### 2. Vision APIs Optimization

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get visual field statistics
visual_stats = sensory.getVisualFieldStats()
print(f"Visual Acuity: {visual_stats['visual_acuity']:.3f}")
print(f"Visual Sensitivity: {visual_stats['visual_sensitivity']:.3f}")

# Optimize based on visual acuity
if visual_stats['visual_acuity'] < 0.8:
    print("Visual acuity needs improvement.")

# Optimize based on visual sensitivity
if visual_stats['visual_sensitivity'] < 0.8:
    print("Visual sensitivity needs improvement.")
```

### 3. Audio APIs Optimization

```python
sensory = SensoryInputAdvanced(sensory_input)

# Get audio statistics
audio_stats = sensory.getAudioStats()
print(f"Sound Clarity: {audio_stats['sound_clarity']:.3f}")

# Optimize based on sound clarity
if audio_stats['sound_clarity'] < 0.8:
    print("Sound clarity needs improvement.")
```

## Best Practices

### 1. Quality Assessment Best Practices

```python
sensory = SensoryInputAdvanced(sensory_input)

# Always assess input quality
quality = sensory.getInputQuality()
snr = sensory.getSignalNoiseRatio()
load = sensory.getSensoryLoad()

print(f"Input Quality: {quality:.3f}")
print(f"Signal-to-Noise Ratio: {snr:.3f}")
print(f"Sensory Load: {load:.3f}")

# Set quality thresholds
quality_threshold = 0.8
snr_threshold = 10.0
load_threshold = 0.8

if quality < quality_threshold:
    print("Input quality below threshold. Consider improvement.")

if snr < snr_threshold:
    print("Signal-to-noise ratio below threshold. Consider improvement.")

if load > load_threshold:
    print("Sensory load above threshold. Consider reduction.")
```

### 2. Percept Creation Best Practices

```python
from nlm import SensoryInputAdvanced

sensory = SensoryInputAdvanced(sensory_input)

# Always validate percept components before creation
vision = {'type': 'vision', 'data': [0.1, 0.2, 0.3]}
internal = {'type': 'internal', 'data': [0.4, 0.5, 0.6]}
audio = {'type': 'audio', 'data': [0.7, 0.8, 0.9]}

# Create percept with appropriate components
percept = sensory.createPercept(vision=vision, internal=internal, audio=audio)

# Validate percept
all_signals = sensory.getAllSignals(percept)
if all_signals:
    print(f"Percept created successfully with {len(all_signals)} signals.")
else:
    print("Percept creation failed. Check input components.")
```

### 3. Signal Analysis Best Practices

```python
sensory = SensoryInputAdvanced(sensory_input)

# Always analyze signal quality
snr = sensory.getSignalNoiseRatio()
quality = sensory.getInputQuality()
load = sensory.getSensoryLoad()

# Set analysis thresholds
snr_good = 10.0
snr_acceptable = 5.0
snr_poor = 1.0

if snr > snr_good:
    print("Signal quality is good.")
elif snr > snr_acceptable:
    print("Signal quality is acceptable.")
elif snr > snr_poor:
    print("Signal quality is poor.")
else:
    print("Signal quality is very poor.")

# Assess input quality
if quality > 0.8:
    print("Input quality is excellent.")
elif quality > 0.6:
    print("Input quality is good.")
elif quality > 0.4:
    print("Input quality is acceptable.")
else:
    print("Input quality is poor.")
```

## Troubleshooting

### Common Issues and Solutions

1. **Quality Assessment Issues**
   - **Issue**: Input quality assessment fails
   - **Solution**: Use `sensory.getInputQuality()` to assess input quality

2. **Signal Analysis Issues**
   - **Issue**: Signal analysis fails
   - **Solution**: Use `sensory.getSignalNoiseRatio()` to analyze signal quality

3. **Sensory Load Issues**
   - **Issue**: Sensory load is too high
   - **Solution**: Use `sensory.getSensoryLoad()` to assess sensory load

4. **Percept Creation Issues**
   - **Issue**: Percept creation fails
   - **Solution**: Use `sensory.createPercept()` to create percepts with appropriate components

5. **Export/Import Issues**
   - **Issue**: Sensory data export/import fails
   - **Solution**: Use `sensory.exportSensoryData()` and `sensory.importSensoryData()` for export/import

### Debug Commands

```python
sensory = SensoryInputAdvanced(sensory_input)

# Debug sensory input
print(f"Input Type: {sensory.getType()}")
print(f"Dimensions: {sensory.getDimensions()}")

# Debug sensory quality
print(f"Input Quality: {sensory.getInputQuality()}")
print(f"Signal-to-Noise Ratio: {sensory.getSignalNoiseRatio()}")
print(f"Sensory Load: {sensory.getSensoryLoad()}")

# Debug vision APIs
visual_stats = sensory.getVisualFieldStats()
print(f"Visual Acuity: {visual_stats['visual_acuity']}")
print(f"Visual Sensitivity: {visual_stats['visual_sensitivity']}")

# Debug audio APIs
audio_stats = sensory.getAudioStats()
print(f"Sound Clarity: {audio_stats['sound_clarity']}")
print(f"Frequency Range: {audio_stats['frequency_range']}")

# Debug internal signals APIs
internal_stats = sensory.getInternalStats()
print(f"Signal Strength: {internal_stats['signal_strength']}")
print(f"Signal Reliability: {internal_stats['signal_reliability']}")
```

## Migration Guide

### From Basic to Advanced API

If you're migrating from basic NLM Python bindings to advanced bindings, follow these steps:

1. **Upgrade Sensory Input Usage**
   ```python
   # Basic
   sensory = SensoryInput(sensory_input)
   
   # Advanced
   sensory = SensoryInputAdvanced(sensory_input)
   
   # Add advanced features
   stats = sensory.getSensoryStats()
   quality = sensory.getInputQuality()
   ```

2. **Upgrade Percept Usage**
   ```python
   # Basic
   percept = SensoryPercept()
   
   # Advanced
   percept = sensory.createPercept(vision, internal, audio)
   
   # Add advanced features
   signals = sensory.getAllSignals(percept)
   ```

3. **Upgrade Data Analysis Usage**
   ```python
   # Basic
   data = sensory.getData()
   
   # Advanced
   sensory = SensoryInputAdvanced(sensory_input)
   
   # Add advanced features
   stats = sensory.getSensoryStats()
   quality = sensory.getInputQuality()
   ```

## Conclusion

The SensoryInputAdvanced class provides comprehensive access to all sensory input systems through Python-friendly APIs. This documentation covers all major features including sensory statistics, input quality assessment, signal processing, vision APIs, audio APIs, internal signals APIs, and percept creation APIs.

By following the best practices outlined in this documentation, you can effectively use the SensoryInputAdvanced class to process sensory inputs with high accuracy and reliability.

## See Also

- [NLM Python Bindings Documentation](https://nlm.org/docs/python-bindings) - General Python bindings documentation
- [BrainAdvanced Documentation](https://nlm.org/docs/brain-advanced) - BrainAdvanced documentation
- [AgentBrainAdvanced Documentation](https://nlm.org/docs/agent-advanced) - AgentBrainAdvanced documentation
- [ConfigAdvanced Documentation](https://nlm.org/docs/config-advanced) - ConfigAdvanced documentation
- [WorldAdvanced Documentation](https://nlm.org/docs/world-advanced) - WorldAdvanced documentation
- [NLM Examples](https://nlm.org/examples) - Code examples and tutorials
- [NLM Community](https://nlm.org/community) - Community support and forums

---

*Last updated: 2025-01-01*  
*Version: 0.1.0-advanced*  
*Copyright: NLM Development Team*