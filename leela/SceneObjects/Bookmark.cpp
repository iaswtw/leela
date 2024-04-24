#include "Bookmark.h"
#include "SphericalBody.h"

void Bookmark::parentChanged()
{
    _sphericalBody = dynamic_cast<SphericalBody*>(_sceneParent);
}
