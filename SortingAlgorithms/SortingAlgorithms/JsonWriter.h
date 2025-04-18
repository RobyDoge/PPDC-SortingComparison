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
     void AddEntry(const std::string& algorithm, int num_cpus, int data_size, double time_seconds);

private:
    std::string _filename;
    json _data;

};
