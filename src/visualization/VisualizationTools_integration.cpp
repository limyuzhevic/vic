// VisualizationTools_integration.cpp
#include "visualization/VisualizationTools_integration.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace nlm {

std::unique_ptr<VisualizationTools> createCompleteVisualizationSetup(
    const Brain& brain,
    const std::string& outputDirectory,
    bool enable3D,
    bool enableVR,
    bool enableWebGL
) {
    auto vizTools = std::make_unique<VisualizationTools>();
    
    // Initialize with basic configuration
    vizTools->initialize(outputDirectory);
    
    // Configure for comprehensive visualization
    nlm::VisualizationToolsConfig config;
    config.outputDirectory = outputDirectory;
    config.enable3D = enable3D;
    config.enableVR = enableVR;
    config.enableWebGL = enableWebGL;
    config.enableAnimations = true;
    config.enableMultiThreading = true;
    config.threadCount = 2;
    config.enableResourceOptimization = true;
    
    // Apply configuration
    configureVisualizationFromConfig(*vizTools, config, &brain);
    
    // Initialize visualization data
    initializeVisualizationDataFromBrain(*vizTools, brain);
    
    return vizTools;
}

std::unique_ptr<VisualizationTools> createInteractiveMonitoringDashboard(
    const std::string& title,
    double updateInterval,
    const Brain* brain
) {
    auto vizTools = nlm::VisualizationTools::createInteractiveDashboard(title);
    vizTools->initialize("./monitoring_dashboard");
    
    // Configure dashboard
    nlm::DashboardConfig dashboardConfig;
    dashboardConfig.title = title;
    dashboardConfig.widgets = {
        "Network Graph",
        "Activity Timeline",
        "Spike Raster Plot",
        "Population Statistics",
        "Weight Matrix Viewer"
    };
    dashboardConfig.enableRealTimeUpdates = true;
    dashboardConfig.updateInterval = updateInterval;
    dashboardConfig.layout = "grid";
    
    // Generate dashboard if brain is available
    if (brain) {
        vizTools->generateInteractiveDashboard("dashboard.html", dashboardConfig);
    }
    
    return vizTools;
}

std::unique_ptr<VisualizationTools> create3DNeuralNetworkVisualizer(
    const Brain& brain,
    float cameraDistance,
    float fieldOfView
) {
    // Configure for 3D visualization
    nlm::SceneConfig sceneConfig;
    sceneConfig.name = "3D Neural Network Visualizer";
    sceneConfig.cameraDistance = cameraDistance;
    sceneConfig.fieldOfView = fieldOfView;
    sceneConfig.backgroundColor = "#000000";
    sceneConfig.lightingMode = "phong";
    sceneConfig.enableShadows = true;
    sceneConfig.enableAntialiasing = true;
    
    auto vizTools = nlm::VisualizationTools::create3DVisualizer(sceneConfig);
    vizTools->initialize("./3d_visualization");
    
    // Generate 3D visualization
    vizTools->generate3DVisualization(brain, "neural_network_3d.html", sceneConfig);
    
    return vizTools;
}

std::unique_ptr<VisualizationTools> createVRNeuralNetworkVisualizer(
    const Brain& brain,
    const std::string& interactionMode,
    bool enablePhysics
) {
    // Configure for VR visualization
    nlm::VRConfig vrConfig;
    vrConfig.trackingSystem = "head-mounted-display";
    vrConfig.displayMode = "stereoscopic";
    vrConfig.enablePhysics = enablePhysics;
    vrConfig.enableInteractions = true;
    vrConfig.interactionMode = interactionMode;
    
    auto vizTools = nlm::VisualizationTools::createVRVisualizer(vrConfig);
    vizTools->initialize("./vr_visualization");
    
    // Generate VR visualization
    vizTools->generateVRScene(brain, "neural_network_vr.vrscene", vrConfig);
    
    return vizTools;
}

std::unique_ptr<VisualizationTools> createComprehensiveReportGenerator(
    const Brain& brain,
    const std::string& outputDirectory,
    bool includeRawData,
    bool includeStatistics
) {
    auto vizTools = std::make_unique<VisualizationTools>();
    vizTools->initialize(outputDirectory);
    
    // Generate comprehensive report
    nlm::ReportParams reportParams;
    reportParams.title = "NLM Comprehensive Neural Network Analysis Report";
    reportParams.description = "This report contains analysis of the NLM neural network simulation";
    reportParams.sections = {"Network Topology", "Neural Activity", "Spike Analysis", "Population Statistics", "Performance Metrics"};
    reportParams.includeRawData = includeRawData;
    reportParams.includeStatistics = includeStatistics;
    reportParams.outputPath = outputDirectory;
    reportParams.visualTypes = {"network_graph", "neuron_activity", "spike_raster", "population_stats"};
    
    vizTools->generateComprehensiveReport("comprehensive_report.html", reportParams);
    
    return vizTools;
}

std::unique_ptr<VisualizationTools> createAnimationGenerator(
    const Brain& brain,
    double duration,
    double frameRate,
    bool loop
) {
    auto vizTools = std::make_unique<VisualizationTools>();
    vizTools->initialize("./animations");
    
    // Configure animation
    nlm::AnimationParams animationParams;
    animationParams.duration = duration;
    animationParams.frameRate = frameRate;
    animationParams.loop = loop;
    animationParams.easing = "ease-in-out";
    animationParams.interpolation = "cubic";
    
    // Generate animation
    vizTools->generateTimeSeriesAnimation(brain, "neural_animation.html", animationParams);
    
    return vizTools;
}

void configureVisualizationFromConfig(
    VisualizationTools& vizTools,
    const VisualizationToolsConfig& config,
    const Brain* brain
) {
    // Configure basic settings
    vizTools.setOutputDirectory(config.outputDirectory);
    vizTools.setUpdateRate(60.0);
    vizTools.setMaxHistorySize(config.maxHistorySize);
    vizTools.setCompressionLevel(config.compressionLevel);
    vizTools.enableMultiThreading(config.enableMultiThreading);
    vizTools.setThreadCount(config.threadCount);
    vizTools.enableResourceOptimization(config.enableResourceOptimization);
    vizTools.setMemoryLimit(config.memoryLimit);
    
    // Initialize visualization data if brain is available
    if (brain) {
        initializeVisualizationDataFromBrain(vizTools, *brain);
    }
}

void initializeVisualizationDataFromBrain(
    VisualizationTools& vizTools,
    const Brain& brain,
    const std::string& dataOutputFile
) {
    // Process network topology
    vizTools.processNetworkTopology(brain);
    
    // Process neural activity
    vizTools.processNeuralActivity(brain);
    
    // Process population statistics
    vizTools.processPopulationStats(brain);
    
    // Save data if requested
    if (!dataOutputFile.empty()) {
        vizTools.exportData(dataOutputFile);
    }
}

void updateVisualizationFromBrain(
    VisualizationTools& vizTools,
    const Brain& brain,
    double deltaTime
) {
    // Update visualization state
    vizTools.update(deltaTime);
    
    // Process current brain state
    vizTools.processNeuralActivity(brain);
    vizTools.processSpikeData(brain.getSpikeSystem()->getCurrentSpikes());
    vizTools.processPopulationStats(brain);
}

std::vector<std::string> generateStandardVisualizations(
    VisualizationTools& vizTools,
    const Brain& brain,
    const std::string& outputPrefix
) {
    std::vector<std::string> generatedFiles;
    
    // Generate all standard visualization types
    vizTools.generateVisualization(nlm::VisualizationType::NETWORK_GRAPH, outputPrefix + "_network_graph");
    generatedFiles.push_back(outputPrefix + "_network_graph.html");
    
    vizTools.generateVisualization(nlm::VisualizationType::NEURON_ACTIVITY, outputPrefix + "_neuron_activity");
    generatedFiles.push_back(outputPrefix + "_neuron_activity.json");
    
    vizTools.generateVisualization(nlm::VisualizationType::SPIKE_RASTER, outputPrefix + "_spike_raster");
    generatedFiles.push_back(outputPrefix + "_spike_raster.csv");
    
    vizTools.generateVisualization(nlm::VisualizationType::WEIGHT_MATRIX, outputPrefix + "_weight_matrix");
    generatedFiles.push_back(outputPrefix + "_weight_matrix.html");
    
    vizTools.generateVisualization(nlm::VisualizationType::POPULATION_STATISTICS, outputPrefix + "_population_stats");
    generatedFiles.push_back(outputPrefix + "_population_stats.json");
    
    // Generate 3D visualization if enabled
    nlm::SceneConfig sceneConfig;
    sceneConfig.name = "Standard 3D Visualization";
    sceneConfig.cameraDistance = 50.0f;
    vizTools.generate3DVisualization(brain, outputPrefix + "_3d.html", sceneConfig);
    generatedFiles.push_back(outputPrefix + "_3d.html");
    
    // Generate WebGL visualization
    vizTools.generateWebGLVisualization(brain, outputPrefix + "_webgl.html");
    generatedFiles.push_back(outputPrefix + "_webgl.html");
    
    return generatedFiles;
}

bool validateVisualizationConfiguration(
    const VisualizationTools& vizTools,
    const Brain& brain
) {
    // Check basic configuration
    if (!vizTools.isInitialized()) {
        return false;
    }
    
    if (vizTools.getCurrentFPS() < 1.0) {
        return false;
    }
    
    // Check brain state
    if (brain.getTotalSpikeCount() < 0) {
        return false;
    }
    
    return true;
}

std::string getVisualizationPerformanceStats(const VisualizationTools& vizTools) {
    std::stringstream stats;
    
    stats << "=== Visualization Performance Statistics ===\n";
    stats << "FPS: " << std::fixed << std::setprecision(2) << vizTools.getCurrentFPS() << "\n";
    stats << "Data Size: " << vizTools.getDataSize() << " bytes\n";
    stats << "Initialized: " << (vizTools.isInitialized() ? "Yes" : "No") << "\n";
    stats << "Rendering: " << (vizTools.isRendering() ? "Yes" : "No") << "\n";
    
    return stats.str();
}

std::unique_ptr<VisualizationTools> createCustomVisualizationPipeline(
    const std::string& name,
    const VisualizationToolsConfig& config,
    const std::unordered_map<std::string, std::string>& pipelineConfig
) {
    auto vizTools = std::make_unique<VisualizationTools>();
    vizTools->initialize("./custom_pipeline");
    
    // Apply configuration
    configureVisualizationFromConfig(*vizTools, config);
    
    return vizTools;
}

bool applyVisualizationSettingsToBrain(
    Brain& brain,
    const VisualizationToolsConfig& config
) {
    try {
        // Configure brain visualization settings
        // This would interface with brain's visualization settings
        
        // In a real implementation, this would:
        // 1. Configure brain's visualization subsystem
        // 2. Set up visualization data collection
        // 3. Configure output formats
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error applying visualization settings to brain: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> exportVisualizationDataForExternalTools(
    VisualizationTools& vizTools,
    const Brain& brain,
    const std::vector<std::string>& formats,
    const std::string& outputDirectory
) {
    std::vector<std::string> exportedFiles;
    
    for (const auto& format : formats) {
        std::string filename = outputDirectory + "/nlm_data_" + format;
        
        if (format == "csv") {
            vizTools.exportToCSV(brain, filename);
            exportedFiles.push_back(filename + ".csv");
        } else if (format == "json") {
            vizTools.exportToJSON(brain, filename);
            exportedFiles.push_back(filename + ".json");
        } else if (format == "html") {
            vizTools.exportToHTML(brain, filename);
            exportedFiles.push_back(filename + ".html");
        } else if (format == "vr") {
            vizTools.exportToVR(brain, filename);
            exportedFiles.push_back(filename + ".vrscene");
        }
    }
    
    return exportedFiles;
}

std::unique_ptr<VisualizationTools> setupComprehensiveVisualizationMonitoring(
    VisualizationTools& vizTools,
    const Brain& brain,
    double monitoringInterval
) {
    auto monitoringTools = std::make_unique<VisualizationTools>();
    monitoringTools->initialize("./visualization_monitoring");
    
    // Configure for monitoring
    nlm::DashboardConfig dashboardConfig;
    dashboardConfig.title = "NLM Visualization Monitoring";
    dashboardConfig.widgets = {
        "Real-Time Network Graph",
        "Activity Timeline",
        "Spike Rate Monitor",
        "Population Stats Monitor",
        "Performance Metrics"
    };
    dashboardConfig.enableRealTimeUpdates = true;
    dashboardConfig.updateInterval = monitoringInterval * 1000.0;
    
    // Generate monitoring dashboard
    monitoringTools->generateInteractiveDashboard("monitoring_dashboard.html", dashboardConfig);
    
    // Set up monitoring callbacks
    monitoringTools->setProgressCallback([](double progress) {
        // Log monitoring progress
    });
    
    monitoringTools->setErrorCallback([](const std::string& error) {
        // Log monitoring errors
    });
    
    return monitoringTools;
}

void shutdownVisualizationSystem(
    VisualizationTools& vizTools,
    bool saveState,
    const std::string& backupFile
) {
    if (saveState) {
        vizTools.exportData(backupFile + "_visualization_state.dat");
    }
    
    vizTools.close();
}

} // namespace nlm