void m1SetSpeed(int s){    
/*  Input desired motor speed value 0-5.
    The function updates the following global variables: 
       M1Speed, M1PWM
		
		Calls sysValUpdate() to update display
		
		
	It increases or decreases the motor speed gradually until it reaches the value.
 */
 
  // PWM SETTINGS :
  
  int s1 = 70;				// motor PWM value at setting 1
  int s2 = 100;				// motor PWM value at setting 2
  int s3 = 150;				// motor PWM value at setting 3
  int s4 = 210;				// motor PWM value at setting 4
  int s5 = 255;				// motor PWM value at setting 5


  
  int dly = 30;            	// How fast the motor goes from value to another

  
  if ( s == 5 ){
    M1Speed = 5;                     // Update global variable
	m1Running = 1;					// Update global variable	
    sysValUpdate();                  // Updates Display
    
      while( M1PWM < s5){             
        M1PWM=M1PWM+5;
        analogWrite(Motor1, M1PWM);
        delay(dly);                  // optional delay
      }  
  return;   
  } // end of 5

 
  if ( s == 4 ){
    M1Speed = 4;
	m1Running = 1;					// Update global variable		
    sysValUpdate();
    if(M1PWM > s4){                 // If current motor speed is higher, slow down
        while( M1PWM > s4 ){
          M1PWM=M1PWM-5;              // decrease speed by 5
          analogWrite(Motor1, M1PWM);
          delay(dly);                 //optional delay
        }
    }
     if(M1PWM < s4){                // if current speed is lower, speed up
        while( M1PWM < s4){
          M1PWM=M1PWM+5;              // increase speed by 5
          analogWrite(Motor1, M1PWM);
          delay(dly);                 //optional delay
        }
     }
  return;
  } // end of 4


  if ( s == 3 ){
    M1Speed = 3;
	m1Running = 1;					// Update global variable		
    sysValUpdate();
    if(M1PWM > s3){                 // If current motor speed is higher, slow down
        while( M1PWM > s3 ){
          M1PWM=M1PWM-5;              // decrease speed by 5
          analogWrite(Motor1, M1PWM);
          delay(dly);                  //optional delay
        }
    }
     if(M1PWM < s3){                // if current speed is lower, speed up
        while( M1PWM < s3){
          M1PWM=M1PWM+5;              // increase speed by 5
          analogWrite(Motor1, M1PWM);
          delay(dly);                  //optional delay
        }
     }
  return;  
  } // end of 3


  if ( s == 2 ){
    M1Speed = 2;
	m1Running = 1;					// Update global variable		  
    sysValUpdate();
      if(M1PWM > s2){                 // If current motor speed is higher, slow down
          while( M1PWM > s2 ){
            M1PWM=M1PWM-5;              // decrease speed by 5
            analogWrite(Motor1, M1PWM);
            delay(dly);                 //optional delay
          }
      }
       if(M1PWM < s2){                // if current speed is lower, speed up
          while( M1PWM < s2){
            M1PWM=M1PWM+5;              // increase speed by 5
            analogWrite(Motor1, M1PWM);
            delay(dly);                 //optional delay
          }
       }
	return;   
    } // end of 2
  

  if ( s == 1 ){
    M1Speed = 1;
	m1Running = 1;					// Update global variable	
    sysValUpdate();
    if(M1PWM > s1){                 // If current motor speed is higher, slow down
        while( M1PWM > s1 ){
          M1PWM=M1PWM-5;              // decrease speed by 5
          analogWrite(Motor1, M1PWM);
          delay(dly);                  //optional delay
        }
    }
     if(M1PWM < s1){                // if current speed is lower, speed up
        while( M1PWM < s1){
          M1PWM=M1PWM+5;              // increase speed by 5
          analogWrite(Motor1, M1PWM);
          delay(dly);                 //optional delay
        }
     }
  return;
  } // end of 1


  if ( s == 0 ){
    M1Speed = 0;                     // Update global variable
	m1Running = 0;					// Update global variable	
    sysValUpdate();
        while( M1PWM > 15){            // Value after it goes to zero  
          M1PWM=M1PWM-5;
          analogWrite(Motor1, M1PWM);
          delay(dly);                      // optional delay
        }
    M1PWM=0;
    analogWrite(Motor1, M1PWM);
	return;   
    }
} // END OF m1SetSpeed()
