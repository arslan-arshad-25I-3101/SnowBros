#pragma once
#include <string>

using namespace std;



class DatabaseManager {
public:
    bool registerUser(string username, string password);
    bool verifyLogin(string username, string password);
};

