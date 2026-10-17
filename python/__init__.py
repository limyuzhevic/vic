#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Advanced Features Package
=====================================================

This package provides advanced capabilities for research scientists, AI/ML researchers,
and computational neuroscience students working with the NLM system. It includes
sophisticated tools for neural network research, experimentation, and analysis.

Key Features:
- Advanced neural network architectures (ring, small-world, scale-free networks)
- Complex reward structures and learning scenarios
- Advanced visualization and analysis tools
- Batch processing and automation
- Research-grade configuration presets
- Performance profiling and monitoring
- Custom neuromodulation patterns
- Advanced learning algorithms (Bayesian, meta-learning, RL)
- Scripting and automation tools
- Extension API for plugins and custom components
"""

import time
import json
import numpy as np
from typing import Dict, List, Optional, Any, Union
from pathlib import Path

# Import all advanced feature modules
try:
    from .advanced_features import AdvancedNLMFeatures
    from .configuration_presets import AdvancedConfigurationPresets, PerformanceOptimizedPresets
    from .performance_monitoring import PerformanceProfiler, RealTimeMonitor
    from .extension_api import ExtensionAPI, ScriptedNLM
    
    # Import core NLM functionality
    import pynlm
    
    # Version and package info
    __version__ = "2.0.0-advanced"
    __author__ = "NLM Advanced Features Team"
    __description__ = "Advanced NLM features for power users, researchers, and developers"
    
    # Global instances for convenience
    _default_advanced_features = None
    _default_extension_api = None
    _default_session_manager = None
    
    def get_advanced_features(config: Optional[Dict[str, Any]] = None) -> 'AdvancedNLMFeatures':
        """
        Get or create the default advanced features instance.
        
        Args:
            config: Optional configuration dictionary
            
        Returns:
            NLMAdvancedFeatures instance
        """
        global _default_advanced_features
        
        if _default_advanced_features is None or config:
            _default_advanced_features = AdvancedNLMFeatures(config)
        
        return _default_advanced_features
    
    def get_extension_api() -> ExtensionAPI:
        """
        Get or create the default extension API instance.
        
        Args:
            plugin_directory: Optional directory for plugins
            
        Returns:
            ExtensionAPI instance
        """
        global _default_extension_api
        
        if _default_extension_api is None:
            _default_extension_api = ExtensionAPI()
        
        return _default_extension_api
    
    # Convenience functions for quick access to advanced features
    def create_ring_network(num_neurons: int, **kwargs) -> Dict[str, Any]:
        """Create a ring-structured neural network."""
        features = get_advanced_features()
        return features.create_advanced_neural_network("ring", num_neurons=num_neurons, **kwargs)
    
    def create_small_world_network(num_neurons: int, **kwargs) -> Dict[str, Any]:
        """Create a small-world neural network."""
        features = get_advanced_features()
        return features.create_advanced_neural_network("small_world", num_neurons=num_neurons, **kwargs)
    
    def create_scale_free_network(num_neurons: int, **kwargs) -> Dict[str, Any]:
        """Create a scale-free neural network."""
        features = get_advanced_features()
        return features.create_advanced_neural_network("scale_free", num_neurons=num_neurons, **kwargs)
    
    def create_research_preset(research_domain: str, **kwargs) -> Dict[str, Any]:
        """Create a research-grade configuration preset."""
        features = get_advanced_features()
        preset = features.get_configuration_presets()
        return preset.get(research_domain, {})
    
    def run_performance_profiler(brain: Any, sampling_interval: float = 0.1) -> PerformanceProfiler:
        """Run performance profiler on a brain simulation."""
        features = get_advanced_features()
        profiler = features.create_performance_profiler()
        profiler.start_profiling(sampling_interval)
        return profiler
    
    def analyze_learning_trajectory(rewards: List[float], novelty: List[float],
                                   curiosity: List[float],
                                   prediction_errors: List[float]) -> Dict[str, Any]:
        """Analyze learning trajectory from experiment data."""
        features = get_advanced_features()
        return features.analyze_learning_trajectory(
            rewards, novelty, curiosity, prediction_errors
        )
    
    # Export main classes and functions
    __all__ = [
        # Core classes
        'AdvancedNLMFeatures',
        'AdvancedConfigurationPresets',
        'PerformanceOptimizedPresets',
        'PerformanceProfiler',
        'RealTimeMonitor',
        'ExtensionAPI',
        'ScriptedNLM',
        
        # Convenience functions
        'create_ring_network',
        'create_small_world_network',
        'create_scale_free_network',
        'create_research_preset',
        'run_performance_profiler',
        'analyze_learning_trajectory',
        'get_advanced_features',
        'get_extension_api',
        
        # Constants
        '__version__',
        '__author__',
        '__description__'
    ]
    
except ImportError as e:
    # Handle import errors gracefully
    print(f"Warning: Failed to import some advanced features: {e}")
    print("Falling back to basic NLM functionality")
    
    # Define basic classes for backward compatibility
    class AdvancedNLMFeatures:
        """Basic advanced features implementation (limited functionality)."""
        pass
    
    class AdvancedConfigurationPresets:
        """Basic configuration presets."""
        pass
    
    class PerformanceProfiler:
        """Basic performance profiler."""
        pass
    
    class ExtensionAPI:
        """Basic extension API."""
        pass
    
    class ScriptedNLM:
        """Basic scripted NLM interface."""
        pass
class NLMAdvancedFeatures:
    """
    Main class providing unified access to all advanced NLM features.
    
    This class integrates all advanced features into a cohesive interface
    for power users, researchers, and developers working with NLM.
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize advanced NLM features with configuration.
        
        Args:
            config: Configuration dictionary for advanced features
        """
        self.config = config or {}
        self._advanced_features = AdvancedNLMFeatures(config)
        self._config_presets = AdvancedConfigurationPresets()
        self._perf_presets = PerformanceOptimizedPresets()
        self._extension_api = ExtensionAPI()
        self._scripted_nlm = ScriptedNLM(self._extension_api)
        
        # Performance monitoring
        self._profiler = None
        self._monitor = None
        
    def create_advanced_neural_network(self, network_type: str, **kwargs) -> Dict[str, Any]:
        """
        Create advanced neural network architectures.
        
        Args:
            network_type: Type of network architecture (ring, small_world, scale_free, etc.)
            **kwargs: Architecture-specific parameters
            
        Returns:
            Dictionary containing the created brain and metadata
        """
        if network_type == "ring":
            return self._advanced_features.create_ring_network(**kwargs)
        elif network_type == "small_world":
            return self._advanced_features.create_small_world_network(**kwargs)
        elif network_type == "scale_free":
            return self._advanced_features.create_scale_free_network(**kwargs)
        else:
            raise ValueError(f"Unknown network type: {network_type}")
    
    def run_advanced_learning_experiment(self, brain: Any, world: Any,
                                        reward_schedule: Optional[Dict[str, Any]] = None,
                                        num_steps: int = 1000) -> Dict[str, Any]:
        """
        Run advanced learning experiments with complex reward structures.
        
        Args:
            brain: NLM brain instance
            world: World instance
            reward_schedule: Reward schedule configuration (optional)
            num_steps: Number of simulation steps
            
        Returns:
            Dictionary containing experiment results and analysis
        """
        from .advanced_features import ComplexRewardSchedule
        
        if reward_schedule is None:
            reward_schedule = ComplexRewardSchedule()
        else:
            reward_schedule = ComplexRewardSchedule(**reward_schedule)
        
        return self._advanced_features.run_complex_reward_experiment(
            brain, world, reward_schedule, num_steps
        )
    
    def analyze_learning_trajectory(self, rewards: List[float], novelty: List[float],
                                   curiosity: List[float],
                                   prediction_errors: List[float]) -> Dict[str, Any]:
        """
        Perform advanced analysis of learning trajectories.
        
        Args:
            rewards: List of reward values
            novelty: List of novelty scores
            curiosity: List of curiosity levels
            prediction_errors: List of prediction error values
            
        Returns:
            Dictionary containing trajectory analysis
        """
        return self._advanced_features.analyze_learning_trajectory(
            rewards, novelty, curiosity, prediction_errors
        )
    
    def create_experiment_runner(self, num_workers: Optional[int] = None) -> Any:
        """
        Create a batch experiment runner.
        
        Args:
            num_workers: Number of worker processes (optional)
            
        Returns:
            BatchExperimentRunner instance
        """
        return self._advanced_features.create_batch_experiment_runner(num_workers)
    
    def create_performance_profiler(self) -> PerformanceProfiler:
        """
        Create a performance profiler.
        
        Returns:
            PerformanceProfiler instance
        """
        self._profiler = PerformanceProfiler()
        return self._profiler
    
    def start_monitoring(self, sampling_interval: float = 0.1):
        """
        Start real-time performance monitoring.
        
        Args:
            sampling_interval: Monitoring sampling interval
        """
        if self._profiler:
            self._profiler.start_profiling(sampling_interval)
    
    def stop_monitoring(self) -> Dict[str, Any]:
        """
        Stop performance monitoring and get analysis.
        
        Returns:
            Performance analysis results
        """
        if self._profiler:
            return self._profiler.stop_profiling()
        return {"error": "Profiler not initialized"}
    
    def create_neuromodulation_pattern(self, pattern_type: str,
                                      parameters: Optional[Dict[str, Any]] = None) -> Any:
        """
        Create custom neuromodulation patterns.
        
        Args:
            pattern_type: Type of neuromodulation pattern
            parameters: Pattern parameters (optional)
            
        Returns:
            Neuromodulation pattern instance
        """
        return self._advanced_features.create_neuromodulation_patterns(
            pattern_type, parameters
        )
    
    def create_learning_algorithms(self) -> Any:
        """
        Create advanced learning algorithms.
        
        Returns:
            AdvancedLearningAlgorithms instance
        """
        return self._advanced_features.create_advanced_learning_algorithms()
    
    def get_configuration_presets(self) -> Dict[str, Any]:
        """
        Get advanced configuration presets.
        
        Returns:
            Dictionary containing all configuration presets
        """
        presets = {}
        
        # Add research presets
        research_presets = self._config_presets.get_all_presets()
        for name, preset in research_presets.items():
            presets[f"research_{name}"] = {
                "name": preset.name,
                "description": preset.description,
                "domain": preset.domain.value,
                "config": preset.config
            }
        
        # Add performance presets
        hardware_preset = self._perf_presets.get_preset_for_hardware(
            cpu_cores=8,
            memory_mb=16000,
            has_gpu=True
        )
        presets["performance_optimized"] = {
            "name": hardware_preset.name,
            "description": hardware_preset.description,
            "domain": hardware_preset.domain.value,
            "config": hardware_preset.config
        }
        
        return presets
    
    def apply_configuration_preset(self, preset_name: str, brain: Any) -> Any:
        """
        Apply a configuration preset to a brain.
        
        Args:
            preset_name: Name of the preset
            brain: Brain instance to apply configuration to
            
        Returns:
            Applied configuration
        """
        # Map preset names to actual preset keys
        preset_mapping = {
            "cognitive_development": "research_cognitive_development",
            "social_learning": "research_social_learning",
            "cognitive_architecture": "research_cognitive_architecture",
            "behavioral_modeling": "research_behavioral_modeling",
            "neuroscience_research": "research_neuroscience_research",
            "high_end_performance": "performance_optimized"
        }
        
        preset_key = preset_mapping.get(preset_name, preset_name)
        
        if preset_key.startswith("research_"):
            preset_name_only = preset_key[8:]  # Remove "research_" prefix
            preset = self._config_presets.get_preset(preset_name_only)
            if preset:
                return preset.apply_to_brain()
        elif preset_key == "performance_optimized":
            preset = self._perf_presets.get_preset_for_hardware(
                cpu_cores=8,
                memory_mb=16000,
                has_gpu=True
            )
            return preset.apply_to_brain()
        
        raise ValueError(f"Preset not found: {preset_name}")
    
    def get_extension_api(self) -> ExtensionAPI:
        """
        Get the extension API instance.
        
        Returns:
            ExtensionAPI instance
        """
        return self._extension_api
    
    def get_scripted_nlm(self) -> ScriptedNLM:
        """
        Get the scripted NLM interface.
        
        Returns:
            ScriptedNLM instance
        """
        return self._scripted_nlm
    
    def execute_script(self, script: str, variables: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Execute a script using the scripted NLM interface.
        
        Args:
            script: Python script to execute
            variables: Variables to make available in script
            
        Returns:
            Execution results
        """
        return self._scripted_nlm.execute_script(script, variables)
    
    def create_macro(self, name: str, steps: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Create a macro for batch operations.
        
        Args:
            name: Macro name
            steps: List of macro steps
            
        Returns:
            Macro definition
        """
        return self._scripted_nlm.create_macro(name, steps)
    
    def execute_macro(self, macro: Dict[str, Any]) -> Dict[str, Any]:
        """
        Execute a macro.
        
        Args:
            macro: Macro definition
            
        Returns:
            Execution results
        """
        return self._scripted_nlm.execute_macro(macro)
    
    def export_configuration(self, filepath: str, format: str = "json"):
        """
        Export current configuration to file.
        
        Args:
            filepath: Path to export file
            format: Export format (json, yaml)
        """
        if format.lower() == "json":
            config_data = {
                "advanced_features_config": self.config,
                "presets": self.get_configuration_presets(),
                "timestamp": time.time()
            }
            
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2, default=str)
        elif format.lower() in ["yaml", "yml"]:
            import yaml
            config_data = {
                "advanced_features_config": self.config,
                "presets": self.get_configuration_presets(),
                "timestamp": time.time()
            }
            
            with open(filepath, 'w') as f:
                yaml.dump(config_data, f, default_flow_style=False)
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    def import_configuration(self, filepath: str) -> Dict[str, Any]:
        """
        Import configuration from file.
        
        Args:
            filepath: Path to import file
            
        Returns:
            Imported configuration
        """
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                return json.load(f)
        elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
            import yaml
            with open(filepath, 'r') as f:
                return yaml.safe_load(f)
        else:
            raise ValueError("Unsupported file format. Use .json or .yaml/.yml")
class NLMSessionManager:
    """
    Session manager for advanced NLM features.
    
    Manages multiple sessions, saves state, and provides session management
    capabilities for research workflows.
    """
    
    def __init__(self):
        self.sessions: Dict[str, Dict[str, Any]] = {}
        self.session_history = []
        
    def create_session(self, name: str, config: Optional[Dict[str, Any]] = None) -> str:
        """
        Create a new session.
        
        Args:
            name: Session name
            config: Optional session configuration
            
        Returns:
            Session ID
        """
        session_id = f"session_{len(self.sessions) + 1}_{int(time.time())}"
        
        session_data = {
            "id": session_id,
            "name": name,
            "config": config or {},
            "created_at": time.time(),
            "advanced_features": NLMAdvancedFeatures(config),
            "brain": None,
            "world": None,
            "results": {}
        }
        
        self.sessions[session_id] = session_data
        self.session_history.append({
            "action": "create_session",
            "session_id": session_id,
            "name": name,
            "timestamp": time.time()
        })
        
        return session_id
    
    def get_session(self, session_id: str) -> Optional[Dict[str, Any]]:
        """
        Get session data.
        
        Args:
            session_id: Session ID
            
        Returns:
            Session data or None if not found
        """
        return self.sessions.get(session_id)
    
    def update_session(self, session_id: str, updates: Dict[str, Any]) -> bool:
        """
        Update session data.
        
        Args:
            session_id: Session ID
            updates: Updates to apply
            
        Returns:
            True if session updated, False if not found
        """
        if session_id in self.sessions:
            self.sessions[session_id].update(updates)
            
            # Record update
            self.session_history.append({
                "action": "update_session",
                "session_id": session_id,
                "updates": updates,
                "timestamp": time.time()
            })
            
            return True
        return False
    
    def save_session(self, session_id: str, filepath: str):
        """
        Save session to file.
        
        Args:
            session_id: Session ID
            filepath: Path to save file
        """
        if session_id not in self.sessions:
            raise ValueError(f"Session {session_id} not found")
        
        session_data = self.sessions[session_id].copy()
        
        # Don't save the advanced features objects (not serializable)
        session_data.pop('advanced_features', None)
        
        # Convert to JSON-serializable format
        session_data['created_at'] = session_data['created_at']
        
        with open(filepath, 'w') as f:
            json.dump(session_data, f, indent=2, default=str)
    
    def load_session(self, filepath: str) -> str:
        """
        Load session from file.
        
        Args:
            filepath: Path to load file
            
        Returns:
            Session ID
        """
        with open(filepath, 'r') as f:
            session_data = json.load(f)
        
        session_id = session_data.pop('id')
        session_data['created_at'] = session_data.get('created_at', time.time())
        
        # Create advanced features instance
        config = session_data.get('config', {})
        session_data['advanced_features'] = NLMAdvancedFeatures(config)
        
        self.sessions[session_id] = session_data
        
        # Record load
        self.session_history.append({
            "action": "load_session",
            "session_id": session_id,
            "source": filepath,
            "timestamp": time.time()
        })
        
        return session_id
    
    def list_sessions(self) -> List[Dict[str, Any]]:
        """
        List all sessions.
        
        Returns:
            List of session information
        """
        session_info = []
        for session_id, session_data in self.sessions.items():
            session_info.append({
                "id": session_id,
                "name": session_data.get('name', 'Unnamed'),
                "created_at": session_data.get('created_at'),
                "has_brain": session_data.get('brain') is not None
            })
        
        return session_info
    
    def delete_session(self, session_id: str) -> bool:
        """
        Delete a session.
        
        Args:
            session_id: Session ID
            
        Returns:
            True if session deleted, False if not found
        """
        if session_id in self.sessions:
            del self.sessions[session_id]
            
            # Record deletion
            self.session_history.append({
                "action": "delete_session",
                "session_id": session_id,
                "timestamp": time.time()
            })
            
            return True
        return False
    
    def get_session_history(self, limit: Optional[int] = None) -> List[Dict[str, Any]]:
        """
        Get session history.
        
        Args:
            limit: Maximum number of entries to return
            
        Returns:
            Session history
        """
        history = self.session_history.copy()
        
        if limit:
            history = history[-limit:]
        
        return history
# Global instances for convenience
_default_advanced_features: Optional[NLMAdvancedFeatures] = None
_default_session_manager: Optional[NLMSessionManager] = None

def get_advanced_features(config: Optional[Dict[str, Any]] = None) -> NLMAdvancedFeatures:
    """
    Get or create the default advanced features instance.
    
    Args:
        config: Optional configuration
        
    Returns:
        NLMAdvancedFeatures instance
    """
    global _default_advanced_features
    
    if _default_advanced_features is None or config:
        _default_advanced_features = NLMAdvancedFeatures(config)
    
    return _default_advanced_features

def get_session_manager() -> NLMSessionManager:
    """
    Get or create the default session manager.
    
    Returns:
        NLMSessionManager instance
    """
    global _default_session_manager
    
    if _default_session_manager is None:
        _default_session_manager = NLMSessionManager()
    
    return _default_session_manager
# Convenience functions for quick access
def create_ring_network(num_neurons: int, **kwargs) -> Dict[str, Any]:
    """Create a ring network using advanced features."""
    features = get_advanced_features()
    return features.create_advanced_neural_network("ring", num_neurons=num_neurons, **kwargs)

def create_small_world_network(num_neurons: int, **kwargs) -> Dict[str, Any]:
    """Create a small-world network using advanced features."""
    features = get_advanced_features()
    return features.create_advanced_neural_network("small_world", num_neurons=num_neurons, **kwargs)

def create_scale_free_network(num_neurons: int, **kwargs) -> Dict[str, Any]:
    """Create a scale-free network using advanced features."""
    features = get_advanced_features()
    return features.create_advanced_neural_network("scale_free", num_neurons=num_neurons, **kwargs)

if __name__ == "__main__":
    # Example usage
    print("Initializing NLM Advanced Features...")
    
    # Create advanced features instance
    features = NLMAdvancedFeatures()
    
    # Create different network architectures
    print("\n1. Creating ring network...")
    ring_network = features.create_advanced_neural_network(
        "ring", num_neurons=1000, connection_probability=0.3
    )
    print(f"   Ring network created with {ring_network['metadata']['num_neurons']} neurons")
    
    print("\n2. Creating small-world network...")
    small_world_network = features.create_advanced_neural_network(
        "small_world", num_neurons=500, k=4, p=0.3
    )
    print(f"   Small-world network created with {small_world_network['metadata']['num_neurons']} neurons")
    
    print("\n3. Creating scale-free network...")
    scale_free_network = features.create_advanced_neural_network(
        "scale_free", num_neurons=2000, gamma=2.5
    )
    print(f"   Scale-free network created with {scale_free_network['metadata']['num_neurons']} neurons")
    
    # Get configuration presets
    print("\n4. Getting configuration presets...")
    presets = features.get_configuration_presets()
    print(f"   Found {len(presets)} configuration presets")
    
    # Create performance profiler
    print("\n5. Creating performance profiler...")
    profiler = features.create_performance_profiler()
    print("   Performance profiler created")
    
    # Start monitoring
    print("\n6. Starting performance monitoring...")
    features.start_monitoring(0.5)
    
    # Simulate some work
    print("\n7. Simulating work...")
    time.sleep(1)
    
    # Stop monitoring
    print("\n8. Stopping performance monitoring...")
    analysis = features.stop_monitoring()
    print(f"   Analysis complete: {len(analysis)} metrics recorded")
    
    # Create extension API
    print("\n9. Creating extension API...")
    extension_api = features.get_extension_api()
    print("   Extension API created")
    
    # Create scripted NLM
    print("\n10. Creating scripted NLM...")
    scripted_nlm = features.get_scripted_nlm()
    print("   Scripted NLM created")
    
    # Session management
    print("\n11. Testing session management...")
    session_manager = get_session_manager()
    session_id = session_manager.create_session("test_session", {"neurons": 1000})
    print(f"   Session created: {session_id}")
    
    # Export configuration
    print("\n12. Exporting configuration...")
    features.export_configuration("nlm_advanced_config.json")
    print("   Configuration exported to 'nlm_advanced_config.json'")
    
    print("\n" + "="*60)
    print("NLM Advanced Features Demo Completed Successfully!")
    print("="*60)
    print("\nKey Features Demonstrated:")
    print("✓ Advanced neural network architectures (ring, small-world, scale-free)")
    print("✓ Configuration presets for research domains")
    print("✓ Performance profiling and monitoring")
    print("✓ Extension API for plugins")
    print("✓ Session management")
    print("✓ Configuration export/import")
    print("✓ Scripting interface")
