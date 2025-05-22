#include "Snake.h"
#include "Kernel.h"

SnakeDir Snake::getdirFromJoystick(double x, double y) {
  // Since we are using a joystick yet can only go one direction at a time, choose based on the higher value
  if(x == 0 && y == 0) { return SNAKE_NONE; }
  
  if(x != 0 && y != 0) {
    if(abs(x) > abs(y)) {
      y = 0;
    } else {
      x = 0;
    }
  }

  if(x != 0) { // and !this->last_x) {
    return (x < 0 ? SNAKE_LEFT : SNAKE_RIGHT );
  }

  if(y != 0) {// and !this->last_y) {
    return (y > 0 ? SNAKE_DOWN : SNAKE_UP );
  }

  return SNAKE_NONE;
}

Snake::Snake(Kernel* kernel) : App(kernel) {
  this->kernel = kernel;
  this->head = new SnakePart;
  this->kernel->loadSmallFont();
  this->move_apple();
}

void Snake::run_code(double x, double y, bool special) {
  this->kernel->drawViewBox();

  time_since_last_move += this->kernel->getDeltaTime();
  
  this->kernel->display.setTextColor(TFT_WHITE, TFT_BLACK, true);
  // this->kernel->display.drawString(String(time_since_last_move), 50, 50);

  if(time_since_last_move >= spm) {
    time_since_last_move -= spm;
    
    this->kernel->display.fillRect(this->apple.x, this->apple.y, this->apple.width, this->apple.height, TFT_RED);

    SnakeDir new_direction = getdirFromJoystick(x, y);

    bool add_part = false;
    RectInt prev_tail;
    auto ptr = this->head;
    auto tail = ptr;
    while(ptr != nullptr) {
      // Propogate update direction for the next one
      SnakeDir old_direction = new_direction;
      new_direction = ptr->dir;
      
      prev_tail = ptr->part;

      if(old_direction != SNAKE_NONE && 
        !(new_direction == SNAKE_LEFT && old_direction == SNAKE_RIGHT) && !(new_direction == SNAKE_RIGHT && old_direction == SNAKE_LEFT) &&
        !(new_direction == SNAKE_UP && old_direction == SNAKE_DOWN) && !(new_direction == SNAKE_DOWN && old_direction == SNAKE_UP) ) {
        ptr->dir = old_direction;
      } else {
        Serial.println("Snake dir is none!!!");
      }

      switch(ptr->dir) {
        case SNAKE_LEFT:
          ptr->part.x -= snake_dim;
          break;
        case SNAKE_RIGHT:
          ptr->part.x += snake_dim;
          break;
        case SNAKE_UP:
          ptr->part.y -= snake_dim;
          break;
        case SNAKE_DOWN:
          ptr->part.y += snake_dim;
          break;
        default:
          break;
      }

      if(ptr == head && ptr->part.x == this->apple.x && ptr->part.y == this->apple.y) {
        add_part = true;
        this->move_apple();
      }

      this->kernel->display.fillRect(ptr->part.x, ptr->part.y, ptr->part.width, ptr->part.height, TFT_GREEN);

      // Head collided with part of its body :(
      if((ptr != this->head && this->head->part.x == ptr->part.x && this->head->part.y == ptr->part.y)
      || this->head->part.x <= left_vb || this->head->part.x + this->head->part.width >= right_vb
      || this->head->part.y <= top_vb || this->head->part.y + this->head->part.height >= bottom_vb ) {
        delete_snake();
        this->kernel->clearNext();
        this->move_apple();
        this->head = new SnakePart;
        break;
      }

      ptr = ptr->next;
      if(ptr != nullptr) {
        tail = ptr;
      } else {
        break;
      }
    }

    // Add part
    if(add_part) {
      tail->next = new SnakePart{prev_tail, new_direction, nullptr};
    }

    // Clear the tail from the screen

    // this->kernel->display.drawString((this->head->part.x == prev_tail.x ? "true" : "false"), 50, 50);
    // this->kernel->display.drawString((this->head->part.y == prev_tail.y ? "true" : "false"), 50, 100);

    if(!add_part && this->head->part.x != prev_tail.x || this->head->part.y != prev_tail.y) {
      this->kernel->display.fillRect(prev_tail.x, prev_tail.y, prev_tail.width, prev_tail.height, TFT_BLACK);
      
    }

    this->last_x = x != 0;
    this->last_y = y != 0;
  }
}

String Snake::get_name() {
  return String("newApp");
}

void Snake::delete_snake() {
  auto ptr = this->head;
  while(ptr != nullptr) {
    auto tmp = ptr->next;
    delete ptr;
    ptr = tmp;
  }
}

void Snake::move_apple() {
  this->kernel->display.fillRect(this->apple.x, this->apple.y, this->apple.width, this->apple.height, TFT_BLACK);
  int rand_x = random(border_space, viewbox_wh / snake_dim - border_space) * snake_dim;
  int rand_y = random(border_space, viewbox_wh / snake_dim - border_space) * snake_dim;

  this->apple.x = left_vb + rand_x;
  this->apple.y = top_vb + rand_y;
  this->kernel->display.fillRect(this->apple.x, this->apple.y, this->apple.width, this->apple.height, TFT_RED);
}

Snake::~Snake() {
 this->delete_snake(); 
}

