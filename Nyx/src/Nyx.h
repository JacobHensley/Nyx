#pragma once

/* Nyx Header File */

#include "Nyx/Application.h"
#include "Nyx/Common.h"
//#include "Nyx/GLError.h"
#include "Nyx/Log.h"

#include "Nyx/events/Event.h"
#include "Nyx/events/AppEvent.h"
#include "Nyx/events/KeyEvent.h"
#include "Nyx/events/MouseEvent.h"

//#include "Nyx/graphics/DebugRenderer.h"
#include "Nyx/Graphics/Window.h"

#include "Nyx/Graphics/ImGUI/ImGUILayer.h"

#include "Nyx/Graphics/layers/Layer.h"
#include "Nyx/Graphics/layers/LayerStack.h"

//#include "Nyx/graphics/renderer/Mesh.h"

#include "Nyx/graphics/renderer/API/BufferLayout.h"
#include "Nyx/graphics/renderer/API/FrameBuffer.h"
#include "Nyx/graphics/renderer/API/IndexBuffer.h"
#include "Nyx/graphics/renderer/API/Texture.h"
#include "Nyx/graphics/renderer/API/TextureCube.h"
#include "Nyx/graphics/renderer/API/VertexArray.h"
#include "Nyx/graphics/renderer/API/VertexBuffer.h"

#include "Nyx/graphics/renderer/camera/Camera.h"

#include "Nyx/graphics/renderer/shaders/Shader.h"
#include "Nyx/graphics/renderer/shaders/ShaderUniform.h"

#include "Nyx/Input/Input.h"
#include "Nyx/Input/KeyCodes.h"

#include "Nyx/math/AABB.h"
#include "Nyx/math/Ray.h"

#include "Nyx/scene/Scene.h"
#include "Nyx/scene/SceneObject.h"

#include "Nyx/scene/component/Component.h"
#include "Nyx/scene/component/ComponentCache.h"
//#include "Nyx/scene/component/MeshComponent.h"
#include "Nyx/scene/component/TransformComponent.h"