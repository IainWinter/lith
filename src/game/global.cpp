#include "global.h"

BulletRenderer* g_bulletRenderer;

void registerBulletRenderer(BulletRenderer* bulletRenderer) {
	g_bulletRenderer = bulletRenderer;
}
