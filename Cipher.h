#pragma once

#include <iostream>

class Cipher
{
    int key;
public:
    void setKey(int key);
    std::string encrypt(std::string& msg);
    std::string decrypt(std::string& msg);
};