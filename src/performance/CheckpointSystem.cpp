#include "../performance/CheckpointSystem.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <map>
#include <chrono>

namespace nlm {

// Static CRC tables for checksums
namespace {
    uint64_t crc64_table[256];
    uint32_t crc32_table[256];
    bool tables_initialized = false;
    
    void init_crc_tables() {
        if (tables_initialized) return;
        
        // Initialize CRC32 table (IEEE 802.3)
        for (int i = 0; i < 256; ++i) {
            uint32_t crc = i;
            for (int j = 0; j < 8; ++j) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ 0xEDB88320;
                } else {
                    crc = crc >> 1;
                }
            }
            crc32_table[i] = crc;
        }
        
        // Initialize CRC64 table (simplified)
        for (int i = 0; i < 256; ++i) {
            uint64_t crc = static_cast<uint64_t>(i);
            for (int j = 0; j < 8; ++j) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ 0xC96C579615C6C693ULL;
                } else {
                    crc = crc >> 1;
                }
            }
            crc64_table[i] = crc;
        }
        
        tables_initialized = true;
    }
    
    uint32_t crc32(const void* data, size_t size) {
        init_crc_tables();
        uint32_t crc = 0xFFFFFFFF;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = bytes[i];
            crc = (crc >> 8) ^ crc32_table[(crc ^ byte) & 0xFF];
        }
        
        return ~crc;
    }
    
    uint64_t crc64(const void* data, size_t size) {
        init_crc_tables();
        uint64_t crc = 0xFFFFFFFFFFFFFFFF;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = bytes[i];
            crc = crc64_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
        }
        
        return crc ^ 0xFFFFFFFFFFFFFFFF;
    }
}