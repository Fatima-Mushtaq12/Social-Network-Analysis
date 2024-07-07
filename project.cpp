#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Define constants
const int MAX_CONNECTIONS = 100;

// UserProfile class to store user information
class UserProfile {
public:
    int userID;
    string name;
    string email;
    string profilePicture;
    int connectionCount;

    UserProfile() : userID(0), connectionCount(0) {}

    UserProfile(int id, const string& name, const string& email, const string& picture)
        : userID(id), name(name), email(email), profilePicture(picture), connectionCount(0) {}
};

// Node structure for LinkedList (used for connections)
template <typename T>
class ListNode {
public:
    T data;
    ListNode<T>* next;
    ListNode(const T& data) : data(data), next(nullptr) {}
};

// LinkedList implementation for managing connections
template <typename T>
class LinkedList {
private:
    ListNode<T>* head;

public:
    LinkedList() : head(nullptr) {}

    void add(const T& data) {
        ListNode<T>* newNode = new ListNode<T>(data);
        if (!head) {
            head = newNode;
        }
        else {
            ListNode<T>* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    bool remove(const T& data) {
        if (!head) return false;
        if (head->data == data) {
            ListNode<T>* temp = head;
            head = head->next;
            delete temp;
            return true;
        }
        ListNode<T>* current = head;
        while (current->next && current->next->data != data) {
            current = current->next;
        }
        if (current->next) {
            ListNode<T>* temp = current->next;
            current->next = current->next->next;
            delete temp;
            return true;
        }
        return false;
    }

    ListNode<T>* search(const T& data) {
        ListNode<T>* current = head;
        while (current) {
            if (current->data == data) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    ListNode<T>* getHead() const {
        return head;
    }

    void display() const {
        ListNode<T>* current = head;
        while (current) {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }
};

// Node structure for Binary Search Tree
template <typename T>
class TreeNode {
public:
    T data;
    TreeNode<T>* left;
    TreeNode<T>* right;
    LinkedList<int> connections;  // LinkedList to manage user connections
    TreeNode(const T& data) : data(data), left(nullptr), right(nullptr) {}
};

// BinarySearchTree implementation to manage UserProfiles
template <typename T>
class BinarySearchTree {
private:
    TreeNode<T>* root;

    void addHelper(TreeNode<T>*& node, const T& data) {
        if (!node) {
            node = new TreeNode<T>(data);
        }
        else if (data.userID < node->data.userID) {
            addHelper(node->left, data);
        }
        else {
            addHelper(node->right, data);
        }
    }

    bool removeHelper(TreeNode<T>*& node, int userID) {
        if (!node) return false;
        if (userID < node->data.userID) {
            return removeHelper(node->left, userID);
        }
        else if (userID > node->data.userID) {
            return removeHelper(node->right, userID);
        }
        else {
            if (!node->left) {
                TreeNode<T>* temp = node;
                node = node->right;
                delete temp;
            }
            else if (!node->right) {
                TreeNode<T>* temp = node;
                node = node->left;
                delete temp;
            }
            else {
                TreeNode<T>* temp = findMin(node->right);
                node->data = temp->data;
                removeHelper(node->right, temp->data.userID);
            }
            return true;
        }
    }

    TreeNode<T>* findMin(TreeNode<T>* node) {
        while (node->left) {
            node = node->left;
        }
        return node;
    }

    TreeNode<T>* searchHelper(TreeNode<T>* node, int userID) {
        if (!node || node->data.userID == userID) {
            return node;
        }
        else if (userID < node->data.userID) {
            return searchHelper(node->left, userID);
        }
        else {
            return searchHelper(node->right, userID);
        }
    }

    void inorderHelper(TreeNode<T>* node) {
        if (node) {
            inorderHelper(node->left);
            cout << "UserID: " << node->data.userID
                << ", Name: " << node->data.name
                << ", Email: " << node->data.email
                << ", Profile Picture: " << node->data.profilePicture
                << ", Connections: " << node->data.connectionCount << "\n";
            cout << "Follows: ";
            node->connections.display(); // Displaying connections for verification
            inorderHelper(node->right);
        }
    }

public:
    BinarySearchTree() : root(nullptr) {}

    void add(const T& data) {
        addHelper(root, data);
    }

    bool remove(int userID) {
        return removeHelper(root, userID);
    }

    TreeNode<T>* search(int userID) {
        return searchHelper(root, userID);
    }

    void inorder() {
        inorderHelper(root);
    }

    TreeNode<T>* getRoot() const {
        return root;
    }
};

// Helper function to perform in-order traversal and save data
void inorderSave(ofstream& outFile, TreeNode<UserProfile>* node) {
    if (node) {
        inorderSave(outFile, node->left);
        outFile << node->data.userID << " "
            << node->data.name << " "
            << node->data.email << " "
            << node->data.profilePicture << " "
            << node->data.connectionCount << "\n";
        inorderSave(outFile, node->right);
    }
}

// Function to save user profiles to a file
void saveToFile(const string& filename, const BinarySearchTree<UserProfile>& userList) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        inorderSave(outFile, userList.getRoot());
        outFile.close();
    }
    else {
        cout << "Error: Unable to open file for saving.\n";
    }
}

// Function to load user profiles from a file
void loadFromFile(const string& filename, BinarySearchTree<UserProfile>& userList) {
    ifstream inFile(filename);
    if (inFile.is_open()) {
        int userID;
        string name, email, profilePicture;
        int connectionCount;
        while (inFile >> userID >> name >> email >> profilePicture >> connectionCount) {
            UserProfile user(userID, name, email, profilePicture);
            user.connectionCount = connectionCount;
            userList.add(user);
        }
        inFile.close();
    }
    else {
        cout << "Error: Unable to open file for loading.\n";
    }
}

int main() {
    BinarySearchTree<UserProfile> userList;

    // Load user profiles from file
    loadFromFile("users.txt", userList);

    while (true) {
        cout << "1. Add User\n2. Remove User\n3. Search User\n4. Display Users\n5. Follow User\n6. Unfollow User\n7. Post Update\n8. Exit\n";
        int choice;
        cin >> choice;

        if (choice == 1) {
            int userID;
            string name, email, picture;
            cout << "Enter UserID: ";
            cin >> userID;
            cout << "Enter name: ";
            cin >> name;
            cout << "Enter email: ";
            cin >> email;
            cout << "Enter profile picture: ";
            cin >> picture;
            userList.add(UserProfile(userID, name, email, picture));
            saveToFile("users.txt", userList);

        }
        else if (choice == 2) {
            int userID;
            cout << "Enter UserID to remove: ";
            cin >> userID;
            if (userList.remove(userID)) {
                saveToFile("users.txt", userList);
            }
            else {
                cout << "User not found.\n";
            }

        }
        else if (choice == 3) {
            int userID;
            cout << "Enter UserID to search: ";
            cin >> userID;
            TreeNode<UserProfile>* user = userList.search(userID);
            if (user) {
                cout << "User found: " << user->data.name << " (" << user->data.email << ")\n";
            }
            else {
                cout << "User not found.\n";
            }

        }
        else if (choice == 4) {
            userList.inorder();

        }
        else if (choice == 5) {
            int userID, followID;
            cout << "Enter your UserID: ";
            cin >> userID;
            cout << "Enter UserID to follow: ";
            cin >> followID;
            TreeNode<UserProfile>* user = userList.search(userID);
            if (user) {
                user->connections.add(followID);
                user->data.connectionCount++;  // Increment connection count
                saveToFile("users.txt", userList);
                cout << "Following UserID: " << followID << "\n";
            }
            else {
                cout << "User not found.\n";
            }

        }
        else if (choice == 6) {
            int userID, unfollowID;
            cout << "Enter your UserID: ";
            cin >> userID;
            cout << "Enter UserID to unfollow: ";
            cin >> unfollowID;
            TreeNode<UserProfile>* user = userList.search(userID);
            if (user) {
                if (user->connections.remove(unfollowID)) {
                    user->data.connectionCount--;
                    saveToFile("users.txt", userList);
                    cout << "Unfollowed UserID: " << unfollowID << "\n";
                }
                else {
                    cout << "Not following UserID: " << unfollowID << "\n";
                }
            }
            else {
                cout << "User not found.\n";
            }

        }
        else if (choice == 7) {
            int userID;
            string update;
            cout << "Enter your UserID: ";
            cin >> userID;
            cout << "Enter update: ";
            cin.ignore(); // Clear newline character from buffer
            getline(cin, update);
            // Here, you can store the update in a list or file associated with the user
            cout << "Update posted: " << update << "\n";

        }
        else if (choice == 8) {
            break;

        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}
