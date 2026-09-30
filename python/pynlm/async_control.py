pynlm/async_control.py
Name: Async control module
Summary: Asynchronous operations for non-blocking control

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType
import asyncio
import time
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass
from enum import Enum
import threading


class AsyncMode(Enum):
    """Async operation mode enumeration"""
    FIFO = "fifo"
    PRIORITY = "priority"
    REAL_TIME = "real_time"
    BATCH = "batch"


@dataclass
class AsyncTask:
    """Asynchronous task definition"""
    id: str
    action: Callable
    priority: int = 0
    timeout: Optional[float] = None
    callback: Optional[Callable] = None
    error_callback: Optional[Callable] = None
    tags: List[str] = None
    created_at: float = 0
    scheduled_at: Optional[float] = None
    completed_at: Optional[float] = None
    status: str = 'pending'
    
    def __post_init__(self):
        if self.tags is None:
            self.tags = []
        if self.created_at == 0:
            self.created_at = time.time()


class AsyncTaskManager:
    """Manage asynchronous operations for NLM control"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.tasks: Dict[str, AsyncTask] = {}
        self.running = False
        self.worker_threads = []
        self.task_queue = asyncio.Queue()
        self.completed_tasks: List[AsyncTask] = []
        self.max_workers = 4
        self.timeout = 30.0
        self.mode = AsyncMode.FIFO
        self.priority_enabled = False
        self.batch_size = 10
        self.retry_attempts = 3
        self.retry_delay = 1.0
        self.event_handlers: Dict[str, List[Callable]] = {}
        self.task_filters: Dict[str, Callable] = {}
    
    def start(self):
        """Start the async task manager"""
        if self.running:
            return
        
        self.running = True
        
        # Create worker threads
        for i in range(self.max_workers):
            thread = threading.Thread(target=self._worker_loop, daemon=True)
            thread.start()
            self.worker_threads.append(thread)
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Async task manager started with {self.max_workers} workers")
    
    def stop(self, timeout: float = 10.0):
        """Stop the async task manager"""
        self.running = False
        
        # Add poison pills to stop workers
        for _ in range(self.max_workers):
            self.task_queue.put(None)
        
        # Wait for workers to stop
        for thread in self.worker_threads:
            thread.join(timeout=timeout)
        
        self.worker_threads.clear()
        
        logger = __import__('logging').getLogger(__name__)
        logger.info("Async task manager stopped")
    
    def submit_task(self, action: Callable, priority: int = 0, 
                   timeout: Optional[float] = None, tags: Optional[List[str]] = None,
                   callback: Optional[Callable] = None,
                   error_callback: Optional[Callable] = None, task_id: Optional[str] = None)
        """Submit an asynchronous task"""
        task_id = task_id or f"task_{int(time.time() * 1000)}_{len(self.tasks)}"
        
        task = AsyncTask(
            id=task_id,
            action=action,
            priority=priority,
            timeout=timeout,
            callback=callback,
            error_callback=error_callback,
            tags=tags or []
        )
        
        self.tasks[task_id] = task
        
        # Put task in queue
        self.task_queue.put(task)
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Task submitted: {task_id} (priority: {priority})")
        
        return task_id
    
    def submit_batch(self, tasks: List[Callable], batch_id: Optional[str] = None,
                    **kwargs) -> List[str]:
        """Submit multiple tasks as a batch"""
        if batch_id is None:
            batch_id = f"batch_{int(time.time() * 1000)}"
        
        task_ids = []
        
        for i, task_func in enumerate(tasks):
            task_id = f"{batch_id}_task_{i}"
            task_ids.append(self.submit_task(
                task_func,
                tags=[batch_id, 'batch'],
                **kwargs
            ))
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Batch submitted: {batch_id} with {len(tasks)} tasks")
        
        return task_ids
    
    def get_task(self, task_id: str) -> Optional[AsyncTask]:
        """Get task information"""
        return self.tasks.get(task_id)
    
    def cancel_task(self, task_id: str) -> bool:
        """Cancel a task"""
        if task_id not in self.tasks:
            return False
        
        task = self.tasks[task_id]
        task.status = 'cancelled'
        
        # Remove from queue if still pending
        # Note: This is simplified - in practice, you might need to scan the queue
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Task cancelled: {task_id}")
        
        return True
    
    def get_task_status(self, task_id: str) -> Optional[str]:
        """Get task status"""
        if task_id not in self.tasks:
            return None
        
        task = self.tasks[task_id]
        return task.status
    
    def get_all_tasks(self) -> List[AsyncTask]:
        """Get all tasks"""
        return list(self.tasks.values())
    
    def get_completed_tasks(self, limit: Optional[int] = None) -> List[AsyncTask]:
        """Get completed tasks"""
        completed = [t for t in self.tasks.values() if t.status == 'completed']
        
        if limit is not None:
            completed = completed[-limit:]
        
        return completed
    
    def get_pending_tasks(self) -> List[AsyncTask]:
        """Get pending tasks"""
        return [t for t in self.tasks.values() if t.status == 'pending']
    
    def get_running_tasks(self) -> List[AsyncTask]:
        """Get running tasks"""
        return [t for t in self.tasks.values() if t.status == 'running']
    
    def wait_for_task(self, task_id: str, timeout: Optional[float] = None) -> AsyncTask:
        """Wait for a task to complete"""
        if task_id not in self.tasks:
            raise ValueError(f"Task not found: {task_id}")
        
        task = self.tasks[task_id]
        
        if task.status == 'completed' or task.status == 'cancelled':
            return task
        
        # Wait for task completion
        start_time = time.time()
        while task.status in ['pending', 'running']:
            if timeout is not None and (time.time() - start_time) > timeout:
                task.status = 'timeout'
                raise TimeoutError(f"Task {task_id} timed out after {timeout}s")
            
            time.sleep(0.1)
        
        return task
    
    def wait_for_all(self, timeout: Optional[float] = None) -> List[AsyncTask]:
        """Wait for all tasks to complete"""
        start_time = time.time()
        
        while any(t.status in ['pending', 'running'] for t in self.tasks.values()):
            if timeout is not None and (time.time() - start_time) > timeout:
                # Cancel remaining tasks
                for task in self.tasks.values():
                    if task.status in ['pending', 'running']:
                        task.status = 'timeout'
                
                raise TimeoutError(f"All tasks timed out after {timeout}s")
            
            time.sleep(0.1)
        
        return self.get_all_tasks()
    
    def filter_tasks(self, filter_func: Callable[[AsyncTask], bool]) -> List[AsyncTask]:
        """Filter tasks by predicate"""
        return [task for task in self.tasks.values() if filter_func(task)]
    
    def add_task_filter(self, name: str, filter_func: Callable[[AsyncTask], bool]):
        """Add task filter"""
        self.task_filters[name] = filter_func
    
    def remove_task_filter(self, name: str):
        """Remove task filter"""
        if name in self.task_filters:
            del self.task_filters[name]
    
    def set_mode(self, mode: AsyncMode):
        """Set async operation mode"""
        self.mode = mode
        
        # Adjust worker behavior based on mode
        if mode == AsyncMode.PRIORITY:
            self.priority_enabled = True
        else:
            self.priority_enabled = False
        
        if mode == AsyncMode.BATCH:
            self.batch_size = 10
        else:
            self.batch_size = 1
    
    def add_event_handler(self, event_name: str, handler: Callable):
        """Add event handler"""
        if event_name not in self.event_handlers:
            self.event_handlers[event_name] = []
        self.event_handlers[event_name].append(handler)
    
    def remove_event_handler(self, event_name: str, handler: Callable):
        """Remove event handler"""
        if event_name in self.event_handlers:
            if handler in self.event_handlers[event_name]:
                self.event_handlers[event_name].remove(handler)
    
    def emit_event(self, event_name: str, data: Any):
        """Emit event to handlers"""
        if event_name in self.event_handlers:
            for handler in self.event_handlers[event_name]:
                try:
                    handler(event_name, data)
                except Exception as e:
                    logger = __import__('logging').getLogger(__name__)
                    logger.error(f"Event handler error: {e}")
    
    def _worker_loop(self):
        """Worker thread loop"""
        while self.running:
            try:
                # Get task from queue
                task = self.task_queue.get(timeout=1.0)
                
                if task is None:
                    # Poison pill, stop worker
                    break
                
                # Execute task
                self._execute_task(task)
                
            except queue.Empty:
                continue
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Worker error: {e}")
    
    def _execute_task(self, task: AsyncTask):
        """Execute a task"""
        task.status = 'running'
        task.scheduled_at = time.time()
        
        try:
            # Execute task with timeout
            if task.timeout:
                # In a real implementation, this would use asyncio or threading
                # For this example, we'll just call it directly
                task.action()
            else:
                task.action()
            
            # Mark task as completed
            task.status = 'completed'
            task.completed_at = time.time()
            
            # Move to completed list
            self.completed_tasks.append(task)
            
            # Call callback if provided
            if task.callback:
                try:
                    task.callback(task)
                except Exception as e:
                    logger = __import__('logging').getLogger(__name__)
                    logger.error(f"Task callback error: {e}")
            
            # Emit completion event
            self.emit_event('task_completed', {
                'task_id': task.id,
                'task': task,
                'duration': task.completed_at - task.scheduled_at
            })
            
        except Exception as e:
            task.status = 'error'
            task.completed_at = time.time()
            
            # Call error callback if provided
            if task.error_callback:
                try:
                    task.error_callback(task, e)
                except Exception as callback_error:
                    logger = __import__('logging').getLogger(__name__)
                    logger.error(f"Task error callback error: {e}")
            
            # Emit error event
            self.emit_event('task_error', {
                'task_id': task.id,
                'task': task,
                'error': str(e),
                'duration': task.completed_at - task.scheduled_at
            })
        
        finally:
            # Clean up
            if task.id in self.tasks:
                del self.tasks[task_id]


class AsyncBrainController:
    """Asynchronous brain controller"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.task_manager = AsyncTaskManager(interface)
        self.control_queue = asyncio.Queue()
        self.command_handlers = {}
    
    async def start(self):
        """Start async brain controller"""
        self.task_manager.start()
        
        # Start control loop
        asyncio.create_task(self._control_loop())
    
    def stop(self):
        """Stop async brain controller"""
        self.task_manager.stop()
    
    async def execute_async(self, action: str, params: Dict[str, Any] = None,
                           priority: int = 0, **kwargs):
        """Execute an action asynchronously"""
        if params is None:
            params = {}
        
        task_id = self.task_manager.submit_task(
            lambda: self._execute_action(action, params, **kwargs),
            priority=priority
        )
        
        return await self._get_task_result(task_id, kwargs.get('timeout', 30.0))
    
    def schedule_async(self, action: str, params: Dict[str, Any] = None,
                      delay: float = 0, priority: int = 0, **kwargs):
        """Schedule an action for later execution"""
        if params is None:
            params = {}
        
        async def delayed_task():
            await asyncio.sleep(delay)
            return await self.execute_async(action, params, priority, **kwargs)
        
        # In a real implementation, this would use asyncio scheduling
        # For this example, we'll create a task with a callback
        def wrapper():
            return asyncio.run(delayed_task())
        
        task_id = self.task_manager.submit_task(
            wrapper,
            priority=priority,
            tags=['scheduled', f'delay_{delay}']
        )
        
        return task_id
    
    async def run_sequence(self, actions: List[Dict[str, Any]], 
                          interval: float = 0, parallel: bool = False):
        """Run a sequence of actions"""
        results = []
        
        for i, action in enumerate(actions):
            if parallel and i < len(actions) - 1:
                # Run next action in parallel
                task = asyncio.create_task(self.execute_async(
                    action['action'],
                    action.get('params', {}),
                    priority=action.get('priority', 0)
                ))
                results.append(task)
            else:
                # Run sequentially
                result = await self.execute_async(
                    action['action'],
                    action.get('params', {}),
                    priority=action.get('priority', 0)
                )
                results.append(result)
            
            if interval > 0 and not parallel:
                await asyncio.sleep(interval)
        
        if parallel:
            # Wait for all parallel tasks
            results = await asyncio.gather(*results, return_exceptions=True)
        
        return results
    
    async def run_experiment(self, experiment_config: Dict[str, Any],
                            callbacks: Optional[Dict[str, Callable]] = None):
        """Run an experiment with configuration"""
        if callbacks is None:
            callbacks = {}
        
        # Setup experiment
        if 'setup' in experiment_config:
            await self.execute_async('setup', experiment_config['setup'])
        
        # Run experiment sequence
        actions = experiment_config.get('actions', [])
        interval = experiment_config.get('interval', 0)
        parallel = experiment_config.get('parallel', False)
        
        results = await self.run_sequence(actions, interval, parallel)
        
        # Teardown if specified
        if 'teardown' in experiment_config:
            await self.execute_async('teardown', experiment_config['teardown'])
        
        # Call completion callback
        if 'completion' in callbacks:
            callbacks['completion'](results)
        
        return results
    
    def add_command_handler(self, command: str, handler: Callable):
        """Add command handler"""
        self.command_handlers[command] = handler
    
    async def _execute_action(self, action: str, params: Dict[str, Any], **kwargs):
        """Execute a brain action"""
        # Handle built-in actions
        if action == 'step':
            dt = params.get('dt', 1.0)
            return await self.interface.run_step(dt, auto=params.get('auto', True))
        
        elif action == 'run':
            duration = params.get('duration', 10.0)
            dt = params.get('dt', 1.0)
            return await self.interface.run_loop(duration, dt, params.get('auto', True))
        
        elif action == 'inject':
            stimulus_type = params.get('type', 'current')
            stimulus_params = params.get('parameters', {})
            self.interface.inject_stimulus(stimulus_type, stimulus_params)
            return {'status': 'success', 'stimulus': stimulus_type}
        
        elif action == 'save':
            filepath = params.get('filepath', 'brain_state.json')
            return {'status': self.interface.save_brain_state(filepath)}
        
        elif action == 'load':
            filepath = params.get('filepath', 'brain_state.json')
            return {'status': self.interface.load_brain_state(filepath)}
        
        elif action == 'reset':
            self.interface.reset_system()
            return {'status': 'success'}
        
        # Handle custom commands
        elif action in self.command_handlers:
            return self.command_handlers[action](params)
        
        else:
            raise ValueError(f"Unknown action: {action}")
    
    async def _get_task_result(self, task_id: str, timeout: float) -> Any:
        """Get task result with timeout"""
        try:
            task = await asyncio.wait_for(
                self._wait_for_task_result(task_id),
                timeout=timeout
            )
            return task
        except asyncio.TimeoutError:
            raise TimeoutError(f"Task {task_id} timed out after {timeout}s")
    
    async def _wait_for_task_result(self, task_id: str) -> Any:
        """Wait for task result (simplified)"""
        # In a real implementation, this would wait for the task manager
        # For this example, we'll return a mock result
        await asyncio.sleep(0.1)
        
        # Simulate task completion
        return {
            'task_id': task_id,
            'status': 'completed',
            'result': 'Mock result'
        }
    
    def _control_loop(self):
        """Control loop for async operations"""
        while True:
            try:
                # Process control commands
                # In a real implementation, this would process a queue
                time.sleep(0.1)
                
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Control loop error: {e}")
                time.sleep(1.0)


class AsyncSimulationController:
    """Asynchronous simulation controller"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.simulation_task = None
        self.simulation_running = False
        self.step_callbacks = []
        self.error_handlers = []
    
    async def start_simulation(self, duration: float, dt: float = 1.0,
                              auto: bool = True) -> AsyncSimulation:
        """Start asynchronous simulation"""
        if self.simulation_running:
            raise RuntimeError("Simulation already running")
        
        self.simulation_running = True
        
        # Create simulation object
        simulation = AsyncSimulation(self.interface, duration, dt, auto)
        
        # Start simulation task
        self.simulation_task = asyncio.create_task(
            simulation.run(),
            name=f"simulation_{int(time.time() * 1000)}"
        )
        
        return simulation
    
    def pause_simulation(self):
        """Pause simulation"""
        if self.simulation_task:
            self.simulation_task.cancel()
        
        self.simulation_running = False
        self.simulation_task = None
    
    def resume_simulation(self) -> AsyncSimulation:
        """Resume simulation"""
        if self.simulation_running:
            raise RuntimeError("Simulation already running")
        
        # Note: This is simplified - in practice, you'd need to track
        # paused simulations and resume them
        raise NotImplementedError("Resume not yet implemented")
    
    def stop_simulation(self):
        """Stop simulation"""
        if self.simulation_task:
            self.simulation_task.cancel()
        
        self.simulation_running = False
        self.simulation_task = None
    
    def add_step_callback(self, callback: Callable):
        """Add step callback"""
        self.step_callbacks.append(callback)
    
    def remove_step_callback(self, callback: Callable):
        """Remove step callback"""
        if callback in self.step_callbacks:
            self.step_callbacks.remove(callback)
    
    def add_error_handler(self, handler: Callable):
        """Add error handler"""
        self.error_handlers.append(handler)
    
    def remove_error_handler(self, handler: Callable):
        """Remove error handler"""
        if handler in self.error_handlers:
            self.error_handlers.remove(handler)
    
    def get_simulation_status(self) -> Dict[str, Any]:
        """Get simulation status"""
        return {
            'running': self.simulation_running,
            'task': self.simulation_task,
            'step_callbacks': len(self.step_callbacks),
            'error_handlers': len(self.error_handlers)
        }


class AsyncSimulation:
    """Asynchronous simulation runner"""
    
    def __init__(self, interface: ScriptingInterface, duration: float,
                 dt: float = 1.0, auto: bool = True):
        self.interface = interface
        self.context = interface.context
        self.duration = duration
        self.dt = dt
        self.auto = auto
        self.steps_completed = 0
        self.start_time = time.time()
        self.completion_callbacks = []
        self.error_callbacks = []
    
    async def run(self):
        """Run the simulation"""
        steps = int(self.duration / self.dt)
        
        try:
            for i in range(steps):
                if not self.context.running:
                    break
                
                # Execute step
                result = await self.interface.run_step(self.dt, self.auto)
                
                # Update state
                self.steps_completed = i + 1
                
                # Call step callbacks
                for callback in self.step_callbacks:
                    try:
                        callback(self, result)
                    except Exception as e:
                        logger = __import__('logging').getLogger(__name__)
                        logger.error(f"Step callback error: {e}")
                
                # Small delay to prevent busy waiting
                await asyncio.sleep(0.01)
            
            # Simulation completed
            self._complete_simulation()
            
        except asyncio.CancelledError:
            # Simulation was cancelled
            self._cancel_simulation()
        except Exception as e:
            # Simulation error
            self._error_simulation(e)
    
    def add_completion_callback(self, callback: Callable):
        """Add completion callback"""
        self.completion_callbacks.append(callback)
    
    def remove_completion_callback(self, callback: Callable):
        """Remove completion callback"""
        if callback in self.completion_callbacks:
            self.completion_callbacks.remove(callback)
    
    def add_error_callback(self, callback: Callable):
        """Add error callback"""
        self.error_callbacks.append(callback)
    
    def remove_error_callback(self, callback: Callable):
        """Remove error callback"""
        if callback in self.error_callbacks:
            self.error_callbacks.remove(callback)
    
    def _complete_simulation(self):
        """Handle simulation completion"""
        self.context.emit_event(ScriptingEvent(
            EventType.SIMULATION_STOPPED,
            time.time(),
            data={
                'steps_completed': self.steps_completed,
                'duration': self.duration,
                'actual_duration': time.time() - self.start_time
            }
        ))
        
        # Call completion callbacks
        for callback in self.completion_callbacks:
            try:
                callback(self)
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Completion callback error: {e}")
    
    def _cancel_simulation(self):
        """Handle simulation cancellation"""
        self.context.emit_event(ScriptingEvent(
            EventType.STATE_CHANGED,
            time.time(),
            data={
                'action': 'cancelled',
                'steps_completed': self.steps_completed
            }
        ))
    
    def _error_simulation(self, error: Exception):
        """Handle simulation error"""
        self.context.emit_event(ScriptingEvent(
            EventType.ERROR_OCCURRED,
            time.time(),
            data={
                'error': str(error),
                'steps_completed': self.steps_completed
            }
        ))


# Export key classes and functions
__all__ = [
    'AsyncMode',
    'AsyncTask',
    'AsyncTaskManager',
    'AsyncBrainController',
    'AsyncSimulationController',
    'AsyncSimulation'
]
