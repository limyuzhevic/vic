//=============================================================================
// Example: Updated Core Logger using new utility headers
//=============================================================================

#pragma once

#include "Logger.hpp"
#include "core/Utils.hpp"
#include "core/Assert.hpp"
#include "core/Exceptions.hpp"
#include <memory>
#include <mutex>

namespace nlm {

// Enhanced ConsoleLogger implementation using new utility headers
class ConsoleLogger {
public:
    ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
        NLM_ASSERT(pImpl, "Failed to create ConsoleLogger implementation");
    }
    
    ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
        NLM_ASSERT(pImpl, "Failed to create ConsoleLogger implementation");
        setLevel(level);
    }
    
    ~ConsoleLogger() = default;
    
    // Disable copying
    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;
    
    // Enable moving with exception safety
    ConsoleLogger(ConsoleLogger&& other) noexcept {
        try {
            safe_assign(pImpl, std::move(other.pImpl));
        } catch (...) {
            // Exception safety guarantee
            pImpl = nullptr;
            throw;
        }
    }
    
    ConsoleLogger& operator=(ConsoleLogger&& other) noexcept {
        if (this != &other) {
            try {
                safe_assign(pImpl, std::move(other.pImpl));
            } catch (...) {
                // Exception safety guarantee
                pImpl = nullptr;
                throw;
            }
        }
        return *this;
    }
    
    void log(const LogEntry& entry) {
        NLM_VALIDATE(&entry, "LogEntry cannot be null");
        
        if (entry.level < getLevel()) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " " << entry.message;
        
        std::string output = oss.str();
        
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        }
    }
    
    void setLevel(LogLevel level) {
        NLM_VALIDATE(static_cast<int>(level), level >= 0 && level <= 4, "Invalid log level");
        pImpl->level = level;
    }
    
    LogLevel getLevel() const {
        return pImpl->level;
    }
    
    void setUseColors(bool useColors) {
        pImpl->useColors = useColors;
    }
    
    bool getUseColors() const {
        return pImpl->useColors;
    }
    
    void setOutputFile(const std::string& filepath) {
        NLM_VALIDATE(!filepath.empty(), "File path cannot be empty");
        pImpl->fileOut.open(filepath, std::ios::app);
        NLM_ASSERT(pImpl->fileOut.is_open(), "Failed to open log file: " + filepath);
    }
    
    void closeOutputFile() {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
    }
    
private:
    struct Impl {
        LogLevel level = LogLevel::Info;
        bool useColors = true;
        std::ofstream fileOut;
        std::mutex mutex;
    };
    
    std::unique_ptr<Impl> pImpl;
    
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    static std::string levelToColor(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
};

//=============================================================================
// Example: Updated RandomGenerator using new utility headers
//=============================================================================

#pragma once

#include "Random.hpp"
#include "core/Assert.hpp"
#include "core/Exceptions.hpp"
#include <stdexcept>
#include <random>

namespace nlm {

// Enhanced RandomGenerator with comprehensive validation
class RandomGenerator {
public:
    using result_type = uint32_t;
    
    explicit RandomGenerator(uint64_t seed) : pImpl(std::make_unique<Impl>(seed)) {
        NLM_ASSERT(pImpl, "Failed to create RandomGenerator implementation");
    }
    
    RandomGenerator() : pImpl(std::make_unique<Impl>(std::random_device{}())) {
        NLM_ASSERT(pImpl, "Failed to create RandomGenerator implementation");
    }
    
    ~RandomGenerator() = default;
    
    // Disable copying
    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;
    
    // Enable moving with exception safety
    RandomGenerator(RandomGenerator&& other) noexcept {
        try {
            safe_assign(pImpl, std::move(other.pImpl));
        } catch (...) {
            // Exception safety guarantee
            pImpl = nullptr;
            throw;
        }
    }
    
    RandomGenerator& operator=(RandomGenerator&& other) noexcept {
        if (this != &other) {
            try {
                safe_assign(pImpl, std::move(other.pImpl));
            } catch (...) {
                // Exception safety guarantee
                pImpl = nullptr;
                throw;
            }
        }
        return *this;
    }
    
    void seed(uint64_t seed) {
        NLM_VALIDATE(seed, seed != 0, "Seed cannot be zero");
        pImpl->gen.seed(seed);
        pImpl->currentSeed = seed;
    }
    
    uint64_t getSeed() const {
        return pImpl->currentSeed;
    }
    
    // Enhanced uniformInt with comprehensive validation
    uint32_t uniformInt(uint32_t min, uint32_t max) {
        NLM_VALIDATE(max >= min, "max must be >= min");
        NLM_VALIDATE(min != max, "min and max cannot be equal");
        
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(pImpl->gen);
    }
    
    uint64_t uniformInt(uint64_t min, uint64_t max) {
        NLM_VALIDATE(max >= min, "max must be >= min");
        NLM_VALIDATE(min != max, "min and max cannot be equal");
        
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(pImpl->gen);
    }
    
    // Enhanced uniformReal with comprehensive validation
    double uniformReal(double min, double max) {
        NLM_VALIDATE(max > min, "max must be > min");
        
        std::uniform_real_distribution<double> dist(min, max);
        return dist(pImpl->gen);
    }
    
    float uniformReal(float min, float max) {
        NLM_VALIDATE(max > min, "max must be > min");
        
        std::uniform_real_distribution<float> dist(min, max);
        return dist(pImpl->gen);
    }
    
    // Enhanced pickRandom with validation
    template<typename Container>
    auto pickRandom(const Container& container) -> typename Container::const_reference {
        NLM_VALIDATE(!container.empty(), "Cannot pick from empty container");
        
        auto it = container.begin();
        std::advance(it, pickRandomIndex(container.size()));
        return *it;
    }
    
    // Enhanced pickRandomIndex with validation
    uint64_t pickRandomIndex(uint64_t n) {
        NLM_VALIDATE(n > 0, "Cannot pick from zero items");
        return uniformInt(0, n - 1);
    }
    
    // ... other methods ...
    
    std::mt19937_64& engine() {
        return pImpl->gen;
    }
    
    const std::mt19937_64& engine() const {
        return pImpl->gen;
    }
    
    // Static minimum/maximum for mt19937_64
    static constexpr result_type min() { return std::mt19937_64::min(); }
    static constexpr result_type max() { return std::mt19937_64::max(); }
    
    // Operator for compatibility with std functions
    result_type operator()() { return static_cast<result_type>(engine()()); }
    
private:
    struct Impl {
        std::mt19937_64 gen;
        uint64_t currentSeed;
        
        Impl(uint64_t seed) : gen(seed), currentSeed(seed) {}
    };
    
    std::unique_ptr<Impl> pImpl;
};

//=============================================================================
// Example: Updated Brain.hpp using new utility headers
//=============================================================================

#pragma once

#include "brain/core/Types.hpp"
#include "brain/core/Neuron.hpp"
#include "brain/core/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Assert.hpp"
#include "core/Exceptions.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Enhanced Brain class with comprehensive validation
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config) : config_(config) {
        NLM_VALIDATE(config_, "Brain configuration cannot be null");
        NLM_ASSERT(config_->has("random_seed"), "Configuration must have random_seed");
    }
    
    ~Brain() = default;
    
    // Disable copying
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Enable moving with exception safety
    Brain(Brain&& other) noexcept {
        try {
            safe_assign(config_, std::move(other.config_));
            // Move other members similarly
        } catch (...) {
            // Exception safety guarantee
            config_ = nullptr;
            throw;
        }
    }
    
    Brain& operator=(Brain&& other) noexcept {
        if (this != &other) {
            try {
                safe_assign(config_, std::move(other.config_));
                // Move other members similarly
            } catch (...) {
                // Exception safety guarantee
                config_ = nullptr;
                throw;
            }
        }
        return *this;
    }
    
    // Initialize brain with configuration
    bool initialize() {
        NLM_ENSURE(!isInitialized_, "Brain already initialized");
        
        try {
            // Initialize all components
            // ...
            isInitialized_ = true;
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Brain initialization failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Enhanced receiveSensoryInput with validation
    void receiveSensoryInput(const SensoryInput& input) {
        NLM_VALIDATE(&input, "SensoryInput cannot be null");
        NLM_ENSURE(isInitialized_, "Brain not initialized");
        
        // Process sensory input
        // ...
    }
    
    // Enhanced injectCurrent with validation
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        NLM_VALIDATE(neuron != INVALID_NEURON_ID, "Invalid neuron ID");
        NLM_VALIDATE(!std::isnan(current), "Current cannot be NaN");
        NLM_ENSURE(isInitialized_, "Brain not initialized");
        
        // Inject current
        // ...
    }
    
    // Enhanced injectCurrentToNeurons with validation
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        NLM_VALIDATE(!std::isnan(current), "Current cannot be NaN");
        NLM_VALIDATE(current >= 0.0f, "Current cannot be negative");
        NLM_ENSURE(isInitialized_, "Brain not initialized");
        
        // Inject current to neurons of specified type
        // ...
    }
    
    // ... other methods ...
    
    // Get random generator with validation
    RandomGenerator* getRandomGenerator() {
        NLM_ENSURE(randomGenerator_, "Random generator not initialized");
        return randomGenerator_.get();
    }
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const {
        return config_;
    }
    
    // Enhanced save with validation
    bool save(const std::string& filepath) const {
        NLM_VALIDATE(!filepath.empty(), "File path cannot be empty");
        NLM_ENSURE(isInitialized_, "Brain not initialized");
        
        try {
            // Save brain state
            // ...
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to save brain: " + std::string(e.what()));
            return false;
        }
    }
    
    // Enhanced load with validation
    bool load(const std::string& filepath) {
        NLM_VALIDATE(!filepath.empty(), "File path cannot be empty");
        NLM_ENSURE(isInitialized_, "Brain not initialized");
        
        try {
            // Load brain state
            // ...
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to load brain: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    std::shared_ptr<Config> config_;
    bool isInitialized_ = false;
    std::unique_ptr<RandomGenerator> randomGenerator_;
    // ... other members
};

} // namespace nlm

//=============================================================================
// Example: Updated main.cpp using new utility headers
//=============================================================================

// This is a simplified example showing how the new utility headers
// would be used in the main application code.

// #include "core/Config/Config.hpp"
// #include "core/Random/Random.hpp"
// #include "core/Logger/Logger.hpp"
// #include "core/Assert.hpp"
// #include "core/Exceptions.hpp"
// #include "brain/Brain.hpp"
// #include <iostream>
// 
// using namespace nlm;
// 
// int main(int argc, char** argv) {
//     // Initialize assertion mode
//     Assert::setExceptionMode(true);
//     Assert::setLogToConsole(true);
//     
//     try {
//         // Create logger
//         auto logger = std::make_shared<Logger>();
//         auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
//         logger->addLogger(consoleLogger);
//         Logger::setGlobal(logger);
//         
//         NLM_LOG_INFO("=== NLM Application Starting ===");
//         
//         // Load configuration
//         auto config = std::make_shared<Config>();
//         
//         // Validate configuration
//         NLM_ASSERT(config->has("random_seed"), "Missing required config: random_seed");
//         NLM_ASSERT(config->has("neuron_count"), "Missing required config: neuron_count");
//         
//         // Create brain
//         auto brain = std::make_shared<Brain>(config);
//         
//         // Initialize brain
//         NLM_ASSERT(brain->initialize(), "Brain initialization failed");
//         
//         // Run simulation
//         // ...
//         
//         NLM_LOG_INFO("=== NLM Application Completed Successfully ===");
//         
//     } catch (const NLMException& e) {
//         NLM_LOG_ERROR("NLM Error: " + std::string(e.what()));
//         return 1;
//     } catch (const std::exception& e) {
//         NLM_LOG_ERROR("Standard error: " + std::string(e.what()));
//         return 1;
//     } catch (...) {
//         NLM_LOG_ERROR("Unknown error occurred");
//         return 1;
//     }
//     
//     return 0;
// }