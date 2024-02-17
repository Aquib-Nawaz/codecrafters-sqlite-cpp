//
// Created by Aquib Nawaz on 17/02/24.
//

#ifndef GIT_STARTER_CPP_COMMAND_H
#define GIT_STARTER_CPP_COMMAND_H

#include <cstring>
#include <string>
#include <vector>

#define COUNT_COMMAND 1
#define COLUMNS_COMMAND 2

#define COLUMN_TYPE_TEXT 1
#define COLUMN_TYPE_INTEGER 2


struct CommandInfo{
    std::string tableName;
    std::vector<std::string> columns;
    int type;
    std::string whereColumn;
    std::string whereColumnValue;

};

std::vector<int> getColumnNums(std::string &, const std::vector<std::string>&);
CommandInfo parseCommand(const std::string&);

#endif //GIT_STARTER_CPP_COMMAND_H
