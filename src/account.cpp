#include "../include/account.h"

int Account::getPrivilege() const {
    return privilege_;
}

std::string Account::getUserID() const {
    return std::string(UserID);
}

std::string Account::getUsername() const {
    return std::string(Username);
}

std::string Account::getPassword() const {
    return std::string(Password);
}

bool fileExists(const std::string& filename) {
    std::ifstream f(filename);
    return f.good();
}

AccountSystem::AccountSystem(const std::string& filename)
    : accountMap(filename), accountFile(filename) {
    if (!userExists("root")) {
        initialize();
    }
}

void AccountSystem::initialize() {
    Account root(7, "root", "Manager", "sjtu");
    accountMap.insert("root", root);
}

bool AccountSystem::isValidUserID(const std::string& uid) const {
    if (uid.empty() || uid.length() > 30) return false;
    for (char c : uid) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool AccountSystem::isValidPassword(const std::string& pwd) const {
    if (pwd.empty() || pwd.length() > 30) return false;
    for (char c : pwd) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool AccountSystem::isValidUsername(const std::string& uname) const {
    if (uname.empty() || uname.length() > 30) return false;
    for (char c : uname) {
        if (c < 32 || c > 126) return false;
    }
    return true;
}

bool AccountSystem::isValidPrivilege(int p) const {
    return p == 1 || p == 3 || p == 7;
}

bool AccountSystem::isValidPrivilegeStr(const std::string& p) const {
    if (p.length() != 1) return false;
    char c = p[0];
    return c == '1' || c == '3' || c == '7';
}

//辅助help
bool AccountSystem::userExists(const std::string& userID) {
    return !accountMap.find(userID).empty();
}
Account AccountSystem::getUser(const std::string& userID) {
    const std::vector<Account> accounts = accountMap.find(userID);
    if (!accounts.empty()) {
        return accounts[0];
    }
    return Account();
}

void AccountSystem::updateUser(const Account& account) {
    const std::vector<Account> accounts = accountMap.find(account.getUserID());
    for (const auto& acc : accounts) {
        accountMap.remove(account.getUserID(), acc);
    }
    accountMap.insert(account.getUserID(), account);
}

bool AccountSystem::checkPrivilege(const int& required) const {
    return getCurrentPrivilege() >= required;
}


// 账户指令实现
bool AccountSystem::login(const std::string& userID, const std::string& password = "") {
    if (!isValidUserID(userID)) {
        return false;
    }

    if (!userExists(userID)) {
        return false;
    }
    const Account tmp = getUser(userID);

    if (password.empty()) {
        if (loginStack.empty() || getCurrentPrivilege() <= tmp.getPrivilege()) {
            return false;
        }
        loginStack.push_back(LoginInfo(tmp));
        return true;
    }
    else {
        if (!isValidPassword(password)) {
            return false;
        }
        if (tmp.getPassword() != password) {
            return false;
        }
        loginStack.push_back(LoginInfo(tmp));
        return true;
    }
}
bool AccountSystem::logout() {
    if (loginStack.empty()) {
        return false;
    }

    loginStack.pop_back();
    return true;
}

bool AccountSystem::registerUser(const std::string& userID, const std::string& password, const std::string& username) {
    if (!isValidUserID(userID) || !isValidPassword(password) || !isValidUsername(username)) {
        return false;
    }
    if (userExists(userID)) {
        return false;
    }
    else {
        Account tmp = Account(1, userID, username, password);
        accountMap.insert(userID, tmp);
        return true;
    }
}

bool AccountSystem::changePassword(const std::string& userID, const std::string& currentPassword, const std::string& newPassword) {
    if (!isValidUserID(userID) || !isValidPassword(newPassword)) {
        return false;
    }
    if (!userExists(userID)) {
        return false;
    }

    Account target = getUser(userID);

    if (currentPassword.empty()) {
        if (getCurrentPrivilege() != 7) {
            return false;
        }
    } else {
        if (!isValidPassword(currentPassword)) {
            return false;
        }

        if (target.getPassword() != currentPassword) {
            return false;
        }
    }

    target.setPassword(newPassword);
    updateUser(target);
    return true;
}

bool AccountSystem::addUser(const std::string& userID, const std::string& password, int privilege, const std::string& username) {
    if (!isValidPassword(password) || !isValidUserID(userID) || !isValidUsername(username) || !isValidPrivilege(privilege)) {
        return false;
    }

    if (privilege >= getCurrentPrivilege()) {
        return false;
    }

    if (userExists(userID)) {
        return false;
    }

    const Account newAccount(privilege, userID, username, password);
    accountMap.insert(userID, newAccount);
    return true;
}

bool AccountSystem::deleteUser(const std::string& userID) {
    if (!isValidUserID(userID)) {
        return false;
    }

    if (!userExists(userID)) {
        return false;
    }

    Account target = getUser(userID);

    if (isUserLoggedIn(userID)) {
        return false;
    }

    accountMap.remove(userID, target);
    return true;
}


// 权限和状态查询
int AccountSystem::getCurrentPrivilege() const {
    if (loginStack.empty()) return 0;
    return loginStack.back().getAccount().getPrivilege();
}

Account AccountSystem::getCurrentAccount() const {
    if (loginStack.empty()) return Account();
    return loginStack.back().getAccount();
}

std::string AccountSystem::getCurrentUserID() const {
    if (loginStack.empty())  return "";
    return loginStack.back().getAccount().getUserID();
}

bool AccountSystem::isLoggedIn() const {
    return !loginStack.empty();
}


// 图书选择相关
bool AccountSystem::selectBook(const std::string& ISBN) {
    if (loginStack.empty())  return false;
    loginStack.back().setSelectedISBN(ISBN);
    return true;
}


std::string AccountSystem::getSelectedISBN() const {
    if (loginStack.empty()) {
        return "";
    }
    return loginStack.back().getSelectedISBN();
}

void AccountSystem::clearSelectedBook() {
    if (!loginStack.empty()) {
        loginStack.back().clearSelectedISBN();
    }
}

size_t AccountSystem::getLoginStackSize() const {
    return loginStack.size();
}


// 登录栈操作
bool AccountSystem::isUserLoggedIn(const std::string& userID) const {
    for (const auto& loginInfo : loginStack) {
        if (loginInfo.getAccount().getUserID() == userID) {
            return true;
        }
    }
    return false;
}

bool AccountSystem::hasPrivilege(const int required) const {
    return checkPrivilege(required);
}

void AccountSystem::updateSelectedISBNForAll(const std::string& oldISBN, const std::string& newISBN) {
    for (auto& loginInfo : loginStack) {
        if (loginInfo.getSelectedISBN() == oldISBN) {
            loginInfo.setSelectedISBN(newISBN);
        }
    }
}

