#pragma once
#include <string>

using namespace std;



class DatabaseManager {
public:
    bool registerUser(string username, string password);
    bool verifyLogin(string username, string password);
    void LoadUser(string username, int &level, int &lives, int &gems, int &score);
    void saveProgress(string username, int level, int lives, int gems, int score);
    void saveScoreToLeaderBoard(string username, int score);
    void LoadLeaderBoard(string filename);
    string hashPassword(string password);
};
