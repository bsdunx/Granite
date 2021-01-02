/* Copyright (c) 2017-2020 Hans-Kristian Arntzen
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "renderer/scene.hpp"
#include "renderer/render_context.hpp"
#include "renderer/renderer.hpp"

#include <vector>

namespace Granite
{

class DeferredLights : public PerFrameRefreshable
{
public:
	void set_scene(Scene *scene);
	void set_renderers(const RendererSuite *suite);
	void set_enable_clustered_stencil_culling(bool state)
	{
		enable_clustered_stencil = state;
	}

	void render_prepass_lights(Vulkan::CommandBuffer &cmd, RenderQueue &queue, const RenderContext &context);
	void render_lights(Vulkan::CommandBuffer &cmd, RenderQueue &queue, const RenderContext &context);

private:
	Scene *scene = nullptr;
	const RendererSuite *renderer_suite = nullptr;
	VisibilityList visible;

	enum { NumClusters = 7 };

	VisibilityList clips;
	VisibilityList clusters[NumClusters];
	bool enable_clustered_stencil = false;

	void refresh(const RenderContext &context, TaskComposer &composer) override;
};

}