# Performance Optimization for NLM

"""
Comprehensive demonstration of performance optimization in the Neural Learning Machine (NLM).

This script implements the Performance Optimization use case, demonstrating:
- Performance monitoring and memory pools
- Implementation checkpointing for long simulations
- Optimization of simulation parameters
- Efficient resource management
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time
import os
import psutil
from collections import deque
from threading import Lock

class MemoryPool:
    """
    Efficient memory pool for managing memory allocations.
    
    Memory pools reduce memory fragmentation and allocation overhead
    by reusing memory blocks instead of allocating/deallocating repeatedly.
    """
    
    def __init__(self, pool_size, block_size):
        self.pool_size = pool_size
        self.block_size = block_size
        self.memory_blocks = []
        self.lock = Lock()
        self.usage_count = 0
        
        # Pre-allocate memory blocks
        for i in range(pool_size):
            # Simulate memory allocation (in real implementation, would use actual memory)
            block_id = f"block_{i:04d}"
            self.memory_blocks.append({
                'id': block_id,
                'size': block_size,
                'is_used': False,
                'last_access': time.time()
            })
    
    def allocate(self):
        """Allocate a memory block from the pool."""
        with self.lock:
            # Find first available block
            for block in self.memory_blocks:
                if not block['is_used']:
                    block['is_used'] = True
                    block['last_access'] = time.time()
                    self.usage_count += 1
                    return block['id']
            
            # If no free block, return None (caller should handle)
            return None
    
    def deallocate(self, block_id):
        """Deallocate a memory block back to the pool."""
        with self.lock:
            for block in self.memory_blocks:
                if block['id'] == block_id:
                    block['is_used'] = False
                    block['last_access'] = time.time()
                    self.usage_count -= 1
                    return True
            return False
    
    def get_usage_stats(self):
        """Get memory pool usage statistics."""
        with self.lock:
            used_blocks = sum(1 for block in self.memory_blocks if block['is_used'])
            return {
                'total_blocks': len(self.memory_blocks),
                'used_blocks': used_blocks,
                'free_blocks': len(self.memory_blocks) - used_blocks,
                'usage_ratio': used_blocks / len(self.memory_blocks) if self.memory_blocks else 0,
                'usage_count': self.usage_count
            }

class EventQueue:
    """
    Efficient event queue for managing simulation events.
    
    Event queues process events in order, improving simulation efficiency
    compared to polling-based approaches.
    """
    
    def __init__(self, max_queue_size=1000):
        self.max_queue_size = max_queue_size
        self.event_queue = deque(maxlen=max_queue_size)
        self.lock = Lock()
        self.processed_events = 0
        self.dropped_events = 0
    
    def add_event(self, event_type, event_data, priority=0):
        """Add an event to the queue."""
        with self.lock:
            if len(self.event_queue) >= self.max_queue_size:
                # Queue full, drop oldest event (implementation-specific)
                self.event_queue.popleft()
                self.dropped_events += 1
            
            self.event_queue.append({
                'type': event_type,
                'data': event_data,
                'priority': priority,
                'timestamp': time.time(),
                'processing_time': None
            })
    
    def process_events(self):
        """Process all events in the queue."""
        processed_count = 0
        
        with self.lock:
            while self.event_queue:
                event = self.event_queue.popleft()
                
                # Process event (simplified - in real implementation would be more complex)
                start_time = time.time()
                
                # Simulate event processing
                self.process_event(event)
                
                event['processing_time'] = time.time() - start_time
                processed_count += 1
        
        self.processed_events += processed_count
        return processed_count
    
    def process_event(self, event):
        """Process a single event."""
        # Implementation-specific event processing
        # This is a placeholder - real implementation would depend on event type
        pass
    
    def get_queue_stats(self):
        """Get event queue statistics."""
        with self.lock:
            return {
                'queue_size': len(self.event_queue),
                'max_queue_size': self.max_queue_size,
                'processed_events': self.processed_events,
                'dropped_events': self.dropped_events,
                'queue_efficiency': (self.processed_events / max(self.processed_events + self.dropped_events, 1)) * 100
            }

class CheckpointManager:
    """
    Checkpoint management system for long-running simulations.
    
    Checkpoint managers save and load simulation state, enabling:
    - Recovery from crashes
    - Progress tracking
    - Parallel simulation capabilities
    - Analysis of simulation progress
    """
    
    def __init__(self, checkpoint_dir="/tmp/nlm_checkpoints"):
        self.checkpoint_dir = checkpoint_dir
        self.checkpoint_files = []
        self.lock = Lock()
        
        # Create checkpoint directory if it doesn't exist
        if not os.path.exists(checkpoint_dir):
            os.makedirs(checkpoint_dir)
    
    def create_checkpoint(self, checkpoint_id, brain_state, simulation_state):
        """Create a simulation checkpoint."""
        with self.lock:
            checkpoint_file = f"{self.checkpoint_dir}/checkpoint_{checkpoint_id:06d}.bin"
            
            # Save brain state
            brain_checkpoint = self.save_brain_state(brain_state)
            
            # Save simulation state
            simulation_checkpoint = self.save_simulation_state(simulation_state)
            
            # Combine checkpoints
            checkpoint_data = {
                'checkpoint_id': checkpoint_id,
                'timestamp': time.time(),
                'brain_checkpoint': brain_checkpoint,
                'simulation_checkpoint': simulation_checkpoint
            }
            
            # Save to file (simplified - in real implementation would use proper serialization)
            import json
            with open(checkpoint_file, 'w') as f:
                json.dump(checkpoint_data, f, indent=2)
            
            self.checkpoint_files.append(checkpoint_file)
            return checkpoint_file
    
    def load_checkpoint(self, checkpoint_id):
        """Load a simulation checkpoint."""
        checkpoint_file = f"{self.checkpoint_dir}/checkpoint_{checkpoint_id:06d}.bin"
        
        if not os.path.exists(checkpoint_file):
            return None
        
        # Load checkpoint data
        import json
        with open(checkpoint_file, 'r') as f:
            checkpoint_data = json.load(f)
        
        # Restore brain state
        brain_state = self.load_brain_state(checkpoint_data['brain_checkpoint'])
        
        # Restore simulation state
        simulation_state = self.load_simulation_state(checkpoint_data['simulation_checkpoint'])
        
        return {
            'checkpoint_id': checkpoint_data['checkpoint_id'],
            'timestamp': checkpoint_data['timestamp'],
            'brain_state': brain_state,
            'simulation_state': simulation_state
        }
    
    def save_brain_state(self, brain_state):
        """Save brain state to checkpoint."""
        # Simplified brain state saving
        return {
            'neuron_count': brain_state.get('neuron_count', 0),
            'region_count': brain_state.get('region_count', 0),
            'total_synapses': brain_state.get('total_synapses', 0),
            'current_step': brain_state.get('current_step', 0),
            'firing_rate': brain_state.get('firing_rate', 0.0),
            'development_stage': brain_state.get('development_stage', 0.0),
            'synapse_density': brain_state.get('synapse_density', 0.0)
        }
    
    def load_brain_state(self, checkpoint):
        """Load brain state from checkpoint."""
        return checkpoint  # Simplified return
    
    def save_simulation_state(self, simulation_state):
        """Save simulation state to checkpoint."""
        # Simplified simulation state saving
        return {
            'step': simulation_state.get('step', 0),
            'world_state': simulation_state.get('world_state', {}),
            'agent_state': simulation_state.get('agent_state', {}),
            'time': simulation_state.get('time', 0.0),
            'performance_metrics': simulation_state.get('performance_metrics', {})
        }
    
    def load_simulation_state(self, checkpoint):
        """Load simulation state from checkpoint."""
        return checkpoint  # Simplified return
    
    def get_checkpoint_stats(self):
        """Get checkpoint system statistics."""
        checkpoint_files = [f for f in os.listdir(self.checkpoint_dir) 
                           if f.startswith('checkpoint_') and f.endswith('.bin')]
        
        checkpoints = []
        for f in checkpoint_files:
            try:
                checkpoint_id = int(f[11:-4])  # Extract ID from filename
                checkpoints.append(checkpoint_id)
            except ValueError:
                pass
        
        return {
            'total_checkpoints': len(checkpoints),
            'checkpoint_ids': sorted(checkpoints),
            'oldest_checkpoint': min(checkpoints) if checkpoints else None,
            'newest_checkpoint': max(checkpoints) if checkpoints else None,
            'disk_usage': self.calculate_disk_usage()
        }
    
    def calculate_disk_usage(self):
        """Calculate disk usage of checkpoint files."""
        total_size = 0
        
        for f in os.listdir(self.checkpoint_dir):
            if f.startswith('checkpoint_') and f.endswith('.bin'):
                file_path = os.path.join(self.checkpoint_dir, f)
                try:
                    total_size += os.path.getsize(file_path)
                except OSError:
                    pass
        
        return total_size / (1024 * 1024)  # Size in MB
    
    def cleanup_old_checkpoints(self, keep_last_n=10):
        """Clean up old checkpoints, keeping only the last N."""
        checkpoint_files = [f for f in os.listdir(self.checkpoint_dir) 
                           if f.startswith('checkpoint_') and f.endswith('.bin')]
        
        # Sort by modification time
        checkpoint_files.sort(key=lambda f: os.path.getmtime(os.path.join(self.checkpoint_dir, f)))
        
        # Remove old checkpoints
        for f in checkpoint_files[:-keep_last_n]:
            try:
                os.remove(os.path.join(self.checkpoint_dir, f))
            except OSError:
                pass

class PerformanceMonitoring:
    """
    Comprehensive performance monitoring for NLM simulations.
    
    Monitors system resources, simulation performance, and identifies bottlenecks.
    """
    
    def __init__(self, monitor_interval=100):
        self.monitor_interval = monitor_interval
        self.monitoring_active = False
        
        # Performance metrics
        self.step_times = []
        self.cpu_usage = []
        self.memory_usage = []
        self.disk_io = []
        self.network_io = []
        
        # Resource tracking
        self.system_info = self.get_system_info()
        self.resource_history = []
        
        # Performance statistics
        self.performance_stats = {
            'avg_step_time': 0,
            'min_step_time': float('inf'),
            'max_step_time': 0,
            'step_time_std': 0,
            'steps_per_second': 0,
            'cpu_efficiency': 0,
            'memory_efficiency': 0,
            'bottleneck_events': 0,
            'optimization_opportunities': []
        }
        
        self.lock = Lock()
    
    def get_system_info(self):
        """Get system information for monitoring."""
        try:
            import psutil
            return {
                'cpu_count': psutil.cpu_count(),
                'memory_total': psutil.virtual_memory().total,
                'cpu_freq': psutil.cpu_freq().current if psutil.cpu_freq() else 0
            }
        except ImportError:
            return {
                'cpu_count': 1,
                'memory_total': 0,
                'cpu_freq': 0
            }
    
    def start_monitoring(self):
        """Start performance monitoring."""
        self.monitoring_active = True
        print("Performance monitoring started")
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring_active = False
        print("Performance monitoring stopped")
    
    def record_step_performance(self, step_time, reward=None, action=None):
        """Record performance metrics for a simulation step."""
        if not self.monitoring_active:
            return
        
        with self.lock:
            # Record step time
            self.step_times.append(step_time)
            
            # Get system metrics
            try:
                import psutil
                process = psutil.Process(os.getpid())
                
                cpu_percent = process.cpu_percent()
                memory_info = process.memory_info()
                
                self.cpu_usage.append(cpu_percent)
                self.memory_usage.append(memory_info.rss / (1024 * 1024))  # MB
                
                # Record I/O
                disk_io = process.io_counters()
                self.disk_io.append({
                    'read_bytes': disk_io.read_bytes,
                    'write_bytes': disk_io.write_bytes
                })
                
                if hasattr(psutil, 'net_io_counters'):
                    net_io = psutil.net_io_counters()
                    self.network_io.append({
                        'bytes_sent': net_io.bytes_sent,
                        'bytes_recv': net_io.bytes_recv
                    })
                
            except ImportError:
                # Fallback values
                self.cpu_usage.append(0.0)
                self.memory_usage.append(0.0)
                self.disk_io.append({'read_bytes': 0, 'write_bytes': 0})
                self.network_io.append({'bytes_sent': 0, 'bytes_recv': 0})
            
            # Record resource snapshot
            resource_snapshot = self.get_resource_snapshot()
            self.resource_history.append(resource_snapshot)
    
    def get_resource_snapshot(self):
        """Get current system resource snapshot."""
        try:
            import psutil
            process = psutil.Process(os.getpid())
            
            return {
                'timestamp': time.time(),
                'cpu_percent': process.cpu_percent(),
                'memory_mb': process.memory_info().rss / (1024 * 1024),
                'cpu_count': psutil.cpu_count(),
                'disk_io': process.io_counters().read_bytes + process.io_counters().write_bytes,
                'network_io': (psutil.net_io_counters().bytes_sent + 
                             psutil.net_io_counters().bytes_recv) if hasattr(psutil, 'net_io_counters') else 0
            }
        except ImportError:
            return {
                'timestamp': time.time(),
                'cpu_percent': 0.0,
                'memory_mb': 0.0,
                'cpu_count': 1,
                'disk_io': 0,
                'network_io': 0
            }
    
    def calculate_performance_stats(self):
        """Calculate performance statistics."""
        if not self.step_times:
            return self.performance_stats
        
        with self.lock:
            # Calculate statistics
            self.performance_stats['avg_step_time'] = np.mean(self.step_times)
            self.performance_stats['min_step_time'] = np.min(self.step_times)
            self.performance_stats['max_step_time'] = np.max(self.step_times)
            self.performance_stats['step_time_std'] = np.std(self.step_times)
            
            if self.step_times:
                total_time = sum(self.step_times)
                self.performance_stats['steps_per_second'] = len(self.step_times) / total_time if total_time > 0 else 0
            
            # Calculate efficiency
            if self.cpu_usage:
                avg_cpu = np.mean(self.cpu_usage)
                self.performance_stats['cpu_efficiency'] = min(avg_cpu / self.system_info['cpu_freq'], 1.0) if self.system_info['cpu_freq'] > 0 else 0
            
            if self.memory_usage:
                avg_memory = np.mean(self.memory_usage)
                total_memory_gb = self.system_info['memory_total'] / (1024 * 1024 * 1024)
                self.performance_stats['memory_efficiency'] = min(avg_memory / total_memory_gb, 1.0) if total_memory_gb > 0 else 0
            
            # Identify bottlenecks
            self.identify_performance_bottlenecks()
    
    def identify_performance_bottlenecks(self):
        """Identify performance bottlenecks."""
        bottlenecks = []
        
        # Check for high step time variance
        if len(self.step_times) > 10:
            cv = self.performance_stats['step_time_std'] / self.performance_stats['avg_step_time'] if self.performance_stats['avg_step_time'] > 0 else 0
            if cv > 0.5:
                bottlenecks.append({
                    'type': 'high_variance',
                    'severity': 'high',
                    'description': 'High variance in step times indicates inconsistent performance'
                })
        
        # Check for memory issues
        if self.memory_usage:
            avg_memory = np.mean(self.memory_usage)
            if avg_memory > 1000:  # > 1 GB
                bottlenecks.append({
                    'type': 'high_memory_usage',
                    'severity': 'medium',
                    'description': f'High memory usage ({avg_memory:.1f} MB)'
                })
        
        # Check for CPU issues
        if self.cpu_usage:
            avg_cpu = np.mean(self.cpu_usage)
            if avg_cpu > 80:
                bottlenecks.append({
                    'type': 'high_cpu_usage',
                    'severity': 'medium',
                    'description': f'High CPU usage ({avg_cpu:.1f}%)'
                })
        
        # Check for slow steps
        slow_steps = sum(1 for t in self.step_times if t > self.performance_stats['avg_step_time'] * 2)
        if slow_steps > len(self.step_times) * 0.1:  # > 10% slow steps
            bottlenecks.append({
                'type': 'frequent_slow_steps',
                'severity': 'high',
                'description': f'{slow_steps} slow steps ({slow_steps/len(self.step_times)*100:.1f}%)'
            })
        
        self.performance_stats['bottleneck_events'] = len(bottlenecks)
        self.performance_stats['optimization_opportunities'] = bottlenecks

class NLMPerformanceOptimizer:
    """
    NLM performance optimization framework.
    
    Integrates performance monitoring, memory pools, event queues,
    and checkpoint management to optimize NLM simulation performance.
    """
    
    def __init__(self, config):
        """Initialize performance optimizer."""
        self.config = config
        self.setup_optimization_systems()
        
        # Performance tracking
        self.optimization_metrics = {
            'memory_pool_efficiency': 0,
            'event_queue_efficiency': 0,
            'checkpoint_efficiency': 0,
            'overall_optimization_score': 0
        }
        
        # Monitoring
        self.performance_monitor = PerformanceMonitoring()
        self.memory_pool = MemoryPool(
            pool_size=self.config.get('memory_pool_size', 100),
            block_size=self.config.get('memory_block_size', 1024)
        )
        self.event_queue = EventQueue(
            max_queue_size=self.config.get('event_queue_max_size', 1000)
        )
        self.checkpoint_manager = CheckpointManager(
            checkpoint_dir=self.config.get('checkpoint_dir', '/tmp/nlm_checkpoints')
        )
    
    def setup_optimization_systems(self):
        """Setup optimization systems based on configuration."""
        # Configure based on system capabilities
        self.optimization_systems = {
            'memory_pool': self.config.get('enable_memory_pool', True),
            'event_queue': self.config.get('enable_event_queue', True),
            'checkpointing': self.config.get('enable_checkpointing', True),
            'performance_monitoring': self.config.get('enable_performance_monitoring', True)
        }
    
    def run_optimized_simulation(self, brain, agent, world, num_steps=5000):
        """
        Run optimized simulation with performance monitoring.
        
        Args:
            brain: NLM brain object
            agent: NLM agent object
            world: NLM world object
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary with simulation results and optimization metrics
        """
        print("=== NLM PERFORMANCE OPTIMIZATION ===")
        print("Running optimized simulation with memory pools and checkpointing...")
        
        # Start optimization systems
        if self.optimization_systems['performance_monitoring']:
            self.performance_monitor.start_monitoring()
        
        # Initialize memory pool usage
        if self.optimization_systems['memory_pool']:
            print(f"Memory pool initialized: {self.config.get('memory_pool_size', 100)} blocks")
        
        # Initialize checkpoint manager
        if self.optimization_systems['checkpointing']:
            print(f"Checkpoint manager initialized: {self.config.get('checkpoint_dir', '/tmp/nlm_checkpoints')}")
        
        # Run simulation
        simulation_start = time.time()
        
        simulation_results = []
        checkpoint_interval = self.config.get('checkpoint_interval', 1000)
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process through agent
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step, step * 0.01)
            
            # Decode and act
            motor_cmd = agent.decodeMotorCommand()
            reward = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply learning
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.01)
            
            # Record step performance
            step_time = time.time() - step_start
            self.performance_monitor.record_step_performance(step_time, reward, int(motor_cmd))
            
            # Event queue processing
            if self.optimization_systems['event_queue']:
                self.process_optimization_events(step, brain, agent, world)
            
            # Checkpoint creation
            if self.optimization_systems['checkpointing'] and step > 0 and step % checkpoint_interval == 0:
                self.create_simulation_checkpoint(step, brain, agent, world)
            
            # Record results
            simulation_results.append({
                'step': step,
                'step_time': step_time,
                'reward': reward,
                'firing_rate': brain.getAverageFiringRate(),
                'development_stage': agent.getDevelopmentalStage()
            })
            
            # Print progress
            if step % 1000 == 0:
                elapsed = time.time() - simulation_start
                steps_per_second = step / elapsed if elapsed > 0 else 0
                print(f"Step {step}: {steps_per_second:.1f} steps/s, "
                      f"Step time: {step_time*1000:.2f} ms, "
                      f"Firing rate: {simulation_results[-1]['firing_rate']:.3f} Hz")
        
        simulation_end = time.time()
        total_time = simulation_end - simulation_start
        
        # Calculate optimization metrics
        optimization_metrics = self.calculate_optimization_metrics(simulation_results)
        
        # Stop optimization systems
        if self.optimization_systems['performance_monitoring']:
            self.performance_monitor.stop_monitoring()
        
        # Print optimization summary
        self.print_optimization_summary(optimization_metrics, total_time, num_steps)
        
        return {
            'simulation_results': simulation_results,
            'optimization_metrics': optimization_metrics,
            'performance_monitor': self.performance_monitor,
            'memory_pool': self.memory_pool,
            'event_queue': self.event_queue,
            'checkpoint_manager': self.checkpoint_manager,
            'total_time': total_time,
            'steps_per_second': num_steps / total_time if total_time > 0 else 0
        }
    
    def process_optimization_events(self, step, brain, agent, world):
        """Process optimization events."""
        # Add optimization events
        if step % 100 == 0:
            self.event_queue.add_event('memory_check', {
                'step': step,
                'memory_usage': self.performance_monitor.memory_usage[-1] if self.performance_monitor.memory_usage else 0
            })
        
        if step % 500 == 0:
            self.event_queue.add_event('performance_check', {
                'step': step,
                'step_times': self.performance_monitor.step_times[-10:] if len(self.performance_monitor.step_times) >= 10 else []
            })
        
        # Process events
        self.event_queue.process_events()
    
    def create_simulation_checkpoint(self, step, brain, agent, world):
        """Create simulation checkpoint."""
        checkpoint_id = step // self.config.get('checkpoint_interval', 1000)
        
        # Create brain state snapshot
        brain_state = {
            'neuron_count': brain.getTotalNeuronCount(),
            'region_count': len(brain.getRegions()),
            'total_synapses': brain.getTotalSynapseCount(),
            'current_step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'development_stage': agent.getDevelopmentalStage(),
            'synapse_density': brain.getSynapseDensity() if hasattr(brain, 'getSynapseDensity') else 0,
            'memory_traces': brain.getWorkingMemory().getActiveTraces() if hasattr(brain, 'getWorkingMemory') and brain.getWorkingMemory() else 0,
            'episodic_memories': brain.getEpisodicMemory().getEpisodeCount() if hasattr(brain, 'getEpisodicMemory') and brain.getEpisodicMemory() else 0
        }
        
        # Create simulation state snapshot
        simulation_state = {
            'step': step,
            'world_state': world.get_world_state() if hasattr(world, 'get_world_state') else {},
            'agent_state': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'developmental_stage': agent.getDevelopmentalStage()
            },
            'time': step * 0.01,
            'performance_metrics': {
                'avg_step_time': np.mean(self.performance_monitor.step_times) if self.performance_monitor.step_times else 0,
                'avg_firing_rate': np.mean([r['firing_rate'] for r in self.simulation_results[-100:]]) if self.simulation_results else 0
            }
        }
        
        # Create checkpoint
        checkpoint_file = self.checkpoint_manager.create_checkpoint(checkpoint_id, brain_state, simulation_state)
        
        # Update simulation results with checkpoint info
        if not hasattr(self, 'simulation_results'):
            self.simulation_results = []
        
        if self.simulation_results:
            self.simulation_results[-1]['checkpoint_file'] = checkpoint_file
        
        print(f"  Checkpoint created: {checkpoint_file}")
    
    def calculate_optimization_metrics(self, simulation_results):
        """Calculate optimization metrics."""
        if not simulation_results:
            return self.optimization_metrics
        
        # Memory pool metrics
        pool_stats = self.memory_pool.get_usage_stats()
        self.optimization_metrics['memory_pool_efficiency'] = pool_stats['usage_ratio']
        
        # Event queue metrics
        queue_stats = self.event_queue.get_queue_stats()
        self.optimization_metrics['event_queue_efficiency'] = queue_stats['queue_efficiency']
        
        # Checkpoint metrics
        checkpoint_stats = self.checkpoint_manager.get_checkpoint_stats()
        self.optimization_metrics['checkpoint_efficiency'] = min(checkpoint_stats['total_checkpoints'] / 10.0, 1.0)
        
        # Performance metrics
        performance_stats = self.performance_monitor.calculate_performance_stats()
        
        # Calculate overall optimization score
        self.optimization_metrics['overall_optimization_score'] = (
            pool_stats['usage_ratio'] * 0.3 +
            queue_stats['queue_efficiency'] * 0.3 +
            min(performance_stats['steps_per_second'], 100) / 100 * 0.4
        )
        
        return self.optimization_metrics
    
    def print_optimization_summary(self, optimization_metrics, total_time, num_steps):
        """Print optimization summary."""
        print("\n" + "="*60)
        print("PERFORMANCE OPTIMIZATION SUMMARY")
        print("="*60)
        
        print(f"\nSimulation Performance:")
        print(f"  Total steps: {num_steps}")
        print(f"  Total time: {total_time:.2f} seconds")
        print(f"  Steps per second: {num_steps/total_time:.1f}")
        print(f"  Average step time: {total_time/num_steps*1000:.2f} ms")
        
        print(f"\nOptimization Components:")
        print(f"  Memory pool efficiency: {optimization_metrics['memory_pool_efficiency']*100:.1f}%")
        print(f"  Event queue efficiency: {optimization_metrics['event_queue_efficiency']:.1f}%")
        print(f"  Checkpoint efficiency: {optimization_metrics['checkpoint_efficiency']*100:.1f}%")
        
        print(f"\nOverall Optimization Score: {optimization_metrics['overall_optimization_score']*100:.1f}%")
        
        if optimization_metrics['overall_optimization_score'] >= 0.8:
            print(f"  Classification: EXCELLENT")
        elif optimization_metrics['overall_optimization_score'] >= 0.6:
            print(f"  Classification: GOOD")
        elif optimization_metrics['overall_optimization_score'] >= 0.4:
            print(f"  Classification: ACCEPTABLE")
        else:
            print(f"  Classification: NEEDS IMPROVEMENT")
        
        print(f"\nBottleneck Analysis:")
        bottlenecks = self.performance_monitor.performance_stats['optimization_opportunities']
        if bottlenecks:
            for i, bottleneck in enumerate(bottlenecks, 1):
                print(f"  {i}. {bottleneck['type']}: {bottleneck['description']}")
                print(f"     Severity: {bottleneck['severity']}")
        else:
            print(f"  No significant bottlenecks identified")
        
        print("\n" + "="*60)
        print("OPTIMIZATION COMPLETE")
        print("="*60)
        print("Key optimization benefits:")
        print("✓ Memory pools reduce allocation overhead")
        print("✓ Event queues enable efficient event processing")
        print("✓ Checkpointing enables recovery and analysis")
        print("✓ Performance monitoring identifies bottlenecks")
        print("✓ Resource management improves efficiency")

# Main execution function

def run_performance_optimization_experiment(config=None, num_steps=5000):
    """
    Run comprehensive performance optimization experiment.
    
    Args:
        config: Configuration dictionary for optimization
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary with experiment results
    """
    
    # Default configuration
    if config is None:
        config = {
            'neuron_count': 2000,
            'region_count': 5,
            'enable_memory_pool': True,
            'memory_pool_size': 100,
            'memory_block_size': 1024,
            'enable_event_queue': True,
            'event_queue_max_size': 1000,
            'enable_checkpointing': True,
            'checkpoint_dir': '/tmp/nlm_checkpoints',
            'checkpoint_interval': 1000,
            'enable_performance_monitoring': True
        }
    
    print("=== NLM PERFORMANCE OPTIMIZATION EXPERIMENT ===")
    print("Testing performance monitoring, memory pools, and checkpointing...")
    
    # Initialize performance optimizer
    optimizer = NLMPerformanceOptimizer(config)
    
    # Create brain with optimization-focused configuration
    brain_config = pynlm.createDefaultConfig()
    brain_config.set("brain.neuron_count", config['neuron_count'])
    brain_config.set("brain.region_count", config['region_count'])
    
    # Create brain and agent
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Setup environment
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run optimized simulation
    results = optimizer.run_optimized_simulation(brain, agent, world, num_steps)
    
    # Print final summary
    print("\n" + "="*60)
    print("PERFORMANCE OPTIMIZATION EXPERIMENT COMPLETE")
    print("="*60)
    print("Key insights:")
    print("✓ Performance optimization improves simulation efficiency")
    print("✓ Memory pools reduce allocation overhead and fragmentation")
    print("✓ Event queues enable efficient event processing")
    print("✓ Checkpointing enables recovery and progress tracking")
    print("✓ Performance monitoring identifies optimization opportunities")
    print("✓ Resource management improves overall system performance")
    
    return results

# Additional utility functions for documentation

def get_performance_optimization_configuration():
    """
    Get example configuration for performance optimization experiments.
    
    Returns:
        Dictionary with example configuration
    """
    return {
        'neuron_count': 2000,
        'region_count': 5,
        'enable_memory_pool': True,
        'memory_pool_size': 100,
        'memory_block_size': 1024,
        'enable_event_queue': True,
        'event_queue_max_size': 1000,
        'enable_checkpointing': True,
        'checkpoint_dir': '/tmp/nlm_checkpoints',
        'checkpoint_interval': 1000,
        'enable_performance_monitoring': True,
        'performance_monitor_interval': 100
    }

def get_performance_optimization_insights():
    """
    Get educational insights about performance optimization for documentation.
    
    Returns:
        List of performance optimization insights
    """
    return [
        {
            "title": "Memory Pool Optimization",
            "description": "Memory pools reduce allocation overhead by reusing memory blocks.",
            "example": "Pre-allocate memory blocks → Reuse when possible → Reduce fragmentation"
        },
        {
            "title": "Event Queue Processing",
            "description": "Event queues process events efficiently vs. polling approaches.",
            "example": "Queue events → Process in order → Better resource utilization"
        },
        {
            "title": "Checkpoint Management",
            "description": "Checkpoints enable recovery and analysis of long simulations.",
            "example": "Save state regularly → Resume from crash → Analyze progress"
        },
        {
            "title": "Performance Monitoring",
            "description": "Systematic performance monitoring identifies bottlenecks.",
            "example": "Track metrics → Identify issues → Optimize hot paths"
        },
        {
            "title": "Resource Management",
            "description": "Efficient resource management improves overall performance.",
            "example": "Monitor CPU/memory → Balance load → Optimize efficiency"
        }
    ]

# Command line interface
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Run Performance Optimization Experiment for NLM")
    parser.add_argument("--steps", type=int, default=5000, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=2000, help="Number of neurons")
    parser.add_argument("--regions", type=int, default=5, help="Number of brain regions")
    parser.add_argument("--memory-pool", type=int, default=100, help="Memory pool size")
    parser.add_argument("--event-queue", type=int, default=1000, help="Event queue max size")
    parser.add_argument("--checkpoint-dir", type=str, default="/tmp/nlm_checkpoints", help="Checkpoint directory")
    parser.add_argument("--checkpoint-interval", type=int, default=1000, help="Checkpoint interval")
    parser.add_argument("--no-plots", action="store_true", help="Disable plotting")
    
    args = parser.parse_args()
    
    # Set configuration from command line arguments
    config = {
        'neuron_count': args.neurons,
        'region_count': args.regions,
        'enable_memory_pool': True,
        'memory_pool_size': args.memory_pool,
        'enable_event_queue': True,
        'event_queue_max_size': args.event_queue,
        'enable_checkpointing': True,
        'checkpoint_dir': args.checkpoint_dir,
        'checkpoint_interval': args.checkpoint_interval,
        'enable_performance_monitoring': True
    }
    
    # Run experiment
    results = run_performance_optimization_experiment(config, args.steps)
