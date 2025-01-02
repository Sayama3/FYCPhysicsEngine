//
// Created by ianpo on 02/01/2025.
//

#pragma once

#include "Physics/Math.hpp"
#include "Physics/World.hpp"

namespace FYC::Application {

	struct EnnemiParameters {
		Real Speed = 2;
		Real ThresholdKillEnnemi = 0.25;
	};

	class EnnemiSerializer {
	public:
		static std::vector<char> ToBinary(EnnemiParameters parameters, const std::vector<FYC::World::ID>& ennemiIds);
		static std::pair<EnnemiParameters, std::vector<FYC::World::ID>> FromBinary(const std::vector<char>& binaries);
	};

} // Application
// FYC
