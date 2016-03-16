// grid_layout.cpp
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
#include <map>
#include <neolib/bresenham_counter.hpp>
#include "grid_layout.hpp"
#include "i_widget.hpp"
#include "spacer.hpp"

namespace neogfx
{
	grid_layout::grid_layout(i_widget& aParent) :
		layout(aParent), iRowLayout(*this)
	{
		iRowLayout.set_margins(neogfx::margins{});
		iRowLayout.set_spacing(spacing());
	}

	grid_layout::grid_layout(i_layout& aParent) :
		layout(aParent), iRowLayout(*this)
	{
		iRowLayout.set_margins(neogfx::margins{});
		iRowLayout.set_spacing(spacing());
	}

	grid_layout::grid_layout(cell_coordinate aRows, cell_coordinate aColumns, i_widget& aParent) :
		layout(aParent), iRowLayout(*this), iDimensions{aColumns, aRows}
	{
		iRowLayout.set_margins(neogfx::margins{});
		iRowLayout.set_spacing(spacing());
	}

	grid_layout::grid_layout(cell_coordinate aRows, cell_coordinate aColumns, i_layout& aParent) :
		layout(aParent), iRowLayout(*this), iDimensions(aColumns, aRows)
	{
		iRowLayout.set_margins(neogfx::margins{});
		iRowLayout.set_spacing(spacing());
	}

	grid_layout::cell_coordinate grid_layout::rows() const
	{
		return iDimensions.cy;
	}

	grid_layout::cell_coordinate grid_layout::columns() const
	{
		return iDimensions.cx;
	}

	grid_layout::cell_coordinates grid_layout::dimensions() const
	{
		return iDimensions;
	}

	void grid_layout::set_dimensions(cell_coordinate aRows, cell_coordinate aColumns)
	{
		iDimensions = cell_dimensions{aColumns, aRows};
	}

	void grid_layout::add_item(i_widget& aWidget)
	{
		add_item(iCursor.y, iCursor.x, aWidget);
		increment_cursor();
	}

	void grid_layout::add_item(std::shared_ptr<i_widget> aWidget)
	{
		add_item(iCursor.y, iCursor.x, aWidget);
		increment_cursor();
	}

	void grid_layout::add_item(i_layout& aLayout)
	{
		add_item(iCursor.y, iCursor.x, aLayout);
		increment_cursor();
	}

	void grid_layout::add_item(std::shared_ptr<i_layout> aLayout)
	{
		add_item(iCursor.y, iCursor.x, aLayout);
		increment_cursor();
	}

	void grid_layout::add_item(cell_coordinate aRow, cell_coordinate aColumn, i_widget& aWidget)
	{
		if (aWidget.has_layout() && &aWidget.layout() == this)
			throw widget_already_added();
		if (iCells.find(cell_coordinates{aColumn, aRow}) != iCells.end())
			throw cell_occupied();
		iCells[cell_coordinates{aColumn, aRow}] = items().insert(items().end(), item(aWidget));
		iDimensions.cy = std::max(iDimensions.cy, aRow + 1);
		iDimensions.cx = std::max(iDimensions.cx, aColumn + 1);
		if (owner() != 0)
			items().back().set_owner(owner());
		row_layout(aRow).replace_item(aColumn, aWidget);
	}

	void grid_layout::add_item(cell_coordinate aRow, cell_coordinate aColumn, std::shared_ptr<i_widget> aWidget)
	{
		if (aWidget->has_layout() && &aWidget->layout() == this)
			throw widget_already_added();
		if (iCells.find(cell_coordinates{aColumn, aRow}) != iCells.end())
			throw cell_occupied();
		iCells[cell_coordinates{aColumn, aRow}] = items().insert(items().end(), item(aWidget));
		iDimensions.cy = std::max(iDimensions.cy, aRow + 1);
		iDimensions.cx = std::max(iDimensions.cx, aColumn + 1);
		if (owner() != 0)
			items().back().set_owner(owner());
		row_layout(aRow).replace_item(aColumn, aWidget);
	}

	void grid_layout::add_item(cell_coordinate aRow, cell_coordinate aColumn, i_layout& aLayout)
	{
		if (&aLayout == &iRowLayout)
		{
			if (owner() != 0)
				aLayout.set_owner(owner());
			return;
		}
		if (iCells.find(cell_coordinates{aColumn, aRow}) != iCells.end())
			throw cell_occupied();
		iCells[cell_coordinates{aColumn, aRow}] = items().insert(items().end(), item(aLayout));
		iDimensions.cy = std::max(iDimensions.cy, aRow + 1);
		iDimensions.cx = std::max(iDimensions.cx, aColumn + 1);
		if (owner() != 0)
			items().back().set_owner(owner());
		row_layout(aRow).replace_item(aColumn, aLayout);
	}

	void grid_layout::add_item(cell_coordinate aRow, cell_coordinate aColumn, std::shared_ptr<i_layout> aLayout)
	{
		if (iCells.find(cell_coordinates{aColumn, aRow}) != iCells.end())
			throw cell_occupied();
		iCells[cell_coordinates{aColumn, aRow}] = items().insert(items().end(), item(aLayout));
		iDimensions.cy = std::max(iDimensions.cy, aRow + 1);
		iDimensions.cx = std::max(iDimensions.cx, aColumn + 1);
		if (owner() != 0)
			items().back().set_owner(owner());
		row_layout(aRow).replace_item(aColumn, aLayout);
	}

	void grid_layout::add_item(cell_coordinate aRow, cell_coordinate aColumn, i_spacer& aSpacer)
	{
		if (iCells.find(cell_coordinates{ aColumn, aRow }) != iCells.end())
			throw cell_occupied();
		iCells[cell_coordinates{ aColumn, aRow }] = items().insert(items().end(), item(aSpacer));
		iDimensions.cy = std::max(iDimensions.cy, aRow + 1);
		iDimensions.cx = std::max(iDimensions.cx, aColumn + 1);
		if (owner() != 0)
			items().back().set_owner(owner());
		aSpacer.set_parent(*this);
		row_layout(aRow).replace_item(aColumn, aSpacer);
	}

	void grid_layout::add_item(cell_coordinate aRow, cell_coordinate aColumn, std::shared_ptr<i_spacer> aSpacer)
	{
		if (iCells.find(cell_coordinates{ aColumn, aRow }) != iCells.end())
			throw cell_occupied();
		iCells[cell_coordinates{ aColumn, aRow }] = items().insert(items().end(), item(aSpacer));
		iDimensions.cy = std::max(iDimensions.cy, aRow + 1);
		iDimensions.cx = std::max(iDimensions.cx, aColumn + 1);
		if (owner() != 0)
			items().back().set_owner(owner());
		aSpacer->set_parent(*this);
		row_layout(aRow).replace_item(aColumn, aSpacer);
	}

	i_spacer& grid_layout::add_spacer()
	{
		auto s = std::make_shared<spacer>(static_cast<i_spacer::expansion_policy_e>(i_spacer::ExpandHorizontally | i_spacer::ExpandVertically));
		add_item(iCursor.y, iCursor.x, s);
		increment_cursor();
		return *s;
	}

	i_spacer& grid_layout::add_spacer(uint32_t aPosition)
	{
		auto s = std::make_shared<spacer>(static_cast<i_spacer::expansion_policy_e>(i_spacer::ExpandHorizontally | i_spacer::ExpandVertically));
		cell_coordinates oldCursor = iCursor;
		iCursor.y = 0;
		iCursor.x = 0;
		while (aPosition)
			increment_cursor();
		add_item(iCursor.y, iCursor.x, s);
		iCursor = oldCursor;
		return *s;
	}

	i_spacer& grid_layout::add_spacer(cell_coordinate aRow, cell_coordinate aColumn)
	{
		if (iCells.find(cell_coordinates{aColumn, aRow}) != iCells.end())
			throw cell_occupied();
		auto s = std::make_shared<spacer>(static_cast<i_spacer::expansion_policy_e>(i_spacer::ExpandHorizontally | i_spacer::ExpandVertically));
		add_item(aRow, aColumn, s);
		return *s;
	}

	void grid_layout::remove_item(std::size_t aIndex)
	{
		auto itemIter = std::next(items().begin(), aIndex);
		for (cell_list::reverse_iterator i = iCells.rbegin(); i != iCells.rend(); ++i)
			if (i->second == itemIter)
			{
				row_layout(i->first.y).remove_item(i->first.x);
				iCells.erase(i.base());
				break;
			}
		iDimensions = cell_dimensions{};
		for (const auto& cell : iCells)
		{
			iDimensions.cy = std::max(iDimensions.cy, cell.first.y);
			iDimensions.cx = std::max(iDimensions.cx, cell.first.x);
		}
		iCursor = cell_coordinates{};
		layout::remove_item(aIndex);
	}

	void grid_layout::remove_items()
	{
		layout::remove_items();
		iRowLayout.remove_items();
		iRows.clear();
		iCells.clear();
		iDimensions = cell_dimensions{};
		iCursor = cell_coordinates{};
	}

	i_widget& grid_layout::get_widget(cell_coordinate aRow, cell_coordinate aColumn)
	{
		if (iCells.find(cell_coordinates{aColumn, aRow}) == iCells.end())
			throw cell_unoccupied();
		if (iCells[cell_coordinates{aColumn, aRow}]->get().is<item::widget_pointer>())
			return *static_variant_cast<item::widget_pointer&>(iCells[cell_coordinates{aColumn, aRow}]->get());
		else
			throw wrong_item_type();
	}

	i_layout& grid_layout::get_layout(cell_coordinate aRow, cell_coordinate aColumn)
	{
		if (iCells.find(cell_coordinates{aColumn, aRow}) == iCells.end())
			throw cell_unoccupied();
		if (iCells[cell_coordinates{aColumn, aRow}]->get().is<item::layout_pointer>())
			return *static_variant_cast<item::layout_pointer&>(iCells[cell_coordinates{aColumn, aRow}]->get());
		else
			throw wrong_item_type();
	}

	size grid_layout::minimum_size() const
	{
		if (items_visible() == 0)
			return size{};
		size result;
		uint32_t visibleColumns = visible_columns();
		uint32_t visibleRows = visible_rows();
		for (cell_coordinate row = 0; row < rows(); ++row)
		{
			if (!is_row_visible(row))
				continue;
			result.cy += row_minimum_size(row);
		}
		for (cell_coordinate column = 0; column < columns(); ++column)
		{
			if (!is_column_visible(column))
				continue;
			result.cx += column_minimum_size(column);
		}
		result.cx += (margins().left + margins().right);
		result.cy += (margins().top + margins().bottom);
		if (result.cx != std::numeric_limits<size::dimension_type>::max() && visibleColumns> 0)
			result.cx += (spacing().cx * (visibleColumns - 1));
		if (result.cy != std::numeric_limits<size::dimension_type>::max() && visibleRows > 0)
			result.cy += (spacing().cy * (visibleRows - 1));
		result.cx = std::max(result.cx, layout::minimum_size().cx);
		result.cy = std::max(result.cy, layout::minimum_size().cy);
		return result;
	}

	size grid_layout::maximum_size() const
	{
		if (items_visible(static_cast<item_type_e>(ItemTypeWidget | ItemTypeLayout | ItemTypeSpacer)) == 0)
			return size{};
		size result;
		for (cell_coordinate row = 0; row < visible_rows(); ++row)
		{
			size::dimension_type rowMaxSize = row_maximum_size(row);
			if (rowMaxSize == std::numeric_limits<size::dimension_type>::max())
				result.cy = rowMaxSize;
			else if (result.cy != std::numeric_limits<size::dimension_type>::max())
				result.cy += rowMaxSize;
		}
		for (cell_coordinate column = 0; column < visible_columns(); ++column)
		{
			size::dimension_type columnMaxSize = column_maximum_size(column);
			if (columnMaxSize == std::numeric_limits<size::dimension_type>::max())
				result.cx = columnMaxSize;
			else if (result.cx != std::numeric_limits<size::dimension_type>::max())
				result.cx += columnMaxSize;
		}
		if (result.cx != std::numeric_limits<size::dimension_type>::max())
			result.cx += (margins().left + margins().right);
		if (result.cy != std::numeric_limits<size::dimension_type>::max())
			result.cy += (margins().top + margins().bottom);
		if (result.cx != std::numeric_limits<size::dimension_type>::max() && visible_columns() > 0)
			result.cx += (spacing().cx * (visible_columns() - 1));
		if (result.cy != std::numeric_limits<size::dimension_type>::max() && visible_rows() > 0)
			result.cy += (spacing().cy * (visible_rows() - 1));
		if (result.cx != std::numeric_limits<size::dimension_type>::max())
			result.cx = std::min(result.cx, layout::maximum_size().cx);
		if (result.cy != std::numeric_limits<size::dimension_type>::max())
			result.cy = std::min(result.cy, layout::maximum_size().cy);
		return result;
	}

	void grid_layout::set_spacing(const size& aSpacing)
	{
		layout::set_spacing(aSpacing);
		iRowLayout.set_spacing(aSpacing);
		for (auto& r : iRows)
			r->set_spacing(aSpacing);
	}

	void grid_layout::layout_items(const point& aPosition, const size& aSize)
	{
		if (!enabled())
			return;
		owner()->layout_items_started();
		for (auto& r : iRows)
			while (r->item_count() < iDimensions.cx)
				r->add_spacer();
		iRowLayout.layout_items(aPosition, aSize);
		owner()->layout_items_completed();
	}

	uint32_t grid_layout::visible_rows() const
	{
		uint32_t result = 0;
		for (cell_coordinate row = 0; row < iDimensions.cy; ++row)
			for (cell_coordinate col = 0; col < iDimensions.cx; ++col)
			{
				auto i = iCells.find(cell_coordinates{col, row});
				if (i != iCells.end() && i->second->get().is<item::widget_pointer>() && i->second->visible() && i->second->minimum_size().cy != 0.0)
				{
					++result;
					break;
				}
			}
		return result;
	}

	bool grid_layout::is_row_visible(uint32_t aRow) const
	{
		for (cell_coordinate col = 0; col < iDimensions.cx; ++col)
		{
			auto i = iCells.find(cell_coordinates{col, aRow});
			if (i != iCells.end() && i->second->get().is<item::widget_pointer>() && i->second->visible() && i->second->minimum_size().cy != 0.0)
				return true;
		}
		return false;
	}

	uint32_t grid_layout::visible_columns() const
	{
		uint32_t result = 0;
		for (cell_coordinate col = 0; col < iDimensions.cx; ++col)
			for (cell_coordinate row = 0; row < iDimensions.cy; ++row)
			{
				auto i = iCells.find(cell_coordinates{col, row});
				if (i != iCells.end() && i->second->get().is<item::widget_pointer>() && i->second->visible() && i->second->minimum_size().cx != 0.0)
				{
					++result;
					break;
				}
			}
		return result;
	}

	bool grid_layout::is_column_visible(uint32_t aColumn) const
	{
		for (cell_coordinate row = 0; row < iDimensions.cy; ++row)
		{
			auto i = iCells.find(cell_coordinates{aColumn, row});
			if (i != iCells.end() && i->second->get().is<item::widget_pointer>() && i->second->visible() && i->second->minimum_size().cx != 0.0)
				return true;
		}
		return false;
	}

	size::dimension_type grid_layout::row_minimum_size(cell_coordinate aRow) const
	{
		size::dimension_type result {};
		for (const auto& item : iCells)
			if (item.first.y == aRow)
				result = std::max(result, item.second->minimum_size().cy);
		return result;
	}

	size::dimension_type grid_layout::column_minimum_size(cell_coordinate aColumn) const
	{
		size::dimension_type result {};
		for (const auto& item : iCells)
			if (item.first.x == aColumn)
				result = std::max(result, item.second->minimum_size().cx);
		return result;
	}

	size::dimension_type grid_layout::row_maximum_size(cell_coordinate aRow) const
	{
		size::dimension_type result {};
		for (const auto& item : iCells)
			if (item.first.y == aRow)
				result = std::max(result, item.second->maximum_size().cy);
		return result;
	}

	size::dimension_type grid_layout::column_maximum_size(cell_coordinate aColumn) const
	{
		size::dimension_type result {};
		for (const auto& item : iCells)
			if (item.first.x == aColumn)
				result = std::max(result, item.second->maximum_size().cx);
		return result;
	}

	void grid_layout::increment_cursor()
	{
		++iCursor.x;
		if (iCursor.x >= columns())
		{
			++iCursor.y;
			iCursor.x = 0;
		}
	}

	horizontal_layout& grid_layout::row_layout(cell_coordinate aRow)
	{
		while (aRow >= iRows.size())
		{
			iRows.push_back(std::make_shared<horizontal_layout>(iRowLayout));
			iRows.back()->set_margins(neogfx::margins{});
			iRows.back()->set_spacing(spacing());
		}
		return *iRows[aRow];
	}
}