#pragma once

namespace MMMEngine
{
	struct Resolution
	{
		int width;
		int height;

		bool operator==(const Resolution& rhs) const
		{
			return width == rhs.width && height == rhs.height;
		}
	};
}