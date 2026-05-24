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

Station* Station::getConnectedStation(Connection* conn) const {
    if (conn->stationA == this) {
        return conn->stationB;
    }
    return conn->stationA;
}

Connection* Station::getEdgeBetweenStations(Station* other) const {
    auto connections = this->getConnections();
    for (auto edge : connections) {
        if ((this == edge->stationA || this == edge->stationB) && (other == edge->stationA || other == edge->stationB)) {
            return edge;
        }
    }
    return nullptr;
}