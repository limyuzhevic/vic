# NLM Advanced Examples

This document provides advanced usage examples for NLM that demonstrate complex scenarios, integration of multiple systems, and production-level usage patterns.

---

## 1. Advanced Reinforcement Learning Simulation

This example demonstrates a complete RL setup with exploration-exploitation balance, reward shaping, and episodic learning.

```python
import pynlm
import numpy as np
import time

def advanced_rl_simulation():
    """
    Advanced reinforcement learning simulation with curiosity-driven exploration,
    working memory, and episodic replay.
    """
    # Create sophisticated brain configuration
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration for RL
    config.set("brain.region_count", 4)
    config.set("brain.neurons_per_region", 200)
    config.set("memory.working_memory_size", 50)
    config.set("memory.episodic_capacity", 1000)
    config.set("neuromodulation.dopamine_gain", 2.0)
    config.set("neuromodulation.curiosity_threshold", 0.3)
    config.set("prediction.prediction_horizon", 20)
    config.set("plasticity.stdp_learning_rate", 0.01)
    config.set("development.maturation_rate", 0.001)
    
    # Initialize brain with advanced features
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world with dynamic obstacles
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.setAgentStart(15, 15)
    
    # Add dynamic elements
    for i in range(5):
        obj = pynlm.WorldObject()
        obj.x = np.random.uniform(5, 25)
        obj.y = np.random.uniform(5, 25)
        obj.type = pynlm.WorldObjectType.Resource
        obj.value = 10.0 + np.random.uniform(-2, 2)
        world.addObject(obj)
    
    # Create agent brain with all systems enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track performance metrics
    performance_metrics = {
        "total_rewards": [],
        "action_counts": {},
        "memory_usage": [],
        "learning_curve": []
    }
    
    # Run episode with advanced tracking
    for step in range(1000):
        # Store previous state for episodic memory
        if step % 10 == 0:
            prev_percept = world.getSensoryPercept()
            brain.getEpisodicMemory().recordExperience(prev_percept, step)
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent brain
        agent.processSensoryInput(percept)
        
        # Brain processes with prediction
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward (dynamic based on context)
        reward = 0.0
        agent_body = world.getAgentBody()
        
        # Get nearby resources
        vision = percept.getVision()
        resource_locations = []
        for y in range(len(vision)):
            for x in range(len(vision[0])):
                if vision[y][x] == 5:  # Resource type
                    resource_locations.append((x, y))
        
        if resource_locations:
            closest = min(resource_locations, 
                         key=lambda pos: (pos[0] - agent_body.x)**2 + 
                                      (pos[1] - agent_body.y)**2)
            distance = ((closest[0] - agent_body.x)**2 + 
                       (closest[1] - agent_body.y)**2)**0.5
            reward += 10.0 / (1.0 + distance)
        
        # Negative reward for collisions/danger
        if agent_body.health < 50:
            reward -= 5.0
        
        # Apply reward with prediction error
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Track metrics
        performance_metrics["total_rewards"].append(reward)
        performance_metrics["action_counts"][str(action)] = \
            performance_metrics["action_counts"].get(str(action), 0) + 1
        performance_metrics["memory_usage"].append(
            brain.getWorkingMemory().getActiveMemoryCount()
        )
        performance_metrics["learning_curve"].append(
            brain.getAverageFiringRate()
        )
        
        # Periodic development and curiosity boosting
        if step % 50 == 0:
            agent.updateDevelopment(step * 0.1)
            if np.random.random() < 0.1:
                # Random curiosity boost for exploration
                agent.enableCuriosity(True)
    
    # Post-episode processing
    brain.getEpisodicMemory().consolidateRecentMemories(50)
    
    # Print performance summary
    print(f"Episode completed in {step} steps")
    print(f"Total reward: {sum(performance_metrics['total_rewards']):.2f}")
    print(f"Average reward per step: {np.mean(performance_metrics['total_rewards']):.2f}")
    print(f"Action distribution: {performance_metrics['action_counts']}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return performance_metrics

if __name__ == "__main__":
    print("Starting Advanced RL Simulation...")
    start_time = time.time()
    metrics = advanced_rl_simulation()
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Metrics collected over {len(metrics['total_rewards'])} steps")
```

---

## 2. Brain Development Tracking and Analysis

This example demonstrates how to track and analyze brain development over time, including structural changes and functional maturation.

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import json
from datetime import datetime

def brain_development_analysis():
    """
    Analyze brain development through multiple stages, tracking changes in
    connectivity, firing patterns, and cognitive capabilities.
    """
    # Create developmental configuration
    config = pynlm.createDefaultConfig()
    
    # Enable developmental tracking
    config.set("development.enable_tracking", True)
    config.set("development.stage_transitions", True)
    config.set("plasticity.development_dependent", True)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Development tracking data
    development_data = {
        "stage_history": [],
        "connectivity_metrics": [],
        "firing_patterns": [],
        "cognitive_performance": [],
        "structural_changes": []
    }
    
    # Run developmental timeline
    developmental_stages = [
        (0, 100, "Initial - Primitive connections"),
        (100, 300, "Critical period - Rapid development"),
        (300, 600, "Maturation - Stabilization"),
        (600, 900, "Adult - Optimized performance"),
        (900, 1000, "Aging - Decline begins")
    ]
    
    for start_step, end_step, stage_description in developmental_stages:
        # Set developmental stage
        stage = pynlm.DevelopmentalStage.Initial
        if "Critical" in stage_description:
            stage = pynlm.DevelopmentalStage.CriticalPeriod
        elif "Maturation" in stage_description:
            stage = pynlm.DevelopmentalStage.Maturation
        elif "Adult" in stage_description:
            stage = pynlm.DevelopmentalStage.Adult
        elif "Aging" in stage_description:
            stage = pynlm.DevelopmentalStage.Aging
        
        brain.setDevelopmentalStage(stage)
        
        # Record metrics for this stage
        for step in range(start_step, end_step):
            # Collect developmental metrics
            stage_info = {
                "step": step,
                "developmental_stage": str(stage),
                "stage_description": stage_description,
                "timestamp": datetime.now().isoformat()
            }
            
            # Neural statistics
            stage_info["total_neurons"] = brain.getTotalNeuronCount()
            stage_info["active_neurons"] = brain.getActiveNeuronCount()
            stage_info["firing_neurons"] = brain.getFiringNeuronCount()
            stage_info["total_spikes"] = brain.getTotalSpikeCount()
            stage_info["avg_firing_rate"] = brain.getAverageFiringRate()
            stage_info["excitatory_inhibition_ratio"] = \
                brain.getExcitationInhibitionRatio()
            
            # Region statistics
            regions = brain.getRegions()
            region_stats = []
            for region in regions:
                region_stats.append({
                    "name": region.getName(),
                    "neuron_count": region.getTotalNeuronCount(),
                    "firing_rate": region.getAverageFiringRate(),
                    "synaptic_density": region.getSynapticDensity()
                })
            stage_info["regions"] = region_stats
            
            # Memory system status
            if brain.getWorkingMemory():
                stage_info["working_memory_usage"] = \
                    brain.getWorkingMemory().getActiveMemoryCount()
            if brain.getEpisodicMemory():
                stage_info["episodic_memory_size"] = \
                    brain.getEpisodicMemory().getMemoryCount()
            if brain.getAssociativeMemory():
                stage_info["associative_memory_strength"] = \
                    brain.getAssociativeMemory().getAverageAssociationStrength()
            
            # Neuromodulation levels
            if brain.getDopamine():
                stage_info["dopamine_level"] = brain.getDopamine().getCurrentLevel()
            if brain.getCuriosity():
                stage_info["curiosity_level"] = brain.getCuriosity().getCurrentLevel()
            if brain.getNovelty():
                stage_info["novelty_level"] = brain.getNovelty().getCurrentLevel()
            
            # Prediction system
            if brain.getPredictionSystem():
                stage_info["prediction_error"] = \
                    brain.getPredictionSystem().getPredictionError()
            
            development_data["stage_history"].append(stage_info)
        
        # Simulate developmental transitions
        if "Critical" in stage_description:
            # Critical period - high plasticity
            for region in brain.getRegions():
                for population in region.getAllPopulations():
                    population.increasePlasticity(10.0)
        elif "Maturation" in stage_description:
            # Maturation - structural pruning
            for region in brain.getRegions():
                region.pruneLowStrengthConnections(0.1)
    
    # Analyze development patterns
    print("Development Analysis Complete!")
    print(f"Total developmental stages tracked: {len(set(d['developmental_stage'] for d in development_data['stage_history']))}")
    
    # Save development data
    with open("development_tracking.json", "w") as f:
        json.dump(development_data, f, indent=2)
    
    # Generate development plots
    plot_development_analysis(development_data)
    
    return development_data

def plot_development_analysis(data):
    """
    Generate plots from development tracking data.
    """
    import matplotlib.pyplot as plt
    import seaborn as sns
    
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    
    # Plot 1: Neural development over time
    steps = [d['step'] for d in data['stage_history']]
    neurons = [d['total_neurons'] for d in data['stage_history']]
    active = [d['active_neurons'] for d in data['stage_history']]
    
    axes[0, 0].plot(steps, neurons, label='Total Neurons', linewidth=2)
    axes[0, 0].plot(steps, active, label='Active Neurons', linewidth=2, alpha=0.7)
    axes[0, 0].set_title('Neural Population Development')
    axes[0, 0].set_xlabel('Simulation Step')
    axes[0, 0].set_ylabel('Neuron Count')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Firing rate evolution
    firing_rates = [d['avg_firing_rate'] for d in data['stage_history']]
    axes[0, 1].plot(steps, firing_rates, linewidth=2, color='green')
    axes[0, 1].set_title('Average Firing Rate Evolution')
    axes[0, 1].set_xlabel('Simulation Step')
    axes[0, 1].set_ylabel('Firing Rate (Hz)')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Excitatory/Inhibition balance
    ei_ratios = [d['excitatory_inhibition_ratio'] for d in data['stage_history']]
    axes[0, 2].plot(steps, ei_ratios, linewidth=2, color='red')
    axes[0, 2].axhline(y=1.0, color='black', linestyle='--', alpha=0.5)
    axes[0, 2].set_title('E/I Balance Ratio')
    axes[0, 2].set_xlabel('Simulation Step')
    axes[0, 2].set_ylabel('E/I Ratio')
    axes[0, 2].grid(True, alpha=0.3)
    
    # Plot 4: Neuromodulation levels
    dopamine = [d.get('dopamine_level', 0) for d in data['stage_history']]
    curiosity = [d.get('curiosity_level', 0) for d in data['stage_history']]
    novelty = [d.get('novelty_level', 0) for d in data['stage_history']]
    
    axes[1, 0].plot(steps, dopamine, label='Dopamine', linewidth=2)
    axes[1, 0].plot(steps, curiosity, label='Curiosity', linewidth=2)
    axes[1, 0].plot(steps, novelty, label='Novelty', linewidth=2)
    axes[1, 0].set_title('Neuromodulation Levels')
    axes[1, 0].set_xlabel('Simulation Step')
    axes[1, 0].set_ylabel('Level')
    axes[1, 0].legend()
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 5: Memory usage
    working_mem = [d.get('working_memory_usage', 0) for d in data['stage_history']]
    episodic_mem = [d.get('episodic_memory_size', 0) for d in data['stage_history']]
    
    axes[1, 1].plot(steps, working_mem, label='Working Memory', linewidth=2)
    axes[1, 1].plot(steps, episodic_mem, label='Episodic Memory', linewidth=2)
    axes[1, 1].set_title('Memory System Usage')
    axes[1, 1].set_xlabel('Simulation Step')
    axes[1, 1].set_ylabel('Memory Items')
    axes[1, 1].legend()
    axes[1, 1].grid(True, alpha=0.3)
    
    # Plot 6: Prediction error
    prediction_errors = [d.get('prediction_error', 0) for d in data['stage_history']]
    axes[1, 2].plot(steps, prediction_errors, linewidth=2, color='purple')
    axes[1, 2].set_title('Prediction Error Dynamics')
    axes[1, 2].set_xlabel('Simulation Step')
    axes[1, 2].set_ylabel('Prediction Error')
    axes[1, 2].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('development_analysis.png', dpi=150, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    print("Starting Brain Development Analysis...")
    data = brain_development_analysis()
    print(f"Development analysis completed with {len(data['stage_history'])} data points")
```

---

## 3. Memory Replay and Consolidation Experiments

This example demonstrates memory replay mechanisms, episodic consolidation, and the interaction between working and long-term memory.

```python
import pynlm
import numpy as np
import random
from collections import defaultdict

def memory_replay_experiment():
    """
    Memory replay and consolidation experiment with sleep-like replay phases,
    memory strengthening, and interference management.
    """
    # Configure for memory experiments
    config = pynlm.createDefaultConfig()
    config.set("memory.replay_enabled", True)
    config.set("memory.consolidation_enabled", True)
    config.set("memory.interference_management", True)
    config.set("memory.replay_frequency", 0.1)  # Replay every 10 steps
    config.set("memory.sleep_phases", True)
    
    # Initialize brain with rich memory systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Experimental phases
    phases = {
        "encoding": {"duration": 500, "replay_interval": 50},
        "replay": {"duration": 200, "replay_intensity": 0.8},
        "consolidation": {"duration": 300, "replay_rate": 0.3},
        "testing": {"duration": 200, "access_patterns": "random"}
    }
    
    # Memory tracking
    memory_stats = {
        "encoding_sessions": [],
        "replay_sessions": [],
        "consolidation_sessions": [],
        "testing_sessions": [],
        "memory_strength": [],
        "retrieval_success": [],
        "interference_effects": []
    }
    
    # Create simple world for experience generation
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.setAgentStart(10, 10)
    
    # Agent for interaction
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    current_phase = "encoding"
    phase_step = 0
    encoding_start_time = 0
    
    # Main experimental loop
    for step in range(1500):  # Total experiment duration
        # Phase management
        if step >= phases[current_phase]["duration"] + encoding_start_time:
            # Move to next phase
            current_phase = get_next_phase(current_phase)
            phase_step = 0
            encoding_start_time = step - phases["encoding"]["duration"]
            
            if current_phase == "replay":
                brain.getEpisodicMemory().initiateReplay()
            elif current_phase == "consolidation":
                brain.getEpisodicMemory().startConsolidation()
        
        # Phase-specific behavior
        if current_phase == "encoding":
            # Normal learning and experience collection
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Store experience for potential replay
            if step % phases[current_phase]["replay_interval"] == 0:
                brain.getEpisodicMemory().recordExperience(percept, step)
                
        elif current_phase == "replay":
            # Memory replay phase
            world.update(0.1)
            
            # Trigger spontaneous replay events
            if random.random() < phases[current_phase]["replay_intensity"]:
                brain.getEpisodicMemory().triggerReplay()
            
            # Background world interaction
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        elif current_phase == "consolidation":
            # Consolidation phase with systematic replay
            world.update(0.1)
            
            # Systematic memory consolidation
            if brain.getEpisodicMemory().shouldConsolidate():
                brain.getEpisodicMemory().consolidateRecentMemories(20)
            
            # Record new experiences that can be consolidated
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        elif current_phase == "testing":
            # Testing phase - memory retrieval without new encoding
            world.update(0.1)
            
            # Track memory access patterns
            if phases[current_phase]["access_patterns"] == "random":
                # Random access for baseline
                if random.random() < 0.2:
                    # Attempt memory retrieval
                    test_percept = world.getSensoryPercept()
                    retrieval_success = brain.getEpisodicMemory().attemptRetrieval(test_percept)
                    memory_stats["retrieval_success"].append(retrieval_success)
            
            # Background processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect memory statistics
        if step % 50 == 0:
            # Memory strength tracking
            if brain.getEpisodicMemory():
                strength = brain.getEpisodicMemory().getAverageMemoryStrength()
                memory_stats["memory_strength"].append(strength)
            
            # Working memory usage
            if brain.getWorkingMemory():
                active_count = brain.getWorkingMemory().getActiveMemoryCount()
            
            # Interference tracking
            if brain.getAssociativeMemory():
                interference = brain.getAssociativeMemory().getInterferenceLevel()
                memory_stats["interference_effects"].append(interference)
    
    # Analysis and reporting
    print("Memory Replay Experiment Complete!")
    print(f"Encoding sessions recorded: {len(memory_stats['encoding_sessions'])}")
    print(f"Replay events triggered: {len(memory_stats['replay_sessions'])}")
    print(f"Consolidation cycles completed: {len(memory_stats['consolidation_sessions'])}")
    print(f"Testing retrievals attempted: {len(memory_stats['testing_sessions'])}")
    print(f"Final memory strength: {memory_stats['memory_strength'][-1] if memory_stats['memory_strength'] else 0:.3f}")
    print(f"Average retrieval success rate: {np.mean(memory_stats['retrieval_success']) * 100:.1f}%")
    print(f"Final interference level: {memory_stats['interference_effects'][-1] if memory_stats['interference_effects'] else 0:.3f}")
    
    return memory_stats

def get_next_phase(current_phase):
    """Get the next experimental phase."""
    phases = ["encoding", "replay", "consolidation", "testing"]
    current_index = phases.index(current_phase)
    return phases[(current_index + 1) % len(phases)]

if __name__ == "__main__":
    print("Starting Memory Replay and Consolidation Experiment...")
    stats = memory_replay_experiment()
```

---

## 4. Visualization Dashboard and Interactive Analysis

This example demonstrates the new visualization system for real-time monitoring and analysis of brain activity.

```python
import pynlm
import time
import threading
import numpy as np
from datetime import datetime
from typing import Dict, List, Optional

class VisualizationDashboard:
    """
    Interactive visualization dashboard for NLM brain monitoring and analysis.
    Displays real-time neural activity, memory states, and system metrics.
    """
    
    def __init__(self, brain: pynlm.Brain, update_interval: float = 0.5):
        """
        Initialize visualization dashboard.
        
        Args:
            brain: The brain instance to visualize
            update_interval: Dashboard update interval in seconds
        """
        self.brain = brain
        self.update_interval = update_interval
        self.running = False
        self.visualizer = pynlm.createVisualizer(brain)
        
        # Performance metrics
        self.metrics_history = {
            "step": [],
            "time": [],
            "firing_rate": [],
            "memory_usage": [],
            "prediction_error": [],
            "neuromodulation": {"dopamine": [], "curiosity": [], "novelty": []},
            "region_activity": defaultdict(lambda: []),
            "synaptic_strength": []
        }
        
        # Visualization components
        self.network_viz = None
        self.performance_viz = None
        self.memory_viz = None
        self.dashboard_viz = None
    
    def start_monitoring(self, duration: Optional[float] = None):
        """
        Start real-time monitoring and visualization.
        
        Args:
            duration: Monitoring duration in seconds (None for continuous)
        """
        self.running = True
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
        self.monitoring_thread.start()
        
        print(f"Visualization dashboard started monitoring...")
        print(f"Update interval: {self.update_interval} seconds")
        
        # Wait for completion or keyboard interrupt
        try:
            if duration:
                time.sleep(duration)
                self.stop_monitoring()
            else:
                # Wait for user interrupt
                while self.running:
                    time.sleep(1)
        except KeyboardInterrupt:
            self.stop_monitoring()
    
    def _monitoring_loop(self):
        """Main monitoring loop for data collection and visualization updates."""
        start_time = time.time()
        step_count = 0
        
        while self.running:
            current_time = time.time()
            step_count += 1
            
            # Collect metrics
            self._collect_metrics(step_count, current_time)
            
            # Update visualizations
            self._update_visualizations()
            
            # Save metrics periodically
            if step_count % 100 == 0:
                self._save_metrics()
            
            # Sleep for update interval
            time.sleep(self.update_interval)
        
        end_time = time.time()
        print(f"Monitoring completed. Total time: {end_time - start_time:.2f} seconds")
    
    def _collect_metrics(self, step: int, timestamp: float):
        """Collect current metrics from brain systems."""
        self.metrics_history["step"].append(step)
        self.metrics_history["time"].append(timestamp)
        
        # Neural metrics
        self.metrics_history["firing_rate"].append(self.brain.getAverageFiringRate())
        self.metrics_history["memory_usage"].append(
            self.brain.getWorkingMemory().getActiveMemoryCount() if self.brain.getWorkingMemory() else 0
        )
        
        # Prediction metrics
        if self.brain.getPredictionSystem():
            self.metrics_history["prediction_error"].append(self.brain.getPredictionSystem().getPredictionError())
        
        # Neuromodulation metrics
        if self.brain.getDopamine():
            self.metrics_history["neuromodulation"]["dopamine"].append(self.brain.getDopamine().getCurrentLevel())
        if self.brain.getCuriosity():
            self.metrics_history["neuromodulation"]["curiosity"].append(self.brain.getCuriosity().getCurrentLevel())
        if self.brain.getNovelty():
            self.metrics_history["neuromodulation"]["novelty"].append(self.brain.getNovelty().getCurrentLevel())
        
        # Regional activity
        for region in self.brain.getRegions():
            activity = region.getAverageFiringRate()
            self.metrics_history["region_activity"][region.getName()].append(activity)
        
        # Synaptic metrics
        total_synapses = self.brain.getTotalSynapseCount()
        if total_synapses > 0:
            avg_strength = self.brain.getAverageSynapticWeight()
            self.metrics_history["synaptic_strength"].append(avg_strength)
    
    def _update_visualizations(self):
        """Update all visualization components."""
        if not self.visualizer:
            return
        
        # Network visualization
        self.network_viz = self.visualizer.createNetworkView()
        self.network_viz.updateLayout()
        
        # Performance dashboard
        self.performance_viz = self.visualizer.createPerformanceDashboard()
        self.performance_viz.updateMetrics(self.metrics_history)
        
        # Memory visualization
        self.memory_viz = self.visualizer.createMemoryViewer()
        self.memory_viz.updateMemoryState()
        
        # Interactive dashboard
        self.dashboard_viz = self.visualizer.createInteractiveDashboard()
        self.dashboard_viz.refresh()
    
    def _save_metrics(self):
        """Save collected metrics to file."""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"dashboard_metrics_{timestamp}.json"
        
        with open(filename, 'w') as f:
            import json
            json.dump(self.metrics_history, f, indent=2)
        
        print(f"Metrics saved to {filename}")
    
    def generate_report(self, output_file: str):
        """Generate comprehensive analysis report."""
        report = {
            "metadata": {
                "generated_at": datetime.now().isoformat(),
                "total_samples": len(self.metrics_history["step"]),
                "brain_configuration": "advanced_research_config"
            },
            "neural_dynamics": {
                "avg_firing_rate": np.mean(self.metrics_history["firing_rate"]),
                "max_firing_rate": np.max(self.metrics_history["firing_rate"]),
                "min_firing_rate": np.min(self.metrics_history["firing_rate"]),
                "firing_rate_variance": np.var(self.metrics_history["firing_rate"])
            },
            "memory_performance": {
                "avg_memory_usage": np.mean(self.metrics_history["memory_usage"]),
                "max_memory_usage": np.max(self.metrics_history["memory_usage"]),
                "memory_efficiency": self._calculate_memory_efficiency()
            },
            "prediction_accuracy": {
                "avg_prediction_error": np.mean(self.metrics_history["prediction_error"]) if self.metrics_history["prediction_error"] else 0,
                "prediction_stability": self._calculate_prediction_stability()
            },
            "neuromodulation_dynamics": {
                "dopamine_dynamics": self._analyze_time_series(self.metrics_history["neuromodulation"]["dopamine"]),
                "curiosity_dynamics": self._analyze_time_series(self.metrics_history["neuromodulation"]["curiosity"]),
                "novelty_dynamics": self._analyze_time_series(self.metrics_history["neuromodulation"]["novelty"])
            },
            "regional_dynamics": self._analyze_regional_dynamics(),
            "synaptic_dynamics": self._analyze_synaptic_dynamics()
        }
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Report generated: {output_file}")
        return report
    
    def _calculate_memory_efficiency(self):
        """Calculate memory efficiency metric."""
        if not self.metrics_history["memory_usage"]:
            return 0
        # Simple efficiency metric: average usage vs maximum capacity
        avg_usage = np.mean(self.metrics_history["memory_usage"])
        max_possible = 100  # Assuming max capacity of 100
        return avg_usage / max_possible
    
    def _calculate_prediction_stability(self):
        """Calculate prediction stability metric."""
        if not self.metrics_history["prediction_error"]:
            return 0
        return 1.0 / (1.0 + np.std(self.metrics_history["prediction_error"]))
    
    def _analyze_time_series(self, series):
        """Analyze time series data."""
        if not series:
            return {"mean": 0, "std": 0, "trend": "stable"}
        
        return {
            "mean": float(np.mean(series)),
            "std": float(np.std(series)),
            "trend": "increasing" if np.mean(series[-10:]) > np.mean(series[:10]) else "decreasing" if np.mean(series[-10:]) < np.mean(series[:10]) else "stable"
        }
    
    def _analyze_regional_dynamics(self):
        """Analyze regional activity dynamics."""
        dynamics = {}
        for region_name, activity in self.metrics_history["region_activity"].items():
            if activity:
                dynamics[region_name] = {
                    "mean_activity": float(np.mean(activity)),
                    "max_activity": float(np.max(activity)),
                    "min_activity": float(np.min(activity)),
                    "activity_variance": float(np.var(activity)),
                    "stability": 1.0 / (1.0 + np.std(activity)) if activity else 0
                }
        return dynamics
    
    def _analyze_synaptic_dynamics(self):
        """Analyze synaptic strength dynamics."""
        if not self.metrics_history["synaptic_strength"]:
            return {"avg_strength": 0, "stability": 0, "plasticity": 0}
        
        strengths = self.metrics_history["synaptic_strength"]
        return {
            "avg_strength": float(np.mean(strengths)),
            "stability": 1.0 / (1.0 + np.std(strengths)),
            "plasticity": float(np.std(strengths))  # Higher std indicates more plasticity
        }
    
    def stop_monitoring(self):
        """Stop the monitoring loop."""
        self.running = False
        if hasattr(self, 'monitoring_thread'):
            self.monitoring_thread.join(timeout=5.0)
        print("Monitoring stopped.")

class InteractiveBrainExperiment:
    """
    Interactive brain experiment with real-time visualization and control.
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.dashboard = VisualizationDashboard(brain)
        self.experiment_running = False
    
    def run_experimental_session(self, duration: float):
        """
        Run experimental session with real-time visualization.
        
        Args:
            duration: Session duration in seconds
        """
        print("Starting Interactive Brain Experiment...")
        print("Press Ctrl+C to stop early")
        
        try:
            self.dashboard.start_monitoring(duration)
            
            # Generate final report
            report_file = f"experiment_report_{int(time.time())}.json"
            report = self.dashboard.generate_report(report_file)
            
            print(f"\nExperiment completed successfully!")
            print(f"Report saved to: {report_file}")
            print(f"Key findings:")
            print(f"  - Average firing rate: {report['neural_dynamics']['avg_firing_rate']:.2f} Hz")
            print(f"  - Memory efficiency: {report['memory_performance']['memory_efficiency']:.2%}")
            print(f"  - Prediction stability: {report['prediction_accuracy']['prediction_stability']:.2%}")
            
            return report
            
        except KeyboardInterrupt:
            print("\nExperiment interrupted by user")
            self.dashboard.stop_monitoring()
        except Exception as e:
            print(f"Error during experiment: {e}")
            self.dashboard.stop_monitoring()

if __name__ == "__main__":
    # Create sophisticated brain for visualization
    config = pynlm.createDefaultConfig()
    
    # Enable all systems for comprehensive visualization
    config.set("visualization.enabled", True)
    config.set("visualization.real_time", True)
    config.set("brain.region_count", 6)
    config.set("brain.neurons_per_region", 150)
    config.set("memory.working_memory_size", 75)
    config.set("memory.episodic_capacity", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable agent systems for realistic interaction
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.setAgentStart(12, 12)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Start interactive visualization experiment
    experiment = InteractiveBrainExperiment(brain)
    experiment.run_experimental_session(duration=60)  # 60 second experiment
```

---

## 5. Neuromodulation Experiments

This example demonstrates the complex interplay of neuromodulatory systems in guiding learning, exploration, and decision-making.

```python
import pynlm
import numpy as np
import time
from enum import Enum
from typing import Dict, List, Tuple

class NeuromodulationExperiment:
    """
    Comprehensive neuromodulation experiments exploring the interaction between
    dopamine, curiosity, novelty, and prediction error systems.
    """
    
    def __init__(self, brain: pynlm.Brain):
        """
        Initialize neuromodulation experiment.
        
        Args:
            brain: The brain instance to experiment with
        """
        self.brain = brain
        self.config = brain.getConfig()
        
        # Experimental parameters
        self.experiment_phase = "baseline"
        self.current_step = 0
        self.max_steps = 2000
        
        # Neuromodulation tracking
        self.neuromodulation_data = {
            "dopamine": [],
            "curiosity": [],
            "novelty": [],
            "prediction_error": [],
            "reward_history": [],
            "behavior_history": []
        }
        
        # Experimental conditions
        self.conditions = {
            "baseline": {"dopamine_gain": 1.0, "curiosity_threshold": 0.5, "novelty_sensitivity": 1.0},
            "high_dopamine": {"dopamine_gain": 2.5, "curiosity_threshold": 0.3, "novelty_sensitivity": 1.5},
            "high_curiosity": {"dopamine_gain": 1.0, "curiosity_threshold": 0.1, "novelty_sensitivity": 0.8},
            "high_novelty": {"dopamine_gain": 1.5, "curiosity_threshold": 0.5, "novelty_sensitivity": 2.0}
        }
        
        # Behavioral patterns
        self.behavioral_patterns = {
            "exploitation": 0,
            "exploration": 0,
            "avoidance": 0,
            "curiosity_driven": 0
        }
    
    def run_complete_experiment(self):
        """Run complete neuromodulation experiment with multiple phases."""
        print("Starting Neuromodulation Experiment...")
        print(f"Total duration: {self.max_steps} simulation steps")
        
        # Initialize world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        world.setAgentStart(10, 10)
        
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Experimental phases
        phases = [
            ("baseline", 500, self.conditions["baseline"]),
            ("high_dopamine", 500, self.conditions["high_dopamine"]),
            ("high_curiosity", 500, self.conditions["high_curiosity"]),
            ("high_novelty", 500, self.conditions["high_novelty"])
        ]
        
        for phase_name, phase_duration, condition in phases:
            print(f"\n--- Phase: {phase_name.upper()} ---")
            print(f"Duration: {phase_duration} steps")
            print(f"Parameters: Dopamine gain={condition['dopamine_gain']}, "
                  f"Curiosity threshold={condition['curiosity_threshold']}, "
                  f"Novelty sensitivity={condition['novelty_sensitivity']}")
            
            # Set neuromodulation parameters
            self._set_neuromodulation_parameters(condition)
            
            # Run phase
            phase_data = self._run_phase(agent, world, phase_duration, phase_name)
            
            # Store results
            for key in self.neuromodulation_data:
                if key in phase_data:
                    self.neuromodulation_data[key].extend(phase_data[key])
        
        # Analyze and report results
        self._analyze_results()
        
        return self.neuromodulation_data
    
    def _set_neuromodulation_parameters(self, condition: Dict):
        """Set neuromodulation parameters based on experimental condition."""
        # In a real implementation, these would be passed to the neuromodulation systems
        # For this example, we track them and analyze their effects
        self.current_condition = condition
    
    def _run_phase(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                   duration: int, phase_name: str) -> Dict:
        """
        Run a single experimental phase.
        
        Args:
            agent: The agent brain interface
            world: The world environment
            duration: Phase duration in steps
            phase_name: Name of the experimental phase
        
        Returns:
            Dictionary with phase results
        """
        phase_data = {
            "dopamine": [],
            "curiosity": [],
            "novelty": [],
            "prediction_error": [],
            "reward_history": [],
            "behavior_history": []
        }
        
        for step in range(duration):
            # Update step counter
            self.current_step = step
            
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Track neuromodulation levels
            if self.brain.getDopamine():
                phase_data["dopamine"].append(self.brain.getDopamine().getCurrentLevel())
            if self.brain.getCuriosity():
                phase_data["curiosity"].append(self.brain.getCuriosity().getCurrentLevel())
            if self.brain.getNovelty():
                phase_data["novelty"].append(self.brain.getNovelty().getCurrentLevel())
            if self.brain.getPredictionErrorSignal():
                phase_data["prediction_error"].append(self.brain.getPredictionErrorSignal().getCurrentError())
            
            # Decode action and apply to world
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate context-dependent reward
            reward = self._calculate_contextual_reward(world, agent, percept)
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, agent.getPredictionError())
            
            # Track behavior
            behavior = self._classify_behavior(action, world, agent)
            phase_data["behavior_history"].append(behavior)
            phase_data["reward_history"].append(reward)
            
            # Record agent state
            if step % 100 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, "
                      f"Dopamine={phase_data['dopamine'][-1]:.3f}, "
                      f"Curiosity={phase_data['curiosity'][-1]:.3f}, "
                      f"Novelty={phase_data['novelty'][-1]:.3f}")
        
        return phase_data
    
    def _calculate_contextual_reward(self, world: pynlm.SimpleWorld, 
                                    agent: pynlm.AgentBrain, 
                                    percept: pynlm.SensoryPercept) -> float:
        """
        Calculate context-dependent reward based on multiple factors.
        
        Args:
            world: Current world state
            agent: Agent brain interface
            percept: Current sensory percept
        
        Returns:
            Calculated reward value
        """
        agent_body = world.getAgentBody()
        reward = 0.0
        
        # Resource-based reward
        vision = percept.getVision()
        resource_locations = []
        for y in range(len(vision)):
            for x in range(len(vision[0])):
                if vision[y][x] == 5:  # Resource type
                    resource_locations.append((x, y))
        
        if resource_locations:
            closest = min(resource_locations,
                         key=lambda pos: (pos[0] - agent_body.x)**2 + (pos[1] - agent_body.y)**2)
            distance = ((closest[0] - agent_body.x)**2 + (closest[1] - agent_body.y)**2)**0.5
            resource_reward = 10.0 / (1.0 + distance)
            reward += resource_reward
        
        # Novelty-based reward (exploration bonus)
        if self.brain.getNovelty():
            novelty = self.brain.getNovelty().getCurrentLevel()
            if novelty > 0.7:  # High novelty bonus
                reward += novelty * 5.0
        
        # Development-based reward
        if self.brain.getDevelopmentSystem():
            developmental_progress = self.brain.getDevelopmentalStage()
            reward += float(developmental_progress) * 0.5
        
        # Curiosity-based exploration bonus
        if self.brain.getCuriosity():
            curiosity = self.brain.getCuriosity().getCurrentLevel()
            if curiosity > 0.6:
                reward += curiosity * 3.0
        
        # Energy-based cost
        energy_cost = agent_body.energy * 0.1
        reward -= energy_cost
        
        # Danger avoidance penalty
        if agent_body.health < 50:
            reward -= 2.0
        
        # Prediction error bonus
        if self.brain.getPredictionErrorSignal():
            prediction_error = abs(self.brain.getPredictionErrorSignal().getCurrentError())
            if prediction_error > 0.5:
                reward += prediction_error * 2.0
        
        return reward
    
    def _classify_behavior(self, action: pynlm.MotorCommand, 
                          world: pynlm.SimpleWorld, 
                          agent: pynlm.AgentBrain) -> str:
        """
        Classify behavioral pattern based on action and context.
        
        Args:
            action: Motor command action
            world: Current world state
            agent: Agent brain interface
        
        Returns:
            Behavior classification
        """
        agent_body = world.getAgentBody()
        
        # Simple behavior classification
        if action == pynlm.MotorCommand.MoveForward or action == pynlm.MotorCommand.MoveRight:
            return "exploitation"
        elif action == pynlm.MotorCommand.TurnLeft or action == pynlm.MotorCommand.TurnRight:
            return "exploration"
        elif action == pynlm.MotorCommand.Look or action == pynlm.MotorCommand.LookUp:
            return "curiosity_driven"
        elif action == pynlm.MotorCommand.Interact:
            return "exploitation"
        else:
            return "avoidance"
    
    def _analyze_results(self):
        """Analyze and report neuromodulation experiment results."""
        print("\n=== NEUROMODULATION EXPERIMENT RESULTS ===\n")
        
        for phase_name in self.conditions.keys():
            phase_start = sum(p["duration"] for p in [
                ("baseline", 500), ("high_dopamine", 500), 
                ("high_curiosity", 500), ("high_novelty", 500)
            ][:list(self.conditions.keys()).index(phase_name)])
            phase_end = phase_start + 500
            
            # Extract phase-specific data
            phase_indices = slice(phase_start, phase_end)
            
            print(f"Phase: {phase_name.upper()}")
            print(f"  Dopamine Levels: {np.mean(self.neuromodulation_data['dopamine'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['dopamine'][phase_indices]):.3f}")
            print(f"  Curiosity Levels: {np.mean(self.neuromodulation_data['curiosity'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['curiosity'][phase_indices]):.3f}")
            print(f"  Novelty Levels: {np.mean(self.neuromodulation_data['novelty'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['novelty'][phase_indices]):.3f}")
            print(f"  Prediction Error: {np.mean(self.neuromodulation_data['prediction_error'][phase_indices]):.3f} ± {np.std(self.neuromodulation_data['prediction_error'][phase_indices]):.3f}")
            
            # Behavior distribution
            behaviors = self.neuromodulation_data['behavior_history'][phase_indices]
            behavior_counts = {}
            for behavior in behaviors:
                behavior_counts[behavior] = behavior_counts.get(behavior, 0) + 1
            
            print(f"  Behavior Distribution:")
            for behavior, count in behavior_counts.items():
                percentage = (count / len(behaviors)) * 100
                print(f"    {behavior}: {count} ({percentage:.1f}%)")
            
            print()
        
        print("=== KEY FINDINGS ===")
        
        # Analyze neuromodulation effectiveness
        dopamine_effectiveness = self._analyze_neuromodulator_effectiveness('dopamine')
        curiosity_effectiveness = self._analyze_neuromodulator_effectiveness('curiosity')
        novelty_effectiveness = self._analyze_neuromodulator_effectiveness('novelty')
        
        print(f"Dopamine effectiveness: {dopamine_effectiveness:.2%}")
        print(f"Curiosity effectiveness: {curiosity_effectiveness:.2%}")
        print(f"Novelty effectiveness: {novelty_effectiveness:.2%}")
        
        # Determine optimal configuration
        optimal_config = self._determine_optimal_config()
        print(f"\nOptimal neuromodulation configuration:")
        for key, value in optimal_config.items():
            print(f"  {key}: {value}")
    
    def _analyze_neuromodulator_effectiveness(self, neuromodulator: str) -> float:
        """
        Calculate effectiveness metric for a neuromodulator.
        
        Args:
            neuromodulator: Name of neuromodulator ('dopamine', 'curiosity', or 'novelty')
        
        Returns:
            Effectiveness metric (0-1)
        """
        if not self.neuromodulation_data[neuromodulator]:
            return 0.0
        
        # Effectiveness defined as the ability to modulate behavior meaningfully
        # Calculated as the ratio of behavioral change to neuromodulator level
        data = self.neuromodulation_data[neuromodulator]
        mean_level = np.mean(data)
        level_variability = np.std(data)
        
        # Higher variability indicates better modulation capability
        effectiveness = min(level_variability / mean_level, 1.0) if mean_level > 0 else 0.0
        return effectiveness
    
    def _determine_optimal_config(self) -> Dict:
        """
        Determine optimal neuromodulation configuration based on experiment results.
        
        Returns:
            Dictionary with optimal configuration parameters
        """
        # Simple heuristic: find configuration with best balance of all neuromodulators
        optimal_config = self.conditions["baseline"].copy()  # Start with baseline
        
        # Evaluate each configuration
        for config_name, config_params in self.conditions.items():
            # Calculate balance score (higher is better)
            dopamine_balance = 1.0 / (1.0 + abs(config_params['dopamine_gain'] - 1.0))
            curiosity_balance = 1.0 / (1.0 + abs(config_params['curiosity_threshold'] - 0.5))
            novelty_balance = 1.0 / (1.0 + abs(config_params['novelty_sensitivity'] - 1.0))
            
            balance_score = (dopamine_balance + curiosity_balance + novelty_balance) / 3.0
            
            # Update optimal if this is better
            if config_name == "high_dopamine" or balance_score > 0.7:
                optimal_config = config_params.copy()
        
        return optimal_config

if __name__ == "__main__":
    # Create experiment configuration
    config = pynlm.createDefaultConfig()
    
    # Enable comprehensive neuromodulation systems
    config.set("neuromodulation.enable_dopamine", True)
    config.set("neuromodulation.enable_curiosity", True)
    config.set("neuromodulation.enable_novelty", True)
    config.set("neuromodulation.enable_prediction_error", True)
    config.set("brain.region_count", 5)
    config.set("brain.neurons_per_region", 100)
    config.set("memory.working_memory_size", 50)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run neuromodulation experiment
    experiment = NeuromodulationExperiment(brain)
    results = experiment.run_complete_experiment()