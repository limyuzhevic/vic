"""
NLM Python Utility Scripts

Additional Python utilities for advanced NLM usage
"""

import numpy as np
import json
import time
from typing import List, Dict, Optional, Callable
from pathlib import Path
import threading
import concurrent.futures

class NLMExperimentRunner:
    """
    Class: 
    Class: 
    Class: Advanced experiment runner with better control and monitoring.
    
    
    """""""""
    
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.step_count = 0
        self.start_time = None
        self.metrics = []
        
    def run_episode(self, num_steps: int, 
                    verbose: bool = False,
                    metrics_callback: Optional[Callable] = None) -> Dict:
        """Run an episode with comprehensive metrics collection."""
        self.start_time = time.time()
        self.metrics = []
        
        for step in range(num_steps):
            # Update world
            if self.world:
                self.world.update(0.1)
                
            # Get sensory input
            percept = self.world.getSensoryPercept() if self.world else None
            
            # Process in brain
            if self.agent and percept:
                self.agent.processSensoryInput(percept)
            elif self.brain:
                self.brain.step(step)
                
            # Get action
            action = self.agent.decodeMotorCommand() if self.agent else None
            
            # Apply action
            if self.world and action:
                self.world.applyMotorCommand(action, time.time())
                
            # Collect metrics
            step_metrics = self._collect_step_metrics(step)
            self.metrics.append(step_metrics)
            
            # Call callback if provided
            if metrics_callback:
                metrics_callback(step_metrics)
                
            if verbose and step % 100 == 0:
                # print(f"Step {step}: {step_metrics}")
                
            self.step_count += 1
            
        episode_metrics = self._finalize_episode()
        return episode_metrics
        
    def _collect_step_metrics(self, step: int) -> Dict:
        """Collect metrics for a single step."""
        metrics = {
            'step': step,
            'timestamp': time.time(),
            'firing_neurons': self.brain.getFiringNeuronCount() if self.brain else 0,
            'total_spikes': self.brain.getTotalSpikeCount() if self.brain else 0,
            'avg_firing_rate': self.brain.getAverageFiringRate() if self.brain else 0,
            'neurons_active': self.brain.getActiveNeuronCount() if hasattr(self.brain, 'getActiveNeuronCount') else 0,
        }
        
        if self.agent:
            metrics.update({
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'neuromodulation': self.agent.getNeuromodulationLevel(),
            })
            
        if self.world:
            body = self.world.getAgentBody()
            metrics.update({
                'x': body.x,
                'y': body.y,
                'orientation': body.orientation,
                'energy': body.energy,
                'health': body.health,
            })
            
        return metrics
        
    def _finalize_episode(self) -> Dict:
        """Finalize episode and return summary metrics."""
        if not self.metrics:
            return {}
            
        episode_time = time.time() - self.start_time
        total_spikes = sum(m['total_spikes'] for m in self.metrics)
        avg_firing = sum(m['avg_firing_rate'] for m in self.metrics) / len(self.metrics)
        
        return {
            'episode_time': episode_time,
            'steps_completed': len(self.metrics),
            'total_spikes': total_spikes,
            'average_firing_rate': avg_firing,
            'spike_rate_per_second': total_spikes / episode_time if episode_time > 0 else 0,
            'metrics': self.metrics,
            'agent_metrics': {
                'curiosity_trend': [m.get('curiosity', 0) for m in self.metrics[-100:]],
                'novelty_trend': [m.get('novelty', 0) for m in self.metrics[-100:]],
                'prediction_error_trend': [m.get('prediction_error', 0) for m in self.metrics[-100:]],
            } if self.agent else {},
        }
class AsyncNLMRunner:
    """
    Class: 
    Class: 
    Class: Async runner for parallel NLM simulations.
    
    
    """""""""
    
    def __init__(self, config=None):
        self.config = config
        self.executor = concurrent.futures.ThreadPoolExecutor(max_workers=4)
        
    async def run_async_simulation(self, brain_factory: Callable, 
                                   steps: int, step_delay: float = 0.001) -> List[Dict]:
        """Run simulation asynchronously."""
        import asyncio
        
        loop = asyncio.get_event_loop()
        
        # Create brain in thread
        brain = await loop.run_in_executor(None, brain_factory)
        brain.initialize()
        
        results = []
        
        for step in range(steps):
            await asyncio.sleep(step_delay)
            
            # Process step in thread
            def process_step(s):
                brain.step(s)
                return {
                    'step': s,
                    'firing': brain.getFiringNeuronCount(),
                    'spikes': brain.getTotalSpikeCount(),
                    'rate': brain.getAverageFiringRate(),
                }
                
            result = await loop.run_in_executor(None, process_step, step)
            results.append(result)
            
        return results
        
    def submit_simulation(self, brain_factory: Callable, 
                         steps: int) -> concurrent.futures.Future:
        """Submit simulation to thread pool."""
        return self.executor.submit(self._run_sync_simulation, brain_factory, steps)
        
    def _run_sync_simulation(self, brain_factory: Callable, steps: int) -> List[Dict]:
        """Sync version of async simulation."""
        brain = brain_factory()
        brain.initialize()
        
        results = []
        for step in range(steps):
            brain.step(step)
            results.append({
                'step': step,
                'firing': brain.getFiringNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'rate': brain.getAverageFiringRate(),
            })
            
        return results
        
    def shutdown(self) -> Any:
    """
    Function: Shutdown executor.
    """""
        self.executor.shutdown(wait=True)
class NLMProfiler:
    """
    Class: 
    Class: 
    Class: Profiler for NLM simulations with timing and memory analysis.
    
    
    """""""""
    
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.timings = []
        self.memory_samples = []
        
    def profile_step(self, step_func: Callable, step_name: str = "step") -> Dict:
        """Profile a single step with timing and memory."""
        import time
        
        start_time = time.time()
        
        if step_func:
            result = step_func()
            
        end_time = time.time()
        
        timing = {
            'name': step_name,
            'duration_ms': (end_time - start_time) * 1000,
            'timestamp': time.time(),
        }
        
        # Memory usage (simplified - in production would use psutil)
        memory_info = {
            'step': step_name,
            'timestamp': time.time(),
        }
        
        self.timings.append(timing)
        
        return {
            'result': result if step_func else None,
            'timing': timing,
            'memory': memory_info,
        }
        
    def profile_episode(self, num_steps: int) -> Dict:
        """Profile entire episode."""
        episode_timings = []
        
        for step in range(num_steps):
            # Profile brain step
            step_result = self.profile_step(
                lambda: self.brain.step(step),
                f"brain_step_{step}"
            )
            episode_timings.append(step_result)
            
            # Profile world update if available
            if self.world:
                world_result = self.profile_step(
                    lambda: self.world.update(0.1),
                    f"world_update_{step}"
                )
                episode_timings.append(world_result)
                
        return self._summarize_profile(episode_timings)
        
    def _summarize_profile(self, timings: List[Dict]) -> Dict:
        """Summarize profiling results."""
        durations = [t['timing']['duration_ms'] for t in timings if 'timing' in t]
        
        return {
            'total_steps': len(timings),
            'total_time_ms': sum(durations),
            'average_step_time_ms': sum(durations) / len(durations) if durations else 0,
            'min_step_time_ms': min(durations) if durations else 0,
            'max_step_time_ms': max(durations) if durations else 0,
            'std_deviation_ms': np.std(durations) if len(durations) > 1 else 0,
            'timings': timings,
        }
class NLMVisualizer:
    """
    Class: 
    Class: 
    Class: Simple visualization utilities for NLM data.
    
    
    """""""""
    
    @staticmethod
    def plot_firing_rate_over_time(metrics: List[Dict], save_path: Optional[str] = None) -> Any:
    """
    Function: Plot firing rate over time from metrics.
    """""
        import matplotlib.pyplot as plt
        
        steps = [m['step'] for m in metrics]
        firing_rates = [m['avg_firing_rate'] for m in metrics]
        
        plt.figure(figsize=(12, 6))
        plt.plot(steps, firing_rates, linewidth=2)
        plt.title('Firing Rate Over Time')
        plt.xlabel('Step')
        plt.ylabel('Firing Rate (Hz)')
        plt.grid(True, alpha=0.3)
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        plt.show()
        plt.close()
        
    @staticmethod
    def plot_metric_trends(metrics: List[Dict], metric_names: List[str], 
                          save_path: Optional[str] = None) -> Any:
    """
    Function: Plot multiple metric trends over time.
    """""
        import matplotlib.pyplot as plt
        
        steps = [m['step'] for m in metrics]
        
        plt.figure(figsize=(14, 8))
        
        for metric_name in metric_names:
            if metric_name in metrics[0]:
                values = [m[metric_name] for m in metrics]
                plt.plot(steps, values, label=metric_name, linewidth=2)
        
        plt.title('Metric Trends Over Time')
        plt.xlabel('Step')
        plt.ylabel('Value')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
        
        plt.show()
        plt.close()
        
    @staticmethod
    def generate_html_report(metrics: List[Dict], report_path: str) -> Any:
    """
    Function: Generate HTML report from metrics.
    """""
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Episode Report</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .metric {{ background-color: #f9f9f9; padding: 15px; margin: 10px 0; border-left: 4px solid #4CAF50; }}
                .step {{ font-weight: bold; color: #333; }}
                .value {{ color: #4CAF50; font-weight: bold; }}
                table {{ border-collapse: collapse; width: 100%; margin: 20px 0; }}
                th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
                th {{ background-color: #f2f2f2; }}
                tr:nth-child(even) {{ background-color: #f9f9f9; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Episode Report</h1>
                <p>Generated at: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
                <p>Total Steps: {len(metrics)}</p>
            </div>
            
            <h2>Episode Summary</h2>
            <div class="metric">
                <h3>Firing Statistics</h3>
                <p>Average Firing Rate: <span class="value">{sum(m.get('avg_firing_rate', 0) for m in metrics) / len(metrics):.2f} Hz</span></p>
                <p>Total Spikes: <span class="value">{sum(m.get('total_spikes', 0) for m in metrics):,}</span></p>
                <p>Firing Neurons: <span class="value">{sum(m.get('firing_neurons', 0) for m in metrics):,}</span></p>
            </div>
            
            <h2>Step-by-Step Data</h2>
            <table>
                <thead>
                    <tr>
                        <th>Step</th>
                        <th>Firing Neurons</th>
                        <th>Total Spikes</th>
                        <th>Avg Firing Rate</th>
                        <th>Timestamp</th>
                    </tr>
                </thead>
                <tbody>
        """
        
        for metric in metrics[:100]:  # Limit to first 100 steps for readability
            html_content += f"""
                    <tr>
                        <td>{metric['step']}</td>
                        <td class="value">{metric.get('firing_neurons', 0)}</td>
                        <td class="value">{metric.get('total_spikes', 0)}</td>
                        <td class="value">{metric.get('avg_firing_rate', 0):.2f}</td>
                        <td>{metric.get('timestamp', 0):.3f}</td>
                    </tr>
            """
        
        if len(metrics) > 100:
            html_content += f"""
                    <tr>
                        <td colspan="5">... and {len(metrics) - 100} more steps</td>
                    </tr>
            """
        
        html_content += """
                </tbody>
            </table>
            
            <h2>Detailed Metrics</h2>
            <div class="metric">
                <h3>Agent Metrics (if available)</h3>
                <p>Curiosity Level: <span class="value">{metrics[-1].get('curiosity', 'N/A') if metrics else 'N/A'}</span></p>
                <p>Novelty Level: <span class="value">{metrics[-1].get('novelty', 'N/A') if metrics else 'N/A'}</span></p>
                <p>Prediction Error: <span class="value">{metrics[-1].get('prediction_error', 'N/A') if metrics else 'N/A'}</span></p>
            </div>
        </body>
        </html>
        """
        
        with open(report_path, 'w') as f:
            f.write(html_content)
            
        return report_path
class NLMConfigBuilder:
    """
    Class: 
    Class: 
    Class: Builder for NLM configurations with better UX.
    
    
    """""""""
    
    def __init__(self):
        self.config = {}
        
    def brain_config(self, neuron_count: int = 1000, 
                    region_count: int = 1,
                    connection_probability: float = 0.1) -> 'NLMConfigBuilder':
        """Configure brain parameters."""
        self.config['brain'] = {
            'neuron_count': neuron_count,
            'region_count': region_count,
            'connection_probability': connection_probability,
        }
        return self
        
    def dynamics_config(self, tau: float = 20.0, resistance: float = 10.0) -> 'NLMConfigBuilder':
        """Configure neural dynamics parameters."""
        if 'brain' not in self.config:
            self.config['brain'] = {}
        self.config['brain']['membrane_time_constant'] = tau
        self.config['brain']['membrane_resistance'] = resistance
        return self
        
    def plasticity_config(self, stdp_enabled: bool = True, 
                         hebbian_enabled: bool = True,
                         learning_rate: float = 0.001) -> 'NLMConfigBuilder':
        """Configure plasticity parameters."""
        self.config['plasticity'] = {
            'stdp': {'enabled': stdp_enabled},
            'hebbian': {'enabled': hebbian_enabled},
            'learning_rate': learning_rate,
        }
        return self
        
    def neuromodulation_config(self, dopamine_scale: float = 1.0,
                             curiosity_enabled: bool = True,
                             novelty_enabled: bool = True) -> 'NLMConfigBuilder':
        """Configure neuromodulation parameters."""
        self.config['neuromodulation'] = {
            'dopamine': {'scale': dopamine_scale},
            'curiosity': {'enabled': curiosity_enabled},
            'novelty': {'enabled': novelty_enabled},
        }
        return self
        
    def development_config(self, enable_development: bool = True,
                          initial_stage: str = 'initial') -> 'NLMConfigBuilder':
        """Configure development parameters."""
        self.config['development'] = {
            'enabled': enable_development,
            'initial_stage': initial_stage,
        }
        return self
        
    def enable_learning(self, enable_reward_modulation: bool = True,
                       enable_structural_plasticity: bool = True,
                       enable_curiosity: bool = True) -> 'NLMConfigBuilder':
        """Enable learning subsystems."""
        self.config['learning'] = {
            'reward_modulation': {'enabled': enable_reward_modulation},
            'structural_plasticity': {'enabled': enable_structural_plasticity},
            'curiosity': {'enabled': enable_curiosity},
        }
        return self
        
    def build(self) -> Dict:
        """Build final configuration."""
        return self.config
        
    def save(self, filepath: str) -> Any:
    """
    Function: Save configuration to file.
    """""
        with open(filepath, 'w') as f:
            json.dump(self.config, f, indent=2)
        return self
class NLMExperimentSuite:
    """
    Class: 
    Class: 
    Class: Suite of predefined NLM experiments.
    
    
    """""""""
    
    @staticmethod
    def create_connectivity_experiment(config: Optional[Dict] = None) -> Dict:
        """Create connectivity experiment configuration."""
        if config is None:
            config = NLMConfigBuilder().brain_config(500, 1, 0.15).build()
            
        config['experiment'] = {
            'name': 'connectivity_test',
            'description': 'Test neural connectivity and spike propagation',
            'steps': 100,
            'inject_current': 50.0,
        }
        
        return config
        
    @staticmethod
    def create_plasticity_experiment(config: Optional[Dict] = None) -> Dict:
        """Create plasticity experiment configuration."""
        if config is None:
            config = NLMConfigBuilder().brain_config(1000, 1, 0.1).build()
            config['plasticity']['stdp'] = {'enabled': True, 'learning_rate': 0.01}
            
        config['experiment'] = {
            'name': 'plasticity_learning',
            'description': 'Demonstrate synaptic changes through experience',
            'steps': 1000,
            'enable_plasticity': True,
        }
        
        return config
        
    @staticmethod
    def create_stdp_experiment(config: Optional[Dict] = None) -> Dict:
        """Create STDP-specific experiment configuration."""
        if config is None:
            config = NLMConfigBuilder().brain_config(800, 1, 0.12).build()
            
        config['experiment'] = {
            'name': 'stdp_verification',
            'description': 'Verify spike-timing dependent plasticity',
            'steps': 200,
            'pre_post_timing': 2.0,  # ms difference between pre and post spikes
        }
        
        return config
        
    @staticmethod
    def create_vision_experiment(config: Optional[Dict] = None) -> Dict:
        """Create vision processing experiment configuration."""
        if config is None:
            config = NLMConfigBuilder().brain_config(1200, 1, 0.08).build()
            
        config['experiment'] = {
            'name': 'vision_processing',
            'description': 'Test visual input processing and feature extraction',
            'steps': 500,
            'vision_enabled': True,
        }
        
        return config
        
    @staticmethod
    def create_development_experiment(config: Optional[Dict] = None) -> Dict:
        """Create development experiment configuration."""
        if config is None:
            config = NLMConfigBuilder().brain_config(1500, 1, 0.05).build()
            
        config['experiment'] = {
            'name': 'development_dynamics',
            'description': 'Study developmental changes over time',
            'steps': 3000,
            'development_enabled': True,
            'stages': ['initial', 'critical_period', 'maturation', 'adult'],
        }
        
        return config
# Example usage functions
def example_basic_brain() -> Any:
    """
    Function: Example: Create and run a basic brain.
    """""
    import pynlm
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
    # print(f"Basic brain: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSpikeCount()} spikes")
    
    return brain

def example_agent_simulation() -> Any:
    """
    Function: Example: Run agent in world simulation.
    """""
    import pynlm
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
    # print(f"Agent simulation complete: {brain.getTotalSpikeCount()} spikes")
    
    return brain, world, agent
def main() -> Any:
    """
    Function: Example usage of NLM utility functions.
    """""
    # print("NLM Python Utilities Examples")
    # print("=" * 40)
    
    # Example 1: Basic brain
    # print("\n1. Basic Brain Example:")
    example_basic_brain()
    
    # Example 2: Agent simulation
    # print("\n2. Agent Simulation Example:")
    example_agent_simulation()
    
    # Example 3: Configuration builder
    # print("\n3. Configuration Builder Example:")
    builder = NLMConfigBuilder()
    config = (builder
              .brain_config(1000, 1, 0.1)
              .dynamics_config(tau=20.0, resistance=10.0)
              .plasticity_config(stdp_enabled=True, learning_rate=0.001)
              .enable_learning()
              .build())
    # print(f"Built config: {config}")
    
    # print("\nAll examples completed successfully!")
if __name__ == "__main__":
    main()
