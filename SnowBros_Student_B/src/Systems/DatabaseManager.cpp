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
    file << username << "," << hashPassword(password) << endl;
    
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
    password = hashPassword(password);
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
    // Read all existing entries
    ifstream fileIn("progress.txt");
    string lines[100];
    int lineCount = 0;
    bool found = false;

    if (fileIn.is_open()) {
        string line;
        while (getline(fileIn, line) && lineCount < 100) {
            stringstream ss(line);
            string savedUser;
            getline(ss, savedUser, ',');

            if (savedUser == username) {
                // Replace this user's entry with updated stats
                lines[lineCount] = username + "," + to_string(level) + "," +
                                   to_string(lives) + "," + to_string(gems) + "," +
                                   to_string(score);
                found = true;
            } else {
                lines[lineCount] = line;
            }
            lineCount++;
        }
        fileIn.close();
    }

    // If user wasn't found, add them as a new entry
    if (!found) {
        lines[lineCount++] = username + "," + to_string(level) + "," +
                             to_string(lives) + "," + to_string(gems) + "," +
                             to_string(score);
    }

    // Write everything back (overwrite mode)
    ofstream fileOut("progress.txt");
    if (fileOut.is_open()) {
        for (int i = 0; i < lineCount; i++) {
            fileOut << lines[i] << "\n";
        }
        fileOut.close();
    }
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

void DatabaseManager::saveScoreToLeaderBoard(string username, int score){
    
    ofstream file("leaderboard.txt",ios::app);

    if(!file.is_open()){
        return;
    }

    file << username << "," << score << endl;
    
    file.close();
    
}

void DatabaseManager::LoadLeaderBoard(string filename){
    
}

string DatabaseManager::hashPassword(string password){
    
    unsigned long hash = 6769;

    for (char c : password){
        hash = hash*33 + c;
    }
    return to_string(hash);
}