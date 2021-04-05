/*
Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "ConfigReader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

bool ConfigReader::Load(const std::string & config_filename)
{
  std::ifstream configFile;
  configFile.open(config_filename, std::ios_base::in);
  int line_number = 0;
  if (configFile.is_open()) {
    // read stream line by line
    for (std::string line; std::getline(configFile, line);) {
      line_number++;

      // Strip whitespace
      std::size_t start = line.find_first_not_of(" \n\r\t");
      if (start == std::string::npos) {
        continue;
      }
      line = line.substr(start);
      // Ignore comments
      if (line[0] == '#') {
        continue;
      }

      // Get the name
      std::size_t end = line.find_first_of(" \t");
      if (end == std::string::npos) {
        std::cerr << "Invalid config file at line " << line_number << std::endl;
        return false;
      }
      std::string name = line.substr(start, end - start);
      line = line.substr(end);

      // Strip whitespace between name value pair
      start = line.find_first_not_of(" \t");
      if (start == std::string::npos) {
        std::cerr << "Invalid config file at line " << line_number << std::endl;
        return false;
      }
      std::string value = line.substr(start);
#ifdef __linux__
      // Remove trailing \r s
      if (value.size() && value[value.size() -1] == '\r')
      {
        value.pop_back();
      }
#endif
      config_dict_.insert({ name, value });
    }
    configFile.close();
  }
  else {
    std::cerr << "Unable to open file: " << config_filename << std::endl;
    return false;
  }

  loaded_ = true;
  return true;
}

bool ConfigReader::IsConfigValueAvailable(const std::string& name) const {
  if (!loaded_) {
    std::cout << "Not loaded" << std::endl;
    return false;
  }

  return config_dict_.find(name) != config_dict_.end();
}

bool ConfigReader::GetConfigValue(const std::string& name, std::string* value) const {
  if (!loaded_) {
    std::cout << "Not loaded" << std::endl;
    return false;
  }

  auto iter = config_dict_.find(name);
  if (iter == config_dict_.end()) {
    std::cerr << "Unable to find \"effect\" configuration" << std::endl;
    return false;
  }

  *value = iter->second;
  return true;
}

std::string ConfigReader::GetConfigValue(const std::string & name) const
{
  std::string value;
  if (GetConfigValue(name, &value) == false) {
    std::cerr << "Config value not found" << std::endl;
    return value;
  }

  return value;
}

std::vector<std::string> ConfigReader::GetConfigValueList(const std::string & name) const
{
  std::string value_list;
  if (GetConfigValue(name, &value_list) == false) {
    std::cerr << "Config value list not found" << std::endl;
    return std::vector<std::string>();
  }

  std::vector<std::string> list;
  // Split string around spaces.
  std::istringstream ss(value_list);
  do {
    std::string value;
    ss >> value;
    if (!value.empty())
      list.push_back(value);
  } while (ss);

  return list;
}

