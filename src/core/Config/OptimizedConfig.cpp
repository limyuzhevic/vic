#include "OptimizedConfig.hpp"
#include <sstream>
#include <typeinfo>

namespace nlm {

// Template to get value as any type
std::any Config::getAllTypes(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(configMutex);
    
    auto it = values.find(key);
    if (it != values.end()) {
        return it->second.storage;
    }
    
    return std::any();
}

} // namespace nlm
