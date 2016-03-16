// surface_manager.hpp
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

#include "neogfx.hpp"
#include <set>
#include "i_rendering_engine.hpp"
#include "i_surface_manager.hpp"

namespace neogfx
{
	class surface_manager : public i_surface_manager
	{
	public:
		struct already_rendering_surfaces : std::logic_error { already_rendering_surfaces() : std::logic_error("neogfx::surface_manager::already_rendering_surfaces") {} };
	private:
		typedef std::set<i_surface*> surface_list;
	public:
		surface_manager(i_rendering_engine& aRenderingEngine);
	public:
		virtual void add_surface(i_surface& aSurface);
		virtual void remove_surface(i_surface& aSurface);
		virtual i_surface& surface_from_handle(void* aHandle);
		virtual std::size_t surface_count() const;
		virtual i_surface& surface(std::size_t aIndex);
		virtual bool process_events(bool& aLastWindowClosed);
		virtual void layout_surfaces();
		virtual void invalidate_surfaces();
		virtual void render_surfaces();
		virtual void display_error_message(const std::string& aTitle, const std::string& aMessage) const;
		virtual void display_error_message(const i_native_surface& aParent, const std::string& aTitle, const std::string& aMessage) const;
	private:
		i_rendering_engine& iRenderingEngine;
		surface_list iSurfaces;
		bool iRenderingSurfaces;
	};
}