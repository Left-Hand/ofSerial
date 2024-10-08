// This class is a standalone version of openFrameworks communication/serial
// Distributerd under the MIT License.
// Copyright (c) 2022 - Jean-François Erdelyi

// openFrameworks is distributed under the MIT License. This gives everyone the freedoms 
// to use openFrameworks in any context: commercial or non-commercial, public or private, open or closed source.
// Copyright (c) 2004 - openFrameworks Community
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without restriction, 
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#if defined( __WIN32__ ) || defined( _WIN32 )
	#define TARGET_WIN32
#elif defined( __APPLE_CC__)
	#define TARGET_OSX
#else
	#define TARGET_LINUX
#endif

#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	#include <termios.h>
#else
	#include <Windows.h>
	#pragma comment (lib, "Setupapi.lib")
	#include <setupapi.h>
	#define MAX_SERIAL_PORTS 256
#endif

#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <cstdint>

// serial error codes
#define OF_SERIAL_NO_DATA 	-2
#define OF_SERIAL_ERROR		-1

#define OF_SERIAL_PARITY_N	0
#define OF_SERIAL_PARITY_O	1
#define OF_SERIAL_PARITY_E	2

/// \brief Describes a Serial device, including ID, name and path.
class ofSerialDeviceInfo{
	friend class ofSerial;

	public:
		/// \brief Construct an ofSerialDeviceInfo with parameters.
		/// \param devicePathIn The path to the device.
		/// \param deviceNameIn The name of the device.
		/// \param deviceIDIn The ID of the device.
		ofSerialDeviceInfo(const std::string_view devicePathIn,const std::string_view deviceNameIn, int deviceIDIn){
			devicePath = devicePathIn;
			deviceName = deviceNameIn;
			deviceID = deviceIDIn;
		}

		/// \brief Construct an undefined serial device.
		ofSerialDeviceInfo(){
			deviceName = "device undefined";
			deviceID   = -1;
		}

		/// \brief Gets the path to the device
		///
		/// Example: `/dev/tty.cu/usbdevice-a440`.
		///
		/// \returns the device path.
		std::string getDevicePath(){
			return devicePath;
		}

		/// \brief Gets the name of the device
		///
		/// Example: `usbdevice-a440` or `COM4`.
		///
		/// \returns the device name.
		std::string getDeviceName(){
			return deviceName;
		}

		/// \brief Gets the ID of the device
		///
		/// Example: `0`,`1`,`2`,`3` etc.
		///
		/// \returns the device ID.
		int getDeviceID(){
			return deviceID;
		}

	protected:
		/// \cond INTERNAL

		/// \brief The device path (e.g /dev/tty.cu/usbdevice-a440).
		std::string devicePath;

		/// \brief The device name (e.g. usbdevice-a440 / COM4).
		std::string deviceName;

		/// \brief The device ID (e.g. 0, 1, 2, 3, etc).
		int deviceID;

		/// \endcond
};

/// \brief ofSerial provides a cross platform system for interfacing with the
/// serial port. You can choose the port and baud rate, and then read and send
/// data. Please note that the port must be set manually in the code, so you
/// should be clear what port your device is on. For example, Arduino users should
/// check the arduino app to see what port their device is on. Alternatively the
/// ofSerial class can attempt to communicate with the first available device it
/// finds.
///
/// To start up a serial connection to another device you do the following:
///
/// ~~~~{.cpp}
/// serial.listDevices();
/// vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
///
/// // Open the first device and talk to it at 57600 baud
/// serial.setup(0, 57600);
/// ~~~~
class ofSerial {

public:
	/// \name Constructor and Destructor
	/// \{

	/// Initializes the serial connection, but doesn't actually open the
	/// connection to any devices. You'll need to use the setup() method
	/// before doing that.
	ofSerial();

	virtual ~ofSerial();

	/// \}
	/// \name List Devices
	/// \{

	/// \brief Returns a vector of ofSerialDeviceInfo instances with the
	/// devicePath, deviceName, deviceID set.
	std::vector <ofSerialDeviceInfo> getDeviceList();

	/// \}
	/// \name Serial Connection
	/// \{

	/// \brief Opens the serial port, with the given name and baud rate.
	///
	/// On OSX and Linux, it might look like:
	/// ~~~~{.cpp}
	/// ofSerial mySerial;
	/// mySerial.setup("/dev/cu.USA19H181P1.1", 57600);
	/// ~~~~
	///
	/// On Windows, like:
	/// ~~~~{.cpp}
	/// ofSerial mySerial;
	/// mySerial.setup("COM4", 57600);
	/// ~~~~
	bool setup(const std::string_view portName, int baudrate = 9600, int data = 8, int parity = OF_SERIAL_PARITY_N, int stop = 1);

	/// \brief Opens the serial port based on the order in which is listed and
	/// sets the baud rate.
	///
	/// The code bellow would open the first serial device found by the system:
	/// ~~~~{.cpp}
	/// ofSerial mySerial;
	/// mySerial.setup(0, 9600);
	/// ~~~~
	bool setup(int deviceNumber = 0, int baudrate = 9600, int data = 8, int parity = OF_SERIAL_PARITY_N, int stop = 1);

	bool isInitialized() const;

	/// \brief Closes the connection to the serial device.
	void close();

	/// \}
	/// \name Read Data
	/// \{

	/// \brief The available method is useful when you want to know how many bytes
	/// are available in the serial port. For instance, if you only want to
	/// read when there are 8 bytes waiting for you, you would do:
	///
	/// ~~~~{.cpp}
	/// if(device.available() > 8) {
	///	 device.readData(buffer, 8);
	/// }
	/// ~~~~
	///
	/// This is useful when you know how long a complete message from a device
	/// is going to be.
	int available();

	/// \brief Reads 'length' bytes from the connected serial device.
	///
	/// In some cases it may read less than 'length' bytes, so for reliable
	/// reading of >1 bytes of data the return value must be checked against the
	/// number of bytes requested, and if fewer bytes than requested were read
	/// then the call must be tried again.
	///
	/// This function should only be called when Serial.available() is reporting
	/// >0 bytes available.
	///
	/// An example of how to reliably read 8 bytes:
	/// ~~~~{.cpp}
	/// // we want to read 8 bytes
	/// int bytesRequired = 8;
	/// unsigned char bytes[bytesRequired];
	/// int bytesRemaining = bytesRequired;
	/// // loop until we've read everything
	/// while ( bytesRemaining > 0 ){
	///	 // check for data
	///	 if ( serial.available() > 0 ){
	///		 // try to read - note offset into the bytes[] array, this is so
	///		 // that we don't overwrite the bytes we already have
	///		 int bytesArrayOffset = bytesRequired - bytesRemaining;
	///		 int result = serial.readData( &bytes[bytesArrayOffset], bytesRemaining );
	///
	///		 // check for error code
	///		 if ( result == OF_SERIAL_ERROR ){
	///			 // something bad happened
	///			 ofLog( OF_LOG_ERROR, "unrecoverable error reading from serial" );
	///			 break;
	///		 } else if ( result == OF_SERIAL_NO_DATA ){
	///			 // nothing was read, try again
	///		 } else {
	///			 // we read some data!
	///			 bytesRemaining -= result;
	///		 }
	///	 }
	/// }
	/// ~~~~
	/// \returns The number of byte readed. If there is no data it will return
	/// `OF_SERIAL_NO_DATA`, and on error it returns `OF_SERIAL_ERROR`
	///
	/// Single byte
	/// ~~~~{.cpp}
	/// int myByte = mySerial.readData();
	///
	/// if ( myByte == OF_SERIAL_NO_DATA ){
	///	 printf("no data was read");
	/// } else if ( myByte == OF_SERIAL_ERROR ){
	///	 printf("an error occurred");
	/// } else {
	///	 printf("myByte is %d", myByte);
	/// }
	/// \returns The single byte as integer. If there is no data it will return
	/// `OF_SERIAL_NO_DATA`, and on error it returns `OF_SERIAL_ERROR`
	/// ~~~~
	///
	/// Be aware that the type of your buffer can only be unsigned char. If you're
	/// trying to receieve ints or signed chars over a serial connection you'll
	/// need to do some bit manipulation to correctly interpret that values.
	std::string readStringUntil(const char delimiter, const int timeout = 1000);
	long readData(unsigned char* buffer, size_t length);
	long readData(char* buffer, size_t length);
	long readData(std::string& buffer, size_t length);
	int readData();

	/// \}
	/// \name writeData Data
	/// \{

	/// \brief This writes bytes into the serial buffer from the buffer pointer passed in
	///
	/// ~~~~{.cpp}
	/// unsigned char buf_str[3] = {'o', 'f', '!'};
	/// char buf_data[2] = {0x31, 0xDA};
	/// unsigned char data = 0x39;
	///
	/// device.writeData(data);
	/// device.writeData('o');
	/// device.writeData("of!");
	/// device.writeData(&buf_str[0], 3);
	/// device.writeData(&buf_data[0], 2);
	/// ~~~~
	long writeData(const std::string& buffer){
		return writeData(buffer.data(), buffer.size());
	}
	long writeData(const std::string_view buffer);
	bool writeData(const char singleByte);
	bool writeData(const uint8_t singleByte);
	long writeData(const char* buffer, const size_t length);
	long writeData(const uint8_t * buffer, const size_t length);

	/// \}
	/// \name Clear Data
	/// \{

	/// \brief Clears data from one or both of the serial buffers.
	///
	/// Any data in the cleared buffers is discarded.
	/// \param flushIn If true then it clears the incoming data buffer
	/// \param flushOut If true then it clears the outgoing data buffer.
	void flush(const bool flushIn = true, const bool flushOut = true);

	/// \brief Drain is only available on OSX and Linux and is very similar to
	/// flush(), but blocks until all the data has been written to or read
	/// from the serial port.
	void drain();

	/// \}

	bool isBuadLegal(const int baud) const {
		return (std::find(supportedBauds.begin(), supportedBauds.end(), baud)
				!= supportedBauds.end());
	}

	void addLegalBaud(const int baud){
		supportedBauds.push_back(baud);
	}
protected:
	/// \brief Enumerate all devices attached to a serial port.
	///
	/// This method tries to collect basic information about all devices
	/// attached to a serial port.
	/// \see ofSerial::listDevices()
	/// \see enumerateWin32Ports()
	void buildDeviceList();

	std::string deviceType;  ///\< \brief Name of the device on the other end of the serial connection.
	std::vector <ofSerialDeviceInfo> devices;  ///\< This vector stores information about all serial devices found.

	std::vector <int> supportedBauds = {
		300, 
		1200, 
		2400, 
		4800,
		9600,
		14400, 
		19200,
		28800, 
		38400,
		57600,
		115200,
		230400,
		12000000
	};
	
	bool bHaveEnumeratedDevices;  ///\< \brief Indicate having enumerated devices (serial ports) available.
	bool bInited = false;;  ///\< \brief Indicate the successful initialization of the serial connection.

#ifdef TARGET_WIN32

	/// \brief Enumerate all serial ports on Microsoft Windows.
	///
	/// This method tries to collect basic information about all devices
	/// attached to a serial port on Microsoft Windows platforms.
	/// \see ofSerial::listDevices()
	/// \see ofSerial::buildDeviceList()
	void enumerateWin32Ports();

	COMMTIMEOUTS oldTimeout;
		///\< \brief The old serial connection timeout.
		/// This is needed to restore settings on Microsoft Windows
		/// platforms upon closing the serial connection.
	char** portNamesShort;
		///\< \brief Short names of COM ports on Microsoft Windows.
		/// Short names only identify/name the COM port. The length
		/// of the array is limited to MAX_SERIAL_PORTS.
		///\see ofSerial::portNamesFriendly
	char** portNamesFriendly;
		///\< \brief Friendly names of COM ports on Microsoft Windows.
		/// The friendly names often describe/name the device attached to
		/// a particular serial port. The length of the array is limited
		/// to MAX_SERIAL_PORTS.
		///\see ofSerial::portNamesShort
	HANDLE hComm = INVALID_HANDLE_VALUE; ///\< This is the handler for the serial port on Microsoft Windows.
	OVERLAPPED osWriter = { 0 }; ///\< This is the handler for the serial writer OVERLAPPED on Microsoft Windows.
	OVERLAPPED osReader = { 0 }; ///\< This is the handler for the serial reader OVERLAPPED on Microsoft Windows.
	int nPorts;  ///\< \brief Number of serial devices (ports) on Microsoft Windows.
	bool bPortsEnumerated;  ///\< \brief Indicate that all serial ports (on Microsoft Windows) have been enumerated.

#else
	int fd; ///< \brief File descriptor for the serial port.
	struct termios oldoptions;  ///< \brief This is the set of (current) terminal attributes to be reused when changing a subset of options.
#endif

};

//----------------------------------------------------------------------
// this serial code contains small portions of the following code-examples:
// ---------------------------------------------------
// http://todbot.com/arduino/host/arduino-serial/arduino-serial.c
// web.mac.com/miked13/iWeb/Arduino/Serial%20Write_files/main.cpp
// www.racer.nl/docs/libraries/qlib/qserial.htm
// ---------------------------------------------------

// to do:
// ----------------------------
// a) support blocking / non-blocking
// b) support numChars available type functions
// c) can we reduce the number of includes here?

// useful :
//	 http://en.wikibooks.org/wiki/Serial_Programming:Unix/termios
//	 http://www.keyspan.com/downloads-files/developer/win/USBSerial/html/DevDocsUSBSerial.html
// ----------------------------
// (also useful, might be this serial example - worth checking out:
// http://web.mit.edu/kvogt/Public/osrc/src/
// if has evolved ways of dealing with blocking
// and non-blocking instances)
// ----------------------------
