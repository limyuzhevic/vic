#pragma once

#include "NVMVisualization.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

class DevelopmentVisualizer : public VisualizationComponent {
public:
    DevelopmentVisualizer();
    ~DevelopmentVisualizer() override;
    
    // Initialize with development system data
    bool initialize(const DevelopmentSystem& developmentSystem) override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render development visualization
    void render() override;
    
    // Handle input events
    void handleInput(const std::string& eventType, const std::map<std::string, float>& parameters) override;
    
    // Check if visualization is active
    bool isActive() const override;
    
    // Set visibility
    void setVisible(bool visible) override;
    
    // Get component name
    std::string getName() const override;
    
    // Get component bounds
    void getBounds(float& x, float& y, float& width, float& height) const override;
    
    // Set component bounds
    void setBounds(float x, float y, float width, float height) override;
    
    // Enable/disable developmental stages visualization
    void setDevelopmentalStagesVisualizationEnabled(bool enabled);
    
    // Check if developmental stages visualization is enabled
    bool getDevelopmentalStagesVisualizationEnabled() const;
    
    // Enable/disable structural plasticity visualization
    void setStructuralPlasticityVisualizationEnabled(bool enabled);
    
    // Check if structural plasticity visualization is enabled
    bool getStructuralPlasticityVisualizationEnabled() const;
    
    // Enable/disable maturation processes visualization
    void setMaturationProcessesVisualizationEnabled(bool enabled);
    
    // Check if maturation processes visualization is enabled
    bool getMaturationProcessesVisualizationEnabled() const;
    
    // Enable/disable experience-dependent development visualization
    void setExperienceDependentDevelopmentVisualizationEnabled(bool enabled);
    
    // Check if experience-dependent development visualization is enabled
    bool getExperienceDependentDevelopmentVisualizationEnabled() const;
    
    // Set visualization style
    void setVisualizationStyle(const std::string& style); // "bar", "graph", "heatmap"
    
    // Get current visualization style
    std::string getVisualizationStyle() const;
    
    // Set time window for visualization
    void setTimeWindow(double seconds);
    
    // Get current time window
    double getTimeWindow() const;
    
    // Get development data
    const DevelopmentData& getDevelopmentData() const;
    
    // Get developmental stage data
    const std::vector<std::pair<int, float>>& getDevelopmentalStageData() const;
    
    // Get structural plasticity data
    const std::vector<std::pair<Timestamp, float>>& getStructuralPlasticityData() const;
    
    // Get maturation data
    const std::vector<std::pair<Timestamp, float>>& getMaturationData() const;
    
    // Get experience data
    const std::vector<std::pair<Timestamp, std::string>>& getExperienceData() const;
    
    // Get experience-dependent development data
    const std::vector<std::pair<Timestamp, float>>& getExperienceDependentData() const;
    
    // Enable/disable real-time updates
    void setRealTimeUpdates(bool enabled);
    
    // Check if real-time updates are enabled
    bool getRealTimeUpdates() const;
    
    // Set update frequency
    void setUpdateFrequency(double hz);
    
    // Get current update frequency
    double getUpdateFrequency() const;
    
    // Export visualization data
    bool exportData(const std::string& filename, const std::string& format = "json");
    
    // Import visualization data
    bool importData(const std::string& filename, const std::string& format = "json");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm