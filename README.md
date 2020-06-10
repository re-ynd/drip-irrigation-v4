# Drip Irrigation Control System v4
This repo contains the drip irrigation control system. 

The ```device``` directory contains code that needs to be uploaded to an ESP8266.
There is a README file in the directory containing the instructions on how to compile
and upload to the device while connected to USB or via OTA.

The ```web``` directory contains the web frontend code which needs to be 
compiled and sent to ESP8266's SPIFFS. You can follow the README.md file in the
directory to run the frontend code locally before uploading to ESP8266. Uploading
to ESP8266 SPIFFS is described in the README file under ```device```.

For details about circuit diagram and components you will need read my blog posts  
https://retired.re-ynd.com/drip-irrigation-control-system-v4-0-hardware/  
https://retired.re-ynd.com/drip-irrigation-control-system-v4-0-software/  
https://retired.re-ynd.com/drip-irrigation-control-system-v4-0-web-server/  
