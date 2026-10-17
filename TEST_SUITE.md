# NLM Test Suite
# Comprehensive Test Suite for Neural Learning Machine

## Overview
This test suite provides comprehensive coverage for the NLM (Neural Learning Machine) codebase, testing all major components including:
- Neural computation (LIF neurons)
- Plasticity rules (STDP, Hebbian)
- Memory systems (working memory, episodic memory)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction systems
- Cognitive functions (attention, planning, concept formation)
- Development system
- Integration testing
- Performance benchmarking

## Test Architecture

### Test Categories
1. **Unit Tests** - Individual component testing
2. **Integration Tests** - System-level testing
3. **Performance Tests** - Benchmarking and scalability testing
4. **Regression Tests** - Ensure existing functionality works
5. **Stress Tests** - Large-scale network testing

### Test Framework
- **Google Test** (or similar) - Unit testing framework
- **Custom test runners** - For integration and system tests
- **Benchmarking tools** - For performance testing
- **Memory leak detection** - Valgrind, AddressSanitizer

## Test Structure

### 1. Core Neural Components
**File**: `tests/neuron_test.cpp`
**Coverage**:
- LIF neuron dynamics
- Membrane potential integration
- Spike generation and refractory periods
- Synaptic input handling
- State management
- Random initialization

**Test Examples**:
```cpp
TEST(NeuronTest, LIFDynamics) {
    Neuron neuron(NeuronId(1));
    neuron.initializeRandom(rng);
    
    // Test basic integration
    float initialPotential = neuron.getMembranePotential();
    neuron.injectCurrent(10.0f);
    neuron.step(0.0);
    
    EXPECT_GT(neuron.getMembranePotential(), initialPotential);
}

TEST(NeuronTest, SpikeGeneration) {
    Neuron neuron(NeuronId(1));
    neuron.setThreshold(-40.0f);
    neuron.setMembranePotential(-30.0f);
    
    bool fired = neuron.step(0.0);
    EXPECT_TRUE(fired);
    EXPECT_TRUE(neuron.isFiring());
}
```

### 2. Synapse and Connectivity
**File**: `tests/synapse_test.cpp`
**Coverage**:
- Synapse properties and behavior
- STDP plasticity
- Hebbian learning
- Short-term plasticity
- Synaptic weight management

**Test Examples**:
```cpp
TEST(SynapseTest, STDPPlasticity) {
    Synapse synapse(SynapseId(1), NeuronId(1), NeuronId(2));
    synapse.setType(SynapseType::Excitatory);
    
    // Simulate pre-post spike timing for LTP
    synapse.recordPreSpike(0.0f);
    synapse.recordPostSpike(5.0f);  // 5ms after pre (should induce LTP)
    
    // Note: This would test the actual STDP implementation
    // float initialWeight = synapse.getWeight();
    // synapse.update(STDP, preSpikes, postSpikes);
    // EXPECT_GT(synapse.getWeight(), initialWeight);
}
```

### 3. Plasticity Rules
**File**: `tests/plasticity_test.cpp`
**Coverage**:
- STDP (Spike-Timing-Dependent Plasticity)
- Hebbian learning
- Reward-modulated plasticity
- Triplet STDP
- Structural plasticity

**Test Examples**:
```cpp
TEST(PlasticityTest, HebbianLearning) {
    // Test Hebbian rule
    HebbianRule hebbian;
    
    // Create mock synapse
    auto synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
    
    // Record correlated activity
    std::vector<Timestamp> preSpikes = {0.0f, 10.0f};
    std::vector<Timestamp> postSpikes = {5.0f, 15.0f};  // Correlated
    
    hebbian.update(synapse.get(), preSpikes, postSpikes, 0.001f);
    
    // Expect weight increase due to correlation
    EXPECT_GT(synapse->getWeight(), 0.0f);
}
```

### 4. Brain System Integration
**File**: `tests/brain_test.cpp`
**Coverage**:
- Brain initialization
- Region and population management
- Network connectivity
- Simulation loop
- Memory system integration
- Neuromodulation effects

**Test Examples**:
```cpp
TEST(BrainTest, BasicConnectivity) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    Brain brain(config);
    EXPECT_TRUE(brain.initialize());
    
    // Test basic network properties
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_EQ(brain.getRegionCount(), 2);
}

TEST(BrainTest, LearningExperiment) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("simulation_timestep", 0.001);
    
    Brain brain(config);
    brain.initialize();
    
    // Apply learning stimulus
    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Check for learning (weight changes)
    float eiratio = brain.getExcitationInhibitionRatio();
    EXPECT_GT(eiratio, 0.0f);  // Should have some excitatory connectivity
}
```

### 5. Memory Systems
**File**: `tests/memory_test.cpp`
**Coverage**:
- Working memory
- Episodic memory
- Associative memory
- Memory consolidation
- Memory replay

**Test Examples**:
```cpp
TEST(WorkingMemoryTest, StorageAndRetrieval) {
    // Test working memory functionality
    // This would test NeuralWorkingMemory class
    
    // Store neural activity patterns
    // Retrieve patterns
    // Test capacity limits
}

TEST(EpisodicMemoryTest, EpisodeStorage) {
    // Test episodic memory storage
    // Test episode replay
    // Test consolidation
}
```

### 6. Neuromodulation
**File**: `tests/neuromodulation_test.cpp`
**Coverage**:
- Dopamine modulation
- Curiosity system
- Novelty detection
- Prediction error
- Reward prediction

**Test Examples**:
```cpp
TEST(DopamineTest, RewardModulation) {
    // Test dopamine response to rewards
    // Test plasticity modulation
    // Test learning rate adjustment
}

TEST(CuriosityTest, ExplorationDrive) {
    // Test curiosity-driven exploration
    // Test novelty detection
    // Test exploration-exploitation balance
}
```

### 7. Cognitive Functions
**File**: `tests/cognition_test.cpp`
**Coverage**:
- Attentional selection
- Neural planning
- Concept formation
- Self-model
- Social learning

**Test Examples**:
```cpp
TEST(AttentionTest, CompetitiveSelection) {
    // Test attentional selection
    // Test winner-take-all dynamics
    // Test top-down/bottom-up integration
}

TEST(PlanningTest, ActionEvaluation) {
    // Test action sequence evaluation
    // Test multi-step planning
    // Test outcome prediction
}
```

### 8. Development System
**File**: `tests/development_test.cpp`
**Coverage**:
- Developmental stages
- Synaptogenesis
- Pruning
- Maturation
- Critical periods

**Test Examples**:
```cpp
TEST(DevelopmentTest, Synaptogenesis) {
    // Test new synapse formation
    // Test age-dependent rates
    // Test activity-dependent refinement
}

TEST(DevelopmentTest, CriticalPeriods) {
    // Test plasticity changes with age
    // Test sensitive periods
    // Test stabilization
}
```

### 9. Performance Tests
**File**: `tests/performance_test.cpp`
**Coverage**:
- Network scalability
- Memory usage
- Simulation speed
- Plasticity computation
- I/O performance

**Test Examples**:
```cpp
TEST(PerformanceTest, NetworkScalability) {
    // Test performance with increasing neuron counts
    std::vector<size_t> neuronCounts = {100, 1000, 10000, 100000};
    
    for (size_t count : neuronCounts) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", count);
        
        Brain brain(config);
        auto startTime = std::chrono::high_resolution_clock::now();
        brain.initialize();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        // Check initialization time
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        EXPECT_LT(duration.count(), 1000.0);  // Should initialize within 1 second
    }
}
```

### 10. Integration Tests
**File**: `tests/integration_test.cpp`
**Coverage**:
- Complete brain loop
- Sensorimotor integration
- Learning and behavior
- Memory consolidation
- Multi-phase development

**Test Examples**:
```cpp
TEST(IntegrationTest, CompleteBrainLoop) {
    // Test the complete integrated brain system
    // This would test all components working together
    // Test from sensory input to motor output
}

TEST(IntegrationTest, LearningAndAdaptation) {
    // Test how the brain learns and adapts over time
    // Test memory formation and retrieval
    // Test behavioral adaptation
}
```

## Test Utilities

### Helper Classes
```cpp
class NLMTestFixture {
protected:
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("random_seed", 42);
        config->set("simulation_timestep", 0.001);
        return config;
    }
    
    std::shared_ptr<RandomGenerator> createRandomGenerator() {
        return std::make_shared<RandomGenerator>(42);
    }
};
```

### Test Data
```cpp
struct TestUtils {
    // Generate test neural activity patterns
    static std::vector<float> generateSensoryInput(size_t size) {
        std::vector<float> input(size);
        std::generate(input.begin(), input.end(), []() { return rand() / RAND_MAX; });
        return input;
    }
    
    // Check for convergence in learning
    static bool hasConverged(const std::vector<float>& weights, float tolerance = 0.001f) {
        return std::adjacent_find(weights.begin(), weights.end(),
            [tolerance](float a, float b) { return std::abs(a - b) > tolerance; }) == weights.end();
    }
};
```

## Test Configuration

### Build Configuration
```cmake
# CMake configuration for tests
enable_testing()

# Add test executables
add_executable(neuron_test tests/neuron_test.cpp)
add_executable(synapse_test tests/synapse_test.cpp)
add_executable(plasticity_test tests/plasticity_test.cpp)
add_executable(brain_test tests/brain_test.cpp)

# Link test executables with NLM libraries
target_link_libraries(neuron_test nlm_core)
target_link_libraries(synapse_test nlm_core)
target_link_libraries(plasticity_test nlm_core)
target_link_libraries(brain_test nlm_core)

# Add tests to CTest
add_test(NAME neuron_test COMMAND neuron_test)
add_test(NAME synapse_test COMMAND synapse_test)
add_test(NAME plasticity_test COMMAND plasticity_test)
add_test(NAME brain_test COMMAND brain_test)
```

### Test Environment
```bash
# Environment variables for testing
export NLM_TEST_SEED=42
export NLM_TEST_DURATION=60  # seconds
export NLM_TEST_NETWORK_SIZE=1000
export NLM_ENABLE_BENCHMARKS=true

# Run all tests
cd /path/to/nlm/build
./test_runner --gtest_output=xml:test_results.xml

# Run specific test category
./test_runner --gtest_filter="*Neur*:*Syn*"

# Run performance tests only
./test_runner --gtest_filter="*Perf*"
```

## Test Results Analysis

### Reporting
- **XML Reports**: For CI/CD integration
- **HTML Reports**: For human-readable summaries
- **Console Output**: For quick feedback
- **Performance Metrics**: Benchmark results

### Automated Analysis
```python
# Python script for test result analysis
import xml.etree.ElementTree as ET
import pandas as pd

def analyze_test_results(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    results = []
    for testsuite in root.findall('testsuite'):
        for testcase in testsuite.findall('testcase'):
            test_name = testcase.get('name')
            status = 'PASS' if 'failure' not in testcase.attrib else 'FAIL'
            time_taken = float(testcase.get('time', 0))
            
            results.append({
                'test_name': test_name,
                'status': status,
                'time_taken': time_taken
            })
    
    df = pd.DataFrame(results)
    
    # Generate summary
    print(f"Total tests: {len(df)}")
    print(f"Passed: {len(df[df['status'] == 'PASS'])}")
    print(f"Failed: {len(df[df['status'] == 'FAIL'])}")
    print(f"Average time: {df['time_taken'].mean():.3f}s")
    
    return df
```

## Continuous Integration

### GitHub Actions
```yaml
# .github/workflows/test.yml
name: NLM Tests
on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
    - name: Build Tests
      run: cmake --build build --target all
    - name: Run Tests
      run: cd build && ./test_runner --gtest_output=xml:test-results.xml
    - name: Upload Test Results
      uses: actions/upload-artifact@v2
      if: always()
      with:
        name: test-results
        path: build/test-results.xml
```

### Jenkins Pipeline
```groovy
pipeline {
    agent any
    
    stages {
        stage('Build') {
            steps {
                sh 'cmake -B build'
                sh 'cmake --build build'
            }
        }
        
        stage('Test') {
            steps {
                sh 'cd build && ./test_runner --gtest_output=xml:test-results.xml'
            }
        }
        
        stage('Analyze') {
            steps {
                sh 'python3 analyze_tests.py build/test-results.xml'
            }
        }
    }
    
    post {
        always {
            archiveArtifacts artifacts: 'build/test-results.xml', fingerprint: true
        }
    }
}
```

## Test Coverage

### Coverage Metrics
- **Statement Coverage**: % of statements executed
- **Branch Coverage**: % of branches taken
- **Function Coverage**: % of functions called
- **Integration Coverage**: System-level functionality

### Coverage Tools
```bash
# Install coverage tools
pip install coverage
pip install pytest-cov

# Run tests with coverage
pytest --cov=nlm --cov-report=html --cov-report=xml

# Generate coverage report
open htmlcov/index.html
```

## Performance Benchmarking

### Benchmark Categories
1. **Initialization Performance**
2. **Simulation Speed**
3. **Memory Usage**
4. **Plasticity Computation**
5. **Network Scalability**

### Benchmark Scripts
```python
import time
import matplotlib.pyplot as plt
import numpy as np

def benchmark_neuron_creation(neuron_counts):
    times = []
    
    for count in neuron_counts:
        start = time.time()
        # Create and initialize neurons
        # (Implementation-dependent)
        end = time.time()
        
        times.append(end - start)
    
    # Plot results
    plt.figure(figsize=(10, 6))
    plt.plot(neuron_counts, times, 'bo-')
    plt.xlabel('Number of Neurons')
    plt.ylabel('Initialization Time (s)')
    plt.title('Neuron Initialization Performance')
    plt.grid(True)
    plt.savefig('neuron_init_benchmark.png')
    
    return times

def benchmark_simulation(neuron_counts, simulation_steps):
    speeds = []
    
    for count in neuron_counts:
        start = time.time()
        # Run simulation
        # (Implementation-dependent)
        end = time.time()
        
        elapsed = end - start
        speed = simulation_steps / elapsed
        speeds.append(speed)
    
    return speeds
```

## Test Data Management

### Generated Test Data
```cpp
class TestDataGenerator {
public:
    // Generate random neural connectivity patterns
    static std::vector<std::pair<NeuronId, NeuronId>> generateRandomConnections(
        size_t neuronCount, float connectionProbability) {
        
        std::vector<std::pair<NeuronId, NeuronId>> connections;
        
        for (size_t i = 0; i < neuronCount; ++i) {
            for (size_t j = 0; j < neuronCount; ++j) {
                if (i != j && rand() / RAND_MAX < connectionProbability) {
                    connections.emplace_back(NeuronId(i), NeuronId(j));
                }
            }
        }
        
        return connections;
    }
    
    // Generate spike trains for STDP testing
    static std::vector<Timestamp> generateSpikeTrain(
        double duration, double frequency, double jitter = 0.0) {
        
        std::vector<Timestamp> spikes;
        double interval = 1.0 / frequency;
        double current_time = 0.0;
        
        while (current_time < duration) {
            double spike_time = current_time + jitter * (rand() / RAND_MAX - 0.5);
            spikes.push_back(static_cast<Timestamp>(spike_time));
            
            current_time = spike_time + interval + 
                jitter * (rand() / RAND_MAX - 0.5) * interval;
        }
        
        std::sort(spikes.begin(), spikes.end());
        return spikes;
    }
};
```

## Test Deployment

### Local Testing
```bash
# Clone test repository
cd /workspace/app
mkdir tests

# Copy test files
cp test_files/*.cpp tests/
cp test_files/*.hpp tests/

# Build and run tests
cd build
cmake ..
make -j4
./test_runner
```

### Cloud Testing
```bash
# Deploy to cloud using containers
docker build -t nlm-tests .
docker run -v $(pwd)/results:/results nlm-tests
```

### CI/CD Pipeline
```bash
# Run comprehensive test suite
./run_all_tests.sh

# Generate test reports
./generate_test_reports.sh

# Upload to test database
./upload_test_results.py
```

## Test Maintenance

### Regular Tasks
- **Review test coverage** quarterly
- **Update test data** with new scenarios
- **Optimize performance tests** for faster execution
- **Add edge case tests** for new features

### Automated Testing
- **Unit test automation** for new code
- **Integration test automation** for new features
- **Performance regression detection**
- **Memory leak detection**

### Test Update Process
1. **Add new tests** for new functionality
2. **Update existing tests** for improvements
3. **Remove obsolete tests** if functionality changes
4. **Update test data** if patterns change
5. **Update documentation** as needed

## Conclusion

This comprehensive test suite ensures the NLM codebase is reliable, maintainable, and performs well under various conditions. The tests cover all major components and provide a foundation for future development and maintenance.

The test suite enables:
- **Early bug detection**
- **Performance optimization**
- **Code quality assurance**
- **Documentation validation**
- **Continuous integration**
- **Automated testing**

This test infrastructure will be essential for maintaining the NLM codebase as it continues to evolve and expand its capabilities.