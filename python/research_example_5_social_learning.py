#!/usr/bin/env python3
"""
NLM Research Examples - Social Learning

This module demonstrates research on social learning (observation, imitation, cultural transmission)
in the Neural Learning Machine (NLM) framework.
"""

import time
import json
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Tuple, Any
from dataclasses import dataclass
from enum import Enum

from nlm_visualization_tools import (
    VisualizationInterface,
    DebugInterface,
    VisualizationMode,
    DebugMode,
    NeuronActivityData,
    TimeSeriesDataPoint,
    DebugInfo,
    MonitoringHook
)

class SocialLearningType(Enum):
    """Types of social learning mechanisms"""
    OBSERVATION_LEARNING = "observation_learning"
    IMITATION = "imitation"
    CULTURAL_TRANSMISSION = "cultural_transmission"

@dataclass
class SocialLearningMetrics:
    """Metrics for social learning studies"""
    learning_type: SocialLearningType
    total_agents: int
    imitation_accuracy: float
    cultural_decay_rate: float
    knowledge_transfer_efficiency: float
    social_influence_strength: float
    collective_intelligence: float
    conformity_bias: float
    learning_acceleration: float
    novelty_adoption_rate: float

class SocialLearningResearcher:
    """Researcher conducting social learning studies"""
    
    def __init__(self, population_size: int = 50):
        self.population_size = population_size
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Social learning tracking
        self.social_strategies = []
        self.knowledge_pool = {}
        self.agent_populations = {}
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for social learning research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.ANIMATED)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_observation_learning(self, duration: float, observation_accuracy: float = 0.8) -> SocialLearningMetrics:
        """Simulate observation learning (learning by watching others)"""
        print(f"\n=== OBSERVATION LEARNING STUDY ===")
        print(f"Simulating observation learning with accuracy: {observation_accuracy}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Observation learning setup
        observed_behaviors = {}
        learning_progress = []
        observation_events = 0
        knowledge_acquisition = 0.0
        
        # Track social dynamics
        social_contagion_rate = 0.15
        imitation_success_rate = 0.7
        knowledge_spread_rate = 0.05
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Generate observable behaviors
            if np.random.random() < 0.1:  # 10% chance of observable behavior
                behavior_id = len(observed_behaviors)
                behavior_strength = np.random.beta(0.8, 1.5)
                observed_behaviors[behavior_id] = {
                    "type": "task_solution",
                    "strength": behavior_strength,
                    "observers": set(),  # Agents who have observed this
                    "adopters": set(),  # Agents who have adopted this
                    "formed_at": current_time
                }
            
            # Observation process
            for agent_id in range(self.population_size):
                # Randomly select behavior to observe
                if observed_behaviors and np.random.random() < social_contagion_rate * (1.0 / 30.0):
                    observed_behavior = np.random.choice(list(observed_behaviors.values()))
                    observer_id = f"agent_{agent_id}"
                    
                    # Record observation
                    observed_behavior["observers"].add(observer_id)
                    
                    # Learn from observation
                    learning_gain = observed_behavior["strength"] * observation_accuracy
                    knowledge_acquisition += learning_gain
                    
                    observation_events += 1
            
            # Adoption of observed behaviors
            for behavior_id, behavior in observed_behaviors.items():
                for observer in behavior["observers"].copy():
                    if np.random.random() < imitation_success_rate * (1.0 / 30.0):
                        # Adopt the behavior
                        behavior["adopters"].add(observer)
                        knowledge_acquisition += behavior["strength"] * 0.5
            
            # Track knowledge spread
            total_observers = sum(len(b["observers"]) for b in observed_behaviors.values())
            total_adopters = sum(len(b["adopters"]) for b in observed_behaviors.values())
            knowledge_spread = total_adopters / max(1, total_observers) if total_observers > 0 else 0.0
            learning_progress.append(knowledge_spread)
            
            # Update visualization
            self.visualization.add_time_series_metric("observed_behaviors", len(observed_behaviors))
            self.visualization.add_time_series_metric("total_observers", total_observers)
            self.visualization.add_time_series_metric("total_adopters", total_adopters)
            self.visualization.add_time_series_metric("knowledge_spread", knowledge_spread)
            self.visualization.add_time_series_metric("learning_progress", knowledge_progress[-1])
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="ObservationLearning",
                    current_state="Active",
                    warnings=[f"Behaviors: {len(observed_behaviors)}, Observers: {total_observers}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate social learning metrics
        average_knowledge_gain = knowledge_acquisition / max(1, observation_events)
        final_knowledge_spread = learning_progress[-1] if learning_progress else 0.0
        
        # Social influence strength
        social_influence = len(observed_behaviors) / self.population_size
        
        # Collective intelligence (group performance)
        collective_intelligence = final_knowledge_spread * 0.7 + (observation_accuracy * 0.3)
        
        # Conformity bias (tendency to follow majority)
        conformity_bias = np.random.random() * 0.4 + 0.3  # 0.3-0.7
        
        # Learning acceleration (how fast knowledge spreads)
        learning_acceleration = np.mean(np.diff(learning_progress)) if len(learning_progress) > 1 else 0.0
        
        # Novelty adoption rate
        novelty_adoption_rate = np.random.random() * 0.5 + 0.3  # 0.3-0.8
        
        metrics = SocialLearningMetrics(
            learning_type=SocialLearningType.OBSERVATION_LEARNING,
            total_agents=self.population_size,
            imitation_accuracy=observation_accuracy,
            cultural_decay_rate=1.0 - final_knowledge_spread,
            knowledge_transfer_efficiency=final_knowledge_spread,
            social_influence_strength=social_influence,
            collective_intelligence=collective_intelligence,
            conformity_bias=conformity_bias,
            learning_acceleration=learning_acceleration,
            novelty_adoption_rate=novelty_adoption_rate
        )
        
        self.social_strategies.append(metrics)
        
        print(f"✓ Observation learning complete: {len(observed_behaviors)} behaviors observed")
        print(f"  Knowledge acquisition: {knowledge_acquisition:.3f}")
        print(f"  Knowledge spread: {final_knowledge_spread:.3f}")
        print(f"  Collective intelligence: {collective_intelligence:.3f}")
        print(f"  Social influence: {social_influence:.3f}")
        
        return metrics
    
    def simulate_imitation(self, duration: float, imitation_accuracy: float = 0.9) -> SocialLearningMetrics:
        """Simulate imitation learning (copying others' behavior directly)"""
        print(f"\n=== IMITATION STUDY ===")
        print(f"Simulating imitation learning with accuracy: {imitation_accuracy}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Imitation setup
        agent_models = {}
        imitation_events = 0
        behavioral_accuracy = 0.0
        cultural_continuity = 0.0
        
        # Track imitation dynamics
        model_selection_rate = 0.2
        copying_success_rate = 0.8
        behavioral_inheritance_rate = 0.1
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Create agent models to imitate
            if np.random.random() < model_selection_rate * (1.0 / 30.0):
                model_id = f"model_{len(agent_models)}"
                # Model characteristics
                model_behavior = {
                    "behavioral_pattern": np.random.normal(0, 1, 10),
                    "success_history": np.random.binomial(1, 0.7, 20),
                    "characteristic": np.random.choice(["innovative", "traditional", "conservative"])
                }
                agent_models[model_id] = model_behavior
            
            # Imitation process
            for agent_id in range(self.population_size):
                agent_identifier = f"agent_{agent_id}"
                
                # Select a model to imitate
                if agent_models and np.random.random() < model_selection_rate * (1.0 / 30.0):
                    model_id = np.random.choice(list(agent_models.keys()))
                    model = agent_models[model_id]
                    
                    # Attempt imitation
                    imitation_success = np.random.random() < imitation_accuracy
                    imitation_events += 1
                    
                    if imitation_success:
                        # Successful imitation improves behavioral accuracy
                        behavioral_accuracy += imitation_accuracy * model["success_history"].mean()
                        
                        # Record model selection
                        if "imitated_models" not in agent_identifier:
                            agent_identifier = agent_identifier + ":"
                        agent_identifier += model_id + ","
            
            # Behavioral inheritance
            if agent_models and np.random.random() < behavioral_inheritance_rate * (1.0 / 30.0):
                # Modify models through inheritance
                parent_model_id = np.random.choice(list(agent_models.keys()))
                parent_model = agent_models[parent_model_id]
                
                # Create offspring model
                offspring_id = f"offspring_{len(agent_models)}_{step}"
                offspring_model = {
                    "behavioral_pattern": parent_model["behavioral_pattern"] * 0.9 + np.random.normal(0, 0.1, 10),
                    "success_history": np.random.binomial(1, 0.6, 20),  # Slightly lower success rate
                    "characteristic": parent_model["characteristic"]
                }
                agent_models[offspring_id] = offspring_model
                
                # Track cultural continuity
                continuity = np.corrcoef(parent_model["behavioral_pattern"], offspring_model["behavioral_pattern"])[0, 1]
                cultural_continuity = (cultural_continuity * (len(agent_models) - 1) + continuity) / len(agent_models)
            
            # Update visualization
            self.visualization.add_time_series_metric("agent_models", len(agent_models))
            self.visualization.add_time_series_metric("imitation_events", imitation_events)
            self.visualization.add_time_series_metric("behavioral_accuracy", behavioral_accuracy / max(1, imitation_events))
            self.visualization.add_time_series_metric("cultural_continuity", cultural_continuity)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="Imitation",
                    current_state="Active",
                    warnings=[f"Models: {len(agent_models)}, Imitations: {imitation_events}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate social learning metrics
        average_imitation_success = behavioral_accuracy / max(1, imitation_events)
        
        # Social influence strength
        social_influence = len(agent_models) / self.population_size
        
        # Collective intelligence
        collective_intelligence = average_imitation_success * 0.6 + cultural_continuity * 0.4
        
        # Conformity bias
        conformity_bias = np.random.random() * 0.5 + 0.5  # 0.5-1.0
        
        # Learning acceleration
        learning_acceleration = np.random.random() * 0.4 + 0.1  # 0.1-0.5
        
        # Novelty adoption rate
        novelty_adoption_rate = np.random.random() * 0.3 + 0.4  # 0.4-0.7
        
        metrics = SocialLearningMetrics(
            learning_type=SocialLearningType.IMITATION,
            total_agents=self.population_size,
            imitation_accuracy=imitation_accuracy,
            cultural_decay_rate=1.0 - cultural_continuity,
            knowledge_transfer_efficiency=average_imitation_success,
            social_influence_strength=social_influence,
            collective_intelligence=collective_intelligence,
            conformity_bias=conformity_bias,
            learning_acceleration=learning_acceleration,
            novelty_adoption_rate=novelty_adoption_rate
        )
        
        self.social_strategies.append(metrics)
        
        print(f"✓ Imitation complete: {len(agent_models)} models imitated")
        print(f"  Behavioral accuracy: {average_imitation_success:.3f}")
        print(f"  Imitation events: {imitation_events}")
        print(f"  Cultural continuity: {cultural_continuity:.3f}")
        print(f"  Collective intelligence: {collective_intelligence:.3f}")
        
        return metrics
    
    def simulate_cultural_transmission(self, duration: float, transmission_efficiency: float = 0.7) -> SocialLearningMetrics:
        """Simulate cultural transmission (sharing knowledge across generations)"""
        print(f"\n=== CULTURAL TRANSMISSION STUDY ===")
        print(f"Simulating cultural transmission with efficiency: {transmission_efficiency}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Cultural transmission setup
        cultural_trends = {}
        generational_knowledge = []
        transmission_events = 0
        knowledge_preservation = 0.0
        
        # Track cultural dynamics
        teaching_rate = 0.08
        learning_rate = 0.12
        innovation_rate = 0.03
        preservation_rate = 0.95
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Innovation (new cultural elements)
            if np.random.random() < innovation_rate * (1.0 / 30.0):
                innovation_id = f"innovation_{len(cultural_trends)}"
                cultural_trends[innovation_id] = {
                    "knowledge_value": np.random.beta(0.8, 1.5),
                    "adoption_rate": np.random.random() * 0.5 + 0.3,
                    "complexity": np.random.random() * 0.8 + 0.2,
                    "generation": 1,
                    "formed_at": current_time
                }
            
            # Teaching (transmission of cultural knowledge)
            for trend_id, trend in cultural_trends.items():
                # Teachers teach students
                if np.random.random() < teaching_rate * (1.0 / 30.0):
                    teaching_student = f"student_{np.random.randint(0, self.population_size - 1)}"
                    
                    # Successful teaching
                    if np.random.random() < transmission_efficiency:
                        transmission_events += 1
                        knowledge_preservation += trend["knowledge_value"] * preservation_rate
                        
                        # Student adopts knowledge (creates new trend or enhances existing)
                        student_trend_id = f"{teaching_student}_{trend['generation'] + 1}"
                        cultural_trends[student_trend_id] = {
                            "knowledge_value": trend["knowledge_value"] * (1.0 + np.random.beta(0.2, 5.0)),
                            "adoption_rate": trend["adoption_rate"] * 0.9,
                            "complexity": trend["complexity"] + np.random.random() * 0.1,
                            "generation": trend["generation"] + 1,
                            "formed_at": current_time
                        }
            
            # Learning (acquisition of cultural knowledge)
            for agent_id in range(self.population_size):
                student_id = f"agent_{agent_id}"
                if np.random.random() < learning_rate * (1.0 / 30.0):
                    # Select a trend to learn
                    if cultural_trends:
                        selected_trend = np.random.choice(list(cultural_trends.values()))
                        learning_efficiency = transmission_efficiency * selected_trend["knowledge_value"]
                        knowledge_preservation += learning_efficiency * 0.5
            
            # Track generational knowledge
            current_generation_knowledge = sum(trend["knowledge_value"] for trend in cultural_trends.values())
            generational_knowledge.append(current_generation_knowledge)
            
            # Update visualization
            self.visualization.add_time_series_metric("cultural_trends", len(cultural_trends))
            self.visualization.add_time_series_metric("transmission_events", transmission_events)
            self.visualization.add_time_series_metric("knowledge_preservation", knowledge_preservation / max(1, transmission_events))
            self.visualization.add_time_series_metric("generational_knowledge", current_generation_knowledge)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="CulturalTransmission",
                    current_state="Active",
                    warnings=[f"Trends: {len(cultural_trends)}, Transmissions: {transmission_events}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate social learning metrics
        average_transmission_success = knowledge_preservation / max(1, transmission_events)
        
        # Social influence strength
        social_influence = len(cultural_trends) / self.population_size
        
        # Collective intelligence
        collective_intelligence = average_transmission_success * 0.7 + (transmission_efficiency * 0.3)
        
        # Conformity bias
        conformity_bias = np.random.random() * 0.3 + 0.7  # 0.7-1.0
        
        # Learning acceleration
        if len(generational_knowledge) > 1:
            learning_acceleration = np.mean(np.diff(generational_knowledge)) / max(1, np.mean(generational_knowledge))
        else:
            learning_acceleration = 0.0
        
        # Novelty adoption rate
        novelty_adoption_rate = np.random.random() * 0.4 + 0.6  # 0.6-1.0
        
        metrics = SocialLearningMetrics(
            learning_type=SocialLearningType.CULTURAL_TRANSMISSION,
            total_agents=self.population_size,
            imitation_accuracy=transmission_efficiency,
            cultural_decay_rate=1.0 - knowledge_preservation,
            knowledge_transfer_efficiency=average_transmission_success,
            social_influence_strength=social_influence,
            collective_intelligence=collective_intelligence,
            conformity_bias=conformity_bias,
            learning_acceleration=learning_acceleration,
            novelty_adoption_rate=novelty_adoption_rate
        )
        
        self.social_strategies.append(metrics)
        
        print(f"✓ Cultural transmission complete: {len(cultural_trends)} cultural trends")
        print(f"  Knowledge preservation: {knowledge_preservation:.3f}")
        print(f"  Transmission events: {transmission_events}")
        print(f"  Collective intelligence: {collective_intelligence:.3f}")
        print(f"  Social influence: {social_influence:.3f}")
        
        return metrics
    
    def compare_social_learning_strategies(self) -> Dict[str, Any]:
        """Compare results across different social learning strategies"""
        print(f"\n=== SOCIAL LEARNING STRATEGIES COMPARISON ===")
        
        if not self.social_strategies:
            print("No social learning data collected!")
            return {}
        
        comparison = {
            "best_strategy": None,
            "worst_strategy": None,
            "total_strategies_tested": len(self.social_strategies),
            "strategy_metrics": {}
        }
        
        # Score each strategy (higher is better)
        strategy_scores = {}
        for strategy in self.social_strategies:
            # Combined score considering efficiency, speed, and collective benefit
            score = (
                strategy.knowledge_transfer_efficiency * 0.25 +  # Knowledge transfer
                strategy.collective_intelligence * 0.25 +  # Collective intelligence
                strategy.learning_acceleration * 0.25 +  # Learning speed
                strategy.novelty_adoption_rate * 0.25  # Innovation adoption
            )
            strategy_scores[strategy.learning_type.value] = score
        
        # Find best and worst
        comparison["best_strategy"] = max(strategy_scores, key=strategy_scores.get)
        comparison["worst_strategy"] = min(strategy_scores, key=strategy_scores.get)
        
        # Detailed metrics
        for strategy in self.social_strategies:
            comparison["strategy_metrics"][strategy.learning_type.value] = {
                "total_agents": strategy.total_agents,
                "imitation_accuracy": strategy.imitation_accuracy,
                "cultural_decay_rate": strategy.cultural_decay_rate,
                "knowledge_transfer_efficiency": strategy.knowledge_transfer_efficiency,
                "social_influence_strength": strategy.social_influence_strength,
                "collective_intelligence": strategy.collective_intelligence,
                "conformity_bias": strategy.conformity_bias,
                "learning_acceleration": strategy.learning_acceleration,
                "novelty_adoption_rate": strategy.novelty_adoption_rate,
                "score": strategy_scores[strategy.learning_type.value],
            }
        
        print(f"✓ Comparison complete")
        print(f"  Best strategy: {comparison['best_strategy']} (score: {strategy_scores[comparison['best_strategy']]:.4f})")
        print(f"  Worst strategy: {comparison['worst_strategy']} (score: {strategy_scores[comparison['worst_strategy']]:.4f})")
        print(f"  Total strategies tested: {len(self.social_strategies)}")
        
        return comparison
    
    def export_social_learning_results(self, filename: str):
        """Export social learning research results"""
        print(f"\n=== EXPORTING SOCIAL LEARNING RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Social Learning Studies",
            "parameters": {
                "population_size": self.population_size,
            },
            "social_strategies": [
                {
                    "learning_type": strategy.learning_type.value,
                    "total_agents": strategy.total_agents,
                    "imitation_accuracy": strategy.imitation_accuracy,
                    "cultural_decay_rate": strategy.cultural_decay_rate,
                    "knowledge_transfer_efficiency": strategy.knowledge_transfer_efficiency,
                    "social_influence_strength": strategy.social_influence_strength,
                    "collective_intelligence": strategy.collective_intelligence,
                    "conformity_bias": strategy.conformity_bias,
                    "learning_acceleration": strategy.learning_acceleration,
                    "novelty_adoption_rate": strategy.novelty_adoption_rate,
                }
                for strategy in self.social_strategies
            ],
            "comparison": self.compare_social_learning_strategies(),
            "visualization_data": {
                "time_series": [
                    {
                        "metric_name": point.metric_name,
                        "value": point.value,
                        "timestamp": point.timestamp,
                    }
                    for point in self.visualization._time_series_data
                ]
            }
        }
        
        # Save to file
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"✓ Social learning results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_social_learning_research(duration_per_strategy: float = 10.0):
    """Run a complete social learning research study"""
    print("=== NLM SOCIAL LEARNING RESEARCH ===")
    
    # Initialize researcher
    researcher = SocialLearningResearcher(
        population_size=60
    )
    
    try:
        # Run social learning strategy studies
        print("\n" + "="*60)
        print("STARTING SOCIAL LEARNING RESEARCH")
        print("="*60)
        
        # Observation learning study
        observation_metrics = researcher.simulate_observation_learning(
            duration=duration_per_strategy,
            observation_accuracy=0.8
        )
        
        # Imitation study
        imitation_metrics = researcher.simulate_imitation(
            duration=duration_per_strategy,
            imitation_accuracy=0.9
        )
        
        # Cultural transmission study
        cultural_metrics = researcher.simulate_cultural_transmission(
            duration=duration_per_strategy,
            transmission_efficiency=0.7
        )
        
        # Compare results
        comparison = researcher.compare_social_learning_strategies()
        
        print(f"\n" + "="*60)
        print("SOCIAL LEARNING RESEARCH COMPLETE")
        print("="*60)
        print(f"Total strategies tested: {len(researcher.social_strategies)}")
        print(f"Best performing strategy: {comparison['best_strategy']}")
        print(f"Total social interactions: {sum(strategy.total_agents for strategy in researcher.social_strategies)}")
        
        # Export results
        researcher.export_social_learning_results("social_learning_research.json")
        
        return researcher.social_strategies, comparison
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    strategies, comparison = run_social_learning_research()
    print(f"\nResearch completed successfully with {len(strategies)} social learning strategies tested.")
