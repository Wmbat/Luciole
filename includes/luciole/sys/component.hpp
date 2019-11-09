/**
 *  Copyright (C) 2018-2019 Wmbat
 *
 *  wmbat@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  You should have received a copy of the GNU General Public License
 *  GNU General Public License for more details.
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUCIOLE_SYS_COMPONENT_HPP
#define LUCIOLE_SYS_COMPONENT_HPP

#include <type_traits>
#include <tuple>

namespace sys
{
   /*
   template<template<typename...> typename type1, typename type2>
   constexpr bool is_specialization_v = false;

   template<template<typename...> typename type, typename... args>
   constexpr bool is_specialization_v<type, type<args...>> = true;

   namespace cpt
   {  
      template<typename type>
      concept node = std::is_object_v<type>;

      template<typename type>
      concept graph = true;
   } // namespace cpt

   template<cpt::node node_t, cpt::graph... connections_t>
   struct graph   
   {
      node_t node;
      std::tuple<connections_t...> connections; 

      graph( ) = default;

      template<std::size_t index_t>
         requires (index_t < sizeof...(connections_t) && index_t >= 0)
      cpt::graph auto& get_connection( )
      {
         return std::get<index_t>(connections);
      }
   }; // struct graph

   template<typename graph, typename graph_location>
   struct graph_context
   {
      graph& instance;
      graph_location location;
   };

   template<typename type>
   concept component_graph = is_specialization_v<graph_context, type>;

   class component
   {
   public:

   private:
   }; // class component  
   */
} // namespace sys

#endif // LUCIOLE_SYS_COMPONENT_HPP
