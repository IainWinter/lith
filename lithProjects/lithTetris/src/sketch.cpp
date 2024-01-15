#include "lith/sketch.h"
#include "lith/timer.h"
#include "lith/random.h"
#include <sstream>

#include "Tetris.h"

#include "lith/log.h"

Grid grid = Grid(10, 20);
Shape currentShape;
Shape heldShape;
RandomBag bag;

int score = 0;
std::string scoreString = "0\n0";
bool gameOver = false;
bool paused = false;

lithTimer moveDownTimer = lithTimer(1.f);
lithTimer lockTimer = lithTimer(.25f);
lithTimer inputTimer = lithTimer(.03f);

int moves = 0;

ivec2 lastMove = ivec2(0);
Shape hardDropShapePreview;

bool heldShapeThisFrame = false;
bool hitEdgeThisMove = false;

const float boxSize = 30;
const vec2 boardOffset = vec2(8, 1);

Font pixelFont;
Font arialFont;

Audio sfx_HitEdge;
Audio sfx_MoveDown;
Audio sfx_MoveSide;
Audio sfx_Placed;
Audio sfx_RotateClockwise;
Audio sfx_RotateCounterClockwise;

void loadAssets() {
    pixelFont
        .source("C:/dev/lith/framework/assets/Baskic8.ttf")
        .scale(32)
        .characterPadding(0.f, .4f)
        .linePadding(0, 4)
		.generate()
        .upload();

    arialFont
        .source("C:/dev/lith/framework/assets/arial.ttf")
        .scale(32)
		.generate()
        .upload();

    sfx_HitEdge.source("C:/dev/lithProjects/lithTetris/assets/HitEdge.wav").load();
    sfx_MoveDown.source("C:/dev/lithProjects/lithTetris/assets/MoveDown.wav").load();
    sfx_MoveSide.source("C:/dev/lithProjects/lithTetris/assets/MoveSide.wav").load();
    sfx_Placed.source("C:/dev/lithProjects/lithTetris/assets/Placed.wav").load();
    sfx_RotateClockwise.source("C:/dev/lithProjects/lithTetris/assets/RotateClockwise.wav").load();
    sfx_RotateCounterClockwise.source("C:/dev/lithProjects/lithTetris/assets/RotateCounterClockwise.wav").load();
}

void spawnNextShape() {
    currentShape = bag.getNextShape();
    currentShape.anchor = vec2(grid.cols / 2 - ceil(currentShape.cols / 2.f), grid.rows + 1);
    heldShapeThisFrame = false;
}

void gameUpdate() {
    // timer for when the shape should be moved down automatically
    // timer for when the shape should be locked in place
    // timer for inputs

    int clearedLines = 0;
    bool placedShape = false;

    if (moveDownTimer.passed(deltaTime)) {
        bool moved = tryMoveShape(currentShape, grid, ivec2(0, -1));
		if (moved) {
        	lockTimer.reset();
		}
    }

    if (lockTimer.passed(deltaTime)) {
        if (shapeCollidesGrid(currentShape, grid, ivec2(0, -1))) {
            if (shapeTotallyCollidesGrid(currentShape, grid)) {
				gameOver = true;
                return;
			}

			else {
				placeShape(currentShape, grid);
				spawnNextShape();
				clearedLines = clearLines(grid);
				placedShape = true;
			}
        }
    }


    ivec2 move = ivec2(0);
	if (keyDown[ButtonLeft]) {
		move.x -= 1;
	}

	if (keyDown[ButtonRight]) {
		move.x += 1;
	}

	if (keyDown[ButtonDown]) {
		move.y -= 1;
	}

    if (inputTimer.passed_until_reset(deltaTime)) {
		moves += 1;

		if (moves == 1) {
			inputTimer.delay = .1f;
		}

		else {
			inputTimer.delay = .03f;
		}

        if (move != ivec2(0)) {
            inputTimer.reset_if_passed();
        }

		else {
			moves = 0;
		}

		bool didMove = false;

		if (move.x != 0) {
			didMove = tryMoveShape(currentShape, grid, ivec2(move.x, 0));

            if (didMove) {
                if (lastMove.x != move.x) {
                    playSound(sfx_MoveSide);
                }
            }

            else {
                if (!hitEdgeThisMove) {
                    hitEdgeThisMove = true;
                    playSound(sfx_HitEdge);
                }
            }
		}

        else {
            hitEdgeThisMove = false;
		}

		if (move.y != 0) {
			didMove = tryMoveShape(currentShape, grid, ivec2(0, move.y));

            if (didMove && lastMove.y != move.y) {
                playSound(sfx_MoveDown);
            }
		}

		if (didMove) {
			lockTimer.reset();
		}

        lastMove = move;
    }

    // Rotation & hard drop which need to repeats

    if (keyCodeOnce == ButtonRotateClockwise) {
        if (tryRotateShape(currentShape, grid, Clockwise)) {
            playSound(sfx_RotateClockwise);
        }
    }

    else if (keyCodeOnce == ButtonRotateCounterClockwise) {
        if (tryRotateShape(currentShape, grid, CounterClockwise)) {
            playSound(sfx_RotateCounterClockwise);
        }
    }

    else if (keyCodeOnce == ButtonHardDrop) {
        // move shape down until it collides and then place it
        while (tryMoveShape(currentShape, grid, ivec2(0, -1))) {}

        placeShape(currentShape, grid);
        spawnNextShape();
        clearedLines = clearLines(grid);
        placedShape = true;
    }

    else if (keyCodeOnce == ButtonHold && !heldShapeThisFrame) {
        heldShapeThisFrame = true;

        if (heldShape.type != ShapeTypeNone) {
            std::swap(currentShape, heldShape);
            currentShape.anchor = vec2(grid.cols / 2 - ceil(currentShape.cols / 2.f), grid.rows + 1);
        }

        else {
            heldShape = currentShape;
            spawnNextShape();
        }

        heldShape.anchor = vec2(0);
    }

    if (clearedLines > 0) {
		switch (clearedLines) {
			case 1:
				score += 100;
				break;
			case 2:
				score += 300;
				break;
			case 3:
				score += 800;
				break;
			case 4:
				score += 1200;
				break;
		}

        std::stringstream ss;
        ss << score;
        scoreString = ss.str();

        //playSound(sfx_RotateClockwise);


        moveDownTimer.delay /= 2;
	}

    else if (placedShape) {
        playSound(sfx_Placed);
    }

    if (   hardDropShapePreview.type          != currentShape.type 
        || hardDropShapePreview.anchor.x      != currentShape.anchor.x
        || hardDropShapePreview.rotationIndex != currentShape.rotationIndex)
    {
		hardDropShapePreview = currentShape;
		while (tryMoveShape(hardDropShapePreview, grid, ivec2(0, -1))) {}
	}
}

void drawShape(const Shape& shape, vec2 offset = vec2(0.f), vec4 colorOverload = vec4(0.f)) {
    for (int c = 0; c < shape.cols; c++)
    for (int r = 0; r < shape.rows; r++)
    {
        vec4 color = colorLookup[shape.get(c, r)];

        if (color.w > 0 && colorOverload.w > 0) {
            color = colorOverload;
        }

        fill(color.x, color.y, color.z, color.w);
        
        vec2 pos = ( vec2(c, r) + shape.anchor + offset) * boxSize;
        rect(pos.x, pos.y, boxSize, boxSize);
    }
}

void drawGameBoard(const Grid& grid, vec2 offset) {
    noStroke();

    for (int c = 0; c < grid.cols; c++)
    for (int r = 0; r < grid.rows; r++)
    {
        vec4 color = colorLookup[grid.get(c, r)];
        fill(color.x, color.y, color.z, color.w);

        vec2 pos = (vec2(c, r) + offset) * boxSize;
        rect(pos.x, pos.y, boxSize, boxSize);
    }
    
    noStroke();
    drawShape(hardDropShapePreview, offset, vec4(128, 128, 128, 255));
    drawShape(currentShape, offset);

    noFill();
    stroke(255);
    rect(offset.x * boxSize, offset.y * boxSize, grid.cols * boxSize, grid.rows * boxSize);
}

void drawNextShapes(const RandomBag& bag, vec2 offset) {
    const int offsetX = grid.cols + 2;
    const int drawCount = 6;
    const int shapeSizeY = 3;

    // draw the next 6 shapes, first from the bag then bag2

    int remaining = 6;

    const int baselineY = grid.rows - bag.bag.size() * shapeSizeY;

    noStroke();

    // shapes are drawn from the bottom of the bag, so draw in reverse order
    // decrement drawCount as well
    for (int i = 0; i < bag.bag.size(); i++, remaining--) {
        drawShape(bag.bag[i], vec2(offsetX, baselineY + i * shapeSizeY) + offset);
    }

    const int baselineY2 = grid.rows - (drawCount - remaining + 1) * shapeSizeY;

    // draw the remaining shapes from bag2
    for (int i = 0; i < remaining; i++) {
        drawShape(bag.bag2[i], vec2(offsetX, baselineY2 - i * shapeSizeY) + offset);
    }
}

void drawHeldShape(const Shape& shape, vec2 offset) {
    if (shape.type == ShapeTypeNone) {
        return;
    }

    drawShape(shape, offset - vec2(4, 0));
}

void setup() {
	title("Tetris");
	size(800, 800);
	vsync(true);

    bag.generateNextPieces();

    loadAssets();
    spawnNextShape();

	textAlign(TextAlignCenter, TextAlignBaseline);
    textFont(pixelFont);
	textSize(64);
}

float x = 0;
bool y = false;

void draw() {
    background(23);

    textSize(64);
    textFont(pixelFont);
    textAlign(TextAlignRight, TextAlignTop);
    text(scoreString, boardOffset.x * boxSize, (boardOffset.y + grid.rows) * boxSize);

    if (!gameOver && !paused) {
        gameUpdate();
    }

    drawGameBoard(grid, boardOffset);
    drawNextShapes(bag, boardOffset);
    drawHeldShape(heldShape, boardOffset);

    if (gameOver) {
		noStroke();
		fill(0, 0, 0, 180);
		rect(0, 0, 800, 800);

        textFont(pixelFont);
		textAlign(TextAlignCenter, TextAlignCenter);
		text("GAME OVER\nPress 'r' to restart", 400, 400);
    }

    if (paused) {
        noStroke();
        fill(0, 0, 0, 180);
        rect(0, 0, 800, 800);

        textFont(pixelFont);
        textAlign(TextAlignCenter, TextAlignCenter);
        text("Paused", 400, 400);
    }
    
    print("Keycode: {}", keyCode);

    if (keyCode == ButtonResetGame) {
        gameOver = false;
        grid.reset();
        bag.reset();
        spawnNextShape();
    }

    if (keyCode == ButtonEscape) {
        paused = !paused;
    }

    //const char* vertex = R"(
    //    layout (location = 0) vec2 pos;
    //    layout (location = 1) vec2 uv;
    //)";

    //const char* fragment = R"(
    //    layout (location = 0) vec2 pos;
    //    layout (location = 1) vec2 uv;
    //)";

    //fillShader(vertex, fragment);
}