/*
Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/
#pragma once

#include <string>
#include <unordered_map>

using config_dict = std::unordered_map<std::string, std::string>;

/**
 Parses and provides access to configuration values. The format of the file expected is:
 # Comments start with #. No comments allowed on lines having name/value
 name value
*/
class ConfigReader {
 public:
   // Parses the config file.
   bool Load(const std::string& config_filename);
   // Returns true if config value is available
   bool IsConfigValueAvailable(const std::string& name) const;
   // Get value associated with name
   bool GetConfigValue(const std::string& name, std::string* value) const;
   // Get value associated with name. Returns empty string if value not found
   std::string GetConfigValue(const std::string& name) const;
   // Get vector of values associated with name. Returns empty vector if no value is found
   std::vector<std::string> GetConfigValueList(const std::string& name) const;
 private:
   // true if config file is loaded
   bool loaded_ = false;
   // Internal config data store
   config_dict config_dict_;
};