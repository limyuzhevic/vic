#pragma once

#include "core/Factory/FactoryManager.hpp"
#include "Logger.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <any>
#include <mutex>
#include <spdlog/spdlog.h>

namespace nlm {

class LoggerFactory : public ObjectFactory {
public:
    LoggerFactory() = default;
    ~LoggerFactory() override = default;
    
    // ObjectFactory interface implementation
    std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by LoggerFactory
    }
    std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by LoggerFactory
    }
    std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) override;
    std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by LoggerFactory
    }
    std::unique_ptr<World> createWorld(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by LoggerFactory
    }
    std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) override {
        return nullptr; // Not supported by LoggerFactory
    }
    
    // Factory metadata
    std::string getFactoryName() const override {
        return "LoggerFactory";
    }
    
    std::string getSupportedType() const override {
        return "Logger";
    }
    
    void configure(const std::unordered_map<std::string, std::any>& config) override {
        config_ = config;
        spdlog::info("LoggerFactory configured with {} parameters", config.size());
    }
    
    // Factory lifecycle
    bool isInitialized() const override {
        return initialized_;
    }
    
    void initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!initialized_) {
            spdlog::info("LoggerFactory initializing");
            initialized_ = true;
        }
    }
    
    void shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        spdlog::info("LoggerFactory shutting down");
        initialized_ = false;
    }
    
    // Factory discovery registration
    void registerFactory() override {
        FactoryManager::getInstance().registerFactory<LoggerFactory>(getFactoryName(), getSupportedType(), true);
    }
    
    // Factory cloning
    std::unique_ptr<ObjectFactory> clone() const override {
        return std::make_unique<LoggerFactory>(*this);
    }
    
    // Factory registration macro
    REGISTER_FACTORY(LoggerFactory, "default_logger", "Logger", true)
    
    // Logger-specific factory methods
    std::unique_ptr<Logger> createConsoleLogger();
    std::unique_ptr<Logger> createFileLogger(const std::string& filepath);
    std::unique_ptr<Logger> createMultiLogger(const std::vector<std::string>& logPaths);
    std::unique_ptr<Logger> createAsynchronousLogger();
    std::unique_ptr<Logger> createCustomLogger(const std::function<void(const LogEntry&)>& handler);
    
    // Object pooling for Logger objects
    std::unique_ptr<Logger> acquireLoggerFromPool();
    void releaseLoggerToPool(std::unique_ptr<Logger> logger);
    
private:
    mutable std::mutex mutex_;
    bool initialized_ = false;
    std::unordered_map<std::string, std::any> config_;
    std::vector<std::unique_ptr<Logger>> loggerPool_;
};

} // namespace nlm
