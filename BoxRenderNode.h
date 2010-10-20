#ifndef _BOX_RENDER_NODE_
#define _BOX_RENDER_NODE_

#include <Scene/RenderNode.h>
#include <Geometry/Box.h>

using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;

class BoxRenderNode : public RenderNode {
    Box box;
public:
    BoxRenderNode(Box b);
    void Apply(OpenEngine::Renderers::RenderingEventArg arg, ISceneNodeVisitor& v);
    
};

#endif
