#include "src/apps/App.h"
#include "src/kernel/Kernel.h"

//Define the constructor
App::App(Kernel& kernel) : kernel(kernel) {}

void App::_setup_sprites() {
  // Nothing to do. Only implement this function if you have some sprites.
}

// void make_sprite(TFT_eSprite*& spr, TFT_eSPI* display, int width, int height, const uint16_t bitmap[]) {
//   if(spr) { return; } // Pointer already defined :/
//   spr = new TFT_eSprite(display);
//   spr->setSwapBytes(true);
//
//   spr->setColorDepth(16);
//   spr->createSprite(width, height);
//   spr->pushImage(0, 0, width, height, bitmap);
// }
//
 std::unique_ptr<TFT_eSprite> make_sprite(TFT_eSPI* tft, int width, int height, const uint16_t bitmap[])
{
  auto spr = std::make_unique<TFT_eSprite>(tft);
  spr->setSwapBytes(true);
  spr->setColorDepth(16);

  // Critical: verify the framebuffer was allocated (createSprite returns nullptr on failure)
  if (spr->createSprite(width, height) == nullptr) {
    Serial.println("createSprite failed (no framebuffer)");
    return nullptr; // unique_ptr destroys the sprite (and anything it allocated)
  }

  spr->pushImage(0, 0, width, height, bitmap);
  return spr;
}
