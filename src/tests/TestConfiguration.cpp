#include "TestConfiguration.hpp"
#include <sstream>
#include <iostream>

namespace nlm {

TestConfiguration* TestConfiguration::instance = nullptr;

void TestConfiguration::setGlobalConfig(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(configMutex);
    globalConfig[key] = value;
}

const std::string& TestConfiguration::getGlobalConfig(const std::string& key) const {
    static const std::string emptyString;
    std::lock_guard<std::mutex> lock(configMutex);
    auto it = globalConfig.find(key);
    return it != globalConfig.end() ? it->second : emptyString;
}

void TestConfiguration::addTestConfig(const std::string& testName, const TestParameters& config) {
    std::lock_guard<std::mutex> lock(configMutex);
    testConfigs[testName] = config;
}

TestParameters& TestConfiguration::getTestConfig(const std::string& testName) {
    std::lock_guard<std::mutex> lock(configMutex);
    return testConfigs[testName];
}

void TestConfiguration::enableCategory(TestCategory category) {
    std::lock_guard<std::mutex> lock(configMutex);
    switch (category) {
        case TestCategory::NEURAL_DYNAMICS:
            enabledCategories.push_back("neural_dynamics");
            break;
        case TestCategory::PLASTICITY:
            enabledCategories.push_back("plasticity");
            break;
        case TestCategory::MEMORY_SYSTEM:
            enabledCategories.push_back("memory_system");
            break;
        case TestCategory::PREDICTION:
            enabledCategories.push_back("prediction");
            break;
        case TestCategory::COGNITION:
            enabledCategories.push_back("cognition");
            break;
        case TestCategory::NEUROMODULATION:
            enabledCategories.push_back("neuromodulation");
            break;
        case TestCategory::DEVELOPMENT:
            enabledCategories.push_back("development");
            break;
        case TestCategory::MULTI_AGENT:
            enabledCategories.push_back("multi_agent");
            break;
        case TestCategory::PERFORMANCE:
            enabledCategories.push_back("performance");
            break;
        case TestCategory::MEMORY_LEAK:
            enabledCategories.push_back("memory_leak");
            break;
        case TestCategory::INTEGRATION:
            enabledCategories.push_back("integration");
            break;
        case TestCategory::REGRESSION:
            enabledCategories.push_back("regression");
            break;
    }
}

void TestConfiguration::disableCategory(TestCategory category) {
    std::lock_guard<std::mutex> lock(configMutex);
    switch (category) {
        case TestCategory::NEURAL_DYNAMICS:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "neural_dynamics"),
                enabledCategories.end()
            );
            break;
        case TestCategory::PLASTICITY:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "plasticity"),
                enabledCategories.end()
            );
            break;
        case TestCategory::MEMORY_SYSTEM:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "memory_system"),
                enabledCategories.end()
            );
            break;
        case TestCategory::PREDICTION:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "prediction"),
                enabledCategories.end()
            );
            break;
        case TestCategory::COGNITION:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "cognition"),
                enabledCategories.end()
            );
            break;
        case TestCategory::NEUROMODULATION:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "neuromodulation"),
                enabledCategories.end()
            );
            break;
        case TestCategory::DEVELOPMENT:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "development"),
                enabledCategories.end()
            );
            break;
        case TestCategory::MULTI_AGENT:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "multi_agent"),
                enabledCategories.end()
            );
            break;
        case TestCategory::PERFORMANCE:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "performance"),
                enabledCategories.end()
            );
            break;
        case TestCategory::MEMORY_LEAK:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "memory_leak"),
                enabledCategories.end()
            );
            break;
        case TestCategory::INTEGRATION:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "integration"),
                enabledCategories.end()
            );
            break;
        case TestCategory::REGRESSION:
            enabledCategories.erase(
                std::remove(enabledCategories.begin(), enabledCategories.end(), "regression"),
                enabledCategories.end()
            );
            break;
    }
}

bool TestConfiguration::isCategoryEnabled(TestCategory category) const {
    std::lock_guard<std::mutex> lock(configMutex);
    switch (category) {
        case TestCategory::NEURAL_DYNAMICS:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "neural_dynamics") != enabledCategories.end();
        case TestCategory::PLASTICITY:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "plasticity") != enabledCategories.end();
        case TestCategory::MEMORY_SYSTEM:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "memory_system") != enabledCategories.end();
        case TestCategory::PREDICTION:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "prediction") != enabledCategories.end();
        case TestCategory::COGNITION:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "cognition") != enabledCategories.end();
        case TestCategory::NEUROMODULATION:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "neuromodulation") != enabledCategories.end();
        case TestCategory::DEVELOPMENT:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "development") != enabledCategories.end();
        case TestCategory::MULTI_AGENT:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "multi_agent") != enabledCategories.end();
        case TestCategory::PERFORMANCE:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "performance") != enabledCategories.end();
        case TestCategory::MEMORY_LEAK:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "memory_leak") != enabledCategories.end();
        case TestCategory::INTEGRATION:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "integration") != enabledCategories.end();
        case TestCategory::REGRESSION:
            return std::find(enabledCategories.begin(), enabledCategories.end(), "regression") != enabledCategories.end();
        default:
            return true;
    }
}

std::vector<std::string> TestConfiguration::getEnabledCategories() const {
    std::lock_guard<std::mutex> lock(configMutex);
    return enabledCategories;
}

void TestConfiguration::setDefaultTimeout(double seconds) {
    std::lock_guard<std::mutex> lock(configMutex);
    for (auto& pair : testConfigs) {
        pair.second.timeout = std::chrono::duration<double>(seconds);
    }
}

void TestConfiguration::setDefaultMemoryLimit(double megabytes) {
    std::lock_guard<std::mutex> lock(configMutex);
    for (auto& pair : testConfigs) {
        pair.second.maxMemory = std::chrono::duration<double>(megabytes * 1024 * 1024);
    }
}

std::string TestConfiguration::toString() const {
    std::lock_guard<std::mutex> lock(configMutex);
    std::stringstream ss;
    ss << "=== Test Configuration ===" << std::endl;
    ss << "Global Config:" << std::endl;
    for (const auto& pair : globalConfig) {
        ss << "  " << pair.first << " = " << pair.second << std::endl;
    }
    ss << "Enabled Categories:" << std::endl;
    for (const auto& category : enabledCategories) {
        ss << "  " << category << std::endl;
    }
    ss << "Test Configurations:" << std::endl;
    for (const auto& pair : testConfigs) {
        ss << "  " << pair.first << ": " << pair.second.name << std::endl;
        ss << "    Level: " << static_cast<int>(pair.second.level) << std::endl;
        ss << "    Category: " << static_cast<int>(pair.second.category) << std::endl;
        ss << "    Enabled: " << (pair.second.enabled ? "true" : "false") << std::endl;
        ss << "    Timeout: " << pair.second.timeout.count() << "s" << std::endl;
        ss << "    Max Memory: " << pair.second.maxMemory.count() << " bytes" << std::endl;
    }
    return ss.str();
}

} // namespace nlm
