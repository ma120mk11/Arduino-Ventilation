
void sensorRead(){
//**** SETTINGS*********

int smoothing = 10; 		  // Sensor smoothing: number of times to read sensor value. 1 = no smoothing
int sDLY = 20;				    // Smoothing delay
const int mVperAmp = 10;	// 20A module

//**** VARIABLES *******
float c = 0;				// For calculation
int n = smoothing;			// For calculation

float t0 = 0;
float t1 = 0;
float t2 = 0;
float t3 = 0;
float v0 = 0;
float i0 = 0;



// store previous values
float t0_1 = t0;
float t1_1 = t1;
float t2_1 = t2;
float t3_1 = t3;
v0_2 = v0_1;
v0_1 = voltage;
i0_2 = i0_1;
i0_1 = current;



float v0Val;
float i0Val;

// *********************** ADC *******************************

	while (n > 0){
		t0 += analogRead(TempSensor0);
		t1 += analogRead(TempSensor1);
		t2 += analogRead(TempSensor2);
		t3 += analogRead(TempSensor3);
		v0 += analogRead(VoltageSensor);	// Voltage
		i0 += analogRead(CurrentSensor);	// Current
		
		n--;
		delay(sDLY);
	}
	
	n = smoothing;			// Restore n value
	// Calculate mean value
	t0 = t0 / n;
	t1 = t1 / n;
	t2 = t2 / n;
	t3 = t3 / n;
	v0 = v0 / n;
	i0 = i0 / n;


//************************ SENSOR 0 OUTSIDE ******************
/*  	// convert to celsius:
    c = t0 * pinReference;				// Convert readings to mV
    c = c / 1024.0;						
    c = c / 0.01;
    c = c + t0sensorOffset;
	temp0C = round(c * 10.0) / 10.0;	// Round to 1 decimal. round() rounds decimal number to whole number. 


	// Update max value
	if (temp0C > temp0C_max){
		temp0C_max = temp0C;
	}   */
	
// **** With OP-amp ****
	t0sensorOffset = 0;
	float gain=10.9395;
	c = t0 * pinReference;				// Convert readings to mV
    c = c / 1024.0;						
    c = (c / 0.01)/gain;
    c = c + t0sensorOffset;
	temp0C = c; //round(c * 10.0) / 10.0;
	
	
	
	
//************************  SENSOR 1 PANEL  ****************** 
	
    // convert to celsius:
    c = t1 * pinReference;				// Convert readings to mV
    c = c / 1024.0;						
    c = c / 0.01;
    c = c + t1sensorOffset;
	temp1C = round(c * 10.0) / 10.0;	// Round to 1 decimal. round() rounds decimal number to whole number. 


	// Update max value
	if (temp1C > temp1C_max){
		temp1C_max = temp1C;
	}
	
// ********************* SENSOR 2 HEATED AIR ****************** 
    // convert to celsius:
    c = t2 * pinReference;				// Convert readings to mV
    c = c / 1024.0;						
    c = c / 0.01;
    c = c + t2sensorOffset;
	temp2C = round(c * 10.0) / 10.0;	// Round to 1 decimal. round() rounds decimal number to whole number. 


	// Update max value only if motor is running
	if (temp2C > temp2C_max && m1Running==1){
		temp2C_max = temp2C;
	}

// ********************* SENSOR 3 INSIDE  ********************* 
  // convert to celsius:
    c = t3 * pinReference;				// Convert readings to mV
    c = c / 1024.0;						// 
    c = (c / 0.01)/gain;
    temp3C = c + t3sensorOffset;
//	temp3C = round(c * 10.0) / 10.0;	// Round to 1 decimal. round() rounds decimal number to whole number. 


	// Update max value
	if (temp3C > temp3C_max){
		temp3C_max = temp3C;
	}
 
// *********************** VOLTAGE ***************************
	// Convert to voltage
	c = v0*pinReference;
	c = c / 1024.0;
	c = c * 5.0;
	// filter the signal
	voltage = filter(c,v0_1,v0_2,0.7);	
	//voltage = round(c * 10.0) / 10.0;
	
	// Update min value
	if (voltage < voltage_min){
			voltage_min = voltage;
	}
	
	// Update max value
	if (voltage > voltage_max){
		voltage_max = voltage;
	}
	
//	*********************** CURRENT ***************************
/* 	Example code:
	https://wiki.dfrobot.com/20A_Current_Sensor_SKU_SEN0214#More */
	
	// The motor controller board cunsumes 65mA in standby.
	
	c = i0*pinReference;
	c = c / 1024.0;
	c = c - pinReference / 2.0;		// Adjust for 0A
	c = c * mVperAmp;
	current = c;
	//current = c 	//round(c * 10.0) / 10.0;
	filteredSignal = filter(c, i0_1, i0_2,0.7);
	
	
	// Update max value
	if (current > current_max){
		current_max = current;
	}


//	*********************** TEMP DELTA ***************************
	if(m1Running==1){
		tempDelta = temp2C-temp0C;
	}
	else if(m1Running==0){
		tempDelta = 0;
	}
	// Update max value
	if(tempDelta>tempDelta_max){
		tempDelta_max=tempDelta;
	}
		
//	*********************** SERIAL PRINT ***************************

   if (enableSerialPrint == 1){
	   
	   
	char outside_text[30];
	char panel_text[30];
	char air_text[30];
	char room_text[30];
	char filtered_text[30];
	char voltage_text[30];
	char current_text[30];
	char tdelta_text[30];
	char m1speed_text[30];

	dtostrf(temp0C, 10, 10, outside_text);
	dtostrf(temp1C, 10, 10, panel_text);
	dtostrf(temp2C, 10, 10, air_text);
	dtostrf(temp3C, 10, 10, room_text);
	dtostrf(filteredSignal, 10, 10, filtered_text);
	dtostrf(voltage, 10, 10, voltage_text);
	dtostrf(current, 10, 10, current_text);
	dtostrf(tempDelta, 10, 10, tdelta_text);
	dtostrf(M1Speed, 10, 10, m1speed_text);

	char text[280];
	snprintf(text, 280, "%s,%s,%s,%s,%s,%s,%s,%s,%s", outside_text, panel_text, air_text, room_text, voltage_text, current_text,filtered_text,tdelta_text,m1speed_text);
	Serial.println(text);
	   
	   
/* 	char values[40];
	sprintf(values, "%d,%d,%d,%d,%d,%d,%d\n", temp0C, temp1C, temp2C, temp3C, voltage, current, filteredSignal);
	Serial.println(values);
	    */
	// For arduino serial plotter:   
/*  		Serial.print (temp0C);
		Serial.print (",");
		Serial.print (temp1C);
		Serial.print (",");
		Serial.print (temp2C);
		Serial.print (",");
		Serial.print (temp3C);
		Serial.print (",");
		Serial.print (voltage);
		Serial.print (",");
		Serial.print (current);
		Serial.print (",");
		Serial.println ("filteredSignal"); */
	}	
	
	return;
}

void sensor_maxReset(){
	temp0C_max = temp0C;
	temp1C_max = temp1C;
	temp2C_max = temp2C;
	temp4C_max = temp4C;	
	voltage_min = voltage;
	voltage_max = voltage;
	current_max = current;
}
