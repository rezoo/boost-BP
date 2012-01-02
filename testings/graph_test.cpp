#include <gtest/gtest.h>

#include <iostream>
#include <boost/optional.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <bp/graph.hpp>

using namespace boost;

typedef property<vertex_belief_t, int> VertexProperty;
typedef property<edge_message_t, int> EdgeProperty;

typedef adjacency_list<
            vecS, vecS, bidirectionalS,
            VertexProperty, EdgeProperty> Graph;
typedef graph_traits<Graph> Traits;
typedef Traits::edge_iterator EdgeIterator;
typedef Traits::edge_descriptor Edge;

struct test_visitor {

    template<typename Graph>
    void init_messages(Graph& graph) {
        typedef graph_traits<Graph> Traits;
        typedef typename Traits::vertex_descriptor Vertex;
        typedef typename Traits::edge_descriptor Edge;

        typename Traits::edge_iterator ei, ei_end;
        tie(ei, ei_end) = edges(graph);
        for(; ei != ei_end; ++ei) {
            put(edge_message, graph, *ei, -1);
        }
    }

    template<typename Edge, typename Graph>
    bool is_initialized(const Edge& edge, const Graph& graph) {
        return (get(edge_message, graph, edge) > 0);
    }

    template<typename Edge, typename EdgeIterator, typename Graph>
    void make_message(const Edge& message,
                      EdgeIterator in_begin,
                      EdgeIterator in_end,
                      Graph& graph) {
        put(edge_message, graph, message, 1);
        std::cout << "message:"
                  << get(vertex_index,
                         graph,
                         boost::source(message, graph))
                  << " -> "
                  << get(vertex_index,
                         graph,
                         target(message, graph))
                  << std::endl;
    }

    template<typename Vertex, typename EdgeIterator, typename Graph>
    void make_belief(const Vertex& target,
                     EdgeIterator in_begin,
                     EdgeIterator in_end,
                     Graph& graph) {
        // do nothing
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
        add_edge(i, i+1, graph);
        add_edge(i+1, i, graph);
    }

    bp::sum_product(graph, test_visitor());

    ASSERT_TRUE(1 == 1);
}

