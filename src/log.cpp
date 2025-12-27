#include "../include/log.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <unordered_map>

LogSystem::LogSystem() : logFileName("system_log.txt"), accountSystem(nullptr) { };

void LogSystem::logOperation(const std::string& userID, const std::string& command,
                            const std::string& details) {
    OperationLog log(userID, command, details);
    operationLogs.push_back(log);

    if (accountSystem) {
        Account account = accountSystem->getAccountByID(userID);
        if (account.getUserID() == userID) {
            updateEmployeeRecord(userID, account.getUsername(), account.getPrivilege());
        } else {
            updateEmployeeRecord(userID, userID, 1);  // 默认顾客权限
        }
    }

    std::ofstream logFile(logFileName, std::ios::app);
    if (logFile.is_open()) {
        logFile << log.toString() << std::endl;
        logFile.close();
    }
}

void LogSystem::updateEmployeeRecord(const std::string& userID, 
                                    const std::string& username, 
                                    int privilege) {
    for (auto& record : employeeRecords) {
        if (record.userID == userID) {
            record.operationCount++;

            if (!username.empty() && username != userID) {
                record.username = username;
            }
            if (privilege != 1) {
                record.privilege = privilege;
            }

            time_t now = time(nullptr);
            char timeStr[100];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
            record.lastActive = timeStr;

            return;
        }
    }

    EmployeeRecord record;
    record.userID = userID;
    record.username = username.empty() ? userID : username;
    record.privilege = privilege;
    record.operationCount = 1;

    time_t now = time(nullptr);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
    record.lastActive = timeStr;

    employeeRecords.push_back(record);
}

std::vector<OperationLog> LogSystem::getAllLogs() const {
    return operationLogs;
}

std::vector<OperationLog> LogSystem::getUserLogs(const std::string& userID) const {
    std::vector<OperationLog> userLogs;
    for (const auto& log : operationLogs) {
        if (log.userID == userID) {
            userLogs.push_back(log);
        }
    }
    return userLogs;
}

std::vector<EmployeeRecord> LogSystem::getEmployeeRecords() const {
    return employeeRecords;
}

void LogSystem::collectEmployeeRecordsFromLogs() {
    if (!accountSystem) {
        std::cerr << "ERROR: AccountSystem not set in LogSystem" << std::endl;
        return;
    }

    std::map<std::string, EmployeeRecord> employeeMap;

    // 从操作日志中统计
    for (const auto& log : operationLogs) {
        std::string userID = log.userID;

        if (userID.empty()) continue;

        auto it = employeeMap.find(userID);
        if (it != employeeMap.end()) {
            it->second.operationCount++;
            it->second.lastActive = log.timestamp;
        } else {
            EmployeeRecord record;
            record.userID = userID;

            try {
                Account account = accountSystem->getAccountByID(userID);
                if (account.getUserID() == userID) {
                    record.username = account.getUsername();
                    record.privilege = account.getPrivilege();
                } else {
                    record.username = userID;
                    record.privilege = 1;
                }
            } catch (...) {
                record.username = userID;
                record.privilege = 1;
            }

            record.operationCount = 1;
            record.lastActive = log.timestamp;
            employeeMap[userID] = record;
        }
    }

    employeeRecords.clear();
    for (const auto& [userID, record] : employeeMap) {
        employeeRecords.push_back(record);
    }

    std::cerr << "DEBUG: 收集到 " << employeeRecords.size() << " 条员工记录" << std::endl;
}

void LogSystem::updateEmployeeRecordsFromAccounts(const std::vector<Account>& accounts) {
    std::unordered_map<std::string, std::pair<std::string, int>> accountInfo;
    for (const auto& account : accounts) {
        accountInfo[account.getUserID()] = {
            account.getUsername(),
            account.getPrivilege()
        };
    }

    for (auto& record : employeeRecords) {
        auto it = accountInfo.find(record.userID);
        if (it != accountInfo.end()) {
            record.username = it->second.first;
            record.privilege = it->second.second;
        }
    }
}

std::string LogSystem::generateFinanceReport(const std::vector<std::pair<double, double>>& financeData) const {
    std::ostringstream oss;
    
    oss << "=========================================================\n";
    oss << "                  财务报表\n";
    oss << "=========================================================\n\n";
    
    if (financeData.empty()) {
        oss << "暂无财务数据\n";
        return oss.str();
    }
    
    double totalIncome = 0.0;
    double totalExpense = 0.0;
    int transactionCount = 1;
    
    oss << "序号 |     收入     |     支出     |     净收益     |  交易时间\n";
    oss << "-----+--------------+--------------+---------------+-------------------\n";
    
    for (const auto& record : financeData) {
        double income = record.first;
        double expense = record.second;
        double netProfit = income - expense;
        
        totalIncome += income;
        totalExpense += expense;
        
        oss << std::setw(4) << std::right << transactionCount++ << " | ";
        oss << std::setw(12) << std::right << std::fixed << std::setprecision(2) << income << " | ";
        oss << std::setw(12) << std::right << expense << " | ";
        oss << std::setw(13) << std::right << netProfit << " | ";
        oss << "第" << transactionCount-1 << "笔交易\n";
    }
    
    oss << "\n=========================================================\n";
    oss << "财务汇总：\n";
    oss << "总交易笔数: " << financeData.size() << "\n";
    oss << "总收入: ¥" << std::fixed << std::setprecision(2) << totalIncome << "\n";
    oss << "总支出: ¥" << totalExpense << "\n";
    oss << "总利润: ¥" << (totalIncome - totalExpense) << "\n";
    oss << "利润率: " << std::setprecision(1) 
        << (totalIncome > 0 ? ((totalIncome - totalExpense) / totalIncome * 100) : 0) << "%\n";
    oss << "=========================================================\n";
    
    return oss.str();
}

std::string LogSystem::generateEmployeeReport() {
    std::ostringstream oss;

    oss << "=========================================================\n";
    oss << "               员工工作情况报告\n";
    oss << "=========================================================\n\n";

    collectEmployeeRecordsFromLogs();

    if (employeeRecords.empty()) {
        oss << "暂无员工记录\n";
        oss << "=========================================================\n";
        return oss.str();
    }

    std::vector<EmployeeRecord> sortedRecords = employeeRecords;
    std::sort(sortedRecords.begin(), sortedRecords.end(),
        [](const EmployeeRecord& a, const EmployeeRecord& b) {
            if (a.privilege != b.privilege) {
                return a.privilege > b.privilege;
            }
            return a.operationCount > b.operationCount;
        });

    oss << "排名 | 员工ID          | 姓名              | 权限 | 操作次数 | 最后活跃时间\n";
    oss << "-----+-----------------+-------------------+------+----------+-------------------\n";

    int rank = 1;
    int totalOperations = 0;
    int ownerCount = 0, employeeCount = 0, customerCount = 0;

    for (const auto& record : sortedRecords) {
        std::string privilegeStr;
        switch (record.privilege) {
            case 7:
                privilegeStr = "店主";
                ownerCount++;
                break;
            case 3:
                privilegeStr = "员工";
                employeeCount++;
                break;
            case 1:
                privilegeStr = "顾客";
                customerCount++;
                break;
            default:
                privilegeStr = "游客(" + std::to_string(record.privilege) + ")";
                break;
        }

        oss << std::setw(4) << std::right << rank++ << " | ";
        oss << std::setw(15) << std::left << record.userID << " | ";
        oss << std::setw(17) << std::left << record.username << " | ";
        oss << std::setw(4) << std::left << privilegeStr << " | ";
        oss << std::setw(8) << std::left << record.operationCount << " | ";
        oss << record.lastActive << "\n";

        totalOperations += record.operationCount;
    }

    oss << "\n=========================================================\n";
    oss << "工作汇总：\n";
    oss << "总记录数: " << sortedRecords.size() << "\n";
    oss << "总操作次数: " << totalOperations << "\n";
    if (!sortedRecords.empty()) {
        oss << "平均每人操作次数: "
            << std::fixed << std::setprecision(2)
            << (totalOperations / (double)sortedRecords.size()) << "\n";
    }

    oss << "\n权限分布：\n";
    oss << "店主 (" << ownerCount << "人): "
        << (ownerCount > 0 ? "✓ 有店主账户" : "⚠️ 无店主账户") << "\n";
    oss << "员工 (" << employeeCount << "人): "
        << (employeeCount > 0 ? "✓ 有员工账户" : "⚠️ 无员工账户") << "\n";
    oss << "顾客 (" << customerCount << "人): "
        << (customerCount > 0 ? "✓ 有顾客账户" : "⚠️ 无顾客账户") << "\n";

    EmployeeRecord mostActiveEmployee;
    for (const auto& record : sortedRecords) {
        if ((record.privilege == 3 || record.privilege == 7) &&
            record.operationCount > mostActiveEmployee.operationCount) {
            mostActiveEmployee = record;
        }
    }

    if (mostActiveEmployee.operationCount > 0) {
        std::string role = (mostActiveEmployee.privilege == 7) ? "店主" : "员工";
        oss << "最活跃" << role << ": " << mostActiveEmployee.username
            << " (" << mostActiveEmployee.operationCount << "次操作)\n";
    }
    
    oss << "=========================================================\n";
    
    return oss.str();
}

std::string LogSystem::generateFullLogReport() const {
    std::ostringstream oss;
    
    oss << "=========================================================\n";
    oss << "                   系统完整日志\n";
    oss << "=========================================================\n\n";
    
    if (operationLogs.empty()) {
        oss << "暂无日志记录\n";
        return oss.str();
    }

    std::vector<OperationLog> sortedLogs = operationLogs;
    std::reverse(sortedLogs.begin(), sortedLogs.end()); // 最新的在前
    
    int count = 1;
    for (const auto& log : sortedLogs) {
        oss << "记录 #" << count++ << "\n";
        oss << "─────────────────────────────────────────────────────────\n";
        oss << log.toString() << "\n\n";

        if (count % 5 == 0) {
            oss << "═════════════════════════════════════════════════════════\n\n";
        }
    }
    
    oss << "=========================================================\n";
    oss << "日志统计：\n";
    oss << "总记录数: " << operationLogs.size() << "\n";
    
    if (!operationLogs.empty()) {
        oss << "最早记录: " << operationLogs.front().timestamp << "\n";
        oss << "最新记录: " << operationLogs.back().timestamp << "\n";
    }

    std::map<std::string, int> userOperationCount;
    for (const auto& log : operationLogs) {
        userOperationCount[log.userID]++;
    }
    
    if (!userOperationCount.empty()) {
        oss << "\n用户操作统计：\n";
        for (const auto& [userID, count] : userOperationCount) {
            oss << "  " << std::setw(15) << std::left << userID 
                << ": " << count << " 次操作\n";
        }
    }
    
    oss << "=========================================================\n";
    
    return oss.str();
}

void LogSystem::clearLogs() {
    operationLogs.clear();
    employeeRecords.clear();
}