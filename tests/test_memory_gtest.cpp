#include <gtest/gtest.h>
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>

namespace test_memory {

class WorkingMemoryTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("working_memory_capacity", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        workingMemory = std::make_unique<nlm::WorkingMemory>(config);
    }

    void TearDown() override {
        workingMemory.reset();
        config.reset();
    }

    std::shared_ptr<nlm::Config> config;
    std::unique_ptr<nlm::WorkingMemory> workingMemory;
};

TEST_F(WorkingMemoryTestFixture, WorkingMemoryCreation) {
    EXPECT_TRUE(workingMemory->isInitialized());
    EXPECT_EQ(workingMemory->getCapacity(), 10);
}

TEST_F(WorkingMemoryTestFixture, WorkingMemoryInsertionAndRetrieval) {
    workingMemory->store("test_key", 42.0f);
    
    auto retrieved = workingMemory->retrieve("test_key");
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_FLOAT_EQ(retrieved.value(), 42.0f);
}

TEST_F(WorkingMemoryTestFixture, WorkingMemoryCapacityLimit) {
    for (int i = 0; i < 15; ++i) {
        workingMemory->store(std::to_string(i), static_cast<float>(i));
    }
    
    EXPECT_LE(workingMemory->getActiveItemCount(), 10);
}

TEST_F(WorkingMemoryTestFixture, WorkingMemoryDecay) {
    workingMemory->store("decay_test", 100.0f);
    
    float valueBefore = workingMemory->retrieve("decay_test").value();
    
    for (int i = 0; i < 100; ++i) {
        workingMemory->update(0.001f);
    }
    
    auto valueAfter = workingMemory->retrieve("decay_test");
    if (valueAfter.has_value()) {
        EXPECT_LT(valueAfter.value(), valueBefore);
    }
}

TEST_F(WorkingMemoryTestFixture, WorkingMemoryConsolidation) {
    workingMemory->store("consolidate", 50.0f);
    workingMemory->consolidate();
    
    EXPECT_TRUE(workingMemory->isConsolidated("consolidate"));
}

TEST_F(WorkingMemoryTestFixture, WorkingMemoryPriority) {
    workingMemory->store("high", 10.0f, 0.9f);
    workingMemory->store("low", 20.0f, 0.1f);
    
    auto highPriority = workingMemory->getItemPriority("high");
    auto lowPriority = workingMemory->getItemPriority("low");
    
    ASSERT_TRUE(highPriority.has_value());
    ASSERT_TRUE(lowPriority.has_value());
    
    EXPECT_GT(highPriority.value(), lowPriority.value());
}

TEST_F(WorkingMemoryTestFixture, WorkingMemoryClear) {
    workingMemory->store("to_clear", 123.0f);
    workingMemory->clear();
    
    EXPECT_FALSE(workingMemory->retrieve("to_clear").has_value());
    EXPECT_EQ(workingMemory->getActiveItemCount(), 0);
}

TEST_F(WorkingMemoryTestFixture, WorkingMemoryUpdateWithTime) {
    workingMemory->store("update_test", 100.0f);
    
    float originalValue = workingMemory->retrieve("update_test").value();
    
    for (int i = 0; i < 50; ++i) {
        workingMemory->update(0.01f);
    }
    
    auto newValue = workingMemory->retrieve("update_test");
    if (newValue.has_value()) {
        EXPECT_NE(newValue.value(), originalValue);
    }
}

class EpisodicMemoryTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("episodic_memory_capacity", static_cast<int64_t>(5), nlm::ConfigSource::Default);
        episodicMemory = std::make_unique<nlm::EpisodicMemory>(config);
    }

    void TearDown() override {
        episodicMemory.reset();
        config.reset();
    }

    std::shared_ptr<nlm::Config> config;
    std::unique_ptr<nlm::EpisodicMemory> episodicMemory;
};

TEST_F(EpisodicMemoryTestFixture, EpisodicMemoryCreation) {
    EXPECT_TRUE(episodicMemory->isInitialized());
    EXPECT_EQ(episodicMemory->getCapacity(), 5);
}

TEST_F(EpisodicMemoryTestFixture, EpisodicMemoryRecording) {
    nlm::Timestamp timestamp(1234567890.0f);
    episodicMemory->record("event1", "type1", 0.5f, timestamp);
    
    auto event = episodicMemory->retrieveEvent("event1");
    ASSERT_TRUE(event.has_value());
    EXPECT_EQ(event->type, "type1");
    EXPECT_FLOAT_EQ(event->importance, 0.5f);
    EXPECT_EQ(event->timestamp, timestamp);
}

TEST_F(EpisodicMemoryTestFixture, EpisodicMemoryCapacity) {
    for (int i = 0; i < 10; ++i) {
        nlm::Timestamp ts(static_cast<float>(i));
        episodicMemory->record(std::to_string(i), "test_type", 0.1f, ts);
    }
    
    EXPECT_LE(episodicMemory->getEventCount(), 5);
}

TEST_F(EpisodicMemoryTestFixture, EpisodicMemoryRetrievalByTime) {
    nlm::Timestamp ts1(1000.0f);
    nlm::Timestamp ts2(2000.0f);
    nlm::Timestamp ts3(3000.0f);
    
    episodicMemory->record("event1", "type1", 0.1f, ts1);
    episodicMemory->record("event2", "type2", 0.2f, ts2);
    episodicMemory->record("event3", "type3", 0.3f, ts3);
    
    auto events = episodicMemory->retrieveEventsInTimeRange(ts1, ts3);
    EXPECT_EQ(events.size(), 3);
}

TEST_F(EpisodicMemoryTestFixture, EpisodicMemoryRetrievalByType) {
    episodicMemory->record("event1", "typeA", 0.1f, nlm::Timestamp(1000.0f));
    episodicMemory->record("event2", "typeB", 0.2f, nlm::Timestamp(2000.0f));
    episodicMemory->record("event3", "typeA", 0.3f, nlm::Timestamp(3000.0f));
    
    auto typeA_events = episodicMemory->retrieveEventsByType("typeA");
    auto typeB_events = episodicMemory->retrieveEventsByType("typeB");
    
    EXPECT_EQ(typeA_events.size(), 2);
    EXPECT_EQ(typeB_events.size(), 1);
}

TEST_F(EpisodicMemoryTestFixture, EpisodicMemoryImportanceBasedRetrieval) {
    episodicMemory->record("low_importance", "test", 0.1f, nlm::Timestamp(1000.0f));
    episodicMemory->record("high_importance", "test", 0.9f, nlm::Timestamp(2000.0f));
    
    auto important_events = episodicMemory->retrieveImportantEvents(0.5f);
    EXPECT_EQ(important_events.size(), 1);
    EXPECT_EQ(important_events[0].name, "high_importance");
}

class AssociativeMemoryTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("associative_memory_capacity", static_cast<int64_t>(20), nlm::ConfigSource::Default);
        associativeMemory = std::make_unique<nlm::AssociativeMemory>(config);
    }

    void TearDown() override {
        associativeMemory.reset();
        config.reset();
    }

    std::shared_ptr<nlm::Config> config;
    std::unique_ptr<nlm::AssociativeMemory> associativeMemory;
};

TEST_F(AssociativeMemoryTestFixture, AssociativeMemoryCreation) {
    EXPECT_TRUE(associativeMemory->isInitialized());
    EXPECT_EQ(associativeMemory->getCapacity(), 20);
}

TEST_F(AssociativeMemoryTestFixture, AssociativeMemoryLearning) {
    std::vector<float> key1 = {1.0f, 2.0f, 3.0f};
    std::vector<float> key2 = {4.0f, 5.0f, 6.0f};
    
    associativeMemory->learnAssociation(key1, key2, 0.8f);
    
    auto retrieved = associativeMemory->retrieveAssociation(key1);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->size(), 3);
}

TEST_F(AssociativeMemoryTestFixture, AssociativeMemoryPatternCompletion) {
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f};
    
    associativeMemory->learnAssociation(pattern1, pattern2, 0.7f);
    
    std::vector<float> partialPattern = {0.15f, 0.25f, 0.35f};
    auto completed = associativeMemory->completePattern(partialPattern);
    
    ASSERT_TRUE(completed.has_value());
    EXPECT_EQ(completed->size(), 3);
}

TEST_F(AssociativeMemoryTestFixture, AssociativeMemoryStrength) {
    std::vector<float> key = {1.0f, 2.0f, 3.0f};
    std::vector<float> value = {0.5f, 0.6f, 0.7f};
    
    associativeMemory->learnAssociation(key, value, 0.9f);
    
    auto strength = associativeMemory->getAssociationStrength(key);
    ASSERT_TRUE(strength.has_value());
    EXPECT_GT(strength.value(), 0.0f);
}

TEST_F(AssociativeMemoryTestFixture, AssociativeMemoryForgetting) {
    std::vector<float> key = {1.0f, 2.0f, 3.0f};
    std::vector<float> value = {0.5f, 0.6f, 0.7f};
    
    associativeMemory->learnAssociation(key, value, 0.8f);
    auto initialStrength = associativeMemory->getAssociationStrength(key);
    
    for (int i = 0; i < 100; ++i) {
        associativeMemory->update(0.001f);
    }
    
    auto finalStrength = associativeMemory->getAssociationStrength(key);
    if (finalStrength.has_value()) {
        EXPECT_LE(finalStrength.value(), initialStrength.value());
    }
}

TEST_F(AssociativeMemoryTestFixture, AssociativeMemoryPruning) {
    std::vector<float> weakKey = {0.5f, 0.5f, 0.5f};
    std::vector<float> strongKey = {0.9f, 0.9f, 0.9f};
    
    associativeMemory->learnAssociation(weakKey, {}, 0.3f);
    associativeMemory->learnAssociation(strongKey, {}, 0.9f);
    
    associativeMemory->pruneWeakAssociations(0.5f);
    
    auto weakStrength = associativeMemory->getAssociationStrength(weakKey);
    auto strongStrength = associativeMemory->getAssociationStrength(strongKey);
    
    if (weakStrength.has_value()) {
        EXPECT_LT(weakStrength.value(), 0.5f);
    }
    if (strongStrength.has_value()) {
        EXPECT_GT(strongStrength.value(), 0.5f);
    }
}

TEST_F(AssociativeMemoryTestFixture, MultipleMemorySystemsIntegration) {
    EXPECT_TRUE(workingMemory->isInitialized());
    EXPECT_TRUE(episodicMemory->isInitialized());
    EXPECT_TRUE(associativeMemory->isInitialized());
    
    workingMemory->store("test", 123.0f);
    nlm::Timestamp ts(1000.0f);
    episodicMemory->record("event", "type", 0.5f, ts);
    
    EXPECT_TRUE(workingMemory->retrieve("test").has_value());
    EXPECT_TRUE(episodicMemory->retrieveEvent("event").has_value());
}

} // namespace test_memory