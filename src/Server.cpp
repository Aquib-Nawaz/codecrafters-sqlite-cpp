#include <cstring>
#include <iostream>
#include <fstream>
#include "btree.h"

int main(int argc, char* argv[]) {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here" << std::endl;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string database_file_path = argv[1];
    std::string command = argv[2];

    if (command == ".dbinfo") {
        std::ifstream database_file(database_file_path, std::ios::binary);
        if (!database_file) {
            std::cerr << "Failed to open the database file" << std::endl;
            return 1;
        }

        // Uncomment this to pass the first stage
         database_file.seekg(16);  // Skip the first 16 bytes of the header

         unsigned short page_size = bigEndian(&database_file, 2);
         unsigned int realPageSize = page_size;

         if(page_size == 1)
             realPageSize = 0x10000;
        char table[] = "table";
        uint64_t cnt = countWithWhereClause(&database_file, 1, 1, (void *)&table, realPageSize);
        std::cout << "database page size: " << realPageSize << std::endl;
        std::cout << "number of tables: " << cnt << std::endl;

        database_file.close();
    }

    return 0;
}
