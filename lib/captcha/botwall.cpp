#include "./botwall.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"
#include <cstdlib>
#include <iostream>

std::vector<std::string> opts = {"SQUARE", "CIRCLE", "TRIANGLE", "DIAMOND"};

std::pair<std::string, std::string> botwall::generate_captcha()
{
    int x = 200;
    int y = 40;
    int n = 1;

    std::vector<std::string> challenge;
    for (int i = 0; i < ((int)(x/20))-2; i++)
        challenge.push_back(opts[rand() % 4]);
    std::string challenge_str;
    for (auto it = challenge.begin(); it != challenge.end(); it++)
        challenge_str.append(*it).append(" ");
    std::cout << challenge_str << std::endl;

    std::vector<std::vector<unsigned char>> proto;
    for (int yi = 0; yi < y; yi++)
    {
        proto.push_back(std::vector<unsigned char>());
        for (int xi = 0; xi < x; xi++)
            proto[yi].push_back(0);
    }

    int mod = rand() % 6;
    mod *= (mod%2) ? 1 : -1;
    for (int xi = 20; xi < x-20; xi += 20)
    {
        int idx = (xi-30)/20;
        int oldxi = xi;
        mod = rand() % 6;
        mod *= (mod%2) ? 1 : -1;
        xi += mod;
        std::string chal = challenge[idx];
        std::cout << chal << std::endl; 

        int basey = (y/2);
        basey += mod;

        if (chal == "DIAMOND")
        {
            for (int deg = 0; deg < 360; deg += 2)
            {
                int r = 10;
                int xt = r*std::pow(cos(deg), 3);
                int yt = r*std::pow(sin(deg), 3);
                proto[yt + basey][xt + xi] = 255;
            }
        }
        else if (chal == "CIRCLE")
        {
            for (int deg = 0; deg < 360; deg += 2)
            {
                int r = 10;
                int xt = r*cos(deg);
                int yt = r*sin(deg);
                proto[yt + basey][xt + xi] = 255;
            }
        }
        else if (chal == "TRIANGLE")
        {
            for (int deg = 0; deg < 360; deg += 2)
            {
                int r = 10;
                int xt = -1 * r*std::pow(cos(deg), 3);
                int yt = -1 * r*std::pow(sin(deg), 2);
                proto[yt + basey][xt + xi] = 255;
            }
            for (int bar = 0; bar < 10; bar++)
                proto[basey][xi - 5 + bar] = 255;
        }
        else if (chal == "SQUARE")
        {
            for (int bar = 0; bar < 10; bar++)
                proto[basey-5][xi - 5 + bar] = 255;
            for (int bar = 0; bar < 10; bar++)
                proto[basey+5][xi - 5 + bar] = 255;
            for (int bar = 0; bar < 10; bar++)
                proto[basey-5 + bar][xi - 5] = 255;
            for (int bar = 0; bar < 10; bar++)
                proto[basey-5 + bar][xi + 5] = 255;
        }
        xi = oldxi;
    }

    for (int i = 0; i < 100; i++)
        proto[rand()%y][rand()%x] = 255;

    std::vector<unsigned char> bytes;
    for (int yi = 0; yi < y; yi++)
    {
        for (int xi = 0; xi < x; xi++)
            bytes.push_back(proto[yi][xi]);
    }

    stbi_write_bmp("static/imgs/aids.bmp", x, y, n, bytes.data());
    
    return std::pair<std::string, std::string>(challenge_str, "static/imgs/aids.bmp");
}