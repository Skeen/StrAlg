#ifndef SUFFIX_TREE_HPP
#define SUFFIX_TREE_HPP

#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <string>
#include <cassert>

namespace std
{
    template<typename T>
    string to_string(T input)
    {
         std::stringstream s;
         s << input;
         return s.str();
    }
}

bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

struct substring_index
{
    substring_index(int offset, int length)
        : offset(offset), length(length)
    {
    }

    bool operator<(const substring_index& other) const
    {
        return offset < other.offset;
    }

    unsigned int offset;
    unsigned int length;
};

substring_index longest_common_prefix(const std::string& input_string, substring_index child, substring_index suffix)
{
    substring_index index(child.offset, 0);
    for(unsigned int x=0; x<child.length; x++)
    {
        assert(x < suffix.length);

        char child_c = input_string.at(child.offset + x);
        char suffix_c = input_string.at(suffix.offset + x);
        if(child_c != suffix_c)
        {
            break;
        }
        index.length++;
    }
    return index;
}

substring_index longest_common_prefix(const std::string& input_string, substring_index child, std::string suffix)
{
    substring_index index(child.offset, 0);
    for(unsigned int x=0; x<child.length; x++)
    {
        if(x >= suffix.length())
        {
            return index;
        }

        char child_c = input_string.at(child.offset + x);
        char suffix_c = suffix.at(x);
        if(child_c != suffix_c)
        {
            break;
        }
        index.length++;
    }
    return index;
}

std::string get_str(const std::string& input_string, substring_index id)
{
    std::string str = std::string(input_string.begin()+id.offset,
            input_string.begin()+id.offset+id.length);

    return str;
}


class SuffixTreeNode
{
    public:
        using pair = std::pair<substring_index, SuffixTreeNode*>;

        SuffixTreeNode(std::string name = "")
            : name(name)
        {
        }

        std::string name;
        //Offset, length
        std::map<substring_index, SuffixTreeNode*> children;
};

class SuffixTree
{
    private:
        SuffixTreeNode* root;
        std::string input_string;

    private:
        SuffixTree(std::string str)
            : root(new SuffixTreeNode()), input_string(str)
        {
        }

    public:
        static SuffixTree naiveConstruct(std::string str)
        {
            // Push back our special terminator
            str.push_back('$');
            // Create a nulled tree
            SuffixTree stree(str);
            // For each suffix
            for(unsigned int offset = 0; offset < str.length(); offset++)
            {
                // We start in the root
                SuffixTreeNode* current = stree.root;
                // Calculate the length of our suffix
                int length = str.size() - offset;
                substring_index suffix(offset, length);
#ifdef DEBUG_PRINT
                // Debug print the suffix
                std::cout << "***********************************" << std::endl;
                std::cout << "SUFFIX_INPUT: " << get_str(str, suffix) << std::endl;
                std::cout << "***********************************" << std::endl;
#endif //DEBUG_PRINT
                while (true)
                {
                    // The first character of our suffix
                    char suffix_c = str.at(suffix.offset);
                    auto test_lambda = [&](SuffixTreeNode::pair child)
                    {
                        char child_c = str.at(std::get<0>(child).offset);
                        return child_c == suffix_c;
                    };
                    // Figure if we're a prefix of any children of this node
                    bool is_prefix = std::any_of(begin(current->children), end(current->children), test_lambda);
                    // If there were no such child
                    if (is_prefix == false)
                    {
                        // Add us, as a child
                        current->children.emplace(suffix,
                            new SuffixTreeNode(std::to_string(offset)));
                        break;
                    }
                    else    // If there were indeed such a child
                            // (that we are a prefix of)
                    {
                        // So let's find the child!
                        // Note: By the precondition, there will be at most one
                        auto child_iter = std::find_if(
                            begin(current->children), end(current->children), test_lambda);

                        SuffixTreeNode::pair child_pair = *child_iter;
                        substring_index child_index = std::get<0>(child_pair);
                        SuffixTreeNode* child_node  = std::get<1>(child_pair);
                        // Find the offset and length of the prefix
                        substring_index prefix = longest_common_prefix(str, child_index, suffix);
/*
                        std::cout << "----------------------------" << std::endl;
                        std::cout << "suffix: " << get_str(str, suffix) << std::endl;
                        std::cout << "child: " << get_str(str, child_index) << std::endl;
                        std::cout << "prefix: " << get_str(str, prefix) << std::endl;
*/
                        // If we consumed the entire child, as prefix, just follow it
                        if (prefix.length == child_index.length)
                        {
                            // But first substract the prefix from ourselves
                            suffix.offset += prefix.length;
                            suffix.length -= prefix.length;
                            current = child_node;
                            continue;
                        }
                        // If we're a substring, branch
                        else if (prefix.length < suffix.length)
                        {
                            // Remove the previous child node
                            current->children.erase(child_iter);
                            // Create one to be in the middle of the line
                            SuffixTreeNode* middle =
                                new SuffixTreeNode();
                            // Add this node, to the parent
                            current->children.emplace(prefix, middle);
                            // Update the child, to represent a smaller
                            // substring
                            child_index.offset += prefix.length;
                            child_index.length -= prefix.length;
                            //child_node->offset = child_index.offset;
                            // Update the suffix, to represent a smaller
                            // substring
                            suffix.offset += prefix.length;
                            suffix.length -= prefix.length;
                            // Add two children to this one, namely to the child
                            // and the suffix
                            middle->children.emplace(suffix, new SuffixTreeNode(std::to_string(offset)));
                            middle->children.emplace(child_index, child_node);
                            break;
                        }
                    }
                    break;
                }
            }
            return stree;
        }

        // Follow the path given by str, return the Node we end in, or nullptr if we fall off
        SuffixTreeNode* followPath(std::string str)
        {
            //std::cout << "followPath called" << std::endl;

            SuffixTreeNode* current = root;
            for(int offset = 0; offset < static_cast<int>(str.length()); offset++)
            {
                //std::cout << "STRING: " << str << std::endl;
                char str_c = str.at(offset);
                auto test_lambda = [&](SuffixTreeNode::pair child)
                {
                    char child_c = input_string.at(std::get<0>(child).offset);
                    return child_c == str_c;
                };
                // Figure if we're a prefix of any children of this node
                bool is_prefix =
                    std::any_of(begin(current->children),
                                end(current->children), test_lambda);
                // If there were no such child
                if (is_prefix == false)
                {
                    return current;
                }
                else    // If there were indeed such a child
                        // (that we are a prefix of)
                {
                    // So let's find the child!
                    // Note: By the precondition, there will be at most one
                    auto child_iter =
                        std::find_if(begin(current->children),
                                     end(current->children), test_lambda);

                    SuffixTreeNode::pair child_pair = *child_iter;
                    substring_index child_index = std::get<0>(child_pair);
                    SuffixTreeNode* child_node = std::get<1>(child_pair);
                    // Find the offset and length of the prefix
                    substring_index prefix =
                        longest_common_prefix(input_string, child_index, str);
/*
                    std::cout << "----------------------------" << std::endl;
                    std::cout << "suffix: " << str << std::endl;
                    std::cout << "child: " << get_str(input_string, child_index)
                              << std::endl;
                    std::cout << "prefix: " << get_str(input_string, prefix)
                              << std::endl;
*/

                    // If we consumed the entire child, as prefix, just follow it
                    if (prefix.length < str.length())
                    {
                        // But first substract the prefix from ourselves
                        str.erase(0, prefix.length);
                        offset = -1;
                        current = child_node;
                        continue;
                    }
                    // If we're a substring, report
                    else if (prefix.length >= str.length())
                    {
                        return child_node;
                    }
                }
            }
            return nullptr;
        }

        bool isSubstring(std::string str)
        {
            return (followPath(str) != nullptr);
        }
/*
        bool isSuffix(std::string str)
        {
            SuffixTreeNode* current = followPath(str);
            return (current != nullptr) && (current->children.find('$') != current->children.end());
        }
*/
        std::vector<SuffixTreeNode*> findLeafNodes(SuffixTreeNode* parent)
        {
            std::vector<SuffixTreeNode*> leafs;

            for(SuffixTreeNode::pair child : parent->children)
            {
                std::string str = get_str(input_string, std::get<0>(child));
                // This is a leaf
                if(hasEnding(str, "$") == true)
                {
                    leafs.push_back(std::get<1>(child));
                }
                else // This is not a leaf
                {
                    std::vector<SuffixTreeNode*> subleaves = findLeafNodes(std::get<1>(child));
                    leafs.insert(leafs.end(), subleaves.begin(), subleaves.end());
                }
            }

            return leafs;
        }

        std::vector<std::string> findOccurences(std::string str)
        {
            std::vector<std::string> occurences;

            SuffixTreeNode* current = followPath(str);
            // There are occourences
            if (current != nullptr)
            {
                /*
                // Debug print
                std::cout << "There are occourences: " << current->name << std::endl;
                for(SuffixTreeNode::pair child : current->children)
                {
                    std::cout << std::get<1>(child)->name << std::endl;
                }
                */

                // We just need to find them
                std::vector<SuffixTreeNode*> leafs = findLeafNodes(current);
                for(SuffixTreeNode* leaf : leafs)
                {
                    occurences.push_back(leaf->name);
                }
            }
            std::sort(occurences.begin(), occurences.end());
            return occurences;
        }

        void to_dot(std::stringstream& dot, SuffixTreeNode* parent, int& id)
        {
            int parent_id = id;
            for(SuffixTreeNode::pair child : parent->children)
            {
                id++;

                substring_index index = std::get<0>(child);
                #ifdef DEBUG_PRINT
                std::cout << index.offset << " " << index.length << std::endl;
                std::cout << get_str(input_string, index) << std::endl;
                #endif //DEBUG_PRINT

                std::string str = get_str(input_string, index);
                std::replace(str.begin(), str.end(), '\n', '#');

                dot << "n" << id << "[label=\"" << std::get<1>(child)->name << "\"];" << std::endl;
                dot << "n" << parent_id << " -> n" << id << " [label=\"" << str << "\"];" << std::endl;

                to_dot(dot, std::get<1>(child), id);
            }
        }

        void to_dot(std::string output_file)
        {
            int id = 0;

            std::stringstream dot;
            dot << "digraph g {" << std::endl;
            dot << "n0 [label=\"ROOT\"];" << std::endl;
            to_dot(dot, root, id);
            dot << "}";

            std::ofstream dot_file;
            dot_file.open(output_file);
            dot_file << dot.str();
            dot_file.close();
        }
};

#endif //SUFFIX_TREE_HPP
