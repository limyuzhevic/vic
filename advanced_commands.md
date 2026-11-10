# Advanced NLM Commands for Python Users

This file provides advanced command-line tools and utilities for working with NLM in Python.

## Overview

Advanced commands provide enhanced functionality for:

• Configuration management and validation
• Performance monitoring and profiling
• Batch processing and experiment management
• Advanced analysis and visualization
• Debugging and troubleshooting
• Research pipeline management
• Production deployment

## Installation

```bash
pip install -e .
```

## Available Commands

### Configuration Management Commands

#### `nlm-config-validate`

Validate NLM configurations with advanced validation rules.

```bash
nlm-config-validate [options] CONFIG_FILE

Options:
  --schema FILE      Path to JSON schema file for validation
  --strict          Strict validation (fail on warnings)
  --verbose         Verbose output
  --output FILE     Path to output validation report

Examples:
  nlm-config-validate config.json
  nlm-config-validate --strict --schema schema.json experiment.config
```

#### `nlm-config-optimize`

Optimize NLM configurations for performance.

```bash
nlm-config-optimize [options] CONFIG_FILE

Options:
  --target-performance FLOAT   Target performance score (0-100)
  --max-neurons INT           Maximum number of neurons
  --max-memory INT           Maximum memory usage in MB
  --profile-for string        Profile for specific use case (research/production/benchmark)
  --output FILE              Path to optimized configuration file

Examples:
  nlm-config-optimize --target-performance 85 experiment.config
  nlm-config-optimize --max-neurons 5000 --profile-for research config.json
```

#### `nlm-config-merge`

Merge multiple configuration files into a single optimized configuration.

```bash
nlm-config-merge [options] CONFIG_FILE1 CONFIG_FILE2 ...

Options:
  --priority-list FILE       Path to file containing priority list
  --conflict-resolution STR   Conflict resolution strategy (keep-first/merge-best/average)
  --output FILE             Path to merged configuration file
  --validate               Validate merged configuration

Examples:
  nlm-config-merge config1.json config2.json config3.json
  nlm-config-merge --priority-list/priorities.txt --conflict-resolution merge-best *.json
```

### Performance Monitoring Commands

#### `nlm-perf-monitor`

Start real-time performance monitoring for NLM simulations.

```bash
nlm-perf-monitor [options] CONFIG_FILE

Options:
  --output FILE              Path to performance data file
  --format STR              Output format (json/csv/text)
  --sampling-interval INT   Monitoring sampling interval in milliseconds
  --max-duration INT        Maximum monitoring duration in seconds
  --alerts ENABLED          Enable performance alerts
  --alert-threshold FLOAT   Alert threshold for performance metrics

Examples:
  nlm-perf-monitor --output perf_data.json config.json
  nlm-perf-monitor --format csv --sampling-interval 100 simulation.config
```

#### `nlm-perf-analyze`

Analyze performance data and generate reports.

```bash
nlm-perf-analyze [options] DATA_FILE

Options:
  --report-type STR         Report type (summary/detailed/benchmark/comparison)
  --time-window INT         Time window for analysis in seconds
  --metrics STR             Comma-separated list of metrics to analyze
  --output-report FILE      Path to detailed report file
  --generate-plots         Generate visualization plots
  --plot-format STR         Plot output format (png/svg/pdf)

Examples:
  nlm-perf-analyze --report-type detailed --metrics step_time,memory,spike_rate perf_data.json
  nlm-perf-analyze --time-window 3600 --generate-plots simulation_perf.csv
```

### Batch Processing Commands

#### `nlm-batch-run`

Run multiple NLM simulations in batch mode.

```bash
nlm-batch-run [options] CONFIG_FILE

Options:
  --parameter-sweep FILE     Path to parameter sweep configuration file
  --parameter-space STR      Parameter space definition (linear/logarithmic/random)
  --parallel-jobs INT        Number of parallel jobs (default: number of CPU cores)
  --job-timeout INT          Maximum job execution time in seconds
  --retry-count INT          Number of retry attempts for failed jobs
  --continue-on-error        Continue processing even if some jobs fail
  --progress-callback STR    Progress callback function (Python code)
  --output-dir STR          Output directory for results
  --save-individual         Save individual job results
  --merge-results           Merge results into single file

Examples:
  nlm-batch-run --parameter-space linear param_sweep.json
  nlm-batch-run --parallel-jobs 8 --retry-count 2 batch_config.json
```

#### `nlm-batch-manage`

Manage batch processing operations (status, cancellation, monitoring).

```bash
nlm-batch-manage [options] BATCH_ID

Options:
  --action STR             Action to perform (status/list/cancel/pause/resume)
  --output-format STR      Output format (table/json/raw)
  --monitor INTERVAL       Monitoring interval in seconds
  --log-file FILE          Path to batch processing log file
  --export-results         Export results to file

Examples:
  nlm-batch-manage --action status --output-format table batch_123
  nlm-batch-manage --action pause --monitor 30 batch_456
```

### Debugging and Profiling Commands

#### `nlm-debug-run`

Run NLM simulation with advanced debugging enabled.

```bash
nlm-debug-run [options] CONFIG_FILE

Options:
  --debug-level INT         Debug level (0-5)
  --enable-memory-debug    Enable memory debugging
  --enable-step-debug      Enable step-by-step debugging
  --enable-spike-debug     Enable spike debugging
  --enable-plasticity-debug Enable plasticity debugging
  --profile PERFORMANCE    Performance profiling mode (timeline/callstack/memory)
  --output-debug-file FILE Path to debug output file
  --generate-callgraph     Generate function call graph
  --generate-memory-map   Generate memory usage map

Examples:
  nlm-debug-run --debug-level 3 --enable-memory-debug config.json
  nlm-debug-run --profile timeline --generate-callgraph simulation.config
```

#### `nlm-debug-analyze`

Analyze debug data and generate debugging reports.

```bash
nlm-debug-analyze [options] DEBUG_FILE

Options:
  --analysis-type STR       Type of analysis (memory/steps/spikes/plasticity)
  --time-window INT         Analysis time window in seconds
  --output-report FILE      Path to analysis report file
  --generate-heatmap        Generate heat maps
  --detect-leaks           Detect memory leaks
  --performance-bottlenecks Detect performance bottlenecks
  --anomaly-detection       Enable anomaly detection

Examples:
  nlm-debug-analyze --analysis-type memory --detect-leaks debug_data.bin
  nlm-debug-analyze --time-window 3600 --generate-heatmap simulation_debug.json
```

### Custom Behavior Commands

#### `nlm-behavior-create`

Create custom agent behaviors with advanced customization.

```bash
nlm-behavior-create [options] BEHAVIOR_NAME

Options:
  --strategy-file FILE      Path to strategy configuration file
  --action-selection STR    Action selection strategy (random/greedy/exploration/exploitation/boltzmann)
  --reward-calculation STR   Reward calculation strategy (simple/advanced/custom)
  --sensory-processing STR   Sensory processing strategy (direct/filtered/transformed)
  --development-strateg STR  Development strategy (fixed/adaptive/learning)
  --export-behavior        Export behavior to file
  --import-behavior FILE   Import behavior from file
  --test-behavior FILE     Test behavior with sample data

Examples:
  nlm-behavior-create --action-selection boltzmann --reward-calculation advanced exploration_agent
  nlm-behavior-create --strategy-file custom_strategy.json my_custom_agent
```

#### `nlm-behavior-test`

Test custom agent behaviors with performance evaluation.

```bash
nlm-behavior-test [options] BEHAVIOR_FILE ENVIRONMENT_FILE

Options:
  --iterations INT          Number of test iterations
  --performance-metrics STR Comma-separated list of metrics (reward/firing_rate/steps/time)
  --statistical-tests       Perform statistical analysis
  --confidence-level FLOAT  Confidence level for statistical tests (0-1)
  --output-file FILE        Path to test results file
  --generate-plots         Generate performance plots
  --compare-baseline       Compare with baseline behavior

Examples:
  nlm-behavior-test --iterations 100 --performance-metrics reward,steps time my_behavior.json test_env.json
  nlm-behavior-test --statistical-tests --confidence-level 0.95 behavior1.json env1.json
```

### Export/Import Commands

#### `nlm-export-data`

Export NLM simulation data with advanced export options.

```bash
nlm-export-data [options] CONFIG_FILE OUTPUT_DIR

Options:
  --export-format STR        Export format (binary/json/csv/numpy)
  --include-brain-state      Include brain state data
  --include-configuration   Include configuration data
  --include-metrics         Include performance metrics
  --include-debug-data      Include debug data
  --compress-data          Compress exported data
  --create-symlinks         Create symlinks for easy access
  --metadata-file FILE      Path to metadata file
  --thumbnail-size INT      Thumbnail size for visualizations

Examples:
  nlm-export-data --export-format json --include-brain-state --compress-data config.json results/
  nlm-export-data --include-metrics --include-debug-data --create-symlinks experiment.config data/
```

#### `nlm-import-data`

Import NLM simulation data from various formats.

```bash
nlm-import-data [options] INPUT_DIR

Options:
  --assume-format STR       Assume input format (auto/binary/json/csv)
  --validate-data          Validate imported data
  --backup-existing        Backup existing data
  --merge-strategy STR      Merge strategy (replace/append/merge)
  --convert-format STR      Convert to specific format
  --clean-after-import     Clean up temporary files
  --generate-report        Generate import report

Examples:
  nlm-import-data --assume-format auto --validate-data experiment_results/
  nlm-import-data --convert-format json --clean-after-import raw_data/
```

### Analysis and Visualization Commands

#### `nlm-analyze-data`

Analyze NLM simulation data with advanced analysis options.

```bash
nlm-analyze-data [options] INPUT_FILE

Options:
  --analysis-type STR       Type of analysis (comprehensive/spike_dynamics/plasticity/memory/learning/behavior)
  --time-range START:END    Time range for analysis
  --smoothing-window INT   Smoothing window size
  --detrend-data           Detrend data before analysis
  --generate-insights       Generate automated insights
  --confidence-intervals    Calculate confidence intervals
  --statistical-tests       Perform statistical tests
  --output-json FILE       Path to JSON output file
  --output-csv FILE        Path to CSV output file
  --generate-visualizations  Generate analysis visualizations

Examples:
  nlm-analyze-data --analysis-type comprehensive --time-range 0:3600 --generate-insights simulation_data.json
  nlm-analyze-data --detrend-data --statistical-tests --confidence-intervals 0.95 data.bin
```

#### `nlm-visualize-data`

Create visualizations and reports from NLM simulation data.

```bash
nlm-visualize-data [options] INPUT_FILE OUTPUT_DIR

Options:
  --visualization-type STR Visualization type (network/time_series/heatmap/scatter/plots)
  --color-scheme STR       Color scheme for visualizations (viridis/plasma/coolwarm/sequential)
  --style STR              Visualization style (publication/presentation/web)
  --resolution INT         Output resolution (default: 300 dpi)
  --formats STR            Output formats (png/svg/pdf/html)
  --interactive HTML       Generate interactive HTML report
  --embed-python          Embed Python analysis scripts
  --create-dashboard       Create interactive dashboard
  --generate-report-card  Generate summary report card

Examples:
  nlm-visualize-data --visualization-type network --style publication --formats png,pdf data.json results/
  nlm-visualize-data --create-dashboard --embed-python --interactive results/data.csv dashboard/
```

### Research Pipeline Commands

#### `nlm-pipeline-run`

Run complete research pipeline with advanced control.

```bash
nlm-pipeline-run [options] PIPELINE_CONFIG

Options:
  --environment-config STR  Environment configuration (development/production/staging)
  --parameter-sweep        Run parameter sweep optimization
  --hyperparameter-optimization Enable hyperparameter optimization
  --cross-validation INT    Number of cross-validation folds
  --early-stopping PATIENCE Early stopping patience for optimization
  --model-selection STR     Model selection strategy (random/search/gradient)
  --output-pipeline-results Export pipeline results
  --generate-paper-data    Generate publication-ready data
  --create-analysis-reports Create comprehensive analysis reports

Examples:
  nlm-pipeline-run --environment-config production --parameter-sweep pipeline.json
  nlm-pipeline-run --hyperparameter-optimization --cross-validation 5 research_pipeline.json
```

#### `nlm-pipeline-manage`

Manage research pipelines (status, execution, monitoring, reporting).

```bash
nlm-pipeline-manage [options] PIPELINE_ID

Options:
  --action STR             Action (status/start/stop/pause/resume/list)
  --monitoring-interval INT Monitoring interval in seconds
  --generate-progress-report Generate progress report
  --export-metrics         Export metrics to file
  --create-analysis-reports Create comprehensive analysis reports
  --backup-pipeline        Backup pipeline state
  --restore-pipeline FILE  Restore pipeline from backup

Examples:
  nlm-pipeline-manage --action status --generate-progress-report pipeline_123
  nlm-pipeline-manage --action start --monitoring-interval 60 research_pipeline.json
```

### Advanced Configuration Commands

#### `nlm-config-create`

Create advanced NLM configurations with template support.

```bash
nlm-config-create [options] CONFIG_NAME DOMAIN

Options:
  --domain STR             Research domain (neuroscience/ai/cognitive/ml)
  --template-file FILE     Path to configuration template file
  --parameter-sweep STR    Parameter sweep configuration (linear/logarithmic/random)
  --constraints FILE       Path to constraints file
  --validate-constraints   Validate against constraints
  --export-template        Export configuration as template
  --generate-recommendations Generate optimization recommendations

Examples:
  nlm-config-create --domain neuroscience --parameter-sweep linear cognitive_research.json
  nlm-config-create --template-file basic_template.json --validate-constraints production
```

#### `nlm-config-optimize-for`

Optimize configuration for specific use case or research question.

```bash
nlm-config-optimize-for [options] TARGET_METRIC OPTIMIZATION_TYPE

Options:
  --target-metric STR      Target performance metric (spike_rate/memory_usage/step_time/reward)
  --optimization-algorithm STR Algorithm (random_search/grid_search/bayesian/gradient)
  --budget INT             Optimization budget (number of iterations)
  --exploitation-strategy STR Exploration/exploitation strategy
  --multi-objective         Enable multi-objective optimization
  --constraints-file FILE  Path to constraints file
  --output-config FILE     Path to optimized configuration file
  --generate-insights       Generate optimization insights

Examples:
  nlm-config-optimize-for --target-metric reward --optimization-algorithm bayesian high_performance_config
  nlm-config-optimize-for --target-metric step_time --exploitation-strategy ucb2 large_scale_config
```

### Production Deployment Commands

#### `nlm-deploy`

Deploy NLM simulation to production environment.

```bash
nlm-deploy [options] CONFIG_FILE

Options:
  --environment STR        Deployment environment (dev/staging/production)
  --resource-limits STR    Resource limits (memory/cpu/time)
  --auto-scaling          Enable auto-scaling
  --monitoring-enabled     Enable monitoring
  --backup-enabled        Enable automatic backups
  --health-checks         Enable health checks
  --load-balancer         Enable load balancing
  --ssl-enabled           Enable SSL
  --webhook-notify        Send webhook notifications

Examples:
  nlm-deploy --environment production --resource-limits "memory=4GB,cpu=2,time=24h" config.json
  nlm-deploy --auto-scaling --monitoring-enabled --backup-enabled simulation.config
```

#### `nlm-monitor`

Monitor production NLM deployment.

```bash
nlm-monitor [options] DEPLOYMENT_ID

Options:
  --metrics STR            Comma-separated list of metrics to monitor
  --thresholds FILE        Path to threshold configuration file
  --alerts EMAIL          Email addresses for alerts
  --dashboard-url STR     URL for monitoring dashboard
  --health-check-interval INT Health check interval in seconds
  --auto-restart          Auto-restart failed instances
  --log-level STR         Log level (debug/info/warn/error)
  --log-file STR          Path to log file

Examples:
  nlm-monitor --metrics cpu,memory,steps --thresholds thresholds.json --alerts admin@company.com
  nlm-monitor --dashboard-url http://localhost:8080 --health-check-interval 30 production_123
```

### Research and Publication Commands

#### `nlm-publish`

Prepare NLM research results for publication.

```bash
nlm-publish [options] RESULTS_DIR

Options:
  --journal STR           Target journal (Neuronal/ComputationalNeuroscience/SCI)
  --format STR            Publication format (full-paper/letter/methods)
  --word-count-limit INT   Word count limit
  --figures FIGURE_COUNT   Number of figures to include
  --tables TABLE_COUNT     Number of tables to include
  --supplementary FILE     Path to supplementary materials
  --generate-abstract      Generate abstract automatically
  --create-references      Create reference section
  --format-citation STR    Citation format (APA/MLA/Chicago/NLM)
  --output-file FILE      Path to publication-ready file
  --generate-slides       Generate presentation slides

Examples:
  nlm-publish --journal ComputationalNeuroscience --format full-paper results/2024/paper.md
  nlm-publish --word-count-limit 8000 --figures 5 --tables 3 research_results/
```

#### `nlm-create-dataset`

Create publication-ready dataset from NLM simulation results.

```bash
nlm-create-dataset [options] SOURCE_DIR DATASET_NAME

Options:
  --include-brain-state    Include brain state data
  --include-metrics        Include performance metrics
  --include-visualizations  Include visualization data
  --include-debug-data     Include debugging data
  --dataset-format STR     Dataset format (neurolab/neuraldata/neuroml)
  --schema-file FILE      Path to dataset schema file
  --documentation FILE    Path to documentation file
  --license STR           Dataset license (CC-BY/CC0/MIT)
  --version STR           Dataset version
  --doi STR              DOI to assign
  --zenodo-upload        Upload to Zenodo
  --include-code         Include analysis code

Examples:
  nlm-create-dataset --include-brain-state --include-metrics --license CC-BY source_data dataset_v1
  nlm-create-dataset --dataset-format neurolab --zenodo-upload experimental_results/dataset_v2
```

## Common Workflows

### 1. Advanced Configuration and Optimization

```bash
# Create advanced configuration
nlm-config-create --domain neuroscience --parameter-sweep linear cognitive_research.json

# Optimize for specific performance
nlm-config-optimize-for --target-metric reward --optimization-algorithm bayesian high_performance_config

# Validate and prepare for experiment
nlm-config-validate --strict --schema schema.json optimized_config.json
```

### 2. Performance Monitoring and Analysis

```bash
# Start performance monitoring
nlm-perf-monitor --output perf_data.json --format json config.json

# Analyze performance with custom metrics
nlm-perf-analyze --report-type detailed --metrics step_time,memory,spike_rate perf_data.json

# Generate comprehensive visualization
nlm-visualize-data --visualization-type network --style publication data.json results/
```

### 3. Research Pipeline Execution

```bash
# Create research pipeline
nlm-pipeline-manage --action start --generate-progress-report pipeline_123

# Run batch optimization
nlm-batch-run --parameter-space linear param_sweep.json

# Analyze results
nlm-analyze-data --analysis-type comprehensive --generate-insights simulation_data.json
```

### 4. Custom Behavior Development

```bash
# Create custom behavior
nlm-behavior-create --action-selection boltzmann --reward-calculation advanced exploration_agent

# Test and optimize behavior
nlm-behavior-test --iterations 100 --performance-metrics reward,steps my_behavior.json test_env.json

# Export optimized behavior
nlm-behavior-create --export-behavior optimized_agent
```

### 5. Production Deployment

```bash
# Deploy to production
nlm-deploy --environment production --resource-limits "memory=4GB,cpu=2,time=24h" config.json

# Monitor deployment
nlm-monitor --metrics cpu,memory,steps --thresholds thresholds.json --alerts admin@company.com

# Generate performance report
nlm-perf-analyze --report-type benchmark --generate-plots production_data.json
```

### 6. Research Publication

```bash
# Prepare publication data
nlm-publish --journal ComputationalNeuroscience --format full-paper results/2024/paper.md

# Create publication dataset
nlm-create-dataset --include-brain-state --include-metrics --license CC-BY source_data dataset_v1

# Generate analysis reports
nlm-analyze-data --analysis-type comprehensive --generate-insights simulation_data.json
```

## Configuration Files

### Configuration Schema

Advanced configurations use the following JSON schema:

```json
{
  "version": "Phase6.1",
  "brain": {
    "neuron_count": 8000,
    "region_count": 6,
    "connection_probability": 0.15,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "tau_mem": 20.0,
    "tau_ref": 5.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.002,
      "time_constant": 20.0,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "strength": 0.1
    },
    "structural": {
      "enable": true,
      "growth_rate": 0.001,
      "pruning_rate": 0.0001
    }
  },
  "development": {
    "enabled": true,
    "plasticity_decay_rate": 0.9,
    "critical_period_duration": 300.0,
    "maturation_threshold": 600.0
  },
  "neuromodulation": {
    "reward": {
      "enable": true,
      "scale": 1.0,
      "decay": 0.95
    },
    "curiosity": {
      "enable": true,
      "threshold": 0.3,
      "max_level": 1.0,
      "exploration_bonus": 0.5
    },
    "novelty": {
      "enable": true,
      "sensitivity": 1.0,
      "decay": 0.9,
      "threshold": 0.25
    }
  },
  "memory": {
    "working_memory": {
      "capacity": 200,
      "decay_rate": 0.95
    },
    "episodic_memory": {
      "capacity": 1000,
      "decay_rate": 0.98
    },
    "concept_formation": {
      "min_instances": 20,
      "similarity_threshold": 0.9
    }
  },
  "prediction": {
    "horizon": 30,
    "accuracy_threshold": 0.8
  },
  "text_processing": {
    "max_tokens": 500,
    "embedding_dimension": 64
  },
  "social_learning": {
    "observation_capacity": 500,
    "forgetting_rate": 0.02,
    "observation_window": 50,
    "influence_decay": 0.85
  },
  "environment": {
    "width": 100,
    "height": 100,
    "vision_width": 16,
    "vision_height": 16,
    "vision_channels": 3
  },
  "simulation": {
    "max_steps": 5000,
    "timestep": 0.1
  },
  "logging": {
    "level": "INFO",
    "to_file": true,
    "filename": "nlm.log",
    "profiling_enabled": true
  }
}
```

### Parameter Sweep Configuration

Advanced parameter sweep configurations:

```json
{
  "name": "brain_size_optimization",
  "parameters": [
    {
      "name": "neuron_count",
      "type": "int",
      "range": [1000, 5000, 500],
      "distribution": "linear"
    },
    {
      "name": "region_count",
      "type": "int",
      "range": [2, 8, 1],
      "distribution": "random"
    },
    {
      "name": "connection_probability",
      "type": "float",
      "range": [0.05, 0.2, 0.05],
      "distribution": "logarithmic"
    }
  ],
  "scenarios": [
    "neuroscience_research",
    "ai_research",
    "cognitive_modeling"
  ],
  "constraints": {
    "max_memory": "4GB",
    "max_time": "2h",
    "min_performance": 0.8
  }
}
```

## Python API Extensions

### Advanced Configuration Class

```python
import pynlm
from pynlm import AdvancedConfig

# Create advanced configuration
config = AdvancedConfig()

# Configure for Phase 6 features
config.configure_for_phase6(enable_text=True, enable_social=True, enable_memory=True)

# Set up for specific scale
config.configure_for_scale(neuron_count=5000, regions=6, mode="balanced")

# Validate configuration
config.validateConfig({
    "brain.neuron_count": lambda x: x >= 100 and x <= 100000,
    "simulation.max_steps": lambda x: x >= 100 and x <= 100000
})

# Merge with another configuration
config2 = AdvancedConfig()
config.mergeConfig(config2)

# Export configuration
config.exportConfig("advanced_config.json")
```

### Performance Monitoring Class

```python
from pynlm import PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor()

# Start monitoring
monitor.startMonitoring()

# Run simulation with monitoring
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    monitor.recordStepTime(performance_data.get_step_time(step))
    monitor.recordMemoryUsage(performance_data.get_memory_usage(step))

# Stop monitoring and get statistics
monitor.stopMonitoring()

stats = monitor.getStatistics()
print(f"Average step time: {monitor.getAverageStepTime():.1f}ms")
print(f"Max memory usage: {monitor.getMaxMemoryUsage() / 1024 / 1024:.1f} MB")
print(f"Steps per second: {monitor.getStepsPerSecond():.1f}")
```

### Batch Processing Class

```python
from pynlm import BatchProcessor

# Create batch processor
processor = BatchProcessor()

# Add configurations
processor.addConfig("config1", config1)
processor.addConfig("config2", config2)
processor.addConfig("config3", config3)

# Run batch with progress callback
def progress_callback(name, completed, total):
    print(f"Processing {name}: {completed}/{total} ({100*completed/total:.1f}%)")

processor.runBatchWithProgress(progress_callback)

# Get results
results = processor.getResults()
for name, result in results.items():
    print(f"{name}: success={result.first}, time={result.second}")
```

### Debug Tools Class

```python
from pynlm import DebugTools

# Create debug tools
debug = DebugTools()

# Enable debugging
debug.enableMemoryDebugging(True)
debug.enableStepDebugging(True)
debug.enableSpikeDebugging(True)

# Start profiling
debug.startProfiling("experiment")

# Run simulation with debugging
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

# Stop profiling
debug.stopProfiling()

# Analyze results
analysis = debug.analyzeMemoryUsage()
print(f"Memory usage analysis: {analysis}")

# Generate report
report = debug.generateDebugReport()
print(report)
```

### Custom Agent Behavior Class

```python
from pynlm import CustomAgentBehavior

# Create custom behavior
behavior = CustomAgentBehavior()

# Set custom action selection strategy
def custom_strategy(brain, percept):
    # Custom logic here
    return brain.produceAction().getType()

behavior.setActionSelectionStrategy(custom_strategy)

# Configure learning parameters
behavior.setLearningRate(0.1)
behavior.setExplorationRate(0.3)
behavior.setExploitationRate(0.7)

# Enable custom features
behavior.enableCustomLearning(True)
behavior.enableCustomDevelopment(True)
behavior.enableCustomSensory(True)
behavior.enableCustomMotor(True)

# Test behavior with environment
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Use custom behavior to select action
    action = behavior.selectAction(agent.decodeMotorCommand())
    world.applyMotorCommand(action, world.getSimulationTime())

# Get behavior statistics
stats = behavior.getBehaviorStats()
print(f"Behavior statistics: {stats}")
```

### Analysis Tools Class

```python
from pynlm import AnalysisTools

# Create analysis tools
analyzer = AnalysisTools()

# Run comprehensive analysis
spike_analysis = analyzer.analyzeSpikeDynamics(brain, 100)
plasticity_analysis = analyzer.analyzePlasticityChanges(brain)
memory_analysis = analyzer.analyzeMemoryUsage(brain)
learning_analysis = analyzer.analyzeLearningProgress(brain, agent)
behavior_analysis = analyzer.analyzeBehaviorPatterns(brain, agent)

# Generate reports
stats_report = analyzer.generateStatisticalReport(brain, agent)
visual_report = analyzer.generateVisualReport(brain, agent, "pdf")
network_graph = analyzer.createNetworkGraph(brain, "spring")

# Create visualizations
heatmap = analyzer.createActivityHeatmap(brain, 100)
time_series = analyzer.createTimeSeriesPlot(spike_analysis["spike_patterns"])
scatter = analyzer.createScatterPlot(memory_analysis["memory_usage"], learning_analysis["learning_progress"])

# Export analysis results
analyzer.exportAnalysisData("analysis_results.json")

# Get analysis statistics
analysis_stats = analyzer.getAnalysisStats()
print(f"Analysis completed: {analysis_stats}")
```

## Troubleshooting

### Common Issues and Solutions

1. **Configuration validation errors**
   ```bash
   # Check configuration schema
   nlm-config-validate --verbose --schema schema.json config.json
   ```

2. **Performance bottlenecks**
   ```bash
   # Analyze performance with detailed profiling
   nlm-perf-analyze --analysis-type detailed --generate-plots perf_data.json
   ```

3. **Batch processing failures**
   ```bash
   # Run batch with error handling
   nlm-batch-run --continue-on-error --retry-count 3 batch_config.json
   ```

4. **Debug issues**
   ```bash
   # Enable comprehensive debugging
   nlm-debug-run --debug-level 5 --enable-memory-debug --enable-step-debug config.json
   ```

5. **Memory issues**
   ```bash
   # Monitor memory usage
   nlm-perf-monitor --output mem_monitor.json --sampling-interval 1000 config.json
   ```

### Error Messages

| Error Code | Description | Solution |
|------------|-------------|----------|
| CONFIG_001 | Configuration validation failed | Check schema and parameter constraints |
| PERF_001 | Performance monitoring failed | Check system resources and permissions |
| BATCH_001 | Batch processing failed | Check configuration files and parameter ranges |
| DEBUG_001 | Debugging failed | Enable appropriate debug flags |
| ANALYZE_001 | Analysis failed | Check input data format |
| EXPORT_001 | Export failed | Check output directory permissions |

### Support

For advanced support, please:
1. Check the documentation files in `docs/`
2. Review the examples in `examples/`
3. Use the `nlm-support` command for automated diagnostics
4. Contact the NLM development team for custom solutions

## Conclusion

The NLM framework provides comprehensive advanced capabilities for researchers and developers working with neural network simulations. The advanced commands and APIs enable:

• **Efficient configuration management** with validation and optimization
• **Comprehensive performance monitoring** and profiling
• **Scalable batch processing** for large-scale experiments
• **Advanced debugging and analysis** tools
• **Custom agent behaviors** and strategies
• **Export/import utilities** for data management
• **Research pipeline support** for complete workflows
• **Production deployment** and monitoring
• **Publication-ready outputs** and dataset creation

These advanced features make NLM suitable for complex research scenarios, large-scale experiments, and production deployments while maintaining ease of use for both beginners and experts.

For further exploration, consider:
- Reading the advanced documentation in `HOW_TO_USE.md`
- Exploring example configurations in `configs/`
- Reviewing the Python API extensions in this document
- Building custom commands for your specific use case
- Joining the NLM research community for collaboration

Happy experimenting with NLM's advanced capabilities!
```