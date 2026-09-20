// BrainFactory.cpp

#include "BrainFactory.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Initialize factory registry with default systems
static bool factoryInitialized = []() {
    initializeDefaultFactories();
    return true;
}();

} // namespace nlm
