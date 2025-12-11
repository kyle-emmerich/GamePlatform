
#include "UI/UIFrame.h"
#include "UIFrame.generated.h"
#include "UI/UILayer.h"
#include "UILayer.generated.h"
#include "UI/UIBase.h"
#include "UIBase.generated.h"
#include "UI/UISurfaceLayer.h"
#include "UISurfaceLayer.generated.h"
#include "UI/UIScreenLayer.h"
#include "UIScreenLayer.generated.h"
#include "UI/UISystem.h"
#include "UISystem.generated.h"
#include "UI/UIButton.h"
#include "UIButton.generated.h"
#include "Core/Engine.h"
#include "Engine.generated.h"
#include "Core/LogSystem.h"
#include "LogSystem.generated.h"
#include "Input/InputSystem.h"
#include "InputSystem.generated.h"
#include "Input/Input.h"
#include "Input.generated.h"
#include "Input/IInputFocusable.h"
#include "IInputFocusable.generated.h"
#include "Platform/Viewport.h"
#include "Viewport.generated.h"
#include "Platform/PlatformWindow.h"
#include "PlatformWindow.generated.h"
#include "Network/NetworkServer.h"
#include "NetworkServer.generated.h"
#include "Instance/ObjectInstance.h"
#include "ObjectInstance.generated.h"
#include "Instance/World.h"
#include "World.generated.h"
#include "Instance/Instance.h"
#include "Instance.generated.h"
#include "Instance/System.h"
#include "System.generated.h"
#include "Rendering/Camera.h"
#include "Camera.generated.h"
#include "Replication/NetworkPeer.h"
#include "NetworkPeer.generated.h"
#include "Replication/Replicator.h"
#include "Replicator.generated.h"
#include "Replication/NetworkClient.h"
#include "NetworkClient.generated.h"
#include "Physics/BoxCollider.h"
#include "BoxCollider.generated.h"
#include "Physics/ColliderBase.h"
#include "ColliderBase.generated.h"
#include "Physics/Attachment.h"
#include "Attachment.generated.h"
#include "Physics/RigidBody.h"
#include "RigidBody.generated.h"
#include "Physics/PhysicsSystem.h"
#include "PhysicsSystem.generated.h"
#include "Physics/Constraints/ConstraintBase.h"
#include "ConstraintBase.generated.h"
#include "Physics/Constraints/HingeConstraint.h"
#include "HingeConstraint.generated.h"
#include "Assets/AssetBase.h"
#include "AssetBase.generated.h"
#include "Assets/MeshAsset.h"
#include "MeshAsset.generated.h"
#include "Assets/AssetSystem.h"
#include "AssetSystem.generated.h"


namespace Reflection {
	static void registerAllClasses() {
		Reflection::register_UIFrame();
		Reflection::register_UILayer();
		Reflection::register_UIBase();
		Reflection::register_UISurfaceLayer();
		Reflection::register_UIScreenLayer();
		Reflection::register_UISystem();
		Reflection::register_UIButton();
		Reflection::register_Engine();
		Reflection::register_LogSystem();
		Reflection::register_InputSystem();
		Reflection::register_Input();
		Reflection::register_IInputFocusable();
		Reflection::register_Viewport();
		Reflection::register_PlatformWindow();
		Reflection::register_NetworkServer();
		Reflection::register_ObjectInstance();
		Reflection::register_World();
		Reflection::register_Instance();
		Reflection::register_System();
		Reflection::register_Camera();
		Reflection::register_NetworkPeer();
		Reflection::register_Replicator();
		Reflection::register_NetworkClient();
		Reflection::register_BoxCollider();
		Reflection::register_ColliderBase();
		Reflection::register_Attachment();
		Reflection::register_RigidBody();
		Reflection::register_PhysicsSystem();
		Reflection::register_ConstraintBase();
		Reflection::register_HingeConstraint();
		Reflection::register_AssetBase();
		Reflection::register_MeshAsset();
		Reflection::register_AssetSystem();

	}
}
