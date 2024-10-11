#ifndef MEMORY_CACHE_H
#define MEMORY_CACHE_H

/*
 * Multi2Sim
 * Copyright (C) 2012 Rafael Ubal (ubal@ece.neu.edu)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <memory>
#include <list>
#include <string> // Include for std::string
#include <cassert>
#include "Misc.h"
#include <cmath> // For log2
//#include <bits/stdc++.h>

namespace mem {

class Cache
{
public:
    // Possible values for a cache block state
    enum BlockState
    {
        BlockInvalid,
        BlockValid,
        BlockModified
    };

    // Cache block class
    class Block
    {
        // Only Cache needs to initialize fields
        friend class Cache;

        // Block tag
        unsigned tag = 0;

        // Transient tag assigned by MOESI protocol
        unsigned transient_tag = 0;

        // Way identifier
        unsigned way_id = 0;

        // Block state
        BlockState state = BlockInvalid;

        // 2-bit counter to track LRU
        unsigned int lru_bits;
    public:
        // Constructor
        Block() {}

        // Get the block tag
        unsigned getTag() const { return tag; }

        // Get the way index of this block
        unsigned getWayId() const { return way_id; }

        // Get the transient tag set in this block
        unsigned getTransientTag() const { return transient_tag; }

        // Get the block state
        BlockState getState() const { return state; }

        // Set new state and tag
        void setStateTag(BlockState state, unsigned tag) {
            this->state = state;
            this->tag = tag;
        }
    };

    // Cache set class
    class Set
    {
        // Only Cache needs to initialize fields
        friend class Cache;

        // List of blocks in LRU order
        std::list<Block> lru_list;
    };

private:
    // Name of the cache, used for debugging purposes
    std::string name;

    // Cache geometry
    unsigned num_sets;
    unsigned num_ways;
    unsigned num_blocks;
    unsigned block_size;
    int write_back; // 1 = write-back, 0 = write-through
    int write_allocate; // 1 = write-allocate, 0 = non-write-allocate

    // Mask used to get the block address
    unsigned block_mask;

    // Log base 2 of the block size
    int log_block_size;

    // Array of sets
    std::unique_ptr<Set[]> sets;

    // Stats
    unsigned long num_hit = 0;
    unsigned long num_miss = 0;
    unsigned long num_evicted_blocks = 0;

public:
    // Constructor
    Cache(const std::string &name,
          unsigned num_sets,
          unsigned num_ways,
          unsigned block_size,
          unsigned replacement_policy,
          unsigned write_policy);

    // Decode a physical address
    void DecodeAddress(unsigned long address, unsigned &set_id, unsigned &tag, unsigned &block_offset) const;

    // Check whether an address is present in the cache
    bool FindBlock(unsigned set_id, unsigned tag, unsigned &way_id) const;

    // Set a new tag and state for a cache block
    void setBlock(unsigned set_id, unsigned way_id, unsigned tag, BlockState state);

    // Access to the cache memory
    void Access(unsigned long address, int access_type);

    // Set LRU
    bool SetLRU(unsigned set_id, unsigned way_id);

    // Return the way index of the block to be replaced
    unsigned ReplaceBlock(unsigned set_id);

    // Get the hit ratio
    double getHitRatio() const {
        return static_cast<double>(num_hit) / (num_hit + num_miss);
    }
};

} // namespace mem

#endif // MEMORY_CACHE_H