# NLM Advanced Experimental Framework

from __future__ import annotations

import numpy as np
from typing import Any, Dict, List, Optional, Callable, Union
from enum import Enum

from .core import (
    Brain,
    Config,
    create_default_config,
    create_brain,
)
from .advanced_api import (
    AdvancedConfig,
    NeuralSimulation,
    CognitiveTest,
    MemoryTest,
    PlasticityTest,
    BrainAnalyzer,
    DynamicsProfiler,
    ConnectivityInspector,
)

__all__ = [
    "Phase3Demo",
    "Phase4Demo",
    "Phase6Demo",
    "SimulationExperiment",
    "ExplorationExperiment",
    "DevelopmentExperiment",
    "MemoryIntegrationExperiment",
    "PlasticityExperiment",
    "AdvancedResearchExperiment",
    "ComparativeExperiment",
]

class Phase3Demo:
    """Phase 3 world interaction demo with advanced API."""
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        self.config = config or AdvancedConfig()
        self.simulation = NeuralSimulation(self.config)
        self.name = "Phase 3 Demo - World Interaction"
        
    def run_demo(self, steps: int = 1000) -> Dict[str, Any]:
        """Run Phase 3 world interaction demo."""
        episode_data = self.simulation.run_episode(steps)
        
        # Process results
        brain_state = episode_data[-1]["brain_state"] if episode_data else {}
        world_state = episode_data[-1]["world_state"] if episode_data else {}
        agent_state = episode_data[-1]["agent_state"] if episode_data else {}
        
        # Calculate summary statistics
        total_rewards = sum(d["reward"] for d in episode_data)
        max_reward = max(d["reward"] for d in episode_data)
        min_reward = min(d["reward"] for d in episode_data)
        
        return {
            "demo": "Phase 3 - World Interaction",
            "steps": steps,
            "total_reward": total_rewards,
            "max_reward": max_reward,
            "min_reward": min_reward,
            "average_reward": total_rewards / steps if steps > 0 else 0,
            "brain_dynamics": {
                "neurons_active": brain_state.get("active_neurons", 0),
                "firing_rate": brain_state.get("average_firing_rate", 0),
                "plasticity": brain_state.get("excitation_inhibition_ratio", 1),
                "neuromodulation": brain_state.get("neuromodulation_level", 0),
            },
            "world_dynamics": {
                "agent_position": (agent_state.get("x", 0), agent_state.get("y", 0)),
                "agent_health": agent_state.get("health", 100),
                "agent_energy": agent_state.get("energy", 100),
            },
            "episode_data": episode_data,
        }
class Phase4Demo:
    """Phase 4 cognitive experiment demo with advanced API."""
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        self.config = config or AdvancedConfig()
        self.simulation = NeuralSimulation(self.config)
        self.name = "Phase 4 Demo - Cognitive Systems"
        
    def run_demo(self, steps: int = 1500) -> Dict[str, Any]:
        """Run Phase 4 cognitive experiment demo."""
        episode_data = self.simulation.run_episode(steps)
        
        # Run cognitive tests
        cognitive_tests = {
            "memory_retention": CognitiveTest.test_memory_retention(self.simulation, steps),
            "prediction_accuracy": CognitiveTest.test_prediction_accuracy(self.simulation, steps),
        }
        
        return {
            "demo": "Phase 4 - Cognitive Systems",
            "steps": steps,
            "cognitive_tests": cognitive_tests,
            "simulation_summary": {
                "total_reward": sum(d["reward"] for d in episode_data),
                "final_brain_state": episode_data[-1]["brain_state"] if episode_data else {},
                "final_agent_state": episode_data[-1]["agent_state"] if episode_data else {},
            },
            "episode_data": episode_data,
        }
class Phase6Demo:
    """Phase 6 integration demo with advanced API."""
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        self.config = config or AdvancedConfig()
        self.name = "Phase 6 Demo - Full Integration"
        
    def run_demo(self, steps: int = 2000) -> Dict[str, Any]:
        """Run Phase 6 integration demo."""
        simulation = NeuralSimulation(self.config)
        episode_data = simulation.run_episode(steps)
        
        # Comprehensive analysis
        analyzer = BrainAnalyzer(simulation.brain)
        connectivity = ConnectivityInspector(simulation.brain)
        profiler = DynamicsProfiler(simulation)
        
        analysis = {
            "brain_analysis": analyzer.analyze_current_state(),
            "connectivity_analysis": connectivity.analyze_connectivity(),
            "dynamics_profile": profiler.profile_dynamics(steps),
        }
        
        # Run advanced integration tests
        integration_tests = {
            "memory_system": MemoryTest.test_working_memory(simulation, steps),
            "plasticity_system": PlasticityTest.test_stdp_effectiveness(simulation, steps),
        }
        
        return {
            "demo": "Phase 6 - Full Integration",
            "steps": steps,
            "simulation_summary": simulation.get_performance_stats(),
            "analysis": analysis,
            "integration_tests": integration_tests,
            "episode_data": episode_data,
        }
class SimulationExperiment:
    """Advanced simulation experiment with configurable parameters."""
    
    def __init__(self, name: str, config: Optional[AdvancedConfig] = None):
        self.name = name
        self.config = config or AdvancedConfig()
        self.results = {}
        
    def run_simulation(self, duration: int = 1000, **kwargs) -> NeuralSimulation:
        """Run a simulation with given parameters."""
        # Create simulation with custom config
        sim = NeuralSimulation(self.config)
        
        # Apply any additional parameter overrides
        for key, value in kwargs.items():
            if hasattr(sim.config.get_config(), key):
                setattr(sim.config.get_config(), key, value)
        
        # Run simulation
        sim.run_episode(duration)
        
        return sim
    
    def compare_configurations(self, config_list: List[AdvancedConfig], duration: int = 1000) -> Dict[str, Any]:
        """Compare multiple configurations."""
        comparison_results = {}
        
        for i, config in enumerate(config_list):
            config_name = f"config_{i+1}"
            simulation = NeuralSimulation(config)
            simulation.run_episode(duration)
            
            comparison_results[config_name] = {
                "simulation_stats": simulation.get_performance_stats(),
                "brain_state": simulation.brain.getTotalNeuronCount(),
                "activity_level": simulation.brain.getActiveNeuronCount() / simulation.brain.getTotalNeuronCount(),
                "firing_rate": simulation.brain.getAverageFiringRate(),
            }
        
        return {
            "experiment": self.name,
            "comparison_results": comparison_results,
            "best_performer": self._find_best_performer(comparison_results),
        }
    
    def _find_best_performer(self, results: Dict[str, Any]) -> str:
        """Find the best performing configuration."""
        if not results:
            return "none"
        
        # Simple metric: highest firing rate with reasonable activity
        best_config = None
        best_score = -1
        
        for config_name, metrics in results.items():
            activity = metrics["activity_level"]
            firing_rate = metrics["firing_rate"]
            score = activity * firing_rate
            
            if score > best_score:
                best_score = score
                best_config = config_name
        
        return best_config
class ExplorationExperiment:
    """Exploration-focused experiments."""
    
    def __init__(self, base_config: Optional[AdvancedConfig] = None):
        self.base_config = base_config or AdvancedConfig()
        self.name = "Exploration Experiment"
        
    def run_exploration_search(self, param_configs: List[Dict[str, Any]], duration: int = 500) -> Dict[str, Any]:
        """Run exploration search over parameter space."""
        results = []
        
        for i, params in enumerate(param_configs):
            # Create config from parameters
            config = AdvancedConfig()
            for key, value in params.items():
                if hasattr(config.get_config(), key):
                    setattr(config.get_config(), key, value)
            
            # Run simulation
            simulation = NeuralSimulation(config)
            simulation.run_episode(duration)
            
            results.append({
                "params": params,
                "simulation_stats": simulation.get_performance_stats(),
                "brain_state": simulation.brain.getTotalNeuronCount(),
                "success": simulation.agent.health > 0,
            })
        
        return {
            "experiment": self.name,
            "search_space_size": len(param_configs),
            "results": results,
            "best_parameters": self._find_best_parameters(results),
        }
    
    def _find_best_parameters(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Find best parameters from search results."""
        if not results:
            return {}
        
        # Use a multi-objective scoring function
        best_result = None
        best_score = -1
        
        for result in results:
            if not result["success"]:
                continue
                
            stats = result["simulation_stats"]
            # Score based on performance
            score = stats["average_reward_per_step"] * stats["steps_per_second"]
            
            if score > best_score:
                best_score = score
                best_result = result["params"]
        
        return best_result or {}
class DevelopmentExperiment:
    """Development stage experiments."""
    
    def __init__(self, base_config: Optional[AdvancedConfig] = None):
        self.base_config = base_config or AdvancedConfig()
        self.name = "Development Experiment"
        
    def run_development_stages(self, stages: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Run simulation through different developmental stages."""
        stage_results = []
        
        for stage_info in stages:
            # Create stage-specific config
            config = AdvancedConfig()
            config.update_config(**stage_info["parameters"])
            
            # Run simulation for this stage
            simulation = NeuralSimulation(config)
            simulation.run_episode(stage_info["duration"])
            
            stage_results.append({
                "stage_name": stage_info["name"],
                "parameters": stage_info["parameters"],
                "duration": stage_info["duration"],
                "simulation_stats": simulation.get_performance_stats(),
                "developmental_effects": self._analyze_developmental_effects(simulation),
            })
        
        return {
            "experiment": self.name,
            "stages": stage_results,
            "development_trajectory": self._analyze_trajectory(stage_results),
        }
    
    def _analyze_developmental_effects(self, simulation: NeuralSimulation) -> Dict[str, Any]:
        """Analyze developmental effects on brain function."""
        return {
            "plasticity": simulation.brain.getExcitationInhibitionRatio(),
            "activity": simulation.brain.getActiveNeuronCount(),
            "firing_rate": simulation.brain.getAverageFiringRate(),
            "neuromodulation": simulation.agent_brain.getNeuromodulationLevel(),
        }
    
    def _analyze_trajectory(self, stage_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze developmental trajectory."""
        trajectory = []
        
        for result in stage_results:
            trajectory.append({
                "stage": result["stage_name"],
                "plasticity": result["developmental_effects"]["plasticity"],
                "activity_level": result["developmental_effects"]["activity"] / 1000,  # Normalize
                "development_score": (result["developmental_effects"]["plasticity"] + 
                                    result["developmental_effects"]["activity"]),
            })
        
        return {
            "trajectory": trajectory,
            "trend": self._analyze_trend([t["development_score"] for t in trajectory]),
            "recommendations": self._generate_development_recommendations(trajectory),
        }
    
    def _analyze_trend(self, scores: List[float]) -> str:
        """Analyze developmental trend."""
        if len(scores) < 2:
            return "insufficient_data"
        
        trend = np.polyfit(range(len(scores)), scores, 1)[0]
        
        if trend > 0.1:
            return "positive_development"
        elif trend < -0.1:
            return "negative_development"
        else:
            return "stable_development"
    
    def _generate_development_recommendations(self, trajectory: List[Dict[str, Any]]) -> List[str]:
        """Generate development recommendations."""
        recommendations = []
        
        if len(trajectory) >= 2:
            recent_score = trajectory[-1]["development_score"]
            earlier_score = trajectory[0]["development_score"]
            
            if recent_score < earlier_score:
                recommendations.append("Consider adjusting developmental parameters")
                recommendations.append("Review plasticity and learning rates")
            
            # Check for instability
            if len(trajectory) >= 3:
                recent_change = abs(trajectory[-1]["development_score"] - trajectory[-2]["development_score"])
                if recent_change > 0.5:
                    recommendations.append("Developmental changes appear unstable - may need finer control")
        
        recommendations.append("Continue monitoring across developmental stages")
        recommendations.append("Consider implementing adaptive developmental control")
        
        return recommendations
class MemoryIntegrationExperiment:
    """Memory integration experiments."""
    
    def __init__(self, base_config: Optional[AdvancedConfig] = None):
        self.base_config = base_config or AdvancedConfig()
        self.name = "Memory Integration Experiment"
        
    def run_memory_integration_test(self, duration: int = 2000) -> Dict[str, Any]:
        """Run memory integration test with various configurations."""
        # Test memory integration with different configurations
        test_configs = [
            {"neuron_count": 1000, "region_count": 2, "dopamine_baseline": 0.1},
            {"neuron_count": 2000, "region_count": 3, "dopamine_baseline": 0.15},
            {"neuron_count": 3000, "region_count": 4, "dopamine_baseline": 0.2},
            {"neuron_count": 1500, "region_count": 2, "dopamine_baseline": 0.05},
        ]
        
        results = []
        
        for i, config_params in enumerate(test_configs):
            config = AdvancedConfig()
            for key, value in config_params.items():
                setattr(config.get_config(), key, value)
            
            simulation = NeuralSimulation(config)
            simulation.run_episode(duration)
            
            results.append({
                "test_id": i + 1,
                "parameters": config_params,
                "memory_test": MemoryTest.test_working_memory(simulation, duration),
                "simulation_stats": simulation.get_performance_stats(),
                "success": simulation.agent.health > 0,
            })
        
        return {
            "experiment": self.name,
            "test_results": results,
            "integration_analysis": self._analyze_memory_integration(results),
            "recommendations": self._generate_memory_recommendations(results),
        }
    
    def _analyze_memory_integration(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze memory integration across configurations."""
        integration_scores = []
        
        for result in results:
            if result["success"]:
                memory_test = result["memory_test"]
                stability = memory_test["stability_score"]
                activity = memory_test["mean_activity"]
                
                integration_score = (stability + activity) / 2
                integration_scores.append({
                    "test_id": result["test_id"],
                    "integration_score": integration_score,
                    "stability": stability,
                    "activity": activity,
                })
        
        return {
            "best_configuration": self._find_best_configuration(integration_scores),
            "integration_trend": self._analyze_integration_trend(integration_scores),
            "requirements_met": len([s for s in integration_scores if s["integration_score"] > 0.7]),
        }
    
    def _find_best_configuration(self, integration_scores: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Find best memory integration configuration."""
        if not integration_scores:
            return {}
        
        best = max(integration_scores, key=lambda x: x["integration_score"])
        return {
            "test_id": best["test_id"],
            "integration_score": best["integration_score"],
            "stability": best["stability"],
            "activity": best["activity"],
        }
    
    def _analyze_integration_trend(self, scores: List[Dict[str, Any]]) -> str:
        """Analyze integration trend."""
        if len(scores) < 2:
            return "insufficient_data"
        
        scores_list = [s["integration_score"] for s in scores]
        trend = np.polyfit(range(len(scores_list)), scores_list, 1)[0]
        
        if trend > 0.01:
            return "improving_integration"
        elif trend < -0.01:
            return "declining_integration"
        else:
            return "stable_integration"
    
    def _generate_memory_recommendations(self, results: List[Dict[str, Any]]) -> List[str]:
        """Generate memory integration recommendations."""
        recommendations = []
        
        successful_tests = [r for r in results if r["success"]]
        if len(successful_tests) < len(results) * 0.5:
            recommendations.append("Memory integration appears challenging - consider adjusting parameters")
        
        integration_scores = [r["memory_test"]["stability_score"] for r in successful_tests]
        if integration_scores and max(integration_scores) < 0.7:
            recommendations.append("Memory stability could be improved with different neuron/region ratios")
        
        recommendations.append("Continue testing different neuromodulation levels")
        recommendations.append("Consider implementing memory consolidation mechanisms")
        
        return recommendations
class PlasticityExperiment:
    """Plasticity-focused experiments."""
    
    def __init__(self, base_config: Optional[AdvancedConfig] = None):
        self.base_config = base_config or AdvancedConfig()
        self.name = "Plasticity Experiment"
        
    def run_plasticity_study(self, variations: List[Dict[str, Any]], duration: int = 1500) -> Dict[str, Any]:
        """Run plasticity study with parameter variations."""
        results = []
        
        for i, variation in enumerate(variations):
            config = AdvancedConfig()
            for key, value in variation.items():
                setattr(config.get_config(), key, value)
            
            simulation = NeuralSimulation(config)
            simulation.run_episode(duration)
            
            plasticity_test = PlasticityTest.test_stdp_effectiveness(simulation, duration)
            
            results.append({
                "variation_id": i + 1,
                "parameters": variation,
                "plasticity_test": plasticity_test,
                "simulation_stats": simulation.get_performance_stats(),
                "success": simulation.agent.health > 0,
            })
        
        return {
            "experiment": self.name,
            "plasticity_results": results,
            "plasticity_analysis": self._analyze_plasticity_results(results),
            "recommendations": self._generate_plasticity_recommendations(results),
        }
    
    def _analyze_plasticity_results(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze plasticity results."""
        effectiveness_scores = []
        
        for result in results:
            if result["success"]:
                plasticity_test = result["plasticity_test"]
                spike_growth = plasticity_test["spike_growth_rate"]
                ratio_stability = plasticity_test["ratio_stability"]
                
                effectiveness = (spike_growth + (1 - ratio_stability)) / 2
                effectiveness_scores.append({
                    "variation_id": result["variation_id"],
                    "effectiveness": effectiveness,
                    "spike_growth": spike_growth,
                    "ratio_stability": ratio_stability,
                })
        
        return {
            "optimal_parameters": self._find_optimal_plasticity_parameters(effectiveness_scores),
            "effectiveness_trend": self._analyze_effectiveness_trend(effectiveness_scores),
            "successful_variations": len([s for s in effectiveness_scores if s["effectiveness"] > 0.7]),
        }
    
    def _find_optimal_plasticity_parameters(self, scores: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Find optimal plasticity parameters."""
        if not scores:
            return {}
        
        optimal = max(scores, key=lambda x: x["effectiveness"])
        return {
            "variation_id": optimal["variation_id"],
            "effectiveness": optimal["effectiveness"],
            "parameters": self._find_parameters_from_variation_id(scores, optimal["variation_id"]),
        }
    
    def _find_parameters_from_variation_id(self, scores: List[Dict[str, Any]], variation_id: int) -> Dict[str, Any]:
        """Find parameters for given variation ID."""
        for result in scores:
            if result["variation_id"] == variation_id:
                return result
        return {}
    
    def _analyze_effectiveness_trend(self, scores: List[Dict[str, Any]]) -> str:
        """Analyze effectiveness trend."""
        if len(scores) < 2:
            return "insufficient_data"
        
        effectiveness_list = [s["effectiveness"] for s in scores]
        trend = np.polyfit(range(len(effectiveness_list)), effectiveness_list, 1)[0]
        
        if trend > 0.01:
            return "improving_effectiveness"
        elif trend < -0.01:
            return "declining_effectiveness"
        else:
            return "stable_effectiveness"
    
    def _generate_plasticity_recommendations(self, results: List[Dict[str, Any]]) -> List[str]:
        """Generate plasticity recommendations."""
        recommendations = []
        
        successful_results = [r for r in results if r["success"]]
        if not successful_results:
            recommendations.append("Plasticity experiments are failing - check health/survival parameters")
            return recommendations
        
        effectiveness_scores = [r["plasticity_test"]["spike_growth_rate"] for r in successful_results]
        avg_effectiveness = np.mean(effectiveness_scores)
        
        if avg_effectiveness < 1.5:
            recommendations.append("STDP effectiveness appears low - consider increasing learning rates")
        
        ratio_stability = [r["plasticity_test"]["ratio_stability"] for r in successful_results]
        avg_stability = np.mean(ratio_stability)
        
        if avg_stability < 0.5:
            recommendations.append("Plasticity ratios appear unstable - consider tuning neuromodulation")
        
        recommendations.append("Continue systematic parameter exploration")
        recommendations.append("Consider implementing adaptive plasticity control")
        
        return recommendations
class AdvancedResearchExperiment:
    """Advanced research experiments for experienced users."""
    
    def __init__(self, base_config: Optional[AdvancedConfig] = None):
        self.base_config = base_config or AdvancedConfig()
        self.name = "Advanced Research Experiment"
        
    def run_multiphase_experiment(self, phases: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Run multi-phase research experiment."""
        phase_results = []
        
        for phase_info in phases:
            phase_name = phase_info["name"]
            config = AdvancedConfig()
            config.update_config(**phase_info["base_parameters"])
            
            # Run simulation for this phase
            simulation = NeuralSimulation(config)
            simulation.run_episode(phase_info["duration"])
            
            # Run phase-specific tests
            phase_tests = {}
            if "memory_tests" in phase_info:
                phase_tests["memory"] = MemoryTest.test_working_memory(simulation, phase_info["duration"])
            
            if "plasticity_tests" in phase_info:
                phase_tests["plasticity"] = PlasticityTest.test_stdp_effectiveness(simulation, phase_info["duration"])
            
            if "cognitive_tests" in phase_info:
                phase_tests["cognitive"] = CognitiveTest.test_prediction_accuracy(simulation, phase_info["duration"])
            
            phase_results.append({
                "phase_name": phase_name,
                "parameters": phase_info["base_parameters"],
                "duration": phase_info["duration"],
                "simulation_stats": simulation.get_performance_stats(),
                "phase_tests": phase_tests,
                "success": simulation.agent.health > 0,
            })
        
        return {
            "experiment": self.name,
            "phases": phase_results,
            "research_insights": self._generate_research_insights(phase_results),
            "recommendations": self._generate_advanced_recommendations(phase_results),
        }
    
    def _generate_research_insights(self, phase_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Generate research insights from phase results."""
        insights = []
        
        successful_phases = [p for p in phase_results if p["success"]]
        
        for phase in successful_phases:
            # Analyze phase-specific insights
            phase_tests = phase["phase_tests"]
            
            if "memory" in phase_tests:
                memory_stability = phase_tests["memory"]["stability_score"]
                if memory_stability > 0.7:
                    insights.append(f"Phase {phase['phase_name']}: Good memory stability achieved")
                elif memory_stability > 0.4:
                    insights.append(f"Phase {phase['phase_name']}: Moderate memory stability - could be improved")
                else:
                    insights.append(f"Phase {phase['phase_name']}: Poor memory stability - need parameter adjustment")
            
            if "plasticity" in phase_tests:
                effectiveness = phase_tests["plasticity"]["spike_growth_rate"]
                if effectiveness > 2.0:
                    insights.append(f"Phase {phase['phase_name']}: Excellent plasticity with high spike growth")
                elif effectiveness > 1.5:
                    insights.append(f"Phase {phase['phase_name']}: Good plasticity effectiveness")
                else:
                    insights.append(f"Phase {phase['phase_name']}: Limited plasticity effectiveness")
        
        return {
            "key_insights": insights,
            "successful_phases": len(successful_phases),
            "successful_rate": len(successful_phases) / len(phase_results) if phase_results else 0,
        }
    
    def _generate_advanced_recommendations(self, phase_results: List[Dict[str, Any]]) -> List[str]:n        """Generate advanced research recommendations."""
        recommendations = []
        
        successful_phases = [p for p in phase_results if p["success"]]
        if len(successful_phases) < len(phase_results) * 0.7:
            recommendations.append("High phase failure rate - consider simplifying research objectives")
        
        for phase in successful_phases:
            # Phase-specific recommendations
            if "memory" in phase["phase_tests"]:
                memory_stability = phase["phase_tests"]["memory"]["stability_score"]
                if memory_stability < 0.6:
                    recommendations.append(f"Phase {phase['phase_name']}: Improve memory stability with more neurons")
            
            if "plasticity" in phase["phase_tests"]:
                effectiveness = phase["phase_tests"]["plasticity"]["spike_growth_rate"]
                if effectiveness < 1.0:
                    recommendations.append(f"Phase {phase['phase_name']}: Increase learning rates for better plasticity")
        
        recommendations.append("Consider implementing cross-phase learning and transfer learning")
        recommendations.append("Develop adaptive parameter tuning based on phase results")
        recommendations.append("Implement automated experiment optimization")
        
        return recommendations
class ComparativeExperiment:
    """Comparative experiments across different research paradigms."""
    
    def __init__(self):
        self.name = "Comparative Experiment"
        
    def compare_research_paradigms(self, paradigms: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare different research paradigms."""
        paradigm_results = []
        
        for paradigm in paradigms:
            paradigm_name = paradigm["name"]
            config = AdvancedConfig()
            config.update_config(**paradigm["parameters"])
            
            # Run simulation for this paradigm
            simulation = NeuralSimulation(config)
            simulation.run_episode(paradigm["duration"])
            
            # Run paradigm-specific tests
            results = {
                "simulation_stats": simulation.get_performance_stats(),
                "brain_dynamics": {
                    "neurons_active": simulation.brain.getActiveNeuronCount(),
                    "firing_rate": simulation.brain.getAverageFiringRate(),
                    "plasticity": simulation.brain.getExcitationInhibitionRatio(),
                },
                "agent_performance": {
                    "health": simulation.agent.health,
                    "energy": simulation.agent.energy,
                    "survival": simulation.agent.health > 0,
                },
                "success": simulation.agent.health > 0,
            }
            
            paradigm_results.append({
                "paradigm": paradigm_name,
                "parameters": paradigm["parameters"],
                "duration": paradigm["duration"],
                "results": results,
            })
        
        return {
            "experiment": self.name,
            "paradigms": paradigm_results,
            "comparative_analysis": self._analyze_paradigm_comparisons(paradigm_results),
            "recommendations": self._generate_comparative_recommendations(paradigm_results),
        }
    
    def _analyze_paradigm_comparisons(self, paradigm_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze comparisons between paradigms."""
        successful_paradigms = [p for p in paradigm_results if p["results"]["success"]]
        
        if not successful_paradigms:
            return {"error": "No successful paradigms to compare"}
        
        # Compare key metrics
        metrics_comparison = {}
        
        for metric in ["neurons_active", "firing_rate", "plasticity"]:
            values = [p["results"]["brain_dynamics"][metric] for p in successful_paradigms]
            metrics_comparison[metric] = {
                "mean": np.mean(values),
                "std": np.std(values),
                "min": np.min(values),
                "max": np.max(values),
                "range": np.max(values) - np.min(values),
            }
        
        # Compare agent performance
        agent_comparison = {}
        for metric in ["health", "energy"]:
            values = [p["results"]["agent_performance"][metric] for p in successful_paradigms]
            agent_comparison[metric] = {
                "mean": np.mean(values),
                "std": np.std(values),
                "min": np.min(values),
                "max": np.max(values),
            }
        
        return {
            "successful_paradigms": len(successful_paradigms),
            "success_rate": len(successful_paradigms) / len(paradigm_results) if paradigm_results else 0,
            "metrics_comparison": metrics_comparison,
            "agent_comparison": agent_comparison,
            "best_performing_paradigm": self._find_best_paradigm(paradigm_results),
        }
    
    def _find_best_paradigm(self, paradigm_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Find best performing paradigm."""
        if not paradigm_results:
            return {}
        
        # Composite score combining multiple metrics
        best_score = -1
        best_paradigm = None
        
        for paradigm in paradigm_results:
            if not paradigm["results"]["success"]:
                continue
            
            brain_metrics = paradigm["results"]["brain_dynamics"]
            agent_metrics = paradigm["results"]["agent_performance"]
            
            # Calculate composite score
            score = (
                brain_metrics["firing_rate"] * 0.4 +  # High firing rate is good
                brain_metrics["plasticity"] * 0.3 +  # Balanced plasticity
                agent_metrics["health"] * 0.3  # High health is good
            )
            
            if score > best_score:
                best_score = score
                best_paradigm = paradigm
        
        if best_paradigm:
            return {
                "paradigm": best_paradigm["paradigm"],
                "score": best_score,
                "parameters": best_paradigm["parameters"],
                "metrics": {
                    "brain": best_paradigm["results"]["brain_dynamics"],
                    "agent": best_paradigm["results"]["agent_performance"],
                },
            }
        
        return {}
    
    def _generate_comparative_recommendations(self, paradigm_results: List[Dict[str, Any]]) -> List[str]:
        """Generate comparative recommendations."""
        recommendations = []
        
        successful_paradigms = [p for p in paradigm_results if p["results"]["success"]]
        if len(successful_paradigms) < len(paradigm_results) * 0.5:
            recommendations.append("Many paradigms failing - review common failure points")
        
        # Find common successful characteristics
        if successful_paradigms:
            common_params = self._find_common_parameters(successful_paradigms)
            if common_params:
                recommendations.append(f"Common successful parameters: {common_params}")
        
        # Compare different parameter sets
        param_impact = self._analyze_parameter_impact(successful_paradigms)
        if param_impact:
            recommendations.append("Parameter analysis suggests focusing on specific parameter ranges")
        
        recommendations.append("Consider implementing adaptive paradigm selection")
        recommendations.append("Develop automated paradigm optimization")
        
        return recommendations
    
    def _find_common_parameters(self, successful_paradigms: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Find common parameters across successful paradigms."""
        if not successful_paradigms:
            return {}
        
        # This is a simplified version - would need more sophisticated analysis
        common = {}
        
        # Find parameter ranges that work well
        for param in ["neuron_count", "region_count", "plasticity_learning_rate", "dopamine_baseline"]:
            values = []
            for paradigm in successful_paradigms:
                if param in paradigm["parameters"]:
                    values.append(paradigm["parameters"][param])
            
            if values:
                common[param] = {
                    "min": min(values),
                    "max": max(values),
                    "mean": np.mean(values),
                }
        
        return common
    
    def _analyze_parameter_impact(self, successful_paradigms: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze impact of different parameters on success."""
        # This would require correlation analysis
        # For now, return a placeholder
        return {
            "note": "Parameter impact analysis would require correlation studies",
            "suggestion": "Consider implementing statistical parameter optimization",
        }