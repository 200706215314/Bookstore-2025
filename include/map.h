#ifndef BOOKSTORE_2025_MAP_H
#define BOOKSTORE_2025_MAP_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include "MemoryRiver.h"

constexpr int BLOCK_SIZE = 2000;

template<typename KeyType, typename ValueType>
class Map {
private:
    struct KeyValue {
        KeyType index;
        ValueType value;

        KeyValue() : index(KeyType()), value(ValueType()) {}

        KeyValue(const KeyType &idx, const ValueType &val) : index(idx), value(val) {}

        bool operator<(const KeyValue &other) const {
            if (index != other.index) {
                return index < other.index;
            }
            return value < other.value;
        }

        bool operator==(const KeyValue &other) const {
            return index == other.index && value == other.value;
        }

        bool empty() const {
            return index == KeyType() && value == ValueType();
        }
    };

    struct Block {
        KeyType min_index;
        KeyType max_index;
        int count;
        int next;
        KeyValue data[BLOCK_SIZE];

        Block() : count(0), next(-1) {
            min_index = KeyType();
            max_index = KeyType();
        }

        void updateMinMax() {
            if (count == 0) {
                min_index = KeyType();
                max_index = KeyType();
                return;
            }
            min_index = data[0].index;
            max_index = data[count - 1].index;
        }

        int findValuePos(const KeyValue &kv) const {
            int left = 0, right = count - 1;
            while (left <= right) {
                const int mid = (left + right) / 2;
                if (data[mid] == kv) return mid;
                if (data[mid] < kv) left = mid + 1;
                else right = mid - 1;
            }
            return -1;
        }

        bool insert(const KeyValue &kv) {
            if (count >= BLOCK_SIZE) return false;

            if (findValuePos(kv) != -1) {
                return false;
            }

            int left = 0, right = count - 1;
            int pos = count;

            while (left <= right) {
                int mid = (left + right) / 2;
                if (data[mid] < kv) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                    pos = mid;
                }
            }

            for (int i = count; i > pos; i--) {
                data[i] = data[i - 1];
            }

            data[pos] = kv;
            count++;

            updateMinMax();

            return true;
        }

        bool remove(const KeyValue &kv) {
            const int pos = findValuePos(kv);
            if (pos == -1) return false;

            for (int i = pos; i < count - 1; i++) {
                data[i] = data[i + 1];
            }
            count--;

            if (count > 0) {
                updateMinMax();
            } else {
                min_index = KeyType();
                max_index = KeyType();
            }

            return true;
        }

        void getAllValues(const KeyType &index, std::vector<ValueType> &values) const {
            if (count == 0 || index < min_index || index > max_index) {
                return;
            }

            int left = 0, right = count - 1, first = -1;
            while (left <= right) {
                int mid = (left + right) / 2;

                if (data[mid].index == index) {
                    first = mid;
                    right = mid - 1;
                } else if (data[mid].index < index) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }

            if (first == -1) return;

            for (int i = first; i < count && data[i].index == index; i++) {
                values.push_back(data[i].value);
            }
        }
    };

    MemoryRiver<Block, 3> blockFile;
    int head;
    int blockCount;
    std::string filename;

    void splitBlock(const int blockAddr) {
        Block oldBlock;
        blockFile.read(oldBlock, blockAddr);

        if (oldBlock.count < BLOCK_SIZE) return;

        Block newBlock;
        int mid = oldBlock.count / 2;

        newBlock.count = oldBlock.count - mid;
        for (int i = 0; i < newBlock.count; i++) {
            newBlock.data[i] = oldBlock.data[mid + i];
        }

        oldBlock.count = mid;
        oldBlock.updateMinMax();
        newBlock.updateMinMax();

        newBlock.next = oldBlock.next;
        oldBlock.next = blockFile.write(newBlock);

        blockFile.update(oldBlock, blockAddr);

        blockCount++;
        blockFile.write_info(blockCount, 2);
    }

    void deleteBlock(int blockAddr, int prevAddr) {
        Block block;
        blockFile.read(block, blockAddr);

        if (prevAddr != -1) {
            Block prevBlock;
            blockFile.read(prevBlock, prevAddr);
            prevBlock.next = block.next;
            blockFile.update(prevBlock, prevAddr);
        } else {
            head = block.next;
            blockFile.write_info(head, 1);
        }

        blockCount--;
        blockFile.write_info(blockCount, 2);
    }

public:
    explicit Map(const std::string &fname) : blockFile(fname), filename(fname) {
        std::ifstream test(fname);
        if (!test.good()) {
            blockFile.initialise(fname);
            head = -1;
            blockCount = 0;
            blockFile.write_info(head, 1);
            blockFile.write_info(blockCount, 2);
        } else {
            blockFile.get_info(head, 1);
            blockFile.get_info(blockCount, 2);
        }
    }

    void insert(const KeyType &index, const ValueType &value) {
        KeyValue kv(index, value);

        if (head == -1) {
            Block newBlock;
            newBlock.insert(kv);
            head = blockFile.write(newBlock);
            blockCount = 1;
            blockFile.write_info(head, 1);
            blockFile.write_info(blockCount, 2);
            return;
        }

        int current = head;
        int prevAddr = -1;
        int targetBlock = -1;
        Block currentBlock;

        while (current != -1) {
            blockFile.read(currentBlock, current);

            if (kv.index <= currentBlock.max_index) {
                targetBlock = current;
                break;
            }

            prevAddr = current;
            current = currentBlock.next;
        }

        if (targetBlock == -1) {
            targetBlock = prevAddr;
            blockFile.read(currentBlock, targetBlock);
        }

        bool inserted = currentBlock.insert(kv);
        if (inserted) {
            blockFile.update(currentBlock, targetBlock);

            if (currentBlock.count >= BLOCK_SIZE) {
                splitBlock(targetBlock);
            }
        }
    }

    void remove(const KeyType &index, const ValueType &value) {
        KeyValue kv(index, value);

        if (head == -1) return;

        int current = head;
        int prevAddr = -1;
        Block block;

        while (current != -1) {
            blockFile.read(block, current);

            if (kv.index >= block.min_index && kv.index <= block.max_index) {
                if (block.remove(kv)) {
                    blockFile.update(block, current);

                    if (block.count == 0) {
                        deleteBlock(current, prevAddr);
                    }
                    return;
                }
            } else if (kv.index < block.min_index) {
                break;
            }

            prevAddr = current;
            current = block.next;
        }
    }

    std::vector<ValueType> find(const KeyType &index) const{
        std::vector<ValueType> values;

        if (head != -1) {
            int current = head;
            Block block;

            while (current != -1) {
                blockFile.read(block, current);

                if (block.count == 0) {
                    current = block.next;
                    continue;
                }

                if (index >= block.min_index && index <= block.max_index) {
                    block.getAllValues(index, values);
                } else if (index < block.min_index) {
                    break;
                }

                current = block.next;
            }
        }

        std::sort(values.begin(), values.end());
        return values;
    }

    void findAndPrint(const KeyType &index) const{
        std::vector<ValueType> values = find(index);

        if (values.empty()) {
            std::cout << "null\n";
        } else {
            for (size_t i = 0; i < values.size(); ++i) {
                if (i > 0) std::cout << " ";
                std::cout << values[i];
            }
            std::cout << "\n";
        }
    }

    int getHead() const { return head; }
    int getBlockCount() const { return blockCount; }

    std::vector<ValueType> getAllValues() const{
        std::vector<ValueType> result;

        if (head == -1) return result;

        int current = head;
        Block block;

        while (current != -1) {
            blockFile.read(block, current);

            for (int i = 0; i < block.count; i++) {
                result.push_back(block.data[i].value);
            }

            current = block.next;
        }

        std::sort(result.begin(), result.end());
        return result;
    }
};

#endif //BOOKSTORE_2025_MAP_H