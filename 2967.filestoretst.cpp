#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>


constexpr int MAX_INDEX_LEN = 65;
constexpr int BLOCK_SIZE = 400;

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 3>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
    bool is_open = false;

    void open() {
        if (!is_open) {
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                file.clear();
                file.open(file_name, std::ios::out | std::ios::binary);
                file.close();
                file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
            }
            is_open = true;
        }
    }

public:
    MemoryRiver() = default;

    explicit MemoryRiver(string  file_name) : file_name(std::move(file_name)) {}

    ~MemoryRiver() {
        if (is_open) {
            file.close();
            is_open = false;
        }
    }

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        ofstream ofs(file_name, std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            ofs.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        ofs.close();
        if (is_open) {
            file.close();
            is_open = false;
        }
    }

    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        open();
        file.seekg((n - 1) * sizeof(int));
        file.read(reinterpret_cast<char*>(&tmp), sizeof(int));
    }

    void write_info(int tmp, int n) {
        if (n > info_len) return;
        open();
        file.seekp((n - 1) * sizeof(int));
        file.write(reinterpret_cast<char*>(&tmp), sizeof(int));
    }

    int write(T &t) {
        open();
        file.seekp(0, std::ios::end);
        const int index = file.tellp();
        file.write(reinterpret_cast<char*>(&t), sizeofT);
        return index;
    }

    void update(T &t, const int index) {
        open();
        file.seekp(index, std::ios::beg);
        file.write(reinterpret_cast<char *>(&t), sizeofT);
    }

    void read(T &t, const int index) {
        open();
        file.seekg(index, std::ios::beg);
        file.read(reinterpret_cast<char *>(&t), sizeofT);
    }

    void Delete(int index) {
        T obj{};
        update(obj, index);
    }

};

class KeyValue {
public:
    char index[MAX_INDEX_LEN]{};
    int value;

    KeyValue() : value(0) {
        memset(index, 0, sizeof(index));
    }

    KeyValue(const std::string &idx, int val) : value(val) {
        strncpy(index, idx.c_str(), MAX_INDEX_LEN - 1);
        index[MAX_INDEX_LEN - 1] = '\0';
    }

    bool operator<(const KeyValue &other) const {
        if (strcmp(index, other.index) != 0)
            return strcmp(index, other.index) < 0;
        return value < other.value;
    }

    bool operator==(const KeyValue &other) const {
        return strcmp(index, other.index) == 0 && value == other.value;
    }

    bool empty() const {
        return index[0] == '\0' && value == 0;
    }
};


class Block {
public:
    char min_index[MAX_INDEX_LEN]{};
    char max_index[MAX_INDEX_LEN]{};
    int count;
    int next;
    KeyValue data[BLOCK_SIZE];

    Block() : count(0), next(-1) {
        memset(min_index, 0, sizeof(min_index));
        memset(max_index, 0, sizeof(max_index));
    }


    void updateMinMax() {
        if (count == 0) {
            min_index[0] = max_index[0] = '\0';
            return;
        }
        strcpy(min_index, data[0].index);
        strcpy(max_index, data[count - 1].index);
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
            min_index[0] = max_index[0] = '\0';
        }

        return true;
    }

    void getAllValues(const char *index, std::vector<int> &values) const {
        if (count == 0 ||
            strcmp(index, min_index) < 0 ||
            strcmp(index, max_index) > 0) {
            return;
        }

        int left = 0, right = count - 1, first = -1;
        while (left <= right) {
            int mid = (left + right) / 2;
            int cmp = strcmp(data[mid].index, index);
            if (cmp == 0) {
                first = mid;
                right = mid - 1;
            } else if (cmp < 0) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        if (first == -1) return;

        for (int i = first; i < count && strcmp(data[i].index, index) == 0; i++) {
            values.push_back(data[i].value);
        }
    }
};

class BlockList {
private:
    MemoryRiver<Block, 3> blockFile;
    std::string filename;
    int head;
    int blockCount;

public:
    explicit BlockList(const std::string &fname) : blockFile(fname), filename(fname) {
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

    void insert(const std::string &index, int value) {
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

            if (strcmp(kv.index, currentBlock.max_index) <= 0) {
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


    void remove(const std::string &index, int value) {
        KeyValue kv(index, value);

        if (head == -1) return;

        int current = head;
        int prevAddr = -1;
        Block block;

        while (current != -1) {
            blockFile.read(block, current);

            int cmp_min = strcmp(kv.index, block.min_index);
            int cmp_max = strcmp(kv.index, block.max_index);

            if (cmp_min >= 0 && cmp_max <= 0) {
                if (block.remove(kv)) {
                    blockFile.update(block, current);

                    if (block.count == 0) {
                        deleteBlock(current, prevAddr);
                    }
                    return;
                }
            } else if (cmp_min < 0) {
                break;
            }

            prevAddr = current;
            current = block.next;
        }
    }

    void find(const std::string &index) {
        std::vector<int> values;
        const char* idx_cstr = index.c_str();

        if (head != -1) {
            int current = head;
            Block block;

            while (current != -1) {
                blockFile.read(block, current);

                if (block.count == 0) {
                    current = block.next;
                    continue;
                }

                int cmp_min = strcmp(idx_cstr, block.min_index);
                int cmp_max = strcmp(idx_cstr, block.max_index);

                if (cmp_min >= 0 && cmp_max <= 0) {
                    block.getAllValues(idx_cstr, values);
                } else if (cmp_min < 0) {
                    break;
                }

                current = block.next;
            }
        }

        if (values.empty()) {
            std::cout << "null\n";
        } else {
            std::sort(values.begin(), values.end());

            for (size_t i = 0; i < values.size(); ++i) {
                if (i > 0) std::cout << " ";
                std::cout << values[i];
            }
            std::cout << "\n";
        }
    }

private:
    void splitBlock(const int blockAddr) {
        Block oldBlock;
        blockFile.read(oldBlock, blockAddr);

        if (oldBlock.count < BLOCK_SIZE) return;

        Block newBlock;
        int mid = oldBlock.count / 2;

        newBlock.count = oldBlock.count - mid;
        memcpy(newBlock.data, oldBlock.data + mid, newBlock.count * sizeof(KeyValue));

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
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    BlockList db("database.dat");

    int n;
    std::cin >> n;

    for (int i = 0; i < n; ++i) {
        std::string cmd, index;
        int value;
        std::cin >> cmd;

        if (cmd == "insert") {
            std::cin >> index >> value;
            db.insert(index, value);
        } else if (cmd == "delete") {
            std::cin >> index >> value;
            db.remove(index, value);
        } else if (cmd == "find") {
            std::cin >> index;
            db.find(index);
        }
    }

    return 0;
}