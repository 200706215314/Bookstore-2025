//
// Created by lenovo on 2025/12/4.
//

#ifndef BOOKSTORE_2025_BLOCKNODE_H
#define BOOKSTORE_2025_BLOCKNODE_H

#include "MemoryRiver.h"
#include <iostream>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

constexpr int BLOCK_SIZE = 320;

// 数据条目
class Data {
private:
    char index[64]{};
    int value;
public:
    Data() {
        memset(index, 0, sizeof(index));
        value = -1;
    }
    Data(const string& s, const int v) {
        strcpy(index, s.c_str());
        value = v;
    }

    bool operator < (const Data& other) const {
        if (strcmp(index, other.index) != 0) {
            return strcmp(index, other.index) < 0;
        }
        return value < other.value;
    }

    bool operator==(const Data& other) const {
        return strcmp(index, other.index) == 0 && value == other.value;
    }
};

// 索引块
template<typename KeyType, typename ValueType>
class BlockNode {    //aim to do 1.插入 2.删除  3.查找
private:
    KeyType min_key;
    KeyType max_key;
    int size;
    int next;
    std::pair<KeyType, ValueType> data[BLOCK_SIZE];

public:
    BlockNode() {
        size = 0;
        next = -1;
        min_key = KeyType();          //这是什么初始化
        max_key = KeyType();
    }

    static bool compare_pair(const std::pair<KeyType, ValueType>& a,
                            const std::pair<KeyType, ValueType>& b) {
        if (!(a.first == b.first)) {
            return a.first < b.first;
        }
        return a.second < b.second;
    }

    bool may_contain(const KeyType& key) {      //运算符重载？
        if (size == 0) return false;
        return !(key < min_key) && !(max_key < key);
    }

    bool insert(const KeyType& key, const ValueType& value) {
        if (size >= BLOCK_SIZE) return false;

        int pos = 0;
        while (pos < size) {
            if (data[pos].first == key && data[pos].second == value) {
                return false;
            }
            if (data[pos].first < key ||
                (data[pos].first == key && data[pos].second < value)) {
                pos++;
                } else {
                    break;
                }
        }

        for (int i = size; i > pos; i--) {
            data[i] = data[i - 1];
        }
        data[pos] = {key, value};
        size++;

        if (size == 1) {
            min_key = key;
            max_key = key;
        } else {
            if (key < min_key) min_key = key;
            if (max_key < key) max_key = key;
        }

        return true;
    }

    bool remove(const KeyType& key, const ValueType& value) {
        int left = 0, right = size - 1;
        int pos = -1;

        while (left <= right) {
            int mid = (left + right) / 2;
            if (data[mid].first == key && data[mid].second == value) {
                pos = mid;
                break;
            }

            if (data[mid].first < key ||
                (data[mid].first == key && data[mid].second < value)) {
                left = mid + 1;
                } else {
                    right = mid - 1;
                }
        }

        if (pos == -1) return false;

        for (int i = pos; i < size - 1; i++) {
            data[i] = data[i + 1];
        }
        size--;

        if (size > 0) {
            min_key = data[0].first;
            max_key = data[size - 1].first;
        } else {
            min_key = KeyType();
            max_key = KeyType();
        }

        return true;
    }

    std::vector<ValueType> find_ids_for_key(const KeyType& key) {   //没写边界条件呢
        std::vector<ValueType> result;

        int left = 0, right = size - 1;
        int start = -1, end = -1;

        while (left <= right) {
            int mid = (left + right) / 2;
            if (data[mid].first == key) {
                start = mid;
                right = mid - 1;
            } else if (data[mid].first < key) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        if (start == -1) return result;

        left = start;
        right = size - 1;
        while (left <= right) {
            int mid = (left + right) / 2;
            if (data[mid].first == key) {
                end = mid;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        for (int i = start; i <= end; i++) {
            result.push_back(data[i].second);
        }

        return result;
    }


    int getSize() const { return size; }
    int getNext() const { return next; }
    const KeyType& getMinKey() const { return min_key; }
    const KeyType& getMaxKey() const { return max_key; }

    //没写特定key value的查找函数
};




#endif //BOOKSTORE_2025_BLOCKNODE_H