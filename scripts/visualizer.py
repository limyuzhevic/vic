#!/usr/bin/env python3
"""
NLM Visualizer Script
Interactive visualization of NLM simulations with matplotlib
"""

import sys
import os
import time
import threading
import queue
from typing import Optional, List

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import pynlm
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

def simple_neuron_plot(brain, interval=1000):
    """Create a simple plot showing neuron firing over time"""
    plt.figure(figsize=(12, 8))
    
    # Create subplots
    ax1 = plt.subplot(2, 2, 1)
    ax2 = plt.subplot(2, 2, 2)
    ax3 = plt.subplot(2, 2, 3)
    ax4 = plt.subplot(2, 2, 4)
    
    # Initialize data
    steps = 200
    time_data = []
    spike_count_data = []
    firing_rate_data = []
    active_neurons_data = []
    
    # Brain simulation thread
    def simulate_brain():
        nonlocal time_data, spike_count_data, firing_rate_data, active_neurons_data
        for i in range(steps):
            brain.step(i)
            time_data.append(i)
            spike_count_data.append(brain.getTotalSpikeCount())
            firing_rate_data.append(brain.getAverageFiringRate())
            active_neurons_data.append(brain.getFiringNeuronCount())
            time.sleep(0.01)
    
    # Start simulation in background thread
    sim_thread = threading.Thread(target=simulate_brain)
    sim_thread.daemon = True
    sim_thread.start()
    
    # Plot 1: Spike count over time
    spike_line, = ax1.plot([], [], 'r-', linewidth=2, label='Total Spikes')
    ax1.set_xlim(0, steps)
    ax1.set_ylim(0, max(spike_count_data) + 50 if spike_count_data else 100)
    ax1.set_xlabel('Time Step')
    ax1.set_ylabel('Spike Count')
    ax1.set_title('Total Spikes Over Time')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Plot 2: Firing rate over time
    firing_line, = ax2.plot([], [], 'b-', linewidth=2, label='Firing Rate')
    ax2.set_xlim(0, steps)
    ax2.set_ylim(0, max(firing_rate_data) + 5 if firing_rate_data else 50)
    ax2.set_xlabel('Time Step')
    ax2.set_ylabel('Firing Rate (Hz)')
    ax2.set_title('Average Firing Rate Over Time')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # Plot 3: Active neurons over time
    active_line, = ax3.plot([], [], 'g-', linewidth=2, label='Active Neurons')
    ax3.set_xlim(0, steps)
    ax3.set_ylim(0, max(active_neurons_data) + 10 if active_neurons_data else 50)
    ax3.set_xlabel('Time Step')
    ax3.set_ylabel('Active Neurons')
    ax3.set_title('Active Neurons Over Time')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # Plot 4: Brain status
    status_text = ax4.text(0.1, 0.5, '', fontsize=12, fontfamily='monospace',
                          verticalalignment='center', wrap=True)
    ax4.set_xlim(0, 1)
    ax4.set_ylim(0, 1)
    ax4.axis('off')
    ax4.set_title('Brain Status')
    
    def update(frame):
        nonlocal spike_line, firing_line, active_line, status_text
        
        # Update plot data
        if time_data:
            spike_line.set_data(time_data, spike_count_data)
            firing_line.set_data(time_data, firing_rate_data)
            active_line.set_data(time_data, active_neurons_data)
        
        # Update status text
        status = f"Brain Status:\n"
        status += f"  Neurons: {brain.getTotalNeuronCount()}\n"
        status += f"  Synapses: {brain.getTotalSynapseCount()}\n"
        status += f"  Total Spikes: {brain.getTotalSpikeCount()}\n"
        status += f"  Active Neurons: {brain.getFiringNeuronCount()}\n"
        status += f"  Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz\n"
        status += f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n"
        status += f"  Development Stage: {brain.getDevelopmentalStage()}\n"
        status += f"\nLast 5 steps firing rates:\n"
        for i in range(max(0, len(firing_rate_data)-5), len(firing_rate_data)):
            status += f"    Step {time_data[i]}: {firing_rate_data[i]:.2f} Hz\n"
        
        status_text.set_text(status)
        
        return spike_line, firing_line, active_line, status_text
    
    # Create animation
    ani = animation.FuncAnimation(plt.gcf(), update, interval=interval, blit=True)
    
    plt.tight_layout()
    plt.show()

def agent_world_visualization(brain: pynlm.Brain, world: pynlm.SimpleWorld, agent: pynlm.AgentBrain, 
                              duration: int = 60, interval: int = 100):
    """Visualize agent-world interaction"""
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    fig.suptitle('NLM Agent-World Visualization', fontsize=16)
    
    # Initialize data
    time_data = []
    reward_data = []
    spike_count_data = []
    firing_rate_data = []
    agent_pos_data = []
    
    # For world visualization
    world_img = axes[0, 0].imshow(np.zeros((world.getHeight(), world.getWidth())), 
                                cmap='gray', origin='upper', vmin=0, vmax=1)
    axes[0, 0].set_title('World Grid')
    axes[0, 0].set_xlabel('X')
    axes[0, 0].set_ylabel('Y')
    
    # For agent trajectory
    ax_traj = axes[0, 1]
    ax_traj.plot([], [], 'b-', alpha=0.5, linewidth=2, label='Agent Path')
    ax_traj.plot([], [], 'ro', markersize=8, label='Agent Position')
    ax_traj.set_xlim(0, world.getWidth())
    ax_traj.set_ylim(0, world.getHeight())
    ax_traj.set_title('Agent Trajectory')
    ax_traj.set_xlabel('X')
    ax_traj.set_ylabel('Y')
    ax_traj.legend()
    ax_traj.grid(True, alpha=0.3)
    
    # For rewards over time
    ax_reward = axes[0, 2]
    ax_reward.plot([], [], 'g-', linewidth=2, label='Cumulative Reward')
    ax_reward.set_xlim(0, duration)
    ax_reward.set_ylim(0, 100)
    ax_reward.set_title('Cumulative Reward')
    ax_reward.set_xlabel('Time Step')
    ax_reward.set_ylabel('Total Reward')
    ax_reward.legend()
    ax_reward.grid(True, alpha=0.3)
    
    # For neural activity
    ax_spikes = axes[1, 0]
    ax_spikes.plot([], [], 'r-', linewidth=2, label='Total Spikes')
    ax_spikes.set_xlim(0, duration)
    ax_spikes.set_ylim(0, 2000)
    ax_spikes.set_title('Total Neural Spikes')
    ax_spikes.set_xlabel('Time Step')
    ax_spikes.set_ylabel('Spike Count')
    ax_spikes.legend()
    ax_spikes.grid(True, alpha=0.3)
    
    # For firing rate
    ax_firing = axes[1, 1]
    ax_firing.plot([], [], 'b-', linewidth=2, label='Firing Rate')
    ax_firing.set_xlim(0, duration)
    ax_firing.set_ylim(0, 100)
    ax_firing.set_title('Average Firing Rate')
    ax_firing.set_xlabel('Time Step')
    ax_firing.set_ylabel('Hz')
    ax_firing.legend()
    ax_firing.grid(True, alpha=0.3)
    
    # For development stage
    ax_dev = axes[1, 2]
    ax_dev.plot([], [], 'm-', linewidth=2, label='Development Stage')
    ax_dev.set_xlim(0, duration)
    ax_dev.set_ylim(0, 6)  # 6 stages
    ax_dev.set_title('Development Stage')
    ax_dev.set_xlabel('Time Step')
    ax_dev.set_ylabel('Stage (0-5)')
    ax_dev.set_yticks([0, 1, 2, 3, 4, 5])
    ax_dev.legend()
    ax_dev.grid(True, alpha=0.3)
    
    # Initialize plot lines
    world_line, = world_img.axes.collections[0].get_paths()[0].vertices[:, 0], world_line.axes.collections[0].get_paths()[0].vertices[:, 1]
    traj_line, = ax_traj.get_lines()[0]
    reward_line, = ax_reward.get_lines()[0]
    spike_line, = ax_spikes.get_lines()[0]
    firing_line, = ax_firing.get_lines()[0]
    dev_line, = ax_dev.get_lines()[0]
    agent_point, = ax_traj.get_lines()[1]
    
    def update(frame):
        # Simulate one step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(frame)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, frame * 0.1)
        agent.updateDevelopment(0.1)
        
        # Update data
        time_data.append(frame)
        reward_data.append(result.reward)
        spike_count_data.append(brain.getTotalSpikeCount())
        firing_rate_data.append(brain.getAverageFiringRate())
        agent_pos_data.append((world.getAgentBody().x, world.getAgentBody().y))
        
        # Update plot data
        if len(time_data) > 1:
            traj_line.set_data([p[0] for p in agent_pos_data], [p[1] for p in agent_pos_data])
            reward_line.set_data(time_data, np.cumsum(reward_data))
            spike_line.set_data(time_data, spike_count_data)
            firing_line.set_data(time_data, firing_rate_data)
            dev_line.set_data(time_data, [brain.getDevelopmentalStage()] * len(time_data))
            
            # Update agent point
            agent_point.set_data([agent_pos_data[-1][0]], [agent_pos_data[-1][1]])
        
        return traj_line, reward_line, spike_line, firing_line, dev_line, agent_point
    
    # Create animation
    ani = animation.FuncAnimation(fig, update, frames=duration, interval=interval, blit=True)
    
    plt.tight_layout()
    plt.show()

def create_brain_comparison():
    """Create comparison of different brain configurations"""
    print("\n=== Brain Comparison Visualization ===")
    
    # Create different brain configurations
    configs = [
        ("Default", pynlm.createDefaultConfig()),
        ("Performance", pynlm.createPerformanceConfig()),
        ("Research", pynlm.createResearchConfig()),
    ]
    
    brains = []
    for name, config in configs:
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append((name, brain))
    
    # Run simulation for each brain
    steps = 50
    for name, brain in brains:
        for i in range(steps):
            brain.step(i)
    
    # Create comparison plot
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Plot firing rates
    colors = ['blue', 'green', 'red']
    for (name, brain), color in zip(brains, colors):
        firing_rates = []
        for i in range(steps):
            brain.step(i)
            firing_rates.append(brain.getAverageFiringRate())
        
        ax.plot(range(steps), firing_rates, color=color, linewidth=2, label=name)
    
    ax.set_xlabel('Time Step')
    ax.set_ylabel('Average Firing Rate (Hz)')
    ax.set_title('Brain Configuration Comparison - Firing Rate Over Time')
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    # Add statistics text
    stats_text = "\n".join([
        f"{name}: {brain.getTotalNeuronCount()} neurons, "
        f"{brain.getAverageFiringRate():.1f} Hz avg, "
        f"{brain.getTotalSpikeCount()} total spikes"
        for name, brain in brains
    ])
    
    plt.figtext(0.02, 0.02, stats_text, fontsize=10, 
                bbox=dict(boxstyle="round", facecolor="wheat", alpha=0.8))
    
    plt.tight_layout()
    plt.show()

def interactive_demo():
    """Run an interactive demo with user input"""
    print("\n=== Interactive NLM Demo ===")
    
    print("\nWelcome to the NLM Interactive Demo!")
    print("Available commands:")
    print("  1 - Simple brain simulation")
    print("  2 - Agent in world simulation")
    print("  3 - Configuration comparison")
    print("  4 - Help")
    print("  5 - Exit")
    
    while True:
        choice = input("\nEnter your choice (1-5): ").strip()
        
        if choice == "1":
            print("\n--- Simple Brain Simulation ---")
            brain = pynlm.createSimpleBrain(200)
            brain.initialize()
            
            for step in range(30):
                brain.step(step)
                if step % 10 == 0:
                    print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes, "
                          f"{brain.getAverageFiringRate():.1f} Hz")
            
            print(f"  Final: {brain.getTotalSpikeCount()} total spikes")
        
        elif choice == "2":
            print("\n--- Agent in World Simulation ---")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(12, 12, 10, 10)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            
            for step in range(20):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                print(f"  Step {step}: Action {action}, Reward {result.reward:.2f}")
            
            print(f"  Final: {brain.getTotalSpikeCount()} spikes, "
                  f"{brain.getAverageFiringRate():.1f} Hz firing rate")
        
        elif choice == "3":
            print("\n--- Configuration Comparison ---")
            create_brain_comparison()
        
        elif choice == "4":
            print("\n--- NLM Interactive Demo Help ---")
            print("This demo showcases NLM (Neural Learning Machine) capabilities.")
            print("\n1. Simple Brain Simulation:")
            print("   - Creates a brain with specified neurons")
            print("   - Runs simulation steps")
            print("   - Shows spike statistics")
            print("\n2. Agent in World Simulation:")
            print("   - Creates agent with world environment")
            print("   - Agent senses, thinks, and acts")
            print("   - Shows reward-based learning")
            print("\n3. Configuration Comparison:")
            print("   - Compares different brain configurations")
            print("   - Shows performance differences")
            print("\n4. Help:")
            print("   - Shows this help text")
            print("\n5. Exit:")
            print("   - Exits the demo")
        
        elif choice == "5":
            print("\nThanks for using NLM Interactive Demo!")
            print("Have a great day exploring neural simulation!")
            break
        
        else:
            print("\nInvalid choice. Please enter 1-5.")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Visualizer - Interactive visualization tool")
    parser.add_argument("--simple-brain", action="store_true", help="Simple neuron plot")
    parser.add_argument("--agent-world", action="store_true", help="Agent-world visualization")
    parser.add_argument("--comparison", action="store_true", help="Brain configuration comparison")
    parser.add_argument("--interactive", action="store_true", help="Run interactive demo")
    parser.add_argument("--demo-only", action="store_true", help="Quick demo without visualizations")
    
    args = parser.parse_args()
    
    if args.demo_only:
        print("=== NLM Quick Demo ===")
        brain = pynlm.createSimpleBrain(100)
        brain.initialize()
        
        for i in range(20):
            brain.step(i)
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"Generated {brain.getTotalSpikeCount()} spikes")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print("Demo complete!")
        return
    
    if args.simple_brain:
        print("Opening simple neuron plot...")
        # Create a brain for visualization
        brain = pynlm.createSimpleBrain(200)
        brain.initialize()
        simple_neuron_plot(brain)
    
    elif args.agent_world:
        print("Opening agent-world visualization...")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(16, 16, 12, 12)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agent_world_visualization(brain, world, agent, duration=60, interval=200)
    
    elif args.comparison:
        create_brain_comparison()
    
    elif args.interactive:
        interactive_demo()
    
    else:
        print("=== NLM Visualizer ===")
        print("\nAvailable visualization modes:")
        print("  --simple-brain      Simple neuron firing plot")
        print("  --agent-world       Agent-world interaction visualization")
        print("  --comparison        Compare brain configurations")
        print("  --interactive       Run interactive demo")
        print("  --demo-only        Quick demo without visualizations")
        
        print("\nTry running: python visualizer.py --interactive")
        print("Or try any visualization mode to see NLM in action!")

if __name__ == "__main__":
    main()
