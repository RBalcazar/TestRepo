#include "yei_threespace_api.h"
#include <stdio.h>
#include <string>
#include <iostream>

using std::string;

/** Find's active YEI devices connected to COM ports.**/
int FindAllComPorts(int ports[]){
	TSS_ComPort comport;
	int offset = 0;
	// API 2.0 CHANGE!!!!!!
	// TSS_FIND_ALL_KNOWN is all known YEI comport devices
	// TSS_FIND_ALL is all comports including TSS_FIND_UNKNOWN
	while (tss_getComPorts(&comport, 1, offset, TSS_FIND_ALL_KNOWN)){
		/*	device = tss_createTSDeviceStr(comport.com_port, TSS_TIMESTAMP_SENSOR);*/
		std::cout << "Current offset:" << offset << " port: " << comport.com_port<< " Friendly Name: "<< comport.friendly_name << std::endl;
		ports[offset] = atoi(&comport.com_port[3]);
		
		offset++;
	}

	return offset;
}

/** Connect to a Specific Comport **/
UINT ConnectToComPort(UINT port){
	TSS_Device_Id  device;
	string com_port = "COM";
	com_port.append(std::to_string(port));

	device = tss_createTSDeviceStr(com_port.c_str(), TSS_TIMESTAMP_SENSOR);
	if (device == TSS_NO_DEVICE_ID){
		return TSS_ERROR_COMMAND_FAIL;
	}
	else{
		return device;
	}
}

/**Returns orientation of a given sensor as a quaternion.**/
 UINT GetOrientationAsQuaternion(UINT deviceID, float quat[]){


	TSS_Error tss_error;
	UINT timestamp;
	tss_error = tss_getTaredOrientationAsQuaternion(deviceID, quat, &timestamp);

	if (!tss_error){
		return timestamp;
	}
	else{
		return tss_error;
	}
}

 /**Shows general methods of connecting and getting data from sensors using YEI's API.
 * Function is self contained.
 */
int creating_class_instances(){
	// If the COM port is already known and the device type is known for the 3-Space
	// Sensor device, we can just create the appropriate instance without doing 
	// a search.
	TSS_Device_Id  device;
	
	string port = "COM";
	unsigned int portNum = 3;
	
	port.append(std::to_string(portNum));
	
	
	
	const char * com_port = port.c_str();
	printf("====Creating TS Devices on %s ====\n", port.c_str());
	device = tss_createTSDeviceStr(port.c_str(), TSS_TIMESTAMP_SENSOR);
	if (device == TSS_NO_DEVICE_ID){
		printf("Could not create device\n");
	}
	else{
		TSS_ComInfo com_info;
		if (tss_getTSDeviceInfo(device, &com_info) == TSS_NO_ERROR){
			printf("============(%s)=============\n", com_port);
			printf("DeviceID:%u\nDeviceType:%s\nSerial:%08X\nHardwareVersion:%s\nFirmwareVersion:%s\nCompatibility:%d\n",
				device,
				TSS_Type_String[com_info.device_type],
				com_info.serial_number,
				com_info.hardware_version,
				com_info.firmware_version,
				com_info.fw_compatibility);
			printf("================================\n");
		}
	}
	tss_closeTSDevice(device);

	// Single comport var, loop through all comports (getfirst get next equivalent)

	// Gets the comports of YEI devices and other available com ports
	//  @param ports_vec the list(or single) of comport structures to return the results
	//  @param ports_vec_size the size of the list entered
	//  @param offset this allows you to call this function multiple times to get all the comports, increment the offest to continue getting more devices
	//  @param filter pass in a combination of TSS_Find enums you want to search for
	//  @return the number of comports found or equal to the size of ports_vec_size
	printf("====Creating Known TS Devices (single comport var) ====\n");
	TSS_ComPort comport;
	int offset = 0;
	// API 2.0 CHANGE!!!!!!
	// TSS_FIND_ALL_KNOWN is all known YEI comport devices
	// TSS_FIND_ALL is all comports including TSS_FIND_UNKNOWN
	
	while (tss_getComPorts(&comport, 1, offset, TSS_FIND_ALL_KNOWN)){
		//printf("While Loop getComports %d\n", offset);
		device = tss_createTSDeviceStr(comport.com_port, TSS_TIMESTAMP_SENSOR);
		if (device == TSS_NO_DEVICE_ID){
			printf("Failed to create a sensor on %s\n", comport.com_port);
			//return 1;
		}
		else{
			TSS_ComInfo com_info;
			if (tss_getTSDeviceInfo(device, &com_info) == TSS_NO_ERROR){
				printf("============(%s)=============\n", comport.com_port);
				printf("DeviceID:%x\nDeviceType:%s\nSerial:%08X\nHardwareVersion:%s\nFirmwareVersion:%s\nCompatibility:%d\n",
					device,
					TSS_Type_String[com_info.device_type],
					com_info.serial_number,
					com_info.hardware_version,
					com_info.firmware_version,
					com_info.fw_compatibility);
				printf("================================\n");

				unsigned char enabled;
				unsigned int timestamp;
				tss_getGyroscopeEnabledState(device, &enabled, &timestamp);
				printf("Gyroscope Enabled State: %d\n", enabled);
				int g_activated = enabled == 0 ?  0: 1;
				printf("g_activated: %d\n", g_activated);

				tss_setGyroscopeEnabled(device, 0, &timestamp);
				tss_getGyroscopeEnabledState(device, &enabled, &timestamp);
				printf("Gyroscope Enabled State: %d\n", enabled);
				g_activated = enabled == 0 ? 0 : 1;
				printf("g_activated: %d\n", g_activated);


				//enabled = 1;
				//tss_setGyroscopeEnabled(device, enabled, &timestamp);
				//tss_getGyroscopeEnabledState(device, &enabled, &timestamp);
				//printf("Gyroscope Enabled State: %d\n", enabled);


				if (device & TSS_WIRELESS_ID)
					printf("Wireless found\n");


			}
		}
		printf("tss_getComPorts returns: %d before tss_closeDevice\n", tss_getComPorts(&comport, 1, offset, TSS_FIND_ALL_KNOWN));
		tss_closeTSDevice(device);
		printf("tss_getComPorts returns: %d after tss_closeDevice\n", tss_getComPorts(&comport, 1, offset, TSS_FIND_ALL_KNOWN));
		offset++;
	}

	printf("====Creating Known TS Devices ( comport array) ====\n");
	TSS_ComPort comport_list[20];
	int device_count = tss_getComPorts(comport_list, 20, 0, TSS_FIND_ALL);
	printf("Found %d Devices\n", device_count);
	for (int i = 0; i< device_count; ++i){
		device = tss_createTSDeviceStr(comport_list[i].com_port, TSS_TIMESTAMP_SENSOR);
		if (device == TSS_NO_DEVICE_ID){
			printf("Failed to create a sensor on %s\n", comport_list[i].com_port);
			//return 1;
		}
		else{
			TSS_ComInfo com_info;
			if (tss_getTSDeviceInfo(device, &com_info) == TSS_NO_ERROR){
				printf("============(%s)=============\n", comport_list[i].com_port);
				printf("DeviceType:%s\nSerial:%08X\nHardwareVersion:%s\nFirmwareVersion:%s\nCompatibility:%d\n",
					TSS_Type_String[com_info.device_type],
					com_info.serial_number,
					com_info.hardware_version,
					com_info.firmware_version,
					com_info.fw_compatibility);
				printf("================================\n");
			}
			else{
				printf("Error Occured calling tss_getTSDeviceInfo\n");
			}
		}
		tss_closeTSDevice(device);
	}

	// This will write bytes to serial ports that are not recognized as 3-Space virtual comports. 
	// If the 3-Space sensors is connected via a serial to usb adapter or a physical serial port this will allow you to get information on what kind of sensor is connected and other useful information
	printf("====Checking Unknown Devices  ====\n");
	device_count = tss_getComPorts(comport_list, 20, 0, TSS_FIND_UNKNOWN);
	printf("Found %d Devices\n", device_count);
	for (int i = 0; i< device_count; ++i){
		TSS_ComInfo com_info;
		TSS_Error error = tss_getTSDeviceInfoFromComPort(comport_list[i].com_port, &com_info);
		if (error == TSS_NO_ERROR){
			printf("============(%s)=============\n", comport_list[i].com_port);
			printf("DeviceType:%s\nSerial:%08X\nHardwareVersion:%s\nFirmwareVersion:%s\nCompatibility:%d\n",
				TSS_Type_String[com_info.device_type],
				com_info.serial_number,
				com_info.hardware_version,
				com_info.firmware_version,
				com_info.fw_compatibility);
			printf("================================\n");
		}
		else{
			printf("Failed to communicate on %s\n", comport_list[i].com_port);
		}
	}
	printf("Finished press Enter to continue");
	getchar();
	return 0;


}

/*Further examples of getting data after a device has been connected. .
 * Function is self contained.
 */
int getting_information_wireless(){

	TSS_Device_Id  device;
	TSS_Device_Id  d_device;
	TSS_Error tss_error;
	TSS_ComPort comport;

	if (tss_getComPorts(&comport, 1, 0, TSS_FIND_DNG)){
		d_device = tss_createTSDeviceStr(comport.com_port, TSS_TIMESTAMP_SENSOR);
		if (d_device == TSS_NO_DEVICE_ID){
			printf("Failed to create a sensor on %s\n", comport.com_port);
			return 1;
		}
		tss_getSensorFromDongle(d_device, 0, &device);
		if (device == TSS_NO_DEVICE_ID){
			printf("Failed to create a wireless sensor on\n");
			return 1;
		}

	}
	else{
		printf("No sensors found\n");
		return 1;
	}
	printf("==================================================\n");
	printf("Getting the filtered tared quaternion orientation.(xyzw)\n");
	float quat[4];
	unsigned int timestamp;
	tss_error = tss_getTaredOrientationAsQuaternion(device, quat, &timestamp);
	if (!tss_error){
		printf("Quaternion: %f, %f, %f, %f Timestamp=%u\n", quat[0], quat[1], quat[2], quat[3], timestamp);
	}
	else{
		printf("TSS_Error: %s\n", TSS_Error_String[tss_error]);
	}
	printf("==================================================\n");
	printf("Getting the Corrected Component Sensor Data.\n");
	float gyro[3];
	float accel[3];
	float compass[3];
	tss_error = tss_getAllCorrectedComponentSensorData(device, gyro, accel, compass, NULL);
	if (!tss_error){
		printf("Gyro:  %f, %f, %f\n", gyro[0], gyro[1], gyro[2]);
		printf("Accel: %f, %f, %f\n", accel[0], accel[1], accel[2]);
		printf("Comp:  %f, %f, %f\n", compass[0], compass[1], compass[2]);
	}
	else{
		printf("TSS_Error: %s\n", TSS_Error_String[tss_error]);
	}
	printf("==================================================\n");
	printf("Getting the LED color of the device.\n");
	float color[3];
	tss_error = tss_getLEDColor(device, color, NULL);
	if (!tss_error){
		printf("Color: %f, %f, %f\n", color[0], color[1], color[2]);
	}
	else{
		printf("TSS_Error: %s\n", TSS_Error_String[tss_error]);
	}
	printf("==================================================\n");
	
	printf("==================================================\n");
	printf("Getting the Temperature of the device.\n");
	float temperature[3];
	tss_error = tss_getTemperatureC(device, temperature, NULL);
	if (!tss_error){
		printf("Temperature : %f C\n", temperature[0]);
	}
	else{
		printf("TSS_Error: %s\n", TSS_Error_String[tss_error]);
	}
	printf("==================================================\n");
	
	while (getchar() != 'f'){
		printf("==================================================\n");
		printf("Getting the button state of the device.\n");
		unsigned char button_state;
		tss_error = tss_getButtonState(device, &button_state, &timestamp);
		if (!tss_error){
			printf("Button STate : %u \n", button_state);
		}
		else{
			printf("TSS_Error: %s\n", TSS_Error_String[tss_error]);
		}
		printf("==================================================\n");
	}
	tss_closeTSDevice(device);


	printf("Finished press Enter to continue");
	getchar();
	return 0;


}


int main(){
	//creating_class_instances();
	//getting_information_wireless();


	//test FindAll Comports

	const int size = 20; 
	int ports[size] = {}; //look for YEI devices in the first 10 COM ports

	int numDevices = FindAllComPorts(ports);
	std::cout << "Found " << numDevices << " YEI Sensors" << std::endl;
	for (int i = 0; i < size; i++)
	{
		std::cout << "Ports[" << i << "] = " << ports[i] << std::endl; 

		if (ports[i] != 0)
		{
			std::cout << "YEI Device Connected at COM PORT " << ports[i] << "\n Attempting to connect...... "<< std::endl;

			unsigned int device = ConnectToComPort(ports[i]);
			
			if (device && TSS_ALL_SENSORS_ID)
				std::cout << "Successfully connected to YEI Device" << std::endl;
			
			float quat[4];

			if (GetOrientationAsQuaternion(device, quat) < 20);
			continue;	

			printf("Orientation of Sensor at COM Port %d is: X=%f,Y=%f,Z=%f,W=%F",ports[i], quat[0],quat[1],quat[2], quat[3]);		

			tss_closeTSDevice(device);
		}
		else
		{
			
		}

	}	

	getchar();
	return 0;
}