void heating(){
	if (enableHeating == 1)
	{
		if (temp1C > tempUpper){
			n--;
			if(n<=0){
				m1SetSpeed(5);
				n=autoCyckle;
			}
		}

		if (temp1C < tempUpper){
			n=autoCyckle;
		}
	  
		if (temp2C < tempLower && M1Speed==5){
			if(k<=0){
				m1SetSpeed(0);
				k=autoCyckle;
			}
			else{
			k--;
			}
		}
	  
		if (temp2C > tempLower && M1Speed==5){
			k=autoCyckle;
		}
	}
}