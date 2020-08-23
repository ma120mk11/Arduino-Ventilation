void m2SetSpeed(int s){    
/*  Input desired motor speed value 0-5.
 *  The function updates the following global variables: 
 *      M2Speed, M2PWM
		
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
    M2Speed = 5;                     // Update global variable
    sysValUpdate();                  // Updates Display
    
      while( M2PWM < s5){             
        M2PWM=M2PWM+5;
        analogWrite(Motor2, M2PWM);
        delay(dly);                     // optional delay
      }  
  return;   
  } // end of 5

 
  if ( s == 4 ){
    M2Speed = 4;
    sysValUpdate();
    if(M2PWM > s4){                 // If current motor speed is higher, slow down
        while( M2PWM > s4 ){
          M2PWM=M2PWM-5;              // decrease speed by 5
          analogWrite(Motor2, M2PWM);
          delay(dly);                 //optional delay
        }
    }
     if(M2PWM < s4){                // if current speed is lower, speed up
        while( M2PWM < s4){
          M2PWM=M2PWM+5;              // increase speed by 5
          analogWrite(Motor2, M2PWM);
          delay(dly);                 //optional delay
        }
     }
  return;
  } // end of 4


  if ( s == 3 ){
    M2Speed = 3;
    sysValUpdate();
    if(M2PWM > s3){                 // If current motor speed is higher, slow down
        while( M2PWM > s3 ){
          M2PWM=M2PWM-5;              // decrease speed by 5
          analogWrite(Motor2, M2PWM);
          delay(dly);                  //optional delay
        }
    }
     if(M2PWM < s3){                // if current speed is lower, speed up
        while( M2PWM < s3){
          M2PWM=M2PWM+5;              // increase speed by 5
          analogWrite(Motor2, M2PWM);
          delay(dly);                  //optional delay
        }
     }
  return;  
  } // end of 3


  if ( s == 2 ){
      M2Speed = 2;
       sysValUpdate();
      if(M2PWM > s2){                 // If current motor speed is higher, slow down
          while( M2PWM > s2 ){
            M2PWM=M2PWM-5;              // decrease speed by 5
            analogWrite(Motor2, M2PWM);
            delay(dly);                 //optional delay
          }
      }
       if(M2PWM < s2){                // if current speed is lower, speed up
          while( M2PWM < s2){
            M2PWM=M2PWM+5;              // increase speed by 5
            analogWrite(Motor2, M2PWM);
            delay(dly);                 //optional delay
          }
       }
	return;   
    } // end of 2
  

  if ( s == 1 ){
    M2Speed = 1;
    sysValUpdate();
    if(M2PWM > s1){                 // If current motor speed is higher, slow down
        while( M2PWM > s1 ){
          M2PWM=M2PWM-5;              // decrease speed by 5
          analogWrite(Motor2, M2PWM);
          delay(dly);                  //optional delay
        }
    }
     if(M2PWM < s1){                // if current speed is lower, speed up
        while( M2PWM < s1){
          M2PWM=M2PWM+5;              // increase speed by 5
          analogWrite(Motor2, M2PWM);
          delay(dly);                 //optional delay
        }
     }
  return;
  } // end of 1


  if ( s == 0 ){
      M2Speed = 0;                     // Update global variable
      sysValUpdate();
        while( M2PWM > 15){            // Value after it goes to zero  
          M2PWM=M2PWM-5;
          analogWrite(Motor2, M2PWM);
          delay(dly);                      // optional delay
        }
      M2PWM=0;
      analogWrite(Motor2, M2PWM);
	return;   
    }
    
} // END OF m2SetSpeed()

