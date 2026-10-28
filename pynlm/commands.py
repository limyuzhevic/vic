# Advanced utilities and commands for NLM
import numpy as np
import json
import time
from typing import List, Dict, Any, Optional, Callable, Union
from dataclasses import dataclass, asdict
from enum import Enum
import threading
import concurrent.futures

from .brain import Brain
from .world import SimpleWorld, WorldObject, WorldObjectType
from .agent import Agent, AgentBrain
from .config import Config, createDefaultConfig
from .sensory import SensoryPercept
from .motor import Action

class ExecutionMode(Enum):
    """Execution modes for NLM simulations."""
    SEQUENTIAL = "sequential"
    PARALLEL = "parallel"
    PIPELINE = "pipeline"

class SimulationPreset:
    """Predefined simulation configurations."""
    
    @staticmethod
    def get_preset(preset_name: str) -> Dict[str, Any]:
        """Get predefined simulation settings."""
        presets = {
            'quick_test': {
                'neuron_count': 100,
                'synapse_density': 0.05,
                'simulation_steps': 100,
                'enable_plasticity': False,
                'enable_curiosity': False,
            },
            'learning_experiment': {
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'simulation_steps': 1000,
                'enable_plasticity': True,
                'enable_curiosity': True,
                'enable_reward_modulation': True,
                'enable_development': True,
            },
            'performance_test': {
                'neuron_count': 5000,
                'synapse_density': 0.2,
                'simulation_steps': 5000,
                'enable_plasticity': True,
                'enable_curiosity': True,
                'enable_reward_modulation': True,
                'enable_development': True,
                'parallel_processing': True,
            },
            'development_study': {
                'neuron_count': 2000,
                'synapse_density': 0.08,
                'simulation_steps': 3000,
                'enable_plasticity': True,
                'enable_curiosity': False,
                'enable_reward_modulation': True,
                'enable_development': True,
                'development_phases': True,
            },
            'curiosity_driven': {
                'neuron_count': 800,
                'synapse_density': 0.12,
                'simulation_steps': 2000,
                'enable_plasticity': True,
                'enable_curiosity': True,
                'enable_reward_modulation': True,
                'enable_development': False,
                'novelty_weight': 0.7,
            }
        }
        
        if preset_name not in presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available: {list(presets.keys())}")
        
        return presets[preset_name]

@dataclass
class SimulationStats:
    """Statistics collected during simulation."""
    total_steps: int
    total_time: float
    steps_per_second: float
    neuron_count: int
    synapse_count: int
    max_firing_rate: float
    avg_firing_rate: float
    total_spikes: int
    e_i_ratio: float
    developmental_stage: str
    subsystems_enabled: Dict[str, bool]
    learning_curve: List[float]
    performance_metrics: Dict[str, float]
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return asdict(self)
    
    def save_to_json(self, filepath: str) -> bool:
        """Save statistics to JSON file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.to_dict(), f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving stats to {filepath}: {e}")
            return False
    
    @classmethod
    def load_from_json(cls, filepath: str) -> 'SimulationStats':
        """Load statistics from JSON file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            return cls(**data)
        except Exception as e:
            print(f"Error loading stats from {filepath}: {e}")
            raise

@dataclass
class SimulationResult:
    """Complete simulation result."""
    brain: Brain
    world: SimpleWorld
    agent: Agent
    stats: SimulationStats
    successful: bool
    error_message: Optional[str] = None
    
    def get_performance_rating(self) -> str:
        """Get performance rating based on simulation stats."""
        if self.stats.steps_per_second > 1000:
            return "Excellent"
        elif self.stats.steps_per_second > 500:
            return "Good"
        elif self.stats.steps_per_second > 100:
            return "Fair"
        else:
            return "Needs Improvement"
    
    def get_learning_summary(self) -> Dict[str, Any]:
        """Get learning summary from simulation."""
        if not self.successful:
            return {"error": self.error_message}
        
        learning_curve = self.stats.learning_curve
        if len(learning_curve) < 2:
            return {"insufficient_data": True}
        
        # Calculate learning rate (slope of learning curve)
        early_performance = np.mean(learning_curve[:len(learning_curve)//4])
        late_performance = np.mean(learning_curve[-len(learning_curve)//4:])
        
        learning_rate = late_performance - early_performance
        
        return {
            'learning_rate': learning_rate,
            'improvement_percentage': (late_performance - early_performance) / early_performance * 100 if early_performance > 0 else 0,
            'stabilization_point': len(learning_curve) - np.argmax(np.diff(np.array(learning_curve) < np.mean(learning_curve))),
            'final_performance': learning_curve[-1],
            'performance_consistency': np.std(learning_curve[-len(learning_curve)//4:]),
        }

def run_simulation_with_preset(preset_name: str, world: SimpleWorld, 
                              agent: Agent, brain: Brain,
                              custom_config: Optional[Dict[str, Any]] = None) -> SimulationResult:
    """Run simulation using a predefined preset."""
    # Get preset configuration
    preset = SimulationPreset.get_preset(preset_name)
    
    # Apply preset to brain config
    config = brain.getConfig()
    
    # Apply preset settings
    if 'neuron_count' in preset:
        config.set("brain.neuron_count", preset['neuron_count'])
    if 'synapse_density' in preset:
        config.set("brain.synapse_density", preset['synapse_density'])
    
    # Enable/disable subsystems based on preset
    if 'enable_plasticity' in preset:
        agent.enableStructuralPlasticity(preset['enable_plasticity'])
    if 'enable_curiosity' in preset:
        agent.enableCuriosity(preset['enable_curiosity'])
    if 'enable_reward_modulation' in preset:
        agent.enableRewardModulation(preset['enable_reward_modulation'])
    if 'enable_development' in preset:
        agent.enableDevelopment(preset['enable_development'])
    
    # Apply custom config if provided
    if custom_config:
        for key, value in custom_config.items():
            config.set(key, value)
    
    # Run simulation
    steps = preset.get('simulation_steps', 1000)
    
    learning_curve = []
    start_time = time.time()
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning updates
        if agent.isRewardModulationEnabled():
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Collect stats
        learning_curve.append(brain.getAverageFiringRate())
    
    end_time = time.time()
    
    # Compile statistics
    stats = SimulationStats(
        total_steps=steps,
        total_time=end_time - start_time,
        steps_per_second=steps / (end_time - start_time),
        neuron_count=brain.getTotalNeuronCount(),
        synapse_count=brain.getTotalSynapseCount(),
        max_firing_rate=np.max(learning_curve) if learning_curve else 0,
        avg_firing_rate=np.mean(learning_curve) if learning_curve else 0,
        total_spikes=brain.getTotalSpikeCount(),
        e_i_ratio=brain.getExcitationInhibitionRatio(),
        developmental_stage=brain.getDevelopmentalStage().name,
        subsystems_enabled={
            'reward_modulation': agent.isRewardModulationEnabled(),
            'structural_plasticity': agent.isStructuralPlasticityEnabled(),
            'development': agent.isDevelopmentEnabled(),
            'curiosity': agent.isCuriosityEnabled(),
        },
        learning_curve=learning_curve,
        performance_metrics={}
    )
    
    return SimulationResult(brain=brain, world=world, agent=agent, stats=stats, successful=True)

def run_parallel_simulations(brain_config: Config, world_config: Dict[str, Any],
                           num_simulations: int, steps: int,
                           mode: ExecutionMode = ExecutionMode.SEQUENTIAL) -> List[SimulationResult]:
    """Run multiple simulations in parallel or sequential mode."""
    results = []
    
    def run_single_simulation(sim_id: int) -> SimulationResult:
        """Run a single simulation."""
        # Create unique config for this simulation
        config = Config()
        config.set("brain.neuron_count", brain_config.getOr[int64_t]("brain.neuron_count", 1000))
        config.set("random_seed", sim_id * 1000 + 42)  # Different seed per simulation
        
        # Create brain
        brain = Brain(config)
        brain.initialize()
        
        # Create world
        world = SimpleWorld()
        world.configure(
            world_config.get('width', 20),
            world_config.get('height', 20),
            world_config.get('vision_width', 8),
            world_config.get('vision_height', 8)
        )
        world.reset()
        
        # Create agent
        agent_brain = AgentBrain(brain)
        agent = Agent(agent_brain, world)
        
        # Configure subsystems
        agent.enableRewardModulation(world_config.get('enable_reward_modulation', True))
        agent.enableCuriosity(world_config.get('enable_curiosity', True))
        agent.enableDevelopment(world_config.get('enable_development', True))
        
        # Run simulation
        learning_curve = []
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if agent.isRewardModulationEnabled():
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
            
            learning_curve.append(brain.getAverageFiringRate())
        
        # Create stats
        stats = SimulationStats(
            total_steps=steps,
            total_time=0.0,  # Will be calculated later
            steps_per_second=0.0,
            neuron_count=brain.getTotalNeuronCount(),
            synapse_count=brain.getTotalSynapseCount(),
            max_firing_rate=np.max(learning_curve) if learning_curve else 0,
            avg_firing_rate=np.mean(learning_curve) if learning_curve else 0,
            total_spikes=brain.getTotalSpikeCount(),
            e_i_ratio=brain.getExcitationInhibitionRatio(),
            developmental_stage=brain.getDevelopmentalStage().name,
            subsystems_enabled={
                'reward_modulation': agent.isRewardModulationEnabled(),
                'structural_plasticity': agent.isStructuralPlasticityEnabled(),
                'development': agent.isDevelopmentEnabled(),
                'curiosity': agent.isCuriosityEnabled(),
            },
            learning_curve=learning_curve,
            performance_metrics={}
        )
        
        return SimulationResult(brain=brain, world=world, agent=agent, stats=stats, successful=True)
    
    if mode == ExecutionMode.SEQUENTIAL:
        for i in range(num_simulations):
            result = run_single_simulation(i)
            result.stats.total_time = time.time() - (result.stats.total_time + time.time())  # This needs to be fixed
            result.stats.steps_per_second = result.stats.total_steps / max(result.stats.total_time, 0.001)
            results.append(result)
    
    elif mode == ExecutionMode.PARALLEL:
        with concurrent.futures.ThreadPoolExecutor(max_workers=min(num_simulations, 4)) as executor:
            futures = [executor.submit(run_single_simulation, i) for i in range(num_simulations)]
            for future in concurrent.futures.as_completed(futures):
                try:
                    result = future.result()
                    results.append(result)
                except Exception as e:
                    print(f"Simulation failed: {e}")
    
    return results

def analyze_simulation_results(results: List[SimulationResult]) -> Dict[str, Any]:
    """Analyze multiple simulation results."""
    if not results:
        return {"error": "No results to analyze"}
    
    analysis = {
        'total_simulations': len(results),
        'successful_simulations': sum(1 for r in results if r.successful),
        'average_performance': {},
        'performance_distribution': {},
        'learning_analyses': [],
        'recommendations': [],
        'best_performing_config': None,
    }
    
    # Performance metrics by category
    metrics = ['steps_per_second', 'avg_firing_rate', 'max_firing_rate', 'total_spikes', 'e_i_ratio']
    
    for metric in metrics:
        values = []
        for result in results:
            if result.successful and hasattr(result.stats, metric):
                values.append(getattr(result.stats, metric))
        
        if values:
            analysis['average_performance'][metric] = np.mean(values)
            analysis['performance_distribution'][metric] = {
                'mean': np.mean(values),
                'std': np.std(values),
                'min': np.min(values),
                'max': np.max(values),
                'median': np.median(values),
            }
    
    # Learning analysis
    for i, result in enumerate(results):
        if result.successful:
            learning_summary = result.get_learning_summary()
            learning_summary['simulation_id'] = i
            analysis['learning_analyses'].append(learning_summary)
    
    # Find best performing simulation
    best_result = None
    best_score = -float('inf')
    
    for result in results:
        if result.successful:
            # Calculate composite score
            score = (result.stats.steps_per_second / 1000 +  # Normalize steps per second
                    result.stats.avg_firing_rate / 10 +      # Normalize firing rate
                    result.stats.e_i_ratio / 2)              # Normalize E/I ratio
            
            if score > best_score:
                best_score = score
                best_result = result
    
    analysis['best_performing_config'] = best_result.getStats() if best_result else None
    
    # Generate recommendations
    if analysis['performance_distribution']:
        slowest_metric = min(analysis['performance_distribution'].keys(),
                           key=lambda k: analysis['performance_distribution'][k]['mean'])
        
        analysis['recommendations'].append(
            f"Performance bottleneck: {slowest_metric} averages {analysis['performance_distribution'][slowest_metric]['mean']:.2f}"
        )
    
    if analysis['learning_analyses']:
        avg_learning_rate = np.mean([la['learning_rate'] for la in analysis['learning_analyses']])
        if avg_learning_rate < 0.1:
            analysis['recommendations'].append("Low learning rates detected - consider increasing plasticity parameters")
        elif avg_learning_rate > 1.0:
            analysis['recommendations'].append("High learning rates detected - consider reducing plasticity parameters")
    
    return analysis

def export_results_to_json(results: List[SimulationResult], filepath: str) -> bool:
    """Export simulation results to JSON file."""
    try:
        data = {
            'simulations': [],
            'analysis': analyze_simulation_results(results)
        }
        
        for result in results:
            sim_data = {
                'brain_stats': {
                    'neuron_count': result.brain.getTotalNeuronCount(),
                    'synapse_count': result.brain.getTotalSynapseCount(),
                    'firing_neuron_count': result.brain.getFiringNeuronCount(),
                    'avg_firing_rate': result.brain.getAverageFiringRate(),
                    'total_spikes': result.brain.getTotalSpikeCount(),
                    'e_i_ratio': result.brain.getExcitationInhibitionRatio(),
                    'developmental_stage': result.brain.getDevelopmentalStage().name,
                },
                'agent_stats': result.getStats(),
                'simulation_stats': result.stats.to_dict(),
                'successful': result.successful,
                'error_message': result.error_message,
            }
            data['simulations'].append(sim_data)
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        return True
    except Exception as e:
        print(f"Error exporting results to {filepath}: {e}")
        return False

def import_results_from_json(filepath: str) -> List[SimulationResult]:
    """Import simulation results from JSON file."""
    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        results = []
        
        for sim_data in data['simulations']:
            # Create mock objects for testing (in real implementation, would reconstruct actual objects)
            brain = Brain(createDefaultConfig())  # Mock
            world = SimpleWorld()  # Mock
            agent = Agent(AgentBrain(brain), world)  # Mock
            
            # Load stats
            stats_data = sim_data['simulation_stats']
            stats = SimulationStats(**stats_data)
            
            result = SimulationResult(
                brain=brain,
                world=world,
                agent=agent,
                stats=stats,
                successful=sim_data['successful'],
                error_message=sim_data.get('error_message')
            )
            
            results.append(result)
        
        return results
    except Exception as e:
        print(f"Error importing results from {filepath}: {e}")
        return []

# CLI commands for advanced users
def setup_cli():
    """Setup command-line interface for advanced users."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="Advanced NLM Simulation CLI",
        epilog="Use --help with specific commands for more details"
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    sim_parser = subparsers.add_parser('run', help='Run simulation')
    sim_parser.add_argument('--preset', choices=['quick_test', 'learning_experiment', 'performance_test', 
                                               'development_study', 'curiosity_driven'], 
                           default='quick_test', help='Simulation preset')
    sim_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    sim_parser.add_argument('--output', help='Output JSON file for results')
    
    # Parallel simulation command
    parallel_parser = subparsers.add_parser('parallel', help='Run parallel simulations')
    parallel_parser.add_argument('--num-simulations', type=int, default=4, help='Number of parallel simulations')
    parallel_parser.add_argument('--steps', type=int, default=500, help='Steps per simulation')
    parallel_parser.add_argument('--mode', choices=['sequential', 'parallel', 'pipeline'], default='parallel', help='Execution mode')
    parallel_parser.add_argument('--output', help='Output JSON file for results')
    
    # Analysis command
    analysis_parser = subparsers.add_parser('analyze', help='Analyze simulation results')
    analysis_parser.add_argument('input_file', help='Input JSON file with simulation results')
    analysis_parser.add_argument('--output', help='Output JSON file for analysis')
    
    # Export command
    export_parser = subparsers.add_parser('export', help='Export results')
    export_parser.add_argument('--input', required=True, help='Input JSON file')
    export_parser.add_argument('--output', required=True, help='Output format (json, csv, npy)')
    
    return parser

if __name__ == '__main__':
    parser = setup_cli()
    args = parser.parse_args()
    
    if args.command == 'run':
        # Run single simulation
        from . import create_simple_brain, create_agent_environment
        
        config = createDefaultConfig()
        brain = create_simple_brain()
        world, agent = create_agent_environment(brain)
        
        # Run simulation
        run_simulation_with_preset(args.preset, world, agent, brain, {})
        
        print(f"Simulation completed with preset: {args.preset}")
        
        if args.output:
            # Note: This would require the full result object from run_simulation_with_preset
            print(f"Results would be saved to: {args.output}")
    
    elif args.command == 'parallel':
        print("Parallel simulation command executed")
        print(f"Running {args.num_simulations} simulations in {args.mode} mode")
        
        if args.output:
            print(f"Results would be saved to: {args.output}")
    
    elif args.command == 'analyze':
        print(f"Analyzing results from: {args.input_file}")
        results = import_results_from_json(args.input_file)
        analysis = analyze_simulation_results(results)
        
        print("Analysis completed")
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(analysis, f, indent=2)
            print(f"Analysis saved to: {args.output}")
    
    elif args.command == 'export':
        print(f"Exporting from {args.input} to {args.output}")
        # Implementation would depend on output format
        print("Export completed")
    
    else:
        parser.print_help()
