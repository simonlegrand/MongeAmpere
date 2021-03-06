// MongeAmpere++
// Copyright (C) 2014 Quentin Mérigot, CNRS
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef MA_QUADRATURE_HPP
#define MA_QUADRATURE_HPP

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>

namespace MA
{

// order 3 formula with 6 points
template <class K, class F>
auto
integrate_albrecht_collatz(const typename CGAL::Point_2<K> &a, 
			   const typename CGAL::Point_2<K> &b, 
			   const typename CGAL::Point_2<K> &c,
			   const F &f) -> decltype(f(a))
{
  typedef typename K::FT FT;
  const FT _1_2 = FT(1)/FT(2), _1_6 = FT(1)/FT(6), _2_3 = FT(2)/FT(3);
  const FT _1_30 = FT(1)/FT(30), _9_30 = FT(9)/FT(30);
  auto  u = b-a, v = c-a;
  auto r1 = _1_30*f(a + _1_2*u + _1_2*v);
  auto r2 = _1_30*f(a + _1_2*u);
  auto r3 = _1_30*f(a + _1_2*v);
  auto r4 = _9_30*f(a + _1_6*u + _2_3*v);
  auto r5 = _9_30*f(a + _1_6*v + _2_3*u);
  auto r6 = _9_30*f(a + _1_6*u + _1_6*v);
  return CGAL::area(a,b,c)*(r1+r2+r3+r4+r5+r6);
}

template <class K, class F>
auto
integrate_midedge(const typename CGAL::Point_2<K> &a, 
		  const typename CGAL::Point_2<K> &b, 
		  const typename CGAL::Point_2<K> &c,
		  const F &f) -> decltype(f(a))
{
  typedef typename K::FT FT;
  auto r1 = f(CGAL::midpoint(a,b));
  auto r2 = f(CGAL::midpoint(a,c));
  auto r3 = f(CGAL::midpoint(b,c));
  return CGAL::area(a,b,c)*(r1+r2+r3)/FT(3);
}

template <class K, class F>
auto
integrate_vertices(const typename CGAL::Point_2<K> &a, 
		   const typename CGAL::Point_2<K> &b, 
		   const typename CGAL::Point_2<K> &c,
		   const F &f) -> decltype(f(a))
{
  typedef typename K::FT FT;
  return CGAL::area(a,b,c)*(f(a)+f(b)+f(c))/FT(3);
}

template <class K, class F>
auto
integrate_centroid(const typename CGAL::Point_2<K> &a, 
		   const typename CGAL::Point_2<K> &b, 
		   const typename CGAL::Point_2<K> &c,
		   const F &f) -> decltype(f(a))
{
  return CGAL::area(a,b,c)*f(CGAL::centroid(a,b,c));
}

template <class K, class F>
auto
integrate_1(const typename CGAL::Segment_2<K> &p, const F &f)
  -> decltype(f(p.source()))
{
  typedef typename K::FT FT;
  return sqrt(p.squared_length()) * f(CGAL::midpoint(p.source(),
						     p.target()));
}

double r01() 
{ 
  return double(rand() / (RAND_MAX + 1.0));
}

template <class K>
typename CGAL::Point_2<K>
rand_in_triangle(const typename CGAL::Point_2<K> &a, 
		 const typename CGAL::Point_2<K> &b, 
		 const typename CGAL::Point_2<K> &c)
{
  double r1 = sqrt(r01()), r2 = r01();
  auto A = a - CGAL::ORIGIN, B = b - CGAL::ORIGIN, C = c - CGAL::ORIGIN;
  return CGAL::ORIGIN + ((1 - r1) * A +
			 (r1 * (1 - r2)) * B +
			 (r1 * r2) * C);
}

template <class K, class F>
auto
integrate_monte_carlo(const typename CGAL::Point_2<K> &a, 
		      const typename CGAL::Point_2<K> &b, 
		      const typename CGAL::Point_2<K> &c,
		      const F &f,
		      size_t N) -> decltype(f(a))
{
  typedef typename K::FT FT;
  decltype(f(a)) r;
  for (size_t i = 0; i < N; ++i)
    {
      auto p = rand_in_triangle(a,b,c);
      r += f(p);
    }
  return CGAL::area(a,b,c)*(r/N);
}

template <class K, class F>
auto
integrate_1(const typename CGAL::Polygon_2<K> &p, const F &f)
  -> decltype(f(p[0]))
{
  typedef decltype(f(p[0])) RT;
  RT r = RT();
  if (p.size() <= 2)
    return RT();
  for (size_t i = 1; i < p.size() - 1; ++i)
    r = r + MA::integrate_centroid(p[0],p[i],p[i+1],f);
  return r;
}


template <class K, class F>
auto
integrate_3(const typename CGAL::Polygon_2<K> &p, const F &f)
  -> decltype(f(p[0]))
{
  typedef decltype(f(p[0])) RT;
  RT r = RT();
  if (p.size() <= 2)
    return r;
  for (size_t i = 1; i < p.size() - 1; ++i)
    r = r + MA::integrate_albrecht_collatz(p[0],p[i],p[i+1],f);
  return r;
}


}

#endif

