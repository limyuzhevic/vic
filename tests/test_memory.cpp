// Working Memory Tests
namespace test_working_memory {

void testWorkingMemoryCreation() {
    nlm::WorkingMemory wm;
    
    assert(wm.getCapacity() == 100);
    assert(wm.getCurrentSize() == 0);
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryStoreRetrieve() {
    nlm::WorkingMemory wm;
    
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    wm.store(neuron1, 0.5f);
    wm.store(neuron2, 0.8f);
    
    assert(wm.contains(neuron1));
    assert(wm.contains(neuron2));
    assert(!wm.contains(nlm::NeuronId(999)));
    
    assert(wm.retrieve(neuron1) == 0.5f);
    assert(wm.retrieve(neuron2) == 0.8f);
    assert(wm.retrieve(nlm::NeuronId(999)) == 0.0f);
    
    std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::WorkingMemory wm;
    
    // Fill beyond capacity
    for (uint64_t i = 0; i < wm.getCapacity() + 10; ++i) {
        wm.store(nlm::NeuronId(i), static_cast<float>(i % 100) * 0.01f);
    }
    
    assert(wm.getCurrentSize() <= wm.getCapacity());
    
    // Verify newer neurons may have replaced older ones
    for (uint64_t i = 0; i < wm.getCurrentSize(); ++i) {
        assert(wm.contains(nlm::NeuronId(i)));
    }
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryDecay() {
    nlm::WorkingMemory wm;
    
    nlm::NeuronId neuron(1);
    wm.store(neuron, 1.0f);
    
    assert(wm.retrieve(neuron) == 1.0f);
    
    wm.decay(0.1f);  // 10% decay
    float afterDecay = wm.retrieve(neuron);
    
    assert(afterDecay == 0.9f);  // 1.0 * (1 - 0.1)
    
    wm.decay(0.5f);  // 50% decay
    afterDecay = wm.retrieve(neuron);
    
    assert(afterDecay == 0.45f);  // 0.9 * (1 - 0.5)
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testWorkingMemoryClear() {
    nlm::WorkingMemory wm;
    
    wm.store(nlm::NeuronId(1), 0.5f);
    wm.store(nlm::NeuronId(2), 0.8f);
    
    assert(wm.getCurrentSize() == 2);
    assert(wm.contains(nlm::NeuronId(1)));
    assert(wm.contains(nlm::NeuronId(2)));
    
    wm.clear();
    
    assert(wm.getCurrentSize() == 0);
    assert(!wm.contains(nlm::NeuronId(1)));
    assert(!wm.contains(nlm::NeuronId(2)));
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void runAll() {
    testWorkingMemoryCreation();
    testWorkingMemoryStoreRetrieve();
    testWorkingMemoryCapacity();
    testWorkingMemoryDecay();
    testWorkingMemoryClear();
}

} // namespace test_working_memory

// Episodic Memory Tests
namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    nlm::EpisodicMemory em;
    
    assert(em.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStoreRetrieve() {
    nlm::EpisodicMemory em;
    
    nlm::EpisodicMemoryItem episode1;
    episode1.timestamp = 1;
    episode1.neurons.push_back(nlm::NeuronId(1));
    episode1.values.push_back(0.5f);
    episode1.metadata = "Episode 1";
    
    nlm::EpisodicMemoryItem episode2;
    episode2.timestamp = 2;
    episode2.neurons.push_back(nlm::NeuronId(2));
    episode2.values.push_back(0.8f);
    episode2.metadata = "Episode 2";
    
    em.storeEpisode(episode1);
    em.storeEpisode(episode2);
    
    assert(em.getEpisodeCount() == 2);
    
    auto retrieved1 = em.retrieveEpisode(0);
    assert(retrieved1.timestamp == 1);
    assert(retrieved1.neurons.size() == 1);
    assert(retrieved1.values.size() == 1);
    assert(retrieved1.metadata == "Episode 1");
    
    auto retrieved2 = em.retrieveEpisode(1);
    assert(retrieved2.timestamp == 2);
    assert(retrieved2.metadata == "Episode 2");
    
    // Retrieve out of bounds
    auto retrievedOutOfBounds = em.retrieveEpisode(100);
    assert(retrievedOutOfBounds.timestamp == 0);
    assert(retrievedOutOfBounds.neurons.empty());
    
    std::cout << "    testEpisodicMemoryStoreRetrieve passed" << std::endl;
}

void testEpisodicMemoryRecentEpisodes() {
    nlm::EpisodicMemory em;
    
    // Store episodes
    for (size_t i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i + 1;
        em.storeEpisode(episode);
    }
    
    // Get recent episodes
    auto recent5 = em.getRecentEpisodes(5);
    assert(recent5.size() == 5);
    assert(recent5[0].timestamp == 10);
    assert(recent5[4].timestamp == 6);
    
    // Get more than available
    auto recent15 = em.getRecentEpisodes(15);
    assert(recent15.size() == 10);
    assert(recent15[0].timestamp == 10);
    assert(recent15[9].timestamp == 1);
    
    std::cout << "    testEpisodicMemoryRecentEpisodes passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::EpisodicMemory em;
    
    // Store many episodes to trigger consolidation
    for (size_t i = 0; i < 100; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        em.storeEpisode(episode);
    }
    
    // Consolidation should not crash
    em.consolidate(0.5f);
    
    // Should still have episodes (some may have been removed)
    assert(em.getEpisodeCount() > 0 && em.getEpisodeCount() <= 100);
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testEpisodicMemoryClear() {
    nlm::EpisodicMemory em;
    
    // Store some episodes
    for (size_t i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        em.storeEpisode(episode);
    }
    
    assert(em.getEpisodeCount() == 5);
    
    em.clear();
    
    assert(em.getEpisodeCount() == 0);
    
    std::cout << "    testEpisodicMemoryClear passed" << std::endl;
}

void runAll() {
    testEpisodicMemoryCreation();
    testEpisodicMemoryStoreRetrieve();
    testEpisodicMemoryRecentEpisodes();
    testEpisodicMemoryConsolidation();
    testEpisodicMemoryClear();
}

} // namespace test_episodic_memory

// Semantic Memory Tests
namespace test_semantic_memory {

void testSemanticMemoryCreation() {
    nlm::SemanticMemory sm;
    
    assert(sm.getAllFacts().empty());
    assert(!sm.hasFact("key"));
    
    std::cout << "    testSemanticMemoryCreation passed" << std::endl;
}

void testSemanticMemoryStoreRetrieve() {
    nlm::SemanticMemory sm;
    
    sm.storeFact("temperature", "warm");
    sm.storeFact("color", "blue");
    sm.storeFact("size", "large");
    
    assert(sm.hasFact("temperature"));
    assert(sm.hasFact("color"));
    assert(sm.hasFact("size"));
    assert(!sm.hasFact("weight"));
    
    assert(sm.retrieveFact("temperature") == "warm");
    assert(sm.retrieveFact("color") == "blue");
    assert(sm.retrieveFact("size") == "large");
    assert(sm.retrieveFact("weight") == "");
    
    // Update existing fact
    sm.storeFact("temperature", "hot");
    assert(sm.retrieveFact("temperature") == "hot");
    
    std::cout << "    testSemanticMemoryStoreRetrieve passed" << std::endl;
}

void testSemanticMemoryGetAllFacts() {
    nlm::SemanticMemory sm;
    
    sm.storeFact("fact1", "value1");
    sm.storeFact("fact2", "value2");
    sm.storeFact("fact3", "value3");
    
    auto allFacts = sm.getAllFacts();
    assert(allFacts.size() == 3);
    
    // Check all facts are present
    bool found1 = false, found2 = false, found3 = false;
    for (const auto& fact : allFacts) {
        if (fact.first == "fact1" && fact.second == "value1") found1 = true;
        if (fact.first == "fact2" && fact.second == "value2") found2 = true;
        if (fact.first == "fact3" && fact.second == "value3") found3 = true;
    }
    
    assert(found1 && found2 && found3);
    
    std::cout << "    testSemanticMemoryGetAllFacts passed" << std::endl;
}

void testSemanticMemoryClear() {
    nlm::SemanticMemory sm;
    
    sm.storeFact("key1", "value1");
    sm.storeFact("key2", "value2");
    
    assert(sm.getAllFacts().size() == 2);
    
    sm.clear();
    
    assert(sm.getAllFacts().empty());
    assert(sm.getAllFacts().size() == 0);
    
    std::cout << "    testSemanticMemoryClear passed" << std::endl;
}

void runAll() {
    testSemanticMemoryCreation();
    testSemanticMemoryStoreRetrieve();
    testSemanticMemoryGetAllFacts();
    testSemanticMemoryClear();
}

} // namespace test_semantic_memory

// Procedural Memory Tests
namespace test_procedural_memory {

void testProceduralMemoryCreation() {
    nlm::ProceduralMemory pm;
    
    assert(pm.getAllSkills().empty());
    
    std::cout << "    testProceduralMemoryCreation passed" << std::endl;
}

void testProceduralMemoryLearnSkill() {
    nlm::ProceduralMemory pm;
    
    std::vector<nlm::NeuronId> pattern1 = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    std::vector<nlm::NeuronId> pattern2 = {nlm::NeuronId(4), nlm::NeuronId(5), nlm::NeuronId(6)};
    
    pm.learnSkill("skill1", pattern1);
    pm.learnSkill("skill2", pattern2);
    
    auto skills = pm.getAllSkills();
    assert(skills.size() == 2);
    
    // Find skills
    nlm::Skill* skill1 = pm.getSkill("skill1");
    nlm::Skill* skill2 = pm.getSkill("skill2");
    
    assert(skill1 != nullptr);
    assert(skill2 != nullptr);
    assert(skill1->name == "skill1");
    assert(skill2->name == "skill2");
    assert(skill1->neuralPattern.size() == 3);
    assert(skill2->neuralPattern.size() == 3);
    assert(skill1->proficiency == 0.0f);
    assert(skill2->proficiency == 0.0f);
    
    // Update proficiency
    pm.updateProficiency("skill1", 0.3f);
    assert(skill1->proficiency == 0.3f);
    
    pm.updateProficiency("skill2", 0.5f);
    assert(skill2->proficiency == 0.5f);
    
    // Update beyond bounds
    pm.updateProficiency("skill1", 0.8f);
    assert(skill1->proficiency == 1.0f);  // Clamped to max
    
    // Negative update
    pm.updateProficiency("skill1", -0.2f);
    assert(skill1->proficiency == 0.8f);  // Should not go below 0
    
    std::cout << "    testProceduralMemoryLearnSkill passed" << std::endl;
}

void testProceduralMemoryUpdateProficiency() {
    nlm::ProceduralMemory pm;
    
    std::vector<nlm::NeuronId> pattern = {nlm::NeuronId(1)};
    pm.learnSkill("testSkill", pattern);
    
    nlm::Skill* skill = pm.getSkill("testSkill");
    assert(skill != nullptr);
    
    // Test proficiency updates
    pm.updateProficiency("testSkill", 0.2f);
    assert(skill->proficiency == 0.2f);
    
    pm.updateProficiency("testSkill", 0.8f);
    assert(skill->proficiency == 1.0f);  // Clamped
    
    pm.updateProficiency("testSkill", -0.5f);
    assert(skill->proficiency == 0.5f);  // Should not go below 0
    
    std::cout << "    testProceduralMemoryUpdateProficiency passed" << std::endl;
}

void testProceduralMemoryClear() {
    nlm::ProceduralMemory pm;
    
    std::vector<nlm::NeuronId> pattern = {nlm::NeuronId(1)};
    pm.learnSkill("skill1", pattern);
    pm.learnSkill("skill2", pattern);
    
    assert(pm.getAllSkills().size() == 2);
    
    pm.clear();
    
    assert(pm.getAllSkills().empty());
    assert(pm.getSkill("skill1") == nullptr);
    assert(pm.getSkill("skill2") == nullptr);
    
    std::cout << "    testProceduralMemoryClear passed" << std::endl;
}

void runAll() {
    testProceduralMemoryCreation();
    testProceduralMemoryLearnSkill();
    testProceduralMemoryUpdateProficiency();
    testProceduralMemoryClear();
}

} // namespace test_procedural_memory

// Associative Memory Tests
namespace test_associative_memory {

void testAssociativeMemoryCreation() {
    nlm::AssociativeMemory am;
    
    assert(am.getAssociations(nlm::NeuronId(1)).empty());
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.0f);
    
    std::cout << "    testAssociativeMemoryCreation passed" << std::endl;
}

void testAssociativeMemoryCreateAssociations() {
    nlm::AssociativeMemory am;
    
    am.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.5f);
    am.associate(nlm::NeuronId(2), nlm::NeuronId(3), 0.8f);
    am.associate(nlm::NeuronId(3), nlm::NeuronId(4), 0.3f);
    
    // Check bidirectional lookup
    auto associations1 = am.getAssociations(nlm::NeuronId(1));
    assert(associations1.size() == 1);
    assert(associations1[0] == nlm::NeuronId(2));
    
    auto associations2 = am.getAssociations(nlm::NeuronId(2));
    assert(associations2.size() == 2);  // Connected to 1 and 3
    bool found1 = false, found3 = false;
    for (auto assoc : associations2) {
        if (assoc == nlm::NeuronId(1)) found1 = true;
        if (assoc == nlm::NeuronId(3)) found3 = true;
    }
    assert(found1 && found3);
    
    // Check association strengths
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.5f);
    assert(am.getAssociationStrength(nlm::NeuronId(2), nlm::NeuronId(3)) == 0.8f);
    assert(am.getAssociationStrength(nlm::NeuronId(3), nlm::NeuronId(4)) == 0.3f);
    
    // Update existing association
    am.updateAssociation(nlm::NeuronId(1), nlm::NeuronId(2), 0.2f);
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.7f);
    
    // Update beyond bounds
    am.updateAssociation(nlm::NeuronId(1), nlm::NeuronId(2), 1.0f);  // 0.7 + 0.2 = 0.9
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.9f);
    
    am.updateAssociation(nlm::NeuronId(1), nlm::NeuronId(2), 0.2f);  // 0.9 + 0.2 = 1.1 -> clamped to 1.0
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 1.0f);
    
    std::cout << "    testAssociativeMemoryCreateAssociations passed" << std::endl;
}

void testAssociativeMemoryClear() {
    nlm::AssociativeMemory am;
    
    am.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.5f);
    am.associate(nlm::NeuronId(3), nlm::NeuronId(4), 0.8f);
    
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) > 0.0f);
    assert(am.getAssociationStrength(nlm::NeuronId(3), nlm::NeuronId(4)) > 0.0f);
    
    am.clear();
    
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.0f);
    assert(am.getAssociationStrength(nlm::NeuronId(3), nlm::NeuronId(4)) == 0.0f);
    
    std::cout << "    testAssociativeMemoryClear passed" << std::endl;
}

void testAssociativeMemoryDuplicatePrevention() {
    nlm::AssociativeMemory am;
    
    // Creating same association multiple times should update strength
    am.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.5f);
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.5f);
    
    am.associate(nlm::NeuronId(2), nlm::NeuronId(1), 0.3f);  // Bidirectional should also work
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.8f);  // 0.5 + 0.3
    
    am.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.2f);  // Add more
    assert(am.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 1.0f);  // Clamped
    
    std::cout << "    testAssociativeMemoryDuplicatePrevention passed" << std::endl;
}

void runAll() {
    testAssociativeMemoryCreation();
    testAssociativeMemoryCreateAssociations();
    testAssociativeMemoryClear();
    testAssociativeMemoryDuplicatePrevention();
}

} // namespace test_associative_memory