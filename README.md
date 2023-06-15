# ingress_beacon
Fri3d Badge 2020 (octopus) code for a beacon which can be "conquered" by red or blue teams

current status:
- ble device is visible for the ingress_agent code (although not for my windows laptop...)
- although devi
- LED lights remain white, which means "unoccupied territory"
- LED lights blink, which means "contested"
- I've written some untested code that exposes a "color" variable, indicating white, blue or red.
- I've written untested code where agents can "push" their color and either contest an enemy beacon or steady a friendly beacon
