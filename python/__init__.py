class BrainSession:
    """Session management class for running complete NLM simulations.
    
    Provides a high-level interface for managing the entire simulation
    lifecycle from initialization to result analysis.
    """
    
    def __init__(self, config: Optional[Config] = None, 
                 world: Optional[SimpleWorld] = None,
                 agent: Optional[AgentBrain] = None):
        """Initialize a simulation session.
        
        Args:
            config: Brain configuration. If None, creates default.
            world: SimpleWorld instance. If None, creates default.
            agent: AgentBrain instance. If None, creates one with brain.
        """
        self.config = config or createDefaultConfig()
        self.brain = None
        self.world = world or createSimpleWorld()
        self.agent = agent
        
        self.step_count = 0
        self.start_time = time.time()
        self.results = []
        self.status = "initialized"
        
    def setup(self, enable_all_features: bool = True) -> 'BrainSession':
        """Setup the brain session with all components.
        
        Args:
            enable_all_features: Whether to enable all advanced subsystems.
            
        Returns:
            Self for method chaining.
        """
        self.status = "setting up"
        
        # Create brain with config
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        # Create agent if not provided
        if not self.agent:
            self.agent = createAgentBrain(self.brain)
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Enable all features if requested
        if enable_all_features:
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)
        
        self.status = "ready"
        return self
        
    def run_steps(self, num_steps: int, timestep: float = 0.1) -> 'BrainSession':
        """Run simulation for specified number of steps.
        
        Args:
            num_steps: Number of simulation steps to run.
            timestep: Time step size in seconds.
            
        Returns:
            Self for method chaining.
        """
        if self.status != "ready":
            raise RuntimeError(f"Session not ready. Status: {self.status}")
            
        self.status = "running"
        
        for step in range(num_steps):
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Record results
            self._record_step_result(step, percept, action)
        
        self.step_count = num_steps
        self.status = "completed"
        return self
        
    def _record_step_result(self, step: int, percept, action) -> None:
        """Record results for a single step."""
        result = {
            'step': step,
            'time': step * 0.1,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'action_type': action.getType() if hasattr(action, 'getType') else None
        }
        self.results.append(result)
        
    def get_stats(self) -> Dict:
        """Get session statistics.
        
        Returns:
            Dictionary with session statistics.
        """
        if not self.results:
            return {}
            
        duration = time.time() - self.start_time
        stats = {
            'total_steps': self.step_count,
            'duration_seconds': duration,
            'steps_per_second': self.step_count / duration if duration > 0 else 0,
            'final_firing_rate': self.results[-1]['avg_firing_rate'],
            'total_spikes': self.results[-1]['total_spikes'],
            'final_curiosity': self.results[-1]['curiosity'],
            'final_novelty': self.results[-1]['novelty'],
            'development_stage': self.results[-1]['development_stage'],
            'results_count': len(self.results)
        }
        
        return stats
        
    def save_state(self, filepath: str) -> None:
        """Save session state to file.
        
        Args:
            filepath: Path to save state file.
        """
        self.brain.save(filepath)
        
    def load_state(self, filepath: str) -> 'BrainSession':
        """Load session state from file.
        
        Args:
            filepath: Path to state file.
            
        Returns:
            Self for method chaining.
        """
        self.brain.load(filepath)
        return self
        
    def __enter__(self) -> 'BrainSession':
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.status = "closed"


class SimulationResult:
    """Wrapper for simulation results with analysis methods."""
    
    def __init__(self, session: BrainSession):
        self.session = session
        self.data = session.results
        
    def get_metrics(self) -> Dict:
        """Get computed metrics from results."""
        if not self.data:
            return {}
            
        steps = [r['step'] for r in self.data]
        firing_rates = [r['avg_firing_rate'] for r in self.data]
        curiosity_levels = [r['curiosity'] for r in self.data]
        novelty_levels = [r['novelty'] for r in self.data]
        prediction_errors = [r['prediction_error'] for r in self.data]
        
        return {
            'steps': steps,
            'firing_rates': firing_rates,
            'curiosity_levels': curiosity_levels,
            'novelty_levels': novelty_levels,
            'prediction_errors': prediction_errors,
            'mean_firing_rate': np.mean(firing_rates),
            'mean_curiosity': np.mean(curiosity_levels),
            'mean_novelty': np.mean(novelty_levels),
            'mean_prediction_error': np.mean(prediction_errors),
            'final_firing_rate': firing_rates[-1],
            'final_curiosity': curiosity_levels[-1],
            'final_novelty': novelty_levels[-1],
            'final_prediction_error': prediction_errors[-1],
            'firing_rate_range': [np.min(firing_rates), np.max(firing_rates)],
            'curiosity_range': [np.min(curiosity_levels), np.max(curiosity_levels)],
            'novelty_range': [np.min(novelty_levels), np.max(novelty_levels)],
            'prediction_error_range': [np.min(prediction_errors), np.max(prediction_errors)]
        }
        
    def analyze_convergence(self, window_size: int = 100) -> Dict:
        """Analyze convergence of learning metrics."""
        if len(self.data) < window_size:
            return {'converged': False, 'reason': 'Not enough data'}
            
        # Analyze convergence of curiosity and novelty
        recent_curiosity = [r['curiosity'] for r in self.data[-window_size:]]
        recent_novelty = [r['novelty'] for r in self.data[-window_size:]]
        
        # Check if metrics are stabilizing (low variance in recent window)
        curiosity_var = np.var(recent_curiosity)
        novelty_var = np.var(recent_novelty)
        
        converged = curiosity_var < 0.01 and novelty_var < 0.01
        
        return {
            'converged': converged,
            'curiosity_variance': curiosity_var,
            'novelty_variance': novelty_var,
            'window_size': window_size,
            'recent_mean_curiosity': np.mean(recent_curiosity),
            'recent_mean_novelty': np.mean(recent_novelty)
        }


# Global result analyzer for convenience
_analyzer = None

def _get_analyzer() -> ResultAnalyzer:
    global _analyzer
    if _analyzer is None:
        _analyzer = ResultAnalyzer()
    return _analyzer


class ResultAnalyzer:
    """Static analyzer for NLM simulation results."""
    
    @staticmethod
    def format_metrics(metrics: Dict) -> str:
        """Format metrics for display."""
        lines = ["Simulation Metrics:"]
        lines.append(f"  Total Steps: {metrics.get('total_steps', 'N/A')}")
        lines.append(f"  Final Firing Rate: {metrics.get('final_firing_rate', 0):.3f}")
        lines.append(f"  Mean Curiosity: {metrics.get('mean_curiosity', 0):.3f}")
        lines.append(f"  Mean Novelty: {metrics.get('mean_novelty', 0):.3f}")
        lines.append(f"  Mean Prediction Error: {metrics.get('mean_prediction_error', 0):.3f}")
        lines.append(f"  Development Stage: {metrics.get('development_stage', 'N/A')}")
        return "\\n".join(lines)
        
    @staticmethod
    def compare_sessions(session1: BrainSession, session2: BrainSession) -> Dict:
        """Compare two simulation sessions."""
        stats1 = session1.get_stats()
        stats2 = session2.get_stats()
        
        comparison = {
            'duration_comparison': {
                'session1': stats1.get('duration_seconds', 0),
                'session2': stats2.get('duration_seconds', 0),
                'faster': 1 if stats1.get('steps_per_second', 0) > stats2.get('steps_per_second', 0) else 2
            },
            'performance_comparison': {
                'session1': {
                    'final_firing_rate': stats1.get('final_firing_rate', 0),
                    'total_spikes': stats1.get('total_spikes', 0)
                },
                'session2': {
                    'final_firing_rate': stats2.get('final_firing_rate', 0),
                    'total_spikes': stats2.get('total_spikes', 0)
                }
            }
        }
        
        return comparison


# High-level factory functions
def createSimpleAgent(config: Optional[Config] = None) -> AgentBrain:
    """Create a simple agent with one line of code.
    
    This is the easiest way to create an agent for basic simulations.
    
    Args:
        config: Optional brain configuration. If None, uses default.
        
    Returns:
        Configured AgentBrain instance ready for use.
        
    Example:
        >>> agent = createSimpleAgent()
        >>> brain = agent.getBrain()
        >>> brain.initialize()
    """
    if config is None:
        config = createDefaultConfig()
    
    brain = createBrain(config)
    agent = createAgentBrain(brain)
    
    # Enable basic features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    return agent


def createDefaultConfig(profile: str = "balanced") -> Config:
    """Create configuration with predefined profiles.
    
    Args:
        profile: Profile type. One of: "explorer", "learner", "developer", "benchmark", "balanced"
        
    Returns:
        Configured Config instance with profile-specific settings.
        
    Raises:
        ValueError: If profile name is not recognized.
    """
    config = Config()
    
    if profile == "explorer":
        # High curiosity, fast exploration
        config.set("brain.neuron_count", 2000)
        config.set("neuromodulation.curiosity.weight", 1.0)
        config.set("neuromodulation.novelty.threshold", 0.05)
        config.set("memory.capacity", 2000)
        config.set("plasticity.structural.enable", True)
        
    elif profile == "learner":
        # High learning rate, good memory
        config.set("brain.neuron_count", 3000)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("memory.capacity", 5000)
        config.set("memory.episodic.max_episodes", 1000)
        config.set("neuromodulation.dopamine.base", 0.2)
        
    elif profile == "developer":
        # Emphasizes development and maturation
        config.set("brain.neuron_count", 1500)
        config.set("development.critical_period_duration", 1000)
        config.set("development.maturation_rate", 0.01)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
    elif profile == "benchmark":
        # Optimized for performance testing
        config.set("brain.neuron_count", 500)
        config.set("simulation.timestep", 0.01)
        config.set("memory.capacity", 1000)
        config.set("plasticity.stdp.enable", False)
        config.set("neuromodulation.curiosity.enable", False)
        
    elif profile == "balanced":
        # Default balanced configuration
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("memory.capacity", 3000)
        config.set("neuromodulation.curiosity.weight", 0.5)
        config.set("neuromodulation.novelty.threshold", 0.1)
        
    else:
        raise ValueError(f"Unknown profile: {profile}. Choose from: explorer, learner, developer, benchmark, balanced")
    
    return config


def createExplorerConfig() -> Config:
    """Create an explorer configuration (alias for createDefaultConfig with profile='explorer')."""
    return createDefaultConfig("explorer")


def createLearnerConfig() -> Config:
    """Create a learner configuration (alias for createDefaultConfig with profile='learner')."""
    return createDefaultConfig("learner")


def createDeveloperConfig() -> Config:
    """Create a developer configuration (alias for createDefaultConfig with profile='developer')."""
    return createDefaultConfig("developer")


def createBenchmarkConfig() -> Config:
    """Create a benchmark configuration (alias for createDefaultConfig with profile='benchmark')."""
    return createDefaultConfig("benchmark")


def runSimulation(config: Optional[Config] = None, 
                world_config: Optional[Dict] = None,
                num_steps: int = 1000,
                timestep: float = 0.1,
                profile: str = "balanced") -> SimulationResult:
    """High-level simulation runner with result analysis.
    
    Runs a complete simulation with brain, world, and agent automatically
    configured and returns analysis of results.
    
    Args:
        config: Optional brain configuration. If None, uses profile-based config.
        world_config: Optional world configuration dict with keys: width, height, 
                     visionWidth, visionHeight.
        num_steps: Number of simulation steps to run.
        timestep: Time step size in seconds.
        profile: Configuration profile if config is None.
        
    Returns:
        SimulationResult object with analysis of simulation results.
        
    Example:
        >>> result = runSimulation(num_steps=500, profile="explorer")
        >>> print(f"Final firing rate: {result.get_metrics()['final_firing_rate']:.3f}")
    """
    if config is None:
        config = createDefaultConfig(profile)
    
    # Setup session
    session = BrainSession(config)
    
    # Configure world if specified
    if world_config:
        session.world.configure(**world_config)
    
    # Setup and run
    session.setup()
    session.run_steps(num_steps, timestep)
    
    return SimulationResult(session)


def PlotResults(results: Union[SimulationResult, Dict], 
              save_path: Optional[str] = None,
              show: bool = True) -> plt.Figure:
    """Generate performance plots from simulation results.
    
    Creates comprehensive plots showing learning curves and performance metrics.
    
    Args:
        results: SimulationResult object or metrics dictionary.
        save_path: Optional path to save the plot.
        show: Whether to display the plot.
        
    Returns:
        Matplotlib figure object.
        
    Example:
        >>> result = runSimulation(100)
        >>> PlotResults(result, save_path="simulation_plot.png")
    """
    if isinstance(results, SimulationResult):
        metrics = results.get_metrics()
        data = results.session.data
    else:
        metrics = results
        data = []
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle("NLM Simulation Results", fontsize=16, fontweight='bold')
    
    # Plot 1: Firing rate over time
    if data:
        steps = [r['step'] for r in data]
        firing_rates = [r['avg_firing_rate'] for r in data]
        axes[0, 0].plot(steps, firing_rates, linewidth=1.5)
        axes[0, 0].set_xlabel("Step")
        axes[0, 0].set_ylabel("Average Firing Rate")
        axes[0, 0].set_title("Learning Curve")
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Curiosity and Novelty
        curiosity = [r['curiosity'] for r in data]
        novelty = [r['novelty'] for r in data]
        axes[0, 1].plot(steps, curiosity, label='Curiosity', linewidth=1.5)
        axes[0, 1].plot(steps, novelty, label='Novelty', linewidth=1.5)
        axes[0, 1].set_xlabel("Step")
        axes[0, 1].set_ylabel("Level")
        axes[0, 1].set_title("Exploration Drives")
        axes[0, 1].legend()
        axes[0, 1].grid(True, alpha=0.3)
        
        # Plot 3: Prediction error
        pred_errors = [r['prediction_error'] for r in data]
        axes[1, 0].plot(steps, pred_errors, color='red', linewidth=1.5)
        axes[1, 0].set_xlabel("Step")
        axes[1, 0].set_ylabel("Prediction Error")
        axes[1, 0].set_title("Prediction Accuracy")
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Development stage changes
        stages = []
        for r in data:
            stage = r.get('development_stage')
            if hasattr(stage, 'value'):
                stages.append(stage.value)
            else:
                stages.append(str(stage))
        
        # Show development stage as horizontal lines with annotations
        unique_stages = list(set(stages))
        for i, stage in enumerate(unique_stages):
            if stage != 'N/A':
                y_pos = len(unique_stages) - i - 1
                axes[1, 1].axhline(y=y_pos, color='green', alpha=0.3, linewidth=0.5)
                axes[1, 1].text(len(steps) - 1, y_pos, str(stage), 
                               verticalalignment='center', fontsize=8)
        
        axes[1, 1].set_xlabel("Step")
        axes[1, 1].set_ylabel("Development Stage")
        axes[1, 1].set_title("Developmental Progress")
        axes[1, 1].set_yticks(range(len(unique_stages)))
        axes[1, 1].set_yticklabels(unique_stages)
        axes[1, 1].grid(True, alpha=0.3)
    
    # Adjust layout
    plt.tight_layout()
    
    # Save if requested
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
    
    # Show if requested
    if show:
        plt.show()
    
    return fig


def plotPerformance(session: BrainSession, 
                   metrics: List[str] = None,
                   save_path: Optional[str] = None) -> plt.Figure:
    """Plot performance metrics for a session.
    
    Args:
        session: BrainSession to plot.
        metrics: List of metric names to plot. If None, plots all available.
        save_path: Optional path to save the plot.
        
    Returns:
        Matplotlib figure object.
    """
    if metrics is None:
        metrics = ['firing_rate', 'curiosity', 'novelty', 'prediction_error']
    
    fig, axes = plt.subplots(len(metrics), 1, figsize=(10, 3 * len(metrics)))
    if len(metrics) == 1:
        axes = [axes]
    
    for ax, metric in zip(axes, metrics):
        data = session.results
        if metric == 'firing_rate':
            values = [r['avg_firing_rate'] for r in data]
            ylabel = 'Average Firing Rate'
        elif metric == 'curiosity':
            values = [r['curiosity'] for r in data]
            ylabel = 'Curiosity Level'
        elif metric == 'novelty':
            values = [r['novelty'] for r in data]
            ylabel = 'Novelty Level'
        elif metric == 'prediction_error':
            values = [r['prediction_error'] for r in data]
            ylabel = 'Prediction Error'
        else:
            continue
        
        ax.plot(range(len(values)), values, linewidth=1.5)
        ax.set_xlabel('Step')
        ax.set_ylabel(ylabel)
        ax.set_title(f'{metric.replace("_", " ").title()} Over Time')
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
    
    plt.show()
    
    return fig


def plotLearningCurve(result: SimulationResult,
                     save_path: Optional[str] = None) -> plt.Figure:
    """Plot learning curve from simulation result.
    
    Args:
        result: SimulationResult object.
        save_path: Optional path to save the plot.
        
    Returns:
        Matplotlib figure object.
    """
    return PlotResults(result, save_path=save_path)


class Benchmark:
    """Benchmark class for comparing configurations."""
    
    @staticmethod
    def run_config(config: Config, 
                  num_steps: int = 100,
                  world_config: Optional[Dict] = None,
                  description: str = "") -> Dict:
        """Run a single configuration benchmark.
        
        Args:
            config: Configuration to benchmark.
            num_steps: Number of steps to simulate.
            world_config: Optional world configuration.
            description: Description of this benchmark.
            
        Returns:
            Dictionary with benchmark results.
        """
        start_time = time.time()
        
        # Create and setup session
        session = BrainSession(config)
        if world_config:
            session.world.configure(**world_config)
        
        session.setup()
        session.run_steps(num_steps)
        
        end_time = time.time()
        
        # Get results
        stats = session.get_stats()
        metrics = SimulationResult(session).get_metrics()
        
        return {
            'description': description,
            'config_profile': getattr(config, 'profile', 'custom'),
            'duration_seconds': end_time - start_time,
            'steps_per_second': stats.get('steps_per_second', 0),
            'final_firing_rate': metrics.get('final_firing_rate', 0),
            'mean_curiosity': metrics.get('mean_curiosity', 0),
            'mean_novelty': metrics.get('mean_novelty', 0),
            'mean_prediction_error': metrics.get('mean_prediction_error', 0),
            'total_spikes': stats.get('total_spikes', 0),
            'steps_run': num_steps,
            'session': session
        }
        
    @staticmethod
    def compare_configurations(configs: List[Config],
                              num_steps: int = 100,
                              world_config: Optional[Dict] = None,
                              descriptions: Optional[List[str]] = None) -> Dict:
        """Compare multiple configurations.
        
        Args:
            configs: List of configurations to compare.
            num_steps: Number of steps for each benchmark.
            world_config: Optional world configuration.
            descriptions: Optional list of descriptions for each config.
            
        Returns:
            Dictionary with comparison results.
        """
        if descriptions is None:
            descriptions = [f"Config {i+1}" for i in range(len(configs))]
        
        results = []
        for config, desc in zip(configs, descriptions):
            result = Benchmark.run_config(config, num_steps, world_config, desc)
            results.append(result)
        
        # Create comparison summary
        comparison = {
            'configs': [r['description'] for r in results],
            'performance': {
                'speed': [r['steps_per_second'] for r in results],
                'firing_rate': [r['final_firing_rate'] for r in results],
                'curiosity': [r['mean_curiosity'] for r in results],
                'novelty': [r['mean_novelty'] for r in results],
                'efficiency': [r['total_spikes'] / r['duration_seconds'] if r['duration_seconds'] > 0 else 0 for r in results]
            },
            'details': results
        }
        
        return comparison


def compareConfigurations(configs: List[Config],
                         num_steps: int = 100,
                         world_config: Optional[Dict] = None,
                         descriptions: Optional[List[str]] = None) -> Dict:
    """Compare multiple configurations (convenience function).
    
    Args:
        configs: List of configurations to compare.
        num_steps: Number of steps for each benchmark.
        world_config: Optional world configuration.
        descriptions: Optional list of descriptions for each config.
        
    Returns:
        Dictionary with comparison results.
        
    Example:
        >>> configs = [createDefaultConfig("explorer"), createDefaultConfig("learner")]
        >>> comparison = compareConfigurations(configs, descriptions=["Explorer", "Learner"])
        >>> print(f"Explorer speed: {comparison['performance']['speed'][0]:.1f} steps/sec")
    """
    return Benchmark.compare_configurations(configs, num_steps, world_config, descriptions)


# Initialize default config for backward compatibility
_default_config = None

def getDefaultConfig() -> Config:
    """Get the default configuration (backward compatibility).
    
    Returns:
        Default configuration.
        
    Deprecated:
        Use createDefaultConfig() instead.
    """
    global _default_config
    if _default_config is None:
        _default_config = createDefaultConfig()
    return _default_config


# Monkey-patch the existing functions for backward compatibility
import sys
if 'pynlm' in sys.modules:
    original_module = sys.modules['pynlm']
    if hasattr(original_module, 'createDefaultConfig'):
        createDefaultConfig = original_module.createDefaultConfig
    if hasattr(original_module, 'createBrain'):
        createBrain = original_module.createBrain
    if hasattr(original_module, 'createSimpleWorld'):
        createSimpleWorld = original_module.createSimpleWorld
    if hasattr(original_module, 'createAgentBrain'):
        createAgentBrain = original_module.createAgentBrain
