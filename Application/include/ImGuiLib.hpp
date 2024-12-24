//
// Created by Iannis on 19/12/2024.
//

#pragma once

#include <imgui.h>
#include "Physics/Math.hpp"

namespace ImGuiLib
{
#ifdef FYC_DOUBLE
	inline static constexpr ImGuiDataType_ ImGuiRealDataType = ImGuiDataType_Double;
#else
	inline static constexpr ImGuiDataType_ ImGuiRealDataType = ImGuiDataType_Float;
#endif

	inline static bool DragReal(const char* label, FYC::Real* v, float v_speed = 1.0f, FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return ImGui::DragScalar(label, ImGuiRealDataType, v, v_speed, &v_min, &v_max, format, flags);
    }
	inline static bool DragReal2(const char* label, FYC::Real v[2], float v_speed = 1.0f, FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return ImGui::DragScalarN(label, ImGuiRealDataType, v, 2, v_speed, &v_min, &v_max, format, flags);
	}
	inline static bool DragReal3(const char* label, FYC::Real v[3], float v_speed = 1.0f, FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return ImGui::DragScalarN(label, ImGuiRealDataType, v, 3, v_speed, &v_min, &v_max, format, flags);
	}
	inline static bool DragReal4(const char* label, FYC::Real v[4], float v_speed = 1.0f, FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return ImGui::DragScalarN(label, ImGuiRealDataType, v, 4, v_speed, &v_min, &v_max, format, flags);
	}

	inline static bool SliderReal(const char* label, FYC::Real* v, FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
    	return ImGui::SliderScalar(label, ImGuiRealDataType, v, &v_min, &v_max, format, flags);
	}
	inline static bool SliderReal2(const char* label, FYC::Real v[2], FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
    	return ImGui::SliderScalarN(label, ImGuiRealDataType, v, 2, &v_min, &v_max, format, flags);
	}
	inline static bool SliderReal3(const char* label, FYC::Real v[3], FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
    	return ImGui::SliderScalarN(label, ImGuiRealDataType, v, 3, &v_min, &v_max, format, flags);
	}
	inline static bool SliderReal4(const char* label, FYC::Real v[4], FYC::Real v_min = 0.0f, FYC::Real v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return ImGui::SliderScalarN(label, ImGuiRealDataType, v, 4, &v_min, &v_max, format, flags);
	}

	inline static bool InputReal(const char* label, FYC::Real* v, FYC::Real step = 0.0f, FYC::Real step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0) {
    	return ImGui::InputScalar(label, ImGuiRealDataType, (void*)v, (void*)(step > 0.0f ? &step : NULL), (void*)(step_fast > 0.0f ? &step_fast : NULL), format, flags);
	}
	inline static bool InputReal2(const char* label, FYC::Real v[2], const char* format = "%.3f", ImGuiInputTextFlags flags = 0) {
    	return ImGui::InputScalarN(label, ImGuiRealDataType, v, 2, NULL, NULL, format, flags);
	}
	inline static bool InputReal3(const char* label, FYC::Real v[3], const char* format = "%.3f", ImGuiInputTextFlags flags = 0) {
    	return ImGui::InputScalarN(label, ImGuiRealDataType, v, 3, NULL, NULL, format, flags);
	}
	inline static bool InputReal4(const char* label, FYC::Real v[4], const char* format = "%.3f", ImGuiInputTextFlags flags = 0) {
		return ImGui::InputScalarN(label, ImGuiRealDataType, v, 4, NULL, NULL, format, flags);
	}

} // ImGuiLib
