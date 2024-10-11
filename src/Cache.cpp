#include "../include/Cache.h"

namespace mem {

Cache::Cache(const std::string &name,
    unsigned num_sets,
    unsigned num_ways,
    unsigned block_size,
    unsigned replacement_policy,
    unsigned write_policy)
    : name(name),
      num_sets(num_sets),
      num_ways(num_ways),
      block_size(block_size) {
    
    // Derived fields
    assert((num_sets & (num_sets - 1)) == 0);
    assert((num_ways & (num_ways - 1)) == 0);
    assert((block_size & (block_size - 1)) == 0);
    num_blocks = num_sets * num_ways;
    log_block_size = log2(block_size);
    block_mask = block_size - 1;

    // Allocate blocks and sets
    sets = misc::new_unique_array<Set>(num_sets);

    // Initialize sets and blocks
    for (unsigned set_id = 0; set_id < num_sets; set_id++) {
        for (unsigned way_id = 0; way_id < num_ways; way_id++) {
            Block block;
            block.way_id = way_id;
            block.state = BlockInvalid;
            sets[set_id].lru_list.push_back(block);
        }
    }
}

void Cache::DecodeAddress(unsigned long address, unsigned &set_id, unsigned &tag, unsigned &block_offset) const {
    set_id = (address >> log_block_size) % num_sets;
    tag = address >> (log_block_size + log2(num_sets));
    block_offset = address & block_mask;
}

bool Cache::FindBlock(unsigned &set_id, unsigned &tag, unsigned &way_id) const {
    std::list<Block>::iterator lru_list;
    lru_list = sets[set_id].lru_list.begin();

    while (lru_list != sets[set_id].lru_list.end()) {
        if (lru_list->tag == tag) {
            lru_list->lru_bits = 0;
            way_id = lru_list->way_id;
            return true;
        }
        lru_list++;
    }
    return false;
}

void Cache::setBlock(unsigned set_id, unsigned way_id, unsigned tag, BlockState state) {
    std::list<Block>::iterator lru_list;
    lru_list = sets[set_id].lru_list.begin();

    while (lru_list != sets[set_id].lru_list.end()) {
        if (lru_list->way_id == way_id) {
            lru_list->tag = tag;
            lru_list->lru_bits = 0;
            lru_list->state = state;
            return;
        }
        lru_list++;
    }
}

bool Cache::SetLRU(unsigned set_id, unsigned way_id) {
    std::list<Block>::iterator lru_list;
    lru_list = sets[set_id].lru_list.begin();

    while (lru_list != sets[set_id].lru_list.end()) {
        if (lru_list->way_id == way_id) {
            lru_list->lru_bits++;
        } else {
            lru_list->lru_bits = 0;
        }
        lru_list++;
    }
    return false;
}

unsigned Cache::ReplaceBlock(unsigned set_id) {
    unsigned lru_bits = 0;
    unsigned lru_way_id = 0;

    std::list<Block>::iterator lru_list;
    lru_list = sets[set_id].lru_list.begin();

    while (lru_list != sets[set_id].lru_list.end()) {
        if (lru_bits < lru_list->lru_bits) {
            lru_bits = lru_list->lru_bits;
            lru_way_id = lru_list->way_id;
        }
        lru_list++;
    }
    return lru_way_id;
}

void Cache::Access(unsigned long address, int access_type) {
    unsigned block_offset = 0;
    unsigned set_id = 0;
    unsigned tag = 0;

    DecodeAddress(address, set_id, tag, block_offset);
    unsigned way_id = 0;

    if (!FindBlock(set_id, tag, way_id)) {
        bool BlockInvalidFound = false;
        std::list<Block>::iterator lru_list;
        lru_list = sets[set_id].lru_list.begin();

        while (lru_list != sets[set_id].lru_list.end()) {
            if (lru_list->state == BlockInvalid) {
                BlockInvalidFound = true;
                way_id = lru_list->way_id;
                break;
            }
            lru_list++;
        }

        if (!BlockInvalidFound) {
            way_id = ReplaceBlock(set_id);
            num_evicted_blocks++;
        }
        setBlock(set_id, way_id, tag, BlockValid);
        num_miss++;
    } else {
        num_hit++;
    }
    SetLRU(set_id, way_id);
}

} // namespace mem