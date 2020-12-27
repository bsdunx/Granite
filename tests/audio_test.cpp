#include "application/global_managers.hpp"
#include "audio/audio_mixer.hpp"
#include "audio/vorbis_stream.hpp"
#include "util/logging.hpp"
#include "util/timer.hpp"

#ifdef _WIN32
#include "filesystem/windows/os_filesystem.hpp"
#else
#include "filesystem/linux/os_filesystem.hpp"
#endif

#include <cstdlib>
#include <chrono>
#include <thread>

using namespace Granite;
using namespace Granite::Audio;

int main()
{
	Global::init();
	Global::filesystem()->register_protocol("assets", std::make_unique<OSFilesystem>(ASSET_DIRECTORY));
 
	auto *stream = create_vorbis_stream("assets://test.ogg");
	if (stream == nullptr)
	{
		return EXIT_FAILURE;
	}

	Global::audio_backend()->start();

	StreamID id = Global::audio_mixer()->add_mixer_stream(stream);

	for (unsigned i = 0; i < 10000; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		LOGI("Play time: %.3f s\n", Global::audio_mixer()->get_play_cursor(id));
	}

	Global::audio_backend()->stop();
	std::this_thread::sleep_for(std::chrono::seconds(3));
	Global::audio_backend()->start();
	std::this_thread::sleep_for(std::chrono::seconds(100));

	return EXIT_SUCCESS;
}
