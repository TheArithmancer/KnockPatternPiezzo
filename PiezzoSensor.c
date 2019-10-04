/* This code is used to detect knocking using a piezzo sensor. It then unlocks a maglock connected to a relay */
/* Code written by Septima Vector, 2019  */
/* Written for Myriam's christmas present */
/* Code can also be used to detect morse code */

/* The code measures the time delay between subsequent knocks using a piezzo sensor connected to Analog pin 1 */
/* The times are normalised into "long" and "short" delays. Short is defined as any time delay less than 3/4 the longest delay */

const int sensorPin = A0;    // select the input pin for the potentiometer
const int ledPin = LED_BUILTIN;      // select the pin for the LED
const int RelayPin= 2;
const int sensorThreshold=3;
const int knockDelay=125;
const int KnockPatternLength=7;

int ResetTime=5000;
int lastKnock=0;
int NumberOfKnocks=0;
int KnockPattern[KnockPatternLength-1]={500, 500,500,500,1000,500};
int KnockPatternHeard[KnockPatternLength-1]={};
int KnockAtTimes[KnockPatternLength]={0,0,0,0,0,0,0};
int sensorValue = 0;  // variable to store the value coming from the sensor
int startTime;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  Serial.begin(9600);
  startTime=millis();
}

void loop() {
  int mytime;
  int elapsedTime;

  elapsedTime=millis()-startTime;
  sensorValue = analogRead(sensorPin);
  if (sensorValue>sensorThreshold && NumberOfKnocks<=KnockPatternLength) {
    KnockAtTimes[NumberOfKnocks]=millis();
    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);
    Serial.print("At time: ");
    Serial.println(KnockAtTimes[NumberOfKnocks]);
    NumberOfKnocks=NumberOfKnocks+1;
    if (NumberOfKnocks==1) {
      startTime=millis();
    } else if (NumberOfKnocks==KnockPatternLength) {
      Serial.println("Success. Lets check if you are right");
      CheckPatternPlease();
    }
    delay(150);
  } else if (NumberOfKnocks>KnockPatternLength || elapsedTime>ResetTime) {
    NumberOfKnocks=0;
    startTime=millis();
    Serial.println("Resetting");
    for (int i=0;i<7;i++) {
      KnockAtTimes[i]=0;
    }
  }

  
}


bool CheckPatternPlease() {
  int mymax;
  float test;
  /* First Change to time elapsed between knocks */ 
  for (int i=1; i<KnockPatternLength; i++) {
    KnockPatternHeard[i-1]=KnockAtTimes[i]-KnockAtTimes[i-1];
    Serial.print(KnockAtTimes[i]);
    Serial.print("-");
    Serial.print(KnockAtTimes[i-1]);
    Serial.print("=");
    Serial.println(KnockPatternHeard[i-1]);
  }
  mymax=CalcMax();
  Serial.print("Max is:");
  Serial.println(mymax);
  
  /* Then normalise */
  
  for (int i=0; i<KnockPatternLength-1;i++) {
    test=(1000*(float)KnockPatternHeard[i])/ (float)mymax;
    //Serial.println(test); 
    if (test<750) {
      KnockPatternHeard[i]=500;
    } else {
      KnockPatternHeard[i]=1000;
    }
    Serial.println(KnockPatternHeard[i]);
    
  }

  /* Now check if equals our pre-set pattern */
  if (ComparePatterns()) {
    OpenLock();
  }
}

bool knockDetected() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  if (sensorValue>sensorThreshold) {
    Serial.println("Knock Detected ");
    lastKnock=1;
    return true;
  } else {
    lastKnock=0;
    return false;
  }
}
  
bool OpenLock() {
  digitalWrite(RelayPin,HIGH);
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
  digitalWrite(RelayPin,LOW);
    
}

int CalcMax() {
  int maximum=KnockPatternHeard[0];
  
  for (int i=1; i<=KnockPatternLength-2;i++) {
    if (KnockPatternHeard[i]>maximum) {
      maximum=KnockPatternHeard[i];
    }
  }
  return(maximum);
}

bool ComparePatterns() {
  bool IsItCorrect=true;
  
  for (int i=0; i<=KnockPatternLength-2;i++) {
    if (KnockPatternHeard[i]!=KnockPattern[i]) {
      IsItCorrect=false;
    }
  }
  return(IsItCorrect);
}
