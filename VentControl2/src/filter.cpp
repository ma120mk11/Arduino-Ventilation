
float filter(float i, float n, float d, float a){
	// a is smoothing factor( normally 0 - 1 )
	float y = 0; 	
	//	float a = 0.6;
	
	y = (1.0-a)*i + ((1.0-a)*a)*n + a*a*d;
	
	return y;
}