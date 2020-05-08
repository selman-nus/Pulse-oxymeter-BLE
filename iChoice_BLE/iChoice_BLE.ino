// to receive data from iChoice pulse oximeter via BLE
// developed for ESP32 devkitc

#include "BLEDevice.h"

//device  id: 5BFE6260-7BEB-C859-1972-62A684F4B447
//Service id: BA11F08C-5F14-0B0D-1080-007CBE84E417
//Charac  id: CD20
//CD01 to CD04
//key: AA5504B10000B56A - ªU±µj

// The remote service we wish to connect to.
static BLEUUID serviceUUID("BA11F08C-5F14-0B0D-1080-007CBE84E417");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("CD20");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

int bpm;
int spo2;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print(" - Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print(" - data: ");
  for (int i = 0; i < length; i++) { Serial.print(pData[i]);  Serial.print(" "); }Serial.println();

  if (pData[3] != 0xB1) {
    spo2 = pData[3];
    bpm  = pData[4];
    Serial.println("\n\n\n\t***   ___________   ***");
    Serial.print("\t \t SpO2 : "); Serial.print(spo2);  Serial.println("%");
    Serial.print("\t \t HR   : "); Serial.print(bpm);   Serial.println(" BPM");
    Serial.println("\t***   ___________   ***\n\n\n");
    connected = false; BLEDevice::getScan()->stop();
    delay(15000);
  }
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }

    void onDisconnect(BLEClient* pclient) 
    {
      connected = false; 
      Serial.println("onDisconnect");
      BLEDevice::getScan()->start(3, false);
    }
};
/**
   Scan for BLE servers and find the first one that advertises the service we are looking for.
*/
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;

      } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop()
{

  Serial.print("Forming a connection to ");  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient*  pClient  = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
  // Connect to the remove BLE Server.
  pClient->connect(myDevice);
  Serial.println("*******************************************");
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");  Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    connected = false;
  }
  else
  {
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr)
    {
      Serial.print("Failed to find our characteristic UUID: ");  Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      connected = false;
    }
    else
    {
      delay(5000);
      connected = true;
      Serial.println(" - Found our characteristic");

      byte message[] = {0xAA, 0x55, 0x04, 0xB1, 0x00, 0x00, 0xB5, 0x6A };
      pRemoteCharacteristic->writeValue(message, sizeof(message));
      Serial.println(" - Key written into our characteristic");





      pRemoteCharacteristic = pRemoteService->getCharacteristic("CD01");
      if (pRemoteCharacteristic == nullptr)
      {
        Serial.print("Failed to find our characteristic UUID: "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        pClient->disconnect();

      }
      else
      {
        Serial.print(" - Found our characteristic "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());

        if (pRemoteCharacteristic->canNotify())
        {
          pRemoteCharacteristic->registerForNotify(notifyCallback);
          Serial.print(" + Registered "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        }
      }



      pRemoteCharacteristic = pRemoteService->getCharacteristic("CD02");
      if (pRemoteCharacteristic == nullptr)
      {
        Serial.print("Failed to find our characteristic UUID: "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        pClient->disconnect();

      }
      else
      {
        Serial.print(" - Found our characteristic "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());

        if (pRemoteCharacteristic->canNotify())
        {
          pRemoteCharacteristic->registerForNotify(notifyCallback);
          Serial.print(" + Registered "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        }
      }



      pRemoteCharacteristic = pRemoteService->getCharacteristic("CD03");
      if (pRemoteCharacteristic == nullptr)
      {
        Serial.print("Failed to find our characteristic UUID: "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        pClient->disconnect();

      }
      else
      {
        Serial.print(" - Found our characteristic "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());

        if (pRemoteCharacteristic->canNotify())
        {
          pRemoteCharacteristic->registerForNotify(notifyCallback);
          Serial.print(" + Registered "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        }
      }

      Serial.println("");
      Serial.print(" *** Waiting for measurement  ");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.println(" ***");



      pRemoteCharacteristic = pRemoteService->getCharacteristic("CD04");
      if (pRemoteCharacteristic == nullptr)
      {
        Serial.print("Failed to find our characteristic UUID: "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        pClient->disconnect();

      }
      else
      {
        Serial.print(" - Found our characteristic "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());

        if (pRemoteCharacteristic->canNotify())
        {
          pRemoteCharacteristic->registerForNotify(notifyCallback);
          Serial.print(" + Registered "); Serial.println(pRemoteCharacteristic ->getUUID().toString().c_str());
        }
      }



    }
  }



  if (!connected)
  {
    if (doScan)
      BLEDevice::getScan()->start(3, false);
  }

  delay(10000); // Delay a second between loops.
}
