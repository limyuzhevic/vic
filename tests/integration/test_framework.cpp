// Integration Test Framework
// Common test utilities and macros for NLM integration tests

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

// Test macros for NLM integration tests
#define REQUIRE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "TEST FAILED: " #condition " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define REQUIRE_EQ(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            std::cerr << "TEST FAILED: " #actual " (" << (actual) << ") != " #expected " (" << (expected) << ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define REQUIRE_NE(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            std::cerr << "TEST FAILED: " #actual " (" << (actual) << ") should not equal " #expected " (" << (expected) << ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define REQUIRE_GT(actual, expected) \
    do { \
        if ((actual) <= (expected)) { \
            std::cerr << "TEST FAILED: " #actual " (" << (actual) << ") should be greater than " #expected " (" << (expected) << ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define REQUIRE_LT(actual, expected) \
    do { \
        if ((actual) >= (expected)) { \
            std::cerr << "TEST FAILED: " #actual " (" << (actual) << ") should be less than " #expected " (" << (expected) << ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define REQUIRE_TRUE(condition) REQUIRE(condition)

#define REQUIRE_FALSE(condition) REQUIRE(!(condition))

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
};

// Integration test utilities
class IntegrationTestFramework {
public:
    static std::shared_ptr<nlm::Config> createDefaultConfig() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01f, nlm::ConfigSource::Default);
        return config;
    }
    
    static bool brainStateVerifier(const nlm::Brain& brain) {
        REQUIRE(brain.getTotalNeuronCount() > 0);
        REQUIRE(brain.getTotalSynapseCount() > 0);
        REQUIRE(brain.getRegionCount() > 0);
        return true;
    }
    
    static bool memorySystemVerifier(const nlm::Brain& brain) {
        auto workingMemory = brain.getWorkingMemory();
        auto episodicMemory = brain.getEpisodicMemory();
        auto associativeMemory = brain.getAssociativeMemory();
        
        REQUIRE(workingMemory != nullptr);
        REQUIRE(episodicMemory != nullptr);
        REQUIRE(associativeMemory != nullptr);
        
        return true;
    }
    
    static bool cognitionSystemVerifier(const nlm::Brain& brain) {
        auto planner = brain.getPlanner();
        auto conceptFormation = brain.getConceptFormation();
        auto attention = brain.getAttention();
        
        REQUIRE(planner != nullptr);
        REQUIRE(conceptFormation != nullptr);
        REQUIRE(attention != nullptr);
        
        return true;
    }
    
    static bool predictionSystemVerifier(const nlm::Brain& brain) {
        auto predictionSystem = brain.getPredictionSystem();
        auto dopamine = brain.getDopamine();
        auto curiosity = brain.getCuriosity();
        auto novelty = brain.getNovelty();
        auto predictionError = brain.getPredictionError();
        
        REQUIRE(predictionSystem != nullptr);
        REQUIRE(dopamine != nullptr);
        REQUIRE(curiosity != nullptr);
        REQUIRE(novelty != nullptr);
        REQUIRE(predictionError != nullptr);
        
        return true;
    }
    
    static bool developmentSystemVerifier(const nlm::Brain& brain) {
        auto developmentSystem = brain.getDevelopmentSystem();
        REQUIRE(developmentSystem != nullptr);
        return true;
    }
};
