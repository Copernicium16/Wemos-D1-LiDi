#include <time.h>
#include <WiFi.h>
#define LED2 2

const int ULTRA_PIN = 16;
const int LIGHT_PIN = 17;
const int PIR_PIN = 27;
const int TRIG_PIN = 23;
const int ECHO_PIN = 19;

char* ssid = "LiDi_IoT";
char* pass = "L1D1_IoT";
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
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  int h = p_tm->tm_hour;
  Serial.print(h);
  Serial.print(p_tm->tm_hour);
  Serial.print(":");
  Serial.print(p_tm->tm_min);
  Serial.print(":");
  Serial.println(p_tm->tm_sec);
  if (h >= 18 or h <= 6)
  {
    value = digitalRead(PIR_PIN);
    nightSensor();
  }
  else
  {
    value = digitalRead(PIR_PIN);
    daySensor();
  }
  delay(1000);
}

void nightSensor()
{
  if (value == HIGH)
  {
    digitalWrite(LED2, HIGH);
    if (pirState == LOW)
    {
      Serial.println("Motion detected!");
      pirState = HIGH;
      while(start < 10)
      {
        int distance1 = distanceSensor();
        delay(1000);
        int distance2 = distanceSensor();
        int difference = abs(distance1 - distance2);
        Serial.print("Movement : ");
        Serial.print(difference);
        Serial.println(" cm");
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
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
}

void daySensor()
{
  if (value == HIGH)
  {
    digitalWrite(LED2, HIGH);
    if (pirState == LOW)
    {
      Serial.println("Motion detected!");
      pirState = HIGH;
      while(start < 10)
      {
        int distance1 = distanceSensor();
        delay(1000);
        int distance2 = distanceSensor();
        int difference = abs(distance1 - distance2);
        Serial.print("Movement : ");
        Serial.print(difference);
        Serial.println(" cm");
        if (difference > 10)
        {
          digitalWrite(ULTRA_PIN, LOW);
        } 
        else
        {
          digitalWrite(ULTRA_PIN, HIGH);
        }
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
    if (pirState == HIGH)
    {
      Serial.println("Motion ended!");
      pirState = LOW;
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
