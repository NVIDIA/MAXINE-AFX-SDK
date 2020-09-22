/*###############################################################################
#
# Copyright 2020 NVIDIA Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###############################################################################*/
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