// vertical_layout.cpp
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

#include "neogfx.hpp"
#include <unordered_map>
#include <unordered_set>
#include <boost/pool/pool_alloc.hpp>
#include <neolib/bresenham_counter.hpp>
#include "vertical_layout.hpp"
#include "i_widget.hpp"
#include "spacer.hpp"

namespace neogfx
{
	vertical_layout::vertical_layout(neogfx::alignment aAlignment) :
		layout(aAlignment)
	{
	}

	vertical_layout::vertical_layout(i_widget& aParent, neogfx::alignment aAlignment) :
		layout(aParent, aAlignment)
	{
	}

	vertical_layout::vertical_layout(i_layout& aParent, neogfx::alignment aAlignment) :
		layout(aParent, aAlignment)
	{
	}

	i_spacer& vertical_layout::add_spacer()
	{
		auto s = std::make_shared<vertical_spacer>();
		add_item(s);
		return *s;
	}

	i_spacer& vertical_layout::add_spacer(uint32_t aPosition)
	{
		auto s = std::make_shared<vertical_spacer>();
		add_item(aPosition, s);
		return *s;
	}

	size vertical_layout::minimum_size() const
	{
		uint32_t itemsVisible = items_visible();
		if (itemsVisible == 0)
			return size{};
		size result;
		uint32_t itemsZeroSized = 0;
		for (const auto& item : items())
		{
			if (!item.visible())
				continue;
			if (!item.get().is<item::spacer_pointer>() && (item.minimum_size().cx == 0.0 || item.minimum_size().cy == 0.0))
			{
				++itemsZeroSized;
				continue;
			}
			result.cx = std::max(result.cx, item.minimum_size().cx);
			result.cy += item.minimum_size().cy;
		}
		result.cx += (margins().left + margins().right);
		result.cy += (margins().top + margins().bottom);
		if (result.cy != std::numeric_limits<size::dimension_type>::max() && (itemsVisible - itemsZeroSized) > 1)
			result.cy += (spacing().cy * (itemsVisible - itemsZeroSized - 1));
		result.cx = std::max(result.cx, layout::minimum_size().cx);
		result.cy = std::max(result.cy, layout::minimum_size().cy);
		return result;
	}

	size vertical_layout::maximum_size() const
	{
		if (items_visible(static_cast<item_type_e>(ItemTypeWidget | ItemTypeLayout | ItemTypeSpacer)) == 0)
			return size{ std::numeric_limits<size::dimension_type>::max(), std::numeric_limits<size::dimension_type>::max() };
		uint32_t itemsVisible = items_visible();
		size result{ 0.0, std::numeric_limits<size::dimension_type>::max() };
		for (const auto& item : items())
		{
			if (!item.visible())
				continue;
			result.cx = std::max(result.cx, item.maximum_size().cx);
			auto cy = std::min(result.cy, item.maximum_size().cy);
			if (cy != std::numeric_limits<size::dimension_type>::max())
				result.cy += cy;
			else
				result.cy = std::numeric_limits<size::dimension_type>::max();
		}
		if (result.cx != std::numeric_limits<size::dimension_type>::max())
			result.cx += (margins().left + margins().right);
		if (result.cy != std::numeric_limits<size::dimension_type>::max())
			result.cy += (margins().top + margins().bottom);
		if (result.cy != std::numeric_limits<size::dimension_type>::max() && itemsVisible > 0)
			result.cy += (spacing().cy * (itemsVisible - 1 - spacer_count()));
		if (result.cx != std::numeric_limits<size::dimension_type>::max())
			result.cx = std::min(result.cx, layout::maximum_size().cx);
		if (result.cy != std::numeric_limits<size::dimension_type>::max())
			result.cy = std::min(result.cy, layout::maximum_size().cy);
		return result;
	}

	void vertical_layout::layout_items(const point& aPosition, const size& aSize)
	{
		if (!enabled())
			return;
		owner()->layout_items_started();
		layout::do_layout_items<layout::row_major<vertical_layout>>(aPosition, aSize);
		owner()->layout_items_completed();
	}
}