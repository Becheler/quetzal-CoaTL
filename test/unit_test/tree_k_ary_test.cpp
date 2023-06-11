// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#define BOOST_TEST_MODULE k_ary_tree_test

#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <quetzal/coalescence/graph/k_ary_tree.hpp>
#include <random>

template <typename Order, typename Vertex>
struct tree_visitor
{
  void operator()(Order stage, Vertex v)
  {
    switch(stage) {
      case boost::visit::pre :
        std::cout << "Pre " << v << std::endl;
        break;
      case boost::visit::in:
        std::cout << "In " << v << std::endl;
        break;
      case boost::visit::post:
        std::cout << "Post " << v << std::endl;
        break;
      default:
        std::cout << "Oops" << std::endl;
    }
  }
};

BOOST_AUTO_TEST_SUITE( k_ary_tree )

    /* Topology :
    *             a
    *           /   \
    *          /     c
    *         /    / | \
    *        b    d  e  f
     */

BOOST_AUTO_TEST_CASE(no_property_kary_interface)
{

  // tree with named verticesand no edge properties attached  
  using tree_type = quetzal::coalescence::k_ary_tree<boost::no_property, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;

  vertex_descriptor a = tree.add_vertex();
  vertex_descriptor b = tree.add_vertex();
  vertex_descriptor c = tree.add_vertex();
  vertex_descriptor d = tree.add_vertex();
  vertex_descriptor e = tree.add_vertex();
  vertex_descriptor f = tree.add_vertex();

  auto edges1 = tree.add_edges(a, {b, c} );
  auto edges2 = tree.add_edges(c, {d, e, f} );

  auto root = tree.find_root_from(f);
  BOOST_CHECK(root == a);
  BOOST_CHECK(tree.degree(c) == 4);
  BOOST_CHECK(tree.in_degree(c) == 1);
  BOOST_CHECK(tree.out_degree(c) == 3);
  BOOST_CHECK(tree.has_predecessor(root) == false);
  BOOST_CHECK(tree.predecessor(c) == root);
  BOOST_CHECK(tree.has_successors(root) == true);
  BOOST_CHECK( ! std::all_of( tree.successors(c) | std::views::transform(has_successors())));

  using vertex_descriptor = boost::graph_traits<tree_type>::vertex_descriptor;
  tree_visitor<boost::visit, vertex_descriptor> visitor;
  tree.depth_first_search(a, visitor);

  std::mt19937 rng{std::random_device{}()};
  auto [tree2, root2] = tree_type::make_random_tree(5, 10, rng);
  BOOST_CHECK(tree.is_isomorphic(tree2) == false);

}


BOOST_AUTO_TEST_CASE(simple_vertex_properties)
{
  // tree with string vertices but no edge properties
  using vertex_property = std::string;
  using tree_type = quetzal::coalescence::k_ary_tree<vertex_property, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;

  vertex_descriptor a = tree.add_vertex("a");
  vertex_descriptor b = tree.add_vertex("b");
  vertex_descriptor c = tree.add_vertex("c");
  vertex_descriptor d = tree.add_vertex("d");
  vertex_descriptor e = tree.add_vertex("e");

  // Read
  BOOST_CHECK(tree[a] == "a");

  // Write
  tree[a] = tree[b];
  BOOST_CHECK(tree[a] == "b"); 

}

struct vertex_info 
{ 
    std::string whatever; 
    int othervalue;
};

BOOST_AUTO_TEST_CASE(structure_vertex_property)
{
  // tree with string & int vertices but no edge properties
  using tree_type = quetzal::coalescence::k_ary_tree<vertex_info, boost::no_property>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = tree.add_vertex(vertex_info{"a", 0} );
  vertex_descriptor b = tree.add_vertex(vertex_info{"b", 1} );
  vertex_descriptor c = tree.add_vertex(vertex_info{"c", 2} );
  vertex_descriptor d = tree.add_vertex(vertex_info{"d", 3} );
  vertex_descriptor e = tree.add_vertex(vertex_info{"e", 4} );

  // Read
  BOOST_CHECK(tree[e].whatever == "e");
  BOOST_CHECK(tree[e].othervalue == 4);

  // Write 
  tree[e] = { "new", 5};

  BOOST_TEST(tree[e].whatever == "new");
  BOOST_CHECK(tree[e].othervalue == 5);

}

BOOST_AUTO_TEST_CASE(simple_edge_properties)
{
  // tree with string edges but no vertex properties attached
  using vertex_properties = boost::no_property;
  using edge_property = std::string;
  using tree_type = quetzal::coalescence::k_ary_tree<vertex_properties, edge_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  vertex_descriptor a = tree.add_vertex();
  vertex_descriptor b = tree.add_vertex();
  vertex_descriptor c = tree.add_vertex();
  vertex_descriptor d = tree.add_vertex();
  vertex_descriptor e = tree.add_vertex();

  // Add two edges with two different edge properties
  auto [ab_edge, ac_edge] = tree.add_edges(a, {b, edge_property("a->b")}, {c, edge_property("a->c")});
  auto [cd_edge, ce_edge] = tree.add_edges(ac_edge.second, {d, edge_property("c->d")}, {e, edge_property("c->e")});

  tree[ab_edge] = "a...b";
  BOOST_CHECK(tree[ab_edge] == "a...b");
}

struct edge_info { 
    std::string whatever; 
    int othervalue;
};

BOOST_AUTO_TEST_CASE(struct_edge_properties)
{
  // default tree with no edge/vertex properties attached
  using vertex_properties = boost::no_property;
  using edge_properties = edge_info;
  using tree_type = quetzal::coalescence::k_ary_tree<vertex_properties, edge_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;

  tree_type tree;
  
  // No property on vertices
  vertex_descriptor a = tree.add_vertex();
  vertex_descriptor b = tree.add_vertex();
  vertex_descriptor c = tree.add_vertex();
  vertex_descriptor d = tree.add_vertex();
  vertex_descriptor e = tree.add_vertex();

  // Pass info to build new edges
  auto [ab_edge, ac_edge] = tree.add_edges(a, { {b, edge_propertie("a->b", 10) }, {c, edge_properties("a->c", 11) } } );
  auto [cd_edge, ce_edge, cf_edge] = tree.add_edges(ac_edge.second, { 
    {d, edge_properties("c->d", 12)}, 
    {e, edge_properties("c->e", 13)},
    {e, edge_properties("c->f", 14)}
  });

  // Read edge info
  BOOST_CHECK(tree[ab_edge].whatever == "a->b");
  BOOST_CHECK(tree[ab_edge].othervalue == 10);

  // Write edge info
  tree[ab_edge] =  { "yolo", 99 };
  BOOST_CHECK(tree[ab_edge].whatever == "yolo");
  BOOST_CHECK(tree[ab_edge].othervalue == 99);
}

BOOST_AUTO_TEST_CASE(struct_both_properties)
{

  using vertex_properties = boost::no_property;
  using edge_properties = edge_info;
  using tree_type = quetzal::coalescence::k_ary_tree<vertex_properties, edge_properties>;
  using vertex_descriptor = tree_type::vertex_descriptor;
  using edge_properties = edge_info;

  tree_type tree;
  
  vertex_descriptor a = tree.add_vertex(vertex_properties("a", 0));
  vertex_descriptor b = tree.add_vertex(vertex_properties("b", 1));
  vertex_descriptor c = tree.add_vertex(vertex_properties("c", 2));
  vertex_descriptor d = tree.add_vertex(vertex_properties("d", 3));
  vertex_descriptor e = tree.add_vertex(vertex_properties("e", 4));

  // Pass info to build new edges
  auto [ab_edge, ac_edge] = tree.add_edges(a, 
    {b, edge_info("a->b", 10)},
    {c, edge_info("a->c", 11)}
  );

  auto [cd_edge, ce_edge, cf_edge] = tree.add_edges(ac_edge.second, { 
    {d, edge_properties("c->d", 12)}, 
    {e, edge_properties("c->e", 13)},
    {e, edge_properties("c->f", 14)}
  });

  // Read vertices
  BOOST_CHECK(tree[e].whatever == "e");
  BOOST_CHECK(tree[e].othervalue == 4);

  // Write vertices
  tree[e] = { "new", 5};
  BOOST_CHECK(tree[e].whatever == "new");
  BOOST_CHECK(tree[e].othervalue == 5);

  // Read edge info
  BOOST_CHECK(tree[ab_edge].whatever == "a->b");
  BOOST_CHECK(tree[ab_edge].othervalue == 10);

  // Write edge info
  tree[ab_edge] =  { "yolo", 99 };
  BOOST_CHECK(tree[ab_edge].whatever == "yolo");
  BOOST_CHECK(tree[ab_edge].othervalue == 99);

}

BOOST_AUTO_TEST_SUITE_END()
