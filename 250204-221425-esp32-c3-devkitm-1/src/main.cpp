#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 96

CRGB leds[NUM_LEDS];
double flat_points[NUM_LEDS][3];

enum Effect {
  RAINBOW,
  MOVING_PLANE
};
enum Effect current_effect;

double points[6][4][4][3] = {
  { { { -22.7, -22.7, 34.4 }, { -22.7, -7.8, 34.4 }, { -22.7, 7.8, 34.4 }, { -22.7, 22.7, 34.4 } },
    { { -7.8, -22.7, 34.4 }, { -7.8, -7.8, 34.4 }, { -7.8, 7.8, 34.4 }, { -7.8, 22.7, 34.4 } },
    { { 7.8, -22.7, 34.4 }, { 7.8, -7.8, 34.4 }, { 7.8, 7.8, 34.4 }, { 7.8, 22.7, 34.4 } },
    { { 22.7, -22.7, 34.4 }, { 22.7, -7.8, 34.4 }, { 22.7, 7.8, 34.4 }, { 22.7, 22.7, 34.4 } } },
  { { { 34.4, -22.7, 22.7 }, { 34.4, -7.8, 22.7 }, { 34.4, 7.8, 22.7 }, { 34.4, 22.7, 22.7 } },
    { { 34.4, -22.7, 7.8 }, { 34.4, -7.8, 7.8 }, { 34.4, 7.8, 7.8 }, { 34.4, 22.7, 7.8 } },
    { { 34.4, -22.7, -7.8 }, { 34.4, -7.8, -7.8 }, { 34.4, 7.8, -7.8 }, { 34.4, 22.7, -7.8 } },
    { { 34.4, -22.7, -22.7 }, { 34.4, -7.8, -22.7 }, { 34.4, 7.8, -22.7 }, { 34.4, 22.7, -22.7 } } },
  {
    { { -22.7, -34.4, 22.7 }, { -7.8, -34.4, 22.7 }, { 7.8, -34.4, 22.7 }, { 22.7, -34.4, 22.7 } },
    { { -22.7, -34.4, 7.8 }, { -7.8, -34.4, 7.8 }, { 7.8, -34.4, 7.8 }, { 22.7, -34.4, 7.8 } },
    { { -22.7, -34.4, -7.8 }, { -7.8, -34.4, -7.8 }, { 7.8, -34.4, -7.8 }, { 22.7, -34.4, -7.8 } },
    { { -22.7, -34.4, -22.7 }, { -7.8, -34.4, -22.7 }, { 7.8, -34.4, -22.7 }, { 22.7, -34.4, -22.7 } },
  },
  {
    { { -34.4, 22.7, 22.7 }, { -34.4, 7.8, 22.7 }, { -34.4, -7.8, 22.7 }, { -34.4, -22.7, 22.7 } },
    { { -34.4, 22.7, 7.8 }, { -34.4, 7.8, 7.8 }, { -34.4, -7.8, 7.8 }, { -34.4, -22.7, 7.8 } },
    { { -34.4, 22.7, -7.8 }, { -34.4, 7.8, -7.8 }, { -34.4, -7.8, -7.8 }, { -34.4, -22.7, -7.8 } },
    { { -34.4, 22.7, -22.7 }, { -34.4, 7.8, -22.7 }, { -34.4, -7.8, -22.7 }, { -34.4, -22.7, -22.7 } },
  },
  {
    { { 22.7, 34.4, 22.7 }, { 7.8, 34.4, 22.7 }, { -7.8, 34.4, 22.7 }, { -22.7, 34.4, 22.7 } },
    { { 22.7, 34.4, 7.8 }, { 7.8, 34.4, 7.8 }, { -7.8, 34.4, 7.8 }, { -22.7, 34.4, 7.8 } },
    { { 22.7, 34.4, -7.8 }, { 7.8, 34.4, -7.8 }, { -7.8, 34.4, -7.8 }, { -22.7, 34.4, -7.8 } },
    { { 22.7, 34.4, -22.7 }, { 7.8, 34.4, -22.7 }, { -7.8, 34.4, -22.7 }, { -22.7, 34.4, -22.7 } },
  },
  { { { -22.7, -22.7, -34.4 }, { -22.7, -7.8, -34.4 }, { -22.7, 7.8, -34.4 }, { -22.7, 22.7, -34.4 } },
    { { -7.8, -22.7, -34.4 }, { -7.8, -7.8, -34.4 }, { -7.8, 7.8, -34.4 }, { -7.8, 22.7, -34.4 } },
    { { 7.8, -22.7, -34.4 }, { 7.8, -7.8, -34.4 }, { 7.8, 7.8, -34.4 }, { 7.8, 22.7, -34.4 } },
    { { 22.7, -22.7, -34.4 }, { 22.7, -7.8, -34.4 }, { 22.7, 7.8, -34.4 }, { 22.7, 22.7, -34.4 } } }
};

int sortByHeight(const void *cmp1, const void *cmp2) {
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  return flat_points[a][2] - flat_points[b][2];
}

void flatten_points() {
  int index = 0;
  for (size_t i = 0; i < 6; i++) {
    for (size_t j = 0; j < 4; j++) {
      for (size_t k = 0; k < 4; k++) {
        flat_points[index][0] = points[i][j][k][0];
        flat_points[index][1] = points[i][j][k][1];
        flat_points[index][2] = points[i][j][k][2];
        index++;
      }
    }
  }
 }

void setup() {  
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  // limit my draw to 1A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);

  // for (int i = 0; i < NUM_LEDS; i++) {
  //   leds[i] = CRGB(255, 255, 255);
  // }
  flatten_points();
  current_effect = RAINBOW;
}

void rainbow() {
  int index = 0;

  int indexs[6 * 4 * 4];
  for (size_t i = 0; i < 6 * 4 * 4; i++) {
    indexs[i] = i;
  }
  qsort(indexs, 6 * 4 * 4, sizeof(indexs[0]), sortByHeight);

  static double hueOffset = 0;  // hue offset
  double hueOffsetDelta = 0.2;

  double highestPoint = flat_points[indexs[0]][2];
  double lowestPoint = flat_points[indexs[6 * 4 * 4 - 1]][2];
  double totalHeight = lowestPoint - highestPoint;
  double deltaHue = 255.0 / (totalHeight*10);  // delta hue

  for (size_t i = 0; i < 6 * 4 * 4; i++) {
    double hue = (flat_points[i][2]) * deltaHue + hueOffset;
    leds[i] = CHSV(hue, 255, 255);
  }
  FastLED.show();

  // change hue offset
  if (hueOffset < 255) {
    hueOffset += hueOffsetDelta;
  } else {
    hueOffset = 0;
  }
}

int width = 20;
void moving_plane() {
  int indexs[6 * 4 * 4];
  for (size_t i = 0; i < 6 * 4 * 4; i++) {
    indexs[i] = i;
  }
  qsort(indexs, 6 * 4 * 4, sizeof(indexs[0]), sortByHeight);

  double lowestZ = flat_points[indexs[0]][2];
  double highestZ = flat_points[indexs[6 * 4 * 4 - 1]][2];

  static double position = highestZ;
  static double offset = -0.1;
  for (size_t i = 0; i < 6 * 4 * 4; i++) {
    double distance = abs(flat_points[i][2] - position);
    if (distance <= width) {
      leds[i] = CHSV(200, 255, 255*(1-(distance/width)));
    } else {
      leds[i] = CHSV(0, 0, 0);
    }
  }
  FastLED.show();
  position += offset;
  
  if (position < lowestZ) {
    offset = 0.1;
  } else if (position > highestZ) {
    offset = -0.1;
  }

}


void loop() {
  switch(current_effect) {
    case RAINBOW:
      rainbow();
      break;
    case MOVING_PLANE:
      moving_plane();
      break;
  }
}

