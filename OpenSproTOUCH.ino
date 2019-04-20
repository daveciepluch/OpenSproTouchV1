#include <core_build_options.h>
#include <secTimer.h>

#include <NexConfig.h>
#include <Nextion.h>

//Configuration and libraries
#include "Configuration.h"
#include "Sensors.h"
#include <SPI.h>

//Set autofill sensor pin
Sensors boiler(9);

//PID and heater control variables
unsigned long lastPIDTime;  // most recent PID update time in ms
float targetTemp = 125.0;  //current temperature goal
float heatPower; // 0 - 1000  milliseconds on per second
String stats = "";
String settings = "";
String timer = "";
int brewState = 0;
unsigned long brewStarted;

//Shot Timer
secTimer group1Timer;

//HMI Button Variable
char buffer[100] = {0};
NexDSButton bt0 = NexDSButton(0, 3, "bt0");
NexDSButton bt1 = NexDSButton(0, 4, "bt1");

NexTouch *nex_listen_list[] =
{
  &bt0, &bt1,
  NULL
};

//Setup program
void setup() {
  setupAutofill();
  delay(1000);
  setupPID(PGAIN_ADR, IGAIN_ADR, DGAIN_ADR); // Send addresses to the PID module
  setupTempSensor();
  setupHeater();
  lastPIDTime = millis();
  bt0.attachPop(bt0PopCallback, &bt0);
  bt1.attachPop(bt1PopCallback, &bt1);
  nexInit();
  setupControls();
  setupSerialInterface();
  dbSerialPrintln("setup done");
}

//Continuous Loop
void loop() {
  //Added while loop to protect against failed thermocouple
  nextionUpdateStatus();
  nextionUpdateSettings();
  updateSerialInterface(); //serial updates regardless of thermocouplestate
  nexLoop(nex_listen_list);
  preinfControl();
  updateAutofill(); //Updates the autofill circuit, ensuring the boiler is full
  while (getTemp() > 2) {
    if (millis() < lastPIDTime) {
      lastPIDTime = 0;
    }
    if ((millis() - lastPIDTime) > PID_UPDATE_INTERVAL) {
      lastPIDTime +=  PID_UPDATE_INTERVAL;
      heatPower = updatePID(targetTemp, getTemp());
      setHeatPowerPercentage(heatPower);
    }
    updateHeater();
  }

}
