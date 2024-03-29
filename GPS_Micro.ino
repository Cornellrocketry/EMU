// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada

//This code is intended for use with Arduino Leonardo and other ATmega32U4-based Arduinos

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// If using software serial (sketch example default):
//   Connect the GPS TX (transmit) pin to Digital 8
//   Connect the GPS RX (receive) pin to Digital 7
// If using hardware serial:
//   Connect the GPS TX (transmit) pin to Arduino RX1 (Digital 0)
//   Connect the GPS RX (receive) pin to matching TX1 (Digital 1)

// If using software serial, keep these lines enabled
// (you can change the pin numbers to match your wiring):
//SoftwareSerial mySerial(8, 7);
//Adafruit_GPS GPS(&mySerial);

// If using hardware serial, comment
// out the above two lines and enable these two lines instead:
Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true

void setup()  
{
    
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial1.begin(9600);
  delay(5000);
  Serial1.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
}

uint32_t timer = millis();
void loop()                     // run over and over again
{
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    //Serial1.write(c); 
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer
    
    Serial1.print("\nTime: ");
    Serial1.print(GPS.hour, DEC); Serial1.print(':');
    Serial1.print(GPS.minute, DEC); Serial1.print(':');
    Serial1.print(GPS.seconds, DEC); Serial1.print('.');
    Serial1.println(GPS.milliseconds);
    Serial1.print("Date: ");
    Serial1.print(GPS.day, DEC); Serial1.print('/');
    Serial1.print(GPS.month, DEC); Serial1.print("/20");
    Serial1.println(GPS.year, DEC);
    Serial1.print("Fix: "); Serial1.print((int)GPS.fix);
    Serial1.print(" quality: "); Serial1.println((int)GPS.fixquality); 
    
    
    
    if (GPS.fix) {
      Serial1.print("Location: ");
      Serial1.print(GPS.latitude, 4); Serial1.print(GPS.lat);
      Serial1.print(", "); 
      Serial1.print(GPS.longitude, 4); Serial1.println(GPS.lon);
      
      Serial1.print("Speed (knots): "); Serial1.println(GPS.speed);
      Serial1.print("Angle: "); Serial1.println(GPS.angle);
      Serial1.print("Altitude: "); Serial1.println(GPS.altitude);
      Serial1.print("Satellites: "); Serial1.println((int)GPS.satellites);
    }
    
    int sensorValue;
    sensorValue = analogRead(A5);            

    //print the results to the serial monitor:
    Serial1.print("sensor = " );                       
    Serial1.println(sensorValue); 
    Serial1.println("----------");
  }
  


  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading: 
  //delay(500);
}
