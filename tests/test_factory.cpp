// Factory Tests
#include "core/Factory/FactoryManager.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigFactory.hpp"
#include "core/Random/Random.hpp"
#include "core/Random/RandomGeneratorFactory.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/LoggerFactory.hpp"
#include "brain/Brain.hpp"
#include "brain/BrainFactory.hpp"
#include "world/World.hpp"
#include "world/WorldFactory.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentFactory.hpp"
#include <cassert>
#include <iostream>

namespace test_factory {

void testFactoryManagerSingleton() {
    std::cout << "Testing FactoryManager singleton..." << std::endl;
    
    // Get the instance multiple times - should be the same
    auto& instance1 = FactoryManager::getInstance();
    auto& instance2 = FactoryManager::getInstance();
    
    assert(&instance1 == &instance2);
    std::cout << "    FactoryManager singleton test passed" << std::endl;
}

void testFactoryRegistration() {
    std::cout << "Testing factory registration..." << std::endl;
    
    FactoryManager& factoryManager = FactoryManager::getInstance();
    
    // Check that factories are registered (via macros)
    std::vector<std::string> registeredConfigs = factoryManager.getRegisteredFactoryIds("Config");
    assert(!registeredConfigs.empty());
    std::cout << "    Config factories registered: " << registeredConfigs.size() << std::endl;
    
    std::vector<std::string> registeredRandoms = factoryManager.getRegisteredFactoryIds("RandomGenerator");
    assert(!registeredRandoms.empty());
    std::cout << "    RandomGenerator factories registered: " << registeredRandoms.size() << std::endl;
    
    std::vector<std::string> registeredLoggers = factoryManager.getRegisteredFactoryIds("Logger");
    assert(!registeredLoggers.empty());
    std::cout << "    Logger factories registered: " << registeredLoggers.size() << std::endl;
    
    std::vector<std::string> registeredBrains = factoryManager.getRegisteredFactoryIds("Brain");
    assert(!registeredBrains.empty());
    std::cout << "    Brain factories registered: " << registeredBrains.size() << std::endl;
    
    std::vector<std::string> registeredWorlds = factoryManager.getRegisteredFactoryIds("World");
    assert(!registeredWorlds.empty());
    std::cout << "    World factories registered: " << registeredWorlds.size() << std::endl;
    
    std::vector<std::string> registeredExperiments = factoryManager.getRegisteredFactoryIds("Experiment");
    assert(!registeredExperiments.empty());
    std::cout << "    Experiment factories registered: " << registeredExperiments.size() << std::endl;
}

void testFactoryConfiguration() {
    std::cout << "Testing factory configuration..." << std::endl;
    
    FactoryManager& factoryManager = FactoryManager::getInstance();
    
    // Register test config
    std::unordered_map<std::string, std::any> testConfig;
    testConfig["test_key"] = std::string("test_value");
    testConfig["test_number"] = int(42);
    testConfig["test_bool"] = bool(true);
    
    factoryManager.registerConfig("test_config", testConfig);
    
    // Retrieve and verify
    auto retrievedConfig = factoryManager.getConfig("test_config");
    assert(retrievedConfig["test_key"] == std::string("test_value"));
    assert(std::any_cast<int>(retrievedConfig["test_number"]) == 42);
    assert(std::any_cast<bool>(retrievedConfig["test_bool"]) == true);
    
    std::cout << "    Factory configuration test passed" << std::endl;
}

void testObjectCreation() {
    std::cout << "Testing object creation..." << std::endl;
    
    FactoryManager& factoryManager = FactoryManager::getInstance();
    
    // Test Config creation
    ObjectParams configParams;
    configParams.setParam("source", std::string("file"));
    configParams.setParam("merge", bool(false));
    
    auto config = factoryManager.createConfig("", configParams);
    assert(config != nullptr);
    std::cout << "    Config creation test passed" << std::endl;
    
    // Test RandomGenerator creation
    ObjectParams randomParams;
    randomParams.setParam("seed", uint64_t(12345));
    randomParams.setParam("type", std::string("deterministic"));
    
    auto randomGenerator = factoryManager.createRandomGenerator("", randomParams);
    assert(randomGenerator != nullptr);
    std::cout << "    RandomGenerator creation test passed" << std::endl;
    
    // Test Logger creation
    ObjectParams loggerParams;
    loggerParams.setParam("level", int(static_cast<int>(LogLevel::Info)));
    loggerParams.setParam("type", std::string("console"));
    loggerParams.setParam("useColors", bool(true));
    
    auto logger = factoryManager.createLogger("", loggerParams);
    assert(logger != nullptr);
    std::cout << "    Logger creation test passed" << std::endl;
    
    // Test Brain creation
    ObjectParams brainParams;
    brainParams.setParam("type", std::string("basic"));
    
    auto brain = factoryManager.createBrain("", brainParams);
    assert(brain != nullptr);
    std::cout << "    Brain creation test passed" << std::endl;
    
    // Test World creation
    ObjectParams worldParams;
    worldParams.setParam("type", std::string("simple"));
    
    auto world = factoryManager.createWorld("", worldParams);
    assert(world != nullptr);
    std::cout << "    World creation test passed" << std::endl;
    
    // Test Experiment creation
    ObjectParams experimentParams;
    experimentParams.setParam("type", std::string("basic"));
    
    auto experiment = factoryManager.createExperiment("", experimentParams);
    assert(experiment != nullptr);
    std::cout << "    Experiment creation test passed" << std::endl;
}

void testObjectPooling() {
    std::cout << "Testing object pooling..." << std::endl;
    
    FactoryManager& factoryManager = FactoryManager::getInstance();
    
    // Test Config object pooling
    auto configPooled = factoryManager.acquireFromPool<Config>("");
    if (configPooled) {
        std::cout << "    Config acquired from pool" << std::endl;
        // Release back to pool
        factoryManager.releaseToPool<Config>("", std::move(configPooled));
    }
    
    // Test RandomGenerator object pooling
    auto randomPooled = factoryManager.acquireFromPool<RandomGenerator>("");
    if (randomPooled) {
        std::cout << "    RandomGenerator acquired from pool" << std::endl;
        // Release back to pool
        factoryManager.releaseToPool<RandomGenerator>("", std::move(randomPooled));
    }
    
    // Test Logger object pooling
    auto loggerPooled = factoryManager.acquireFromPool<Logger>("");
    if (loggerPooled) {
        std::cout << "    Logger acquired from pool" << std::endl;
        // Release back to pool
        factoryManager.releaseToPool<Logger>("", std::move(loggerPooled));
    }
    
    std::cout << "    Object pooling test passed" << std::endl;
}

void testFactoryLifecycle() {
    std::cout << "Testing factory lifecycle..." << std::endl;
    
    FactoryManager& factoryManager = FactoryManager::getInstance();
    
    // Initialize all factories
    factoryManager.initializeAllFactories();
    std::cout << "    Factories initialized" << std::endl;
    
    // Check initialization status
    for (const auto& factoryId : factoryManager.getRegisteredFactoryIds()) {
        bool isInitialized = factoryManager.isInitialized(factoryId);
        if (!isInitialized) {
            std::cout << "    ERROR: Factory " << factoryId << " not initialized" << std::endl;
            assert(false);
        }
    }
    
    // Shutdown all factories
    factoryManager.shutdownAllFactories();
    std::cout << "    Factories shutdown" << std::endl;
    
    // Check initialization status after shutdown
    bool anyInitialized = false;
    for (const auto& factoryId : factoryManager.getRegisteredFactoryIds()) {
        if (factoryManager.isInitialized(factoryId)) {
            anyInitialized = true;
            break;
        }
    }
    assert(!anyInitialized);
    
    std::cout << "    Factory lifecycle test passed" << std::endl;
}

void testFactoryDiscovery() {
    std::cout << "Testing factory discovery..." << std::endl;
    
    FactoryManager& factoryManager = FactoryManager::getInstance();
    
    // Discover and auto-register factories
    factoryManager.discoverFactories();
    factoryManager.autoRegisterFactories();
    
    // Check that factories were discovered
    auto allFactories = factoryManager.getAllFactories();
    assert(!allFactories.empty());
    
    std::cout << "    Found " << allFactories.size() << " factories" << std::endl;
    std::cout << "    Factory discovery test passed" << std::endl;
}

void testFactoryTests() {
    std::cout << "Testing factory test capabilities..." << std::endl;
    
    FactoryManager& factoryManager = FactoryManager::getInstance();
    
    // Test individual factory
    bool configFactoryTest = factoryManager.testFactory("default_config", "Config");
    assert(configFactoryTest);
    
    bool randomFactoryTest = factoryManager.testFactory("default_random", "RandomGenerator");
    assert(randomFactoryTest);
    
    bool loggerFactoryTest = factoryManager.testFactory("default_logger", "Logger");
    assert(loggerFactoryTest);
    
    bool brainFactoryTest = factoryManager.testFactory("default_brain", "Brain");
    assert(brainFactoryTest);
    
    bool worldFactoryTest = factoryManager.testFactory("default_world", "World");
    assert(worldFactoryTest);
    
    bool experimentFactoryTest = factoryManager.testFactory("default_experiment", "Experiment");
    assert(experimentFactoryTest);
    
    // Get test results
    auto testResults = factoryManager.getFactoryTestsResults();
    assert(!testResults.empty());
    
    std::cout << "    All factory tests passed" << std::endl;
    std::cout << "    Factory test capabilities test passed" << std::endl;
}

void runAll() {
    testFactoryManagerSingleton();
    testFactoryRegistration();
    testFactoryConfiguration();
    testObjectCreation();
    testObjectPooling();
    testFactoryLifecycle();
    testFactoryDiscovery();
    testFactoryTests();
    
    std::cout << std::endl;
    std::cout << "=== All Factory Tests Passed! ===" << std::endl;
    std::cout << std::endl;
}

} // namespace test_factory
