// Implementation of BrainSystemBase
// This file should be included after all system headers are available

#include "BrainSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

// BrainSystemBase implementation
BrainSystemBase::BrainSystemBase() : initialized_(false), brain_(nullptr) {
}

BrainSystemBase::~BrainSystemBase() {
}

bool BrainSystemBase::initialize() {
    if (!initialized_) {
        if (doInitialize()) {
            initialized_ = true;
            return true;
        }
    }
    return false;
}

void BrainSystemBase::shutdown() {
    if (initialized_) {
        doShutdown();
        initialized_ = false;
    }
}

void BrainSystemBase::step(SimulationStep step, Timestamp time) {
    if (initialized_) {
        doStep(step, time);
    }
}

bool BrainSystemBase::configure(const std::shared_ptr<Config>& config) {
    return doConfigure(config);
}

void BrainSystemBase::setBrainReference(Brain* brain) {
    brain_ = brain;
    doSetBrainReference(brain);
}

bool BrainSystemBase::isInitialized() const {
    return initialized_;
}

const std::string& BrainSystemBase::getName() const {
    return name_;
}

std::string BrainSystemBase::getStatus() const {
    return getStatusImpl();
}

size_t BrainSystemBase::getMemoryUsage() const {
    return getMemoryUsageImpl();
}

void BrainSystemBase::logStatus() const {
    Logger* logger = getLogger();
    if (logger) {
        logger->info("Status: " + getStatus());
    }
}

Logger* BrainSystemBase::getLogger() {
    if (brain_ && brain_->getConfig()) {
        return Logger::getGlobal();
    }
    return nullptr;
}

// BrainSystemFacade implementation
BrainSystemFacade::BrainSystemFacade() : initialized_(false) {
}

BrainSystemFacade::~BrainSystemFacade() {
    shutdown();
}

bool BrainSystemFacade::initialize() {
    if (!initialized_) {
        // Notify systems about initialization
        forEachSystem([](BrainSystem* sys) {
            sys->initialize();
        });
        initialized_ = true;
        return true;
    }
    return false;
}

void BrainSystemFacade::shutdown() {
    if (initialized_) {
        // Notify systems about shutdown
        forEachSystem([](BrainSystem* sys) {
            sys->shutdown();
        });
        initialized_ = false;
    }
}

void BrainSystemFacade::step(SimulationStep step, Timestamp time) {
    if (initialized_) {
        // Notify systems about step
        forEachSystem([step, time](BrainSystem* sys) {
            sys->step(step, time);
        });
    }
}

bool BrainSystemFacade::configure(const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }
    
    // Configure all systems
    forEachSystem([config](BrainSystem* sys) {
        sys->configure(config);
    });
    
    return true;
}

const std::string& BrainSystemFacade::getName() const {
    static const std::string name = "BrainSystemFacade";
    return name;
}

std::string BrainSystemFacade::getStatus() const {
    std::stringstream ss;
    ss << "BrainSystemFacade: " << getSystemCount() << " systems initialized";
    
    forEachSystem([&ss](const BrainSystem* sys) {
        ss << "\n  " << sys->getName() << ": " << (sys->isInitialized() ? "initialized" : "not initialized");
    });
    
    return ss.str();
}

bool BrainSystemFacade::isInitialized() const {
    return initialized_;
}

void BrainSystemFacade::setBrainReference(Brain* brain) {
    forEachSystem([brain](BrainSystem* sys) {
        sys->setBrainReference(brain);
    });
}

void BrainSystemFacade::logStatus() const {
    Logger* logger = getLogger();
    if (logger) {
        logger->info(getStatus());
    }
}

size_t BrainSystemFacade::getMemoryUsage() const {
    size_t totalUsage = 0;
    forEachSystem([&totalUsage](const BrainSystem* sys) {
        totalUsage += sys->getMemoryUsage();
    });
    return totalUsage;
}

void BrainSystemFacade::loadSystemConfigurations(const std::shared_ptr<Config>& config) {
    if (config) {
        configure(config);
    }
}

void BrainSystemFacade::resetAllSystems() {
    forEachSystem([](BrainSystem* sys) {
        if (sys->isInitialized()) {
            sys->shutdown();
            sys->initialize();
        }
    });
}

// SystemRegistry implementation
void SystemRegistry::initializeDefaultFactories(Brain* brain) {
    // This function should be implemented by including all system headers
    // and registering default factories
}

} // namespace nlm
