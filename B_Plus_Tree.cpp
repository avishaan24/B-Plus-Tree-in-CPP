#include<bits/stdc++.h>
#define IOS ios_base::sync_with_stdio(0);cin.tie(NULL);cout.tie(NULL);
using namespace std;
#define ll long long
#define fl(x,y,n) for(ll x=y;x<n;x++)
#define rfl(x,y,n) for(ll x=y;x>=n;x--)
#define line "\n"
#define yesno  if(result) cout<<"YES"<<line;else cout<<"NO"<<line;
#define pb push_back
#define all(v) v.begin(),v.end()
#define rev(v) v.rbegin(),v.rend()
#define mod  1000000007

// Sample test case : 3 2 1 20 1 11 1 14 1 25 1 30 1 12 1 22 1 23 1 24 2 20 2 22 2 24 2 14 2 12

class Node{
    public:
        bool is_leaf;

        ll fill;

        vector<ll>keys;

        vector<Node*>pointer;
        
        Node(ll bucket_size, bool leaf);
};

class B_plus_tree{
    private:
        ll order, leaf_order, min_leaf, min_order;

        Node *root;

        map<ll, vector<ll>>directory;

        set<ll>keys;
    public:
        B_plus_tree(ll order, ll leaf_order);

        Node * get_parent(Node * par, Node * curr);

        Node* get_proper_leaf(ll val);

        void fix_internal_nodes(Node * parent, Node * child, ll val);

        void split_bucket(Node * curr, ll val);

        void insert_into_node(Node * bucket, ll val);

        void insertion(ll val);

        Node * find_internal_node(ll val);

        int get_maximum_key(Node * curr);

        void delete_from_node(Node * bucket, ll val);

        void merge_internal_node(Node * bucket);

        void deletion(ll val);

        void print(Node * temp, ll level);

        void print_directory();
};

// All fuctions declarations of nodes class

// Constructor function of nodes class
Node::Node(ll bucket_size, bool leaf){
    this->is_leaf = leaf;
    this->fill = 1;
    keys.resize(bucket_size);
    pointer.resize(bucket_size + 1, NULL);
}

// All functions declarations of B+ Tree directory class

// Constructor function of B+ Tree directory class
B_plus_tree::B_plus_tree(ll order, ll leaf_order){
    this->order = order;
    this->leaf_order = leaf_order;
    this->min_order = (ll)(ceil((order - 1) / 2.0));
    this->min_leaf = (ll)(ceil(leaf_order/ 2.0));
    this->root = NULL;
}

// Function to find parent of some bucket node
Node * B_plus_tree::get_parent(Node * par, Node * curr){
    Node * check = NULL;
    if(par->is_leaf)
        return NULL;
    fl(i,0,par->fill + 1){
        if(par->pointer[i] == curr){
            check = par;
            return par;
        }
        else{
            check = get_parent(par->pointer[i], curr);
            if(check != NULL)
                return check;
        }    
    }
    return check;
}

// Function to get proper leaf where that key is present or we have to add 
Node * B_plus_tree::get_proper_leaf(ll val){
    Node * curr = root;
    while(!curr->is_leaf){
        bool result = false;
        fl(i,0,curr->fill){
            if(curr->keys[i] >= val){
                curr = curr->pointer[i];
                result = true;
                break;
            }
        }
        if(!result)
            curr = curr->pointer[curr->fill];
    }
    return curr;
}


void B_plus_tree::fix_internal_nodes(Node * parent, Node * child, ll val){
    cout << "[INFO] Internal nodes splitted" << line;
    // if parent has empty space for the incoming key then place it on that parent
    if(parent->fill < order - 1){
        bool result = false;

        // find appropriate position, place it and maintain the pointers
        rfl(i,parent->fill - 1, 0){
            if(val < parent->keys[i]){
                parent->keys[i + 1] = parent->keys[i];
                parent->pointer[i + 2] = parent->pointer[i + 1];
            }
            else{
                parent->keys[i + 1] = val;
                parent->pointer[i + 2] = child;
                result = true;
                break;
            }
        }
        if(!result){
            parent->keys[0] = val;
            parent->pointer[1] = child;
        }
        parent->fill++;
    }
    else{
        // if parent already filled then we have split the internal node
        Node * extra = new Node(order - 1, false);
        vector<ll>elements = parent->keys;
        elements.pb(100);
        vector<Node*>pointer = parent->pointer;
        pointer.pb(NULL);
        bool result = false;

        // add new key to the extra bucket and then split into two internal nodes
        rfl(i,elements.size() - 2,0){
            if(val < elements[i]){
                elements[i + 1] = elements[i];
                pointer[i + 2] = pointer[i + 1];
            }
            else{
                elements[i + 1] = val;
                pointer[i  + 2] = child;
                result = true;
                break;
            }
        }
        if(!result){
            elements[0] = val;
            pointer[1] = child;
        }

        // split into two internal nodes and maintain all pointers
        ll up_index = (order + 1) / 2;
        fl(i,0,up_index)
            parent->pointer[i] = pointer[i];
        fl(i,0,up_index - 1)
            parent->keys[i] = elements[i];
        fl(i,up_index,order + 1)
            extra->pointer[i - up_index] = pointer[i];
        fl(i,up_index,order)
            extra->keys[i - up_index] = elements[i];
        parent->fill = up_index - 1, extra->fill = order - up_index;

        // if previous bucket is root then we have to create a new root to adjust two internal nodes
        if(parent == root){
            root = new Node(order - 1, false);
            root->keys[0] = elements[up_index - 1];
            root->pointer[0] = parent, root->pointer[1] = extra;
            cout << "[INFO] Root shifted upwards" << line;
        }
        // else again we have to fix the parent of both and add keys to the parent
        else
            fix_internal_nodes(get_parent(root, parent), extra, elements[up_index - 1]);
    }
}

// Function to split to remove overflow of buckets
void B_plus_tree::split_bucket(Node * curr, ll val){
    cout << "[INFO] Leaf Node is splitted into leaf nodes" << line;
    vector<ll>elements = curr->keys;
    elements.pb(100);
    bool result = false;

    // place that element in proper place
    rfl(i,elements.size() - 2,0){
        if(val < elements[i])
            elements[i + 1] = elements[i];
        else{
            elements[i + 1] = val;
            result = true;
            break;
        }
    }
    if(!result)
        elements[0] = val;

    ll new_size = (ll)(ceil((leaf_order + 1) / 2.0)), new_leaf_size = leaf_order + 1 - new_size;
    Node * extra = new Node(leaf_order, true), * temp = curr->pointer[leaf_order];
    curr->pointer[new_size] = extra, extra->pointer[new_leaf_size] = temp;
    curr->fill = new_size, extra->fill = new_leaf_size;

    // place half elements in one block and other half on other
    fl(i,0,leaf_order  + 1){
        if(i < new_size)
            curr->keys[i] = elements[i];
        else
            extra->keys[i - new_size] = elements[i];
    }

    // if intial half block is root then we have to create one root which join both blocks
    if(curr == root){
        root = new Node(order - 1, false);
        root->keys[0] = curr->keys[new_size - 1];
        root->pointer[0] = curr, root->pointer[1] = extra;
        cout << "[INFO] Root shifted upwards" << line;
    }
    else{
        Node * parent = get_parent(root, curr);

        // now we have to maintain internal nodes after insertion
        fix_internal_nodes(parent, extra, curr->keys[new_size - 1]);
    }
}


// Function to insert key in the specified node 
void B_plus_tree::insert_into_node(Node * bucket, ll val){
    bool result = false;

    // finding proper position of that key and shift all pointer
    rfl(i, bucket->fill - 1, 0){
        if(val < bucket->keys[i]){
            bucket->keys[i + 1] = bucket->keys[i];
            bucket->pointer[i + 2] = bucket->pointer[i + 1];
        }
        else{
            bucket->keys[i + 1] = val;
            result = true;
            break;
        }
    }
    if(bucket->is_leaf){
        bucket->pointer[bucket->fill + 1] = bucket->pointer[bucket->fill];
        bucket->pointer[bucket->fill] = NULL;
    }
    if(!result)
        bucket->keys[0] = val;
    bucket->fill++;
}


// Function to insert key into the tree
void B_plus_tree::insertion(ll val){

    if(keys.find(val) != keys.end()){
        cout << "[ERROR] Key already exists" << line;
        return;
    }

    // if tree is empty then create one else add on the same tree
    if(root == NULL){
        cout << "[INFO] New directory is created" << line;
        root = new Node(leaf_order, true);
        root->keys[0] = val;
    }
    else{
        Node * bucket = get_proper_leaf(val); // proper leaf to insert key
        // if bucket is fill then split bucket
        if(bucket->fill < leaf_order)
            insert_into_node(bucket, val);
        else
            split_bucket(bucket, val);
    }
    keys.insert(val);
    cout << "[SUCCESS] Key added into the directory" << line;
}

// Function to find internal nodes that contain that value
Node * B_plus_tree::find_internal_node(ll val){
    Node * curr = root;
    while(!curr->is_leaf){
        // if bucket contain that key then retun that bucket
        fl(i,0,curr->fill){
            if(curr->keys[i] == val)
                return curr;
        }
        bool result = false;
        // else move to that bucket which have possiblity to find that key
        fl(i,0,curr->fill){
            if(val < curr->keys[i]){
                curr = curr->pointer[i];
                result = true;
                break;
            }
        }
        if(!result)
            curr = curr->pointer[curr->fill];
    }

    // if key does not belong to any internal nodes
    return NULL;
}

// Function to find maximum key which can be replced to right side of the given pointer
int B_plus_tree::get_maximum_key(Node * curr){
    while(!curr->is_leaf)
        curr = curr->pointer[curr->fill];
    return curr->keys[curr->fill - 1];
}

// Function to delete key from the given bucket
void B_plus_tree::delete_from_node(Node * bucket, ll val){
    bool result = false;

    // find the index which contain that key and then adjust the pointers
    fl(i,0,bucket->fill){
        if(!result){
            if(bucket->keys[i] == val)
                result = true;
        }
        else{
            bucket->keys[i - 1] = bucket->keys[i];
            bucket->pointer[i] = bucket->pointer[i + 1]; 
        }
    }
    bucket->fill--;
    if(bucket->keys[bucket->fill] == val && bucket->is_leaf)
        bucket->pointer[bucket->fill] = bucket->pointer[bucket->fill + 1];
}

// Function to merge internal node after deletion
void B_plus_tree::merge_internal_node(Node * bucket){
    Node * par = get_parent(root, bucket), *merge;
    int it = -1;

    // find index of the appropriate pointer that stores the given bucket address
    fl(i,0,par->fill + 1){
        if(par->pointer[i] == bucket){
            it = i;
            break;
        }
    }

    // borrow from left sibling if possible
    if(it > 0 && par->pointer[it - 1]->fill > min_order){
        cout << "[INFO] Borrow from left sibling node" << line;
        merge = par->pointer[it - 1];
        insert_into_node(bucket, par->keys[it - 1]);
        par->keys[it - 1] = merge->keys[merge->fill - 1];
        bucket->pointer[0] = merge->pointer[merge->fill];
        delete_from_node(merge, merge->keys[merge->fill - 1]);
    }
    
    // else borrow from right sibling if possible
    else if(it < par->fill && par->pointer[it + 1]->fill > min_order){
        cout << "[INFO] Borrow from right sibling node" << line;
        merge = par->pointer[it + 1];
        insert_into_node(bucket, par->keys[it]);
        par->keys[it] = merge->keys[0];
        bucket->pointer[bucket->fill] = merge->pointer[0];
        delete_from_node(merge, merge->keys[0]);
    }

    // else merge internal nodes via drop down the parent of merging buckets
    else{
        Node *remain;

        // merge with left sibling
        if(it > 0){
            cout << "[INFO] Merge from left sibling node" << line;
            merge = par->pointer[it - 1];
            insert_into_node(merge, par->keys[it - 1]);
            ll contain = merge->fill;
            fl(i,0,bucket->fill)
                insert_into_node(merge, bucket->keys[i]);
            fl(i,0,bucket->fill + 1)
                merge->pointer[contain + i] = bucket->pointer[i];
            delete_from_node(par, par->keys[it - 1]);
            free(bucket);
            remain = merge;
        }

        // merge with right sibling
        else{
            cout << "[INFO] Borrow from right sibling node" << line;
            merge = par->pointer[it + 1];
            insert_into_node(bucket, par->keys[it]);
            ll contain = bucket->fill;
            fl(i,0,merge->fill)
                insert_into_node(bucket, merge->keys[i]);
            fl(i,0,merge->fill + 1)
                bucket->pointer[contain + i] = merge->pointer[i];
            delete_from_node(par, par->keys[it]);
            free(merge);
            remain = bucket;
        }

        // if the parent bucket is root and empty then we assigned root to the merged buckets
        if(par == root){
            if(par->fill)
                return;
            else{
                root = remain;
                cout << "[INFO] Root shifted downwards" << line;
            }
        }

        // else if the parent bucket is underflow then again merge internal node of their parents
        else if(par->fill < min_order)
            merge_internal_node(par);
    }
}

// Function to delete a key from the directory tree
void B_plus_tree::deletion(ll val){

    // find proper leaf that contains the key
    if(root == NULL){
        cout << "[ERROR] Directory is empty" << line;
        return;
    }
    Node * bucket = get_proper_leaf(val);
    ll index = -1;
    fl(i,0,bucket->fill){
        if(bucket->keys[i] > val)
            break;
        index = i;
    }
    // condition for not finding the specified key
    if(index == -1 || (bucket->keys[index] != val)){
        cout << "[Error] Element not found " << line;
        return;
    }

    // if that bucket contain more than minimum key then we simply delete that key
    if(bucket->fill > min_leaf || bucket == root)
        delete_from_node(bucket, val);

    // else try to borrow from siblings or merge with the siblings after deleting that key
    else{
        Node * par = get_parent(root, bucket); // parent of that bucket

        // find pointer which stores the bucket
        auto it = -1; 
        fl(i,0, par->fill + 1){
            if(par->pointer[i] == bucket){
                it = i;
                break;
            }
        }

        // if possiblity of borrowing key from left sibling 
        if(it > 0 && par->pointer[it - 1]->fill > min_leaf){
            cout << "[INFO] Borrow from left sibling leaf nodes" << line;
            delete_from_node(bucket, val);
            insert_into_node(bucket, par->pointer[it - 1]->keys[par->pointer[it - 1]->fill - 1]);
            delete_from_node(par->pointer[it - 1], par->pointer[it - 1]->keys[par->pointer[it - 1]->fill - 1]);
            par->keys[it - 1] = get_maximum_key(par->pointer[it - 1]);
        }

        // else if possiblity of borrowing key from right sibling
        else if(it < par->fill && par->pointer[it + 1]->fill > min_leaf){
            cout << "[INFO] Borrow from right sibling leaf nodes" << line;
            delete_from_node(bucket, val);
            insert_into_node(bucket, par->pointer[it + 1]->keys[0]);
            delete_from_node(par->pointer[it + 1], par->pointer[it + 1]->keys[0]);
            par->keys[it] = get_maximum_key(par->pointer[it]);
        }

        // else we have to merge with siblings
        else{
            Node * remain = NULL;
            // merging with left sibling
            if(it > 0){
                cout << "[INFO] Merge from left sibling leaf nodes" << line;
                delete_from_node(bucket, val);
                Node * merge = par->pointer[it - 1];
                fl(i,0,bucket->fill)
                    insert_into_node(merge, bucket->keys[i]);
                merge->pointer[merge->fill] = bucket->pointer[bucket->fill];
                remain = merge;
                free(bucket);
                delete_from_node(par, par->keys[it - 1]);
            }

            // else merging with right sibling
            else{
                cout << "[INFO] Merge from right sibling leaf nodes" << line;
                delete_from_node(bucket, val);
                Node * merge = par->pointer[it + 1];
                fl(i,0,merge->fill)
                    insert_into_node(bucket, merge->keys[i]);
                bucket->pointer[bucket->fill] = merge->pointer[merge->fill];
                remain = bucket;
                free(merge);
                delete_from_node(par, par->keys[it]);
            }

            // if parent goes underflow after merging
            // if the parent bucket is root and empty then we assigned root to the merged buckets
            if(par == root){
                if(par->fill)
                    return;
                else{
                    root = remain;
                    cout << "[INFO] Root shifted downwards" << line;
                }
            }

            // else if the parent bucket is underflow then again merge internal node of their parents
            else if(par->fill < min_order)
                merge_internal_node(par);
        }
    }
    cout << "[SUCCESS] Node successfully deleted" << line;
    keys.erase(val);
    if(keys.empty()){
        root = NULL;
        cout << "[INFO] Directory is empty and deleted" << line;
        return;
    }
    // update internal node which contain that key in the internal nodes by replacing with maximum from its 
    // left pointer
    Node * update = find_internal_node(val);
    if(update != NULL){
        index = 0;
        fl(i,0,update->fill){
            if(update->keys[i] > val)
                break;
            index = i;
        }
        update->keys[index] = get_maximum_key(update->pointer[index]);
    }
}

// Utility function to print the directory in specified format
void B_plus_tree::print(Node * temp, ll level){
    fl(i,0,temp->fill)
        directory[level].pb(temp->keys[i]);
    directory[level].pb(-1);
    if(temp->is_leaf)
        return;
    fl(i,0,temp->fill + 1)
        print(temp->pointer[i], level + 1);
}

// Function to print internal nodes as well as leaf nodes in designed format
void B_plus_tree::print_directory(){
    if(root == NULL){
        cout << "[ERROR] Directory is empty" << line;
        return;
    }
    directory.clear();
    print(root, 0);
    ll total_height = directory.size();

    // printing internal nodes
    fl(i,0,total_height - 1){
        cout << "Level " << i << ": [ ";
        fl(j,0,directory[i].size() - 1){
            if(directory[i][j] == -1)
                cout << " ] \t [ ";
            else{
                if(j < ((ll)directory[i].size() - 1) && directory[i][j + 1] == -1)
                    cout << directory[i][j];
                else
                    cout << directory[i][j] << " | ";
            }
        }
        cout << " ]" << line;
    }

    // printing leaf nodes
    cout << "Leaf Nodes : ";
    Node * temp = root;
    while(!temp->is_leaf)
        temp = temp->pointer[0];
    while(temp != NULL){
        cout << "[ ";
        fl(i,0,temp->fill){
            if(i == temp->fill - 1)
                cout << temp->keys[i];
            else
                cout << temp->keys[i] << " | ";
        }
        temp = temp->pointer[temp->fill];
        if(temp != NULL)
            cout << " ]  ------> ";
        else
            cout << " ]";
    }
    cout << line << line;
    cout << "[SUCCESS] All nodes printed successfully" << line;
}

// main function
int main(){
    ll order, leaf_order, operation, val;
    cout << "Enter internal-node order and leafnode order: ";

    // taking order of leaf nodes as well as order of internal nodes for the tree
    cin >> order >> leaf_order; 

    // creating one tree directory with given orders
    B_plus_tree tree(order, leaf_order);
    bool result = true;
    while(result){
        cout << "Insertion(1) \t Deletion(2) \t Show Directory(3) \t Exit(Any key apart from these) : ";
        cin >> operation;
        cout << line;

        // Insertion
        if(operation == 1){
            cout << "Enter inserting value : ";
            cin >> val;
            tree.insertion(val);
        }

        // Deletion
        else if(operation == 2){
            cout << "Enter deletion value : ";
            cin >> val;
            tree.deletion(val);
        }

        // Show Directory
        else if(operation == 3)
            tree.print_directory();

        // Exit 
        else{
            cout << "Thank You! \nBye, See you soon!";
            result = false;
        }
        cout << line;
    }
    return 0;