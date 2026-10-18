// NLM Types Tests with Enhanced Test Framework
// 
// This file provides enhanced type system tests that follow the new
// test framework patterns, improving upon the original:
// 1. Integration with enhanced test runner
// 2. Better error handling and reporting
// 3. Consistent test patterns
// 4. Enhanced validation utilities
// 5. Support for parameterized testing

#include "TestFramework.hpp"
#include "core/Types/Types.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace nlm::test {
    namespace types {
        
        void testNeuronIdGeneration() {
            NeuronId id1(1);
            NeuronId id2(2);
            NeuronId id3(1);
            
            assert(id1 != id2);
            assert(id1 == id3);
            assert(id1 < id2);
            assert(!(id1 < id3));
            
            std::cout << "    testNeuronIdGeneration passed" << std::endl;
        }
        
        void testSynapseIdGeneration() {
            SynapseId id1(1);
            SynapseId id2(2);
            SynapseId id3(1);
            
            assert(id1 != id2);
            assert(id1 == id3);
            assert(id1 < id2);
            assert(!(id1 < id3));
            
            std::cout << "    testSynapseIdGeneration passed" << std::endl;
        }
        
        void testRegionIdGeneration() {
            RegionId id1(1);
            RegionId id2(2);
            RegionId id3(1);
            
            assert(id1 != id2);
            assert(id1 == id3);
            assert(id1 < id2);
            assert(!(id1 < id3));
            
            std::cout << "    testRegionIdGeneration passed" << std::endl;
        }
        
        void testNeuronTypeEnumeration() {
            // Test all neuron types
            NeuronType types[] = {
                NeuronType::Internal,
                NeuronType::Excitatory,
                NeuronType::Inhibitory,
                NeuronType::Sensory,
                NeuronType::Motor
            };
            
            for (int i = 0; i < 5; ++i) {
                NeuronType type = types[i];
                assert(static_cast<int>(type) >= 0 && static_cast<int>(type) < 5);
            }
            
            std::cout << "    testNeuronTypeEnumeration passed" << std::endl;
        }
        
        void testSimulationStep() {
            SimulationStep step1(0);
            SimulationStep step2(10);
            SimulationStep step3(0);
            
            assert(step1 == step3);
            assert(step1 < step2);
            assert(step2 > step1);
            
            step1 += SimulationStep(5);
            assert(step1 == SimulationStep(5));
            
            std::cout << "    testSimulationStep passed" << std::endl;
        }
        
        void testConfigSourceEnumeration() {
            ConfigSource sources[] = {
                ConfigSource::Default,
                ConfigSource::File,
                ConfigSource::CommandLine,
                ConfigSource::Programmatic
            };
            
            for (int i = 0; i < 4; ++i) {
                ConfigSource source = sources[i];
                assert(static_cast<int>(source) >= 0 && static_cast<int>(source) < 4);
            }
            
            std::cout << "    testConfigSourceEnumeration passed" << std::endl;
        }
        
        void testVectorIds() {
            std::vector<NeuronId> ids;
            ids.push_back(NeuronId(1));
            ids.push_back(NeuronId(2));
            ids.push_back(NeuronId(3));
            
            assert(ids.size() == 3);
            assert(ids[0] == NeuronId(1));
            assert(ids[2] == NeuronId(3));
            
            // Test sorting
            std::sort(ids.begin(), ids.end());
            assert(ids[0] == NeuronId(1));
            assert(ids[1] == NeuronId(2));
            assert(ids[2] == NeuronId(3));
            
            std::cout << "    testVectorIds passed" << std::endl;
        }
        
        void testUniqueIdProperties() {
            // Test that IDs maintain uniqueness
            std::vector<NeuronId> uniqueIds;
            for (int i = 0; i < 100; ++i) {
                NeuronId id(i);
                // Should not already exist
                assert(std::find(uniqueIds.begin(), uniqueIds.end(), id) == uniqueIds.end());
                uniqueIds.push_back(id);
            }
            
            assert(uniqueIds.size() == 100);
            std::cout << "    testUniqueIdProperties passed" << std::endl;
        }
        
        void runAll(TestRunner& runner) {
            // Register all neuron ID tests
            runner.addTest("test_types", "testNeuronIdGeneration", []() {
                NeuronId id1(1);
                NeuronId id2(2);
                NeuronId id3(1);
                assert(id1 != id2);
                assert(id1 == id3);
                assert(id1 < id2);
                assert(!(id1 < id3));
            });
            
            runner.addTest("test_types", "testSynapseIdGeneration", []() {
                SynapseId id1(1);
                SynapseId id2(2);
                SynapseId id3(1);
                assert(id1 != id2);
                assert(id1 == id3);
                assert(id1 < id2);
                assert(!(id1 < id3));
            });
            
            runner.addTest("test_types", "testRegionIdGeneration", []() {
                RegionId id1(1);
                RegionId id2(2);
                RegionId id3(1);
                assert(id1 != id2);
                assert(id1 == id3);
                assert(id1 < id2);
                assert(!(id1 < id3));
            });
            
            // Register other tests
            runner.addTest("test_types", "testNeuronTypeEnumeration", []() {
                NeuronType types[] = {
                    NeuronType::Internal,
                    NeuronType::Excitatory,
                    NeuronType::Inhibitory,
                    NeuronType::Sensory,
                    NeuronType::Motor
                };
                
                for (int i = 0; i < 5; ++i) {
                    NeuronType type = types[i];
                    assert(static_cast<int>(type) >= 0 && static_cast<int>(type) < 5);
                }
            });
            
            runner.addTest("test_types", "testSimulationStep", []() {
                SimulationStep step1(0);
                SimulationStep step2(10);
                SimulationStep step3(0);
                
                assert(step1 == step3);
                assert(step1 < step2);
                assert(step2 > step1);
                
                step1 += SimulationStep(5);
                assert(step1 == SimulationStep(5));
            });
            
            runner.addTest("test_types", "testConfigSourceEnumeration", []() {
                ConfigSource sources[] = {
                    ConfigSource::Default,
                    ConfigSource::File,
                    ConfigSource::CommandLine,
                    ConfigSource::Programmatic
                };
                
                for (int i = 0; i < 4; ++i) {
                    ConfigSource source = sources[i];
                    assert(static_cast<int>(source) >= 0 && static_cast<int>(source) < 4);
                }
            });
            
            runner.addTest("test_types", "testVectorIds", []() {
                std::vector<NeuronId> ids;
                ids.push_back(NeuronId(1));
                ids.push_back(NeuronId(2));
                ids.push_back(NeuronId(3));
                
                assert(ids.size() == 3);
                assert(ids[0] == NeuronId(1));
                assert(ids[2] == NeuronId(3));
                
                std::sort(ids.begin(), ids.end());
                assert(ids[0] == NeuronId(1));
                assert(ids[1] == NeuronId(2));
                assert(ids[2] == NeuronId(3));
            });
            
            runner.addTest("test_types", "testUniqueIdProperties", []() {
                std::vector<NeuronId> uniqueIds;
                for (int i = 0; i < 100; ++i) {
                    NeuronId id(i);
                    assert(std::find(uniqueIds.begin(), uniqueIds.end(), id) == uniqueIds.end());
                    uniqueIds.push_back(id);
                }
                
                assert(uniqueIds.size() == 100);
            });
            
            // Print completion message
            std::cout << "Types tests completed" << std::endl;
        }
        
    } // namespace types
} // namespace nlm::test