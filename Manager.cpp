/*
Name(s): Kevin Nguyen & Robert Posada
Date: 11/20/2015
Project #3
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include "Queue.h"
#include "Queue.cpp"
using namespace std;

const string LOGFILE = "log.txt";

class Record {
private:
    int time;
    string name;
    int games; // number of games they want to play
    int rank; // priority
public:
    // public methods for Record class
    // getters
    int get_time();
    string get_name();
    int get_games();
    int get_rank();
    
    // friends
    friend void read(vector <Record>& records);
    friend void printR(Record rec);
    
    friend class Manager;
};

void printR(Record rec) {
    cout << "Name: " << rec.name << endl << "Rank: ";
    if (rec.rank == 1)
        cout << "Gold";
    else if (rec.rank == 2)
        cout << "Silver";
    else
        cout << "Bronze";
    cout << endl << "Games: " << rec.games << endl;
}

class Manager {
private:
    Qlist<Record> gold, silver, bronze;
    vector <Record> team1, team2;
    int clock, gameStart, gameEnd;
public:
    // constructor
    Manager(Qlist<Record> g, Qlist<Record> s, Qlist<Record> b);
    
    // getters
    int get_clock();
    int get_gametime();
    
    // methods
    void make_teams();
    void write(Record rec, int option);
    void reassign();
    void updateLog(vector<Record>& records);
    void game(int win, vector <Record>& team1, vector <Record>& team2);
};

Manager::Manager(Qlist<Record> g,Qlist<Record> s, Qlist<Record> b) {
    clock = 0;
    gold = g;
    silver = s;
    bronze = b;
}

int Manager::get_clock() {
    return clock;
}

void Manager::make_teams() {
    team1.push_back(gold[0]);
    team2.push_back(gold[1]);
    team1.push_back(gold[2]);
    team2.push_back(silver[0]);
    team2.push_back(silver[1]);
    team1.push_back(bronze[0]);
    
    // Should put code from here down in separate function(s)
    vector<Record> teams = team1;
    teams.insert(teams.end(), team2.begin(), team2.end());
    for (int i = 0; i < teams.size() - 1; i++) {
        for (int j = 1; j < teams.size(); j++) {
            if (teams.at(j).get_time() < teams.at(j - 1).get_time()) {
                Record temp = teams.at(j);
                teams.at(j) = teams.at(j - 1);
                teams.at(j - 1) = temp;
            }
        }
    }
    clock = gameStart = teams.at(teams.size()-1).get_time();
    gameEnd = gameStart + 10;
    for (Record r: teams) {
        write(r, 1);
    }
    for (Record r : team1) {
        write(r, 2);
    }
    for (Record r : team2) {
        write(r, 2);
    }
    for (int i = 0; i < 3; i++) {
        gold.pop_front();
    }
    for (int i = 0; i < 2; i++) {
        silver.pop_front();
    }
    bronze.pop_front();
}

// -- NOTES -- //
//
// Need a function that simulates the requests until the queue is empty - while (!is_empty()) { ... }
// is_empty() should return true if and only if queues g, s, and b are empty
//
// -- NOTES -- //

void Manager::write(Record rec, int option) {
    ofstream outfile;
    string fline;
    outfile.open(LOGFILE, ios::app);
    if (outfile) {
        switch (option) {
            case 1:
                outfile << setfill('0') << setw(2) << rec.get_time() << " -- " << rec.get_name() << " entered queue for " << rec.get_games() << " games" << endl;
                break;
            case 2:
                outfile << setfill('0') << setw(2) << gameStart << " -- " << rec.get_name() << " started game" << endl;
                break;
            case 3:
                outfile << setfill('0') << setw(2) << gameEnd << " -- " << rec.get_name() << " ended game" << endl;
                break;
            case 4:
                outfile << setfill('0') << setw(2) << gameEnd << " -- " << rec.get_name() << " was promoted to rank " << rec.get_rank() << endl;
                break;
            case 5:
                outfile << setfill('0') << setw(2) << gameEnd << " -- " << rec.get_name() << " was demoted to rank " << rec.get_rank() << endl;
                break;
            case 6:
                outfile << setfill('0') << setw(2) << clock << " -- " << rec.get_name() << " left queue" << endl;
        }
    }
    else {
        cout << "ERROR: Cannot open file." << endl;
    }
    outfile.close();
    return;
}

int Record::get_time() {
    return time;
}

string Record::get_name() {
    return name;
}

int Record::get_games() {
    return games;
}

int Record::get_rank() {
    return rank;
}

void read(vector <Record>& records) {
    ifstream infile;
    string filename, fline;
    
    cout << "Name of file: ";
    getline(cin, filename);
    infile.open(filename);
    
    if (infile) { // check if file is "good"/not corrupted
        while (getline(infile, fline)) { // alternative to eof flag.
            Record temp;
            stringstream(fline) >> temp.time >> temp.name >> temp.games >> temp.rank;
            records.push_back(temp);
        }
    }
    else {
        cout << "ERROR: Cannot open file." << endl;
    }
    infile.close();
    return;
}

void assignQ(vector <Record>& records, Qlist<Record>& g, Qlist<Record>& s, Qlist<Record>& b) {
    // assigning players to respective queue
    for (Record r : records) {
        if (r.get_games() > 0) {
            if (r.get_rank() == 1)
                g.push_back(r);
            else if (r.get_rank() == 2)
                s.push_back(r);
            else
                b.push_back(r);
        }
    }
}

void Manager::game(int win, vector <Record>& team1, vector <Record>& team2) {
    for (int i = 0; i < team1.size(); i++) {
        // promotes the winning team's priority unless they're already priority 1
        if (team1.at(i).rank != 1) {
            team1.at(i).rank--;
            team1.at(i).games--;
        }
        
        // demotes the losing team's priority unless they're already priority 3
        if (team2.at(i).rank != 3) {
            team2.at(i).rank++;
            team2.at(i).games--;
        }
    }
    for (Record r: team1) {
        write(r, 4);
    }
    for (Record r: team2) {
        write(r, 5);
    }
}

void Manager::updateLog(vector<Record>& records) {
    for (Record r : records) {
        if (r.time > gameStart && r.time < gameEnd) {
            write(r, 1);
        }
    }
}

void Manager::reassign() {
    for (Record r : team1) {
        write(r, 3);
    }
    for (Record r : team2) {
        write(r, 3);
    }
    
    srand(time(0));
    int winner = rand() % 2;
    if (winner == 0) {
        game(winner, team1, team2);
    }
    else {
        game(winner, team2, team1);
    }
    
    assignQ(team1, gold, silver, bronze);
    assignQ(team2, gold, silver, bronze);
    
    team1.clear();
    team2.clear();
}

int main() {
    vector <Record> records;
    Qlist<Record> gold, silver, bronze;
    read(records);
    assignQ(records, gold, silver, bronze);
    Manager rito = Manager(gold, silver, bronze);
    while (gold.getSize() + silver.getSize() + bronze.getSize() > 5) {
        rito.make_teams();
        rito.updateLog(records);
        rito.reassign();
    }
    
    return 0;
}
