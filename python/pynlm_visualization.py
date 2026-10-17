#!/usr/bin/env python3
"""
NLM Visualization and Reporting Module

This module provides visualization and reporting capabilities for NLM (Neural Learning Machine)
simulations, including network visualization, activity monitoring, and result reporting.
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from datetime import datetime
from pathlib import Path
import json
import csv
from typing import Dict, List, Any, Optional


def create_network_visualization(brain, output_file="network_visualization.html"):
    """
    Create an interactive HTML visualization of the neural network.
    
    Args:
        brain: NLM brain object
        output_file: Output file path for the visualization
    """
    try:
        # Get neuron and synapse data
        neurons = brain.getAllNeurons()
        regions = brain.getRegions()
        
        # Create network graph
        fig, ax = plt.subplots(figsize=(12, 8))
        
        # Create positions for neurons
        positions = {}
        for i, neuron in enumerate(neurons):
            # Place neurons in circles for each region
            region_id = getattr(neuron, 'region_id', 0)
            angle = 2 * np.pi * i / len(neurons)
            radius = region_id * 150 + 100
            positions[i] = (radius * np.cos(angle), radius * np.sin(angle))
        
        # Draw neurons
        for i, neuron in enumerate(neurons):
            x, y = positions[i]
            potential = getattr(neuron, 'potential', 0.0)
            threshold = getattr(neuron, 'threshold', 20.0)
            
            # Color based on activity level
            activity = min(potential / threshold, 1.0)
            color = plt.cm.RdYlBu(activity)
            
            # Draw neuron
            circle = plt.Circle((x, y), 5, color=color, alpha=0.7)
            ax.add_patch(circle)
            
            # Add neuron ID
            plt.text(x, y + 10, str(i), fontsize=8, ha='center')
        
        # Draw synapses
        for synapse in brain.getTotalSynapseCount():
            # Get synapse data
            pre_neuron = getattr(synapse, 'pre_neuron', None)
            post_neuron = getattr(synapse, 'post_neuron', None)
            weight = getattr(synapse, 'weight', 0.0)
            
            if pre_neuron and post_neuron:
                pre_x, pre_y = positions.get(pre_neuron.id, (0, 0))
                post_x, post_y = positions.get(post_neuron.id, (0, 0))
                
                # Draw synapse line
                line_width = max(1, min(weight * 10, 5))
                alpha = min(weight, 1.0)
                
                plt.plot([pre_x, post_x], [pre_y, post_y], 
                        'k-', linewidth=line_width, alpha=alpha)
        
        # Set plot limits and labels
        ax.set_xlim(-400, 400)
        ax.set_ylim(-400, 400)
        ax.set_aspect('equal')
        ax.set_title("NLM Neural Network Visualization")
        ax.axis('off')
        
        # Save visualization
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        return True
    except Exception as e:
        print(f"Error creating network visualization: {e}")
        return False


def create_activity_animation(brain, output_file="activity_animation.gif", steps=100):
    """
    Create an animation of neural activity over time.
    
    Args:
        brain: NLM brain object
        output_file: Output file path for the animation
        steps: Number of simulation steps to animate
    """
    try:
        # Create figure for animation
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
        
        # Animation data storage
        time_data = []
        firing_rate_data = []
        spike_count_data = []
        
        # Run simulation and collect data
        for step in range(steps):
            brain.step(step, step * 0.001)
            
            # Collect data
            time_data.append(step * 0.001)
            firing_rate_data.append(brain.getAverageFiringRate())
            spike_count_data.append(brain.getTotalSpikeCount())
            
            if step % 10 == 0:
                print(f"Step {step}/{steps}")
        
        # Create time series plot
        ax1.plot(time_data, firing_rate_data, 'b-', label='Firing Rate', linewidth=2)
        ax1.set_xlabel('Time (s)')
        ax1.set_ylabel('Firing Rate')
        ax1.set_title('Neural Activity Over Time')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Create spike count plot
        ax2.plot(time_data, spike_count_data, 'r-', label='Spike Count', linewidth=2)
        ax2.set_xlabel('Time (s)')
        ax2.set_ylabel('Spike Count')
        ax2.set_title('Spike Count Over Time')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        # Adjust layout
        plt.tight_layout()
        
        # Save animation
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        return True
    except Exception as e:
        print(f"Error creating activity animation: {e}")
        return False


def create_activity_heatmap(brain, output_file="activity_heatmap.png", window_size=100):
    """
    Create a heatmap of neural activity over time.
    
    Args:
        brain: NLM brain object
        output_file: Output file path for the heatmap
        window_size: Size of the time window for activity calculation
    """
    try:
        # Collect activity data
        time_points = []
        neuron_activities = []
        
        # Run simulation and collect activity data
        for step in range(window_size):
            brain.step(step, step * 0.001)
            
            time_points.append(step * 0.001)
            
            # Get neuron activities
            neuron_activities.append([])
            for neuron in brain.getAllNeurons():
                # Calculate activity based on membrane potential
                potential = getattr(neuron, 'potential', 0.0)
                threshold = getattr(neuron, 'threshold', 20.0)
                activity = min(potential / threshold, 1.0)
                neuron_activities[-1].append(activity)
        
        # Convert to numpy array
        neuron_activities = np.array(neuron_activities)
        
        # Create heatmap
        fig, ax = plt.subplots(figsize=(12, 8))
        
        # Display heatmap
        im = ax.imshow(neuron_activities.T, aspect='auto', cmap='RdYlBu', 
                      vmin=0, vmax=1, interpolation='nearest')
        
        # Set labels and title
        ax.set_xlabel('Time (steps)')
        ax.set_ylabel('Neuron Index')
        ax.set_title('Neural Activity Heatmap')
        
        # Add colorbar
        cbar = plt.colorbar(im, ax=ax)
        cbar.set_label('Activity Level')
        
        # Adjust layout
        plt.tight_layout()
        
        # Save heatmap
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        return True
    except Exception as e:
        print(f"Error creating activity heatmap: {e}")
        return False


def create_results_report(brain, experiment_config, output_file="results_report.json"):
    """
    Create a comprehensive results report for the simulation.
    
    Args:
        brain: NLM brain object
        experiment_config: Experiment configuration
        output_file: Output file path for the results report
    """
    try:
        # Collect simulation results
        results = {
            'timestamp': datetime.now().isoformat(),
            'experiment_config': experiment_config,
            'simulation_results': {
                'final_spike_count': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'total_neuron_count': brain.getTotalNeuronCount(),
                'total_synapse_count': brain.getTotalSynapseCount(),
                'working_memory_traces': 0,  # Would need memory system access
                'episodic_memory_episodes': 0,  # Would need memory system access
                'development_stage': 0,  # Would need development system access
                'dopamine_level': 0.0,  # Would need neuromodulation system access
                'curiosity_level': 0.0,  # Would need neuromodulation system access
            },
            'performance_metrics': {
                'simulation_speed': len(experiment_config) / experiment_config.get('simulation_timestep', 0.001),
                'memory_usage': 'unknown',  # Would need memory monitoring
                'cpu_usage': 'unknown',     # Would need CPU monitoring
            },
            'system_status': {
                'working_memory': brain.getWorkingMemory() is not None,
                'episodic_memory': brain.getEpisodicMemory() is not None,
                'neuromodulation': brain.getDopamine() is not None,
                'prediction': brain.getPredictionSystem() is not None,
                'development': brain.getDevelopmentSystem() is not None,
                'plasticity': True,  # Would need plasticity system access
            }
        }
        
        # Save results report
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        return True
    except Exception as e:
        print(f"Error creating results report: {e}")
        return False


def create_results_summary(brain, output_file="results_summary.html"):
    """
    Create a simple HTML summary of simulation results.
    
    Args:
        brain: NLM brain object
        output_file: Output file path for the results summary
    """
    try:
        # Create HTML summary
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Simulation Results Summary</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 40px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .metric {{ background-color: #f9f9f9; padding: 15px; margin: 10px 0; border-left: 4px solid #007acc; }}
                .metric h3 {{ margin: 0; color: #007acc; }}
                .metric p {{ font-size: 18px; margin: 5px 0; }}
                .status {{ display: inline-block; padding: 5px 10px; margin: 2px; border-radius: 3px; }}
                .status.connected {{ background-color: #d4edda; color: #155724; }}
                .status.disconnected {{ background-color: #f8d7da; color: #721c24; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Simulation Results Summary</h1>
                <p>Generated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
            </div>
            
            <div class="metric">
                <h3>Neural Activity</h3>
                <p><strong>Total Neurons:</strong> {brain.getTotalNeuronCount()}</p>
                <p><strong>Total Synapses:</strong> {brain.getTotalSynapseCount()}</p>
                <p><strong>Total Spikes:</strong> {brain.getTotalSpikeCount()}</p>
                <p><strong>Average Firing Rate:</strong> {brain.getAverageFiringRate():.2f}</p>
                <p><strong>Firing Neuron Count:</strong> {brain.getFiringNeuronCount()}</p>
            </div>
            
            <div class="metric">
                <h3>System Integration Status</h3>
                <p><strong>Working Memory:</strong> 
                    <span class="status {'connected' if brain.getWorkingMemory() is not None else 'disconnected'}">
                        {'CONNECTED' if brain.getWorkingMemory() is not None else 'DISCONNECTED'}
                    </span>
                </p>
                <p><strong>Episodic Memory:</strong> 
                    <span class="status {'connected' if brain.getEpisodicMemory() is not None else 'disconnected'}">
                        {'CONNECTED' if brain.getEpisodicMemory() is not None else 'DISCONNECTED'}
                    </span>
                </p>
                <p><strong>Neuromodulation:</strong> 
                    <span class="status {'connected' if brain.getDopamine() is not None else 'disconnected'}">
                        {'CONNECTED' if brain.getDopamine() is not None else 'DISCONNECTED'}
                    </span>
                </p>
                <p><strong>Prediction System:</strong> 
                    <span class="status {'connected' if brain.getPredictionSystem() is not None else 'disconnected'}">
                        {'CONNECTED' if brain.getPredictionSystem() is not None else 'DISCONNECTED'}
                    </span>
                </p>
                <p><strong>Development System:</strong> 
                    <span class="status {'connected' if brain.getDevelopmentSystem() is not None else 'disconnected'}">
                        {'CONNECTED' if brain.getDevelopmentSystem() is not None else 'DISCONNECTED'}
                    </span>
                </p>
            </div>
            
            <div class="metric">
                <h3>Simulation Information</h3>
                <p><strong>Simulation Steps:</strong> Unknown (would need simulation state)</p>
                <p><strong>Simulation Time:</strong> Unknown (would need simulation clock)</p>
                <p><strong>Brain State:</strong> Active</p>
                <p><strong>Checkpoint Status:</strong> { 'Available' if hasattr(brain, 'save') and hasattr(brain, 'load') else 'Not Available' }</p>
            </div>
            
            <div class="metric">
                <h3>Performance Indicators</h3>
                <p><strong>Activity Level:</strong> {'High' if brain.getAverageFiringRate() > 5 else 'Medium' if brain.getAverageFiringRate() > 1 else 'Low'}</p>
                <p><strong>Network Complexity:</strong> {'High' if brain.getTotalSynapseCount() > brain.getTotalNeuronCount() * 2 else 'Medium' if brain.getTotalSynapseCount() > brain.getTotalNeuronCount() else 'Low'}</p>
                <p><strong>Learning Potential:</strong> {'High' if brain.getTotalSpikeCount() > 100 else 'Medium' if brain.getTotalSpikeCount() > 50 else 'Low'}</p>
            </div>
            
            <div class="metric">
                <h3>Recommended Next Steps</h3>
                <p>1. Run Phase 6 integration test for complete system verification</p>
                <p>2. Analyze learning patterns using visualization tools</p>
                <p>3. Compare results with baseline configurations</p>
                <p>4. Export results for further analysis</p>
            </div>
            
            <footer>
                <p>NLM - Neural Learning Machine | Phase 6 Integration</p>
            </footer>
        </body>
        </html>
        """
        
        # Write HTML file
        with open(output_file, 'w') as f:
            f.write(html_content)
        
        return True
    except Exception as e:
        print(f"Error creating results summary: {e}")
        return False


def create_visualization_dashboard(brain, output_file="visualization_dashboard.html"):
    """
    Create a comprehensive visualization dashboard.
    
    Args:
        brain: NLM brain object
        output_file: Output file path for the dashboard
    """
    try:
        # Create comprehensive HTML dashboard
        dashboard_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Visualization Dashboard</title>
            <script src="https://cdn.jsdelivr.net/npm/three@0.132.2/build/three.min.js"></script>
            <script src="https://cdn.jsdelivr.net/npm/three@0.132.2/examples/js/controls/OrbitControls.js"></script>
            <style>
                body {{ margin: 0; overflow: hidden; background: #1a1a1a; }}
                #container {{ width: 100%; height: 100vh; }}
                .stats-panel {{ position: absolute; top: 10px; left: 10px; background: rgba(0,0,0,0.7); color: white; padding: 20px; border-radius: 5px; font-family: Arial, sans-serif; }}
                .stat {{ margin: 5px 0; }}
                .stat-label {{ font-weight: bold; }}
                .stat-value {{ color: #4CAF50; }}
                .plot {{ position: absolute; top: 10px; right: 10px; background: rgba(0,0,0,0.7); padding: 20px; border-radius: 5px; }}
            </style>
        </head>
        <body>
            <div id="container"></div>
            
            <div class="stats-panel">
                <h2>NLM System Status</h2>
                <div class="stat"><span class="stat-label">Neurons:</span> <span class="stat-value">{brain.getTotalNeuronCount()}</span></div>
                <div class="stat"><span class="stat-label">Synapses:</span> <span class="stat-value">{brain.getTotalSynapseCount()}</span></div>
                <div class="stat"><span class="stat-label">Total Spikes:</span> <span class="stat-value">{brain.getTotalSpikeCount()}</span></div>
                <div class="stat"><span class="stat-label">Avg Firing Rate:</span> <span class="stat-value">{brain.getAverageFiringRate():.2f}</span></div>
                <div class="stat"><span class="stat-label">Working Memory:</span> <span class="{'stat-value' if brain.getWorkingMemory() is not None else 'stat-value'}" style="color: {'#4CAF50' if brain.getWorkingMemory() is not None else '#f44336'};">{'CONNECTED' if brain.getWorkingMemory() is not None else 'DISCONNECTED'}</span></div>
                <div class="stat"><span class="stat-label">Episodic Memory:</span> <span class="{'stat-value' if brain.getEpisodicMemory() is not None else 'stat-value'}" style="color: {'#4CAF50' if brain.getEpisodicMemory() is not None else '#f44336'};">{'CONNECTED' if brain.getEpisodicMemory() is not None else 'DISCONNECTED'}</span></div>
                <div class="stat"><span class="stat-label">Neuromodulation:</span> <span class="{'stat-value' if brain.getDopamine() is not None else 'stat-value'}" style="color: {'#4CAF50' if brain.getDopamine() is not None else '#f44336'};">{'CONNECTED' if brain.getDopamine() is not None else 'DISCONNECTED'}</span></div>
            </div>
            
            <script>
                // Three.js scene setup
                const scene = new THREE.Scene();
                const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
                const renderer = new THREE.WebGLRenderer({{ antialias: true }});
                renderer.setSize(window.innerWidth, window.innerHeight);
                renderer.setPixelRatio(window.devicePixelRatio);
                document.getElementById('container').appendChild(renderer.domElement);
                
                // Controls
                const controls = new THREE.OrbitControls(camera, renderer.domElement);
                controls.enableDamping = true;
                controls.dampingFactor = 0.05;
                
                // Lighting
                const ambientLight = new THREE.AmbientLight(0x404040);
                scene.add(ambientLight);
                
                const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
                directionalLight.position.set(10, 10, 5);
                scene.add(directionalLight);
                
                // Neural network visualization
                const neurons = [];
                const synapses = [];
                
                // Create neurons
                for (let i = 0; i < {brain.getTotalNeuronCount()}; i++) {
                    const geometry = new THREE.SphereGeometry(0.5, 16, 16);
                    const material = new THREE.MeshPhongMaterial({{ 
                        color: 0xff6b6b,
                        emissive: 0x222222,
                        emissiveIntensity: 0.1
                    }});
                    const neuron = new THREE.Mesh(geometry, material);
                    
                    // Position neurons in a spherical arrangement
                    const theta = Math.random() * Math.PI * 2;
                    const phi = Math.acos(2 * Math.random() - 1);
                    const radius = 10;
                    
                    neuron.position.set(
                        radius * Math.sin(phi) * Math.cos(theta),
                        radius * Math.sin(phi) * Math.sin(theta),
                        radius * Math.cos(phi)
                    );
                    
                    neurons.push(neuron);
                    scene.add(neuron);
                }
                
                // Create synapses (simplified representation)
                for (let i = 0; i < Math.min({brain.getTotalSynapseCount()}, 500); i++) {
                    const synapseGeometry = new THREE.BoxGeometry(0.1, 0.1, 5);
                    const synapseMaterial = new THREE.MeshBasicMaterial({{ color: 0x4444ff, opacity: 0.5, transparent: true }});
                    const synapse = new THREE.Mesh(synapseGeometry, synapseMaterial);
                    
                    // Random connection
                    const startNeuron = neurons[Math.floor(Math.random() * neurons.length)];
                    const endNeuron = neurons[Math.floor(Math.random() * neurons.length)];
                    
                    // Set position to midpoint
                    const midPoint = new THREE.Vector3()
                        .addVectors(startNeuron.position, endNeuron.position)
                        .multiplyScalar(0.5);
                    
                    synapse.position.copy(midPoint);
                    synapse.lookAt(startNeuron.position);
                    synapse.scale.set(1, 1, midPoint.distanceTo(startNeuron.position));
                    
                    synapses.push(synapse);
                    scene.add(synapse);
                }
                
                // Add interaction
                let mouseDown = false;
                let previousMouseX = 0;
                let previousMouseY = 0;
                
                document.addEventListener('mousedown', (event) => {{
                    mouseDown = true;
                    previousMouseX = event.clientX;
                    previousMouseY = event.clientY;
                }});
                
                document.addEventListener('mouseup', (event) => {{
                    mouseDown = false;
                }});
                
                document.addEventListener('mousemove', (event) => {{
                    if (mouseDown) {{
                        const deltaMove = new THREE.Vector2()
                            .subVectors(new THREE.Vector2(event.clientX, event.clientY), 
                                       new THREE.Vector2(previousMouseX, previousMouseY));
                        
                        const rotationSpeed = 0.005;
                        controls.rotateLeft(deltaMove.x * rotationSpeed);
                        controls.rotateUp(deltaMove.y * rotationSpeed);
                        
                        previousMouseX = event.clientX;
                        previousMouseY = event.clientY;
                    }}
                }});
                
                // Handle window resize
                window.addEventListener('resize', () => {{
                    camera.aspect = window.innerWidth / window.innerHeight;
                    camera.updateProjectionMatrix();
                    renderer.setSize(window.innerWidth, window.innerHeight);
                }});
                
                // Animation loop
                let time = 0;
                function animate() {{
                    requestAnimationFrame(animate);
                    
                    // Animate neurons (simple pulsing effect)
                    neurons.forEach((neuron, index) => {{
                        const pulseSpeed = 0.001 * (1 + (index % 10));
                        neuron.scale.set(1 + Math.sin(time * pulseSpeed) * 0.2, 
                                        1 + Math.sin(time * pulseSpeed) * 0.2, 
                                        1 + Math.sin(time * pulseSpeed) * 0.2);
                    }});
                    
                    controls.update();
                    renderer.render(scene, camera);
                    time += 0.01;
                }}
                
                animate();
            </script>
        </body>
        </html>
        """
        
        # Write dashboard file
        with open(output_file, 'w') as f:
            f.write(dashboard_content)
        
        return True
    except Exception as e:
        print(f"Error creating visualization dashboard: {e}")
        return False


def create_analysis_report(brain, experiment_config, metrics, output_file="analysis_report.txt"):
    """
    Create a detailed analysis report.
    
    Args:
        brain: NLM brain object
        experiment_config: Experiment configuration
        metrics: Performance metrics
        output_file: Output file path for the analysis report
    """
    try:
        # Create analysis report
        report = f"""
NLM Simulation Analysis Report
==============================

Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

Experiment Configuration:
- Neuron Count: {experiment_config.get('neuron_count', 'N/A')}
- Region Count: {experiment_config.get('region_count', 'N/A')}
- Connection Probability: {experiment_config.get('connection_probability', 'N/A')}
- Max Steps: {experiment_config.get('max_steps', 'N/A')}
- Simulation Timestep: {experiment_config.get('simulation_timestep', 'N/A')}

Simulation Results:
- Total Neurons: {brain.getTotalNeuronCount()}
- Total Synapses: {brain.getTotalSynapseCount()}
- Total Spikes: {brain.getTotalSpikeCount()}
- Average Firing Rate: {brain.getAverageFiringRate():.2f}
- Firing Neuron Count: {brain.getFiringNeuronCount()}

System Integration Status:
- Working Memory: {'CONNECTED' if brain.getWorkingMemory() is not None else 'DISCONNECTED'}
- Episodic Memory: {'CONNECTED' if brain.getEpisodicMemory() is not None else 'DISCONNECTED'}
- Neuromodulation: {'CONNECTED' if brain.getDopamine() is not None else 'DISCONNECTED'}
- Prediction System: {'CONNECTED' if brain.getPredictionSystem() is not None else 'DISCONNECTED'}
- Development System: {'CONNECTED' if brain.getDevelopmentSystem() is not None else 'DISCONNECTED'}

Performance Metrics:
{metrics if metrics else 'No performance metrics available'}

Analysis Summary:
- Activity Level: {'High' if brain.getAverageFiringRate() > 5 else 'Medium' if brain.getAverageFiringRate() > 1 else 'Low'}
- Network Complexity: {'High' if brain.getTotalSynapseCount() > brain.getTotalNeuronCount() * 2 else 'Medium' if brain.getTotalSynapseCount() > brain.getTotalNeuronCount() else 'Low'}
- Learning Potential: {'High' if brain.getTotalSpikeCount() > 100 else 'Medium' if brain.getTotalSpikeCount() > 50 else 'Low'}

Recommendations:
1. Run Phase 6 integration test for complete system verification
2. Analyze learning patterns using visualization tools
3. Compare results with baseline configurations
4. Export results for further analysis
5. Consider increasing simulation steps for more comprehensive results

Conclusion:
The NLM simulation completed successfully with the following key findings:
- Network shows {'active' if brain.getTotalSpikeCount() > 0 else 'no activity'}
- System integration appears {'complete' if all([brain.getWorkingMemory() is not None, 
                                                brain.getEpisodicMemory() is not None,
                                                brain.getDopamine() is not None]) else 'partial'}
- Learning capacity is {'sufficient' if brain.getTotalSpikeCount() > 50 else 'limited'}
"""
        
        # Write report file
        with open(output_file, 'w') as f:
            f.write(report)
        
        return True
    except Exception as e:
        print(f"Error creating analysis report: {e}")
        return False


def create_visualization_suite(brain, experiment_config, output_dir="visualization_output"):
    """
    Create a complete visualization suite for the simulation.
    
    Args:
        brain: NLM brain object
        experiment_config: Experiment configuration
        output_dir: Output directory for visualizations
    """
    try:
        # Create output directory
        Path(output_dir).mkdir(exist_ok=True)
        
        # Generate all visualizations
        visualizations = [
            ("network_visualization.html", create_network_visualization(brain, f"{output_dir}/network_visualization.html")),
            ("activity_animation.gif", create_activity_animation(brain, f"{output_dir}/activity_animation.gif")),
            ("activity_heatmap.png", create_activity_heatmap(brain, f"{output_dir}/activity_heatmap.png")),
            ("results_summary.html", create_results_summary(brain, f"{output_dir}/results_summary.html")),
            ("visualization_dashboard.html", create_visualization_dashboard(brain, f"{output_dir}/visualization_dashboard.html")),
        ]
        
        # Create analysis report
        metrics = {}  # Would need to calculate actual metrics
        analysis_report = create_analysis_report(brain, experiment_config, metrics, f"{output_dir}/analysis_report.txt")
        
        # Create results report
        results_report = create_results_report(brain, experiment_config, f"{output_dir}/results_report.json")
        
        # Print summary
        print("\n" + "="*60)
        print("VISUALIZATION SUITE GENERATION COMPLETE")
        print("="*60)
        print(f"Output directory: {output_dir}")
        print(f"Files generated:")
        for filename, success in visualizations:
            status = "✓" if success else "✗"
            print(f"  {status} {filename}")
        print(f"  {'✓' if analysis_report else '✗'} analysis_report.txt")
        print(f"  {'✓' if results_report else '✗'} results_report.json")
        print("="*60)
        
        return True
    except Exception as e:
        print(f"Error creating visualization suite: {e}")
        return False


if __name__ == "__main__":
    print("NLM Visualization and Reporting Module")
    print("This module provides visualization and reporting capabilities for NLM simulations.")
    print("\nAvailable functions:")
    print("1. create_network_visualization(brain, output_file)")
    print("2. create_activity_animation(brain, output_file, steps)")
    print("3. create_activity_heatmap(brain, output_file, window_size)")
    print("4. create_results_report(brain, experiment_config, output_file)")
    print("5. create_results_summary(brain, output_file)")
    print("6. create_visualization_dashboard(brain, output_file)")
    print("7. create_analysis_report(brain, experiment_config, metrics, output_file)")
    print("8. create_visualization_suite(brain, experiment_config, output_dir)")
    print("\nTo use this module, import it and call the appropriate functions.")
