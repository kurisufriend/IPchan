#include <fstream>
#include <istream>
#include <iterator>
#include <regex>
#include "dumbstr.h"
#include <iostream>

std::string dumbfmt(std::vector<std::string> o)
{
    std::string base = "";
    for (auto iter = o.begin(); iter != o.end(); iter++)
        base.append(*iter);
    return base;
}

std::string dumbfmt_file(std::string path, std::map<std::string, std::string> dict)
{
    std::fstream f;
    f.open(path);
    std::string body = {(std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()};
    f.close();
    for (std::pair<std::string, std::string> p : dict)
    {
        body = std::regex_replace(body,
            std::regex(dumbfmt({"\\{\\{\\{", p.first,"\\}\\}\\}"})),
            p.second);
    }
    return body;
}

std::string dumbfmt_html_escape(std::string o)
{
    std::string res = o;
    res = std::regex_replace(res, std::regex("&"), "&amp;");
    res = std::regex_replace(res, std::regex(">"), "&gt;");
    res = std::regex_replace(res, std::regex("<"), "&lt;");
    res = std::regex_replace(res, std::regex("\""), "&quot;");
    res = std::regex_replace(res, std::regex("\'"), "&apos;");
    res = std::regex_replace(res, std::regex("\n"), "<br>");
    res = std::regex_replace(res, std::regex("!"), "<i>!</i>");
    return res;
}