#include "DatabaseManager.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

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
    ifstream file("progress.txt");
    
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    // 2. Read the file line by line
    while (getline(file, line)) {
        
        // Find where the comma is
        stringstream ss(line);
        string savedUser, savedPass;

        getline(ss, savedUser, ',');
        getline(ss, savedPass, ',');

        if (savedUser == username && savedPass == password){
            file.close();
            return true; // Login success!
        }
        
    }
    
    file.close();
    return false; // Wrong username or password
}


void DatabaseManager::saveProgress(string username, int level, int lives, int gems, int score) {
    // 1. Open "progress.txt" in append mode
    ofstream file("progress.txt", ios::app);
    // 2. Check if the file is open
    if(!file.is_open()){
        return;
    }
    
    // 3. Write the data separated by commas:
    // Format: username,level,lives,gems,score
    file<< username <<"," <<level << "," << lives << "," << gems << "," << score << endl;
    // 4. Close the file
    file.close();
}

void DatabaseManager::LoadUser(string username, int &level, int &lives, int &gems, int &score){
    
    ifstream file("progress.txt");

    if (!file.is_open()) {
        return;
    }

    string line;

    while (getline(file, line)) {
        stringstream ss(line); // Turn the line into a stream
        string savedUser;
        
        // Read the stream up to the first comma, save it in savedUser
        getline(ss, savedUser, ',');
        // If this is the user we are looking for, extract the rest of their stats!
        if (savedUser == username) {
            string temp;
            
            // Read until next comma into 'temp', then convert to integer
            getline(ss, temp, ','); 
            level = stoi(temp); // stoi = String TO Integer
            
            getline(ss, temp, ','); 
            lives = stoi(temp);
            
            getline(ss, temp, ','); 
            gems = stoi(temp);
            
            getline(ss, temp, ','); 
            score = stoi(temp);
        }
    }
}