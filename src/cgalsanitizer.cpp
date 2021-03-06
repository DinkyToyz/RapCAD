/*
 *   RapCAD - Rapid prototyping CAD IDE (www.rapcad.org)
 *   Copyright (C) 2010-2020 Giles Bathgate
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "cgalsanitizer.h"


CGALSanitizer::CGALSanitizer(CGAL::Polyhedron3& p) :
	polyhedron(p)
{}

void CGALSanitizer::sanitize()
{
	fixZeros();
	fixZeroEdges();
	fixZeroTriangles();
	fixIsolated();
}

void CGALSanitizer::erase(CGAL::VertexHandle h)
{
	handle=h;
	polyhedron.delegate(*this);
}

void CGALSanitizer::fixIsolated()
{
	using VertexIterator = CGAL::Polyhedron3::Vertex_iterator;
	for(VertexIterator vi=polyhedron.vertices_begin(); vi!=polyhedron.vertices_end(); ++vi) {
		if(vi->halfedge() == nullptr)
			erase(vi);
	}
}

bool CGALSanitizer::fixZero()
{
	using FacetIterator = CGAL::Polyhedron3::Facet_iterator;
	for(FacetIterator f=polyhedron.facets_begin(); f!=polyhedron.facets_end(); ++f) {
		if(f->facet_degree()<3) {
			polyhedron.erase_facet(f->halfedge());
			return true;
		}
	}
	return false;
}

void CGALSanitizer::fixZeros()
{
	while(fixZero());
}

void CGALSanitizer::fixZeroTriangles()
{
	using FacetIterator = CGAL::Polyhedron3::Facet_iterator;
	for(FacetIterator f=polyhedron.facets_begin(); f!=polyhedron.facets_end(); ++f) {
		if(f->facet_degree()>3) continue;
		CGAL::HalfedgeHandle h1=f->halfedge();
		CGAL::HalfedgeHandle h2=h1->next();
		CGAL::HalfedgeHandle h3=h2->next();
		const CGAL::Point3& p1=h1->vertex()->point();
		const CGAL::Point3& p2=h2->vertex()->point();
		const CGAL::Point3& p3=h3->vertex()->point();
		if(CGAL::collinear(p1,p2,p3)) {
			removeShortestEdges(h1,h2,h3);
		}
	}
}

void CGALSanitizer::fixZeroEdges()
{
	while(removeShortEdges());
}

CGAL::Scalar CGALSanitizer::getLength(CGAL::HalfedgeHandle h)
{
	return CGAL::squared_distance(h->vertex()->point(),h->opposite()->vertex()->point());
}

void CGALSanitizer::removeShortEdge(CGAL::HalfedgeHandle h)
{
	// Determine the number of edges surrounding the vertex. e.g. \|/ or |/
	auto edges=circulator_size(h->vertex_begin());
	if(edges<3) {
		polyhedron.erase_facet(h);
	} else if(edges==3) {
		polyhedron.join_facet(h->next());
		polyhedron.join_vertex(h);
	} else {
		polyhedron.join_facet(h->next());
		polyhedron.join_facet(h->opposite()->next());
		polyhedron.join_vertex(h);
	}
}

void CGALSanitizer::removeShortestEdges(CGAL::HalfedgeHandle h1, CGAL::HalfedgeHandle h2, CGAL::HalfedgeHandle h3)
{
	CGAL::Scalar l1=getLength(h1);
	CGAL::Scalar l2=getLength(h2);
	CGAL::Scalar l3=getLength(h3);

	if(l1<l2||l1<l3)
		polyhedron.join_facet(h1);
	if(l2<l1||l2<l3)
		polyhedron.join_facet(h2);
	if(l3<l1||l3<l2)
		polyhedron.join_facet(h3);
}

bool CGALSanitizer::removeShortEdges()
{
	using HalfedgeIterator = CGAL::Polyhedron3::Halfedge_iterator;
	for(HalfedgeIterator h=polyhedron.halfedges_begin(); h!=polyhedron.halfedges_end(); ++h) {
		if(getLength(h)==0.0) {
			removeShortEdge(h);
			return true;
		}
	}
	return false;
}

void CGALSanitizer::operator()(CGAL::HalfedgeDS& hds)
{
	CGAL::HalfedgeDS_decorator<CGAL::HalfedgeDS> d(hds);
	d.vertices_erase(handle);
}
