#pragma once

struct lithImageData {
	char* buffer;
	int width;
	int height;
	int channels;
};

// Make sure to free the buffer with 'free()'
lithImageData lithLoadImage(const char* filepath);
