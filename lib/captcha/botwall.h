#pragma once
#include <vector>
#include <string>

namespace botwall
{
    //returns answer || challenge filename
    std::pair<std::string, std::string> generate_captcha();
};