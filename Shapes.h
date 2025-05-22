
#pragma once
struct PointDouble {
  public: 
    double x;
    double y;

    bool operator==(const PointDouble& b) const {
      return x == b.x and y == b.y;
    }

    bool operator!=(const PointDouble& b) const {
      return not (*this == b);
    }
};

struct PointInt {
  public:
    int x;
    int y;
};

struct RectInt {
  int x;
  int y;
  int width;
  int height;

  bool contains(const PointInt p) const {
    return (x <= p.x and p.x <= (x+width)) and (y <= p.y and p.y <= (y + height));
  }

  bool intersect(const RectInt r) const {
    return (contains(PointInt{r.x, r.y}) or contains(PointInt{r.x + r.width, r.y}) or contains(PointInt{r.x, r.y + height}) or contains(PointInt{r.x + width, r.y + height}));
  }
};

struct RectDouble {
  double x;
  double y;
  int width;
  int height;

  RectInt toRectInt() const {
    return RectInt{round(x), round(y), width, height};
  }

  PointInt toPointInt () const {
    RectInt r = toRectInt();
    return PointInt{r.x, r.y};
  }

  bool contains(const PointDouble p) const {
    return (x <= p.x and p.x <= (x+width)) and (y <= p.y and p.y <= (y + height));
  }

  bool intersect(const RectDouble r) const { 
    //Code adapted from the SFML graphics library
    const double interLeft   = max(x, r.x);
    const double interTop    = max(y, r.y);
    const double interRight  = min(x + width, r.x + r.width);
    const double interBottom = min(y + height, r.y + r.height);

    return (interLeft < interRight) && (interTop < interBottom); 
  }
};

static PointDouble NO_POINT{-69, -420}; 
//TFW no algebraic-type like enums from rust :_(
//Seriously this could just be an enum like 
//
//enum Intersects {
//  Yes(PointDouble),
//  No,
//};
//
//Which is so much more expressive and clear than having a dummy value to test against or having multiple functions
//This is why I hate C++ grrr (Clash Royale);

struct LineDouble {
  PointDouble p1;
  PointDouble p2; 

  PointDouble intersects(const LineDouble l) const {
    double m0 = slope();
    double m1 = l.slope();

    if (m0 == m1) return NO_POINT; //Lines are parallel

    //Calculate intersection point
    double x = (m1 * l.p1.x - m0 * p1.x + p1.y - l.p1.y) / (m1 - m0);
    double y = m0 * (x - p1.x) + p1.y;
    PointDouble intersect{x, y};

    //Make sure intersection is actually on the line segment itself (and not just the line)
    if(p1.x <= x and x <= p2.x) return intersect;
    else return NO_POINT;
  }

  double slope() const {
    return (p2.y - p1.y) / (p2.x - p1.x);
  }
};

struct CircleDouble {
  PointDouble center;
  int radius;
  
  //Finds point of intersect on rectangle (if there is one);
  PointDouble intersectRect(const RectDouble r) const {
    LineDouble l_1{PointDouble{r.x, r.y}, PointDouble{r.x + r.width, r.y}};
    LineDouble l_2{PointDouble{r.x, r.y}, PointDouble{r.x, r.y + r.height}};
    LineDouble l_3{PointDouble{r.x + r.width, r.y}, PointDouble{r.x + r.width, r.y + r.height}};
    LineDouble l_4{PointDouble{r.x, r.y+r.height}, PointDouble{r.x + r.width, r.y+r.height}};

    LineDouble c_to_r{center, PointDouble{r.x + r.width / 2, r.y + r.height / 2}};

    PointDouble i1 = c_to_r.intersects(l_1);
    PointDouble i2 = c_to_r.intersects(l_2);
    PointDouble i3 = c_to_r.intersects(l_3);
    PointDouble i4 = c_to_r.intersects(l_4);

    if(i1 != NO_POINT)      { return i1; }
    else if(i2 != NO_POINT) { return i2; }
    else if(i3 != NO_POINT) { return i3; }
    else if(i4 != NO_POINT) { return i4; }
    else { return NO_POINT; }
  }
};
