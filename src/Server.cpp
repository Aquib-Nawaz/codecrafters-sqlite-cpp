#include <cstring>
#include <iostream>
#include <fstream>
#include "btree.h"
#include <sstream>

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

        uint64_t cnt = countWithWhereClause<char*>(&database_file, 1, 1, (void *)table, realPageSize);
        std::cout << "database page size: " << realPageSize << std::endl;
        std::cout << "number of tables: " << cnt << std::endl;

        database_file.close();
    }

    else if(command == ".tables") {
        std::vector<char *> retList;
        countWithWhereClause(&database_file, 1, 1, (void *)table, realPageSize, 2, &retList);
        for(int i=retList.size()-1; i>0;i--){
            printf("%s ", retList[i]);
            free(retList[i]);
        }
        printf("%s",retList.front());
        free(retList.front());

    }

    else{
        std::string query = argv[2];
        std::vector<uint64_t> retList;
        std::vector<std::string> keyWords = split(query, " ");
        countWithWhereClause(&database_file, 1, 2, (void *)(keyWords.back().data()),
                             realPageSize, 4, &retList);

        assert(!retList.empty());
        std::cout << countRows(&database_file, retList.front(), realPageSize) << std::endl;
    }

    return 0;
}
