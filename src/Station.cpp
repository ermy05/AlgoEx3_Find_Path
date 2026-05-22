#include "headers/Station.h"

#include <iostream>
#include <algorithm>

Station::Station(const std::string& stationName)
    : name(stationName) {

}

void Station::addConnection(Connection* conn) {
    connections.push_back(conn);
}

void Station::printLines() const{
    std::cout << name << " serves lines: ";
    for (const Connection* conn : connections) {
        std::cout << conn->lineName << " ";
    }
    std::cout << std::endl;
}

void Station::printConnectedStations() const{
    std::cout << "Connections from " << name << ":" << std::endl;

    if (connections.empty()) {
        std::cout << "  (No direct connections)" << std::endl;
        return;
    }

    for (const Connection* conn : connections) {
        Station* neighbor = nullptr;

        if (conn->stationA == this) {
            neighbor = conn->stationB;
        }
        else {
            neighbor = conn->stationA;
        }

        if (neighbor != nullptr) {
            std::cout << " -> " << neighbor->getName() 
                      << " (Weight/Time: " << conn->weight << ") " 
                      << conn->lineName 
                      << std::endl;
        }
    }
    std::cout << std::endl;
}

std::string Station::getName() const {
    return name;
}

const std::vector<Connection*>& Station::getConnections() const {
    return connections;
}