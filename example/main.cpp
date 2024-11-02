// This is an example of the standalone version of openFrameworks communication/serial
// Distributerd under the MIT License.
// Copyright (c) 2022 - Jean-François Erdelyi

#include "ofSerial.h"

#include <iostream>
#include <thread>

// ofSerial standalone example
int main(int argc, char* argv[]) {

	// Check and get params 
	ofSerial l_serial;
	if (argc < 3) {
		std::cout << "Usage: serial <port> <baudrate>" << std::endl << std::endl;
		std::cout << "List of serial ports detected:" << std::endl;
		for (const auto& l_port : l_serial.getDeviceList()) {
			std::cout << "\t- " << l_port.getDeviceName() << std::endl;
		}
		std::cout << "EXIT" << std::endl;
		return EXIT_FAILURE;
	}
	char* l_port = argv[1];
	const auto l_baudrate = size_t(atoi(argv[2]));
	std::cout << "attemp to connect" << l_port << ',' << l_baudrate << std::endl;
	// Setup and connect serial
	bool l_connected = l_serial.setup(std::string_view(l_port), l_baudrate);
	if (l_connected) {
		std::cout << "CONNECTED" << std::endl;
	} else {
		std::cout << "NOT CONNECTED" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << std::endl;
	while (true) {

		// Get and send data
		// std::string l_input;
		// std::cout << "SEND DATA (EXIT to quit): ";
		// std::getline(std::cin, l_input); 
		// if (l_input == "") {
		// 	continue;
		// } else if (l_input == "EXIT") {
		// 	break;
		// }
		// l_serial.flush(true, true);
		// l_serial.writeData(l_input);

		// Wait the answer
		while(!l_serial.available()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		// Check if there is data to read
		int l_bytes_to_read = l_serial.available();
	
		// // Print number of bytes 
		// std::cout << std::endl << "RECEIVED " << std::dec << l_bytes_to_read << " BYTES" << std::endl;
		// int l_bytes_read = l_serial.readData(l_bytes_to_process, l_bytes_to_read);
		auto bytes = l_serial.readBytes();
		{
			auto print_hex = [](const uint8_t chr){

				if(chr < 0x10){
					std::cout << '0';
				}
				std::cout << +chr;
			};

			// Print hexa data
			std::cout << "bytes:" << std::oct << bytes.size(); 
			std::cout << "str:" << std::string(bytes.begin(), bytes.end()); 
			std::cout << std::endl << std::hex << std::uppercase << '[';
			for (auto it = bytes.cbegin(); it != bytes.cend(); ++it) {
				print_hex(*it);
				if(it != std::prev(bytes.cend())) std::cout << ',';
			}
			std::cout << ']' << std::endl;
		}
	}

	// Close and return
	l_serial.close();
	std::cout << "CLOSED" << std::endl;
	return EXIT_SUCCESS;
}
