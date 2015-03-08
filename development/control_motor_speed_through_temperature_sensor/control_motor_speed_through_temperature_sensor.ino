/*
 *  Control Motor Speed Through Temperature Sensor.
 *
 *  Copyright (C) 2010 Efstathios Chatzikyriakidis (contact@efxa.org)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// include float limits for DBL_EPSILON.
#include <float.h>

const int motorPin = 9;  // the pin number (PWM) for the motor.
const int sensorPin = 0; // the pin number for the input sensor.

// enable this if you want to get messages on serial line.
const boolean debugMode = false;

const int N = 10;  // number of sensor samples.
double samples[N]; // array of sensor samples.
double curSample;  // current sensor sample.
double curValue;   // current sensor value.
int motorSpeed;    // speed of the motor.

// delay time (millis) for the next sensor sample fetching.
const int NEXT_SAMPLE_DELAY = 40;

double sensorMin = 0.0;   // start minimum sensor value (Celsius).
double sensorMax = 100.0; // start maximum sensor value (Celsius).

// startup point entry (runs once).
void setup()
{
  // current sensor value works first as sum counter.
  curValue = 0.0;

  // for debug.
  if(debugMode)
    Serial.begin(9600);

  // set motor as output.
  pinMode(motorPin, OUTPUT);

  // set sensor as input.
  pinMode(sensorPin, INPUT);
}

// loop the main sketch.
void loop()
{
  // get sensor samples first with delay to calculate the sum of them.
  for (int i = 0; i < N; i++) {
    // get sensor sample and convert it to Celsius temperature.
    curSample = getCelsius(getVoltage(sensorPin));

    // store the sample for future use.
    samples[i] = curSample;

    // add sample to the sum counter.
    curValue += curSample;
    
    // delay some time for the next sample.
    delay(NEXT_SAMPLE_DELAY);
  }  

  // get the average sensor value (with precision).
  curValue /= (double) N;

  // perform dynamic calibration on run-time.
  if(isGreaterThan(curValue, sensorMax)) {
    sensorMax = curValue; // set max temperature.
  }
  
  if(isLessThan(curValue, sensorMin)) {
    sensorMin = curValue; // set min temperature.
  }

  // for debug.
  if(debugMode)
    Serial.println(curValue);

  // map the sensor value for the motor (PWM).
  int motorSpeed = map(curValue, (int) sensorMin, (int) sensorMax, 20, 255);

  // set the signal to the motor.
  analogWrite(motorPin, motorSpeed);
}

// get the voltage on the analog input pin.
double getVoltage (const int pin) {
  // converting from a 0 to 1024 digital range to
  // 0 to 5 volts (each 1 reading equals ~ 5 mv).
  return (analogRead(pin) * .004882814);
}

// convert the voltage to Celsius temperature.
double getCelsius (const double voltage) {
  return (voltage - .5) * 100;
}

// check if double a is less than double b.
bool
isLessThan(const double a, const double b) {
    return (b - a) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * DBL_EPSILON);
}

// check if double a is greater than double b.
bool
isGreaterThan(const double a, const double b) {
    return (a - b) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * DBL_EPSILON);
}
