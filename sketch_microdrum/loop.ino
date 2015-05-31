//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     LOOP
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void loop()
{
  //simpleSysex(0xFF,Mode,0x00,0x00);
  USB_Input();
  #if MENU
  Menu();
  #endif
   
  if(Mode==OffMode)
  {
    #if LICENSE
    //HANDSHAKE
    CheckLicense();
    delay(100);
    #endif
    //Mode=StandbyMode;
    //NSensor=2;
    return;
  }
  
  //==========UNROLLING======
  //{0, 1, 3, 2, 6, 7, 5, 4}
  //=========================
  
  //0
  fastWrite(2,0);/*fastWrite(3,0);fastWrite(4,0);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,0); }
  
  //1
  /*fastWrite(2,0);fastWrite(3,0);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,1); }

  //3
  /*fastWrite(2,0);*/fastWrite(3,1);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,3); }
  
  //2
  /*fastWrite(2,0);fastWrite(3,1);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,2); }

  //6
  fastWrite(2,1);/*fastWrite(3,1);fastWrite(4,0);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,6); }
  
  //7
  /*fastWrite(2,1);fastWrite(3,1);*/fastWrite(4,1);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,7); }
  
  //5
  /*fastWrite(2,1);*/fastWrite(3,0);/*fastWrite(4,1);*/
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,5); }
  
  //4
  /*fastWrite(2,1);fastWrite(3,0);*/fastWrite(4,0);
  delayMicroseconds(delayTime);
  for(byte Sensor=0;Sensor<NSensor;Sensor++){ analogRead(Sensor); fastCheckMulti(Sensor,4); }

  //===============================
  if (Mode==StandbyMode) return;
  //===============================

  //Time=TIMEFUNCTION;
  for(byte i=0;i<(NSensor*8);i++)
  {
    byte TS=TypeSensor[i];
    //===============================
    //        Disabled, HHC
    //===============================
    if(TS==Disabled || TS==HHC) continue;
 
    //===============================
    //        HH
    //===============================
    if(TS==HH)
    {
      byte Hhc=DualSensor(i);
      
      //Foot Splash
      if(StateSensor[Hhc]==FOOTSPLASH_TIME)
      {
        if (Mode==MIDIMode) 
        {
        #if USB_MIDI                      //////////////////////////////////////////////////////////////////////
        usbMIDI.sendNoteOn(HHFootNoteSensor[0],127,ChannelSensor[i]); 
        #else 
        MIDI.sendNoteOn(HHFootNoteSensor[0],127,ChannelSensor[i]);
        #endif///////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        StateSensor[Hhc]=0;}
      }
      else if(StateSensor[Hhc]==FOOTCLOSE_TIME)
      {
        if (Mode==MIDIMode) 
        {
        #if USB_MIDI ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        usbMIDI.sendNoteOn(HHFootNoteSensor[1],127,ChannelSensor[i]); 
        #else 
        MIDI.sendNoteOn(HHFootNoteSensor[1],127,ChannelSensor[i]);
        
        #endif///////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        StateSensor[Hhc]=0;}
      }
      continue;
    }
    //===============================
    
    byte SS=StateSensor[i];
    if(SS==NORMAL_TIME || SS==SCAN_TIME || SS==MASK_TIME || SS==RETRIGGER_TIME) continue;

    //=============XTALK==================
      if(MaxMultiplexerXtalk[i%8]!=-1 && MaxMultiplexerXtalk[i%8]>(2*MaxReadingSensor[i]))  //Multiplexer XTalk
      {
        MaxReadingSensor[i]=-1;
        continue;
      }
      if(MaxXtalkGroup[XtalkGroupSensor[i]]!=-1 && MaxXtalkGroup[XtalkGroupSensor[i]]>(MaxReadingSensor[i]+(64-XtalkSensor[i])*4))
      {
        MaxReadingSensor[i]=-1;
        continue;
      }  
      PlaySensorMIDIMode(i);

  }
  //RESET XTALK
  for(int i=0;i<8;i++)
    MaxMultiplexerXtalk[i]=MaxXtalkGroup[i]=-1;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    PLAYSENSOR MIDIMODE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void PlaySensorMIDIMode(byte i)
{
  //===============================
  //        Single Switch
  //===============================
  if(TypeSensor[i]==SWITCH)
  { 
    /*if(TypeSensor[DualSensor(i)]==SWITCH) //Switch-Switch
    { 
      MIDI.sendNoteOn(ChannelSensor[i],NoteSensor[i],MaxReadingSensor[i]/8);
    
      #if WAVTRIGGER
      wavTrigger(i,MaxReadingSensor[i]/8);
      #endif
            
      MaxReadingSensor[i] = -1;
    }
    else if(TypeSensor[DualSensor(i)]==Disabled) //Switch-Disabled
    {
      
    }*/
    if(StateSensor[i]==SWITCH_TIME)
    { 
      #if USB_MIDI
      usbMIDI.sendNoteOn(NoteSensor[i],127,ChannelSensor[i]);//MaxReadingSensor[i]*18);
      #else
      MIDI.sendNoteOn(NoteSensor[i],127,ChannelSensor[i]);//MaxReadingSensor[i]*18);
      #endif
    
      #if WAVTRIGGER
      wavTrigger(i,126);//MaxReadingSensor[i]*18);
      #endif
           
      StateSensor[i]=MASK_TIME;
      MaxReadingSensor[i] = -1;
    }
    else if(StateSensor[i]==CHOKE_TIME) //Choke
    { 
      #if USB_MIDI
      usbMIDI.sendNoteOn(ChokeNoteSensor[i],127,ChannelSensor[i]);
      #else
      MIDI.sendNoteOn(ChokeNoteSensor[i],127,ChannelSensor[i]);
      #endif
      
      #if WAVTRIGGER
      wavChoke(i);
      #endif
      
      StateSensor[i]=MASK_TIME;
      MaxReadingSensor[i] = -1;
    }
    return;
  }
  //===============================
  
  Time=TIMEFUNCTION;
  
  //===============================
  //          YSwitch
  //===============================
  if(TypeSensor[i]==5)
  {
    if(MaxReadingSensor[i] <= 512)
    {
      #if USB_MIDI
      usbMIDI.sendNoteOn(NoteSensor[i],min(127,MaxReadingSensor[i]*8),ChannelSensor[i]);
      #else
      MIDI.sendNoteOn(NoteSensor[i],min(127,MaxReadingSensor[i]*8),ChannelSensor[i]);
      #endif
    }
    else
    #if USB_MIDI
      //noteOn(ChannelSensor[i],DualNoteSensor[i],min(127,(MaxReadingSensor[i]-512)*8));//DUAL
      usbMIDI.sendNoteOn(DualSensor(i),min(127,(MaxReadingSensor[i]-512)*8),ChannelSensor[i]);
      #else
      MIDI.sendNoteOn(DualSensor(i),min(127,(MaxReadingSensor[i]-512)*8),ChannelSensor[i]);
      #endif
      
    if(DualSensor(i)!=127)//Dual
    {
      MaxReadingSensor[DualSensor(i)]=-1;
      TimeSensor[DualSensor(i)]=Time-ScanTimeSensor[DualSensor(i)];
    }
    
    MaxReadingSensor[i] = -1;
    return;
  }
  //===============================
  //		Piezo
  //====================================================================
  if (/*(Time-TimeSensor[i]) >= ScanTimeSensor[i]*/ StateSensor[i]==PIEZO_TIME)
  {          
      //Piezo
      if(/*DualSensor(i)!=127 &&*/ TypeSensor[i]==PIEZO)
      {
        byte v=UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]);
          
        #if WAVTRIGGER
        wavTrigger(i,v);
        #endif
        
        #if USB_MIDI  
        usbMIDI.sendNoteOn(NoteSensor[i],v,ChannelSensor[i]);
        //qBlink(); delay(100);
        usbMIDI.sendNoteOff(NoteSensor[i],v,ChannelSensor[i]);
        #else        
        MIDI.sendNoteOn(NoteSensor[i],v,ChannelSensor[i]);
        MIDI.sendNoteOff(NoteSensor[i],v,ChannelSensor[i]);
        #endif
        
        StateSensor[i]=MASK_TIME;
              
        //Piezo-Switch
        if(TypeSensor[DualSensor(i)]==SWITCH && StateSensor[DualSensor(i)]==SWITCH_TIME )
        {     
              #if USB_MIDI
              usbMIDI.sendNoteOn(NoteSensor[DualSensor(i)],127,ChannelSensor[DualSensor(i)]);
              #else              
              MIDI.sendNoteOn(NoteSensor[DualSensor(i)],127,ChannelSensor[DualSensor(i)]);
              #endif
              
              #if WAVTRIGGER
              wavTrigger(DualSensor(i),126);
              #endif
            
              StateSensor[DualSensor(i)]=MASK_TIME;
              MaxReadingSensor[DualSensor(i)] = -1;
         }
         /*
         else if(TypeSensor[DualSensor(i)]==PIEZO && MaxReadingSensor[DualSensor(i)]> ThresoldSensor[DualSensor(i)]) //Piezo-Piezo
        {
            byte v=UseCurve(CurveSensor[DualSensor(i)],MaxReadingSensor[DualSensor(i)],CurveFormSensor[DualSensor(i)]);
            #if WAVTRIGGER
             wavTrigger(DualSensor(i),v);
            #endif
            MIDI.sendNoteOn(ChannelSensor[DualSensor(i)],NoteSensor[DualSensor(i)],v);
            
            MaxReadingSensor[DualSensor(i)]=-1;  //Dual XTalk
          
        }*/
      }
      else //HH========================================
      {
        byte Note=NoteSensor[i];
        byte Hhc=DualSensor(i);
             
             //if(ZeroCountSensor[HHC]>DualThresoldSensor[i])//DUAL
             if(MaxReadingSensor[Hhc]>HHCTHRESOLD)
               Note=ChokeNoteSensor[i];
             else if(MaxReadingSensor[Hhc]>HHThresoldSensor[3])
               Note=HHNoteSensor[3];
             else if(MaxReadingSensor[Hhc]>HHThresoldSensor[2])
               Note=HHNoteSensor[2];
             else if(MaxReadingSensor[Hhc]>HHThresoldSensor[1])
               Note=HHNoteSensor[1];
             else if(MaxReadingSensor[Hhc]>HHThresoldSensor[0])
               Note=HHNoteSensor[0];

        #if USB_MIDI
        usbMIDI.sendNoteOn(Note,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),ChannelSensor[i]);
        #else
        MIDI.sendNoteOn(Note,UseCurve(CurveSensor[i],MaxReadingSensor[i],CurveFormSensor[i]),ChannelSensor[i]);
        #endif
      }//HH=======================
               
    MaxReadingSensor[i] = -1;
  }
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    CHECKMULTI
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CheckMulti(byte Sensor,byte count)
{
  byte MulSensor=count+(Sensor<<3);
  //if(TypeSensor[MulSensor]==127/*Disabled*/) return;

  //Time=TIMEFUNCTION;
  //int sensorReading = analogRead(Sensor); 
  int yn_0 = -1;

  //===============================
  //        HHC
  //===============================
  if(TypeSensor[MulSensor]==HHC) { CheckHHControl(MulSensor,analogRead(Sensor)/8); return; }
    
  //===============================
  //        Switch
  //===============================
  if(TypeSensor[MulSensor]==SWITCH)
  {
    yn_0 = analogRead(Sensor);
    
     //DrawDiagnostic(MulSensor,yn_0/8);
    if(StateSensor[MulSensor]==NORMAL_TIME) 
    {
      if(yn_0<ThresoldSensor[MulSensor]*10 && yn_1[MulSensor]<ThresoldSensor[MulSensor]*10 )  
      {
        //DrawDiagnostic(MulSensor,0);
      
        StateSensor[MulSensor]=SCAN_TIME;
        MaxReadingSensor[MulSensor]=0;
      }
    }
    
    if(StateSensor[MulSensor]==SCAN_TIME) 
    {      
      if(yn_0<ThresoldSensor[MulSensor]*10 && yn_1[MulSensor]<ThresoldSensor[MulSensor]*10 )  
        MaxReadingSensor[MulSensor]=MaxReadingSensor[MulSensor]+1;
      else
      {
        if(MaxReadingSensor[MulSensor]>ScanTimeSensor[MulSensor])
        {
          StateSensor[MulSensor]=SWITCH_TIME;
          MaxRetriggerSensor[MulSensor] = RetriggerSensor[MulSensor];
        }
        else
        {
          StateSensor[MulSensor]=NORMAL_TIME;
        }
      }
      
      if(MaxReadingSensor[MulSensor]>MaskTimeSensor[MulSensor])
      {
          StateSensor[MulSensor]=CHOKE_TIME;
          MaxRetriggerSensor[MulSensor] = RetriggerSensor[MulSensor];
      }
    }
    
    if(StateSensor[MulSensor]==MASK_TIME)  
    { 
      //if(ZeroCountSensor[MulSensor]>0) DrawDiagnostic(MulSensor,ZeroCountSensor[MulSensor]*16);
      if(MaxRetriggerSensor[MulSensor] > 0)
      {
        MaxRetriggerSensor[MulSensor]=MaxRetriggerSensor[MulSensor]-1;
        //DrawDiagnostic(MulSensor,128);
      }
      else
      {
        if(yn_0>=ThresoldSensor[MulSensor]*10 && yn_1[MulSensor]>=ThresoldSensor[MulSensor]*10 ) 
          StateSensor[MulSensor]=NORMAL_TIME;
      }
    }
  }
  //===============================
  //        YSwitch
  //===============================
  else if(TypeSensor[MulSensor]==5)
  {
    yn_0 = analogRead(Sensor);
    
    if(yn_0<ThresoldSensor[MulSensor]*4 )
    {
      StateSensor[MulSensor]=SCAN_TIME;
      
      if(MaxReadingSensor[MulSensor]<=MaskTimeSensor[MulSensor]) 
        MaxReadingSensor[MulSensor]=MaxReadingSensor[MulSensor]+1;
    }
    else
    {
      if(MaxReadingSensor[MulSensor]!=255 && MaxReadingSensor[MulSensor]>ScanTimeSensor[MulSensor]) //SwitchTime
      {
        ///DUAL
        //if(yn_0>DualThresoldSensor[MulSensor]*4)
        if(yn_0>CurveFormSensor[MulSensor]*4)
          MaxReadingSensor[MulSensor] = MaxReadingSensor[MulSensor];
        else
          MaxReadingSensor[MulSensor] = 512+MaxReadingSensor[MulSensor];
      }
      StateSensor[MulSensor]=SWITCH_TIME; 
    }
  }
  //===============================
  //        Piezo, HH
  //===============================
  else
  {
    Time=TIMEFUNCTION;
    
    if(StateSensor[MulSensor]==MASK_TIME)  
    { 
      if ((Time-TimeSensor[MulSensor])>MaskTimeSensor[MulSensor])
      {
        StateSensor[MulSensor]=RETRIGGER_TIME;
      }
    }
    
    yn_0 = 0.5 + ((float)analogRead(Sensor)*(float)ChokeNoteSensor[MulSensor])/64.0;  //GAIN
    
        
    if(StateSensor[MulSensor]==RETRIGGER_TIME)
    {
      if(MaxRetriggerSensor[MulSensor]>=0)//(Time-TimeSensor[MulSensor])<3*MaskTimeSensor[MulSensor])
      {
          if((yn_0 - yn_1[MulSensor])> ThresoldSensor[MulSensor] && yn_0 > MaxRetriggerSensor[MulSensor])
          {
            StateSensor[MulSensor]=SCAN_TIME;
            TimeSensor[MulSensor]=Time;
            MaxReadingSensor[MulSensor] = 0;
          }
          else MaxRetriggerSensor[MulSensor]=(MaxRetriggerSensor[MulSensor]-RetriggerSensor[MulSensor]-1)>0?(MaxRetriggerSensor[MulSensor]-RetriggerSensor[MulSensor]-1):0;
      }
      else
      {
          StateSensor[MulSensor]=NORMAL_TIME;
      }
    }
    
    if(StateSensor[MulSensor]==NORMAL_TIME) 
    {
      if((yn_0 - yn_1[MulSensor])> ThresoldSensor[MulSensor]) 
      {
        StateSensor[MulSensor]=SCAN_TIME;
        TimeSensor[MulSensor]=Time;
        MaxReadingSensor[MulSensor] = 0;
      }
    }
    
    if(StateSensor[MulSensor]==SCAN_TIME) 
    {
      if ((Time-TimeSensor[MulSensor]) < ScanTimeSensor[MulSensor])
      {
        if(yn_0 > MaxReadingSensor[MulSensor])
        {
          MaxReadingSensor[MulSensor] = yn_0;
	  MaxRetriggerSensor[MulSensor] = yn_0;//(yn_0 > RetriggerSensor[MulSensor])?(yn_0 - RetriggerSensor[MulSensor]):0;
        
          if(MaxXtalkGroup[XtalkGroupSensor[MulSensor]]==-1 || MaxXtalkGroup[XtalkGroupSensor[MulSensor]]<yn_0) //MaxGroup
            MaxXtalkGroup[XtalkGroupSensor[MulSensor]]=yn_0;
          
          if(MaxMultiplexerXtalk[count]==-1 || MaxMultiplexerXtalk[count]<yn_0)
            MaxMultiplexerXtalk[count]=yn_0;
        }
      }
      else
      {
        StateSensor[MulSensor]=PIEZO_TIME;
      }
    }
  }
  
  //====================================
  //TOOL
  //====================================
  if(Mode==ToolMode && LogPin==MulSensor) LogTool(yn_0,MulSensor);
  //====================================
  
  yn_1[MulSensor]=yn_0;

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    CHECKHHCONTROL
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CheckHHControl(byte HHControl,byte sensorReading)
{
  if ((Time-TimeSensor[HHControl]) > MaskTimeSensor[HHControl])
  {
    if(sensorReading>(/*yn_1*/MaxReadingSensor[HHControl]+HHCTHRESOLD) || sensorReading<(/*yn_1*/MaxReadingSensor[HHControl]-HHCTHRESOLD))
    {
      if (Mode==MIDIMode)
      {
        #if WAVTRIGGER
        wavTriggerHHC(sensorReading);
        #endif
        
        #if USB_MIDI
        usbMIDI.sendControlChange(NoteSensor[HHControl], sensorReading, ChannelSensor[HHControl]);
        #else
        MIDI.sendControlChange(NoteSensor[HHControl], sensorReading, ChannelSensor[HHControl]);
        #endif
        
      }
        
        else if(Mode==ToolMode && Diagnostic==true)
        simpleSysex(0x6F,HHControl,sensorReading,0);
        
      float m=(((float)MaxReadingSensor[HHControl]-(float)sensorReading)/((float)TimeSensor[HHControl]-(float)Time))*100;

      MaxReadingSensor[HHControl]=sensorReading;//LastReading
        
      //Foot Splash
      if(m>0 && m>HHFootThresoldSensor[0])
        StateSensor[HHControl]=FOOTSPLASH_TIME;

      //FootClose
      else if(m<0 && -m>HHFootThresoldSensor[1])
        StateSensor[HHControl]=FOOTCLOSE_TIME;

      TimeSensor[HHControl]=Time;
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    USECURVE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//const float X[9]={0, 128, 256, 384, 512, 640, 768, 896, 1024};
const float Exp[9]={2.33, 3.85, 6.35, 10.48, 17.28, 28.5, 46.99 , 77.47, 127.74}; //e^((x/256)+0,85)
const float Log[9]={0, 83.67, 98.23, 106.74, 112.78, 117.47, 121.30 , 124.53, 127.34}; //21*ln(0,42*x)
const float Sigma[9]={2.28, 6.02, 15.13, 34.15, 63.5, 92.84, 111.86 , 120.97, 127.71}; //128/(1+e^((512-x)/128))
const float Flat[9]={0, 32.86, 46.42, 55.82, 64.0, 72.17, 81.57 , 95.13, 127}; //64-16*ln((1024/x)-1)

byte UseCurve(byte Curve,int Value,byte Form)
{
  int ret=0;
  //float Xn=(float)Value;
  float f=((float)Form)/32.0;//[1;127]->[0.;4.0]
    
  if(Curve==0)
  {
    ret=0.5 + ((float)Value*f/8.0);   //ret=((float)Value*f/32.0) - 0.5; maybe?
  }
  else
  {
    int i=Value/128;
    int m=Value % 128;
    
    switch(Curve)
    {
      case 1: ret = 0.5 + (((float)m*(Exp[i+1]-Exp[i])/128.0) + Exp[i])*f; break;
      case 2: ret = 0.5 + (((float)m*(Log[i+1]-Log[i])/128.0) + Log[i])*f; break;
      case 3: ret = 0.5 + (((float)m*(Sigma[i+1]-Sigma[i])/128.0) + Sigma[i])*f; break;
      case 4: ret = 0.5 + (((float)m*(Flat[i+1]-Flat[i])/128.0) + Flat[i])*f; break;
      
      default: ret = i*16; break;//Test mode
    }
  }
  
  /*switch(Curve)
  {
    //[0-1023]x[0-127]
    case 0: ret=0.5 + (x*f/8.0); break;
    case 1: ret=(127.0/(exp(2.0*f)-1))*(exp(f*x/512.0)-1.0);break; //Exp 4*(exp(x/256)-1)
    case 2: ret=log(1.0+(f*x/128.0))*(127.0/log((8*f)+1));break; //Log 64*log(1+x/128)
    case 3: ret=(127.0/(1.0+exp(f*(512.0-x)/64.0)));break; //Sigma
    case 4: ret=(64.0-((8.0/f)*log((1024/(1+x))-1)));break; //Flat
  }*/
  
  if(ret<=0) return 0;
  if(ret>=127) return 127;//127
  return ret;
}





