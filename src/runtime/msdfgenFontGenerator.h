#pragma once

#include "lith/font.h"

class msdfgenFontGenerator : public FontGeneratorInterface {
public:
    FontGenerationOutput generate(const FontGenerationInput& config) const override;
};