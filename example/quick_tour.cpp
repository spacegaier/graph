//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, University of Notre Dame, Notre
// Dame, IN 46556.
//
// Permission to modify the code and to distribute modified code is
// granted, provided the text of this NOTICE is retained, a notice that
// the code was modified is included with the above COPYRIGHT NOTICE and
// with the COPYRIGHT NOTICE in the LICENSE file, and that the LICENSE
// file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================

#include <boost/config.hpp>
#include <iostream>                         // for std::cout
#include <utility>                         // for std::pair
#include <algorithm>                         // for std::for_each
#include <boost/utility.hpp>             // for boost::tie
#include <boost/graph/graph_traits.hpp>  // for boost::graph_traits
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

using namespace boost;

template <class Graph> struct exercise_vertex {
  exercise_vertex(Graph& g_) : g(g_) { }
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  void operator()(const Vertex& v) const
  {
    using namespace boost;
    typename property_map<Graph, vertex_index_t>::type 
      vertex_id = get(vertex_index, g);
    std::cout << "vertex: " << get(vertex_id, v) << std::endl;

    // Write out the outgoing edges
    std::cout << "\tout-edges: ";
    typename graph_traits<Graph>::out_edge_iterator out_i, out_end;
    typename graph_traits<Graph>::edge_descriptor e;
    for (tie(out_i, out_end) = out_edges(v, g); 
         out_i != out_end; ++out_i)
    {
      e = *out_i;
      Vertex src = source(e, g), targ = target(e, g);
      std::cout << "(" << get(vertex_id, src)
                << "," << get(vertex_id, targ) << ") ";
    }
    std::cout << std::endl;

    // Write out the incoming edges    
    std::cout << "\tin-edges: ";
    typename graph_traits<Graph>::in_edge_iterator in_i, in_end;
    for (tie(in_i, in_end) = in_edges(v, g); in_i != in_end; ++in_i)
    {
      e = *in_i;
      Vertex src = source(e, g), targ = target(e, g);
      std::cout << "(" << get(vertex_id, src)
                << "," << get(vertex_id, targ) << ") ";
    }
    std::cout << std::endl;

    // Write out all adjacent vertices    
    std::cout << "\tadjacent vertices: ";
    typename graph_traits<Graph>::adjacency_iterator ai, ai_end;
    for (tie(ai,ai_end) = adjacent_vertices(v, g);  ai != ai_end; ++ai)
      std::cout << get(vertex_id, *ai) <<  " ";
    std::cout << std::endl;
  }
  Graph& g;
};


int main(int,char*[])
{
  // create a typedef for the Graph type
  typedef adjacency_list<vecS, vecS, bidirectionalS,
     no_property, property<edge_weight_t, float> > Graph;

  // Make convenient labels for the vertices
  enum { A, B, C, D, E, N };
  const int num_vertices = N;
  const char* name = "ABCDE";

  // writing out the edges in the graph
  typedef std::pair<int,int> Edge;
  Edge edge_array[] = 
  { Edge(A,B), Edge(A,D), Edge(C,A), Edge(D,C),
    Edge(C,E), Edge(B,D), Edge(D,E), };
  const int num_edges = sizeof(edge_array)/sizeof(Edge);

  // average transmission delay (in milliseconds) for each connection
  float transmission_delay[] = { 1.2, 4.5, 2.6, 0.4, 5.2, 1.8, 3.3, 9.1 };

  // declare a graph object, adding the edges and edge properties
  Graph g(num_vertices, 
          edge_array, edge_array + num_edges,
          transmission_delay);

  boost::property_map<Graph, vertex_index_t>::type 
    vertex_id = get(vertex_index, g);
  boost::property_map<Graph, edge_weight_t>::type
    trans_delay = get(edge_weight, g);

  std::cout << "vertices(g) = ";
  typedef graph_traits<Graph>::vertex_iterator vertex_iter;
  std::pair<vertex_iter, vertex_iter> vp;
  for (vp = vertices(g); vp.first != vp.second; ++vp.first)
    std::cout << name[get(vertex_id, *vp.first)] <<  " ";
  std::cout << std::endl;
  
  std::cout << "edges(g) = ";
  graph_traits<Graph>::edge_iterator ei, ei_end;
  for (tie(ei,ei_end) = edges(g); ei != ei_end; ++ei)
    std::cout << "(" << name[get(vertex_id, source(*ei, g))]
              << "," << name[get(vertex_id, target(*ei, g))] << ") ";
  std::cout << std::endl;
  
  std::for_each(vertices(g).first, vertices(g).second,
                exercise_vertex<Graph>(g));
  
  std::ofstream out("quick-tour.dot");

  std::map<std::string,std::string> graph_attr, vertex_attr, edge_attr;
  graph_attr["size"] = "3,3";
  graph_attr["rankdir"] = "LR";
  graph_attr["ratio"] = "fill";
  vertex_attr["shape"] = "circle";

  boost::write_graphviz(out, g, 
                        make_vertex_label_writer(name),
                        make_edge_label_writer(trans_delay),
                        make_graph_attr_writer(graph_attr, vertex_attr, 
                                               edge_attr));
  
  return 0;
}


