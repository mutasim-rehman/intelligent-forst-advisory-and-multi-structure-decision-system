#ifndef IFAMDS_UTILS_H
#define IFAMDS_UTILS_H

#include "models.h"

namespace ifamds {

SensorReading makeReading(int zoneId, float temperature, float smoke, float humidity);
void printReading(const SensorReading& reading);
int readInt();
float readFloat();

}  // namespace ifamds

#endif
