#include "src/apps/Snake.h"
#include "src/kernel/Kernel.h"

SnakeDir Snake::getDirFromTouch(const TouchHandler::oGesture& gesture, const oPointInt& pt) {
  if(gesture.has_value()) {
    switch(gesture.value()) {
      case TouchHandler::Gesture::SWIPE_UP:
        return SNAKE_UP;
        break;
      case TouchHandler::Gesture::SWIPE_DOWN:
        return SNAKE_DOWN;
        break;
      case TouchHandler::Gesture::SWIPE_LEFT:
        return SNAKE_LEFT;
        break;
      case TouchHandler::Gesture::SWIPE_RIGHT:
        return SNAKE_RIGHT;
        break;
      default:
        return SNAKE_NONE;
        break;
    }
  }
  return SNAKE_NONE;
}

Snake::Snake(Kernel& kernel) : App(kernel) {
  this->body.push_back(SnakePart());
  this->kernel.loadSmallFont();
  this->move_apple();
}

void Snake::run_code(const TouchHandler::oGesture& gesture, const oPointInt& pt) {
  this->kernel.drawViewBox();

  time_since_last_move += this->kernel.getDeltaTime();
  
  this->kernel.display.setTextColor(TFT_WHITE, TFT_BLACK, true);
  // this->kernel.display.drawString(String(time_since_last_move), 50, 50);
  
  // Cache the gesture for the next loop update
  if (!this->cached_gesture.has_value()) {
    this->cached_gesture = gesture;
  }

  if(time_since_last_move >= spm) {
    time_since_last_move -= spm;
    
    this->kernel.display.fillRect(this->apple.x, this->apple.y, this->apple.width, this->apple.height, TFT_RED);

    SnakeDir new_direction = this->getDirFromTouch(this->cached_gesture, pt);
    this->cached_gesture.reset(); // Reset the cache

    bool add_part = false;
    bool restarted = false;
    RectInt prev_tail;

    // Direction propagates from the head to the tail: each segment takes the
    // direction the segment ahead of it had at the start of this tick.
    for(size_t i = 0; i < this->body.size(); i++) {
      SnakePart& seg = this->body[i];
      prev_tail = seg.part; // Pre-move position (last iteration = old tail spot)

      SnakeDir old_direction = new_direction;
      new_direction = seg.dir;

      if(old_direction != SNAKE_NONE && 
        !(new_direction == SNAKE_LEFT && old_direction == SNAKE_RIGHT) && !(new_direction == SNAKE_RIGHT && old_direction == SNAKE_LEFT) &&
        !(new_direction == SNAKE_UP && old_direction == SNAKE_DOWN) && !(new_direction == SNAKE_DOWN && old_direction == SNAKE_UP) ) {
        seg.dir = old_direction;
      } else {
        // Serial.println("Snake dir is none!!!");
      }

      switch(seg.dir) {
        case SNAKE_LEFT:
          seg.part.x -= snake_dim;
          break;
        case SNAKE_RIGHT:
          seg.part.x += snake_dim;
          break;
        case SNAKE_UP:
          seg.part.y -= snake_dim;
          break;
        case SNAKE_DOWN:
          seg.part.y += snake_dim;
          break;
        default:
          break;
      }

      if(i == 0 && seg.part.x == this->apple.x && seg.part.y == this->apple.y) {
        add_part = true;
        this->move_apple();
      }

      this->kernel.display.fillRect(seg.part.x, seg.part.y, seg.part.width, seg.part.height, TFT_GREEN);

      // Head collided with part of its body :(
      if((i != 0 && this->body[0].part.x == seg.part.x && this->body[0].part.y == seg.part.y)
      || this->body[0].part.x <= left_vb || this->body[0].part.x + this->body[0].part.width >= right_vb
      || this->body[0].part.y <= top_vb || this->body[0].part.y + this->body[0].part.height >= bottom_vb ) {
        this->delete_snake(); // Clears body, adds a fresh head segment
        this->kernel.clearNext();
        this->move_apple();
        this->cached_gesture.reset();
        restarted = true;
        break;
      }
    }

    // Grow the tail (never after a restart - the body was just rebuilt)
    if(add_part && !restarted) {
      this->body.push_back(SnakePart(prev_tail, new_direction));
    }

    // Clear the tail from the screen

    // this->kernel.display.drawString((this->body[0].part.x == prev_tail.x ? "true" : "false"), 50, 50);
    // this->kernel.display.drawString((this->body[0].part.y == prev_tail.y ? "true" : "false"), 50, 100);

    if(!add_part && this->body[0].part.x != prev_tail.x || this->body[0].part.y != prev_tail.y) {
      this->kernel.display.fillRect(prev_tail.x, prev_tail.y, prev_tail.width, prev_tail.height, TFT_BLACK);
      
    }
  }
}

String Snake::get_name() {
  return String("Snake");
}

// Clears the body and starts a fresh one-segment snake
void Snake::delete_snake() {
  this->body.clear();
  this->body.push_back(SnakePart());
}

void Snake::move_apple() {
  this->kernel.display.fillRect(this->apple.x, this->apple.y, this->apple.width, this->apple.height, TFT_BLACK);
  int rand_x = random(border_space, viewbox_wh / snake_dim - border_space) * snake_dim;
  int rand_y = random(border_space, viewbox_wh / snake_dim - border_space) * snake_dim;

  this->apple.x = left_vb + rand_x;
  this->apple.y = top_vb + rand_y;
  this->kernel.display.fillRect(this->apple.x, this->apple.y, this->apple.width, this->apple.height, TFT_RED);
}

