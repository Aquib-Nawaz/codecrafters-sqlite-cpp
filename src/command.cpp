//
// Created by Aquib Nawaz on 17/02/24.
//

#include "command.h"
#include "utility.h"

CommandInfo parseCommand(const std::string& command){
    std::vector<std::string> keyWords = split(command, " ");
    toLower(keyWords[1]);
    CommandInfo ret;
    if(keyWords[1].starts_with("count")){
        ret.type = COUNT_COMMAND;
        ret.tableName = trim(keyWords.back());
        return ret;
    }
    int i=1;
    ret.type = COLUMNS_COMMAND;
    while(i < keyWords.size()){
        std::string word = trim(keyWords[i]);
        ret.columns.push_back(word);
        toLower(word);
        if(word=="from"){
            ret.columns.pop_back();
            break;
        }
        i++;
    }
    ret.tableName = trim(keyWords[i+1]);
    i+=3;
    ret.whereColumn = trim(keyWords[i]);
    int valueStartPos = command.find('=');
    if(valueStartPos!=std::string::npos){
        ret.whereColumnValue = trim(command.substr(valueStartPos+1), "' ");
    }

    return ret;
}

std::vector<int> getColumnNums(std::string & createSqlText, const std::vector<std::string>&columnNames){

    createSqlText = createSqlText.substr(createSqlText.find('(')+1, createSqlText.size()-createSqlText.find('(')-2);
    std::vector<std::string>tableColumnNames = split( createSqlText, ",");

    std::vector<int> ret;

    for(int i=0; i<columnNames.size(); i++){
        int columnNum = 1;
        std::vector<std::string>columnNameType;
        while(columnNum <= tableColumnNames.size()){
            columnNameType = split(trim(tableColumnNames[columnNum - 1])," ");
            if(columnNameType[0]==columnNames[i]){
                ret.push_back(columnNum);
                break;
            }
            columnNum++;
        }
        if(columnNum > tableColumnNames.size()){
            for(int j=0; j<tableColumnNames.size(); j++){
                std::cout << tableColumnNames[j] << std::endl;
            }
            std::cout << createSqlText << std::endl;
        }
    }
    return ret;
}