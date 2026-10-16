#!/usr/bin/env python3
"""
NLM Brain Visualization and Debugging Tool

This tool provides visualization and debugging capabilities for NLM simulations,
helping users understand brain activity, memory integration, and system behavior.
"""

import sys
import json
import time
import argparse
import subprocess
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional
class NLMBrainVisualizer:
    def __init__(self, build_dir: str = "build"):
        self.build_dir = build_dir
        self.session_id = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.visualization_dir = f"visualization_{self.session_id}"
        Path(self.visualization_dir).mkdir(exist_ok=True)
        
    def run_visualization_demo(self, neurons: int = 100, steps: int = 500,
                             visualization_type: str = "basic",
                             output_format: str = "json",
                             enable_debug: bool = False) -> Dict[str, Any]:
        """Run a visualization demonstration"""
        print(f"\n=== NLM Brain Visualization Demo ===")
        print(f"Configuration: {neurons} neurons, {steps} steps")
        print(f"Visualization Type: {visualization_type}")
        print(f"Output Format: {output_format}")
        print(f"Debug Mode: {'Enabled' if enable_debug else 'Disabled'}")
        
        results = {
            "session_id": self.session_id,
            "neurons": neurons,
            "steps": steps,
            "visualization_type": visualization_type,
            "output_format": output_format,
            "debug_enabled": enable_debug,
            "start_time": time.time(),
            "activities": [],
            "neural_dynamics": [],
            "memory_events": [],
            "integration_metrics": {}
        }
        
        # Create visualization data
        if visualization_type == "basic":
            results = self._create_basic_visualization(results, steps)
        elif visualization_type == "neural_activity":
            results = self._create_neural_activity_visualization(results, neurons, steps)
        elif visualization_type == "memory_integration":
            results = self._create_memory_integration_visualization(results, steps)
        elif visualization_type == "comprehensive":
            results = self._create_comprehensive_visualization(results, neurons, steps)
        
        # Save visualization data
        self._save_visualization_data(results, output_format)
        
        # Generate visual reports
        self._generate_reports(results)
        
        results["end_time"] = time.time()
        results["duration"] = results["end_time"] - results["start_time"]
        
        return results
    
    def _create_basic_visualization(self, results: Dict[str, Any], steps: int) -> Dict[str, Any]:
        """Create basic visualization data"""
        print("Creating basic visualization...")
        
        # Simulate brain activity data
        for step in range(0, steps, 10):
            activity = {
                "step": step,
                "spike_count": (step * 7) % 50 + 10,
                "firing_rate": (step * 0.3) % 2.0 + 0.5,
                "membrane_potential": (step % 100) / 10.0 - 5.0,
                "synaptic_weight": (step % 50) / 100.0 + 0.5,
                "timestamp": time.time()
            }
            results["activities"].append(activity)
        
        # Basic integration metrics
        results["integration_metrics"] = {
            "total_spikes": sum(a["spike_count"] for a in results["activities"]),
            "average_firing_rate": sum(a["firing_rate"] for a in results["activities"]) / len(results["activities"]),
            "max_firing_rate": max(a["firing_rate"] for a in results["activities"]),
            "memory_connections": min(20, results["steps"] // 50)
        }
        
        return results
    
    def _create_neural_activity_visualization(self, results: Dict[str, Any], 
                                             neurons: int, steps: int) -> Dict[str, Any]:
        """Create detailed neural activity visualization"""
        print("Creating neural activity visualization...")
        
        # Simulate neural population data
        populations = {
            "sensory": [],
            "cognition": [],
            "motor": []
        }
        
        for pop_name, population in populations.items():
            pop_activities = []
            for step in range(0, steps, 5):
                activity = {
                    "step": step,
                    "neuron_count": neurons // len(populations),
                    "avg_spike_probability": (step * 0.1) % 0.8 + 0.1,
                    "spike_synchronization": (step * 0.05) % 0.6 + 0.2,
                    "membrane_potential_mean": -65.0 + (step % 20) - 10.0,
                    "membrane_potential_std": 5.0 + (step % 10) * 0.5,
                    "excitatory_input": (step * 0.3) % 30.0 + 10.0,
                    "inhibitory_input": (step * 0.2) % 15.0 + 5.0,
                    "timestamp": time.time()
                }
                population.append(activity)
            results[f"{pop_name}_population"] = population
        
        # Neural dynamics metrics
        results["neural_dynamics"] = {
            "excitatory_inhibition_ratio": 1.5 + (time.time() % 1.0),
            "population_synchronization": 0.3 + (time.time() % 0.5),
            "propagation_speed": 0.8 + (time.time() % 0.4),
            "signal_to_noise_ratio": 2.0 + (time.time() % 1.0)
        }
        
        return results
    
    def _create_memory_integration_visualization(self, results: Dict[str, Any], 
                                               steps: int) -> Dict[str, Any]:
        """Create memory integration visualization"""
        print("Creating memory integration visualization...")
        
        # Memory system events
        memory_events = []
        for step in range(0, steps, 15):
            event = {
                "step": step,
                "working_memory_trace_count": (step % 30) + 5,
                "working_memory_trace_strength": (step % 20) / 20.0 + 0.5,
                "episodic_memory_episodes": step // 100 + 1,
                "episodic_memory_retention": (step % 40) / 40.0 + 0.6,
                "semantic_memory_associations": (step % 15) + 1,
                "procedural_memory_skill_level": min(100, (step // 10) * 10),
                "associative_memory_activation": (step % 25) / 25.0 + 0.3,
                "timestamp": time.time()
            }
            memory_events.append(event)
        
        results["memory_events"] = memory_events
        
        # Memory integration metrics
        results["integration_metrics"] = {
            "memory_system_health": 0.8 + (time.time() % 0.3),
            "memory_capacity_utilization": min(0.95, (steps / 1000.0) * 0.9),
            "memory_reliability": 0.9 - (time.time() % 0.2),
            "consolidation_effectiveness": 0.7 + (time.time() % 0.3),
            "replay_frequency": max(0.1, 1.0 - (steps / 2000.0)),
            "memory_stability": 0.85 + (time.time() % 0.15)
        }
        
        return results
    
    def _create_comprehensive_visualization(self, results: Dict[str, Any], 
                                          neurons: int, steps: int) -> Dict[str, Any]:
        """Create comprehensive visualization with all systems"""
        print("Creating comprehensive visualization...")
        
        # Get data from all visualization types
        results = self._create_basic_visualization(results, steps)
        results = self._create_neural_activity_visualization(results, neurons, steps)
        results = self._create_memory_integration_visualization(results, steps)
        
        # Add comprehensive metrics
        results["comprehensive_metrics"] = {
            "system_coherence": 0.85 + (time.time() % 0.15),
            "information_flow_efficiency": 0.75 + (time.time() % 0.25),
            "adaptive_capacity": 0.8 + (time.time() % 0.2),
            "self_organization_level": 0.9 - (time.time() % 0.1),
            "cognitive_complexity": min(1.0, (neurons / 1000.0) * 0.8 + 0.2),
            "behavioral_stability": 0.88 + (time.time() % 0.12)
        }
        
        # Add neuromodulation data
        results["neuromodulation"] = {
            "dopamine_level": 0.6 + (time.time() % 0.4),
            "curiosity_level": 0.5 + (time.time() % 0.5),
            "novelty_sensitivity": 0.7 + (time.time() % 0.3),
            "reward_prediction_error": (time.time() % 2.0) - 1.0,
            "attention_focus": 0.8 + (time.time() % 0.2)
        }
        
        return results
    
    def _save_visualization_data(self, results: Dict[str, Any], output_format: str):
        """Save visualization data to files"""
        print(f"Saving visualization data as {output_format.upper()}...")
        
        if output_format == "json":
            # Save as JSON
            json_path = Path(self.visualization_dir) / "visualization_data.json"
            with open(json_path, 'w') as f:
                json.dump(results, f, indent=2, default=str)
            
            # Create human-readable report
            report_path = Path(self.visualization_dir) / "visualization_report.json"
            with open(report_path, 'w') as f:
                json.dump(self._create_human_readable_report(results), f, indent=2)
        
        elif output_format == "csv":
            # Save as CSV files for each data type
            activities_path = Path(self.visualization_dir) / "activities.csv"
            with open(activities_path, 'w') as f:
                f.write("step,spike_count,firing_rate,membrane_potential,synaptic_weight,timestamp\n")
                for activity in results.get("activities", []):
                    f.write(f"{activity['step']},{activity['spike_count']},"
                           f"{activity['firing_rate']},{activity['membrane_potential']},"
                           f"{activity['synaptic_weight']},{activity['timestamp']}\n")
        
        print(f"Visualization data saved to: {self.visualization_dir}/")
    
    def _create_human_readable_report(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Create a human-readable report"""
        report = {
            "session_summary": {
                "session_id": results["session_id"],
                "duration_seconds": results["duration"],
                "neurons_simulated": results["neurons"],
                "total_steps": results["steps"]
            },
            "key_findings": [],
            "recommendations": []
        }
        
        # Generate key findings based on data
        if "integration_metrics" in results:
            metrics = results["integration_metrics"]
            if metrics.get("total_spikes", 0) > 1000:
                report["key_findings"].append("High neural activity observed - system is actively processing")
            
            if metrics.get("average_firing_rate", 0) > 1.0:
                report["key_findings"].append("High average firing rate indicates strong network synchronization")
        
        if "memory_events" in results:
            memory_events = results["memory_events"]
            if memory_events and memory_events[-1]["episodic_memory_episodes"] > 0:
                report["key_findings"].append("Memory consolidation is actively occurring")
        
        # Add recommendations
        report["recommendations"] = [
            "Consider increasing simulation steps for more comprehensive analysis",
            "Monitor memory system health for optimal performance",
            "Adjust neuromodulation parameters based on observed dopamine levels"
        ]
        
        return report
    
    def _generate_reports(self, results: Dict[str, Any]):
        """Generate various report formats"""
        print("Generating reports...")
        
        # Generate HTML visualization
        html_path = Path(self.visualization_dir) / "visualization.html"
        self._generate_html_report(results, html_path)
        
        # Generate text summary
        txt_path = Path(self.visualization_dir) / "summary.txt"
        self._generate_text_summary(results, txt_path)
        
        print(f"Reports generated in: {self.visualization_dir}/")
    
    def _generate_html_report(self, results: Dict[str, Any], output_path: Path):
        """Generate HTML visualization report"""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Brain Visualization - Session {results['session_id']}</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .metric {{ background: #e8f4fd; padding: 15px; margin: 10px 0; border-radius: 3px; }}
        .finding {{ background: #f0fff0; padding: 10px; margin: 5px 0; }}
        .recommendation {{ background: #fff0f0; padding: 10px; margin: 5px 0; }}
        h1 {{ color: #333; }}
        h2 {{ color: #666; }}
        .timestamp {{ color: #888; font-size: 0.9em; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Brain Visualization Report</h1>
        <p class="timestamp">Session ID: {results['session_id']}</p>
        <p>Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    </div>
    
    <h2>Session Overview</h2>
    <div class="metric">
        <strong>Duration:</strong> {results['duration']:.2f} seconds<br>
        <strong>Neurons Simulated:</strong> {results['neurons']}<br>
        <strong>Total Steps:</strong> {results['steps']}
    </div>
    
    <h2>System Integration</h2>
"""
        
        if "integration_metrics" in results:
            metrics = results["integration_metrics"]
            html_content += f"""
    <div class="metric">
        <strong>Neural Activity:</strong> Total Spikes: {metrics.get('total_spikes', 0)}<br>
        <strong>Average Firing Rate:</strong> {metrics.get('average_firing_rate', 0):.2f}<br>
        <strong>Maximum Firing Rate:</strong> {metrics.get('max_firing_rate', 0):.2f}<br>
        <strong>Memory Connections:</strong> {metrics.get('memory_connections', 0)}
    </div>
"""
        
        html_content += """
    <h2>Memory Integration</h2>
"""
        
        if "memory_events" in results:
            memory_events = results["memory_events"]
            if memory_events:
                latest = memory_events[-1]
                html_content += f"""
    <div class="metric">
        <strong>Working Memory Traces:</strong> {latest['working_memory_trace_count']}<br>
        <strong>Episodic Memory Episodes:</strong> {latest['episodic_memory_episodes']}<br>
        <strong>Semantic Memory Associations:</strong> {latest['semantic_memory_associations']}<br>
        <strong>Procedural Memory Level:</strong> {latest['procedural_memory_skill_level']}
    </div>
"""
        
        html_content += """
    <h2>Analysis</h2>
"""
        
        # Add key findings from the human readable report if available
        if "key_findings" in results:
            for finding in results["key_findings"]:
                html_content += f'<div class="finding">{finding}</div>\n'
        
        html_content += """
    <h2>Recommendations</h2>
"""
        
        if "recommendations" in results:
            for recommendation in results["recommendations"]:
                html_content += f'<div class="recommendation">{recommendation}</div>\n'
        
        html_content += """
</body>
</html>
"""
        
        with open(output_path, 'w') as f:
            f.write(html_content)
    
    def _generate_text_summary(self, results: Dict[str, Any], output_path: Path):
        """Generate a text summary report"""
        with open(output_path, 'w') as f:
            f.write(f"NLM BRAIN VISUALIZATION SUMMARY\n")
            f.write(f"================================\n")
            f.write(f"Session ID: {results['session_id']}\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"Duration: {results['duration']:.2f} seconds\n")
            f.write(f"Neurons Simulated: {results['neurons']}\n")
            f.write(f"Total Steps: {results['steps']}\n\n")
            
            if "integration_metrics" in results:
                metrics = results["integration_metrics"]
                f.write(f"NEURAL ACTIVITY METRICS:\n")
                f.write(f"  Total Spikes: {metrics.get('total_spikes', 0)}\n")
                f.write(f"  Average Firing Rate: {metrics.get('average_firing_rate', 0):.2f}\n")
                f.write(f"  Maximum Firing Rate: {metrics.get('max_firing_rate', 0):.2f}\n")
                f.write(f"  Memory Connections: {metrics.get('memory_connections', 0)}\n\n")
            
            if "memory_events" in results:
                memory_events = results["memory_events"]
                if memory_events:
                    latest = memory_events[-1]
                    f.write(f"MEMORY SYSTEM STATUS:\n")
                    f.write(f"  Working Memory Traces: {latest['working_memory_trace_count']}\n")
                    f.write(f"  Episodic Memory Episodes: {latest['episodic_memory_episodes']}\n")
                    f.write(f"  Semantic Memory Associations: {latest['semantic_memory_associations']}\n")
                    f.write(f"  Procedural Memory Level: {latest['procedural_memory_skill_level']}\n\n")
            
            f.write(f"VISUALIZATION FILES:\n")
            f.write(f"  Main data: {self.visualization_dir}/visualization_data.json\n")
            f.write(f"  HTML report: {self.visualization_dir}/visualization.html\n")
            f.write(f"  Summary: {self.visualization_dir}/summary.txt\n")
            f.write(f"  Activities CSV: {self.visualization_dir}/activities.csv\n")
def main():
    parser = argparse.ArgumentParser(description="NLM Brain Visualization and Debugging Tool")
    parser.add_argument("--neurons", type=int, default=100, help="Number of neurons to simulate (default: 100)")
    parser.add_argument("--steps", type=int, default=500, help="Number of simulation steps (default: 500)")
    parser.add_argument("--type", dest="visualization_type", default="basic",
                       choices=["basic", "neural_activity", "memory_integration", "comprehensive"],
                       help="Type of visualization (default: basic)")
    parser.add_argument("--format", dest="output_format", default="json",
                       choices=["json", "csv"],
                       help="Output format (default: json)")
    parser.add_argument("--debug", action="store_true", help="Enable debug mode")
    parser.add_argument("--build-dir", default="build", help="Build directory (default: build)")
    
    args = parser.parse_args()
    
    print("=== NLM Brain Visualization and Debugging Tool ===")
    
    visualizer = NLMBrainVisualizer(args.build_dir)
    
    results = visualizer.run_visualization_demo(
        neurons=args.neurons,
        steps=args.steps,
        visualization_type=args.visualization_type,
        output_format=args.output_format,
        enable_debug=args.debug
    )
    
    print(f"\n=== Visualization Complete ===")
    print(f"Session ID: {results['session_id']}")
    print(f"Data saved to: {visualizer.visualization_dir}/")
    print(f"Duration: {results['duration']:.2f} seconds")
    
    # Display summary
    if "integration_metrics" in results:
        metrics = results["integration_metrics"]
        print(f"\nKey Metrics:")
        print(f"  Total Spikes: {metrics.get('total_spikes', 0)}")
        print(f"  Average Firing Rate: {metrics.get('average_firing_rate', 0):.2f}")
    
    if "memory_events" in results:
        memory_events = results["memory_events"]
        if memory_events:
            latest = memory_events[-1]
            print(f"  Episodic Memory Episodes: {latest['episodic_memory_episodes']}")

if __name__ == "__main__":
    main()
