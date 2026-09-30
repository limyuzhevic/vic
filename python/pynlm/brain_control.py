pynlm/brain_control.py
Name: Brain control and monitoring module
Summary: Provides real-time brain state access and manipulation capabilities

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType
import time
from typing import Dict, List, Any, Optional


class BrainController:
    """Advanced brain controller with real-time monitoring and control"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.monitoring = False
        self.monitoring_thread = None
        self.monitoring_interval = 1.0
        self.callbacks = []
        self.neuron_subscribers = {}
        self.region_subscribers = {}
    
    def start_monitoring(self, interval: float = 1.0, 
                        callbacks: Optional[List[Callable]] = None):
        """Start real-time monitoring of brain state"""
        self.monitoring = True
        self.monitoring_interval = interval
        
        if callbacks:
            self.callbacks.extend(callbacks)
        
        self.monitoring_thread = threading.Thread(
            target=self._monitoring_loop,
            daemon=True
        )
        self.monitoring_thread.start()
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Started brain monitoring with {interval}s interval")
    
    def stop_monitoring(self):
        """Stop real-time monitoring"""
        self.monitoring = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
        
        logger = __import__('logging').getLogger(__name__)
        logger.info("Stopped brain monitoring")
    
    def subscribe_to_neuron(self, neuron_id: int, callback: Callable,
                          event_types: Optional[List[EventType]] = None):
        """Subscribe to neuron-specific events"""
        if neuron_id not in self.neuron_subscribers:
            self.neuron_subscribers[neuron_id] = []
        
        if event_types is None:
            event_types = [EventType.NEURON_FIRED, EventType.STATE_CHANGED]
        
        self.neuron_subscribers[neuron_id].append((callback, event_types))
    
    def subscribe_to_region(self, region_id: int, callback: Callable,
                          event_types: Optional[List[EventType]] = None):
        """Subscribe to region-specific events"""
        if region_id not in self.region_subscribers:
            self.region_subscribers[region_id] = []
        
        if event_types is None:
            event_types = [EventType.STATE_CHANGED, EventType.SYNAPSE_CHANGED]
        
        self.region_subscribers[region_id].append((callback, event_types))
    
    def inject_stimulus_advanced(self, stimulus: Dict[str, Any]) -> Dict[str, Any]:
        """Advanced stimulus injection with validation"""
        if not self.context.brain:
            raise ValueError("Brain not initialized")
        
        stimulus_type = stimulus.get('type')
        parameters = stimulus.get('parameters', {})
        
        # Validate parameters
        if stimulus_type not in ['current', 'type', 'sensory']:
            raise ValueError(f"Unknown stimulus type: {stimulus_type}")
        
        # Apply validation based on type
        if stimulus_type == 'current':
            if 'neuron_id' not in parameters:
                parameters['neuron_id'] = 0
            if 'current' not in parameters:
                parameters['current'] = 1.0
        
        # Execute stimulus
        result = self.interface.inject_stimulus(stimulus_type, parameters)
        
        # Log the stimulus
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Stimulus {stimulus_type} injected: {parameters}")
        
        return {'success': result, 'stimulus': stimulus}
    
    def get_brain_statistics(self) -> Dict[str, Any]:
        """Get detailed brain statistics"""
        if not self.context.brain:
            return {}
        
        brain = self.context.brain
        regions = brain.getRegions()
        
        stats = {
            'basic': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount(),
                'developmental_stage': brain.getDevelopmentalStage()
            },
            'regions': [],
            'neuron_types': {},
            'activity_distribution': {}
        }
        
        # Region statistics
        for region in regions:
            region_id = region.getId()
            region_neurons = []
            
            for neuron in region.getNeurons():
                region_neurons.append({
                    'id': neuron.getId(),
                    'type': neuron.getType(),
                    'firing_state': neuron.getFiringState(),
                    'potential': neuron.getMembranePotential(),
                    'refractory_time': neuron.getRefractoryTime()
                })
            
            stats['regions'].append({
                'id': region_id,
                'neuron_count': len(region_neurons),
                'active_count': sum(1 for n in region_neurons if n['firing_state'] == 'Active'),
                'neurons': region_neurons
            })
        
        # Neuron type statistics
        for neuron in regions[0].getNeurons() if regions else []:
            neuron_type = neuron.getType()
            if neuron_type not in stats['neuron_types']:
                stats['neuron_types'][neuron_type] = {
                    'count': 0,
                    'active_count': 0,
                    'firing_count': 0
                }
            
            stats['neuron_types'][neuron_type]['count'] += 1
            if neuron.getFiringState() == 'Active':
                stats['neuron_types'][neuron_type]['active_count'] += 1
            if neuron.getFiringState() == 'Firing':
                stats['neuron_types'][neuron_type]['firing_count'] += 1
        
        return stats
    
    def get_synaptic_activity(self) -> Dict[str, Any]:
        """Get synaptic activity statistics"""
        if not self.context.brain:
            return {}
        
        brain = self.context.brain
        regions = brain.getRegions()
        
        activity = {
            'total_strength': 0.0,
            'active_synapses': 0,
            'by_type': {},
            'by_region': {},
            'strength_distribution': []
        }
        
        for region in regions:
            region_id = region.getId()
            activity['by_region'][region_id] = {
                'total_strength': 0.0,
                'active_synapses': 0,
                'synapse_count': 0
            }
            
            for neuron in region.getNeurons():
                for synapse in neuron.getSynapses():
                    strength = synapse.getStrength()
                    activity['total_strength'] += strength
                    activity['strength_distribution'].append(strength)
                    
                    activity['by_region'][region_id]['total_strength'] += strength
                    activity['by_region'][region_id]['synapse_count'] += 1
                    
                    if synapse.getActive():
                        activity['active_synapses'] += 1
                        activity['by_region'][region_id]['active_synapses'] += 1
                    
                    synapse_type = synapse.getType()
                    if synapse_type not in activity['by_type']:
                        activity['by_type'][synapse_type] = {
                            'total_strength': 0.0,
                            'active_count': 0,
                            'count': 0
                        }
                    
                    activity['by_type'][synapse_type]['total_strength'] += strength
                    activity['by_type'][synapse_type]['count'] += 1
                    if synapse.getActive():
                        activity['by_type'][synapse_type]['active_count'] += 1
        
        return activity
    
    def _monitoring_loop(self):
        """Internal monitoring loop"""
        while self.monitoring:
            try:
                # Get current state
                state = self.interface.get_state()
                
                # Call all monitoring callbacks
                for callback in self.callbacks:
                    try:
                        callback(state)
                    except Exception as e:
                        logger = __import__('logging').getLogger(__name__)
                        logger.error(f"Monitoring callback error: {e}")
                
                # Process event queue
                while not self.context.event_queue.empty():
                    event = self.context.event_queue.get()
                    
                    # Check neuron-specific subscriptions
                    for neuron_id, subscribers in self.neuron_subscribers.items():
                        for callback, event_types in subscribers:
                            if event.event_type in event_types:
                                try:
                                    callback(event, neuron_id)
                                except Exception as e:
                                    logger = __import__('logging').getLogger(__name__)
                                    logger.error(f"Neuron subscriber error: {e}")
                    
                    # Check region-specific subscriptions
                    for region_id, subscribers in self.region_subscribers.items():
                        for callback, event_types in subscribers:
                            if event.event_type in event_types:
                                try:
                                    callback(event, region_id)
                                except Exception as e:
                                    logger = __import__('logging').getLogger(__name__)
                                    logger.error(f"Region subscriber error: {e}")
                
                time.sleep(self.monitoring_interval)
                
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Monitoring loop error: {e}")
                time.sleep(1.0)


class ConfigManager:
    """Configuration management for NLM system"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.config_history = []
    
    def load_config_file(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from file"""
        config = self.context.config
        config.loadFromFile(filepath)
        
        self.config_history.append({
            'action': 'load',
            'filepath': filepath,
            'timestamp': time.time(),
            'config': config.summary()
        })
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Configuration loaded from {filepath}")
        
        return config.summary()
    
    def save_config_file(self, filepath: str, format: str = 'json') -> bool:
        """Save configuration to file"""
        config = self.context.config
        
        if format.lower() == 'json':
            config.saveToFile(filepath)
        elif format.lower() == 'python':
            # Save as Python dictionary
            with open(filepath, 'w') as f:
                f.write(f"config = {config.summary()}")
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        self.config_history.append({
            'action': 'save',
            'filepath': filepath,
            'format': format,
            'timestamp': time.time()
        })
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Configuration saved to {filepath}")
        
        return True
    
    def get_config_summary(self) -> Dict[str, Any]:
        """Get configuration summary"""
        if self.context.config:
            return {
                'summary': self.context.config.summary(),
                'history': self.config_history[-10:]  # Last 10 actions
            }
        return {}
    
    def compare_configs(self, config1: str, config2: str) -> Dict[str, Any]:
        """Compare two configurations"""
        # This would normally load and compare the actual configs
        # For this example, we'll return mock data
        return {
            'identical': False,
            'differences': ['param1 differs', 'param2 differs'],
            'similarity': 0.75
        }
    
    def validate_config(self, config_dict: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration dictionary"""
        # Basic validation - in practice this would be more comprehensive
        errors = []
        warnings = []
        
        for key, value in config_dict.items():
            # Basic type checking and value validation
            if not isinstance(key, str):
                errors.append(f"Invalid key type: {key}")
            
            # Add specific validation rules based on key
            if 'neuron' in key.lower() and value < 0:
                warnings.append(f"Negative neuron count for {key}")
        
        return {
            'valid': len(errors) == 0,
            'errors': errors,
            'warnings': warnings
        }
