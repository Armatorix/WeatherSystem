#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//CONFIGURATION
#define INSTANCE_NAME "dummy_placement"
#define PUSHGATEWAY_IP "RPI_IP"
#define PUSHGATEWAY_PORT "9091"
const char *ssid = "network_ssid";
const char *password = "network_pass";

#define REQUEST_URL "http://" PUSHGATEWAY_IP ":" PUSHGATEWAY_PORT "/metrics/job/measure_environment/instance/" INSTANCE_NAME

Adafruit_BME280 bme;

void setupESP()
{
  pinMode(2, OUTPUT);
}
void setupSensor()
{
  Serial.begin(9600);
  while (!bme.begin(0x76))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    signalSensorError();
    delay(500);
  }
}

void setupWiFi()
{
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    signalNetworkError();
    delay(200);
    Serial.print(".");
  }
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  setupESP();
  setupSensor();
  setupWiFi();
}

float tmp, prs, hum;

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    metricsUpdate();
    String mts = promFormatMetrics();
    HTTPClient http;                              //Declare object of class HTTPClient;
    http.begin(REQUEST_URL);                      //Specify request destination
    http.addHeader("Content-Type", "text/plain"); //Specify content-type header
    int code = http.POST(mts);
    if (code == 200)
    {
      signalSuccess();
    }
    else
    {
      signalPrometheusUpdateError();
      Serial.println("Some error occured while updating metrics");
      Serial.println(http.getString());
    }
    http.end();
  }
  else
  {
    signalNetworkError();
    Serial.println("Connection issue");
    ESP.restart();
  }
  delay(1000);
}

void metricsUpdate()
{
  tmp = bme.readTemperature();
  prs = bme.readHumidity();
  hum = bme.readPressure() / 100.0F;
}

String promFormatMetrics()
{
  String ptr = "";
  ptr += newMetric("temperature", tmp);
  ptr += newMetric("humidity", hum);
  ptr += newMetric("pressure", prs);
  return ptr;
}

String newMetric(String name, float v)
{
  String ptr = "";
  ptr += "#HELP ";
  ptr += name;
  ptr += " gauge.\n";
  ptr += "#TYPE ";
  ptr += name;
  ptr += " gauge\n";
  ptr += name;
  ptr += " ";
  ptr += v;
  ptr += "\n";
  return ptr;
}
void signalSuccess()
{
  signalHelper(1);
}

void signalNetworkError()
{
  signalHelper(2);
}

void signalSensorError()
{
  signalHelper(3);
}

void signalPrometheusUpdateError()
{
  signalHelper(4);
}

//up to 6
void signalHelper(int n)
{
  for (int i = 0; i < n; ++i)
  {
    digitalWrite(2, LOW);
    delay(200);
    digitalWrite(2, HIGH);
    delay(150);
  }
  delay(1000 - 150 * n);
}