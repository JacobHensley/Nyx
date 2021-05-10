#pragma once

/* Nyx Header File */

// Scene
#include "Nyx/scene/Components.h"
#include "Nyx/scene/Scene.h"
#include "Nyx/scene/SceneObject.h"
#include "Nyx/scene/SceneSerializer.h"

// Math
#include "Nyx/math/AABB.h"
#include "Nyx/math/Math.h"
#include "Nyx/math/Ray.h"
#include "Nyx/math/Triangle.h"

// Input
#include "Nyx/input/Input.h"
#include "Nyx/input/KeyCodes.h"

// Graphics
#include "Nyx/Graphics/Camera.h"
#include "Nyx/Graphics/GraphicsContext.h"
#include "Nyx/Graphics/LightEnvironment.h"
#include "Nyx/Graphics/Material.h"
#include "Nyx/Graphics/Mesh.h"
#include "Nyx/Graphics/Renderer.h"
#include "Nyx/Graphics/RenderPass.h"
#include "Nyx/Graphics/SceneRenderer.h"
#include "Nyx/Graphics/Shader.h"
#include "Nyx/Graphics/Window.h"
#include "Nyx/Graphics/API/BufferLayout.h"
#include "Nyx/Graphics/API/FrameBuffer.h"
#include "Nyx/Graphics/API/IndexBuffer.h"
#include "Nyx/Graphics/API/Texture.h"
#include "Nyx/Graphics/API/TextureCube.h"
#include "Nyx/Graphics/API/VertexArray.h"
#include "Nyx/Graphics/API/VertexBuffer.h"
#include "Nyx/Graphics/Debug/DebugRenderer.h"
#include "Nyx/Graphics/ImGUI/ImGUILayer.h"

// Events
#include "Nyx/Events/AppEvent.h"
#include "Nyx/Events/Event.h"
#include "Nyx/Events/KeyEvent.h"
#include "Nyx/Events/MouseEvent.h"

// Core
#include "Nyx/Core/Application.h"
#include "Nyx/Core/Core.h"
#include "Nyx/Core/Layer.h"
#include "Nyx/Core/LayerStack.h"
#include "Nyx/Core/Log.h"

// Asset
#include "Nyx/Asset/Asset.h"
#include "Nyx/Asset/AssetManager.h"
#include "Nyx/Asset/UUID.h"