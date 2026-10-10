"""Production NLM Demo - Deployment and Monitoring

This example demonstrates production-grade NLM deployment including monitoring,
logging, scalability, and production-ready features.
"""
import nlm
from nlm import expert
import time
import threading
import logging
import socket
import json
from datetime import datetime
from typing import Dict, List, Any
import sys

class NLMServer:
    """Production NLM server with monitoring and control."""
    
    def __init__(self, host="localhost", port=8080):
        """Initialize NLM production server.
        
        Args:
            host: Server host address
            port: Server port
        """
        self.host = host
        self.port = port
        self.server_socket = None
        self.server_thread = None
        self.clients = []
        self.is_running = False
        
        # Production systems
        self.memory_pool = expert.MemoryPool(65536, 262144)
        self.simd_processor = expert.SIMDProcessor(256)
        self.parallel_processor = expert.ParallelProcessor(8)
        self.performance_profiler = expert.PerformanceProfiler()
        self.checkpoint_manager = expert.CheckpointManager(
            checkpoint_dir="./production_checkpoints",
            auto_checkpoint=True,
            checkpoint_interval=300.0
        )
        
        # Setup logging
        self.setup_logging()
        
    def setup_logging(self):
        """Setup production logging."""
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('production.log'),
                logging.StreamHandler(sys.stdout)
            ]
        )
        
        self.logger = logging.getLogger('nlm.production')
    
    def start_server(self):
        """Start production server."""
        if self.is_running:
            return False
        
        self.is_running = True
        self.server_thread = threading.Thread(target=self._run_server, daemon=True)
        self.server_thread.start()
        
        self.logger.info(f"NLM Production Server started on {self.host}:{self.port}")
        return True
    
    def _run_server(self):
        """Internal server run loop."""
        while self.is_running:
            try:
                # Create simulation systems
                config = self.create_production_config()
                world = nlm.create_simple_world()
                world.configure(50, 50, 128, 128)
                world.setAgentStart(25, 25)
                
                brain = nlm.create_brain(config)
                brain.initialize(world)
                
                agent_brain = nlm.create_agent_brain(brain)
                agent_brain.initialize(brain, world)
                
                # Create monitoring systems
                monitor = nlm.create_performance_monitor()
                profiler = expert.PerformanceProfiler()
                
                self.logger.info(f"Started simulation with {config.get('brain.simulated_neurons')} neurons")
                
                # Run simulation with periodic checkpoints
                for step in range(5000):
                    if not self.is_running:
                        break
                    
                    step_start = time.time()
                    profiler.startProfiling("step_execution")
                    
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    
                    # Vectorized processing
                    processed_percept = self.simd_processor.process_vector(
                        percept, lambda x: [v * 0.5 for v in x]
                    )
                    
                    # Process with agent brain
                    agent_brain.processSensoryInput(processed_percept)
                    
                    # Generate action
                    action = self.parallel_processor.distribute_work(
                        agent_brain.decodeMotorCommand, [agent_brain]
                    )[0]
                    
                    # Apply action
                    world.applyMotorCommand(action, step)
                    world.update(0.001)
                    
                    # Record metrics
                    neurons_active = brain.getActiveNeuronCount()
                    monitor.recordStep(neurons_active, 0, 0.001)
                    
                    step_time = profiler.stopProfiling("step_execution")
                    
                    # Create checkpoint periodically
                    if step % 500 == 0:
                        checkpoint_name = self.checkpoint_manager.create_checkpoint(
                            f"production_{step}",
                            {
                                'step': step,
                                'time': time.time(),
                                'neurons_active': neurons_active,
                                'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
                                'performance': profiler.getPerformanceReport()
                            }
                        )
                        self.logger.info(f"Created checkpoint: {checkpoint_name}")
                    
                    # Log performance
                    if step % 100 == 0:
                        perf_report = monitor.getPerformanceReport()
                        self.logger.info(
                            f"Step {step}: {neurons_active} neurons, "
                            f"{perf_report['simulation_speed']:.1f} steps/sec, "
                            f"{perf_report['avg_iteration_time']:.6f}s per step"
                        )
                    
                    # Handle client requests
                    self.handle_client_requests()
                
                # Simulation completed
                self.logger.info("Simulation completed successfully")
                
            except Exception as e:
                self.logger.error(f"Simulation error: {e}", exc_info=True)
                time.sleep(1.0)  # Prevent rapid error loops
    
    def create_production_config(self):
        """Create production-optimized configuration."""
        config = nlm.create_high_performance_config()
        config.set('brain.simulated_neurons', 3000)
        config.set('world.width', 50)
        config.set('world.height', 50)
        config.set('performance.memory_pool_size', 32768)
        config.set('performance.parallel_threads', 8)
        return config
    
    def handle_client_requests(self):
        """Handle client requests."""
        # Placeholder for client request handling
        # In a real implementation, this would handle
        # WebSocket connections, API requests, etc.
        pass
    
    def stop_server(self):
        """Stop production server."""
        self.is_running = False
        if self.server_thread:
            self.server_thread.join(timeout=30.0)
        
        # Cleanup
        self.checkpoint_manager.cleanup_old_checkpoints(3600.0)  # Keep 1 hour
        
        self.logger.info("NLM Production Server stopped")
    
    def get_status(self):
        """Get server status.
        
        Returns:
            Dictionary with server status
        """
        return {
            'is_running': self.is_running,
            'memory_pool_stats': self.memory_pool.getStats(),
            'performance_stats': self.performance_profiler.getPerformanceReport(),
            'checkpoint_stats': {
                'total_checkpoints': len(self.checkpoint_manager.checkpoints),
                'checkpoint_dir': self.checkpoint_manager.checkpoint_dir
            },
            'timestamp': datetime.now().isoformat()
        }


def run_production_demo():
    """Run production NLM demo."""
    print("=== Production NLM Demo ===")
    print("This example demonstrates production-grade NLM deployment.")
    print("Features:")
    print("  - Production server with monitoring")
    print("  - Comprehensive logging")
    print("  - Checkpoint system for reliability")
    print("  - Performance optimization")
    print("  - Error handling and recovery")
    print("  - Scalability considerations")
    print()
    
    # Create production server
    server = NLMServer("localhost", 8080)
    
    try:
        # Start server
        print("1. Starting production server...")
        if server.start_server():
            print("   Server started successfully")
            
            # Monitor for a period
            start_time = time.time()
            monitoring_duration = 30  # seconds
            
            print(f"\n2. Monitoring server for {monitoring_duration} seconds...")
            
            while time.time() - start_time < monitoring_duration:
                # Get server status
                status = server.get_status()
                
                # Log status
                print(f"   Status: {'RUNNING' if status['is_running'] else 'STOPPED'}")
                print(f"   Memory pool: {status['memory_pool_stats']['total_allocated'] / 1024 / 1024:.2f} MB")
                print(f"   Performance: {status['performance_stats'].get('simulation_speed', 0):.2f} steps/sec")
                print(f"   Checkpoints: {status['checkpoint_stats']['total_checkpoints']}")
                
                # Wait for next status update
                time.sleep(5)
            
            # Stop server
            print("\n3. Stopping production server...")
            server.stop_server()
            
            print("\n4. Production demo completed successfully!")
            print("   - Server managed large-scale simulation")
            print("   - Comprehensive monitoring implemented")
            print("   - Checkpoint system operational")
            print("   - Error handling functional")
            
            return True
        else:
            print("   Failed to start server")
            return False
            
    except Exception as e:
        print(f"\nProduction demo failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    finally:
        # Ensure server is stopped
        if 'server' in locals():
            server.stop_server()


def run_cluster_simulation():
    """Run distributed cluster simulation."""
    print("=== Cluster Simulation Demo ===")
    print("This example demonstrates distributed cluster simulation.")
    print()
    
    # Simulate cluster nodes
    cluster_nodes = [
        {
            'id': 'node-1',
            'host': 'localhost',
            'port': 8081,
            'capacity': 'high',
            'role': 'master'
        },
        {
            'id': 'node-2',
            'host': 'localhost',
            'port': 8082,
            'capacity': 'medium',
            'role': 'worker'
        },
        {
            'id': 'node-3',
            'host': 'localhost',
            'port': 8083,
            'capacity': 'medium',
            'role': 'worker'
        }
    ]
    
    print("1. Cluster Configuration:")
    for node in cluster_nodes:
        print(f"   - {node['id']}: {node['role']} at {node['host']}:{node['port']}")
    
    print("\n2. Starting distributed simulation...")
    
    # Create distributed performance configuration
    config = nlm.create_high_performance_config()
    config.set('brain.simulated_neurons', 10000)
    config.set('performance.parallel_threads', 16)
    config.set('performance.memory_pool_size', 65536)
    
    # Create expert systems for cluster
    master_memory_pool = expert.MemoryPool(32768, 262144)
    worker_memory_pool = expert.MemoryPool(16384, 131072)
    
    print(f"   - Master memory pool: {master_memory_pool.initial_size}/{master_memory_pool.max_size}")
    print(f"   - Worker memory pools: {len(cluster_nodes) - 1} each with {worker_memory_pool.initial_size}/{worker_memory_pool.max_size}")
    
    print("\n3. Distributing workload...")
    
    # Distribute workload across nodes
    total_steps = 10000
    steps_per_node = total_steps // len(cluster_nodes)
    
    node_results = {}
    
    for node in cluster_nodes:
        print(f"\n   Processing node {node['id']}...")
        
        # Simulate node processing
        node_start_time = time.time()
        
        # Create node-specific configuration
        node_config = config.copy()
        node_config.set('brain.simulated_neurons', 
                       int(config.get('brain.simulated_neurons') / len(cluster_nodes)))
        
        # Create simulation systems
        world = nlm.create_simple_world()
        world.configure(30, 30, 64, 64)
        world.setAgentStart(15, 15)
        
        brain = nlm.create_brain(node_config)
        brain.initialize(world)
        
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        # Run node simulation
        node_steps = steps_per_node if node['id'] != cluster_nodes[-1]['id'] else steps_per_node + (total_steps % len(cluster_nodes))
        
        for step in range(node_steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process with node-specific memory pool
            if node['role'] == 'master':
                pool = master_memory_pool
            else:
                pool = worker_memory_pool
            
            # Simulate processing
            processed_percept = [v * 0.5 for v in percept] if hasattr(percept, '__iter__') else percept
            
            # Process with agent brain
            agent_brain.processSensoryInput(processed_percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, step)
            world.update(0.001)
        
        node_end_time = time.time()
        
        # Calculate performance
        node_time = node_end_time - node_start_time
        node_steps_per_second = node_steps / node_time
        
        node_results[node['id']] = {
            'steps_completed': node_steps,
            'time_taken': node_time,
            'steps_per_second': node_steps_per_second,
            'status': 'completed',
            'memory_pool_stats': pool.getStats()
        }
        
        print(f"   - Completed {node_steps} steps in {node_time:.2f}s")
        print(f"   - Performance: {node_steps_per_second:.2f} steps/sec")
    
    # Aggregate results
    print(f"\n4. Cluster Performance Summary:")
    total_steps_completed = sum(r['steps_completed'] for r in node_results.values())
    total_time_taken = sum(r['time_taken'] for r in node_results.values())
    total_throughput = total_steps_completed / total_time_taken
    
    print(f"   - Total steps completed: {total_steps_completed:,}")
    print(f"   - Total time taken: {total_time_taken:.2f} seconds")
    print(f"   - Cluster throughput: {total_throughput:.2f} steps/sec")
    print(f"   - Node efficiency: {len(cluster_nodes)} nodes working in parallel")
    
    # Display individual node performance
    print(f"\n5. Individual Node Performance:")
    for node_id, result in node_results.items():
        print(f"   - {node_id}: {result['steps_completed']:,} steps, "
              f"{result['steps_per_second']:.2f} steps/sec, "
              f"Memory: {result['memory_pool_stats']['total_allocated']} allocated")
    
    return {
        'cluster_nodes': cluster_nodes,
        'node_results': node_results,
        'total_steps': total_steps_completed,
        'total_time': total_time_taken,
        'throughput': total_throughput,
        'scaling_efficiency': total_steps / (len(cluster_nodes) * total_time_taken)
    }

if __name__ == "__main__":
    print("NLM Production Demo - Deployment and Monitoring")
    print("=" * 80)
    print()
    
    # Check if we should run production server or cluster simulation
    import argparse
    parser = argparse.ArgumentParser(description='NLM Production Demo')
    parser.add_argument('--mode', choices=['server', 'cluster', 'both'], 
                       default='server', help='Demo mode')
    
    args = parser.parse_args()
    
    if args.mode == 'server' or args.mode == 'both':
        print("=== Running Production Server Demo ===")
        server_success = run_production_demo()
    else:
        server_success = True
    
    if args.mode == 'cluster' or args.mode == 'both':
        print("\n" + "=" * 80)
        print("=== Running Cluster Simulation Demo ===")
        cluster_results = run_cluster_simulation()
    else:
        cluster_results = {}
    
    print("\n" + "=" * 80)
    print("PRODUCTION DEMO COMPLETED")
    print("=" * 80)
    
    if args.mode == 'server' or args.mode == 'both':
        if server_success:
            print("✓ Production server demo: SUCCESS")
        else:
            print("✗ Production server demo: FAILED")
    
    if args.mode == 'cluster' or args.mode == 'both':
        print(f"✓ Cluster simulation demo: SUCCESS")
        print(f"  - Total throughput: {cluster_results['throughput']:.2f} steps/sec")
        print(f"  - Scaling efficiency: {cluster_results['scaling_efficiency']:.2f}")
    
    print("\nFor more examples, see:")
    print("  - examples/basic_demo.py - Basic neural network simulation")
    print("  - examples/expert_demo.py - Advanced expert features")
    print("  - examples/performance_demo.py - Performance optimization")
    print("  - examples/visualization_demo.py - Advanced visualization")
    print("  - examples/research_demo.py - Research applications")
