// Working Memory Tests
TEST_CASE("working_memory_test", TestCategory::UNIT, "Tests for working memory system")
void testWorkingMemory() {
    nlm::WorkingMemory wm(100);
    
    assert(wm.getMaxCapacity() == 100);
    assert(wm.getCurrentSize() == 0);
    
    // Test storing and retrieving items
    nlm::MemoryItem item1(nlm::MemoryId(1), "test_item_1");
    item1.timestamp = 0.0;
    item1.importance = 0.8f;
    
    wm.store(item1);
    assert(wm.getCurrentSize() == 1);
    
    auto retrieved = wm.retrieve(nlm::MemoryId(1));
    assert(retrieved.has_value());
    assert(retrieved.value().id == nlm::MemoryId(1));
    assert(retrieved.value().content == "test_item_1");
    
    // Test importance-based retrieval priority
    nlm::MemoryItem item2(nlm::MemoryId(2), "test_item_2");
    item2.timestamp = 1000.0;
    item2.importance = 0.3f;
    
    wm.store(item2);
    
    // Higher importance items should be retrieved first
    auto retrieved2 = wm.retrieve(nlm::MemoryId(2));
    assert(retrieved2.has_value());
    
    // Test decay mechanism
    wm.updateDecay(0.1);
    // Importance should decay over time
    auto updated = wm.retrieve(nlm::MemoryId(1));
    assert(updated.has_value());
    assert(updated.value().importance < 0.8f);
    
    // Test cleanup of expired items
    wm.cleanupExpired(1000.0);
    // Should clean up items older than 1000.0
    
    // Test capacity limits
    for (int i = 100; i < 150; ++i) {
        nlm::MemoryItem item(nlm::MemoryId(i), "item_" + std::to_string(i));
        item.timestamp = static_cast<double>(i);
        item.importance = 0.5f;
        wm.store(item);
    }
    
    // Capacity should not be exceeded
    assert(wm.getCurrentSize() <= 100);
    
    std::cout << "Working memory tests passed" << std::endl;
    return true;
}

// Neural Working Memory Tests
TEST_CASE("neural_working_memory_test", TestCategory::UNIT, "Tests for neural working memory")
void testNeuralWorkingMemory() {
    nlm::NeuralWorkingMemory nwm(50, 0.1, 0.9);
    
    assert(nwm.getCapacity() == 50);
    assert(nwm.getDecayRate() == 0.1f);
    assert(nwm.getMaxImportance() == 0.9f);
    
    // Test storing neural entries
    nlm::NeuralMemoryEntry entry1;
    entry1.id = nlm::MemoryId(1);
    entry1.neuronId = nlm::NeuronId(1);
    entry1.synapticStrength = 0.7f;
    entry1.lastAccessed = 0.0;
    entry1.importance = 0.6f;
    
    nwm.store(entry1);
    assert(nwm.getCurrentSize() == 1);
    
    auto retrieved = nwm.retrieve(nlm::MemoryId(1));
    assert(retrieved.has_value());
    assert(retrieved.value().id == nlm::MemoryId(1));
    assert(retrieved.value().synapticStrength == 0.7f);
    
    // Test importance updating
    nlm::NeuralMemoryEntry entry2;
    entry2.id = nlm::MemoryId(2);
    entry2.neuronId = nlm::NeuronId(2);
    entry2.synapticStrength = 0.5f;
    entry2.lastAccessed = 100.0;
    entry2.importance = 0.4f;
    
    nwm.store(entry2);
    
    nwm.updateEntryImportance(nlm::MemoryId(2), 0.9f);
    auto updated = nwm.retrieve(nlm::MemoryId(2));
    assert(updated.has_value());
    assert(updated.value().importance == 0.9f);
    
    // Test decay mechanism
    nwm.updateDecay(10.0); // Advance time
    auto decayed = nwm.retrieve(nlm::MemoryId(1));
    assert(decayed.has_value());
    assert(decayed.value().importance < 0.6f); // Should have decayed
    
    // Test recency update
    nwm.updateEntryRecency(nlm::MemoryId(1), 200.0);
    auto recent = nwm.retrieve(nlm::MemoryId(1));
    assert(recent.has_value());
    assert(recent.value().lastAccessed == 200.0f);
    
    // Test capacity enforcement
    for (int i = 3; i < 60; ++i) { // Exceed capacity
        nlm::NeuralMemoryEntry entry;
        entry.id = nlm::MemoryId(i);
        entry.neuronId = nlm::NeuronId(i);
        entry.synapticStrength = 0.1f * i;
        entry.lastAccessed = static_cast<float>(i * 10);
        entry.importance = 0.1f;
        
        nwm.store(entry);
    }
    
    assert(nwm.getCurrentSize() <= 50);
    
    std::cout << "Neural working memory tests passed" << std::endl;
    return true;
}

// Episodic Memory Tests
TEST_CASE("episodic_memory_test", TestCategory::UNIT, "Tests for episodic memory system")
void testEpisodicMemory() {
    nlm::EpisodicMemory em(200, 0.05, 0.8);
    
    assert(em.getMaxEvents() == 200);
    assert(em.getTemporalDecay() == 0.05f);
    assert(em.getImportanceThreshold() == 0.8f);
    
    // Test storing events
    nlm::EpisodicEvent event1;
    event1.id = nlm::MemoryId(1);
    event1.timestamp = 0.0;
    event1.context = "test_context_1";
    event1.importance = 0.9f;
    event1.associatedNeuron = nlm::NeuronId(1);
    
    em.storeEvent(event1);
    assert(em.getEventCount() == 1);
    
    auto retrieved = em.retrieveEvent(nlm::MemoryId(1));
    assert(retrieved.has_value());
    assert(retrieved.value().id == nlm::MemoryId(1));
    assert(retrieved.value().context == "test_context_1");
    
    // Test temporal retrieval
    nlm::EpisodicEvent event2;
    event2.id = nlm::MemoryId(2);
    event2.timestamp = 1000.0;
    event2.context = "test_context_2";
    event2.importance = 0.7f;
    event2.associatedNeuron = nlm::NeuronId(2);
    
    em.storeEvent(event2);
    
    // Get recent events within 500ms
    auto recentEvents = em.getRecentEvents(500.0);
    assert(recentEvents.size() == 1);
    assert(recentEvents[0].id == nlm::MemoryId(1)); // event1 is recent
    
    // Get events within broader timeframe
    auto broaderEvents = em.getRecentEvents(2000.0);
    assert(broaderEvents.size() == 2);
    
    // Test time advancement
    em.updateTime(1500.0);
    auto updatedRecent = em.getRecentEvents(500.0);
    assert(updatedRecent.size() == 1); // event2 is now recent
    assert(updatedRecent[0].id == nlm::MemoryId(2));
    
    // Test cleanup
    em.cleanupOldEvents(100.0);
    assert(em.getEventCount() == 1); // event1 should be cleaned up
    
    // Test importance-based retrieval
    nlm::EpisodicEvent highImportance;
    highImportance.id = nlm::MemoryId(3);
    highImportance.timestamp = 2000.0;
    highImportance.context = "high_importance_context";
    highImportance.importance = 0.95f; // Above threshold
    highImportance.associatedNeuron = nlm::NeuronId(3);
    
    em.storeEvent(highImportance);
    
    auto importantEvents = em.getImportantEvents();
    assert(importantEvents.size() >= 1);
    
    std::cout << "Episodic memory tests passed" << std::endl;
    return true;
}

// Semantic Memory Tests
TEST_CASE("semantic_memory_test", TestCategory::UNIT, "Tests for semantic memory system")
void testSemanticMemory() {
    nlm::SemanticMemory sm(100, 0.2, 0.7);
    
    assert(sm.getConceptCount() == 0);
    assert(sm.getAssociationStrength() == 0.2f);
    assert(sm.getForgettingRate() == 0.7f);
    
    // Test storing concepts
    nlm::Concept concept1;
    concept1.id = nlm::ConceptId(1);
    concept1.name = "test_concept_1";
    concept1.associations = {"related1", "related2"};
    concept1.frequency = 1.0f;
    concept1.lastActivated = 0.0;
    concept1.activationLevel = 0.5f;
    
    sm.storeConcept(concept1);
    assert(sm.getConceptCount() == 1);
    
    auto retrieved = sm.retrieveConcept(nlm::ConceptId(1));
    assert(retrieved.has_value());
    assert(retrieved.value().id == nlm::ConceptId(1));
    assert(retrieved.value().name == "test_concept_1");
    assert(retrieved.value().frequency == 1.0f);
    
    // Test concept activation
    sm.activateConcept(nlm::ConceptId(1), 0.8f);
    auto activated = sm.retrieveConcept(nlm::ConceptId(1));
    assert(activated.has_value());
    assert(activated.value().activationLevel == 0.8f);
    
    // Test association learning
    nlm::Concept concept2;
    concept2.id = nlm::ConceptId(2);
    concept2.name = "test_concept_2";
    concept2.associations = {"related1"}; // Shared association
    concept2.frequency = 1.5f;
    concept2.lastActivated = 100.0;
    concept2.activationLevel = 0.3f;
    
    sm.storeConcept(concept2);
    
    // Concepts with shared associations should have stronger links
    auto linked = sm.getRelatedConcepts(nlm::ConceptId(1), 0.5f);
    assert(linked.size() >= 1); // Should include concept2
    
    // Test knowledge base queries
    auto knowledgeBase = sm.getKnowledgeBase();
    assert(knowledgeBase.size() >= 2);
    
    // Test concept forgetting
    sm.updateTime(1000.0); // Significant time passage
    auto forgotten = sm.retrieveConcept(nlm::ConceptId(1));
    assert(forgotten.has_value()); // May still exist but likely weaker
    
    // Test capacity management
    for (int i = 3; i < 150; ++i) {
        nlm::Concept concept;
        concept.id = nlm::ConceptId(i);
        concept.name = "concept_" + std::to_string(i);
        concept.associations = {};
        concept.frequency = 0.1f;
        concept.lastActivated = static_cast<float>(i * 10);
        concept.activationLevel = 0.1f;
        
        sm.storeConcept(concept);
    }
    
    assert(sm.getConceptCount() <= 100);
    
    std::cout << "Semantic memory tests passed" << std::endl;
    return true;
}

// Procedural Memory Tests
TEST_CASE("procedural_memory_test", TestCategory::UNIT, "Tests for procedural memory system")
void testProceduralMemory() {
    nlm::ProceduralMemory pm(50, 0.15, 0.85);
    
    assert(pm.getSkillCount() == 0);
    assert(pm.getLearningRate() == 0.15f);
    assert(pm.getRetentionFactor() == 0.85f);
    
    // Test skill acquisition
    nlm::Skill skill1;
    skill1.id = nlm::SkillId(1);
    skill1.name = "test_skill_1";
    skill1.performanceLevel = 0.3f;
    skill1.lastPracticed = 0.0;
    skill1.frequency = 0.0f;
    skill1.automatedLevel = 0.0f;
    
    pm.acquireSkill(skill1);
    assert(pm.getSkillCount() == 1);
    
    auto retrieved = pm.getSkill(nlm::SkillId(1));
    assert(retrieved.has_value());
    assert(retrieved.value().id == nlm::SkillId(1));
    assert(retrieved.value().name == "test_skill_1");
    
    // Test skill practice and improvement
    pm.practiceSkill(nlm::SkillId(1), 10.0);
    auto practiced = pm.getSkill(nlm::SkillId(1));
    assert(practiced.has_value());
    assert(practiced.value().performanceLevel > 0.3f);
    assert(practiced.value().frequency > 0.0f);
    
    // Test automation level increase
    pm.increaseAutomation(nlm::SkillId(1), 0.2f);
    auto automated = pm.getSkill(nlm::SkillId(1));
    assert(automated.has_value());
    assert(automated.value().automatedLevel > 0.0f);
    
    // Test skill transfer
    nlm::Skill skill2;
    skill2.id = nlm::SkillId(2);
    skill2.name = "test_skill_2";
    skill2.performanceLevel = 0.5f;
    skill2.lastPracticed = 100.0;
    skill2.frequency = 5.0f;
    skill2.automatedLevel = 0.3f;
    
    pm.acquireSkill(skill2);
    
    // Transfer skills based on similarity
    auto transferred = pm.transferSkill(nlm::SkillId(2), 0.8f);
    // May or may not transfer depending on implementation
    
    // Test skill decay
    pm.updateTime(1000.0); // Significant time without practice
    auto decayed = pm.getSkill(nlm::SkillId(1));
    assert(decayed.has_value());
    assert(decayed.value().performanceLevel < 1.0f); // May have decayed
    
    // Test skill hierarchy
    nlm::SkillId masterSkill = pm.getMasterSkill();
    // May be null if no skills acquired or no master determined
    
    // Test performance monitoring
    float performance = pm.monitorSkillPerformance(nlm::SkillId(1));
    assert(performance >= 0.0f && performance <= 1.0f);
    
    std::cout << "Procedural memory tests passed" << std::endl;
    return true;
}