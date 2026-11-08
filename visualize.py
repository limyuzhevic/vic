#!/usr/bin/env python3
"""
NLM Visualization Tool

This script provides visualization capabilities for NLM (熙然) experiments.
It can read experiment data and create plots using matplotlib.

Usage:
  python visualize.py --file data.csv --type [metrics|neural|reward|development]
  python visualize.py --directory results/ --all

Features:
  - Metrics visualization (firing rates, reward, novelty, curiosity)
  - Neural activity patterns
  - Reward prediction and learning curves
  - Development over time
  - Interactive plots with zoom/pan
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import pandas as pd
import argparse
import os
from pathlib import Path
from typing import List, Dict, Optional
import json

def load_experiment_data(file_path: str) -> pd.DataFrame:
    """Load experiment data from CSV file."""
    try:
        df = pd.read_csv(file_path)
        return df
    except Exception as e:
        print(f"Error loading file {file_path}: {e}")
        return pd.DataFrame()

def create_metrics_plot(df: pd.DataFrame, save_path: str = None):
    """Create a comprehensive metrics visualization."""
    fig, axes = plt.subplots(3, 2, figsize=(15, 12))
    fig.suptitle('NLM Experiment Metrics', fontsize=16, fontweight='bold')
    
    # Normalize time axis
    if 'step' in df.columns:
        time = df['step'] / df['step'].max()
    else:
        time = np.arange(len(df))
    
    # 1. Total Reward over time
    if 'total_reward' in df.columns:
        ax = axes[0, 0]
        ax.plot(time, df['total_reward'], linewidth=2, color='green')
        ax.set_title('Total Reward Over Time')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Total Reward')
        ax.grid(True, alpha=0.3)
    
    # 2. Average Firing Rate
    if 'avg_firing_rate' in df.columns:
        ax = axes[0, 1]
        ax.plot(time, df['avg_firing_rate'], linewidth=2, color='red')
        ax.set_title('Average Firing Rate')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.grid(True, alpha=0.3)
    
    # 3. Novelty over time
    if 'final_novelty' in df.columns:
        ax = axes[1, 0]
        ax.plot(time, df['final_novelty'], linewidth=2, color='blue')
        ax.set_title('Novelty Detection')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Novelty Level')
        ax.grid(True, alpha=0.3)
    
    # 4. Curiosity over time
    if 'final_curiosity' in df.columns:
        ax = axes[1, 1]
        ax.plot(time, df['final_curiosity'], linewidth=2, color='orange')
        ax.set_title('Curiosity Level')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Curiosity Level')
        ax.grid(True, alpha=0.3)
    
    # 5. Developmental stages
    if 'developmental_stage' in df.columns:
        ax = axes[2, 0]
        # Plot developmental stage as text
        for idx, row in df.iterrows():
            ax.text(row['developmental_stage'], row['developmental_age'], 
                   f"Step {int(row['step'])}", fontsize=8)
        ax.set_title('Development Stages')
        ax.set_xlabel('Stage')
        ax.set_ylabel('Age')
        ax.grid(True, alpha=0.3)
    
    # 6. Memory episodes stored
    if 'memory_episodes_stored' in df.columns:
        ax = axes[2, 1]
        ax.plot(time, df['memory_episodes_stored'], linewidth=2, color='purple')
        ax.set_title('Memory Episodes Stored')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Episode Count')
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"Saved metrics plot to {save_path}")
    
    return fig

def create_reward_prediction_plot(df: pd.DataFrame, save_path: str = None):
    """Create reward prediction and learning visualization."""
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Reward Prediction and Learning', fontsize=16, fontweight='bold')
    
    if 'step' in df.columns:
        time = df['step'] / df['step'].max()
    else:
        time = np.arange(len(df))
    
    # Reward over time with prediction error
    if 'reward' in df.columns:
        ax = axes[0, 0]
        ax.plot(time, df['reward'], linewidth=2, label='Actual Reward', color='green')
        if 'prediction_error' in df.columns:
            ax.plot(time, df['prediction_error'], linewidth=1, label='Prediction Error', color='red', alpha=0.7)
        ax.set_title('Reward and Prediction Error')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Reward/Error')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    # Dopamine levels over time
    if 'dopamine_level' in df.columns:
        ax = axes[0, 1]
        ax.plot(time, df['dopamine_level'], linewidth=2, color='gold')
        ax.set_title('Dopamine Levels')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Dopamine Level')
        ax.grid(True, alpha=0.3)
    
    # Curiosity and novelty correlation
    if 'final_curiosity' in df.columns and 'final_novelty' in df.columns:
        ax = axes[1, 0]
        scatter = ax.scatter(df['final_curiosity'], df['final_novelty'], 
                           c=time, cmap='viridis', alpha=0.7, s=50)
        ax.set_title('Curiosity vs Novelty Correlation')
        ax.set_xlabel('Curiosity Level')
        ax.set_ylabel('Novelty Level')
        plt.colorbar(scatter, ax=ax, label='Time')
        ax.grid(True, alpha=0.3)
    
    # Learning curve (reward per step)
    if 'reward' in df.columns:
        ax = axes[1, 1]
        rolling_reward = df['reward'].rolling(window=min(50, len(df)//10)).mean()
        ax.plot(time, rolling_reward, linewidth=2, color='blue')
        ax.set_title('Learning Curve (Smoothed Reward)')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Average Reward')
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"Saved reward prediction plot to {save_path}")
    
    return fig

def create_neural_activity_plot(df: pd.DataFrame, save_path: str = None):
    """Create neural activity and connectivity visualization."""
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Neural Activity and Connectivity', fontsize=16, fontweight='bold')
    
    if 'step' in df.columns:
        time = df['step'] / df['step'].max()
    else:
        time = np.arange(len(df))
    
    # Firing neuron count
    if 'firing_neuron_count' in df.columns:
        ax = axes[0, 0]
        ax.plot(time, df['firing_neuron_count'], linewidth=2, color='red')
        ax.set_title('Number of Firing Neurons')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Neuron Count')
        ax.grid(True, alpha=0.3)
    
    # Total spike count
    if 'total_spikes' in df.columns:
        ax = axes[0, 1]
        ax.plot(time, df['total_spikes'], linewidth=2, color='darkred')
        ax.set_title('Total Spike Count')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Spike Count')
        ax.grid(True, alpha=0.3)
    
    # Synaptic weight statistics
    if 'average_weight' in df.columns:
        ax = axes[1, 0]
        ax.plot(time, df['average_weight'], linewidth=2, color='brown')
        ax.set_title('Average Synaptic Weight')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Weight Value')
        ax.grid(True, alpha=0.3)
    
    # Maximum weight
    if 'max_weight' in df.columns:
        ax = axes[1, 1]
        ax.plot(time, df['max_weight'], linewidth=2, color='maroon')
        ax.set_title('Maximum Synaptic Weight')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Max Weight')
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"Saved neural activity plot to {save_path}")
    
    return fig

def create_development_plot(df: pd.DataFrame, save_path: str = None):
    """Create development over time visualization."""
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Development Over Time', fontsize=16, fontweight='bold')
    
    if 'step' in df.columns:
        time = df['step'] / df['step'].max()
    else:
        time = np.arange(len(df))
    
    # Developmental age
    if 'developmental_age' in df.columns:
        ax = axes[0, 0]
        ax.plot(time, df['developmental_age'], linewidth=2, color='teal')
        ax.set_title('Developmental Age')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Age')
        ax.grid(True, alpha=0.3)
    
    # Structural plasticity (synaptogenesis and pruning)
    if 'synaptogenesis_rate' in df.columns and 'pruning_rate' in df.columns:
        ax = axes[0, 1]
        ax.plot(time, df['synaptogenesis_rate'], linewidth=2, label='Synaptogenesis', color='cyan')
        ax.plot(time, df['pruning_rate'], linewidth=2, label='Pruning', color='navy')
        ax.set_title('Structural Plasticity Rates')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Rate')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    # Learning curves by developmental stage
    if 'developmental_stage' in df.columns and 'total_reward' in df.columns:
        ax = axes[1, 0]
        for stage in df['developmental_stage'].unique():
            stage_data = df[df['developmental_stage'] == stage]
            if len(stage_data) > 0:
                ax.plot(time[stage_data.index], stage_data['total_reward'], 
                       linewidth=2, label=stage, alpha=0.7)
        ax.set_title('Reward by Developmental Stage')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Total Reward')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    # Memory consolidation
    if 'memory_episodes_stored' in df.columns:
        ax = axes[1, 1]
        # Normalize memory count for better visualization
        normalized_memory = df['memory_episodes_stored'] / df['memory_episodes_stored'].max() if df['memory_episodes_stored'].max() > 0 else df['memory_episodes_stored']
        ax.plot(time, normalized_memory, linewidth=2, color='indigo')
        ax.set_title('Memory Consolidation Progress')
        ax.set_xlabel('Normalized Time')
        ax.set_ylabel('Memory Storage (Normalized)')
        ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"Saved development plot to {save_path}")
    
    return fig

def main():
    parser = argparse.ArgumentParser(description="NLM Experiment Visualization Tool")
    parser.add_argument("--file", help="Path to experiment data CSV file")
    parser.add_argument("--directory", help="Path to directory containing experiment data files")
    parser.add_argument("--type", choices=['metrics', 'neural', 'reward', 'development', 'all'], 
                       default='all', help="Type of visualization to create")
    parser.add_argument("--output-dir", default="./plots", help="Output directory for plots")
    parser.add_argument("--format", choices=['png', 'pdf', 'svg'], default='png', help="Output format")
    
    args = parser.parse_args()
    
    # Create output directory if it doesn't exist
    os.makedirs(args.output_dir, exist_ok=True)
    
    if args.directory:
        # Process all CSV files in directory
        directory_path = Path(args.directory)
        csv_files = list(directory_path.glob("*.csv"))
        
        if not csv_files:
            print(f"No CSV files found in directory {args.directory}")
            return
        
        for csv_file in csv_files:
            print(f"Processing {csv_file.name}...")
            df = load_experiment_data(str(csv_file))
            if df.empty:
                continue
            
            # Create visualizations based on type
            if args.type in ['metrics', 'all']:
                plot_path = os.path.join(args.output_dir, 
                                       f"{csv_file.stem}_metrics.{args.format}")
                create_metrics_plot(df, plot_path)
            
            if args.type in ['neural', 'all']:
                plot_path = os.path.join(args.output_dir, 
                                       f"{csv_file.stem}_neural.{args.format}")
                create_neural_activity_plot(df, plot_path)
            
            if args.type in ['reward', 'all']:
                plot_path = os.path.join(args.output_dir, 
                                       f"{csv_file.stem}_reward.{args.format}")
                create_reward_prediction_plot(df, plot_path)
            
            if args.type in ['development', 'all']:
                plot_path = os.path.join(args.output_dir, 
                                       f"{csv_file.stem}_development.{args.format}")
                create_development_plot(df, plot_path)
    
    elif args.file:
        # Process single file
        df = load_experiment_data(args.file)
        if df.empty:
            return
        
        # Create visualizations based on type
        if args.type in ['metrics', 'all']:
            plot_path = os.path.join(args.output_dir, 
                                   f"{Path(args.file).stem}_metrics.{args.format}")
            create_metrics_plot(df, plot_path)
        
        if args.type in ['neural', 'all']:
            plot_path = os.path.join(args.output_dir, 
                                   f"{Path(args.file).stem}_neural.{args.format}")
            create_neural_activity_plot(df, plot_path)
        
        if args.type in ['reward', 'all']:
            plot_path = os.path.join(args.output_dir, 
                                   f"{Path(args.file).stem}_reward.{args.format}")
            create_reward_prediction_plot(df, plot_path)
        
        if args.type in ['development', 'all']:
            plot_path = os.path.join(args.output_dir, 
                                   f"{Path(args.file).stem}_development.{args.format}")
            create_development_plot(df, plot_path)
    
    else:
        print("Error: Either --file or --directory must be specified")
        parser.print_help()

if __name__ == "__main__":
    main()