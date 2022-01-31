#pragma once

namespace Gui {
	void UpdatePointers(float* originalNormalX, float* originalNormalZ, float* modifiedNormalX, float* modifiedNormalZ, bool* applyNewNormal);
	bool InitGui();
	void DestroyGui();
}