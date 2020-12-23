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

#include <cstdint>

struct VolkDeviceTable;

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T *VkCommandBuffer;

struct VkCommandPool_T;
typedef VkCommandPool_T *VkCommandPool;

struct VkDeviceMemory_T;
typedef VkDeviceMemory_T *VkDeviceMemory;

struct VkEvent_T;
typedef VkEvent_T *VkEvent;

struct VkFence_T;
typedef VkFence_T *VkFence;

struct VkImage_T;
typedef VkImage_T *VkImage;

struct VkSemaphore_T;
typedef VkSemaphore_T *VkSemaphore;

struct VkPhysicalDeviceMemoryProperties;
struct VkPhysicalDeviceSubgroupProperties;

typedef uint64_t VkDeviceSize;

typedef uint32_t VkFlags;
typedef VkFlags VkBufferUsageFlags;
typedef VkFlags VkPipelineStageFlags;

#define VK_NULL_HANDLE 0
