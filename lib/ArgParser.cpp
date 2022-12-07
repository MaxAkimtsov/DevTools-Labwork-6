#include "ArgParser.h"

#include <utility>



ArgumentParser::ArgParser::ArgParser(std::string parser_name) {
  parser_name_ = std::move(parser_name);
}

bool ArgumentParser::ArgParser::Parse(std::vector<std::string> splitted_string) {
  if (splitted_string.empty()) {
    return false;
  } else {
    if (splitted_string.size() == 1 && string_arguments_.empty() && int_arguments_.empty() && bool_arguments_.empty()) {
      return true;
    } else if (splitted_string.size() == 2 && (splitted_string[1] == "--help" || splitted_string[1] == "-h")) {
      return true;
    }
    for (int i = 1; i < splitted_string.size(); i++) {
      if (splitted_string[i].find("--") != std::string::npos && splitted_string[i].find('=') != std::string::npos) {
        std::string argument = splitted_string[i].substr(2, splitted_string[i].find('=') - 2);
        std::string value = splitted_string[i].substr(splitted_string[i].find('=') + 1, splitted_string[i].length() - splitted_string[i].find('=') - 1);

        if (!string_arguments_.empty()) {
          for (StrNode& Node : string_arguments_) {
            if (Node.long_cmd_parameter_ == argument) {
              if (Node.is_multi_mode_) {
                Node.multi_value_.push_back(value);
              } else {
                Node.single_value_ = value;
                if (Node.store_value_linker_ != nullptr) {
                  *Node.store_value_linker_ = Node.single_value_;
                }
              }
            }
          }
        }

        if (!int_arguments_.empty()) {
          for (IntNode& Node : int_arguments_) {
            if (Node.long_cmd_parameter_ == argument) {
              if (Node.is_multi_mode_) {
                Node.multi_value_.push_back(std::stoi(value));
                if (Node.store_value_linker_ != nullptr) {
                  *Node.store_value_linker_ = Node.multi_value_;
                }
              } else {
                Node.single_value_ = std::stoi(value);
              }
            }
          }
        }
      } else if (splitted_string[i].find('-') != std::string::npos
          && splitted_string[i].find('=') != std::string::npos) {
        char short_argument = splitted_string[i].c_str()[1];
        std::string value = splitted_string[i].substr(splitted_string[i].find('=') + 1, splitted_string[i].length() - splitted_string[i].find('=') - 1);

        if (!string_arguments_.empty()) {
          for (StrNode& Node : string_arguments_) {
            if (Node.short_cmd_parameter_ == short_argument) {
              if (Node.is_multi_mode_) {
                Node.multi_value_.push_back(value);
              } else {
                Node.single_value_ = value;
              }
            }
          }
        }
        if (!int_arguments_.empty()) {
          for (IntNode& Node : int_arguments_) {
            if (Node.short_cmd_parameter_ == short_argument) {
              if (Node.is_multi_mode_) {
                Node.multi_value_.push_back(std::stoi(value));
              } else {
                Node.single_value_ = std::stoi(value);
              }
            }
          }
        }
      } else if (splitted_string[i].find("--") != std::string::npos
          && splitted_string[i].find('=') == std::string::npos) {
        std::string flag_name = splitted_string[i].substr(2, splitted_string[i].length() - 2);

        if (!bool_arguments_.empty()) {
          for (BooleanNode& Node : bool_arguments_) {
            if (Node.long_cmd_parameter_ == flag_name) {
              Node.single_value_ = Node.default_value_;
            }
          }
        }
      } else if (splitted_string[i].find('-') != std::string::npos
          && splitted_string[i].find('=') == std::string::npos) {
        char* cstr = splitted_string[i].data();

        for (int j = 0; j < splitted_string[i].size(); j++) {
          for (BooleanNode& Node : bool_arguments_) {
            if (Node.short_cmd_parameter_ == cstr[j]) {
              Node.single_value_ = true;
              if (Node.store_value_linker_ != nullptr) {
                *Node.store_value_linker_ = Node.single_value_;
              }
            }
          }
        }

      } else if (splitted_string[i].find('-') == std::string::npos && splitted_string[i].find('=') == std::string::npos) {
        std::string value = splitted_string[i];
        int_arguments_[int_arguments_.size() - 1].multi_value_.push_back(std::stoi(splitted_string[i]));
      }
    }
    // NoDefault checking

    for (const IntNode& LinkerNode : int_arguments_) {
      if (LinkerNode.store_value_linker_ != nullptr) {
        *LinkerNode.store_value_linker_ = LinkerNode.multi_value_;
      }
    }

    for (const StrNode& Defaulter : string_arguments_) {
      if (Defaulter.single_value_.empty()) {
        return false;
      }
    }

    for (const StrNode& MultiStrChecker : string_arguments_) {
      if (MultiStrChecker.is_multi_mode_) {
        if (MultiStrChecker.multi_count_ > MultiStrChecker.multi_value_.size() && MultiStrChecker.multi_count_ != -1) {
          return false;
        }
      }
    }

    for (const IntNode& MultiIntChecker : int_arguments_) {
      if (MultiIntChecker.is_multi_mode_) {
        if (MultiIntChecker.multi_count_ > MultiIntChecker.multi_value_.size() && MultiIntChecker.multi_count_ != -1) {
          return false;
        }
      }
    }

    return true;
  }
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddStringArgument(std::string parameter_name) {
  StrNode newNode;

  newNode.long_cmd_parameter_ = std::move(parameter_name);

  string_arguments_.push_back(newNode);
  last_added_node_type_ = "str";

  return *this;
}

ArgumentParser::ArgParser&
ArgumentParser::ArgParser::AddStringArgument(char cmd_argument, std::string parameter_name) {
  StrNode newNode;

  newNode.long_cmd_parameter_ = std::move(parameter_name);
  newNode.short_cmd_parameter_ = cmd_argument;

  string_arguments_.push_back(newNode);
  last_added_node_type_ = "str";

  return *this;
}

ArgumentParser::ArgParser&
ArgumentParser::ArgParser::AddStringArgument(char cmd_argument, std::string parameter_name,
                                             std::string parameter_description) {
  StrNode newNode;

  newNode.long_cmd_parameter_ = std::move(parameter_name);
  newNode.short_cmd_parameter_ = cmd_argument;
  newNode.parameter_description_ = std::move(parameter_description);

  string_arguments_.push_back(newNode);
  last_added_node_type_ = "str";

  return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddIntArgument(std::string parameter_name) {
  IntNode newNode;

  newNode.long_cmd_parameter_ = std::move(parameter_name);

  int_arguments_.push_back(newNode);
  last_added_node_type_ = "int";

  return *this;
}

ArgumentParser::ArgParser&
ArgumentParser::ArgParser::AddIntArgument(std::string parameter_name, std::string parameter_description) {
  IntNode newNode;

  newNode.long_cmd_parameter_ = std::move(parameter_name);
  newNode.parameter_description_ = std::move(parameter_description);

  int_arguments_.push_back(newNode);
  last_added_node_type_ = "int";

  return *this;
}

ArgumentParser::ArgParser&
ArgumentParser::ArgParser::AddIntArgument(char cmd_argument, std::string parameter_name) {
  IntNode newNode;

  newNode.short_cmd_parameter_ = cmd_argument;
  newNode.long_cmd_parameter_ = std::move(parameter_name);

  int_arguments_.push_back(newNode);
  last_added_node_type_ = "int";

  return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddFlag(char cmd_argument, std::string parameter_name) {
  BooleanNode newNode;

  newNode.short_cmd_parameter_ = cmd_argument;
  newNode.long_cmd_parameter_ = std::move(parameter_name);

  bool_arguments_.push_back(newNode);
  last_added_node_type_ = "flag";

  return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddFlag(char cmd_argument,
                                                              std::string parameter_name,
                                                              std::string parameter_description) {
  BooleanNode newNode;

  newNode.short_cmd_parameter_ = cmd_argument;
  newNode.long_cmd_parameter_ = std::move(parameter_name);
  newNode.parameter_description_ = std::move(parameter_description);

  bool_arguments_.push_back(newNode);
  last_added_node_type_ = "flag";

  return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::AddHelp(char cmd_argument,
                                                              std::string parameter_name,
                                                              std::string parameter_description) {
  HelpingNode.short_cmd_parameter_ = cmd_argument;
  HelpingNode.long_cmd_parameter = std::move(parameter_name);
  HelpingNode.parameter_description_ = std::move(parameter_description);
  HelpingNode.is_used = true;

  return *this;
}

std::string ArgumentParser::ArgParser::GetStringValue(const std::string& string_parameter_name) {
  for (const StrNode& newNode : string_arguments_) {
    if (newNode.long_cmd_parameter_ == string_parameter_name) {
      return newNode.single_value_;
    }
  }
  std::cout << "ParameterError: No such parameter in command line";
  exit(1);
}

int ArgumentParser::ArgParser::GetIntValue(const std::string& int_parameter_name) {
  for (const IntNode& newNode : int_arguments_) {
    if (newNode.long_cmd_parameter_ == int_parameter_name) {
      return newNode.single_value_;
    }
  }
  std::cout << "ParameterError: No such parameter in command line";
  exit(1);
}

int ArgumentParser::ArgParser::GetIntValue(const std::string& int_parameter_name, int index) {
  for (IntNode newNode : int_arguments_) {
    if (newNode.long_cmd_parameter_ == int_parameter_name) {
      return newNode.multi_value_[index];
    }
  }
  std::cout << "ParameterError: No such parameter in command line";
  exit(1);
}

bool ArgumentParser::ArgParser::GetFlag(const std::string& bool_parameter_name) {
  for (const BooleanNode& newNode : bool_arguments_) {
    if (newNode.long_cmd_parameter_ == bool_parameter_name) {
      return newNode.single_value_;
    }
  }
  std::cout << "ParameterError: No such parameter in command line";
  exit(1);
}

bool ArgumentParser::ArgParser::Help() const {
  return HelpingNode.is_used;
}

void ArgumentParser::ArgParser::Default(const char * input_value) {
  string_arguments_[string_arguments_.size() - 1].single_value_ = input_value;
  string_arguments_[string_arguments_.size() - 1].default_use_ = true;
}

void ArgumentParser::ArgParser::Default(bool input_value) {
  bool_arguments_[bool_arguments_.size() - 1].single_value_ = input_value;
  bool_arguments_[bool_arguments_.size() - 1].default_use_ = true;
}

void ArgumentParser::ArgParser::StoreValue(std::string& input_value) {
  string_arguments_[string_arguments_.size() - 1].store_value_linker_ = &input_value;
}

void ArgumentParser::ArgParser::StoreValues(std::vector<int>& input_values) {
  int_arguments_[int_arguments_.size() - 1].store_value_linker_ = &input_values;
}

void ArgumentParser::ArgParser::StoreValue(bool& input_value) {
  bool_arguments_[bool_arguments_.size() - 1].store_value_linker_ = &input_value;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::MultiValue() {
  if (last_added_node_type_ == "str") {
    string_arguments_[string_arguments_.size() - 1].is_multi_mode_ = true;
  } else if (last_added_node_type_ == "int") {
    int_arguments_[int_arguments_.size() - 1].is_multi_mode_ = true;
  }

  return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::MultiValue(int minimal_arguments_count) {
  if (last_added_node_type_ == "str") {
    string_arguments_[string_arguments_.size() - 1].is_multi_mode_ = true;
    string_arguments_[string_arguments_.size() - 1].multi_count_ = minimal_arguments_count;
  } else if (last_added_node_type_ == "int") {
    int_arguments_[int_arguments_.size() - 1].is_multi_mode_ = true;
    int_arguments_[int_arguments_.size() - 1].multi_count_ = minimal_arguments_count;
  }
  return *this;
}

ArgumentParser::ArgParser& ArgumentParser::ArgParser::Positional() {
  if (last_added_node_type_ == "str") {
    string_arguments_[string_arguments_.size() - 1].is_positional_ = true;
  } else if (last_added_node_type_ == "int") {
    int_arguments_[int_arguments_.size() - 1].is_positional_ = true;
  } else if (last_added_node_type_ == "bool") {
    bool_arguments_[bool_arguments_.size() - 1].is_positional_ = true;
  }
  return *this;
}

std::string ArgumentParser::ArgParser::HelpDescription() {
  std::string result;

  std::string program_description = "La-la-la";

  result += parser_name_ + '\n';
  result += program_description + "\n\n";

  for (const IntNode& Integer : int_arguments_) {
    result += '-' + Integer.short_cmd_parameter_ + '\t' + "--" + Integer.long_cmd_parameter_ + '\t' + Integer.parameter_description_ + '\n';
  }
  for (const StrNode& String : string_arguments_) {
    result += '-' + String.short_cmd_parameter_ + '\t' + "--" + String.long_cmd_parameter_ + '\t' + String.parameter_description_ + '\n';
  }
  for (const BooleanNode& Boolean : bool_arguments_) {
    result += '-' + Boolean.short_cmd_parameter_ + '\t' + "--" + Boolean.long_cmd_parameter_ + '\t' + Boolean.parameter_description_ + '\n';
  }
  result += '\n' + '-' + HelpingNode.short_cmd_parameter_ + '\t' + "--" + HelpingNode.long_cmd_parameter + '\t' + HelpingNode.parameter_description_ + '\n';
}
