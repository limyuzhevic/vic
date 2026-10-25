// Documentation implementation
#include "Documentation.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <unordered_map>

namespace nlm {

// DefaultDocumentationGenerator implementation
void DefaultDocumentationGenerator::setPluginManager(void* manager) {
    std::lock_guard<std::mutex> lock(mutex_);
    pluginManager_ = static_cast<IPluginManager*>(manager);
}

void* DefaultDocumentationGenerator::getPluginManager() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pluginManager_;
}

} // namespace nlm
