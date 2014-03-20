Arduino_mqtt_temperature
========================

Arduino sketch to retrieve current temperature from a Dallas ds18b20 and push to MQTT

This is essentially a base sketch for all Arduino based home automation boards for my house. It does several things,

1) uses the DS18B20 sensors unique ID to generate a mac address for use with an ethernet shield and starts above network using generated mac and DHCP, this allows the same piece of code to be copied to multiple boards without massive effort to configure the networking.

2) As we are using the DS18B20 for mac generation we may as well use the data so the sensor is actually probed for temperatures at a regular interval.

3) the data is then farmed off to a data logger or end point using MQTT.

You must copy the config.h.sample file to config.h and edit your own variables for this to work.

MQTT_SERVER - set this to the hostname or IP of your MQTT Broker
TEMP_TOPIC set the topic you wish the temperature to be published to here
CLIENT_ID - This can be anything but should be unique to this sketch - this will probably be changed in future
