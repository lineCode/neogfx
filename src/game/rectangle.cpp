// rectangle.cpp
/*
  neogfx C++ GUI Library
  Copyright(C) 2016 Leigh Johnston
  
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <neogfx/neogfx.hpp>
#include <neogfx/game/rectangle.hpp>

namespace neogfx
{
	rectangle::rectangle(i_shape_container& aContainer, const vec3& aPosition, const size& aSize) : 
		shape(aContainer)
	{
		set_position_3D(aPosition);
		set_size(aSize);
	}

	rectangle::rectangle(i_shape_container& aContainer, const vec3& aPosition, const size& aSize, const colour& aColour) :
		shape(aContainer, aColour)
	{
		set_position_3D(aPosition);
		set_size(aSize);
	}

	rectangle::rectangle(i_shape_container& aContainer, const vec3& aPosition, const size& aSize, const i_texture& aTexture, const optional_rect& aTextureRect) :
		shape(aContainer, aTexture, aTextureRect)
	{
		set_position_3D(aPosition);
		set_size(aSize);
	}

	rectangle::rectangle(i_shape_container& aContainer, const vec3& aPosition, const size& aSize, const i_image& aImage, const optional_rect& aTextureRect) :
		shape(aContainer, aImage, aTextureRect)
	{
		set_position_3D(aPosition);
		set_size(aSize);
	}

	rectangle::rectangle(const rectangle& aOther) :
		shape(aOther)
	{
	}

	std::size_t rectangle::vertex_count(bool aIncludeCentre) const
	{
		return aIncludeCentre ? 5 : 4;
	}

	vec3_list rectangle::vertices(bool aIncludeCentre) const
	{
		vec3_list result = shape::vertices(aIncludeCentre);
		auto r = bounding_box();
		result.push_back(r.top_left().to_vector3());
		result.push_back(r.top_right().to_vector3());
		result.push_back(r.bottom_right().to_vector3());
		result.push_back(r.bottom_left().to_vector3());
		return result;
	}
}