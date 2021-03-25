
#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include <fmod/fmod.hpp>

#include "be/application.hpp"

namespace be
{
	namespace mem
	{
		namespace fmod
		{
			class FmodException : public std::runtime_error
			{
			private:
				FMOD_RESULT m_result;
			public:
				FmodException(std::string const& msg, FMOD_RESULT const result)
					: std::runtime_error(msg + " (FMOD_RESULT: " + std::to_string(result) + ')')
					, m_result(result)
				{}
				FMOD_RESULT result() const { return m_result; }
			};

			inline void require_ok(FMOD_RESULT const result, std::string const& msg)
			{
				if (FMOD_OK != result) {
					throw FmodException(msg, result);
				}
			}

			template<class FmodType>
			struct FmodDeleter {
				using pointer = FmodType*;
				void operator()(pointer p)
				{
					try {
						require_ok(p->release(), "[be::mem::fmod] failed to release memory");
					}
					catch (FmodException const&) {
						Application::logException();
					}
				}
			};

			using SystemDeleter = FmodDeleter<FMOD::System>;
			using System = std::unique_ptr<FMOD::System, SystemDeleter>;
			inline System System_Create()
			{
				FMOD::System* p;
				require_ok(FMOD::System_Create(&p), "[be::mem::fmod] System_Create failed");
				return be::mem::fmod::System(p);
			}
			inline System System_Create_init(int maxchannels, FMOD_INITFLAGS flags, void* extradriverdata)
			{
				System system = be::mem::fmod::System_Create();
				require_ok(system->init(maxchannels, flags, extradriverdata), "[be::mem::fmod] System init failed");
				return system;
			}

			using SoundDeleter = FmodDeleter<FMOD::Sound>;
			using Sound = std::unique_ptr<FMOD::Sound, SoundDeleter>;
			inline Sound createSound(FMOD::System& system, const char* name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo)
			{
				FMOD::Sound* p;
				require_ok(system.createSound(name_or_data, mode, exinfo, &p), "[be::mem::fmod] System createSound failed");
				return be::mem::fmod::Sound(p);
			}
		}
	}
}
