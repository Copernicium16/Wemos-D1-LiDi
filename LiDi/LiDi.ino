#include <time.h>
#include <WiFi.h>
#define LED2 2

const int ULTRA_PIN = ;
const int LIGHT_PIN = ;
const int PIR_PIN = ;
const int TRIG_PIN = ;
const int ECHO_PIN = ;

char* ssid = "****";
char* pass = "****";
int timezone = 7*3600;
int dst = 0;
int pirState = LOW;
int value = 0;
int start = 0;

void setup()
{
  pinMode(LED2, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ULTRA_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(ULTRA_PIN, HIGH);
  digitalWrite(LIGHT_PIN, HIGH);
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFI...");
  while(true)
  {
    if(WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }
    else
      {
        Serial.print("\nIP address: ");
        Serial.println(WiFi.localIP());
        break;
      }
  }
  Serial.println("Connected");
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for local time");
  while(!time(nullptr))
  {
    Serial.print("*");
    delay(1000); 
  }
  Serial.println("\nTime response...");
  digitalWrite(LED2, HIGH);
}
 
void loop()
{
  timeClock();
  value = digitalRead(PIR_PIN);
  sensor();
}

int timeClock()
{
  /*Time output*/
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  Serial.print(p_tm->tm_hour);
  Serial.print(":");
  Serial.print(p_tm->tm_min);
  Serial.print(":");
  Serial.print(p_tm->tm_sec);
}

int sensor()
{
  /*PIR Sensor*/
  if (value == HIGH)
  {
    digitalWrite(LED2, HIGH);
    if (pirState == LOW)
    {
      timeClock();
      Serial.println(" Motion detected!");
      pirState = HIGH;
      while(start < 10)
      {
        output();
        start = start+1;
        delay(10000);
      }
      start = 0;
    }
  }
  else
  {
    digitalWrite(LED2, LOW);
    digitalWrite(ULTRA_PIN, HIGH);
    digitalWrite(LIGHT_PIN, HIGH);
    if (pirState == HIGH)
    {
      timeClock();
      Serial.println(" Motion ended!");
      pirState = LOW;
    }
  }
}

int output()
{
  /*Ultrasonic Pest Repeller and Strobe Light*/
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  int h = p_tm->tm_hour;
  int distance1 = distanceSensor();
  delay(1000);
  int distance2 = distanceSensor();
  int difference = abs(distance1 - distance2);
  timeClock();
  Serial.print(" Movement : ");
  Serial.print(difference);
  Serial.println(" cm");
  if (h >= 18 or h <= 6)
  {
    if (difference > 10)
    {
      digitalWrite(ULTRA_PIN, LOW);
      digitalWrite(LIGHT_PIN, LOW);
    }
    else
    {
      digitalWrite(ULTRA_PIN, HIGH);
      digitalWrite(LIGHT_PIN, LOW);
    }
  }
  else
  {
    if (difference > 10)
    {
      digitalWrite(ULTRA_PIN, LOW);
    } 
    else
    {
      digitalWrite(ULTRA_PIN, HIGH);
    }
  }
}

int distanceSensor()
{
  long duration, distanceCm, distanceIn; 
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = duration / 29.1 / 2;
  distanceIn = duration / 74 / 2;
  return distanceCm;
}
