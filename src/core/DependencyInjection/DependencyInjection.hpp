#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <typeindex>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class Logger;
class SimulationClock;
class Brain;

// Abstract factory interface for creating components
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::string getName() const = 0;
};

// Config factory
class ConfigFactory : public IComponentFactory {
public:
    std::string getName() const override { return "ConfigFactory"; }
    virtual std::shared_ptr<Config> createConfig() = 0;
};

// RandomGenerator factory
class RandomGeneratorFactory : public IComponentFactory {
public:
    std::string getName() const override { return "RandomGeneratorFactory"; }
    virtual std::unique_ptr<RandomGenerator> createRandomGenerator() = 0;
};

// Logger factory
class LoggerFactory : public IComponentFactory {
public:
    std::string getName() const override { return "LoggerFactory"; }
    virtual std::shared_ptr<Logger> createLogger() = 0;
    virtual std::shared_ptr<Logger> createConsoleLogger(LogLevel level) = 0;
};

// SimulationClock factory
class SimulationClockFactory : public IComponentFactory {
public:
    std::string getName() const override { return "SimulationClockFactory"; }
    virtual std::unique_ptr<SimulationClock> createSimulationClock(double timestep) = 0;
};

// Brain factory
class BrainFactory : public IComponentFactory {
public:
    std::string getName() const override { return "BrainFactory"; }
    virtual std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) = 0;
};

// Dependency injection container
class DIContainer {
public:
    static DIContainer& getInstance() {
        static DIContainer instance;
        return instance;
    }
    
    // Register a factory
    template<typename TFactory>
    void registerFactory(std::unique_ptr<TFactory> factory) {
        std::type_index typeIndex(typeid(TFactory));
        factories_[typeIndex] = std::move(factory);
    }
    
    // Get a factory
    template<typename TFactory>
    TFactory* getFactory() {
        std::type_index typeIndex(typeid(TFactory));
        auto it = factories_.find(typeIndex);
        if (it != factories_.end()) {
            return dynamic_cast<TFactory*>(it->second.get());
        }
        return nullptr;
    }
    
    // Check if a factory is registered
    template<typename TFactory>
    bool hasFactory() const {
        std::type_index typeIndex(typeid(TFactory));
        return factories_.find(typeIndex) != factories_.end();
    }
    
    // Create a component using registered factories
    std::shared_ptr<Config> createConfig() {
        auto factory = getFactory<ConfigFactory>();
        if (factory) {
            return factory->createConfig();
        }
        // Default factory if none registered
        return std::make_shared<Config>();
    }
    
    std::unique_ptr<RandomGenerator> createRandomGenerator() {
        auto factory = getFactory<RandomGeneratorFactory>();
        if (factory) {
            return factory->createRandomGenerator();
        }
        return nullptr;
    }
    
    std::shared_ptr<Logger> createLogger() {
        auto factory = getFactory<LoggerFactory>();
        if (factory) {
            return factory->createLogger();
        }
        return nullptr;
    }
    
    std::shared_ptr<Logger> createConsoleLogger(LogLevel level) {
        auto factory = getFactory<LoggerFactory>();
        if (factory) {
            return factory->createConsoleLogger(level);
        }
        return nullptr;
    }
    
    std::unique_ptr<SimulationClock> createSimulationClock(double timestep) {
        auto factory = getFactory<SimulationClockFactory>();
        if (factory) {
            return factory->createSimulationClock(timestep);
        }
        return nullptr;
    }
    
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        auto factory = getFactory<BrainFactory>();
        if (factory) {
            return factory->createBrain(config);
        }
        return nullptr;
    }
    
    // Get all registered factory names
    std::vector<std::string> getRegisteredFactoryNames() const {
        std::vector<std::string> names;
        for (const auto& pair : factories_) {
            auto factory = dynamic_cast<IComponentFactory*>(pair.second.get());
            if (factory) {
                names.push_back(factory->getName());
            }
        }
        return names;
    }
    
    // Default factories (built-in implementations)
    class DefaultConfigFactory : public ConfigFactory {
    public:
        std::string getName() const override { return "DefaultConfigFactory"; }
        std::shared_ptr<Config> createConfig() override {
            return std::make_shared<Config>();
        }
    };
    
    class DefaultRandomGeneratorFactory : public RandomGeneratorFactory {
    public:
        std::string getName() const override { return "DefaultRandomGeneratorFactory"; }
        std::unique_ptr<RandomGenerator> createRandomGenerator() override {
            return std::make_unique<RandomGenerator>();
        }
    };
    
    class DefaultLoggerFactory : public LoggerFactory {
    public:
        std::string getName() const override { return "DefaultLoggerFactory"; }
        std::shared_ptr<Logger> createLogger() override {
            return std::make_shared<Logger>();
        }
        std::shared_ptr<Logger> createConsoleLogger(LogLevel level) override {
            return std::make_shared<ConsoleLogger>(level);
        }
    };
    
    class DefaultSimulationClockFactory : public SimulationClockFactory {
    public:
        std::string getName() const override { return "DefaultSimulationClockFactory"; }
        std::unique_ptr<SimulationClock> createSimulationClock(double timestep) override {
            return std::make_unique<SimulationClock>(timestep);
        }
    };
    
    class DefaultBrainFactory : public BrainFactory {
    public:
        std::string getName() const override { return "DefaultBrainFactory"; }
        std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) override {
            return std::make_shared<Brain>(config);
        }
    };
    
private:
    DIContainer() {
        // Register default factories
        registerFactory(std::make_unique<DefaultConfigFactory>());
        registerFactory(std::make_unique<DefaultRandomGeneratorFactory>());
        registerFactory(std::make_unique<DefaultLoggerFactory>());
        registerFactory(std::make_unique<DefaultSimulationClockFactory>());
        registerFactory(std::make_unique<DefaultBrainFactory>());
    }
    
    std::unordered_map<std::type_index, std::unique_ptr<IComponentFactory>> factories_;
};

// Service locator pattern for convenience
class ServiceLocator {
public:
    static void setContainer(DIContainer& container) {
        container_ = &container;
    }
    
    template<typename TFactory>
    static TFactory* getFactory() {
        if (!container_) return nullptr;
        return container_->getFactory<TFactory>();
    }
    
    static DIContainer* getContainer() {
        return container_;
    }
    
private:
    static DIContainer* container_;
};

// Initialize static member
DIContainer* ServiceLocator::container_ = nullptr;

} // namespace nlm