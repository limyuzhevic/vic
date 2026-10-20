#!/usr/bin/env python3
"""
NLM Research Examples - Memory Formation

This module demonstrates research on memory formation (working memory, episodic memory, associative learning)
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

class MemoryType(Enum):
    """Types of memory systems"""
    WORKING_MEMORY = "working_memory"
    EPISODIC_MEMORY = "episodic_memory"
    ASSOCIATIVE_MEMORY = "associative_memory"

@dataclass
class MemoryMetrics:
    """Metrics for memory formation studies"""
    memory_type: MemoryType
    capacity: int
    retention_time: float
    encoding_efficiency: float
    retrieval_success_rate: float
    interference_level: float
    consolidation_strength: float
    memory_stability: float
    pattern_completion_rate: float
    synaptic_weight_changes: int

class MemoryFormationResearcher:
    """Researcher conducting memory formation studies"""
    
    def __init__(self, neuron_count: int = 400):
        self.neuron_count = neuron_count
        self.visualization = VisualizationInterface()
        self.debug = DebugInterface()
        self.initialize_research_environment()
        
        # Memory tracking
        self.memory_systems = []
        self.pattern_database = {}
        self.activation_patterns = []
    
    def initialize_research_environment(self):
        """Initialize the research environment with visualization and debugging"""
        if self.visualization.initialize() and self.debug.initialize():
            # Configure for memory research
            self.visualization.set_update_rate(30.0)
            self.visualization.set_visualization_mode(VisualizationMode.HEAT_MAP)
            self.visualization.enable_real_time_updates(True)
            self.visualization.enable_debug_overlay(True)
            
            self.debug.set_debug_mode(DebugMode.DETAILED)
            self.debug.enable_real_time_debugging(True)
            self.debug.enable_memory_leak_detection(True)
            self.debug.enable_performance_profiling(True)
    
    def simulate_working_memory(self, duration: float, capacity: int = 10) -> MemoryMetrics:
        """Simulate working memory with capacity constraints"""
        print(f"\n=== WORKING MEMORY STUDY ===")
        print(f"Simulating working memory with capacity: {capacity}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Working memory setup
        memory_capacity = capacity
        current_memory_items = []
        memory_strengths = []
        retrieval_attempts = 0
        successful_retrievals = 0
        
        # Track memory dynamics
        item_presentation_rate = 0.5
        item_forgetting_rate = 0.01
        interference_factor = 0.3
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Present new items (simulate short-term input)
            if np.random.random() < item_presentation_rate * (1.0 / 30.0):
                new_item = {
                    "item_id": len(current_memory_items),
                    "pattern": np.random.normal(0, 1, memory_capacity),
                    "strength": np.random.beta(0.5, 2.0),
                    "age": 0
                }
                current_memory_items.append(new_item)
                memory_strengths.append(new_item["strength"])
            
            # Age existing items (simulate time-based decay)
            for item in current_memory_items:
                item["age"] += 1
                # Apply decay
                item["strength"] *= (1.0 - item_forgetting_rate)
            
            # Interfere with memory items
            if len(current_memory_items) > 1 and np.random.random() < interference_factor * (1.0 / 30.0):
                # Random interference between items
                idx1, idx2 = np.random.choice(len(current_memory_items), 2, replace=False)
                interference_strength = np.random.beta(0.3, 3.0)
                current_memory_items[idx1]["strength"] *= (1.0 - interference_strength)
                current_memory_items[idx2]["strength"] *= (1.0 - interference_strength)
            
            # Attempt retrieval
            if current_memory_items and np.random.random() < 0.3:  # 30% retrieval attempt
                retrieval_attempts += 1
                # Calculate retrieval probability based on strengths
                strengths = [item["strength"] for item in current_memory_items]
                total_strength = sum(strengths)
                if total_strength > 0:
                    retrieval_probabilities = [s / total_strength for s in strengths]
                    retrieval_target = np.random.choice(len(current_memory_items), p=retrieval_probabilities)
                    
                    # Attempt retrieval
                    if np.random.random() < current_memory_items[retrieval_target]["strength"]:
                        successful_retrievals += 1
            
            # Trim memory to capacity
            if len(current_memory_items) > memory_capacity:
                # Remove weakest items
                sorted_items = sorted(current_memory_items, key=lambda x: x["strength"])
                current_memory_items = sorted_items[-memory_capacity:]
                memory_strengths = [item["strength"] for item in current_memory_items]
            
            # Update visualization
            self.visualization.add_time_series_metric("memory_items", len(current_memory_items))
            self.visualization.add_time_series_metric("total_strength", sum(memory_strengths))
            self.visualization.add_time_series_metric("retrieval_success_rate", 
                                                     successful_retrievals / max(1, retrieval_attempts))
            self.visualization.add_time_series_metric("interference_level", interference_factor)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="WorkingMemory",
                    current_state="Active",
                    warnings=[f"Items: {len(current_memory_items)}, Strength: {sum(memory_strengths):.3f}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate memory metrics
        final_memory_items = len(current_memory_items)
        average_retention = np.mean([item["strength"] for item in current_memory_items]) if current_memory_items else 0.0
        final_retrieval_rate = successful_retrievals / max(1, retrieval_attempts)
        encoding_efficiency = min(1.0, final_memory_items / max(1, steps / 30.0))  # Items per second
        
        # Calculate consolidation strength (how well items are maintained)
        consolidation_strength = np.mean(memory_strengths) if memory_strengths else 0.0
        
        # Pattern completion rate (spontaneous recall)
        pattern_completion_rate = np.random.random() * final_retrieval_rate
        
        metrics = MemoryMetrics(
            memory_type=MemoryType.WORKING_MEMORY,
            capacity=memory_capacity,
            retention_time=duration,
            encoding_efficiency=encoding_efficiency,
            retrieval_success_rate=final_retrieval_rate,
            interference_level=interference_factor,
            consolidation_strength=consolidation_strength,
            memory_stability=0.8 - (duration / 300.0),  # Decreases with time
            pattern_completion_rate=pattern_completion_rate,
            synaptic_weight_changes=len(current_memory_items) * 5  # Estimate based on items
        )
        
        self.memory_systems.append(metrics)
        
        print(f"✓ Working memory complete: {final_memory_items} items retained")
        print(f"  Average retention strength: {average_retention:.4f}")
        print(f"  Retrieval success rate: {final_retrieval_rate:.4f}")
        print(f"  Encoding efficiency: {encoding_efficiency:.4f}")
        
        return metrics
    
    def simulate_episodic_memory(self, duration: float, episode_length: float = 10.0) -> MemoryMetrics:
        """Simulate episodic memory formation and consolidation"""
        print(f"\n=== EPISODIC MEMORY STUDY ===")
        print(f"Simulating episodic memory formation")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Episodic memory setup
        current_episodes = {}
        consolidation_events = 0
        retrieval_successes = 0
        total_retrieval_attempts = 0
        
        # Track episodic dynamics
        episode_creation_rate = 0.3
        consolidation_rate = 0.02
        interference_level = 0.2
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Create new episode
            if np.random.random() < episode_creation_rate * (1.0 / 30.0):
                episode_id = len(current_episodes)
                # Simulate episodic representation
                episode_pattern = np.random.normal(0, 1, 20)  # 20-dimensional episode
                current_episodes[episode_id] = {
                    "pattern": episode_pattern,
                    "strength": np.random.beta(0.7, 1.5),
                    "age": 0,
                    "last_accessed": current_time
                }
            
            # Age episodes (simulate time-based decay)
            for episode_id, episode in current_episodes.items():
                episode["age"] += 1
                episode["last_accessed"] = current_time
                
                # Apply decay
                decay_factor = 1.0 - (episode["age"] * 0.01 / 60.0)  # Decay over time
                episode["strength"] *= decay_factor
                
                # Consolidation
                if np.random.random() < consolidation_rate * (1.0 / 30.0):
                    consolidation_events += 1
                    # Enhance episode strength through consolidation
                    episode["strength"] = min(1.0, episode["strength"] * 1.1)
            
            # Attempt retrieval
            if current_episodes and np.random.random() < 0.2:
                total_retrieval_attempts += 1
                # Choose episode based on strength
                episode_ids = list(current_episodes.keys())
                if episode_ids:
                    strengths = [current_episodes[eid]["strength"] for eid in episode_ids]
                    total_strength = sum(strengths)
                    if total_strength > 0:
                        retrieval_probs = [s / total_strength for s in strengths]
                        target_ep = np.random.choice(episode_ids, p=retrieval_probs)
                        
                        # Attempt retrieval
                        if np.random.random() < current_episodes[target_ep]["strength"]:
                            retrieval_successes += 1
            
            # Interference between episodes
            if len(current_episodes) > 1 and np.random.random() < interference_level * (1.0 / 30.0):
                # Select two episodes for interference
                ep_ids = np.random.choice(list(current_episodes.keys()), 2, replace=False)
                ep1, ep2 = current_episodes[ep_ids[0]], current_episodes[ep_ids[1]]
                
                # Apply interference
                interference_strength = np.random.beta(0.2, 5.0)
                ep1["strength"] *= (1.0 - interference_strength)
                ep2["strength"] *= (1.0 - interference_strength)
            
            # Update visualization
            self.visualization.add_time_series_metric("active_episodes", len(current_episodes))
            self.visualization.add_time_series_metric("average_strength", 
                                                     np.mean([e["strength"] for e in current_episodes.values()]) if current_episodes else 0.0)
            self.visualization.add_time_series_metric("consolidation_events", consolidation_events)
            self.visualization.add_time_series_metric("retrieval_success_rate",
                                                     retrieval_successes / max(1, total_retrieval_attempts))
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="EpisodicMemory",
                    current_state="Active",
                    warnings=[f"Episodes: {len(current_episodes)}, Consolidation: {consolidation_events}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate memory metrics
        final_episode_count = len(current_episodes)
        average_retrieval_rate = retrieval_successes / max(1, total_retrieval_attempts)
        encoding_efficiency = min(1.0, final_episode_count / max(1, steps / 30.0))
        
        # Pattern completion rate (spontaneous episode recall)
        pattern_completion_rate = average_retrieval_rate * 0.8
        
        # Memory stability (how well episodes are maintained over time)
        if current_episodes:
            memory_stability = np.mean([min(1.0, e["strength"]) for e in current_episodes.values()])
        else:
            memory_stability = 0.0
        
        metrics = MemoryMetrics(
            memory_type=MemoryType.EPISODIC_MEMORY,
            capacity=len(current_episodes),  # Dynamic capacity
            retention_time=duration,
            encoding_efficiency=encoding_efficiency,
            retrieval_success_rate=average_retrieval_rate,
            interference_level=interference_level,
            consolidation_strength=1.0 - (consolidation_events / max(1, steps / 30.0)),
            memory_stability=memory_stability,
            pattern_completion_rate=pattern_completion_rate,
            synaptic_weight_changes=len(current_episodes) * 3
        )
        
        self.memory_systems.append(metrics)
        
        print(f"✓ Episodic memory complete: {final_episode_count} episodes stored")
        print(f"  Retrieval success rate: {average_retrieval_rate:.4f}")
        print(f"  Encoding efficiency: {encoding_efficiency:.4f}")
        print(f"  Consolidation events: {consolidation_events}")
        
        return metrics
    
    def simulate_associative_memory(self, duration: float, pattern_size: int = 8) -> MemoryMetrics:
        """Simulate associative memory formation and pattern completion"""
        print(f"\n=== ASSOCIATIVE MEMORY STUDY ===")
        print(f"Simulating associative memory formation with pattern size: {pattern_size}")
        
        start_time = time.time()
        steps = int(duration * 30)
        
        # Associative memory setup
        number_of_patterns = 5
        patterns = {}
        associations = []
        activation_patterns = []
        
        # Track associative dynamics
        pattern_learning_rate = 0.05
        association_strength = np.random.uniform(0.5, 0.9)
        retrieval_threshold = 0.6
        
        for step in range(steps):
            current_time = time.time() - start_time
            
            # Learn new patterns
            if np.random.random() < pattern_learning_rate * (1.0 / 30.0):
                pattern_id = len(patterns)
                # Create random pattern
                pattern = np.random.normal(0, 1, pattern_size)
                patterns[pattern_id] = {
                    "pattern": pattern,
                    "strength": np.random.beta(0.8, 1.5),
                    "associations": [],
                    "last_accessed": current_time
                }
            
            # Form associations between patterns
            if len(patterns) >= 2 and np.random.random() < association_strength * (1.0 / 30.0):
                # Select two patterns to associate
                pattern_ids = np.random.choice(list(patterns.keys()), 2, replace=False)
                pat1, pat2 = patterns[pattern_ids[0]], patterns[pattern_ids[1]]
                
                # Calculate association strength
                similarity = np.corrcoef(pat1["pattern"], pat2["pattern"])[0, 1]
                similarity = max(0, min(1, similarity))  # Normalize to [0, 1]
                
                new_association = {
                    "pattern1_id": pattern_ids[0],
                    "pattern2_id": pattern_ids[1],
                    "strength": similarity * 0.8 + np.random.beta(0.2, 5.0),  # Add noise
                    "formed_at": current_time
                }
                
                associations.append(new_association)
                
                # Update pattern associations
                if pattern_ids[0] not in pat1["associations"]:
                    pat1["associations"].append({"id": pattern_ids[1], "strength": new_association["strength"]})
                if pattern_ids[1] not in pat2["associations"]:
                    pat2["associations"].append({"id": pattern_ids[0], "strength": new_association["strength"]})
                
                # Update pattern strengths based on association
                pat1["strength"] = min(1.0, pat1["strength"] + new_association["strength"] * 0.1)
                pat2["strength"] = min(1.0, pat2["strength"] + new_association["strength"] * 0.1)
            
            # Activate patterns and test retrieval
            if patterns and np.random.random() < 0.2:
                # Activate a pattern
                pattern_id = np.random.choice(list(patterns.keys()))
                current_pattern = patterns[pattern_id]
                activation_patterns.append(pattern_id)
                
                # Test pattern completion through associations
                completed_patterns = set([pattern_id])
                queue = [pattern_id]
                
                while queue:
                    current_pat_id = queue.pop(0)
                    current_pat = patterns.get(current_pat_id)
                    if not current_pat:
                        continue
                    
                    # Complete through associations
                    for assoc in current_pat["associations"]:
                        if assoc["id"] not in completed_patterns:
                            if np.random.random() < assoc["strength"]:
                                completed_patterns.add(assoc["id"])
                                queue.append(assoc["id"])
                
                # Count successful completions
                successful_completions = len(completed_patterns)
                self.activation_patterns.append({
                    "input_pattern": pattern_id,
                    "completed_patterns": list(completed_patterns),
                    "success_rate": successful_completions / len(patterns) if patterns else 0
                })
            
            # Update visualization
            self.visualization.add_time_series_metric("stored_patterns", len(patterns))
            self.visualization.add_time_series_metric("associations", len(associations))
            self.visualization.add_time_series_metric("pattern_completion_rate",
                                                     np.mean([p["success_rate"] for p in self.activation_patterns]) if self.activation_patterns else 0.0)
            
            # Log progress
            if step % 50 == 0:
                self.debug.add_debug_info(DebugInfo(
                    component_name="AssociativeMemory",
                    current_state="Active",
                    warnings=[f"Patterns: {len(patterns)}, Associations: {len(associations)}"],
                    errors=[],
                    last_update_time=current_time,
                    is_healthy=True
                ))
            
            time.sleep(1.0 / 30.0)
        
        # Calculate memory metrics
        average_retrieval_rate = np.mean([p["success_rate"] for p in self.activation_patterns]) if self.activation_patterns else 0.0
        encoding_efficiency = min(1.0, len(patterns) / max(1, steps / 30.0))
        
        # Pattern completion rate
        pattern_completion_rate = average_retrieval_rate
        
        # Memory stability (strength of associations over time)
        if associations:
            average_association_strength = np.mean([a["strength"] for a in associations])
            memory_stability = average_association_strength
        else:
            memory_stability = 0.0
        
        metrics = MemoryMetrics(
            memory_type=MemoryType.ASSOCIATIVE_MEMORY,
            capacity=len(patterns),
            retention_time=duration,
            encoding_efficiency=encoding_efficiency,
            retrieval_success_rate=average_retrieval_rate,
            interference_level=0.1,  # Lower interference for associative memory
            consolidation_strength=0.9,  # Strong consolidation for associative memory
            memory_stability=memory_stability,
            pattern_completion_rate=pattern_completion_rate,
            synaptic_weight_changes=len(patterns) * 10
        )
        
        self.memory_systems.append(metrics)
        
        print(f"✓ Associative memory complete: {len(patterns)} patterns stored")
        print(f"  Pattern completion rate: {pattern_completion_rate:.4f}")
        print(f"  Encoding efficiency: {encoding_efficiency:.4f}")
        print(f"  Total associations: {len(associations)}")
        
        return metrics
    
    def compare_memory_systems(self) -> Dict[str, Any]:
        """Compare results across different memory types"""
        print(f"\n=== MEMORY SYSTEMS COMPARISON ===")
        
        if not self.memory_systems:
            print("No memory data collected!")
            return {}
        
        comparison = {
            "best_system": None,
            "worst_system": None,
            "total_systems_tested": len(self.memory_systems),
            "system_metrics": {}
        }
        
        # Score each memory system (higher is better)
        system_scores = {}
        for memory in self.memory_systems:
            # Combined score considering capacity, efficiency, and stability
            score = (
                memory.encoding_efficiency * 0.25 +  # Encoding efficiency
                memory.retrieval_success_rate * 0.25 +  # Retrieval success
                memory.memory_stability * 0.25 +  # Memory stability
                memory.pattern_completion_rate * 0.25  # Pattern completion
            )
            system_scores[memory.memory_type.value] = score
        
        # Find best and worst
        comparison["best_system"] = max(system_scores, key=system_scores.get)
        comparison["worst_system"] = min(system_scores, key=system_scores.get)
        
        # Detailed metrics
        for memory in self.memory_systems:
            comparison["system_metrics"][memory.memory_type.value] = {
                "capacity": memory.capacity,
                "retention_time": memory.retention_time,
                "encoding_efficiency": memory.encoding_efficiency,
                "retrieval_success_rate": memory.retrieval_success_rate,
                "interference_level": memory.interference_level,
                "consolidation_strength": memory.consolidation_strength,
                "memory_stability": memory.memory_stability,
                "pattern_completion_rate": memory.pattern_completion_rate,
                "synaptic_weight_changes": memory.synaptic_weight_changes,
                "score": system_scores[memory.memory_type.value],
            }
        
        print(f"✓ Comparison complete")
        print(f"  Best system: {comparison['best_system']} (score: {system_scores[comparison['best_system']]:.4f})")
        print(f"  Worst system: {comparison['worst_system']} (score: {system_scores[comparison['worst_system']]:.4f})")
        print(f"  Total systems tested: {len(self.memory_systems)}")
        
        return comparison
    
    def export_memory_results(self, filename: str):
        """Export memory research results"""
        print(f"\n=== EXPORTING MEMORY RESULTS ===")
        
        # Prepare export data
        export_data = {
            "research_type": "Memory Formation Studies",
            "parameters": {
                "neuron_count": self.neuron_count,
            },
            "memory_systems": [
                {
                    "memory_type": memory.memory_type.value,
                    "capacity": memory.capacity,
                    "retention_time": memory.retention_time,
                    "encoding_efficiency": memory.encoding_efficiency,
                    "retrieval_success_rate": memory.retrieval_success_rate,
                    "interference_level": memory.interference_level,
                    "consolidation_strength": memory.consolidation_strength,
                    "memory_stability": memory.memory_stability,
                    "pattern_completion_rate": memory.pattern_completion_rate,
                    "synaptic_weight_changes": memory.synaptic_weight_changes,
                }
                for memory in self.memory_systems
            ],
            "comparison": self.compare_memory_systems(),
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
        
        print(f"✓ Memory results exported to: {filename}")
        print(f"  File size: {len(json.dumps(export_data, indent=2))} characters")
    
    def cleanup(self):
        """Cleanup research resources"""
        if self.visualization.is_active():
            self.visualization.close()
        if self.debug.is_active():
            self.debug.close()
        print("✓ Research environment cleaned up")

def run_memory_formation_research(duration_per_system: float = 12.0):
    """Run a complete memory formation research study"""
    print("=== NLM MEMORY FORMATION RESEARCH ===")
    
    # Initialize researcher
    researcher = MemoryFormationResearcher(
        neuron_count=400
    )
    
    try:
        # Run memory system studies
        print("\n" + "="*60)
        print("STARTING MEMORY FORMATION RESEARCH")
        print("="*60)
        
        # Working memory study
        working_memory_metrics = researcher.simulate_working_memory(
            duration=duration_per_system,
            capacity=12
        )
        
        # Episodic memory study
        episodic_memory_metrics = researcher.simulate_episodic_memory(
            duration=duration_per_system,
            episode_length=15.0
        )
        
        # Associative memory study
        associative_memory_metrics = researcher.simulate_associative_memory(
            duration=duration_per_system,
            pattern_size=10
        )
        
        # Compare results
        comparison = researcher.compare_memory_systems()
        
        print(f"\n" + "="*60)
        print("MEMORY FORMATION RESEARCH COMPLETE")
        print("="*60)
        print(f"Total memory systems tested: {len(researcher.memory_systems)}")
        print(f"Best performing system: {comparison['best_system']}")
        print(f"Total pattern completions: {sum(len(p['completed_patterns']) for p in researcher.activation_patterns)}")
        
        # Export results
        researcher.export_memory_results("memory_formation_research.json")
        
        return researcher.memory_systems, comparison
        
    finally:
        researcher.cleanup()

if __name__ == "__main__":
    # Run the research
    systems, comparison = run_memory_formation_research()
    print(f"\nResearch completed successfully with {len(systems)} memory systems tested.")
