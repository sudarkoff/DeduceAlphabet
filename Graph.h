#ifndef _GRAPH_H_DD36A3CA_6B51_4DB0_9FBF_2E2BFA6EF218_DEFINED_
#define _GRAPH_H_DD36A3CA_6B51_4DB0_9FBF_2E2BFA6EF218_DEFINED_

#include <vector>
#include <fstream>
#include <stdexcept>
#include <assert.h>

#define ALPHABET_MAX 66

namespace amz {

template <typename T>
class graph
{
public:
    typedef T value_type;
    typedef std::vector<value_type> nodes_type;
    typedef std::vector<int> order_type;

    graph() { clear(); }
    virtual ~graph() {}
    
    void clear()
    {
        nodes_.clear();
        order_.clear();
        for (size_t i = 0; i < ALPHABET_MAX; ++i) {
            for (size_t j = 0; j < ALPHABET_MAX; ++j) {
                edges_[i][j] = 0;
            }
        }
    }
    
    // inserts new (disconnected) node to the graph
    void insert_node(value_type const& node)
    { nodes_.push_back(node); }

    // connects two existing nodes
    void insert_edge(value_type const& node1, value_type const& node2, int weight = 1)
    {
        size_t i1 = get_node_index(node1);
        assert(i1 < nodes_.size());
        size_t i2 = get_node_index(node2);
        assert(i2 < nodes_.size());
        edges_[i1][i2] = weight;
    }

    // removes the connection between two nodes
    int erase_edge(value_type const& node1, value_type const& node2)
    {
        size_t i1 = get_node_index(node1);
        assert(i1 < nodes_.size());
        size_t i2 = get_node_index(node2);
        assert(i2 < nodes_.size());
        int weight = edges_[i1][i2];
        edges_[i1][i2] = 0;
        return weight;
    }
    
    // returns true if the element already exists in graph
    bool node_exists(value_type const& node) const
    {
        size_t size = nodes_.size();
        return (size != 0) && (get_node_index(node) < size);
    }
    
    // returns true if there's a path between two specified nodes
    bool path_exists(value_type const& node1, value_type const& node2) const
    {
        nodes_type adj;
        dfs(node1, adj);
        return (find(adj.begin(), adj.end(), node2) != adj.end());
    }
    
    // returns the list of all nodes directly reachable from a given node
    void find_neighbors(value_type const& node, nodes_type& result) const
    {
        size_t index = get_node_index(node);
        assert(index < nodes_.size());
        for (size_t i=0; i<nodes_.size(); ++i) {
            if (edges_[index][i] > 0) {
                result.push_back(nodes_[i]);
            }
        }
    }
    
    // returns the list of all nodes reachable from a given one in a depth-first order
    void dfs(value_type const& node, nodes_type& result) const
    {
        std::vector<value_type> stack;
        stack.push_back(node);
        
        while (!stack.empty()) {
            value_type n = stack.back();
            stack.pop_back();
            if (find(result.begin(), result.end(), n) == result.end()) {
                result.push_back(n);
                nodes_type adj;
                find_neighbors(n, adj);
                
                typename nodes_type::iterator i = adj.begin();
                for (; i != adj.end(); ++i) {
                    if (find(result.begin(), result.end(), *i) == result.end()) {
                        stack.push_back(*i);
                    }
                }
            }
        }
    }

    // 1) checks whether the graph is degenerate and connected
    // 2) sorts the graph topologically and stores the order
    void sort()
    {
        int head = get_head();
        order_.push_back(head);

        int tail = get_tail();
        // if the tail is not rechable from the head, we don't have enough data to
        // deduce the alphabet
        if (!path_exists(nodes_[head], nodes_[tail])) {
            throw std::runtime_error("not enough data to deduce the alphabet");
        }
        
        do {
            // find all neighbors
            nodes_type adj;
            for (size_t i=0; i<nodes_.size(); ++i) {
                if (edges_[head][i] > 0) {
                    adj.push_back(i);
                }
            }
            if (adj.size() == 0) {
                break;
            }
            // if there's more than one neighbor (branch)
            else if (adj.size() > 1) {
                throw std::runtime_error("not enough data to deduce the alphabet");
            }
            // if we already visited this node (loop)
            if (find(order_.begin(), order_.end(), adj[0]) != order_.end()) {
                throw std::runtime_error("data is inconsistent");
            }
            head = adj[0];
            order_.push_back(head);
        } while (head != tail);
    }
    
    // removes the direct connection between two nodes if there's another path between them
    void eliminate_shortcuts()
    {
        // for all edges
        for (size_t i = 0; i < nodes_.size(); ++i) {
            for (size_t j = 0; j < nodes_.size(); ++j) {
                if (edges_[i][j] > 0) {
                    // remove the connection
                    erase_edge(nodes_[i], nodes_[j]);
                    // can't reach the node anymore?
                    if (!path_exists(nodes_[i], nodes_[j])) {
                        // insert the connection back
                        insert_edge(nodes_[i], nodes_[j]);
                    }
                }
            }
        }
    }
    
    // exports the graph to a dot-file
    void export_dot(char const* filename) const
    {
        std::ofstream ofs(filename);
        ofs << "digraph Alphabet {" << std::endl 
            << "\tsize = \"8,8\";" << std::endl;
        // output all edges
        for (size_t i = 0; i < nodes_.size(); ++i) {
            for (size_t j = 0; j < nodes_.size(); ++j) {
                if (edges_[i][j] > 0) {
                    ofs << "\t" << nodes_[i] << " -> " << nodes_[j] << ";" << std::endl;
                }
            }
        }
        // output all nodes (some of which might have no connections)
        for (size_t i = 0; i < nodes_.size(); ++i) {
            ofs << "\t" << nodes_[i] << ";" << std::endl;
        }
        ofs << "}" << std::endl;
    }
    
    // outputs the nodes in topological order
    void output (std::ostream& os) const
    {
        order_type::const_iterator i = order_.begin();
        for (; i != order_.end(); ++i) {
            os << nodes_[*i] << " ";
        }
    }


private:
    // returns the index of a give node
    size_t get_node_index(value_type const& node) const
    {
        size_t index = 0;        
        while ((index != nodes_.size()) && (nodes_[index] != node)) {
            index++;
        }        
        return index;
    }

    // returns the head
    //@todo templatize and pass the selector functor as a template parameter 
    // to avoid duplicate code in get_tail
    size_t get_head() const
    {
        std::vector<size_t> indices;
        bool empty = true;
        for (size_t c = 0; c < nodes_.size(); ++c) {
            empty = true;
            for (size_t r = 0; r < nodes_.size(); ++r) {
                if (edges_[r][c] > 0) {
                    empty = false;
                    break;
                }
            }
            if (empty) {
                indices.push_back(c);
            }
        }
        // no head (loop)
        if (indices.empty())
            throw std::runtime_error("the data is inconsistent");
        // more than one head
        if (indices.size() > 1)
            throw std::runtime_error("not enough data to deduce the alphabet");
            
        return indices[0];
    }

    // returns the tail
    size_t get_tail() const
    {
        std::vector<size_t> indices;
        bool empty = true;
        for (size_t r = 0; r < nodes_.size(); ++r) {
            empty = true;
            for (size_t c = 0; c < nodes_.size(); ++c) {
                if (edges_[r][c] > 0) {
                    empty = false;
                    break;
                }
            }
            if (empty) {
                indices.push_back(r);
            }
        }
        if (indices.empty())
            throw std::runtime_error("the data is inconsistent");
        if (indices.size() > 1)
            throw std::runtime_error("not enough data to deduce the alphabet");

        return indices[0];
    }
    
private:
    // list of all nodes
    nodes_type nodes_;
    // adjacency matrix
    int edges_[ALPHABET_MAX][ALPHABET_MAX]; //@todo Use a custom 2d array container
    order_type order_;

};

} // namespace amz

#endif // _GRAPH_H_DD36A3CA_6B51_4DB0_9FBF_2E2BFA6EF218_DEFINED_
