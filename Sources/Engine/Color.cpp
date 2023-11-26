//
// Created by cydeamon on 26.11.2023.
//

#include <stdexcept>
#include "Color.h"

namespace Cygine
{
    Color::Color(const std::string &hex)
    {
        SetHex(hex);
    }

    void Color::SetHex(const std::string &hex)
    {
        bool checkAlpha = false;
        bool hashStart = hex[0] == '#';

        // Check length
        if (hashStart)
        {
            if (hex.length() >= 7)
            {
                if (hex.length() == 9)
                    checkAlpha = true;
                else if (hex.length() != 7)
                    throw std::invalid_argument("Invalid hex code (wrong length). Expected 7 or 9. Length: " + std::to_string(hex.length()) + ". Hex: " + hex);
            }
        }
        else
        {
            if (hex.length() != 6)
            {
                if (hex.length() == 8)
                    checkAlpha = true;
                else if (hex.length() != 6)
                    throw std::invalid_argument("Invalid hex code (wrong length). Expected 6 or 8. Length: " + std::to_string(hex.length()) + ". Hex: " + hex);
            }
        }

        // Check characters
        int i = 0;
        for (char c : hex)
        {
            if (hashStart && i == 0 && c == '#')
            {
                i++;
                continue;
            }

            if (!std::isxdigit(c))
                throw std::invalid_argument("Invalid hex code (wrong character). Character: " + std::string(1, c) + ". Hex: " + hex);

            i++;
        }

        // Apply values
        if (hashStart)
        {
            r = std::stoi(hex.substr(1, 2), nullptr, 16);
            g = std::stoi(hex.substr(3, 2), nullptr, 16);
            b = std::stoi(hex.substr(5, 2), nullptr, 16);

            if (checkAlpha)
                a = std::stoi(hex.substr(7, 2), nullptr, 16);
        }
        else
        {
            r = std::stoi(hex.substr(0, 2), nullptr, 16);
            g = std::stoi(hex.substr(2, 2), nullptr, 16);
            b = std::stoi(hex.substr(4, 2), nullptr, 16);

            if (checkAlpha)
                a = std::stoi(hex.substr(6, 2), nullptr, 16);
        }
    }
} // Cygine