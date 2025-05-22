#include "App.h"
#include "Kernel.h"

//Define the constructor
App::App(Kernel* kernel) {
  
};

void App::_setup_sprites() {
  // Nothing to do. Only implement this function if you have some sprites.
}

void make_sprite(TFT_eSprite*& spr, TFT_eSPI* display, int width, int height, const uint16_t bitmap[]) {
  if(spr) { return; } // Pointer already defined :/
  spr = new TFT_eSprite(display);
  spr->setSwapBytes(true);

  spr->setColorDepth(16);
  spr->createSprite(width, height);
  spr->pushImage(0, 0, width, height, bitmap);
}