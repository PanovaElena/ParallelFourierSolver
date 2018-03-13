#pragma once

typedef int Type;
typedef int Dir;
typedef const int Field;
typedef const int Coords;

static const Dir FromGridToArray = -1;
static const Dir FromArrayToGrid = 1;

static const Type Complex = -1;
static const Type Double = 1;

static const Field E = 0, B = 1;
static const Field Ex = 0, Ey = 1, Ez = 2;
static const Field Bx = 3, By = 4, Bz = 5;

static const Coords x = 0;
static const Coords y = 1;
static const Coords z = 2;

    
