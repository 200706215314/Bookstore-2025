//
// Created by lenovo on 2025/12/12.
//

#ifndef BOOKSTORE_2025_ACCOUNT_H
#define BOOKSTORE_2025_ACCOUNT_H

#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <algorithm>
#include "map.h"
#include "MemoryRiver.h"
#include "book.h"

constexpr int MAX_INDEX_LEN = 31;

class Account {
private:
    int privilege_;
    char UserID[31];
    char Username[31];
    char Password[31];

public:
    Account():privilege_(0){
        memset(UserID, 0, sizeof(UserID));
        memset(Username, 0, sizeof(Username));
        memset(Password, 0, sizeof(Password));
    }

    Account(int p, const std::string& user_id, const std::string& user_name, const std::string& password):privilege_(p){
        strcpy(UserID, user_id.c_str());
        strcpy(Username, user_name.c_str());
        strcpy(Password, password.c_str());
    }

    ~Account() = default;

    void setPassword(const std::string& newPassword) {
        strcpy(Password, newPassword.c_str());
    }

    void setPrivilege(int p) { privilege_ = p; }

    //获取信息类函数
    int getPrivilege() const;
    std::string getUserID() const;
    std::string getUsername() const;
    std::string getPassword() const;

    bool operator<(const Account& other) const {
        return strcmp(UserID, other.UserID) < 0;
    }

    bool operator==(const Account& other) const {
        return strcmp(UserID, other.UserID) == 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const Account& acc) {
        os << acc.getUserID() << " " << acc.getUsername();
        return os;
    }
};

class CharIndex {
private:
    char data[MAX_INDEX_LEN] = {0};

public:
    CharIndex() {
        data[0] = '\0';
    }

    CharIndex(const std::string &str) {
        strncpy(data, str.c_str(), MAX_INDEX_LEN - 1);
        data[MAX_INDEX_LEN - 1] = '\0';
    }

    CharIndex(const char* str) {
        if (str) {
            strncpy(data, str, MAX_INDEX_LEN - 1);
            data[MAX_INDEX_LEN - 1] = '\0';
        } else {
            data[0] = '\0';
        }
    }

    CharIndex(const CharIndex &other) {
        strncpy(data, other.data, MAX_INDEX_LEN);
    }

    CharIndex& operator=(const CharIndex &other) {
        if (this != &other) {
            strncpy(data, other.data, MAX_INDEX_LEN);
        }
        return *this;
    }

    bool operator<(const CharIndex &other) const {
        return strcmp(data, other.data) < 0;
    }

    bool operator>(const CharIndex &other) const {
        return strcmp(data, other.data) > 0;
    }

    bool operator<=(const CharIndex &other) const {
        return strcmp(data, other.data) <= 0;
    }

    bool operator>=(const CharIndex &other) const {
        return strcmp(data, other.data) >= 0;
    }

    bool operator==(const CharIndex &other) const {
        return strcmp(data, other.data) == 0;
    }

    bool operator!=(const CharIndex &other) const {
        return strcmp(data, other.data) != 0;
    }

    friend std::ostream& operator<<(std::ostream &os, const CharIndex &obj) {
        os << obj.data;
        return os;
    }

    const char* c_str() const {
        return data;
    }

    bool empty() const {
        return data[0] == '\0';
    }
};

class LoginInfo {
private:
    Account account;
    std::string selectedISBN;

public:
    LoginInfo() : account(Account()), selectedISBN("") {}
    LoginInfo(const Account& acc, const std::string& isbn = "")
        : account(acc), selectedISBN(isbn) {}

    Account getAccount() const { return account; }
    std::string getSelectedISBN() const { return selectedISBN; }
    void setSelectedISBN(const std::string& isbn) { selectedISBN = isbn; }
    void clearSelectedISBN() { selectedISBN = ""; }
};

class AccountSystem {
    friend class Bookstore;
private:
    Map<CharIndex, Account> accountMap;
    std::vector<LoginInfo> loginStack;
    std::string accountFile;


    //验证函数
    bool isValidUserID(const std::string& uid) const;
    bool isValidPassword(const std::string& pwd) const;
    bool isValidUsername(const std::string& uname) const;
    bool isValidPrivilege(int p) const;
    bool isValidPrivilegeStr(const std::string& p) const;

    //辅助help
    bool userExists(const std::string& userID);
    Account getUser(const std::string& userID);
    void updateUser(const Account& account);
    bool checkPrivilege(const int& required) const;

public:
    explicit AccountSystem(const std::string& filename);

    void initialize();

    // 账户指令实现
    bool login(const std::string& userID, const std::string& password );
    bool logout();
    bool registerUser(const std::string& userID, const std::string& password,
                      const std::string& username);
    bool changePassword(const std::string& userID, const std::string& currentPassword,
                        const std::string& newPassword);
    bool addUser(const std::string& userID, const std::string& password,
                 int privilege, const std::string& username);
    bool deleteUser(const std::string& userID);

    // 权限和状态查询
    int getCurrentPrivilege() const;
    Account getCurrentAccount() const;
    std::string getCurrentUserID() const;
    bool isLoggedIn() const;

    // 图书选择相关
    bool selectBook(const std::string& ISBN);
    std::string getSelectedISBN() const;
    void clearSelectedBook();

    // 登录栈操作
    size_t getLoginStackSize() const;
    bool isUserLoggedIn(const std::string& userID) const;

    bool hasPrivilege(int required) const;

    void updateSelectedISBNForAll(const std::string& oldISBN, const std::string& newISBN);
};
#endif //BOOKSTORE_2025_ACCOUNT_H