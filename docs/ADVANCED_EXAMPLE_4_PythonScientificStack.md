# Advanced Example 4: Integration with Python Scientific Stack

## Overview

This advanced example demonstrates seamless integration between NLM and the Python scientific ecosystem. It showcases how NLM can be combined with NumPy, SciPy, Pandas, Matplotlib, and other scientific Python libraries for comprehensive neural analysis and research.

## Learning Objectives

After completing this example, you will be able to:

1. **Integrate NLM with NumPy** for efficient numerical computations
2. **Combine NLM with SciPy** for advanced algorithms and optimization
3. **Use Pandas** for data analysis and result management
4. **Leverage Matplotlib/Plotly** for advanced visualization
5. **Integrate with Jupyter notebooks** for interactive research
6. **Use Scikit-learn** for machine learning analysis
7. **Apply scientific computing patterns** in neural research

## What This Example Demonstrates

- **Numerical integration**: Efficient array operations with NumPy
- **Optimization algorithms**: Using SciPy for neural parameter optimization
- **Data analysis pipelines**: Processing neural data with Pandas
- **Advanced visualization**: Publication-quality plots with Matplotlib
- **Machine learning integration**: Applying ML algorithms to neural data
- **Research workflows**: Complete end-to-end scientific computing pipeline

## System Requirements

For Python scientific integration, you'll need:

- **Python scientific stack**: NumPy, SciPy, Pandas, Matplotlib, Jupyter, etc.
- **NLM Python bindings**: pynlm with scientific computing support
- **Research environment**: Jupyter notebooks or Python IDE
- **Data storage**: For large neural datasets
- **Visualization tools**: For publication-quality plots

## Technical Architecture

### 1. NumPy Integration for Neural Data Processing

```cpp
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

class NumpyEnhancedNeuralProcessor {
private:
    pynlm::Brain* brain;
    py::array_t<double> neural_data;
    py::array_t<double> spike_times;
    py::array_t<double> firing_rates;
    
public:
    NumpyEnhancedNeuralProcessor(pynlm::Brain* b) : brain(b) {
        initialize_numpy_arrays();
    }
    
    void initialize_numpy_arrays() {
        int total_neurons = brain->getTotalNeuronCount();
        
        // Allocate NumPy arrays for neural data
        neural_data = py::array_t<double>({total_neurons, 3});  // x, y, z positions
        spike_times = py::array_t<double>({total_neurons, 100}); // Last 100 spike times
        firing_rates = py::array_t<double>({total_neurons});     // Current firing rates
        
        // Initialize arrays
        std::fill_n(neural_data.mutable_data(), total_neurons * 3, 0.0);
        std::fill_n(spike_times.mutable_data(), total_neurons * 100, -1.0);
        std::fill_n(firing_rates.mutable_data(), total_neurons, 0.0);
    }
    
    py::array_t<double> extract_neural_data() {
        // Extract neural data into NumPy arrays
        int total_neurons = brain->getTotalNeuronCount();
        
        for (int i = 0; i < total_neurons; ++i) {
            // Extract neuron data
            neural_data.mutable_at(i, 0) = brain->getNeuronPositionX(i);
            neural_data.mutable_at(i, 1) = brain->getNeuronPositionY(i);
            neural_data.mutable_at(i, 2) = brain->getNeuronPositionZ(i);
            
            firing_rates.mutable_at(i) = brain->getNeuronFiringRate(i);
            
            // Extract spike times
            for (int j = 0; j < 100; ++j) {
                spike_times.mutable_at(i, j) = brain->getSpikeTime(i, j);
            }
        }
        
        return neural_data;
    }
    
    py::array_t<double> compute_neural_metrics() {
        // Compute comprehensive neural metrics using NumPy
        int total_neurons = brain->getTotalNeuronCount();
        
        // Extract firing rates as NumPy array
        py::array_t<double> firing_rates_array = extract_neural_data();
        
        // Compute metrics using NumPy operations
        py::array_t<double> metrics = py::array_t<double>({6});
        
        // Mean firing rate across all neurons
        double sum_firing = std::accumulate(firing_rates_array.data(), 
                                           firing_rates_array.data() + total_neurons, 0.0);
        metrics.mutable_at(0) = sum_firing / total_neurons;
        
        // Firing rate standard deviation
        double variance_sum = 0.0;
        for (int i = 0; i < total_neurons; ++i) {
            double deviation = firing_rates_array.at(i) - metrics.at(0);
            variance_sum += deviation * deviation;
        }
        double std_dev = std::sqrt(variance_sum / total_neurons);
        metrics.mutable_at(1) = std_dev;
        
        // Firing rate skewness
        double skew_sum = 0.0;
        for (int i = 0; i < total_neurons; ++i) {
            double deviation = firing_rates_array.at(i) - metrics.at(0);
            skew_sum += deviation * deviation * deviation;
        }
        double skewness = (total_neurons / ((total_neurons - 1) * (total_neurons - 2))) * 
                        (skew_sum / (std_dev * std_dev * std_dev));
        metrics.mutable_at(2) = skewness;
        
        // High firing neurons (top 10%)
        int high_firing_count = std::count_if(firing_rates_array.data(), 
                                            firing_rates_array.data() + total_neurons,
                                            [](double rate) { return rate > 50.0; });
        metrics.mutable_at(3) = (double)high_firing_count / total_neurons * 100;
        
        // Low firing neurons (bottom 10%)
        int low_firing_count = std::count_if(firing_rates_array.data(), 
                                           firing_rates_array.data() + total_neurons,
                                           [](double rate) { return rate < 1.0; });
        metrics.mutable_at(4) = (double)low_firing_count / total_neurons * 100;
        
        // Total spike rate across population
        double total_spike_rate = std::accumulate(firing_rates_array.data(),
                                                firing_rates_array.data() + total_neurons, 0.0);
        metrics.mutable_at(5) = total_spike_rate;
        
        return metrics;
    }
};
```

### 2. SciPy Integration for Neural Optimization

```python
import pynlm
import numpy as np
import scipy.optimize as opt
import scipy.integrate as integrate
from scipy.stats import norm, kde

class SciPyNeuralOptimizer:
    """Optimize neural parameters using SciPy algorithms"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Optimization history
        self.optimization_history = []
        
        # Objective function cache
        self.objective_cache = {}
    
    def optimize_neural_parameters(self, initial_params, bounds):
        """Optimize neural parameters using SciPy optimization algorithms"""
        
        print(f"=== Neural Parameter Optimization ===")
        print(f"Initial parameters: {initial_params}")
        print(f"Parameter bounds: {bounds}")
        
        # Define objective function
        def objective_function(params):
            return self.compute_optimization_objective(params)
        
        # Run different optimization algorithms
        results = {}
        
        # 1. Gradient-based optimization (BFGS)
        print("   Running BFGS optimization...")
        bfgs_result = opt.minimize(
            objective_function,
            initial_params,
            bounds=bounds,
            method='L-BFGS-B',
            options={'maxiter': 100, 'disp': False}
        )
        results['BFGS'] = bfgs_result
        
        # 2. Global optimization (Differential Evolution)
        print("   Running Differential Evolution optimization...")
        de_result = opt.differential_evolution(
            objective_function,
            bounds,
            maxiter=50,
            popsize=10,
            seed=42
        )
        results['DifferentialEvolution'] = de_result
        
        # 3. Bayesian optimization (if scipy-optimize-gui available)
        try:
            print("   Running Bayesian Optimization...")
            from scipy.optimize import minimize
            bayes_result = opt.dual_annealing(
                objective_function,
                bounds,
                maxiter=30,
                seed=123
            )
            results['DualAnnealing'] = bayes_result
        except:
            print("   ! Bayesian optimization not available, skipping")
        
        # 4. Simulated annealing
        print("   Running Simulated Annealing...")
        sa_result = opt.dual_annealing(
            objective_function,
            bounds,
            maxiter=40,
            seed=456
        )
        results['SimulatedAnnealing'] = sa_result
        
        # Compare results
        best_result = self.select_best_result(results)
        
        return best_result, results
    
    def compute_optimization_objective(self, params):
        """Compute objective function for parameter optimization"""
        
        # Create a cache key
        param_tuple = tuple(params)
        if param_tuple in self.objective_cache:
            return self.objective_cache[param_tuple]
        
        # Parse parameters
        neuron_count = int(params[0])
        plasticity_strength = params[1]
        learning_rate = params[2]
        connection_density = params[3]
        
        # Create temporary brain with new parameters
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("plasticity.stdp.learning_rate", plasticity_strength)
        config.set("plasticity.hebbian.learning_rate", learning_rate)
        config.set("brain.synapse_density", connection_density)
        
        # Create temporary simulation
        temp_brain = pynlm.createBrain(config)
        temp_brain.initialize()
        
        # Run simulation and compute metrics
        metrics = self.evaluate_neural_performance(temp_brain, neuron_count)
        
        # Compute objective (lower is better)
        # Balance multiple objectives
        objective_value = (
            0.4 * (metrics['firing_rate_stability'] / 100) +  # Want stable firing
            0.3 * (1 - metrics['learning_effectiveness'] / 100) +  # Want good learning
            0.2 * (metrics['energy_efficiency'] / 100) +  # Want efficient energy use
            0.1 * (1 - metrics['population_balance'] / 100)  # Want balanced population
        )
        
        # Store in cache
        self.objective_cache[param_tuple] = objective_value
        
        return objective_value
    
    def evaluate_neural_performance(self, brain, neuron_count):
        """Evaluate neural performance with given parameters"""
        
        # Run simulation
        simulation_steps = min(500, neuron_count // 10)
        
        # Track performance metrics
        firing_rates = []
        energy_consumption = []
        learning_progress = []
        population_distribution = []
        
        for step in range(simulation_steps):
            brain.step(step)
            
            # Collect metrics
            firing_rates.append(brain.getAverageFiringRate())
            energy_consumption.append(brain.getTotalEnergyConsumption())
            
            # Simple learning metric (neuron changes)
            learning_progress.append(brain.getTotalSpikeCount() / simulation_steps)
            
            # Population distribution
            excitatory = brain.getNeuronCount(pynlm.NeuronType.Excitatory)
            inhibitory = brain.getNeuronCount(pynlm.NeuronType.Inhibitory)
            population_distribution.append(excitatory / (excitatory + inhibitory))
        
        # Convert to numpy arrays for analysis
        firing_rates = np.array(firing_rates)
        energy_consumption = np.array(energy_consumption)
        learning_progress = np.array(learning_progress)
        population_distribution = np.array(population_distribution)
        
        # Compute performance metrics
        metrics = {
            'firing_rate_stability': 100 * (1 - np.std(firing_rates) / np.mean(firing_rates)),
            'learning_effectiveness': np.mean(learning_progress) * 100,
            'energy_efficiency': 100 * (1 - np.std(energy_consumption) / np.mean(energy_consumption)),
            'population_balance': 100 * (1 - np.std(population_distribution)),
            'simulation_consistency': self.compute_consistency_metrics(firing_rates, learning_progress)
        }
        
        return metrics
    
    def compute_consistency_metrics(self, firing_rates, learning_progress):
        """Compute consistency and stability metrics"""
        
        # Autocorrelation of firing rates
        if len(firing_rates) > 10:
            autocorr = np.corrcoef(firing_rates[:-1], firing_rates[1:])
            consistency = autocorr[0, 1] if not np.isnan(autocorr[0, 1]) else 0
        else:
            consistency = 0
        
        # Learning consistency (variance of learning progress)
        learning_variance = np.var(learning_progress)
        consistency_metric = max(0, 100 - learning_variance * 1000)
        
        # Combined consistency score
        return (consistency * 0.7 + consistency_metric * 0.3)
    
    def select_best_result(self, results):
        """Select the best optimization result"""
        
        best_method = None
        best_value = float('inf')
        best_result = None
        
        for method, result in results.items():
            if result.fun < best_value:
                best_value = result.fun
                best_method = method
                best_result = result
        
        print(f"\n   Best optimization method: {best_method}")
        print(f"   Best objective value: {best_value:.4f}")
        print(f"   Optimal parameters: {best_result.x}")
        
        return best_result
```

### 3. Pandas Integration for Data Analysis

```python
import pandas as pd
import numpy as np
import pynlm

class PandasNeuralDataAnalysis:
    """Analyze neural data using Pandas for comprehensive data processing"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Data storage
        self.data_frames = {}
        self.time_series_data = {}
        self.analysis_results = {}
    
    def create_comprehensive_dataframe(self, simulation_steps=1000):
        """Create comprehensive pandas DataFrame from simulation"""
        
        print(f"=== Creating Comprehensive Data Analysis ===")
        
        # Initialize data structures
        data = {
            'time_step': [],
            'neuron_count': [],
            'total_spikes': [],
            'average_firing_rate': [],
            'firing_neuron_percentage': [],
            'excitatory_neurons': [],
            'inhibitory_neurons': [],
            'sensory_neurons': [],
            'motor_neurons': [],
            'modulatory_neurons': [],
            'internal_neurons': [],
            'energy_consumption': [],
            'agent_energy': [],
            'agent_curiosity': [],
            'agent_novelty': [],
            'agent_prediction_error': [],
            'action_type': [],
            'reward_received': [],
            'simulation_time': []
        }
        
        # Run simulation and collect data
        for step in range(simulation_steps):
            # Get current state
            current_time = time.time()
            
            # Brain metrics
            data['time_step'].append(step)
            data['neuron_count'].append(self.brain.getTotalNeuronCount())
            data['total_spikes'].append(self.brain.getTotalSpikeCount())
            data['average_firing_rate'].append(self.brain.getAverageFiringRate())
            data['firing_neuron_percentage'].append(
                (self.brain.getFiringNeuronCount() / self.brain.getTotalNeuronCount()) * 100
            )
            
            # Neuron type distribution
            for neuron_type in pynlm.NeuronType:
                count = self.brain.getNeuronCount(neuron_type)
                data[f'{neuron_type.name.lower()}_neurons'].append(count)
            
            # Get agent and world state
            body = self.world.getAgentBody()
            percept = self.world.getSensoryPercept()
            
            data['energy_consumption'].append(self.brain.getTotalEnergyConsumption())
            data['agent_energy'].append(body.getEnergy())
            data['agent_curiosity'].append(self.agent.getCuriosityLevel())
            data['agent_novelty'].append(self.agent.getNoveltyLevel())
            data['agent_prediction_error'].append(self.agent.getPredictionError())
            
            # Get action
            action = self.agent.decodeMotorCommand()
            data['action_type'].append(str(action))
            
            # Get reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            data['reward_received'].append(reward)
            
            # Get simulation time
            data['simulation_time'].append(self.world.getSimulationTime())
            
            # Update simulation
            self.world.update(0.1)
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
        
        # Create DataFrame
        df = pd.DataFrame(data)
        
        # Convert time_step to index
        df.set_index('time_step', inplace=True)
        
        # Store for analysis
        self.data_frames['simulation_data'] = df
        
        print(f"   ✓ Created DataFrame with {len(df):,} rows and {len(df.columns)} columns")
        print(f"   ✓ Time range: {df.index[0]} to {df.index[-1]} steps")
        
        return df
    
    def analyze_neural_patterns(self, df):
        """Analyze neural patterns using Pandas"""
        
        print(f"\n   Analyzing neural patterns...")
        
        # Initialize results dictionary
        analysis_results = {}
        
        # 1. Temporal analysis
        print("      Performing temporal analysis...")
        analysis_results['temporal'] = self.analyze_temporal_patterns(df)
        
        # 2. Spatial analysis
        print("      Performing spatial analysis...")
        analysis_results['spatial'] = self.analyze_neural_spatial_distribution(df)
        
        # 3. Behavioral analysis
        print("      Performing behavioral analysis...")
        analysis_results['behavioral'] = self.analyze_behavioral_patterns(df)
        
        # 4. Learning analysis
        print("      Performing learning analysis...")
        analysis_results['learning'] = self.analyze_learning_patterns(df)
        
        # 5. Correlation analysis
        print("      Performing correlation analysis...")
        analysis_results['correlations'] = self.analyze_correlations(df)
        
        # Store results
        self.analysis_results.update(analysis_results)
        
        return analysis_results
    
    def analyze_temporal_patterns(self, df):
        """Analyze temporal patterns in neural data"""
        
        temporal_analysis = {}
        
        # Rolling statistics
        temporal_analysis['rolling_mean_firing_rate'] = df['average_firing_rate'].rolling(window=50).mean()
        temporal_analysis['rolling_std_firing_rate'] = df['average_firing_rate'].rolling(window=50).std()
        temporal_analysis['rolling_energy'] = df['energy_consumption'].rolling(window=50).mean()
        
        # Trend analysis
        temporal_analysis['firing_rate_trend'] = np.polyfit(
            df.index, df['average_firing_rate'], 1
        )[0]  # Slope
        
        temporal_analysis['energy_trend'] = np.polyfit(
            df.index, df['energy_consumption'], 1
        )[0]
        
        # Stationarity test (simplified)
        from scipy import stats
        _, p_value = stats.kstest(df['average_firing_rate'].dropna(), 'norm')
        temporal_analysis['stationarity_p_value'] = p_value
        
        # Peak activity identification
        temporal_analysis['peak_activity_steps'] = df['average_firing_rate'].nlargest(5).index.tolist()
        temporal_analysis['peak_activity_values'] = df['average_firing_rate'].nlargest(5).values.tolist()
        
        return temporal_analysis
    
    def analyze_neural_spatial_distribution(self, df):
        """Analyze spatial distribution of neural populations"""
        
        spatial_analysis = {}
        
        # Neuron type proportions
        neuron_types = ['excitatory', 'inhibitory', 'sensory', 'motor', 'modulatory', 'internal']
        for neuron_type in neuron_types:
            col_name = f'{neuron_type}_neurons'
            if col_name in df.columns:
                spatial_analysis[f'{neuron_type}_proportion'] = df[col_name].sum() / df['neuron_count']
        
        # Population balance metrics
        excitatory_total = df['excitatory_neurons'].sum()
        inhibitory_total = df['inhibitory_neurons'].sum()
        
        spatial_analysis['e_i_ratio'] = excitatory_total / inhibitory_total if inhibitory_total > 0 else float('inf')
        spatial_analysis['balance_score'] = 2 * (excitatory_total * inhibitory_total) / (
            (excitatory_total + inhibitory_total) ** 2 + 1e-10
        )
        
        # Activity distribution
        spatial_analysis['high_activity_neurons'] = (df['average_firing_rate'] > 50).sum()
        spatial_analysis['low_activity_neurons'] = (df['average_firing_rate'] < 5).sum()
        spatial_analysis['silent_neurons'] = (df['average_firing_rate'] == 0).sum()
        
        return spatial_analysis
    
    def analyze_behavioral_patterns(self, df):
        """Analyze behavioral patterns"""
        
        behavioral_analysis = {}
        
        # Action distribution
        behavioral_analysis['action_distribution'] = df['action_type'].value_counts()
        
        # Reward analysis
        behavioral_analysis['reward_statistics'] = {
            'mean': df['reward_received'].mean(),
            'std': df['reward_received'].std(),
            'min': df['reward_received'].min(),
            'max': df['reward_received'].max(),
            'positive_count': (df['reward_received'] > 0).sum(),
            'negative_count': (df['reward_received'] < 0).sum(),
            'zero_count': (df['reward_received'] == 0).sum()
        }
        
        # Curiosity and novelty dynamics
        behavioral_analysis['curiosity_trend'] = df['agent_curiosity'].rolling(window=100).mean()
        behavioral_analysis['novelty_trend'] = df['agent_novelty'].rolling(window=100).mean()
        behavioral_analysis['prediction_error_trend'] = df['agent_prediction_error'].rolling(window=100).mean()
        
        # Energy dynamics
        behavioral_analysis['energy_dynamics'] = {
            'initial_energy': df['agent_energy'].iloc[0],
            'final_energy': df['agent_energy'].iloc[-1],
            'energy_change': df['agent_energy'].iloc[-1] - df['agent_energy'].iloc[0],
            'energy_efficiency': df['agent_energy'].iloc[-1] / df['agent_energy'].iloc[0]
        }
        
        return behavioral_analysis
    
    def analyze_learning_patterns(self, df):
        """Analyze learning patterns and progress"""
        
        learning_analysis = {}
        
        # Learning curves
        learning_analysis['cumulative_spikes'] = df['total_spikes'].cumsum()
        learning_analysis['learning_rate'] = df['total_spikes'].diff() / df.index.to_series().diff()
        
        # Learning plateau detection
        learning_analysis['plateau_steps'] = self.detect_learning_plateaus(df['learning_rate'])
        
        # Exploration vs exploitation
        learning_analysis['exploration_metrics'] = {
            'curiosity_variance': df['agent_curiosity'].var(),
            'novelty_peaks': (df['agent_novelty'] > df['agent_novelty'].quantile(0.75)).sum(),
            'action_diversity': len(df['action_type'].unique())
        }
        
        return learning_analysis
    
    def analyze_correlations(self, df):
        """Analyze correlations between different metrics"""
        
        correlation_analysis = {}
        
        # Select relevant numeric columns for correlation
        numeric_columns = df.select_dtypes(include=[np.number]).columns
        
        # Compute correlation matrix
        correlation_matrix = df[numeric_columns].corr()
        correlation_analysis['correlation_matrix'] = correlation_matrix
        
        # Identify strong correlations
        strong_correlations = []
        for i in range(len(correlation_matrix.columns)):
            for j in range(i + 1, len(correlation_matrix.columns)):
                col1 = correlation_matrix.columns[i]
                col2 = correlation_matrix.columns[j]
                corr_value = correlation_matrix.iloc[i, j]
                
                if abs(corr_value) > 0.7:
                    strong_correlations.append({
                        'variables': (col1, col2),
                        'correlation': corr_value,
                        'interpretation': self.interpret_correlation(corr_value)
                    })
        
        correlation_analysis['strong_correlations'] = strong_correlations
        
        return correlation_analysis
    
    def detect_learning_plateaus(self, learning_rate_series):
        """Detect learning plateaus in the learning rate"""
        
        plateaus = []
        window_size = 20
        
        for i in range(len(learning_rate_series) - window_size):
            window = learning_rate_series.iloc[i:i + window_size]
            mean_window = window.mean()
            std_window = window.std()
            
            # Check if window has low variance (plateau)
            if std_window < mean_window * 0.1 and mean_window > 0:
                plateaus.append({
                    'start_step': i,
                    'end_step': i + window_size,
                    'mean_rate': mean_window,
                    'stability': 1 - (std_window / mean_window)
                })
        
        return plateaus
    
    def interpret_correlation(self, correlation):
        """Interpret correlation value"""
        
        if abs(correlation) < 0.3:
            return 'weak'
        elif abs(correlation) < 0.7:
            return 'moderate'
        else:
            return 'strong'
```

### 4. Matplotlib Integration for Visualization

```python
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import numpy as np
import pynlm
from matplotlib.animation import FuncAnimation

class MatplotlibNeuralVisualizer:
    """Advanced visualization using Matplotlib for neural data"""
    
    def __init__(self, brain, world, agent, figsize=(16, 12)):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Setup figure with sophisticated layout
        self.fig = plt.figure(figsize=figsize)
        self.gs = gridspec.GridSpec(3, 4, figure=self.fig, hspace=0.3, wspace=0.2)
        
        # Initialize subplot references
        self.setup_subplots()
        
        # Data storage for animation
        self.plot_data = {
            'time_series': [],
            'neural_activity': [],
            'behavioral_metrics': [],
            'spatial_data': []
        }
        
        # Animation parameters
        self.current_frame = 0
        self.max_frames = 200
    
    def setup_subplots(self):
        """Setup sophisticated subplot layout"""
        
        # Large neural activity heatmap (2x2)
        self.ax_heatmap = self.fig.add_subplot(self.gs[0, :2])
        
        # Firing rate distribution (top right)
        self.ax_firing_dist = self.fig.add_subplot(self.gs[0, 2])
        
        # Spike raster plot (top left second)
        self.ax_raster = self.fig.add_subplot(self.gs[0, 3])
        
        # Behavioral metrics (middle row)
        self.ax_behavioral = self.fig.add_subplot(self.gs[1, :])
        
        # Learning curves (bottom left)
        self.ax_learning = self.fig.add_subplot(self.gs[2, :2])
        
        # Environmental interaction (bottom right)
        self.ax_environment = self.fig.add_subplot(self.gs[2, 2:])
        
        # Apply styling
        self.style_subplots()
    
    def style_subplots(self):
        """Apply styling to all subplots"""
        
        # Common styling
        for ax in [self.ax_heatmap, self.ax_firing_dist, self.ax_raster,
                  self.ax_behavioral, self.ax_learning, self.ax_environment]:
            ax.grid(True, alpha=0.3)
            ax.spines['top'].set_alpha(0.0)
            ax.spines['right'].set_alpha(0.0)
    
    def create_advanced_visualization(self, simulation_steps=300):
        """Create advanced visualization with multiple subplots"""
        
        print(f"=== Creating Advanced Matplotlib Visualization ===")
        
        # Initialize data storage
        self.plot_data = {
            'time': list(range(simulation_steps)),
            'firing_rates': [],
            'spike_counts': [],
            'behavioral_metrics': {
                'energy': [],
                'curiosity': [],
                'novelty': [],
                'prediction_error': []
            },
            'spatial_data': {
                'excitatory': [],
                'inhibitory': [],
                'sensory': [],
                'motor': []
            }
        }
        
        # Collect initial data
        self.collect_initial_data(simulation_steps)
        
        # Create visualization
        self.create_neural_heatmap()
        self.create_firing_distribution()
        self.create_spike_raster()
        self.create_behavioral_dashboard()
        self.create_learning_curves()
        self.create_environment_visualization()
        
        # Add annotation and styling
        self.add_visual_annotations()
        
        plt.tight_layout()
        
        print(f"   ✓ Advanced visualization created with {simulation_steps} steps")
        
        return self.fig
    
    def collect_initial_data(self, simulation_steps):
        """Collect initial data for visualization"""
        
        for step in range(simulation_steps):
            # Update simulation
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Collect neural data
            firing_rate = self.brain.getAverageFiringRate()
            total_spikes = self.brain.getTotalSpikeCount()
            
            self.plot_data['firing_rates'].append(firing_rate)
            self.plot_data['spike_counts'].append(total_spikes)
            
            # Collect behavioral data
            body = self.world.getAgentBody()
            self.plot_data['behavioral_metrics']['energy'].append(body.getEnergy())
            self.plot_data['behavioral_metrics']['curiosity'].append(self.agent.getCuriosityLevel())
            self.plot_data['behavioral_metrics']['novelty'].append(self.agent.getNoveltyLevel())
            self.plot_data['behavioral_metrics']['prediction_error'].append(self.agent.getPredictionError())
            
            # Collect spatial data
            for neuron_type in pynlm.NeuronType:
                count = self.brain.getNeuronCount(neuron_type)
                self.plot_data['spatial_data'][neuron_type.name.lower()].append(count)
    
    def create_neural_heatmap(self):
        """Create neural activity heatmap"""
        
        # Create synthetic neural activity data for visualization
        time_steps = len(self.plot_data['time'])
        neurons = min(50, self.brain.getTotalNeuronCount() // 20)  # Sample neurons
        
        # Create 2D neural activity heatmap
        heatmap_data = np.zeros((neurons, time_steps))
        
        for t in range(time_steps):
            # Simulate neural activity patterns
            base_activity = self.plot_data['firing_rates'][t] / 100
            noise = np.random.normal(0, 0.1, neurons)
            
            # Create spatiotemporal patterns
            for n in range(neurons):
                # Add spatial spread
                spatial_factor = np.exp(-((n - neurons//2) ** 2) / (2 * 10 ** 2))
                
                # Add temporal dynamics
                temporal_factor = 0.5 + 0.5 * np.sin(t * 0.05 + n * 0.1)
                
                heatmap_data[n, t] = base_activity * spatial_factor * temporal_factor + noise[n]
        
        # Create heatmap
        im = self.ax_heatmap.imshow(heatmap_data, 
                                  aspect='auto',
                                  cmap='hot',
                                  interpolation='bilinear',
                                  vmin=0, vmax=1)
        
        self.ax_heatmap.set_title('Neural Activity Heatmap', fontsize=12, fontweight='bold')
        self.ax_heatmap.set_xlabel('Time Steps')
        self.ax_heatmap.set_ylabel('Neuron Index')
        
        # Add colorbar
        cbar = plt.colorbar(im, ax=self.ax_heatmap, shrink=0.8)
        cbar.set_label('Normalized Activity', rotation=270, labelpad=20)
    
    def create_firing_distribution(self):
        """Create firing rate distribution plot"""
        
        # Create kernel density estimate of firing rates
        firing_rates = np.array(self.plot_data['firing_rates'])
        
        # Filter out zero rates for better visualization
        non_zero_rates = firing_rates[firing_rates > 0]
        
        if len(non_zero_rates) > 1:
            # Create KDE plot
            from scipy import stats
            kde = stats.gaussian_kde(non_zero_rates)
            
            # Create density curve
            x_range = np.linspace(non_zero_rates.min(), non_zero_rates.max(), 200)
            density = kde(x_range)
            
            self.ax_firing_dist.plot(x_range, density, 'b-', linewidth=2, label='Firing Rate Distribution')
            
            # Add statistical indicators
            mean_rate = np.mean(non_zero_rates)
            std_rate = np.std(non_zero_rates)
            median_rate = np.median(non_zero_rates)
            
            self.ax_firing_dist.axvline(mean_rate, color='red', linestyle='--', alpha=0.7, label=f'Mean: {mean_rate:.1f} Hz')
            self.ax_firing_dist.axvline(median_rate, color='green', linestyle='--', alpha=0.7, label=f'Median: {median_rate:.1f} Hz')
            
            # Add percentile lines
            p25 = np.percentile(non_zero_rates, 25)
            p75 = np.percentile(non_zero_rates, 75)
            
            self.ax_firing_dist.axvline(p25, color='orange', linestyle=':', alpha=0.7, label=f'25-75%: {p25:.1f}-{p75:.1f} Hz')
        
        self.ax_firing_dist.set_title('Firing Rate Distribution', fontsize=12, fontweight='bold')
        self.ax_firing_dist.set_xlabel('Firing Rate (Hz)')
        self.ax_firing_dist.set_ylabel('Density')
        self.ax_firing_dist.legend()
        self.ax_firing_dist.grid(True, alpha=0.3)
    
    def create_spike_raster(self):
        """Create spike raster plot"""
        
        # Create synthetic spike data for visualization
        time_steps = len(self.plot_data['time'])
        num_neurons = min(100, self.brain.getTotalNeuronCount() // 10)
        
        # Generate synthetic spike trains
        spike_raster = np.zeros((num_neurons, time_steps))
        
        for neuron in range(num_neurons):
            # Generate irregular spike trains
            spike_prob = self.plot_data['firing_rates'] / 200  # Convert Hz to probability per step
            
            for t in range(time_steps):
                if np.random.random() < spike_prob[t]:
                    spike_raster[neuron, t] = 1
        
        # Create raster plot
        self.ax_raster.imshow(spike_raster, 
                            cmap='binary',
                            aspect='auto',
                            interpolation='nearest',
                            vmin=0, vmax=1)
        
        self.ax_raster.set_title('Spike Raster Plot', fontsize=12, fontweight='bold')
        self.ax_raster.set_xlabel('Time Steps')
        self.ax_raster.set_ylabel('Neuron Index')
        
        # Add statistical annotations
        total_spikes = np.sum(spike_raster)
        firing_rate = total_spikes / (num_neurons * time_steps)
        
        self.ax_raster.text(0.02, 0.98, f'Total Spikes: {total_spikes:,}\n'
                            f'Average Firing Rate: {firing_rate:.2f} Hz',
                            transform=self.ax_raster.transAxes,
                            fontsize=10,
                            verticalalignment='top',
                            bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    def create_behavioral_dashboard(self):
        """Create behavioral metrics dashboard"""
        
        # Create subplots for behavioral metrics
        behavioral_data = self.plot_data['behavioral_metrics']
        
        # Energy dynamics with dual y-axis
        ax1 = self.ax_behavioral.twinx()
        
        # Plot energy on primary axis
        ax1.plot(self.plot_data['time'], behavioral_data['energy'],
                'r-', linewidth=2, label='Agent Energy')
        ax1.set_ylabel('Energy', color='red')
        ax1.tick_params(axis='y', labelcolor='red')
        
        # Plot curiosity on secondary axis
        ax2 = self.ax_behavioral.twinx()
        ax2.spines['right'].set_position(('outward', 60))
        
        ax2.plot(self.plot_data['time'], behavioral_data['curiosity'],
                'b-', linewidth=2, label='Curiosity')
        ax2.set_ylabel('Curiosity Level', color='blue')
        ax2.tick_params(axis='y', labelcolor='blue')
        
        # Plot novelty on another axis
        ax3 = self.ax_behavioral.twinx()
        ax3.spines['right'].set_position(('outward', 120))
        
        ax3.plot(self.plot_data['time'], behavioral_data['novelty'],
                'g-', linewidth=2, label='Novelty')
        ax3.set_ylabel('Novelty Level', color='green')
        ax3.tick_params(axis='y', labelcolor='green')
        
        # Plot prediction error
        ax3.plot(self.plot_data['time'], behavioral_data['prediction_error'],
                'purple', linewidth=2, linestyle='--', label='Prediction Error')
        
        # Combine legends
        lines, labels = self.ax_behavioral.get_legend_handles_labels()
        lines2, labels2 = ax1.get_legend_handles_labels()
        lines3, labels3 = ax2.get_legend_handles_labels()
        lines4, labels4 = ax3.get_legend_handles_labels()
        
        self.ax_behavioral.legend(lines + lines2 + lines3 + lines4,
                                labels + labels2 + labels3 + labels4,
                                loc='upper left', bbox_to_anchor=(0.02, 0.98))
        
        self.ax_behavioral.set_title('Behavioral Metrics Dashboard', fontsize=12, fontweight='bold')
        self.ax_behavioral.set_xlabel('Time Steps')
        self.ax_behavioral.set_ylabel('Energy')
        self.ax_behavioral.grid(True, alpha=0.3)
    
    def create_learning_curves(self):
        """Create learning curves visualization"""
        
        # Create learning curves with shaded uncertainty regions
        time_steps = len(self.plot_data['time'])
        
        # Calculate rolling statistics
        window_size = 20
        firing_rates = np.array(self.plot_data['firing_rates'])
        
        rolling_mean = np.convolve(firing_rates, np.ones(window_size)/window_size, mode='valid')
        rolling_std = np.std(np.lib.stride_tricks.sliding_window_view(
            firing_rates, window_size), axis=1)
        
        # Create time alignment for rolling statistics
        rolling_time = np.arange(window_size-1, time_steps)
        
        # Plot learning curves
        self.ax_learning.plot(rolling_time, rolling_mean,
                            'b-', linewidth=2, label='Rolling Mean Firing Rate')
        
        # Add uncertainty band
        self.ax_learning.fill_between(rolling_time,
                                    rolling_mean - 2 * rolling_std,
                                    rolling_mean + 2 * rolling_std,
                                    alpha=0.3, color='blue', label='±2 Standard Deviations')
        
        # Add target level (learning goal)
        target_level = 50.0
        self.ax_learning.axhline(y=target_level, color='red', linestyle='--',
                               alpha=0.7, label=f'Target Level ({target_level} Hz)')
        
        self.ax_learning.set_title('Learning Curves and Stability', fontsize=12, fontweight='bold')
        self.ax_learning.set_xlabel('Time Steps')
        self.ax_learning.set_ylabel('Firing Rate (Hz)')
        self.ax_learning.legend()
        self.ax_learning.grid(True, alpha=0.3)
        
        # Add inset plot for final statistics
        inset_ax = self.ax_learning.inset_axes([0.65, 0.65, 0.3, 0.25])
        
        final_firing_rate = firing_rates[-1]
        final_std = np.std(firing_rates[-window_size:])
        
        inset_ax.bar(['Final', 'Std Dev'], [final_firing_rate, final_std],
                    color=['skyblue', 'lightcoral'], alpha=0.7)
        inset_ax.set_ylabel('Firing Rate (Hz)')
        inset_ax.set_title('Final Statistics')
    
    def create_environment_visualization(self):
        """Create environment visualization"""
        
        # Create a simplified environment map
        env_size = 20
        env_map = np.zeros((env_size, env_size, 3), dtype=np.uint8)
        
        # Add environmental features (simplified)
        # In a real implementation, this would use actual world data
        
        # Draw environment boundaries
        env_map[0, :] = [128, 128, 128]  # Top wall
        env_map[-1, :] = [128, 128, 128]  # Bottom wall
        env_map[:, 0] = [128, 128, 128]  # Left wall
        env_map[:, -1] = [128, 128, 128]  # Right wall
        
        # Draw agent position
        agent_x, agent_y = 10, 10  # Simplified position
        env_map[agent_x, agent_y] = [255, 0, 0]  # Red agent
        
        # Display environment map
        self.ax_environment.imshow(env_map, interpolation='nearest')
        
        self.ax_environment.set_title('Environment Context', fontsize=12, fontweight='bold')
        self.ax_environment.set_xlabel('X Position')
        self.ax_environment.set_ylabel('Y Position')
        
        # Add legend
        from matplotlib.patches import Patch
        legend_elements = [
            Patch(facecolor=[255, 0, 0], label='Agent'),
            Patch(facecolor=[0, 255, 0], label='Resource'),
            Patch(facecolor=[255, 0, 0], label='Hazard'),
        ]
        self.ax_environment.legend(handles=legend_elements, loc='upper right')
    
    def add_visual_annotations(self):
        """Add annotations and visual elements"""
        
        # Add main title
        self.fig.suptitle('Advanced Neural Activity Visualization with Scientific Python Integration',
                         fontsize=14, fontweight='bold', y=0.98)
        
        # Add subplot titles
        annotations = [
            (self.ax_heatmap, 'Real-time neural activity heatmap showing spatiotemporal patterns'),
            (self.ax_firing_dist, 'Statistical distribution of firing rates across the neural population'),
            (self.ax_raster, 'Spike raster plot showing individual neuron spiking patterns'),
            (self.ax_behavioral, 'Multi-axis dashboard showing agent behavioral metrics'),
            (self.ax_learning, 'Learning curves showing neural adaptation and stability'),
            (self.ax_environment, 'Environmental context showing agent-world interaction')
        ]
        
        for ax, text in annotations:
            ax.annotate(text, xy=(0.5, -0.1), xycoords='axes fraction',
                       ha='center', fontsize=8, style='italic', alpha=0.7)
    
    def create_animation(self, interval=50):
        """Create animation of neural visualization"""
        
        print(f"   Creating animation with {interval}ms interval...")
        
        # Create animation function
        def update_frame(frame):
            self.current_frame = frame
            # Update any dynamic elements here
            return []
        
        # Create animation
        ani = FuncAnimation(self.fig, update_frame, 
                           frames=self.max_frames, 
                           interval=interval, 
                           blit=False, 
                           repeat=True)
        
        return ani
```

### 5. Jupyter Notebook Integration

```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from IPython.display import display, HTML

class JupyterNeuralNotebook:
    """Create Jupyter notebook with NLM integration"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Notebook content
        self.notebook_cells = []
        
        # Code execution history
        self.execution_history = []
    
    def create_neural_research_notebook(self):
        """Create comprehensive Jupyter notebook for neural research"""
        
        print(f"=== Creating Neural Research Jupyter Notebook ===")
        
        # Create notebook structure
        self.notebook_cells.append({
            'cell_type': 'markdown',
            'content': '''
# Neural Network Research with NLM Framework

## Overview
This notebook demonstrates the integration of the Neural Learning Machine (NLM) framework with Python scientific computing tools for advanced neural research.

## Learning Objectives
- Understand neural network dynamics using NLM
- Analyze neural data with Pandas and NumPy
- Visualize neural activity with Matplotlib
- Apply machine learning algorithms to neural data
- Conduct hypothesis testing with statistical analysis

## Environment Setup
```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats, optimize
import seaborn as sns

# Initialize NLM system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```
'''
        })
        
        # 1. Data Collection Cell
        self.notebook_cells.append({
            'cell_type': 'code',
            'content': '''
# === Data Collection and Initial Analysis ===

import pandas as pd
import numpy as np
import time

# Set up data collection
def collect_neural_data(simulation_steps=500):
    """Collect comprehensive neural data for analysis"""
    
    data = {
        'time_step': [],
        'neuron_count': [],
        'total_spikes': [],
        'average_firing_rate': [],
        'energy_consumption': [],
        'agent_energy': [],
        'agent_curiosity': [],
        'agent_novelty': [],
        'reward_received': []
    }
    
    for step in range(simulation_steps):
        # Collect simulation state
        data['time_step'].append(step)
        data['neuron_count'].append(brain.getTotalNeuronCount())
        data['total_spikes'].append(brain.getTotalSpikeCount())
        data['average_firing_rate'].append(brain.getAverageFiringRate())
        data['energy_consumption'].append(brain.getTotalEnergyConsumption())
        
        # Get agent and world state
        body = world.getAgentBody()
        percept = world.getSensoryPercept())
        
        data['agent_energy'].append(body.getEnergy())
        data['agent_curiosity'].append(agent.getCuriosityLevel())
        data['agent_novelty'].append(agent.getNoveltyLevel())
        data['reward_received'].append(
            percept.getInternal()[0] if percept.getInternal() else 0.0
        )
        
        # Update simulation
        world.update(0.1)
        agent.processSensoryInput(percept)
        brain.step(step)
    
    # Create DataFrame
    df = pd.DataFrame(data)
    df.set_index('time_step', inplace=True)
    
    return df

# Collect and display data
sim_data = collect_neural_data(200)
print(f"Collected {len(sim_data)} time steps of neural data")
print(f"\nData summary:")
print(f"Neuron count: {sim_data['neuron_count'].iloc[-1]}")
print(f"Total spikes: {sim_data['total_spikes'].iloc[-1]}")
print(f"Average firing rate: {sim_data['average_firing_rate'].mean():.2f} Hz")
print(f"Energy efficiency: {sim_data['agent_energy'].iloc[-1]/sim_data['agent_energy'].iloc[0]:.2f}")

# Display data sample
print(f"\nData sample (first 5 time steps):")
print(sim_data.head())
'''
        })
        
        # 2. Statistical Analysis Cell
        self.notebook_cells.append({
            'cell_type': 'code',
            'content': '''
# === Statistical Analysis of Neural Data ===

import seaborn as sns
import matplotlib.pyplot as plt
from scipy import stats
import numpy as np

# 1. Basic statistics
print("=== Basic Statistics ===")
print(sim_data.describe())

# 2. Correlation analysis
print("\n=== Correlation Analysis ===")
numeric_cols = sim_data.select_dtypes(include=[np.number]).columns
correlation_matrix = sim_data[numeric_cols].corr()

# Display correlation heatmap
plt.figure(figsize=(10, 8))
sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', center=0, square=True)
plt.title('Neural Data Correlation Matrix')
plt.tight_layout()
plt.show()

# 3. Distribution analysis
print("\n=== Distribution Analysis ===")

fig, axes = plt.subplots(2, 2, figsize=(12, 10))
fig.suptitle('Neural Data Distributions', fontsize=14)

# Firing rate distribution
axes[0, 0].hist(sim_data['average_firing_rate'], bins=30, alpha=0.7, color='skyblue', edgecolor='black')
axes[0, 0].set_xlabel('Firing Rate (Hz)')
axes[0, 0].set_ylabel('Frequency')
axes[0, 0].set_title('Firing Rate Distribution')
axes[0, 0].axvline(sim_data['average_firing_rate'].mean(), color='red', linestyle='--', label=f'Mean: {sim_data["average_firing_rate"].mean():.2f} Hz')
axes[0, 0].legend()

# Energy consumption distribution
axes[0, 1].hist(sim_data['energy_consumption'], bins=30, alpha=0.7, color='lightgreen', edgecolor='black')
axes[0, 1].set_xlabel('Energy Consumption')
axes[0, 1].set_ylabel('Frequency')
axes[0, 1].set_title('Energy Consumption Distribution')

# Scatter plot: firing rate vs energy
axes[1, 0].scatter(sim_data['average_firing_rate'], sim_data['energy_consumption'], 
                  alpha=0.6, c=sim_data['agent_energy'], cmap='viridis', s=20)
axes[1, 0].set_xlabel('Firing Rate (Hz)')
axes[1, 0].set_ylabel('Energy Consumption')
axes[1, 0].set_title('Firing Rate vs Energy Consumption')
axes[1, 0].colorbar(label='Agent Energy')

# Time series with moving average
axes[1, 1].plot(sim_data.index, sim_data['average_firing_rate'], alpha=0.7, label='Firing Rate')
rolling_mean = sim_data['average_firing_rate'].rolling(window=10).mean()
axes[1, 1].plot(rolling_mean, 'r-', linewidth=2, label='Moving Average (10 steps)')
axes[1, 1].set_xlabel('Time Steps')
axes[1, 1].set_ylabel('Firing Rate (Hz)')
axes[1, 1].set_title('Temporal Dynamics of Firing Rate')
axes[1, 1].legend()
axes[1, 1].grid(True, alpha=0.3)

plt.tight_layout()
plt.show()

# 4. Hypothesis testing
print("\n=== Hypothesis Testing ===")

# Test if firing rate changes over time
from scipy import stats
firing_rate_trend = np.polyfit(sim_data.index, sim_data['average_firing_rate'], 1)
trend_slope = firing_rate_trend[0]

print(f"Linear regression slope: {trend_slope:.6f}")
print(f"P-value for trend: {stats.ttest_1samp(sim_data['average_firing_rate'], sim_data['average_firing_rate'].mean())[1]:.4f}")

# Test for stationarity (simplified)
from statsmodels.tsa.stattools import adfuller
adfull_result = adfuller(sim_data['average_firing_rate'].dropna())
print(f"\nAugmented Dickey-Fuller test:")
print(f"  Statistic: {adfull_result[0]:.4f}")
print(f"  P-value: {adfull_result[1]:.4f}")
print(f"  Critical values: {adfull_result[4]}")
'''
        })
        
        # 3. Machine Learning Integration Cell
        self.notebook_cells.append({
            'cell_type': 'code',
            'content': '''
# === Machine Learning Analysis of Neural Data ===

from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.cluster import KMeans
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, r2_score
import seaborn as sns
import matplotlib.pyplot as plt

# 1. Feature preparation
print("=== Feature Preparation ===")

# Select features for ML analysis
features = sim_data[['average_firing_rate', 'energy_consumption', 
                     'agent_curiosity', 'agent_novelty', 'agent_prediction_error']]
target = sim_data['agent_energy']

# Handle missing values
features = features.fillna(features.mean())

# Normalize features
scaler = StandardScaler()
features_scaled = scaler.fit_transform(features)

# Display feature statistics
print("\nFeature statistics after scaling:")
print(f"  Shape: {features_scaled.shape}")
print(f"  Mean: {features_scaled.mean(axis=0)}")
print(f"  Std: {features_scaled.std(axis=0)}")

# 2. Principal Component Analysis (PCA)
print("\n=== Principal Component Analysis (PCA) ===")
pca = PCA()
pca_features = pca.fit_transform(features_scaled)

# Explained variance ratio
explained_variance = pca.explained_variance_ratio_
print(f"Explained variance by components: {explained_variance}")
print(f"Cumulative explained variance: {explained_variance.cumsum()}")

# Visualize PCA results
plt.figure(figsize=(10, 8))
plt.scatter(pca_features[:, 0], pca_features[:, 1], 
           c=sim_data['agent_energy'], cmap='viridis', alpha=0.7, s=50)
plt.colorbar(label='Agent Energy')
plt.xlabel(f'PC1 ({explained_variance[0]*100:.1f}%)')
plt.ylabel(f'PC2 ({explained_variance[1]*100:.1f}%)')
plt.title('Neural Data - Principal Component Analysis')
plt.grid(True, alpha=0.3)
plt.show()

# 3. Clustering Analysis
print("\n=== Clustering Analysis ===")

# Determine optimal number of clusters
inertia = []
for k in range(2, 11):
    kmeans = KMeans(n_clusters=k, random_state=42)
    kmeans.fit(pca_features)
    inertia.append(kmeans.inertia_)

# Plot elbow curve
plt.figure(figsize=(8, 5))
plt.plot(range(2, 11), inertia, 'bo-', linewidth=2)
plt.xlabel('Number of Clusters')
plt.ylabel('Inertia')
plt.title('Elbow Method for Optimal Cluster Number')
plt.grid(True, alpha=0.3)
plt.show()

# Apply K-means with optimal clusters (using elbow method)
n_clusters = 3
kmeans = KMeans(n_clusters=n_clusters, random_state=42)
cluster_labels = kmeans.fit_predict(pca_features)

# Visualize clusters
plt.figure(figsize=(10, 8))
scatter = plt.scatter(pca_features[:, 0], pca_features[:, 1], 
                     c=cluster_labels, cmap='viridis', alpha=0.7, s=50)
plt.xlabel(f'PC1 ({explained_variance[0]*100:.1f}%)')
plt.ylabel(f'PC2 ({explained_variance[1]*100:.1f}%)')
plt.title(f'Neural Data Clustering (K={n_clusters})')
plt.legend(handles=scatter.legend_elements()[0], labels=[f'Cluster {i}' for i in range(n_clusters)])
plt.grid(True, alpha=0.3)
plt.show()

# 4. Regression Analysis
print("\n=== Regression Analysis ===")

# Split data for regression
train_size = int(0.8 * len(features_scaled))
X_train, X_test = features_scaled[:train_size], features_scaled[train_size:]
y_train, y_test = target[:train_size], target[train_size:]

# Train linear regression
lr = LinearRegression()
lr.fit(X_train, y_train)

# Make predictions
y_pred = lr.predict(X_test)

# Evaluate model performance
mse = mean_squared_error(y_test, y_pred)
r2 = r2_score(y_test, y_pred)

print(f"Linear Regression Performance:")
print(f"  Mean Squared Error: {mse:.4f}")
print(f"  R-squared: {r2:.4f}")

# Visualize predictions
plt.figure(figsize=(10, 8))
plt.scatter(y_test, y_pred, alpha=0.7, s=50)
plt.plot([y_test.min(), y_test.max()], [y_test.min(), y_test.max()], 
         'r--', linewidth=2, label='Perfect prediction')
plt.xlabel('Actual Energy')
plt.ylabel('Predicted Energy')
plt.title('Linear Regression: Actual vs Predicted Energy')
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()

print("\nMachine Learning Analysis Complete!")
'''
        })
        
        # 4. Interactive Visualization Cell
        self.notebook_cells.append({
            'cell_type': 'code',
            'content': '''
# === Interactive Visualization Dashboard ===

import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from IPython.display import display, HTML

# Create interactive dashboard
print("Creating interactive visualization dashboard...")

# Create figure with multiple subplots
fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Interactive Neural Data Dashboard', fontsize=16, fontweight='bold')

# 1. Time series with interactive controls
title1 = axes[0, 0].set_title('Firing Rate Time Series', fontsize=12, fontweight='bold')
axes[0, 0].plot(sim_data.index, sim_data['average_firing_rate'], 
               color='blue', linewidth=1, label='Firing Rate')
axes[0, 0].set_xlabel('Time Steps')
axes[0, 0].set_ylabel('Firing Rate (Hz)')
axes[0, 0].legend()
axes[0, 0].grid(True, alpha=0.3)

# 2. Distribution comparison
title2 = axes[0, 1].set_title('Distribution Comparison', fontsize=12, fontweight='bold')
hist1 = axes[0, 1].hist(sim_data['average_firing_rate'], bins=20, alpha=0.7, 
                       color='skyblue', density=True, label='Firing Rate')
axes[0, 1].set_xlabel('Firing Rate (Hz)')
axes[0, 1].set_ylabel('Density')
axes[0, 1].legend()

# 3. Correlation heatmap
title3 = axes[1, 0].set_title('Correlation Heatmap', fontsize=12, fontweight='bold')
corr_data = sim_data[['average_firing_rate', 'energy_consumption', 
                      'agent_energy', 'agent_curiosity', 'agent_novelty']]
sns.heatmap(corr_data.corr(), annot=True, cmap='coolwarm', center=0, 
           square=True, ax=axes[1, 0])
axes[1, 0].set_title('')

# 4. Scatter plot with hover information
title4 = axes[1, 1].set_title('Interactive Scatter Plot', fontsize=12, fontweight='bold')
scatter = axes[1, 1].scatter(sim_data['average_firing_rate'], sim_data['agent_energy'],
                           c=sim_data['agent_curiosity'], cmap='viridis',
                           alpha=0.7, s=50, edgecolors='black', linewidth=0.5)
axes[1, 1].set_xlabel('Firing Rate (Hz)')
axes[1, 1].set_ylabel('Agent Energy')
axes[1, 1].set_title('Firing Rate vs Agent Energy (Colored by Curiosity)')
plt.colorbar(scatter, ax=axes[1, 1], label='Curiosity Level')

plt.tight_layout()

# Display the plot
plt.show()

# Create HTML widget for additional interactivity
html_widget = HTML('''
<div>
<h3>Neural Data Analysis Controls</h3>
<ul>
<li><b>Time Range:</b> <input type="range" min="0" max="199" value="100" id="timeSlider"></li>
<li><b>Firing Rate Threshold:</b> <input type="range" min="0" max="100" value="10" id="firingSlider"></li>
<li><b>Energy Threshold:</b> <input type="range" min="0" max="100" value="50" id="energySlider"></li>
</ul>
<p>Use the sliders to filter and explore the neural data interactively.</p>
</div>
''')

# Note: Full interactivity would require JavaScript widgets (ipywidgets)
# This is a simplified version for demonstration
print("Dashboard created successfully!")
print("Full interactive features would use ipywidgets for enhanced interactivity.")
'''
        })
        
        # 5. Advanced Machine Learning Cell
        self.notebook_cells.append({
            'cell_type': 'code',
            'content': '''
# === Advanced Machine Learning: Neural Network Prediction ===

from sklearn.neural_network import MLPRegressor
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import cross_val_score
import matplotlib.pyplot as plt
import numpy as np

# Prepare data for advanced ML
print("=== Advanced Machine Learning Analysis ===")

# Create additional features for better predictions
ml_features = sim_data[['average_firing_rate', 'energy_consumption',
                       'agent_curiosity', 'agent_novelty',
                       'agent_prediction_error']].copy()

# Add derived features
ml_features['firing_rate_squared'] = ml_features['average_firing_rate'] ** 2
ml_features['energy_squared'] = ml_features['energy_consumption'] ** 2
ml_features['curiosity_energy_interaction'] = ml_features['agent_curiosity'] * ml_features['energy_consumption']
ml_features['novelty_prediction_interaction'] = ml_features['agent_novelty'] * ml_features['agent_prediction_error']

# Time-based features
ml_features['time_of_day'] = ml_features.index % 24  # Simulated time
ml_features['day_of_week'] = ml_features.index % 7   # Simulated day

# Scale features for neural networks
scaler = StandardScaler()
ml_features_scaled = scaler.fit_transform(ml_features)

# 1. Multi-Layer Perceptron (Neural Network)
print("\n1. Training Multi-Layer Perceptron...")
mlp = MLPRegressor(
    hidden_layer_sizes=(50, 25, 10),
    activation='relu',
    solver='adam',
    max_iter=500,
    random_state=42,
    alpha=0.001
)

# Cross-validation score
mlp_scores = cross_val_score(mlp, ml_features_scaled, sim_data['agent_energy'], 
                            cv=5, scoring='r2')
print(f"   MLP R-squared (5-fold CV): {mlp_scores.mean():.4f} ± {mlp_scores.std():.4f}")

# 2. Random Forest
print("\n2. Training Random Forest...")
rf = RandomForestRegressor(
    n_estimators=100,
    max_depth=10,
    random_state=42,
    n_jobs=-1
)

rf_scores = cross_val_score(rf, ml_features_scaled, sim_data['agent_energy'],
                            cv=5, scoring='r2')
print(f"   Random Forest R-squared (5-fold CV): {rf_scores.mean():.4f} ± {rf_scores.std():.4f}")

# 3. Compare models and make predictions
print("\n3. Model Comparison...")

# Train models on full dataset
mlp.fit(ml_features_scaled, sim_data['agent_energy'])
rf.fit(ml_features_scaled, sim_data['agent_energy'])

# Make predictions
mlp_pred = mlp.predict(ml_features_scaled)
rf_pred = rf.predict(ml_features_scaled)

# Calculate performance metrics
mlp_mse = mean_squared_error(sim_data['agent_energy'], mlp_pred)
rf_mse = mean_squared_error(sim_data['agent_energy'], rf_pred)

mlp_r2 = r2_score(sim_data['agent_energy'], mlp_pred)
rf_r2 = r2_score(sim_data['agent_energy'], rf_pred)

print(f"   MLP Performance: R² = {mlp_r2:.4f}, MSE = {mlp_mse:.4f}")
print(f"   RF Performance:  R² = {rf_r2:.4f}, MSE = {rf_mse:.4f}")

# 4. Feature importance analysis
print("\n4. Feature Importance Analysis...")

# Get feature importances from Random Forest
importances = rf.feature_importances_
feature_names = ml_features.columns.tolist()

# Display feature importances
feature_importance_df = pd.DataFrame({
    'feature': feature_names,
    'importance': importances
}).sort_values('importance', ascending=False)

print(feature_importance_df)

# Visualize feature importances
plt.figure(figsize=(10, 6))
plt.barh(range(len(feature_names)), feature_importances)
plt.yticks(range(len(feature_names)), feature_names)
plt.xlabel('Feature Importance')
plt.title('Feature Importance from Random Forest')
plt.gca().invert_yaxis()
plt.tight_layout()
plt.show()

# 5. Prediction visualization
print("\n5. Prediction Visualization...")

plt.figure(figsize=(12, 8))

# Actual vs predicted for both models
plt.subplot(2, 2, 1)
plt.scatter(sim_data['agent_energy'], rf_pred, alpha=0.7, s=20, label='Random Forest')
plt.plot([sim_data['agent_energy'].min(), sim_data['agent_energy'].max()],
         [sim_data['agent_energy'].min(), sim_data['agent_energy'].max()], 'r--', lw=2, label='Perfect Prediction')
plt.xlabel('Actual Energy')
plt.ylabel('Predicted Energy')
plt.title('Random Forest: Actual vs Predicted')
plt.legend()
plt.grid(True, alpha=0.3)

plt.subplot(2, 2, 2)
plt.scatter(sim_data['agent_energy'], mlp_pred, alpha=0.7, s=20, color='orange', label='MLP')
plt.plot([sim_data['agent_energy'].min(), sim_data['agent_energy'].max()],
         [sim_data['agent_energy'].min(), sim_data['agent_energy'].max()], 'r--', lw=2, label='Perfect Prediction')
plt.xlabel('Actual Energy')
plt.ylabel('Predicted Energy')
plt.title('MLP: Actual vs Predicted')
plt.legend()
plt.grid(True, alpha=0.3)

# Residual analysis
plt.subplot(2, 2, 3)
residuals_rf = sim_data['agent_energy'] - rf_pred
residuals_mlp = sim_data['agent_energy'] - mlp_pred

plt.hist(residuals_rf, bins=20, alpha=0.7, label='Random Forest', density=True)
plt.hist(residuals_mlp, bins=20, alpha=0.7, label='MLP', density=True)
plt.xlabel('Residuals')
plt.ylabel('Density')
plt.title('Residual Distribution')
plt.legend()
plt.grid(True, alpha=0.3)

# Prediction error over time
plt.subplot(2, 2, 4)
rf_error = np.abs(residuals_rf)
mlp_error = np.abs(residuals_mlp)

plt.plot(sim_data.index, rf_error, label='Random Forest Error', alpha=0.7)
plt.plot(sim_data.index, mlp_error, label='MLP Error', alpha=0.7)
plt.xlabel('Time Steps')
plt.ylabel('Absolute Error')
plt.title('Prediction Error Over Time')
plt.legend()
plt.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()

print("\nAdvanced Machine Learning Analysis Complete!")
'''
        })
        
        # 6. Summary Cell
        self.notebook_cells.append({
            'cell_type': 'code',
            'content': '''
# === Jupyter Notebook Summary ===

print("=== Neural Network Research with NLM - Jupyter Notebook Summary ===")

# Notebook structure
notebook_structure = {
    "Getting Started": [
        "Data Collection and Initial Analysis",
        "Statistical Analysis of Neural Data"
    ],
    "Machine Learning Integration": [
        "Machine Learning Analysis of Neural Data",
        "Advanced Machine Learning: Neural Network Prediction"
    ],
    "Visualization": [
        "Interactive Visualization Dashboard"
    ],
    "Research Workflow": [
        "Reproducible Research Practices",
        "Documentation and Reporting"
    ]
}

print("Notebook Structure:")
for section, subsections in notebook_structure.items():
    print(f"\n{section}:")
    for subsection in subsections:
        print(f"  - {subsection}")

print("\nKey Features:")
print("✓ Comprehensive data collection and analysis")
print("✓ Statistical hypothesis testing")
print("✓ Machine learning model evaluation")
print("✓ Feature importance analysis")
print("✓ Interactive visualization dashboard")
print("✓ Reproducible research workflow")

print("\nScientific Computing Capabilities:")
print("✓ Data manipulation with Pandas")
print("✓ Numerical computing with NumPy")
print("✓ Statistical analysis with SciPy")
print("✓ Visualization with Matplotlib")
print("✓ Machine learning with Scikit-learn")
print("✓ Interactive computing with Jupyter")

print("\nResearch Applications:")
print("• Neural network dynamics analysis")
print("• Brain-computer interface research")
print("• Computational neuroscience studies")
print("• Neuroinformatics data analysis")
print("• Artificial neural network research")

print("\nNotebook Advantages:")
print("• Interactive exploration of neural data")
print("• Reproducible research workflows")
print("• Integration with existing Python ecosystem")
print("• Publication-quality visualizations")
print("• Automated data analysis pipelines")

print("\nNext Steps for Research:")
print("1. Load and analyze experimental neural data")
print("2. Apply machine learning models for prediction")
print("3. Visualize results for scientific communication")
print("4. Document methodology for reproducibility")
print("5. Extend with custom analysis tools")

print("\n=== Notebook Complete ===")
'''
        })
    
    def display_notebook_content(self):
        """Display the notebook content"""
        
        print("=== NLM Neural Network Research - Jupyter Notebook ===")
        print("\nThis notebook provides comprehensive analysis of neural data")
        print("using the NLM framework with Python scientific computing tools.\n")
        
        # Display notebook structure
        print("Notebook Cells:")
        print("-" * 50)
        
        for i, cell in enumerate(self.notebook_cells):
            if cell['cell_type'] == 'markdown':
                print(f"\n{i+1}. Markdown: {cell['content'].split(chr(10))[0][:80]}...")
            else:
                print(f"\n{i+1}. Code: Neural data analysis and ML models")
        
        print(f"\nTotal cells: {len(self.notebook_cells)}")
        
        # Display example code
        print("\n" + "="*60)
        print("EXAMPLE NOTEBOK CELLS:")
        print("="*60)
        
        for i, cell in enumerate(self.notebook_cells[:3]):  # Show first 3 cells as examples
            if cell['cell_type'] == 'markdown':
                print(f"\n{i+1}. Markdown Section:")
                print("-" * 40)
                print(cell['content'][:200] + "...\n")
            else:
                print(f"\n{i+1}. Code Section:")
                print("-" * 40)
                print("Neural data collection and analysis code\n")
    
    def export_notebook(self, filename):
        """Export notebook to file format"""
        
        # In a real implementation, this would use nbformat to save as Jupyter notebook
        # For this demo, we create a text representation
        
        notebook_content = f"""# NLM Neural Network Research - Jupyter Notebook

## Overview
This notebook demonstrates integration of the Neural Learning Machine (NLM) framework with Python scientific computing for neural data analysis.

## Environment Setup
```python
import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats, optimize
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.model_selection import cross_val_score

# Initialize NLM system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

## Data Collection and Analysis
Comprehensive neural data collection and initial statistical analysis.

## Machine Learning Integration
Advanced machine learning models for neural data prediction and classification.

## Visualization Dashboard
Interactive visualization of neural activity and behavioral metrics.

## Research Workflow
Reproducible research practices and documentation.

## Key Features
- Data collection with Pandas
- Statistical analysis with SciPy
- Machine learning with Scikit-learn
- Visualization with Matplotlib
- Interactive computing with Jupyter

## Files Generated
- neural_data.csv: Collected simulation data
- analysis_results.json: Statistical analysis results
- model_performance.json: Machine learning model performance
- visualization_plots/: Generated plots and figures
- notebook_content.md: This notebook content

## Usage
1. Load the notebook in Jupyter
2. Execute cells in order
3. Customize parameters for your research
4. Export results for further analysis

## Citation
This notebook demonstrates the use of NLM framework with Python scientific computing for neural network research.
"""
        
        # Write to file
        with open(filename, 'w') as f:
            f.write(notebook_content)
        
        print(f"\nNotebooks exported to: {filename}")
        print("Note: Full Jupyter notebook export would use nbformat for .ipynb format")
    
    def run_complete_analysis(self):
        """Run complete analysis workflow"""
        
        print("=== Running Complete Neural Data Analysis ===")
        
        # Step 1: Create data collection dataframe
        print("1. Creating neural data collection...")
        sim_data = self.create_comprehensive_dataframe(500)
        
        # Step 2: Statistical analysis
        print("2. Performing statistical analysis...")
        self.analyze_neural_patterns(sim_data)
        
        # Step 3: Create notebook
        print("3. Creating Jupyter notebook...")
        self.create_neural_research_notebook()
        
        # Step 4: Display notebook structure
        print("4. Displaying notebook structure...")
        self.display_notebook_content()
        
        # Step 5: Export notebook
        print("5. Exporting notebook...")
        self.export_notebook("nlm_neural_research_notebook.md")
        
        print("\n=== Complete Analysis Workflow Finished ===")
        print("\nGenerated deliverables:")
        print("• Comprehensive neural data analysis")
        print("• Jupyter notebook with code and documentation")
        print("• Statistical analysis results")
        print("• Machine learning model evaluations")
        print("• Visualization code and examples")
        
        return self.notebook_cells
```

### 6. Complete Example Integration

```python
def main():
    """Complete Python scientific stack integration example"""
    
    print("=== NLM Python Scientific Stack Integration Example ===")
    print("This example demonstrates seamless integration of NLM with Python scientific computing.\n")
    
    # 1. Setup NLM system
    print("1. Setting up NLM neural system...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("   ✓ NLM system initialized")
    
    # 2. Create data analysis components
    print("\n2. Creating data analysis components...")
    
    # Create scientific computing components
    numpy_enhanced = NumpyEnhancedNeuralProcessor(brain)
    scipy_optimizer = SciPyNeuralOptimizer(brain, world, agent)
    pandas_analyzer = PandasNeuralDataAnalysis(brain, world, agent)
    matplotlib_visualizer = MatplotlibNeuralVisualizer(brain, world, agent)
    jupyter_notebook = JupyterNeuralNotebook(brain, world, agent)
    
    print("   ✓ NumPy enhanced processor created")
    print("   ✓ SciPy optimizer created")
    print("   ✓ Pandas analyzer created")
    print("   ✓ Matplotlib visualizer created")
    print("   ✓ Jupyter notebook creator created")
    
    # 3. Run complete analysis pipeline
    print("\n3. Running complete analysis pipeline...")
    
    # Collect data using Pandas analyzer
    sim_data = pandas_analyzer.create_comprehensive_dataframe(300)
    print("   ✓ Neural data collected")
    
    # Analyze patterns
    analysis_results = pandas_analyzer.analyze_neural_patterns(sim_data)
    print("   ✓ Neural patterns analyzed")
    
    # Optimize parameters
    initial_params = [100, 0.001, 0.01, 0.05]  # neuron_count, stdp, hebbian, density
    bounds = [(50, 1000), (0.0001, 0.01), (0.001, 0.1), (0.01, 0.2)]
    
    optimal_result, all_results = scipy_optimizer.optimize_neural_parameters(initial_params, bounds)
    print("   ✓ Neural parameters optimized")
    
    # Create advanced visualizations
    fig = matplotlib_visualizer.create_advanced_visualization(300)
    print("   ✓ Advanced visualizations created")
    
    # Create Jupyter notebook
    notebook_cells = jupyter_notebook.run_complete_analysis()
    print("   ✓ Jupyter notebook created")
    
    # 4. Save results
    print("\n4. Saving results...")
    
    # Save neural data
    sim_data.to_csv('neural_data.csv', index=False)
    print("   ✓ Neural data saved to neural_data.csv")
    
    # Save analysis results
    import json
    with open('analysis_results.json', 'w') as f:
        json.dump(analysis_results, f, indent=2, default=str)
    print("   ✓ Analysis results saved to analysis_results.json")
    
    # Save optimization results
    optimal_result_dict = {
        'best_method': optimal_result.method if hasattr(optimal_result, 'method') else 'unknown',
        'optimal_parameters': optimal_result.x.tolist() if hasattr(optimal_result, 'x') else optimal_result,
        'optimal_value': optimal_result.fun if hasattr(optimal_result, 'fun') else None,
        'all_results': {k: {'fun': v.fun if hasattr(v, 'fun') else None, 
                           'x': v.x.tolist() if hasattr(v, 'x') else None}
                       for k, v in all_results.items()}
    }
    
    with open('optimization_results.json', 'w') as f:
        json.dump(optimal_result_dict, f, indent=2)
    print("   ✓ Optimization results saved to optimization_results.json")
    
    # 5. Generate summary report
    print("\n5. Generating summary report...")
    
    create_scientific_summary(
        brain, world, agent,
        sim_data, analysis_results,
        optimal_result, matplotlib_visualizer
    )
    
    print("   ✓ Scientific summary generated")
    
    print("\n=== Python Scientific Stack Integration Complete ===")
    print("\nIntegration capabilities demonstrated:")
    print("✓ NumPy for efficient numerical computations")
    print("✓ SciPy for optimization and statistical analysis")
    print("✓ Pandas for comprehensive data management")
    print("✓ Matplotlib for advanced visualization")
    print("✓ Jupyter notebooks for reproducible research")
    print("✓ Machine learning with Scikit-learn")
    print("✓ Interactive scientific computing environment")
    
    print("\nScientific computing workflow:")
    print("1. Data collection and preprocessing with Pandas")
    print("2. Statistical analysis with SciPy and NumPy")
    print("3. Parameter optimization with SciPy")
    print("4. Visualization with Matplotlib")
    print("5. Machine learning with Scikit-learn")
    print("6. Documentation with Jupyter notebooks")
    
    print("\nResearch applications:")
    print("• Computational neuroscience")
    print("• Neural network modeling")
    print("• Brain-computer interface research")
    print("• Neuroinformatics analysis")
    print("• Artificial intelligence research")
    
    print("\nGenerated files:")
    print("• neural_data.csv: Raw neural simulation data")
    print("• analysis_results.json: Statistical analysis results")
    print("• optimization_results.json: Parameter optimization results")
    print("• scientific_summary.md: Complete analysis summary")
    
    return {
        'neural_data': sim_data,
        'analysis_results': analysis_results,
        'optimization_results': optimal_result_dict,
        'notebook_cells': notebook_cells
    }

def create_scientific_summary(brain, world, agent, sim_data, analysis_results, optimal_result, visualizer):
    """Create scientific summary of the analysis"""
    
    print(f"\n{'='*60}")
    print("SCIENTIFIC ANALYSIS SUMMARY")
    print(f"{'='*60}")
    
    # System overview
    print(f"\nNeural System Overview:")
    print(f"   Total neurons: {brain.getTotalNeuronCount():,}")
    print(f"   Excitatory neurons: {brain.getNeuronCount(pynlm.NeuronType.Excitatory)}")
    print(f"   Inhibitory neurons: {brain.getNeuronCount(pynlm.NeuronType.Inhibitory)}")
    print(f"   Simulation steps: {len(sim_data):,}")
    print(f"   Total spikes: {brain.getTotalSpikeCount():,}")
    
    # Performance metrics
    print(f"\nPerformance Metrics:")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Agent energy: {world.getAgentBody().getEnergy():.1f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Development stage: {brain.getDevelopmentalStage()}")
    
    # Analysis highlights
    print(f"\nAnalysis Highlights:")
    if 'temporal' in analysis_results:
        temporal = analysis_results['temporal']
        print(f"   Firing rate trend: {'Increasing' if temporal['firing_rate_trend'] > 0 else 'Decreasing'}")
        print(f"   Stationarity: {'Stationary' if temporal['stationarity_p_value'] > 0.05 else 'Non-stationary'}")
    
    if 'spatial' in analysis_results:
        spatial = analysis_results['spatial']
        print(f"   E/I ratio: {spatial['e_i_ratio']:.2f}")
        print(f"   Population balance: {spatial['balance_score']*100:.1f}%")
    
    # Optimization results
    print(f"\nParameter Optimization:")
    if hasattr(optimal_result, 'x'):
        print(f"   Optimal neuron count: {int(optimal_result.x[0]):,}")
        print(f"   Optimal STDP learning rate: {optimal_result.x[1]:.6f}")
        print(f"   Optimal Hebbian learning rate: {optimal_result.x[2]:.6f}")
        print(f"   Optimal connection density: {optimal_result.x[3]:.4f}")
    
    if hasattr(optimal_result, 'fun'):
        print(f"   Optimal objective value: {optimal_result.fun:.6f}")
    
    # Visualization capabilities
    print(f"\nVisualization Capabilities:")
    print(f"   Neural heatmap: {visualizer.fig.get_size_inches()[0]:.0f}x{visualizer.fig.get_size_inches()[1]:.0f} inches")
    print(f"   Subplot count: 6")
    print(f"   Animation support: Yes")
    
    # Scientific computing stack
    print(f"\nScientific Computing Stack:")
    print(f"   NumPy: {sim_data.memory_usage(deep=True).sum() / (1024**2):.1f} MB memory usage")
    print(f"   Pandas: {len(sim_data):,} data points analyzed")
    print(f"   SciPy: Statistical analysis and optimization")
    print(f"   Matplotlib: Advanced visualization system")
    print(f"   Scikit-learn: Machine learning models")
    print(f"   Jupyter: Interactive notebooks")
    
    # Research implications
    print(f"\nResearch Implications:")
    print(f"   This analysis demonstrates the power of integrating NLM with Python scientific computing")
    print(f"   The framework supports comprehensive neural data analysis from collection to visualization")
    print(f"   Results can be used for hypothesis testing and scientific discovery")
    print(f"   The workflow is reproducible and documented for scientific communication")
    
    print(f"\n{'='*60}")
    print("ANALYSIS COMPLETE - READY FOR RESEARCH PUBLICATION")
    print(f"{'='*60}")
```

## Best Practices for Python Scientific Integration

1. **Start Simple**: Begin with basic data collection and visualization
2. **Leverage Libraries**: Use well-established scientific Python libraries
3. **Maintain Documentation**: Keep code and results well-documented
4. **Use Version Control**: Track changes with git for reproducibility
5. **Implement Error Handling**: Robust error handling for scientific analysis
6. **Test Thoroughly**: Validate results with statistical methods
7. **Optimize Performance**: Use efficient algorithms for large datasets
8. **Create Modular Code**: Separate analysis steps for reuse

## Extensions and Variations

1. **Real-time Integration**: Live data streaming and visualization
2. **Cloud Computing**: Distributed analysis using cloud resources
3. **Machine Learning Pipelines**: End-to-end ML workflows
4. **Statistical Modeling**: Bayesian methods and uncertainty quantification
5. **Interactive Dashboards**: Web-based interactive analysis tools
6. **Automated Reporting**: Generate research reports automatically
7. **Multi-modal Data**: Integrate with other data types (imaging, EEG, etc.)
8. **Collaborative Research**: Share notebooks and analysis results

## Common Pitfalls and Solutions

| Pitfall | Solution |
|---------|----------|
| Large memory usage | Use efficient data structures and streaming |
| Slow computations | Vectorize operations and use compiled code |
| Poor reproducibility | Use version control and containerization |
| Visualization complexity | Start simple and add complexity gradually |
| Data quality issues | Implement robust data validation |
| Code maintenance | Use modular design and clear documentation |

## Further Reading

- [Python for Scientific Computing](https://www.pythonforscientific.com/)
- [Pandas Documentation](https://pandas.pydata.org/docs/)
- [NumPy Documentation](https://numpy.org/doc/)
- [SciPy Documentation](https://docs.scipy.org/doc/scipy/)
- [Matplotlib Gallery](https://matplotlib.org/stable/gallery/index.html)
- [Scikit-learn Documentation](https://scikit-learn.org/stable/)

This example provides a comprehensive foundation for integrating NLM with Python scientific computing, enabling sophisticated neural analysis and research workflows. It demonstrates how to leverage the entire Python scientific ecosystem for neural data analysis, from data collection to visualization and machine learning.