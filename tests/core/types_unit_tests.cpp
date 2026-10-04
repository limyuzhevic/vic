// Unit test for Types (NeuronId, SynapseId, RegionId, etc.)
#include "core/Types/Types.hpp"
#include <cassert>
#include <iostream>

namespace test_types_unit {

void testNeuronId() {
    nlm::NeuronId id1(42);
    nlm::NeuronId id2(42);
    nlm::NeuronId id3(43);
    
    assert(id1.value == 42);
    assert(id2.value == 42);
    assert(id3.value == 43);
    
    // Equality operator
    assert(id1 == id2);
    assert(id1 != id3);
    
    // Ordering operators
    assert(id1 < id3);
    assert(id2 <= id3);
    assert(id3 > id2);
    assert(id3 >= id2);
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId() {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(100);
    nlm::SynapseId id3(101);
    
    assert(id1.value == 100);
    assert(id2.value == 100);
    assert(id3.value == 101);
    
    // Equality operator
    assert(id1 == id2);
    assert(id1 != id3);
    
    // Ordering operators
    assert(id1 < id3);
    assert(id3 > id2);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1(5);
    nlm::RegionId id2(5);
    nlm::RegionId id3(6);
    
    assert(id1.value == 5);
    assert(id2.value == 5);
    assert(id3.value == 6);
    
    // Equality operator
    assert(id1 == id2);
    assert(id1 != id3);
    
    // Ordering operators
    assert(id1 < id3);
    assert(id3 > id2);
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1(99);
    nlm::PopulationId id2(99);
    nlm::PopulationId id3(100);
    
    assert(id1.value == 99);
    assert(id2.value == 99);
    assert(id3.value == 100);
    
    // Equality operator
    assert(id1 == id2);
    assert(id1 != id3);
    
    // Ordering operators
    assert(id1 < id3);
    assert(id3 > id2);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testVector3() {
    nlm::Vector3 vec1(1.0f, 2.0f, 3.0f);
    nlm::Vector3 vec2(1.0f, 2.0f, 3.0f);
    nlm::Vector3 vec3(4.0f, 5.0f, 6.0f);
    
    assert(vec1.x == 1.0f);
    assert(vec1.y == 2.0f);
    assert(vec1.z == 3.0f);
    
    // Equality operator
    assert(vec1 == vec2);
    assert(vec1 != vec3);
    
    // Vector operations
    nlm::Vector3 sum = vec1 + vec2;
    assert(sum.x == 2.0f);
    assert(sum.y == 4.0f);
    assert(sum.z == 6.0f);
    
    nlm::Vector3 scaled = vec1 * 2.0f;
    assert(scaled.x == 2.0f);
    assert(scaled.y == 4.0f);
    assert(scaled.z == 6.0f);
    
    std::cout << "    testVector3 passed" << std::endl;
}

void testMatrix4x4() {
    nlm::Matrix4x4 mat1;
    nlm::Matrix4x4 mat2;
    
    // Test initialization
    assert(mat1(0, 0) == 1.0f);
    assert(mat2(0, 0) == 1.0f);
    
    // Test identity matrix
    mat1(1, 1) = 2.0f;
    assert(mat1(1, 1) == 2.0f);
    
    // Test multiplication
    nlm::Vector4 vec(1.0f, 2.0f, 3.0f, 1.0f);
    nlm::Vector4 result = mat1 * vec;
    
    std::cout << "    testMatrix4x4 passed" << std::endl;
}

void testTimestamps() {
    nlm::SimulationStep step = 500;
    nlm::Timestamp time = 0.5;
    
    assert(static_cast<nlm::SimulationStep>(time * 1000.0f) == 500);
    assert(static_cast<nlm::Timestamp>(step * 0.001f) == 0.5);
    
    std::cout << "    testTimestamps passed" << std::endl;
}

void testNeuronType() {
    nlm::NeuronType excType = nlm::NeuronType::Excitatory;
    nlm::NeuronType inhType = nlm::NeuronType::Inhibitory;
    nlm::NeuronType modType = nlm::NeuronType::Modulatory;
    
    assert(static_cast<int>(excType) == 0);
    assert(static_cast<int>(inhType) == 1);
    assert(static_cast<int>(modType) == 2);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    nlm::SynapseType excType = nlm::SynapseType::Excitatory;
    nlm::SynapseType inhType = nlm::SynapseType::Inhibitory;
    nlm::SynapseType modType = nlm::SynapseType::Modulatory;
    
    assert(static_cast<int>(excType) == 0);
    assert(static_cast<int>(inhType) == 1);
    assert(static_cast<int>(modType) == 2);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void runAllTypesUnitTests() {
    testNeuronId();
    testSynapseId();
    testRegionId();
    testPopulationId();
    testVector3();
    testMatrix4x4();
    testTimestamps();
    testNeuronType();
    testSynapseType();
}

} // namespace test_types_unit