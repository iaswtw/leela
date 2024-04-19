#include "SceneObject.h"
#include "Component.h"


// transform to move this scene object from its parent's position
glm::mat4 SceneObject::getPositionTransform()
{
    return glm::mat4(1.0f);
}

// transform to rotate/orient this object in its local coordinate system
glm::mat4 SceneObject::getOrientationTransform()
{
    return glm::mat4(1.0f);
}

// combined transform
glm::mat4 SceneObject::getTransform()
{
    return getPositionTransform() * getOrientationTransform();
}

void SceneObject::addComponent(Component* component)
{
    component->setParent(this);
    _components.push_back(component);
}

// Add a child scene object
void SceneObject::addSceneObject(SceneObject* childObject)
{
    childObject->_setParent(this);					// tell the child it has a new parent
    _childSceneObjects.push_back(childObject);
}

void SceneObject::_setParent(SceneObject* parent)
{
    _sceneParent = parent;
    parentChanged();		// allow subclasses to process this change
}

void SceneObject::_printIndent(int indent)
{
    for (int i = 0; i < indent; i++)
        printf(" ");
}

void SceneObject::printTree(SceneObject* obj, int indent)
{
    _printIndent(indent);
    printf("%s:\n", obj->_name.c_str());

    indent += 4;
    for (Component* c : obj->_components)
    {
        _printIndent(indent);
        printf("%s\n", typeid(*c).name());
    }

    for (SceneObject* o : obj->_childSceneObjects)
    {
        printTree(o, indent);
    }
}
