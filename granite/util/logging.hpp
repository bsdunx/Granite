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

#include <cstdio>
#include <cstdarg>
#include <cstring>

#ifdef GRANITE_LOGGING_QUEUE
#include "application/global_managers.hpp"
#include "util/message_queue.hpp"

namespace Util
{

static inline void queued_log(const char *tag, const char *fmt, ...)
{
	auto *message_queue = ::Granite::Global::message_queue();
	if (!message_queue || !message_queue->is_uncorked())
		return;

	char message_buffer[16 * 1024];
	memcpy(message_buffer, tag, strlen(tag));
	va_list va;
	va_start(va, fmt);
	vsnprintf(message_buffer + strlen(tag), sizeof(message_buffer) - strlen(tag), fmt, va);
	va_end(va);

	size_t message_size = strlen(message_buffer) + 1;

	while (message_size >= 2 && message_buffer[message_size - 2] == '\n')
	{
		message_buffer[message_size - 2] = '\0';
		message_size--;
	}

	auto message_payload = message_queue->allocate_write_payload(message_size);
	if (message_payload)
	{
		memcpy(static_cast<char *>(message_payload.get_payload_data()), message_buffer, message_size);
		message_queue->push_written_payload(std::move(message_payload));
	}
}

}

#define QUEUED_LOGE(...) do { \
	::Util::queued_log("[ERROR]: ", __VA_ARGS__); \
} while(0)
#define QUEUED_LOGW(...) do { \
	::Util::queued_log("[WARN]: ", __VA_ARGS__); \
} while(0)
#define QUEUED_LOGI(...) do { \
	::Util::queued_log("[INFO]: ", __VA_ARGS__); \
} while(0)
#else
#define QUEUED_LOGE(...)
#define QUEUED_LOGW(...)
#define QUEUED_LOGI(...)
#endif // GRANITE_LOGGING_QUEUE

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define LOGE(...) do { \
    fprintf(stderr, "[ERROR]: " __VA_ARGS__); \
    fflush(stderr); \
    char buffer[16 * 1024]; \
    snprintf(buffer, sizeof(buffer), "[ERROR]: " __VA_ARGS__); \
    OutputDebugStringA(buffer); \
    QUEUED_LOGE(__VA_ARGS__); \
} while(false)

#define LOGW(...) do { \
    fprintf(stderr, "[WARN]: " __VA_ARGS__); \
    fflush(stderr); \
    char buffer[16 * 1024]; \
    snprintf(buffer, sizeof(buffer), "[WARN]: " __VA_ARGS__); \
    OutputDebugStringA(buffer); \
    QUEUED_LOGW(__VA_ARGS__); \
} while(false)

#define LOGI(...) do { \
    fprintf(stderr, "[INFO]: " __VA_ARGS__); \
    fflush(stderr); \
    char buffer[16 * 1024]; \
    snprintf(buffer, sizeof(buffer), "[INFO]: " __VA_ARGS__); \
    OutputDebugStringA(buffer); \
    QUEUED_LOGI(__VA_ARGS__); \
} while(false)
#else
#define LOGE(...)                                 \
	do                                            \
	{                                             \
		fprintf(stderr, "[ERROR]: " __VA_ARGS__); \
		fflush(stderr);                           \
		QUEUED_LOGE(__VA_ARGS__);                 \
	} while (false)

#define LOGW(...)                                \
	do                                           \
	{                                            \
		fprintf(stderr, "[WARN]: " __VA_ARGS__); \
		fflush(stderr);                          \
		QUEUED_LOGW(__VA_ARGS__);                \
	} while (false)

#define LOGI(...)                                \
	do                                           \
	{                                            \
		fprintf(stderr, "[INFO]: " __VA_ARGS__); \
		fflush(stderr);                          \
		QUEUED_LOGI(__VA_ARGS__);                \
	} while (false)
#endif
