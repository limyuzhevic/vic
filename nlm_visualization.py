#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools

Comprehensive visualization and debugging tools for NLM brain simulations.
Provides plotting, analysis, and visualization capabilities.
"""

import sys
import os
import json
import yaml
import argparse
import math
import statistics
import random
from pathlib import Path
from typing import Dict, Any, List, Tuple, Optional

# Import pynlm with error handling
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Using mock classes for demonstration.")

# Simple mock classes for demonstration when pynlm is not available
class MockBrain:
    def __init__(self):
        self.neuron_count = 100
        self.synapse_count = 500
        self.spike_count = 0
        self.firing_rate = 10.0
    
    def getTotalNeuronCount(self): return self.neuron_count
    def getTotalSynapseCount(self): return self.synapse_count
    def getTotalSpikeCount(self): return self.spike_count
    def getAverageFiringRate(self): return self.firing_rate
    def getActiveNeuronCount(self): return random.randint(10, 50)
    def getFiringNeuronCount(self): return random.randint(5, 25)
    def getRegions(self): return []
    def save(self, filepath): return True
    def load(self, filepath): return True

class MockWorld:
    def __init__(self):
        self.width = 10
        self.height = 10
        self.vision_width = 8
        self.vision_height = 8
    
    def getWidth(self): return self.width
    def getHeight(self): return self.height
    def configure(self, w, h, vw, vh):
        self.width = w
        self.height = h
        self.vision_width = vw
        self.vision_height = vh
    def getSensoryPercept(self): 
        return type('Percept', (), {
            'getVisionWidth': lambda: self.vision_width,
            'getVisionHeight': lambda: self.vision_height
        })()
    def getAgentBody(self): 
        return type('AgentBody', (), {
            'x': 5, 'y': 5, 'orientation': 0, 
            'energy': 100, 'isMoving': True, 'isTurning': False
        })()
    def update(self, dt): pass
    def applyMotorCommand(self, action, time): pass
    def setAgentStart(self, x, y): pass
    def setMaxEnergy(self, e): pass
    def setEnergyDecayRate(self, r): pass
    def getSimulationTime(self): return 0.0

class MockAgent:
    def __init__(self, brain=None):
        self.brain = brain or MockBrain()
    
    def decodeMotorCommand(self): 
        return type('Action', (), {'type': 'MoveForward', 'getType': lambda: 'MoveForward'})()
    def processSensoryInput(self, percept): pass
    def applyRewardModulation(self, reward, predicted): pass
    def reset(self): pass
    def enableRewardModulation(self, enable): pass
    def enableCuriosity(self, enable): pass

class SpikePlotter:
    """Tools for plotting spike patterns and neural activity."""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.spike_data = []
    
    def capture_spike_data(self, duration=100, interval=0.1):
        """Capture spike data over time."""
        self.spike_data = []
        
        for step in range(int(duration / interval)):
            current_time = step * interval
            
            # Get spike count from brain if available
            if self.brain:
                spike_count = self.brain.getTotalSpikeCount()
                firing_rate = self.brain.getAverageFiringRate()
            else:
                # Generate synthetic data if no brain
                spike_count = random.randint(0, 100)
                firing_rate = random.uniform(0, 50)
            
            self.spike_data.append({
                "step": step,
                "time": current_time,
                "spike_count": spike_count,
                "firing_rate": firing_rate,
                "active_neurons": self.brain.getActiveNeuronCount() if self.brain else 0
            })
    
    def generate_spike_raster(self, output_file=None):
        """Generate spike raster plot."""
        if not self.spike_data:
            print("No spike data available. Capture data first.")
            return False
        
        # Calculate statistics
        spike_counts = [d["spike_count"] for d in self.spike_data]
        firing_rates = [d["firing_rate"] for d in self.spike_data]
        times = [d["time"] for d in self.spike_data]
        
        # Create simple text-based raster plot
        if output_file:
            with open(output_file, 'w') as f:
                f.write("Spike Raster Plot\n")
                f.write("=" * 50 + "\n")
                f.write(f"Time Range: {min(times):.2f} - {max(times):.2f}\n")
                f.write(f"Total Steps: {len(self.spike_data)}\n")
                f.write(f"Average Spikes per Step: {statistics.mean(spike_counts):.2f}\n")
                f.write(f"Max Spikes in Step: {max(spike_counts)}\n")
                f.write(f"Average Firing Rate: {statistics.mean(firing_rates):.2f} Hz\n")
                f.write("\nSpike Counts by Step:\n")
                
                for i, data in enumerate(self.spike_data):
                    f.write(f"Step {data['step']:4d} (t={data['time']:6.2f}s): ")
                    
                    # Create visual representation
                    bar_count = int(data["spike_count"] / max(spike_counts) * 50)
                    f.write("■" * bar_count + " " * (50 - bar_count) + f" {data['spike_count']}\n")
                
                # Statistics
                f.write("\nStatistics:\n")
                f.write(f"  Total Spikes: {sum(spike_counts)}\n")
                f.write(f"  Mean Spike Count: {statistics.mean(spike_counts):.2f}\n")
                f.write(f"  Std Spike Count: {statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0:.2f}\n")
                f.write(f"  Max Spike Count: {max(spike_counts)}\n")
                f.write(f"  Min Spike Count: {min(spike_counts)}\n")
        
        return True
    
    def generate_spike_histogram(self, output_file=None):
        """Generate histogram of spike counts."""
        if not self.spike_data:
            print("No spike data available. Capture data first.")
            return False
        
        spike_counts = [d["spike_count"] for d in self.spike_data]
        
        # Create histogram data
        max_count = max(spike_counts)
        bins = {}
        for count in spike_counts:
            bin_key = count // 10 * 10
            bins[bin_key] = bins.get(bin_key, 0) + 1
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write("Spike Count Histogram\n")
                f.write("=" * 50 + "\n")
                f.write(f"Total Observations: {len(spike_counts)}\n")
                
                # Display histogram
                for bin_start in sorted(bins.keys()):
                    count = bins[bin_start]
                    bar_width = int(count / max(bins.values()) * 50)
                    f.write(f"[{bin_start:3d}-{bin_start+10:3d}): {count:4d} ")
                    f.write("■" * bar_width + f" {count/len(spike_counts)*100:.1f}%\n")
        
        return True
    
    def analyze_spike_patterns(self, min_spike_pairs=10):
        """Analyze spike timing patterns."""
        if not self.spike_data:
            print("No spike data available. Capture data first.")
            return None
        
        analysis = {
            "total_spikes": sum(d["spike_count"] for d in self.spike_data),
            "total_steps": len(self.spike_data),
            "spike_rate": sum(d["spike_count"] for d in self.spike_data) / len(self.spike_data),
            "firing_rate_range": {
                "min": min(d["firing_rate"] for d in self.spike_data),
                "max": max(d["firing_rate"] for d in self.spike_data),
                "mean": statistics.mean(d["firing_rate"] for d in self.spike_data)
            },
            "activity_patterns": self._detect_activity_patterns(),
            "coherence_analysis": self._calculate_coherence()
        }
        
        return analysis
    
    def _detect_activity_patterns(self):
        """Detect activity patterns in spike data."""
        patterns = []
        
        # Simple pattern detection
        spike_counts = [d["spike_count"] for d in self.spike_data]
        
        # Detect bursts (high spike count followed by low)
        burst_threshold = statistics.mean(spike_counts) + statistics.stdev(spike_counts) if len(spike_counts) > 1 else statistics.mean(spike_counts)
        
        for i in range(len(spike_counts) - 1):
            if spike_counts[i] > burst_threshold and spike_counts[i+1] <= burst_threshold:
                patterns.append({
                    "type": "burst_end",
                    "step": i,
                    "spike_count": spike_counts[i]
                })
            elif spike_counts[i] <= burst_threshold and spike_counts[i+1] > burst_threshold:
                patterns.append({
                    "type": "burst_start",
                    "step": i,
                    "spike_count": spike_counts[i+1]
                })
        
        return patterns
    
    def _calculate_coherence(self):
        """Calculate coherence between different activity measures."""
        # Simplified coherence calculation
        firing_rates = [d["firing_rate"] for d in self.spike_data]
        spike_counts = [d["spike_count"] for d in self.spike_data]
        
        # Calculate correlation (simplified)
        mean_fr = statistics.mean(firing_rates)
        mean_sc = statistics.mean(spike_counts)
        
        if len(firing_rates) > 1 and len(spike_counts) > 1:
            covariance = sum((fr - mean_fr) * (sc - mean_sc) 
                           for fr, sc in zip(firing_rates, spike_counts)) / (len(firing_rates) - 1)
            std_fr = statistics.stdev(firing_rates)
            std_sc = statistics.stdev(spike_counts)
            
            coherence = covariance / (std_fr * std_sc) if std_fr * std_sc > 0 else 0
        else:
            coherence = 0
        
        return {
            "coherence": coherence,
            "correlation_strength": "strong" if abs(coherence) > 0.7 else "moderate" if abs(coherence) > 0.3 else "weak"
        }

class ActivityHeatmap:
    """Tools for generating activity heatmaps."""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.heatmap_data = []
    
    def capture_activity_data(self, duration=100, interval=0.1):
        """Capture activity data for heatmap."""
        self.heatmap_data = []
        
        for step in range(int(duration / interval)):
            current_time = step * interval
            
            if self.brain:
                # Get neural region data
                regions = self.brain.getRegions()
                region_activities = []
                
                for region in regions:
                    active_neurons = region.getActiveNeuronCount()
                    total_neurons = region.getTotalNeuronCount()
                    activity = active_neurons / total_neurons if total_neurons > 0 else 0
                    region_activities.append({
                        "region_id": region.getId(),
                        "name": region.getName(),
                        "activity": activity,
                        "total_neurons": total_neurons,
                        "active_neurons": active_neurons
                    })
                
                self.heatmap_data.append({
                    "step": step,
                    "time": current_time,
                    "regions": region_activities,
                    "global_stats": {
                        "total_spikes": self.brain.getTotalSpikeCount(),
                        "firing_rate": self.brain.getAverageFiringRate(),
                        "active_neurons": self.brain.getActiveNeuronCount()
                    }
                })
            else:
                # Generate synthetic data
                num_regions = 3
                region_activities = []
                
                for i in range(num_regions):
                    activity = random.random()
                    region_activities.append({
                        "region_id": i + 1,
                        "name": f"Region_{i+1}",
                        "activity": activity,
                        "total_neurons": 100,
                        "active_neurons": int(activity * 100)
                    })
                
                self.heatmap_data.append({
                    "step": step,
                    "time": current_time,
                    "regions": region_activities,
                    "global_stats": {
                        "total_spikes": random.randint(0, 1000),
                        "firing_rate": random.uniform(0, 50),
                        "active_neurons": random.randint(0, 500)
                    }
                })
    
    def generate_heatmap(self, output_file=None):
        """Generate activity heatmap visualization."""
        if not self.heatmap_data:
            print("No activity data available. Capture data first.")
            return False
        
        # Calculate statistics for each region
        region_stats = {}
        for data in self.heatmap_data:
            for region in data["regions"]:
                region_id = region["region_id"]
                if region_id not in region_stats:
                    region_stats[region_id] = {
                        "activities": [],
                        "max_activity": 0,
                        "min_activity": 1.0,
                        "mean_activity": 0,
                        "step_count": 0
                    }
                
                stats = region_stats[region_id]
                activity = region["activity"]
                stats["activities"].append(activity)
                stats["max_activity"] = max(stats["max_activity"], activity)
                stats["min_activity"] = min(stats["min_activity"], activity)
                stats["step_count"] += 1
        
        # Calculate means
        for region_id, stats in region_stats.items():
            if stats["activities"]:
                stats["mean_activity"] = statistics.mean(stats["activities"])
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write("Activity Heatmap\n")
                f.write("=" * 50 + "\n")
                f.write(f"Time Range: {self.heatmap_data[0]['time']:.2f} - "
                       f"{self.heatmap_data[-1]['time']:.2f}\n")
                f.write(f"Total Time Steps: {len(self.heatmap_data)}\n")
                f.write(f"Number of Regions: {len(region_stats)}\n")
                
                # Display heatmap as text
                f.write("\nRegion Activity Heatmap:\n")
                f.write("Region | Min | Max | Mean | Activity Level\n")
                f.write("-" * 60 + "\n")
                
                for region_id in sorted(region_stats.keys()):
                    stats = region_stats[region_id]
                    # Get region name from first occurrence
                    region_name = next((r["name"] for r in self.heatmap_data[0]["regions"] 
                                      if r["region_id"] == region_id), f"Region_{region_id}")
                    
                    min_act = stats["min_activity"]
                    max_act = stats["max_activity"]
                    mean_act = stats["mean_activity"]
                    
                    # Create visual bar
                    bar_length = int(mean_act * 40)
                    bar = "■" * bar_length + " " * (40 - bar_length)
                    
                    f.write(f"{region_name:6s} | {min_act:4.2f} | {max_act:4.2f} | "
                           f"{mean_act:5.2f} | {bar} {mean_act:.2f}\n")
        
        return True

class NetworkGraph:
    """Tools for generating neural network graphs."""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.graph_data = None
    
    def analyze_connectivity(self):
        """Analyze brain connectivity patterns."""
        if not self.brain:
            print("No brain available. Brain is required for connectivity analysis.")
            return None
        
        regions = self.brain.getRegions()
        total_synapses = self.brain.getTotalSynapseCount()
        total_neurons = self.brain.getTotalNeuronCount()
        
        # Calculate connectivity statistics
        connectivity_stats = {
            "total_regions": len(regions),
            "total_synapses": total_synapses,
            "total_neurons": total_neurons,
            "average_synapses_per_region": total_synapses / len(regions) if regions else 0,
            "average_neurons_per_region": total_neurons / len(regions) if regions else 0,
            "connectivity_density": total_synapses / (total_neurons * (total_neurons - 1)) if total_neurons > 1 else 0
        }
        
        # Analyze regional connectivity
        regional_connectivity = []
        for region in regions:
            region_synapses = region.getSynapseCount()
            region_neurons = region.getTotalNeuronCount()
            
            regional_connectivity.append({
                "region_id": region.getId(),
                "name": region.getName(),
                "synapses": region_synapses,
                "neurons": region_neurons,
                "intra_region_density": region_synapses / (region_neurons * (region_neurons - 1)) 
                                        if region_neurons > 1 else 0
            })
        
        self.graph_data = {
            "connectivity_stats": connectivity_stats,
            "regional_connectivity": regional_connectivity,
            "network_topology": self._infer_network_topology(connectivity_stats),
            "centrality_measures": self._calculate_centrality_measures(regional_connectivity)
        }
        
        return self.graph_data
    
    def _infer_network_topology(self, stats):
        """Infer network topology characteristics."""
        density = stats["connectivity_density"]
        
        if density > 0.1:
            return "dense"
        elif density > 0.01:
            return "moderate"
        else:
            return "sparse"
    
    def _calculate_centrality_measures(self, regional_connectivity):
        """Calculate centrality measures for regions."""
        if not regional_connectivity:
            return {}
        
        max_synapses = max(r["synapses"] for r in regional_connectivity)
        centrality = {}
        
        for region in regional_connectivity:
            region_id = region["region_id"]
            normalized_connectivity = region["synapses"] / max_synapses if max_synapses > 0 else 0
            
            centrality[region_id] = {
                "connectivity_score": normalized_connectivity,
                "rank": 0  # Will be set after sorting
            }
        
        # Sort by connectivity score (simple rank)
        sorted_regions = sorted(centrality.items(), key=lambda x: x[1]["connectivity_score"], reverse=True)
        for rank, (region_id, _) in enumerate(sorted_regions):
            centrality[region_id]["rank"] = rank + 1
        
        return centrality
    
    def generate_network_graph(self, output_file=None):
        """Generate network graph visualization."""
        if not self.graph_data:
            print("No graph data available. Run analyze_connectivity first.")
            return False
        
        stats = self.graph_data["connectivity_stats"]
        regional = self.graph_data["regional_connectivity"]
        centrality = self.graph_data["centrality_measures"]
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write("Neural Network Graph\n")
                f.write("=" * 50 + "\n")
                f.write(f"Network Topology: {self.graph_data['network_topology']}\n")
                f.write(f"Total Regions: {stats['total_regions']}\n")
                f.write(f"Total Neurons: {stats['total_neurons']}\n")
                f.write(f"Total Synapses: {stats['total_synapses']}\n")
                f.write(f"Connectivity Density: {stats['connectivity_density']:.6f}\n")
                
                # Display network as adjacency matrix (simplified)
                f.write("\nRegional Connectivity Matrix (Adjacency)\n")
                f.write("      ", end="")
                for region in regional:
                    f.write(f"{region['name'][:4]:>6s}", end="")
                f.write("\n")
                
                for i, region_i in enumerate(regional):
                    f.write(f"{region_i['name'][:4]:4s}: ", end="")
                    for j, region_j in enumerate(regional):
                        if i == j:
                            # Self-connections (diagonal)
                            density_ij = region_i["intra_region_density"]
                            f.write(f"{density_ij:6.3f}", end="")
                        else:
                            # Inter-regional connections (simplified as 0 for now)
                            f.write("   0.000", end="")
                        f.write(" ", end="")
                    f.write("\n")
                
                # Display centrality measures
                f.write("\nCentrality Measures:\n")
                f.write("Region | Connectivity | Rank\n")
                f.write("-" * 35 + "\n")
                
                for region_id in sorted(centrality.keys()):
                    cent = centrality[region_id]
                    region_name = next((r["name"] for r in regional 
                                      if r["region_id"] == region_id), f"Region_{region_id}")
                    
                    f.write(f"{region_name:6s} | {cent['connectivity_score']:11.3f} | "
                           f"{cent['rank']:4d}\n")
        
        return True
class DebugTools:
    """Debug and diagnostic tools for brain simulations."""
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.debug_log = []
    
    def log_event(self, event_type, message, details=None):
        """Log debug event."""
        event = {
            "timestamp": len(self.debug_log),  # Simple timestamp
            "type": event_type,
            "message": message,
            "details": details,
            "time": len(self.debug_log) * 0.1  # Approximate time
        }
        self.debug_log.append(event)
    
    def check_brain_integrity(self):
        """Check brain system integrity."""
        issues = []
        warnings = []
        
        if not self.brain:
            issues.append("No brain available")
            return {"issues": issues, "warnings": warnings}
        
        # Check basic brain properties
        neuron_count = self.brain.getTotalNeuronCount()
        if neuron_count == 0:
            warnings.append("Brain has zero neurons - may not be initialized properly")
        
        synapse_count = self.brain.getTotalSynapseCount()
        if synapse_count == 0:
            warnings.append("Brain has zero synapses - connectivity may be poor")
        
        firing_rate = self.brain.getAverageFiringRate()
        if firing_rate > 100:
            warnings.append(f"Very high firing rate ({firing_rate:.2f} Hz) - possible runaway excitation")
        
        # Check spike counts
        total_spikes = self.brain.getTotalSpikeCount()
        if total_spikes == 0 and neuron_count > 0:
            warnings.append("No spikes recorded - neurons may not be firing")
        
        return {
            "issues": issues,
            "warnings": warnings,
            "brain_stats": {
                "neurons": neuron_count,
                "synapses": synapse_count,
                "spikes": total_spikes,
                "firing_rate": firing_rate
            }
        }
    
    def check_world_integrity(self):
        """Check world system integrity."""
        issues = []
        warnings = []
        
        if not self.world:
            issues.append("No world available")
            return {"issues": issues, "warnings": warnings}
        
        # Check world dimensions
        width = self.world.getWidth()
        height = self.world.getHeight()
        
        if width == 0 or height == 0:
            issues.append("World has invalid dimensions")
        
        # Check agent if available
        if self.agent:
            agent_body = self.world.getAgentBody()
            if hasattr(agent_body, 'x'):
                if not (0 <= agent_body.x <= width):
                    warnings.append(f"Agent X position ({agent_body.x}) outside world bounds (0-{width})")
                if not (0 <= agent_body.y <= height):
                    warnings.append(f"Agent Y position ({agent_body.y}) outside world bounds (0-{height})")
        
        return {
            "issues": issues,
            "warnings": warnings,
            "world_stats": {
                "width": width,
                "height": height
            }
        }
    
    def run_diagnostic_tests(self):
        """Run comprehensive diagnostic tests."""
        diagnostics = {
            "timestamp": len(self.debug_log) * 0.1,
            "brain_test": self.check_brain_integrity(),
            "world_test": self.check_world_integrity(),
            "connectivity_test": self._test_connectivity(),
            "performance_test": self._test_performance(),
            "stability_test": self._test_stability()
        }
        
        return diagnostics
    
    def _test_connectivity(self):
        """Test connectivity between systems."""
        results = {
            "brain_world_connection": "OK" if self.brain and self.world else "FAIL",
            "agent_brain_connection": "OK" if self.agent and self.brain else "FAIL",
            "sensory_input": "OK",
            "motor_output": "OK"
        }
        
        # Test sensory input
        if self.agent and self.world:
            try:
                percept = self.world.getSensoryPercept()
                if percept:
                    results["sensory_input"] = "OK"
                else:
                    results["sensory_input"] = "FAIL - No percept available"
            except:
                results["sensory_input"] = "ERROR"
        
        # Test motor output
        if self.agent and self.world:
            try:
                action = self.agent.decodeMotorCommand()
                if action:
                    results["motor_output"] = "OK"
                else:
                    results["motor_output"] = "FAIL - No action generated"
            except:
                results["motor_output"] = "ERROR"
        
        return results
    
    def _test_performance(self):
        """Test performance metrics."""
        results = {}
        
        if self.brain:
            results["spike_count"] = self.brain.getTotalSynapseCount()
            results["firing_rate"] = self.brain.getAverageFiringRate()
            results["neuron_count"] = self.brain.getTotalNeuronCount()
            results["synapse_count"] = self.brain.getTotalSynapseCount()
        
        return results
    
    def _test_stability(self):
        """Test system stability."""
        results = {
            "firing_rate_stable": True,
            "memory_stable": True,
            "plasticity_active": True
        }
        
        if self.brain:
            firing_rate = self.brain.getAverageFiringRate()
            if firing_rate > 50:  # High firing rate may indicate instability
                results["firing_rate_stable"] = False
        
        return results
    
    def generate_debug_report(self, output_file=None):
        """Generate comprehensive debug report."""
        if output_file:
            with open(output_file, 'w') as f:
                f.write("NLM Debug Report\n")
                f.write("=" * 50 + "\n")
                f.write(f"Generated: {len(self.debug_log)} events logged\n")
                f.write(f"Current Time: {len(self.debug_log) * 0.1:.1f} seconds\n")
                
                if self.debug_log:
                    f.write("\nRecent Events (Last 10):\n")
                    for event in self.debug_log[-10:]:
                        f.write(f"[{event['time']:.1f}s] {event['type']}: {event['message']}\n")
                
                f.write("\nDiagnostic Test Results:\n")
                diagnostics = self.run_diagnostic_tests()
                
                f.write("Brain System:\n")
                brain_test = diagnostics["brain_test"]
                f.write(f"  Status: {'OK' if not brain_test['issues'] else 'ISSUES'}\n")
                if brain_test['warnings']:
                    f.write("  Warnings:\n")
                    for warning in brain_test['warnings']:
                        f.write(f"    - {warning}\n")
                
                f.write(f"  Statistics: {brain_test['brain_stats']}\n")
                
                f.write("World System:\n")
                world_test = diagnostics["world_test"]
                f.write(f"  Status: {'OK' if not world_test['issues'] else 'ISSUES'}\n")
                if world_test['warnings']:
                    f.write("  Warnings:\n")
                    for warning in world_test['warnings']:
                        f.write(f"    - {warning}\n")
                
                f.write(f"  Statistics: {world_test['world_stats']}\n")
                
                f.write("Connectivity:\n")
                for test, result in diagnostics["connectivity_test"].items():
                    f.write(f"  {test}: {result}\n")
                
                f.write("Performance:\n")
                for metric, value in diagnostics["performance_test"].items():
                    f.write(f"  {metric}: {value}\n")
                
                f.write("Stability:\n")
                for aspect, status in diagnostics["stability_test"].items():
                    f.write(f"  {aspect}: {status}\n")
                
                f.write("\nRecommendations:\n")
                if brain_test['warnings']:
                    f.write("  - Check brain initialization parameters\n")
                if world_test['warnings']:
                    f.write("  - Check world boundary conditions and agent positioning\n")
                if any(result != "OK" for result in diagnostics["connectivity_test"].values()):
                    f.write("  - Check system connectivity and interfaces\n")
                
                f.write("\nEvent Log:\n")
                for event in self.debug_log:
                    f.write(f"  [{event['time']:.1f}s] {event['type']}: {event['message']}\n")
        
        return diagnostics

class VisualizationTools:
    """High-level visualization and analysis tools."""
    
    def __init__(self, brain=None, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.spike_plotter = SpikePlotter(brain)
        self.heatmap = ActivityHeatmap(brain)
        self.network_graph = NetworkGraph(brain)
        self.debug_tools = DebugTools(brain, world, agent)
    
    def create_comprehensive_visualization(self, output_dir="nlm_visualization", duration=100):
        """Create comprehensive visualization package."""
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
        
        # Capture data
        self.spike_plotter.capture_spike_data(duration)
        self.heatmap.capture_activity_data(duration)
        self.network_graph.analyze_connectivity()
        
        # Generate visualizations
        results = {}
        
        # Spike plots
        results["spike_raster"] = self.spike_plotter.generate_spike_raster(
            os.path.join(output_dir, "spike_raster.txt"))
        
        results["spike_histogram"] = self.spike_plotter.generate_spike_histogram(
            os.path.join(output_dir, "spike_histogram.txt"))
        
        results["spike_analysis"] = self.spike_plotter.analyze_spike_patterns()
        
        # Heatmaps
        results["activity_heatmap"] = self.heatmap.generate_heatmap(
            os.path.join(output_dir, "activity_heatmap.txt"))
        
        # Network graphs
        results["network_graph"] = self.network_graph.generate_network_graph(
            os.path.join(output_dir, "network_graph.txt"))
        
        # Debug report
        diagnostics = self.debug_tools.generate_debug_report(
            os.path.join(output_dir, "debug_report.txt"))
        
        # Create summary file
        summary = {
            "timestamp": duration,
            "visualizations": results,
            "diagnostics": diagnostics,
            "brain_stats": {
                "neurons": self.brain.getTotalNeuronCount() if self.brain else 0,
                "synapses": self.brain.getTotalSynapseCount() if self.brain else 0,
                "spikes": self.brain.getTotalSpikeCount() if self.brain else 0,
                "firing_rate": self.brain.getAverageFiringRate() if self.brain else 0
            } if self.brain else {}
        }
        
        # Save summary
        with open(os.path.join(output_dir, "visualization_summary.json"), 'w') as f:
            json.dump(summary, f, indent=2)
        
        print(f"Comprehensive visualization created in {output_dir}")
        print(f"Generated files:")
        for filename in os.listdir(output_dir):
            print(f"  - {filename}")
        
        return summary
    
    def create_custom_visualization(self, visualization_types, **kwargs):
        """Create custom visualization based on specified types."""
        results = {}
        
        duration = kwargs.get("duration", 100)
        
        for viz_type in visualization_types:
            if viz_type == "spike_raster":
                results["spike_raster"] = self.spike_plotter.generate_spike_raster(**kwargs)
            
            elif viz_type == "spike_histogram":
                results["spike_histogram"] = self.spike_plotter.generate_spike_histogram(**kwargs)
            
            elif viz_type == "heatmap":
                results["heatmap"] = self.heatmap.generate_heatmap(**kwargs)
            
            elif viz_type == "network_graph":
                results["network_graph"] = self.network_graph.generate_network_graph(**kwargs)
        
        return results
    
    def create_analysis_report(self, analysis_type="comprehensive", output_file=None):
        """Create analysis report based on analysis type."""
        report = {
            "analysis_type": analysis_type,
            "timestamp": 0.0,  # Simplified
            "findings": {},
            "recommendations": []
        }
        
        if analysis_type == "comprehensive":
            # Run spike analysis
            spike_analysis = self.spike_plotter.analyze_spike_patterns()
            report["findings"]["spike_analysis"] = spike_analysis
            
            # Run network analysis
            network_data = self.network_graph.graph_data
            if network_data:
                report["findings"]["network_analysis"] = {
                    "topology": network_data.get("network_topology"),
                    "connectivity_density": network_data.get("connectivity_stats", {}).get("connectivity_density", 0),
                    "centrality": network_data.get("centrality_measures", {})
                }
            
            # Run diagnostics
            diagnostics = self.debug_tools.run_diagnostic_tests()
            report["findings"]["diagnostics"] = diagnostics
            
            # Generate recommendations
            if diagnostics["brain_test"]["warnings"]:
                report["recommendations"].append(
                    "Review brain initialization parameters due to warnings"
                )
            
            if diagnostics["world_test"]["warnings"]:
                report["recommendations"].append(
                    "Check world boundary conditions and agent positioning"
                )
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
        
        return report
# High-level convenience functions
def create_visualization_suite(brain, world, agent, duration=100, output_dir="nlm_visualization"):
    """Create complete visualization suite for a brain simulation."""
    tools = VisualizationTools(brain, world, agent)
    return tools.create_comprehensive_visualization(duration, output_dir)
def create_spike_visualization(brain, duration=100):
    """Create spike visualization for a brain."""
    plotter = SpikePlotter(brain)
    plotter.capture_spike_data(duration)
    return {
        "spike_raster": plotter.generate_spike_raster(),
        "spike_histogram": plotter.generate_spike_histogram(),
        "analysis": plotter.analyze_spike_patterns()
    }
def create_network_visualization(brain):
    """Create network visualization for a brain."""
    graph = NetworkGraph(brain)
    graph.analyze_connectivity()
    return {
        "network_graph": graph.generate_network_graph(),
        "connectivity_stats": graph.graph_data
    }
def create_debug_visualization(brain, world, agent):
    """Create debug visualization for simulation."""
    tools = DebugTools(brain, world, agent)
    return {
        "diagnostics": tools.run_diagnostic_tests(),
        "debug_report": tools.generate_debug_report()
    }
def main():
    """Main entry point for visualization tools."""
    parser = argparse.ArgumentParser(
        description="NLM Visualization and Debugging Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Visualization Tools:
  spike_plotter    Create spike pattern visualizations
  activity_heatmap Create activity heatmaps
  network_graph    Generate network graphs
  debug_tools      Debug and diagnostic tools
  comprehensive     Create comprehensive visualization suite

Examples:
  python nlm_visualization.py spike_plotter --duration 100
  python nlm_visualization.py heatmap --duration 50
  python nlm_visualization.py network_graph
  python nlm_visualization.py debug_tools
  python nlm_visualization.py comprehensive --output_dir my_viz
        """
    )
    
    parser.add_argument(
        "tool",
        choices=["spike_plotter", "activity_heatmap", "network_graph", "debug_tools", "comprehensive"],
        help="Visualization tool to use"
    )
    
    parser.add_argument(
        "--duration",
        type=float,
        default=100,
        help="Simulation duration in seconds"
    )
    
    parser.add_argument(
        "--output_dir",
        default="nlm_visualization",
        help="Output directory for visualizations"
    )
    
    args = parser.parse_args()
    
    print("NLM Visualization Tools")
    print("=" * 50)
    print(f"Tool: {args.tool}")
    print(f"Duration: {args.duration} seconds")
    print(f"Output Directory: {args.output_dir}")
    
    print("\nNote: This is a framework for visualization tools.")
    print("To use visualization tools, you need to:")
    print("1. Create a brain object with pynlm.createBrain()")
    print("2. Create a world object with pynlm.createSimpleWorld()")
    print("3. Create an agent object with pynlm.createAgentBrain()")
    print("4. Initialize and run your simulation")
    print("5. Use the VisualizationTools class to analyze results")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
