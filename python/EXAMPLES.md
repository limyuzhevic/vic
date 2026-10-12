# NLM Examples and Tutorials

## Overview

This guide provides comprehensive examples and tutorials for using the enhanced NLM Python bindings. It covers basic usage, research workflows, visualization, analysis, and advanced integration patterns.

## What This Guide Covers

1. **Basic Examples**: Simple demonstrations of NLM functionality
2. **Research Workflows**: Complete research workflows with examples
3. **Visualization Examples**: Static and interactive visualizations
4. **Analysis Examples**: Statistical analysis and data processing
5. **Integration Examples**: Real-time monitoring and session management
6. **Advanced Examples**: Complex scenarios and best practices

## Installation

```bash
# Install with all examples dependencies
pip install pynlm[examples,visualization,advanced_ml]

# Or install just the core package
pip install pynlm
```

## Basic Examples

### Example 1: Basic Brain Simulation

```python
import pynlm
import time

# Create brain manager
brain = pynlm.BrainManager()

# Configure brain
brain.set_learning_rate(0.01)
brain.set_neuron_count(1000)

# Run experiment with timing
start_time = time.time()
results = brain.run_experiment(steps=1000)
end_time = time.time()

print(f"Experiment completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes: {results.metrics.get('total_spike_count', 'N/A')}")
print(f"Active neurons: {results.metrics.get('active_neuron_count', 'N/A')}")

# Visualize results
visualizer = pynlm.create_matplotlib_visualizer(brain)
visualizer.plot_neural_activity()
visualizer.show_all_plots()
```

### Example 2: Configuration Management

```python
import pynlm

# Create configuration
config = pynlm.PythonConfig(
    learning_rate=0.01,
    neuron_count=1000,
    brain_type='standard'
)

# Use Pythonic property access
print(f"Learning rate: {config.learning_rate}")
config.learning_rate = 0.05
print(f"Updated learning rate: {config.learning_rate}")

# Load from JSON
config.load_json("config.json")
config.save_json("new_config.json")

# Use factory methods
learning_config = pynlm.PythonConfig.createLearningExperiment(learning_rate=0.01, steps=1000)
integration_config = pynlm.PythonConfig.createIntegrationTest(neuron_count=500)
```

### Example 3: Research Session

```python
import pynlm
from pynlm import ResearchSession

# Create and use research session
with ResearchSession() as session:
    for step in range(500):
        session.run_step(step)
        
        # Collect metrics every 50 steps
        if step % 50 == 0:
            metrics = session.collect_metrics()
            print(f"Step {step}: active_neurons={metrics.get('active_neuron_count', 0)}, firing_rate={metrics.get('average_firing_rate', 0):.3f}")

# Get results
results = session.get_results()
print(f"Final step count: {results['step_count']}")
print(f"Session active: {results['session_active']}")
```

## Research Workflow Examples

### Example 1: Learning Experiment Workflow

```python
import pynlm
from pynlm import LearningExperimentAPI

# Create learning experiment API
learning_api = LearningExperimentAPI()

# Run different types of learning experiments
basic_learning = learning_api.run_basic_learning(
    steps=1000,
    learning_rate=0.01,
    neuron_count=1000
)

plasticity_learning = learning_api.run_plasticity_learning(
    steps=2000,
    learning_rate=0.05,
    enable_stdp=True,
    enable_hebbian=True
)

development_learning = learning_api.run_development_learning(
    steps=5000,
    learning_rate=0.001,
    enable_development=True
)

print("Learning experiments completed successfully!")
```

### Example 2: Integration Test Workflow

```python
import pynlm
from pynlm import IntegrationTestAPI

# Create integration test API
integration_api = IntegrationTestAPI()

# Run different types of integration tests
complete_integration = integration_api.run_complete_integration(
    steps=10000,
    enable_checkpointing=True,
    enable_replay=True,
    enable_development=True
)

memory_integration = integration_api.run_memory_integration(steps=3000)

neuromodulation_integration = integration_api.run_neuromodulation_integration(steps=2000)

print("Integration tests completed successfully!")
```

### Example 3: Parameter Tuning Workflow

```python
import pynlm
from pynlm import ParameterTuningAPI

# Create parameter tuning API
param_tuning_api = ParameterTuningAPI()

# Tune learning rates
learning_rate_tuning = param_tuning_api.tune_learning_rate(
    learning_rates=[0.001, 0.005, 0.01, 0.05, 0.1],
    n_trials=20
)

print(f"Best learning rate: {learning_rate_tuning['best_learning_rate']}")
print(f"Best score: {learning_rate_tuning['best_score']}")

# Tune network sizes
network_size_tuning = param_tuning_api.tune_network_size(
    neuron_counts=[500, 1000, 2000, 5000],
    n_trials=20
)

print(f"Best neuron count: {network_size_tuning['best_neuron_count']}")
print(f"Best score: {network_size_tuning['best_score']}")
```

### Example 4: Ablation Study Workflow

```python
import pynlm
from pynlm import AblationStudyAPI

# Create ablation study API
ablation_api = AblationStudyAPI()

# Run different ablation studies
memory_ablation = ablation_api.disable_memory_systems(steps=500)
neuromodulation_ablation = ablation_api.disable_neuromodulation(steps=500)
plasticity_ablation = ablation_api.disable_plasticity(steps=500)
cognition_ablation = ablation_api.disable_cognition(steps=500)

print("Ablation studies completed successfully!")
```

## Visualization Examples

### Example 1: Static Visualization

```python
import pynlm

# Create visualizer
visualizer = pynlm.create_matplotlib_visualizer()

# Generate and save visualizations
visualizer.plot_neural_activity(step_range=(0, 100))
visualizer.save_plot('neural_activity', 'neural_activity.png')

visualizer.plot_memory_metrics()
visualizer.save_plot('memory_metrics', 'memory_metrics.png')

visualizer.plot_learning_progress()
visualizer.save_plot('learning_progress', 'learning_progress.png')

print("Static visualizations saved successfully")
```

### Example 2: Interactive Visualization

```python
import pynlm

# Create interactive visualizer
visualizer = pynlm.create_plotly_visualizer()

# Create dashboard
dashboard = visualizer.create_interactive_dashboard()

# Create exploration plots
plots = visualizer.create_exploration_plots()

# Display plots
visualizer.show_interactive()

# Save dashboard for sharing
dashboard.save_html("brain_simulation_dashboard.html")

print("Interactive dashboard created and saved")
```

### Example 3: Real-Time Visualization

```python
import pynlm
from pynlm import ResearchSession
import time

# Create visualizer
visualizer = pynlm.create_plotly_visualizer()

# Create research session
with ResearchSession() as session:
    for step in range(100):
        session.run_step(step)
        
        # Update real-time plot every 20 steps
        if step % 20 == 0:
            visualizer.plot_real_time_metrics()
            visualizer.show_interactive()
        
        # Small delay to control simulation speed
        time.sleep(0.1)

print("Real-time visualization completed")
```

## Analysis Examples

### Example 1: Statistical Analysis

```python
import pynlm
import pandas as pd
import numpy as np

# Create sample data
data = pd.DataFrame({
    'experiment': ['A', 'B', 'C', 'A', 'B', 'C'] * 10,
    'performance': np.random.normal(0.8, 0.1, 60),
    'memory_usage': np.random.uniform(0.5, 0.9, 60),
    'learning_rate': np.random.uniform(0.001, 0.1, 60)
})

# Create statistical analysis
analysis = pynlm.create_statistical_analysis(data)

# Describe data
print("Data Description:")
describe_results = analysis.describe()
for column, stats in describe_results.items():
    print(f"  {column}: mean={stats.mean:.3f}, std={stats.std:.3f}")

# Perform correlation analysis
corr_results = analysis.correlation_analysis(data['learning_rate'], data['performance'])
print(f"\nLearning Rate vs Performance correlation: {corr_results.correlation:.3f} (p={corr_results.p_value:.3f})")

# Generate report
report = analysis.generate_report()
print("\nStatistical Report:")
print(report)
```

### Example 2: Data Processing

```python
import pynlm
import pandas as pd
import numpy as np

# Create sample data
data = pd.DataFrame({
    'feature1': np.random.randn(100),
    'feature2': np.random.randn(100),
    'target': np.random.randn(100),
    'category': np.random.choice(['A', 'B', 'C'], 100)
})

# Add missing values
data.loc[10:20, 'feature1'] = np.nan
data.loc[30:40, 'feature2'] = np.nan

# Create data processor
data_processor = pynlm.create_research_data_processor(data)

# Clean data
data_processor.clean_data(dropna=False, fillna=True, strategy='mean')
print("Data cleaned and normalized")

# Normalize data
data_processor.normalize()
print("Data normalized")

# Add features
data_processor.feature_engineering({
    'squared_feature1': lambda df: df['feature1'] ** 2,
    'interaction_feature1_2': lambda df: df['feature1'] * df['feature2']
})
print("Features engineered")

# Split data
X_train, X_test, y_train, y_test = data_processor.split_data('target')
print(f"Data split: {X_train.shape[0]} train, {X_test.shape[0]} test")

# Get correlation matrix
corr_matrix = data_processor.get_correlation_matrix()
print("Correlation matrix shape:", corr_matrix.shape)
```

## Integration Examples

### Example 1: Research Session Manager

```python
import pynlm
from pynlm import ResearchSession

# Create brain manager
brain_manager = pynlm.BrainManager()
brain_manager.set_learning_rate(0.01)
brain_manager.set_neuron_count(1000)

# Create multiple research sessions
sessions = []
for i in range(3):
    session = ResearchSession(brain_manager)
    session.__enter__()  # Start session
    sessions.append(session)
    print(f"Session {i + 1} started")

# Run all sessions
for i, session in enumerate(sessions):
    for step in range(50):
        session.run_step(step)
    
    # End session
    session.__exit__(None, None, None)
    print(f"Session {i + 1} completed")

print("All research sessions completed")
```

### Example 2: Experiment Manager

```python
import pynlm
from pynlm import ExperimentManager

# Create experiment manager
exp_manager = ExperimentManager()

# Run multiple experiments
for i in range(3):
    result = exp_manager.conduct_learning_experiment(
        experiment_name=f"exp_{i + 1}",
        steps=200,
        learning_rate=0.01 * (i + 1),
        neuron_count=1000 + i * 500
    )
    
    print(f"Experiment {i + 1} completed: {result.experiment_name}")
    print(f"  Metrics: {result.metrics}")
```

## Advanced Examples

### Example 1: Complete Research Workflow

```python
import pynlm
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# Create comprehensive research workflow
print("Starting comprehensive research workflow...")

# Step 1: Create brain manager
brain_manager = pynlm.BrainManager()
brain_manager.set_learning_rate(0.01)
brain_manager.set_neuron_count(1000)

# Step 2: Run learning experiments
learning_api = pynlm.LearningExperimentAPI()
learning_api.set_brain_manager(brain_manager)

experiments = []
for i in range(3):
    experiment = learning_api.run_basic_learning(
        steps=500 + i * 200,
        learning_rate=0.01 * (i + 1),
        neuron_count=1000 + i * 500
    )
    experiments.append(experiment)

# Step 3: Run parameter tuning
param_tuning_api = pynlm.ParameterTuningAPI()
param_tuning_api.set_brain_manager(brain_manager)

param_space = {
    'learning_rate': [0.001, 0.005, 0.01, 0.05],
    'neuron_count': [500, 1000, 2000, 5000]
}

best_params = param_tuning_api.parameter_tuning(
    param_space=param_space,
    n_trials=10,
    metric='performance'
)

# Step 4: Create visualizer and generate plots
visualizer = pynlm.create_plotly_visualizer(brain_manager)
dashboard = visualizer.create_interactive_dashboard()
exploration_plots = visualizer.create_exploration_plots()

# Step 5: Perform statistical analysis
np.random.seed(42)
sample_data = pd.DataFrame({
    'experiment': np.random.choice(['A', 'B', 'C'], 100),
    'performance': np.random.normal(0.8, 0.1, 100),
    'memory_usage': np.random.uniform(0.5, 1.0, 100),
    'learning_rate': np.random.uniform(0.001, 0.1, 100)
})

analysis = pynlm.create_statistical_analysis(sample_data)

# Step 6: Generate report and visualizations
report = analysis.generate_report()
visualizer.show_interactive()

print("Comprehensive research workflow completed successfully!")
```

### Example 2: Real-Time Monitoring

```python
import pynlm
from pynlm import ResearchSession
import pandas as pd
import numpy as np
import time
import threading
from queue import Queue, Empty

# Create data collection queue
data_queue = Queue()

# Create data collector function
def data_collector(brain_manager, queue, stop_event, interval=0.5):
    step = 0
    while not stop_event.is_set():
        metrics = brain_manager.get_metrics()
        metrics['step'] = step
        metrics['timestamp'] = time.time()
        queue.put(metrics)
        step += 1
        time.sleep(interval)

# Create real-time monitor function
def real_time_monitor(brain_manager, queue, stop_event, plot_interval=2.0):
    last_update = time.time()
    
    while not stop_event.is_set():
        current_time = time.time()
        
        if current_time - last_update >= plot_interval:
            try:
                # Create real-time dashboard
                fig = go.Figure()
                
                # Collect data
                data_points = []
                while True:
                    try:
                        data_points.append(queue.get_nowait())
                    except Empty:
                        break
                
                if data_points:
                    df = pd.DataFrame(data_points)
                    
                    # Add plots
                    fig.add_trace(go.Scatter(
                        x=df['step'],
                        y=df['total_spike_count'],
                        mode='lines',
                        name='Spike Count',
                        line=dict(color='blue')
                    ))
                    
                    fig.update_layout(
                        title='Real-Time Brain Simulation Monitor',
                        xaxis_title='Step',
                        yaxis_title='Spike Count',
                        height=500
                    )
                    
                    fig.show()
                
                last_update = current_time
                
            except Exception as e:
                print(f"Error in real-time monitor: {e}")
                time.sleep(0.5)

# Start real-time monitoring
brain_manager = pynlm.BrainManager()
stop_event = threading.Event()

# Start data collector thread
collector_thread = threading.Thread(
    target=data_collector,
    args=(brain_manager, data_queue, stop_event, 0.5),
    daemon=True
)
collector_thread.start()

# Start real-time monitor thread
monitor_thread = threading.Thread(
    target=real_time_monitor,
    args=(brain_manager, data_queue, stop_event, 2.0),
    daemon=True
)
monitor_thread.start()

# Run simulation
print("Running simulation with real-time monitoring...")
for step in range(100):
    brain_manager.runExperiment(steps=1, record_metrics=False)
    time.sleep(0.5)

# Stop monitoring
stop_event.set()
collector_thread.join(timeout=5)
monitor_thread.join(timeout=5)

print("Real-time monitoring completed")
```

## File Structure

The enhanced NLM Python bindings include the following file structure:

```
pynlm/
├── research_api.py          # High-level research API
├── viz_api.py              # Visualization API
├── analysis_api.py         # Analysis API
├── examples/               # Example scripts
│   ├── basic_example.py
│   ├── research_workflow_example.py
│   ├── viz_analysis_example.py
│   └── integration_realtime_example.py
├── docs/                   # Documentation
│   ├── RESEARCH_API_GUIDE.md
│   ├── VISUALIZATION_API.md
│   └── ANALYSIS_API.md
└── setup.py               # Installation script
```

## Running Examples

To run the examples, execute the script directly:

```bash
# Run basic example
python examples/basic_example.py

# Run research workflow example
python examples/research_workflow_example.py

# Run visualization and analysis example
python examples/viz_analysis_example.py

# Run integration and real-time monitoring example
python examples/integration_realtime_example.py
```

## Common Issues and Solutions

### Issue: Import Error

**Solution:** Ensure all dependencies are installed:

```bash
pip install pynlm[examples,visualization,advanced_ml]
```

### Issue: Plot Display Issues

**Solution:** For headless environments, use the Agg backend:

```python
import matplotlib
matplotlib.use('Agg')
```

### Issue: Memory Usage

**Solution:** Monitor memory usage and use appropriate data sampling:

```python
# Sample data for large datasets
sample_data = data.sample(n=1000, random_state=42)
```

### Issue: Real-Time Performance

**Solution:** Optimize real-time visualization:

```python
# Use efficient plot update methods
fig = go.Figure()
# ... add traces
fig.show()
```

## Performance Tips

1. **For Large Datasets:** Use vectorized operations and data sampling
2. **For Real-Time Applications:** Use efficient plot update methods and background threads
3. **For Complex Analysis:** Use appropriate statistical tests and efficient algorithms
4. **For Visualization:** Cache generated plots and use hardware acceleration

## Next Steps

1. **Explore the Documentation:** Read the detailed API guides for each module
2. **Try Examples:** Run the example scripts to understand the APIs
3. **Customize Workflows:** Adapt examples to your specific research needs
4. **Contribute:** Share your examples and workflows with the community

## Acknowledgments

The enhanced NLM Python bindings are built on the contributions of the following communities:

- **NumPy**: Numerical computing library
- **Pandas**: Data manipulation library
- **Matplotlib**: Plotting library
- **Plotly**: Interactive visualization library
- **scikit-learn**: Machine learning library
- **SciPy**: Scientific computing library
- **statsmodels**: Statistical modeling library

We thank them for providing these powerful tools for scientific research.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Version Information

- **Version:** 0.5.0
- **Release Date:** October 12, 2026
- **Python Support:** >= 3.8
- **Documentation:** Available in README.md and doc/ directories

## Getting Help

### Online Resources

- **Documentation:** https://nlm.readthedocs.io
- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Issues:** https://github.com/nlm-project/nlm/issues
- **Discussions:** https://github.com/nlm-project/nlm/discussions

### Community

Join the NLM community to:

- Get help with examples
- Share your research workflows
- Contribute to the project
- Stay updated with new features

### Support

For support with the enhanced NLM Python bindings:

1. **Check Documentation:** Review the examples and API guides
2. **Report Issues:** Create GitHub issues with detailed error messages
3. **Ask Questions:** Post questions on GitHub Discussions
4. **Provide Feedback:** Share your experiences and suggestions

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface for conducting research with the Neural Learning Machine framework. The examples in this guide demonstrate the full range of capabilities, from basic simulations to complex research workflows.

Whether you're a researcher, scientist, or developer, the enhanced bindings provide the tools you need to:

- **Conduct Experiments:** Run learning experiments and integration tests
- **Analyze Results:** Perform statistical analysis and data processing
- **Visualize Data:** Create static and interactive visualizations
- **Monitor in Real-Time:** Track simulations as they run
- **Export Data:** Save results in Python-friendly formats

Start exploring the enhanced NLM Python bindings today and transform your research workflows!

---

*For more information, visit the NLM documentation at https://nlm.readthedocs.io*

---