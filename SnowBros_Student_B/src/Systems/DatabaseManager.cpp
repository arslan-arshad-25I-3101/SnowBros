#include "DatabaseManager.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;


bool DatabaseManager::registerUser(string username, string password) {
    // 1. Open the file in "append" mode (app) so
    //  we don't delete old users
    ofstream file("users.txt", ios::app);
    
    // 2. Check if the file opened successfully
    if (!file.is_open()) {
        return false;
    }
    
    // 3. Write the user to the file (Format: username,password)
    file << username << "," << password << endl;
    
    file.close();

    return true;
}

bool DatabaseManager::verifyLogin(string username, string password) {
    // 1. Open the file in "read" mode
    ifstream file("users.txt");
    
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    // 2. Read the file line by line
    while (getline(file, line)) {
        
        // Find where the comma is
        size_t commaPos = line.find(',');
        
        if (commaPos != string::npos) {
            // Split the line into two pieces: user and pass
            string savedUser = line.substr(0, commaPos);
            string savedPass = line.substr(commaPos + 1);
            
            // 3. Check if they match what the player typed!
            if (savedUser == username && savedPass == password) {
                file.close();
                return true; // Login success!
            }
        }
    }
    
    file.close();
    return false; // Wrong username or password
}
