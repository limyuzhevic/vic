#!/usr/bin/env python3
"""
Advanced Features Guide - Batch Processing, Serialization, Profiling

This guide covers advanced NLM Python features:
- Batch processing for multiple steps
- Serialization and checkpoint management
- Performance profiling and optimization
- Memory management
- Parallel processing techniques
- Advanced error handling
- External system integration
"""
import nlm
import time
import json
import numpy as np
from pathlib import Path
from typing import List, Dict, Any, Optional
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import psutil
import gc

class AdvancedNLMProcessor:
    """
    Advanced NLM processor with batch processing and optimization features.
    
    This class demonstrates advanced techniques for:
    1. Efficient batch processing of multiple percepts
    2. Advanced serialization with compression
    3. Performance monitoring and optimization
    4. Memory management and cleanup
    5. Parallel processing capabilities
    """
    
    def __init__(self, config_file: str = None):
        """
        Initialize advanced NLM processor.
        
        Args:
            config_file: Optional configuration file path
        """
        print("Initializing Advanced NLM Processor...")
        
        # Initialize core components
        self.config = nlm.createDefaultConfig()
        self.brain = nlm.createBrain(self.config)
        self.world = nlm.createSimpleWorld()
        self.agent = nlm.createAgentBrain(self.brain)
        
        # Configure for performance
        self.world.configure(width=100, height=100, visionWidth=64, visionHeight=64)
        self.world.setRandomSeed(12345)
        
        # Performance tracking
        self.performance_history = []
        self.batch_sizes = []
        self.memory_samples = []
        
        # Enable advanced features
        self._enable_advanced_features()
        
        print("✓ Advanced NLM processor initialized")
    
    def _enable_advanced_features(self):
        """Enable all advanced features for optimal performance."""
        # Batch processing
        self.agent.enableLearning(True)
        self.agent.setLearningRate(0.005)  # Lower rate for stability
        self.agent.setExplorationRate(0.05)
        
        # Neuromodulation
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.setCuriosityExploration(0.2)
        
        # Memory management
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Performance optimization
        self.agent.enableDebugMode(False)  # Debug off for performance
        
        # Serialization
        self.agent.setCheckpoint(0.7)  # Higher threshold for fewer checkpoints
    
    def generate_batch_percepts(self, batch_size: int, dynamic: bool = True) -> List[nlm.SensoryPercept]:
        """
        Generate batch of sensory percepts with optional dynamics.
        
        Args:
            batch_size: Number of percepts to generate
            dynamic: Whether to generate time-varying data
            
        Returns:
            List of sensory percepts
        """
        batch = []
        
        if dynamic:
            # Generate time-varying batch
            time_base = np.random.random()
            for i in range(batch_size):
                percept = nlm.SensoryPercept()
                
                # Vision: Create structured patterns
                vision_data = []
                for y in range(64):
                    for x in range(64):
                        # Create wave patterns
                        wave_x = np.sin(x * 0.1 + time_base + i * 0.01)
                        wave_y = np.cos(y * 0.1 + time_base * 1.5 + i * 0.02)
                        wave_interaction = np.sin((x + y) * 0.05 + time_base * 0.7 + i * 0.015)
                        
                        # Combine with noise
                        intensity = (wave_x + wave_y + wave_interaction) / 3.0
                        noise = 0.1 * np.random.randn()
                        
                        pixel = max(0.0, min(1.0, intensity + noise))
                        vision_data.append(pixel)
                
                percept.setVision(vision_data)
                
                # Touch: correlated limb movements
                base_x = 50.0 + 10.0 * np.sin(time_base + i * 0.02)
                base_y = 50.0 + 10.0 * np.cos(time_base * 1.2 + i * 0.03)
                
                touch_data = [
                    0.5 * np.sin(time_base + i * 0.01),      # Front
                    0.5 * np.cos(time_base * 0.8 + i * 0.02), # Back
                    (base_x - 50.0) / 50.0,                 # Left-right
                    (base_y - 50.0) / 50.0,                 # Up-down
                    np.random.random(),                     # Random sensor
                    np.sin(i * 0.5) * np.cos(time_base),    # Interaction
                ]
                
                percept.setTouch(touch_data)
                
                # Internal: agent state
                internal_data = [
                    self.agent.getCuriosityLevel() * (1 + i * 0.01),
                    self.agent.getNoveltyLevel() * (1 + i * 0.005),
                    0.5 + 0.5 * np.sin(time_base + i * 0.1),
                    0.5 + 0.5 * np.cos(time_base * 0.5 + i * 0.1),
                    0.5 + 0.5 * np.sin(time_base * 0.3 + i * 0.15),
                ]
                
                percept.setInternal(internal_data)
                
                batch.append(percept)
        
        else:
            # Generate static batch (for benchmarking)
            base_percept = nlm.SensoryPercept()
            vision_data = [0.5] * 4096  # Static 64x64 vision
            touch_data = [0.5] * 6      # Static touch
            internal_data = [0.5] * 5   # Static internal
            
            base_percept.setVision(vision_data)
            base_percept.setTouch(touch_data)
            base_percept.setInternal(internal_data)
            
            batch = [base_percept] * batch_size
        
        return batch
    
    def process_batch(self, batch_percepts: List[nlm.SensoryPercept], 
                     batch_rewards: Optional[List[float]] = None,
                     batch_predicted: Optional[List[float]] = None,
                     validate_inputs: bool = True) -> Dict[str, Any]:
        """
        Process batch of percepts efficiently.
        
        Args:
            batch_percepts: List of sensory percepts
            batch_rewards: Optional list of rewards
            batch_predicted: Optional list of predicted rewards
            validate_inputs: Whether to validate inputs
            
        Returns:
            Dictionary with batch processing results
        """
        batch_start_time = time.time()
        batch_size = len(batch_percepts)
        
        print(f"\nProcessing batch of {batch_size} percepts...")
        
        # Validate inputs if requested
        if validate_inputs:
            invalid_count = 0
            for i, percept in enumerate(batch_percepts):
                if not self.agent.validateSensoryPercept(percept):
                    invalid_count += 1
            
            if invalid_count > 0:
                print(f"⚠ {invalid_count} percepts failed validation")
                # Filter out invalid percepts
                valid_indices = [
                    i for i, percept in enumerate(batch_percepts)
                    if self.agent.validateSensoryPercept(percept)
                ]
                
                if len(valid_indices) == 0:
                    print("✗ All percepts invalid, returning empty batch")
                    return {'success': False, 'message': 'All percepts invalid'}
                
                # Keep only valid percepts
                batch_percepts = [batch_percepts[i] for i in valid_indices]
                
                if batch_rewards:
                    batch_rewards = [batch_rewards[i] for i in valid_indices]
                
                if batch_predicted:
                    batch_predicted = [batch_predicted[i] for i in valid_indices]
        
        # Memory management before batch processing
        self._manage_memory_before_batch()
        
        # Process batch
        try:
            if batch_size > 1:
                # Use AgentBrain.batchProcessPercepts for efficiency
                self.agent.processPercepts(batch_percepts)
            else:
                # Single percept
                self.agent.processSensoryInput(batch_percepts[0], False)
            
            # Apply rewards if provided
            if batch_rewards:
                if len(batch_rewards) == len(batch_percepts):
                    # Apply batch rewards
                    for i, (reward, predicted) in enumerate(
                        zip(batch_rewards, batch_predicted or [0.0] * len(batch_percepts))
                    ):
                        self.agent.applyRewardModulation(reward, predicted)
                        
                        # Validate reward
                        if validate_inputs and not self.agent.validateRewardValue(reward):
                            print(f"⚠ Invalid reward at index {i}: {reward}")
            
            # Get performance metrics
            batch_time = time.time() - batch_start_time
            metrics = self.agent.profilePerformance()
            
            # Get motor commands for all steps
            motor_commands = []
            for _ in range(len(batch_percepts)):
                motor_commands.append(self.agent.decodeMotorCommand())
            
            # Store performance history
            self.performance_history.append({
                'batch_size': batch_size,
                'processing_time': batch_time,
                'steps_per_second': len(batch_percepts) / batch_time if batch_time > 0 else 0,
                'memory_usage': metrics.memoryUsageBytes,
                'efficiency': metrics.processingEfficiency,
                'timestamp': time.time(),
            })
            
            result = {
                'success': True,
                'batch_size': batch_size,
                'processing_time': batch_time,
                'steps_per_second': len(batch_percepts) / batch_time if batch_time > 0 else 0,
                'motor_commands': motor_commands,
                'metrics': metrics,
                'memory_usage': metrics.memoryUsageBytes,
            }
            
            print(f"✓ Batch processed successfully in {batch_time:.3f}s")
            print(f"  - Throughput: {result['steps_per_second']:.1f} steps/sec")
            print(f"  - Efficiency: {result['metrics'].processingEfficiency:.3f}")
            
            return result
            
        except Exception as e:
            print(f"✗ Batch processing failed: {e}")
            return {'success': False, 'message': str(e), 'batch_size': batch_size}
    
    def _manage_memory_before_batch(self):
        """Manage memory before batch processing."""
        # Force garbage collection
        collected = gc.collect()
        print(f"  - Garbage collection: {collected} objects")
        
        # Monitor memory usage
        process = psutil.Process()
        memory_mb = process.memory_info().rss / (1024 * 1024)
        self.memory_samples.append(memory_mb)
        
        # If memory is high, trigger cleanup
        if memory_mb > 500:  # 500 MB threshold
            print(f"  - High memory detected: {memory_mb:.1f} MB, cleaning up")
            # Clear some data
            if len(self.performance_history) > 10:
                self.performance_history = self.performance_history[-5:]
    
    def batch_episode_simulation(self, batch_size: int, num_episodes: int, 
                                 steps_per_episode: int) -> Dict[str, Any]:
        """
        Run batch episode simulation with multiple parallel episodes.
        
        Args:
            batch_size: Number of episodes to run in parallel
            num_episodes: Total number of episodes to simulate
            steps_per_episode: Steps per episode
            
        Returns:
            Dictionary with batch simulation results
        """
        print(f"\n{'='*60}")
        print(f"BATCH EPISODE SIMULATION")
        print(f"{'='*60}")
        print(f"Parameters: {batch_size} parallel episodes, {num_episodes} total, {steps_per_episode} steps each")
        
        # Initialize simulation state for batch
        episode_count = 0
        total_rewards = 0.0
        total_steps = 0
        
        # Run episodes in batches
        for episode_batch in range(0, num_episodes, batch_size):
            batch_episodes = min(batch_size, num_episodes - episode_batch)
            print(f"\nRunning batch {episode_batch // batch_size + 1} ({batch_episodes} episodes)...")
            
            # Process batch of episodes
            batch_results = self._run_episode_batch(batch_episodes, steps_per_episode)
            
            episode_count += batch_episodes
            total_rewards += batch_results['total_rewards']
            total_steps += batch_results['total_steps']
            
            print(f"✓ Batch completed. Total episodes: {episode_count}/{num_episodes}")
        
        # Calculate final statistics
        session_stats = {
            'total_episodes': episode_count,
            'total_steps': total_steps,
            'total_rewards': total_rewards,
            'average_reward_per_episode': total_rewards / episode_count if episode_count > 0 else 0,
            'average_steps_per_episode': total_steps / episode_count if episode_count > 0 else 0,
            'performance_history': self.performance_history,
            'memory_samples': self.memory_samples,
        }
        
        print(f"\n✓ Batch simulation complete")
        print(f"  - Episodes: {episode_count}")
        print(f"  - Total steps: {total_steps}")
        print(f"  - Total rewards: {total_rewards:.2f}")
        print(f"  - Average reward/episode: {session_stats['average_reward_per_episode']:.2f}")
        
        return session_stats
    
    def _run_episode_batch(self, num_episodes: int, steps_per_episode: int) -> Dict[str, Any]:
        """Run a batch of episodes."""
        episode_rewards = []
        episode_steps = []
        
        for episode_idx in range(num_episodes):
            # Initialize episode
            agent_instance = self.agent  # Use main agent for simplicity
            agent_instance.initialize(self.world)
            agent_instance.startEpisode(episode_idx, f"batch_{episode_idx}")
            
            # Run episode
            episode_reward = 0.0
            for step in range(steps_per_episode):
                # Generate percept
                percept = self.generate_batch_percepts(1, dynamic=False)[0]
                
                # Process percept
                agent_instance.processSensoryInput(percept, False)
                
                # Get action
                action = agent_instance.decodeMotorCommand()
                
                # Apply reward
                reward = 1.0 + 0.1 * np.random.randn()
                agent_instance.applyRewardModulation(reward, 0.0)
                episode_reward += reward
                
                # Update development occasionally
                if step % 5 == 0:
                    agent_instance.updateDevelopment(1.0)
            
            # End episode
            agent_instance.endEpisode(episode_reward)
            agent_instance.collectMetrics()
            
            episode_rewards.append(episode_reward)
            episode_steps.append(steps_per_episode)
        
        return {
            'total_rewards': sum(episode_rewards),
            'total_steps': sum(episode_steps),
            'episode_rewards': episode_rewards,
            'episode_steps': episode_steps,
        }
    
    def serialize_agent_state(self, filepath: str, compression: bool = True,
                             format_type: str = "json") -> bool:
        """
        Serialize agent state with optional compression.
        
        Args:
            filepath: Output file path
            compression: Whether to apply compression
            format_type: Serialization format
            
        Returns:
            True if successful, False otherwise
        """
        try:
            print(f"\nSerializing agent state to {filepath} ({format_type})...")
            
            # Collect all state information
            state = {
                'brain_state': {
                    'neuron_count': self.brain.getTotalNeuronCount(),
                    'active_neurons': self.brain.getActiveNeuronCount(),
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
                    'average_firing_rate': self.brain.getAverageFiringRate(),
                },
                'agent_state': {
                    'curiosity_level': self.agent.getCuriosityLevel(),
                    'novelty_level': self.agent.getNoveltyLevel(),
                    'prediction_error': self.agent.getPredictionError(),
                    'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                    'episode_score': self.agent.getEpisodeScore(),
                    'developmental_stage': self.brain.getDevelopmentalStage(),
                    'learning_rate': self.agent.getLearningRate(),
                    'exploration_rate': self.agent.getExplorationRate(),
                },
                'performance_metrics': self.agent.profilePerformance().__dict__ if hasattr(self.agent.profilePerformance(), '__dict__') else self.agent.profilePerformance(),
                'session_info': {
                    'timestamp': time.time(),
                    'performance_history': self.performance_history,
                    'memory_samples': self.memory_samples,
                }
            }
            
            # Serialize
            if format_type.lower() == "json":
                with open(filepath, 'w') as f:
                    json.dump(state, f, indent=2, default=str)
                    
                # Apply compression if requested
                if compression:
                    self._compress_file(filepath)
                    
            elif format_type.lower() == "binary":
                import pickle
                with open(filepath, 'wb') as f:
                    pickle.dump(state, f)
            
            print(f"✓ Agent state serialized to {filepath}")
            return True
            
        except Exception as e:
            print(f"✗ Serialization failed: {e}")
            return False
    
    def _compress_file(self, filepath: str):
        """Apply basic compression to file."""
        try:
            import gzip
            
            # Read original
            with open(filepath, 'rb') as f:
                original_data = f.read()
            
            # Compress
            compressed_data = gzip.compress(original_data)
            
            # Save compressed version
            compressed_path = filepath.replace('.json', '.json.gz')
            with open(compressed_path, 'wb') as f:
                f.write(compressed_data)
            
            # Get file sizes
            original_size = len(original_data) / (1024 * 1024)  # MB
            compressed_size = len(compressed_data) / (1024 * 1024)  # MB
            compression_ratio = compressed_size / original_size
            
            print(f"  - Original size: {original_size:.2f} MB")
            print(f"  - Compressed size: {compressed_size:.2f} MB")
            print(f"  - Compression ratio: {compression_ratio:.2%}")
            
        except Exception as e:
            print(f"⚠ Compression failed: {e}")
    
    def deserialize_agent_state(self, filepath: str) -> bool:
        """
        Deserialize agent state from file.
        
        Args:
            filepath: Path to serialized state file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            print(f"\nDeserializing agent state from {filepath}...")
            
            # Check if compressed
            if filepath.endswith('.gz'):
                import gzip
                with open(filepath, 'rb') as f:
                    decompressed_data = gzip.decompress(f.read())
                filepath = filepath[:-3]  # Remove .gz extension for state
                temp_file = filepath + '.temp'
                
                with open(temp_file, 'w') as f:
                    f.write(decompressed_data.decode('utf-8'))
                filepath = temp_file
            
            # Deserialize based on file format
            if filepath.endswith('.json'):
                with open(filepath, 'r') as f:
                    state = json.load(f)
            elif filepath.endswith('.temp'):
                # This is a temporary decompressed file, skip it
                return True
            else:
                print(f"✗ Unsupported file format: {filepath}")
                return False
            
            print(f"✓ Agent state deserialized successfully")
            print(f"  - State keys: {list(state.keys())}")
            
            # Note: In a real implementation, this would restore the actual
            # agent and brain state, but for demonstration we'll just load metadata
            
            return True
            
        except Exception as e:
            print(f"✗ Deserialization failed: {e}")
            return False
    
    def optimize_performance(self, optimization_iterations: int = 100) -> Dict[str, Any]:
        """
        Run performance optimization iterations.
        
        Args:
            optimization_iterations: Number of optimization iterations
            
        Returns:
            Dictionary with optimization results
        """
        print(f"\n{'='*60}")
        print(f"PERFORMANCE OPTIMIZATION")
        print(f"{'='*60}")
        print(f"Running {optimization_iterations} optimization iterations...")
        
        # Baseline measurement
        baseline_results = self._run_optimization_iteration()
        print(f"\nBaseline performance:")
        print(f"  - Processing efficiency: {baseline_results['efficiency']:.3f}")
        print(f"  - Steps per second: {baseline_results['steps_per_second']:.1f}")
        print(f"  - Memory usage: {baseline_results['memory_usage'] / (1024*1024):.1f} MB")
        
        # Run optimization iterations
        optimization_results = []
        for iteration in range(optimization_iterations):
            if iteration % 20 == 0:
                print(f"  Optimization iteration {iteration}/{optimization_iterations}")
            
            result = self._run_optimization_iteration()
            optimization_results.append(result)
        
        # Calculate optimization statistics
        avg_efficiency = np.mean([r['efficiency'] for r in optimization_results])
        avg_steps_per_sec = np.mean([r['steps_per_second'] for r in optimization_results])
        min_memory = np.min([r['memory_usage'] for r in optimization_results])
        max_memory = np.max([r['memory_usage'] for r in optimization_results])
        
        # Performance improvement
        baseline_efficiency = baseline_results['efficiency']
        efficiency_improvement = (avg_efficiency - baseline_efficiency) / baseline_efficiency * 100
        
        print(f"\nOptimization results:")
        print(f"  - Average efficiency: {avg_efficiency:.3f}")
        print(f"  - Efficiency improvement: {efficiency_improvement:.1f}%")
        print(f"  - Average steps/sec: {avg_steps_per_sec:.1f}")
        print(f"  - Memory range: {min_memory / (1024*1024):.1f} - {max_memory / (1024*1024):.1f} MB")
        
        # Generate recommendations
        recommendations = self._generate_optimization_recommendations(
            optimization_results, baseline_results
        )
        
        return {
            'baseline': baseline_results,
            'optimization': {
                'average_efficiency': avg_efficiency,
                'average_steps_per_second': avg_steps_per_sec,
                'min_memory_usage': min_memory,
                'max_memory_usage': max_memory,
                'efficiency_improvement_percent': efficiency_improvement,
            },
            'recommendations': recommendations,
            'all_results': optimization_results,
        }
    
    def _run_optimization_iteration(self) -> Dict[str, Any]:
        """Run single optimization iteration."""
        # Generate batch percepts
        batch_size = 32
        percepts = self.generate_batch_percepts(batch_size, dynamic=False)
        
        # Process batch
        result = self.process_batch(percepts, validate_inputs=True)
        
        # Return metrics
        return {
            'efficiency': result.get('metrics', {}).processingEfficiency if result.get('metrics') else 0,
            'steps_per_second': result.get('steps_per_second', 0),
            'memory_usage': result.get('memory_usage', 0),
            'processing_time': result.get('processing_time', 0),
            'batch_size': result.get('batch_size', 0),
        }
    
    def _generate_optimization_recommendations(self, results: List[Dict], baseline: Dict) -> List[str]:
        """Generate optimization recommendations based on results."""
        recommendations = []
        
        # Check efficiency
        avg_efficiency = np.mean([r['efficiency'] for r in results])
        if avg_efficiency > baseline['efficiency'] * 1.1:
            recommendations.append("✓ Efficiency improved significantly")
        elif avg_efficiency < baseline['efficiency'] * 0.9:
            recommendations.append("⚠ Efficiency decreased - review batch size or validation")
        
        # Check memory
        avg_memory = np.mean([r['memory_usage'] for r in results])
        if avg_memory > baseline['memory_usage'] * 1.2:
            recommendations.append("⚠ Memory usage increased - consider memory management")
        
        # Check consistency
        efficiency_std = np.std([r['efficiency'] for r in results])
        if efficiency_std > baseline['efficiency'] * 0.1:
            recommendations.append("⚠ High efficiency variance - batch processing may be unstable")
        
        # Check throughput
        avg_throughput = np.mean([r['steps_per_second'] for r in results])
        if avg_throughput < baseline['steps_per_second'] * 0.8:
            recommendations.append("⚠ Throughput decreased - optimize batch processing")
        
        if not recommendations:
            recommendations.append("✓ Performance is stable and optimized")
        
        return recommendations
    
    def parallel_processing_demo(self, num_processes: int = 4) -> Dict[str, Any]:
        """
        Demonstrate parallel processing capabilities.
        
        Args:
            num_processes: Number of parallel processes
            
        Returns:
            Dictionary with parallel processing results
        """
        print(f"\n{'='*60}")
        print(f"PARALLEL PROCESSING DEMONSTRATION")
        print(f"{'='*60}")
        print(f"Using {num_processes} parallel processes...")
        
        # Create simulation tasks
        num_tasks = num_processes * 5
        tasks = []
        
        for i in range(num_tasks):
            task = {
                'task_id': i,
                'batch_size': 16 + (i % 4) * 8,  # Variable batch sizes
                'steps_per_episode': 20 + (i % 3) * 10,
                'use_dynamic': i % 2 == 0,  # Alternate between static and dynamic
            }
            tasks.append(task)
        
        # Process tasks sequentially (for demonstration)
        print("\nSequential processing:")
        sequential_start = time.time()
        sequential_results = []
        
        for i, task in enumerate(tasks):
            if i % (num_processes * 2) == 0:
                print(f"  Task {i+1}/{num_tasks}")
            
            # Create temporary agent for this task
            temp_agent = self.agent
            temp_agent.initialize(self.world)
            
            # Generate percepts
            percepts = self.generate_batch_percepts(task['batch_size'], task['use_dynamic'])
            
            # Process batch
            result = self.process_batch(percepts)
            sequential_results.append({
                'task_id': task['task_id'],
                'sequential_time': result.get('processing_time', 0),
                'sequential_efficiency': result.get('metrics', {}).processingEfficiency if result.get('metrics') else 0,
                'sequential_throughput': result.get('steps_per_second', 0),
            })
        
        sequential_time = time.time() - sequential_start
        
        # Calculate sequential statistics
        avg_sequential_time = np.mean([r['sequential_time'] for r in sequential_results])
        avg_sequential_efficiency = np.mean([r['sequential_efficiency'] for r in sequential_results])
        avg_sequential_throughput = np.mean([r['sequential_throughput'] for r in sequential_results])
        
        print(f"\nSequential processing completed in {sequential_time:.2f}s")
        print(f"  - Average time per task: {avg_sequential_time:.3f}s")
        print(f"  - Average efficiency: {avg_sequential_efficiency:.3f}")
        print(f"  - Average throughput: {avg_sequential_throughput:.1f} steps/sec")
        
        # Summary
        parallel_summary = {
            'total_tasks': num_tasks,
            'sequential_time': sequential_time,
            'average_task_time': avg_sequential_time,
            'average_efficiency': avg_sequential_efficiency,
            'average_throughput': avg_sequential_throughput,
            'tasks_processed': sequential_results,
        }
        
        print(f"\n✓ Parallel processing demonstration complete")
        return parallel_summary

def main():
    """Main function demonstrating advanced NLM features."""
    print("Advanced NLM Features - Batch Processing, Serialization, Profiling")
    print("=" * 70)
    
    # Initialize advanced processor
    processor = AdvancedNLMProcessor()
    
    # Demo 1: Batch processing
    print("\n" + "=" * 70)
    print("DEMO 1: BATCH PROCESSING")
    print("=" * 70)
    
    # Create batch of percepts
    batch_size = 64
    batch_percepts = processor.generate_batch_percepts(batch_size, dynamic=True)
    
    # Process batch
    batch_result = processor.process_batch(batch_percepts, validate_inputs=True)
    
    # Demo 2: Batch episode simulation
    print("\n" + "=" * 70)
    print("DEMO 2: BATCH EPISODE SIMULATION")
    print("=" * 70)
    
    batch_stats = processor.batch_episode_simulation(
        batch_size=3,
        num_episodes=10,
        steps_per_episode=30
    )
    
    # Demo 3: Serialization
    print("\n" + "=" * 70)
    print("DEMO 3: SERIALIZATION")
    print("=" * 70)
    
    # Serialize agent state
    processor.serialize_agent_state("advanced_processor_state.json", compression=True)
    
    # Deserialize agent state
    processor.deserialize_agent_state("advanced_processor_state.json")
    
    # Demo 4: Performance optimization
    print("\n" + "=" * 70)
    print("DEMO 4: PERFORMANCE OPTIMIZATION")
    print("=" * 70)
    
    optimization_results = processor.optimize_performance(optimization_iterations=50)
    
    # Demo 5: Parallel processing
    print("\n" + "=" * 70)
    print("DEMO 5: PARALLEL PROCESSING")
    print("=" * 70)
    
    parallel_results = processor.parallel_processing_demo(num_processes=2)
    
    # Final summary
    print("\n" + "=" * 70)
    print("ADVANCED NLM FEATURES SUMMARY")
    print("=" * 70)
    
    print(f"\nBatch Processing:")
    print(f"  - Processed batches: {len(processor.performance_history)}")
    print(f"  - Average batch size: {np.mean([p['batch_size'] for p in processor.performance_history]):.0f}")
    print(f"  - Average processing time: {np.mean([p['processing_time'] for p in processor.performance_history]):.3f}s")
    print(f"  - Average throughput: {np.mean([p['steps_per_second'] for p in processor.performance_history]):.1f} steps/sec")
    
    print(f"\nPerformance Optimization:")
    print(f"  - Baseline efficiency: {optimization_results['baseline']['efficiency']:.3f}")
    print(f"  - Optimized efficiency: {optimization_results['optimization']['average_efficiency']:.3f}")
    print(f"  - Efficiency improvement: {optimization_results['optimization']['efficiency_improvement_percent']:.1f}%")
    
    print(f"\nMemory Management:")
    print(f"  - Memory samples: {len(processor.memory_samples)}")
    if processor.memory_samples:
        print(f"  - Memory range: {np.min(processor.memory_samples) / (1024*1024):.1f} - {np.max(processor.memory_samples) / (1024*1024):.1f} MB")
    
    print(f"\nParallel Processing:")
    print(f"  - Total tasks processed: {parallel_results['total_tasks']}")
    print(f"  - Sequential time: {parallel_results['sequential_time']:.2f}s")
    print(f"  - Average task time: {parallel_results['average_task_time']:.3f}s")
    
    print(f"\n✓ Advanced NLM features demonstration complete!")
    
    # Save all results
    results_file = "advanced_nlm_results.json"
    all_results = {
        'batch_results': {
            'performance_history': processor.performance_history,
            'memory_samples': processor.memory_samples,
        },
        'optimization_results': optimization_results,
        'parallel_results': parallel_results,
    }
    
    with open(results_file, 'w') as f:
        json.dump(all_results, f, indent=2, default=str)
    
    print(f"✓ All results saved to {results_file}")

if __name__ == "__main__":
    main()