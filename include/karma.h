#pragma once

#include <allocator.h>

#include <map>
#include <unordered_map>
#include <vector>

#define PUBLIC_ID 0
#define DUMMY_ID std::numeric_limits<uint32_t>::max()

class KarmaAllocator : public Allocator {
   public:
    KarmaAllocator(uint32_t num_blocks, float alpha, uint32_t init_credits);

    virtual ~KarmaAllocator() = default;

    void add_user(uint32_t id);

    void remove_user(uint32_t id);

    void allocate();

    void set_demand(uint32_t id, uint32_t demand);

    uint32_t get_num_tenants();

    void output_tenant(std::ostream& s, uint32_t id);

   private:
    struct Tenant {
        uint32_t demand_ = 0, allocation_ = 0, credits_;
        int32_t rate_ = 0;

        Tenant() : credits_(0) {
        }

        Tenant(uint32_t init_credits) : credits_(init_credits) {
        }
    };

    struct Candidate {
        int64_t credits_;
        uint32_t id_, blocks_;

        Candidate(uint32_t id, int64_t credits, uint32_t blocks)
            : id_(id), credits_(credits), blocks_(blocks) {
        }
    };

    uint64_t total_credits_ = 0;
    uint32_t public_blocks_, fair_share_, init_credits_;
    std::unordered_map<uint32_t, Tenant> tenants_;

    uint32_t get_block_surplus(uint32_t id);

    void borrow_from_poor(uint32_t demand, std::vector<uint32_t>& donors, std::vector<uint32_t>& borrowers);

    void donate_to_rich(uint32_t supply, std::vector<uint32_t>& donors, std::vector<uint32_t>& borrowers);
};