#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct Node {
    int data;
    Node* next;
};
/********************DO NOT EDIT**********************/
// Function prototype. Defined later.
void read_opinions(string filename); // reads file into opinions vector and updates total_nodes as needed
void read_edges(string filename); // reads file into edge_list, defined later
void build_adj_list(); // convert edge_list to adj list
Node* insertBack(Node* head, int value);
int total_nodes = 0; // We keep track of the total number of nodes based on largest node id.


/****************************************************************/

/******** Create adjacency matrix and vector of opinions */
// simple vector to hold each node's opinion (0 or 1)
std::vector<int> opinions;

// global adjacency LIST initialized later (n x variable matrix, 1 where connection is true)
std::vector<Node*> adj;

// edge list: each row contains {source, target}
std::vector<std::vector<int>> edge_list;

Node* insertBack(Node* head, int value) {
    Node* newNode = new Node();
    newNode->data = value;
    newNode->next = NULL;
    
    if (head == NULL) {
        return newNode;
    }
    
    Node* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
    
    return head;
}

void build_adj_list()
{
    // go through entire edge list and set adj[trg][src] to 1 -- can traverse all influences of trg easily
    // want to look at all target's potential influences to decide how their stance changes
    
    adj.assign(total_nodes, NULL); // reserve vector of total_nodes Node* init to null

    int src;
    int trg;
    for (int i = 0; i < edge_list.size(); i++) { // traverse entire edge_list, get src and trg
        src = edge_list[i][0];
        trg = edge_list[i][1];
        adj[trg] = insertBack(adj[trg], src); // add src to trg's list of influences
    }
}

double calculate_fraction_of_ones()
{
    double num_ones = 0;
    for (int i = 0; i < total_nodes; i++) {
        if (opinions[i] == 1) num_ones++;
    }
    return (num_ones / total_nodes);
}

// For a given node, count majority opinion among its neighbours. Tie -> 0.
int get_majority_friend_opinions(int node)
{
    int num_ones = 0;
    int num_zeros = 0;

    Node* curr = adj[node];
    while (curr != NULL) { // for all of node's influences
        if (opinions[curr->data] == 1) {  // if influence opinion is 1, add point for 1s
            num_ones++;
        } else {
            num_zeros++; // influence's opinion is a 0; add point for 0s
        }
        curr = curr->next;
    }

    if (num_ones > num_zeros) {
        return 1; // majority friend opinion is 1
    } else {
        return 0; // majority friend opinion is 0
    }

}

// Calculate new opinions for all voters and return if anyone's opinion changed
bool update_opinions() // asynchronous update - each actor opinion is updated from node 0-39.
{
    int changedFlag = 0;
    int friendOpinion = 0;
    for (int i = 0; i < total_nodes; i++) { // for all actors
        friendOpinion = get_majority_friend_opinions(i); // get majority opinion of their influences
        if (friendOpinion != opinions[i]) { // if influencers' opinion differs from current actor, change actor opinion
            changedFlag = 1;
            opinions[i] = friendOpinion;
        }
    }
    return changedFlag; // return 1 for any opinion changed, otherwise 0

}

int main() {
    // no preallocation; vectors grow on demand

    // Read input files
    read_opinions("opinions.txt"); 
    read_edges("edge_list.txt");

    // convert edge list into adjacency matrix once we know total_nodes
    build_adj_list();
    
    cout << "Total nodes: " << total_nodes << endl;
    
    // Run simulation
    int max_iterations = 30;
    int iteration = 0;
    bool opinions_changed = true;
    
    // Print initial state
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;
    
    /// (6)  //////////////////////////////////////////////
    int changed = 1;
    double fraction = calculate_fraction_of_ones();
    while ((iteration < max_iterations) && (fraction != 1.0) && (fraction != 0.0) && (changed)) {
        iteration++;
        changed = update_opinions();
        fraction = calculate_fraction_of_ones();
        if ((iteration%5) == 0) { // print update every 5 iterations
            cout << "Iteration " << iteration << ": fraction of 1's = " << fraction << endl;
        }
    }

    ////////////////////////////////////////////////////////
    // Print final result
    double final_fraction = calculate_fraction_of_ones();
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << final_fraction << endl;
    
    if(final_fraction == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if(final_fraction == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << iteration << " iterations" << endl;
    
    return 0;
}


/*********** Functions to read files **************************/ 

// Read opinion vector from file.
void read_opinions(string filename)
{
    ifstream file(filename);
    int id, opinion;
    while(file >> id >> opinion)
    {
        opinions.push_back(opinion);
        if(id >= total_nodes) total_nodes = id+1;
    }
    file.close();
}

// Read edge list from file and update total nodes as needed.
void read_edges(string filename)
{
    ifstream file(filename);
    int source, target;
    
    while(file >> source >> target)
    {
        edge_list.push_back({source, target});
        if(source >= total_nodes) total_nodes = source+1;
        if(target >= total_nodes) total_nodes = target+1;
    }
    file.close();
}

/********************************************************************** */