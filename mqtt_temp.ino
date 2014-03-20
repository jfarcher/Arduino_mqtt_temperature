/**************************************************** 
 * mqtt_temp
 *
 * Jon Archer
 * 
 * Sketch to take the temperature from an attached
 * Dallas sensor and post it over MQTT, this also
 * generates a mac address for ethernet use from 
 * the Dallas sensors unique ID.
 * This is essentially a starting point for all HA
 * sketches as this will feature in every room.
 ****************************************************/


#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

byte mac[]= { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
OneWire ds(2);


EthernetClient ethClient;  // Ethernet object
PubSubClient client( MQTT_SERVER, 1883, callbackMQTT, ethClient); // MQTT object
DallasTemperature dallas(&ds);

void ethernetFromDS(){
    byte i;
    byte dsAddress[8];
    delay( 500 );
#ifdef DEBUG_PRINT
    Serial.print ("Searching for DS18B20...");
#endif
    ds.reset_search();

  if ( !ds.search(dsAddress) )
  {
#ifdef DEBUG_PRINT
    Serial.println("none found. Using specified MAC Address.");
#endif
  } else {
#ifdef DEBUG_PRINT
    Serial.print( "Success! \nSetting MAC address...." );
#endif
  mac[1] = dsAddress[3];
  mac[2] = dsAddress[4];
  mac[3] = dsAddress[5];
  mac[4] = dsAddress[6];
  mac[5] = dsAddress[7];
  }
#ifdef DEBUG_PRINT  
  Serial.print( "Ethernet MAC =" );
  for( i = 0; i < 6; i++ )
  {
    Serial.write( ' ' );
    Serial.print( mac[i], HEX );
  }
  Serial.println();
#endif

  if (Ethernet.begin(mac) == 0) {
#ifdef DEBUG_PRINT
    Serial.println("Failed to configure Ethernet using DHCP");
#endif
    for(;;)
      ;
  }  

#ifdef DEBUG_PRINT
  Serial.print("IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
      Serial.println(Ethernet.localIP()[thisByte], DEC);
    }
#endif  
}

//Start MQTT goodness
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
}

void checkMQTT() {
    if(!client.connected()) {
    if (client.connect(CLIENT_ID)) {
#ifdef DEBUG_PRINT
      Serial.println(F("MQTT Reconecting"));
#endif
    } 
  } 
} // end checkMQTT()


void getTemp()
{

   dallas.requestTemperatures(); // Send the command to get temperatures
    
      char* temp;
      unsigned long tempTimeout = 0;
      char message_buffer[100];
      temp = dtostrf(dallas.getTempCByIndex(0), 5, 2, message_buffer);
      
      // push each stright out via mqtt
       if ( (millis() - tempTimeout) > 10000 ) {
       // if (millis() > (time + 150000)) {
    tempTimeout = millis();
    client.publish(TEMP_TOPIC, temp);
    delay( 500 );
    
#ifdef DEBUG_PRINT
      Serial.println(temp);

#endif
        }
  
} // end getTemp()



void setup()
{
    Serial.begin(9600);
//Start Ethernet using mac formed from DS
 ethernetFromDS();

//Start the dallas sensor
 dallas.begin();

// Start MQTT
  checkMQTT();

// short delay to make sure we're happy
  delay(100);
}


void loop()
{
   client.loop();
//Get the temperatures 
  getTemp();
  
// are we still connected to MQTT
  checkMQTT();
}
