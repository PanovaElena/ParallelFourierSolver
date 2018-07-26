#pragma once

enum Type {
    Complex,
    Double
};

enum Dimension {
    d0,
    d1,
    d2,
    d3
};

enum Field {
    E,
    B,
    J
};

enum Coordinate {
    x,
    y,
    z
};

typedef int Direction;

static const Direction FromGridToArray = -1;
static const Direction FromArrayToGrid = 1;

    
