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
#include "renderer/render_queue.hpp"
#include "threading/task_composer.hpp"
#include "util/hash.hpp"
#include "math/frustum.hpp"

#include <functional>

namespace Granite::Threaded
{

void scene_gather_opaque_renderables(const Scene &scene, TaskComposer &composer, const Frustum &frustum,
                                     VisibilityList *lists, const unsigned num_tasks);
void scene_gather_transparent_renderables(const Scene &scene, TaskComposer &composer, const Frustum &frustum,
                                          VisibilityList *lists, const unsigned num_tasks);
void scene_gather_static_shadow_renderables(const Scene &scene, TaskComposer &composer, const Frustum &frustum,
                                            VisibilityList *lists, Util::Hash *transform_hashes,
                                            const unsigned num_tasks, const std::function<bool ()> &cond = {});
void scene_gather_dynamic_shadow_renderables(const Scene &scene, TaskComposer &composer, const Frustum &frustum,
                                             VisibilityList *lists, Util::Hash *transform_hashes,
                                             const unsigned num_tasks, const std::function<bool ()> &cond = {});
void scene_gather_positional_light_renderables(const Scene &scene, TaskComposer &composer, const Frustum &frustum,
                                               VisibilityList *lists, const unsigned num_tasks);
void scene_gather_positional_light_renderables_sorted(const Scene &scene, TaskComposer &composer, const RenderContext &context,
                                                      PositionalLightList *lists, const unsigned num_tasks);

void compose_parallel_push_renderables(TaskComposer &composer, const RenderContext &context,
                                       RenderQueue *queues, const VisibilityList *visibility, const unsigned count);

void scene_update_cached_transforms(Scene &scene, TaskComposer &composer, const unsigned num_tasks);

}
