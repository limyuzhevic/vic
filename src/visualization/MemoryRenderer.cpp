#include "MemoryRenderer.hpp"
#include "ColorManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_set>

namespace nlm {

MemoryRenderer::MemoryRenderer()
    : memoryViewMode_(MemoryViewMode::WorkingMemory), memoryAnalysisType_(MemoryAnalysisType::Capacity),
      showMemoryLabels_(true), showStrengthIndicators_(true), showAgeGradient_(true),
      consolidationThreshold_(0.7f), selectedMemoryEntry_(""), showOnlySelected_(false),
      currentMemoryUsage_(0.0f), consolidationProgress_(0.0f),
      maxMemoryEntries_(1000), memoryDecayRate_(0.001f), autoConsolidation_(true),
      colorScheme_("default") {
    initializeDefaultConfiguration();
}

void MemoryRenderer::initializeDefaultConfiguration() {
    // Initialize default rendering parameters
    maxMemoryEntries_ = 1000;
    memoryDecayRate_ = 0.001f;
    autoConsolidation_ = true;
    colorScheme_ = "default";
}

void MemoryRenderer::render(const Brain& brain) {
    // Collect current memory data
    collectMemoryData(brain);
    updateMemoryStatistics();
    updateMemoryTimeline();
    
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    // Render title
    ColorManager colorManager;
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightBlue) << "=== NEURAL MEMORY VISUALIZATION ===" << "\033[0m" << std::endl;
    
    // Render based on memory view mode
    switch (memoryViewMode_) {
        case MemoryViewMode::WorkingMemory:
            renderWorkingMemory(brain);
            break;
        case MemoryViewMode::EpisodicMemory:
            renderEpisodicMemory(brain);
            break;
        case MemoryViewMode::SemanticMemory:
            renderSemanticMemory(brain);
            break;
        case MemoryViewMode::ProceduralMemory:
            renderProceduralMemory(brain);
            break;
        case MemoryViewMode::AssociativeMemory:
            renderAssociativeMemory(brain);
            break;
        case MemoryViewMode::Consolidated:
            renderConsolidatedMemory(brain);
            break;
    }
    
    // Render additional elements
    if (showMemoryLabels_) {
        renderMemoryStats();
    }
    
    // Render controls
    renderControls();
}

void MemoryRenderer::update(const Brain& brain) {
    collectMemoryData(brain);
    updateMemoryStatistics();
}

void MemoryRenderer::collectMemoryData(const Brain& brain) {
    // Clear previous memory entries
    memoryEntries_.clear();
    
    // Extract data from each memory system
    extractWorkingMemoryData(brain);
    extractEpisodicMemoryData(brain);
    extractSemanticMemoryData(brain);
    extractProceduralMemoryData(brain);
    extractAssociativeMemoryData(brain);
    
    // Apply memory decay
    for (auto& entry : memoryEntries_) {
        entry.strength *= (1.0f - memoryDecayRate_);
    }
    
    // Remove weak entries
    memoryEntries_.erase(
        std::remove_if(memoryEntries_.begin(), memoryEntries_.end(),
                      [](const MemoryEntry& entry) { return entry.strength < 0.01f; }),
        memoryEntries_.end()
    );
    
    // Limit number of entries
    if (memoryEntries_.size() > maxMemoryEntries_) {
        // Sort by strength and keep strongest
        std::partial_sort(memoryEntries_.begin(), memoryEntries_.begin() + maxMemoryEntries_,
                         memoryEntries_.end(),
                         [](const MemoryEntry& a, const MemoryEntry& b) {
                             return a.strength > b.strength;
                         });
        memoryEntries_.resize(maxMemoryEntries_);
    }
}

void MemoryRenderer::updateMemoryStatistics() {
    calculateMemoryStatistics();
    updateMemoryTimeline();
}

void MemoryRenderer::calculateMemoryStatistics() {
    memoryStats_.totalEntries = memoryEntries_.size();
    memoryStats_.totalStrength = 0.0f;
    
    for (const auto& entry : memoryEntries_) {
        memoryStats_.totalStrength += entry.strength;
    }
    
    if (!memoryEntries_.empty()) {
        memoryStats_.avgStrength = memoryStats_.totalStrength / memoryEntries_.size();
    }
    
    // Count by type
    memoryStats_.workingMemoryCount = 0;
    memoryStats_.episodicMemoryCount = 0;
    memoryStats_.semanticMemoryCount = 0;
    memoryStats_.proceduralMemoryCount = 0;
    memoryStats_.associativeMemoryCount = 0;
    
    for (const auto& entry : memoryEntries_) {
        switch (entry.type) {
            case MemoryEntry::Type::Working:
                memoryStats_.workingMemoryCount++;
                break;
            case MemoryEntry::Type::Episodic:
                memoryStats_.episodicMemoryCount++;
                break;
            case MemoryEntry::Type::Semantic:
                memoryStats_.semanticMemoryCount++;
                break;
            case MemoryEntry::Type::Procedural:
                memoryStats_.proceduralMemoryCount++;
                break;
            case MemoryEntry::Type::Associative:
                memoryStats_.associativeMemoryCount++;
                break;
        }
    }
    
    // Calculate capacity usage (simplified)
    currentMemoryUsage_ = memoryStats_.totalEntries / static_cast<float>(maxMemoryEntries_);
    
    // Calculate consolidation progress (simplified)
    consolidationProgress_ = memoryStats_.totalStrength / (maxMemoryEntries_ * 1.0f);
}

void MemoryRenderer::updateMemoryTimeline() {
    memoryTimeline_.clear();
    memoryStrength_.clear();
    
    if (memoryEntries_.empty()) return;
    
    // Sort by timestamp
    std::vector<MemoryEntry> sortedEntries = memoryEntries_;
    std::sort(sortedEntries.begin(), sortedEntries.end(),
              [](const MemoryEntry& a, const MemoryEntry& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Extract timeline data
    for (const auto& entry : sortedEntries) {
        memoryTimeline_.push_back(entry.timestamp);
        memoryStrength_.push_back(entry.strength);
    }
}

void MemoryRenderer::extractWorkingMemoryData(const Brain& brain) {
    // Extract working memory traces
    auto workingMemory = brain.getWorkingMemory();
    if (!workingMemory) return;
    
    size_t activeTraces = workingMemory->getActiveTraces();
    
    for (size_t i = 0; i < activeTraces && i < 20; ++i) {
        MemoryEntry entry;
        entry.type = MemoryEntry::Type::Working;
        entry.id = "wm_" + std::to_string(i);
        entry.content = "Working Memory Trace " + std::to_string(i);
        entry.strength = workingMemory->getTraceStrength(i) * 10.0f; // Scale to 0-10 range
        entry.timestamp = static_cast<Timestamp>(brain.getCurrentStep()) * 0.001f;
        entry.step = brain.getCurrentStep();
        entry.neuronId = i + 1000;
        
        memoryEntries_.push_back(entry);
    }
}

void MemoryRenderer::extractEpisodicMemoryData(const Brain& brain) {
    // Extract episodic memory episodes
    auto episodicMemory = brain.getEpisodicMemory();
    if (!episodicMemory) return;
    
    size_t episodeCount = episodicMemory->getEpisodeCount();
    
    for (size_t i = 0; i < episodeCount && i < 15; ++i) {
        MemoryEntry entry;
        entry.type = MemoryEntry::Type::Episodic;
        entry.id = "ep_" + std::to_string(i);
        entry.content = "Episode " + std::to_string(i) + " at step " + std::to_string(i * 10);
        entry.strength = (1.0f - (static_cast<float>(episodeCount - i) / episodeCount)) * 10.0f;
        entry.timestamp = static_cast<Timestamp>(i * 10) * 0.001f;
        entry.step = i * 10;
        entry.neuronId = i + 2000;
        
        memoryEntries_.push_back(entry);
    }
}

void MemoryRenderer::extractSemanticMemoryData(const Brain& brain) {
    // Extract semantic memory concepts (simplified)
    size_t numConcepts = brain.getTotalNeuronCount() / 100;
    
    for (size_t i = 0; i < numConcepts && i < 10; ++i) {
        MemoryEntry entry;
        entry.type = MemoryEntry::Type::Semantic;
        entry.id = "sm_" + std::to_string(i);
        entry.content = "Concept " + std::to_string(i) + " (concept about " + std::to_string(i * 10) + ")";
        entry.strength = (1.0f - std::abs(static_cast<float>(i) - static_cast<float>(numConcepts) / 2.0f) / 
                         (static_cast<float>(numConcepts) / 2.0f)) * 10.0f;
        entry.timestamp = static_cast<Timestamp>(brain.getCurrentStep()) * 0.001f;
        entry.step = brain.getCurrentStep();
        entry.neuronId = i + 3000;
        
        memoryEntries_.push_back(entry);
    }
}

void MemoryRenderer::extractProceduralMemoryData(const Brain& brain) {
    // Extract procedural memory skills (simplified)
    size_t numSkills = 5;
    
    for (size_t i = 0; i < numSkills; ++i) {
        MemoryEntry entry;
        entry.type = MemoryEntry::Type::Procedural;
        entry.id = "pm_" + std::to_string(i);
        entry.content = "Skill " + std::to_string(i) + ": Action " + std::to_string(i) + 
                       " with proficiency " + std::to_string(i * 0.2f);
        entry.strength = (i + 1) * 2.0f; // Skills 1-5 with strengths 2-10
        entry.timestamp = static_cast<Timestamp>(brain.getCurrentStep()) * 0.001f;
        entry.step = brain.getCurrentStep();
        entry.neuronId = i + 4000;
        
        memoryEntries_.push_back(entry);
    }
}

void MemoryRenderer::extractAssociativeMemoryData(const Brain& brain) {
    // Extract associative memory associations (simplified)
    size_t numAssociations = brain.getTotalSynapseCount() / 100;
    
    for (size_t i = 0; i < numAssociations && i < 20; ++i) {
        MemoryEntry entry;
        entry.type = MemoryEntry::Type::Associative;
        entry.id = "am_" + std::to_string(i);
        entry.content = "Association between neuron " + std::to_string(i * 10) + 
                       " and neuron " + std::to_string(i * 10 + 5);
        entry.strength = static_cast<float>(rand()) / RAND_MAX * 10.0f;
        entry.timestamp = static_cast<Timestamp>(brain.getCurrentStep()) * 0.001f;
        entry.step = brain.getCurrentStep();
        entry.sourceId = "src_" + std::to_string(i * 10);
        entry.targetId = "tgt_" + std::to_string(i * 10 + 5);
        
        memoryEntries_.push_back(entry);
    }
}

void MemoryRenderer::renderWorkingMemory(const Brain& brain) {
    // Render working memory as dynamic traces
    ColorManager colorManager;
    
    int gridWidth = 60;
    int gridHeight = 20;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightGreen) << "=== WORKING MEMORY ===" << "\033[0m" << std::endl;
    
    // Filter and display working memory entries
    std::vector<MemoryEntry> workingEntries;
    for (const auto& entry : memoryEntries_) {
        if (entry.type == MemoryEntry::Type::Working) {
            workingEntries.push_back(entry);
        }
    }
    
    if (workingEntries.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "No active working memory traces" << "\033[0m" << std::endl;
        return;
    }
    
    // Display traces in a grid
    for (size_t row = 0; row < workingEntries.size() && row < static_cast<size_t>(gridHeight); ++row) {
        for (int col = 0; col < gridWidth; ++col) {
            if (col == 0) {
                std::cout << colorManager.getMemoryEntryColor(workingEntries[row]) << "[" << workingEntries[row].id.substr(0, 3) << "]" << "\033[0m";
            } else if (col <= 20) {
                // Show strength indicator
                float normalizedStrength = workingEntries[row].strength / 10.0f;
                if (col < normalizedStrength * 20) {
                    std::cout << colorManager.getMemoryStrengthColor(workingEntries[row].strength) << "#" << "\033[0m";
                } else {
                    std::cout << " ";
                }
            } else {
                // Show content snippet
                std::string snippet = workingEntries[row].content.substr(0, 10);
                std::cout << snippet << std::string(20 - snippet.length(), ' ');
            }
        }
        std::cout << "\n";
    }
}

void MemoryRenderer::renderEpisodicMemory(const Brain& brain) {
    // Render episodic memory as timeline
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightCyan) << "=== EPISODIC MEMORY ===" << "\033[0m" << std::endl;
    
    // Filter and display episodic memory entries
    std::vector<MemoryEntry> episodicEntries;
    for (const auto& entry : memoryEntries_) {
        if (entry.type == MemoryEntry::Type::Episodic) {
            episodicEntries.push_back(entry);
        }
    }
    
    if (episodicEntries.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "No episodic memory stored" << "\033[0m" << std::endl;
        return;
    }
    
    // Create timeline display
    int timelineHeight = 15;
    int timelineWidth = 60;
    
    for (int y = timelineHeight - 1; y >= 0; --y) {
        for (int x = 0; x < timelineWidth; ++x) {
            // Find entry at this position
            float targetX = static_cast<float>(x) / timelineWidth * 100.0f;
            bool entryFound = false;
            
            for (const auto& entry : episodicEntries) {
                if (std::abs(targetX - (entry.timestamp * 100.0f)) < 5.0f) {
                    std::string color = colorManager.getMemoryEntryColor(entry);
                    std::cout << color << "*" << "\033[0m";
                    entryFound = true;
                    break;
                }
            }
            
            if (!entryFound) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    
    // Display episode details
    std::cout << "\n" << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Recent Episodes:" << "\033[0m" << std::endl;
    for (size_t i = 0; i < std::min<size_t>(episodicEntries.size(), 5); ++i) {
        std::cout << colorManager.getMemoryEntryColor(episodicEntries[i]) 
                  << "  " << episodicEntries[i].id << ": " << episodicEntries[i].content << " (strength: " 
                  << episodicEntries[i].strength << ")" << "\033[0m" << std::endl;
    }
}

void MemoryRenderer::renderSemanticMemory(const Brain& brain) {
    // Render semantic memory as concept network
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightYellow) << "=== SEMANTIC MEMORY ===" << "\033[0m" << std::endl;
    
    // Filter and display semantic memory entries
    std::vector<MemoryEntry> semanticEntries;
    for (const auto& entry : memoryEntries_) {
        if (entry.type == MemoryEntry::Type::Semantic) {
            semanticEntries.push_back(entry);
        }
    }
    
    if (semanticEntries.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "No semantic memory stored" << "\033[0m" << std::endl;
        return;
    }
    
    // Create concept network visualization
    int networkWidth = 40;
    int networkHeight = 20;
    
    for (int y = networkHeight - 1; y >= 0; --y) {
        for (int x = 0; x < networkWidth; ++x) {
            // Simple concept placement
            float conceptX = static_cast<float>(x) / networkWidth * 100.0f;
            bool conceptFound = false;
            
            for (const auto& entry : semanticEntries) {
                if (std::abs(conceptX - (entry.timestamp * 100.0f)) < 10.0f) {
                    std::string color = colorManager.getMemoryEntryColor(entry);
                    std::cout << color << "O" << "\033[0m";
                    conceptFound = true;
                    break;
                }
            }
            
            if (!conceptFound) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    
    // Display concept statistics
    std::cout << "\n" << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Concept Network:" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Total Concepts: " << semanticEntries.size() << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Average Strength: " << memoryStats_.avgStrength << "\033[0m" << std::endl;
}

void MemoryRenderer::renderProceduralMemory(const Brain& brain) {
    // Render procedural memory as skill hierarchy
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightRed) << "=== PROCEDURAL MEMORY ===" << "\033[0m" << std::endl;
    
    // Filter and display procedural memory entries
    std::vector<MemoryEntry> proceduralEntries;
    for (const auto& entry : memoryEntries_) {
        if (entry.type == MemoryEntry::Type::Procedural) {
            proceduralEntries.push_back(entry);
        }
    }
    
    if (proceduralEntries.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "No procedural memory stored" << "\033[0m" << std::endl;
        return;
    }
    
    // Create skill hierarchy visualization
    for (size_t i = 0; i < proceduralEntries.size(); ++i) {
        const auto& entry = proceduralEntries[i];
        
        // Draw skill with indentation based on proficiency
        std::string indent(i, ' ');
        std::string skillMarker = (i == 0) ? "> " : "  ";
        
        std::cout << colorManager.getMemoryEntryColor(entry) 
                  << indent << skillMarker << entry.content << " (level: " << entry.strength << ")" << "\033[0m" << std::endl;
    }
    
    // Display skill statistics
    std::cout << "\n" << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Skill Statistics:" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Total Skills: " << proceduralEntries.size() << "\033[0m" << std::endl;
    
    float maxSkillLevel = 0.0f;
    for (const auto& entry : proceduralEntries) {
        maxSkillLevel = std::max(maxSkillLevel, entry.strength);
    }
    
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Max Skill Level: " << maxSkillLevel << "\033[0m" << std::endl;
}

void MemoryRenderer::renderAssociativeMemory(const Brain& brain) {
    // Render associative memory as association network
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightMagenta) << "=== ASSOCIATIVE MEMORY ===" << "\033[0m" << std::endl;
    
    // Filter and display associative memory entries
    std::vector<MemoryEntry> associativeEntries;
    for (const auto& entry : memoryEntries_) {
        if (entry.type == MemoryEntry::Type::Associative) {
            associativeEntries.push_back(entry);
        }
    }
    
    if (associativeEntries.empty()) {
        std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "No associative memory stored" << "\033[0m" << std::endl;
        return;
    }
    
    // Create association network visualization
    int netWidth = 50;
    int netHeight = 20;
    
    for (int y = netHeight - 1; y >= 0; --y) {
        for (int x = 0; x < netWidth; ++x) {
            // Find association at this position
            bool associationFound = false;
            
            for (const auto& entry : associativeEntries) {
                // Simple visualization based on strength
                if (entry.strength > 5.0f && x % 3 == 0) {
                    std::string color = colorManager.getMemoryEntryColor(entry);
                    std::cout << color << "=" << "\033[0m";
                    associationFound = true;
                    break;
                }
            }
            
            if (!associationFound) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
    
    // Display association statistics
    std::cout << "\n" << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Association Network:" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Total Associations: " << associativeEntries.size() << "\033[0m" << std::endl;
    
    float totalAssociationStrength = 0.0f;
    for (const auto& entry : associativeEntries) {
        totalAssociationStrength += entry.strength;
    }
    
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Total Association Strength: " << totalAssociationStrength << "\033[0m" << std::endl;
}

void MemoryRenderer::renderConsolidatedMemory(const Brain& brain) {
    // Render consolidated memory view
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightCyan) << "=== CONSOLIDATED MEMORY ===" << "\033[0m" << std::endl;
    
    // Display overall memory statistics
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Memory Overview:" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Total Entries: " << memoryStats_.totalEntries << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Total Strength: " << memoryStats_.totalStrength << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "Average Strength: " << memoryStats_.avgStrength << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Red) << "Memory Usage: " << (memoryStats_.totalEntries / static_cast<float>(maxMemoryEntries_) * 100.0f) << "%" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Blue) << "Consolidation Progress: " << (consolidationProgress_ * 100.0f) << "%" << "\033[0m" << std::endl;
    
    std::cout << "\n" << colorManager.getColorCode(ColorManager::Color::BrightWhite) << "Memory System Distribution:" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Working Memory: " << memoryStats_.workingMemoryCount << " entries\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Cyan) << "Episodic Memory: " << memoryStats_.episodicMemoryCount << " episodes\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "Semantic Memory: " << memoryStats_.semanticMemoryCount << " concepts\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Red) << "Procedural Memory: " << memoryStats_.proceduralMemoryCount << " skills\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Magenta) << "Associative Memory: " << memoryStats_.associativeMemoryCount << " associations\033[0m" << std::endl;
}

void MemoryRenderer::renderMemoryStats() const {
    ColorManager colorManager;
    std::string statColor = colorManager.getColorCode(ColorManager::Color::White);
    
    std::cout << "\n" << statColor << "=== MEMORY STATISTICS ===" << "\033[0m" << std::endl;
    std::cout << statColor << "Total Entries: " << memoryStats_.totalEntries << "\033[0m" << std::endl;
    std::cout << statColor << "Total Strength: " << memoryStats_.totalStrength << "\033[0m" << std::endl;
    std::cout << statColor << "Average Strength: " << memoryStats_.avgStrength << "\033[0m" << std::endl;
    std::cout << statColor << "Capacity Usage: " << (memoryStats_.totalEntries / static_cast<float>(maxMemoryEntries_) * 100.0f) << "%" << "\033[0m" << std::endl;
    std::cout << statColor << "Consolidation: " << (consolidationProgress_ * 100.0f) << "%" << "\033[0m" << std::endl;
}

void MemoryRenderer::renderControls() const {
    ColorManager colorManager;
    std::string helpColor = colorManager.getColorCode(ColorManager::Color::BrightYellow);
    
    std::cout << "\033[2J\033[H"; // Clear screen (temporary)
    
    std::cout << helpColor << "=== MEMORY VISUALIZATION CONTROLS ===" << "\033[0m" << std::endl;
    std::cout << helpColor << "1-5: Change Memory View Mode (Working/Episodic/Semantic/Procedural/Associative)" << "\033[0m" << std::endl;
    std::cout << helpColor << "C: Toggle Consolidated View" << "\033[0m" << std::endl;
    std::cout << helpColor << "L: Toggle Memory Labels" << "\033[0m" << std::endl;
    std::cout << helpColor << "S: Toggle Strength Indicators" << "\033[0m" << std::endl;
    std::cout << helpColor << "A: Toggle Age Gradient" << "\033[0m" << std::endl;
    std::cout << helpColor << "R: Reset Memory System" << "\033[0m" << std::endl;
    std::cout << helpColor << "Q: Quit" << "\033[0m" << std::endl;
    std::cout << std::endl;
    
    // Show current mode
    std::cout << helpColor << "Memory Mode: " << static_cast<int>(memoryViewMode_)
              << "  Analysis: " << static_cast<int>(memoryAnalysisType_)
              << "  Consolidation: " << (autoConsolidation_ ? "ON" : "OFF") << "\033[0m" << std::endl;
}

std::string MemoryRenderer::getMemoryEntryColor(const MemoryEntry& entry) const {
    ColorManager colorManager;
    
    switch (entry.type) {
        case MemoryEntry::Type::Working:
            return colorManager.getMemoryEntryColor(entry.strength);
        case MemoryEntry::Type::Episodic:
            return colorManager.getEpisodicMemoryColor(entry.step);
        case MemoryEntry::Type::Semantic:
            return colorManager.getColorCode(ColorManager::Color::BrightYellow);
        case MemoryEntry::Type::Procedural:
            return colorManager.getColorCode(ColorManager::Color::BrightRed);
        case MemoryEntry::Type::Associative:
            return colorManager.getColorCode(ColorManager::Color::BrightMagenta);
        default:
            return colorManager.getColorCode(ColorManager::Color::White);
    }
}

std::string MemoryRenderer::getMemoryStrengthColor(float strength) const {
    ColorManager colorManager;
    
    if (strength > 8.0f) {
        return colorManager.getColorCode(ColorManager::Color::BrightRed);
    } else if (strength > 6.0f) {
        return colorManager.getColorCode(ColorManager::Color::Yellow);
    } else if (strength > 4.0f) {
        return colorManager.getColorCode(ColorManager::Color::Green);
    } else if (strength > 2.0f) {
        return colorManager.getColorCode(ColorManager::Color::Cyan);
    } else {
        return colorManager.getColorCode(ColorManager::Color::Blue);
    }
}

std::string MemoryRenderer::getMemoryAgeColor(float age) const {
    ColorManager colorManager;
    
    if (age > 100.0f) {
        return colorManager.getColorCode(ColorManager::Color::BrightBlack);
    } else if (age > 50.0f) {
        return colorManager.getColorCode(ColorManager::Color::BrightWhite);
    } else if (age > 20.0f) {
        return colorManager.getColorCode(ColorManager::Color::White);
    } else if (age > 5.0f) {
        return colorManager.getColorCode(ColorManager::Color::Green);
    } else {
        return colorManager.getColorCode(ColorManager::Color::BrightGreen);
    }
}

float MemoryRenderer::calculateMemoryAge(Timestamp timestamp) const {
    // Calculate age based on current time
    return static_cast<float>(timestamp);
}

float MemoryRenderer::calculateMemoryRecencyWeight(Timestamp timestamp) const {
    // Calculate recency weight - older memories have less weight
    float age = calculateMemoryAge(timestamp);
    return std::exp(-age / 50.0f); // Exponential decay
}

std::string MemoryRenderer::formatMemoryContent(const MemoryEntry& entry) const {
    // Format memory content for display
    std::stringstream ss;
    ss << "[" << entry.id << "] " << entry.content << " (strength: " << entry.strength << ")";
    return ss.str();
}

} // namespace nlm
