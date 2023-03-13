#pragma once
#include <string>
#include <vector>
#include <map>

std::string dumbfmt(std::vector<std::string> o);
std::string dumbfmt_file(std::string path, std::map<std::string, std::string> dict);
std::string dumbfmt_html_escape(std::string o);