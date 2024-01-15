#include "lith/font.h"
#include "lith/log.h"

// why is this here?
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static FontGeneratorInterface* backend;

void registerFontGeneratorInterface(FontGeneratorInterface* generator) {
    backend = generator;
}

FontGenerationInput::FontGenerationInput()
    : filepath          (nullptr)
    , generationScale   (0.f)
    , linePaddingTop    (0.f)
    , linePaddingBottom (0.f)
{}

FontGenerationOutput::FontGenerationOutput()
    : spaceAdvance (0.f)
    , lineHeight   (0.f)
    , topHeight    (0.f)
    , bottomHeight (0.f)
{}

FontGlyph FontGenerationOutput::getGlyph(int c) const {
    FontGlyph glyph = {};

    auto currentItr = glpyhs.find(c);

    if (currentItr != glpyhs.end()) {
        glyph = currentItr->second;
    }

    else {
        currentItr = glpyhs.find('?');

        if (currentItr == glpyhs.end()) {
            glyph.index = -1;
        }
    }

    return glyph;
}

Font::Font() 
    : characterPaddingX (0.f)
    , characterPaddingY (0.f)
    , linePaddingTop    (0.f)
    , linePaddingBottom (0.f)
    , tabSpaceCount     (4)
{}

Font& Font::source(const char* filepath) {
    this->input.filepath = filepath;
    return *this;
}

Font& Font::scale(float generationScale) {
    this->input.generationScale = generationScale;
    return *this;
}

Font& Font::linePadding(float paddingTop, float paddingBottom) {
    this->input.linePaddingTop = linePaddingTop;
    this->input.linePaddingBottom = linePaddingBottom;
    return *this;
}

Font& Font::characterPadding(float paddingX, float paddingY) {
    this->characterPaddingX = paddingX;
    this->characterPaddingY = paddingY;
    return *this;
}

Font& Font::generate() {
    print("Generating font texture atlas for {}...", input.filepath);
    data = backend->generate(input);
    print("Done");
    return *this;
}

struct Line {
    std::vector<TextMeshGlyph> glyphs;
    float width;
};

TextMesh Font::createTextMesh(const char* string, const TextMeshGenerationConfig& config) const {
    std::vector<Line> lines;
    Line line = {};

    vec2 cursor = vec2(0, 0);
	const int length = strlen(string);
    for (int i = 0; i < length; i++) {
        char c = string[i];

        if (c == '\r') {
            // just ignore because it wont overwrite the line, like in a console
            //cursor.x = 0;
            continue;
        }

        if (c == '\n') {
            lines.push_back(line);
            line = {};

            cursor.x = 0;
            cursor.y -= data.lineHeight * config.lineHeightScale + characterPaddingY;
            continue;
        }

        if (c == ' ') {
            float kerning = i != length - 1 
                ? getKerning(' ', string[i + 1]) 
                : 0;

            cursor.x += data.spaceAdvance + kerning + characterPaddingX;
            continue;
        }

        if (c == '\t') {
            cursor.x += tabSpaceCount * data.spaceAdvance + characterPaddingX;
            continue;
        }
        
        FontGlyph current = getGlyph(string[i]);

        if (current.index == -1) {
            // this is a missing character AND there is no ? character
            throw nullptr;
        }

        TextMeshGlyph glyph;
        glyph.posMin = (current.posMin + cursor) * data.lineHeight;
        glyph.posMax = (current.posMax + cursor) * data.lineHeight;
        glyph.uvMin = current.uvMin;
        glyph.uvMax = current.uvMax;
		glyph.textureHandle = data.atlas.getHandle();

        line.glyphs.push_back(glyph);
        line.width = max(line.width, glyph.posMax.x);

        // no need to advance for the last character
        if (i == length - 1) {
            continue;
        }

        cursor.x += current.advance + getKerning(string[i], string[i + 1]) + characterPaddingX;
    }

    lines.push_back(line);

    TextMesh mesh;
    mesh.create();

    vec2 alignmentCursor = vec2(0, 0);
    float totalHeight = data.lineHeight * (lines.size() - 1);

    switch (config.alignY) {
        case TextAlignTop:
            alignmentCursor.y = -data.topHeight;
            break;
        case TextAlignBottom:
            alignmentCursor.y = totalHeight + data.bottomHeight;
            break;
        case TextAlignCenter:
            alignmentCursor.y = totalHeight / 2.f - data.bottomHeight;
            break;
        case TextAlignBaseline:
            alignmentCursor.y = 0;
            break;
        default:
            // others are invalid
            throw nullptr;
            break;
    }

    for (const Line& line : lines) {

        switch (config.alignX) {
            case TextAlignLeft: 
			    alignmentCursor.x = 0;
			    break;
            case TextAlignCenter:
                alignmentCursor.x = -line.width / 2;
				break;
            case TextAlignRight:
                alignmentCursor.x = -line.width;
                break;
            default:
                // others are invalid
                throw nullptr;
                break;
        }

        for (const TextMeshGlyph& glyph : line.glyphs) {
            TextMeshGlyph alignedGlyph = glyph;
            alignedGlyph.posMin += alignmentCursor;
            alignedGlyph.posMax += alignmentCursor;

            mesh.addGlyph(alignedGlyph);
        }
    }

	return mesh;
}

void Font::upload() {
    data.atlas.upload();
}

void Font::download() {
    data.atlas.download();
}

void Font::free() {
    data.atlas.free();
    data.kerning = {};
    data.glpyhs = {};
}

void Font::activate(int unit) const {
    data.atlas.activate(unit);
}

void Font::activateImage(int unit) const {
    data.atlas.activateImage(unit);
}

int Font::getHandle() const {
    return data.atlas.getHandle();
}

int Font::getWidth() const {
    return data.atlas.getWidth();
}

int Font::getHeight() const {
    return data.atlas.getHeight();
}

float Font::getAspect() const {
    return data.atlas.getAspect();
}

FontGlyph Font::getGlyph(int c) const {
    return data.getGlyph(c);
}

float Font::getKerning(int c1, int c2) const {
    float kerningAdvance = 0;

    auto currentItr = data.glpyhs.find(c1);
    auto nextItr = data.glpyhs.find(c2);

    if (currentItr != data.glpyhs.end() && nextItr != data.glpyhs.end()) {
        auto kerningItr = data.kerning.find({ currentItr->second.index, nextItr->second.index });
        if (kerningItr != data.kerning.end()) {
			kerningAdvance = kerningItr->second;
		}
    }

    return kerningAdvance;
}

size_t hash_string(const char* string) {
    size_t result = 0;

    const size_t prime = 31;
    const size_t length = strlen(string);
    for (size_t i = 0; i < length; i++) {
        result = string[i] + (result * prime);
    }

    return result;
}

size_t hash_config(const TextMeshGenerationConfig& config) {
    size_t result = 0;
    const size_t prime = 31;

    result = (size_t)config.alignX + (result * prime);
    result = (size_t)config.alignY + (result * prime);
    result = (size_t)(config.lineHeightScale * 1000.f) + (result * prime);

    return result;
}

size_t hash_combine(size_t hash1, size_t hash2) {
    const size_t prime = 31;
    return hash1 + hash2 * prime;
}

TextMesh& FontTextMeshCache::getOrCreateTextMesh(const char* string, const TextMeshGenerationConfig& config, const Font& font) {
    size_t hash = hash_combine(hash_string(string), hash_config(config));
    auto itr = strings.find(hash);

    if (itr == strings.end()) {
        TextMesh mesh = font.createTextMesh(string, config);
        mesh.upload();

        itr = strings.insert(itr, { hash, { mesh }});
    }

    // keep in cache for 2 frames, idk the best config
    itr->second.keepAliveFrameCount = 2;

    return itr->second.mesh;
}

void FontTextMeshCache::clear() {
    for (auto it = strings.begin(); it != strings.end();) {
        it->second.keepAliveFrameCount -= 1;
        
        if (it->second.keepAliveFrameCount <= 0) {
            it = strings.erase(it);
        }

        else {
            it++;
        }
    }
}

void FontTextMeshCache::forceClear() {
    for (auto& [hash, string] : strings) {
        string.mesh.free();
    }
    strings = {};
}