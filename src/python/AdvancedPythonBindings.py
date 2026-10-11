# Python bindings for Advanced NLM Features

This file contains enhanced Python bindings for the NLM framework, providing:
- Advanced configuration options
- Improved simulation control
- Enhanced visualization capabilities
- Machine learning integration

The bindings use pybind11 to expose the C++ functionality to Python
while maintaining a user-friendly interface.

Key features:
- Type-safe configuration access
- Event-driven simulation control
- Advanced visualization exports
- Integration with machine learning frameworks
- Comprehensive error handling

Example usage:
    import nlm_pybind
    
    # Create brain with advanced configuration
    config = nlm_pybind.create_advanced_config({
        'brain': {
            'neuron_count': 5000,
            'region_count': 3
        },
        'plasticity': {
            'stdp': {'learning_rate': 0.005, 'tau': 25.0},
            'hebbian': {'learning_rate': 0.001}
        }
    })
    
    brain = nlm_pybind.create_brain(config)
    brain.initialize()
    
    # Run simulation with callbacks
    def progress_callback(step, brain):
        print(f"Step {step}: {brain.get_firing_rate():.2f} Hz")
    
    brain.run_simulation(1000, progress_callback)
    
    # Export for analysis
    data = brain.export_data()
    import numpy as np
    np.savetxt('spike_times.txt', data['spike_times'])
    np.savetxt('weights.txt', data['synapse_weights'])