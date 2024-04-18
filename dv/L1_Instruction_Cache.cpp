#include <iostream>
#include <vector>
#include <bitset>

const int BLOCK_SIZE = 4;
const int CACHE_SIZE = 1024; // 1 KB
const int ASSOCIATIVITY = 2;
const int NUM_SETS = CACHE_SIZE / (BLOCK_SIZE * ASSOCIATIVITY);
const int DATA_WIDTH = 32;
const int ADDR_WIDTH = 32;
const int INDEX_WIDTH = 7;
const int TAG_WIDTH = ADDR_WIDTH-INDEX_WIDTH;

struct ram{
    std::uint32_t data[2048]={0};
    bool ready=1;
    std::uint32_t res_data=0;
    void read(uint32_t address){
        res_data=data[address];
    }
    void reset(){
        for(size_t i {0};i<2048;i++){
            data[i]=0;

        }
        res_data=0;
    }
};


class CacheSimulator {
public:
    std::vector<std::vector<uint32_t>> memory;
    std::vector<std::vector<bool>> valid_bits;
    std::vector<std::vector<uint32_t>> tags;
    std::vector<std::vector<int>> lru_counters;
    std::uint32_t output;
    CacheSimulator() {
        memory.resize(NUM_SETS, std::vector<uint32_t>(ASSOCIATIVITY, 0));
        valid_bits.resize(NUM_SETS, std::vector<bool>(ASSOCIATIVITY, false));
        tags.resize(NUM_SETS, std::vector<uint32_t>(ASSOCIATIVITY, 0));
        lru_counters.resize(NUM_SETS, std::vector<int>(ASSOCIATIVITY, 0));
    }

    void reset() {
        for (int i = 0; i < NUM_SETS; ++i) {
            for (int j = 0; j < ASSOCIATIVITY; ++j) {
                memory[i][j] = 0;
                valid_bits[i][j] = false;
                tags[i][j] = 0;
                lru_counters[i][j] = j; 
                //lru_counters[i][j] = 1;
            }
        }
    }

    void accessCache(uint32_t address, bool write,bool read, uint32_t write_data,ram &mem) {
        uint32_t tag = address >> (ADDR_WIDTH-TAG_WIDTH);
        uint32_t index = address  & ((1 << INDEX_WIDTH) - 1);

        bool hit = false;
        int way = 0;

        // hit check
        for (int i = 0; i < ASSOCIATIVITY; ++i) {
            if (valid_bits[index][i] && tags[index][i] == tag) {
                hit = true;
                way = i;
                break;
            }
        }

        if (hit) {
            if (read) {
                output=memory[index][way];
            }
            updateLRU(index, way);
        } else {
            if (read) {
                way = findLRUWay(index);
                memory[index][way] = mem.data[address];
                tags[index][way] = tag;
                valid_bits[index][way] = true;
                updateLRU(index, way);
            }
        }
    }

private:

    uint32_t address;
    uint32_t tag;
    uint32_t index;

    int findLRUWay(int set_index) {
        int lru_way = 0, max_usage = 0;
        for (int i = 0; i < ASSOCIATIVITY; ++i) {
            if (lru_counters[set_index][i] > max_usage) {
                max_usage = lru_counters[set_index][i];
                lru_way = i;
            }
        }
        return lru_way;
    }

    void updateLRU(int set_index, int accessed_way) {
        for (int i = 0; i < ASSOCIATIVITY; ++i) {
            if (i == accessed_way) {
                lru_counters[set_index][i] = 0;
            } else {
                lru_counters[set_index][i]++;
            }
        }
    }
};

