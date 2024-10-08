// This is an example of the standalone version of openFrameworks communication/serial
// Distributerd under the MIT License.
// Copyright (c) 2022 - Jean-François Erdelyi

#include "ofSerial.h"

#include <iostream>
#include <thread>

using namespace std;

// ofSerial standalone example
int main(int argc, char* argv[]) {

	// Check and get params 
	ofSerial l_serial;
	if (argc < 3) {
		std::cout << "Usage: serial <port> <baudrate>" << std::endl << std::endl;
		std::cout << "List of serial ports detected:" << std::endl;
		for (auto& l_port : l_serial.getDeviceList()) {
			std::cout << "\t- " << l_port.getDeviceName() << std::endl;
		}
		return EXIT_FAILURE;
	}
	char* l_port = argv[1];
	int l_baudrate = atoi(argv[2]);

	// Setup and connect serial
	bool l_connected = l_serial.setup(l_port, l_baudrate);
	if (l_connected) {
		std::cout << "CONNECTED" << std::endl;
	} else {
		std::cerr << "NOT CONNECTED" << std::endl;
		return EXIT_FAILURE;
	}

	// While is running
	std::string l_bytes_to_process;
	std::cout << std::endl;
	while (true) {

		// Get and send data
		std::string l_input;
		std::cout << "SEND DATA ([EXIT] to quit): ";
		getline(cin, l_input); 
		if (l_input == "") {
			continue;
		} else if (l_input == "EXIT") {
			break;
		}
		l_input += "\n";
		l_serial.writeData(l_input);

		// Wait the answer
		while (!l_serial.available()) {
			this_thread::sleep_for(chrono::milliseconds(100));
		}

		// Check if there is data to read
		int l_bytes_to_read = l_serial.available();
	
		// Print number of bytes 
		std::cout << std::endl << "RECEIVED " << std::dec << l_bytes_to_read << " BYTES" << std::endl;
		int l_bytes_read = l_serial.readData(l_bytes_to_process, l_bytes_to_read);

		// Print hexa data
		std::cout << hex << uppercase;
		const char* l_c_str = l_bytes_to_process.c_str();
		for (int i = 0; i < l_bytes_read; ++i) {
			std::cout << static_cast<unsigned short>(l_c_str[i] & 0x00FF) << " ";
		}

		// Print std::string data 
		std::cout << std::dec << "-> " << l_bytes_to_process << std::endl << std::endl;
	}

	// Close and return
	l_serial.close();
	std::cout << "CLOSED" << std::endl;
	return EXIT_SUCCESS;
}
