# Memory Management Issues - Specific File Locations

## Files Requiring Immediate Memory Management Improvements

### 1. Manual Memory Allocations (new/delete) without Smart Pointers

**src/brain/Brain.cpp**
- Line 160: `pImpl(new Impl(config))` - Manual allocation in constructor
- Line 170: `delete pImpl;` - Manual deletion in move assignment
- Line 162: `~Brain() = default;` - Destructor doesn't clean up pImpl

**src/brain/Neuron.cpp**
- Line 30: `pImpl(new Impl)` - Manual allocation in constructor  
- Line 46: `delete pImpl;` - Manual deletion in move assignment
- Line 38: `~Neuron() = default;` - Destructor doesn't clean up pImpl

**src/core/Random/Random.cpp**
- Line 13: `pImpl(new Impl(seed))` - Manual allocation in constructor
- Line 15: `pImpl(new Impl(std::random_device{}()))` - Manual allocation in constructor
- Line 25: `delete pImpl;` - Manual deletion in move assignment
- Line 17: `~RandomGenerator() = default;` - Destructor doesn't clean up pImpl

**src/core/SimulationClock/SimulationClock.cpp**
- Line 17: `pImpl(new Impl(timestep))` - Manual allocation in constructor
- Line 27: `delete pImpl;` - Manual deletion in move assignment
- Line 19: `~SimulationClock() = default;` - Destructor doesn't clean up pImpl

**src/brain/NeuralPopulation.cpp**
- Line 17: `pImpl(new Impl(id, size))` - Manual allocation in constructor
- Line 27: `delete pImpl;` - Manual deletion in move assignment

**src/brain/NeuralRegion.cpp**
- Line 19: `pImpl(new Impl(id))` - Manual allocation in constructor
- Line 33: `delete pImpl;` - Manual deletion in move assignment

**src/brain/Synapse.cpp**
- Line 50: `pImpl(new Impl)` - Manual allocation in constructor
- Line 73: `delete pImpl;` - Manual deletion in move assignment

**src/motor/MotorSystem.cpp**
- Line 24: `pImpl(new Impl)` - Manual allocation in constructor
- Line 51: `delete action;` - Manual deletion in destructor

**src/dynamics/SpikeSystem.cpp**
- Line 21: `pImpl(new Impl)` - Manual allocation in constructor
- Line 31: `delete pImpl;` - Manual deletion in move assignment

**src/neuromodulation/Reward.cpp**
- Line 13: `pImpl(new Impl)` - Manual allocation in constructor

**src/neuromodulation/PredictionError.cpp**
- Line 17: `pImpl(new Impl)` - Manual allocation in constructor

**src/neuromodulation/Novelty.cpp**
- Line 19: `pImpl(new Impl)` - Manual allocation in constructor

**src/neuromodulation/Neuromodulator.cpp**
- Line 16: `Dopamine() : pImpl(new Impl)` - Manual allocation in constructor

**src/neuromodulation/Curiosity.cpp**
- Line 21: `pImpl(new Impl)` - Manual allocation in constructor

**src/neuromodulation/PredictionError.cpp**
- Line 17: `pImpl(new Impl)` - Manual allocation in constructor

**src/neuromodulation/Novelty.cpp**
- Line 19: `pImpl(new Impl)` - Manual allocation in constructor

**src/sensory/Vision.cpp**
- Line 12: `pImpl(new Impl)` - Manual allocation in constructor

**src/sensory/InternalSignals.cpp**
- Line 12: `pImpl(new Impl)` - Manual allocation in constructor

**src/sensory/Audio.cpp**
- Line 12: `pImpl(new Impl)` - Manual allocation in constructor

**src/motor/MotorSystem.cpp**
- Line 24: `pImpl(new Impl)` - Manual allocation in constructor

**src/sensory/SensoryInput.cpp**
- Line 24: `pImpl(new Impl(0, 0, 0))` - Manual allocation in constructor
- Line 27: `: pImpl(new Impl(width, height, channels))` - Manual allocation in constructor
- Line 73: `pImpl(new Impl(0, 0))` - Manual allocation in constructor
- Line 76: `: pImpl(new Impl(sampleRate, numSamples))` - Manual allocation in constructor
- Line 117: `pImpl(new Impl)` - Manual allocation in constructor

**src/prediction/PredictionSystem.cpp**
- Line 13: `pImpl(new Impl)` - Manual allocation in constructor

**src/cognition/NeuralPlanner.cpp**
- Line 15: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 263: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 448: `: pImpl(new Impl)` - Manual allocation in constructor

**src/experiments/Experiment.cpp**
- Line 18: `pImpl(new Impl)` - Manual allocation in constructor
- Line 20: `: pImpl(new Impl)` - Manual allocation in constructor

**src/experiments/Metrics.cpp**
- Line 13: `pImpl(new Impl)` - Manual allocation in constructor

**src/memory/NeuralWorkingMemory.cpp**
- Line 21: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 257: `: pImpl(new Impl)` - Manual allocation in constructor

**src/brain/Synapse.cpp**
- Line 50: `: pImpl(new Impl)` - Manual allocation in constructor

**src/experiments/Phase3Experiment.cpp**
- Line 28: `: pImpl(new Impl(filename))` - Manual allocation in constructor

**src/cognition/ConceptFormation.cpp**
- Line 16: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 307: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 410: `: pImpl(new Impl)` - Manual allocation in constructor

**src/experiments/ExperimentRunner.cpp**
- Line 12: `pImpl(new Impl)` - Manual allocation in constructor

**src/memory/Memory.cpp**
- Line 14: `pImpl(new Impl(100))` - Manual allocation in constructor
- Line 75: `pImpl(new Impl(1000))` - Manual allocation in constructor
- Line 119: `pImpl(new Impl)` - Manual allocation in constructor
- Line 164: `pImpl(new Impl)` - Manual allocation in constructor
- Line 217: `pImpl(new Impl)` - Manual allocation in constructor

**src/memory/NeuralEpisodicMemory.cpp**
- Line 19: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 329: `: pImpl(new Impl)` - Manual allocation in constructor

**src/development/Synaptogenesis.cpp**
- Line 12: `pImpl(new Impl)` - Manual allocation in constructor

**src/development/Maturation.cpp**
- Line 18: `pImpl(new Impl)` - Manual allocation in constructor

**src/brain/Neuron.cpp**
- Line 30: `pImpl(new Impl)` - Manual allocation in constructor

**src/brain/NeuralPopulation.cpp**
- Line 17: `pImpl(new Impl(id, size))` - Manual allocation in constructor

**src/plasticity/StructuralPlasticity.cpp**
- Line 10: `float synaptogenesisRate;` - Comment about new synapses
- Line 29: `pImpl(new Impl)` - Manual allocation in constructor

**src/plasticity/STDP.cpp**
- Line 19: `pImpl(new Impl)` - Manual allocation in constructor

**src/environment/Environment.cpp**
- Line 11: `pImpl(new Impl)` - Manual allocation in constructor
- Line 42: `: pImpl(new Impl(width, height))` - Manual allocation in constructor

**src/brain/NeuralRegion.cpp**
- Line 19: `pImpl(new Impl(id))` - Manual allocation in constructor
- Line 21: `: pImpl(new Impl(id))` - Manual allocation in constructor

**src/prediction/NeuralPrediction.cpp**
- Line 31: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 325: `: pImpl(new Impl)` - Manual allocation in constructor
- Line 450: `: pImpl(new Impl)` - Manual allocation in constructor

**src/development/DevelopmentSystem.cpp**
- Line 16: `pImpl(new Impl), age_(0.0)` - Manual allocation in constructor

**src/development/Pruning.cpp**
- Line 12: `pImpl(new Impl)` - Manual allocation in constructor

**src/plasticity/Hebbian.cpp**
- Line 17: `pImpl(new Impl)` - Manual allocation in constructor

**src/plasticity/PlasticityRule.cpp**
- Line 19: `HebbianRule() : pImpl(new Impl)` - Manual allocation in constructor

**src/dynamics/SpikeSystem.cpp**
- Line 21: `pImpl(new Impl)` - Manual allocation in constructor

**src/dynamics/NeuralDynamics.cpp**
- Line 13: `pImpl(new Impl)` - Manual allocation in constructor

**src/experiments/Phase6IntegratedExperiment.cpp**
- Line 134: `// Create new brain and load` - Comment about dynamic creation
- Line 347: `// Create new brain and load` - Comment about dynamic creation

### 2. Resource Management Issues

**src/core/Config/Config.cpp**
- Line 38: `std::ifstream file(filepath);` - File handle created but not explicitly managed
- Line 104: `std::ofstream file(filepath);` - File handle created but not explicitly managed
- Line 140: `std::ifstream file(filepath);` - File handle created but not explicitly managed
- Line 146: `file.close();` - Explicit close (good practice)

**src/core/Logger/Logger.cpp**
- Line 13: `std::ofstream fileOut;` - File handle member without explicit cleanup
- Line 74: `pImpl->fileOut.open(filepath, std::ios::app);` - File opened
- Line 79: `pImpl->fileOut.close();` - Explicit close method available

**src/experiments/Phase3Experiment.cpp**
- Line 10: `std::ofstream file;` - File handle member
- Line 14: `file.open(filename);` - File opened in constructor
- Line 22: `file.close();` - Explicit close

### 3. Potential Smart Pointer Issues

**src/experiments/Phase5Experiment.cpp**
- Line 76: `std::function<std::shared_ptr<Brain>(const LifetimeExperimentConfig& config)> createBrain,` - Shared pointer in function parameter
- Line 191: `std::shared_ptr<Brain> brain,` - Shared pointer member
- Line 228: `std::shared_ptr<Brain> brain,` - Shared pointer member
- Line 404: `std::function<std::shared_ptr<Brain>(const LifetimeExperimentConfig& config, uint64_t seed)> createBrain,` - Shared pointer in function

### 4. Circular Reference Potential

No obvious circular reference patterns found in the codebase.

### 5. Memory Leak Indicators

**src/experiments/Phase6IntegratedExperiment.cpp**
- Line 134: `// Create new brain and load` - Comment indicates potential dynamic allocation patterns
- Line 347: `// Create new brain and load` - Comment indicates potential dynamic allocation patterns

## Files Requiring Most Urgent Attention

1. **src/brain/Brain.cpp** (1117 lines) - Core class with pImpl pattern
2. **src/brain/Neuron.cpp** (302 lines) - Critical component class
3. **src/core/Random/Random.cpp** (134 lines) - Core utility class
4. **src/core/SimulationClock/SimulationClock.cpp** (110 lines) - Core timing class
5. **src/core/Logger/Logger.cpp** (197 lines) - File I/O operations
6. **src/core/Config/Config.cpp** (566 lines) - Configuration file operations

## Summary

**~60+ files** found with manual memory management patterns using pImpl with `new` and `delete`.

**Key Issues:**
- Manual pImpl allocations (~60 occurrences)
- Missing destructor implementations
- File handle management inconsistencies
- Potential resource leaks in long-running experiments

**Recommendation:** Replace all `pImpl(new Impl(...))` with `pImpl(std::make_unique<Impl>(...))` and ensure proper destructor implementations.
