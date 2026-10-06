#pragma once

#include <string>

namespace nlm {

// Memory Analysis Tools Demo Header
// This header declares the MemoryAnalysisToolsDemo class

class MemoryAnalysisToolsDemo {
public:
    MemoryAnalysisToolsDemo();
    ~MemoryAnalysisToolsDemo();
    
    void run();

private:
    // Helper method to create test configurations
    std::shared_ptr<Config> createConfig(int neuronCount, int regionCount, float connectionProb);
};

} // namespace nlm