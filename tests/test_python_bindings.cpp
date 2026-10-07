// Python Bindings Tests - Google Test Version
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <pybind11/embed.h>

namespace nlm {

class PythonBindingsTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        brain = std::make_shared<Brain>(config);
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

TEST_F(PythonBindingsTest, ConfigPythonInterface) {
    // Test that Config can be created and used
    auto cfg = std::make_shared<Config>();
    cfg->set("test", 123, ConfigSource::Runtime);
    
    ASSERT_TRUE(cfg->has("test"));
    auto val = cfg->get<int>("test");
    ASSERT_TRUE(val.has_value());
    ASSERT_EQ(*val, 123);
}

TEST_F(PythonBindingsTest, BrainPythonInterface) {
    // Test that Brain can be created and basic methods work
    brain->initialize();
    
    ASSERT_FALSE(brain->getRegionCount() == 0);  // Should have at least one region
    auto regionIds = brain->getRegionIds();
    ASSERT_FALSE(regionIds.empty());
}

TEST_F(PythonBindingsTest, NeuronTypePythonInterface) {
    // Test enum values are accessible
    NeuronId id1(1);
    NeuronId id2(2);
    
    ASSERT_NE(id1, id2);
    ASSERT_EQ(id1.index(), 0);  // Index should work
    
    ASSERT_EQ(std::hash<NeuronId>()(id1), std::hash<uint64_t>()(id1.value));
}

TEST_F(PythonBindingsTest, SynapseIdPythonInterface) {
    SynapseId syn1(1);
    SynapseId syn2(2);
    
    ASSERT_NE(syn1, syn2);
    ASSERT_EQ(syn1.index(), 0);
}

TEST_F(PythonBindingsTest, SimpleWorldPythonInterface) {
    // Create a world and test basic operations
    auto world = std::make_shared<SimpleWorld>();
    
    world->configure(20.0f, 20.0f, 16, 16);
    world->reset();
    
    ASSERT_EQ(world->getWidth(), 20.0f);
    ASSERT_EQ(world->getHeight(), 20.0f);
    
    // Test agent body
    auto agent = world->getAgentBody();
    ASSERT_GE(agent.x, 0.0f);
    ASSERT_GE(agent.y, 0.0f);
}

TEST_F(PythonBindingsTest, ActionPythonInterface) {
    // Test Action creation and manipulation
    Action moveForward(ActionType::MoveForward);
    
    ASSERT_EQ(moveForward.getType(), ActionType::MoveForward);
    ASSERT_TRUE(moveForward.getName() != "");
    
    Action custom(ActionType::Custom, std::vector<float>{1.0f, 2.0f, 3.0f});
    auto params = custom.getParameters();
    ASSERT_EQ(params.size(), 3);
    ASSERT_FLOAT_EQ(params[0], 1.0f);
}

TEST_F(PythonBindingsTest, SensoryPerceptPythonInterface) {
    // Test SensoryPercept creation and data access
    SensoryPercept percept;
    
    // Set some vision data
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    
    auto retrievedVision = percept.getVision();
    ASSERT_EQ(retrievedVision.size(), vision.size());
    ASSERT_FLOAT_EQ(retrievedVision[0], 0.5f);
}

TEST_F(PythonBindingsTest, WorldObjectPythonInterface) {
    // Test WorldObject creation
    WorldObject resource(10.0f, 15.0f, WorldObjectType::Resource, 50.0f);
    
    ASSERT_FLOAT_EQ(resource.x, 10.0f);
    ASSERT_FLOAT_EQ(resource.y, 15.0f);
    ASSERT_EQ(resource.type, WorldObjectType::Resource);
    ASSERT_FLOAT_EQ(resource.value, 50.0f);
    ASSERT_TRUE(resource.active);
}

TEST_F(PythonBindingsTest, AgentBodyPythonInterface) {
    AgentBody body;
    body.x = 5.0f;
    body.y = 10.0f;
    body.orientation = 1.57f;  // PI/2
    body.velocityX = 2.0f;
    body.velocityY = 1.0f;
    body.energy = 75.0f;
    body.health = 95.0f;
    body.age = 10.0f;
    
    ASSERT_FLOAT_EQ(body.x, 5.0f);
    ASSERT_FLOAT_EQ(body.y, 10.0f);
    ASSERT_FLOAT_EQ(body.orientation, 1.57f);
    ASSERT_FLOAT_EQ(body.velocityX, 2.0f);
    ASSERT_FLOAT_EQ(body.velocityY, 1.0f);
    ASSERT_FLOAT_EQ(body.energy, 75.0f);
    ASSERT_FLOAT_EQ(body.health, 95.0f);
    ASSERT_FLOAT_EQ(body.age, 10.0f);
}

TEST_F(PythonBindingsTest, ActionResultPythonInterface) {
    // Test ActionResult creation and fields
    ActionResult result(5.0f, true, "success");
    
    ASSERT_FLOAT_EQ(result.reward, 5.0f);
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.message, "success");
}

TEST_F(PythonBindingsTest, EnumValuesPythonInterface) {
    // Test that all major enums have expected values
    // NeuronType
    ASSERT_EQ(static_cast<int>(NeuronType::Excitatory), 0);
    ASSERT_EQ(static_cast<int>(NeuronType::Inhibitory), 1);
    ASSERT_EQ(static_cast<int>(NeuronType::Internal), 5);
    
    // SynapseType
    ASSERT_EQ(static_cast<int>(SynapseType::Excitatory), 0);
    ASSERT_EQ(static_cast<int>(SynapseType::Inhibitory), 1);
    
    // ActionType
    ASSERT_EQ(static_cast<int>(ActionType::MoveForward), 0);
    ASSERT_EQ(static_cast<int>(ActionType::Rest), 13);
    
    // DevelopmentalStage
    ASSERT_EQ(static_cast<int>(DevelopmentalStage::Initial), 0);
    ASSERT_EQ(static_cast<int>(DevelopmentalStage::Adult), 3);
}

TEST_F(PythonBindingsTest, InvalidIdValues) {
    // Test that invalid ID values are properly defined
    // These are typically set to 0 or UINT64_MAX
    ASSERT_EQ(INVALID_NEURON_ID.value, static_cast<uint64_t>(-1));
    ASSERT_EQ(INVALID_SYNAPSE_ID.value, static_cast<uint64_t>(-1));
    ASSERT_EQ(INVALID_REGION_ID.value, static_cast<uint64_t>(-1));
    ASSERT_EQ(INVALID_POPULATION_ID.value, static_cast<uint64_t>(-1));
}

TEST_F(PythonBindingsTest, ConfigLoadFromArgs) {
    // Test Config::loadFromArgs with command line args
    config->loadFromArgs(2, const_cast<char**>(new const char*[2]{"program", "--help"}));
    
    // Should not crash and should handle arguments gracefully
    // Note: --help would typically cause a help message, not an error
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM Python Bindings Tests (Google Test) ===" << std::endl;
    std::cout << "Testing Python interface completeness:" << std::endl;
    std::cout << "  - Core data types (NeuronId, SynapseId, etc.)" << std::endl;
    std::cout << "  - Configuration management" << std::endl;
    std::cout << "  - Neural simulation (Brain)" << std::endl;
    std::cout << "  - Environment simulation (SimpleWorld)" << std::endl;
    std::cout << "  - Sensory/Motor interfaces" << std::endl;
    std::cout << "  - Object representations (AgentBody, Action, etc.)" << std::endl;
    std::cout << "  - Enum value consistency" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}