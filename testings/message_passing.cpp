#include <gtest/gtest.h>

#include <iostream>
#include <boost/optional.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <bp/belief_propagation.hpp>

using namespace boost;

struct test_visitor {

    template<typename MessagePropertyMap, typename Graph>
    void init_messages(MessagePropertyMap message_map,
                       const Graph& graph) {}

    template<typename BeliefPropertyMap, typename Graph>
    void init_beliefs(BeliefPropertyMap, const Graph&) {}

    template<typename Edge,
             typename EdgeIterator,
             typename MessagePropertyMap,
             typename Graph>
    void make_message(Edge message,
                      EdgeIterator, EdgeIterator,
                      MessagePropertyMap message_map,
                      const Graph& graph) {
        put(message_map, message, 1);
        /*
        std::cout << "message:"
                  << get(vertex_index, graph,
                         boost::source(message, graph))
                  << " -> "
                  << get(vertex_index, graph,
                         target(message, graph))
                  << std::endl;
        */
    }

    template<typename Vertex,
             typename EdgeIterator,
             typename MessageMap,
             typename BeliefMap,
             typename Graph>
    void make_belief(Vertex vertex,
                     EdgeIterator in_begin,
                     EdgeIterator in_end,
                     MessageMap,
                     BeliefMap belief_map,
                     const Graph& graph) {
        int result = (int)std::distance(in_begin, in_end);
        put(belief_map, vertex, result);
        /*
        std::cout << "belief: "
                  << get(vertex_index, graph, vertex)
                  << " " << get(belief_map, vertex)
                  << std::endl;
        */
    }
};


TEST(MessagePassing, linear_graph)
{
    typedef property<vertex_belief_t, int> VertexProperty;
    typedef property<edge_index_t, std::size_t,
            property<edge_message_t, int> > EdgeProperty;
    typedef adjacency_list<
                vecS, vecS, bidirectionalS,
                VertexProperty, EdgeProperty> Graph;
    typedef graph_traits<Graph> Traits;

    Graph graph;

    const int max_n_vertex = 100;
    for(int i=0; i<max_n_vertex; ++i) {
        add_vertex(graph);
    }
    for(int i=0; i<max_n_vertex-1; ++i) {
        add_edge(i, i+1, 2*i, graph);
        add_edge(i+1, i, 2*i + 1, graph);
    }

    bp::apply_belief_propagation(graph, test_visitor());

    Traits::edge_iterator ei, ei_end;
    tie(ei, ei_end) = edges(graph);
    for(; ei != ei_end; ++ei) {
        ASSERT_EQ(get(edge_message, graph, *ei), 1);
    }
    Traits::vertex_iterator vi, vi_end;
    tie(vi, vi_end) = vertices(graph);
    for(; vi != vi_end; ++vi) {
        int index = (int)get(vertex_index, graph, *vi);
        int result = get(vertex_belief, graph, *vi);
        if(index == 0 || index == max_n_vertex-1)
            ASSERT_EQ(result, 1);
        else
            ASSERT_EQ(result, 2);
    }
}

TEST(MessagePassing, tree_graph)
{
    typedef property<vertex_belief_t, int> VertexProperty;
    typedef property<edge_index_t, std::size_t,
            property<edge_message_t, int> > EdgeProperty;
    typedef adjacency_list<
                vecS, vecS, bidirectionalS,
                VertexProperty, EdgeProperty> Graph;
    typedef graph_traits<Graph> Traits;

    Graph graph;

    const int max_n_depth = 10;
    const int max_n_vertex = (1 << (max_n_depth + 1)) - 1;
    for(int i=0; i<max_n_vertex; ++i) {
        add_vertex(graph);
    }
    const int max_n_loop = (1 << max_n_depth) - 1;
    int edge_counter = 0;
    for(int i=0; i<max_n_loop; ++i, edge_counter += 4) {
        const int i1 = 2*i + 1;
        const int i2 = 2*i + 2;
        add_edge(i, i1, edge_counter + 0, graph);
        add_edge(i1, i, edge_counter + 1, graph);
        add_edge(i, i2, edge_counter + 2, graph);
        add_edge(i2, i, edge_counter + 3, graph);
    }

    bp::apply_belief_propagation(graph, test_visitor());

    Traits::edge_iterator ei, ei_end;
    tie(ei, ei_end) = edges(graph);
    for(; ei != ei_end; ++ei) {
        ASSERT_EQ(get(edge_message, graph, *ei), 1);
    }
    Traits::vertex_iterator vi, vi_end;
    tie(vi, vi_end) = vertices(graph);
    for(; vi != vi_end; ++vi) {
        int index = (int)get(vertex_index, graph, *vi);
        int result = get(vertex_belief, graph, *vi);
        if(index == 0)
            ASSERT_EQ(result, 2);
        else if(max_n_loop <= index && index < max_n_vertex)
            ASSERT_EQ(result, 1);
        else
            ASSERT_EQ(result, 3);
    }
}
