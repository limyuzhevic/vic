# NLM Visualization Tools

## Overview

This document provides comprehensive documentation for visualization tools available in the NLM project. Visualization is a critical component for understanding and debugging neural network behavior, especially for complex systems like NLM with multiple integrated components.

## Available Visualization Tools

### 1. NLM Visualization Module (C++)

**Location:** `src/visualization/`

**Description:** Built-in visualization system for C++ simulations.

**Key Features:**
- Real-time neural activity monitoring
- Spike visualization
- Memory system displays
- World simulation visualization
- Integration dashboards

**Installation and Usage:**

```bash
# Build with visualization support
cmake .. -DENABLE_VISUALIZATION=ON
make -j4

# Run demo with visualization
./nlm_phase4_demo --enable-visualization
```

**Visualization Outputs:**
- **Neural Activity Plot:** Real-time firing rates of neuron populations
- **Spike Raster Plot:** Individual neuron spike timing
- **Memory Integration Display:** Working memory traces and episodic memory storage
- **World Visualization:** Agent position, landmarks, and sensory input
- **Neuromodulation Metrics:** Dopamine, curiosity, and novelty levels over time
- **Development Timeline:** Brain developmental stages and changes

**API Example:**
```cpp
#include "visualization/Visualizer.hpp"

namespace nlm {
    class Visualizer {
    public:
        static void showNeuralActivity(const Brain& brain);
        static void showMemorySystems(const Brain& brain);
        static void showWorldVisualization(const World& world);
        static void showNeuromodulation(const Brain& brain);
        static void showIntegrationDashboard(const Brain& brain, const World& world);
    };
}

// Usage in demo
Visualizer::showIntegrationDashboard(brain, world);
```

### 2. Python Visualization (pynlm)

**Description:** Visualization tools accessible via Python API for post-processing and analysis.

**Key Libraries:**
- `nlmviz` - Main visualization module
- `nlmanimate` - Animation of simulation runs
- `nlmanalyze` - Statistical analysis and visualization
- `nlm3d` - 3D neural network visualization

**Installation:**
```bash
pip install nlm[visualization]

# Or install specific modules
pip install nlmviz nlmanimate nlmanalyze nlm3d
```

**Python Visualization Examples:**

#### Neural Activity Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

# Run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect firing rate data
firing_rates = []
for step in range(1000):
    brain.step(step)
    firing_rates.append(brain.getAverageFiringRate())

# Plot firing rate over time
plt.figure(figsize=(12, 5))
plt.subplot(1, 2, 1)
plt.plot(firing_rates)
plt.title("Average Firing Rate Over Time")
plt.xlabel("Simulation Step")
plt.ylabel("Firing Rate (Hz)")
plt.grid(True, alpha=0.3)

# Plot firing rate distribution
plt.subplot(1, 2, 2)
neuron_counts = brain.getRegionCount()
for i in range(neuron_counts):
    region = brain.getRegion(i)
    # Get firing statistics for each region
    plt.hist([], alpha=0.5, label=f"Region {i}")

plt.title("Firing Rate Distribution")
plt.xlabel("Firing Rate (Hz)")
plt.ylabel("Count")
plt.legend()
plt.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig("neural_activity.png")
plt.show()
```

#### Memory System Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import seaborn as sns

# Run simulation with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect memory metrics
memory_data = {
    'working_memory_traces': [],
    'episodic_memory_count': [],
    'working_memory_capacity': brain.getWorkingMemory().getCapacity() if brain.getWorkingMemory() else 0,
    'development_stage': []
}

for step in range(500):
    brain.step(step)
    
    if brain.getWorkingMemory():
        memory_data['working_memory_traces'].append(
            brain.getWorkingMemory().getActiveTraces()
        )
    
    if brain.getEpisodicMemory():
        memory_data['episodic_memory_count'].append(
            brain.getEpisodicMemory().getEpisodeCount()
        )
    
    memory_data['development_stage'].append(
        brain.getDevelopmentalStage()
    )

# Create comprehensive memory visualization
fig, axes = plt.subplots(2, 2, figsize=(15, 12))

# Working memory traces over time
axes[0, 0].plot(memory_data['working_memory_traces'])
axes[0, 0].set_title("Working Memory Traces Over Time")
axes[0, 0].set_xlabel("Step")
axes[0, 0].set_ylabel("Active Traces")
axes[0, 0].grid(True, alpha=0.3)

# Episodic memory growth
axes[0, 1].plot(memory_data['episodic_memory_count'])
axes[0, 1].set_title("Episodic Memory Growth")
axes[0, 1].set_xlabel("Step")
axes[0, 1].set_ylabel("Episode Count")
axes[0, 1].grid(True, alpha=0.3)

# Development stages
stage_map = {
    pynlm.DevelopmentalStage.Initial: 0,
    pynlm.DevelopmentalStage.CriticalPeriod: 1,
    pynlm.DevelopmentalStage.Maturation: 2,
    pynlm.DevelopmentalStage.Adult: 3,
    pynlm.DevelopmentalStage.Aging: 4
}

development_stages = [stage_map[stage] for stage in memory_data['development_stage']]
axes[1, 0].plot(development_stages)
axes[1, 0].set_title("Brain Development Stages")
axes[1, 0].set_xlabel("Step")
axes[1, 0].set_ylabel("Stage")
axes[1, 0].set_yticks(list(stage_map.values()))
axes[1, 0].set_yticklabels(list(stage_map.keys()))
axes[1, 0].grid(True, alpha=0.3)

# Memory capacity vs. utilization
axes[1, 1].scatter(memory_data['working_memory_traces'], 
                 memory_data['episodic_memory_count'],
                 alpha=0.6, c=development_stages, cmap='viridis')
axes[1, 1].set_title("Memory Utilization vs. Capacity")
axes[1, 1].set_xlabel("Working Memory Traces")
axes[1, 1].set_ylabel("Episodic Memory Count")
axes[1, 1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig("memory_visualization.png", dpi=150, bbox_inches='tight')
plt.show()
```

#### 3D Neural Network Visualization

```python
import pynlm
import numpy as np
from nlm3d import Neural3DVisualizer

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation to collect data
simulation_steps = 200
neuron_positions = []
synapse_connections = []
spike_times = []

for step in range(simulation_steps):
    brain.step(step)
    
    # Collect neuron positions (simplified: neurons arranged in 3D space)
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        for population in region.getPopulations():
            for neuron in population.getNeurons():
                neuron_positions.append([
                    np.random.uniform(-10, 10),  # x position
                    np.random.uniform(-10, 10),  # y position
                    np.random.uniform(-10, 10)   # z position
                ])
    
    # Collect synapse information
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            synapse_connections.append([
                synapse.getSourceNeuron().index(),
                synapse.getDestinationNeuron().index(),
                synapse.getWeight()
            ])

# Create 3D visualization
visualizer = Neural3DVisualizer(neuron_positions, synapse_connections)

# Configure visualization
visualizer.set_neuron_color_by_activity('excitatory', 'red')
visualizer.set_neuron_color_by_activity('inhibitory', 'blue')
visualizer.set_synapse_width_by_weight(True)
visualizer.set_spike_animation(True)

# Add interactive elements
visualizer.add_legend(['Excitatory Neurons', 'Inhibitory Neurons'])
visualizer.add_time_display()
visualizer.add_connection_info()

# Render visualization
visualizer.render(title="3D NLM Neural Network")
visualizer.save("neural_network_3d.png")
visualizer.show()
```

### 3. Animation and Time-Series Visualization

**Description:** Create animations and time-series visualizations of simulation runs.

**Key Features:**
- Real-time animation of neural activity
- Time-series plotting for multiple metrics
- Comparative visualization of different runs
- Interactive widgets for parameter exploration

**Installation and Usage:**
```bash
pip install nlmanimate
```

**Animation Example:**
```python
import nlmanimate as anim
import pynlm

# Run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create animation
animation = anim.NLMSimulationAnimation(
    brain,
    duration=10,  # seconds
    fps=30,
    width=1200,
    height=800
)

# Add neural activity visualization
animation.add_neural_activity_plot(
    metric='firing_rate',
    region='all',
    color='red'
)

# Add memory visualization
animation.add_memory_traces_plot(
    working_memory=True,
    episodic_memory=True
)

# Add world visualization
animation.add_world_view(
    show_agent=True,
    show_landmarks=True,
    show_sensory_input=True
)

# Create animation
animation.create()

# Save animation
animation.save("nlm_simulation.gif")

# Or create video
animation.save_video("nlm_simulation.mp4", codec='ffmpeg')
```

### 4. Analysis and Statistical Visualization

**Description:** Statistical analysis and visualization tools for experimental data.

**Key Features:**
- Statistical significance testing
- Confidence interval visualization
- Performance comparison across conditions
- Heatmap visualization of neural patterns

**Installation and Usage:**
```bash
pip install nlmanalyze
```

**Statistical Analysis Example:**
```python
import nlmanalyze as ana
import pynlm
import pandas as pd
import matplotlib.pyplot as plt

# Run multiple simulation replicates
conditions = [
    {'neuron_count': 500, 'seed': 42},
    {'neuron_count': 500, 'seed': 123},
    {'neuron_count': 800, 'seed': 456},
    {'neuron_count': 1000, 'seed': 789}
]

results = []

for i, cond in enumerate(conditions):
    print(f"Running condition {i+1}/{len(conditions)}: {cond}")
    
    # Run simulation multiple times for statistics
    for rep in range(5):  # 5 replicates per condition
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", cond['neuron_count'])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(cond.get('steps', 500)):
            brain.step(step)
        
        # Collect metrics
        metrics = {
            'condition': i,
            'replicate': rep,
            'neuron_count': cond['neuron_count'],
            'seed': cond['seed'],
            'total_reward': np.random.random() * 0.5,  # Simulated metric
            'avg_firing_rate': np.random.random() * 10 + 5,  # Simulated metric
            'final_development_stage': np.random.choice(list(pynlm.DevelopmentalStage)),
            'memory_episodes': np.random.randint(0, 50)
        }
        
        results.append(metrics)

# Convert to DataFrame
results_df = pd.DataFrame(results)

# Create comprehensive statistical visualization
fig, axes = plt.subplots(2, 3, figsize=(18, 12))

# Neuron count effect on total reward
ana.plot_neuron_count_effect(results_df, ax=axes[0, 0])

# Distribution of performance metrics
ana.plot_performance_distributions(results_df, ax=axes[0, 1])

# Developmental stage progression
ana.plot_development_trends(results_df, ax=axes[0, 2])

# Memory formation by neuron count
ana.plot_memory_effects(results_df, ax=axes[1, 0])

# Statistical significance tests
ana.plot_statistical_tests(results_df, ax=axes[1, 1])

# Heatmap of conditions vs. replicates
ana.plot_condition_heatmap(results_df, ax=axes[1, 2])

plt.tight_layout()
plt.savefig("statistical_analysis.png", dpi=150, bbox_inches='tight')
plt.show()

# Perform statistical tests
anova_results = ana.run_anova_analysis(results_df)
print("ANOVA Results:")
print(anova_results)

# Post-hoc tests
posthoc_results = ana.run_posthoc_tests(results_df, anova_results)
print("Post-hoc Test Results:")
print(posthoc_results)
```

## Visualization File Formats

### 2D Plots
- **PNG** - High-quality static images
- **SVG** - Scalable vector graphics for publications
- **PDF** - Professional document formatting

### 3D Visualizations
- **GLTF/GLB** - 3D model files for web visualization
- **OBJ** - Wavefront object files
- **STL** - Stereolithography files for 3D printing

### Animations
- **GIF** - Lightweight animations for web
- **MP4** - High-quality video files
- **WebM** - Web-optimized video format

### Interactive Visualizations
- **HTML/JS** - Web-based interactive visualizations
- **Jupyter Notebooks** - Interactive analysis environments
- **Streamlit** - Web apps for visualization

## Visualization Best Practices

### 1. Color Schemes
- Use colorblind-friendly palettes
- Use meaningful color mappings (e.g., heat maps for intensity)
- Maintain consistency across visualizations

### 2. Labeling and Annotation
- Include clear axis labels
- Add informative titles
- Include scale references
- Mark significant events or conditions

### 3. Data Quality
- Ensure adequate sampling for smooth plots
- Check for outliers and handle appropriately
- Use appropriate statistical summaries

### 4. Performance Considerations
- Optimize for real-time visualization
- Use efficient data structures
- Implement progressive loading for large datasets

## Integration with NLM CLI

### Command-Line Visualization Options

```bash
# Run demo with visualization
./nlm_phase6_demo --enable-visualization --viz-type=3d

# Save visualization to file
./nlm_phase4_demo --enable-visualization --viz-output=neural_network.png

# Generate statistical plots
./nlm_test --generate-plots --output-dir=./results/viz

# Create animation from simulation data
./nlm --export-animation --output=simulation.gif --fps=30
```

### API Integration

```cpp
#include "visualization/Visualizer.hpp"

int main() {
    // Initialize NLM systems
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Initialize components
    brain->initialize();
    agent->initialize(*world);
    
    // Create visualizer
    auto visualizer = std::make_shared<Visualizer>();
    
    // Configure visualization
    visualizer->enable_real_time_drawing();
    visualizer->set_update_rate(30); // 30 Hz
    visualizer->add_neural_activity_display();
    visualizer->add_memory_monitor();
    visualizer->add_world_view();
    
    // Run simulation with visualization
    for (int step = 0; step < 1000; ++step) {
        // Update simulation
        world->update(0.1);
        agent->processSensoryInput(world->getSensoryPercept());
        brain->step(step);
        
        // Update visualization
        visualizer->update(brain, world, step);
        
        // Process events
        visualizer->process_events();
    }
    
    // Save visualization
    visualizer->save("final_visualization.png");
    
    return 0;
}
```

## Visualization Configuration

### Configuration File Example

```json
{
    "visualization": {
        "enabled": true,
        "backend": "openGL",
        "resolution": {
            "width": 1920,
            "height": 1080
        },
        "fps": 60,
        "components": {
            "neural_activity": {
                "enabled": true,
                "type": "2d_plot",
                "data": ["firing_rate", "excitatory_inhibition", "inhibitory_activity"]
            },
            "memory_systems": {
                "enabled": true,
                "type": "3d_visualization",
                "show_working_memory": true,
                "show_episodic_memory": true,
                "show_associative_memory": false
            },
            "world_simulation": {
                "enabled": true,
                "type": "top_down_view",
                "show_agent": true,
                "show_landmarks": true,
                "show_resources": true
            },
            "neuromodulation": {
                "enabled": true,
                "type": "time_series",
                "metrics": ["dopamine", "curiosity", "novelty"],
                "duration": 300
            }
        },
        "output": {
            "format": "png",
            "quality": 95,
            "path": "./visualizations/",
            "prefix": "nlm_simulation_"
        }
    }
}
```

## Summary

The NLM visualization system provides comprehensive tools for understanding and analyzing neural network behavior:

1. **Built-in C++ Visualization** for real-time simulation monitoring
2. **Python Visualization Libraries** for advanced analysis and post-processing
3. **Animation and Time-Series Tools** for creating dynamic visualizations
4. **Statistical Analysis Tools** for scientific visualization and reporting
5. **3D Neural Network Visualization** for spatial understanding of connectivity

These visualization tools integrate seamlessly with the NLM CLI and provide multiple output formats for different use cases, from real-time debugging to publication-quality figures.
