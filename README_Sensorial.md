Vision
=====

The VisionProcessor (VisionProcessor) extracts meaningful features from raw vision data (16x16 grid) using realistic but simple computational techniques:

**Key Features:**
- **Center of Mass**: Calculates the weighted average position of visual intensity, normalized to [0,1] range
- **Edge Orientation**: Simple Sobel-like edge detection that computes gradients and orientations
- **Motion Detection**: Compares current frame with previous frame to compute motion energy
- **Frequency Domain**: Uses DFT approximation on center row to extract spectral characteristics
- **Statistical Compression**: Reduces 256 raw values to 32 meaningful features via statistical analysis

**Implementation Details:**
- Processes grayscale intensity from RGB data (first channel only)
- Uses efficient sampling for large datasets (limited to 10,000 samples)
- Maintains temporal memory for motion detection
- Provides debug output for observability

**Output**: 32-dimensional feature vector representing visual scene

---

InternalSignalsProcessor
========================

The InternalSignalsProcessor transforms internal homeostatic signals (energy, health, movement, turning) into meaningful state variables:

**Key Features:**
- **State Variables**: Direct representation of energy, health, movement, turning rates
- **Psychological Metrics**: Stress, motivation, and homeostasis calculations
- **Temporal Dynamics**: Time derivatives and trend analysis
- **State Quality**: Stability, recovery rate, and resilience metrics

**Implementation Details:**
- Uses sliding window history for trend analysis
- Computes homeostatic signals (allostatic load, metabolic state)
- Calculates psychological states (stress, motivation)
- Provides real-time debug output
- Maintains 100-sample history for trend detection

**Output**: 32-dimensional feature vector representing internal state

---

AudioProcessor
=============

The AudioProcessor processes audio signals to extract meaningful acoustic features:

**Key Features:**
- **Spectral Analysis**: DFT-based frequency analysis with dominant frequency detection
- **Temporal Features**: Zero-crossing rate, energy, rhythm patterns
- **Timbre Features**: Frequency band energy distribution
- **Temporal Dynamics**: Motion between consecutive frames
- **Statistical Compression**: Reduces raw samples to 16 meaningful features

**Implementation Details:**
- Uses 1024-sample DFT for spectral analysis
- Computes spectral flatness for noise characterization
- Calculates zero-crossing rate for rhythm detection
- Implements group downsampling for efficiency
- Maintains previous frame for temporal comparison
- Provides debug output for monitoring

**Output**: 16-dimensional feature vector representing audio characteristics

---

Integration Benefits
====================

These processors provide:
- **Real-time observability** through console debug output
- **Memory-efficient** processing with streaming capabilities
- **Feature compression** reducing data dimensionality significantly
- **Temporal awareness** with motion and trend detection
- **Noise-robust** feature extraction
- **Neuro-instructive** information for downstream neural processing

The implementations focus on computing meaningful statistics rather than requiring full ML libraries, making them suitable for embedded or resource-constrained environments.

### Build and Run Instructions

**Prerequisites:** C++ compiler with C++11 support (g++, clang, MSVC)

**Build:**
```bash
# From project root
mkdir build
cmplex build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -- -j$(nproc)

# Or using Makefile directly
make -j$(nproc)
```

**Run:**
```bash
# After building
./build/main      # or ./nlm if built directly

# With custom config file
./build/main --config my_config.cfg
```

**Running Tests:**
```bash
# Build tests
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build build -- -j$(nproc)

# Run all tests
ctest --output-on-failure

# Or specific tests
./build/test_brain
./build/test_neuron
./build/test_stdp
```

### Key Features

1. **Real-time Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spikes
2. **Plasticity**: STDP and Hebbian learning rules for synaptic modification
3. **Structural Plasticity**: Synaptogenesis and pruning mechanisms
4. **Sensory Processing**: Vision, audio, and internal signals processing
5. **Behavioral Control**: Action selection and motor control
6. **Memory Systems**: Episodic and working memory
7. **Planning**: Cognitive concept formation and neural planning
8. **Development**: Synaptogenesis and pruning over time
9. **Learning**: Experience-driven neural changes
10. **Visualization**: Interactive system monitoring

### Configuration

The system uses JSON configuration files. Sample configs:
- `configs/default.cfg` - Basic configuration
- `configs/debug.cfg` - Debug mode with verbose output
- `configs/performance.cfg` - Optimized for performance

Override configuration via command line:
```bash
./build/main --neuron_count=1000 --connection_probability=0.2
```

### Usage Example

```cpp
// Create and run the NLM system
auto config = std::make_shared<Config>();
config->set("neuron_count", 500);
config->set("region_count", 3);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Run simulation
for (SimulationStep step = 0; step < 1000; ++step) {
    brain->step(step, step * 0.001);
    
    // Process sensory inputs
    VisionProcessor vision;
    vision.process(currentVision);
    
    AudioProcessor audio;
    audio.process(currentAudio);
    
    // Update internal state
    InternalSignalsProcessor internal;
    internal.process(brain->getInternalSignals());
    
    // Generate actions
    brain->generateAction();
}
```

### Files Modified

- `src/sensory/Vision.cpp`: Implemented real vision processing
- `src/sensory/InternalSignals.cpp`: Implemented real internal signals processing
- `src/sensory/Audio.cpp`: Implemented real audio processing

All three placeholder systems now provide realistic sensory processing capabilities with meaningful feature extraction, debug output, and efficient computation.
