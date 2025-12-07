//
// Created by lenovo on 2025/12/4.
//

#ifndef BOOKSTORE_2025_BLOCKLIST_H
#define BOOKSTORE_2025_BLOCKLIST_H

#include "MemoryRiver.h"
#include "Blocknode.h"

constexpr int MAX_Block = 320;

template<typename KeyType, typename ValueType>
class KeyValueDatabase {
private:
    MemoryRiver<BlockNode<KeyType, ValueType>> db_file{};
    int head_addr{};

    void split_block(int addr, BlockNode<KeyType, ValueType>& block) {    //分裂也是有条件的吧
        BlockNode<KeyType, ValueType> new_block;
        const int mid = block.size / 2;
        for (int i = mid; i < block.size; i++) {
            new_block.data[new_block.size++] = block.data[i];
        }
        block.size = mid;

        if (block.size > 0) {
            block.min_key = block.data[0].first;
            block.max_key = block.data[block.size - 1].first;
        } else {
            block.min_key = KeyType();
            block.max_key = KeyType();
        }

        if (new_block.size > 0) {
            new_block.min_key = new_block.data[0].first;
            new_block.max_key = new_block.data[new_block.size - 1].first;
        }

        new_block.next = block.next;
        block.next = db_file.write(new_block);    //写入的位置对吗？
        db_file.update(block, addr);
    }

    int find_block_for_insert(const KeyType& key) {
        if (head_addr == -1) return -1;

        BlockNode<KeyType, ValueType> block;
        int curr_addr = head_addr;
        int last_addr = -1;

        while (curr_addr != -1) {
            db_file.read(block, curr_addr);

            if (block.may_contain(key)) {
                return curr_addr;
            }

            if (block.size > 0 && key < block.min_key) {
                if (last_addr != -1) {
                    BlockNode<KeyType, ValueType> prev_block;
                    db_file.read(prev_block, last_addr);
                    if (prev_block.may_contain(key)) {
                        return last_addr;
                    }
                }
                return head_addr;
            }

            if (block.next == -1) {
                return curr_addr;
            }

            last_addr = curr_addr;
            curr_addr = block.next;
        }

        return -1;
    }
public:




};





#endif //BOOKSTORE_2025_BLOCKLIST_H