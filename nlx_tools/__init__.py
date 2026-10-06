#!/usr/bin/env python3
"""
NLM - Neural Learning Machine Visualization and Debugging Tools

Comprehensive visualization and debugging tools for NLM that provide sophisticated
capabilities for researchers and developers.

Core tools implemented:
- nlx-viz-activity: Real-time neural activity visualization
- nlx-viz-network: Network connectivity and topology visualization
- nlx-viz-memory: Memory system activity visualization
- nlx-debug-memory: Memory leak detection and visualization
- nlx-debug-performance: Performance profiling and visualization
- nlx-debug-step: Step-by-step execution debugging
- nlx-debug-config: Configuration validation and visualization
"""

import sys
import os
from pathlib import Path

# Add the current directory to the Python path
sys.path.insert(0, str(Path(__file__).parent))

# Import all visualization and debugging modules directly
from nlx_tools.viz_activity import NLActivityVisualizer
from nlx_tools.viz_network import NLNetworkVisualizer
from nlx_tools.viz_memory import NLMemoryVisualizer
from nlx_tools.debug_memory import NLMemoryDebugger
from nlx_tools.debug_performance import NLPerformanceProfiler
from nlx_tools.debug_step import NLStepDebugger
from nlx_tools.debug_config import NLConfigValidator

# For backward compatibility, create aliases
nlx_viz_activity = NLActivityVisualizer
nlx_viz_network = NLNetworkVisualizer
nlx_viz_memory = NLMemoryVisualizer
nlx_debug_memory = NLMemoryDebugger
nlx_debug_performance = NLPerformanceProfiler
nlx_debug_step = NLStepDebugger
nlx_debug_config = NLConfigValidator

# Version and metadata
__version__ = "1.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

# Available tools
AVAILABLE_TOOLS = [
    "nlx-viz-activity",
    "nlx-viz-network", 
    "nlx-viz-memory",
    "nlx-debug-memory",
    "nlx-debug-performance",
    "nlx-debug-step",
    "nlx-debug-config"
]

# Documentation and examples
TOOL_DOCUMENTS = {
    "nlx-viz-activity": {
        "description": "Real-time neural activity visualization",
        "features": [
            "Live neural population activity visualization",
            "Spike train visualization with multiple channels",
            "Population firing rate visualization",
            "Membrane potential visualization",
            "Synaptic activity visualization",
            "Real-time animation of neural dynamics"
        ],
        "example_usage": """
        # Example: Real-time neural activity visualization
        import nlx_tools.viz_activity as viz_activity
        
        # Create activity visualizer with custom settings
        visualizer = viz_activity.NLActivityVisualizer(
            update_rate=60.0,  # Hz
            channels=128,
            colormap="diverging"
        )
        
        # Start real-time visualization
        visualizer.start_real_time_viz(brain_data)
        """
    },
    "nlx-viz-network": {
        "description": "Network connectivity and topology visualization",
        "features": [
            "Neural network graph visualization",
            "Synaptic connectivity matrix visualization",
            "Network topology visualization",
            "Region connectivity visualization",
            "Network dynamics visualization",
            "Community detection visualization"
        ],
        "example_usage": """
        # Example: Network connectivity visualization
        import nlx_tools.viz_network as viz_network
        
        # Create network visualizer
        visualizer = viz_network.NLNetworkVisualizer(
            graph_layout="force_directed",
            node_size_type="degree",
            edge_width_type="strength"
        )
        
        # Visualize brain network
        visualizer.visualize_connectivity(brain.get_connectivity_matrix())
        """
    },
    "nlx-viz-memory": {
        "description": "Memory system activity visualization",
        "features": [
            "Working memory visualization",
            "Episodic memory visualization",
            "Associative memory visualization",
            "Memory encoding/decoding visualization",
            "Memory consolidation visualization"
        ],
        "example_usage": """
        # Example: Memory system visualization
        import nlx_tools.viz_memory as viz_memory
        
        # Create memory visualizer
        visualizer = viz_memory.NLMemoryVisualizer(
            memory_types=["working", "episodic", "associative"],
            show_encoding=True,
            show_decoding=True
        )
        
        # Visualize memory activity
        visualizer.visualize_memory_system(memory_system)
        """
    },
    "nlx-debug-memory": {
        "description": "Memory leak detection and visualization",
        "features": [
            "Memory allocation tracking",
            "Memory leak detection visualization",
            "Memory usage monitoring",
            "Memory fragmentation visualization",
            "Garbage collection visualization"
        ],
        "example_usage": """
        # Example: Memory debugging
        import nlx_tools.debug_memory as debug_memory
        
        # Create memory debugger
        debugger = debug_memory.NLMemoryDebugger(
            track_allocations=True,
            detect_leaks=True,
            visualize_fragmentation=True
        )
        
        # Monitor memory usage
        debugger.start_memory_monitoring()
        """
    },
    "nlx-debug-performance": {
        "description": "Performance profiling and visualization",
        "features": [
            "Execution time profiling",
            "CPU usage visualization",
            "Memory usage visualization",
            "I/O performance visualization",
            "Network performance visualization"
        ],
        "example_usage": """
        # Example: Performance profiling
        import nlx_tools.debug_performance as debug_performance
        
        # Create performance profiler
        profiler = debug_performance.NLPerformanceProfiler(
            profile_cpu=True,
            profile_memory=True,
            profile_io=True,
            real_time_updates=True
        )
        
        # Start performance monitoring
        profiler.start_profiling()
        """
    },
    "nlx-debug-step": {
        "description": "Step-by-step execution debugging",
        "features": [
            "Single step execution with visualization",
            "State inspection at each step",
            "Action trace visualization",
            "Input/output visualization",
            "Error detection and visualization"
        ],
        "example_usage": """
        # Example: Step-by-step debugging
        import nlx_tools.debug_step as debug_step
        
        # Create step debugger
        debugger = debug_step.NLStepDebugger(
            enable_single_step=True,
            track_state_changes=True,
            visualize_actions=True
        )
        
        # Enable step-by-step execution
        debugger.enable_step_by_step_mode()
        """
    },
    "nlx-debug-config": {
        "description": "Configuration validation and visualization",
        "features": [
            "Configuration validation",
            "Configuration difference visualization",
            "Configuration dependency visualization",
            "Configuration impact analysis",
            "Configuration optimization visualization"
        ],
        "example_usage": """
        # Example: Configuration debugging
        import nlx_tools.debug_config as debug_config
        
        # Create configuration validator
        validator = debug_config.NLConfigValidator(
            validate_dependencies=True,
            show_impact_analysis=True,
            suggest_optimizations=True
        )
        
        # Validate configuration
        validation_results = validator.validate_config(config_dict)
        """
    }
}

def main():
    """Main entry point for NLM visualization and debugging tools."""
    print("NLM - Neural Learning Machine Visualization and Debugging Tools")
    print("=" * 70)
    print(f"Version: {__version__}")
    print(f"Author: {__author__}")
    print(f"License: {__license__}")
    print()
    
    print("Available tools:")
    for tool in AVAILABLE_TOOLS:
        print(f"  - {tool}")
        print(f"    {TOOL_DOCUMENTS[tool]['description']}")
        print()
    
    print("For detailed documentation and examples, visit:")
    print("  https://nlm.readthedocs.io/tools/")
    print()
    print("To use a specific tool, import its module directly:")
    print("  Example: import nlx_tools.viz_activity as viz_activity")

if __name__ == "__main__":
    main()
