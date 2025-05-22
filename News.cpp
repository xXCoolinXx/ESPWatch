#include "News.h"
#include "Kernel.h"

News::News(Kernel* kernel) : App(kernel) {
  this->kernel = kernel;
}

void News::run_code(double x, double y, bool special, Kernel* kernel) {}

String News::get_name() {
  return String("News");
}