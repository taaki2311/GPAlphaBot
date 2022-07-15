#include <esp_wifi.h>
#include <ESPAsyncUDP.h>
#include <vector>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

#define BUFFER_SIZE 1472
#define DMA_BUFFER_SIZE 512
#define ONE_SECOND_DELAY 1000

const char ssid[] = "zFiHome";
const char pass[] = "homePatiala!0Reston";
const char hostname[] = "AlphaBotController.local";
const unsigned int port = 55555;

AsyncUDP udp;
TFT_eSPI tft = TFT_eSPI();
using namespace std;

vector<uint8_t> image;
uint16_t dma1[DMA_BUFFER_SIZE], dma2[DMA_BUFFER_SIZE];
uint16_t *dmaPtr;
bool dmaSel;

extern "C" {
  void app_main();
}

void app_main() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  tft.begin();
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.initDMA();
  TJpgDec.setJpgScale(1);
  tft.setSwapBytes(true);
  TJpgDec.setCallback(jpg_decode);
  dmaSel = false;

  if (udp.listen(port)) {
    udp.onPacket([](AsyncUDPPacket udpPacket) {

      if ((udpPacket.length() == BUFFER_SIZE) &&
          (udpPacket.data()[0] == 0xFF) &&
          (udpPacket.data()[1] == 0xD8) &&
          (udpPacket.data()[2] == 0xFF)) {
        image.clear();
      }

      vector<uint8_t> packet_vector(&udpPacket.data()[0], &udpPacket.data()[udpPacket.length()]);
      image.insert(image.end(), packet_vector.begin(), packet_vector.end());

      if ((udpPacket.length() < BUFFER_SIZE) &&
          (udpPacket.data()[udpPacket.length() - 2] == 0xFF) &&
          (udpPacket.data()[udpPacket.length() - 1] == 0xD9)) {
        tft.startWrite();
        TJpgDec.drawJpg(0, 0, image.data(), image.size());
        tft.endWrite();
      }
    });
  }
}

bool jpg_decode(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if ((y >= tft.height()) || (x >= tft.width())) {
    return 0;
  }
  tft.pushImageDMA(x, y, w, h, bitmap, dmaPtr);

  switch (dmaSel) {
    case false:
      dmaPtr = dma1;
      break;
    case true:
      dmaPtr = dma2;
      break;
  } dmaSel = !dmaSel;
  return 1;
}
