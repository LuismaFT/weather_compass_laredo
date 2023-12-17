
#ifndef __HEADER_UTILS__
#define __HEADER_UTILS__

#define Vcc 5.0           // Supply voltage
#define Rl 10.0           // Load resistance in kilo-ohms
#define a 1.0             // 'a' constant for the specific gas (example value)
#define b -0.44           // 'b' constant for the specific gas (example value)
#define Ro 10.0           // Calibration resistance at known concentration in kilo-ohms

float mv_to_celsius(int mv);
float mv_to_lux(int mv);
float value_to_percent(int mv);
int value_from_json(const char* json_str);
int analogToPPM(int analogValue);

#endif // __HEADER_UTILS__

