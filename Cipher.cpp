#include "Cipher.h"

using std::string;

void Cipher::setKey(int key)
{
    this->key = key;
}
string Cipher::encrypt(string &msg)
{

    for(char& c : msg)
    {

        if(c >= 'a' && c <= 'z')
        {

            c += key;
            if(c > 'z')
                c = c - 'z' + 'a' - 1;

        }
        else if(c >= 'A' && c <= 'Z')
        {

            c += key;
            if(c > 'Z')
                c = c - 'Z' + 'A' - 1;

        }

    }

    return msg;

}
string Cipher::decrypt(string &msg)
{

    for(char& c : msg)
    {

        if(c >= 'a' && c <= 'z')
        {
            c -= key;
            if(c < 'a')
                c = c + 'z' - 'a' + 1;
        }
        else if(c >= 'A' && c <= 'Z')
        {

            c -= key;
            if (c < 'A')
                c = c + 'Z' - 'A' + 1;

        }

    }

    return msg;

}