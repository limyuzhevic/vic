type CheckEnvironment = {
    # System information
    python_version: str
    platform: str
    architecture: str
    
    # NLM library information
    nlm_version: str
    nlm_features: string[]
    
    # Configuration checks
    config: {
        has_neural_networks: bool
        has_plasticity: bool
        has_memory: bool
        has_prediction: bool
        has_cognition: bool
        has_neuromodulation: bool
    }
    
    # Performance metrics (if available)
    performance?: {
        neuron_count: number
        synapse_count: number
        max_firing_rate: number
        memory_capacity: number
    }
}

// Helper functions for environment checking
function checkEnvironment(): CheckEnvironment {
    return {
        python_version: Python.version,
        platform: Python.platform,
        architecture: Python.architecture(),
        nlm_version: __version__,
        nlm_features: [
            "Neurons",
            "Synapses", 
            "Plasticity (STDP, Hebbian)",
            "Working Memory",
            "Episodic Memory",
            "Associative Memory",
            "Prediction System",
            "Neural Planning",
            "Concept Formation",
            "Attention",
            "Neuromodulation (Dopamine, Curiosity, Novelty)",
            "Development",
            "Structural Plasticity"
        ],
        config: {
            has_neural_networks: true,
            has_plasticity: true,
            has_memory: true,
            has_prediction: true,
            has_cognition: true,
            has_neuromodulation: true
        }
    }
}