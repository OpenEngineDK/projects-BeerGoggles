// main
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Meta/Config.h>
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Core/Engine.h>

// SimpleSetup
#include <Utils/SimpleSetup.h>

#include <Resources/SDLFont.h>
#include <Resources/AssimpResource.h>
#include <Resources/ResourceManager.h>

#include <Display/WallCanvas.h>
#include <Display/CanvasQueue.h>
#include <Display/IFrame.h>
#include <Display/OpenGL/SplitScreenCanvas.h>
#include <Display/OpenGL/TextureCopy.h>
#include <Display/PerspectiveViewingVolume.h>

#include <Utils/MeshCreator.h>

#include <Scene/MeshNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/RenderStateNode.h>
#include <Scene/DirectionalLightNode.h>

#include <Core/IListener.h>
#include <Core/EngineEvents.h>

#include <Renderers/OpenGL/Renderer.h>


#include "BoxRenderNode.h"

// Game factory
//#include "GameFactory.h"

// name spaces that we will be using.
// this combined with the above imports is almost the same as
// fx. import OpenEngine.Logging.*; in Java.
using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Display;
using namespace OpenEngine::Display::OpenGL;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Scene;

class Rotator : public IListener<OpenEngine::Core::ProcessEventArg> {
    TransformationNode *node;
public:
    Rotator(TransformationNode *node) : node(node) {}
    void Handle(OpenEngine::Core::ProcessEventArg arg) {
        float dt = arg.approx / 1000000.0;
        node->Rotate(0, dt, 0);
    }
};

class VerticeCollector : public ISceneNodeVisitor {

    IDataBlockPtr block;

public:
    VerticeCollector(ISceneNode& node) : block(IDataBlockPtr()) {
        node.Accept(*this);
    }

    void VisitMeshNode(MeshNode *meshNode) {

        MeshPtr mesh = meshNode->GetMesh();
        if (block.get()) {
            logger.info << "Mesh!" << logger.end;
            IDataBlockPtr vecs = mesh->GetGeometrySet()->GetVertices();
            int ns = vecs->GetSize() + block->GetSize();
            IDataBlockPtr nb = IDataBlockPtr(new DataBlock<3,float>(ns));
            int offset = sizeof(float) * 3 * block->GetSize();
            memcpy(nb->GetVoidDataPtr(), block->GetVoidDataPtr(), offset);
            memcpy((void*)((long)(nb->GetVoidDataPtr())+offset),
                   vecs->GetVoidDataPtr(), 
                   sizeof(float) * 3 * vecs->GetSize());
            block = nb;
        } else {
            IDataBlockPtr vecs = mesh->GetGeometrySet()->GetVertices();
            block = vecs;
        }
        meshNode->VisitSubNodes(*this);

    }

    IDataBlockPtr GetBlock() {
        return block;
    }

};

/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */
int main(int argc, char** argv) {
    // Print usage info.
    logger.info << "========= Running OpenEngine Test Project =========" << logger.end;

    DirectoryManager::AppendPath("projects/BeerGoggles/data/");
    
    ResourceManager<IFontResource>::AddPlugin(new SDLFontPlugin());
    ResourceManager<IModelResource>::AddPlugin(new AssimpPlugin());

    
    Vector<4,float> blueColor = Vector<4,float>(0,0,1,1);
 
    // Create simple setup
    SimpleSetup* setup = new SimpleSetup("Beer Goggles");

    if (1) {
        IFontResourcePtr font =
            ResourceManager<IFontResource>::Create("Fonts/FreeSerifBold.ttf");
        font->Load();

        IFontResourcePtr datFont =
            ResourceManager<IFontResource>::Create("Fonts/palatino/pala.ttf");
        datFont->Load();

        //TextureLoader& tl = setup->GetTextureLoader();

        IRenderer* r = new Renderer();
        TextureLoader* tl = new TextureLoader(*r);
        r->PreProcessEvent().Attach(*tl);


        WallCanvas *wc = new WallCanvas(new TextureCopy(), 
                                        *r, //setup->GetRenderer(), 
                                        *tl,
                                        font);
    
        datFont->SetSize(42);
        wc->AddText("Fredagscafeen", datFont);
        wc->SetBackgroundColor(blueColor);
        
        
        ITextureResourcePtr datcafeLogo = 
            ResourceManager<ITextureResource>::Create("datcafe_logo/datcafe_logo.png");
        datcafeLogo->Load();
        tl->Load(datcafeLogo);
        wc->AddTextureWithText(datcafeLogo, "Logo :D");


        // stuff
        ICanvas *mainC = setup->GetCanvas();

        wc->AddTextureWithText(mainC->GetTexture(), "3d!");

        ICanvas *splitCanvas = new SplitScreenCanvas(new TextureCopy(), *mainC, *wc);

        CanvasQueue *cq = new CanvasQueue();
        cq->PushCanvas(wc);
        cq->PushCanvas(mainC);
        

        IFrame &frame = setup->GetFrame();
        //frame.SetCanvas(splitCanvas);
        frame.SetCanvas(cq);
        setup->GetMouse().MouseMovedEvent().Attach(*wc);
        setup->GetMouse().MouseButtonEvent().Attach(*wc);

    }

    IRenderer& renderer = setup->GetRenderer();
    renderer.SetBackgroundColor(blueColor);
    renderer.SetBackgroundColor(Vector<4,float>(0,0,0,0));
    
        
   

    TransformationNode* tn = new TransformationNode();

    TransformationNode *budT = new TransformationNode();

    //IModelResourcePtr budModel = ResourceManager<IModelResource>::Create("Models/Euro 50L Keg/models/Euro 50L Keg.dae");
    IModelResourcePtr budModel = ResourceManager<IModelResource>::Create("Models/bud/models/bud.dae");
    budModel->Load();    


    budT->AddNode(budModel->GetSceneNode());
    budT->SetRotation(Quaternion<float>(-PI/3.0, Vector<3,float>(1,0,0)));
    tn->AddNode(budT);


    VerticeCollector f(*budT);
    IDataBlockPtr blk = f.GetBlock();
    
    
    Box mbox(blk);
    BoxRenderNode *bnode = new BoxRenderNode(mbox);   
    TransformationNode* boxT = new TransformationNode();
    boxT->AddNode(bnode);
    
    Quaternion<float> rQ = Quaternion<float>(-PI/2.0, Vector<3,float>(1,0,0));

    boxT->SetRotation(rQ);

    tn->AddNode(boxT);


    //tn->AddNode(mn);
    
    

    Camera *cam = new Camera(*(new PerspectiveViewingVolume(10,
                                                            3000,
                                                            4.0/(3.0*2.0),
                                                            PI/4.0)));

    logger.info << mbox.GetSize() << logger.end;

    float dist = mbox.GetSize()[2];
    cam->SetPosition(Vector<3,float>(dist,dist,-dist));

    Vector<3,float> cent = mbox.GetCenter();
    cent = rQ.RotateVector(cent);

    TransformationNode *tn2 = new TransformationNode();
    tn2->AddNode(tn);
    tn->SetPosition(-cent);
    Rotator *rot = new Rotator(tn2);
    setup->GetEngine().ProcessEvent().Attach(*rot);
    
    
    RenderStateNode* rsn = new RenderStateNode();
    rsn->EnableOption(RenderStateNode::LIGHTING);
    rsn->EnableOption(RenderStateNode::COLOR_MATERIAL);
    ISceneNode *root = rsn;

    root->AddNode(tn2);

    setup->SetScene(*root);

    cam->LookAt(Vector<3,float>(0.0));
    setup->SetCamera(*cam);

    // Light
    TransformationNode *lightT = new TransformationNode();
    
    DirectionalLightNode *light = new DirectionalLightNode();

    lightT->AddNode(light);
    lightT->SetPosition(Vector<3,float>(dist,dist,-dist));

    lightT->SetRotation(cam->GetDirection());

    root->AddNode(lightT);
    


    // Start the engine.
    setup->GetEngine().Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


