#include <WiFi.h>
#include "Pushover.h"

const unsigned int MOISTURE_SENSOR_PIN = 34;
const unsigned int BATT_PIN = 35;
const unsigned int LED_PIN = 16;
const unsigned int INBUILT_LED_PIN = 5;
const unsigned int PLANT_WATER_REMINDER_COOLDOWN = 30 * 60 / 60;
const unsigned int BATT_CHARGE_REMINDER_COOLDOWN = 30 * 60 / 60;
const unsigned int SLEEP_TIME_US = 60 * 1000 * 1000;

// WiFi network name and password:
const char * networkName = "<ssid>";
const char * networkPswd = "<ssidpw>";
// Pushover keys
const char * po_app_token = "<app>>";
const char * po_user_token = "<user>";
const char * po_device = "<device>";

RTC_DATA_ATTR bool moisture_alert_sent = false;
RTC_DATA_ATTR bool batt_alert_sent = false;

unsigned long on_status_timer = 0;
unsigned long mositure_alert_timer = 0;
unsigned long batt_alert_timer = 0;
bool connectedToWiFi = false;

RTC_DATA_ATTR unsigned int plant_water_reminder_count = 0;
RTC_DATA_ATTR unsigned int battery_charge_reminder_count = 0;
RTC_DATA_ATTR unsigned int boot_count = 0;

Pushover po = Pushover(po_app_token, po_user_token, false);

void printLine()
{
  Serial.println();
  for (int i=0; i<30; i++)
    Serial.print("-");
  Serial.println();
}

void connectToWiFi(const char * ssid, const char * pwd)
{
  digitalWrite(LED_PIN, HIGH);
  if(connectedToWiFi){
    return;
  }
  int ledState = 0;

  printLine();
  Serial.println("Connecting to WiFi network: " + String(ssid));

  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) 
  {
    // Blink LED while we're connecting:
    digitalWrite(INBUILT_LED_PIN, ledState);
    ledState = (ledState + 1) % 2; // Flip ledState
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  connectedToWiFi = true;

  // Blink to show WiFi up
  digitalWrite(LED_PIN, LOW);
  delay(100);
  
  for(int i = 0; i < 5; i++){
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(INBUILT_LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  esp_sleep_enable_timer_wakeup(SLEEP_TIME_US);
  po.setDevice(po_device);
  if(boot_count == 0){
    connectToWiFi(networkName, networkPswd);
    po.setMessage("Plant water reminder device startup");
    Serial.print(po.send());
  }

  reset_cooldowns();
    
  readBatt();
  readMoisture();
  
  //led_mode = LED_MODE_PWR_ON;
  Serial.println("Going to sleep now");
  Serial.flush(); 
  delay(200);
  digitalWrite(LED_PIN, LOW);
  boot_count++;
  esp_deep_sleep_start();
}

void sendMoistureAlert(){
  if(!moisture_alert_sent){
    connectToWiFi(networkName, networkPswd);
    int m_val = analogRead(MOISTURE_SENSOR_PIN);
    char moisture_msg_buff[128];
    sprintf(moisture_msg_buff, "Plant needs watering: %d.", m_val);
    po.setMessage(moisture_msg_buff);
    po.send();
    moisture_alert_sent = true;
  }
}

void sendBattAlert(){
  if(!batt_alert_sent){
    connectToWiFi(networkName, networkPswd);
    int b_val = analogRead(BATT_PIN);
    char batt_msg_buff[128];
    sprintf(batt_msg_buff, "Plant reminder device needs battery charged: %d.", b_val);
    po.setMessage(batt_msg_buff);
    po.send();
    batt_alert_sent = true;
  }
}

void readBatt(){
  int b_val = analogRead(BATT_PIN);
  Serial.print("Batt: ");
  Serial.println(b_val);
  if(b_val < 1800){
    sendBattAlert();
  }
}

void readMoisture(){
  int m_val = analogRead(MOISTURE_SENSOR_PIN);
  Serial.print("Moisture: ");
  Serial.println(m_val);
  if(m_val >= 0 && m_val < 750){
    sendMoistureAlert();
  }
}


void reset_cooldowns(){
  if(moisture_alert_sent){
    plant_water_reminder_count++;
    if(plant_water_reminder_count >= PLANT_WATER_REMINDER_COOLDOWN){
      moisture_alert_sent = false;
      plant_water_reminder_count = 0;
    }
  }
  if(batt_alert_sent){
    battery_charge_reminder_count++;
    if(battery_charge_reminder_count >= BATT_CHARGE_REMINDER_COOLDOWN){
      batt_alert_sent = false;
      battery_charge_reminder_count = 0;
    }
  }
}

void loop() {
 
}
