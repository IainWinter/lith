#pragma once

#include "lith/math.h"
#include "lith/input.h"

enum ShapeType {
    ShapeTypeNone = -1,
    I, J, L, O, S, T, Z,

    ShapeTypeCount
};

enum Rotation {
    Clockwise,
    CounterClockwise
};

enum Button {
    ButtonLeft = Key_Left,
    ButtonRight = Key_Right,
    ButtonDown = Key_Down,
    ButtonRotateCounterClockwise = Key_Z,
    ButtonRotateClockwise = Key_X,
    ButtonHardDrop = Key_Space,
    ButtonHold = Key_C,

    ButtonResetGame = Key_R,
    ButtonEscape = Key_Escape,
};

struct Shape {
    ShapeType type;
    std::vector<int> blocks;
    int cols;
    int rows;

    vec2 anchor;
    int rotationIndex;

    Shape();
    Shape(ShapeType type, int cols, int rows, std::vector<int> blocks);

    int index(int c, int r) const;
    int get(int c, int r) const;
    void set(int c, int r, int type);
    bool outOfBounds(int c, int r) const;
};

struct Grid {
    int* grid;
    int cols;
    int rows;

    Grid(int cols, int rows);
    ~Grid();

    void reset();
    int index(int c, int r) const;
    int get(int c, int r) const;
    void set(int c, int r, int type);
    bool outOfBounds(int c, int r) const;
};

struct RandomBag {
    std::vector<Shape> bag;
    std::vector<Shape> bag2; // so there is always some pieces to draw on the side

    void generateNextPieces();
    Shape getNextShape();
    void reset();
};

// this table is indexed by shape type + 1, 0 is empty
extern const vec4 colorLookup[ShapeTypeCount + 1];

// this table is indexed by shape type
extern const Shape shapeLookup[ShapeTypeCount];

// this table is indexed by the rotation index
extern const ivec2 offsetsForBasicRotation[4];

// this table is indexed by shape type
// the div factor has to do with the dimensions of the shape relative to the origin
// use int division to get 0 offset in some cases
extern const int divFactorForBasicRotation[ShapeTypeCount];

// this table is index by rotation index then test index - 1
extern const ivec2 kicksForJLTSZ[4][4];

// this table is index by rotation index then test index - 1
extern const ivec2 kicksForI[4][4];

int wrap(int x, int max);
int getRotationSign(Rotation rotation);
int getRootRotationIndex(int rotationIndex, Rotation rotation);
int getNextRotationIndex(int rotationIndex, Rotation rotation);

vec2 getBaseRotationOffset(ShapeType shapeType, int rotationIndex, Rotation rotation);
vec2 getKickOffset(ShapeType shapeType, int rotationIndex, Rotation rotation, int testIndex);

bool shapeCollidesGrid(const Shape& shape, const Grid& grid, vec2 offset = vec2(0, 0));
bool shapeTotallyCollidesGrid(const Shape& shape, const Grid& grid);
bool tryMoveShape(Shape& shape, const Grid& grid, ivec2 move);
void placeShape(const Shape& shape, Grid& grid);
int clearLines(Grid& grid);
Shape getRotatedShape(const Shape& shape, Rotation rotation);
bool tryRotateShape(Shape& shape, const Grid& grid, Rotation rotation);