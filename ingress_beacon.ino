
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include <Adafruit_NeoPixel.h>
#include <Badge2020_TFT.h>
#include <Badge2020_arduino.h>
#include "printscroll.h"
#include "ingress.h"

// UUIDs were generated randomly using https://www.uuidgenerator.net/
static BLEUUID FRI3D_SERVICE_UUID("ea5c0c62-079e-11ee-be56-0242ac120002");
static BLEUUID FRI3D_COLOR_UUID  ("086aece6-079f-11ee-be56-0242ac120002");
static BLEUUID FRI3D_PUSH_UUID   ("3ed75c2e-079f-11ee-be56-0242ac120002");

Badge2020_TFT tft;
scroll::ForwardScroll scr(tft);
Ingress kamp;
BLEAdvertising* pAdvertising;

// definitions for 5 LEDs at bottom of badge
#define LED_PIN    2
#define LED_COUNT 5
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


class PushCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {      
      std::string beschrijving = pCharacteristic->getValue();
      scr.println("string pushed was ");
      scr.println(beschrijving.c_str());
      uint8_t kleur = *(uint8_t *) pCharacteristic->getData();
      kamp.push(kleur);
      scr.print("push "); scr.println(Ingress::toCampName(kleur));
      scr.print("camp is ");
      if (kamp.isContested()) scr.println("CONTESTED");
      else scr.println(Ingress::toCampName(kleur));
    }
};

void initBeacon() {
  BLEDevice::init("Fri3dBacon");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(FRI3D_SERVICE_UUID);

  BLECharacteristic *colorCharacteristic = pService->createCharacteristic(
                                         FRI3D_COLOR_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );
  colorCharacteristic->addDescriptor(new BLE2902());
  colorCharacteristic->setValue("white");
  BLECharacteristic *pushCharacteristic = pService->createCharacteristic(
                                         FRI3D_PUSH_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pushCharacteristic->addDescriptor(new BLE2902());
  pushCharacteristic->setCallbacks(new PushCallbacks());
  pService->start();

  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(FRI3D_SERVICE_UUID);
  //appearance constant taken from https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned_Numbers.pdf
  //pAdvertising->setAppearance(0x1441);
  
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  oAdvertisementData.setName("Fri3dBaconAdvertised");
  oAdvertisementData.setShortName("FriedShort");
  oAdvertisementData.setCompleteServices(FRI3D_SERVICE_UUID);
  //oAdvertisementData.setPartialServices(FRI3D_SERVICE_UUID);
  oAdvertisementData.setAppearance(0x1441);
  oAdvertisementData.setServiceData(FRI3D_SERVICE_UUID, "Fri3dIngress");
  oAdvertisementData.addData("AddedData");
  //oAdvertisementData.setManufacturerData("color=blue");

  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->start();
}

void initBadge() {
  // these 4 lines to enable our tft screen:
  tft.init(240, 240);
  tft.setRotation(2);
  tft.enableBacklight();
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(0,0);
  
  scr.init();
  scr.cls();

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip.clear();
}

void setup() {
  Serial.begin(115200);

  initBadge();
  initBeacon();

  scr.println("Fri3d Bacon v5.0");
}

void loop() {
  if (kamp.isContested() && ((millis()>>10) % 2)==0)
  {
    strip.fill(0); //all black
  }
  else if (kamp.getCamp() == CAMP_RED)
  {
    strip.fill(strip.Color(55,0,0));
  }
  else if (kamp.getCamp() == CAMP_GREEN)
  {
    strip.fill(strip.Color(0,55,0));
  }
  else if (kamp.getCamp() == CAMP_BLUE)
  {
    strip.fill(strip.Color(0,0,55));
  }
  else
  {
    strip.fill(strip.Color(55,55,55));
  }

  strip.show();
  delay(100);
  if (((millis()/1000UL) % 30) == 0) pAdvertising->start();
}
