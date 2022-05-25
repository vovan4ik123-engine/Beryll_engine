#include "DrawAnyFunction.h"

namespace Beryll
{
    DrawAnyFunction::DrawAnyFunction(std::function<void()> fn)
    {
        m_action = fn;
    }

    DrawAnyFunction::~DrawAnyFunction()
    {

    }

    void DrawAnyFunction::updateBeforePhysics()
    {

    }

    void DrawAnyFunction::updateAfterPhysics()
    {

    }

    void DrawAnyFunction::draw()
    {
        m_action();
    }

    void DrawAnyFunction::playSound()
    {

    }

}