#include "BoxRenderNode.h"
#include <Renderers/IRenderer.h>
#include <Meta/OpenGL.h>

using namespace OpenEngine::Renderers;

BoxRenderNode::BoxRenderNode(Box b) : box(b) {

}


void BoxRenderNode::Apply(OpenEngine::Renderers::RenderingEventArg arg, ISceneNodeVisitor& v) {
    IRenderer& renderer = arg.renderer;

    // bottom
    Line l1(box.GetCorner(0,0,0), box.GetCorner(1,0,0));
    Line l2(box.GetCorner(1,0,0), box.GetCorner(1,0,1));
    Line l3(box.GetCorner(1,0,1), box.GetCorner(0,0,1));
    Line l4(box.GetCorner(0,0,1), box.GetCorner(0,0,0));

    // top
    Line l5(box.GetCorner(0,1,0), box.GetCorner(1,1,0));
    Line l6(box.GetCorner(1,1,0), box.GetCorner(1,1,1));
    Line l7(box.GetCorner(1,1,1), box.GetCorner(0,1,1));
    Line l8(box.GetCorner(0,1,1), box.GetCorner(0,1,0));

    // sides

    Line l9 (box.GetCorner(0,1,0), box.GetCorner(0,0,0));
    Line l10(box.GetCorner(1,1,0), box.GetCorner(1,0,0));
    Line l11(box.GetCorner(0,1,1), box.GetCorner(0,0,1));
    Line l12(box.GetCorner(1,1,1), box.GetCorner(1,0,1));


    
    renderer.DrawLine(l1, Vector<3,float>(1,0,0));
    renderer.DrawLine(l2, Vector<3,float>(1,0,0));
    renderer.DrawLine(l3, Vector<3,float>(1,0,0));
    renderer.DrawLine(l4, Vector<3,float>(1,0,0));
    renderer.DrawLine(l5, Vector<3,float>(1,0,0));
    renderer.DrawLine(l6, Vector<3,float>(1,0,0));
    renderer.DrawLine(l7, Vector<3,float>(1,0,0));
    renderer.DrawLine(l8, Vector<3,float>(1,0,0));
    renderer.DrawLine(l9, Vector<3,float>(1,0,0));
    renderer.DrawLine(l10, Vector<3,float>(1,0,0));
    renderer.DrawLine(l11, Vector<3,float>(1,0,0));
    renderer.DrawLine(l12, Vector<3,float>(1,0,0));


    // hack
    glColor3f(1,1,1);
    
}
