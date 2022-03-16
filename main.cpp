#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <dirent.h>
#include "Cipher.h"

namespace operations {
    void listFiles();
    void openFile(std::string &openedFile);
    void addPassword(std::string openedFile);
    void deletePassword(std::string openedFile);
    void sortPasswords(std::string openedFile);
    void removeCategory(std::string openedFile);
    void findCategory(std::string openedFile);
    void createNewFile();
    void removeFile();
    void displayHelp();
}
namespace login {
    bool verifyAccess();
    void getLastLogin();
    void saveLoginTimestamp(time_t time);
}
namespace comparisonFunctions {
    bool sortByLogins(std::array<std::string, 4> i, std::array<std::string, 4> j) {
        return i[1] < j[1];
    }
    bool sortByPasswords(std::array<std::string, 4> i, std::array<std::string, 4> j) {
        return i[2] < j[2];
    }
}
void splitPass(std::string pass, std::vector<std::array<std::string, 4>> &resultVector);
void displayFullPass(std::vector<std::array<std::string, 4>> fragmentedPasswords);

Cipher cipher;
int main() {

    namespace op = operations;
    namespace log = login;

    bool hasQuit = false;
    bool isLoggedIn = false;
    enum operations
    {
        l,
        o,
        s,
        a,
        d,
        h,
        r,
        c,
        n,
        f,
        q
    };
    cipher.setKey(5);

    while(!isLoggedIn)
        isLoggedIn = log::verifyAccess();

    if(isLoggedIn)
    {

        std::string currentFile;

        time_t timeNow = time(0);
        log::saveLoginTimestamp(timeNow);

        while(!hasQuit)
        {

            char currentOperation;
            std::cout<<std::endl;
            std::cout<<"Type 'h' to see available operations."<<std::endl;
            std::cout<<"Enter next operation: "<<std::endl;
            std::cin>>currentOperation;

            namespace op = operations;
            switch(currentOperation)
            {
                case 'l':
                    op::listFiles();
                    break;
                case 'o':
                    op::openFile(currentFile);
                    break;
                case 'a':
                    op::addPassword(currentFile);
                    break;
                case 'd':
                    op::deletePassword(currentFile);
                    break;
                case 's':
                    op::sortPasswords(currentFile);
                    break;
                case 'h':
                    op::displayHelp();
                    break;
                case 'r':
                    op::removeCategory(currentFile);
                    break;
                case 'c':
                    op::findCategory(currentFile);
                    break;
                case 'n':
                    op::createNewFile();
                    break;
                case 'f':
                    op::removeFile();
                    break;
                case 'q':
                    hasQuit = true;
                    break;
                default:
                    std::cout<<"Operation not recognized, try again.";
                    break;
            }

        }

    }

}

namespace operations
{
    void listFiles()
    {
        using std::endl;
        using std::cout;
        using std::vector;

        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir ("textFiles/")) != nullptr) {

            while ((ent = readdir (dir)) != nullptr)
            {

                if(strcmp(ent->d_name, "logins.txt") != 0 && strcmp(ent->d_name, "master.txt") != 0 &&
                strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
                {
                    cout << ent->d_name << endl;
                }

            }

            closedir (dir);

        }
        else {
            cout<<"Can't open directory";
        }

    }
    void openFile(std::string &openedFile) {
        using std::cout;
        using std::cin;
        using std::endl;

        cout << "Which file to open?" << endl;
        std::string fileToOpen;
        cin >> fileToOpen;
        std::ifstream passwordFile;
        passwordFile.open("textFiles/" + fileToOpen);

        if (passwordFile.is_open()) {
            cout << "NAME " << "LOGIN " << "PASSWORD " << "CATEGORY " << endl;
            cout << "---------------------------" << endl;
            std::string currentLine;
            int onWhichRow = 1;
            while (std::getline(passwordFile, currentLine)) {
                cipher.decrypt(currentLine);
                cout << onWhichRow << ". " << currentLine << endl;
                onWhichRow++;
            }
            openedFile = fileToOpen;
            passwordFile.close();
        } else
            cout << "Can't open " << fileToOpen;
    }
    void addPassword(std::string openedFile) {
        using std::cout;
        using std::cin;
        using std::endl;

        if (!openedFile.empty()) {

            std::ofstream passwordFile;
            passwordFile.open("textFiles/" + openedFile, std::ios::app);

            if (passwordFile.is_open()) {

                std::string newCredentials;
                std::cout << "Enter your new credentials: " << endl;
                cin.ignore();
                std::getline(cin, newCredentials);
                cipher.encrypt(newCredentials);
                passwordFile << endl;
                passwordFile << newCredentials;
                passwordFile.close();

            } else
                cout << "Can't open " << openedFile;
        } else
            cout << "Open a file first";

    }
    void deletePassword(std::string openedFile) {
        using std::cout;
        using std::cin;
        using std::endl;

        if (!openedFile.empty()) {

            std::ifstream passwordFile;
            passwordFile.open("textFiles/" + openedFile);

            if (passwordFile.is_open()) {

                int whichRowToDelete;
                cout << "Which row do you want to remove?" << endl;
                cin >> whichRowToDelete;

                int onWhichRow = 1;

                std::string currentLine;
                std::ofstream tempFile;
                tempFile.open("textFiles/temp.txt", std::fstream::out);

                while (std::getline(passwordFile, currentLine)) {

                    if (onWhichRow != whichRowToDelete) {
                        tempFile << currentLine<<endl;
                    }

                    onWhichRow++;

                }

                passwordFile.close();
                tempFile.close();

                std::remove(("textFiles/" + openedFile).c_str());
                std::rename("textFiles/temp.txt", ("textFiles/" + openedFile).c_str());

            } else
                cout << "Can't open " << openedFile;

        } else
            cout << "Open a file first";

    }
    void sortPasswords(std::string openedFile) {
        using std::cout;
        using std::cin;
        using std::endl;
        using std::sort;
        namespace comp = comparisonFunctions;

        if (!openedFile.empty()) {

            std::ifstream passwordFile;
            passwordFile.open("textFiles/" + openedFile, std::ios::app);

            if (passwordFile.is_open()) {

                std::string currentLine;
                std::vector<std::array<std::string, 4>> fragmentedPasswords;

                while (std::getline(passwordFile, currentLine)) {

                    cipher.decrypt(currentLine);
                    splitPass(currentLine, fragmentedPasswords);

                }

                char sortAnswer;
                cout << "What do you want to sort by?" << endl << "'n' for names, 'l' for logins, 'p' for passwords."
                     << endl;
                cin >> sortAnswer;

                switch (sortAnswer) {
                    case 'n':
                        sort(fragmentedPasswords.begin(), fragmentedPasswords.end());
                        displayFullPass(fragmentedPasswords);
                        break;
                    case 'l':
                        sort(fragmentedPasswords.begin(), fragmentedPasswords.end(), comp::sortByLogins);
                        displayFullPass(fragmentedPasswords);
                        break;
                    case 'p':
                        sort(fragmentedPasswords.begin(), fragmentedPasswords.end(), comp::sortByPasswords);
                        displayFullPass(fragmentedPasswords);
                        break;
                    default:
                        cout << "Wrong instruction, try again.";
                        break;
                }

            } else
                cout << "Can't open " << openedFile;
        } else
            cout << "Open a file first";

    }
    void removeCategory(std::string openedFile) {

        using std::cout;
        using std::cin;
        using std::endl;

        if (!openedFile.empty()) {

            std::ifstream passwordFile;
            passwordFile.open("textFiles/" + openedFile);

            if (passwordFile.is_open()) {

                std::string whichCategoryToDelete;
                cout << "Which category do you want to remove?" << endl
                     << "This will also remove all passwords belonging to it." << endl;
                cin >> whichCategoryToDelete;

                std::vector<std::array<std::string, 4>> fragmentedPasswords;
                std::string currentLine;
                std::ofstream tempFile;

                tempFile.open("textFiles/temp.txt", std::fstream::out);

                while (std::getline(passwordFile, currentLine)) {

                    cipher.decrypt(currentLine);
                    splitPass(currentLine, fragmentedPasswords);

                }

                for (int i = 0; i < fragmentedPasswords.size(); i++) {

                    if (fragmentedPasswords[i][3] != whichCategoryToDelete) {

                        for (int j = 0; j < 4; j++) {

                            cipher.encrypt(fragmentedPasswords[i][j]);
                            tempFile << fragmentedPasswords[i][j] << " ";

                        }

                        tempFile << endl;

                    }

                }

                passwordFile.close();
                tempFile.close();

                std::remove(("textFiles/" + openedFile).c_str());
                std::rename("textFiles/temp.txt", ("textFiles/" + openedFile).c_str());

            } else
                cout << "Can't open " << openedFile;

        } else
            cout << "Open a file first";

    }
    void findCategory(std::string openedFile) {
        using std::cout;
        using std::cin;
        using std::endl;

        if (!openedFile.empty()) {

            std::ifstream passwordFile;
            passwordFile.open("textFiles/" + openedFile);

            if (passwordFile.is_open()) {

                std::string categoryToFind;
                cout << "Which category do you want to search for?" << endl;
                cin >> categoryToFind;

                std::vector<std::array<std::string, 4>> fragmentedPasswords;
                std::string currentLine;

                while (std::getline(passwordFile, currentLine)) {

                    cipher.decrypt(currentLine);
                    splitPass(currentLine, fragmentedPasswords);

                }

                for (int i = 0; i < fragmentedPasswords.size(); i++) {

                    if (fragmentedPasswords[i][3] == categoryToFind) {

                        for (int j = 0; j < 4; j++) {

                            cout << fragmentedPasswords[i][j] << " ";

                        }

                        cout << endl;

                    }

                }

            } else
                cout << "Can't open " << openedFile;

        } else
            cout << "Open a file first";

    }
    void createNewFile()
    {

        using std::cout;
        using std::cin;
        using std::endl;

        std::ofstream newFile;
        std::string fileName;

        cout<<"What do you want to name your file?"<<endl;
        cin>>fileName;

        newFile.open("textFiles/"+fileName);

        if(newFile.is_open())
            cout<<fileName<<" successfully created.";
        else
            cout<<"Could not create "<<fileName;

    }
    void removeFile()
    {
        using std::cout;
        using std::cin;
        using std::endl;

        std::string fileName;
        char confirmation;

        cout<<"Which file do you want to remove?"<<endl;
        cin>>fileName;

        cout<<"Are you sure? 'y' - Yes, 'n' - No"<<endl;
        cin>>confirmation;

        if(confirmation == 'y')
        {

            std::remove(("textFiles/" + fileName).c_str());
            cout<<fileName<<" successfully removed"<<endl;

        }



    }
    void displayHelp() {
        using std::cout;
        using std::endl;

        cout << endl << "Available operations: " << endl << endl;
        cout << "l - list all files containing passwords." << endl;
        cout << "o - open a file containing passwords." << endl;
        cout << "a - add a new password to a file." << endl;
        cout << "d - delete a single password from a file." << endl;
        cout << "s - sort passwords in a file." << endl;
        cout << "r - remove a whole category of passwords." << endl;
        cout << "c - search by category." << endl;
        cout << "n - create new file." << endl;
        cout << "f - remove a file." << endl;
        cout << "h - get help regarding operations." << endl;
        cout << "q - quit the program." << endl;

    }
}
namespace login {
    void getLastLogin() {

        std::ifstream loginsIn;

        loginsIn.open("textFiles/logins.txt");

        if (loginsIn.is_open()) {
            std::string lastSession;
            while (std::getline(loginsIn, lastSession)) {
                std::cout << "Last Session: " << lastSession << std::endl;
            }

        } else
            std::cout << "Can't open logins.txt";


    }
    void saveLoginTimestamp(time_t time) {
        using std::cout;
        using std::endl;

        std::ofstream loginsOut;
        loginsOut.open("textFiles/logins.txt", std::ofstream::out | std::ofstream::trunc);

        if (loginsOut.is_open()) {

            char *dt = std::ctime(&time);
            loginsOut << dt;

        } else
            cout << "Can't open logins.txt";

        loginsOut.close();

    }
    bool verifyAccess() {
        using std::ifstream;
        using std::cout;
        using std::endl;

        ifstream masterFile;
        masterFile.open("textFiles/master.txt");
        if (masterFile.is_open()) {

            std::string user_password;
            std::cout << "Enter Password: " << std::endl;
            std::cin >> user_password;

            std::string masterPass;
            masterFile >> masterPass;
            cipher.decrypt(masterPass);

            if (user_password == masterPass) {
                cout << "Password correct. Files may now be accessed." << endl;
                login::getLastLogin();
                masterFile.close();
                return true;
            } else {
                cout << "Password incorrect. Access denied." << endl;
                return false;
            }

        } else
            cout << "Could not access the master file";

        return false;

    }
}
void splitPass(std::string pass, std::vector<std::array<std::string, 4>> &resultVector)
{
    using std::string;
    using std::vector;
    using std::array;

    array<string, 4> fragmentedPass;
    string temp;
    int tempInt = 0;

    std::stringstream streamIn(pass);
    while(streamIn.good() && tempInt < 4)
    {
        streamIn >> fragmentedPass[tempInt];
        ++tempInt;
    }

    resultVector.push_back(fragmentedPass);

}
void displayFullPass(std::vector<std::array<std::string, 4>> fragmentedPasswords)
{
    using std::cout;
    using std::endl;

    for(int i = 0; i < fragmentedPasswords.size(); i++)
    {
        for (int j = 0; j < 4; j++)
            cout<<fragmentedPasswords[i][j]<<" ";
        cout<<endl;
    }

}