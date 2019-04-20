#include <NexConfig.h>
#include <Nextion.h>

//Configuration and libraries
#include "Configuration.h"
#include "Sensors.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Time.h>

//Set autofill sensor pin
Sensors boiler(9);

//PID and heater control variables
unsigned long lastPIDTime;  // most recent PID update time in ms
float targetTemp = 125.0;  //current temperature goal
float heatPower; // 0 - 1000  milliseconds on per second

int mode = 0;
String stats = "";

//HMI Button Variable
char buffer[100] = {0};
NexDSButton bt0 = NexDSButton(0, 3, "bt0");
NexDSButton bt1 = NexDSButton(0, 4, "bt1");
NexDSButton bt2 = NexDSButton(0, 5, "bt2");

NexTouch *nex_listen_list[] =
{
  &bt0, &bt1, &bt2,
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
  bt2.attachPop(bt2PopCallback, &bt2);
  nexInit();
  setupControls();
  setupSerialInterface();
  dbSerialPrintln("setup done");

}

//Continuous Loop
void loop() {
  //Added while loop to protect against failed thermocouple
  nextionUpdateStatus();
  updateSerialInterface(); //serial updates regardless of thermocouplestate
  nexLoop(nex_listen_list);
  updateAutofill();
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
