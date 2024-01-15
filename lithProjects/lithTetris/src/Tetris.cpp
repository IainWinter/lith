#include "Tetris.h"
#include "lith/random.h"

const vec4 colorLookup[ShapeTypeCount + 1] = {
    vec4(0, 0, 0, 0),
    vec4(0, 100, 100, 255),
    vec4(200, 100, 100, 255),
    vec4(0, 200, 100, 255),
    vec4(200, 200, 100, 255),
    vec4(0, 200, 200, 255),
    vec4(200, 0, 100, 255),
    vec4(200, 200, 200, 255)
};

const Shape shapeLookup[7] = {
    // I
    Shape(I, 4, 1, { 1, 1, 1, 1 }),
    
    // J
    Shape(J, 3, 2, { 2, 0, 0,
                     2, 2, 2 }),
    
    // L
    Shape(L, 3, 2, { 0, 0, 3,
                     3, 3, 3 }),
    
    // O
    Shape(O, 2, 2, { 4, 4,
                     4, 4 }),

    // S
    Shape(S, 3, 2, { 0, 5, 5,
                     5, 5, 0 }),

    // T
    Shape(T, 3, 2, { 0, 6, 0,
                     6, 6, 6 }),

    // Z
    Shape(Z, 3, 2, { 7, 7, 0,
                     0, 7, 7 }),
};

// this table is indexed by the rotation index
const ivec2 offsetsForBasicRotation[4] = { { 2, -2 }, { -2, 1 }, { 1, -1 }, { -1, 2 } };

// this table is indexed by shape type
// the div factor has to do with the dimensions of the shape relative to the origin
// use int division to get 0 offset in some cases
const int divFactorForBasicRotation[7] = { 1, 2, 2, 0, 2, 2, 2 };

// this table is index by rotation index then test index - 1
const ivec2 kicksForJLTSZ[4][4] = {
    //              test 2     test 3      test 4      test 5
    /* 0 >> 1 */ {{ -1, 0 }, { -1,  1 }, { 0, -2 }, { -1, -2 }},
    /* 1 >> 2 */ {{  1, 0 }, {  1, -1 }, { 0,  2 }, {  1,  2 }},
    /* 2 >> 3 */ {{  1, 0 }, {  1,  1 }, { 0, -2 }, {  1, -2 }},
    /* 3 >> 0 */ {{ -1, 0 }, { -1, -1 }, { 0,  2 }, { -1,  2 }}
};

// this table is index by rotation index then test index - 1
const ivec2 kicksForI[4][4] = {
    //              test 2     test 3      test 4      test 5
    /* 0 >> 1 */ {{ -2, 0 }, {  1,  0 }, { -2, -1 }, {  1,  2 }},
    /* 1 >> 2 */ {{ -1, 0 }, {  2,  0 }, { -1,  2 }, {  2, -1 }},
    /* 2 >> 3 */ {{  2, 0 }, { -1,  0 }, {  2,  1 }, { -1, -2 }},
    /* 3 >> 0 */ {{  1, 0 }, { -2,  0 }, {  1, -2 }, { -2,  1 }}
};

Shape::Shape()
    : type          (ShapeTypeNone)
    , cols          (0)
    , rows          (0)
    , blocks        (0)
    , anchor        (0, 0)
    , rotationIndex (0)
{}

Shape::Shape(ShapeType type, int cols, int rows, std::vector<int> blocks)
    : type          (type)
    , cols          (cols)
    , rows          (rows)
    , blocks        (blocks)
    , anchor        (0, 0)
    , rotationIndex (0)
{
    if (blocks.size() == 0) {
        this->blocks.resize(cols * rows);
    }
}
    
int Shape::index(int c, int r) const {
    r = rows - 1 - r; // index the blocks in reverse y direction

    if (outOfBounds(c, r))
        return -1;
        
    return c + r * cols;
}
    
int Shape::get(int c, int r) const {
    if (outOfBounds(c, r))
        return -1;
        
    return blocks[index(c, r)];
}

void Shape::set(int c, int r, int type) {
    if (outOfBounds(c, r))
        return;

    blocks[index(c, r)] = type;
}

bool Shape::outOfBounds(int c, int r) const {
    return c < 0 || r < 0 || c >= cols || r >= rows;
}

Grid::Grid(int cols, int rows)
    : cols (cols)
    , rows (rows)
{
    grid = new int[cols * rows];
    reset();
}
    
Grid::~Grid() {
    delete[] grid;
}

void Grid::reset() {
    for (int i = 0; i < cols * rows; i++)
        grid[i] = 0;
}
    
int Grid::index(int c, int r) const {
    if (outOfBounds(c, r))
        return -1;
        
    return c + r * cols;
}
    
int Grid::get(int c, int r) const {
    if (outOfBounds(c, r))
        return -1;
        
    return grid[index(c, r)];
}
    
void Grid::set(int c, int r, int type) {
    if (outOfBounds(c, r))
        return;
        
    grid[index(c, r)] = type;
}

void RandomBag::generateNextPieces() {
    bag = std::move(bag2);

    for (int i = 0; i < ShapeTypeCount; i++) {
        bag2.insert(bag2.begin() + rand_im(bag2.size()), shapeLookup[i]);
    }

    // shuffle the bag
    for (int i = 0; i < ShapeTypeCount; i++) {
        std::swap(bag2[i], bag2[rand_im(bag2.size())]);
    }
}

Shape RandomBag::getNextShape() {
    if (bag.empty()) {
        generateNextPieces();
    }

    Shape out = std::move(bag.back());
    bag.pop_back();

    return out;
}

void RandomBag::reset() {
    generateNextPieces();
    generateNextPieces();
}

bool Grid::outOfBounds(int c, int r) const {
    // don't collide with top of grid
    return c < 0 || r < 0 || c >= cols || r >= rows;
}

int wrap(int x, int max) {
    return x < 0 ? max + x : x % max;
}

int getRotationSign(Rotation rotation) {
    return rotation == Clockwise ? 1 : -1;
}

int getRootRotationIndex(int rotationIndex, Rotation rotation) {
    if (rotation == CounterClockwise) {
        return wrap(rotationIndex - 1, 4);
    }

    return rotationIndex;
}

int getNextRotationIndex(int rotationIndex, Rotation rotation) {
    return wrap(rotationIndex + 1 * getRotationSign(rotation), 4);
}

vec2 getBaseRotationOffset(ShapeType shapeType, int rotationIndex, Rotation rotation) {
    // O shape has no rotation
    // could also make divFactor 3
    if (shapeType == O) {
        return vec2(0);
    }

    int sign = getRotationSign(rotation);
    int rootRotationIndex = getRootRotationIndex(rotationIndex, rotation);

    ivec2 offset = sign * offsetsForBasicRotation[rootRotationIndex] / divFactorForBasicRotation[shapeType];

    return (vec2)offset;
}

vec2 getKickOffset(ShapeType shapeType, int rotationIndex, Rotation rotation, int testIndex) {
    // O shape has no kicks
    if (shapeType == O) {
        return vec2(0);
    }

    int sign = getRotationSign(rotation);
    int rootRotationIndex = getRootRotationIndex(rotationIndex, rotation);

    ivec2 offset = shapeType == I
        ? sign * kicksForI    [rootRotationIndex][testIndex]
        : sign * kicksForJLTSZ[rootRotationIndex][testIndex];

    return (vec2)offset;
}

bool shapeCollidesGrid(const Shape& shape, const Grid& grid, vec2 offset) {
    // This is a hack, if the shape has no blocks it is colliding
    if (shape.blocks.size() == 0) {
        return true;
    }

    for (int sc = 0; sc < shape.cols; sc++)
    for (int sr = 0; sr < shape.rows; sr++)
    {
        if (shape.get(sc, sr) == 0) {
            continue;
        }
        
        int gc = sc + (int)(shape.anchor.x + offset.x);
        int gr = sr + (int)(shape.anchor.y + offset.y);
        
        gr = clamp(gr, -1, grid.rows - 1);
        
        if (grid.get(gc, gr) != 0) {
            return true;
        }
    }
    
    return false;
}

bool shapeTotallyCollidesGrid(const Shape& shape, const Grid& grid) {
    for (int sc = 0; sc < shape.cols; sc++)
    for (int sr = 0; sr < shape.rows; sr++)
    {
        if (shape.get(sc, sr) == 0) {
            continue;
        }
        
        int gc = sc + (int)shape.anchor.x;
        int gr = sr + (int)shape.anchor.y;
        
        if (gr < grid.rows) {
            if (grid.get(gc, gr) == 0) {
                return false;
            }
        }
    }
    
    return true;
}

bool tryMoveShape(Shape& shape, const Grid& grid, ivec2 move) {
    shape.anchor += move;
    if (shapeCollidesGrid(shape, grid)) {
        shape.anchor -= move;
        return false;
    }

    return true;
}

void placeShape(const Shape& shape, Grid& grid) {
    for (int sc = 0; sc < shape.cols; sc++)
    for (int sr = 0; sr < shape.rows; sr++)
    {
        int sblock = shape.get(sc, sr);

        if (sblock == 0) {
            continue;
        }

        int gc = sc + (int)shape.anchor.x;
        int gr = sr + (int)shape.anchor.y;

        grid.set(gc, gr, sblock);
    }
}

int clearLines(Grid& grid) {
    int numbLinesCleared = 0;

    for (int gr = 0; gr < grid.rows; gr++) {
        bool rowFilled = true;
        for (int gc = 0; gc < grid.cols; gc++) {
            if (grid.get(gc, gr) == 0) {
                rowFilled = false;
                break;
            }
        }

        if (rowFilled) {
            int line = gr;
            for (int swap = line + 1; swap < grid.rows; swap++, line++) {
                for (int swapc = 0; swapc < grid.cols; swapc++) {
                    int block = grid.get(swapc, swap);
                    grid.set(swapc, line, block);
                }
            }

            gr--;
            numbLinesCleared++;
        }
    }

    return numbLinesCleared;
}

Shape getRotatedShape(const Shape& shape, Rotation rotation) {
    Shape newShape = Shape(shape.type, shape.rows, shape.cols, {});
    newShape.rotationIndex = getNextRotationIndex(shape.rotationIndex, rotation);
    newShape.anchor = shape.anchor + getBaseRotationOffset(shape.type, shape.rotationIndex, rotation);

    if (rotation == Clockwise) {
        for (int sc = 0; sc < shape.cols; sc++)
        for (int sr = 0; sr < shape.rows; sr++)
	    {
            int sblock = shape.get(sc, sr);

            int nc = sr;
            int nr = shape.cols - 1 - sc;

            newShape.set(nc, nr, sblock);
	    }
    }

    // counter-clockwise
    else {
        for (int sc = 0; sc < shape.cols; sc++)
        for (int sr = 0; sr < shape.rows; sr++)
        {
            int sblock = shape.get(sc, sr);

            int nc = shape.rows - 1 - sr;
            int nr = sc;

            newShape.set(nc, nr, sblock);
        }
    }

    return newShape;
}

bool tryRotateShape(Shape& shape, const Grid& grid, Rotation rotation) {
    Shape newShape = getRotatedShape(shape, rotation);

    if (!shapeCollidesGrid(newShape, grid)) {
        shape = newShape;
        return true;
    }

    // If the shape collides the grid, then test the kicks
    printf("Basic rotation failed, trying kicks\n");

    for (int testIndex = 0; testIndex < 5; testIndex++) {
        vec2 kickOffset = getKickOffset(newShape.type, newShape.rotationIndex, rotation, testIndex);

        if (!shapeCollidesGrid(newShape, grid, kickOffset)) {
            printf("Kick %d works\n", testIndex);
            newShape.anchor += kickOffset;
            
            shape = newShape;
            return true;
        }
    }

    printf("All kicks failed\n");
    return false;
}