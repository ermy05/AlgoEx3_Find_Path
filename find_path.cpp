#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "headers/Station.h"

using namespace std;

using StationMap = std::unordered_map<std::string, Station*>;

void addTransitEdge(StationMap& stationMap,
    const std::string& nameA,
    const std::string& nameB,
    int weight,
    const std::string& lineName);

bool loadNetworkFromFile(const std::string& filename, StationMap& stationMap);

void freeMemory(StationMap& stationMap);

int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cout << "Missing arguments" << std::endl;
        return 0;
    }

    std::string dataFolder = DATA_DIR;
	std::string filePath = dataFolder + argv[1];
    
    std::string start = argv[2];
    std::string finish = argv[3];

    StationMap stationMap;

    if (!loadNetworkFromFile(filePath, stationMap)) {
        std::cout << "Could not open file" << std::endl;
        return 0;
    }

    if (stationMap.count(start)) {
        Station* karls = stationMap[start];
        karls->printConnectedStations();
    }
    else {
        std::cout << "Could not find start" << std::endl;
    }

    freeMemory(stationMap);

    return 0;
}

void addTransitEdge(StationMap& stationMap,
    const std::string& nameA,
    const std::string& nameB,
    int weight,
    const std::string& lineName) {

    Station* stationA = nullptr;
    Station* stationB = nullptr;

    //check if station exists
    //create new if doesnt exist
    //repeat for both stations
    if (stationMap.count(nameA)) {
        stationA = stationMap[nameA];
    }
    else {
        stationA = new Station(nameA);
        stationMap[nameA] = stationA;
    }

    if (stationMap.count(nameB)) {
        stationB = stationMap[nameB];
    }
    else {
        stationB = new Station(nameB);
        stationMap[nameB] = stationB;
    }

    Connection* newConn = new Connection{ stationA, stationB, weight, lineName};

    stationA->addConnection(newConn);
    stationB->addConnection(newConn);
}

bool loadNetworkFromFile(const std::string& filename, StationMap& stationMap) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string fileRow;
    while (std::getline(file, fileRow)) {
        if (fileRow.empty()) continue;

        std::stringstream ss(fileRow);
        std::string lineName;

        std::getline(ss, lineName, ':');

        std::string currentStationName = "";
        std::string nextStationName = "";
        int weight = 0;

        //save station between "" in currentStaionName
        ss.ignore(256, '"');
        if (!std::getline(ss, currentStationName, '"')) continue;

        //check if a number follows and save it in weight
        while (ss >> weight) {
            //save following station between "" in nextStationName
            ss.ignore(256, '"');
            if (!std::getline(ss, nextStationName, '"')) {
                break;
            }

            addTransitEdge(stationMap, currentStationName, nextStationName, weight, lineName);
            currentStationName = nextStationName;
            //repeates untill the is no number/weight to read
        }
    }

    file.close();
    return true;
}

void freeMemory(StationMap& transitMap){
    // track all connections by going through the stations
    std::vector<Connection*> connectionsToDelete;

    for (auto const& [name, stationPtr] : transitMap) {
        for (Connection* conn : stationPtr->getConnections()) {
            if (std::find(connectionsToDelete.begin(), connectionsToDelete.end(), conn) == connectionsToDelete.end()) {
                connectionsToDelete.push_back(conn);
            }
        }
    }

    for (Connection* conn : connectionsToDelete) {
        delete conn;
    }

    for (auto const& [name, stationPtr] : transitMap) {
        delete stationPtr;
    }
}
