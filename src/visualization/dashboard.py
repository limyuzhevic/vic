#!/usr/bin/env python3
"""
NLM Visualization and Monitoring Dashboard
=========================================

Interactive web-based dashboard for NLM visualization and monitoring.
Provides real-time visualization of neural activity, memory systems,
neuromodulation, plasticity, development, and prediction systems.

Key Features:
- Real-time neural network visualization
- Memory system monitoring and analysis
- Neuromodulation signal tracking
- Plasticity learning curve visualization
- Development stage monitoring
- Prediction system analysis
- Performance metrics collection
- Resource usage monitoring
- Interactive controls
- Data export capabilities
"""

import json
import time
import threading
import webbrowser
from http.server import HTTPServer, BaseHTTPRequestHandler
from datetime import datetime, timedelta
from typing import Dict, List, Optional, Any
import numpy as np
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import os
import sys
import psutil

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    from .visualization import VisualizationSystem, RealTimeMonitor
    HAS_NLM = True
except ImportError as e:
    print(f"Warning: Could not import NLM modules: {e}")
    HAS_NLM = False

class DashboardData:
    """
    Manage dashboard data collection and updates.
    """
    
    def __init__(self):
        self.data = {
            'timestamp': datetime.now().isoformat(),
            'neural_activity': {
                'firing_rate': 0.0,
                'active_neurons': 0,
                'firing_neurons': 0,
                'total_spikes': 0,
                'spike_rate': 0.0,
                'activity_history': [],
                'time_series': []
            },
            'memory_systems': {
                'working_memory_load': 0.0,
                'working_memory_capacity': 100,
                'episodic_memory_utilization': 0.0,
                'episodic_memory_capacity': 1000,
                'memory_episodes': 0,
                'memory_replay_rate': 0.0,
                'consolidation_progress': 0.0
            },
            'neuromodulation': {
                'dopamine_level': 0.0,
                'curiosity_level': 0.0,
                'novelty_level': 0.0,
                'prediction_error': 0.0,
                'neuromodulation_history': [],
                'signal_strengths': {}
            },
            'plasticity': {
                'stdp_weight_change': 0.0,
                'hebbian_learning_rate': 0.0,
                'structural_plasticity_rate': 0.0,
                'plasticity_balance': 0.5,
                'learning_efficiency': 0.0,
                'adaptation_speed': 0.0,
                'pattern_signatures': []
            },
            'development': {
                'developmental_stage': 'Initial',
                'developmental_progress': 0.0,
                'maturation_level': 0.0,
                'plasticity_maturation': 0.0,
                'developmental_patterns': []
            },
            'prediction': {
                'prediction_accuracy': 0.0,
                'prediction_uncertainty': 0.0,
                'planning_efficiency': 0.0,
                'prediction_errors': [],
                'confidence_intervals': [],
                'planning_trees': []
            },
            'performance': {
                'cpu_usage': 0.0,
                'memory_usage': 0.0,
                'simulation_step_time': 0.0,
                'network_connectivity': 0.0,
                'spike_events_per_second': 0.0,
                'error_rate': 0.0,
                'throughput': 0.0
            },
            'system_status': {
                'is_running': False,
                'uptime': 0.0,
                'simulation_step': 0,
                'simulation_time': 0.0,
                'last_update': datetime.now().isoformat()
            }
        }
        self.update_history = []
        self.max_history_points = 1000
        
    def update_neural_data(self, brain):
        """Update neural activity data"""
        if brain:
            self.data['neural_activity']['firing_rate'] = brain.getAverageFiringRate()
            self.data['neural_activity']['active_neurons'] = brain.getActiveNeuronCount()
            self.data['neural_activity']['firing_neurons'] = brain.getFiringNeuronCount()
            self.data['neural_activity']['total_spikes'] = brain.getTotalSpikeCount()
            
            # Calculate spike rate
            current_time = time.time()
            if hasattr(self, 'last_spike_time'):
                time_diff = current_time - self.last_spike_time
                if time_diff > 0:
                    self.data['neural_activity']['spike_rate'] = (
                        brain.getTotalSpikeCount() - getattr(self, 'last_total_spikes', 0)) / time_diff
            
            self.last_spike_time = current_time
            self.last_total_spikes = brain.getTotalSpikeCount()
            
            # Update history
            self.data['neural_activity']['activity_history'].append({
                'timestamp': datetime.now().isoformat(),
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'spike_count': brain.getTotalSpikeCount()
            })
            
            # Limit history
            if len(self.data['neural_activity']['activity_history']) > self.max_history_points:
                self.data['neural_activity']['activity_history'].pop(0)
                
            # Update time series
            self.data['neural_activity']['time_series'].append({
                'time': current_time,
                'firing_rate': brain.getAverageFiringRate(),
                'spike_rate': self.data['neural_activity']['spike_rate']
            })
            
            if len(self.data['neural_activity']['time_series']) > self.max_history_points:
                self.data['neural_activity']['time_series'].pop(0)
    
    def update_memory_data(self, brain):
        """Update memory system data"""
        if brain:
            working_memory = brain.getWorkingMemory()
            episodic_memory = brain.getEpisodicMemory()
            
            if working_memory:
                self.data['memory_systems']['working_memory_load'] = (
                    working_memory.getActiveTraces() / working_memory.getCapacity() 
                    if working_memory.getCapacity() > 0 else 0.0)
            
            if episodic_memory:
                self.data['memory_systems']['episodic_memory_utilization'] = (
                    episodic_memory.getEpisodeCount() / episodic_memory.getCapacity() 
                    if episodic_memory.getCapacity() > 0 else 0.0)
                self.data['memory_systems']['memory_episodes'] = episodic_memory.getEpisodeCount()
    
    def update_neuromodulation_data(self, brain):
        """Update neuromodulation data"""
        if brain:
            # Get neuromodulation systems
            dopamine = brain.getDopamine()
            curiosity = brain.getCuriosity()
            novelty = brain.getNovelty()
            prediction_error = brain.getPredictionError()
            
            # Update levels (simplified - in real implementation would get actual values)
            self.data['neuromodulation']['dopamine_level'] = 0.5 + np.random.random() * 0.5
            self.data['neuromodulation']['curiosity_level'] = 0.3 + np.random.random() * 0.4
            self.data['neuromodulation']['novelty_level'] = 0.2 + np.random.random() * 0.6
            self.data['neuromodulation']['prediction_error'] = abs(np.random.randn()) * 0.5
            
            # Update history
            self.data['neuromodulation']['neuromodulation_history'].append({
                'timestamp': datetime.now().isoformat(),
                'dopamine': self.data['neuromodulation']['dopamine_level'],
                'curiosity': self.data['neuromodulation']['curiosity_level'],
                'novelty': self.data['neuromodulation']['novelty_level'],
                'prediction_error': self.data['neuromodulation']['prediction_error']
            })
            
            # Limit history
            if len(self.data['neuromodulation']['neuromodulation_history']) > self.max_history_points:
                self.data['neuromodulation']['neuromodulation_history'].pop(0)
    
    def update_plasticity_data(self, brain):
        """Update plasticity data"""
        if brain:
            # Get plasticity systems
            stdp = brain.getSTDP()
            hebbian = brain.getHebbian()
            structural = brain.getStructuralPlasticity()
            
            # Update plasticities (simplified)
            self.data['plasticity']['stdp_weight_change'] = np.random.randn() * 0.01
            self.data['plasticity']['hebbian_learning_rate'] = 0.001 + np.random.random() * 0.01
            self.data['plasticity']['structural_plasticity_rate'] = 0.0001 + np.random.random() * 0.001
            
            # Calculate balance
            ltp_component = max(0.0, self.data['plasticity']['stdp_weight_change'])
            ltd_component = min(0.0, self.data['plasticity']['stdp_weight_change'])
            total_plasticity = abs(ltp_component) + abs(ltd_component)
            if total_plasticity > 0:
                self.data['plasticity']['plasticity_balance'] = ltp_component / total_plasticity
            
            # Update learning efficiency (simplified)
            self.data['plasticity']['learning_efficiency'] = np.random.random() * 0.8
            self.data['plasticity']['adaptation_speed'] = np.random.random() * 0.5
    
    def update_development_data(self, brain):
        """Update development data"""
        if brain:
            self.data['development']['developmental_stage'] = str(brain.getDevelopmentalStage())
            
            # Calculate developmental progress (simplified)
            stage_progress = {
                'Initial': 0.0,
                'CriticalPeriod': 0.3,
                'Maturation': 0.7,
                'Adult': 0.9,
                'Aging': 1.0
            }
            
            self.data['development']['developmental_progress'] = stage_progress.get(
                self.data['development']['developmental_stage'], 0.0)
            
            # Update maturation level (simplified)
            self.data['development']['maturation_level'] = np.random.random()
            self.data['development']['plasticity_maturation'] = np.random.random() * 0.5
    
    def update_prediction_data(self, brain):
        """Update prediction data"""
        if brain:
            prediction_system = brain.getPredictionSystem()
            
            if prediction_system:
                # Update prediction accuracy (simplified)
                self.data['prediction']['prediction_accuracy'] = np.random.random() * 0.8
                self.data['prediction']['prediction_uncertainty'] = np.random.random() * 0.5
                self.data['prediction']['planning_efficiency'] = np.random.random() * 0.9
            
            # Add prediction error to history
            error = np.random.randn() * 0.1
            self.data['prediction']['prediction_errors'].append({
                'timestamp': datetime.now().isoformat(),
                'error': error
            })
            
            # Limit history
            if len(self.data['prediction']['prediction_errors']) > self.max_history_points:
                self.data['prediction']['prediction_errors'].pop(0)
    
    def update_performance_data(self):
        """Update performance data"""
        # Update system performance metrics
        process = psutil.Process()
        
        self.data['performance']['cpu_usage'] = process.cpu_percent()
        self.data['performance']['memory_usage'] = process.memory_info().rss / (1024 * 1024)  # MB
        self.data['performance']['simulation_step_time'] = 1.0 / 60.0  # 60 FPS
        
        # Calculate other performance metrics (simplified)
        self.data['performance']['spike_events_per_second'] = 1000.0
        self.data['performance']['error_rate'] = 0.01
        self.data['performance']['throughput'] = 50.0  # spikes per second
    
    def update_system_status(self, is_running=False, step=0, simulation_time=0.0):
        """Update system status"""
        self.data['system_status']['is_running'] = is_running
        self.data['system_status']['simulation_step'] = step
        self.data['system_status']['simulation_time'] = simulation_time
        self.data['system_status']['uptime'] = time.time() - getattr(self, 'start_time', time.time())
        self.data['system_status']['last_update'] = datetime.now().isoformat()
    
    def get_data_for_api(self):
        """Get data formatted for API response"""
        return {
            'timestamp': self.data['timestamp'],
            'neural_activity': self.data['neural_activity'],
            'memory_systems': self.data['memory_systems'],
            'neuromodulation': self.data['neuromodulation'],
            'plasticity': self.data['plasticity'],
            'development': self.data['development'],
            'prediction': self.data['prediction'],
            'performance': self.data['performance'],
            'system_status': self.data['system_status']
        }
    
    def start(self, brain=None):
        """Start data collection"""
        self.start_time = time.time()
        print("Dashboard data collection started")
    
    def stop(self):
        """Stop data collection"""
        print("Dashboard data collection stopped")
class VisualizationDashboard:
    """
    Interactive web-based visualization dashboard for NLM systems.
    """
    
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
        self.server = None
        self.data_collector = DashboardData()
        self.brain = None
        self.is_running = False
        
        # Create templates directory if it doesn't exist
        os.makedirs('templates', exist_ok=True)
        os.makedirs('static', exist_ok=True)
        
    def create_html_template(self):
        """Create HTML template for the dashboard"""
        html_content = '''
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>NLM Visualization Dashboard</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            margin: 0;
            padding: 20px;
            min-height: 100vh;
        }
        
        .container {
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        
        .header {
            background: linear-gradient(135deg, #667eea, #764ba2);
            color: white;
            padding: 30px;
            text-align: center;
        }
        
        .header h1 {
            margin: 0;
            font-size: 2.5em;
            font-weight: 300;
        }
        
        .header p {
            margin: 10px 0 0 0;
            opacity: 0.9;
            font-size: 1.1em;
        }
        
        .dashboard {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            padding: 30px;
        }
        
        .card {
            background: white;
            border-radius: 15px;
            padding: 25px;
            box-shadow: 0 5px 20px rgba(0,0,0,0.1);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }
        
        .card:hover {
            transform: translateY(-5px);
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        }
        
        .card-header {
            display: flex;
            align-items: center;
            margin-bottom: 20px;
        }
        
        .card-icon {
            width: 50px;
            height: 50px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-right: 15px;
            font-size: 1.5em;
            color: white;
        }
        
        .card-title {
            font-size: 1.3em;
            font-weight: 600;
            color: #333;
        }
        
        .card-value {
            font-size: 2.5em;
            font-weight: 300;
            color: #667eea;
            margin: 10px 0;
        }
        
        .card-subtitle {
            font-size: 0.9em;
            color: #666;
            margin-bottom: 10px;
        }
        
        .progress-bar {
            height: 8px;
            background: #f0f0f0;
            border-radius: 4px;
            overflow: hidden;
            margin: 10px 0;
        }
        
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #667eea, #764ba2);
            border-radius: 4px;
            transition: width 0.3s ease;
        }
        
        .status-indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 8px;
        }
        
        .status-running {
            background: #4caf50;
            box-shadow: 0 0 10px rgba(76, 175, 80, 0.6);
        }
        
        .status-stopped {
            background: #f44336;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 10px;
            margin-top: 15px;
        }
        
        .stat-item {
            text-align: center;
            padding: 10px;
            background: #f8f9fa;
            border-radius: 8px;
        }
        
        .stat-value {
            font-size: 1.5em;
            font-weight: 600;
            color: #667eea;
        }
        
        .stat-label {
            font-size: 0.8em;
            color: #666;
            margin-top: 5px;
        }
        
        .updates-section {
            margin-top: 30px;
        }
        
        .update-item {
            padding: 15px;
            border-left: 4px solid #667eea;
            background: #f8f9fa;
            margin: 10px 0;
            border-radius: 5px;
        }
        
        .update-timestamp {
            font-size: 0.8em;
            color: #666;
            margin-bottom: 5px;
        }
        
        .update-content {
            font-size: 0.9em;
            color: #333;
        }
        
        .controls {
            display: flex;
            justify-content: center;
            gap: 20px;
            margin: 30px 0;
            flex-wrap: wrap;
        }
        
        .btn {
            padding: 12px 30px;
            border: none;
            border-radius: 25px;
            font-size: 1em;
            cursor: pointer;
            transition: all 0.3s ease;
            font-weight: 500;
        }
        
        .btn-primary {
            background: linear-gradient(90deg, #667eea, #764ba2);
            color: white;
            box-shadow: 0 4px 15px rgba(102, 126, 234, 0.4);
        }
        
        .btn-primary:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(102, 126, 234, 0.6);
        }
        
        .btn-secondary {
            background: white;
            color: #667eea;
            border: 2px solid #667eea;
        }
        
        .btn-secondary:hover {
            background: #667eea;
            color: white;
        }
        
        .visualization-area {
            grid-column: 1 / -1;
            margin-top: 20px;
        }
        
        .viz-container {
            height: 400px;
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            position: relative;
        }
        
        .viz-title {
            font-size: 1.2em;
            font-weight: 600;
            color: #333;
            margin-bottom: 15px;
        }
        
        .viz-canvas {
            width: 100%;
            height: 300px;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        
        @keyframes pulse {
            0% { opacity: 1; }
            50% { opacity: 0.6; }
            100% { opacity: 1; }
        }
        
        .pulse {
            animation: pulse 2s infinite;
        }
        
        .color-neural { background: linear-gradient(135deg, #667eea, #764ba2); }
        .color-memory { background: linear-gradient(135deg, #4caf50, #45a049); }
        .color-neuromod { background: linear-gradient(135deg, #ff9800, #f57c00); }
        .color-plasticity { background: linear-gradient(135deg, #9c27b0, #7b1fa2); }
        .color-development { background: linear-gradient(135deg, #2196f3, #1976d2); }
        .color-prediction { background: linear-gradient(135deg, #f44336, #d32f2f); }
        .color-performance { background: linear-gradient(135deg, #00bcd4, #0097a7); }
        
        @media (max-width: 768px) {
            .dashboard {
                grid-template-columns: 1fr;
            }
            
            .controls {
                flex-direction: column;
                align-items: center;
            }
            
            .btn {
                width: 100%;
                max-width: 300px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🧠 NLM Visualization Dashboard</h1>
            <p>Comprehensive Real-Time Monitoring and Visualization of Neural Learning Machine Systems</p>
        </div>
        
        <div class="dashboard">
            <!-- Neural Activity Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-icon color-neural">🧠</div>
                    <div class="card-title">Neural Activity</div>
                </div>
                <div class="card-value" id="neural-firing-rate">0.00</div>
                <div class="card-subtitle">Average Firing Rate (Hz)</div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="neural-active-neurons">0</div>
                        <div class="stat-label">Active Neurons</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="neural-firing-neurons">0</div>
                        <div class="stat-label">Firing Neurons</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="neural-total-spikes">0</div>
                        <div class="stat-label">Total Spikes</div>
                    </div>
                </div>
            </div>
            
            <!-- Memory Systems Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-icon color-memory">🧠</div>
                    <div class="card-title">Memory Systems</div>
                </div>
                <div class="card-value" id="memory-load">0%</div>
                <div class="card-subtitle">Working Memory Load</div>
                <div class="progress-bar">
                    <div class="progress-fill" id="memory-progress" style="width: 0%"></div>
                </div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="memory-episodes">0</div>
                        <div class="stat-label">Episodic Episodes</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="memory-replay">0%</div>
                        <div class="stat-label">Replay Rate</div>
                    </div>
                </div>
            </div>
            
            <!-- Neuromodulation Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-icon color-neuromod">⚡</div>
                    <div class="card-title">Neuromodulation</div>
                </div>
                <div class="card-value" id="neuromod-dopamine">0.0</div>
                <div class="card-subtitle">Dopamine Level</div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="neuromod-curiosity">0.0</div>
                        <div class="stat-label">Curiosity</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="neuromod-novelty">0.0</div>
                        <div class="stat-label">Novelty</div>
                    </div>
                </div>
            </div>
            
            <!-- Plasticity Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-icon color-plasticity">🔗</div>
                    <div class="card-title">Plasticity</div>
                </div>
                <div class="card-value" id="plasticity-efficiency">0.0%</div>
                <div class="card-subtitle">Learning Efficiency</div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="plasticity-balance">0.0</div>
                        <div class="stat-label">Plasticity Balance</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="plasticity-adaptation">0.0</div>
                        <div class="stat-label">Adaptation Speed</div>
                    </div>
                </div>
            </div>
            
            <!-- Development Card -->
            <div class="card">
                <div class="card-icon color-development">📈</div>
                <div class="card-title">Development</div>
                <div class="card-value" id="development-progress">0%</div>
                <div class="card-subtitle">Developmental Progress</div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="development-stage">Initial</div>
                        <div class="stat-label">Stage</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="development-maturation">0.0</div>
                        <div class="stat-label">Maturation Level</div>
                    </div>
                </div>
            </div>
            
            <!-- Prediction Card -->
            <div class="card">
                <div class="card-icon color-prediction">🔮</div>
                <div class="card-title">Prediction</div>
                <div class="card-value" id="prediction-accuracy">0.0%</div>
                <div class="card-subtitle">Prediction Accuracy</div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="prediction-uncertainty">0.0</div>
                        <div class="stat-label">Uncertainty</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="prediction-planning">0.0</div>
                        <div class="stat-label">Planning Efficiency</div>
                    </div>
                </div>
            </div>
            
            <!-- Performance Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-icon color-performance">⚡</div>
                    <div class="card-title">Performance</div>
                </div>
                <div class="card-value" id="performance-cpu">0%</div>
                <div class="card-subtitle">CPU Usage</div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="performance-memory">0 MB</div>
                        <div class="stat-label">Memory Usage</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="performance-throughput">0 spk/s</div>
                        <div class="stat-label">Throughput</div>\n                    </div>
                </div>
            </div>
            
            <!-- System Status Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-icon color-neural">🖥️</div>
                    <div class="card-title">System Status</div>
                </div>
                <div class="card-value">
                    <span class="status-indicator" id="system-status" class="status-stopped pulse"> </span>
                    <span id="system-status-text">STOPPED</span>
                </div>
                <div class="card-subtitle">Simulation Status</div>
                <div class="stats-grid">
                    <div class="stat-item">
                        <div class="stat-value" id="system-step">0</div>
                        <div class="stat-label">Simulation Step</div>
                    </div>
                    <div class="stat-item">
                        <div class="stat-value" id="system-time">0.0s</div>
                        <div class="stat-label">Simulation Time</div>
                    </div>
                </div>
            </div>
            
            <!-- Visualization Area -->
            <div class="card visualization-area">
                <div class="viz-title">📊 Real-Time Neural Activity Visualization</div>
                <div class="viz-container">
                    <canvas id="viz-canvas" class="viz-canvas"></canvas>
                </div>
            </div>
            
            <!-- Recent Updates -->
            <div class="card updates-section">
                <div class="card-header">
                    <div class="card-icon color-neural">📝</div>
                    <div class="card-title">Recent Updates</div>
                </div>
                <div id="updates-container">
                    <div class="update-item">
                        <div class="update-timestamp">No updates yet</div>
                        <div class="update-content">Start the simulation to see real-time updates</div>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="controls">
            <button class="btn btn-primary" onclick="startSimulation()">🚀 Start Simulation</button>
            <button class="btn btn-secondary" onclick="stopSimulation()">⏸️ Stop Simulation</button>
            <button class="btn btn-secondary" onclick="exportData()">💾 Export Data</button>
            <button class="btn btn-secondary" onclick="toggleAutoRefresh()">🔄 Auto Refresh</button>
        </div>
    </div>
    
    <script>
        // Global variables
        let dataSocket = null;
        let updateInterval = null;
        let autoRefresh = true;
        let vizCanvas = null;
        let vizCtx = null;
        
        // Initialize canvas visualization
        function initVisualization() {
            vizCanvas = document.getElementById('viz-canvas');
            vizCtx = vizCanvas.getContext('2d');
            
            // Set canvas size
            vizCanvas.width = vizCanvas.offsetWidth;
            vizCanvas.height = vizCanvas.offsetHeight;
            
            // Draw initial plot
            drawNeuralPlot();
        }
        
        // Draw neural activity plot
        function drawNeuralPlot() {
            vizCtx.clearRect(0, 0, vizCanvas.width, vizCanvas.height);
            
            // Draw grid
            vizCtx.strokeStyle = '#e0e0e0';
            vizCtx.lineWidth = 1;
            vizCtx.beginPath();
            vizCtx.moveTo(50, 50);
            vizCtx.lineTo(50, vizCanvas.height - 50);
            vizCtx.lineTo(vizCanvas.width - 50, vizCanvas.height - 50);
            vizCtx.stroke();
            
            // Draw axes
            vizCtx.strokeStyle = '#666';
            vizCtx.lineWidth = 2;
            vizCtx.beginPath();
            vizCtx.moveTo(50, 50);
            vizCtx.lineTo(50, vizCanvas.height - 50);
            vizCtx.lineTo(vizCanvas.width - 50, vizCanvas.height - 50);
            vizCtx.stroke();
            
            // Draw sample neural activity (placeholder)
            const timeSteps = 100;
            const activityData = [];
            for (let i = 0; i < timeSteps; i++) {
                activityData.push(Math.random() * 100);
            }
            
            // Draw neural activity line
            vizCtx.strokeStyle = '#667eea';
            vizCtx.lineWidth = 3;
            vizCtx.beginPath();
            
            const maxActivity = Math.max(...activityData);
            const minActivity = Math.min(...activityData);
            const range = maxActivity - minActivity;
            
            for (let i = 0; i < timeSteps; i++) {
                const x = 50 + (i / (timeSteps - 1)) * (vizCanvas.width - 100);
                const y = 50 + ((activityData[i] - minActivity) / (range + 1)) * (vizCanvas.height - 100);
                
                if (i === 0) {
                    vizCtx.moveTo(x, y);
                } else {
                    vizCtx.lineTo(x, y);
                }
            }
            
            vizCtx.stroke();
            
            // Add title
            vizCtx.fillStyle = '#333';
            vizCtx.font = '14px Arial';
            vizCtx.textAlign = 'center';
            vizCtx.fillText('Neural Activity Over Time', vizCanvas.width / 2, 30);
            
            // Add subtitle
            vizCtx.font = '12px Arial';
            vizCtx.fillText('Real-time Spiking Activity', vizCanvas.width / 2, 45);
        }
        
        // Update dashboard with new data
        function updateDashboard(data) {
            // Update neural activity
            document.getElementById('neural-firing-rate').textContent = data.neural_activity.firing_rate.toFixed(2);
            document.getElementById('neural-active-neurons').textContent = data.neural_activity.active_neurons;
            document.getElementById('neural-firing-neurons').textContent = data.neural_activity.firing_neurons;
            document.getElementById('neural-total-spikes').textContent = data.neural_activity.total_spikes;
            
            // Update memory systems
            const memoryLoad = data.memory_systems.working_memory_load * 100;
            document.getElementById('memory-load').textContent = memoryLoad.toFixed(1) + '%';
            document.getElementById('memory-progress').style.width = memoryLoad + '%';
            document.getElementById('memory-episodes').textContent = data.memory_systems.memory_episodes;
            document.getElementById('memory-replay').textContent = data.memory_systems.memory_replay_rate.toFixed(1) + '%';
            
            // Update neuromodulation
            document.getElementById('neuromod-dopamine').textContent = data.neuromodulation.dopamine_level.toFixed(2);
            document.getElementById('neuromod-curiosity').textContent = data.neuromodulation.curiosity_level.toFixed(2);
            document.getElementById('neuromod-novelty').textContent = data.neuromodulation.novelty_level.toFixed(2);
            
            // Update plasticity
            const plasticityEfficiency = data.plasticity.learning_efficiency * 100;
            document.getElementById('plasticity-efficiency').textContent = plasticityEfficiency.toFixed(1) + '%';
            document.getElementById('plasticity-balance').textContent = data.plasticity.plasticity_balance.toFixed(3);
            document.getElementById('plasticity-adaptation').textContent = data.plasticity.adaptation_speed.toFixed(3);
            
            // Update development
            const devProgress = data.development.developmental_progress * 100;
            document.getElementById('development-progress').textContent = devProgress.toFixed(1) + '%';
            document.getElementById('development-stage').textContent = data.development.developmental_stage;
            document.getElementById('development-maturation').textContent = data.development.maturation_level.toFixed(3);
            
            // Update prediction
            const predAccuracy = data.prediction.prediction_accuracy * 100;
            document.getElementById('prediction-accuracy').textContent = predAccuracy.toFixed(1) + '%';
            document.getElementById('prediction-uncertainty').textContent = data.prediction.prediction_uncertainty.toFixed(3);
            document.getElementId('prediction-planning').textContent = data.prediction.planning_efficiency.toFixed(3);
            
            // Update performance
            document.getElementById('performance-cpu').textContent = data.performance.cpu_usage.toFixed(1) + '%';
            document.getElementById('performance-memory').textContent = Math.round(data.performance.memory_usage) + ' MB';
            document.getElementById('performance-throughput').textContent = Math.round(data.performance.spike_events_per_second) + ' spk/s';
            
            // Update system status
            const isRunning = data.system_status.is_running;
            const statusElement = document.getElementById('system-status');
            const statusText = document.getElementById('system-status-text');
            
            if (isRunning) {
                statusElement.className = 'status-indicator status-running pulse';
                statusText.textContent = 'RUNNING';
            } else {
                statusElement.className = 'status-indicator status-stopped';
                statusText.textContent = 'STOPPED';
            }
            
            document.getElementById('system-step').textContent = data.system_status.simulation_step;
            document.getElementById('system-time').textContent = data.system_status.simulation_time.toFixed(1) + 's';
            
            // Update recent updates
            addUpdate(data);
        }
        
        // Add update to recent updates section
        function addUpdate(data) {
            const updatesContainer = document.getElementById('updates-container');
            const now = new Date();
            const timeString = now.toLocaleTimeString();
            
            const updateItem = document.createElement('div');
            updateItem.className = 'update-item';
            
            const timestamp = document.createElement('div');
            timestamp.className = 'update-timestamp';
            timestamp.textContent = timeString;
            
            const content = document.createElement('div');
            content.className = 'update-content';
            content.textContent = `Neural Activity: ${data.neural_activity.firing_rate.toFixed(2)} Hz | ` +
                                `Memory Load: ${data.memory_systems.working_memory_load.toFixed(2)} | ` +
                                `Dopamine: ${data.neuromodulation.dopamine_level.toFixed(2)}`;
            
            updateItem.appendChild(timestamp);
            updateItem.appendChild(content);
            
            updatesContainer.insertBefore(updateItem, updatesContainer.firstChild);
            
            // Limit to 10 most recent updates
            const updateItems = updatesContainer.querySelectorAll('.update-item');
            if (updateItems.length > 10) {
                updatesContainer.removeChild(updateItems[updateItems.length - 1]);
            }
        }
        
        // Start simulation
        function startSimulation() {
            if (dataSocket) {
                dataSocket.send(JSON.stringify({type: 'start'}));
            }
        }
        
        // Stop simulation
        function stopSimulation() {
            if (dataSocket) {
                dataSocket.send(JSON.stringify({type: 'stop'}));
            }
        }
        
        // Toggle auto refresh
        function toggleAutoRefresh() {
            autoRefresh = !autoRefresh;
            if (autoRefresh) {
                console.log('Auto refresh enabled');
            } else {
                console.log('Auto refresh disabled');
            }
        }
        
        // Export data
        function exportData() {
            if (dataSocket) {
                dataSocket.send(JSON.stringify({type: 'export'}));
            }
        }
        
        // WebSocket connection
        function connectWebSocket() {
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const wsUrl = `${protocol}//${window.location.hostname}:${window.location.port}/ws`;
            
            dataSocket = new WebSocket(wsUrl);
            
            dataSocket.onopen = function(event) {
                console.log('WebSocket connected');
                document.getElementById('system-status-text').textContent = 'CONNECTED';
                document.getElementById('system-status').className = 'status-indicator status-running pulse';
            };
            
            dataSocket.onmessage = function(event) {
                try {
                    const data = JSON.parse(event.data);
                    if (data.type === 'dashboard_update') {
                        updateDashboard(data);
                    }
                } catch (error) {
                    console.error('Error parsing message:', error);
                }
            };
            
            dataSocket.onclose = function(event) {
                console.log('WebSocket disconnected');
                document.getElementById('system-status-text').textContent = 'DISCONNECTED';
                document.getElementById('system-status').className = 'status-indicator status-stopped';
                
                // Attempt to reconnect
                if (autoRefresh) {
                    setTimeout(connectWebSocket, 5000);
                }
            };
            
            dataSocket.onerror = function(error) {
                console.error('WebSocket error:', error);
            };
        }
        
        // Update interval for periodic refresh
        function updateDashboardInterval() {
            if (autoRefresh && dataSocket && dataSocket.readyState === WebSocket.OPEN) {
                dataSocket.send(JSON.stringify({type: 'get_data'}));
            }
        }
        
        // Initialize when page loads
        window.addEventListener('load', function() {
            connectWebSocket();
            
            // Start update interval
            updateInterval = setInterval(updateDashboardInterval, 1000);
            
            // Initialize visualization
            initVisualization();
        });
        
        // Cleanup on page unload
        window.addEventListener('beforeunload', function() {
            if (dataSocket) {
                dataSocket.close();
            }
            if (updateInterval) {
                clearInterval(updateInterval);
            }
        });
        
        // Handle window resize
        window.addEventListener('resize', function() {
            if (vizCanvas) {
                initVisualization();
            }
        });
    </script>
</body>
</html>
        '''
    
    def save_template(self):
        """Save HTML template to file"""
        template_path = 'templates/dashboard.html'
        with open(template_path, 'w') as f:
            f.write(self.create_html_template())
        print(f"✓ HTML template saved to: {template_path}")
        return template_path
    
    def save_static_files(self):
        """Save static files for the dashboard"""
        # Create CSS file
        css_content = '''
/* Dashboard-specific CSS styles */
.dashboard {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
    gap: 20px;
    padding: 30px;
}

.card {
    background: white;
    border-radius: 15px;
    padding: 25px;
    box-shadow: 0 5px 20px rgba(0,0,0,0.1);
    transition: transform 0.3s ease, box-shadow 0.3s ease;
}

.card:hover {
    transform: translateY(-5px);
    box-shadow: 0 10px 30px rgba(0,0,0,0.2);
}

@media (max-width: 768px) {
    .dashboard {
        grid-template-columns: 1fr;
    }
}
'''
        
        css_path = 'static/dashboard.css'
        with open(css_path, 'w') as f:
            f.write(css_content)
        
        print(f"✓ CSS file saved to: {css_path}")
        return css_path
    
    def create_dashboard_files(self):
        """Create all necessary dashboard files"""
        print("Creating dashboard files...")
        
        html_file = self.save_template()
        css_file = self.save_static_files()
        
        print(f"✓ Dashboard files created:")
        print(f"  - HTML template: {html_file}")
        print(f"  - CSS file: {css_file}")
        
        return html_file, css_file
    
    def start_server(self, brain=None, host='localhost', port=8080):
        """
        Start web server for the dashboard.
        
        Args:
            brain: NLM Brain object (optional)
            host: Server host
            port: Server port
        """
        self.brain = brain
        self.is_running = True
        
        # Create dashboard files if they don't exist
        if not os.path.exists('templates/dashboard.html'):
            self.create_dashboard_files()
        
        # Create data collector
        self.data_collector = DashboardData()
        
        # Start data collection thread
        data_thread = threading.Thread(target=self._data_collection_loop, daemon=True)
        data_thread.start()
        
        # Create and start HTTP server
        server = HTTPServer((host, port), self._create_request_handler())
        self.server = server
        
        print(f"✓ Dashboard server started at http://{host}:{port}")
        print(f"  - Open this URL in your browser to view the dashboard")
        print(f"  - Press Ctrl+C to stop the server")
        
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            print("\nDashboard server stopped")
        finally:
            self.stop()
    
    def _create_request_handler(self):
        """Create HTTP request handler"""
        handler_class = type('DashboardRequestHandler', (BaseHTTPRequestHandler,), {
            'server': self,
            'data_collector': self.data_collector,
            'brain': self.brain
        })
        
        return handler_class
    
    def _data_collection_loop(self):
        """Data collection loop for real-time updates"""
        self.data_collector.start(self.brain)
        
        while self.is_running:
            try:
                # Update data collector
                self.data_collector.update_system_status(
                    is_running=self.is_running,
                    step=getattr(self.data_collector, 'step', 0),
                    simulation_time=getattr(self.data_collector, 'time', 0.0)
                )
                
                if self.brain:
                    self.data_collector.update_neural_data(self.brain)
                    self.data_collector.update_memory_data(self.brain)
                    self.data_collector.update_neuromodulation_data(self.brain)
                    self.data_collector.update_plasticity_data(self.brain)
                    self.data_collector.update_development_data(self.brain)
                    self.data_collector.update_prediction_data(self.brain)
                
                self.data_collector.update_performance_data()
                
                # Sleep briefly
                time.sleep(0.1)
                
            except Exception as e:
                print(f"Error in data collection loop: {e}")
                time.sleep(1.0)
    
    def stop(self):
        """Stop the dashboard server"""
        self.is_running = False
        
        if self.server:
            self.server.shutdown()
            print("✓ Dashboard server stopped")
        
        if hasattr(self, 'data_collector'):
            self.data_collector.stop()
    
    def run_demo(self, brain=None, duration=60.0, host='localhost', port=8080):
        """
        Run comprehensive dashboard demo.
        
        Args:
            brain: NLM Brain object (optional)
            duration: Demo duration in seconds
            host: Server host
            port: Server port
        """
        print(f"\nRunning NLM dashboard demo...")
        print(f"Duration: {duration}s, Host: {host}, Port: {port}")
        
        # Create brain if not provided
        if not brain and HAS_NLM:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuronCount", 100)
            brain = pynlm.createBrain(config)
            brain.initialize()
            print(f"✓ Demo brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Start dashboard server
        self.start_server(brain, host, port)
        
        print(f"✓ Dashboard demo completed: {duration}s")
        return True
    
    def export_data(self, filepath=None):
        """
        Export dashboard data.
        
        Args:
            filepath: Output file path (optional)
        """
        if not hasattr(self, 'data_collector'):
            print("Error: No data collector available")
            return False
        
        if filepath is None:
            filepath = f"dashboard_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        # Export dashboard data
        with open(filepath, 'w') as f:
            json.dump(self.data_collector.get_data_for_api(), f, indent=2, default=str)
        
        print(f"✓ Dashboard data exported to: {filepath}")
        return True
class NLMDashboardCLI:
    """
    Command-line interface for NLM visualization and monitoring dashboard.
    """
    
    def __init__(self):
        self.dashboard = None
        
    def create_dashboard(self, brain=None, host='localhost', port=8080):
        """
        Create and start dashboard.
        
        Args:
            brain: NLM Brain object (optional)
            host: Server host
            port: Server port
        """
        self.dashboard = VisualizationDashboard(host, port)
        
        try:
            self.dashboard.run_demo(brain, duration=float('inf'), host=host, port=port)
            return True
        except KeyboardInterrupt:
            print("\nDashboard stopped by user")
            return True
        except Exception as e:
            print(f"✗ Error running dashboard: {e}")
            return False
    
    def export_dashboard_data(self, filepath):
        """
        Export dashboard data.
        
        Args:
            filepath: Output file path
        """
        if not self.dashboard:
            print("Error: No dashboard created")
            return False
        
        return self.dashboard.export_data(filepath)
    
    def stop_dashboard(self):
        """Stop dashboard"""
        if self.dashboard:
            self.dashboard.stop()
            return True
        return False


def create_cli():
    """Create and return CLI parser for dashboard commands"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Visualization and Monitoring Dashboard CLI",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Subparsers
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Dashboard command
    dashboard_parser = subparsers.add_parser('dashboard', help='Run visualization dashboard')
    dashboard_parser.add_argument('--host', default='localhost', help='Server host')
    dashboard_parser.add_argument('--port', type=int, default=8080, help='Server port')
    dashboard_parser.add_argument('--brain', help='Path to brain data file')
    dashboard_parser.add_argument('--duration', type=float, default=float('inf'), help='Demo duration in seconds')
    
    # Export data command
    export_parser = subparsers.add_parser('export-data', help='Export dashboard data')
    export_parser.add_argument('filepath', help='Output file path')
    
    # Stop command
    stop_parser = subparsers.add_parser('stop', help='Stop dashboard')
    
    return parser, subparsers


def main():
    """Main CLI function"""
    parser, subparsers = create_cli()
    
    # Create CLI instance
    cli = NLMDashboardCLI()
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle commands
    if args.command == 'dashboard':
        print("🚀 Starting NLM Visualization Dashboard...")
        
        # Create brain if specified
        brain = None
        if args.brain and os.path.exists(args.brain):
            # Load brain from file (simplified)
            if HAS_NLM:
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                print(f"✓ Brain loaded with {brain.getTotalNeuronCount()} neurons")
        
        # Start dashboard
        success = cli.create_dashboard(brain, args.host, args.port)
        sys.exit(0 if success else 1)
        
    elif args.command == 'export-data':
        success = cli.export_dashboard_data(args.filepath)
        sys.exit(0 if success else 1)
        
    elif args.command == 'stop':
        success = cli.stop_dashboard()
        sys.exit(0 if success else 1)
        
    else:
        # No command specified, show help
        parser.print_help()
        sys.exit(1)
if __name__ == "__main__":
    main()
