#include "App.h"
// #include "rssRead.hpp"

class Kernel;


class News : public App
{
  unsigned short news_idx = 1;
  String news_summary = "";
  Kernel* kernel;
  bool last_x = false;
  TFT_eSprite* news_sprite = nullptr;
  public:
  News(Kernel* kernel);
  ~News();
  void run_code(const TouchPoints& tp);
  String get_name();

  // Put it on the news scyeah
  void updateNews();
};
