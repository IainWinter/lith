#pragma once

struct color {
	char r, g, b, a;

	color() : r(0), g(0), b(0), a(0) {}
	color(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) {}
	color(char r, char g, char b) : r(r), g(g), b(b), a((char)255) {}
	color(float r, float g, float b, float a) : r(char(r * 255)), g(char(g * 255)), b(char(b * 255)), a(char(a * 255)) {}
	color(float r, float g, float b) : r(char(r * 255)), g(char(g * 255)), b(char(b * 255)), a((char)255) {}
};