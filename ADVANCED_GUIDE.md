# Advanced NLM Command Line Interface

## Overview

This document describes the advanced command-line interface features for NLM, designed for researchers and advanced users who need enhanced control over brain simulations, experimentation, and analysis.

## Advanced Command Line Options

### Brain Simulation Commands

```bash
# Run a custom brain simulation with advanced options
nlm --brain-config custom_config.json --steps 5000 --checkpoint-interval 1000 --verbose

# Run multiple trials with different configurations
nlm --trial 10 --config-pattern "config_%d.json" --output-dir results/

# Run with custom random seed for reproducible experiments
nlm --seed 12345 --steps 10000 --save-path experiment1.json

# Run with real-time visualization
nlm --world --visualization --update-rate 10 --max-frames 1000

# Run with experimental controls
nlm --experiment phase6_integration --metrics --analyze --output analysis.json
```

### Brain Configuration Commands

```bash
# Create a custom configuration file
nlm config --create --output config.json

# List all available configuration options
nlm config --list --advanced

# Show configuration help
nlm config --help

# Validate configuration file
nlm config --validate config.json

# Merge configuration files
nlm config --merge base.json override.json --output merged.json
```

### Agent Commands

```bash
# Create and configure an advanced agent
nlm agent --brain-config brain.json --world-config world.json --enable-all-subsystems

# Load and run a saved agent
nlm agent --load agent_state.json --resume --steps 2000

# Train an agent in a custom environment
nlm agent --train --environment custom_world.py --episodes 500 --learning-rate 0.01

# Evaluate agent performance
nlm agent --evaluate --model model.json --test-worlds 10 --metrics accuracy,reward,novelty
```

### Experiment Commands

```bash
# Run Phase 6 integration experiment
nlm experiment --phase 6 --duration 60000 --repetitions 5 --collect-metrics

# Run memory consolidation experiment
nlm experiment --type memory_consolidation --interval 1000 --threshold 0.5 --epochs 10

# Run curiosity-driven exploration experiment
nlm experiment --type curiosity --exploration-rate 0.3 --novelty-threshold 0.7 --target-rewards 100

# Run developmental stages experiment
nlm experiment --type development --stages Initial,Critical,Maturation,Adult --plasticity-variation 0.8
```

### Analysis Commands

```bash
# Analyze brain state from checkpoint
nlm analyze --input checkpoint.json --output analysis.json --metrics neural-activity,connectivity,synaptic-strength

# Extract patterns from simulation data
nlm analyze --input simulation_data.json --patterns spike-trains,burst-patterns --clustering kmeans

# Visualize brain connectivity
nlm visualize --input brain.json --output connectivity.png --layout spring-embeddings

# Compare two brain states
nlm compare --brain1 checkpoint1.json --brain2 checkpoint2.json --similarity metrics
```

## Advanced Configuration Options

### Brain Configuration

```python
# Advanced brain configuration with more parameters
config = pynlm.createAdvancedConfig()

# Enhanced neuron configuration
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.08)
config.set("brain.initial_weight_mean", 0.3)
config.set("brain.initial_weight_std", 0.15)
config.set("brain.v_thresh", -45.0)
config.set("brain.v_rest", -65.0)
config.set("brain.v_reset", -70.0)
config.set("brain.tau_mem", 15.0)
config.set("brain.tau_ref", 3.0)

# Advanced plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.association_window", 100)
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
config.set("plasticity.structural.pruning_rate", 0.00005)
config.set("plasticity.structural.max_synapses", 10000)

# Advanced neuromodulation configuration
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.dopamine.baseline", 0.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.exploration_rate", 0.4)
config.set("neuromod.curiosity.exploration_bonus", 0.5)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.decay_rate", 0.98)
config.set("neuromod.prediction.error_threshold", 0.1)
```

### Advanced Agent Configuration

```python
# Create advanced agent with custom neural architecture
config = pynlm.createBrainConfig()

# Custom neural population configuration
config.set("brain.population.sensory.vision.neurons", 256)
config.set("brain.population.sensory.touch.neurons", 32)
config.set("brain.population.sensory.internal.neurons", 16)
config.set("brain.population.sensory.proprioception.neurons", 24)
config.set("brain.population.motor.actions", [
    "move_forward", "move_backward", "turn_left", "turn_right", 
    "interact", "wait", "look_left", "look_right", "eat"
])

# Advanced neuromodulation configuration
config.set("neuromod.dopamine.baseline", -0.1)
config.set("neuromod.curiosity.exploration_rate", 0.5)
config.set("neuromod.curiosity.max_curiosity", 2.0)

# Advanced development configuration
config.set("development.stage.duration.Initial", 60.0)
config.set("development.stage.duration.CriticalPeriod", 240.0)
config.set("development.stage.duration.Maturation", 720.0)
config.set("development.plasticity.modulation", 0.8)
```

### Advanced World Configuration

```python
# Create advanced world with custom physics and objects
world = pynlm.createAdvancedWorld()

# Enhanced world configuration
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16,
    timestep=0.05,
    gravity=9.8,
    friction=0.1,
    maxEnergy=1000.0,
    objectDecayRate=0.01
)

# Add advanced objects
world.addObject(pynlm.WorldObject(
    x=10.0, y=10.0,
    type=pynlm.WorldObjectType.Resource,
    value=10.0,
    decayRate=0.001,
    replenishRate=0.01,
    respawnTime=30.0
))

world.addObject(pynlm.WorldObject(
    x=30.0, y=30.0,
    type=pynlm.WorldObjectType.Hazard,
    damage=5.0,
    duration=100.0,
    radius=2.0
))

# Advanced agent body configuration
body = world.getAgentBody()
body.setEnergyCapacity(500.0)
body.setEnergyConsumptionRate(0.1)
body.setMetabolicRate(0.95)
body.setMovementEfficiency(0.85)
body.setVisionRange(10.0)
body.setHearingRange(8.0)
```

## Advanced Experiment Framework

### Experiment Types

#### 1. Memory Consolidation Experiment

```python
def run_memory_consolidation_experiment(brain, world, agent, duration=10000):
    """Advanced memory consolidation experiment with analysis."""
    
    # Enable memory tracking
    brain.enableEpisodicMemory(True)
    brain.enableSemanticMemory(True)
    
    # Configure consolidation parameters
    brain.setMemoryConsolidationInterval(1000)
    brain.setConsolidationThreshold(0.3)
    brain.setForgettingRate(0.05)
    
    # Run simulation
    for step in range(duration):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        if step % 100 == 0:
            metrics = {
                'step': step,
                'active_neuron_count': brain.getActiveNeuronCount(),
                'total_synapse_count': brain.getTotalSynapseCount(),
                'episodic_memory_size': brain.getEpisodicMemorySize(),
                'working_memory_usage': brain.getWorkingMemoryUsage(),
                'consolidation_events': brain.getConsolidationCount()
            }
            record_metrics(metrics)
    
    # Analyze memory consolidation patterns
    return analyzeMemoryConsolidationPatterns()
```

#### 2. Neuromodulation Integration Experiment

```python
def run_neuromodulation_experiment(brain, world, agent, trials=5):
    """Advanced neuromodulation integration experiment."""
    
    results = []
    
    for trial in range(trials):
        # Configure neuromodulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableNoveltyDetection(True)
        
        # Set different neuromodulation strengths
        agent.setDopamineBaseline(-0.1 + trial * 0.05)
        agent.setCuriosityStrength(0.3 + trial * 0.1)
        agent.setNoveltySensitivity(0.5 + trial * 0.05)
        
        # Run trial
        trial_results = run_simulation(brain, world, agent, 2000)
        results.append(trial_results)
        
        # Reset for next trial
        brain.reset()
        world.reset()
        agent.reset()
    
    # Analyze neuromodulation effects
    return analyzeNeuromodulationEffects(results)
```

#### 3. Developmental Plasticity Experiment

```python
def run_development_experiment(brain, world, agent, durations=[60.0, 300.0, 900.0, 1800.0]):
    """Advanced developmental plasticity experiment."""
    
    developmental_results = {}
    
    for duration in durations:
        # Configure developmental stage
        brain.setDevelopmentalStage('Initial')
        brain.setPlasticityModifier(1.0)
        
        # Run simulation with specific developmental period
        for step in range(int(duration * 100)):  # Convert to steps
            brain.step(step)
            
            # Record developmental metrics
            if step % 500 == 0:
                metrics = {
                    'age': step * 0.001,
                    'developmental_stage': brain.getDevelopmentalStage(),
                    'plasticity_modifier': brain.getPlasticityModifier(),
                    'synaptogenesis_rate': brain.getSynaptogenesisRate(),
                    'pruning_rate': brain.getPruningRate(),
                    'learning_rate': brain.getLearningRate()
                }
                developmental_results[f"duration_{duration}"] = metrics
        
        # Transition to next developmental stage
        brain.advanceDevelopmentalStage()
    
    return analyzeDevelopmentalPatterns(developmental_results)
```

## Advanced Analysis Tools

### Neural Activity Analysis

```python
# Advanced neural activity analysis
class NeuralActivityAnalyzer:
    def __init__(self, brain):
        self.brain = brain
    
    def analyze_spike_patterns(self, step_range=(0, 1000)):
        """Analyze spike patterns across neurons."""
        spike_patterns = []
        
        for step in range(step_range[0], step_range[1]):
            # Get spike data for this step
            spikes = self.brain.getSpikesAtStep(step)
            
            # Analyze pattern characteristics
            pattern_features = {
                'total_spikes': len(spikes),
                'firing_rate': len(spikes) / (step_range[1] - step_range[0]),
                'burst_count': self.detect_bursts(spikes),
                'coherence': self.calculate_coherence(spikes),
                'regularity': self.calculate_spike_regularity(spikes)
            }
            
            spike_patterns.append(pattern_features)
        
        return self.summarize_patterns(spike_patterns)
    
    def detect_bursts(self, spikes):
        """Detect burst patterns in spike train."""
        if len(spikes) < 3:
            return 0
        
        # Implement burst detection algorithm
        # For example: inter-spike interval based detection
        intervals = []
        for i in range(1, len(spikes)):
            intervals.append(spikes[i] - spikes[i-1])
        
        # Count short intervals indicating bursts
        burst_threshold = 0.01  # 10ms threshold
        burst_count = sum(1 for interval in intervals if interval < burst_threshold)
        
        return burst_count // 2  # Each burst requires 2 short intervals
```

### Synaptic Connectivity Analysis

```python
# Advanced synaptic connectivity analysis
class SynapticConnectivityAnalyzer:
    def __init__(self, brain):
        self.brain = brain
    
    def analyze_connectivity_patterns(self):
        """Analyze synaptic connectivity patterns."""
        connectivity_metrics = {}
        
        # Analyze excitatory-inhibitory balance
        e_to_i_ratio = self.calculate_excitatory_inhibitory_ratio()
        connectivity_metrics['e_i_ratio'] = e_to_i_ratio
        
        # Analyze clustering coefficients
        clustering_coefficients = self.calculate_clustering_coefficients()
        connectivity_metrics['clustering'] = clustering_coefficients
        
        # Analyze path lengths
        path_lengths = self.calculate_path_lengths()
        connectivity_metrics['path_lengths'] = path_lengths
        
        # Analyze modularity
        modularity = self.calculate_modularity()
        connectivity_metrics['modularity'] = modularity
        
        # Analyze small-world properties
        small_world_metrics = self.calculate_small_world_metrics()
        connectivity_metrics['small_world'] = small_world_metrics
        
        return connectivity_metrics
    
    def calculate_excitatory_inhibitory_ratio(self):
        """Calculate E/I balance in the brain."""
        total_excitatory = 0.0
        total_inhibitory = 0.0
        
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                weight = synapse.getWeight()
                if weight > 0:
                    total_excitatory += weight
                else:
                    total_inhibitory += abs(weight)
        
        if total_inhibitory > 0:
            return total_excitatory / total_inhibitory
        else:
            return float('inf') if total_excitatory > 0 else 0.0
```

### Memory Analysis

```python
# Advanced memory analysis
class MemoryAnalyzer:
    def __init__(self, brain):
        self.brain = brain
    
    def analyze_episodic_memory(self, episode_range=(0, 100)):
        """Analyze episodic memory patterns."""
        episodes = self.brain.getEpisodicMemory(episode_range)
        
        if not episodes:
            return None
        
        analysis = {
            'total_episodes': len(episodes),
            'memory_strength': self.calculate_memory_strength(episodes),
            'replay_frequency': self.calculate_replay_frequency(episodes),
            'consolidation_efficiency': self.calculate_consolidation_efficiency(episodes),
            'pattern_extraction': self.extract_memory_patterns(episodes)
        }
        
        return analysis
    
    def calculate_memory_strength(self, episodes):
        """Calculate average memory strength."""
        strengths = []
        for episode in episodes:
            # Calculate memory strength based on activation patterns
            strength = self.calculate_episode_strength(episode)
            strengths.append(strength)
        
        return sum(strengths) / len(strengths) if strengths else 0.0
    
    def extract_memory_patterns(self, episodes):
        """Extract common patterns from episodes."""
        # Implement pattern extraction algorithm
        # Could use clustering, association rule mining, etc.
        patterns = []
        
        # Example: extract frequently co-active neuron pairs
        co_activations = self.calculate_co_activations(episodes)
        
        # Apply threshold to extract significant patterns
        threshold = self.calculate_pattern_threshold(co_activations)
        
        for neuron_pair, frequency in co_activations.items():
            if frequency > threshold:
                patterns.append({
                    'neurons': neuron_pair,
                    'frequency': frequency,
                    'strength': self.calculate_pattern_strength(neuron_pair, episodes)
                })
        
        return patterns
```

## Advanced Visualization

### Brain State Visualization

```python
# Advanced brain state visualization
class BrainVisualizer:
    def __init__(self, brain):
        self.brain = brain
    
    def visualize_connectivity(self, output_file="connectivity.png"):
        """Create detailed connectivity visualization."""
        import matplotlib.pyplot as plt
        import networkx as nx
        
        # Create network graph
        G = nx.Graph()
        
        # Add neurons as nodes
        for region in self.brain.getRegions():
            for neuron in region.getNeurons():
                G.add_node(neuron.getId(), 
                          type=neuron.getType(),
                          region=region.getName())
        
        # Add synapses as edges
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                G.add_edge(synapse.getSourceNeuron(), 
                          synapse.getDestinationNeuron(),
                          weight=synapse.getWeight(),
                          type=synapse.getType())
        
        # Apply layout algorithm
        pos = nx.spring_layout(G, k=1, iterations=50)
        
        # Draw network
        plt.figure(figsize=(12, 10))
        
        # Draw nodes by type
        node_colors = []
        for node in G.nodes():
            node_type = G.nodes[node]['type']
            if node_type == 'Excitatory':
                node_colors.append('red')
            elif node_type == 'Inhibitory':
                node_colors.append('blue')
            elif node_type == 'Sensory':
                node_colors.append('green')
            elif node_type == 'Motor':
                node_colors.append('orange')
            else:
                node_colors.append('gray')
        
        nx.draw_networkx_nodes(G, pos, node_color=node_colors, node_size=50)
        nx.draw_networkx_edges(G, pos, alpha=0.6, width=1)
        nx.draw_networkx_labels(G, pos, font_size=8)
        
        plt.title("Brain Connectivity Network")
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
```

### Activity Visualization

```python
# Activity visualization over time
class ActivityVisualizer:
    def __init__(self, brain):
        self.brain = brain
    
    def visualize_activity_over_time(self, output_file="activity_over_time.png"):
        """Visualize neural activity patterns over time."""
        import matplotlib.pyplot as plt
        import numpy as np
        
        # Collect activity data
        time_steps = []
        activity_levels = []
        firing_rates = []
        e_i_ratios = []
        
        for step in range(0, min(1000, self.brain.getCurrentStep())):
            time_steps.append(step)
            activity_levels.append(self.brain.getActiveNeuronCountAtStep(step))
            firing_rates.append(self.brain.getFiringRateAtStep(step))
            e_i_ratios.append(self.brain.getExcitationInhibitionRatioAtStep(step))
        
        # Create subplots
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Active neurons
        axes[0, 0].plot(time_steps, activity_levels, 'b-', linewidth=1)
        axes[0, 0].set_title('Active Neuron Count')
        axes[0, 0].set_xlabel('Time Step')
        axes[0, 0].set_ylabel('Count')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Firing rate
        axes[0, 1].plot(time_steps, firing_rates, 'r-', linewidth=1)
        axes[0, 1].set_title('Average Firing Rate')
        axes[0, 1].set_xlabel('Time Step')
        axes[0, 1].set_ylabel('Hz')
        axes[0, 1].grid(True, alpha=0.3)
        
        # E/I ratio
        axes[1, 0].plot(time_steps, e_i_ratios, 'g-', linewidth=1)
        axes[1, 0].set_title('Excitatory-Inhibitory Ratio')
        axes[1, 0].set_xlabel('Time Step')
        axes[1, 0].set_ylabel('E/I Ratio')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Activity spectrum
        if activity_levels:
            axes[1, 1].hist(activity_levels, bins=30, alpha=0.7, color='purple')
            axes[1, 1].set_title('Activity Distribution')
            axes[1, 1].set_xlabel('Active Neuron Count')
            axes[1, 1].set_ylabel('Frequency')
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        plt.close()
```

## Advanced Command Line Interface Examples

### Example 1: Complex Experiment with Advanced Configuration

```bash
#!/bin/bash

# Advanced experiment script
# Run a Phase 6 integration experiment with comprehensive analysis

EXPERIMENT_NAME="advanced_phase6_integration"
OUTPUT_DIR="results/${EXPERIMENT_NAME}_$(date +%Y%m%d_%H%M%S)"
CONFIG_FILE="configs/advanced_config.json"

# Create output directory
mkdir -p "${OUTPUT_DIR}"

# Run the experiment with advanced options
nlm \
    --brain-config "${CONFIG_FILE}" \
    --steps 50000 \
    --checkpoint-interval 5000 \
    --experiment phase6_integration \
    --verbose \
    --analyze \
    --collect-metrics \
    --output "${OUTPUT_DIR}/metrics.json" \
    --log "${OUTPUT_DIR}/experiment.log" \
    --plot \
    --visualization \
    --save-brain "${OUTPUT_DIR}/final_brain.json"

# Post-process results
python analyze_results.py \
    --input "${OUTPUT_DIR}/metrics.json" \
    --output "${OUTPUT_DIR}/analysis.json" \
    --patterns memory,neural,behavior \
    --clustering hierarchical

# Generate final report
python generate_report.py \
    --input "${OUTPUT_DIR}/analysis.json" \
    --template "reports/advanced_report.html" \
    --output "${OUTPUT_DIR}/final_report.html"

echo "Experiment completed successfully! Results saved to ${OUTPUT_DIR}"
```

### Example 2: Agent Training with Advanced Features

```bash
#!/bin/bash

# Advanced agent training script
# Train an agent using curiosity-driven exploration with advanced features

TRAINING_SCRIPT="scripts/advanced_training.py"
CONFIG_DIR="configs/agent_training"
RESULTS_DIR="results/agent_training"
MODEL_DIR="models"

# Create directories
mkdir -p "${RESULTS_DIR}"
mkdir -p "${MODEL_DIR}"

# Train agent with advanced configuration
python "${TRAINING_SCRIPT}" \
    --config-dir "${CONFIG_DIR}" \
    --episodes 2000 \
    --max-steps-per-episode 1000 \
    --environment "custom_world.py" \
    --algorithm "reinforcement_learning" \
    --exploration "curiosity" \
    --novelty-threshold 0.7 \
    --learning-rate 0.001 \
    --discount-factor 0.99 \
    --batch-size 32 \
    --target-update 100 \
    --epsilon-decay 0.9995 \
    --final-epsilon 0.01 \
    --checkpoint-interval 500 \
    --save-models \
    --model-dir "${MODEL_DIR}" \
    --results-dir "${RESULTS_DIR}" \
    --log-file "${RESULTS_DIR}/training.log" \
    --tensorboard \
    --tensorboard-dir "${RESULTS_DIR}/tensorboard" \
    --cuda \.

# Evaluate trained agent
evaluate_agent.py \
    --model "${MODEL_DIR}/final_agent.pth" \
    --test-worlds 20 \
    --episodes 100 \
    --metrics "reward,success_rate,efficiency,curiosity" \
    --output "${RESULTS_DIR}/evaluation.json"

# Generate performance analysis
analyze_performance.py \
    --input "${RESULTS_DIR}/training.log" \
    --output "${RESULTS_DIR}/performance_analysis.json" \
    --plot \
    --visualization

echo "Agent training completed! Results saved to ${RESULTS_DIR}"
```

### Example 3: Memory Analysis with Advanced Tools

```bash
#!/bin/bash

# Advanced memory analysis script
# Perform comprehensive memory analysis on saved brain states

ANALYSIS_SCRIPT="scripts/memory_analysis.py"
CHECKPOINT_DIR="checkpoints"
ANALYSIS_DIR="analysis_results"

# Create analysis directory
mkdir -p "${ANALYSIS_DIR}"

# Analyze memory consolidation patterns
python "${ANALYSIS_SCRIPT}" \
    --input-dir "${CHECKPOINT_DIR}" \
    --output-dir "${ANALYSIS_DIR}" \
    --analysis-type "consolidation" \
    --window-size 1000 \
    --consolidation-threshold 0.3 \
    --forgetting-rate 0.05 \
    --pattern-detection \
    --clustering \
    --dimensionality-reduction \
    --t-sne \
    --output-format "json,png,html" \
    --generate-report \
    --report-template "reports/memory_report.html" \
    --custom-metrics \
    --custom-metric-expressions "strength,replay_frequency,extinction_rate" \
    --compare-sessions \
    --baseline-session "session_001" \
    --comparison-metrics "similarity,divergence,stability"

# Extract memory patterns and associations
extract_memory_patterns.py \
    --input "${ANALYSIS_DIR}/consolidation_analysis.json" \
    --patterns "co_activation,sequential,contextual" \
    --threshold "0.7" \
    --min-support "50" \
    --output "${ANALYSIS_DIR}/memory_patterns.json"

# Visualize memory dynamics
visualize_memory_dynamics.py \
    --input "${ANALYSIS_DIR}/consolidation_analysis.json" \
    --output "${ANALYSIS_DIR}/memory_dynamics.html" \
    --animation \
    --update-rate "100ms" \
    --show-consolidation \
    --show-replay \
    --show-forgetting

echo "Memory analysis completed! Results saved to ${ANALYSIS_DIR}"
```

## Advanced Configuration File Format

```json
{
    "advanced_brain": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.08,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.002,
                "tau_plus": 25.0,
                "tau_minus": 25.0,
                "weight_max": 1.0,
                "weight_min": -1.0
            },
            "hebbian": {
                "enable": true,
                "association_window": 100,
                "learning_rate": 0.001
            },
            "structural": {
                "enable": true,
                "synaptogenesis_rate": 0.0002,
                "pruning_rate": 0.00005,
                "max_synapses": 10000,
                "min_synapses": 100,
                "homeostasis_enabled": true,
                "homeostasis_target": 2000
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 2.0,
                "baseline": -0.1,
                "plasticity_factor": 1.5,
                "signaling_strength": 0.8
            },
            "curiosity": {
                "enable": true,
                "exploration_rate": 0.5,
                "exploration_bonus": 0.5,
                "max_curiosity": 2.0,
                "decay_rate": 0.95
            },
            "novelty": {
                "enable": true,
                "decay_rate": 0.98,
                "sensitivity": 1.0,
                "threshold": 0.3
            }
        },
        "development": {
            "stages": {
                "Initial": {
                    "duration": 60.0,
                    "plasticity_modifier": 1.0,
                    "synaptogenesis_rate": 0.001,
                    "pruning_rate": 0.0001
                },
                "CriticalPeriod": {
                    "duration": 240.0,
                    "plasticity_modifier": 0.8,
                    "synaptogenesis_rate": 0.0005,
                    "pruning_rate": 0.00005
                },
                "Maturation": {
                    "duration": 720.0,
                    "plasticity_modifier": 0.5,
                    "synaptogenesis_rate": 0.0002,
                    "pruning_rate": 0.00001
                },
                "Adult": {
                    "duration": 1800.0,
                    "plasticity_modifier": 0.2,
                    "synaptogenesis_rate": 0.00005,
                    "pruning_rate": 0.000001
                }
            },
            "transition_smoothness": 0.1,
            "plasticity_variation": 0.3
        }
    },
    "advanced_agent": {
        "neural_architecture": {
            "input_layer_size": 256,
            "hidden_layers": [
                {"size": 512, "activation": "relu", "dropout": 0.2},
                {"size": 256, "activation": "tanh", "dropout": 0.2},
                {"size": 128, "activation": "sigmoid", "dropout": 0.1}
            ],
            "output_layer_size": 9,
            "output_activation": "softmax"
        },
        "behavioral_planning": {
            "planning_depth": 5,
            "action_selection": "softmax",
            "exploration_strategy": "epsilon_greedy",
            "epsilon_decay": 0.995,
            "final_epsilon": 0.01,
            "epsilon_start": 1.0
        },
        "memory_systems": {
            "working_memory_size": 100,
            "working_memory_decay": 0.9,
            "episodic_memory_capacity": 1000,
            "semantic_memory_enabled": true,
            "procedural_memory_enabled": true
        }
    }
}
```

## Summary

The advanced NLM features provide researchers and advanced users with:

1. **Enhanced Command Line Interface** - Comprehensive CLI for running experiments, analysis, and visualization
2. **Advanced Configuration Options** - Detailed configuration for brain, agent, and world parameters
3. **Advanced Experiment Framework** - Comprehensive experiment types with analysis capabilities
4. **Advanced Analysis Tools** - Neural activity analysis, synaptic connectivity analysis, memory analysis
5. **Advanced Visualization** - Detailed visualizations for brain connectivity and activity patterns

These features enable sophisticated research and experimentation with NLM, supporting advanced use cases in computational neuroscience and artificial intelligence research.