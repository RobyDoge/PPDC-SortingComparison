#pragma once
#include <fstream>
#include <string>
#include "3rdParty/json.hpp"

using json = nlohmann::json;

class JsonWriter 
{
public:
    JsonWriter(const std::string& filename);
    ~JsonWriter();
     void AddEntry(const std::string& algorithm, int numCpus, int dataSize, double totalTime, double commTime, double sortTime);

private:
    std::string _filename;
    json _data;

};
