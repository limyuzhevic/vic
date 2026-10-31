#!/usr/bin/env python3
"""
Advanced Interactive Visualization and Monitoring for NLM Brain Simulation

This script provides real-time visualization, monitoring, and interactive control
tools for NLM brain simulations, enabling users to observe and interact with
the simulation in real-time.

Features:
- Real-time neural activity visualization
- Interactive parameter control
- Performance monitoring
- Data export and analysis
- Multi-view display
"""

import tkinter as tk
from tkinter import ttk, messagebox
import matplotlib
matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import numpy as np
import threading
import time
import json
import sys
from pathlib import Path
from typing import Dict, Any, Optional, List
import subprocess
import socket
import struct

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

from src.core.Config.Config import Config
from src.core.Logger.Logger import Logger, LogLevel
from src.brain.Brain import Brain

class NLMVisualizer:
    """Interactive visualization and monitoring tool for NLM simulations."""
    
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("NLM Brain Simulation Visualizer")
        self.root.geometry("1200x800")
        
        self.logger = Logger.getGlobal()
        self.logger->setLevel(LogLevel::Info)
        
        # Simulation state
        self.simulation_running = False
        self.current_step = 0
        self.total_steps = 0
        self.brain_config = None
        self.brain_instance = None
        self.config_file = None
        
        # Visualization data
        self.neuron_activity_data = []
        self.synaptic_weight_data = []
        self.firing_rate_history = []
        self.spike_count_history = []
        self.plots_data = {}
        
        # Visualization settings
        self.update_interval = 100  # ms
        self.max_data_points = 1000
        self.color_scheme = {
            'background': '#2b2b2b',
            'neuron_active': '#ff6b6b',
            'neuron_inactive': '#4a4a4a',
            'synapse_excitatory': '#4caf50',
            'synapse_inhibitory': '#2196f3',
            'text': '#ffffff'
        }
        
        # Create GUI components
        self.create_gui()
        
    def create_gui(self):
        """Create the main GUI interface."""
        # Main container
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Control panel
        control_frame = ttk.LabelFrame(main_frame, text="Controls", padding=10)
        control_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5)
        
        # Simulation controls
        sim_control_frame = ttk.LabelFrame(control_frame, text="Simulation Control", padding=5)
        sim_control_frame.pack(fill=tk.X, pady=5)
        
        self.run_button = ttk.Button(sim_control_frame, text="Start Simulation", 
                                   command=self.toggle_simulation)
        self.run_button.pack(fill=tk.X, pady=2)
        
        self.stop_button = ttk.Button(sim_control_frame, text="Stop Simulation", 
                                    command=self.stop_simulation, state=tk.DISABLED)
        self.stop_button.pack(fill=tk.X, pady=2)
        
        # Step controls
        step_frame = ttk.Frame(sim_control_frame)
        step_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(step_frame, text="Total Steps:").pack(side=tk.LEFT)
        self.steps_var = tk.IntVar(value=1000)
        self.steps_entry = ttk.Entry(step_frame, textvariable=self.steps_var, width=10)
        self.steps_entry.pack(side=tk.LEFT, padx=5)
        
        ttk.Label(step_frame, text="Timestep: 0.001").pack(side=tk.LEFT, padx=10)
        
        # Configuration controls
        config_frame = ttk.LabelFrame(control_frame, text="Configuration", padding=5)
        config_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(config_frame, text="Config File:").grid(row=0, column=0, sticky=tk.W)
        self.config_var = tk.StringVar(value="configs/default.cfg")
        self.config_entry = ttk.Entry(config_frame, textvariable=self.config_var, width=20)
        self.config_entry.grid(row=0, column=1, padx=5)
        
        self.browse_button = ttk.Button(config_frame, text="Browse", 
                                       command=self.browse_config_file)
        self.browse_button.grid(row=0, column=2, padx=5)
        
        # Visualization controls
        viz_frame = ttk.LabelFrame(control_frame, text="Visualization", padding=5)
        viz_frame.pack(fill=tk.X, pady=5)
        
        # Neuron view
        self.neuron_view_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(viz_frame, text="Show Neurons", 
                      variable=self.neuron_view_var, 
                      command=self.toggle_neuron_view).pack(anchor=tk.W)
        
        # Synapse view
        self.synapse_view_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(viz_frame, text="Show Synapses", 
                      variable=self.synapse_view_var, 
                      command=self.toggle_synapse_view).pack(anchor=tk.W)
        
        # Metrics display
        self.metrics_frame = ttk.LabelFrame(viz_frame, text="Metrics", padding=5)
        self.metrics_frame.pack(fill=tk.X, pady=5)
        
        # Real-time metrics
        self.metrics_labels = {}
        metrics_data = [
            ("Total Neurons", "neurons"),
            ("Total Synapses", "synapses"),
            ("Active Neurons", "active_neurons"),
            ("Firing Rate", "firing_rate"),
            ("Total Spikes", "total_spikes"),
            ("Avg Synaptic Weight", "avg_weight")
        ]
        
        for i, (label_text, key) in enumerate(metrics_data):
            row_frame = ttk.Frame(self.metrics_frame)
            row_frame.grid(row=i, column=0, sticky=tk.W, pady=2)
            
            ttk.Label(row_frame, text=label_text + ":", width=20).pack(side=tk.LEFT)
            self.metrics_labels[key] = ttk.Label(row_frame, text="0")
            self.metrics_labels[key].pack(side=tk.LEFT, padx=5)
        
        # Control buttons for metrics
        button_frame = ttk.Frame(self.metrics_frame)
        button_frame.grid(row=len(metrics_data), column=0, sticky=tk.W, pady=5)
        
        ttk.Button(button_frame, text="Refresh", command=self.refresh_metrics).pack(side=tk.LEFT, padx=2)
        ttk.Button(button_frame, text="Export", command=self.export_metrics).pack(side=tk.LEFT, padx=2)
        
        # Main visualization area
        viz_main_frame = ttk.Frame(main_frame)
        viz_main_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=5)
        
        # Create matplotlib figures
        self.create_visualizations(viz_main_frame)
        
        # Status bar
        status_frame = ttk.Frame(self.root, relief=tk.SUNKEN, padding=5)
        status_frame.pack(side=tk.BOTTOM, fill=tk.X)
        
        self.status_label = ttk.Label(status_frame, text="Ready")
        self.status_label.pack(side=tk.LEFT)
        
        self.step_label = ttk.Label(status_frame, text="Step: 0 / 0")
        self.step_label.pack(side=tk.RIGHT)
        
    def create_visualizations(self, parent_frame):
        """Create matplotlib visualization panels."""
        # Create tabs for different views
        tab_control = ttk.Notebook(parent_frame)
        tab_control.pack(fill=tk.BOTH, expand=True)
        
        # Neuron activity tab
        neuron_tab = ttk.Frame(tab_control)
        tab_control.add(neuron_tab, text="Neuron Activity")
        
        # Create neuron activity plot
        self.neuron_fig = Figure(figsize=(10, 6), dpi=100)
        self.neuron_ax = self.neuron_fig.add_subplot(111)
        self.neuron_canvas = FigureCanvasTkAgg(self.neuron_fig, parent=neuron_tab)
        self.neuron_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # Synapse dynamics tab
        synapse_tab = ttk.Frame(tab_control)
        tab_control.add(synapse_tab, text="Synapse Dynamics")
        
        # Create synapse plot
        self.synapse_fig = Figure(figsize=(10, 6), dpi=100)
        self.synapse_ax = self.synapse_fig.add_subplot(111)
        self.synapse_canvas = FigureCanvasTkAgg(self.synapse_fig, parent=synapse_tab)
        self.synapse_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # Metrics tab
        metrics_tab = ttk.Frame(tab_control)
        tab_control.add(metrics_tab, text="Metrics")
        
        # Create metrics plot
        self.metrics_fig = Figure(figsize=(10, 6), dpi=100)
        self.metrics_ax = self.metrics_fig.add_subplot(111)
        self.metrics_canvas = FigureCanvasTkAgg(self.metrics_fig, parent=metrics_tab)
        self.metrics_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        # Initialize plots
        self.initialize_plots()
        
    def initialize_plots(self):
        """Initialize matplotlib plots."""
        # Neuron activity plot
        self.neuron_ax.set_title("Neuron Activity Over Time")
        self.neuron_ax.set_xlabel("Step")
        self.neuron_ax.set_ylabel("Activity Level")
        self.neuron_ax.set_xlim(0, self.max_data_points)
        self.neuron_ax.set_ylim(0, 1.0)
        self.neuron_ax.grid(True, alpha=0.3)
        
        # Synapse dynamics plot
        self.synapse_ax.set_title("Synaptic Weight Distribution")
        self.synapse_ax.set_xlabel("Synapse Index")
        self.synapse_ax.set_ylabel("Weight")
        self.synapse_ax.grid(True, alpha=0.3)
        
        # Metrics plot
        self.metrics_ax.set_title("Simulation Metrics")
        self.metrics_ax.set_xlabel("Step")
        self.metrics_ax.set_ylabel("Value")
        self.metrics_ax.grid(True, alpha=0.3)
        
    def toggle_simulation(self):
        """Toggle simulation running state."""
        if self.simulation_running:
            self.stop_simulation()
        else:
            self.start_simulation()
            
    def start_simulation(self):
        """Start the simulation."""
        if self.brain_instance == None:
            # Create brain instance
            if self.config_var.get() and Path(self.config_var.get()).exists():
                self.brain_config = Config()
                if self.brain_config->loadFromFile(self.config_var.get()) {
                    self.brain_instance = Brain(this->brain_config);
                    if (!this->brain_instance->initialize()) {
                        messagebox.showerror("Error", "Failed to initialize brain");
                        return;
                    }
                } else {
                    // Create default configuration
                    this->brain_config = Config();
                    this->brain_config->set("neuron_count", 500);
                    this->brain_config->set("region_count", 1);
                    this->brain_config->set("connection_probability", 0.15);
                    this->brain_config->set("random_seed", 42);
                    this->brain_config->set("simulation_timestep", 0.001);
                    
                    this->brain_instance = Brain(this->brain_config);
                    if (!this->brain_instance->initialize()) {
                        messagebox.showerror("Error", "Failed to initialize brain");
                        return;
                    }
                }
            } else {
                // Create configuration
                this->brain_config = Config();
                this->brain_config->set("neuron_count", 500);
                this->brain_config->set("region_count", 1);
                this->brain_config->set("connection_probability", 0.15);
                this->brain_config->set("random_seed", 42);
                this->brain_config->set("simulation_timestep", 0.001);
                
                this->brain_instance = Brain(this->brain_config);
                if (!this->brain_instance->initialize()) {
                    messagebox.showerror("Error", "Failed to initialize brain");
                    return;
                }
            }
            
        self.simulation_running = true;
        self.run_button["state"] = tk.DISABLED;
        self.stop_button["state"] = tk.NORMAL;
        self.status_label["text"] = "Simulation running...";
        
        // Start simulation thread
        self.simulation_thread = threading.Thread(target=self.run_simulation_loop);
        self.simulation_thread.daemon = true;
        this->simulation_thread->start();
        
    def run_simulation_loop(self):
        """Main simulation loop running in background thread."""
        this->total_steps = this->steps_var.get();
        
        for (int step = 0; step < this->total_steps && this->simulation_running; ++step) {
            Timestamp current_time = step * 0.001;
            
            // Apply sensory input
            if (step % 10 == 0) {
                this->brain_instance->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            this->brain_instance->step(step, current_time);
            
            // Update data for visualization
            this->update_visualization_data(step);
            
            // Update UI (from main thread)
            this->root.after(0, [this, step]()->void {
                this->current_step = step;
                this->update_plots();
                this->update_metrics();
                this->step_label["text"] = std::format("Step: {}/{}", this->current_step, this->total_steps);
            });
            
            // Control simulation speed
            std::this_thread::sleep_for(std::chrono::milliseconds(this->update_interval));
        }
        
        if (this->simulation_running) {
            this->root.after(0, [this]()->void {
                this->stop_simulation();
                messagebox.showinfo("Simulation", "Simulation completed successfully");
            });
        }
        
    def update_visualization_data(self, step):
        """Update visualization data from brain simulation."""
        // Get neuron activity
        auto neurons = this->brain_instance->get_regions()[0]->getAllNeurons();
        std::vector<float> activity_levels;
        
        for (auto* neuron : neurons) {
            // Calculate activity level based on membrane potential
            float potential = neuron->getMembranePotential();
            float activity = (potential + 100.0f) / 200.0f;  // Normalize to [0, 1]
            activity = std::max(0.0f, std::min(1.0f, activity));
            activity_levels.push_back(activity);
        }
        
        this->neuron_activity_data.push_back(activity_levels);
        if (this->neuron_activity_data.size() > this->max_data_points) {
            this->neuron_activity_data.erase(this->neuron_activity_data.begin());
        }
        
        // Get synaptic weights
        auto synapses = this->brain_instance->get_regions()[0]->getSynapses();
        std::vector<float> weights;
        for (auto* syn : synapses) {
            weights.push_back(syn->getWeight());
        }
        
        this->synaptic_weight_data.push_back(weights);
        if (this->synaptic_weight_data.size() > this->max_data_points) {
            this->synaptic_weight_data.erase(this->synaptic_weight_data.begin());
        }
        
        // Update firing rate history
        float avg_firing_rate = this->brain_instance->getAverageFiringRate();
        this->firing_rate_history.push_back(avg_firing_rate);
        if (this->firing_rate_history.size() > this->max_data_points) {
            this->firing_rate_history.erase(this->firing_rate_history.begin());
        }
        
        // Update spike count history
        size_t total_spikes = this->brain_instance->getTotalSpikeCount();
        this->spike_count_history.push_back(static_cast<float>(total_spikes));
        if (this->spike_count_history.size() > this->max_data_points) {
            this->spike_count_history.erase(this->spike_count_history.begin());
        }
        
    }
    
    def update_plots(self):
        """Update matplotlib plots with new data."""
        // Clear and redraw neuron activity plot
        this->neuron_ax.clear();
        
        if (!this->neuron_activity_data.empty()) {
            std::vector<std::vector<float>> &data = this->neuron_activity_data;
            std::vector<float> time_points;
            
            for (int i = 0; i < std::min(50, (int)data.size()); ++i) {
                time_points.push_back(i * 10);
            }
            
            // Plot average activity across all neurons
            std::vector<float> avg_activity;
            for (const auto& neuron_frame : data) {
                float sum = 0.0f;
                for (float activity : neuron_frame) {
                    sum += activity;
                }
                if (!neuron_frame.empty()) {
                    avg_activity.push_back(sum / neuron_frame.size());
                }
            }
            
            this->neuron_ax.plot(time_points, avg_activity, 'b-', linewidth=2);
            this->neuron_ax.fill_between(time_points, 0, avg_activity, alpha=0.3);
        }
        
        this->neuron_ax.set_title("Neuron Activity Over Time");
        this->neuron_ax.set_xlabel("Step");
        this->neuron_ax.set_ylabel("Activity Level");
        this->neuron_ax.set_xlim(0, std::min(this->max_data_points * 10, this->total_steps));
        this->neuron_ax.set_ylim(0, 1.0);
        this->neuron_ax.grid(True, alpha=0.3);
        
        this->neuron_canvas.draw()
        
        // Update synapse dynamics plot
        this->synapse_ax.clear();
        
        if (!this->synaptic_weight_data.empty()) {
            const std::vector<float> &weights = this->synaptic_weight_data.back();
            
            if (!weights.empty()) {
                // Create histogram of synaptic weights
                this->synapse_ax.hist(weights, bins=50, alpha=0.7, color='skyblue', edgecolor='black');
                this->synapse_ax.set_xlabel("Synaptic Weight");
                this->synapse_ax.set_ylabel("Count");
                this->synapse_ax.set_title("Synaptic Weight Distribution");
            }
        }
        
        this->synapse_ax.grid(True, alpha=0.3);
        this->synapse_canvas.draw()
        
        // Update metrics plot
        this->metrics_ax.clear();
        
        // Plot multiple metrics
        if (!this->firing_rate_history.empty()) {
            std::vector<float> &firing_rate = this->firing_rate_history;
            std::vector<float> time_points;
            
            for (int i = 0; i < std::min(100, (int)firing_rate.size()); ++i) {
                time_points.push_back(i * 10);
            }
            
            this->metrics_ax.plot(time_points, firing_rate, 'r-', linewidth=2, label="Firing Rate");
        }
        
        if (!this->spike_count_history.empty()) {
            std::vector<float> &spike_counts = this->spike_count_history;
            std::vector<float> time_points;
            
            for (int i = 0; i < std::min(100, (int)spike_counts.size()); ++i) {
                time_points.push_back(i * 10);
            }
            
            this->metrics_ax.plot(time_points, spike_counts, 'g-', linewidth=2, label="Total Spikes");
        }
        
        this->metrics_ax.set_title("Simulation Metrics");
        this->metrics_ax.set_xlabel("Step");
        this->metrics_ax.set_ylabel("Value");
        this->metrics_ax.legend();
        this->metrics_ax.grid(True, alpha=0.3);
        
        this->metrics_canvas.draw()
        
    }
    
    def update_metrics(self):
        """Update metrics display."""
        if (this->brain_instance == None) return;
        
        // Update metrics labels
        this->metrics_labels["neurons"]->"text" = std::to_string(this->brain_instance->getTotalNeuronCount());
        this->metrics_labels["synapses"]->"text" = std::to_string(this->brain_instance->getTotalSynapseCount());
        this->metrics_labels["active_neurons"]->"text" = std::to_string(this->brain_instance->getActiveNeuronCount());
        this->metrics_labels["firing_rate"]->"text" = std::to_string(this->brain_instance->getAverageFiringRate());
        this->metrics_labels["total_spikes"]->"text" = std::to_string(this->brain_instance->getTotalSpikeCount());
        
        // Calculate average synaptic weight
        auto synapses = this->brain_instance->get_regions()[0]->getSynapses();
        float total_weight = 0.0f;
        if (!synapses.empty()) {
            for (auto* syn : synapses) {
                total_weight += std::abs(syn->getWeight());
            }
            total_weight /= synapses.size();
        }
        this->metrics_labels["avg_weight"]->"text" = std::to_string(total_weight);
        
    }
    
    def toggle_neuron_view(self):
        """Toggle neuron view visibility."""
        // Implementation for showing/hiding neuron activity
        pass
        
    def toggle_synapse_view(self):
        """Toggle synapse view visibility."""
        // Implementation for showing/hiding synapse dynamics
        pass
        
    def browse_config_file(self):
        """Open file dialog to browse configuration files."""
        file_path = tk.filedialog.askopenfilename(
            title="Select Configuration File",
            filetypes=[("Config Files", "*.cfg *.json *.yaml"), ("All Files", "*")]
        )
        
        if file_path:
            this->config_var.set(file_path);
        
    }
    
    def refresh_metrics(self):
        """Refresh metrics from current simulation."""
        this->update_metrics();
        messagebox.showinfo("Info", "Metrics refreshed");
        
    }
    
    def export_metrics(self):
        """Export metrics to file."""
        file_path = tk.filedialog.asksaveasfilename(
            title="Export Metrics",
            defaultextension=".json",
            filetypes=[("JSON Files", "*.json"), ("CSV Files", "*.csv"), ("All Files", "*")]
        )
        
        if file_path:
            try {
                // Export metrics as JSON
                json_data = {
                    "timestamp": std::chrono::system_clock::now(),
                    "total_neurons": this->brain_instance->getTotalNeuronCount(),
                    "total_synapses": this->brain_instance->getTotalSynapseCount(),
                    "active_neurons": this->brain_instance->getActiveNeuronCount(),
                    "firing_rate": this->brain_instance->getAverageFiringRate(),
                    "total_spikes": this->brain_instance->getTotalSpikeCount(),
                    "neuron_activity_data": this->neuron_activity_data,
                    "synaptic_weight_data": this->synaptic_weight_data,
                    "firing_rate_history": this->firing_rate_history,
                    "spike_count_history": this->spike_count_history
                };
                
                std::ofstream file(file_path);
                if (file.is_open()) {
                    json::to_stream(json_data, file);
                    messagebox.showinfo("Info", "Metrics exported successfully");
                } else {
                    messagebox.showerror("Error", "Failed to open file for writing");
                }
            } catch (const std::exception& e) {
                messagebox.showerror("Error", "Failed to export metrics: " + std::string(e.what()));
            }
        
    }
    
    def stop_simulation(self):
        """Stop the simulation."""
        this->simulation_running = false;
        this->run_button["state"] = tk.NORMAL;
        this->stop_button["state"] = tk.DISABLED;
        this->status_label["text"] = "Simulation stopped";
        
    }
    
    def run(self):
        """Start the GUI application."""
        this->root.mainloop();
        
    }

int main():
    // Initialize SDL and GUI library
    if (!SDL_WasInit(0)) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            std::cerr << "Failed to initialize SDL" << std::endl;
            return 1;
        }
    }
    
    try {
        NLMVisualizer visualizer;
        visualizer.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    SDL_Quit();
    return 0;
}
