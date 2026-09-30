// Visualization and Analysis Module for NLM (Neural Learning Machine)
// Phase 2: Real Neural Computation - Enhanced Visualization and Analysis

// This module provides comprehensive visualization and analysis capabilities for the NLM system.
// It includes:
// - Neural network visualization (neurons, connections, regions)
// - Spike train visualization (raster plots, time series)
// - Memory system visualization (working memory, episodic memory)
// - Plasticity tracking (synaptic weight changes, STDP)
// - Network connectivity analysis (graph metrics, centrality)
// - Statistical analysis of neural activity (firing rates, correlations)
// - Pattern recognition and learning analysis
// - Real-time export capabilities (JSON, CSV, PNG, WebSocket)

// Key Features:
// - Efficient rendering for large-scale networks
// - Streaming support for real-time visualization
// - Multi-format export support
// - Memory-efficient data structures
// - Integration with existing NLM monitoring systems
// - Python bindings for advanced visualization
// - Modular design for easy extension

// Architecture:
// - VisualizationInterface: Base interface for all visualization modules
// - NeuralVisualization: Core neural network visualization
// - SpikeVisualization: Spike train and activity visualization
// - MemoryVisualization: Memory system state visualization
// - PlasticityVisualization: Plasticity tracking and analysis
// - NetworkVisualization: Network connectivity and graph visualization
// - StatisticalAnalysis: Statistical analysis of neural activity
// - PatternAnalysis: Pattern recognition and feature extraction
// - NetworkAnalysis: Graph theory and connectivity analysis
// - LearningAnalysis: Learning efficiency and trajectory analysis

// Integration Points:
// - Brain class: Provides data for visualization (neurons, synapses, spikes, memory)
// - VisualizationInterface: Base class for all visualization modules
// - Existing monitoring systems: Integration with logging and status reporting
// - Python bindings: pybind11 interface for advanced visualization capabilities

// Performance Considerations:
// - GPU acceleration for large networks (OpenGL/Vulkan)
// - Streaming compression for real-time updates
// - Multi-threading for parallel processing
// - Adaptive sampling for large datasets
// - Efficient data structures for memory management

// File Structure:
// - src/visualization/
//   - NeuralVisualization.hpp/.cpp
//   - SpikeVisualization.hpp/.cpp
//   - MemoryVisualization.hpp/.cpp
//   - PlasticityVisualization.hpp/.cpp
//   - NetworkVisualization.hpp/.cpp
//   - VisualizationInterface.hpp/.cpp (enhanced)
// - src/analysis/
//   - StatisticalAnalysis.hpp/.cpp
//   - PatternAnalysis.hpp/.cpp
//   - NetworkAnalysis.hpp/.cpp
//   - LearningAnalysis.hpp/.cpp
// - examples/ (usage examples)
// - tests/ (unit tests)

// Usage Examples:
// - Basic network visualization: visualization.visualizeNetwork(brain)
// - Real-time monitoring: visualization.startRealTimeUpdates()
// - Statistical analysis: analysis.computeNetworkStatistics(brain)
// - Pattern detection: analysis.detectLearningPatterns(brain)
// - Export capabilities: visualization.exportToFile(format, filename)

// License: MIT
// Copyright: NLM Authors

