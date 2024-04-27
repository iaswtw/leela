#include "ViewportSceneObject.h"
#include "UniverseMinimal.h"

#include <spdlog/spdlog.h>

ViewportSceneObject::ViewportSceneObject(ViewportType viewportType)
{
	_viewportType = viewportType;
}

void ViewportSceneObject::setDimensions(int x, int y, int w, int h)
{
	_x = x;
	_y = y;
	_w = w;
	_h = h;

	if (_h != 0) {
		_aspectRatio = float(_w) / _h;
	}

	bChanged = true;
}

void ViewportSceneObject::setWidth(int w)
{
	_w = w;
	_h = int(_w / _aspectRatio);

	bChanged = true;
}
