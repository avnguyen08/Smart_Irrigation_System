//Arduino 2 code: Takes 16 soil moisture sensors values from analog pins and outputs them to the serial port with labeling.
//String format: "b0,Value0,b1,Value1,b2,Value2, ... etc"
//Analog pin 0: Soil Moisture 1, Analog pin 1: Soil Moisture 2,  Analog pin 2: Soil Moisture 3, ..., Analog pin 15: Soil Moisture 16

// SOIL MOISTURE SENSORS
const int AirValue = 520;    //value outputted by a dry sensor
const int WaterValue = 260;  //value outputted by a sensor in water
String comma = ",";
String SoilMoistureString[16];
int str_size = sizeof(SoilMoistureString) / sizeof(SoilMoistureString[0]);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Wait 10 seconds between measurements.
  delay(5000);
  //Updates Soil moisture sensor values and logs it into string
  GatherMoistureValues(SoilMoistureString, str_size);
  Serial.flush();
  for (int i = 0; i < str_size; ++i) {
    Serial.print("b" + String(i) + "," + SoilMoistureString[i]);
    if (i < str_size - 1)
      Serial.print(",");
  }
  Serial.println();
  Serial.flush();
}

void GatherMoistureValues(String* arr, int size) {
  int i;
  int value = 0;
  for (i = 0; i < size; i++) {
    value = analogRead(i);  //put Sensor insert into soil
    arr[i] = String(value);
  }
}
