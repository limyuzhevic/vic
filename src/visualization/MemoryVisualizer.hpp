#pragma once

#include "NVMVisualization.hpp"
#include "../memory/MemorySystem.hpp"
#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

class MemoryVisualizer : public VisualizationComponent {
public:
    MemoryVisualizer();
    ~MemoryVisualizer() override;
    
    // Initialize with memory system data
    bool initialize(const MemorySystem& memorySystem) override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render memory system visualization
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
    
    // Set memory type to visualize
    void setMemoryType(const std::string& type); // "working", "episodic", "semantic"
    
    // Get current memory type
    std::string getMemoryType() const;
    
    // Enable/disable memory trace visualization
    void setMemoryTraceVisualizationEnabled(bool enabled);
    
    // Check if memory trace visualization is enabled
    bool getMemoryTraceVisualizationEnabled() const;
    
    // Enable/disable memory consolidation visualization
    void setMemoryConsolidationVisualizationEnabled(bool enabled);
    
    // Check if memory consolidation visualization is enabled
    bool getMemoryConsolidationVisualizationEnabled() const;
    
    // Enable/disable memory decay visualization
    void setMemoryDecayVisualizationEnabled(bool enabled);
    
    // Check if memory decay visualization is enabled
    bool getMemoryDecayVisualizationEnabled() const;
    
    // Enable/disable episodic memory timeline
    void setEpisodicMemoryTimelineEnabled(bool enabled);
    
    // Check if episodic memory timeline is enabled
    bool getEpisodicMemoryTimelineEnabled() const;
    
    // Enable/disable semantic memory network
    void setSemanticMemoryNetworkEnabled(bool enabled);
    
    // Check if semantic memory network is enabled
    bool getSemanticMemoryNetworkEnabled() const;
    
    // Set timeline visualization style
    void setTimelineVisualizationStyle(const std::string& style); // "linear", "circular", "tree"
    
    // Get current timeline visualization style
    std::string getTimelineVisualizationStyle() const;
    
    // Set memory strength visualization style
    void setMemoryStrengthVisualizationStyle(const std::string& style); // "color", "size", "opacity"
    
    // Get current memory strength visualization style
    std::string getMemoryStrengthVisualizationStyle() const;
    
    // Highlight specific memories
    void highlightMemories(const std::vector<MemoryId>& memoryIds, bool highlight = true);
    
    // Get highlighted memories
    const std::vector<MemoryId>& getHighlightedMemories() const;
    
    // Get memory traces
    const std::vector<MemoryTrace>& getMemoryTraces() const;
    
    // Get memory consolidation progress
    const std::vector<float>& getConsolidationProgress() const;
    
    // Get memory decay rates
    const std::vector<float>& getMemoryDecayRates() const;
    
    // Get episodic memory timeline data
    const std::vector<std::pair<Timestamp, std::string>>& getEpisodicMemoryTimeline() const;
    
    // Get semantic memory associations
    const std::unordered_map<std::string, std::vector<std::string>>& getSemanticMemoryNetwork() const;
    
    // Enable/disable real-time memory trace updates
    void setRealTimeMemoryUpdates(bool enabled);
    
    // Check if real-time memory updates are enabled
    bool getRealTimeMemoryUpdates() const;
    
    // Set memory visualization update frequency
    void setMemoryUpdateFrequency(double hz);
    
    // Get current memory update frequency
    double getMemoryUpdateFrequency() const;
    
    // Export visualization data
    bool exportData(const std::string& filename, const std::string& format = "json");
    
    // Import visualization data
    bool importData(const std::string& filename, const std::string& format = "json");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm