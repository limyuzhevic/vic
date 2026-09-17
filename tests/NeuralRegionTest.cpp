#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include "brain/NeuralRegion.hpp"
#include "core/Random/Random.hpp"

namespace nlm {

TEST_CASE("NeuralRegion Basic Functionality", "[NeuralRegion][Unit]" ) {
    SECTION("Constructor with ID only") {
        NeuralRegion region(RegionId(1));
        REQUIRE(region.getId() == RegionId(1));
        REQUIRE(region.getName().empty());
    }
    
    SECTION("Constructor with ID and name") {
        NeuralRegion region(RegionId(2), "TestRegion");
        REQUIRE(region.getId() == RegionId(2));
        REQUIRE(region.getName() == "TestRegion");
    }
    
    SECTION("Set name") {
        NeuralRegion region(RegionId(3));
        region.setName("NewRegion");
        REQUIRE(region.getName() == "NewRegion");
    }
    
    SECTION("Add population") {
        NeuralRegion region(RegionId(4));
        PopulationId popId = region.addPopulation(100, NeuronType::Internal);
        REQUIRE(popId.index() == 1);
        REQUIRE(region.getPopulationCount() == 1);
        
        NeuralPopulation* pop = region.getPopulation(popId);
        REQUIRE(pop != nullptr);
        REQUIRE(pop->getSize() == 100);
        REQUIRE(pop->getNeuronType() == NeuronType::Internal);
    }
    
    SECTION("Add synapse") {
        NeuralRegion region(RegionId(5));
        PopulationId popId = region.addPopulation(10, NeuronType::Excitatory);
        auto neurons = region.getAllNeurons();
        REQUIRE(!neurons.empty());
        
        SynapseId synId = region.addSynapse(neurons[0]->getId(), neurons[1]->getId(), 0.5f, 2);
        REQUIRE(synId.index() == 1);
        REQUIRE(region.getSynapseCount() == 1);
        
        Synapse* syn = region.getSynapse(synId);
        REQUIRE(syn != nullptr);
        REQUIRE(syn->getSourceNeuron() == neurons[0]->getId());
        REQUIRE(syn->getDestinationNeuron() == neurons[1]->getId());
        REQUIRE(syn->getWeight() == 0.5f);
        REQUIRE(syn->getDelay() == 2);
    }
    
    SECTION("Random connectivity initialization") {
        NeuralRegion region(RegionId(6));
        PopulationId popId = region.addPopulation(20, NeuronType::Excitatory);
        RandomGenerator rng(42);
        
        region.initializeRandomConnectivity(rng, 0.5f, 1.0f, 0.1f);
        
        // Check that some synapses were created (not all due to randomness)
        REQUIRE(region.getSynapseCount() > 0);
        REQUIRE(region.getSynapseCount() < 20 * 20);  // No self-connections
    }
    
    SECTION("Move constructor and assignment") {
        NeuralRegion region1(RegionId(7), "Region1");
        PopulationId popId1 = region1.addPopulation(50, NeuronType::Sensory);
        
        NeuralRegion region2(std::move(region1));
        REQUIRE(region2.getId() == RegionId(7));
        REQUIRE(region2.getName() == "Region1");
        REQUIRE(region2.getPopulationCount() == 1);
        REQUIRE(region1.getPopulationCount() == 0);  // Moved from
        
        region1 = std::move(region2);
        REQUIRE(region1.getId() == RegionId(7));
        REQUIRE(region1.getName() == "Region1");
        REQUIRE(region1.getPopulationCount() == 1);
        REQUIRE(region2.getPopulationCount() == 0);  // Moved from
    }
}

TEST_CASE("NeuralRegion Statistics", "[NeuralRegion][Statistics]" ) {
    SECTION("Empty region statistics") {
        NeuralRegion region(RegionId(8));
        REQUIRE(region.getTotalNeuronCount() == 0);
        REQUIRE(region.getActiveNeuronCount() == 0);
        REQUIRE(region.getFiringNeuronCount() == 0);
        REQUIRE(region.getAverageFiringRate() == 0.0f);
        REQUIRE(region.getAverageSynapticWeight() == 0.0f);
        REQUIRE(region.getSynapticDensity() == 0.0f);
    }
    
    SECTION("Region with neurons and synapses") {
        NeuralRegion region(RegionId(9));
        PopulationId popId1 = region.addPopulation(10, NeuronType::Excitatory);
        PopulationId popId2 = region.addPopulation(5, NeuronType::Inhibitory);
        
        auto neurons = region.getAllNeurons();
        REQUIRE(neurons.size() == 15);
        
        // Add some synapses
        for (size_t i = 0; i < neurons.size() - 1; i += 2) {
            region.addSynapse(neurons[i]->getId(), neurons[i+1]->getId(), 0.1f * ((i % 3) + 1));
        }
        
        REQUIRE(region.getTotalNeuronCount() == 15);
        REQUIRE(region.getPopulationCount() == 2);
        
        float avgWeight = region.getAverageSynapticWeight();
        REQUIRE(avgWeight > 0.0f);
        
        float density = region.getSynapticDensity();
        REQUIRE(density > 0.0f);
        REQUIRE(density <= 1.0f);
    }
}

} // namespace nlm

int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}