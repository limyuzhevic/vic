# NLM Visualization Tutorial
# Step-by-Step Guide to Using Visualization and Monitoring Tools

This tutorial provides step-by-step instructions for using the NLM Visualization and Monitoring Tools to visualize neural simulations, monitor system performance, and analyze experiment results.

## Prerequisites

Before starting with the visualization and monitoring tools, ensure you have:

1. **NLM Installation**: NLM (Neural Learning Machine) is installed and running
2. **Data Files**: You have neural simulation data or experiment results in the supported format
3. **Build Environment**: The visualization tools are built and available in your PATH

### Installation Steps

1. **Clone the NLM Repository**
   ```bash
   git clone https://github.com/Kilo-Org/kilocode.git
   cd kilocode
   ```

2. **Build NLM with Visualization Support**
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make -j$(nproc)
   ```

3. **Verify Installation**
   ```bash
   cd ..
   ./nlm-cli --help
   ```

## Tutorial Overview

This tutorial covers:

1. **Basic Visualization**: Simple visualization of neural data
2. **Simulation Visualization**: Visualizing simulation results
3. **Brain Visualization**: Visualizing brain structure and connectivity
4. **Experiment Visualization**: Visualizing experiment results and comparisons
5. **Performance Monitoring**: Monitoring system and brain performance
6. **Real-time Monitoring**: Real-time visualization and monitoring
7. **Data Export/Import**: Working with external visualization tools
8. **Report Generation**: Creating comprehensive reports

## Step 1: Basic Visualization

### 1.1 Visualize Brain Structure

Let's start by visualizing the structure of a simulated brain:

1. **Create a sample brain file** (if you don't have one):
   ```bash
   ./nlm-cli brain-info --create-sample sample_brain.json
   ```

2. **Visualize brain structure**:
   ```bash
   ./nlm-cli viz-brain-structure --brain sample_brain.json --region-type all --output brain_structure.png
   ```

**What this does**:
- Loads the brain state from `sample_brain.json`
- Visualizes all brain regions (cortex, subcortical, brainstem)
- Creates a detailed structure visualization in PNG format
- Shows neuron counts, region types, and connectivity

### 1.2 Visualize Network Connectivity

1. **Create a connectivity visualization**:
   ```bash
   ./nlm-cli viz-brain-connectivity --brain sample_brain.json --threshold 0.5 --output connectivity_matrix.png
   ```

**What this does**:
- Creates an adjacency matrix visualization
- Shows connections between brain regions above the threshold
- Displays correlation and efficiency metrics
- Generates a connectivity heatmap

## Step 2: Simulation Visualization

### 2.1 Visualize Neural Dynamics

1. **Run a simulation** (if you have a running simulation):
   ```bash
   ./nlm-cli simulate --create-sample simulation.dat
   ```

2. **Visualize membrane potential dynamics**:
   ```bash
   ./nlm-cli viz-dynamics --simulation simulation.dat --variable membrane_potential --output dynamics.png
   ```

**What this does**:
- Loads time-series data from `simulation.dat`
- Plots membrane potential over time
- Shows spike events and firing patterns
- Displays statistics like mean, max, and min voltages

### 2.2 Visualize Activity Patterns

1. **Analyze activity patterns**:
   ```bash
   ./nlm-cli viz-activity-patterns --simulation simulation.dat --neuron-type excitatory --pattern-type periodic --output activity_patterns.png
   ```

**What this does**:
- Analyzes firing patterns of excitatory neurons
- Identifies periodic and bursting activity
- Shows activity statistics and patterns
- Provides pattern classification

## Step 3: Brain Visualization

### 3.1 Visualize Space-Time Activity

1. **Create space-time visualization**:
   ```bash
   ./nlm-cli viz-space-time --simulation simulation.dat --space-scale 1.0 --time-scale 10.0 --visualization heatmap --output spacetime.png
   ```

**What this does**:
- Creates a 2D projection of space-time activity
- Shows how activity propagates through the brain over time
- Uses heatmap coloring to show activity intensity
- Displays propagation speed and direction

### 3.2 Visualize Synapse Dynamics

1. **Visualize synaptic changes**:
   ```bash
   ./nlm-cli viz-synapses --brain sample_brain.json --plasticity --type structural --output synapse_dynamics.png
   ```

**What this does**:
- Shows structural and functional synaptic connections
- Displays plasticity changes over time
- Shows weight changes and connection patterns
- Visualizes synaptogenesis and pruning

## Step 4: Experiment Visualization

### 4.1 Visualize Experiment Results

1. **Create experiment visualization**:
   ```bash
   ./nlm-cli viz-experiment-results --experiment learning_experiment.json --metric total_reward --output experiment_results.png
   ```

**What this does**:
- Loads experiment results from `learning_experiment.json`
- Plots key performance metrics over time
- Shows learning curves and performance trends
- Compares different experimental conditions

### 4.2 Compare Experiment Results

1. **Compare two experiments**:
   ```bash
   ./nlm-cli viz-experiment-comparison --experiments exp1.json exp2.json --metric learning_rate --output comparison.png
   ```

**What this does**:
- Compares performance metrics between two experiments
- Shows statistical significance of differences
- Displays effect sizes and confidence intervals
- Generates side-by-side comparisons

### 4.3 Visualize Experiment Timeline

1. **Create experiment timeline**:
   ```bash
   ./nlm-cli viz-experiment-timeline --experiment learning_experiment.json --include-metrics --output experiment_timeline.png
   ```

**What this does**:
- Shows the chronological sequence of experiment events
- Displays timing of key phases (setup, data collection, analysis)
- Includes performance metrics and statistics
- Shows experiment progress and milestones

## Step 5: Performance Monitoring

### 5.1 Monitor System Performance

1. **Start system monitoring**:
   ```bash
   ./nlm-cli monitor-system --output system_monitor.log --interval 1.0
   ```

**What this does**:
- Continuously monitors CPU, memory, and disk usage
- Logs performance metrics to `system_monitor.log`
- Samples at 1-second intervals
- Can be stopped with Ctrl+C

### 5.2 Monitor Brain Performance

1. **Monitor brain metrics**:
   ```bash
   ./nlm-cli monitor-brain --brain sample_brain.json --output brain_performance.log
   ```

**What this does**:
- Monitors brain-specific metrics (firing rates, connectivity, plasticity)
- Logs performance to `brain_performance.log`
- Shows real-time brain health and performance
- Tracks learning progress and adaptation

### 5.3 Monitor Plasticity Mechanisms

1. **Track plasticity changes**:
   ```bash
   ./nlm-cli monitor-plasticity --brain sample_brain.json --output plasticity_monitor.log
   ```

**What this does**:
- Monitors synaptic weight changes over time
- Tracks STDP and Hebbian plasticity
- Shows potentiation and depression patterns
- Logs plasticity statistics and trends

## Step 6: Real-time Monitoring

### 6.1 Start Real-time System Monitoring

1. **Create real-time monitoring dashboard**:
   ```bash
   ./nlm-cli live-monitor --backend web --port 8080 --host 0.0.0.0
   ```

**What this does**:
- Starts a web-based real-time monitoring dashboard
- Listens on port 8080 for web connections
- Provides real-time system metrics
- Can be accessed via browser at http://localhost:8080

### 6.2 Real-time Brain Monitoring

1. **Monitor brain in real-time**:
   ```bash
   ./nlm-cli live-brain --brain sample_brain.json --update-interval 0.1
   ```

**What this does**:
- Continuously monitors brain state with 0.1-second updates
- Shows real-time neural activity
- Displays network dynamics and connectivity
- Updates visualization in real-time

### 6.3 Real-time Simulation Monitoring

1. **Monitor simulation in real-time**:
   ```bash
   ./nlm-cli live-simulation --simulation simulation.dat --update-rate 10.0
   ```

**What this does**:
- Monitors simulation progress with 10 updates per second
- Shows current simulation state and metrics
- Updates visualization as simulation progresses
- Provides real-time feedback on simulation performance

## Step 7: Data Export/Import

### 7.1 Export Data for External Visualization

1. **Export simulation data**:
   ```bash
   ./nlm-cli viz-data-export --simulation simulation.dat --format csv --output simulation_data.csv
   ```

**What this does**:
- Exports simulation data to CSV format
- Includes time-series data and metadata
- Compatible with external visualization tools
- Can be imported into Python, R, or other tools

### 7.2 Import Data for Visualization

1. **Import external data**:
   ```bash
   ./nlm-cli viz-data-import --file external_data.csv --neuron-data --output imported_brain.json
   ```

**What this does**:
- Imports CSV data into NLM format
- Creates brain state file from external data
- Supports various data formats and structures
- Can be used for analysis and visualization

## Step 8: Report Generation

### 8.1 Generate Experiment Report

1. **Create comprehensive report**:
   ```bash
   ./nlm-cli generate-report --experiment learning_experiment.json --format pdf --output experiment_report.pdf
   ```

**What this does**:
- Generates a PDF report with all experiment results
- Includes visualizations, statistics, and analysis
- Provides comprehensive documentation of experiment
- Can be shared with collaborators

### 8.2 Generate Comparison Report

1. **Create comparison report**:
   ```bash
   ./nlm-cli generate-comparison --experiments exp1.json exp2.json --format html --output comparison_report.html
   ```

**What this does**:
- Generates an HTML report comparing experiments
- Includes side-by-side visualizations and statistics
- Shows statistical significance and effect sizes
- Provides detailed analysis and interpretation

### 8.3 Generate Metrics Report

1. **Create metrics report**:
   ```bash
   ./nlm-cli generate-metrics --experiment learning_experiment.json --format markdown --output metrics_report.md
   ```

**What this does**:
- Generates a markdown report with key metrics
- Includes tables, charts, and statistical summaries
- Easy to integrate with documentation
- Can be converted to other formats

## Advanced Usage

### Using Configuration Files

For complex workflows, use configuration files:

1. **Create a configuration file**:
   ```bash
   ./nlm-cli config-info --create config.json
   ```

2. **Use configuration in commands**:
   ```bash
   ./nlm-cli viz-brain-structure --config config.json --brain sample_brain.json
   ```

### Batch Processing

For processing multiple files:

1. **Process multiple brain files**:
   ```bash
   for brain in brains/*.json; do
     ./nlm-cli viz-brain-structure --brain "$brain" --output "viz_${brain%.json}.png"
   done
   ```

2. **Process multiple experiments**:
   ```bash
   for exp in experiments/*.json; do
     ./nlm-cli viz-experiment-results --experiment "$exp" --output "results_${exp%.json}.png"
   done
   ```

### Scripting and Automation

Create Python scripts for automation:

```python
import subprocess
import os

# Run visualization
for brain_file in os.listdir("brains"):
    if brain_file.endswith(".json"):
        subprocess.run([
            "nlm-cli", "viz-brain-structure",
            "--brain", f"brains/{brain_file}",
            "--output", f"output/{brain_file}_structure.png"
        ])
```

## Troubleshooting

### Common Issues

1. **Tool Not Found**: Ensure the tool is built and in your PATH
2. **Missing Dependencies**: Install required libraries
3. **File Not Found**: Check file paths and permissions
4. **Format Not Supported**: Verify file format is supported
5. **Memory Issues**: Monitor memory usage for large datasets

### Getting Help

1. **Command-line Help**: Use `--help` flag for each tool
2. **Documentation**: Refer to the visualization documentation
3. **Error Messages**: Read error messages carefully
4. **Examples**: Check the examples directory for sample files
5. **Community**: Join the NLM community for support

## Next Steps

### Learning Resources

1. **Documentation**: Read the visualization tool documentation
2. **Examples**: Check the examples directory for sample files
3. **Tutorials**: Follow more advanced tutorials
4. **Best Practices**: Learn best practices for visualization
5. **Performance**: Optimize performance for your specific use case

### Advanced Features

1. **Custom Visualizations**: Create custom visualization tools
2. **Integration**: Integrate with other tools and platforms
3. **Automation**: Automate visualization workflows
4. **Real-time**: Implement real-time visualization systems
5. **Cloud**: Deploy visualization tools to the cloud

### Research Applications

1. **Neural Modeling**: Visualize neural network models
2. **Brain Mapping**: Create brain connectivity maps
3. **Learning Analysis**: Analyze learning curves and patterns
4. **Performance Monitoring**: Monitor system and brain performance
5. **Experimental Results**: Document and analyze experimental results

---

This tutorial provides a comprehensive introduction to using the NLM Visualization and Monitoring Tools. As you become more familiar with the tools, you can explore more advanced features and create custom visualization workflows for your specific research needs.

For the most up-to-date information and additional tutorials, visit the NLM documentation and community resources.