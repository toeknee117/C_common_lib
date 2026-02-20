//  Developed Libraries
#include "GPIO_common.h"

//  Global Static Variables
static uint8_t errorArray[120] = {0};                                                               //  Error array to help print specific function

/*
    Function: Export GPIO pin to allow for use
    gpio_pin: GPIO pin that will be used
*/
int32_t gpio_export(uint16_t gpio_pin) {
	uint8_t exportArray[120] = {0};																	//	Initialize Export Array
	snprintf(exportArray, sizeof(exportArray), "echo %hu > /sys/class/gpio/export", gpio_pin);     	//  Populate Export Array
	system(exportArray);																			//	Use CLI to echo command
    return 1;                                                                                       //  Return good
}

/*
    Function: Change Direction of GPIO pin
    direction: Direction that will be used
*/
int32_t gpio_direction(uint16_t gpio_pin, const uint8_t *direction) {
	if (strcmp(direction, "out") && strcmp(direction, "in")) {
		snprintf(errorArray, sizeof(errorArray), "%s: Invalid Direction\n", __FUNCTION__);          //  Populate Error Array
        printf("%s", errorArray);                                                                   //  Print out this if it failed
        return -1;                                                                                  //  Return error
	}

	uint8_t dir_file[120] = {0};																	//	Initialize Direction File
	snprintf(dir_file, sizeof(dir_file), "/sys/class/gpio/gpio%hhu/direction", gpio_pin);   		//  Populate Direction File
	FILE* direction_fd = fopen(dir_file, "r");														//	Open file as read only
	if (!direction_fd) {
		snprintf(errorArray, sizeof(errorArray), "%s: Could not open direction\n", __FUNCTION__);   //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
	}
	uint8_t direction_str[100] = {0};																//	Initialize direction string
    if (!fgets(direction_str, sizeof(direction_str), direction_fd)) {								//	Populate direction string
		fclose(direction_fd);																		//	Close file
        snprintf(errorArray, sizeof(errorArray), "%s: fgets\n", __FUNCTION__);   					//  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    fclose(direction_fd);																			//	Close file

	uint8_t echo_dir[120] = {0};																	//	Initialize echo direction
	if (!strcmp(direction, "out") && strcmp(direction_str, "out\n")) {								//	If direction is out and direction string is not out
		snprintf(echo_dir, sizeof(echo_dir), "echo out > /sys/class/gpio/gpio%hhu/direction", gpio_pin);	//  Populate echo direction
		system(echo_dir);																			//	Use CLI to echo command
		
	}
	if (!strcmp(direction, "in") && strcmp(direction_str, "in\n")) {								//	If direction is out and direction string is not out
		snprintf(echo_dir, sizeof(echo_dir), "echo in > /sys/class/gpio/gpio%hhu/direction", gpio_pin);	//  Populate echo direction
		system(echo_dir);																			//	Use CLI to echo command
	}
	
	
    return 1;                                                                                       //  Return good
}

/*
    Function: Write value to GPIO pin
    value: value that will be used
*/
int32_t gpio_write(uint16_t gpio_pin, uint8_t value) {
	if (value != 0 && value != 1) {
		snprintf(errorArray, sizeof(errorArray), "%s: Invalid Value\n", __FUNCTION__);        		//  Populate Error Array
        printf("%s", errorArray);                                                                   //  Print out this if it failed
        return -1;                                                                                  //  Return error
	}

	uint8_t val_file[120] = {0};																	//	Initialize Value File
	snprintf(val_file, sizeof(val_file), "/sys/class/gpio/gpio%hhu/value", gpio_pin);   			//  Populate Value File
	FILE* value_fd = fopen(val_file, "r");															//	Open file as read only
	if (!value_fd) {
		snprintf(errorArray, sizeof(errorArray), "%s: Could not open value\n", __FUNCTION__);   	//  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
	}
	uint8_t value_str[100] = {0};																	//	Initialize value string
    if (!fgets(value_str, sizeof(value_str), value_fd)) {											//	Populate value string
		fclose(value_fd);																			//	Close file
        snprintf(errorArray, sizeof(errorArray), "%s: fgets\n", __FUNCTION__);   					//  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    fclose(value_fd);																				//	Close file

	uint8_t echo_val[120] = {0};																	//	Initialize echo value
	if (value == 0 && strcmp(value_str, "0\n")) {													//	If value is out and value string is not out
		snprintf(echo_val, sizeof(echo_val), "echo 0 > /sys/class/gpio/gpio%hhu/value", gpio_pin);	//  Populate echo value
		system(echo_val);																			//	Use CLI to echo command
		
	}
	if (value == 1 && strcmp(value_str, "1\n")) {													//	If value is out and value string is not out
		snprintf(echo_val, sizeof(echo_val), "echo 1 > /sys/class/gpio/gpio%hhu/value", gpio_pin);	//  Populate echo value
		system(echo_val);																			//	Use CLI to echo command
	}
	
	
    return 1;                                                                                       //  Return good
}

/*
    Function: Read value to GPIO pin
    value: value that will be used
*/
int32_t gpio_read(uint16_t gpio_pin) {
	uint8_t val_file[120] = {0};																	//	Initialize Value File
	snprintf(val_file, sizeof(val_file), "/sys/class/gpio/gpio%hhu/value", gpio_pin);   			//  Populate Value File
	FILE* value_fd = fopen(val_file, "r");															//	Open file as read only
	if (!value_fd) {
		snprintf(errorArray, sizeof(errorArray), "%s: Could not open value\n", __FUNCTION__);   	//  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
	}
	uint8_t value_str[100] = {0};																	//	Initialize value string
    if (!fgets(value_str, sizeof(value_str), value_fd)) {											//	Populate value string
		fclose(value_fd);																			//	Close file
        snprintf(errorArray, sizeof(errorArray), "%s: fgets\n", __FUNCTION__);   					//  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    fclose(value_fd);																				//	Close file

	if (!strcmp(value_str, "0\n")) {																//	If value is out and value string is not out
		return 0;																					//	Return 0
		
	}
	if (!strcmp(value_str, "1\n")) {																//	If value is out and value string is not out
		return 1;																					//	Return 1
	}
	
    return -1;                                                                                       //  Return bad
}