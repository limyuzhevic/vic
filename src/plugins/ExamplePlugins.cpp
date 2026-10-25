// ExamplePlugins.cpp implementation
#include "ExamplePlugins.hpp"
#include <iostream>

namespace nlm {

// Example plugins don't need additional implementation since they use the
template-based BasicPlugin implementation

} // namespace nlm

// Create plugin factory function
extern "C" {
    NLMPlugin* createPlugin() {
        return new MemoryAnalysisPlugin();
    }
    
    void destroyPlugin(NLMPlugin* plugin) {
        delete plugin;
    }
}

// Create plugin factory function for performance optimizer
extern "C" {
    NLMPlugin* createPerformancePlugin() {
        return new PerformanceOptimizationPlugin();
    }
    
    void destroyPerformancePlugin(NLMPlugin* plugin) {
        delete plugin;
    }
}

// Create plugin factory function for visualization plugin
extern "C" {
    NLMPlugin* createVisualizationPlugin() {
        return new VisualizationPlugin();
    }
    
    void destroyVisualizationPlugin(NLMPlugin* plugin) {
        delete plugin;
    }
}

// Create plugin factory function for debug analyzer
extern "C" {
    NLMPlugin* createDebugPlugin() {
        return new DebugAnalysisPlugin();
    }
    
    void destroyDebugPlugin(NLMPlugin* plugin) {
        delete plugin;
    }
}