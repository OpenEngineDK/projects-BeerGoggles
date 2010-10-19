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
#include <Display/IFrame.h>
#include <Display/OpenGL/SplitScreenCanvas.h>
#include <Display/PerspectiveViewingVolume.h>

#include <Utils/MeshCreator.h>

#include <Scene/MeshNode.h>
#include <Scene/TransformationNode.h>
#include <Core/IListener.h>
#include <Core/EngineEvents.h>

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

    

    // Create simple setup
    SimpleSetup* setup = new SimpleSetup("Beer Goggles");

    if (1) {
        IFontResourcePtr font =
            ResourceManager<IFontResource>::Create("Fonts/FreeSerifBold.ttf");
        font->Load();

        IFontResourcePtr datFont =
            ResourceManager<IFontResource>::Create("Fonts/palatino/pala.ttf");
        datFont->Load();

        WallCanvas *wc = new WallCanvas(setup->GetRenderer(), 
                                        setup->GetTextureLoader(),
                                        font);
    
        datFont->SetSize(42);
        wc->AddText("Fredagscafeen", datFont);
        wc->SetBackgroundColor(Vector<4,float>(0,0,1,1));
                                
        ICanvas *mainC = setup->GetCanvas();
        ICanvas *splitCanvas = new SplitScreenCanvas(*mainC, *wc);

        IFrame &frame = setup->GetFrame();
        frame.SetCanvas(splitCanvas);

        setup->GetMouse().MouseMovedEvent().Attach(*wc);
        setup->GetMouse().MouseButtonEvent().Attach(*wc);

    }

    MeshPtr mp = MeshCreator::CreateCube(10,1,Vector<3,float>(1,0,0));
    MeshNode* mn = new MeshNode(mp);

    TransformationNode* tn = new TransformationNode();

    TransformationNode *budT = new TransformationNode();

    IModelResourcePtr budModel = ResourceManager<IModelResource>::Create("Models/bud/models/bud.dae");
    budModel->Load();
    

    budT->AddNode(budModel->GetSceneNode());
    budT->SetRotation(Quaternion<float>(-PI/3.0, Vector<3,float>(1,0,0)));
    tn->AddNode(budT);

    Rotator *rot = new Rotator(tn);
    setup->GetEngine().ProcessEvent().Attach(*rot);

    //tn->AddNode(mn);
    ISceneNode *root = setup->GetScene();
    root->AddNode(tn);
    
    

    Camera *cam = new Camera(*(new PerspectiveViewingVolume(1,
                                                            3000,
                                                            4.0/(3.0*2.0),
                                                            PI/4.0)));
    cam->SetPosition(Vector<3,float>(10,10,-10));
    cam->LookAt(Vector<3,float>(0,5,0));

    setup->SetCamera(*cam);

    // Start the engine.
    setup->GetEngine().Start();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


