#ifndef BOOKSTORE_2025_LOG_H
#define BOOKSTORE_2025_LOG_H

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "account.h"

struct OperationLog {
    std::string timestamp;
    std::string userID;
    std::string command;
    std::string details;

    OperationLog() = default;
    OperationLog(const std::string& uid, const std::string& cmd, const std::string& det = "")
        : userID(uid), command(cmd), details(det) {
        time_t now = time(nullptr);
        char timeStr[100];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
        timestamp = timeStr;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << timestamp << "] "
            << "用户: " << std::setw(15) << std::left << userID
            << " | 命令: " << std::setw(20) << std::left << command;
        if (!details.empty()) {
            oss << " | 详情: " << details;
        }
        return oss.str();
    }
};

struct EmployeeRecord {
    std::string userID;
    std::string username;
    int privilege;
    int operationCount;
    std::string lastActive;

    EmployeeRecord() : privilege(0), operationCount(0) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << "员工ID: " << std::setw(15) << std::left << userID
            << " | 姓名: " << std::setw(15) << std::left << username
            << " | 权限: " << privilege
            << " | 操作次数: " << std::setw(8) << std::left << operationCount
            << " | 最后活跃: " << lastActive;
        return oss.str();
    }
};

class LogSystem {
private:
    std::vector<OperationLog> operationLogs;
    std::vector<EmployeeRecord> employeeRecords;
    std::string logFileName;

    AccountSystem* accountSystem;

    void updateEmployeeRecord(const std::string& userID, const std::string& username, int privilege);

public:
    LogSystem();

    void setAccountSystem(AccountSystem* accSystem) {
        accountSystem = accSystem;
    }

    void logOperation(const std::string& userID, const std::string& command,
                     const std::string& details = "");

    std::vector<OperationLog> getAllLogs() const;

    std::vector<OperationLog> getUserLogs(const std::string& userID) const;

    std::vector<EmployeeRecord> getEmployeeRecords() const;

    std::string generateFinanceReport(const std::vector<std::pair<double, double>>& financeData) const;

    std::string generateEmployeeReport() ;

    std::string generateFullLogReport() const;

    void collectEmployeeRecordsFromLogs();

    void updateEmployeeRecordsFromAccounts(const std::vector<Account>& accounts);

    void clearLogs();
};

#endif // BOOKSTORE_2025_LOG_H