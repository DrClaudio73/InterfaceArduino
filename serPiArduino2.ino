//http://www.arduino.cc

#include <ctype.h>
//comando in 1 leggi, 2 scrivi, 3 imposta, 4 get impostazione
#define cmdLeggi 1
#define cmdScrivi 2
#define cmdImpostaModo 3
#define cmdGetModo 4

//modo  0 = DigInp, 1=DigInpPullUp, 2=DigOut, 3 = DigOutPWM, 4=AnInp
#define DigInp 0
#define DigInpPullUp 1
#define DigOut 2
#define DigOutPWM 3
#define AnInp 4

#define dummy 0
#define pin_mode_index 0
#define pin_value_index 1

//ERROR CODES
#define OK 0
#define invalidCRC 0xE1
#define noValidOpforPinMode 0xE2
#define noValidModeforPinNumber 0xE3
#define invalidPinNumber 0xE4
#define invalidRequestedMode 0xE5
#define invalidCommand 0xE6

//String incomingMessage;   for incoming serial data
byte incomingByte;

//dB[pinmode, pinvalue]
//pinmode =     # 0 = DigInp, 1=DigInpPullUp, 2=DigOut, 3 = DigOutPWM, 4=AnInp
int dB[22][2];
/*= { 
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { DigOut, 0 } ,
   { DigInp, 0 } ,
   { DigInp, 0 } ,
   { AnInp, 0 } ,
   { AnInp, 0 } ,
   { AnInp, 0 } ,
   { AnInp, 0 } ,
   { AnInp, 0 } ,
   { AnInp, 0 } ,
};*/


void setup() {
  int i;
  dB[0][pin_mode_index]=DigInp;
  //dB[0][pin_value_index]=digitalRead(0);
  
  dB[1][pin_mode_index]=DigInp;
  //dB[1][pin_value_index]=digitalRead(1);
  
  for(i=2;i<13;i++){
    pinMode(i,INPUT);
    dB[i][pin_mode_index]=DigInp;
    //dB[i][pin_value_index]=digitalRead(i);
  }
  
  for(i=14;i<16;i++){
      dB[i][pin_mode_index]=DigInp;
      dB[i][pin_value_index]=0;
  }
  for(i=16;i<=21;i++){
      dB[i][pin_mode_index]=AnInp;
    //  dB[i][pin_value_index]=analogRead(i);
  }
  
  pinMode(13,OUTPUT);
  dB[13][pin_mode_index]=DigOut;
  dB[13][pin_value_index]=0;
  digitalWrite(13,LOW);
  
  //digitalWrite(13,HIGH); //turn on debugging LED delay(15000);
  Serial.begin(57600);
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }*/
}

int calcola_CRC(int totale){
    return(totale%256);
}

void invia_risposta(int comando, int PIN, int arg, int errCode){
  Serial.write(lowByte(0xF7));
  Serial.write(lowByte(comando+48));
  Serial.write(lowByte(PIN));
  Serial.write(highByte(arg));
  Serial.write(lowByte(arg));
  Serial.write(lowByte(errCode));
  Serial.write(calcola_CRC(lowByte(comando)+48+lowByte(PIN)+highByte(arg)+lowByte(arg)+lowByte(errCode)));  
}
  
int decodificaMsg(){
  int totale = 0;
  int comando = 0 ;
  int numeroPin = 0 ;
  int  arg1 = 0 ;
  int arg2 = 0 ;
  int arg = 0;
  int in_CRC = 0;
  int valore=0 ;
  int pinRef;
  int num_bytes_letti = 0;
  int numBytesRead = 0;
  //messaggio = comando, numeroPin, arg (es: Valore) , CRC , 0xF6
  //comando in 1 leggi, 2 scrivi, 3 imposta, 4 get impostazione
  //delay(350);
  //digitalWrite(13,HIGH);
  numBytesRead = 0;
  while (numBytesRead < 5) {
    if ((Serial.available() > 0) && (numBytesRead==0)) {
      comando = Serial.read();
      totale = totale + comando;
      numBytesRead++;
    }  
    
    if ((Serial.available() > 0) && (numBytesRead==1)) {
      numeroPin=Serial.read();
      totale = totale + numeroPin;
      numBytesRead++;
    }  
                        
    if ((Serial.available() > 0) && (numBytesRead==2)) {
      arg1=Serial.read();
      totale = totale + arg1;
      numBytesRead++;
    }  
    if ((Serial.available() > 0) && (numBytesRead==3)) {
      arg2=Serial.read();
      totale = totale + arg2;
      numBytesRead++;
    }  
    if ((Serial.available() > 0) && (numBytesRead==4)) {
      arg = arg1*256+arg2;
      in_CRC=Serial.read();      
      numBytesRead++;
    }  
    

  }
  
  //digitalWrite(13,LOW);

  pinRef = numeroPin;
  if (numeroPin==16)  pinRef = A0;
  if (numeroPin==17)  pinRef = A1;
  if (numeroPin==18)  pinRef = A2;
  if (numeroPin==19)  pinRef = A3;
  if (numeroPin==20)  pinRef = A4;
  if (numeroPin==21)  pinRef = A5;

  if (calcola_CRC(totale) == in_CRC ){
    if ((numeroPin == 14)||(numeroPin == 15)||((numeroPin > 21))){ //se numero pin > 13 (deve evitare che valga 14 15 ee da 16 in avanti il PINMODE
      invia_risposta(comando, numeroPin, arg, invalidPinNumber);; // KO
      return(invalidPinNumber);
    } else { //valid pin number
      switch (comando) {
      case 1: //leggi valore INPUT
        //do something when comando equals 1
        if ((dB[numeroPin][pin_mode_index] == DigInp)||(dB[numeroPin][pin_mode_index] == DigInpPullUp)){
          valore = digitalRead(pinRef);
          dB[numeroPin][pin_value_index]=valore;
          invia_risposta(comando, numeroPin, valore, OK);
        } else if (dB[numeroPin][pin_mode_index] == AnInp) {
          valore = analogRead(pinRef);
          dB[numeroPin][pin_value_index]=valore;
          invia_risposta(comando, numeroPin, valore, OK);
        } else {
          // errore
          invia_risposta(comando, numeroPin, dB[numeroPin][pin_value_index], noValidOpforPinMode);
          return(noValidOpforPinMode); // err_code 0xE2: richiesta lettura valore di una uscita 
        }     
        break;
      case 2: //scrivi valore OUTPUT
        //do something when comando equals 2 
        if (dB[numeroPin][pin_mode_index] == DigOut){
          digitalWrite(pinRef,arg);
          dB[numeroPin][pin_value_index] = arg;
          invia_risposta(comando, numeroPin, arg, OK);
        } else if (dB[numeroPin][pin_mode_index] == DigOutPWM) {
          analogWrite(pinRef,arg);
          dB[numeroPin][pin_value_index] = arg;
          invia_risposta(comando, numeroPin, arg, OK);
        } else {
          // errore
          invia_risposta(comando, numeroPin, arg, noValidOpforPinMode);
          return(noValidOpforPinMode); // err_code E2: richiesta scrittura di un ingresso
        }     
        break;
      case 3: //imposta modo PIN
        //do something when comando equals 3
        if ((arg >= DigInp)&&(arg <= AnInp)) {
          switch(arg) {
          case DigInp: //INPUT
            pinMode(pinRef,INPUT); 
            //lo devi usare simbolico ad esempio come pinMode(A0, OUTPUT);
            dB[numeroPin][pin_mode_index] = DigInp;
            invia_risposta(comando, numeroPin, arg, OK);
          break;
          case DigInpPullUp: //INPUT_PULLUP
            if (numeroPin<=13) {
              pinMode(pinRef,INPUT_PULLUP);
              dB[numeroPin][pin_mode_index] = DigInpPullUp;
              invia_risposta(comando, numeroPin, arg, OK);
            } else {
              digitalWrite(pinRef,INPUT_PULLUP);
              dB[numeroPin][pin_mode_index] = DigInpPullUp;
              invia_risposta(comando, numeroPin, arg, OK);
            }    
          break;
          case DigOut: //OUTPUT
            pinMode(pinRef,OUTPUT);
            dB[numeroPin][pin_mode_index] = DigOut;
            invia_risposta(comando, numeroPin, arg, OK);
          break;
          case DigOutPWM: //OUTPUT PWM
            if ((numeroPin==3)||(numeroPin==5)||(numeroPin==6)||(numeroPin==10)||(numeroPin==11)||(numeroPin==9)) {
              pinMode(pinRef,OUTPUT);
              dB[numeroPin][pin_mode_index] = DigOutPWM;
              invia_risposta(comando, numeroPin, arg, OK);
            } else {
              invia_risposta(comando, numeroPin, arg, noValidModeforPinNumber);
              return(noValidModeforPinNumber);
            } 
          break;
          case AnInp: //ANALOGUE INPUT
            if ((numeroPin>=16)&&(numeroPin<=21))  {
              pinMode(pinRef,INPUT);
              dB[numeroPin][pin_mode_index]= AnInp;
              invia_risposta(comando, numeroPin, arg, OK);
            } else {
              invia_risposta(comando, numeroPin, arg, noValidModeforPinNumber);
              return(noValidModeforPinNumber);
            } 
          break;
          } //switch(arg)       
        } else {// if not((arg >= DigInp)&&(arg <= AnInp)) {
            invia_risposta(comando, numeroPin, arg, invalidRequestedMode);
            return(invalidRequestedMode);
        }
      break;
        
      case 4: // get PIN mode
        //do something when comando equals 4
        invia_risposta(comando, numeroPin, dB[numeroPin][pin_mode_index], OK);
        break;
      default: // no matching command number
        // if nothing else matches, do the default
        invia_risposta(comando, numeroPin, arg, invalidCommand);
        return(invalidCommand);
      break;
      } //switch(comando)
      } 
      return(OK); // OK
    } else { // CRC KO
       invia_risposta(comando, numeroPin, arg, invalidCRC); // KO
      return(invalidCRC); // OK
    }
}

void loop(){
    int err_code;
    if (Serial.available() > 0) {
       // read the incoming byte:
       incomingByte = Serial.read();
       //Serial.write(incomingByte);    
       //wait for starting char 0xF7:
       if (incomingByte == 247) {
          err_code=decodificaMsg();
       }
       if (incomingByte == 246) { //risponde con 0xF8 per dire che a riconosciuto fine messaggio 
          Serial.write(0xF8);
       }    
     } 
    delay(10);
}
