# Weather System

System to provide temperature, humidity and pressure monitoring with Prometheus, Prometheus Pushgateway and Grafana.

## Requirements
### Hardware
RPi
ESP8266 + BME280
Router

### Software
* Ansible
* Arduino IDE

On RPi:
* docker
* docker-compose
## How to 


1. Assign to you RPi static IP
   1. on your DHCP server
   2. on your RPi (but then it should be assign out of DHCP pool)
2. Setup envs
   1. [ansible/hosts](ansible/hosts)
        * RPI_IP
   2. [ansible/roles/base/files/grafana/provisioning/datasources/datasouce.yml](ansible/roles/base/files/grafana/provisioning/datasources/datasouce.yml)
        * RPI_IP
        * network_ssid 
        * network_pass 
        * INSTANCE_NAME - placement name of sensor
   to your RPI static IP
3. `cd ansible && ansible-playbook init.yml`
4. Setup your microcontroller like [here](https://how2electronics.com/esp8266-bme280-based-mini-weather-station/), but with this code: [controller/src/sensor.ino](controller/src/sensor.ino).

Aaand you're ready to rock!

Your graphs with temperature and humidity should be available on http://RPI_IP:3000