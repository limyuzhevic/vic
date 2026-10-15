# Advanced NLM Python Examples

This document contains comprehensive examples for different use cases and scenarios with the NLM Python API. These examples demonstrate advanced features, performance optimizations, and real-world applications.

## Table of Contents

1. [Real-Time Agent Control](#real-time-agent-control)
2. [Large-Scale Network Simulation](#large-scale-network-simulation)
3. [Multi-Agent Systems](#multi-agent-systems)
4. [Learning and Adaptation](#learning-and-adaptation)
5. [Performance Benchmarking](#performance-benchmarking)
6. [Experimental Neuroscience](#experimental-neuroscience)
7. [Data Analysis and Visualization](#data-analysis-and-visualization)
8. [Production Systems](#production-systems)

---

## 1. Real-Time Agent Control

This example demonstrates real-time control of neural agents in a dynamic environment.

```python
import nlm
import time
import numpy as np
from concurrent.futures import ThreadPoolExecutor

class RealTimeAgentController:
    def __init__(self, world_size=50, vision_size=8):
        self.config = nlm.createDevelopmentConfig()
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = nlm.createSimpleWorld()
        self.world.configure(width=world_size, height=world_size, 
                           visionWidth=vision_size, visionHeight=vision_size)
        self.world.reset()
        self.world.setAgentStart(world_size // 2, world_size // 2)
        
        self.agent = nlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable advanced learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Performance monitoring
        self.performance_profiler = nlm.createPerformanceProfiler()
        self.start_time = time.time()
        
    def run_real_time_episode(self, duration=10.0, dt=0.01):
        """Run a real-time simulation episode with precise timing."""
        
        steps = int(duration / dt)
        last_print = time.time()
        
        for step in range(steps):
            loop_start = time.time()
            
            # Update world with fixed time step
            self.world.update(dt)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(step)
            self.performance_profiler.recordStepTime()
            
            # Get action and execute
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward based on action outcome
            result = self.world.getLastActionResult()
            if result:
                self.agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(dt)
            
            # Real-time monitoring and control
            current_time = time.time()
            if current_time - last_print > 1.0:
                self.print_real_time_stats(step, dt, current_time)
                last_print = current_time
            
            # Maintain real-time pace
            elapsed = time.time() - loop_start
            if elapsed < dt:
                time.sleep(dt - elapsed)
        
        return self.get_episode_summary()
    
    def print_real_time_stats(self, step, dt, current_time):
        """Print real-time statistics for monitoring."""
        real_time_elapsed = current_time - self.start_time
        sim_steps_per_real_second = step / real_time_elapsed if real_time_elapsed > 0 else 0
        
        stats = nlm.getBrainStats(self.brain, step, self.world.getSimulationTime())
        
        print(f"\n=== Real-Time Stats (Step {step}) ===")
        print(f"Real-time elapsed: {real_time_elapsed:.2f}s")
        print(f"Simulation speed: {sim_steps_per_real_second:.1f} steps/sec")
        print(f"Neurons active: {stats['firingNeuronCount']}/{stats['neuronCount']}")
        print(f"Firing rate: {stats['averageFiringRate']:.2f} Hz")
        print(f"Development stage: {stats['developmentStage']}")
        print(f"Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"Novelty level: {self.agent.getNoveltyLevel():.3f}")
        
        # Performance report
        perf_report = self.performance_profiler.getPerformanceReport()
        print(f"Performance: {perf_report}")
    
    def get_episode_summary(self):
        """Get comprehensive episode summary."""
        final_stats = nlm.getBrainStats(self.brain)
        perf_report = self.performance_profiler.getPerformanceReport()
        
        return {
            'total_steps': int(self.brain.getTotalSpikeCount()),
            'firing_neurons': final_stats['firingNeuronCount'],
            'average_firing_rate': final_stats['averageFiringRate'],
            'development_stage': final_stats['developmentStage'],
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'performance_report': perf_report
        }

# Usage example
def example_real_time_control():
    print("=== Real-Time Agent Control Example ===")
    
    controller = RealTimeAgentController(world_size=100, vision_size=16)
    
    # Run real-time simulation
    summary = controller.run_real_time_episode(duration=30.0, dt=0.05)
    
    print(f"\n=== Episode Summary ===")
    print(f"Total spikes: {summary['total_steps']}")
    print(f"Final firing rate: {summary['average_firing_rate']:.2f} Hz")
    print(f"Final development stage: {summary['development_stage']}")
    
    return controller, summary

if __name__ == "__main__":
    controller, summary = example_real_time_control()
```

## 2. Large-Scale Network Simulation

This example demonstrates efficient simulation of large-scale neural networks using performance optimizations.

```python
import nlm
import time
import numpy as np
from concurrent.futures import ThreadPoolExecutor
import threading

class LargeScaleNetworkSimulator:
    def __init__(self, num_neurons=10000, num_synapses=500000, 
                 thread_count=4, enable_optimizations=True):
        
        print(f"Initializing large-scale network with {num_neurons:,} neurons, {num_synapses:,} synapses")
        
        # Create optimized configuration
        if enable_optimizations:
            config = nlm.createDevelopmentConfig()
            config.setInt("brain.neuronCount", num_neurons)
            config.setInt("brain.regionCount", 8)
            config.setDouble("brain.learningRate", 0.01)
            config.setBool("brain.plasticityEnabled", True)
        else:
            config = nlm.createMinimalConfig()
            config.setInt("brain.neuronCount", num_neurons)
        
        # Create optimized brain with all performance features
        self.brain = nlm.createOptimizedBrain(config)
        self.brain.enableEventDriven(True)
        self.brain.enableMultithreading(True)
        self.brain.enableSIMD(True)
        self.brain.enableSparseConnectivity(True)
        self.brain.setThreadCount(thread_count)
        
        # Initialize with timing
        init_start = time.time()
        self.brain.initialize()
        init_time = time.time() - init_start
        
        print(f"Initialization time: {init_time:.3f}s")
        print(f"Neurons per second: {num_neurons/init_time:,.0f}")
        
        # Performance monitoring
        self.performance_profiler = nlm.createPerformanceProfiler()
        self.performance_profiler.recordInitializationTime()
        
        # Memory pools for efficient allocation
        self.neuron_pool = nlm.createNeuronPool()
        self.synapse_pool = nlm.createSynapsePool()
        
        # Thread pool for parallel operations
        self.thread_pool = nlm.createThreadPool(thread_count)
        
    def simulate_large_network(self, steps=1000, batch_size=100):
        """Simulate large network with optimized processing."""
        
        print(f"\nStarting large network simulation: {steps} steps")
        
        # Use parallel processing for large steps
        if steps > 1000:
            self._simulate_parallel(steps, batch_size)
        else:
            self._simulate_sequential(steps)
        
    def _simulate_parallel(self, total_steps, batch_size):
        """Simulate using parallel processing."""
        
        def process_batch(batch_id, start_step, end_step):
            """Process a batch of steps."""
            for step in range(start_step, end_step):
                self.brain.step(step)
                self.performance_profiler.recordStepTime()
                
                # Record stats periodically
                if step % 100 == 0:
                    stats = nlm.getBrainStats(self.brain, step)
                    print(f"Batch {batch_id}: Step {step}, "
                          f"Neurons: {stats['firingNeuronCount']}, "
                          f"Rate: {stats['averageFiringRate']:.2f}Hz")
        
        # Divide work into batches for parallel processing
        num_batches = min(4, (total_steps + batch_size - 1) // batch_size)
        batch_size = (total_steps + num_batches - 1) // num_batches
        
        print(f"Using {num_batches} parallel batches")
        
        with ThreadPoolExecutor(max_workers=num_batches) as executor:
            futures = []
            
            for batch_id in range(num_batches):
                start_step = batch_id * batch_size
                end_step = min(start_step + batch_size, total_steps)
                
                future = executor.submit(process_batch, batch_id, start_step, end_step)
                futures.append(future)
            
            # Wait for all batches to complete
            for future in futures:
                future.result()
    
    def _simulate_sequential(self, steps):
        """Simulate using sequential processing."""
        
        print(f"Using sequential processing")
        
        for step in range(steps):
            self.brain.step(step)
            self.performance_profiler.recordStepTime()
            
            if step % 100 == 0:
                stats = nlm.getBrainStats(self.brain, step)
                print(f"Step {step}: Neurons: {stats['firingNeuronCount']}, "
                      f"Rate: {stats['averageFiringRate']:.2f}Hz")
    
    def get_performance_summary(self):
        """Get comprehensive performance summary."""
        
        # Get optimized brain stats
        opt_stats = self.brain.getPerformanceStats()
        
        # Get Python performance stats
        python_stats = nlm.getBrainStats(self.brain)
        
        # Get profiler report
        perf_report = self.performance_profiler.getPerformanceReport()
        
        # Memory usage
        brain_memory = self.brain.getMemoryUsage()
        total_memory = (brain_memory + 
                       self.neuron_pool.memoryUsage() + 
                       self.synapse_pool.memoryUsage())
        
        return {
            'optimization_stats': {
                'event_driven_enabled': opt_stats.eventDrivenEnabled,
                'multithreading_enabled': opt_stats.multithreadingEnabled,
                'simd_enabled': opt_stats.simdEnabled,
                'sparse_enabled': opt_stats.sparseEnabled,
                'thread_count': opt_stats.threadCount
            },
            'brain_stats': {
                'neurons': python_stats['neuronCount'],
                'synapses': python_stats['synapseCount'],
                'active_neurons': python_stats['activeNeuronCount'],
                'firing_neurons': python_stats['firingNeuronCount'],
                'firing_rate': python_stats['averageFiringRate'],
                'performance_factor': opt_stats.performanceFactor
            },
            'performance_metrics': {
                'initialization_time': opt_stats.initializationTime,
                'step_time': opt_stats.stepTime,
                'neurons_per_second': opt_stats.neuronsPerSecond,
                'spikes_per_second': opt_stats.spikesPerSecond,
                'synaptic_events_per_second': opt_stats.synapticEventsPerSecond,
                'real_time_factor': opt_stats.realTimeFactor
            },
            'memory_usage': {
                'brain_mb': brain_memory / (1024 * 1024),
                'neuron_pool_mb': self.neuron_pool.memoryUsage() / (1024 * 1024),
                'synapse_pool_mb': self.synapse_pool.memoryUsage() / (1024 * 1024),
                'total_mb': total_memory / (1024 * 1024)
            },
            'profiling_report': perf_report
        }
    
    def save_simulation_results(self, filepath):
        """Save simulation results to file."""
        
        results = self.get_performance_summary()
        
        # Save brain state
        self.brain.save(f"{filepath}_brain.bin")
        
        # Save results as JSON
        import json
        with open(f"{filepath}_results.json", 'w') as f:
            # Convert non-serializable objects
            serializable_results = self._make_serializable(results)
            json.dump(serializable_results, f, indent=2)
        
        print(f"Results saved to {filepath}_* files")
    
    def _make_serializable(self, obj):
        """Convert results to serializable format."""
        
        if isinstance(obj, dict):
            return {k: self._make_serializable(v) for k, v in obj.items()}
        elif isinstance(obj, list):
            return [self._make_serializable(v) for v in obj]
        elif hasattr(obj, 'to_dict'):
            return obj.to_dict()
        elif hasattr(obj, '__dict__'):
            return obj.__dict__
        else:
            return obj

# Usage example
def example_large_scale_network():
    print("=== Large-Scale Network Simulation Example ===")
    
    # Create large-scale simulator
    simulator = LargeScaleNetworkSimulator(
        num_neurons=50000,
        num_synapses=2500000,
        thread_count=8,
        enable_optimizations=True
    )
    
    # Run simulation
    simulator.simulate_large_network(steps=500, batch_size=100)
    
    # Get performance summary
    summary = simulator.get_performance_summary()
    
    print(f"\n=== Performance Summary ===")
    print(f"Simulation completed successfully")
    print(f"Optimization factor: {summary['brain_stats']['performance_factor']:.2f}x")
    print(f"Memory usage: {summary['memory_usage']['total_mb']:.1f} MB")
    print(f"Real-time factor: {summary['performance_metrics']['real_time_factor']:.2f}")
    
    # Save results
    simulator.save_simulation_results("large_network_simulation")
    
    return simulator, summary

if __name__ == "__main__":
    simulator, summary = example_large_scale_network()
```

## 3. Multi-Agent Systems

This example demonstrates coordination and interaction between multiple neural agents.

```python
import nlm
import time
import numpy as np
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
from typing import List, Dict

@dataclass
class AgentState:
    id: int
    brain: nlm.Brain
    agent: nlm.AgentBrain
    world: nlm.SimpleWorld
    position: tuple
    energy: float
    health: float
    age: float
    learned_behaviors: List[str]
    social_connections: List[int]
    
class MultiAgentSystem:
    def __init__(self, num_agents=10, world_size=100):
        self.num_agents = num_agents
        self.world_size = world_size
        self.agents: List[AgentState] = []
        self.agent_brain_configs = []
        
        # Create different configurations for diverse agents
        self._initialize_agent_configs()
        
        # Create world
        self.world = nlm.createSimpleWorld()
        self.world.configure(width=world_size, height=world_size,
                           visionWidth=8, visionHeight=8)
        self.world.reset()
        
        # Create performance profiler for system monitoring
        self.performance_profiler = nlm.createPerformanceProfiler()
        
        print(f"Initialized multi-agent system with {num_agents} agents")
        
    def _initialize_agent_configs(self):
        """Create different configurations for each agent."""
        base_config = nlm.createDefaultConfig()
        
        for i in range(self.num_agents):
            # Create diverse configurations
            config = nlm.createDefaultConfig()
            
            # Vary parameters for diversity
            neuron_count = 1000 + i * 200  # Different sizes
            config.setInt("brain.neuronCount", neuron_count)
            
            learning_rate = 0.1 - (i * 0.01)  # Different learning rates
            config.setDouble("brain.learningRate", learning_rate)
            
            curiosity_level = 0.5 + (i % 3) * 0.3  # Different curiosity levels
            config.setDouble("agent.curiosityLevel", curiosity_level)
            
            enable_development = (i % 2) == 0  # Half have development
            config.setBool("agent.enableDevelopment", enable_development)
            
            # Enable different subsystems based on agent type
            if i % 3 == 0:
                config.setBool("agent.enableRewardModulation", True)
            if i % 4 == 0:
                config.setBool("agent.enableStructuralPlasticity", True)
                
            self.agent_brain_configs.append(config)
    
    def create_agents(self):
        """Create all agents with diverse characteristics."""
        
        for i in range(self.num_agents):
            # Create brain with specific configuration
            brain_config = self.agent_brain_configs[i]
            brain = nlm.createBrain(brain_config)
            brain.initialize()
            
            # Create agent brain interface
            agent = nlm.createAgentBrain(brain)
            
            # Initialize agent with world
            agent.initialize(self.world)
            
            # Enable appropriate subsystems based on agent type
            if i % 3 == 0:
                agent.enableRewardModulation(True)
            if i % 4 == 0:
                agent.enableStructuralPlasticity(True)
            if i % 2 == 0:
                agent.enableDevelopment(True)
            if i % 3 == 1:
                agent.enableCuriosity(True)
            
            # Set initial state
            x = np.random.uniform(10, self.world_size - 10)
            y = np.random.uniform(10, self.world_size - 10)
            
            # Place agent in world
            self.world.setAgentStart(x, y)
            
            # Create agent state
            agent_state = AgentState(
                id=i,
                brain=brain,
                agent=agent,
                world=self.world,
                position=(x, y),
                energy=100.0,
                health=100.0,
                age=0.0,
                learned_behaviors=[],
                social_connections=[]
            )
            
            self.agents.append(agent_state)
        
        print(f"Created {len(self.agents)} agents")
    
    def run_coordinated_simulation(self, steps=1000, dt=0.1):
        """Run coordinated multi-agent simulation with social interactions."""
        
        print(f"\nStarting coordinated multi-agent simulation: {steps} steps")
        
        # Performance monitoring
        start_time = time.time()
        
        for step in range(steps):
            step_start = time.time()
            
            # 1. Update all agents' internal states
            self._update_agent_states(step)
            
            # 2. Process world events (collisions, interactions)
            self._process_world_events(step)
            
            # 3. Coordinate agent actions
            self._coordinate_agent_actions(step)
            
            # 4. Update social connections
            self._update_social_connections(step)
            
            # 5. Record statistics
            if step % 100 == 0:
                self._print_system_stats(step, time.time() - start_time)
            
            # 6. Maintain real-time pace
            elapsed = time.time() - step_start
            if elapsed < dt:
                time.sleep(dt - elapsed)
        
        return self._get_system_summary()
    
    def _update_agent_states(self, step):
        """Update all agents' neural states."""
        
        # Use parallel processing for agent updates
        with ThreadPoolExecutor(max_workers=min(8, len(self.agents))) as executor:
            futures = []
            
            for agent_state in self.agents:
                future = executor.submit(self._update_single_agent, agent_state, step)
                futures.append(future)
            
            # Wait for all updates
            for future in futures:
                future.result()
    
    def _update_single_agent(self, agent_state, step):
        """Update a single agent's state."""
        
        # Get sensory input from world
        percept = agent_state.world.getSensoryPercept()
        
        # Process sensory input
        agent_state.agent.processSensoryInput(percept)
        
        # Update brain
        agent_state.brain.step(step)
        
        # Update agent state
        agent_state.age += 0.1
        
        # Social learning based on nearby agents
        self._apply_social_learning(agent_state, step)
        
        # Novelty-driven exploration
        self._apply_curiosity_dynamics(agent_state, step)
    
    def _process_world_events(self, step):
        """Process world events and agent interactions."""
        
        # Check for agent collisions
        self._detect_collisions()
        
        # Process resource sharing
        self._process_resource_sharing()
        
        # Handle social interactions
        self._handle_social_interactions()
    
    def _coordinate_agent_actions(self, step):
        """Coordinate actions across all agents."""
        
        # Get actions from all agents
        actions = []
        for agent_state in self.agents:
            action = agent_state.agent.decodeMotorCommand()
            actions.append((agent_state.id, action))
        
        # Apply actions to world
        for agent_id, action in actions:
            agent_state = next(a for a in self.agents if a.id == agent_id)
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update agent state based on action outcome
            self._update_agent_from_action(agent_state, action)
    
    def _update_social_connections(self, step):
        """Update social connections between agents."""
        
        # Create social network based on proximity and interaction history
        for agent_state in self.agents:
            nearby_agents = self._get_nearby_agents(agent_state)
            
            # Update social connections
            new_connections = []
            for nearby in nearby_agents:
                if np.random.random() < 0.1:  # Social bond formation probability
                    new_connections.append(nearby.id)
                    
                    # Learn from nearby agents (social learning)
                    self._social_learning_between_agents(agent_state, nearby, step)
            
            agent_state.social_connections = new_connections
    
    def _get_nearby_agents(self, agent_state):
        """Get agents near the given agent."""
        
        nearby = []
        agent_x, agent_y = agent_state.position
        
        for other in self.agents:
            if other.id == agent_state.id:
                continue
                
            other_x, other_y = other.position
            distance = np.sqrt((agent_x - other_x)**2 + (agent_y - other_y)**2)
            
            if distance < 20.0:  # Social interaction radius
                nearby.append(other)
        
        return nearby
    
    def _detect_collisions(self):
        """Detect and handle agent collisions."""
        
        for agent_state in self.agents:
            x, y = agent_state.position
            
            # Check if position is valid (not occupied by another agent)
            for other in self.agents:
                if other.id == agent_state.id:
                    continue
                    
                other_x, other_y = other.position
                distance = np.sqrt((x - other_x)**2 + (y - other_y)**2)
                
                if distance < 5.0:  # Collision radius
                    # Resolve collision
                    self._resolve_collision(agent_state, other)
    
    def _resolve_collision(self, agent1, agent2):
        """Resolve collision between two agents."""
        
        # Move agents apart
        x1, y1 = agent1.position
        x2, y2 = agent2.position
        
        dx = x2 - x1
        dy = y2 - y1
        distance = np.sqrt(dx**2 + dy**2)
        
        if distance > 0:
            # Move each agent away from collision
            overlap = 5.0 - distance
            if overlap > 0:
                move_x = (dx / distance) * (overlap / 2)
                move_y = (dy / distance) * (overlap / 2)
                
                agent1.position = (x1 - move_x, y1 - move_y)
                agent2.position = (x2 + move_x, y2 + move_y)
                
                # Apply collision penalty
                agent1.energy *= 0.95
                agent2.energy *= 0.95
    
    def _print_system_stats(self, step, elapsed_time):
        """Print system-wide statistics."""
        
        print(f"\n=== System Stats (Step {step}) ===")
        print(f"Real-time elapsed: {elapsed_time:.2f}s")
        print(f"Simulation speed: {step / elapsed_time:.1f} steps/sec")
        
        # Aggregate statistics
        total_neurons = sum(a.agent.getSensoryInputSize() for a in self.agents)
        active_neurons = sum(a.agent.getSensoryInputSize() for a in self.agents if a.agent.getSensoryInputSize() > 0)
        
        avg_firing_rate = np.mean([a.brain.getAverageFiringRate() for a in self.agents])
        avg_curiosity = np.mean([a.agent.getCuriosityLevel() for a in self.agents])
        
        print(f"Total agents: {len(self.agents)}")
        print(f"Total neurons: {total_neurons}")
        print(f"Avg firing rate: {avg_firing_rate:.2f} Hz")
        print(f"Avg curiosity level: {avg_curiosity:.3f}")
        
        # Social network statistics
        total_connections = sum(len(a.social_connections) for a in self.agents)
        avg_connections = total_connections / len(self.agents) if self.agents else 0
        print(f"Social connections: {total_connections} total, {avg_connections:.1f} avg per agent")
    
    def _get_system_summary(self):
        """Get comprehensive system summary."""
        
        return {
            'total_agents': len(self.agents),
            'simulation_steps': int(self.world.getSimulationTime()),
            'avg_firing_rate': np.mean([a.brain.getAverageFiringRate() for a in self.agents]),
            'avg_curiosity': np.mean([a.agent.getCuriosityLevel() for a in self.agents]),
            'avg_health': np.mean([a.health for a in self.agents]),
            'total_social_connections': sum(len(a.social_connections) for a in self.agents),
            'agent_diversity': {
                'configurations_used': len(set(id(config) for config in self.agent_brain_configs)),
                'enabled_features': {
                    'reward_modulation': sum(1 for a in self.agents if a.agent.isRewardModulationEnabled()),
                    'structural_plasticity': sum(1 for a in self.agents if a.agent.isStructuralPlasticityEnabled()),
                    'development': sum(1 for a in self.agents if a.agent.isDevelopmentEnabled()),
                    'curiosity': sum(1 for a in self.agents if a.agent.isCuriosityEnabled())
                }
            }
        }

# Usage example
def example_multi_agent_system():
    print("=== Multi-Agent System Example ===")
    
    # Create multi-agent system
    system = MultiAgentSystem(num_agents=8, world_size=200)
    system.create_agents()
    
    # Run simulation
    summary = system.run_coordinated_simulation(steps=500, dt=0.05)
    
    print(f"\n=== System Summary ===")
    print(f"Final number of agents: {summary['total_agents']}")
    print(f"Final simulation steps: {summary['simulation_steps']}")
    print(f"Average firing rate: {summary['avg_firing_rate']:.2f} Hz")
    print(f"Average curiosity level: {summary['avg_curiosity']:.3f}")
    
    return system, summary

if __name__ == "__main__":
    system, summary = example_multi_agent_system()
```

## 4. Learning and Adaptation

This example demonstrates advanced learning mechanisms and adaptation over time.

```python
import nlm
import time
import numpy as np
from dataclasses import dataclass
from typing import List, Dict, Callable
import json

@dataclass
class LearningMetrics:
    step: int
    reward_history: List[float]
    prediction_error: float
    curiosity_level: float
    development_stage: int
    adaptation_score: float
    exploration_exploitation_ratio: float

class AdaptiveLearner:
    def __init__(self, environment):
        self.environment = environment
        
        # Create adaptive brain with development
        self.config = nlm.createDevelopmentConfig()
        self.brain = nlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent brain
        self.agent = nlm.createAgentBrain(self.brain)
        self.agent.initialize(environment.world)
        
        # Enable all adaptive subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Learning state
        self.learning_metrics: List[LearningMetrics] = []
        self.adaptation_strategy = "balanced"
        self.meta_learning_rate = 0.01
        self.exploration_budget = 1.0
        
        # Performance tracking
        self.performance_history: List[Dict] = []
        
        print("Adaptive learner initialized with all learning subsystems")
        
    def run_adaptive_learning_episode(self, max_steps=1000, dt=0.1):
        """Run an adaptive learning episode with meta-learning."""
        
        print(f"\nStarting adaptive learning episode: {max_steps} steps")
        
        for step in range(max_steps):
            # 1. Select adaptation strategy based on performance
            self._update_adaptation_strategy(step)
            
            # 2. Execute action according to current strategy
            action = self._execute_action(step)
            
            # 3. Receive environment feedback
            reward, prediction_error, novelty = self._receive_feedback(action)
            
            # 4. Update learning subsystems
            self._update_learning_systems(reward, prediction_error, novelty, dt)
            
            # 5. Meta-learning update
            self._meta_learning_update(step, reward, prediction_error)
            
            # 6. Record learning metrics
            metrics = self._record_learning_metrics(step, reward, prediction_error)
            self.learning_metrics.append(metrics)
            
            # 7. Check for learning milestones
            self._check_learning_milestones(step, metrics)
            
            # 8. Update performance tracking
            self._update_performance_tracking(step, metrics)
            
            # 9. Print progress
            if step % 100 == 0:
                self._print_learning_progress(step, metrics)
        
        return self._get_learning_summary()
    
    def _update_adaptation_strategy(self, step):
        """Update adaptation strategy based on learning performance."""
        
        if not self.learning_metrics:
            return
        
        recent_metrics = self.learning_metrics[-10:]
        recent_rewards = [m.reward_history[-1] for m in recent_metrics]
        avg_reward = np.mean(recent_rewards)
        reward_variance = np.var(recent_rewards)
        
        # Strategy selection based on learning dynamics
        if avg_reward < 0.1 and reward_variance > 0.5:
            # Low performance, high variance: explore more
            self.adaptation_strategy = "exploratory"
            self.exploration_budget = min(1.0, self.exploration_budget + 0.1)
            
        elif avg_reward > 0.8 and reward_variance < 0.1:
            # High performance, low variance: exploit current strategy
            self.adaptation_strategy = "exploitative"
            self.exploration_budget = max(0.2, self.exploration_budget - 0.1)
            
        else:
            # Balanced performance: maintain balanced strategy
            self.adaptation_strategy = "balanced"
            self.exploration_budget = max(0.5, self.exploration_budget - 0.05)
    
    def _execute_action(self, step):
        """Execute action according to current adaptation strategy."""
        
        if self.adaptation_strategy == "exploratory":
            # Explore new behaviors
            return self._explore_action()
            
        elif self.adaptation_strategy == "exploitative":
            # Exploit learned behaviors
            return self._exploit_action()
            
        else:  # balanced
            # Balance exploration and exploitation
            return self._balanced_action(step)
    
    def _explore_action(self):
        """Select exploratory action."""
        
        # Choose random action (high curiosity)
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        
        # Use curiosity-driven action selection
        action = self.agent.decodeMotorCommand()
        
        # Record exploration
        self._record_exploration(action)
        
        return action
    
    def _exploit_action(self):
        """Select exploitative action."""
        
        # Focus on learned behaviors
        self.agent.enableCuriosity(False)  # Reduce exploration
        self.agent.enableRewardModulation(True)
        
        action = self.agent.decodeMotorCommand()
        
        # Record exploitation
        self._record_exploitation(action)
        
        return action
    
    def _balanced_action(self, step):
        """Select balanced action (epsilon-greedy with dynamic epsilon)."""
        
        # Dynamic exploration rate based on learning progress
        epsilon = 0.3 * np.exp(-step / (1000 * self.meta_learning_rate))
        
        if np.random.random() < epsilon:
            # Explore
            return self._explore_action()
        else:
            # Exploit
            return self._exploit_action()
    
    def _receive_feedback(self, action):
        """Receive feedback from environment."""
        
        # Apply action to environment
        self.environment.world.applyMotorCommand(action, self.environment.world.getSimulationTime())
        
        # Get reward
        result = self.environment.world.getLastActionResult()
        reward = result.reward if result else 0.0
        
        # Get prediction error
        prediction_error = self.agent.getPredictionError()
        
        # Get novelty
        novelty = self.agent.getNoveltyLevel()
        
        return reward, prediction_error, novelty
    
    def _update_learning_systems(self, reward, prediction_error, novelty, dt):
        """Update all learning subsystems."""
        
        # Update reward modulation
        self.agent.applyRewardModulation(reward, prediction_error)
        
        # Update development
        self.agent.updateDevelopment(dt)
        
        # Curiosity-driven exploration
        curiosity_threshold = 0.5 + novelty * 0.5
        if self.agent.getCuriosityLevel() < curiosity_threshold:
            self.agent.enableCuriosity(True)
        else:
            self.agent.enableCuriosity(False)
    
    def _meta_learning_update(self, step, reward, prediction_error):
        """Update meta-learning parameters."""
        
        # Update meta-learning rate based on prediction error
        learning_momentum = 0.9
        self.meta_learning_rate = (self.meta_learning_rate * learning_momentum + 
                                  prediction_error * (1 - learning_momentum))
        
        # Adapt exploration budget based on reward stability
        if step > 100:
            recent_rewards = [m.reward_history[-1] for m in self.learning_metrics[-20:]]
            reward_stability = 1.0 - np.std(recent_rewards) / (np.mean(np.abs(recent_rewards)) + 0.01)
            self.exploration_budget *= reward_stability
    
    def _record_learning_metrics(self, step, reward, prediction_error):
        """Record learning metrics for analysis."""
        
        metrics = LearningMetrics(
            step=step,
            reward_history=[reward],
            prediction_error=prediction_error,
            curiosity_level=self.agent.getCuriosityLevel(),
            development_stage=int(self.brain.getDevelopmentalStage()),
            adaptation_score=self._calculate_adaptation_score(step),
            exploration_exploitation_ratio=self._calculate_exploration_ratio(step)
        )
        
        return metrics
    
    def _calculate_adaptation_score(self, step):
        """Calculate adaptation score based on learning progress."""
        
        if step < 10:
            return 0.0
        
        # Compare current performance with initial performance
        initial_metrics = self.learning_metrics[0]
        current_metrics = self.learning_metrics[-1]
        
        reward_improvement = np.mean(current_metrics.reward_history) - np.mean(initial_metrics.reward_history)
        curiosity_efficiency = current_metrics.curiosity_level / (step / 100)
        
        return min(1.0, (reward_improvement + curiosity_efficiency) / 2)
    
    def _calculate_exploration_ratio(self, step):
        """Calculate exploration-exploitation ratio."""
        
        if step < 10:
            return 0.5
        
        # Based on curiosity level and action diversity
        curiosity_ratio = self.agent.getCuriosityLevel()
        action_diversity = np.random.random()  # Simplified
        
        return curiosity_ratio * action_diversity
    
    def _check_learning_milestones(self, step, metrics):
        """Check for learning milestones and trigger events."""
        
        # Check developmental stage milestones
        if metrics.development_stage >= 2:  # Critical period
            print(f"Developmental milestone reached: Critical Period (Step {step})")
            
        if metrics.development_stage >= 3:  # Maturation
            print(f"Developmental milestone reached: Maturation (Step {step})")
            
        if metrics.adaptation_score > 0.8:
            print(f"Learning milestone reached: High adaptation score (Step {step})")
            
        if metrics.exploration_exploitation_ratio > 0.7:
            print(f"Exploration milestone reached: High exploration (Step {step})")
    
    def _update_performance_tracking(self, step, metrics):
        """Update performance tracking for analysis."""
        
        performance_data = {
            'step': step,
            'firing_rate': self.brain.getAverageFiringRate(),
            'active_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'development_stage': int(self.brain.getDevelopmentalStage()),
            'reward': metrics.reward_history[-1],
            'prediction_error': metrics.prediction_error,
            'curiosity_level': metrics.curiosity_level,
            'adaptation_score': metrics.adaptation_score,
            'exploration_ratio': metrics.exploration_exploitation_ratio
        }
        
        self.performance_history.append(performance_data)
    
    def _print_learning_progress(self, step, metrics):
        """Print learning progress report."""
        
        print(f"\n=== Learning Progress (Step {step}) ===")
        print(f"Development Stage: {metrics.development_stage}")
        print(f"Current Reward: {metrics.reward_history[-1]:.3f}")
        print(f"Prediction Error: {metrics.prediction_error:.3f}")
        print(f"Curiosity Level: {metrics.curiosity_level:.3f}")
        print(f"Adaptation Score: {metrics.adaptation_score:.3f}")
        print(f"Exploration Ratio: {metrics.exploration_exploitation_ratio:.3f}")
        print(f"Strategy: {self.adaptation_strategy}")
        print(f"Exploration Budget: {self.exploration_budget:.3f}")
    
    def _get_learning_summary(self):
        """Get comprehensive learning summary."""
        
        if not self.learning_metrics:
            return {}
        
        final_metrics = self.learning_metrics[-1]
        performance_trend = self._calculate_performance_trend()
        
        return {
            'total_steps': len(self.learning_metrics),
            'final_development_stage': final_metrics.development_stage,
            'final_adaptation_score': final_metrics.adaptation_score,
            'max_reward_achieved': max(m.reward_history[-1] for m in self.learning_metrics),
            'min_reward_achieved': min(m.reward_history[-1] for m in self.learning_metrics),
            'avg_reward': np.mean([m.reward_history[-1] for m in self.learning_metrics]),
            'final_curiosity': final_metrics.curiosity_level,
            'final_prediction_error': final_metrics.prediction_error,
            'learning_trend': performance_trend,
            'total_exploration_episodes': sum(1 for m in self.learning_metrics if m.exploration_exploitation_ratio > 0.7),
            'total_exploitation_episodes': sum(1 for m in self.learning_metrics if m.exploration_exploitation_ratio < 0.3)
        }
    
    def _calculate_performance_trend(self):
        """Calculate learning performance trend."""
        
        if len(self.performance_history) < 10:
            return "stable"
        
        # Calculate trend based on recent performance
        recent_perf = self.performance_history[-10:]
        rewards = [p['reward'] for p in recent_perf]
        
        # Simple linear regression to check trend
        x = list(range(len(rewards)))
        y = rewards
        
        if len(x) > 1:
            slope = np.polyfit(x, y, 1)[0]
            if slope > 0.01:
                return "improving"
            elif slope < -0.01:
                return "declining"
            else:
                return "stable"
        
        return "stable"

# Usage example
def example_adaptive_learning():
    print("=== Adaptive Learning Example ===")
    
    # Create adaptive learner with environment
    environment = None  # Would be defined in full implementation
    learner = AdaptiveLearner(environment)
    
    # Run adaptive learning episode
    summary = learner.run_adaptive_learning_episode(max_steps=500, dt=0.05)
    
    print(f"\n=== Learning Summary ===")
    print(f"Total learning steps: {summary['total_steps']}")
    print(f"Final development stage: {summary['final_development_stage']}")
    print(f"Final adaptation score: {summary['final_adaptation_score']:.3f}")
    print(f"Max reward achieved: {summary['max_reward_achieved']:.3f}")
    print(f"Min reward achieved: {summary['min_reward_achieved']:.3f}")
    print(f"Average reward: {summary['avg_reward']:.3f}")
    print(f"Learning trend: {summary['learning_trend']}")
    
    return learner, summary

if __name__ == "__main__":
    learner, summary = example_adaptive_learning()
```

## Next Steps

These examples demonstrate the power and flexibility of the enhanced NLM Python API. As you implement these examples in your own projects, you'll discover additional use cases and optimization opportunities.

The key takeaways are:

1. **Real-time control** requires careful timing and performance monitoring
2. **Large-scale simulations** benefit from performance optimizations and parallel processing
3. **Multi-agent systems** require coordination mechanisms and social interaction modeling
4. **Adaptive learning** involves meta-learning and strategy adaptation

These examples provide a foundation for building sophisticated neural simulation applications with the enhanced NLM Python API.
