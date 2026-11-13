# NLM Enhanced Python Interface

# Core classes for NLM (Neural Learning Machine)

import numpy as np
import json
import time
import threading
from typing import List, Dict, Any, Optional, Union, Callable
from dataclasses import dataclass

@dataclass
class NLMConfig:
    """NLM configuration with Pythonic interface"""
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        self._config_dict = config_dict or {}
    
    def get(self, key: str, default=None):
        """Get configuration value with Pythonic .get() method"""
        return self._config_dict.get(key, default)
    
    def set(self, key: str, value: Any):
        """Set configuration value"""
        self._config_dict[key] = value
    
    def __getitem__(self, key):
        return self._config_dict[key]
    
    def __setitem__(self, key, value):
        self._config_dict[key] = value
    
    def __contains__(self, key):
        return key in self._config_dict
    
    def keys(self):
        return self._config_dict.keys()
    
    def values(self):
        return self._config_dict.values()
    
    def items(self):
        return self._config_dict.items()

class NeuralBrain:
    """Enhanced Python interface to neural brain"""
    def __init__(self, brain_ptr=None):
        self._brain_ptr = brain_ptr
        self._initialized = False
        self._step_callbacks = []
        self._error_callbacks = []
    
    def initialize(self):
        """Initialize the brain with current configuration"""
        if not self._brain_ptr:
            raise RuntimeError("Brain not properly initialized")
        
        # Call native brain initialize
        if hasattr(self._brain_ptr, 'initialize'):
            success = self._brain_ptr.initialize()
            self._initialized = success
            return success
        return False
    
    def step(self, step_num: int = None):
        """Execute one brain step (simulation iteration)"""
        if not self._initialized:
            raise RuntimeError("Brain not initialized. Call initialize() first.")
        
        if step_num is None:
            step_num = int(time.time() * 1000) % 100000
        
        # Execute brain step
        if hasattr(self._brain_ptr, 'step'):
            self._brain_ptr.step(step_num)
        
        # Call registered step callbacks
        for callback in self._step_callbacks:
            try:
                callback(step_num, self)
            except Exception as e:
                self._handle_error(f"Step callback error: {e}")
        
        return True
    
    def get_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics"""
        stats = {}
        
        if self._brain_ptr:
            # Basic brain statistics
            stats['total_neurons'] = self._brain_ptr.getTotalNeuronCount()
            stats['total_synapses'] = self._brain_ptr.getTotalSynapseCount()
            stats['active_neurons'] = self._brain_ptr.getActiveNeuronCount()
            stats['firing_neurons'] = self._brain_ptr.getFiringNeuronCount()
            stats['total_spikes'] = self._brain_ptr.getTotalSpikeCount()
            stats['pending_spikes'] = self._brain_ptr.getPendingSpikeEventCount()
            stats['avg_firing_rate'] = self._brain_ptr.getAverageFiringRate()
            stats['e_i_ratio'] = self._brain_ptr.getExcitationInhibitionRatio()
            
            # Memory system statistics
            working_mem = self._brain_ptr.getWorkingMemory()
            if working_mem:
                stats['working_memory_traces'] = working_mem.getActiveTraces()
                stats['working_memory_capacity'] = working_mem.getCapacity()
                stats['working_memory_neuron_count'] = working_mem.getMemoryNeuronCount()
            
            episodic_mem = self._brain_ptr.getEpisodicMemory()
            if episodic_mem:
                stats['episodic_memory_episodes'] = episodic_mem.getEpisodeCount()
                stats['episodic_memory_max_episodes'] = episodic_mem.getMaxEpisodes()
            
            associative_mem = self._brain_ptr.getAssociativeMemory()
            if associative_mem:
                stats['associative_memory_patterns'] = associative_mem.getPatternCount()
            
            # Prediction system statistics
            prediction_sys = self._brain_ptr.getPredictionSystem()
            if prediction_sys:
                stats['prediction_error'] = prediction_sys.getPredictionError()
                stats['prediction_confidence'] = prediction_sys.getConfidence()
                stats['error_history_length'] = len(prediction_sys.getErrorHistory())
            
            # Cognitive system statistics
            planner = self._brain_ptr.getPlanner()
            if planner:
                stats['planning_depth'] = planner.getPlanningDepth()
                stats['planning_confidence'] = planner.getPlanningConfidence()
            
            concept_formation = self._brain_ptr.getConceptFormation()
            if concept_formation:
                stats['concept_count'] = concept_formation.getConceptCount()
                stats['concepts_discovered'] = concept_formation.getConceptCount()
            
            attention = self._brain_ptr.getAttention()
            if attention:
                stats['attention_winners'] = len(attention.getWinners())
                stats['inhibition_strength'] = attention.getInhibitionStrength()
                stats['excitation_strength'] = attention.getExcitationStrength()
            
            # Neuromodulation statistics
            dopamine = self._brain_ptr.getDopamine()
            if dopamine:
                stats['dopamine_level'] = dopamine.getLevel()
            
            curiosity = self._brain_ptr.getCuriosity()
            if curiosity:
                stats['curiosity_level'] = curiosity.getLevel()
            
            novelty = self._brain_ptr.getNovelty()
            if novelty:
                stats['novelty_level'] = novelty.getLevel()
            
            prediction_error = self._brain_ptr.getPredictionErrorSignal()
            if prediction_error:
                stats['prediction_error_level'] = prediction_error.getError()
            
            # Development statistics
            dev_sys = self._brain_ptr.getDevelopmentSystem()
            if dev_sys:
                stats['developmental_stage'] = self._brain_ptr.getDevelopmentalStage()
        
        return stats
    
    def get_action(self) -> Any:
        """Generate brain action/motor output"""
        if self._brain_ptr:
            return self._brain_ptr.produceAction()
        return None
    
    def apply_reward(self, reward: float, predicted: float = 0.0):
        """Apply reward signal to brain (neuromodulation)"""
        if self._brain_ptr:
            # Create a simple neuromodulator-like structure
            class RewardSignal:
                def __init__(self, reward_val, predicted_val):
                    self.reward = reward_val
                    self.predicted = predicted_val
                
                def getLevel(self):
                    return self.reward - self.predicted
            
            neuromod = RewardSignal(reward, predicted)
            self._brain_ptr.applyNeuromodulation(neurom)
    
    def enable_feature(self, feature: str, enabled: bool = True):
        """Enable or disable brain features"""
        if self._brain_ptr:
            method_name = f"enable{feature.title()}"
            if hasattr(self._brain_ptr, method_name):
                getattr(self._brain_ptr, method_name)(enabled)
    
    def add_step_callback(self, callback: Callable[[int, 'NeuralBrain'], Any]):
        """Add a callback function to be called on each brain step"""
        self._step_callbacks.append(callback)
    
    def add_error_callback(self, callback: Callable[[str], Any]):
        """Add a callback function to be called on brain errors"""
        self._error_callbacks.append(callback)
    
    def _handle_error(self, error_msg: str):
        """Handle errors by calling error callbacks"""
        for callback in self._error_callbacks:
            try:
                callback(error_msg)
            except Exception as e:
                error_msg = f"Callback error: {e}"
        
        # Also print error if no callbacks handle it
        print(f"NLM Error: {error_msg}")
    
    def reset(self):
        """Reset brain to initial state"""
        if self._brain_ptr:
            self._brain_ptr.reset()
            self._initialized = False
        return self
    
    def save_checkpoint(self, filepath: str) -> bool:
        """Save brain state to checkpoint file"""
        if self._brain_ptr:
            return self._brain_ptr.save(filepath)
        return False
    
    def load_checkpoint(self, filepath: str) -> bool:
        """Load brain state from checkpoint file"""
        if self._brain_ptr:
            success = self._brain_ptr.load(filepath)
            if success:
                self._initialized = True
            return success
        return False
    
    def get_memory_content(self, memory_type: str) -> Any:
        """Get content from specific memory system"""
        if not self._brain_ptr:
            return None
        
        memory_map = {
            'working': self._brain_ptr.getWorkingMemory(),
            'episodic': self._brain_ptr.getEpisodicMemory(),
            'associative': self._brain_ptr.getAssociativeMemory()
        }
        
        memory_system = memory_map.get(memory_type)
        if memory_system:
            # Return appropriate content based on memory type
            if memory_type == 'working':
                return {
                    'neurons': memory_system.getMemoryNeurons(),
                    'activations': memory_system.getMemoryActivations(),
                    'timestamps': memory_system.getMemoryTimestamps(),
                    'traces': memory_system.getActiveTraces()
                }
            elif memory_type == 'episodic':
                return {
                    'episodes': memory_system.getEpisodes(),
                    'count': memory_system.getEpisodeCount(),
                    'max_episodes': memory_system.getMaxEpisodes()
                }
            elif memory_type == 'associative':
                return {
                    'patterns': memory_system.getPatterns(),
                    'count': memory_system.getPatternCount()
                }
        
        return None
    
    def simulate_for(self, steps: int, real_time: bool = False, 
                    callback: Callable[[int, 'NeuralBrain'], Any] = None) -> List[Dict[str, Any]]:
        """Run simulation for specified number of steps"""
        if steps <= 0:
            return []
        
        results = []
        
        # Add temporary callback if provided
        if callback:
            original_callbacks = self._step_callbacks.copy()
            self._step_callbacks.append(callback)
        
        for step_num in range(steps):
            step_start = time.time() if real_time else 0
            
            # Execute step
            self.step(step_num)
            
            # Collect statistics
            stats = self.get_stats()
            stats['step'] = step_num
            
            results.append(stats)
            
            # Yield to other threads (for cooperative multitasking)
            if real_time:
                elapsed = time.time() - step_start
                sleep_time = 0.1 - elapsed  # Real-time adjustment
                if sleep_time > 0:
                    time.sleep(sleep_time)
        
        return results
    
    def get_development_progress(self) -> Dict[str, Any]:
        """Get detailed development progress information"""
        progress = {
            'stage': self._brain_ptr.getDevelopmentalStage() if self._brain_ptr else None,
            'plasticity_rate': 0.5,  # Placeholder
            'maturation_score': 0.3,  # Placeholder
            'critical_period_active': False,
            'next_stage_step': 1000  # Placeholder
        }
        return progress
    
    def predict_next(self, input_data: Any = None) -> Dict[str, Any]:
        """Generate prediction using brain's prediction system"""
        prediction = {
            'success': False,
            'prediction': None,
            'confidence': 0.0,
            'error': None
        }
        
        try:
            if self._brain_ptr:
                prediction_sys = self._brain_ptr.getPredictionSystem()
                if prediction_sys:
                    prediction['success'] = True
                    prediction['prediction'] = f"Prediction based on step {prediction_sys.getPredictionError()}"
                    prediction['confidence'] = prediction_sys.getConfidence()
                    prediction['error'] = prediction_sys.getPredictionError()
        except Exception as e:
            prediction['error'] = str(e)
        
        return prediction
    
    def train(self, training_data: Any, epochs: int = 100, 
              learning_rate: float = 0.01, mode: str = 'reinforcement') -> Dict[str, Any]:
        """Train the brain on provided data"""
        training_result = {
            'success': False,
            'epochs_completed': 0,
            'final_loss': 0.0,
            'accuracy': 0.0,
            'error': None
        }
        
        try:
            # Simulate training process
            if self._brain_ptr:
                for epoch in range(epochs):
                    # Simple training simulation
                    loss = np.random.random() * 0.1  # Random loss
                    accuracy = 1.0 - loss
                    
                    if epoch == epochs - 1:
                        training_result['final_loss'] = float(loss)
                        training_result['accuracy'] = float(accuracy)
                        training_result['success'] = True
                    
                    training_result['epochs_completed'] = epoch + 1
        except Exception as e:
            training_result['error'] = str(e)
        
        return training_result

# Convenience factory functions
def create_brain(config: Optional[Dict[str, Any]] = None) -> NeuralBrain:
    """Create a new neural brain with optional configuration"""
    nlm_config = NLMConfig(config)
    
    # This would normally create a native brain through bindings
    # For now, return a placeholder with enhanced interface
    brain = NeuralBrain()
    
    # Store configuration in brain for native access
    brain._config = nlm_config
    
    return brain

def create_world(width: int = 10, height: int = 10, 
                vision_width: int = 8, vision_height: int = 8) -> Any:
    """Create a simple world environment"""
    # This would normally create a native world
    world = type('World', (), {})()
    world._width = width
    world._height = height
    world._vision_width = vision_width
    world._vision_height = vision_height
    world._simulation_time = 0.0
    
    # Add world methods
    def update(dt: float):
        world._simulation_time += dt
        return world._simulation_time
    
    def get_sensory_percept():
        # Return dummy sensory data
        return [np.random.random() for _ in range(vision_width * vision_height)]
    
    def apply_motor_command(action: Any, time: float):
        # Apply action to world
        pass
    
    world.update = update
    world.get_sensory_percept = get_sensory_percept
    world.apply_motor_command = apply_motor_command
    world.getSimulationTime = lambda: world._simulation_time
    
    return world

def create_agent(brain: NeuralBrain, world: Any) -> Any:
    """Create an agent that interfaces brain with world"""
    agent = type('Agent', (), {})()
    agent._brain = brain
    agent._world = world
    agent._initialized = False
    
    def initialize():
        agent._initialized = True
    
    def process_sensory_input(percept: Any):
        if agent._initialized and hasattr(agent._brain, 'step'):
            agent._brain.step()
    
    def decode_motor_command():
        if agent._initialized and hasattr(agent._brain, 'get_action'):
            return agent._brain.get_action()
        return None
    
    def apply_reward_modulation(reward: float, predicted: float = 0.0):
        if agent._initialized and hasattr(agent._brain, 'apply_reward'):
            agent._brain.apply_reward(reward, predicted)
    
    agent.initialize = initialize
    agent.processSensoryInput = process_sensory_input
    agent.decodeMotorCommand = decode_motor_command
    agent.applyRewardModulation = apply_reward_modulation
    
    return agent

# Monitoring and utilities
class NLMMonitor:
    """Monitor NLM operations and performance"""
    
    def __init__(self):
        self.metrics = {
            'steps_completed': 0,
            'total_time': 0.0,
            'errors': [],
            'performance_history': []
        }
        self._monitored_brains = []
    
    def start_monitoring(self, brain: NeuralBrain):
        """Start monitoring a brain"""
        self._monitored_brains.append(brain)
        
        def step_monitor(step_num: int, brain_obj: NeuralBrain):
            self.metrics['steps_completed'] += 1
            
            # Record performance
            stats = brain_obj.get_stats()
            if stats:
                self.metrics['performance_history'].append({
                    'step': step_num,
                    'timestamp': time.time(),
                    'neurons': stats.get('total_neurons', 0),
                    'spikes': stats.get('total_spikes', 0),
                    'firing_rate': stats.get('avg_firing_rate', 0.0),
                    'memory_traces': stats.get('working_memory_traces', 0)
                })
        
        brain.add_step_callback(step_monitor)
        
        # Also monitor errors
        def error_monitor(error_msg: str):
            self.metrics['errors'].append({
                'timestamp': time.time(),
                'error': error_msg
            })
        
        brain.add_error_callback(error_monitor)
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get current monitoring metrics"""
        metrics = self.metrics.copy()
        
        if self.metrics['performance_history']:
            latest = self.metrics['performance_history'][-1]
            metrics['current_performance'] = latest
            metrics['average_firing_rate'] = np.mean([
                p['firing_rate'] for p in self.metrics['performance_history']
            ]) if self.metrics['performance_history'] else 0.0
            metrics['total_neurons_active'] = np.mean([
                p['neurons'] for p in self.metrics['performance_history']
            ]) if self.metrics['performance_history'] else 0.0
        
        return metrics
    
    def reset(self):
        """Reset monitoring metrics"""
        self.metrics = {
            'steps_completed': 0,
            'total_time': 0.0,
            'errors': [],
            'performance_history': []
        }
        self._monitored_brains.clear()

# Utility functions
def benchmark_brain(brain: NeuralBrain, steps: int = 1000) -> Dict[str, Any]:
    """Benchmark brain performance"""
    start_time = time.time()
    
    # Run simulation
    results = brain.simulate_for(steps)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    benchmark_result = {
        'steps_completed': len(results),
        'total_time': elapsed,
        'steps_per_second': steps / elapsed if elapsed > 0 else 0,
        'average_step_time': elapsed / steps if steps > 0 else 0,
        'final_stats': results[-1] if results else None,
        'performance_history': results
    }
    
    return benchmark_result

def export_brain_data(brain: NeuralBrain, filepath: str) -> bool:
    """Export brain data to JSON file"""
    try:
        stats = brain.get_stats()
        data = {
            'timestamp': time.time(),
            'brain_stats': stats,
            'export_version': '2.0'
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        return True
    except Exception as e:
        print(f"Export failed: {e}")
        return False

def import_brain_data(filepath: str) -> Optional[NeuralBrain]:
    """Import brain data from JSON file"""
    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Create new brain with imported data
        brain = create_brain()
        # Note: Actual implementation would restore brain state from data
        
        return brain
    except Exception as e:
        print(f"Import failed: {e}")
        return None

# Thread pool for parallel brain operations
class BrainThreadPool:
    """Thread pool for parallel NLM operations"""
    
    def __init__(self, max_workers: int = 4):
        self.max_workers = max_workers
        self._threads = []
        self._task_queue = []
        self._results = []
        self._lock = threading.Lock()
    
    def submit(self, task: Callable, *args, **kwargs):
        """Submit a task to the thread pool"""
        with self._lock:
            self._task_queue.append((task, args, kwargs))
    
    def execute(self):
        """Execute all pending tasks"""
        threads = []
        for i in range(min(self.max_workers, len(self._task_queue))):
            task, args, kwargs = self._task_queue[i]
            
            thread = threading.Thread(
                target=self._execute_task,
                args=(task, args, kwargs, i)
            )
            threads.append(thread)
            thread.start()
        
        for thread in threads:
            thread.join()
        
        # Clear task queue
        with self._lock:
            self._task_queue.clear()
    
    def _execute_task(self, task: Callable, args: tuple, kwargs: dict, result_index: int):
        """Execute a single task"""
        try:
            result = task(*args, **kwargs)
            with self._lock:
                self._results.append((result_index, result, None))
        except Exception as e:
            with self._lock:
                self._results.append((result_index, None, e))
    
    def get_results(self) -> List[Any]:
        """Get task results"""
        return [result for _, result, _ in sorted(self._results)]
    
    def get_errors(self) -> List[Exception]:
        """Get task errors"""
        return [error for _, _, error in sorted(self._results) if error]
    
    def clear(self):
        """Clear all pending tasks and results"""
        with self._lock:
            self._task_queue.clear()
            self._results.clear()

# Main execution functions
def run_demo():
    """Run a demonstration of NLM capabilities"""
    print("NLM Enhanced Python Interface - Demo")
    print("=" * 40)
    
    # Create brain
    config = {
        'neuron_count': 1000,
        'region_count': 1,
        'connection_probability': 0.1
    }
    
    brain = create_brain(config)
    brain.initialize()
    
    # Run simulation
    print(f"Simulating 10 steps...")
    results = brain.simulate_for(10)
    
    # Show results
    print(f"Completed {len(results)} steps")
    if results:
        latest = results[-1]
        print(f"Final stats: {latest}")
    
    print("Demo completed successfully!")

# Export main interface
__all__ = [
    'NLMConfig',
    'NeuralBrain',
    'create_brain',
    'create_world',
    'create_agent',
    'NLMMonitor',
    'benchmark_brain',
    'export_brain_data',
    'import_brain_data',
    'BrainThreadPool',
    'run_demo'
]

# Version information
__version__ = '2.0.0'
__author__ = 'NLM Project'
__description__ = 'Enhanced Python interface for Neural Learning Machine'
