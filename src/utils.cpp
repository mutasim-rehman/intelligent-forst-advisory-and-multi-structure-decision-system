#include "utils.h"

#include <ctime>
#include <iostream>

namespace ifamds {

SensorReading makeReading(int zoneId, float temperature, float smoke, float humidity) {
    SensorReading r;
    r.zoneId = zoneId;
    r.temperature = temperature;
    r.smoke = smoke;
    r.humidity = humidity;
    r.timestamp = static_cast<std::int64_t>(std::time(nullptr));
    return r;
}

void printReading(const SensorReading& reading) {
    std::cout << "Zone " << reading.zoneId
              << " | Temp: " << reading.temperature
              << " | Smoke: " << reading.smoke
              << " | Humidity: " << reading.humidity
              << " | TS: " << reading.timestamp << '\n';
}

int readInt() {
    int value{};
    std::cin >> value;
    return value;
}

float readFloat() {
    float value{};
    std::cin >> value;
    return value;
}

}  // namespace ifamds
