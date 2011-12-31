#include <gtest/gtest.h>

#include <iostream>
#include <boost/optional.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <bp/graph.hpp>

using namespace std;
using namespace boost;

typedef adjacency_list<
            vecS, vecS, bidirectionalS,
            no_property,
            property<edge_index_t, std::size_t> > Graph;
typedef graph_traits<Graph> Traits;
typedef Traits::edge_iterator EdgeIterator;
typedef Traits::edge_descriptor Edge;
typedef property_map<Graph, edge_index_t>::type EdgeIndexMap;

struct test_visitor {
    template<typename Message, typename Edge,
             typename MessageIterator,
             typename Vertex, typename Graph>
    void make_message(Message& target, const Edge& edge,
                      MessageIterator begin,
                      MessageIterator end,
                      const Vertex& vertex, const Graph& graph) {
        target = Message(1);
        std::cout << "message:"
                  << boost::get(boost::vertex_index,
                                graph,
                                boost::source(edge, graph))
                  << " -> "
                  << boost::get(boost::vertex_index,
                                graph,
                                boost::target(edge, graph))
                  << std::endl;
    }
};


TEST(graphTest, construction)
{
    Graph graph;

    const int max_n_vertex = 5;
    for(int i=0; i<max_n_vertex; ++i) {
        add_vertex(graph);
    }
    for(int i=0; i<max_n_vertex-1; ++i) {
        add_edge(i, i+1, 2*i, graph);
        add_edge(i+1, i, 2*i + 1, graph);
    }

    int belief[max_n_vertex];
    bp::sum_product<int>(graph, belief, test_visitor());

    ASSERT_TRUE(1 == 1);
}

