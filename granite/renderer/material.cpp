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

#include "renderer/material.hpp"
#include "math/muglm/muglm_impl.hpp"

namespace Granite
{

void Material::bake()
{
	Util::Hasher h;
	for (auto &tex : textures)
		h.pointer(tex);
	for (unsigned i = 0; i < 4; i++)
		h.f32(base_color[i]);
	for (unsigned i = 0; i < 3; i++)
		h.f32(emissive[i]);
	h.f32(roughness);
	h.f32(metallic);
	h.f32(normal_scale);
	h.u32(Util::ecast(pipeline));
	h.u32(Util::ecast(sampler));
	h.u32(two_sided);
	h.u32(shader_variant);
	hash = h.get();
	needs_emissive = any(notEqual(emissive, vec3(0.0f)));
}

}
