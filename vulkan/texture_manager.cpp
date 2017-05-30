#include "texture_manager.hpp"
#include "device.hpp"
#include "stb_image.h"

#include "gli/load.hpp"

using namespace std;

namespace Vulkan
{
Texture::Texture(Device *device, const std::string &path, VkFormat format)
	: VolatileSource(path), device(device), format(format)
{
	init();
}

void Texture::update(const void *data, size_t size)
{
	static const uint8_t png_magic[] = {
		0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a,
	};

	if (size >= sizeof(png_magic) && memcmp(data, png_magic, sizeof(png_magic)) == 0)
		update_png(data, size);
	else
		update_gli(data, size);
}

static VkFormat gli_format_to_vk(gli::format format)
{
#define fmt(g, vk) \
	case gli::format::FORMAT_##g: \
		return VK_FORMAT_##vk

	switch (format)
	{
	fmt(RGB_ETC2_UNORM_BLOCK8, ETC2_R8G8B8_UNORM_BLOCK);
	fmt(RGBA_ETC2_UNORM_BLOCK8, ETC2_R8G8B8A1_UNORM_BLOCK);
	fmt(RGBA_ETC2_UNORM_BLOCK16, ETC2_R8G8B8A8_UNORM_BLOCK);
	fmt(RGB_ETC2_SRGB_BLOCK8, ETC2_R8G8B8_SRGB_BLOCK);
	fmt(RGBA_ETC2_SRGB_BLOCK8, ETC2_R8G8B8A1_SRGB_BLOCK);
	fmt(RGBA_ETC2_SRGB_BLOCK16, ETC2_R8G8B8A8_SRGB_BLOCK);
	fmt(R_EAC_SNORM_BLOCK8, EAC_R11_SNORM_BLOCK);
	fmt(R_EAC_UNORM_BLOCK8, EAC_R11_UNORM_BLOCK);
	fmt(RG_EAC_SNORM_BLOCK16, EAC_R11G11_SNORM_BLOCK);
	fmt(RG_EAC_UNORM_BLOCK16, EAC_R11G11_UNORM_BLOCK);

	fmt(RGB_DXT1_UNORM_BLOCK8, BC1_RGB_UNORM_BLOCK);
	fmt(RGB_DXT1_SRGB_BLOCK8, BC1_RGB_SRGB_BLOCK);
	fmt(RGBA_DXT1_UNORM_BLOCK8, BC1_RGBA_UNORM_BLOCK);
	fmt(RGBA_DXT1_SRGB_BLOCK8, BC1_RGBA_SRGB_BLOCK);
	fmt(RGBA_DXT3_UNORM_BLOCK16, BC2_UNORM_BLOCK);
	fmt(RGBA_DXT3_SRGB_BLOCK16, BC2_SRGB_BLOCK);
	fmt(RGBA_DXT5_UNORM_BLOCK16, BC3_UNORM_BLOCK);
	fmt(RGBA_DXT5_SRGB_BLOCK16, BC3_SRGB_BLOCK);

	// ASTC
	fmt(RGBA_ASTC_4X4_SRGB_BLOCK16, ASTC_4x4_SRGB_BLOCK);
	fmt(RGBA_ASTC_5X4_SRGB_BLOCK16, ASTC_5x4_SRGB_BLOCK);
	fmt(RGBA_ASTC_5X5_SRGB_BLOCK16, ASTC_5x5_SRGB_BLOCK);
	fmt(RGBA_ASTC_6X5_SRGB_BLOCK16, ASTC_6x5_SRGB_BLOCK);
	fmt(RGBA_ASTC_6X6_SRGB_BLOCK16, ASTC_6x6_SRGB_BLOCK);
	fmt(RGBA_ASTC_8X5_SRGB_BLOCK16, ASTC_8x5_SRGB_BLOCK);
	fmt(RGBA_ASTC_8X6_SRGB_BLOCK16, ASTC_8x6_SRGB_BLOCK);
	fmt(RGBA_ASTC_8X8_SRGB_BLOCK16, ASTC_8x8_SRGB_BLOCK);
	fmt(RGBA_ASTC_10X5_SRGB_BLOCK16, ASTC_10x5_SRGB_BLOCK);
	fmt(RGBA_ASTC_10X6_SRGB_BLOCK16, ASTC_10x6_SRGB_BLOCK);
	fmt(RGBA_ASTC_10X8_SRGB_BLOCK16, ASTC_10x8_SRGB_BLOCK);
	fmt(RGBA_ASTC_10X10_SRGB_BLOCK16, ASTC_10x10_SRGB_BLOCK);
	fmt(RGBA_ASTC_12X10_SRGB_BLOCK16, ASTC_12x10_SRGB_BLOCK);
	fmt(RGBA_ASTC_12X12_SRGB_BLOCK16, ASTC_12x12_SRGB_BLOCK);
	fmt(RGBA_ASTC_4X4_UNORM_BLOCK16, ASTC_4x4_UNORM_BLOCK);
	fmt(RGBA_ASTC_5X4_UNORM_BLOCK16, ASTC_5x4_UNORM_BLOCK);
	fmt(RGBA_ASTC_5X5_UNORM_BLOCK16, ASTC_5x5_UNORM_BLOCK);
	fmt(RGBA_ASTC_6X5_UNORM_BLOCK16, ASTC_6x5_UNORM_BLOCK);
	fmt(RGBA_ASTC_6X6_UNORM_BLOCK16, ASTC_6x6_UNORM_BLOCK);
	fmt(RGBA_ASTC_8X5_UNORM_BLOCK16, ASTC_8x5_UNORM_BLOCK);
	fmt(RGBA_ASTC_8X6_UNORM_BLOCK16, ASTC_8x6_UNORM_BLOCK);
	fmt(RGBA_ASTC_8X8_UNORM_BLOCK16, ASTC_8x8_UNORM_BLOCK);
	fmt(RGBA_ASTC_10X5_UNORM_BLOCK16, ASTC_10x5_UNORM_BLOCK);
	fmt(RGBA_ASTC_10X6_UNORM_BLOCK16, ASTC_10x6_UNORM_BLOCK);
	fmt(RGBA_ASTC_10X8_UNORM_BLOCK16, ASTC_10x8_UNORM_BLOCK);
	fmt(RGBA_ASTC_10X10_UNORM_BLOCK16, ASTC_10x10_UNORM_BLOCK);
	fmt(RGBA_ASTC_12X10_UNORM_BLOCK16, ASTC_12x10_UNORM_BLOCK);
	fmt(RGBA_ASTC_12X12_UNORM_BLOCK16, ASTC_12x12_UNORM_BLOCK);

	fmt(RGBA8_UNORM_PACK8, R8G8B8A8_UNORM);

	default:
		return VK_FORMAT_UNDEFINED;
	}

#undef fmt
}

void Texture::update_gli(const void *data, size_t size)
{
	gli::texture tex = gli::load(static_cast<const char *>(data), size);
	if (tex.empty())
	{
		LOGE("Texture is empty.");
		return;
	}

	ImageCreateInfo info = {};
	info.domain = ImageDomain::Physical;
	info.layers = tex.layers();
	info.levels = tex.levels();
	info.width = tex.extent(0).x;
	info.height = tex.extent(0).y;
	info.depth = tex.extent(0).z;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	info.initial_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	info.format = gli_format_to_vk(tex.format());

	if (!device->format_is_supported(info.format, VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
	{
		LOGE("Format is not supported!\n");
		return;
	}

	switch (tex.target())
	{
	case gli::target::TARGET_1D_ARRAY:
		info.misc |= IMAGE_MISC_FORCE_ARRAY_BIT;
		info.type = VK_IMAGE_TYPE_1D;
		info.height = 1;
		info.depth = 1;
		break;

	case gli::target::TARGET_1D:
		info.type = VK_IMAGE_TYPE_1D;
		info.height = 1;
		info.depth = 1;
		break;

	case gli::target::TARGET_2D_ARRAY:
		info.misc |= IMAGE_MISC_FORCE_ARRAY_BIT;
		info.depth = 1;
		info.type = VK_IMAGE_TYPE_2D;
		break;

	case gli::target::TARGET_2D:
		info.depth = 1;
		info.type = VK_IMAGE_TYPE_2D;
		break;

	case gli::target::TARGET_CUBE_ARRAY:
		info.misc |= IMAGE_MISC_FORCE_ARRAY_BIT;
		info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		info.depth = 1;
		info.layers *= tex.faces();
		info.type = VK_IMAGE_TYPE_2D;
		break;

	case gli::target::TARGET_CUBE:
		info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		info.depth = 1;
		info.layers *= tex.faces();
		info.type = VK_IMAGE_TYPE_2D;
		break;

	case gli::target::TARGET_3D:
		info.type = VK_IMAGE_TYPE_3D;
		break;

	default:
		LOGE("Unknown target type.\n");
		return;
	}

	ImageInitialData initial[32] = {};

	for (unsigned i = 0; i < info.levels; i++)
	{
		auto *mip = tex.data(0, 0, i);
		initial[i].data = mip;
	}

	handle = device->create_image(info, initial);
}

void Texture::update_png(const void *data, size_t size)
{
	int width, height;
	int components;
	auto *buffer = stbi_load_from_memory(static_cast<const stbi_uc *>(data), size, &width, &height, &components, 4);

	if (!buffer)
		throw runtime_error("stbi_load_from_memory failed.");

	handle.reset();
	auto desc = ImageCreateInfo::immutable_2d_image(unsigned(width), unsigned(height),
	                                                format != VK_FORMAT_UNDEFINED ? format : VK_FORMAT_R8G8B8A8_SRGB,
	                                                true);

	ImageInitialData initial = {};
	initial.data = buffer;
	handle = device->create_image(desc, &initial);
	stbi_image_free(buffer);
}

void Texture::load()
{
	if (!handle)
		init();
}

void Texture::unload()
{
	deinit();
	handle.reset();
}

TextureManager::TextureManager(Device *device)
	: device(device)
{
}

Texture *TextureManager::request_texture(const std::string &path)
{
	auto itr = textures.find(path);
	if (itr == end(textures))
	{
		unique_ptr<Texture> texture(new Texture(device, path));
		auto *ret = texture.get();
		textures[path] = move(texture);
		return ret;
	}
	else
		return itr->second.get();
}

}