#include "Visualizers.hpp"
#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace nlm {

struct MemorySystemVisualizer::Impl {
    std::shared_ptr<VisualizationInterface> interface;
    std::ofstream outputFile;
    std::string currentFile;
    bool initialized;
    std::vector<double> memoryUsageHistory;
    
    Impl() : initialized(false) {}
    ~Impl() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }
};

MemorySystemVisualizer::MemorySystemVisualizer() : pImpl(std::make_unique<Impl>()) {}

MemorySystemVisualizer::~MemorySystemVisualizer() = default;

bool MemorySystemVisualizer::initialize(std::shared_ptr<VisualizationInterface> interface) {
    pImpl->interface = interface;
    pImpl->initialized = true;
    
    if (pImpl->interface->getConfig().saveSnapshots) {
        std::string filename = pImpl->interface->getConfig().outputDirectory + "/memory_system_visualization.txt";
        pImpl->outputFile.open(filename);
        if (!pImpl->outputFile.is_open()) {
            std::cerr << "MemorySystemVisualizer: Failed to open output file: " << filename << std::endl;
            return false;
        }
        pImpl->currentFile = filename;
    }
    
    return true;
}

void MemorySystemVisualizer::visualize(const Brain& brain, bool showWorkingMemory, bool showEpisodicMemory, bool showAssociativeMemory) {
    if (!pImpl->initialized) return;
    
    std::cout << "MemorySystemVisualizer: Visualizing memory systems" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Memory System Visualization - Step " << pImpl->interface->getConfig().snapshotInterval << std::endl;
        pImpl->outputFile << "===========================" << std::endl;
        pImpl->outputFile << "Show Working Memory: " << (showWorkingMemory ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Show Episodic Memory: " << (showEpisodicMemory ? "Yes" : "No") << std::endl;
        pImpl->outputFile << "Show Associative Memory: " << (showAssociativeMemory ? "Yes" : "No") << std::endl;
        pImpl->outputFile << std::endl;
        
        // Get memory systems from brain
        auto workingMemory = brain.getWorkingMemory();
        auto episodicMemory = brain.getEpisodicMemory();
        auto associativeMemory = brain.getAssociativeMemory();
        
        pImpl->outputFile << "Memory System Statistics:" << std::endl;
        
        // Working memory visualization
        if (showWorkingMemory && workingMemory) {
            pImpl->outputFile << "  Working Memory:" << std::endl;
            pImpl->outputFile << "    Size: " << workingMemory->getCurrentSize() << "/" << workingMemory->getMaxSize() << std::endl;
            pImpl->outputFile << "    Capacity Utilization: " << (workingMemory->getCurrentSize() / workingMemory->getMaxSize() * 100.0) << "%" << std::endl;
            
            // Track memory usage
            double usage = workingMemory->getCurrentSize() / static_cast<double>(workingMemory->getMaxSize());
            pImpl->memoryUsageHistory.push_back(usage);
            if (pImpl->memoryUsageHistory.size() > 20) {
                pImpl->memoryUsageHistory.erase(pImpl->memoryUsageHistory.begin());
            }
            
            // Show some sample items
            pImpl->outputFile << "    Sample Items:" << std::endl;
            for (size_t i = 0; i < std::min(static_cast<size_t>(5), workingMemory->getCurrentSize()); ++i) {
                pImpl->outputFile << "      Item " << i << ": Key=" << i << ", Value=" << workingMemory->retrieve(i) << std::endl;
            }
            
            pImpl->outputFile << std::endl;
        }
        
        // Episodic memory visualization
        if (showEpisodicMemory && episodicMemory) {
            pImpl->outputFile << "  Episodic Memory:" << std::endl;
            pImpl->outputFile << "    Episode Count: " << episodicMemory->getEpisodeCount() << std::endl;
            pImpl->outputFile << "    Max Episodes: " << episodicMemory->getMaxCapacity() << std::endl;
            pImpl->outputFile << "    Capacity Utilization: " << (episodicMemory->getEpisodeCount() / static_cast<double>(episodicMemory->getMaxCapacity()) * 100.0) << "%" << std::endl;
            
            // Show recent episodes
            pImpl->outputFile << "    Recent Episodes:" << std::endl;
            auto recentEpisodes = episodicMemory->getRecentEpisodes(3);
            for (size_t i = 0; i < recentEpisodes.size(); ++i) {
                const auto& episode = recentEpisodes[i];
                pImpl->outputFile << "      Episode " << (recentEpisodes.size() - i) << ": " << episode.metadata << std::endl;
                pImpl->outputFile << "        Neurons: [";
                for (size_t j = 0; j < episode.neurons.size() && j < 10; ++j) {
                    pImpl->outputFile << episode.neurons[j];
                    if (j < episode.neurons.size() - 1) pImpl->outputFile << ",";
                }
                if (episode.neurons.size() > 10) pImpl->outputFile << "...";
                pImpl->outputFile << "], Values: [";
                for (size_t j = 0; j < episode.values.size() && j < 5; ++j) {
                    pImpl->outputFile << episode.values[j];
                    if (j < episode.values.size() - 1) pImpl->outputFile << ",";
                }
                if (episode.values.size() > 5) pImpl->outputFile << "...";
                pImpl->outputFile << "]" << std::endl;
            }
            
            pImpl->outputFile << std::endl;
        }
        
        // Associative memory visualization
        if (showAssociativeMemory && associativeMemory) {
            pImpl->outputFile << "  Associative Memory:" << std::endl;
            // Note: We would need to add methods to get association statistics
            pImpl->outputFile << "    Associations: (Statistics available through API)" << std::endl;
            pImpl->outputFile << "    Pattern Database Size: Unknown (requires implementation)" << std::endl;
            pImpl->outputFile << std::endl;
        }
        
        // Memory consolidation visualization
        if (episodicMemory) {
            pImpl->outputFile << "  Memory Consolidation:" << std::endl;
            pImpl->outputFile << "    Current Episodes: " << episodicMemory->getEpisodeCount() << std::endl;
            pImpl->outputFile << "    Recent Episodes Retrieved: " << episodicMemory->getRecentEpisodes(1).size() << std::endl;
            
            // Simple consolidation visualization
            int maxBar = 50;
            int consolidatedBar = static_cast<int>(episodicMemory->getEpisodeCount() / 100.0 * maxBar); // Placeholder: assume max 100
            std::string bar = "    [";
            for (int i = 0; i < consolidatedBar; ++i) bar += "=";
            for (int i = consolidatedBar; i < maxBar; ++i) bar += " ";
            bar += "] " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
            pImpl->outputFile << bar;
            pImpl->outputFile << std::endl;
        }
        
        pImpl->outputFile << "---" << std::endl << std::endl;
    }
}

void MemorySystemVisualizer::visualizeWorkingMemory(const WorkingMemory& memory, int maxItems) {
    std::cout << "MemorySystemVisualizer: Visualizing working memory (" << memory.getCurrentSize() << " items)" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Working Memory Details:" << std::endl;
        pImpl->outputFile << "  Current Size: " << memory.getCurrentSize() << "/" << memory.getMaxSize() << std::endl;
        
        for (size_t i = 0; i < std::min(static_cast<size_t>(maxItems), memory.getCurrentSize()); ++i) {
            pImpl->outputFile << "    Item " << i << ": Key=" << i << ", Value=" << memory.retrieve(i) << std::endl;
        }
        
        pImpl->outputFile << std::endl;
    }
}

void MemorySystemVisualizer::visualizeEpisodicMemory(const EpisodicMemory& memory, int maxEpisodes) {
    std::cout << "MemorySystemVisualizer: Visualizing episodic memory (" << memory.getEpisodeCount() << " episodes)" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Episodic Memory Details:" << std::endl;
        pImpl->outputFile << "  Episode Count: " << memory.getEpisodeCount() << "/" << memory.getMaxCapacity() << std::endl;
        
        auto recent = memory.getRecentEpisodes(std::min(maxEpisodes, static_cast<int>(memory.getEpisodeCount())));
        for (size_t i = 0; i < recent.size(); ++i) {
            const auto& episode = recent[i];
            pImpl->outputFile << "    Episode " << (recent.size() - i) << ": " << episode.metadata << std::endl;
        }
        
        pImpl->outputFile << std::endl;
    }
}

void MemorySystemVisualizer::visualizeAssociativeMemory(const AssociativeMemory& memory) {
    std::cout << "MemorySystemVisualizer: Visualizing associative memory" << std::endl;
    
    if (pImpl->outputFile.is_open()) {
        pImpl->outputFile << "Associative Memory Details:" << std::endl;
        // Note: This would require additional API methods in the AssociativeMemory class
        pImpl->outputFile << "  Association Count: (requires implementation)" << std::endl;
        pImpl->outputFile << "  Pattern Similarity: (requires implementation)" << std::endl;
        pImpl->outputFile << std::endl;
    }
}

} // namespace nlm