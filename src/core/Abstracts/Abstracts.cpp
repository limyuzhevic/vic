// Abstract Classes Implementation

#include "Abstracts.hpp"

namespace nlm {
    
    // IBrain static method implementation
    std::unique_ptr<IBrain> IBrain::createBrain(const Config& config) {
        return IComponentFactory::getInstance().createBrain(config);
    }
    
    // ILogger static method implementation
    std::shared_ptr<ILogger> ILogger::getGlobalLogger() {
        return IComponentFactory::getInstance().createLogger();
    }
    
    void ILogger::setGlobalLogger(std::shared_ptr<ILogger> logger) {
        IComponentFactory::setFactory(std::make_unique<IComponentFactory>());
    }
    
    // IComponentFactory singleton implementation
    namespace FactoryImpl {
        std::unique_ptr<IComponentFactory> globalFactory;
    }
    
    IComponentFactory& IComponentFactory::getInstance() {
        if (!FactoryImpl::globalFactory) {
            // Create default factory implementation
            FactoryImpl::globalFactory = std::make_unique<IComponentFactory>();
        }
        return *FactoryImpl::globalFactory;
    }
    
    void IComponentFactory::setFactory(std::unique_ptr<IComponentFactory> factory) {
        FactoryImpl::globalFactory = std::move(factory);
    }
    
} // namespace nlm