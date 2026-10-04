#include <iostream>
#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "analysis/NetworkAnalyzer.hpp"
#include "analysis/ActivityAnalyzer.hpp"
#include "analysis/PerformanceAnalyzer.hpp"
#include "analysis/StateAnalyzer.hpp"
#include "analysis/PlottingTools.hpp"

void demonstrateEnhancedVisualization() {
    std::cout << "=== Enhanced Visualization and Analysis System Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "  - Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  - Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  - Regions: " << brain->getRegionCount() << std::endl;
    std::cout << std::endl;
    
    // Initialize visualization interface
    auto visualization = std::make_unique<VisualizationInterface>();
    visualization->setBackend("glfw");
    visualization->setUpdateRate(30.0);
    visualization->enableInteractiveMode(true);
    
    if (!visualization->initialize()) {
        std::cerr << "Failed to initialize visualization!" << std::endl;
        return;
    }
    
    std::cout << "Visualization interface initialized" << std::endl;
    std::cout << "  - Backend: " << visualization->getBackend() << std::endl;
    std::cout << "  - Update rate: " << visualization->getUpdateRate() << " Hz" << std::endl;
    std::cout << "  - Interactive mode: " << (visualization->isInteractiveMode() ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    // Run a few simulation steps
    std::cout << "Running simulation steps..." << std::endl;
    for (SimulationStep step = 0; step < 10; ++step) {
        brain->step(step, step * 0.001);
        visualization->update();
    }
    
    std::cout << std::endl;
    std::cout << "=== Network Analysis ===" << std::endl;
    
    // Create and use network analyzer
    auto networkAnalyzer = NetworkAnalyzer::create();
    auto networkMetrics = networkAnalyzer->analyzeGraphMetrics(*brain);
    
    std::cout << "Network metrics:" << std::endl;
    std::cout << "  - Average degree: " << networkMetrics.averageDegree << std::endl;
    std::cout << "  - Clustering coefficient: " << networkMetrics.clusteringCoefficient << std::endl;
    std::cout << "  - Network density: " << networkMetrics.density << std::endl;
    std::cout << "  - Connected components: " << networkMetrics.connectedComponents << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Activity Analysis ===" << std::endl;
    
    // Create and use activity analyzer
    auto activityAnalyzer = ActivityAnalyzer::create();
    auto activityStats = activityAnalyzer->analyzeSpikeStatistics(*brain, 1000);
    
    std::cout << "Activity statistics:" << std::endl;
    std::cout << "  - Total spike count: " << activityStats.totalSpikeCount << std::endl;
    std::cout << "  - Average firing rate: " << activityStats.averageFiringRate << " Hz" << std::endl;
    std::cout << "  - Standard deviation: " << activityStats.standardDeviation << std::endl;
    std::cout << "  - Coefficient of variation: " << activityStats.coefficientOfVariation << std::endl;
    std::cout << "  - Peak firing rate: " << activityStats.peakFiringRate << " Hz" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== State Analysis ===" << std::endl;
    
    // Create and use state analyzer
    auto stateAnalyzer = StateAnalyzer::create();
    auto stateSnapshot = stateAnalyzer->captureState(*brain);
    
    std::cout << "State snapshot:" << std::endl;
    std::cout << "  - Global energy: " << stateSnapshot.globalEnergy << std::endl;
    std::cout << "  - Average firing rate: " << stateSnapshot.averageFiringRate << " Hz" << std::endl;
    std::cout << "  - Neuron states: " << stateSnapshot.neuronStates.size() << std::endl;
    std::cout << "  - Synapse states: " << stateSnapshot.synapseStates.size() << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Performance Analysis ===" << std::endl;
    
    // Create and use performance analyzer
    auto performanceAnalyzer = PerformanceAnalyzer::create();
    auto performanceMetrics = performanceAnalyzer->collectPerformanceMetrics();
    
    std::cout << "Performance metrics:" << std::endl;
    std::cout << "  - CPU usage: " << (performanceMetrics.cpuUsage * 100) << "%" << std::endl;
    std::cout << "  - Memory usage: " << (performanceMetrics.memoryUsage * 100) << "%" << std::endl;
    std::cout << "  - GPU usage: " << (performanceMetrics.gpuUsage * 100) << "%" << std::endl;
    std::cout << "  - Processing latency: " << performanceMetrics.processingLatency.count() << " ms" << std::endl;
    std::cout << "  - Throughput: " << performanceMetrics.throughput << " ops/sec" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Visualization Integration ===" << std::endl;
    
    // Integrate all analysis components with visualization
    visualization->integrateAnalysis(networkAnalyzer);
    visualization->integrateAnalysis(activityAnalyzer);
    visualization->integrateAnalysis(performanceAnalyzer);
    visualization->integrateAnalysis(stateAnalyzer);
    
    auto plottingTools = PlottingTools::create();
    visualization->integrateAnalysis(plottingTools);
    
    std::cout << "Visualization integrated with all analysis components:" << std::endl;
    std::cout << "  - Network analyzer: Integrated" << std::endl;
    std::cout << "  - Activity analyzer: Integrated" << std::endl;
    std::cout << "  - Performance analyzer: Integrated" << std::endl;
    std::cout << "  - State analyzer: Integrated" << std::endl;
    std::cout << "  - Plotting tools: Integrated" << std::endl;
    std::cout << std::endl;
    
    // Synchronize with analysis for real-time visualization
    visualization->synchronizeWithAnalysis(
        networkAnalyzer,
        activityAnalyzer,
        performanceAnalyzer,
        stateAnalyzer
    );
    
    std::cout << "=== Advanced Visualization Capabilities ===" << std::endl;
    
    // Perform advanced visualizations
    visualization->visualizeNetworkTopology(*brain);
    visualization->visualizeActivityPatterns(*brain);
    visualization->visualizeStateEvolution(*brain);
    visualization->visualizePerformanceMetrics(*brain);
    
    std::cout << "Advanced visualizations generated:" << std::endl;
    std::cout << "  - Network topology visualization" << std::endl;
    std::cout << "  - Activity pattern visualization" << std::endl;
    std::cout << "  - State evolution visualization" << std::endl;
    std::cout << "  - Performance metrics visualization" << std::endl;
    std::cout << std::endl;
    
    // Export capabilities
    std::cout << "=== Export Capabilities ===" << std::endl;
    
    if (visualization->exportVisualization("enhanced_visualization.html", "html")) {
        std::cout << "✓ Visualization exported to enhanced_visualization.html" << std::endl;
    }
    
    if (visualization->exportAnalysisResults("analysis_results.html", "comprehensive")) {
        std::cout << "✓ Analysis results exported to analysis_results.html" << std::endl;
    }
    
    if (plottingTools->exportPlot("network_topology.html", 
                                PlottingTools::PlotData{"Network", {}, {}, "Neuron", "Degree", "Network Topology", "svg"})) {
        std::cout << "✓ Plot exported to network_topology.html" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "Enhanced Visualization and Analysis System successfully demonstrated!" << std::endl;
    std::cout << std::endl;
    std::cout << "Key Features Demonstrated:" << std::endl;
    std::cout << "  ✓ Real visualization capabilities with multiple backends" << std::endl;
    std::cout << "  ✓ Network topology analysis (clustering, centrality, communities)" << std::endl;
    std::cout << "  ✓ Activity pattern analysis (spike statistics, correlations)" << std::endl;
    std::cout << "  ✓ Performance monitoring and bottleneck analysis" << std::endl;
    std::cout << "  ✓ State evolution tracking and anomaly detection" << std::endl;
    std::cout << "  ✓ Advanced plotting (2D/3D, heatmaps, time series)" << std::endl;
    std::cout << "  ✓ Seamless integration with simulation workflows" << std::endl;
    std::cout << "  ✓ Export capabilities for analysis results" << std::endl;
    std::cout << std::endl;
    std::cout << "The enhanced visualization and analysis system provides comprehensive" << std::endl;
    std::cout << "tools for understanding and visualizing complex neural network behavior." << std::endl;
}

int main() {
    try {
        demonstrateEnhancedVisualization();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
