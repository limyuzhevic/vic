# NLM — Advanced Usage Guide

**Comprehensive documentation for advanced users and researchers**

## Introduction

This guide covers advanced usage patterns, configuration options, and research applications for NLM Phase 6. It builds on the beginner guide (`easy_usage.md`) and provides deeper insights into system capabilities and optimization strategies.

## Advanced Configuration

### 1. Custom Brain Configuration

NLM allows fine-grained control over brain architecture and behavior:

```cpp
// Example: Custom brain configuration
#include "nlm/core/Config/Config.hpp"

auto config = std::make_shared<Config>();

// Basic configuration
config->set("neuron_count", 2000);
config->set("region_count", 3);
config->set("connection_probability", 0.08f);
config->set("simulation_timestep", 0.0005);

// Neuromodulation parameters
config->set("stdp_ltp_weight", 0.015f);
config->set("stdp_ltd_weight", 0.018f);
config->set("novelty_threshold", 0.3f);
config->set("curiosity_multiplier", 2.0f);

// Memory system parameters
config->set("working_memory_capacity", 50);
config->set("episodic_memory_limit", 1000);
config->set("memory_consolidation_interval", 100);

// Development parameters
config->set("synaptogenesis_rate", 0.0002f);
config->set("pruning_rate", 0.00003f);
config->set("critical_period_end", 500.0);
```

### 2. Advanced Agent Configuration

```cpp
// Enable and configure advanced agent features
void setupAdvancedAgent(AgentBrain& agent) {
    // Enable all advanced features
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Configure neuromodulation parameters
    agent.setDopamineSensitivity(1.0f);
    agent.setCuriosityThreshold(0.2f);
    agent.setNoveltyDecay(0.95f);
    
    // Configure action selection strategies
    agent.setExplorationMode(ExplorationMode::EPSILON_GREEDY);
    agent.setExplorationRate(0.1f);
    agent.setExploitationThreshold(0.7f);
}
```

## Advanced World Configuration

### 1. Complex Environment Setup

```cpp
// Create and configure a complex world
SimpleWorld world;
world.configure(
    width: 50.0,           // Larger world
    height: 50.0,          // Larger world
    visionWidth: 32,       // Higher resolution vision
    visionHeight: 24       // Different aspect ratio
);

// Add diverse objects
world.addResourceObject(10.0f, 10.0f, 5.0f);      // Positive reward
world.addHazardObject(40.0f, 40.0f, 2.0f);        // Negative reward
world.addWallSegment(0.0f, 25.0f, 50.0f, 0.0f);  // Vertical wall
world.addMarkerObject(25.0f, 25.0f);              // Navigation aid

// Configure physics and dynamics
world.setMaxEnergy(2.0f);          // Higher energy capacity
world.setEnergyDecayRate(0.9f);    // Slower energy loss
world.setFriction(0.3f);           // Realistic physics
world.setGravity(9.81f);           // Earth gravity
```

### 2. Multi-Agent Environment

```cpp
// Set up competition or cooperation scenarios
void setupMultiAgentEnvironment(std::vector<std::shared_ptr<AgentBrain>>& agents) {
    SimpleWorld world;
    world.configure(100.0, 100.0, 16, 16);
    
    // Place agents at different starting positions
    for (size_t i = 0; i < agents.size(); ++i) {
        float angle = 2.0f * M_PI * i / agents.size();
        float x = 10.0f + 10.0f * cos(angle);
        float y = 10.0f + 10.0f * sin(angle);
        world.setAgentStart(x, y);
    }
    
    // Add resources for competition
    for (int i = 0; i < 20; ++i) {
        float x = static_cast<float>(rand() % 90 + 5);
        float y = static_cast<float>(rand() % 90 + 5);
        world.addResourceObject(x, y, 1.0f + static_cast<float>(rand() % 10) / 10.0f);
    }
    
    // Initialize all agents
    for (auto& agent : agents) {
        agent->initialize(world);
    }
}
```

## Advanced Learning Patterns

### 1. Curriculum Learning

```cpp
// Implement curriculum learning: simple tasks first
class CurriculumLearner {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    int currentDifficultyLevel_;
    
public:
    CurriculumLearner() : currentDifficultyLevel_(1) {}
    
    void runCurriculum() {
        while (currentDifficultyLevel_ <= 5) {
            switch (currentDifficultyLevel_) {
                case 1:
                    runSimpleNavigation(100);
                    if (checkProgress()) {
                        increaseDifficulty();
                    }
                    break;
                case 2:
                    runObstacleCourse(200);
                    if (checkProgress()) {
                        increaseDifficulty();
                    }
                    break;
                // ... higher difficulty levels
            }
        }
    }
};
```

### 2. Transfer Learning

```cpp
// Demonstrate transfer learning across tasks
class TransferLearning {
private:
    std::shared_ptr<Brain> baseBrain_;
    std::vector<std::string> learnedTasks_;
    
public:
    void learnTask1(SimpleWorld& world1) {
        // Learn in first environment
        auto task1Brain = cloneBrain(baseBrain_);
        auto agent1 = std::make_shared<AgentBrain>(task1Brain);
        agent1->initialize(world1);
        
        runTask(task1Brain, world1, agent1, "navigation");
        learnedTasks_.push_back("navigation");
    }
    
    void applyToTask2(SimpleWorld& world2) {
        // Transfer knowledge to second environment
        auto task2Brain = cloneBrain(baseBrain_);
        auto agent2 = std::make_shared<AgentBrain>(task2Brain);
        agent2->initialize(world2);
        
        // Reduce exploration for transfer
        agent2->setExplorationRate(0.05f);
        
        runTask(task2Brain, world2, agent2, "object_manipulation");
    }
    
private:
    std::shared_ptr<Brain> cloneBrain(std::shared_ptr<Brain> source) {
        // Implementation for brain cloning
        return std::make_shared<Brain>(source->getConfig());
    }
    
    void runTask(std::shared_ptr<Brain> brain, SimpleWorld& world, 
                std::shared_ptr<AgentBrain> agent, const std::string& taskName) {
        // Common task execution logic
        for (int step = 0; step < 1000; ++step) {
            // ... task-specific implementation
        }
    }
};
```

## Advanced Research Experiments

### 1. Neural Development Study

```cpp
// Study how brain development affects learning
class DevelopmentStudy {
public:
    void runDevelopmentExperiment() {
        std::vector<std::pair<double, float>> developmentAffect;
        
        for (float plasticityModifier : {1.0f, 0.8f, 0.6f, 0.4f, 0.2f}) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 500);
            config->set("plasticity_modifier", plasticityModifier);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->enableDevelopment(true);
            
            double learningRate = measureLearningRate(brain, agent);
            developmentAffect.push_back({plasticityModifier, learningRate});
            
            NLM_LOG_INFO("Plasticity " + std::to_string(plasticityModifier) + 
                        ": Learning rate = " + std::to_string(learningRate));
        }
        
        analyzeDevelopmentCurve(developmentAffect);
    }
    
private:
    float measureLearningRate(std::shared_ptr<Brain> brain, 
                             std::shared_ptr<AgentBrain> agent) {
        // Measure learning performance
        float initialWeight = brain->getAverageSynapticWeight();
        
        // Run task
        for (int step = 0; step < 500; ++step) {
            // ... task execution
        }
        
        float finalWeight = brain->getAverageSynapticWeight();
        return std::abs(finalWeight - initialWeight) / 500.0f;
    }
};
```

### 2. Neuromodulation Research

```cpp
// Investigate neuromodulation effects
class NeuromodulationStudy {
public:
    struct ExperimentResult {
        float dopamineLevel;
        float rewardPredictionError;
        float learningSpeed;
        float stability;
    };
    
    void studyDopamineEffects() {
        std::vector<ExperimentResult> results;
        
        for (float dopamineGain : {0.1f, 0.3f, 0.5f, 0.7f, 0.9f}) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 300);
            config->set("dopamine_gain", dopamineGain);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->enableRewardModulation(true);
            
            ExperimentResult result = runDopamineExperiment(brain, agent, dopamineGain);
            results.push_back(result);
            
            NLM_LOG_INFO("Dopamine gain " + std::to_string(dopamineGain) + 
                        ": Learning = " + std::to_string(result.learningSpeed) +
                        ", Stability = " + std::to_string(result.stability));
        }
        
        analyzeDopamineCurve(results);
    }
};
```

## Performance Optimization

### 1. Profiling and Optimization

```cpp
// Profile brain performance
class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        double totalTime;
        size_t totalSpikes;
        size_t activeNeurons;
        float averageFiringRate;
        float memoryUsage;
    };
    
    PerformanceMetrics profileBrain(std::shared_ptr<Brain> brain) {
        PerformanceMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 1000; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        metrics.totalSpikes = brain->getTotalSpikeCount();
        metrics.activeNeurons = brain->getFiringNeuronCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        metrics.memoryUsage = getMemoryUsage();
        
        return metrics;
    }
    
    void optimizeConfiguration() {
        // Try different configurations and measure performance
        std::vector<std::pair<Config, PerformanceMetrics>> configs;
        
        // Test different neuron counts
        for (int neuronCount : {500, 1000, 2000, 5000}) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", neuronCount);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            PerformanceMetrics metrics = profileBrain(brain);
            configs.push_back({config, metrics});
            
            NLM_LOG_INFO("Neurons: " + std::to_string(neuronCount) +
                        ", Time: " + std::to_string(metrics.totalTime) +
                        ", Spikes: " + std::to_string(metrics.totalSpikes));
        }
        
        // Select best configuration
        auto best = std::min_element(configs.begin(), configs.end(),
            [](const auto& a, const auto& b) {
                return a.second.totalTime < b.second.totalTime;
            });
        
        NLM_LOG_INFO("Best configuration: " + std::to_string(best->first->getOr<int64_t>("neuron_count", 1000)) + " neurons");
    }
};
```

### 2. Memory Management

```cpp
// Optimize memory usage
class MemoryManager {
public:
    void optimizeMemoryUsage() {
        // Monitor memory usage
        size_t initialMemory = getCurrentMemoryUsage();
        
        // Clear unused memory
        forceGarbageCollection();
        
        size_t afterGC = getCurrentMemoryUsage();
        NLM_LOG_INFO("Memory before GC: " + std::to_string(initialMemory) +
                    ", after GC: " + std::to_string(afterGC) +
                    ", saved: " + std::to_string(initialMemory - afterGC));
        
        // Profile different aspects
        profileMemoryAllocationPatterns();
        profileMemoryAccessPatterns();
        optimizeMemoryPools();
    }
    
private:
    void profileMemoryAllocationPatterns() {
        // Measure allocation frequency and size
        std::map<size_t, size_t> allocationSizes;
        
        for (int i = 0; i < 1000; ++i) {
            // Simulate memory allocations
            size_t size = 100 + (rand() % 900);
            allocationSizes[size]++;
        }
        
        // Analyze results
        for (const auto& pair : allocationSizes) {
            NLM_LOG_INFO("Allocation size: " + std::to_string(pair.first) +
                        ", frequency: " + std::to_string(pair.second));
        }
    }
    
    void optimizeMemoryPools() {
        // Configure memory pools for common allocation sizes
        // This reduces fragmentation and improves performance
        MemoryPoolConfig config;
        config.smallPoolSize = 256;      // For small objects
        config.mediumPoolSize = 1024;    // For medium objects
        config.largePoolSize = 4096;     // For large objects
        
        applyMemoryPoolConfiguration(config);
    }
};
```

## Advanced Command Line Interface

### 1. Command Line Options

```bash
# Enhanced command line interface for advanced users

# Basic usage
./nlm_phase6_demo --help

# Phase 6 demo with custom configuration
./nlm_phase6_demo --neurons 2000 --steps 5000 --checkpoint ./my_checkpoint.bin

# Run specific integration tests
./nlm_phase6_demo --test memory --test neuromodulation --test replay

# Performance profiling mode
./nlm_phase6_demo --profile --output ./performance_metrics.json

# Custom environment configuration
./nlm_phase6_demo --world-width 100 --world-height 100 --vision-width 32 --vision-height 32

# Development mode
./nlm_phase6_demo --development --plasticity-modifier 0.8 --critical-period-end 1000

# Debug mode with detailed logging
./nlm_phase6_demo --debug --log-level debug --log-file ./debug.log

# Batch processing for multiple experiments
./nlm_phase6_demo --batch --experiments 10 --results ./batch_results.csv
```

### 2. Configuration File Format

Create custom configuration files:

```ini
# config_advanced.ini
[simulation]
neurons = 2000
regions = 2
connection_probability = 0.12
timestep = 0.0005
max_steps = 10000

[neuromodulation]
dopamine_gain = 1.0
novelty_threshold = 0.25
curiosity_multiplier = 2.5
reward_prediction_error = true

[development]
enable_development = true
plasticity_modifier = 0.8
critical_period_end = 600
synaptogenesis_rate = 0.0003
pruning_rate = 0.00005

[memory]
working_memory_capacity = 75
episodic_memory_limit = 2000
memory_consolidation_interval = 150

[world]
width = 80.0
height = 80.0
vision_width = 20
vision_height = 20
max_energy = 2.0
energy_decay_rate = 0.85
friction = 0.4
gravity = 9.81

[logging]
level = info
file = ./simulation.log
console = true
```

```bash
# Use custom configuration file
./nlm_phase6_demo --config ./config_advanced.ini
```

## Advanced Python API

### 1. Python Extensions

```python
import pynlm
import pynlm.extensions

# Extended brain creation with advanced options
brain = pynlm.createBrain(
    pynlm.createDefaultConfig(),
    neuron_count=2000,
    enable_advanced_features=True,
    custom_parameters={
        'stdp_ltp_weight': 0.015,
        'novelty_threshold': 0.3,
        'curiosity_multiplier': 2.0
    }
)

# Advanced agent configuration
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable sophisticated learning mechanisms
agent.enableAdvancedLearning(True)
agent.setLearningStrategy(pynlm.LearningStrategy.CURRICULUM_LEARNING)
agent.setTransferLearning(True, alpha=0.7)

# Custom action selection
agent.setActionSelection(pynlm.ActionSelection.BETA_GREEDY,
                        exploration_rate=0.15,
                        exploitation_threshold=0.6)

# Performance monitoring
agent.enablePerformanceMonitoring(True, sample_rate=100)
agent.setProfilingEnabled(True, output_file='./agent_profile.json')

# Advanced world interaction
world = pynlm.createAdvancedWorld(width=100, height=100, vision_width=32, vision_height=32)
world.setPhysicsEnabled(True, gravity=9.81, friction=0.3)
world.addDynamicObstacles(50, obstacle_type='hazardous')

# Custom reward functions
def custom_reward_function(world, agent, action):
    base_reward = world.getBaseReward(action)
    novelty_bonus = agent.getNoveltyLevel() * 2.0
    curiosity_bonus = agent.getCuriosityLevel() * 1.5
    development_bonus = agent.getDevelopmentalProgress() * 0.5
    
    return base_reward + novelty_bonus + curiosity_bonus + development_bonus

world.setRewardFunction(custom_reward_function)

# Advanced simulation loop
def advanced_simulation_loop(brain, world, agent, num_steps):
    for step in range(num_steps):
        # Update world with physics
        world.update(0.1)
        
        # Get advanced sensory input
        percept = world.getAdvancedSensoryInput(agent.getPosition(), agent.getOrientation())
        
        # Process with agent's advanced processing
        agent.processAdvancedSensoryInput(percept, step)
        
        # Brain step with performance monitoring
        brain.step(step, step * 0.001)
        
        # Get action with advanced selection
        action = agent.getAdvancedAction(world.getObstacles(), step)
        
        # Apply with physics and interactions
        result = world.applyAdvancedAction(action, agent.getPosition(), step)
        
        # Advanced reward with learning
        reward = agent.calculateAdvancedReward(
            result.reward,
            world.getTargetState(),
            brain.getPredictionSystem()
        )
        
        agent.applyAdvancedRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateAdvancedDevelopment(0.1, step)
        
        # Collect metrics
        if step % 100 == 0:
            metrics = agent.getPerformanceMetrics()
            print(f"Step {step}: Reward={reward:.2f}, Firing={metrics.firing_rate:.2f}, " +
                  f"Curiosity={agent.getCuriosityLevel():.2f}, Novelty={agent.getNoveltyLevel():.2f}")
    
    return agent.getFinalMetrics()
```

### 2. Research Python Interface

```python
import pynlm.research
import numpy as np
import matplotlib.pyplot as plt

# Experimental framework
experiment = pynlm.research.Experiment("developmental_study")

@experiment.parameter("neuron_count", [500, 1000, 2000])
@experiment.parameter("plasticity_modifier", [1.0, 0.8, 0.6, 0.4])
@experiment.parameter("development_enabled", [True, False])
def run_development_experiment(neuron_count, plasticity_modifier, development_enabled):
    # Create brain with parameters
    config = pynlm.createDefaultConfig()
    config.neuron_count = neuron_count
    config.plasticity_modifier = plasticity_modifier
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 16, 16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(development_enabled)
    agent.enableRewardModulation(True)
    
    # Run experiment
    for step in range(1000):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 100 == 0:
            experiment.record_data({
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'memory_traces': agent.getWorkingMemorySize(),
                'dopamine_level': agent.getNeuromodulationLevel(),
                'curiosity_level': agent.getCuriosityLevel()
            })
    
    return experiment.get_results()

# Run experiment suite
results = experiment.run_all()

# Analyze results
experiment.analyze_results({
    'neuron_count': 'bar',
    'plasticity_modifier': 'line',
    'development_enabled': 'grouped_bar'
})

experiment.generate_report('./development_report.html')
```

## Advanced Topics

### 1. Neural Architecture Search

```cpp
// Automated neural architecture search
class NeuralArchitectureSearch {
public:
    struct NetworkArchitecture {
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        std::vector<std::string> enabledFeatures;
    };
    
    NetworkArchitecture searchBestArchitecture() {
        std::vector<NetworkArchitecture> candidateArchitectures;
        
        // Generate candidate architectures
        for (size_t neurons : {500, 1000, 2000, 5000}) {
            for (size_t regions : {1, 2, 3}) {
                for (float connProb : {0.05f, 0.1f, 0.15f}) {
                    NetworkArchitecture arch;
                    arch.neuronCount = neurons;
                    arch.regionCount = regions;
                    arch.connectionProbability = connProb;
                    arch.enabledFeatures = generateFeatureSet();
                    candidateArchitectures.push_back(arch);
                }
            }
        }
        
        // Evaluate each architecture
        NetworkArchitecture bestArchitecture;
        float bestScore = -1.0f;
        
        for (const auto& arch : candidateArchitectures) {
            float score = evaluateArchitecture(arch);
            if (score > bestScore) {
                bestScore = score;
                bestArchitecture = arch;
            }
        }
        
        return bestArchitecture;
    }
};
```

### 2. Multi-Objective Optimization

```python
import pynlm.optimization

# Multi-objective optimization for brain configuration
def multi_objective_optimization():
    # Define objectives
    objectives = [
        pynlm.optimization.Objective('learning_speed', 'minimize', weight=0.4),
        pynlm.optimization.Objective('memory_usage', 'minimize', weight=0.3),
        pynlm.optimization.Objective('behavioral_stability', 'maximize', weight=0.3)
    ]
    
    # Define search space
    search_space = {
        'neuron_count': [500, 1000, 2000, 5000],
        'connection_probability': [0.05, 0.1, 0.15],
        'enable_curiosity': [True, False],
        'plasticity_modifier': [0.5, 0.8, 1.0, 1.2]
    }
    
    # Run optimization
    optimizer = pynlm.optimization.PAREO(
        objectives=objectives,
        search_space=search_space,
        max_evaluations=100,
        population_size=20
    )
    
    results = optimizer.optimize()
    
    # Get Pareto front
    pareto_front = results.get_pareto_front()
    
    # Plot results
    optimizer.visualize_pareto_front(
        x='learning_speed',
        y='memory_usage',
        color='behavioral_stability',
        filename='./pareto_front.png'
    )
    
    return pareto_front
```

## Research and Academic Use

### 1. Citation and Attribution

For academic use, please cite:

```bibtex
@misc{nlm_phase6_2026,
  title={NLM Phase 6: Final Integration of an Artificial Developmental Brain},
  author={Research Team},
  year={2026},
  howpublished={\url{https://github.com/nlm-project/nlm-phase6}},
  note={Phase 6: Final Integration with complete brain system integration}
}
```

### 2. Research Applications

NLM Phase 6 can be used for:

- **Neural Development Research**: Study how developmental processes affect learning
- **Neuromodulation Studies**: Investigate dopamine, curiosity, and novelty effects
- **Memory Integration Research**: Explore how different memory systems interact
- **Optimization Research**: Test algorithms in complex, adaptive environments
- **Cognitive Architecture Research**: Study emergent cognitive functions
- **Learning Theory Research**: Test theories of continuous learning and adaptation

### 3. Data Collection and Analysis

```python
# Comprehensive data collection for research
class ResearchDataCollector {
    def __init__(self, experiment_name):
        self.experiment_name = experiment_name
        self.data = {
            'timestamp': [],
            'simulation_step': [],
            'brain_state': [],
            'neural_activity': [],
            'behavioral_output': [],
            'environmental_context': [],
            'reward_signals': [],
            'memory_activity': [],
            'neuromodulation_levels': [],
            'developmental_metrics': []
        }
    
    def collect_step_data(self, brain, world, agent, step):
        self.data['timestamp'].append(time.time())
        self.data['simulation_step'].append(step)
        self.data['brain_state'].append(brain.getBrainStateVector())
        self.data['neural_activity'].append(brain.getNeuralActivitySnapshot())
        self.data['behavioral_output'].append(agent.getActionHistory())
        self.data['environmental_context'].append(world.getEnvironmentSnapshot())
        self.data['reward_signals'].append(agent.getRecentRewards())
        self.data['memory_activity'].append(agent.getMemoryActivitySummary())
        self.data['neuromodulation_levels'].append(agent.getNeuromodulationSummary())
        self.data['developmental_metrics'].append(agent.getDevelopmentalSummary())
    
    def export_data(self, filename):
        # Export in multiple formats for analysis
        df = pd.DataFrame(self.data)
        
        # Save as CSV for statistical analysis
        df.to_csv(f'{self.experiment_name}_data.csv', index=False)
        
        # Save as pickle for machine learning
        import pickle
        with open(f'{self.experiment_name}_data.pkl', 'wb') as f:
            pickle.dump(self.data, f)
        
        # Generate summary statistics
        summary = self.generate_summary_statistics()
        summary.to_csv(f'{self.experiment_name}_summary.csv')
    
    def generate_summary_statistics(self):
        # Generate statistical summaries for each data type
        summary = pd.DataFrame()
        
        for key, values in self.data.items():
            if values and isinstance(values[0], (list, np.ndarray)):
                # For vector data, compute statistics
                array_data = np.array(values)
                summary[f'{key}_mean'] = np.mean(array_data, axis=0)
                summary[f'{key}_std'] = np.std(array_data, axis=0)
                summary[f'{key}_min'] = np.min(array_data, axis=0)
                summary[f'{key}_max'] = np.max(array_data, axis=0)
            else:
                # For scalar data, compute simple statistics
                summary[f'{key}_mean'] = np.mean(values)
                summary[f'{key}_std'] = np.std(values)
        
        return summary
}
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Memory Usage Too High
**Solution**: Reduce neuron count or enable memory pooling

```bash
# Build with memory optimization
./nlm_phase6_demo --neurons 1000 --enable-memory-pooling
```

#### Issue 2: Learning Too Slow
**Solution**: Increase neuromodulation gains or adjust plasticity parameters

```bash
# Enable aggressive learning
./nlm_phase6_demo --enable-reward-modulation --dopamine-gain 2.0 --novelty-threshold 0.1
```

#### Issue 3: Simulation Crashes
**Solution**: Check configuration files and enable safe defaults

```bash
# Run with safe defaults
./nlm_phase6_demo --safe-mode --max-steps 1000
```

#### Issue 4: Performance Issues
**Solution**: Enable profiling and optimize configuration

```bash
# Profile performance
./nlm_phase6_demo --profile --output ./profile_results.json
```

## Further Reading

### Recommended Documentation
- `docs/ARCHITECTURE.md`: Detailed system architecture
- `docs/SCIENCE.md`: Scientific background and research context
- `docs/ROADMAP.md`: Future development plans
- `docs/LIMITATIONS.md`: Current limitations and future work
- `docs/PHASE6_FINAL_AUDIT.md`: Phase 6 specific documentation

### Research Papers
- "An Experimental Artificial Developmental Brain" (NLM Phase 6 documentation)
- "Neuromodulation in Artificial Brains" (NLM Phase 3+ research)
- "Memory Integration in Neural Systems" (NLM Phase 4 research)
- "Developmental Processes in Artificial Neural Networks" (NLM Phase 5 research)

## Getting Help

For technical support, please:

1. **Check the documentation**: Review this guide and related documentation
2. **Review logs**: Check `./simulation.log` for detailed error messages
3. **Join the community**: Contact the NLM research team
4. **Report issues**: Submit bug reports with detailed information
5. **Share your results**: Contribute to the NLM research community

## Version Information

- **Version**: Phase 6.0 (Final Integration)
- **Build Date**: September 2026
- **API Version**: 1.0
- **Backward Compatibility**: Full (Phase 6 builds on previous phases)

---

*This document is continuously updated as NLM Phase 6 evolves. For the latest information, check the official documentation repository.*