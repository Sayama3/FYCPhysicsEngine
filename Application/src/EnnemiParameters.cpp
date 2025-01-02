//
// Created by ianpo on 02/01/2025.
//

#include "EnnemiParameters.hpp"


namespace FYC::Application {

	std::vector<char> EnnemiSerializer::ToBinary(EnnemiParameters parameters, const std::vector<FYC::World::ID>& ennemiIds)
	{
		std::vector<char> binary(sizeof(EnnemiParameters) + sizeof(FYC::World::ID) * ennemiIds.size());
		*reinterpret_cast<EnnemiParameters*>(binary.data()) = parameters;
		FYC::World::ID* ids = reinterpret_cast<FYC::World::ID*>(binary.data() + sizeof(EnnemiParameters));
		for (uint64_t i = 0; i < ennemiIds.size(); ++i) {
			ids[i] = ennemiIds[i];
		}
		return std::move(binary);
	}

	std::pair<EnnemiParameters, std::vector<FYC::World::ID>> EnnemiSerializer::FromBinary(const std::vector<char>& binaries)
	{
		std::pair<EnnemiParameters, std::vector<FYC::World::ID>> result;
		result.first = *reinterpret_cast<const EnnemiParameters*>(binaries.data());

		uint64_t count = (binaries.size() - sizeof(EnnemiParameters)) / sizeof(FYC::World::ID);
		result.second.resize(count);
		const FYC::World::ID* ids = reinterpret_cast<const FYC::World::ID*>(binaries.data() + sizeof(EnnemiParameters));
		for (uint64_t i = 0; i < count; ++i) {
			result.second[i] = ids[i];
		}
		return std::move(result);
	}
} // Application
// FYC