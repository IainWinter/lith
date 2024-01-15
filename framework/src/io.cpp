#include "lith/io.h"
#include "lith/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

lithImageData lithLoadImage(const char* filepath)
{
	int width, height, channels, format;
	stbi_info(filepath, &width, &height, &channels);

	stbi_set_flip_vertically_on_load(true);

	switch (channels)
	{
	case 1: format = STBI_grey;       break;
	case 2: format = STBI_grey_alpha; break;
	case 3: format = STBI_rgb;        break;
	case 4: format = STBI_rgb_alpha;  break;
	default:
		print("Failed to load image '{}'. Reason: Invalid number of channels ({})", filepath, channels);
		return {};
	}

	char* pixels = (char*)stbi_load(filepath, &width, &height, &channels, format);

	if (!pixels) {
		print("Failed to load image '{}'. Reason: {}", filepath, stbi_failure_reason());
	}

	return lithImageData{ pixels, width, height, channels };
}