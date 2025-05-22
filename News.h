#include "App.h"
#include "rssRead.hpp"

class Kernel;

const char *url = "https://feeds.npr.org/1003/rss.xml";

class News : public App
{
  Kernel* kernel;
  rssRead rss;
  public:
  News(Kernel* kernel);
  void run_code(double x, double y, bool special, Kernel* kernel);
  String get_name();
}  