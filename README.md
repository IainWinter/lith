# lith

This project is an attempt to recreate the processing API in C++.

## Framework

The framework is a set of tools which provide a rendering, audio, and math api.

## Runtime

The runtime facilitates the creation and execution of sketches.

## The default sketch

When creating a new project, the runtime makes this skeleton.

```C++
#include "lith/sketch.h"

void setup() {
	title("Welcome to lith");
	size(480, 480);
}

void draw() {
	background(23);
	stroke(245);
	line(width/2, height/2, mouseX, mouseY);
}
```
