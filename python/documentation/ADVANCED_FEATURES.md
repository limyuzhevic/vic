# NLM Advanced Features Guide

This document provides comprehensive guidance on advanced features and techniques
for the enhanced NLM Python bindings, including simulation management, monitoring,
visualization, and custom implementations.

## 1. Advanced Simulation Management

### 1.1 Continuous Simulation

Run simulations continuously without explicit episode boundaries:

```python
from pynlm import AdvancedBrain

brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    simulation_speed=0.5,
    enable_monitoring=True,
    enable_visualization=True
)

brain.visualization.setup_plotting('matplotlib')

print("Starting continuous simulation (10 seconds max)...")
print("Press Ctrl+C to stop early\n")

try:
    brain.run_continuous(duration=10.0)
    print("\n✅ Continuous simulation completed")
except KeyboardInterrupt:
    print("\n⏹️  Continuous simulation stopped by user")
```

**Key Parameters:**
- `max_steps`: Maximum number of steps (0 for unlimited)
- `duration`: Maximum duration in seconds (0 for unlimited)

### 1.2 Multi-Episode Management

Run multiple episodes and aggregate results:

```python
from pynlm import AdvancedBrain

brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=100,
    enable_monitoring=True
)

# Run multiple episodes
print("Running 5 episodes...")
episode_results = brain.run_episodes(5, max_steps_per_episode=100)

# Aggregate results
total_steps = sum(r['steps_completed'] for r in episode_results)
total_reward = sum(r['total_reward'] for r in episode_results)
success_rate = sum(1 for r in episode_results if r['success']) / len(episode_results)

print(f"Episode Results Summary:")
print(f"  Total episodes: {len(episode_results)}")
print(f"  Total steps: {total_steps}")
print(f"  Average steps per episode: {total_steps/len(episode_results):.1f}")
print(f"  Total reward: {total_reward:.2f}")
print(f"  Success rate: {success_rate*100:.1f}%")
```

### 1.3 Simulation State Management

Manage simulation state and recovery:

```python
from pynlm import AdvancedBrain

brain = AdvancedBrain(verbose=True)
brain.configure_simulation(max_steps=200)

# Start simulation
brain.start_simulation('episode')
print(f"Simulation started: {brain._simulation_running}")

# Run some steps
for step in range(50):
    if brain._simulation_running:
        # Execute step (placeholder for actual simulation)
        brain._brain.step(step) if brain._brain else None
        brain.monitoring.collect_metrics(brain) if brain._config.get('enable_monitoring') else None
    else:
        break

# Pause simulation
brain.pause_simulation()
print("Simulation paused")

# Resume simulation
brain.resume_simulation()
print("Simulation resumed")

# Stop simulation
brain.stop_simulation()
print("Simulation stopped")
```

## 2. Advanced Monitoring and Diagnostics

### 2.1 Custom Metrics Collection

Extend the monitoring system with custom metrics:

```python
from pynlm import AdvancedBrain
from pynlm import MonitoringSystem

class CustomMonitoringSystem(MonitoringSystem):
    def __init__(self, brain):
        super().__init__()
        self.brain = brain
        self.custom_metrics = {}
    
    def collect_custom_metrics(self):
        """Collect custom metrics specific to your simulation."""
        # Example custom metrics
        self.custom_metrics = {
            'learning_progress': self._calculate_learning_progress(),
            'exploration_rate': self._calculate_exploration_rate(),
            'behavior_diversity': self._calculate_behavior_diversity(),
            'adaptation_score': self._calculate_adaptation_score()
        }
    
    def _calculate_learning_progress(self):
        # Custom implementation based on your simulation
        return len(self.brain._episode_history) / 100.0
    
    def _calculate_exploration_rate(self):
        # Calculate based on exploration actions
        return 0.5  # Placeholder
    
    def _calculate_behavior_diversity(self):
        # Calculate diversity of behaviors
        return 0.7  # Placeholder
    
    def _calculate_adaptation_score(self):
        # Calculate adaptation capability
        return 0.8  # Placeholder
    
    def generate_comprehensive_report(self):
        """Generate comprehensive report including custom metrics."""
        base_report = super().generate_report()
        custom_section = f"\n=== Custom Metrics ===\n"
        
        for metric, value in self.custom_metrics.items():
            custom_section += f"{metric.replace('_', ' ').title()}: {value:.3f}\n"
        
        return base_report + custom_section

# Use custom monitoring
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(enable_monitoring=True)

if brain._config.get('enable_monitoring'):
    custom_monitoring = CustomMonitoringSystem(brain)
    brain.monitoring = custom_monitoring
    
    # Run simulation and collect custom metrics
    results = brain.run_episode(max_steps=50)
    
    # Generate comprehensive report
    report = custom_monitoring.generate_comprehensive_report()
    print(report)
```

### 2.2 Performance Optimization

Monitor and optimize simulation performance:

```python
from pynlm import AdvancedBrain
import time

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.performance_data = []
        self.start_time = time.time()
    
    def record_performance(self):
        """Record current performance metrics."""
        current_time = time.time()
        elapsed = current_time - self.start_time
        
        # Calculate performance metrics
        metrics = {
            'timestamp': current_time,
            'elapsed_time': elapsed,
            'steps_per_second': len(self.brain._episode_history) / elapsed if elapsed > 0 else 0,
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': self._get_cpu_usage(),
            'simulation_speed': self.brain._config.get('simulation_speed', 1.0)
        }
        
        self.performance_data.append(metrics)
        return metrics
    
    def analyze_performance(self):
        """Analyze recorded performance data."""
        if not self.performance_data:
            return None
        
        speeds = [m['steps_per_second'] for m in self.performance_data]
        return {
            'average_speed': sum(speeds) / len(speeds),
            'max_speed': max(speeds),
            'min_speed': min(speeds),
            'speed_std': (sum((s - sum(speeds)/len(speeds))**2 for s in speeds) / len(speeds))**0.5,
            'total_steps': len(self.performance_data)
        }
    
    def get_optimization_suggestions(self):
        """Get suggestions for performance optimization."""
        analysis = self.analyze_performance()
        if not analysis:
            return []
        
        suggestions = []
        
        if analysis['speed_std'] > 10:  # High variance
            suggestions.append("High performance variance detected. Consider reducing simulation complexity.")
        
        if analysis['average_speed'] < 1.0:
            suggestions.append("Low simulation speed. Consider increasing simulation_speed parameter.")
        
        if analysis['max_speed'] / analysis['min_speed'] > 5:
            suggestions.append("Large performance variation. Check for inefficient operations.")
        
        return suggestions

# Use performance monitoring
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(enable_monitoring=True)

performance_monitor = PerformanceMonitor(brain)

# Run simulation while monitoring performance
results = brain.run_episode(max_steps=50)

# Analyze performance
analysis = performance_monitor.analyze_performance()
if analysis:
    print(f"Performance Analysis:")
    print(f"  Average speed: {analysis['average_speed']:.2f} steps/sec")
    print(f"  Max speed: {analysis['max_speed']:.2f} steps/sec")
    print(f"  Speed std: {analysis['speed_std']:.2f} steps/sec")
    
    suggestions = performance_monitor.get_optimization_suggestions()
    if suggestions:
        print(f"  Optimization Suggestions:")
        for suggestion in suggestions:
            print(f"    - {suggestion}")
```

## 3. Advanced Visualization

### 3.1 Real-Time Visualization

Create real-time visualizations that update during simulation:

```python
from pynlm import AdvancedBrain
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from IPython.display import display

class RealTimeVisualizer:
    def __init__(self, brain):
        self.brain = brain
        self.fig = None
        self.reward_data = []
        self.step_data = []
        self.neuron_data = []
        
    def setup_animation(self):
        """Setup real-time animation."""
        fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 10))
        self.fig = fig
        
        # Reward plot
        self.reward_line, = ax1.plot([], [], 'g-', label='Reward')
        ax1.set_title('Reward over Time')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Reward')
        ax1.grid(True)
        ax1.legend()
        
        # Steps plot
        self.step_line, = ax2.plot([], [], 'b-', label='Steps')
        ax2.set_title('Simulation Steps')
        ax2.set_xlabel('Time Step')
        ax2.set_ylabel('Steps')
        ax2.grid(True)
        ax2.legend()
        
        # Neurons plot
        self.neuron_line, = ax3.plot([], [], 'r-', label='Active Neurons')
        ax3.set_title('Active Neurons over Time')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Active Neurons')
        ax3.grid(True)
        ax3.legend()
        
        plt.tight_layout()
        return fig
    
    def update_animation(self, frame):
        """Update animation frame."""
        if not self.brain._episode_history:
            return
        
        # Get current state
        state = self.brain._episode_history[-1]
        
        # Update data
        self.reward_data.append(state.reward)
        self.step_data.append(state.step)
        self.neuron_data.append(state.neurons_active)
        
        # Update plots
        self.reward_line.set_data(range(len(self.reward_data)), self.reward_data)
        self.step_line.set_data(range(len(self.step_data)), self.step_data)
        self.neuron_line.set_data(range(len(self.neuron_data)), self.neuron_data)
        
        # Adjust axes
        for ax in self.fig.axes:
            ax.relim()
            ax.autoscale_view()
        
        return self.fig.axes
    
    def start_real_time_visualization(self, num_frames=100):
        """Start real-time visualization."""
        self.setup_animation()
        
        # Create animation
        ani = animation.FuncAnimation(
            self.fig, self.update_animation, 
            frames=num_frames, interval=100, blit=True
        )
        
        plt.show()
        return ani

# Use real-time visualization
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=200,
    enable_visualization=True,
    enable_monitoring=True
)

# Setup real-time visualizer
visualizer = RealTimeVisualizer(brain)
visualizer.setup_animation()

# Run simulation with real-time updates
print("Starting simulation with real-time visualization...")
results = brain.run_episode(max_steps=100, verbose=True)

print("Real-time visualization completed")
```

### 3.2 Multi-Modal Visualization

Combine different visualization modalities:

```python
from pynlm import AdvancedBrain

class MultiModalVisualizer:
    def __init__(self, brain):
        self.brain = brain
        self.visualizations = {}
        self.backend_configs = {
            'matplotlib': {'dpi': 100, 'figsize': (12, 8)},
            'plotly': {'width': 800, 'height': 600},
            'bokeh': {'tools': 'pan,wheel_zoom,box_zoom,reset,save'}
        }
    
    def setup_all_backends(self):
        """Setup all available visualization backends."""
        for backend in ['matplotlib', 'plotly', 'bokeh']:
            if self.brain.visualization.setup_plotting(backend):
                self.visualizations[backend] = self._create_backend_visualizer(backend)
                print(f"✓ {backend} visualization setup successful")
            else:
                print(f"✗ {backend} visualization setup failed")
    
    def _create_backend_visualizer(self, backend):
        """Create visualizer for specific backend."""
        if backend == 'matplotlib':
            return self._create_matplotlib_visualizer()
        elif backend == 'plotly':
            return self._create_plotly_visualizer()
        elif backend == 'bokeh':
            return self._create_bokeh_visualizer()
        return None
    
    def _create_matplotlib_visualizer(self):
        """Create matplotlib-based visualizer."""
        import matplotlib.pyplot as plt
        
        def create_plots(brain):
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('Multi-Modal NLM Simulation Visualization', fontsize=16)
            
            # Plot 1: Reward over time
            if brain._episode_history:
                rewards = [s.reward for s in brain._episode_history]
                axes[0, 0].plot(rewards)
                axes[0, 0].set_title('Reward over Time')
                axes[0, 0].set_xlabel('Step')
                axes[0, 0].set_ylabel('Reward')
                axes[0, 0].grid(True)
            
            # Plot 2: Neuron activity
            if brain._episode_history:
                active_neurons = [s.neurons_active for s in brain._episode_history]
                axes[0, 1].plot(active_neurons)
                axes[0, 1].set_title('Active Neurons')
                axes[0, 1].set_xlabel('Step')
                axes[0, 1].set_ylabel('Active Neurons')
                axes[0, 1].grid(True)
            
            # Plot 3: Episode summary
            if brain._episode_history:
                final_state = brain._episode_history[-1]
                metrics_text = f"Steps: {final_state.step}\n"
                metrics_text += f"Reward: {final_state.reward:.2f}\n"
                metrics_text += f"Success: {final_state.success}\n"
                metrics_text += f"Time: {final_state.time:.2f}"
                
                axes[1, 0].text(0.1, 0.5, metrics_text, 
                              fontsize=12, verticalalignment='center',
                              bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
                axes[1, 0].set_title('Current State')
                axes[1, 0].axis('off')
            
            # Plot 4: Monitoring status
            if brain.monitoring.get_performance_stats():
                stats = brain.monitoring.get_performance_stats()
                status_text = f"Memory Usage: {stats.get('current_memory_usage', 0):.1f}%\n"
                status_text += f"CPU Usage: {stats.get('current_cpu_usage', 0):.1f}%\n"
                status_text += f"Alerts: {stats.get('recent_alerts', 0)}"
                
                axes[1, 1].text(0.1, 0.5, status_text,
                              fontsize=12, verticalalignment='center',
                              bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.5))
                axes[1, 1].set_title('System Status')
                axes[1, 1].axis('off')
            
            plt.tight_layout()
            return fig
        
        return create_plots
    
    def _create_plotly_visualizer(self):
        """Create plotly-based visualizer."""
        try:
            import plotly.graph_objects as go
            from plotly.subplots import make_subplots
            
            def create_plots(brain):
                fig = make_subplots(
                    rows=2, cols=2,
                    subplot_titles=('Reward over Time', 'Active Neurons', 
                                   'Current State', 'System Status'),
                    specs=[[{'secondary_y': False}], [{'secondary_y': False}],
                           [{'secondary_y': False}], [{'secondary_y': False}]]
                )
                
                # Add traces (simplified)
                if brain._episode_history:
                    steps = list(range(len(brain._episode_history)))
                    rewards = [s.reward for s in brain._episode_history]
                    active_neurons = [s.neurons_active for s in brain._episode_history]
                    
                    fig.add_trace(
                        go.Scatter(x=steps, y=rewards, name='Reward'),
                        1, 1
                    )
                    
                    fig.add_trace(
                        go.Scatter(x=steps, y=active_neurons, name='Active Neurons'),
                        1, 2
                    )
                
                fig.update_layout(height=800, title='Plotly Multi-Modal Visualization')
                return fig
            
            return create_plots
        except ImportError:
            return None
    
    def _create_bokeh_visualizer(self):
        """Create bokeh-based visualizer."""
        try:
            from bokeh.plotting import figure
            from bokeh.models import ColumnDataSource, HoverTool
            from bokeh.layout import column, row
            
            def create_plots(brain):
                if not brain._episode_history:
                    return None
                
                # Create data source
                steps = list(range(len(brain._episode_history)))
                rewards = [s.reward for s in brain._episode_history]
                active_neurons = [s.neurons_active for s in brain._episode_history]
                
                source = ColumnDataSource(data={
                    'steps': steps,
                    'rewards': rewards,
                    'active_neurons': active_neurons
                })
                
                # Create figures
                reward_plot = figure(
                    title='Reward over Time', 
                    x_axis_label='Step', 
                    y_axis_label='Reward',
                    width=400, height=300
                )
                reward_plot.line('steps', 'rewards', source=source, color='green')
                
                neuron_plot = figure(
                    title='Active Neurons',
                    x_axis_label='Step',
                    y_axis_label='Active Neurons',
                    width=400, height=300
                )
                neuron_plot.line('steps', 'active_neurons', source=source, color='red')
                
                # Add hover tools
                reward_plot.add_tools(HoverTool(tooltips=[('@steps', '@rewards')]))
                neuron_plot.add_tools(HoverTool(tooltips=[('@steps', '@active_neurons')]))
                
                return column(reward_plot, neuron_plot)
            
            return create_plots
        except ImportError:
            return None
    
    def generate_multi_modal_report(self, brain):
        """Generate report using all available backends."""
        report = "Multi-Modal Visualization Report\n"
        report += "=" * 40 + "\n\n"
        
        for backend, visualizer in self.visualizations.items():
            if visualizer:
                report += f"{backend.upper()} Visualization:\n"
                report += f"  ✓ Available and configured\n"
                
                # Generate sample plot data
                if brain._episode_history:
                    last_state = brain._episode_history[-1]
                    report += f"  Last Step: {last_state.step}\n"
                    report += f"  Last Reward: {last_state.reward:.2f}\n"
                    report += f"  Last Active Neurons: {last_state.neurons_active}\n"
                
                report += "\n"
        
        if not self.visualizations:
            report += "No visualization backends available.\n"
        
        return report

# Use multi-modal visualization
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=100,
    enable_visualization=True,
    enable_monitoring=True
)

# Setup multi-modal visualizer
multi_visualizer = MultiModalVisualizer(brain)
multi_visualizer.setup_all_backends()

# Run simulation
results = brain.run_episode(max_steps=50, verbose=True)

# Generate multi-modal report
report = multi_visualizer.generate_multi_modal_report(brain)
print(report)
```

## 4. Configuration and Customization

### 4.1 Environment-Specific Configuration

Create configurations for different environments:

```python
from pynlm import AdvancedBrain, Config

class EnvironmentConfig:
    """Configuration manager for different environments."""
    
    @staticmethod
    def get_development_config():
        """Get development environment configuration."""
        return Config().configure(
            max_steps=500,
            simulation_speed=1.0,
            enable_monitoring=True,
            monitoring_interval=0.05,
            enable_development=True,
            enable_curiosity=True,
            enable_visualization=True,
            visualization_backend='matplotlib',
            enable_reward_modulation=True
        )
    
    @staticmethod
    def get_production_config():
        """Get production environment configuration."""
        return Config().configure(
            max_steps=2000,
            simulation_speed=1.5,
            enable_monitoring=True,
            monitoring_interval=0.1,
            enable_visualization=True,
            visualization_backend='plotly',
            enable_reward_modulation=True,
            enable_structural_plasticity=True
        )
    
    @staticmethod
    def get_testing_config():
        """Get testing environment configuration."""
        return Config().configure(
            max_steps=50,
            simulation_speed=2.0,
            enable_monitoring=False,
            enable_visualization=False,
            enable_development=False,
            enable_curiosity=False
        )
    
    @staticmethod
    def get_real_time_config():
        """Get real-time environment configuration."""
        return Config().configure(
            max_steps=100,
            simulation_speed=0.5,
            enable_monitoring=True,
            monitoring_interval=0.02,
            enable_visualization=True,
            visualization_backend='bokeh',
            enable_reward_modulation=True
        )

# Use environment-specific configurations
env_configs = EnvironmentConfig()

# Development configuration
dev_config = env_configs.get_development_config()
dev_brain = AdvancedBrain(config=dev_config, verbose=True)
print("Development environment configured")

# Production configuration
prod_config = env_configs.get_production_config()
prod_brain = AdvancedBrain(config=prod_config, verbose=True)
print("Production environment configured")

# Testing configuration
test_config = env_configs.get_testing_config()
test_brain = AdvancedBrain(config=test_config, verbose=True)
print("Testing environment configured")

# Real-time configuration
rt_config = env_configs.get_real_time_config()
rt_brain = AdvancedBrain(config=rt_config, verbose=True)
print("Real-time environment configured")
```

### 4.2 Custom Validation Rules

Implement custom validation rules for specialized requirements:

```python
from pynlm import Config, ValidationError

class SpecializedConfig:
    """Specialized configuration for specific use cases."""
    
    @staticmethod
    def validate_for_neural_network(config_dict):
        """Validate configuration for neural network simulations."""
        errors = []
        
        # Neural network specific validation
        if config_dict.get('max_steps', 0) < 10:
            errors.append("Neural network simulations require at least 10 steps")
        
        if config_dict.get('simulation_speed', 1.0) <= 0:
            errors.append("Simulation speed must be positive for neural network simulations")
        
        # Reward requirements
        if config_dict.get('reward_threshold', 0) < -1000:
            errors.append("Reward threshold too negative (neural networks need reasonable rewards)")
        
        # Monitoring requirements
        if not config_dict.get('enable_monitoring', True):
            errors.append("Monitoring required for neural network simulations")
        
        if errors:
            raise ValidationError(
                "Neural network configuration validation failed",
                errors
            )
        
        return True
    
    @staticmethod
    def validate_for_adaptive_control(config_dict):
        """Validate configuration for adaptive control simulations."""
        errors = []
        
        # Adaptive control requires certain features
        required_features = ['enable_reward_modulation', 'enable_development']
        for feature in required_features:
            if not config_dict.get(feature, False):
                errors.append(f"Adaptive control requires {feature} enabled")
        
        # Learning rate constraints
        if config_dict.get('simulation_speed', 1.0) > 5.0:
            errors.append("Simulation speed too high for adaptive control (max 5.0)")
        
        return len(errors) == 0
    
    @staticmethod
    def validate_for_reinforcement_learning(config_dict):
        """Validate configuration for reinforcement learning."""
        errors = []
        
        # RL specific requirements
        if config_dict.get('max_steps', 0) < 100:
            errors.append("Reinforcement learning requires at least 100 steps")
        
        if not config_dict.get('enable_reward_modulation', True):
            errors.append("Reinforcement learning requires reward modulation")
        
        if not config_dict.get('enable_structural_plasticity', False):
            errors.append("Advanced RL may benefit from structural plasticity")
        
        # Exploration requirements
        exploration_min = config_dict.get('exploration_rate', 0.1)
        if exploration_min < 0.05:
            errors.append("Exploration rate too low (min 0.05 for RL)")
        
        return len(errors) == 0

# Use specialized validation
from pynlm import AdvancedBrain

# Example: Configure for reinforcement learning
rl_config = Config().configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_monitoring=True,
    enable_reward_modulation=True,
    enable_structural_plasticity=True,
    enable_development=True,
    exploration_rate=0.2
)

# Validate the configuration
try:
    SpecializedConfig.validate_for_reinforcement_learning(dict(rl_config._parameters))
    print("✓ Reinforcement learning configuration validated")
    
    rl_brain = AdvancedBrain(config=rl_config, verbose=True)
    print("RL brain created successfully")
    
except ValidationError as e:
    print(f"✗ Configuration validation failed: {e}")

# Example: Configure for adaptive control
ac_config = Config().configure(
    max_steps=500,
    simulation_speed=1.0,
    enable_monitoring=True,
    enable_reward_modulation=True,
    enable_development=True,
    enable_curiosity=True
)

try:
    SpecializedConfig.validate_for_adaptive_control(dict(ac_config._parameters))
    print("✓ Adaptive control configuration validated")
    
    ac_brain = AdvancedBrain(config=ac_config, verbose=True)
    print("Adaptive control brain created successfully")
    
except ValidationError as e:
    print(f"✗ Configuration validation failed: {e}")
```

## 5. Custom Agent Development

### 5.1 Building Custom Agent Classes

Create sophisticated custom agents:

```python
from pynlm import AdvancedBrain, AgentBrain, NLMTypes, Action, SensoryPercept
from typing import List, Dict, Any, Optional
import numpy as np

class HierarchicalAgent(AgentBrain):
    """Agent with hierarchical decision-making capabilities."""
    
    def __init__(self, brain: AdvancedBrain):
        super().__init__(brain)
        self.levels = {
            'reflexive': ReflexiveController(),
            'goal_oriented': GoalOrientedController(),
            'meta_cognitive': MetaCognitiveController()
        }
        self.current_strategy = 'reflexive'
        self.strategy_history: List[str] = []
        self.performance_metrics: Dict[str, List[float]] = {
            'reward': [],
            'success_rate': [],
            'efficiency': []
        }
    
    def process_sensory_input(self, percept: SensoryPercept):
        """Process sensory input through hierarchical controllers."""
        super().process_sensory_input(percept)
        
        # Determine which controller to use based on context
        controller = self._select_appropriate_controller(percept)
        
        # Get action from selected controller
        action = controller.get_action(percept, self._get_context())
        
        # Store strategy usage
        self.strategy_history.append(self.current_strategy)
        
        # Apply action to brain
        self._apply_action_to_brain(action)
    
    def _select_appropriate_controller(self, percept: SensoryPercept) -> Any:
        """Select the most appropriate controller based on percept."""
        # Simple strategy selection based on percept features
        if percept and hasattr(percept, 'vision') and percept.vision:
            # Use goal-oriented controller for visual tasks
            return self.levels['goal_oriented']
        elif np.random.random() < 0.1:  # 10% chance
            # Use meta-cognitive controller for creative exploration
            return self.levels['meta_cognitive']
        else:
            # Use reflexive controller for default responses
            return self.levels['reflexive']
    
    def _get_context(self) -> Dict[str, Any]:
        """Get current context for controller selection."""
        return {
            'current_strategy': self.current_strategy,
            'performance': self._calculate_performance(),
            'time_pressure': self._assess_time_pressure(),
            'uncertainty': self._assess_uncertainty()
        }
    
    def _calculate_performance(self) -> float:
        """Calculate current performance metrics."""
        if not self.performance_metrics['reward']:
            return 0.0
        
        return np.mean(self.performance_metrics['reward'][-10:])
    
    def _assess_time_pressure(self) -> float:
        """Assess time pressure based on simulation state."""
        return min(1.0, len(self.strategy_history) / 100.0)
    
    def _assess_uncertainty(self) -> float:
        """Assess uncertainty in the environment."""
        return np.random.random() * 0.5  # Simplified uncertainty measure
    
    def _apply_action_to_brain(self, action: Action):
        """Apply selected action to brain."""
        # Convert action to brain command
        # This would need implementation based on your brain interface
        pass
    
    def update_meta_cognition(self, reward: float, success: bool):
        """Update meta-cognitive state based on performance."""
        # Record performance
        self.performance_metrics['reward'].append(reward)
        self.performance_metrics['success_rate'].append(1.0 if success else 0.0)
        self.performance_metrics['efficiency'].append(reward / len(self.strategy_history) if self.strategy_history else 1.0)
        
        # Adjust strategy based on performance
        recent_performance = self._calculate_performance()
        
        if recent_performance < 0:
            # Poor performance - try new strategies
            self.current_strategy = self._select_new_strategy()
        elif recent_performance > 5:
            # Excellent performance - stick with current strategy
            self.current_strategy = 'goal_oriented' if recent_performance > 10 else 'reflexive'
    
    def _select_new_strategy(self) -> str:
        """Select a new strategy based on performance history."""
        strategy_performance = {}
        
        for strategy in self.levels.keys():
            strategy_performance[strategy] = self._get_strategy_performance(strategy)
        
        # Select strategy with highest performance
        return max(strategy_performance.items(), key=lambda x: x[1])[0]
    
    def _get_strategy_performance(self, strategy: str) -> float:
        """Get performance of specific strategy."""
        # This would track performance of different strategies
        # Simplified implementation
        return np.random.random() * 2 - 1  # Random performance

# Controller classes (simplified implementations)
class ReflexiveController:
    def get_action(self, percept: SensoryPercept, context: Dict) -> Action:
        # Simple reflexive responses
        return Action(NLMTypes.ActionType.WAIT)

class GoalOrientedController:
    def get_action(self, percept: SensoryPercept, context: Dict) -> Action:
        # Goal-directed behavior
        if np.random.random() < 0.3:
            return Action(NLMTypes.ActionType.EXPLORE)
        return Action(NLMTypes.ActionType.WAIT)

class MetaCognitiveController:
    def get_action(self, percept: SensoryPercept, context: Dict) -> Action:
        # Meta-cognitive decision making
        if context.get('uncertainty', 0) > 0.5:
            return Action(NLMTypes.ActionType.LOOK)
        return Action(NLMTypes.ActionType.WAIT)

# Use hierarchical agent
brain = AdvancedBrain(verbose=True)
hierarchical_agent = HierarchicalAgent(brain)

print("Created Hierarchical Agent with multiple decision-making levels")
print(f"Available strategies: {list(hierarchical_agent.levels.keys())}")
```

### 5.2 Learning and Adaptation

Implement agents that learn and adapt over time:

```python
from pynlm import AdvancedBrain, AgentBrain, NLMTypes
import numpy as np

class AdaptiveLearningAgent(AgentBrain):
    """Agent that learns through experience and adapts behavior."""
    
    def __init__(self, brain: AdvancedBrain):
        super().__init__(brain)
        self.experience_buffer: List[Dict[str, Any]] = []
        self.max_buffer_size = 1000
        
        # Learning parameters
        self.exploration_rate = 1.0
        self.exploration_decay = 0.995
        self.min_exploration = 0.01
        
        # Value estimation
        self.q_table: Dict[str, np.ndarray] = {}
        self.action_space = list(NLMTypes.ActionType)
        self.num_actions = len(self.action_space)
        
        # Adaptation statistics
        self.adaptation_history: List[Dict[str, Any]] = []
    
    def learn_from_experience(self, state: str, action: Action, reward: float, 
                             next_state: str, done: bool):
        """Learn from experience using reinforcement learning."""
        # Store experience
        experience = {
            'state': state,
            'action': action,
            'reward': reward,
            'next_state': next_state,
            'done': done
        }
        
        self.experience_buffer.append(experience)
        
        # Trim buffer if too large
        if len(self.experience_buffer) > self.max_buffer_size:
            self.experience_buffer.pop(0)
        
        # Learn from experience
        self._update_q_table(state, action, reward, next_state, done)
        
        # Record adaptation
        self.adaptation_history.append({
            'step': len(self.adaptation_history),
            'action': action.type.value,
            'reward': reward,
            'exploration_rate': self.exploration_rate,
            'q_table_size': len(self.q_table)
        })
    
    def _update_q_table(self, state: str, action: Action, reward: float,
                       next_state: str, done: bool):
        """Update Q-table using Q-learning."""
        # Initialize Q-tables if needed
        if state not in self.q_table:
            self.q_table[state] = np.zeros(self.num_actions)
        
        if next_state not in self.q_table:
            self.q_table[next_state] = np.zeros(self.num_actions)
        
        # Get action index
        action_idx = self.action_space.index(action.type)
        
        # Q-learning update
        current_q = self.q_table[state][action_idx]
        
        if done:
            target_q = reward
        else:
            target_q = reward + 0.99 * np.max(self.q_table[next_state])
        
        # Update Q-value
        self.q_table[state][action_idx] += 0.1 * (target_q - current_q)
        
        # Decay exploration rate
        self.exploration_rate = max(self.min_exploration, 
                                   self.exploration_rate * self.exploration_decay)
    
    def act(self, state: str) -> Action:
        """Select action using epsilon-greedy policy."""
        # Explore with probability exploration_rate
        if np.random.random() < self.exploration_rate:
            action_idx = np.random.randint(self.num_actions)
        else:
            # Exploit best known action
            if state in self.q_table:
                action_idx = np.argmax(self.q_table[state])
            else:
                action_idx = np.random.randint(self.num_actions)
        
        action_type = self.action_space[action_idx]
        return Action(action_type)
    
    def get_adaptation_summary(self) -> Dict[str, Any]:
        """Get summary of adaptation behavior."""
        if not self.adaptation_history:
            return {}
        
        rewards = [exp['reward'] for exp in self.adaptation_history]
        exploration_rates = [exp['exploration_rate'] for exp in self.adaptation_history]
        
        return {
            'total_experiences': len(self.adaptation_history),
            'average_reward': np.mean(rewards),
            'max_reward': np.max(rewards),
            'min_reward': np.min(rewards),
            'final_exploration_rate': exploration_rates[-1],
            'q_table_states': len(self.q_table),
            'adaptation_trend': self._calculate_adaptation_trend()
        }
    
    def _calculate_adaptation_trend(self) -> str:
        """Calculate adaptation trend over time."""
        if len(self.adaptation_history) < 10:
            return 'insufficient_data'
        
        recent_rewards = [exp['reward'] for exp in self.adaptation_history[-10:]]
        first_rewards = [exp['reward'] for exp in self.adaptation_history[:10]]
        
        recent_avg = np.mean(recent_rewards)
        first_avg = np.mean(first_rewards)
        
        if recent_avg > first_avg + 0.5:
            return 'improving'
        elif recent_avg < first_avg - 0.5:
            return 'declining'
        else:
            return 'stable'
    
    def reset_for_new_episode(self):
        """Reset agent for new episode."""
        # Keep some experiences but clear others
        self.experience_buffer = self.experience_buffer[-100:] if len(self.experience_buffer) > 100 else []
        
        # Reset exploration rate
        self.exploration_rate = 1.0
        
        # Clear adaptation history for new episode
        if len(self.adaptation_history) > 100:
            self.adaptation_history = self.adaptation_history[-100:]

# Use adaptive learning agent
brain = AdvancedBrain(verbose=True)
learning_agent = AdaptiveLearningAgent(brain)

print("Created Adaptive Learning Agent")
print(f"Exploration rate: {learning_agent.exploration_rate}")
print(f"Action space size: {learning_agent.num_actions}")

# Simulate learning episode
state = "initial_state"
action = learning_agent.act(state)
reward = np.random.uniform(-10, 10)
next_state = "next_state"
done = False

learning_agent.learn_from_experience(state, action, reward, next_state, done)

print(f"Taken action: {action.type}")
print(f"Received reward: {reward:.2f}")
print(f"New exploration rate: {learning_agent.exploration_rate:.3f}")
print(f"Q-table states: {learning_agent.get_adaptation_summary()}")
```

## 6. Advanced Error Handling and Recovery

### 6.1 Comprehensive Error Handling

Implement robust error handling and recovery:

```python
from pynlm import AdvancedBrain, ValidationError, NLMError
import traceback

class RobustSimulation:
    """Robust simulation with comprehensive error handling."""
    
    def __init__(self, brain: AdvancedBrain):
        self.brain = brain
        self.error_handlers = {}
        self.recovery_strategies = {}
        self.error_history: List[Dict[str, Any]] = []
    
    def register_error_handler(self, error_type: str, handler: Callable):
        """Register error handler for specific error type."""
        self.error_handlers[error_type] = handler
    
    def register_recovery_strategy(self, error_type: str, strategy: Callable):
        """Register recovery strategy for specific error type."""
        self.recovery_strategies[error_type] = strategy
    
    def run_with_robust_error_handling(self, max_steps: int, max_retries: int = 3):
        """Run simulation with robust error handling and recovery."""
        retry_count = 0
        
        while retry_count < max_retries:
            try:
                print(f"Attempt {retry_count + 1}/{max_retries}")
                
                # Run simulation with callbacks that handle errors
                results = self._run_simulation_with_error_handling(max_steps)
                
                if results.get('success', False):
                    print(f"✅ Simulation completed successfully on attempt {retry_count + 1}")
                    return results
                else:
                    print(f"⚠️  Simulation failed on attempt {retry_count + 1}")
                    retry_count += 1
                    
            except Exception as e:
                retry_count += 1
                error_info = {
                    'attempt': retry_count,
                    'error_type': type(e).__name__,
                    'error_message': str(e),
                    'traceback': traceback.format_exc(),
                    'timestamp': time.time()
                }
                
                self.error_history.append(error_info)
                
                print(f"❌ Error on attempt {retry_count}: {error_info['error_type']}: {error_info['error_message']}")
                
                # Try recovery strategy
                if type(e).__name__ in self.recovery_strategies:
                    print(f"Applying recovery strategy: {type(e).__name__}")
                    recovery_result = self.recovery_strategies[type(e).__name__](e)
                    
                    if recovery_result:
                        print("✅ Recovery successful")
                        continue
                    else:
                        print("❌ Recovery failed")
                else:
                    print(f"⚠️  No recovery strategy for {type(e).__name__}")
        
        # All retries failed
        final_error = NLMError(
            f"Simulation failed after {max_retries} attempts",
            self.error_history
        )
        
        print(f"❌ Simulation failed after {max_retries} attempts")
        raise final_error
    
    def _run_simulation_with_error_handling(self, max_steps: int):
        """Run simulation with error handling callbacks."""
        
        def error_callback(error, brain):
            error_type = type(error).__name__
            print(f"  [ERROR CALLBACK] {error_type}: {error}")
            
            if error_type in self.error_handlers:
                self.error_handlers[error_type](error, brain)
        
        def step_callback(state):
            # Check for potential errors
            if state.step % 100 == 0 and state.neurons_active > 1000:
                raise NLMError(f"High neuron activity at step {state.step}: {state.neurons_active}")
            
            if state.step % 50 == 0 and abs(state.reward) > 100:
                raise NLMError(f"Extreme reward at step {state.step}: {state.reward}")
        
        # Add error handling callbacks
        self.brain.add_callback('on_error', error_callback)
        self.brain.add_callback('step', step_callback)
        
        # Run simulation
        try:
            results = self.brain.run_episode(max_steps=max_steps, verbose=True)
            return results
        except Exception as e:
            # Remove error callbacks to prevent infinite recursion
            self.brain.remove_callback('on_error')
            self.brain.remove_callback('step')
            raise e
    
    def generate_error_report(self) -> str:
        """Generate comprehensive error report."""
        if not self.error_history:
            return "No errors recorded."
        
        report = "Simulation Error Report\n"
        report += "=" * 40 + "\n\n"
        
        for i, error in enumerate(self.error_history):
            report += f"Error {i + 1}: {error['error_type']}\n"
            report += f"  Attempt: {error['attempt']}\n"
            report += f"  Message: {error['error_message']}\n"
            report += f"  Time: {error['timestamp']}\n"
            report += f"  Traceback:\n"
            
            # Limit traceback length for readability
            lines = error['traceback'].split('\n')
            for line in lines[-10:]:  # Last 10 lines
                report += f"    {line}\n"
            
            report += "\n"
        
        report += f"Summary:\n"
        report += f"  Total errors: {len(self.error_history)}\n"
        
        error_types = {}
        for error in self.error_history:
            error_type = error['error_type']
            error_types[error_type] = error_types.get(error_type, 0) + 1
        
        report += f"  Error types:\n"
        for error_type, count in error_types.items():
            report += f"    {error_type}: {count}\n"
        
        return report

# Use robust error handling
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(max_steps=200)

robust_sim = RobustSimulation(brain)

# Register error handlers
robust_sim.register_error_handler('RuntimeError', 
    lambda error, brain: print(f"  🔄 RuntimeError handled: {error}"))

robust_sim.register_recovery_strategy('NLMError',
    lambda error: print(f"  💊 NLMError recovery attempted: {error}") or True)

# Run robust simulation
print("Running simulation with robust error handling...")
try:
    results = robust_sim.run_with_robust_error_handling(max_steps=50, max_retries=2)
    print(f"✅ Final results: {results['success']}, steps={results['steps_completed']}")
except Exception as e:
    print(f"❌ Final simulation failed: {e}")

# Generate error report
report = robust_sim.generate_error_report()
print(f"\n=== Error Report ===\n{report}")
```

### 6.2 Graceful Degradation

Implement graceful degradation when components fail:

```python
from pynlm import AdvancedBrain, Config

class GracefulDegradationSimulation:
    """Simulation with graceful degradation capabilities."""
    
    def __init__(self, brain: AdvancedBrain):
        self.brain = brain
        self.original_config = Config().configure(**brain._config._parameters)
        self.degradation_level = 0
        self.performance_metrics = {}
    
    def degrade_simulation(self, reason: str):
        """Degrade simulation capabilities gracefully when components fail."""
        self.degradation_level += 1
        print(f"🔄 Degradation level {self.degradation_level}: {reason}")
        
        degradation_strategies = [
            self._degrade_visualization,
            self._degrade_monitoring,
            self._degrade_simulation_complexity,
            self._degrade_feature_set
        ]
        
        # Apply degradation strategies
        for strategy in degradation_strategies:
            if strategy():
                print(f"  ✓ Applied {strategy.__name__}")
                break
    
    def _degrade_visualization(self) -> bool:
        """Degrade visualization capabilities."""
        if self.brain._config.get('enable_visualization', False):
            self.brain._config.configure(
                enable_visualization=False,
                visualization_backend='matplotlib'
            )
            return True
        return False
    
    def _degrade_monitoring(self) -> bool:
        """Degrade monitoring capabilities."""
        if self.brain._config.get('enable_monitoring', True):
            self.brain._config.configure(
                enable_monitoring=False,
                monitoring_interval=10.0
            )
            return True
        return False
    
    def _degrade_simulation_complexity(self) -> bool:
        """Degrade simulation complexity."""
        current_steps = self.brain._config.get('max_steps', 100)
        
        if current_steps > 50:
            self.brain._config.configure(max_steps=50)
            return True
        return False
    
    def _degrade_feature_set(self) -> bool:
        """Degrade feature set to basic capabilities."""
        self.brain._config.configure(
            enable_development=False,
            enable_curiosity=False,
            enable_structural_plasticity=False,
            enable_reward_modulation=True
        )
        return True
    
    def can_run_simulation(self) -> bool:
        """Check if simulation can run with current degradation level."""
        min_steps = 10 if self.degradation_level >= 4 else 50 if self.degradation_level >= 3 else 100
        
        return (
            self.brain._config.get('max_steps', 0) >= min_steps and
            self.brain._config.get('enable_monitoring', True) or
            self.degradation_level <= 3
        )
    
    def run_with_graceful_degradation(self, max_steps: int):
        """Run simulation with graceful degradation."""
        if not self.can_run_simulation():
            raise NLMError("Cannot run simulation with current degradation level")
        
        print(f"Starting simulation with graceful degradation (level {self.degradation_level})")
        
        try:
            # Record initial performance
            self.performance_metrics['start_time'] = time.time()
            
            # Run simulation
            results = self.brain.run_episode(max_steps=max_steps, verbose=True)
            
            # Record final performance
            self.performance_metrics['end_time'] = time.time()
            self.performance_metrics['degradation_level'] = self.degradation_level
            self.performance_metrics['results'] = results
            
            print(f"✅ Simulation completed with degradation level {self.degradation_level}")
            return results
            
        except Exception as e:
            print(f"❌ Simulation failed: {e}")
            
            # Try to recover with further degradation
            if self.degradation_level < 5:
                self.degrade_simulation(f"Simulation failed: {e}")
                return self.run_with_graceful_degradation(max_steps)
            else:
                raise NLMError("Maximum degradation level reached. Cannot continue simulation.")
    
    def get_degradation_report(self) -> str:
        """Generate degradation report."""
        report = f"Graceful Degradation Report\n"
        report += f"Degradation Level: {self.degradation_level}\n"
        report += f"Original Config: {self.original_config.summary()}\n"
        report += f"Current Config: {self.brain._config.summary()}\n"
        
        if self.performance_metrics:
            report += f"Performance:\n"
            if 'start_time' in self.performance_metrics:
                duration = self.performance_metrics['end_time'] - self.performance_metrics['start_time']
                report += f"  Duration: {duration:.2f}s\n"
            
            if 'results' in self.performance_metrics:
                results = self.performance_metrics['results']
                report += f"  Steps: {results['steps_completed']}\n"
                report += f"  Success: {results['success']}\n"
                report += f"  Total Reward: {results['total_reward']:.2f}\n"
        
        return report

# Use graceful degradation
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(max_steps=500, enable_visualization=True)

degradation_sim = GracefulDegradationSimulation(brain)

print("Testing graceful degradation...")

# Simulate normal operation
print("1. Running normal simulation...")
results1 = degradation_sim.run_with_graceful_degradation(max_steps=100)
print(f"   Result: {results1['success']}, steps={results1['steps_completed']}")

# Simulate component failure
print("\n2. Simulating component failure...")
brain._config.configure(max_steps=50)  # Reduce max_steps to test degradation

results2 = degradation_sim.run_with_graceful_degradation(max_steps=30)
print(f"   Result: {results2['success']}, steps={results2['steps_completed']}")

# Generate degradation report
report = degradation_sim.get_degradation_report()
print(f"\n=== Degradation Report ===\n{report}")
```

## 7. Performance Optimization

### 7.1 Simulation Performance Monitoring

Monitor and optimize simulation performance:

```python
from pynlm import AdvancedBrain
import time
import psutil
import numpy as np

class PerformanceOptimizer:
    """Performance optimization for NLM simulations."""
    
    def __init__(self, brain: AdvancedBrain):
        self.brain = brain
        self.performance_data = []
        self.optimization_history: List[Dict[str, Any]] = []
        self.start_time = time.time()
    
    def collect_performance_data(self):
        """Collect comprehensive performance data."""
        current_time = time.time()
        elapsed = current_time - self.start_time
        
        # System metrics
        process = psutil.Process()
        memory_info = process.memory_info()
        
        performance_data = {
            'timestamp': current_time,
            'elapsed_time': elapsed,
            'simulation_speed': self.brain._config.get('simulation_speed', 1.0),
            'max_steps': self.brain._config.get('max_steps', 100),
            'cpu_percent': psutil.cpu_percent(interval=0.1),
            'memory_rss_mb': memory_info.rss / 1024 / 1024,
            'memory_vms_mb': memory_info.vms / 1024 / 1024,
            'memory_percent': psutil.virtual_memory().percent,
            'active_neurons': self.brain.get_active_neuron_count() if hasattr(self.brain, 'get_active_neuron_count') else 0,
            'total_spikes': self.brain.get_total_spike_count() if hasattr(self.brain, 'get_total_spike_count') else 0,
            'simulation_running': self.brain._simulation_running
        }
        
        self.performance_data.append(performance_data)
        return performance_data
    
    def analyze_performance(self) -> Dict[str, Any]:
        """Analyze performance data and identify optimization opportunities."""
        if len(self.performance_data) < 10:
            return {'status': 'insufficient_data'}
        
        recent_data = self.performance_data[-10:]
        
        # Calculate metrics
        cpu_usage = [d['cpu_percent'] for d in recent_data]
        memory_usage = [d['memory_percent'] for d in recent_data]
        simulation_speeds = [d['simulation_speed'] for d in recent_data]
        step_rates = [d['active_neurons'] / (d['elapsed_time'] / len(recent_data)) 
                     for d in recent_data if d['elapsed_time'] > 0]
        
        analysis = {
            'status': 'analyzed',
            'cpu_usage': {
                'average': np.mean(cpu_usage),
                'maximum': np.max(cpu_usage),
                'minimum': np.min(cpu_usage),
                'std': np.std(cpu_usage)
            },
            'memory_usage': {
                'average': np.mean(memory_usage),
                'maximum': np.max(memory_usage),
                'minimum': np.min(memory_usage),
                'std': np.std(memory_usage)
            },
            'simulation_speed': {
                'average': np.mean(simulation_speeds),
                'maximum': np.max(simulation_speeds),
                'minimum': np.min(simulation_speeds),
                'std': np.std(simulation_speeds)
            },
            'performance_indicators': {
                'cpu_efficiency': 1.0 - (np.mean(cpu_usage) / 100.0),
                'memory_efficiency': 1.0 - (np.mean(memory_usage) / 100.0),
                'stability_index': 1.0 / (1.0 + np.std(cpu_usage) + np.std(memory_usage))
            },
            'recommendations': self._generate_optimization_recommendations(recent_data)
        }
        
        return analysis
    
    def _generate_optimization_recommendations(self, data: List[Dict[str, Any]]) -> List[str]:
        """Generate optimization recommendations based on performance data."""
        recommendations = []
        
        # CPU recommendations
        avg_cpu = np.mean([d['cpu_percent'] for d in data])
        if avg_cpu > 80:
            recommendations.append(
                "High CPU usage detected. Consider reducing simulation_speed or "
                "optimizing neural network architecture."
            )
        
        # Memory recommendations
        avg_memory = np.mean([d['memory_percent'] for d in data])
        if avg_memory > 85:
            recommendations.append(
                "High memory usage detected. Consider reducing model complexity "
                "or enabling memory-efficient simulation modes."
            )
        
        # Simulation speed recommendations
        speeds = [d['simulation_speed'] for d in data]
        if np.std(speeds) > 0.5:
            recommendations.append(
                "Variable simulation speed detected. Consider stabilizing "
                "simulation parameters or implementing adaptive speed control."
            )
        
        # Step rate recommendations
        if len(data) >= 5:
            recent_rates = [d['active_neurons'] / (d['elapsed_time'] / 5) 
                          for d in data[-5:] if d['elapsed_time'] > 0]
            if np.mean(recent_rates) < 10:
                recommendations.append(
                    "Low neuron activity detected. Consider adjusting network "
                    "parameters or increasing simulation complexity."
                )
        
        # Stability recommendations
        if len(recommendations) == 0:
            recommendations.append("Performance is within optimal ranges. Consider maintaining current settings.")
        
        return recommendations
    
    def optimize_configuration(self) -> Dict[str, Any]:
        """Optimize simulation configuration based on performance data."""
        analysis = self.analyze_performance()
        
        if analysis['status'] != 'analyzed':
            return {'status': 'cannot_optimize', 'reason': analysis['status']}
        
        optimization_result = {
            'status': 'optimized',
            'original_config': dict(self.brain._config._parameters),
            'optimized_config': {},
            'improvements': [],
            'recommendations': analysis['recommendations']
        }
        
        # Optimize CPU usage
        if analysis['cpu_usage']['average'] > 75:
            optimization_result['optimized_config']['simulation_speed'] = max(
                0.5, analysis['cpu_usage']['average'] / 100.0 * 2.0
            )
            optimization_result['improvements'].append(
                f"Reduced simulation_speed from {self.brain._config.get('simulation_speed', 1.0):.2f} to "
                f"{optimization_result['optimized_config']['simulation_speed']:.2f} to reduce CPU load"
            )
        
        # Optimize memory usage
        if analysis['memory_usage']['average'] > 80:
            optimization_result['optimized_config']['enable_visualization'] = False
            optimization_result['improvements'].append(
                "Disabled visualization to reduce memory usage"
            )
        
        # Optimize step rate
        if analysis.get('performance_indicators', {}).get('cpu_efficiency', 1.0) < 0.7:
            optimization_result['optimized_config']['enable_monitoring'] = False
            optimization_result['improvements'].append(
                "Disabled monitoring to improve performance"
            )
        
        # Apply optimizations
        if optimization_result['optimized_config']:
            self.brain.configure_simulation(**optimization_result['optimized_config'])
        
        return optimization_result
    
    def run_optimized_simulation(self, max_steps: int) -> Dict[str, Any]:
        """Run simulation with performance optimization."""
        # Collect initial performance data
        self.collect_performance_data()
        
        # Optimize configuration
        optimization_result = self.optimize_configuration()
        print(f"Configuration optimized: {optimization_result['status']}")
        
        if optimization_result['improvements']:
            print("Optimizations applied:")
            for improvement in optimization_result['improvements']:
                print(f"  ✓ {improvement}")
        
        # Run simulation with optimized configuration
        results = self.brain.run_episode(max_steps=max_steps, verbose=True)
        
        # Collect final performance data
        self.collect_performance_data()
        
        # Analyze final performance
        final_analysis = self.analyze_performance()
        
        return {
            'results': results,
            'optimization_result': optimization_result,
            'final_analysis': final_analysis
        }

# Use performance optimization
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=200,
    simulation_speed=1.5,
    enable_visualization=True,
    enable_monitoring=True
)

print("Starting simulation with performance optimization...")

optimizer = PerformanceOptimizer(brain)

# Run optimized simulation
optimization_results = optimizer.run_optimized_simulation(max_steps=100)

print(f"\n=== Optimization Results ===")
print(f"Status: {optimization_results['optimization_result']['status']}")
print(f"Impressions: {len(optimization_results['optimization_result']['improvements'])}")

if optimization_results['optimization_result']['improvements']:
    print("Optimizations:")
    for improvement in optimization_results['optimization_result']['improvements']:
        print(f"  ✓ {improvement}")

final_analysis = optimization_results['final_analysis']
print(f"\nFinal Performance Analysis:")
print(f"  CPU Usage: {final_analysis['cpu_usage']['average']:.1f}% (std: {final_analysis['cpu_usage']['std']:.1f}%)")
print(f"  Memory Usage: {final_analysis['memory_usage']['average']:.1f}% (std: {final_analysis['memory_usage']['std']:.1f}%)")
print(f"  Simulation Speed: {final_analysis['simulation_speed']['average']:.2f} (std: {final_analysis['simulation_speed']['std']:.2f})")

# Show recommendations
recommendations = optimization_results['optimization_result']['recommendations']
if recommendations:
    print(f"\nRecommendations:")
    for recommendation in recommendations:
        print(f"  {recommendation}")
```

## 8. Advanced Configuration Management

### 8.1 Configuration Templates

Create configuration templates for common use cases:

```python
from pynlm import Config, AdvancedBrain

class ConfigurationTemplates:
    """Configuration templates for common simulation scenarios."""
    
    @staticmethod
    def create_research_template() -> Config:
        """Create research template configuration."""
        return Config().configure(
            # Research requirements
            max_steps=2000,
            simulation_speed=1.0,
            enable_monitoring=True,
            monitoring_interval=0.05,
            
            # Advanced features for research
            enable_development=True,
            enable_curiosity=True,
            enable_structural_plasticity=True,
            enable_reward_modulation=True,
            
            # Visualization for analysis
            enable_visualization=True,
            visualization_backend='plotly',
            
            # Data collection
            collect_detailed_metrics=True,
            export_data_format='csv'
        )
    
    @staticmethod
    def create_production_template() -> Config:
        """Create production template configuration."""
        return Config().configure(
            # Production requirements
            max_steps=5000,
            simulation_speed=1.2,
            enable_monitoring=True,
            monitoring_interval=0.1,
            
            # Core features
            enable_reward_modulation=True,
            enable_development=True,
            enable_visualization=True,
            visualization_backend='matplotlib',
            
            # Performance tuning
            enable_optimization=True,
            memory_efficient_mode=True,
            parallel_processing=True
        )
    
    @staticmethod
    def create_real_time_template() -> Config:
        """Create real-time template configuration."""
        return Config().configure(
            # Real-time requirements
            max_steps=100,
            simulation_speed=0.5,
            enable_monitoring=True,
            monitoring_interval=0.01,
            
            # Real-time features
            enable_visualization=True,
            visualization_backend='bokeh',
            enable_reward_modulation=True,
            
            # Low latency
            prioritize_response_time=True,
            background_processing=False,
            streaming_mode=True
        )
    
    @staticmethod
    def create_testing_template() -> Config:
        """Create testing template configuration."""
        return Config().configure(
            # Testing requirements
            max_steps=50,
            simulation_speed=2.0,
            enable_monitoring=False,
            enable_visualization=False,
            
            # Fast testing
            skip_complex_features=True,
            simplified_model=True,
            minimal_dependencies=True
        )
    
    @staticmethod
    def create_custom_template(self, custom_params: Dict[str, Any]) -> Config:
        """Create custom template with specific parameters."""
        # Base research template
        config = Config().configure(
            max_steps=1000,
            simulation_speed=1.0,
            enable_monitoring=True,
            enable_visualization=False
        )
        
        # Apply custom parameters
        config.configure(**custom_params)
        
        return config
    
    @staticmethod
    def validate_template_consistency(config: Config) -> List[str]:
        """Validate that configuration is consistent and valid."""
        errors = []
        
        # Check required parameters
        required_params = ['max_steps', 'simulation_speed']
        for param in required_params:
            if not config.has(param):
                errors.append(f"Missing required parameter: {param}")
        
        # Check parameter ranges
        if config.has('max_steps') and config.get('max_steps', 0) <= 0:
            errors.append("max_steps must be positive")
        
        if config.has('simulation_speed') and config.get('simulation_speed', 0) <= 0:
            errors.append("simulation_speed must be positive")
        
        if config.has('monitoring_interval') and config.get('monitoring_interval', 0) <= 0:
            errors.append("monitoring_interval must be positive")
        
        # Check parameter consistency
        if (config.has('enable_visualization') and config.has('visualization_backend') and
            config.get('enable_visualization') and not config.get('visualization_backend')):
            errors.append("visualization_backend must be specified when enable_visualization is True")
        
        if (config.has('enable_monitoring') and config.has('monitoring_interval') and
            config.get('enable_monitoring') and config.get('monitoring_interval', 0) < 0.01):
            errors.append("monitoring_interval should not be less than 0.01 for real-time monitoring")
        
        return errors

# Use configuration templates
brain = AdvancedBrain(verbose=True)

print("Creating configuration templates...")

# Research template
research_config = ConfigurationTemplates.create_research_template()
research_brain = AdvancedBrain(config=research_config, verbose=True)
print(f"✓ Research template: {research_config.get('max_steps')} steps, "
      f"visualization={research_config.get('enable_visualization')}")

# Production template
production_config = ConfigurationTemplates.create_production_template()
production_brain = AdvancedBrain(config=production_config, verbose=True)
print(f"✓ Production template: {production_config.get('max_steps')} steps, "
      f"optimization={production_config.get('enable_optimization')}")

# Real-time template
realtime_config = ConfigurationTemplates.create_real_time_template()
realtime_brain = AdvancedBrain(config=realtime_config, verbose=True)
print(f"✓ Real-time template: {realtime_config.get('max_steps')} steps, "
      f"monitoring_interval={realtime_config.get('monitoring_interval')}")

# Testing template
testing_config = ConfigurationTemplates.create_testing_template()
testing_brain = AdvancedBrain(config=testing_config, verbose=True)
print(f"✓ Testing template: {testing_config.get('max_steps')} steps, "
      f"minimal_features={testing_config.get('simplified_model')}")

# Validate templates
print("\nValidating template consistency...")
for template_name, template_config in [
    ('Research', research_config),
    ('Production', production_config),
    ('Real-time', realtime_config),
    ('Testing', testing_config)
]:
    errors = ConfigurationTemplates.validate_template_consistency(template_config)
    if errors:
        print(f"  ✗ {template_name} template validation failed: {errors}")
    else:
        print(f"  ✓ {template_name} template validation passed")
```

### 8.2 Configuration Inheritance and Override

Implement configuration inheritance and override mechanisms:

```python
from pynlm import Config, AdvancedBrain

class ConfigurationManager:
    """Configuration manager with inheritance and override support."""
    
    def __init__(self):
        self.config_registry: Dict[str, Config] = {}
        self.inheritance_map: Dict[str, List[str]] = {}
        self.override_rules: Dict[str, Dict[str, Any]] = {}
    
    def register_config(self, name: str, config: Config, inherit_from: List[str] = None):
        """Register a configuration with optional inheritance."""
        self.config_registry[name] = config
        
        if inherit_from:
            self.inheritance_map[name] = inherit_from
        
        # Apply inheritance
        if inherit_from:
            for parent_name in inherit_from:
                if parent_name in self.config_registry:
                    self._inherit_from_parent(name, parent_name)
    
    def _inherit_from_parent(self, child_name: str, parent_name: str):
        """Inherit configuration from parent."""
        child_config = self.config_registry[child_name]
        parent_config = self.config_registry[parent_name]
        
        # Copy parameters from parent (child can override)
        for key in parent_config.get_keys():
            if not child_config.has(key):  # Only inherit if child doesn't have it
                child_config.configure(**{key: parent_config.get(key)})
    
    def override_config(self, config_name: str, override_params: Dict[str, Any]):
        """Override configuration parameters."""
        if config_name not in self.config_registry:
            raise ValueError(f"Configuration not found: {config_name}")
        
        # Apply override rules
        for key, value in override_params.items():
            if key in self.override_rules:
                # Apply override rule
                rule = self.override_rules[key]
                if rule.get('validate', True):
                    # Validate override
                    errors = rule.get('validation', lambda x: [])(value)
                    if errors:
                        raise ValidationError(f"Override validation failed for {key}: {errors}")
                
                # Apply constraints
                constraints = rule.get('constraints', {})
                if constraints.get('min_value') is not None and value < constraints['min_value']:
                    raise ValueError(f"Override value for {key} below minimum: {value} < {constraints['min_value']}")
                
                if constraints.get('max_value') is not None and value > constraints['max_value']:
                    raise ValueError(f"Override value for {key} above maximum: {value} > {constraints['max_value']}")
                
                if constraints.get('allowed_values') and value not in constraints['allowed_values']:
                    raise ValueError(f"Override value for {key} not in allowed values: {value}")
            
            # Apply override
            self.config_registry[config_name].configure(**{key: value})
    
    def get_effective_config(self, config_name: str) -> Config:
        """Get effective configuration considering inheritance and overrides."""
        if config_name not in self.config_registry:
            raise ValueError(f"Configuration not found: {config_name}")
        
        return self.config_registry[config_name]
    
    def create_brain_from_config(self, config_name: str, brain_kwargs: Dict[str, Any] = None) -> AdvancedBrain:
        """Create brain from configuration with additional brain parameters."""
        effective_config = self.get_effective_config(config_name)
        
        # Merge brain parameters with configuration
        if brain_kwargs:
            # Create a new config with brain parameters
            final_config = Config()
            final_config.configure(**dict(effective_config._parameters))
            final_config.configure(**brain_kwargs)
        else:
            final_config = effective_config
        
        return AdvancedBrain(config=final_config, verbose=False)

# Use configuration manager
brain = AdvancedBrain(verbose=True)

# Create configuration manager
config_manager = ConfigurationManager()

# Define base configuration
base_config = Config().configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_monitoring=True,
    enable_visualization=False
)

# Define experiment-specific configuration
experiment_config = Config().configure(
    max_steps=500,
    simulation_speed=1.5,
    enable_curiosity=True,
    enable_structural_plasticity=True
)

# Register configurations with inheritance
config_manager.register_config('base', base_config)
config_manager.register_config('experiment', experiment_config, inherit_from=['base'])

# Apply overrides
config_manager.override_config('experiment', {
    'max_steps': 750,  # Override max_steps
    'enable_visualization': True,  # Add visualization
    'reward_threshold': 0.5  # Add new parameter
})

# Create brain from configuration
experiment_brain = config_manager.create_brain_from_config(
    'experiment',
    {'verbose': True, 'enable_optimization': True}
)

print("Configuration manager example completed")
print(f"Base config: {base_config.get('max_steps')} steps")
print(f"Experiment config: {experiment_brain._config.get('max_steps')} steps")
print(f"Experiment config has visualization: {experiment_brain._config.get('enable_visualization')}")
print(f"Experiment config has curiosity: {experiment_brain._config.get('enable_curiosity')}")
print(f"Experiment config has optimization: {experiment_brain._config.get('enable_optimization')}")
```

## 9. Integration and Extensibility

### 9.1 Plugin Architecture

Implement plugin architecture for extending NLM functionality:

```python
from pynlm import AdvancedBrain, Config
from abc import ABC, abstractmethod
from typing import Dict, Any, List

class Plugin(ABC):
    """Base class for NLM plugins."""
    
    @abstractmethod
    def initialize(self, brain: AdvancedBrain) -> bool:
        """Initialize the plugin with brain instance. Return True if successful."""
        pass
    
    @abstractmethod
    def process_sensory_input(self, percept: Any) -> Any:
        """Process sensory input through plugin."""
        pass
    
    @abstractmethod
    def decode_motor_command(self) -> Any:
        """Decode brain output to motor command through plugin."""
        pass
    
    @abstractmethod
    def get_plugin_info(self) -> Dict[str, Any]:
        """Get plugin information and metadata."""
        pass

class SensoryEnhancementPlugin(Plugin):
    """Plugin for enhancing sensory processing."""
    
    def __init__(self):
        self.enhancement_level = 1.0
        self.processing_history: List[Dict[str, Any]] = []
    
    def initialize(self, brain: AdvancedBrain) -> bool:
        """Initialize sensory enhancement plugin."""
        try:
            brain.configure_simulation(
                enable_advanced_sensing=True,
                sensing_enhancement_level=self.enhancement_level
            )
            return True
        except Exception as e:
            print(f"Failed to initialize sensory enhancement plugin: {e}")
            return False
    
    def process_sensory_input(self, percept: Any) -> Any:
        """Enhance sensory input processing."""
        # Apply enhancement to percept
        enhanced_percept = percept
        
        # Add enhancement metadata
        enhanced_percept.plugin_metadata = {
            'plugin_name': 'SensoryEnhancementPlugin',
            'enhancement_level': self.enhancement_level,
            'processing_timestamp': time.time()
        }
        
        # Store processing history
        self.processing_history.append({
            'input': percept,
            'output': enhanced_percept,
            'timestamp': time.time()
        })
        
        return enhanced_percept
    
    def decode_motor_command(self) -> Any:
        """Decode motor command with enhancement."""
        # Apply enhancement to motor commands
        # This would interface with brain motor output
        return "enhanced_motor_command"
    
    def get_plugin_info(self) -> Dict[str, Any]:
        """Get plugin information."""
        return {
            'name': 'SensoryEnhancementPlugin',
            'version': '1.0.0',
            'description': 'Enhances sensory input processing for better agent perception',
            'capabilities': ['sensory_enhancement', 'noise_reduction', 'feature_extraction'],
            'configurable': True
        }

class RewardPredictionPlugin(Plugin):
    """Plugin for reward prediction and credit assignment."""
    
    def __init__(self):
        self.prediction_model = {}
        self.credit_assignment_window = 10
        self.prediction_history: List[Dict[str, Any]] = []
    
    def initialize(self, brain: AdvancedBrain) -> bool:
        """Initialize reward prediction plugin."""
        try:
            brain.configure_simulation(
                enable_reward_prediction=True,
                prediction_window=self.credit_assignment_window
            )
            return True
        except Exception as e:
            print(f"Failed to initialize reward prediction plugin: {e}")
            return False
    
    def process_sensory_input(self, percept: Any) -> Any:
        """Process sensory input for reward prediction."""
        # Extract features relevant to reward prediction
        # This would depend on the specific percept structure
        features = self._extract_reward_features(percept)
        
        # Predict reward based on features
        predicted_reward = self._predict_reward(features)
        
        # Store prediction
        self.prediction_history.append({
            'features': features,
            'predicted_reward': predicted_reward,
            'timestamp': time.time()
        })
        
        return percept
    
    def decode_motor_command(self) -> Any:
        """Decode motor command with reward consideration."""
        # Apply reward-based modulation to motor commands
        # This would interface with brain motor output
        return "reward_weighted_motor_command"
    
    def get_plugin_info(self) -> Dict[str, Any]:
        """Get plugin information."""
        return {
            'name': 'RewardPredictionPlugin',
            'version': '1.0.0',
            'description': 'Predicts rewards and assigns credit for learning',
            'capabilities': ['reward_prediction', 'credit_assignment', 'value_estimation'],
            'configurable': True
        }
    
    def _extract_reward_features(self, percept: Any) -> Dict[str, Any]:
        """Extract features relevant to reward prediction."""
        # This would be customized based on percept structure
        return {
            'novelty': np.random.random(),
            'difficulty': np.random.random(),
            'relevance': np.random.random(),
            'immediacy': np.random.random()
        }
    
    def _predict_reward(self, features: Dict[str, Any]) -> float:
        """Predict reward based on features."""
        # Simple linear combination of features
        return (features['novelty'] * 0.3 + 
                features['difficulty'] * 0.4 + 
                features['relevance'] * 0.2 + 
                features['immediacy'] * 0.1)

class VisualizationPlugin(Plugin):
    """Plugin for enhanced visualization."""
    
    def __init__(self):
        self.visualization_config = {}
        self.plot_generators: List[Callable] = []
    
    def initialize(self, brain: AdvancedBrain) -> bool:
        """Initialize visualization plugin."""
        try:
            brain.configure_simulation(
                enable_advanced_visualization=True,
                visualization_theme='default'
            )
            return True
        except Exception as e:
            print(f"Failed to initialize visualization plugin: {e}")
            return False
    
    def process_sensory_input(self, percept: Any) -> Any:
        """Process sensory input for visualization purposes."""
        # Generate visualization data
        viz_data = self._generate_visualization_data(percept)
        
        # Store visualization data
        if not hasattr(percept, 'visualization_data'):
            percept.visualization_data = []
        
        percept.visualization_data.append(viz_data)
        
        return percept
    
    def decode_motor_command(self) -> Any:
        """Decode motor command with visualization."""
        # Generate visualization for motor command
        return "visualized_motor_command"
    
    def get_plugin_info(self) -> Dict[str, Any]:
        """Get plugin information."""
        return {
            'name': 'VisualizationPlugin',
            'version': '1.0.0',
            'description': 'Enhances visualization capabilities for simulation analysis',
            'capabilities': ['data_visualization', 'plot_generation', 'real_time_viz'],
            'configurable': True
        }
    
    def _generate_visualization_data(self, percept: Any) -> Dict[str, Any]:
        """Generate visualization data from percept."""
        return {
            'timestamp': time.time(),
            'features': [np.random.random() for _ in range(10)],
            'visualization_type': 'sensory_enhancement'
        }

class NLMPluginManager:
    """Manager for NLM plugins with lifecycle management."""
    
    def __init__(self):
        self.plugins: Dict[str, Plugin] = {}
        self.plugin_lifecycle: Dict[str, Dict[str, Any]] = {}
    
    def register_plugin(self, plugin: Plugin) -> str:
        """Register a plugin and return its name."""
        plugin_name = plugin.get_plugin_info()['name']
        self.plugins[plugin_name] = plugin
        self.plugin_lifecycle[plugin_name] = {
            'status': 'uninitialized',
            'initialization_time': None,
            'last_activity': None,
            'configuration': {}
        }
        return plugin_name
    
    def initialize_plugin(self, plugin_name: str, brain: AdvancedBrain) -> bool:
        """Initialize a specific plugin."""
        if plugin_name not in self.plugins:
            print(f"Plugin not found: {plugin_name}")
            return False
        
        plugin = self.plugins[plugin_name]
        try:
            success = plugin.initialize(brain)
            
            if success:
                self.plugin_lifecycle[plugin_name]['status'] = 'initialized'
                self.plugin_lifecycle[plugin_name]['initialization_time'] = time.time()
                print(f"✓ Plugin {plugin_name} initialized successfully")
            else:
                self.plugin_lifecycle[plugin_name]['status'] = 'initialization_failed'
                print(f"✗ Plugin {plugin_name} initialization failed")
            
            return success
        except Exception as e:
            self.plugin_lifecycle[plugin_name]['status'] = 'error'
            print(f"✗ Plugin {plugin_name} initialization error: {e}")
            return False
    
    def activate_plugins(self, brain: AdvancedBrain) -> List[str]:
        """Activate all registered plugins."""
        activated_plugins = []
        
        for plugin_name, plugin in self.plugins.items():
            print(f"Initializing plugin: {plugin_name}")
            if self.initialize_plugin(plugin_name, brain):
                activated_plugins.append(plugin_name)
        
        return activated_plugins
    
    def get_plugin_status(self) -> Dict[str, Any]:
        """Get status of all plugins."""
        return {
            'total_plugins': len(self.plugins),
            'active_plugins': len([p for p in self.plugin_lifecycle.values() if p['status'] == 'initialized']),
            'failed_plugins': len([p for p in self.plugin_lifecycle.values() if p['status'] != 'initialized']),
            'plugins': self.plugin_lifecycle
        }

# Use plugin architecture
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=100,
    enable_visualization=True,
    enable_monitoring=True
)

# Create plugin manager
plugin_manager = NLMPluginManager()

# Create and register plugins
sensory_plugin = SensoryEnhancementPlugin()
reward_plugin = RewardPredictionPlugin()
viz_plugin = VisualizationPlugin()

plugin_manager.register_plugin(sensory_plugin)
plugin_manager.register_plugin(reward_plugin)
plugin_manager.register_plugin(viz_plugin)

# Activate all plugins
activated = plugin_manager.activate_plugins(brain)

print(f"✓ Activated {len(activated)} plugins: {', '.join(activated)}")

# Get plugin status
status = plugin_manager.get_plugin_status()
print(f"\nPlugin Status:")
print(f"  Total: {status['total_plugins']}")
print(f"  Active: {status['active_plugins']}")
print(f"  Failed: {status['failed_plugins']}")

# Simulate plugin usage
print("\nSimulating plugin usage...")
sample_percept = type('SamplePercept', (), {})()

# Process through sensory plugin
processed_percept = sensory_plugin.process_sensory_input(sample_percept)
print(f"✓ Processed through {sensory_plugin.get_plugin_info()['name']}")

# Decode motor command through reward plugin
motor_command = reward_plugin.decode_motor_command()
print(f"✓ Motor command through {reward_plugin.get_plugin_info()['name']}: {motor_command}")

# Generate visualization through viz plugin
viz_command = viz_plugin.decode_motor_command()
print(f"✓ Visualization through {viz_plugin.get_plugin_info()['name']}: {viz_command}")
```

### 9.2 Custom Agent Framework

Create custom agent framework with advanced capabilities:

```python
from pynlm import AdvancedBrain, AgentBrain, NLMTypes
from abc import ABC, abstractmethod
from typing import Dict, Any, List, Optional
import numpy as np

class AgentCapability(ABC):
    """Base class for agent capabilities."""
    
    @abstractmethod
    def activate(self, agent: 'AdvancedAgent') -> bool:
        """Activate the capability. Return True if successful."""
        pass
    
    @abstractmethod
    def deactivate(self, agent: 'AdvancedAgent') -> bool:
        """Deactivate the capability. Return True if successful."""
        pass
    
    @abstractmethod
    def get_capability_info(self) -> Dict[str, Any]:
        """Get capability information."""
        pass

class PerceptualCapability(AgentCapability):
    """Capability for perceptual processing."""
    
    def __init__(self, enhancement_level: float = 1.0):
        self.enhancement_level = enhancement_level
        self.processing_accuracy = 0.8
    
    def activate(self, agent: 'AdvancedAgent') -> bool:
        """Activate perceptual capability."""
        try:
            agent.configure_perception(
                enhancement_level=self.enhancement_level,
                processing_accuracy=self.processing_accuracy
            )
            return True
        except Exception as e:
            print(f"Failed to activate perceptual capability: {e}")
            return False
    
    def deactivate(self, agent: 'AdvancedAgent') -> bool:
        """Deactivate perceptual capability."""
        try:
            agent.configure_perception(enhancement_level=1.0)
            return True
        except Exception as e:
            print(f"Failed to deactivate perceptual capability: {e}")
            return False
    
    def get_capability_info(self) -> Dict[str, Any]:
        """Get capability information."""
        return {
            'name': 'PerceptualCapability',
            'description': 'Enhances perceptual processing and sensory interpretation',
            'parameters': ['enhancement_level', 'processing_accuracy'],
            'activation_cost': 'low'
        }

class MotorControlCapability(AgentCapability):
    """Capability for motor control."""
    
    def __init__(self, precision: float = 1.0):
        self.precision = precision
        self.control_stability = 0.9
    
    def activate(self, agent: 'AdvancedAgent') -> bool:
        """Activate motor control capability."""
        try:
            agent.configure_motor_control(
                precision=self.precision,
                control_stability=self.control_stability
            )
            return True
        except Exception as e:
            print(f"Failed to activate motor control capability: {e}")
            return False
    
    def deactivate(self, agent: 'AdvancedAgent') -> bool:
        """Deactivate motor control capability."""
        try:
            agent.configure_motor_control(precision=1.0)
            return True
        except Exception as e:
            print(f"Failed to deactivate motor control capability: {e}")
            return False
    
    def get_capability_info(self) -> Dict[str, Any]:
        """Get capability information."""
        return {
            'name': 'MotorControlCapability',
            'description': 'Controls agent movement and motor actions with precision',
            'parameters': ['precision', 'control_stability'],
            'activation_cost': 'medium'
        }

class LearningCapability(AgentCapability):
    """Capability for learning and adaptation."""
    
    def __init__(self, learning_rate: float = 0.1, memory_size: int = 1000):
        self.learning_rate = learning_rate
        self.memory_size = memory_size
        self.adaptation_efficiency = 0.8
    
    def activate(self, agent: 'AdvancedAgent') -> bool:
        """Activate learning capability."""
        try:
            agent.configure_learning(
                learning_rate=self.learning_rate,
                memory_size=self.memory_size,
                adaptation_efficiency=self.adaptation_efficiency
            )
            return True
        except Exception as e:
            print(f"Failed to activate learning capability: {e}")
            return False
    
    def deactivate(self, agent: 'AdvancedAgent') -> bool:
        """Deactivate learning capability."""
        try:
            agent.configure_learning(learning_rate=0.01)
            return True
        except Exception as e:
            print(f"Failed to deactivate learning capability: {e}")
            return False
    
    def get_capability_info(self) -> Dict[str, Any]:
        """Get capability information."""
        return {
            'name': 'LearningCapability',
            'description': 'Enables learning, adaptation, and experience-based improvement',
            'parameters': ['learning_rate', 'memory_size', 'adaptation_efficiency'],
            'activation_cost': 'high'
        }

class AdvancedAgent:
    """Advanced agent with modular capabilities and lifecycle management."""
    
    def __init__(self, brain: AdvancedBrain):
        self.brain = brain
        self.capabilities: Dict[str, AgentCapability] = {}
        self.active_capabilities: List[str] = []
        self.agent_state: Dict[str, Any] = {
            'health': 100.0,
            'energy': 100.0,
            'age': 0.0,
            'experience': 0.0,
            'skills': [],
            'memory': {}
        }
        self.behavior_history: List[Dict[str, Any]] = []
    
    def add_capability(self, capability: AgentCapability) -> str:
        """Add a capability to the agent."""
        capability_name = capability.get_capability_info()['name']
        self.capabilities[capability_name] = capability
        return capability_name
    
    def activate_capability(self, capability_name: str) -> bool:
        """Activate a capability."""
        if capability_name not in self.capabilities:
            print(f"Capability not found: {capability_name}")
            return False
        
        capability = self.capabilities[capability_name]
        success = capability.activate(self)
        
        if success:
            self.active_capabilities.append(capability_name)
            print(f"✓ Activated capability: {capability_name}")
        
        return success
    
    def deactivate_capability(self, capability_name: str) -> bool:
        """Deactivate a capability."""
        if capability_name not in self.capabilities:
            print(f"Capability not found: {capability_name}")
            return False
        
        capability = self.capabilities[capability_name]
        success = capability.deactivate(self)
        
        if success:
            if capability_name in self.active_capabilities:
                self.active_capabilities.remove(capability_name)
            print(f"✓ Deactivated capability: {capability_name}")
        
        return success
    
    def configure_perception(self, **kwargs) -> bool:
        """Configure perceptual processing."""
        # This would configure the brain's perceptual capabilities
        print(f"Configuring perception: {kwargs}")
        return True
    
    def configure_motor_control(self, **kwargs) -> bool:
        """Configure motor control."""
        # This would configure the brain's motor control capabilities
        print(f"Configuring motor control: {kwargs}")
        return True
    
    def configure_learning(self, **kwargs) -> bool:
        """Configure learning capabilities."""
        # This would configure the brain's learning capabilities
        print(f"Configuring learning: {kwargs}")
        return True
    
    def process_sensory_input(self, percept: Any) -> Any:
        """Process sensory input through active capabilities."""
        processed_percept = percept
        
        # Apply perceptual capabilities
        for capability_name in self.active_capabilities:
            if capability_name == 'PerceptualCapability':
                processed_percept = self._apply_perceptual_processing(processed_percept)
        
        # Store behavior
        self.behavior_history.append({
            'action': 'process_sensory_input',
            'percept': percept,
            'processed_percept': processed_percept,
            'capabilities_used': self.active_capabilities.copy(),
            'timestamp': time.time()
        })
        
        return processed_percept
    
    def _apply_perceptual_processing(self, percept: Any) -> Any:
        """Apply perceptual processing (simplified)."""
        # Simulate perceptual enhancement
        return percept
    
    def make_decision(self, percept: Any) -> Any:
        """Make decision based on percept and active capabilities."""
        # Get input from brain
        brain_output = self._get_brain_output(percept)
        
        # Apply motor control capabilities
        motor_command = self._apply_motor_control(brain_output)
        
        # Store decision
        self.behavior_history.append({
            'action': 'make_decision',
            'percept': percept,
            'brain_output': brain_output,
            'motor_command': motor_command,
            'capabilities_used': self.active_capabilities.copy(),
            'timestamp': time.time()
        })
        
        return motor_command
    
    def _get_brain_output(self, percept: Any) -> Any:
        """Get brain output for given percept (simplified)."""
        return f"brain_output_for_{percept}"
    
    def _apply_motor_control(self, brain_output: Any) -> Any:
        """Apply motor control to brain output (simplified)."""
        return f"controlled_{brain_output}"
    
    def learn_from_experience(self, reward: float, success: bool):
        """Learn from experience and update capabilities."""
        # Update agent state
        self.agent_state['experience'] += reward
        self.agent_state['health'] = min(100.0, self.agent_state['health'] + (reward * 0.1))
        
        # Update capability performance based on experience
        for capability_name in self.active_capabilities:
            if capability_name == 'LearningCapability':
                self._update_learning_performance(reward, success)
        
        # Store learning event
        self.behavior_history.append({
            'action': 'learn_from_experience',
            'reward': reward,
            'success': success,
            'agent_state': self.agent_state.copy(),
            'timestamp': time.time()
        })
    
    def _update_learning_performance(self, reward: float, success: bool):
        """Update learning capability performance."""
        if success:
            self.agent_state['skills'].append(f'lesson_{len(self.agent_state["skills"])}')
        
        # Adjust learning parameters based on performance
        if reward > 10:
            # Good performance - maintain learning rate
            pass
        elif reward < 0:
            # Poor performance - reduce learning rate
            pass
    
    def get_agent_status(self) -> Dict[str, Any]:
        """Get current agent status."""
        return {
            'active_capabilities': self.active_capabilities,
            'agent_state': self.agent_state,
            'behavior_history_length': len(self.behavior_history),
            'total_experience': self.agent_state['experience'],
            'health': self.agent_state['health'],
            'energy': self.agent_state['energy'],
            'age': self.agent_state['age']
        }
    
    def generate_agent_report(self) -> str:
        """Generate comprehensive agent report."""
        status = self.get_agent_status()
        
        report = f"Advanced Agent Report\n"
        report += f"{'=' * 40}\n"
        report += f"Active Capabilities: {', '.join(status['active_capabilities']) if status['active_capabilities'] else 'None'}\n"
        report += f"Health: {status['health']:.1f}/100.0\n"
        report += f"Energy: {status['energy']:.1f}/100.0\n"
        report += f"Age: {status['age']:.2f}\n"
        report += f"Total Experience: {status['total_experience']:.2f}\n"
        report += f"Skills Acquired: {len(status['skills'])}\n"
        report += f"Behavior History: {status['behavior_history_length']} events\n"
        
        if status['skills']:
            report += f"Skills: {', '.join(status['skills'][:5])}\n"
        
        return report

# Use advanced agent framework
brain = AdvancedBrain(verbose=True)
advanced_agent = AdvancedAgent(brain)

# Create capabilities
perceptual_cap = PerceptualCapability(enhancement_level=1.2)
motor_cap = MotorControlCapability(precision=1.1)
learning_cap = LearningCapability(learning_rate=0.15)

# Add capabilities
advanced_agent.add_capability(perceptual_cap)
advanced_agent.add_capability(motor_cap)
advanced_agent.add_capability(learning_cap)

print("Created Advanced Agent with modular capabilities")

# Activate capabilities
advanced_agent.activate_capability('PerceptualCapability')
advanced_agent.activate_capability('MotorControlCapability')
advanced_agent.activate_capability('LearningCapability')

print(f"Active capabilities: {advanced_agent.get_agent_status()['active_capabilities']}")

# Simulate agent interaction
print("\nSimulating agent interaction...")
sample_percept = type('SamplePercept', (), {})()

# Process sensory input
processed_percept = advanced_agent.process_sensory_input(sample_percept)
print(f"✓ Processed percept")

# Make decision
motor_command = advanced_agent.make_decision(sample_percept)
print(f"✓ Made decision: {motor_command}")

# Learn from experience
advanced_agent.learn_from_experience(reward=5.0, success=True)
print(f"✓ Learned from experience (reward: 5.0)")

# Get agent status
status = advanced_agent.get_agent_status()
print(f"\nAgent Status:")
print(f"  Health: {status['health']:.1f}")
print(f"  Experience: {status['experience']:.2f}")
print(f"  Skills: {len(status['skills'])}")

# Generate agent report
report = advanced_agent.generate_agent_report()
print(f"\n=== Agent Report ===\n{report}")
```

## 10. Performance and Scalability

### 10.1 Simulation Performance Optimization

Optimize simulation performance for large-scale simulations:

```python
from pynlm import AdvancedBrain
import multiprocessing
import concurrent.futures
import time

class ScalableSimulation:
    """Scalable simulation for large-scale neural networks."""
    
    def __init__(self, brain: AdvancedBrain):
        self.brain = brain
        self.workers = []
        self.parallel_mode = False
        self.performance_cache = {}
        self.optimization_history: List[Dict[str, Any]] = []
    
    def configure_parallel_processing(self, enable: bool = True, num_workers: int = None):
        """Configure parallel processing for simulation steps."""
        if enable:
            if num_workers is None:
                num_workers = multiprocessing.cpu_count()
            
            self.workers = [multiprocessing.Process() for _ in range(num_workers)]
            self.parallel_mode = True
            print(f"✓ Enabled parallel processing with {num_workers} workers")
        else:
            self.parallel_mode = False
            print("✓ Disabled parallel processing")
    
    def execute_step_parallel(self, step: int) -> Dict[str, Any]:
        """Execute simulation step in parallel."""
        if not self.parallel_mode:
            return self._execute_step_sequential(step)
        
        # Distribute work among workers
        with concurrent.futures.ProcessPoolExecutor(max_workers=len(self.workers)) as executor:
            future = executor.submit(self._execute_step_sequential, step)
            return future.result()
    
    def _execute_step_sequential(self, step: int) -> Dict[str, Any]:n        """Execute simulation step sequentially."""
        # Execute brain step
        if self.brain._brain:
            self.brain._brain.step(step)
        
        # Collect metrics
        metrics = {
            'step': step,
            'timestamp': time.time(),
            'processing_time': time.time() - self.brain._start_time if hasattr(self.brain, '_start_time') else 0,
            'neurons_active': self.brain.get_active_neuron_count() if hasattr(self.brain, 'get_active_neuron_count') else 0
        }
        
        # Cache performance
        self._cache_performance(metrics)
        
        return metrics
    
    def _cache_performance(self, metrics: Dict[str, Any]):
        """Cache performance data for optimization."""
        # Implement caching strategy
        pass
    
    def optimize_for_scale(self, target_steps: int, available_memory: float):
        """Optimize simulation for scale based on target steps and memory."""
        optimization_result = {
            'target_steps': target_steps,
            'available_memory': available_memory,
            'recommended_config': {},
            'optimizations': [],
            'performance_improvements': []
        }
        
        # Analyze current configuration
        current_steps = self.brain._config.get('max_steps', 100)
        current_memory = self._estimate_memory_usage()
        
        # Optimize based on target
        if target_steps > current_steps:
            # For large-scale simulation
            optimization_result['recommended_config'].update({
                'max_steps': min(target_steps, int(available_memory * 0.1)),
                'enable_monitoring': False,
                'enable_visualization': False,
                'simulation_speed': 0.8
            })
            optimization_result['optimizations'].append(
                "Reduced memory-intensive features for scale"
            )
        elif available_memory < current_memory * 0.5:
            # Memory-constrained optimization
            optimization_result['recommended_config'].update({
                'max_steps': max(50, current_steps // 2),
                'enable_monitoring': False,
                'enable_visualization': False,
                'simulation_speed': 1.2
            })
            optimization_result['optimizations'].append(
                "Memory-efficient configuration for reduced resources"
            )
        else:
            # Standard optimization
            optimization_result['recommended_config'].update({
                'simulation_speed': min(2.0, self.brain._config.get('simulation_speed', 1.0) * 1.1)
            })
            optimization_result['optimizations'].append(
                "Optimized simulation speed for performance"
            )
        
        # Apply optimizations
        if optimization_result['recommended_config']:
            self.brain.configure_simulation(**optimization_result['recommended_config'])
        
        optimization_result['performance_improvements'] = self._estimate_performance_improvements(
            optimization_result['recommended_config']
        )
        
        return optimization_result
    
    def _estimate_memory_usage(self) -> float:
        """Estimate current memory usage."""
        try:
            import psutil
            process = psutil.Process()
            return process.memory_info().rss / 1024 / 1024  # MB
        except ImportError:
            return 0.0
    
    def _estimate_performance_improvements(self, config: Dict[str, Any]) -> List[str]:
        """Estimate performance improvements from configuration changes."""
        improvements = []
        
        if not config.get('enable_monitoring', True):
            improvements.append("Reduced memory overhead by disabling monitoring")
        
        if not config.get('enable_visualization', True):
            improvements.append("Reduced memory overhead by disabling visualization")
        
        speed = config.get('simulation_speed', 1.0)
        if speed > 1.0:
            improvements.append(f"Increased simulation speed to {speed:.1f}x")
        
        return improvements
    
    def run_scalable_simulation(self, target_steps: int, max_workers: int = None):
        """Run scalable simulation with optimized configuration."""
        print(f"Starting scalable simulation with target {target_steps} steps")
        
        # Estimate system resources
        available_memory = self._estimate_memory_usage()
        print(f"Available memory: {available_memory:.1f} MB")
        
        # Optimize configuration for scale
        optimization = self.optimize_for_scale(target_steps, available_memory)
        
        if optimization['recommended_config']:
            print(f"Optimizations applied: {optimization['optimizations']}")
        
        # Configure parallel processing
        self.configure_parallel_processing(
            enable=target_steps > 500,
            num_workers=max_workers
        )
        
        # Run simulation
        results = self.brain.run_episode(max_steps=min(target_steps, 1000), verbose=True)
        
        # Analyze results
        final_analysis = self._analyze_scalable_performance(results)
        
        return {
            'results': results,
            'optimization': optimization,
            'final_analysis': final_analysis
        }
    
    def _analyze_scalable_performance(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze performance for scalable simulation."""
        steps_completed = results['steps_completed']
        
        analysis = {
            'scale_efficiency': steps_completed / self.brain._config.get('max_steps', 100),
            'performance_metrics': {
                'steps_per_second': steps_completed / (results['metrics'].get('performance_stats', {}).get('total_simulation_time', 1) if results['metrics'].get('performance_stats') else 1),
                'memory_efficiency': 1.0 / self._estimate_memory_usage() if self._estimate_memory_usage() > 0 else 0,
                'simulation_stability': 1.0 if results['success'] else 0.5
            },
            'recommendations': self._generate_scale_recommendations(steps_completed)
        }
        
        return analysis
    
    def _generate_scale_recommendations(self, steps_completed: int) -> List[str]:
        """Generate scale-specific recommendations."""
        recommendations = []
        
        efficiency = steps_completed / self.brain._config.get('max_steps', 100)
        
        if efficiency < 0.5:
            recommendations.append("Consider reducing max_steps for better efficiency")
        
        if efficiency > 0.9:
            recommendations.append("Good efficiency achieved. Consider increasing max_steps for more data")
        
        memory_usage = self._estimate_memory_usage()
        if memory_usage > 500:
            recommendations.append("High memory usage. Consider memory optimizations for larger scale")
        
        if efficiency > 0.8 and memory_usage < 200:
            recommendations.append("Excellent performance. Ready for larger scale simulations")
        
        return recommendations

# Use scalable simulation
brain = AdvancedBrain(verbose=True)
scalable_sim = ScalableSimulation(brain)

print("Testing scalable simulation...")

# Test different scales
print("\n1. Small-scale simulation (50 steps)...")
results1 = scalable_sim.run_scalable_simulation(target_steps=50)
print(f"   Efficiency: {results1['final_analysis']['scale_efficiency']:.2f}")

print("\n2. Medium-scale simulation (200 steps)...")
results2 = scalable_sim.run_scalable_simulation(target_steps=200)
print(f"   Efficiency: {results2['final_analysis']['scale_efficiency']:.2f}")

print("\n3. Large-scale simulation (500 steps)...")
results3 = scalable_sim.run_scalable_simulation(target_steps=500, max_workers=4)
print(f"   Efficiency: {results3['final_analysis']['scale_efficiency']:.2f}")

# Generate final recommendations
final_recommendations = []
for result in [results1, results2, results3]:
    final_recommendations.extend(result['final_analysis']['recommendations'])

if final_recommendations:
    print(f"\n=== Scale Recommendations ===")
    unique_recommendations = list(set(final_recommendations))
    for i, recommendation in enumerate(unique_recommendations, 1):
        print(f"  {i}. {recommendation}")
```

### 10.2 Distributed Simulation

Implement distributed simulation for very large-scale simulations:

```python
from pynlm import AdvancedBrain
import multiprocessing
import time
import numpy as np

class DistributedSimulation:
    """Distributed simulation for very large-scale neural networks."""
    
    def __init__(self, brain: AdvancedBrain):
        self.brain = brain
        self.worker_pool = None
        self.distributed_mode = False
        self.simulation_chunks = []
        self.distributed_results = []
    
    def setup_distributed_environment(self, num_workers: int = None, memory_limit: float = None):
        """Setup distributed environment for simulation."""
        if num_workers is None:
            num_workers = multiprocessing.cpu_count()
        
        self.num_workers = num_workers
        
        # Create worker pool
        self.worker_pool = multiprocessing.Pool(
            processes=num_workers,
            maxtasksperchild=10,  # Limit memory growth
            initializer=self._worker_initializer
        )
        
        self.distributed_mode = True
        print(f"✓ Setup distributed environment with {num_workers} workers")
        
        if memory_limit:
            self.memory_limit = memory_limit
            print(f"  Memory limit: {memory_limit} MB")
    
    def _worker_initializer(self):
        """Initialize worker processes."""
        # Set up worker-specific configuration
        pass
    
    def split_simulation(self, total_steps: int, num_chunks: int = None):
        """Split simulation into chunks for distributed processing."""
        if num_chunks is None:
            # Determine optimal chunk size based on system resources
            num_chunks = min(self.num_workers * 2, total_steps // 10)
        
        chunk_size = total_steps // num_chunks
        remainder = total_steps % num_chunks
        
        self.simulation_chunks = []
        start_step = 0
        
        for i in range(num_chunks):
            chunk_end = start_step + chunk_size + (1 if i < remainder else 0)
            
            self.simulation_chunks.append({
                'chunk_id': i,
                'start_step': start_step,
                'end_step': chunk_end,
                'size': chunk_end - start_step
            })
            
            start_step = chunk_end
        
        print(f"✓ Split simulation into {len(self.simulation_chunks)} chunks")
        
        for chunk in self.simulation_chunks:
            print(f"  Chunk {chunk['chunk_id']}: steps {chunk['start_step']}-{chunk['end_step']-1} ({chunk['size']} steps)")
    
    def execute_chunk_parallel(self, chunk: Dict[str, Any]) -> Dict[str, Any]:
        """Execute simulation chunk in parallel."""
        if not self.distributed_mode:
            return self._execute_chunk_sequential(chunk)
        
        # Execute chunk in worker process
        chunk_result = self.worker_pool.apply_async(
            self._execute_chunk_sequential,
            (chunk,)
        )
        
        return chunk_result.get(timeout=300)  # 5 minute timeout
    
    def _execute_chunk_sequential(self, chunk: Dict[str, Any]) -> Dict[str, Any]:
        """Execute simulation chunk sequentially."""
        print(f"  Worker executing chunk {chunk['chunk_id']}: steps {chunk['start_step']}-{chunk['end_step']-1}")
        
        # Simulate chunk execution (this would call actual brain simulation)
        chunk_results = {
            'chunk_id': chunk['chunk_id'],
            'start_step': chunk['start_step'],
            'end_step': chunk['end_step'],
            'steps_completed': chunk['size'],
            'execution_time': np.random.uniform(0.5, 2.0),  # Simulated execution time
            'performance_metrics': {
                'cpu_usage': np.random.uniform(40, 80),
                'memory_usage': np.random.uniform(200, 500),
                'neurons_per_second': np.random.uniform(50, 200)
            },
            'success': True
        }
        
        # Simulate some processing time
        time.sleep(chunk['execution_time'])
        
        return chunk_results
    
    def run_distributed_simulation(self, total_steps: int, max_workers: int = None):
        """Run distributed simulation across multiple workers."""
        print(f"Starting distributed simulation with {total_steps} steps")
        
        # Setup distributed environment
        self.setup_distributed_environment(num_workers=max_workers)
        
        # Split simulation into chunks
        self.split_simulation(total_steps)
        
        # Execute chunks in parallel
        print("\nExecuting simulation chunks in parallel...")
        chunk_futures = []
        
        for chunk in self.simulation_chunks:
            if self.distributed_mode:
                # Submit chunk to worker pool
                future = self.worker_pool.apply_async(self._execute_chunk_sequential, (chunk,))
                chunk_futures.append(future)
            else:
                # Execute immediately
                result = self._execute_chunk_sequential(chunk)
                self.distributed_results.append(result)
        
        # Collect results
        if self.distributed_mode and chunk_futures:
            self.distributed_results = []
            for future in chunk_futures:
                result = future.get(timeout=300)
                self.distributed_results.append(result)
        
        # Aggregate results
        aggregated_results = self._aggregate_distributed_results()
        
        # Cleanup
        if self.distributed_mode and self.worker_pool:
            self.worker_pool.close()
            self.worker_pool.join()
            self.distributed_mode = False
        
        return aggregated_results
    
    def _aggregate_distributed_results(self) -> Dict[str, Any]:
        """Aggregate results from all chunks."""
        if not self.distributed_results:
            return {'success': False, 'error': 'No results to aggregate'}
        
        # Calculate total metrics
        total_steps = sum(r['steps_completed'] for r in self.distributed_results)
        total_execution_time = sum(r['execution_time'] for r in self.distributed_results)
        
        # Average performance metrics
        avg_cpu_usage = np.mean([r['performance_metrics']['cpu_usage'] for r in self.distributed_results])
        avg_memory_usage = np.mean([r['performance_metrics']['memory_usage'] for r in self.distributed_results])
        avg_neurons_per_second = np.mean([r['performance_metrics']['neurons_per_second'] for r in self.distributed_results])
        
        # Calculate efficiency
        efficiency = total_steps / total_execution_time if total_execution_time > 0 else 0
        
        # Determine overall success
        success = all(r['success'] for r in self.distributed_results)
        
        aggregated = {
            'total_steps': total_steps,
            'chunks_executed': len(self.distributed_results),
            'total_execution_time': total_execution_time,
            'average_cpu_usage': avg_cpu_usage,
            'average_memory_usage': avg_memory_usage,
            'average_neurons_per_second': avg_neurons_per_second,
            'efficiency': efficiency,
            'success': success,
            'chunk_results': self.distributed_results
        }
        
        return aggregated
    
    def get_distributed_performance_report(self) -> str:
        """Generate performance report for distributed simulation."""
        if not self.distributed_results:
            return "No distributed simulation results available."
        
        aggregated = self._aggregate_distributed_results()
        
        report = f"Distributed Simulation Performance Report\n"
        report += f"{'=' * 50}\n\n"
        
        report += f"Total Steps: {aggregated['total_steps']}\n"
        report += f"Chunks Executed: {aggregated['chunks_executed']}\n"
        report += f"Total Execution Time: {aggregated['total_execution_time']:.2f}s\n"
        report += f"Average CPU Usage: {aggregated['average_cpu_usage']:.1f}%\n"
        report += f"Average Memory Usage: {aggregated['average_memory_usage']:.1f} MB\n"
        report += f"Average Neurons per Second: {aggregated['average_neurons_per_second']:.1f}\n"
        report += f"Overall Efficiency: {aggregated['efficiency']:.2f} steps/sec\n"
        report += f"Success Rate: {'100%' if aggregated['success'] else '0%'}\n\n"
        
        # Per-chunk performance
        report += f"Per-Chunk Performance:\n"
        for chunk in aggregated['chunk_results']:
            report += f"  Chunk {chunk['chunk_id']}: {chunk['steps_completed']} steps in {chunk['execution_time']:.2f}s "
            report += f"(CPU: {chunk['performance_metrics']['cpu_usage']:.1f}%, "
            report += f"Memory: {chunk['performance_metrics']['memory_usage']:.1f} MB)\n"
        
        # Scalability analysis
        report += f"\nScalability Analysis:\n"
        efficiency = aggregated['efficiency']
        if efficiency > 10:
            report += f"  ✓ Excellent scalability: {efficiency:.2f} steps/sec\n"
        elif efficiency > 5:
            report += f"  ✓ Good scalability: {efficiency:.2f} steps/sec\n"
        elif efficiency > 1:
            report += f"  ✓ Acceptable scalability: {efficiency:.2f} steps/sec\n"
        else:
            report += f"  ✗ Poor scalability: {efficiency:.2f} steps/sec\n"
        
        # Resource utilization
        if aggregated['average_cpu_usage'] < 70:
            report += f"  ✓ CPU utilization within optimal range\n"
        else:
            report += f"  ⚠ High CPU utilization - consider optimization\n"
        
        if aggregated['average_memory_usage'] < 300:
            report += f"  ✓ Memory utilization within optimal range\n"
        else:
            report += f"  ⚠ High memory utilization - consider memory optimization\n"
        
        return report

# Use distributed simulation
brain = AdvancedBrain(verbose=True)
distributed_sim = DistributedSimulation(brain)

print("Testing distributed simulation...")

# Test distributed simulation
print("\nRunning distributed simulation (1000 steps)...")
results = distributed_sim.run_distributed_simulation(total_steps=1000, max_workers=4)

print(f"\n=== Distributed Simulation Results ===")
print(f"Success: {results['success']}")
print(f"Total Steps: {results['total_steps']}")
print(f"Chunks Executed: {results['chunks_executed']}")
print(f"Total Execution Time: {results['total_execution_time']:.2f}s")
print(f"Efficiency: {results['efficiency']:.2f} steps/sec")

# Generate performance report
report = distributed_sim.get_distributed_performance_report()
print(f"\n=== Performance Report ===\n{report}")
```

## 11. Migration Guide

### 11.1 From Legacy Bindings to Enhanced Bindings

Migration guide for upgrading from legacy NLM Python bindings to enhanced bindings:

#### Step 1: Update Imports

**Legacy bindings:**
```python
from nlm.bindings import Brain, Agent, World, Config
```

**Enhanced bindings:**
```python
from pynlm import AdvancedBrain, AgentBrain, World, Config
```

#### Step 2: Update Class Names and Constructors

**Legacy:**
```python
class MyBrain(Brain):
    def __init__(self, config):
        super().__init__(config)
```

**Enhanced:**
```python
class MyBrain(AdvancedBrain):
    def __init__(self, config=None, enable_optimization=True, verbose=False):
        super().__init__(config, enable_optimization, verbose)
```

#### Step 3: Update Method Calls

**Legacy methods:**
```python
brain.get_neurons()
brain.set_neurons(neurons)
brain.is_neurons_active()
```

**Enhanced properties and methods:**
```python
brain.neurons  # Property
brain.neurons = neurons  # Property setter
brain.is_neurons_active()  # Method
brain.get_neuron_count()  # More descriptive method
```

#### Step 4: Update Configuration

**Legacy configuration:**
```python
config = Config()
config.set('max_steps', 1000)
config.set('enable_visualization', True)
```

**Enhanced configuration:**
```python
config = Config()
config.configure(max_steps=1000, enable_visualization=True)
```

#### Step 5: Add Error Handling

**Legacy error handling:**
```python
try:
    result = brain.run_episode(100)
except Exception as e:
    print(f"Error: {e}")
```

**Enhanced error handling:**
```python
from pynlm import ValidationError, NLMError

try:
    result = brain.run_episode(100)
except ValidationError as e:
    print(f"Configuration error: {e}")
except NLMError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

#### Step 6: Update Simulation Execution

**Legacy simulation:**
```python
def run_simulation():
    config = Config()
    config.set('max_steps', 100)
    
    brain = Brain(config)
    brain.initialize()
    
    for step in range(config.get('max_steps', 0)):
        brain.step(step)
    
    return brain.get_results()
```

**Enhanced simulation:**
```python
from pynlm import create_simulation

def run_simulation():
    # Create simulation with convenience function
    brain, world, agent_brain = create_simulation()
    
    # Configure
    brain.configure_simulation(
        max_steps=100,
        enable_monitoring=True,
        enable_visualization=False
    )
    
    # Initialize agent
    agent_brain.initialize(world)
    
    # Run episode with advanced features
    results = brain.run_episode(max_steps=100, verbose=True)
    
    return results
```

### 11.2 Migration Checklist

#### ✅ Import Updates
- [ ] Update import statements from legacy to enhanced bindings
- [ ] Update import paths if necessary

#### ✅ Class and Method Updates
- [ ] Rename classes (Brain → AdvancedBrain, Agent → AgentBrain)
- [ ] Update constructor parameters
- [ ] Replace repetitive get/set methods with properties
- [ ] Update method signatures and parameter names

#### ✅ Configuration Updates
- [ ] Update configuration API (configure() instead of set())
- [ ] Update configuration parameter names
- [ ] Update validation rules
- [ ] Add new configuration options

#### ✅ Simulation Updates
- [ ] Replace manual step-by-step execution with run_episode()
- [ ] Add error handling with ValidationError and NLMError
- [ ] Update configuration initialization
- [ ] Add monitoring and visualization setup

#### ✅ Performance Updates
- [ ] Update performance monitoring setup
- [ ] Add visualization configuration
- [ ] Update callback system usage
- [ ] Add async support if needed

#### ✅ Testing Updates
- [ ] Update test cases to new API
- [ ] Add tests for new features (type hints, context managers)
- [ ] Update test assertions
- [ ] Add integration tests for enhanced features

### 11.3 Migration Timeline

**Phase 1: Basic Migration (1-2 weeks)**
1. Update imports and class definitions
2. Update basic configuration
3. Test basic simulation functionality
4. Verify error handling

**Phase 2: Feature Migration (2-3 weeks)**
1. Update simulation execution
2. Add monitoring and visualization
3. Implement callbacks and context managers
4. Test advanced features

**Phase 3: Optimization and Testing (1-2 weeks)**
1. Optimize configuration for performance
2. Add comprehensive test suite
3. Update documentation
4. Final testing and validation

### 11.4 Rollback Strategy

If migration issues arise:

1. **Partial Rollback:** Revert specific components while maintaining enhancements
2. **Feature Flags:** Use feature flags to toggle between old and new APIs
3. **Gradual Migration:** Migrate in phases rather than all at once
4. **Fallback Mechanisms:** Maintain compatibility layers for critical functionality

#### Common Migration Issues and Solutions:

**Issue:** Method not found in enhanced bindings
- **Solution:** Check method names in enhanced bindings.py
- **Example:** `get_neurons()` → `neurons` property

**Issue:** Configuration not being applied
- **Solution:** Ensure configuration uses `configure()` method
- **Example:** `config.set('key', 'value')` → `config.configure(key='value')`

**Issue:** Simulation not running
- **Solution:** Ensure simulation is properly configured and initialized
- **Example:** Add missing configuration parameters

## 12. Frequently Asked Questions

### Q: How does the enhanced bindings improve performance?

A: The enhanced bindings provide several performance improvements:

1. **Optimized Configuration:** Efficient validation and parameter handling
2. **Memory Management:** Better resource allocation and cleanup
3. **Parallel Processing:** Support for distributed and parallel simulation
4. **Caching:** Performance caching for frequently accessed data
5. **Streamlined APIs:** Reduced method call overhead

### Q: Can I use the enhanced bindings with existing C++ code?

A: Yes! The enhanced bindings are designed to work seamlessly with the existing C++ NLM framework. They provide a Pythonic interface while maintaining compatibility with the underlying C++ implementation.

### Q: What are the system requirements for running enhanced bindings?

A: Minimum requirements:
- Python 3.8 or higher
- Basic installation with optional dependencies for enhanced features

Recommended for full functionality:
- matplotlib for visualization
- numpy for numerical operations
- psutil for system monitoring

### Q: How do I handle large-scale simulations?

A: Use the scalable simulation features:

1. **Distributed Simulation:** Run simulation across multiple processes
2. **Chunk-based Processing:** Split large simulations into manageable chunks
3. **Memory Optimization:** Configure memory-efficient settings
4. **Performance Tuning:** Use performance monitoring to optimize configuration

### Q: What's the difference between `run_episode()` and `run_episodes()`?

A: 
- `run_episode()`: Run a single simulation episode with advanced controls
- `run_episodes()`: Run multiple episodes and aggregate results

### Q: How do I add custom visualization?

A: Use the `VisualizationIntegration` class to add custom visualization:

```python
brain.visualization.setup_plotting('matplotlib')
plot = brain.visualization.create_brain_network_plot(brain)
```

### Q: Can I use both async and sync operations?

A: Yes! The enhanced bindings support both synchronous and asynchronous operations:

```python
# Synchronous
sync_results = brain.run_episode(100)

# Asynchronous
async def async_example():
    async_results = await brain.async_step(10)
    return async_results
```

## 13. Future Enhancements

### Planned Features

1. **Advanced Machine Learning Integration:** Plug-and-play ML components
2. **Real-time Collaboration:** Multi-user simulation sharing
3. **Cloud Deployment:** Native cloud integration
4. **Mobile Support:** Enhanced mobile platform support
5. **Extended Reality (XR):** VR/AR visualization integration

### Community Contributions

We welcome community contributions to enhance the NLM Python bindings:

- **Documentation:** Improve guides and examples
- **Features:** Add new visualization backends, plugins, or capabilities
- **Performance:** Optimize existing features
- **Testing:** Add comprehensive test coverage
- **Bug Fixes:** Address issues and improve reliability

## 14. Support and Community

### Getting Help

1. **Documentation:** Check the `documentation/` directory for comprehensive guides
2. **Examples:** Run examples in the `examples/` directory
3. **Community:** Join the NLM Python bindings community
4. **Issues:** Report bugs and feature requests

### Contributing

1. **Code Quality:** Follow Python best practices and code style
2. **Testing:** Ensure comprehensive test coverage
3. **Documentation:** Update documentation with examples
4. **Performance:** Optimize for both ease of use and performance

### Acknowledgments

The enhanced NLM Python bindings build upon the foundation of the original C++ NLM framework and incorporate insights from the broader scientific computing community.

## 15. License

MIT License. See LICENSE file for details.

## 16. Copyright

© 2024 Enhanced NLM Python Bindings Team

---

*This documentation provides comprehensive guidance for using the enhanced NLM Python bindings. For the most up-to-date information, refer to the API reference and examples in the repository.*

Enjoy building sophisticated neural simulations with the enhanced NLM Python bindings! 🤖✨