#include "Brain.hpp"
#include "PluginSystem.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Plugin integration with Brain
// This demonstrates how plugins can be integrated into the existing brain architecture

// Initialize brain with plugin system
bool Brain::initialize() {
    // Initialize core brain systems first
    if (!BrainCore::initialize()) {
        return false;
    }

    // Initialize plugin system
    if (!PluginSystem::getInstance().initialize(this, config)) {
        return false;
    }

    return true;
}

// Update brain with plugin integration
void Brain::step(SimulationStep currentStep) {
    // Update core brain systems
    BrainCore::step(currentStep);

    // Update plugins
    PluginSystem::getInstance().update(currentStep);

    // Process any plugin-generated events
    PluginSystem::getInstance().publishEvent("step_completed", currentStep);
}

// Shutdown brain with plugin cleanup
void Brain::shutdown() {
    // Shutdown plugins first
    PluginSystem::getInstance().shutdown();

    // Shutdown core systems
    BrainCore::shutdown();
}

} // namespace nlm