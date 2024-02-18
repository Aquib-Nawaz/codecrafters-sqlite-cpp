#include <cstring>
#include <iostream>
#include <fstream>
#include "btree.h"
#include <sstream>
#include "command.h"

int main(int argc, char* argv[]) {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
//    std::cout << "Logs from your program will appear here" << std::endl;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string database_file_path = argv[1];
    std::string command = argv[2];

    std::ifstream database_file(database_file_path, std::ios::binary);
    if (!database_file) {
        std::cerr << "Failed to open the database file" << std::endl;
        return 1;
    }

    database_file.seekg(16);
    unsigned short page_size = bigEndian(&database_file, 2);
    int realPageSize = page_size;

    if(page_size == 1)
        realPageSize = 0x10000;
    char table[] = "table";

    if (command == ".dbinfo") {

        uint64_t cnt = countWithWhereClause<char*>(&database_file, 1, SQLITE_SCHEMA_TYPE_COLUMN, (void *)table,
                                                   realPageSize, -1, nullptr);
        std::cout << "database page size: " << realPageSize << std::endl;
        std::cout << "number of tables: " << cnt << std::endl;

        database_file.close();
    }

    else if(command == ".tables") {
        std::vector<char *> retList;
        countWithWhereClause(&database_file, 1, SQLITE_SCHEMA_TYPE_COLUMN, (void *)table, realPageSize, SQLITE_SCHEMA_NAME_COLUMN, &retList);
        for(int i=0; i<retList.size()-1;i++){
            printf("%s ", retList[i]);
            free(retList[i]);
        }
        printf("%s",retList.back());
        free(retList.back());

    }

    else{
        std::string query = argv[2];
        std::vector<uint64_t> pageNum;

        CommandInfo commandInfo = parseCommand(query);

        countWithWhereClause(&database_file, 1, SQLITE_SCHEMA_NAME_COLUMN, (void *) (commandInfo.tableName.c_str()),
                             realPageSize, SQLITE_SCHEMA_PAGE_NUM_COLUMN, &pageNum);

        assert(!pageNum.empty());
        if(commandInfo.type == COUNT_COMMAND) {
            std::cout << countRows(&database_file, pageNum.front(), realPageSize) << std::endl;

        }
        else {

            std::vector<char*> tableCreateSql;
            countWithWhereClause(&database_file, 1, SQLITE_SCHEMA_NAME_COLUMN,
                                 (void *) (commandInfo.tableName.data()),realPageSize,
                                 SQLITE_SCHEMA_TEXT_COLUMN, &tableCreateSql);

            std::string createSqlText (tableCreateSql.front());
            std::vector<int> columnNums = getColumnNums(createSqlText, commandInfo.columns);

            int whereColumnNum = -1;
            void* whereColumnValue = nullptr;

            if(!commandInfo.whereColumn.empty()){
                whereColumnNum = getColumnNums(createSqlText, {commandInfo.whereColumn}).front();
                whereColumnValue = (void*)commandInfo.whereColumnValue.c_str();
            }

            std::vector<std::string> values;
            countWithWhereClause(&database_file, pageNum.front(), whereColumnNum, whereColumnValue,
                                     realPageSize, columnNums, &values);
            for(int i=0; i<values.size(); i++){
                std::cout << values[i] << std::endl;
            }
        }

    }

    return 0;
}
