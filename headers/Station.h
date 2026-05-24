#ifndef STATION_H
#define STATION_H

#include <string>
#include <vector>

// Forward declarations
class Station;

struct Connection {
    Station* stationA;
    Station* stationB;
    int weight;
    std::string lineName;
};

class Station {
private:
    std::string name;
    std::vector<Connection*> connections;

public:
    Station(const std::string& stationName);

    void addConnection(Connection* conn);
    void printLines() const;
    void printConnectedStations() const;

    std::string getName() const;
    const std::vector<Connection*>& getConnections() const;
    Station* getConnectedStation(Connection* conn) const;
    Connection* getEdgeBetweenStations(Station* other) const;
};

#endif // STATION_H