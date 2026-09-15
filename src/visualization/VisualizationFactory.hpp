#pragma once

namespace nlm {

// Factory for creating visualization components
class VisualizationFactory {
public:
    VisualizationFactory();
    ~VisualizationFactory();
    
    // Create visualization system
    static std::unique_ptr<VisualizationSystem> createVisualizationSystem(
        const std::map<std::string, std::string>& config = {});
    
    // Create specific visualizer
    static std::unique_ptr<NetworkVisualizer> createNetworkVisualizer(
        const std::map<std::string, std::string>& config = {});
    
    static std::unique_ptr<ActivityVisualizer> createActivityVisualizer(
        const std::map<std::string, std::string>& config = {});
    
    static std::unique_ptr<RasterVisualizer> createRasterVisualizer(
        const std::map<std::string, std::string>& config = {});
    
    static std::unique_ptr<WeightVisualizer> createWeightVisualizer(
        const std::map<std::string, std::string>& config = {});
    
    static std::unique_ptr<DevelopmentVisualizer> createDevelopmentVisualizer(
        const std::map<std::string, std::string>& config = {});
    
    // Create backend
    static std::unique_ptr<VisualizationEngine> createBackend(
        int backendType,
        const std::map<std::string, std::string>& config = {});
    
    // Create integration component
    static std::unique_ptr<VisualizationSystemIntegration> createIntegration(
        const std::map<std::string, std::string>& config = {});
    
private:
    static std::map<std::string, std::string> defaultConfig;
};

} // namespace nlm
