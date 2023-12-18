
//Project EDEN: Smart Irrigation System
//Arduino 1 code: Takes 16 sensors values and outputs them into a string.
#include <DHT.h>
#include <DHT_U.h>
#include <dht.h>
#define TEMP_HUM_PIN 0
//Attach sensorInterrupt to digital pin 2
//String format: "Humidity, Temperature, Liters, PSI, Soil Moisture 1, Soil Moisture 2, Soil Moisture 3, ..., Soil Moisture 13"
//Analog Pin 0: Temperature/Humidity Sensor, Analog Pin 1: Water Usage Sensor,  Analog Pin 2: Pressure Sensor, Analog Pins 3-15: Soil Moisture Sensors
//###################################################################
//ALL SENSOR CODE STUFF SETUP
String SensorFinal; 
String comma = ",";
//###################################################################
// HUMIDITY/TEMPERATURE SENSORS

dht DHT;
String temp_hum_str;
//###################################################################
// WATER FLOW RATE CODE SETUP
String total_ltr_str;

byte statusLed    = 13;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 1;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowLitres;
unsigned long totalLitres;

unsigned long oldTime;
//###################################################################
// WATER PRESSURE CODE SETUP
String water_psi_str;
const int pressureInput = A2; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //analog reading of pressure transducer at 0psi
const int pressureMax = 921.6; //analog reading of pressure transducer at 100psi
const int pressuretransducermaxPSI = 100; //psi value of transducer being used
const int sensorreadDelay = 250; //constant integer to set the sensor read delay in milliseconds

float pressureValue = 0; //variable to store the value coming from the pressure transducer
//###################################################################
// SOIL MOISTURE CODE SETUP
//const int AirValue = 520;   //upper bound value: outputted by a sensor in air
//const int WaterValue = 260; //lower bound value: value outputted by a sensor in water
String SoilMoistureString[13];

void setup()
{
    Serial.begin(9600);

//###################################################################
// WATER FLOW RATE CODE SETUP
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pulseCount        = 0;
  flowRate          = 0.0;
  flowLitres        = 0;
  totalLitres       = 0;
  oldTime           = 0;
  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  //###################################################################
    
}
 
void loop()
{
    // Wait 10 seconds between measurements.
 
GatherMoistureValues(SoilMoistureString);
GatherTempHum(&temp_hum_str);
if((millis() - oldTime) > 1000){
  GatherWaterFlowSensor(&total_ltr_str);
  }

GatherWaterPressureSensor(&water_psi_str);
//preceding the values are the pin numbers but I add 21 to mask them 
SensorFinal =       "a2" + comma + int(pressureValue) + comma + 
                    "a3" + comma + SoilMoistureString[0] + comma + "a4" + comma + SoilMoistureString[1] + comma + "a5" + comma + SoilMoistureString[2] + comma + "a6" + comma + 
                    SoilMoistureString[3] + comma + "a7" + comma + SoilMoistureString[4] + comma +"a8" + comma +  SoilMoistureString[5] + comma + "a9" + comma + 
                    SoilMoistureString[6] + comma +"a10" + comma +  SoilMoistureString[7] + comma +"a11" + comma +  SoilMoistureString[8] + comma + "a12" + comma + 
                    SoilMoistureString[9] + comma + "a13" + comma + SoilMoistureString[10] + comma +"a14" + comma +  SoilMoistureString[11] + comma + "a15" + comma + 
                    SoilMoistureString[12];
Serial.flush();
Serial.print(temp_hum_str + comma + "a1" + comma + String(total_ltr_str) + comma);              
Serial.println(SensorFinal);
Serial.flush();
 delay(5000);
}

void GatherMoistureValues(String * arr) {
  int i;
  int value = 0;
  for (i = 0; i <13; i++) {
      value = analogRead(i+3);  //put Sensor insert into soil
      arr[i] = String(value);
  }
}
void GatherTempHum(String* str){
  int chk = DHT.read11(TEMP_HUM_PIN);
//  *str = "10" + comma + String(DHT.humidity) + comma + "10"+ comma+ String(DHT.temperature);
  *str = "a0" + comma + "0" + comma + "a0"+ comma+ "0";
  }
void GatherWaterFlowSensor(String* Ltr){
     // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowLitres = (flowRate / 60);
    
    // Add the millilitres passed in this second to the cumulative total
    totalLitres += flowLitres;
    *Ltr = totalLitres;
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute


    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING); 
  }

  void GatherWaterPressureSensor(String* str){
    pressureValue = analogRead(pressureInput); //reads value from input pin and assigns to variable
    *str = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  }
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}