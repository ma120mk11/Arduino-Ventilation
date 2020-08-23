void nexButtons_INIT(){
    springHeat.attachPop(springHeatPopCallback, &springHeat); // MENU
//  to_sleep.attachPop(to_sleepPopCallback, &to_sleep);
//  t1.attachPop(t1PopCallback, &t1);


    bMS1.attachPop(bMS1PopCallback, &bMS1);       // Page 4
    bMS2.attachPop(bMS2PopCallback, &bMS2);
    bMS3.attachPop(bMS3PopCallback, &bMS3);
    bMS4.attachPop(bMS4PopCallback, &bMS4);
    bMS5.attachPop(bMS5PopCallback, &bMS5);
    bMS0.attachPop(bMS0PopCallback, &bMS0);
    bM2.attachPop(bM2PopCallback, &bM2);
    
    bM2S1.attachPop(bM2S1PopCallback, &bM2S1);      // Page 5
    bM2S2.attachPop(bM2S2PopCallback, &bM2S2);
    bM2S3.attachPop(bM2S3PopCallback, &bM2S3);
    bM2S4.attachPop(bM2S4PopCallback, &bM2S4);
    bM2S5.attachPop(bM2S5PopCallback, &bM2S5);
    bM2S0.attachPop(bM2S0PopCallback, &bM2S0);
    bM1.attachPop(bM1PopCallback, &bM1);
  
  springExit.attachPop(springExitPopCallback, &springExit);   // PAGE 6 SPRING
  
  
  Dec_Utemp.attachPop(Dec_UtempPopCallback, &Dec_Utemp);
  Inc_Utemp.attachPop(Inc_UtempPopCallback, &Inc_Utemp);
  Dec_Ltemp.attachPop(Dec_LtempPopCallback, &Dec_Ltemp);
  Inc_Ltemp.attachPop(Inc_LtempPopCallback, &Inc_Ltemp);
  
  Measure.attachPop(MeasurePopCallback, &Measure);
  Reset.attachPop(ResetPopCallback, &Reset);
  Update.attachPop(UpdatePopCallback, &Update);
  
  v_err_exit.attachPop(v_err_exitPopCallback, & v_err_exit);
  ignore.attachPop(ignorePopCallback, &exit);
  
  v_errDec.attachPop(v_errDecPopCallback, &v_errDec);     // page 15 - settings 2
  v_errInc.attachPop(v_errIncPopCallback, &v_errInc);


  // *** Update NEXTION start values ***
  sensorRead();               // We need to read values first
  NEXtempThrUpdate();
  NEXsensor_maxUpdate();
  
  // Update voltage thr
  int thr = e_voltageThr*10;
  Serial2.print("settings_2.v_err.val=");
  Serial2.print(thr);  
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}
