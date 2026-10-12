# NLM Python Convenience Functions - Usage Examples

## 1. BRAIN CONFIGURATION CONVENIENCE FUNCTIONS

### Preset Configuration Factories

```python
import pynlm as nlm

# Create a learning-optimized configuration
learning_config = nlm.createLearningConfig()
print(f"Learning config keys: {learning_config.getKeys()}")

# Create a simulation-optimized configuration  
simulation_config = nlm.createSimulationConfig()

# Create a development-optimized configuration
development_config = nlm.createDevelopmentConfig()

# Create an exploration-optimized configuration
exploration_config = nlm.createExplorationConfig()
```

### Configuration Validation Functions

```python
import pynlm as nlm

# Create a configuration
config = nlm.createLearningConfig()

# Validate the configuration
if nlm.validateBrainConfig(config):
    print("Configuration is valid")
else:
    print("Configuration is invalid")

# Check for learning-specific issues
warnings = nlm.validateLearningConfig(config)
for warning in warnings:
    print(f"Warning: {warning}")

# Check for simulation-specific issues  
warnings = nlm.validateSimulationConfig(config)
for warning in warnings:
    print(f"Warning: {warning}")
```

### Configuration Modifier Functions

```python
import pynlm as nlm

# Start with a basic config
config = nlm.createDefaultConfig()

# Enable learning features
config = nlm.enableLearningMode(config)
print(f"Enabled learning mode: {config.has('brain.learning.enabled')}")

# Or start with development mode
dev_config = nlm.createDevelopmentConfig()
dev_config = nlm.enableDevelopmentMode(dev_config)

# Optimize for speed
speed_config = nlm.optimizeForSpeed(config)
```

### Configuration Copy and Clone Functions

```python
import pynlm as nlm

# Create base configuration
base_config = nlm.createLearningConfig()

# Create an exact copy
backup_config = nlm.copyConfig(base_config)
print("Created backup of learning config")

# Create override configuration
override_config = nlm.createSimulationConfig()

# Merge them (override takes precedence)
merged_config = nlm.mergeConfigs(base_config, override_config)
print(f"Merged config has learning: {merged_config.has('brain.learning.enabled')}")
print(f"Merged config has simulation settings: {merged_config.has('simulation.dt')}")
```

## 2. SIMULATION HELPER FUNCTIONS

### Environment Setup Functions

```python
import pynlm as nlm

# Create a standard arena environment
world = nlm.createStandardEnvironment()
world.setAgentStart(50, 50)
print(f"Created standard world: {world.getWidth()}x{world.getHeight()}")

# Create a training environment with resources and hazards
training_world = nlm.createTrainingEnvironment()
world.setAgentStart(100, 100)
print(f"Created training world with resources and hazards")

# Create an exploration environment for discovery
exploration_world = nlm.createExplorationEnvironment()
world.setAgentStart(200, 200)
print(f"Created exploration world with diverse objects")
```

### Brain Initialization Helpers

```python
import pynlm as nlm

# Setup a brain for learning
world = nlm.createTrainingEnvironment()
brain = nlm.createDefaultBrain()
nlm.setupBrainForLearning(brain, world)
print("Set up brain for learning with optimal configuration")

# Setup a brain for simulation
sim_brain = nlm.createDefaultBrain()
nlm.setupBrainForSimulation(sim_brain, world)
print("Set up brain for controlled simulation")

# Setup a brain for development
dev_brain = nlm.createDefaultBrain()
nlm.setupBrainForDevelopment(dev_brain, world)
print("Set up brain for developmental processes")
```

### Progress Tracking and Monitoring

```python
import pynlm as nlm

# Track episode performance
rewards = [1.0, 2.0, 1.5, 3.0, 2.5, 4.0, 3.5]
errors = [0.5, 0.3, 0.4, 0.2, 0.6, 0.1, 0.3]

# Calculate learning progress
progress = nlm.trackLearningProgress(rewards, evaluationInterval=2)
print(f"Learning progress (smoothed): {progress}")

# Check if learning has converged
converged = nlm.calculateConvergence(rewards, threshold=0.01, windowSize=3)
print(f"Learning converged: {converged}")

# Detect bursty patterns in spike data
spikes = [0.0, 0.0, 1.5, 2.0, 1.8, 0.0, 0.0, 0.5, 1.2, 0.0]
bursts = nlm.detectBursting(spikes, duration=100, threshold=1.0)
print(f"Detected {len(bursts)} burst events")
```

### Episode Management Functions

```python
import pynlm as nlm

# Create an episode manager for reinforcement learning
world = nlm.createTrainingEnvironment()
brain = nlm.createDefaultBrain()
nlm.setupBrainForLearning(brain, world)

# The episode management would be handled through the agent brain
# episode_manager = nlm.createEpisodeManager(brain, world)
# resetForNewEpisode(episode_manager)
# stats = getEpisodeStatistics(episode_manager)
# print(f"Episode stats: {stats}")
```

## 3. AGENT SYSTEM ABSTRACTIONS

### High-Level Agent Setup Functions

```python
import pynlm as nlm

# Create a brain for the agent
brain = nlm.createDefaultBrain()

# Create standard agent (basic capabilities)
standard_agent = nlm.createStandardAgent(brain)
print(f"Standard agent reward modulation: {standard_agent.isRewardModulationEnabled()}")
print(f"Standard agent curiosity: {standard_agent.isCuriosityEnabled()}")

# Create exploration agent (exploration-focused)
exploration_agent = nlm.createExplorationAgent(brain)
print(f"Exploration agent curiosity: {exploration_agent.isCuriosityEnabled()}")

# Create learning agent (learning-focused)
learning_agent = nlm.createLearningAgent(brain)
print(f"Learning agent development: {learning_agent.isDevelopmentEnabled()}")
```

### Neuromodulation Configuration Functions

```python
import pynlm as nlm

# Create agent and brain
brain = nlm.createDefaultBrain()
agent = nlm.createStandardAgent(brain)

# Setup reward modulation with learning rate
nlm.setupRewardModulation(agent, learningRate=0.1)
print("Configured reward modulation system")

# Setup curiosity modulation
nlm.setupCuriosityModulation(agent, noveltyWeight=0.7, errorWeight=0.3)
print("Configured curiosity modulation system")

# Setup prediction error modulation
nlm.setupPredictionErrorModulation(agent, precisionWeight=0.5)
print("Configured prediction error modulation")
```

### Development System Helpers

```python
import pynlm as nlm

# Create brain for development
brain = nlm.createDefaultBrain()

# Enable critical period for enhanced plasticity
nlm.enableCriticalPeriod(brain, duration=1000)
print("Enabled critical period for development")

# Schedule developmental stages
nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.CriticalPeriod, duration=500)
nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.Maturation, duration=2000)
print("Scheduled developmental stages")

# Trigger synaptogenesis
nlm.triggerSynaptogenesis(brain, strength=0.8)
print("Triggered synaptogenesis event")
```

### Curiosity and Exploration Management

```python
import pynlm as nlm

# Create exploration agent
brain = nlm.createDefaultBrain()
agent = nlm.createExplorationAgent(brain)

# Set exploration strategy
nlm.setExplorationStrategy(agent, strategy=1)  # Strategy enum value
print("Set exploration strategy")

# Create curiosity profile
nlm.createCuriosityProfile(agent, noveltyWeight=0.8, errorWeight=0.2)
print("Created curiosity profile")

# Adjust exploration drive dynamically
nlm.adjustExplorationDrive(agent, driveMultiplier=1.5)
print("Adjusted exploration drive")
```

## 4. WORLD INTEGRATION HELPERS

### World Initialization Functions

```python
import pynlm as nlm

# Create arena world
arena = nlm.createArenaWorld(width=100.0, height=100.0, withWalls=True)
print(f"Created arena world: {arena.getWidth()}x{arena.getHeight()}")

# Create resource world
resource_world = nlm.createResourceWorld(width=200.0, height=200.0, numResources=15, resourceRadius=3.0)
print(f"Created resource world with {resource_world.getWidth()}x{resource_world.getHeight()}")

# Create hazard world
hazard_world = nlm.createHazardWorld(width=150.0, height=150.0, numHazards=8, hazardDamage=5.0)
print(f"Created hazard world with {hazard_world.getWidth()}x{hazard_world.getHeight()}")
```

### Sensory Input Processing Utilities

```python
import pynlm as nlm

# Create vision percept
vision = nlm.createVisionPercept(width=32, height=32, channels=3)
print(f"Created vision percept: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")

# Create audio percept
audio = nlm.createAudioPercept(sampleRate=44100, numSamples=1024)
print(f"Created audio percept: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")

# Create internal percept
internal = nlm.createInternalPercept()
print("Created internal signals percept")
```

### Motor Output Processing Functions

```python
import pynlm as nlm

# Create movement action
action = nlm.createMovementAction(
    type=nlm.ActionType.MoveForward, 
    x=10.0, 
    y=5.0, 
    angle=45.0
)
print(f"Created movement action: type={action.getType()}, x={action.getParameters()[1]}, y={action.getParameters()[2]}")

# Create interaction action
interact_action = nlm.createInteractionAction(
    type=nlm.ActionType.Interact,
    x=50.0,
    y=30.0,
    force=2.0
)
print(f"Created interaction action: force={interact_action.getParameters()[3]}")

# Create custom action
params = [nlm.ActionType.Custom, 1.0, 2.0, 3.0, 4.0]
custom_action = nlm.createCustomAction("my_action", params)
print(f"Created custom action: {custom_action.getName()}")
```

### Environment Interaction Helpers

```python
import pynlm as nlm

# Create world and setup agent
world = nlm.createArenaWorld()
nlm.setupAgentStartPosition(world, x=10.0, y=10.0, orientation=0.0)
print("Setup agent start position")

# Spawn resources
nlm.spawnResource(world, x=50.0, y=50.0, value=15.0)
nlm.spawnResource(world, x=80.0, y=30.0, value=25.0)
print("Spawned resources")

# Create hazards
nlm.createHazard(world, x=70.0, y=70.0, damage=10.0)
nlm.createHazard(world, x=90.0, y=90.0, damage=15.0)
print("Created hazards")
```

## 5. DATA PROCESSING AND ANALYSIS

### Neural Activity Analysis Functions

```python
import pynlm as nlm
import random

# Generate sample spike data
spikes = [0.0] * 100
for i in range(100):
    if random.random() < 0.1:  # 10% chance of spike
        spikes[i] = random.gauss(2.0, 0.5)

# Calculate firing rate
rate = nlm.calculateFiringRate(spikes, duration=100)
print(f"Firing rate: {rate} Hz")

# Analyze firing pattern
pattern = nlm.analyzeFiringPattern(spikes, duration=100)
print(f"Firing pattern metrics: mean_rate={pattern[0]:.2f}, cv={pattern[1]:.2f}")

# Detect bursting
bursts = nlm.detectBursting(spikes, duration=100, threshold=1.0)
print(f"Detected {len(bursts)} burst events")
```

### Statistics and Metrics Calculation

```python
import pynlm as nlm
import random

# Generate sample data
data = [random.gauss(0.0, 1.0) for _ in range(100)]

# Calculate statistics
mean = nlm.calculateMean(data)
print(f"Mean: {mean:.2f}")

std_dev = nlm.calculateStandardDeviation(data, mean)
print(f"Standard deviation: {std_dev:.2f}")

# Calculate correlation
corr_data = [x * 0.8 + random.gauss(0, 0.2) for x in data]
correlation = nlm.calculateCorrelation(data, corr_data)
print(f"Correlation: {correlation:.2f}")
```

### Spike Pattern Analysis

```python
import pynlm as nlm

# Generate spike train
spikes = []
for i in range(200):
    if i % 20 == 0:  # Regular spikes
        spikes.append(2.0)
    elif i % 30 == 0:  # Bursty spikes
        spikes.append(3.0)
    else:  # Random spikes
        spikes.append(0.0 if random.random() > 0.8 else 1.0)

# Calculate coefficient of variation
cv = nlm.calculateCoefficientOfVariation(spikes, duration=200)
print(f"Coefficient of variation: {cv:.2f}")

# Detect rate modulation
modulation = nlm.detectRateModulation(spikes, windowSize=20, stepSize=10)
print(f"Rate modulation ({len(modulation)} windows): {modulation[:5]}...")
```

### Learning Progress Tracking

```python
import pynlm as nlm

# Track learning progress over episodes
performance = [0.1, 0.3, 0.5, 0.7, 0.8, 0.9, 0.95, 0.98, 0.99, 1.0]

# Get smoothed learning curve
progress = nlm.trackLearningProgress(performance, evaluationInterval=3)
print(f"Learning progress (smoothed): {progress}")

# Check convergence
converged = nlm.calculateConvergence(performance, threshold=0.01, windowSize=3)
print(f"Learning converged: {converged}")
```

## 6. VALIDATION AND CHECKING FUNCTIONS

### Brain Consistency Validation

```python
import pynlm as nlm

# Create and setup a brain
brain = nlm.createDefaultBrain()
nlm.setupBrainForLearning(brain, nlm.createTrainingEnvironment())

# Validate brain connectivity
connectivity_issues = nlm.validateBrainConnectivity(brain)
if connectivity_issues:
    print("Brain connectivity issues:")
    for issue in connectivity_issues:
        print(f"  - {issue}")
else:
    print("Brain connectivity is valid")

# Validate brain dynamics
brain_issues = nlm.validateBrainDynamics(brain, duration=1000)
if brain_issues:
    print("Brain dynamics issues:")
    for issue in brain_issues:
        print(f"  - {issue}")
else:
    print("Brain dynamics are stable")

# Check brain resource usage
resource_report = nlm.checkBrainResourceUsage(brain)
print(f"Resource report:\n{resource_report}")
```

### World State Validation

```python
import pynlm as nlm

# Create world and agent
world = nlm.createArenaWorld()
agent = world.getAgentBody()

# Validate world state
world_issues = nlm.validateWorldState(world)
if world_issues:
    print("World state issues:")
    for issue in world_issues:
        print(f"  - {issue}")
else:
    print("World state is valid")

# Check boundaries
within_bounds = nlm.checkWorldBoundaries(world, agent)
print(f"Agent within world bounds: {within_bounds}")
```

### Connection Health Checks

```python
import pynlm as nlm

# Create and setup brain
brain = nlm.createDefaultBrain()
nlm.setupBrainForLearning(brain, nlm.createTrainingEnvironment())

# Check synapse health
region_id = brain.getRegionIds()[0] if brain.getRegionCount() > 0 else nlm.RegionId(0)
synapse_health = nlm.checkSynapseHealth(brain, threshold=0.5)
print(f"Synapse health check completed: {len(synapse_health)} issues found")

# Check neural region health
region_health = nlm.checkNeuralRegionHealth(brain, region_id)
print(f"Region health:\n{region_health}")
```

### Resource Availability Checks

```python
import pynlm as nlm

# Check memory availability
memory_ok = nlm.checkMemoryAvailability(100 * 1024 * 1024)  # 100MB
print(f"Memory available: {memory_ok}")

# Check CPU load
cpu_ok = nlm.checkCPULoad(threshold=0.8)
print(f"CPU load acceptable: {cpu_ok}")

# Check simulation time (requires running simulation)
# brain = nlm.createDefaultBrain()
# sim_ok = nlm.checkSimulationTime(brain, maxStepTime=1000)
# print(f"Simulation time acceptable: {sim_ok}")
```

## 7. COMPARISON AND UTILITY FUNCTIONS

### Brain State Comparison

```python
import pynlm as nlm

# Create two different brains
brain1 = nlm.createDefaultBrain()
brain2 = nlm.createDefaultBrain()

# Compare brain states
comparison = nlm.compareBrainStates(brain1, brain2)
print(f"Brain state comparison:\n{comparison}")

# Calculate similarity
similarity = nlm.calculateBrainSimilarity(brain1, brain2, metric="neuron_count")
print(f"Neuron count similarity: {similarity:.2f}")
```

### Configuration Comparison

```python
import pynlm as nlm

# Create two different configurations
config1 = nlm.createLearningConfig()
config2 = nlm.createSimulationConfig()

# Compare configurations
config_comparison = nlm.compareConfigs(config1, config2)
print(f"Configuration comparison:\n{config_comparison}")

# Find specific differences
differences = nlm.findConfigDifferences(config1, config2)
print(f"Configuration differences ({len(differences)}):")
for diff in differences:
    print(f"  - {diff}")
```

### Performance Benchmarking

```python
import pynlm as nlm

# Create brain and world
brain = nlm.createDefaultBrain()
world = nlm.createTrainingEnvironment()
nlm.setupBrainForLearning(brain, world)

# Benchmark brain stepping
brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=1000)
print(f"Brain stepping benchmark:\n{brain_benchmark}")

# Benchmark simulation
simulation_benchmark = nlm.benchmarkSimulation(world, brain, numSteps=500)
print(f"Simulation benchmark:\n{simulation_benchmark}")
```

### Time Series Analysis

```python
import pynlm as nlm
import random

# Generate sample time series data
timeseries = [random.gauss(0.0, 1.0) for _ in range(100)]

# Analyze time series
analysis = nlm.analyzeTimeSeries(timeseries, samplingRate=100)
print(f"Time series analysis:\n{analysis}")

# Detect anomalies
anomalies = nlm.detectAnomalies(timeseries, threshold=2.0)
print(f"Detected {len(anomalies)} anomalies at indices: {anomalies}")
```

# END OF USAGE EXAMPLES
