#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"

namespace py = pybind11;
namespace nlm {

// Unit tests for the Python bindings
class BindingsTests {
public:
    static void runAllTests() {
        testIdTypes();
        testConfig();
        testSimpleWorld();
        testAction();
        testBrain();
        testAgentBrain();
        testWrappers();
        std::cout << "All tests passed!" << std::endl;
    }
    
private:
    static void testIdTypes() {
        // Test NeuronId
        NeuronId id1(42);
        NeuronId id2(42);
        NeuronId id3(99);
        
        assert(id1 == id2);
        assert(id1 != id3);
        assert(id1.index() == 42);
        assert(hash<NeuronId>{}(id1) == std::hash<uint64_t>{}(42));
        
        // Test other ID types
        SynapseId synId(100);
        RegionId regId(200);
        PopulationId popId(300);
        
        assert(synId.index() == 100);
        assert(regId.index() == 200);
        assert(popId.index() == 300);
        
        std::cout << "ID types test passed!" << std::endl;
    }
    
    static void testConfig() {
        auto config = std::make_shared<Config>();
        
        // Test basic operations
        config->set("test_int", 42);
        config->set("test_string", "hello");
        config->set("test_double", 3.14);
        config->set("test_bool", true);
        
        assert(config->has("test_int"));
        assert(!config->has("nonexistent"));
        
        auto value = config->get<int>("test_int");
        assert(value && *value == 42);
        
        value = config->get<std::string>("test_string");
        assert(value && *value == "hello");
        
        value = config->get<double>("test_double");
        assert(value && std::abs(*value - 3.14) < 0.01);
        
        value = config->get<bool>("test_bool");
        assert(value && *value == true);
        
        // Test loadFromArgs
        std::vector<std::string> args = {"program", "--test", "123"};
        bool loaded = config->loadFromArgs(args.size(), args.data());
        
        std::cout << "Config test passed!" << std::endl;
    }
    
    static void testSimpleWorld() {
        auto world = std::make_shared<SimpleWorld>();
        
        // Test configuration
        world->configure(10.0, 10.0, 84, 84);
        assert(world->getWidth() == 10.0);
        assert(world->getHeight() == 10.0);
        assert(world->getVisionWidth() == 84);
        assert(world->getVisionHeight() == 84);
        
        // Test reset
        world->reset();
        
        // Test agent start
        world->setAgentStart(5.0, 5.0);
        
        // Test update
        world->update(0.1);
        assert(world->getSimulationTime() > 0.0);
        
        // Test object operations
        WorldObject obj(1.0, 1.0, WorldObjectType::Resource, 10.0);
        world->addObject(obj);
        
        WorldObject* found = world->getObjectAt(1.0, 1.0);
        assert(found != nullptr);
        assert(found->type == WorldObjectType::Resource);
        assert(found->value == 10.0);
        
        world->removeObject(1.0, 1.0);
        found = world->getObjectAt(1.0, 1.0);
        assert(found == nullptr);
        
        // Test valid position
        assert(world->isValidPosition(5.0, 5.0)); // agent position
        assert(!world->isValidPosition(1.0, 1.0)); // where object was removed
        
        std::cout << "SimpleWorld test passed!" << std::endl;
    }
    
    static void testAction() {
        Action action(ActionType::MoveForward, {0.5, 0.5});
        
        assert(action.getType() == ActionType::MoveForward);
        assert(action.getParameters().size() == 2);
        assert(action.getParameters()[0] == 0.5);
        assert(action.getParameters()[1] == 0.5);
        
        action.setType(ActionType::TurnLeft);
        assert(action.getType() == ActionType::TurnLeft);
        
        std::string name = action.getName();
        assert(!name.empty());
        
        auto cloned = action.clone();
        assert(cloned->getType() == ActionType::TurnLeft);
        assert(cloned->getParameters()[0] == 0.5);
        
        std::cout << "Action test passed!" << std::endl;
    }
    
    static void testBrain() {
        auto config = std::make_shared<Config>();
        config->set("neurons_per_region", 10);
        
        auto brain = std::make_shared<Brain>(config);
        
        // Test initialization
        bool initialized = brain->initialize();
        assert(initialized);
        
        // Test region operations
        RegionId regionId = brain->addRegion("test_region");
        assert(regionId.index() != 0);
        
        NeuralRegion* region = brain->getRegion(regionId);
        assert(region != nullptr);
        
        assert(brain->getRegionCount() == 1);
        
        auto regionIds = brain->getRegionIds();
        assert(regionIds.size() == 1);
        assert(regionIds[0] == regionId);
        
        // Test step
        brain->step(1);
        brain->step(1, 0.0);
        
        // Test statistics
        assert(brain->getTotalNeuronCount() > 0);
        assert(brain->getTotalSynapseCount() > 0);
        
        // Test development
        brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        assert(brain->getDevelopmentalStage() == DevelopmentalStage::Adult);
        
        // Test save/load
        std::string testFile = "test_brain.bin";
        bool saved = brain->save(testFile);
        assert(saved);
        
        // Reset and load
        brain->reset();
        bool loaded = brain->load(testFile);
        assert(loaded);
        
        std::cout << "Brain test passed!" << std::endl;
    }
    
    static void testAgentBrain() {
        auto config = std::make_shared<Config>();
        config->set("neurons_per_region", 10);
        
        auto brain = std::make_shared<Brain>(config);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10.0, 10.0, 84, 84);
        world->reset();
        
        // Test initialization
        agentBrain->initialize(*world);
        
        // Test sensory input size
        assert(agentBrain->getSensoryInputSize() > 0);
        assert(agentBrain->getMotorOutputSize() > 0);
        
        // Test neural activity
        auto percept = world->getSensoryPercept();
        agentBrain->processSensoryInput(percept);
        
        MotorCommand cmd = agentBrain->decodeMotorCommand();
        assert(static_cast<int>(cmd) >= 0);
        
        // Test neuromodulation
        agentBrain->applyRewardModulation(1.0, 0.5);
        
        // Test development
        agentBrain->updateDevelopment(0.1);
        
        // Test state
        assert(agentBrain->getDevelopmentalStage() != DevelopmentalStage::Initial);
        assert(agentBrain->getNeuromodulationLevel() >= 0.0f);
        
        // Test reset
        agentBrain->reset();
        
        std::cout << "AgentBrain test passed!" << std::endl;
    }
    
    static void testWrappers() {
        // Test Config wrapper
        auto config = std::make_shared<Config>();
        ConfigWrapper configWrapper(config);
        
        configWrapper.set("test", 123);
        assert(configWrapper.getInt("test") == 123);
        
        // Test world wrapper
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10.0, 10.0, 84, 84);
        SimpleWorldWrapper worldWrapper(world);
        
        worldWrapper.setAgentStart(3.0, 3.0);
        assert(world->getAgentBody().x == 3.0);
        assert(world->getAgentBody().y == 3.0);
        
        // Test context managers
        {
            ConfigWrapper testConfig(config);
            testConfig.set("temp", "value");
            assert(testConfig.has("temp"));
        } // config wrapper should be destroyed here
        
        std::cout << "Wrappers test passed!" << std::endl;
    }
};

PYBIND11_MODULE(bindings_tests, m) {
    m.doc() = R"pbdoc(Unit tests for NLM Python bindings)pbdoc";
    
    m.def("runAllTests", &BindingsTests::runAllTests,
          "Run all unit tests for the Python bindings");
}

} // namespace nlm
