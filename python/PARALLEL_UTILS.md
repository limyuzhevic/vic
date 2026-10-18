# NLM Python Bindings - Parallel Processing Utilities

This guide provides parallel processing utilities and tools for optimizing NLM operations using multiprocessing and concurrent processing.

## Parallel Processing Overview

Parallel processing can significantly improve the performance of NLM operations, especially for batch processing, large simulations, and multi-agent scenarios. This guide provides utilities for parallel vision processing, audio processing, batch operations, and multi-agent simulations.

## Parallel Processing Classes

### 1. BatchProcessor Class

```python
import pynlm
import threading
import multiprocessing
import time
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
from typing import List, Dict, Any, Callable, Optional, Union
from queue import Queue
import queue

class BatchProcessor:
    """Parallel batch processor for NLM operations."""
    
    def __init__(self, num_workers: Optional[int] = None, use_processes: bool = False):
        """Initialize batch processor.
        
        Args:
            num_workers: Number of worker threads/processes
            use_processes: Whether to use processes instead of threads
        """
        if num_workers is None:
            num_workers = multiprocessing.cpu_count()
        
        self.num_workers = num_workers
        self.use_processes = use_processes
        self.workers = []
        self.task_queue = Queue()
        self.result_queue = Queue()
        self.is_running = False
        self.processed_count = 0
        self.total_tasks = 0
    
    def start(self):
        """Start the batch processor."""
        if self.is_running:
            return
        
        self.is_running = True
        
        if self.use_processes:
            self.workers = []
            for i in range(self.num_workers):
                worker = multiprocessing.Process(target=self._worker_process)
                worker.daemon = True
                worker.start()
                self.workers.append(worker)
        else:
            self.workers = []
            for i in range(self.num_workers):
                worker = threading.Thread(target=self._worker_thread)
                worker.daemon = True
                worker.start()
                self.workers.append(worker)
        
        print(f"Started batch processor with {self.num_workers} {'processes' if self.use_processes else 'threads'}")
    
    def _worker_process(self):
        """Worker process function."""
        while self.is_running:
            try:
                task = self.task_queue.get(timeout=1)
                
                if task is None:  # Sentinel value
                    break
                
                # Process task
                func, args, kwargs = task
                result = func(*args, **kwargs)
                
                # Send result
                self.result_queue.put(result)
                
                # Update processed count (atomic operation)
                # Note: This is simplified - in practice, you'd use proper synchronization
                self.processed_count += 1
                
            except queue.Empty:
                continue
            except Exception as e:
                print(f"Worker error: {e}")
    
    def _worker_thread(self):
        """Worker thread function."""
        while self.is_running:
            try:
                task = self.task_queue.get(timeout=1)
                
                if task is None:  # Sentinel value
                    break
                
                # Process task
                func, args, kwargs = task
                result = func(*args, **kwargs)
                
                # Send result
                self.result_queue.put(result)
                
                # Update processed count
                self.processed_count += 1
                
            except queue.Empty:
                continue
            except Exception as e:
                print(f"Worker error: {e}")
    
    def submit(self, func: Callable, *args, **kwargs):
        """Submit a task to the batch processor.
        
        Args:
            func: Function to execute
            *args: Arguments to pass to function
            **kwargs: Keyword arguments to pass to function
            
        Returns:
            Future object
        """
        if not self.is_running:
            self.start()
        
        task = (func, args, kwargs)
        self.task_queue.put(task)
        self.total_tasks += 1
        
        # Create a future for this task
        future = _BatchFuture(self, task)
        return future
    
    def map(self, func: Callable, tasks: List, chunksize: int = 1) -> List:
        """Map function to iterable of tasks.
        
        Args:
            func: Function to execute
            tasks: Iterable of tasks
            chunksize: Number of tasks per worker
            
        Returns:
            List of results
        """
        if not self.is_running:
            self.start()
        
        # Submit all tasks
        futures = []
        for task in tasks:
            if isinstance(task, tuple):
                future = self.submit(func, *task)
            else:
                future = self.submit(func, task)
            futures.append(future)
        
        # Collect results
        results = []
        for future in futures:
            results.append(future.result())
        
        return results
    
    def shutdown(self, wait: bool = True):
        """Shutdown the batch processor.
        
        Args:
            wait: Whether to wait for tasks to complete
        """
        if not self.is_running:
            return
        
        # Send sentinel values to workers
        for _ in range(self.num_workers):
            self.task_queue.put(None)
        
        self.is_running = False
        
        if wait:
            # Wait for all workers to finish
            for worker in self.workers:
                worker.join(timeout=10)
        
        self.workers.clear()
        
        print("Batch processor shutdown complete")
    
    def get_progress(self) -> Dict[str, Any]:
        """Get current progress information.
        
        Returns:
            Dictionary with progress information
        """
        return {
            'processed': self.processed_count,
            'total': self.total_tasks,
            'progress_percent': (self.processed_count / max(self.total_tasks, 1)) * 100,
            'is_running': self.is_running
        }

class _BatchFuture:
    """Future object for batch processing tasks."""
    
    def __init__(self, processor: BatchProcessor, task):
        """Initialize future.
        
        Args:
            processor: Batch processor
            task: Task information
        """
        self.processor = processor
        self.task = task
        self._result = None
        self._exception = None
        self._completed = False
    
    def result(self, timeout: Optional[float] = None):
        """Get result of task.
        
        Args:
            timeout: Timeout in seconds
            
        Returns:
            Task result
        """
        if not self._completed:
            # Wait for result
            start_time = time.time()
            
            while not self._completed:
                # Check result queue
                try:
                    result = self.processor.result_queue.get(timeout=0.1)
                    
                    # Check if this is our result
                    if result is not None:
                        # In practice, you'd need proper result matching
                        self._result = result
                        self._completed = True
                        break
                except queue.Empty:
                    pass
                
                if timeout and time.time() - start_time > timeout:
                    raise TimeoutError(f"Task timeout after {timeout} seconds")
        
        if self._exception is not None:
            raise self._exception
        
        return self._result
    
    def add_done_callback(self, fn: Callable):
        """Add callback to be called when task completes.
        
        Args:
            fn: Callback function
        """
        def callback():
            fn(self)
        
        # In practice, you'd implement proper callback mechanism
        pass
```

### 2. Vision Processing Parallelizer

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
from typing import List, Callable, Optional
import time
import os

class VisionProcessorParallelizer:
    """Parallel vision processor for batch operations."""
    
    def __init__(self, num_workers: Optional[int] = None, use_processes: bool = False):
        """Initialize vision processor parallelizer.
        
        Args:
            num_workers: Number of worker threads/processes
            use_processes: Whether to use processes instead of threads
        """
        if num_workers is None:
            num_workers = multiprocessing.cpu_count()
        
        self.num_workers = num_workers
        self.use_processes = use_processes
    
    def process_visions(self, visions: List[pynlm.Vision], 
                       processor_func: Optional[Callable] = None,
                       batch_size: int = 10,
                       max_workers: Optional[int] = None) -> List:
        """Process visions in parallel.
        
        Args:
            visions: List of vision objects
            processor_func: Function to process each vision
            batch_size: Number of visions to process in each batch
            max_workers: Maximum number of workers
            
        Returns:
            List of processed results
        """
        if max_workers is None:
            max_workers = self.num_workers
        
        if processor_func is None:
            processor_func = self._default_processor
        
        # Split visions into batches
        batches = [visions[i:i + batch_size] 
                  for i in range(0, len(visions), batch_size)]
        
        # Process batches in parallel
        all_results = []
        
        if self.use_processes:
            with ProcessPoolExecutor(max_workers=max_workers) as executor:
                futures = []
                for batch in batches:
                    future = executor.submit(self._process_vision_batch, 
                                           batch, processor_func)
                    futures.append(future)
                
                # Collect results
                for future in as_completed(futures):
                    try:
                        results = future.result()
                        all_results.extend(results)
                    except Exception as e:
                        print(f"Batch processing error: {e}")
        else:
            with ThreadPoolExecutor(max_workers=max_workers) as executor:
                futures = []
                for batch in batches:
                    future = executor.submit(self._process_vision_batch,
                                           batch, processor_func)
                    futures.append(future)
                
                # Collect results
                for future in as_completed(futures):
                    try:
                        results = future.result()
                        all_results.extend(results)
                    except Exception as e:
                        print(f"Batch processing error: {e}")
        
        return all_results
    
    def _default_processor(self, vision: pynlm.Vision) -> Dict[str, Any]:
        """Default vision processor."""
        processor = pynlm.VisionProcessor()
        processor.process(vision)
        
        return {
            'vision': vision,
            'features': processor.getFeatures(),
            'dimensions': {
                'width': vision.getWidth(),
                'height': vision.getHeight(),
                'channels': vision.getChannels()
            }
        }
    
    def _process_vision_batch(self, batch: List[pynlm.Vision], 
                             processor_func: Callable) -> List:
        """Process a batch of visions."""
        results = []
        for vision in batch:
            try:
                result = processor_func(vision)
                results.append(result)
            except Exception as e:
                print(f"Error processing vision: {e}")
                results.append({'error': str(e), 'vision': vision})
        
        return results
    
    def process_vision_sequences(self, vision_sequences: List[List[pynlm.Vision]],
                                processor_func: Optional[Callable] = None,
                                parallel_sequences: bool = True) -> List[List]:
        """Process sequences of visions in parallel.
        
        Args:
            vision_sequences: List of vision sequences
            processor_func: Function to process each vision
            parallel_sequences: Whether to process sequences in parallel
            
        Returns:
            List of processed vision sequences
        """
        if parallel_sequences:
            # Process sequences in parallel
            with ThreadPoolExecutor(max_workers=self.num_workers) as executor:
                futures = []
                for sequence in vision_sequences:
                    future = executor.submit(self._process_vision_sequence,
                                           sequence, processor_func)
                    futures.append(future)
                
                results = []
                for future in as_completed(futures):
                    try:
                        results.append(future.result())
                    except Exception as e:
                        print(f"Sequence processing error: {e}")
                
                return results
        else:
            # Process sequences sequentially
            return [self._process_vision_sequence(seq, processor_func) 
                   for seq in vision_sequences]
    
    def _process_vision_sequence(self, sequence: List[pynlm.Vision],
                                processor_func: Callable) -> List:
        """Process a sequence of visions."""
        return [processor_func(vision) for vision in sequence]
    
    def streaming_vision_processing(self, vision_stream, batch_size: int = 10,
                                   processor_func: Optional[Callable] = None) -> List:
        """Process vision stream in real-time.
        
        Args:
            vision_stream: Stream of vision objects (generator)
            batch_size: Number of visions per batch
            processor_func: Function to process each vision
            
        Returns:
            List of processed results
        """
        if processor_func is None:
            processor_func = self._default_processor
        
        batch = []
        results = []
        
        for vision in vision_stream:
            batch.append(vision)
            
            if len(batch) >= batch_size:
                batch_results = self._process_vision_batch(batch, processor_func)
                results.extend(batch_results)
                batch.clear()
        
        # Process remaining batch
        if batch:
            batch_results = self._process_vision_batch(batch, processor_func)
            results.extend(batch_results)
        
        return results
```

### 3. Audio Processing Parallelizer

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
from typing import List, Callable, Optional
import time
import numpy as np

class AudioProcessorParallelizer:
    """Parallel audio processor for batch operations."""
    
    def __init__(self, num_workers: Optional[int] = None, use_processes: bool = False):
        """Initialize audio processor parallelizer.
        
        Args:
            num_workers: Number of worker threads/processes
            use_processes: Whether to use processes instead of threads
        """
        if num_workers is None:
            num_workers = multiprocessing.cpu_count()
        
        self.num_workers = num_workers
        self.use_processes = use_processes
    
    def process_audios(self, audios: List[pynlm.Audio],
                      processor_func: Optional[Callable] = None,
                      batch_size: int = 10,
                      max_workers: Optional[int] = None) -> List:
        """Process audios in parallel.
        
        Args:
            audios: List of audio objects
            processor_func: Function to process each audio
            batch_size: Number of audios to process in each batch
            max_workers: Maximum number of workers
            
        Returns:
            List of processed results
        """
        if max_workers is None:
            max_workers = self.num_workers
        
        if processor_func is None:
            processor_func = self._default_processor
        
        # Split audios into batches
        batches = [audios[i:i + batch_size]
                  for i in range(0, len(audios), batch_size)]
        
        # Process batches in parallel
        all_results = []
        
        if self.use_processes:
            with ProcessPoolExecutor(max_workers=max_workers) as executor:
                futures = []
                for batch in batches:
                    future = executor.submit(self._process_audio_batch,
                                           batch, processor_func)
                    futures.append(future)
                
                # Collect results
                for future in as_completed(futures):
                    try:
                        results = future.result()
                        all_results.extend(results)
                    except Exception as e:
                        print(f"Batch processing error: {e}")
        else:
            with ThreadPoolExecutor(max_workers=max_workers) as executor:
                futures = []
                for batch in batches:
                    future = executor.submit(self._process_audio_batch,
                                           batch, processor_func)
                    futures.append(future)
                
                # Collect results
                for future in as_completed(futures):
                    try:
                        results = future.result()
                        all_results.extend(results)
                    except Exception as e:
                        print(f"Batch processing error: {e}")
        
        return all_results
    
    def _default_processor(self, audio: pynlm.Audio) -> Dict[str, Any]:
        """Default audio processor."""
        processor = pynlm.AudioProcessor()
        processor.process(audio)
        
        return {
            'audio': audio,
            'features': processor.getFeatures(),
            'metadata': {
                'sample_rate': audio.getSampleRate(),
                'num_samples': audio.getNumSamples(),
                'duration_ms': (audio.getNumSamples() / audio.getSampleRate()) * 1000
            }
        }
    
    def _process_audio_batch(self, batch: List[pynlm.Audio],
                            processor_func: Callable) -> List:
        """Process a batch of audios."""
        results = []
        for audio in batch:
            try:
                result = processor_func(audio)
                results.append(result)
            except Exception as e:
                print(f"Error processing audio: {e}")
                results.append({'error': str(e), 'audio': audio})
        
        return results
    
    def process_audio_files(self, audio_files: List[str], 
                           processor_func: Optional[Callable] = None,
                           batch_size: int = 5) -> List:
        """Process multiple audio files.
        
        Args:
            audio_files: List of audio file paths
            processor_func: Function to process each audio
            batch_size: Number of files per batch
            
        Returns:
            List of processed results
        """
        # Load audio files
        audios = []
        for filepath in audio_files:
            try:
                # This assumes you have a way to load audio files
                # In practice, you'd need audio loading functionality
                audio = pynlm.createAudio(44100, 1000)  # Placeholder
                audios.append(audio)
            except Exception as e:
                print(f"Error loading audio file {filepath}: {e}")
        
        return self.process_audios(audios, processor_func, batch_size)
    
    def real_time_audio_processing(self, audio_stream, batch_size: int = 10,
                                   processor_func: Optional[Callable] = None) -> List:
        """Process audio stream in real-time.
        
        Args:
            audio_stream: Stream of audio objects (generator)
            batch_size: Number of audios per batch
            processor_func: Function to process each audio
            
        Returns:
            List of processed results
        """
        if processor_func is None:
            processor_func = self._default_processor
        
        batch = []
        results = []
        
        for audio in audio_stream:
            batch.append(audio)
            
            if len(batch) >= batch_size:
                batch_results = self._process_audio_batch(batch, processor_func)
                results.extend(batch_results)
                batch.clear()
        
        # Process remaining batch
        if batch:
            batch_results = self._process_audio_batch(batch, processor_func)
            results.extend(batch_results)
        
        return results
```

### 4. Mixed Modal Processing

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
from typing import List, Dict, Any, Optional, Union
import time

class MixedModalProcessor:
    """Parallel processor for mixed sensory modalities."""
    
    def __init__(self, num_workers: Optional[int] = None, use_processes: bool = False):
        """Initialize mixed modal processor.
        
        Args:
            num_workers: Number of worker threads/processes
            use_processes: Whether to use processes instead of threads
        """
        if num_workers is None:
            num_workers = multiprocessing.cpu_count()
        
        self.num_workers = num_workers
        self.use_processes = use_processes
        
        # Separate processors for each modality
        self.vision_parallelizer = VisionProcessorParallelizer(num_workers, use_processes)
        self.audio_parallelizer = AudioProcessorParallelizer(num_workers, use_processes)
    
    def process_multimodal_data(self, multimodal_data: List[Dict[str, Any]],
                               processor_func: Optional[Callable] = None,
                               batch_size: int = 10) -> List:
        """Process multimodal data in parallel.
        
        Args:
            multimodal_data: List of multimodal data dictionaries
            processor_func: Function to process each data item
            batch_size: Number of items per batch
            
        Returns:
            List of processed results
        """
        if processor_func is None:
            processor_func = self._default_processor
        
        # Split data into batches
        batches = [multimodal_data[i:i + batch_size]
                  for i in range(0, len(multimodal_data), batch_size)]
        
        # Process batches in parallel
        all_results = []
        
        if self.use_processes:
            with ProcessPoolExecutor(max_workers=self.num_workers) as executor:
                futures = []
                for batch in batches:
                    future = executor.submit(self._process_multimodal_batch,
                                           batch, processor_func)
                    futures.append(future)
                
                # Collect results
                for future in as_completed(futures):
                    try:
                        results = future.result()
                        all_results.extend(results)
                    except Exception as e:
                        print(f"Batch processing error: {e}")
        else:
            with ThreadPoolExecutor(max_workers=self.num_workers) as executor:
                futures = []
                for batch in batches:
                    future = executor.submit(self._process_multimodal_batch,
                                           batch, processor_func)
                    futures.append(future)
                
                # Collect results
                for future in as_completed(futures):
                    try:
                        results = future.result()
                        all_results.extend(results)
                    except Exception as e:
                        print(f"Batch processing error: {e}")
        
        return all_results
    
    def _default_processor(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Default processor for multimodal data."""
        result = {'input': data, 'processed': True}
        
        # Process vision data
        if 'vision' in data:
            try:
                vision_result = self.vision_parallelizer._default_processor(data['vision'])
                result['vision'] = vision_result
            except Exception as e:
                result['vision_error'] = str(e)
        
        # Process audio data
        if 'audio' in data:
            try:
                audio_result = self.audio_parallelizer._default_processor(data['audio'])
                result['audio'] = audio_result
            except Exception as e:
                result['audio_error'] = str(e)
        
        # Process internal signals
        if 'internal' in data:
            try:
                internal_result = self._process_internal_signals(data['internal'])
                result['internal'] = internal_result
            except Exception as e:
                result['internal_error'] = str(e)
        
        return result
    
    def _process_multimodal_batch(self, batch: List[Dict[str, Any]],
                                  processor_func: Callable) -> List:
        """Process a batch of multimodal data."""
        results = []
        for data in batch:
            try:
                result = processor_func(data)
                results.append(result)
            except Exception as e:
                print(f"Error processing multimodal data: {e}")
                results.append({'error': str(e), 'input': data})
        
        return results
    
    def _process_internal_signals(self, signals: Any) -> Dict[str, Any]:
        """Process internal signals (placeholder)."""
        # This would depend on your specific internal signals format
        return {
            'signals': signals,
            'summary': {
                'count': len(signals) if hasattr(signals, '__len__') else 0,
                'mean': sum(signals) / len(signals) if hasattr(signals, '__len__') and len(signals) > 0 else 0
            }
        }
    
    def adaptive_processing(self, multimodal_data: List[Dict[str, Any]],
                           processing_capability: str = "auto") -> List:
        """Adaptive processing based on system capabilities.
        
        Args:
            multimodal_data: List of multimodal data
            processing_capability: Processing capability level
            
        Returns:
            List of processed results
        """
        if processing_capability == "auto":
            # Choose processing method based on data size and available resources
            data_size = len(multimodal_data)
            
            if data_size < 100:
                # Small dataset, use threading
                return self.process_multimodal_data(multimodal_data, batch_size=10)
            elif data_size < 1000:
                # Medium dataset, use both threading and processing
                return self.process_multimodal_data(multimodal_data, batch_size=50)
            else:
                # Large dataset, use processing
                return self.process_multimodal_data(multimodal_data, batch_size=100)
        
        elif processing_capability == "threads":
            return self.process_multimodal_data(multimodal_data, batch_size=10)
        
        elif processing_capability == "processes":
            return self.process_multimodal_data(multimodal_data, batch_size=50)
        
        elif processing_capability == "batch":
            return self.process_multimodal_data(multimodal_data, batch_size=100)
        
        else:
            raise ValueError(f"Unknown processing capability: {processing_capability}")
```

### 5. Parallel Agent Processing

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
from typing import List, Callable, Optional, Dict, Any
import time
import threading

class ParallelAgentManager:
    """Manager for running multiple agents in parallel."""
    
    def __init__(self, num_agents: int = 1, use_processes: bool = False):
        """Initialize parallel agent manager.
        
        Args:
            num_agents: Number of parallel agents
            use_processes: Whether to use separate processes for agents
        """
        self.num_agents = num_agents
        self.use_processes = use_agents
        self.agents = []
        self.is_running = False
    
    def create_agents(self, base_config: pynlm.Config, 
                     num_agents: Optional[int] = None) -> List[pynlm.AgentBrain]:
        """Create multiple agents from base configuration.
        
        Args:
            base_config: Base configuration for all agents
            num_agents: Number of agents to create
            
        Returns:
            List of agent brains
        """
        if num_agents is None:
            num_agents = self.num_agents
        
        agents = []
        
        for i in range(num_agents):
            # Create unique config for each agent
            agent_config = pynlm.createDefaultConfig()
            
            # Copy settings from base config
            keys = base_config.getKeys()
            for key in keys:
                agent_config.set(key, base_config.get(key))
            
            # Add agent-specific settings
            agent_config.set("agent.id", i)
            
            # Create agent
            brain = pynlm.createBrain(agent_config)
            agent = pynlm.createAgentBrain(brain)
            
            agents.append(agent)
        
        self.agents = agents
        return agents
    
    def run_agents_parallel(self, world: pynlm.SimpleWorld, 
                           num_steps: int,
                           agent_configs: Optional[List[Dict[str, Any]]] = None) -> List[Dict[str, Any]]:
        """Run multiple agents in parallel.
        
        Args:
            world: Shared world
            num_steps: Number of simulation steps
            agent_configs: Optional list of agent-specific configurations
            
        Returns:
            List of agent results
        """
        if not self.agents:
            raise ValueError("No agents available. Create agents first.")
        
        # Initialize agents with world
        for agent in self.agents:
            agent.initialize(world)
        
        # Process agents in parallel
        results = self._process_agents_parallel(self.agents, world, num_steps)
        
        return results
    
    def _process_agents_parallel(self, agents: List[pynlm.AgentBrain], 
                                world: pynlm.SimpleWorld,
                                num_steps: int) -> List[Dict[str, Any]]:
        """Process agents in parallel.
        
        Args:
            agents: List of agents to process
            world: Shared world
            num_steps: Number of simulation steps
            
        Returns:
            List of agent results
        """
        if self.use_processes:
            with ProcessPoolExecutor(max_workers=len(agents)) as executor:
                futures = []
                for agent in agents:
                    future = executor.submit(self._run_single_agent,
                                           agent, world, num_steps)
                    futures.append(future)
                
                results = []
                for future in as_completed(futures):
                    try:
                        results.append(future.result())
                    except Exception as e:
                        print(f"Agent processing error: {e}")
                
                return results
        else:
            with ThreadPoolExecutor(max_workers=len(agents)) as executor:
                futures = []
                for agent in agents:
                    future = executor.submit(self._run_single_agent,
                                           agent, world, num_steps)
                    futures.append(future)
                
                results = []
                for future in as_completed(futures):
                    try:
                        results.append(future.result())
                    except Exception as e:
                        print(f"Agent processing error: {e}")
                
                return results
    
    def _run_single_agent(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld,
                         num_steps: int) -> Dict[str, Any]:
        """Run a single agent.
        
        Args:
            agent: Agent to run
            world: World to simulate in
            num_steps: Number of simulation steps
            
        Returns:
            Agent result
        """
        result = {
            'agent_id': agent.getBrain().getConfig().get("agent.id", 0) if agent.getBrain().getConfig() else 0,
            'steps': num_steps,
            'metrics': {},
            'history': []
        }
        
        # Run simulation
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain = agent.getBrain()
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record metrics
            step_metrics = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'reward': reward
            }
            
            result['metrics'][step] = step_metrics
            result['history'].append(step_metrics)
        
        return result
    
    def run_agent_episodes(self, world: pynlm.SimpleWorld, 
                          episodes: int,
                          steps_per_episode: int) -> List[Dict[str, Any]]:
        """Run multiple agent episodes.
        
        strategy.
        
        Args:
            world: World to simulate in
            episodes: Number of episodes
            steps_per_episode: Number of steps per episode
            
        Returns:
            List of episode results
        """
        all_episode_results = []
        
        for episode in range(episodes):
            print(f"Running episode {episode + 1}/{episodes}")
            
            # Reset agents for new episode
            for agent in self.agents:
                agent.reset()
                world.reset()
            
            # Run episode
            episode_result = self.run_agents_parallel(world, steps_per_episode)
            all_episode_results.extend(episode_result)
        
        return all_episode_results
    
    def shutdown(self):
        """Shutdown all agents."""
        # Cleanup agents
        self.agents.clear()
        self.is_running = False
        
        print("Parallel agent manager shutdown complete")

# Convenience functions for parallel processing

def parallel_vision_processing(visions: List[pynlm.Vision],
                               processor_func: Optional[Callable] = None,
                               num_workers: Optional[int] = None) -> List:
    """Process visions in parallel using VisionProcessorParallelizer.
    
    Args:
        visions: List of vision objects
        processor_func: Function to process each vision
        num_workers: Number of worker threads/processes
        
    Returns:
        List of processed results
    """
    parallelizer = VisionProcessorParallelizer(num_workers)
    return parallelizer.process_visions(visions, processor_func)

def parallel_audio_processing(audios: List[pynlm.Audio],
                              processor_func: Optional[Callable] = None,
                              num_workers: Optional[int] = None) -> List:
    """Process audios in parallel using AudioProcessorParallelizer.
    
    Args:
        audios: List of audio objects
        processor_func: Function to process each audio
        num_workers: Number of worker threads/processes
        
    Returns:
        List of processed results
    """
    parallelizer = AudioProcessorParallelizer(num_workers)
    return parallelizer.process_audios(audios, processor_func)

def parallel_multimodal_processing(multimodal_data: List[Dict[str, Any]],
                                   processor_func: Optional[Callable] = None,
                                   num_workers: Optional[int] = None) -> List:
    """Process multimodal data in parallel using MixedModalProcessor.
    
    Args:
        multimodal_data: List of multimodal data dictionaries
        processor_func: Function to process each data item
        num_workers: Number of worker threads/processes
        
    Returns:
        List of processed results
    """
    processor = MixedModalProcessor(num_workers)
    return processor.process_multimodal_data(multimodal_data, processor_func)

def parallel_agent_simulation(agent_configs: List[pynlm.Config],
                              world: pynlm.SimpleWorld,
                              num_steps: int) -> List[Dict[str, Any]]:
    """Run multiple agents in parallel.
    
    Args:
        agent_configs: List of agent configurations
        world: Shared world
        num_steps: Number of simulation steps
        
    Returns:
        List of agent results
    """
    manager = ParallelAgentManager()
    agents = manager.create_agents(agent_configs[0], len(agent_configs))
    
    return manager.run_agents_parallel(world, num_steps)
```

## Performance Considerations

### 1. Thread vs Process Pool

- **Thread Pool**: Better for I/O-bound tasks, lower overhead
- **Process Pool**: Better for CPU-bound tasks, avoids Python GIL limitations

### 2. Batch Size Optimization

```python
def find_optimal_batch_size(data_size: int, num_workers: int, 
                           time_func: Callable) -> int:
    """Find optimal batch size for parallel processing."""
    batch_sizes = [1, 2, 4, 8, 16, 32, 64]
    best_batch_size = 1
    best_time = float('inf')
    
    for batch_size in batch_sizes:
        if batch_size > data_size:
            continue
        
        # Process with current batch size
        start_time = time.time()
        
        # (In practice, you would implement actual processing here)
        
        end_time = time.time()
        elapsed_time = end_time - start_time
        
        if elapsed_time < best_time:
            best_time = elapsed_time
            best_batch_size = batch_size
    
    return best_batch_size
```

### 3. Resource Management

```python
class ResourceManager:
    """Manage resources for parallel processing."""
    
    def __init__(self, max_memory_mb: int = 1024, max_cpu_percent: float = 80.0):
        self.max_memory_mb = max_memory_mb
        self.max_cpu_percent = max_cpu_percent
        self._check_system_resources()
    
    def _check_system_resources(self):
        """Check available system resources."""
        import psutil
        
        self.available_memory = psutil.virtual_memory().available / 1024 / 1024
        self.cpu_count = psutil.cpu_count()
        
        print(f"Available memory: {self.available_memory:.1f} MB")
        print(f"CPU cores: {self.cpu_count}")
    
    def can_process(self, estimated_memory_mb: float, num_workers: int) -> bool:
        """Check if processing can proceed with given resources.
        
        Args:
            estimated_memory_mb: Estimated memory usage
            num_workers: Number of workers needed
            
        Returns:
            True if processing can proceed, False otherwise
        """
        if estimated_memory_mb > self.available_memory:
            print(f"Insufficient memory: need {estimated_memory_mb:.1f} MB, "
                  f"available {self.available_memory:.1f} MB")
            return False
        
        if num_workers > self.cpu_count:
            print(f"Too many workers: need {num_workers}, "
                  f"available {self.cpu_count}")
            return False
        
        return True
    
    def get_optimal_workers(self, estimated_memory_mb: float, num_tasks: int) -> int:
        """Get optimal number of workers.
        
        Args:
            estimated_memory_mb: Estimated memory per task
            num_tasks: Number of tasks
            
        Returns:
            Optimal number of workers
        """
        # Calculate memory constraint
        max_workers_by_memory = max(1, int(self.available_memory / estimated_memory_mb))
        
        # Calculate CPU constraint
        max_workers_by_cpu = max(1, self.cpu_count - 1)  # Leave one core for system
        
        # Calculate task constraint
        max_workers_by_tasks = min(num_tasks, self.cpu_count)
        
        # Use minimum of all constraints
        optimal_workers = min(max_workers_by_memory, max_workers_by_cpu, max_workers_by_tasks)
        
        return optimal_workers
```

### 4. Scalability Testing

```python
def test_scalability(data_sizes: List[int], num_workers_list: List[int],
                     processing_function: Callable) -> Dict[str, Any]:
    """Test scalability of parallel processing.
    
    Args:
        data_sizes: List of data sizes to test
        num_workers_list: List of worker counts to test
        processing_function: Function to test
        
    Returns:
        Dictionary with scalability results
    """
    results = {}
    
    for data_size in data_sizes:
        print(f"\nTesting with data size: {data_size}")
        
        data_size_results = {}
        
        for num_workers in num_workers_list:
            print(f"  Workers: {num_workers}")
            
            # Create test data
            test_data = [i for i in range(data_size)]
            
            # Measure performance
            start_time = time.time()
            
            # Process with current settings
            result = processing_function(test_data, num_workers)
            
            end_time = time.time()
            
            processing_time = end_time - start_time
            throughput = data_size / processing_time if processing_time > 0 else 0
            
            data_size_results[num_workers] = {
                'processing_time_s': processing_time,
                'throughput_items_per_sec': throughput
            }
            
            print(f"    Processing time: {processing_time:.3f}s, "
                  f"Throughput: {throughput:.2f} items/s")
        
        results[data_size] = data_size_results
    
    return results
```

## Summary

The parallel processing utilities provide comprehensive support for:

1. **Batch Processing**: Parallel processing of sensory inputs
2. **Vision Processing**: Optimized vision processing in parallel
3. **Audio Processing**: Optimized audio processing in parallel
4. **Multimodal Processing**: Processing of mixed sensory modalities
5. **Agent Processing**: Parallel simulation of multiple agents
6. **Resource Management**: Automatic resource allocation and management
7. **Scalability Testing**: Systematic performance testing

These utilities enable users to significantly improve the performance of NLM applications by leveraging parallel processing capabilities while maintaining ease of use and providing robust error handling.

## Next Steps

1. **Explore Parallel Capabilities**: Test different parallel processing strategies
2. **Optimize Batch Sizes**: Find optimal batch sizes for your specific use case
3. **Monitor Resource Usage**: Implement resource monitoring in your applications
4. **Benchmark Performance**: Use the provided benchmarking tools to measure improvements
5. **Scale Up**: Apply parallel processing to larger, more complex simulations

These parallel processing utilities make it much easier to build high-performance NLM applications that can handle large-scale simulations and complex multi-agent scenarios efficiently.

---

*End of Configuration Management and Parallel Processing Guide*