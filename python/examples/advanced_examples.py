"""
Advanced NLM Python Examples
===========================

This file demonstrates advanced usage of the NLM (Neural Learning Machine) 
Python API with enhanced features for brain configuration, agent learning,
monitoring, and neuroscience operations.
"""

import numpy as np
import matplotlib.pyplot as plt
import time
from datetime import datetime

# Import NLM bindings
import pynlm

class AdvancedBrainExample:
    """
    Example demonstrating advanced brain configuration with presets,
    inheritance, and schema validation.
    """
    
    def __init__(self):
        print("=== Advanced Brain Configuration Example ===\n")
        
        # Create custom configuration using advanced API
        params = {
            'neuron_count': 2000,
            'simulation_timestep': 0.001,
            'enable_plasticity': True,
            'plasticity_type': 1,
            'plasticity_learning_rate': 0.01,
            'enable_neuromodulation': True,
            'enable_curiosity': True,
            'enable_novelty': True,
            'enable_prediction_error': True,
            'enable_structural_plasticity': True,
            'enable_development': True,
            'learning_rate': 0.001,
            'discount_factor': 0.99
        }
        
        self.brain = pynlm.createBrain(pynlm.createConfig(params))
        print(f"Created brain with {self.brain.getTotalNeuronCount()} neurons")
        print(f"Brain region count: {self.brain.getRegionCount()}")
        
        # Apply preset configuration
        self.preset_config = pynlm.python.AdvancedBrainConfiguration.applyPreset("research")
        print("Applied 'research' preset configuration")
        
        # Demonstrate configuration validation
        schema = pynlm.python.AdvancedBrainConfiguration.getDefaultBrainSchema()
        is_valid = pynlm.python.AdvancedBrainConfiguration.validateConfig(self.brain.getConfig(), schema)
        print(f"Configuration validation: {'✓ Passed' if is_valid else '✗ Failed'}")
        
        # Test configuration inheritance
        override_params = {
            'neuron_count': 3000,
            'enable_curiosity': False
        }
        inherited_config = pynlm.python.AdvancedBrainConfiguration.inheritFrom(
            self.brain.getConfig(), override_params
        )
        print(f"Inherited configuration with overrides: {inherited_config.getKeys().size()} parameters")
        
        # Export configuration
        export_success = pynlm.python.AdvancedBrainConfiguration.exportConfiguration("brain_config.json")
        print(f"Configuration export: {'✓ Success' if export_success else '✗ Failed'}")
        
        self.brain_initialized = True
    
    def run_example(self):
        """Run the brain configuration example"""
        if not self.brain_initialized:
            print("Error: Brain not initialized")
            return
        
        print("\n--- Brain Configuration Metrics ---")
        metrics = pynlm.python.AdvancedBrainConfiguration.getConfigurationStatistics()
        for key, value in metrics.items():
            print(f"{key}: {value}")
        
        print("\nAdvanced Brain Configuration Example completed successfully!")
        return True


class ReinforcementLearningExample:
    """
    Example demonstrating advanced reinforcement learning with experience replay
    and custom reward functions.
    """
    
    def __init__(self):
        print("\n=== Advanced Reinforcement Learning Example ===\n")
        
        # Create brain and agent
        config = pynlm.createBrain(pynlm.createConfig({
            'neuron_count': 1000,
            'simulation_timestep': 0.001
        }))
        
        self.brain = config
        self.agent_brain = pynlm.createAgentBrain(config)
        
        # Configure advanced agent with RL parameters
        rl_params = {
            'algorithm': 'DEEP_Q_LEARNING',
            'learning_rate': 0.001,
            'discount_factor': 0.99,
            'exploration_rate': 0.1,
            'exploration_decay_rate': 0.0001,
            'replay_buffer_size': 10000,
            'batch_size': 32,
            'target_network_update_rate': 0.01
        }
        
        self.advanced_agent = pynlm.python.AdvancedAgent(self.agent_brain, rl_params)
        print("Created advanced DQN agent")
        
        # Register custom reward function
        def custom_reward(state, action, next_state):
            # Example reward function: encourage reaching higher values
            state_value = sum(state) / len(state)
            next_state_value = sum(next_state) / len(next_state)
            
            # Base reward for state change
            reward = next_state_value - state_value
            
            # Bonus for exploration
            exploration_bonus = 0.1 if action == 0 else 0.0
            
            # Penalty for large state changes (stability)
            state_change_penalty = -0.01 * abs(next_state_value - state_value)
            
            return reward + exploration_bonus + state_change_penalty
        
        reward_info = {
            'name': 'state_transition_reward',
            'description': 'Rewards based on state transitions',
            'formula': 'next_value - current_value + exploration_bonus - stability_penalty'
        }
        
        self.advanced_agent.registerRewardFunction('custom_reward', custom_reward)
        print("Registered custom reward function")
        
        # Enable advanced perception
        self.advanced_agent.enableAdvancedPerception(
            enable_predictions=True,
            enable_novelty=True,
            enable_curiosity=True,
            enable_attention=True
        )
        print("Enabled advanced perception features")
        
        # Initialize learning
        self.advanced_agent.startLearning()
        print("Started learning process")
        
        self.episode_count = 0
        self.total_rewards = 0
    
    def run_episode(self, max_steps=100):
        """Run a single episode of reinforcement learning"""
        print(f"\n--- Episode {self.episode_count + 1} ---")
        
        episode_rewards = []
        
        for step in range(max_steps):
            # Generate random state (in real scenario, this would come from environment)
            state = np.random.rand(20).astype(np.float32)
            
            # Select action using agent's policy
            action = self.advanced_agent.selectAction(state, useExploration=True)
            
            # Execute action (in real scenario, this would interact with environment)
            next_state = np.random.rand(20).astype(np.float32)
            reward = self.advanced_agent.computeCustomReward(state, action, next_state)
            
            # Determine if episode is done
            done = step == max_steps - 1 or np.random.random() < 0.05
            
            # Store experience
            self.advanced_agent.addExperience(state, action, reward, next_state, done)
            
            # Perform learning step
            self.advanced_agent.performRLStep()
            
            episode_rewards.append(reward)
            self.total_rewards += reward
            
            if done:
                break
        
        self.episode_count += 1
        
        # Get learning statistics
        stats = self.advanced_agent.getLearningStatistics()
        
        print(f"Episode completed - Steps: {len(episode_rewards)}, "
              f"Total reward: {sum(episode_rewards):.3f}, "
              f"Average reward: {np.mean(episode_rewards):.3f}")
        print(f"Learning stats - Q-values avg: {stats.qValueAverage:.3f}, "
              f"Exploration rate: {stats.explorationRate:.3f}")
        
        return sum(episode_rewards)
    
    def run_example(self, num_episodes=5):
        """Run the reinforcement learning example"""
        print(f"Running {num_episodes} episodes of reinforcement learning...")
        
        episode_rewards = []
        start_time = time.time()
        
        for episode in range(num_episodes):
            reward = self.run_episode()
            episode_rewards.append(reward)
        
        end_time = time.time()
        
        # Stop learning
        self.advanced_agent.stopLearning()
        
        # Generate summary
        print(f"\n--- RL Example Summary ---")
        print(f"Episodes completed: {num_episodes}")
        print(f"Total time: {end_time - start_time:.2f} seconds")
        print(f"Average episode reward: {np.mean(episode_rewards):.3f}")
        print(f"Best episode reward: {np.max(episode_rewards):.3f}")
        print(f"Reward standard deviation: {np.std(episode_rewards):.3f}")
        
        # Plot learning curve
        plt.figure(figsize=(10, 6))
        plt.plot(range(1, num_episodes + 1), episode_rewards)
        plt.xlabel('Episode')
        plt.ylabel('Total Reward')
        plt.title('Reinforcement Learning Performance')
        plt.grid(True, alpha=0.3)
        plt.savefig('rl_learning_curve.png')
        print("Learning curve saved as 'rl_learning_curve.png'")
        
        print("\nAdvanced Reinforcement Learning Example completed successfully!")
        return True


class MonitoringAndVisualizationExample:
    """
    Example demonstrating real-time monitoring and visualization capabilities.
    """
    
    def __init__(self):
        print("\n=== Monitoring and Visualization Example ===\n")
        
        # Create brain
        config = pynlm.createBrain(pynlm.createConfig({
            'neuron_count': 800,
            'simulation_timestep': 0.001
        }))
        
        self.brain = config
        
        # Configure monitoring system
        monitoring_config = {
            'enableMetricsStreaming': True,
            'metricsStreamIntervalMs': 100.0,
            'enableRealTimeVisualization': True,
            'visualizationBackend': 'web',
            'visualizationServerPort': 8080,
            'enablePerformanceProfiling': True,
            'profilingIntervalSeconds': 1.0,
            'enableActivityPatternAnalysis': True,
            'analysisWindowSeconds': 60.0,
            'enableAnomalyDetection': True,
            'anomalyThreshold': 2.0
        }
        
        pynlm.python.MonitoringSystem.initializeMonitoring(monitoring_config)
        
        # Start metrics streaming
        pynlm.python.MonitoringSystem.startMetricsStreaming(self.brain, 100.0)
        
        print("Initialized monitoring system")
        print("Started metrics streaming")
        
        # Initialize data collection
        self.metric_history = []
        self.visualization_data = []
        self.performance_data = []
        self.activity_data = []
        
        self.monitoring_active = True
    
    def collect_data(self):
        """Collect monitoring data"""
        # Get current metrics
        metrics = pynlm.python.MonitoringSystem.getCurrentMetrics(self.brain)
        self.metric_history.append(metrics)
        
        # Get brain state visualization
        visualization = pynlm.python.MonitoringSystem.getBrainStateVisualization(self.brain)
        self.visualization_data.append(visualization)
        
        # Get performance profile
        profile = pynlm.python.MonitoringSystem.getPerformanceProfile(self.brain)
        self.performance_data.append(profile)
        
        # Get activity pattern analysis
        activity = pynlm.python.MonitoringSystem.analyzeActivityPatterns(self.brain, 60.0)
        self.activity_data.append(activity)
        
        # Print summary
        total_neurons = self.brain.getTotalNeuronCount()
        active_neurons = self.brain.getActiveNeuronCount()
        firing_rate = self.brain.getAverageFiringRate()
        
        print(f"Metrics: {len(metrics)} collected, "
              f"Neurons: {active_neurons}/{total_neurons}, "
              f"Firing rate: {firing_rate:.3f}, "
              f"Step time: {profile.averageStepTime:.3f} ms")
    
    def run_example(self, duration_seconds=30):
        """Run the monitoring example"""
        print(f"Running monitoring simulation for {duration_seconds} seconds...")
        
        start_time = time.time()
        step = 0
        
        while time.time() - start_time < duration_seconds and self.monitoring_active:
            self.collect_data()
            step += 1
            time.sleep(0.5)  # Collect data every 500ms
        
        # Stop monitoring
        pynlm.python.MonitoringSystem.stopMetricsStreaming()
        pynlm.python.MonitoringSystem.shutdownMonitoring()
        
        print(f"\n--- Monitoring Example Summary ---")
        print(f"Monitoring duration: {time.time() - start_time:.2f} seconds")
        print(f"Steps collected: {step}")
        print(f"Total metrics: {len(self.metric_history)}")
        
        # Generate visualizations
        self.generate_visualizations()
        
        print("\nMonitoring and Visualization Example completed successfully!")
        return True
    
    def generate_visualizations(self):
        """Generate and save visualization data"""
        # Generate metrics plot
        if self.metric_history:
            plt.figure(figsize=(12, 8))
            
            # Plot firing rate over time
            plt.subplot(2, 2, 1)
            firing_rates = []
            for metrics in self.metric_history:
                for metric in metrics:
                    if metric.name == 'average_firing_rate':
                        firing_rates.append(metric.currentValue)
            if firing_rates:
                plt.plot(range(len(firing_rates)), firing_rates)
                plt.xlabel('Time Step')
                plt.ylabel('Firing Rate')
                plt.title('Firing Rate Over Time')
                plt.grid(True, alpha=0.3)
            
            # Plot active neurons over time
            plt.subplot(2, 2, 2)
            active_neurons = []
            total_neurons = self.brain.getTotalNeuronCount()
            for metrics in self.metric_history:
                for metric in metrics:
                    if metric.name == 'active_neuron_count':
                        active_neurons.append(metric.currentValue)
            if active_neurons:
                plt.plot(range(len(active_neurons)), active_neurons, 'r-')
                plt.xlabel('Time Step')
                plt.ylabel('Active Neurons')
                plt.title('Active Neuron Count Over Time')
                plt.grid(True, alpha=0.3)
            
            # Plot step times
            plt.subplot(2, 2, 3)
            step_times = []
            for profile in self.performance_data:
                step_times.append(profile.averageStepTime)
            if step_times:
                plt.plot(range(len(step_times)), step_times, 'g-')
                plt.xlabel('Time Step')
                plt.ylabel('Step Time (ms)')
                plt.title('Average Step Time Over Time')
                plt.grid(True, alpha=0.3)
            
            # Plot activity patterns
            plt.subplot(2, 2, 4)
            coherence_levels = []
            for activity in self.activity_data:
                coherence_levels.append(activity.globalCoherenceIndex)
            if coherence_levels:
                plt.plot(range(len(coherence_levels)), coherence_levels, 'b-')
                plt.xlabel('Time Step')
                plt.ylabel('Coherence Index')
                plt.title('Network Coherence Over Time')
                plt.grid(True, alpha=0.3)
            
            plt.tight_layout()
            plt.savefig('monitoring_dashboard.png')
            print("Dashboard saved as 'monitoring_dashboard.png'")
        
        # Generate HTML visualization
        html_content = self.generate_html_visualization()
        with open('monitoring_dashboard.html', 'w') as f:
            f.write(html_content)
        print("HTML dashboard saved as 'monitoring_dashboard.html'")
    
    def generate_html_visualization(self):
        """Generate HTML for web-based visualization"""
        html = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Monitoring Dashboard</title>
            <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
            <style>
                body { font-family: Arial, sans-serif; margin: 20px; }
                .container { max-width: 1200px; margin: 0 auto; }
                .plot { margin: 20px 0; }
                .metric-card { background: #f5f5f5; padding: 15px; margin: 10px; border-radius: 5px; }
                .metric-value { font-size: 24px; font-weight: bold; color: #2c3e50; }
                .metric-label { color: #7f8c8d; }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>NLM Monitoring Dashboard</h1>
                <p>Generated at: """ + datetime.now().strftime("%Y-%m-%d %H:%M:%S") + """
                
                <div class="plot" id="firingRatePlot"></div>
                <div class="plot" id="activeNeuronsPlot"></div>
                <div class="plot" id="stepTimePlot"></div>
                <div class="plot" id="coherencePlot"></div>
                
                <h2>Real-time Metrics</h2>
                <div id="metricsContainer">
        """
        
        # Add current metrics
        if self.metric_history:
            latest_metrics = self.metric_history[-1]
            for metric in latest_metrics:
                html += f"""
                    <div class="metric-card">
                        <div class="metric-label">{metric.name}</div>
                        <div class="metric-value">{metric.currentValue:.3f} {metric.unit}</div>
                    </div>
                """
        
        html += """
                </div>
            </div>
            
            <script>
                // Plot data
                const firingRates = [
        """
        
        # Add firing rate data
        firing_rates = []
        for metrics in self.metric_history:
            for metric in metrics:
                if metric.name == 'average_firing_rate':
                    firing_rates.append(metric.currentValue)
        
        for i, rate in enumerate(firing_rates):
            html += f"{rate}, "
        
        html += """
                ];
                
                const activeNeurons = [
        """
        
        # Add active neuron data
        active_neurons = []
        for metrics in self.metric_history:
            for metric in metrics:
                if metric.name == 'active_neuron_count':
                    active_neurons.append(metric.currentValue)
        
        for i, neurons in enumerate(active_neurons):
            html += f"{neurons}, "
        
        html += """
                ];
                
                const stepTimes = [
        """
        
        # Add step time data
        step_times = []
        for profile in self.performance_data:
            step_times.append(profile.averageStepTime)
        
        for i, time in enumerate(step_times):
            html += f"{time}, "
        
        html += """
                ];
                
                const coherenceLevels = [
        """
        
        # Add coherence data
        coherence_levels = []
        for activity in self.activity_data:
            coherence_levels.append(activity.globalCoherenceIndex)
        
        for i, coherence in enumerate(coherence_levels):
            html += f"{coherence}, "
        
        html += """
                ];
                
                // Plot configurations
                const firingRatePlot = {
                    x: Array.from({length: firingRates.length}, (_, i) => i),
                    y: firingRates,
                    type: 'scatter',
                    mode: 'lines',
                    name: 'Firing Rate',
                    line: {color: '#3498db'}
                };
                
                const activeNeuronsPlot = {
                    x: Array.from({length: activeNeurons.length}, (_, i) => i),
                    y: activeNeurons,
                    type: 'scatter',
                    mode: 'lines',
                    name: 'Active Neurons',
                    line: {color: '#e74c3c'}
                };
                
                const stepTimePlot = {
                    x: Array.from({length: stepTimes.length}, (_, i) => i),
                    y: stepTimes,
                    type: 'scatter',
                    mode: 'lines',
                    name: 'Step Time',
                    line: {color: '#2ecc71'}
                };
                
                const coherencePlot = {
                    x: Array.from({length: coherenceLevels.length}, (_, i) => i),
                    y: coherenceLevels,
                    type: 'scatter',
                    mode: 'lines',
                    name: 'Coherence',
                    line: {color: '#f39c12'}
                };
                
                // Layout configurations
                const layout = {
                    title: 'NLM Monitoring Dashboard',
                    xaxis: {title: 'Time Step'},
                    yaxis: {title: 'Value'},
                    showlegend: true,
                    legend: {x: 0, y: 1}
                };
                
                // Plot graphs
                Plotly.newPlot('firingRatePlot', [firingRatePlot], layout);
                Plotly.newPlot('activeNeuronsPlot', [activeNeuronsPlot], layout);
                Plotly.newPlot('stepTimePlot', [stepTimePlot], layout);
                Plotly.newPlot('coherencePlot', [coherencePlot], layout);
                
                // Auto-refresh every 5 seconds
                setInterval(function() {
                    location.reload();
                }, 5000);
            </script>
        </body>
        </html>
        """
        
        return html
    
    def run_example(self, duration_seconds=30):
        """Run the monitoring example (alternative implementation)"""
        print(f"\nRunning monitoring simulation for {duration_seconds} seconds...")
        
        start_time = time.time()
        
        while time.time() - start_time < duration_seconds:
            self.collect_data()
            time.sleep(0.5)
        
        # Stop monitoring
        pynlm.python.MonitoringSystem.stopMetricsStreaming()
        pynlm.python.MonitoringSystem.shutdownMonitoring()
        
        print(f"\n--- Monitoring Example Summary ---")
        print(f"Monitoring duration: {time.time() - start_time:.2f} seconds")
        print(f"Data collected: {len(self.metric_history)} metric sets")
        
        # Generate visualizations
        self.generate_visualizations()
        
        print("\nMonitoring and Visualization Example completed successfully!")
        return True


class IntegrationExample:
    """
    Example demonstrating integration with Python ML libraries and data export/import.
    """
    
    def __init__(self):
        print("\n=== Integration Example ===\n")
        
        # Check available packages
        print(f"NumPy available: {pynlm.python.IntegrationSystem.isNumPyAvailable()}")
        print(f"SciPy available: {pynlm.python.IntegrationSystem.isSciPyAvailable()}")
        
        if pynlm.python.IntegrationSystem.isNumPyAvailable():
            # Create NumPy arrays for integration
            self.data_array = np.random.rand(100, 10).astype(np.float32)
            print(f"Created NumPy array with shape: {self.data_array.shape}")
            
            # Convert to NLM format
            self.nlm_data = pynlm.python.IntegrationSystem.convertToNumPyFormat(self.data_array)
            print(f"Converted to NLM format with size: {len(self.nlm_data)} elements")
            
            # Convert back to NumPy
            self.converted_array = pynlm.python.IntegrationSystem.convertFromNumPyFormat(self.nlm_data)
            print(f"Converted back to NumPy with shape: {self.converted_array.shape}")
        
        if pynlm.python.IntegrationSystem.isSciPyAvailable():
            # Compute statistical analysis
            self.stats = pynlm.python.IntegrationSystem.computeStatisticalAnalysis(self.data_array)
            print(f"Computed statistics - Result: {self.stats.result}, Confidence: {self.stats.confidence}")
            
            # Compute similarity matrix
            self.similarity_matrix = pynlm.python.IntegrationSystem.computeSimilarityMatrix(
                self.data_array.numpy()
            )
            print(f"Computed similarity matrix with shape: {self.similarity_matrix.shape}")
        
        # Test data export/import
        self.test_data = np.array([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
        
        # Export to CSV
        csv_filename = "integration_test_data.csv"
        self.csv_config = pynlm.python.IntegrationSystem.exportDataToCSV(csv_filename, self.test_data)
        print(f"Exported data to CSV: {'✓ Success' if self.csv_config.success else '✗ Failed'}")
        
        # Import from CSV
        self.imported_data = pynlm.python.IntegrationSystem.importDataFromCSV(csv_filename)
        print(f"Imported data from CSV: {self.imported_data.shape if hasattr(self.imported_data, 'shape') else 'Unknown'}")
        
        # Export to JSON
        json_filename = "integration_test_data.json"
        self.json_data = {"data": self.test_data.tolist()}
        self.json_config = pynlm.python.IntegrationSystem.exportDataToJSON(json_filename, self.json_data)
        print(f"Exported data to JSON: {'✓ Success' if self.json_config.success else '✗ Failed'}")
        
        # Import from JSON
        self.imported_json = pynlm.python.IntegrationSystem.importDataFromJSON(json_filename)
        print(f"Imported data from JSON: {'✓ Success' if self.imported_json else '✗ Failed'}")
        
        # Test serialization
        self.serialized = pynlm.python.IntegrationSystem.serializeToJSON(self.test_data)
        print(f"Serialized data to JSON with size: {len(self.serialized)} bytes")
        
        self.deserialized = pynlm.python.IntegrationSystem.deserializeFromJSON(self.serialized)
        print(f"Deserialized data: {'✓ Success' if self.deserialized else '✗ Failed'}")
    
    def run_example(self):
        """Run the integration example"""
        print(f"\n--- Integration Example Summary ---")
        
        if hasattr(self, 'data_array'):
            print(f"NumPy array operations completed")
            print(f"Data array shape: {self.data_array.shape}")
            print(f"Converted array shape: {self.converted_array.shape}")
        
        if hasattr(self, 'stats'):
            print(f"SciPy analysis completed")
            print(f"Statistical result: {self.stats.result}")
            print(f"Similarity matrix shape: {self.similarity_matrix.shape}")
        
        if hasattr(self, 'imported_data'):
            print(f"CSV import/export completed")
            print(f"Imported data type: {type(self.imported_data)}")
        
        if hasattr(self, 'deserialized'):
            print(f"JSON serialization/deserialization completed")
        
        print("\nIntegration Example completed successfully!")
        return True


if __name__ == "__main__":
    print("Advanced NLM Python Examples")
    print("=" * 50)
    
    # Example 1: Advanced Brain Configuration
    brain_example = AdvancedBrainExample()
    brain_example.run_example()
    
    # Example 2: Reinforcement Learning
    rl_example = ReinforcementLearningExample()
    rl_example.run_example(num_episodes=3)
    
    # Example 3: Monitoring and Visualization
    monitoring_example = MonitoringAndVisualizationExample()
    monitoring_example.run_example(duration_seconds=15)
    
    # Example 4: Integration
    integration_example = IntegrationExample()
    integration_example.run_example()
    
    print("\n" + "=" * 50)
    print("All examples completed successfully!")
    print("Check the generated files:")
    print("- rl_learning_curve.png: Reinforcement learning performance")
    print("- monitoring_dashboard.png: Real-time monitoring dashboard")
    print("- monitoring_dashboard.html: Interactive web dashboard")
    print("- brain_config.json: Brain configuration export")
    print("- integration_test_data.csv: Exported test data")
    print("- integration_test_data.json: JSON data export")
"""