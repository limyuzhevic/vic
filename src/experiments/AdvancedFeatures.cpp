// ============================================================================
// Global instances for advanced features
// ============================================================================

#include "AdvancedBatchProcessor.hpp"
#include "ConfigTemplateManager.hpp"
#include "PerformanceMonitor.hpp"
#include "PluginManager.hpp"
#include "VisualizationTools.hpp"

namespace nlm {

std::unique_ptr<AdvancedBatchProcessor> gBatchProcessor = nullptr;
std::unique_ptr<ConfigTemplateManager> gConfigTemplateManager = nullptr;
std::unique_ptr<PerformanceMonitor> gPerformanceMonitor = nullptr;
std::unique_ptr<PluginManager> gPluginManager = nullptr;
std::unique_ptr<VisualizationTools> gVisualizationTools = nullptr;

} // namespace nlm
