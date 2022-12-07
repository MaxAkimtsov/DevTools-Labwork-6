#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace ArgumentParser {

struct StrNode {
  bool default_use_ = false;
  bool is_multi_mode_ = false;
  bool is_positional_ = false;
  char short_cmd_parameter_ = ' ';
  int multi_count_ = -1;
  std::string long_cmd_parameter_ = " ";
  std::string single_value_;
  std::string default_value_;
  std::string parameter_description_;
  std::string* store_value_linker_ = nullptr;
  std::vector<std::string> multi_value_;
};

struct IntNode {
  bool default_use_ = false;
  bool is_multi_mode_ = false;
  bool is_positional_ = false;
  char short_cmd_parameter_ = ' ';
  int single_value_;
  int multi_count_ = -1;
  int default_value_;
  std::string long_cmd_parameter_ = " ";
  std::string parameter_description_;
  std::vector<int> multi_value_;
  std::vector<int>* store_value_linker_ = nullptr;
};

struct BooleanNode {
  bool default_value_ = true;
  bool default_use_ = false;
  bool single_value_;
  bool is_positional_ = false;
  bool* store_value_linker_ = nullptr;
  char short_cmd_parameter_ = ' ';
  std::string long_cmd_parameter_ = " ";
  std::string parameter_description_;
};

struct HelpNode {
  std::string long_cmd_parameter;
  char short_cmd_parameter_;
  std::string parameter_description_;
  bool is_used = false;
};

class ArgParser {
 private:
  std::string parser_name_;
  std::string last_added_node_type_;
  std::vector<StrNode> string_arguments_;
  std::vector<IntNode> int_arguments_;
  std::vector<BooleanNode> bool_arguments_;
  HelpNode HelpingNode;

 public:
  explicit ArgParser(std::string parser_name);

  bool Parse(std::vector<std::string> splitted_string);

  ArgParser& AddStringArgument(std::string parameter_name);
  ArgParser& AddStringArgument(char cmd_argument, std::string parameter_name);
  ArgParser& AddStringArgument(char cmd_argument, std::string parameter_name, std::string parameter_description);

  ArgParser& AddIntArgument(std::string parameter_name);
  ArgParser& AddIntArgument(std::string parameter_name, std::string parameter_description);
  ArgParser& AddIntArgument(char cmd_argument, std::string parameter_name);

  ArgParser& AddFlag(char cmd_argument, std::string parameter_name);
  ArgParser& AddFlag(char cmd_argument, std::string parameter_name, std::string parameter_description);

  ArgParser& AddHelp(char cmd_argument, std::string parameter_name, std::string parameter_description);

  std::string GetStringValue(const std::string& string_parameter_name);
  int GetIntValue(const std::string& int_parameter_name);
  int GetIntValue(const std::string& int_parameter_name, int index);
  bool GetFlag(const std::string& bool_parameter_name);

  bool Help() const;
  std::string HelpDescription();

  void Default(const char* input_value);
  void Default(bool input_value);

  void StoreValue(std::string& input_value);
  void StoreValues(std::vector<int>& input_values);
  void StoreValue(bool& input_value);

  ArgParser& MultiValue();
  ArgParser& MultiValue(int minimal_arguments_count);

  ArgParser& Positional();
};

}  // namespace ArgumentParser
