#include "JsonWriter.h"

JsonWriter::JsonWriter(const std::string& filename) : _filename(filename) {
	std::ifstream inputFile(_filename);
	if (inputFile.is_open()) {
		try {
			inputFile >> _data;
			if (!_data.is_array()) {
				_data = json::array(); // Reset if not an array
			}
		}
		catch (...) {
			_data = json::array(); // Handle corrupt or empty file
		}
	}
	else {
		_data = json::array(); // File doesn't exist, start new array
	}
}

JsonWriter::~JsonWriter()
{
	std::ofstream outputFile(_filename, std::ios::trunc); // Overwrite file
	if (!outputFile.is_open()) {
		throw std::runtime_error("Failed to open file for writing: " + _filename);
	}
	outputFile << _data.dump(4); // Pretty-print with 4-space indent
}

void JsonWriter::AddEntry(const std::string& algorithm, int num_cpus, int data_size, double time_seconds)
{
	json entry = {
			{"algorithm", algorithm},
			{"num_cpus", num_cpus},
			{"data_size", data_size},
			{"time_seconds", time_seconds}
	};
	_data.push_back(entry);
}