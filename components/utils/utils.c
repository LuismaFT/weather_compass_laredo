
#include <math.h>
#include <stdlib.h>

#include "utils.h"

float mv_to_celsius(int mv) {
    return (float) ((mv-500)/10);
}

float mv_to_lux(int mv) {
    return (float) ((mv*10000)/((1500-mv)*150));
}

float value_to_percent(int mv){
   return (float) (mv*0.048840);
}

int value_from_json(const char* json_str) {
    char str[10];
    int i=0;
    for (const char *p=json_str; *p!=0; p++)
        if ('0' <= *p && *p <= '9') {
            str[i] = *p;
            i++;
        }
    return atoi(str);
}

int analogToPPM(int analogValue) {
    float sensorVoltage = (analogValue / 4095.0) * Vcc;
    float sensorResistance = ((Vcc / sensorVoltage) - 1) * Rl;
    float ratio = sensorResistance / Ro;
    float ppm = a * pow(ratio, b);

    return (int) ppm;
}

