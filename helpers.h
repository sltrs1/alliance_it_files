#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <thread>
#include <future>
#include <fstream>
#include <mutex>
#include <chrono>
#include <unordered_map>

constexpr int RECORD_MAX_LEN = 7;
constexpr int FLIGHT_MAX_LEN = 5;

std::string validateString(const std::string& input);

bool compareFlightRecords(const std::string& firstRecord, const std::string& secondtRecord);

std::string getOutFileName(const std::string& fileName);

int processFile(const std::string& fileName);

#endif // HELPERS_H
