# ESP-KNX-IP #

This is a library for the ESP8266 to enable KNXnet/IP communication. It uses UDP multicast on 224.0.23.12:3671.
It is intended to be used with the Arduino platform for the ESP8266.

## How to use ##

The library is under development. API may change multiple times in the future.

A simple example:

```c++
#include <esp-knx-ip.h>

const char* ssid = "my-ssid";  //  your network SSID (name)
const char* pass = "my-pw";    // your network password

config_id_t my_GA;
config_id_t param_id;

void setup()
{
	// Register a callback that is called when a configurable group address is receiving a telegram
	knx.register_callback("callback_name", my_callback);

	// Register a configurable group address for sending out answers
	my_GA = knx.config_register_ga("send to this GA");

	int default_val = 21;
	param_id = knx.config_register_int("My Parameter", default_val);

	knx.load(); // Try to load a config from EEPROM

	WiFi.begin(ssid, pass);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
	}

	knx.start(); // Start everything. Must be called after WiFi connection has been established


}

void loop()
{
	knx.loop();
}


void my_callback(knx_command_type_t ct, address_t const &received_on, uint8_t data_len, uint8_t *data)
{
	switch (ct)
	{
	case KNX_CT_WRITE:
		// Do something, like a digitalWrite
		// Or send a telegram like this:
		uint8_t my_msg = 42;
		knx.write1ByteInt(knx.config_get_ga(my_GA), my_msg);
		break;
	case KNX_CT_READ:
		// Answer with a value
		int value = knx.config_get_int(param_id);
		knx.answer1ByteInt(received_on, (uint8_t)value);
		break;
	}
}
```

## How to configure (buildtime) ##

Open the `esp-knx-ip.h` and take a look at the config options at the top inside the block marked `CONFIG`

## How to configure (runtime) ##

Simply visit the IP of your ESP with a webbrowser. You can configure the following:
* KNX physical address
* Which group address should trigger which callback
* Which group address are to be used by the program (e.g. for status replies)

The configuration is dynamically generated from the code.