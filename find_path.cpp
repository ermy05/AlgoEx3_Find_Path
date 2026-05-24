#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "headers/Station.h"
#include <list>
#define INF 1000000

using namespace std;

struct PathResult {
    std::unordered_map<std::string, int> distances;
    std::unordered_map<std::string, Station*> previous;
    bool pathFound;
};

using StationMap = std::unordered_map<std::string, Station*>;

void addTransitEdge(StationMap& stationMap,
    const std::string& nameA,
    const std::string& nameB,
    int weight,
    const std::string& lineName);

bool loadNetworkFromFile(const std::string& filename, StationMap& stationMap);

void freeMemory(StationMap& stationMap);

PathResult dijkstra(const StationMap& graph, std::string startName, std::string targetName);
std::vector<std::string> reconstructPath(const PathResult& result, const std::string& startName, const std::string& targetName);
void findShortestPath(const std::unordered_map<std::string, Station*>& graph, const std::string& startName, const std::string& targetName);

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
        //karls->printConnectedStations();
        findShortestPath(stationMap, start, finish);
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

PathResult dijkstra(
    const StationMap& graph, 
    std::string startName, 
    std::string targetName
) {
    PathResult result;
    result.pathFound = false;

    auto startIterator = graph.find(startName);
    auto targetIterator = graph.find(targetName);

    if (startIterator == graph.end() || targetIterator == graph.end()) {
        std::cout << "invalid start or target values" << std::endl;
        return result;
    }

    Station* start = startIterator->second;
    Station* target = targetIterator->second;

    for (const auto& pair : graph) {
        result.distances[pair.first] = INF;
        result.previous[pair.first] = nullptr;
    }
    result.distances[startName] = 0;

    using PQElement = std::pair<int, Station*>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;

    pq.push({ 0, start });

    std::unordered_map<std::string, bool> visited;

    while (!pq.empty()) {
        auto [currentDistance, currentStation] = pq.top();
        pq.pop();

        std::string currentName = currentStation->getName();
        if (visited[currentName]) {
            continue;
        }
        visited[currentName] = true;

        if (currentStation == target) {
            result.pathFound = true;
            break;
        }

        for (Connection* connection : currentStation->getConnections()) {
            Station* neighbor = currentStation->getConnectedStation(connection);
            std::string neighborName = neighbor->getName();;

            if (visited[neighborName]) {
                continue;
            }

            int newDistance = currentDistance + connection->weight;

            if (newDistance < result.distances[neighborName]) {
                result.distances[neighborName] = newDistance;
                result.previous[neighborName] = currentStation;
                pq.push({ newDistance, neighbor });
            }
        }
    }
    return result;
}

std::vector<std::string> reconstructPath(
    const PathResult& result,
    const std::string& startName,
    const std::string& targetName
) {
    std::vector<std::string> path;

    if (!result.pathFound) {
        return path;
    }

    std::string current = targetName;

    while (current != startName) {
        path.push_back(current);

        Station* prev = result.previous.at(current);
        if (prev == nullptr) {
            return {};
        }
        current = prev->getName();
    }

    path.push_back(startName);

    std::reverse(path.begin(), path.end());

    return path;
}

void findShortestPath(
    const std::unordered_map<std::string, Station*>& graph,
    const std::string& startName,
    const std::string& targetName
) {
    PathResult result = dijkstra(graph, startName, targetName);

    if (!result.pathFound) {
        std::cout << "No Path from " << startName << " to " << targetName << " found!" << std::endl;
        return;
    }

    std::vector<std::string> path = reconstructPath(result, startName, targetName);
    int totalDistance = result.distances[targetName];

    std::cout << "Shortest Path from: " << startName << " to " << targetName << ":" << std::endl;
    std::cout << "Distance: " << totalDistance << std::endl;
    std::cout << "Path: ";

    for (size_t i = 0; i < path.size(); i++) {
        std::cout << path[i];
        if (i < path.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
}
