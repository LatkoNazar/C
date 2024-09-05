//   Latko Nazar KM-32
//   2-3 Tree implementionan
//   Last edit 19.05.2024 (19:31)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// list to trace busy ids
int id_list[1000];
int id_list_len = 0;


typedef struct date_of_birth{
    int year;
    int month;
    int day;
} date;


typedef enum gender{
    MALE, FEMALE
} gender;


typedef struct student_info {
    int id;
    float mark;
    char *first_name;
    char *patronimic;
    char *surname;
    date date_of_birth;
    char *group;
    gender gender;
} student_info;


typedef struct node {
    int size;      
    student_info key[3];    
    struct node *first;  
    struct node *second; 
    struct node *third;  
    struct node *fourth; 
    struct node *parent; 
} Node;



Node *split(Node *item);
Node *insert(Node *p, student_info *k);
Node *search(Node *p, int k);
Node *search_min(Node *p);
Node *merge(Node *leaf);
Node *redistribute(Node *leaf);
Node *fix(Node *leaf);
int index_det(Node *node, int id);
Node *delete_student(Node *root, int id);
Node *remove_el(Node *p, student_info *k);
Node *delete_func(Node *node, int id);


Node *create_node(student_info *k) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_node->size = 1;
    new_node->key[0] = *k;
    new_node->first = NULL;
    new_node->second = NULL;
    new_node->third = NULL;
    new_node->fourth = NULL;
    new_node->parent = NULL;
    return new_node;
}


bool find(Node *node, int k) { 
    for (int i = 0; i < node->size; ++i)
        if (node->key[i].id == k) return true;
    return false;
}

void swap(student_info *x, student_info *y) {
    student_info r = *x;
    *x = *y;
    *y = r;
}

// sort 3 node
void sort2(student_info *x, student_info *y) {   
    if (x->id > y->id) swap(x, y);
}

// sort 4 node
void sort3(student_info *x, student_info *y, student_info *z) {
    if (x->id > y->id) swap(x, y);
    if (x->id > z->id) swap(x, z);
    if (y->id > z->id) swap(y, z);
}

void sort(Node *node) { 
    if (node->size == 1) return;
    if (node->size == 2) sort2(&(node->key[0]), &(node->key[1]));
    if (node->size == 3) sort3(&(node->key[0]), &(node->key[1]), &(node->key[2]));
}


void insert_to_node(Node *node, student_info *k) {  
        node->key[node->size] = *k;
        node->size++;
        sort(node);
    }

void remove_from_node(Node *node, student_info *k) { // Delete key from k node
    if (node->size >= 1 && node->key[0].id == k->id) {
        node->key[0] = node->key[1];
        node->key[1] = node->key[2];
        node->size--;
    } else if (node->size == 2 && node->key[1].id == k->id) {
        node->key[1] = node->key[2];
        node->size--;
    }
}

void become_node2(Node *node, student_info *k, Node *first, Node *second) { 
        node->key[0] = *k;
        node->first = first;
        node->second = second;
        node->third = NULL;
        node->fourth = NULL;
        node->parent = NULL;
        node->size = 1;
    }

// check if leaf
bool is_leaf(Node *node) {
    return (node->first == NULL) && (node->second == NULL) && (node->third == NULL) && (node->fourth == NULL);
}

// inserting key k into tree rooted p; We always return the root of the tree, because it can change
// return root because it can be changed
Node *insert(Node *p, student_info *k) { 
    if (!p) return create_node(k);

    if (is_leaf(p)) insert_to_node(p, k);
    else if (k->id <= p->key[0].id) insert(p->first, k);
    else if ((p->size == 1) || ((p->size == 2) && k->id <= p->key[1].id)) insert(p->second, k);
    else insert(p->third, k);

    return split(p);
}


Node *split(Node *item) {
    if (item->size < 3) return item;

    // Create new nodes with appropriate keys
    Node *x = create_node(&(item->key[0]));
    Node *y = create_node(&(item->key[2]));

    // Move child's nodes
    x->first = item->first;
    x->second = item->second;
    x->parent = item->parent;

    y->first = item->third;
    y->second = item->fourth;
    y->parent = item->parent;

    // Update parent link on child's node
    if (x->first)  x->first->parent = x;
    if (x->second) x->second->parent = x;
    if (y->first)  y->first->parent = y;
    if (y->second) y->second->parent = y;

    if (item->parent) {
        // Paste key in parent node
        insert_to_node(item->parent, &(item->key[1]));

        // Find and delete current node link
        if (item->parent->first == item) {
            item->parent->first = NULL;
        } else if (item->parent->second == item) {
            item->parent->second = NULL;
        } else if (item->parent->third == item) {
            item->parent->third = NULL;
        }

        // Sortion
        if (item->parent->first == NULL) {
            item->parent->fourth = item->parent->third;
            item->parent->third = item->parent->second;
            item->parent->second = y;
            item->parent->first = x;
        } else if (item->parent->second == NULL) {
            item->parent->fourth = item->parent->third;
            item->parent->third = y;
            item->parent->second = x;
        } else {
            item->parent->fourth = y;
            item->parent->third = x;
        }

        Node *tmp = item->parent;
        free(item);
        return tmp;
    } else {
        // If node was root, create new root node
        become_node2(item, &(item->key[1]), x, y);
        x->parent = item;
        y->parent = item;
        return item;
    }
}





// Search for key k in a 2-3 tree with root p.
Node *search(Node *p, int k) {
    if (!p) return NULL;

    if (find(p, k)) return p;
    else if (k < p->key[0].id) return search(p->first, k);
    else if ((p->size == 2) && (k < p->key[1].id) || (p->size == 1)) return search(p->second, k);
    else if (p->size == 2) return search(p->third, k);
}

// Finding the node with the minimum elements in a 2-3 tree rooted p.
// Min element in right subtree
Node *search_min(Node *p) {
    if (!p) return p;
    if (!(p->first)) return p;
    else return search_min(p->first);
}

// Deleting key k in a 2-3 tree rooted p.
Node *remove_el(Node *p, student_info *k) {
    Node *item = search(p, k->id);
    student_info tmp = *k;
    if (!item) return p;

    Node *min = NULL;
    if (item->key[0].id == k->id) min = search_min(item->second); 
    else min = search_min(item->third);

    if (min) {
        student_info *z = (k->id == item->key[0].id ? &item->key[0] : &item->key[1]);
        swap(z, &(min->key[0]));
        item = min; 
    }

    remove_from_node(item, &tmp);
    return fix(item);
}




Node *delete_student(Node *root, int id) {
    root = delete_func(root, id);
    return root;
}

// Function to delete student
// Use most often
Node *delete_func(Node *node, int id) {
    Node *node_to_delete = search(node, id);

    if (node_to_delete != NULL) {
        printf("Found node. Removing...\n");
        int i = index_det(node_to_delete, id);
        node = remove_el(node, &(node_to_delete->key[i]));
    } else {
        printf("Node is not found.\n");
    }
    return node; 
}

// Redistribute - algorithm for restoring properties of a 2-3-tree
Node *redistribute(Node *leaf) {
    Node *parent = leaf->parent;
    Node *first = parent->first;
    Node *second = parent->second;
    Node *third = parent->third;

    if ((parent->size == 2) && (first->size < 2) && (second->size < 2) && (third->size < 2)) {
        if (first == leaf) {
            parent->first = parent->second;
            parent->second = parent->third;
            parent->third = NULL;
            insert_to_node(parent->first, &(parent->key[0]));
            parent->first->third = parent->first->second;
            parent->first->second = parent->first->first;

            if (leaf->first != NULL) parent->first->first = leaf->first;
            else if (leaf->second != NULL) parent->first->first = leaf->second;

            if (parent->first->first != NULL) parent->first->first->parent = parent->first;

            remove_from_node(parent, &(parent->key[0]));
            free(first);
        } else if (second == leaf) {
            insert_to_node(first, &(parent->key[0]));
            remove_from_node(parent, &(parent->key[0]));
            if (leaf->first != NULL) first->third = leaf->first;
            else if (leaf->second != NULL) first->third = leaf->second;

            if (first->third != NULL) first->third->parent = first;

            parent->second = parent->third;
            parent->third = NULL;

            free(second);
        } else if (third == leaf) {
            insert_to_node(second, &(parent->key[1]));
            parent->third = NULL;
            remove_from_node(parent, &(parent->key[1]));
            if (leaf->first != NULL) second->third = leaf->first;
            else if (leaf->second != NULL) second->third = leaf->second;

            if (second->third != NULL)  second->third->parent = second;

            free(third);
        }
    } else if ((parent->size == 2) && ((first->size == 2) || (second->size == 2) || (third->size == 2))) {
        if (third == leaf) {
            if (leaf->first != NULL) {
                leaf->second = leaf->first;
                leaf->first = NULL;
            }

            insert_to_node(leaf, &(parent->key[1]));
            if (second->size == 2) {
                parent->key[1] = second->key[1];
                remove_from_node(second, &(second->key[1]));
                leaf->first = second->third;
                second->third = NULL;
                if (leaf->first != NULL) leaf->first->parent = leaf;
            } else if (first->size == 2) {
                parent->key[1] = second->key[0];
                leaf->first = second->second;
                second->second = second->first;
                if (leaf->first != NULL) leaf->first->parent = leaf;

                second->key[0] = parent->key[0];
                parent->key[0] = first->key[1];
                remove_from_node(first, &(first->key[1]));
                second->first = first->third;
                if (second->first != NULL) second->first->parent = second;
                first->third = NULL;
            }
        } else if (second == leaf) {
            if (third->size == 2) {
                if (leaf->first == NULL) {
                    leaf->first = leaf->second;
                    leaf->second = NULL;
                }
                insert_to_node(second, &(parent->key[1]));
                parent->key[1] = third->key[0];
                remove_from_node(third, &(third->key[0]));
                second->second = third->first;
                if (second->second != NULL) second->second->parent = second;
                third->first = third->second;
                third->second = third->third;
                third->third = NULL;
            } else if (first->size == 2) {
                if (leaf->second == NULL) {
                    leaf->second = leaf->first;
                    leaf->first = NULL;
                }
                insert_to_node(second, &(parent->key[0]));
                parent->key[0] = first->key[1];
                remove_from_node(first, &(first->key[1]));
                second->first = first->third;
                if (second->first != NULL) second->first->parent = second;
                first->third = NULL;
            }
        } else if (first == leaf) {
            if (leaf->first == NULL) {
                leaf->first = leaf->second;
                leaf->second = NULL;
            }
            insert_to_node(first, &(parent->key[0]));
            if (second->size == 2) {
                parent->key[0] = second->key[0];
                remove_from_node(second, &(second->key[0]));
                first->second = second->first;
                if (first->second != NULL) first->second->parent = first;
                second->first = second->second;
                second->second = second->third;
                second->third = NULL;
            } else if (third->size == 2) {
                parent->key[0] = second->key[0];
                second->key[0] = parent->key[1];
                parent->key[1] = third->key[0];
                remove_from_node(third, &(third->key[0]));
                first->second = second->first;
                if (first->second != NULL) first->second->parent = first;
                second->first = second->second;
                second->second = third->first;
                if (second->second != NULL) second->second->parent = second;
                third->first = third->second;
                third->second = third->third;
                third->third = NULL;
            }
        }
    } else if (parent->size == 1) {
        insert_to_node(leaf, &(parent->key[0]));

        if (first == leaf && second->size == 2) {
            parent->key[0] = second->key[0];
            remove_from_node(second, &(second->key[0]));

            if (leaf->first == NULL) leaf->first = leaf->second;

            leaf->second = second->first;
            second->first = second->second;
            second->second = second->third;
            second->third = NULL;
            if (leaf->second != NULL) leaf->second->parent = leaf;
        } else if (second == leaf && first->size == 2) {
            parent->key[0] = first->key[1];
            remove_from_node(first, &(first->key[1]));

            if (leaf->second == NULL) leaf->second = leaf->first;

            leaf->first = first->third;
            first->third = NULL;
            if (leaf->first != NULL) leaf->first->parent = leaf;
        }
    }
    return parent;
}


Node *fix(Node *leaf) {
    // Case when we delete the only key in the tree
    if (leaf->size == 0 && leaf->parent == NULL) {
        free(leaf);
        return NULL;
    }
    // Case 1, when the vertex in which the key was deleted had two keys
    if (leaf->size != 0) {
        if (leaf->parent) return fix(leaf->parent);
        else return leaf;
    }

    Node *parent = leaf->parent;
    if (parent->first->size == 2 || parent->second->size == 2 || parent->size == 2) leaf = redistribute(leaf);
    else if (parent->size == 2 && parent->third->size == 2) leaf = redistribute(leaf);
    else leaf = merge(leaf);

    return fix(leaf);
}


Node *merge(Node *leaf) {
    Node *parent = leaf->parent;

    if (parent->first == leaf) {
        insert_to_node(parent->second, &(parent->key[0]));
        parent->second->third = parent->second->second;
        parent->second->second = parent->second->first;

        if (leaf->first != NULL) parent->second->first = leaf->first;
        else if (leaf->second != NULL) parent->second->first = leaf->second;

        if (parent->second->first != NULL) parent->second->first->parent = parent->second;

        remove_from_node(parent, &(parent->key[0]));
        free(parent->first);
        parent->first = NULL;

    } else if (parent->second == leaf) {
        insert_to_node(parent->first, &(parent->key[0]));

        if (leaf->first != NULL) parent->first->third = leaf->first;
        else if (leaf->second != NULL) parent->first->third = leaf->second;

        if (parent->first->third != NULL) parent->first->third->parent = parent->first;

        remove_from_node(parent, &(parent->key[0]));
        free(parent->second);
        parent->second = NULL;

    } else if (parent->third == leaf) {
        insert_to_node(parent->second, &(parent->key[1]));
        parent->second->third = parent->second->second;
        parent->second->second = parent->second->first;

        if (leaf->first != NULL) parent->second->first = leaf->first;
        else if (leaf->second != NULL) parent->second->first = leaf->second;

        if (parent->second->first != NULL) parent->second->first->parent = parent->second;

        remove_from_node(parent, &(parent->key[1]));
        free(parent->third);
        parent->third = NULL;
    }
    sort(parent);
    
    if (parent->parent == NULL) {
        Node *tmp = NULL;
        if (parent->first != NULL) tmp = parent->first;
        else if (parent->second != NULL) tmp = parent->second;
        else if (parent->third != NULL) tmp = parent->third;
        tmp->parent = NULL;
        free(parent);
        return tmp;
    }
    return parent;
}


// Function to print the tree (Prints only ids)
// Prints elements like this:
/*
    Example

    20 30 
            10 (left child)
                            1 (left child)
                            12 (right child)
            25 (mid child)
                            24 (left child)
                            26 (right child)
            45 (right child)
                            43 (left child)
                            47 (right child)

*/
void recursivePrint(Node *node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
    for (int i = 0; i < node->size; i++) {
        printf("%d ", node->key[i].id);
    }
    printf("\n");

    if (is_leaf(node)) {
        return;
    }

    if (node->size == 1) {
        recursivePrint(node->first, depth + 1);
        recursivePrint(node->second, depth + 1);
    } else if (node->size == 2) {
        recursivePrint(node->first, depth + 1);
        recursivePrint(node->second, depth + 1);
        recursivePrint(node->third, depth + 1);
    } else if (node->size == 3) {
        recursivePrint(node->first, depth + 1);
        recursivePrint(node->second, depth + 1);
        recursivePrint(node->third, depth + 1);
        recursivePrint(node->fourth, depth + 1);
    }
}

// Gets height of the tree
int getLevelCount(Node *node)
{
    if (node == NULL)
    {
        return 0;
    }
    int leftMaxLevel = 1 + getLevelCount(node->first);
    return leftMaxLevel;
}


int index_det(Node *node, int id) {
        for (int i = 0; i < 3; i++) {
            if (node->key[i].id == id) {
                return i;
            }
        }
}


// Check int type
int is_int_type(char *input) {
  int i = 0;
  int number;

  while (input[i] >= '0' && input[i] <= '9') {
    i++;
  }
  if (input[i] != '\0')
    return -1000;
  else if (strlen(input) == 1 && input[0] == '\n')
    return -1000;
  else {
    number = atoi(input);
    return number;
  }
}

// Function for students' data input
void char_input(char **name) {
    char *input;
    input = malloc(100 * sizeof(char));
    scanf("%s", input);
    *name = malloc((strlen(input) + 1) * sizeof(char));
    if (*name == NULL) {
        printf("Problem with memory!!!");
        free(name);
        free(input);
        return;
    }
    strcpy(*name, input);
    free(input);
    while(getchar() != '\n');
}


int int_input() {
    char *task_number;
    int got_number;
    task_number = malloc(100 * sizeof(char));
    fgets(task_number, sizeof(task_number), stdin);
    task_number[strcspn(task_number, "\n")] = '\0';
    got_number = is_int_type(task_number);
    free(task_number);
    return got_number;
}

// Check if id is busy
int check_id_list(int id) {
    for(int i = 0; i < 1000; i++) {
        if (id_list[i] == id) return -1;
    }
    return 0;
}


int id_input() {
    int id;
    while(true) {
        printf("Id: ");
        id = int_input();
        if (check_id_list(id) == 0) break;
        else printf("This id is not available now! Choose another one\n");
    }
    return id;
}


float float_inputing() {
  char value[50];
  char *endptr;
  float result;

  while (true) {
    printf("Enter student's rating score: ");
    fgets(value, sizeof(value), stdin);
    result = strtof(value, &endptr);
    if (endptr == value || *endptr != '\n') {
      printf("Invalid input! Please enter a valid floating-point number.\n");
      continue;
    } else
      break;
  }
  return result;
}


student_info *data_input() {
    student_info *new_student = malloc(sizeof(student_info));
    char *letter;

    printf("Please enter student's first name: ");
    char_input(&(new_student->first_name));
    printf("Please enter student's patronimic: ");
    char_input(&(new_student->patronimic));
    printf("Please enter student's sursname: ");
    char_input(&(new_student->surname));
    printf("Please enter student's date of birth:\n");

    new_student->id = id_input();
    id_list[id_list_len] = new_student->id;
    id_list_len++;

    while (true) {
        printf("Year: ");
        new_student->date_of_birth.year = int_input();
        if (new_student->date_of_birth.year != -1000) break;
        else printf("Invalid input!\n");
    }

    while (true) {
        printf("Month: ");
        new_student->date_of_birth.month = int_input();
        if (new_student->date_of_birth.month != -1000) break;
        else printf("Invalid input!\n");
    }

    while (true) {
        printf("Day: ");
        new_student->date_of_birth.day = int_input();
        if (new_student->date_of_birth.day != -1000) break;
        else printf("Invalid input!\n");
    }

    printf("Please enter student's group: ");
    char_input(&(new_student->group));
    new_student->mark = float_inputing();
    while(true) {

      printf("Please enter student's gender (M/m) or (F/f): "); 
      char_input(&letter);
      if ((strcmp(letter, "M") == 0) || (strcmp(letter, "m") == 0)) {
        new_student->gender = MALE;
        free(letter);
        break;
      }
      if ((strcmp(letter, "F") == 0 )  || (strcmp(letter, "f") == 0)) {
        new_student->gender = FEMALE;
        free(letter);
        break;
      }
      else printf("Invalid input!\n");
      free(letter);
    }
    
    return new_student;
}


int find_place(Node *node, int id) {
    Node *found = search(node, id);
    if (found) {
        int index = index_det(found, id);
        return index;
    }
}


// Write each field to the data.bin file
void write_to_binary_file(student_info *info, char *filename) {
    FILE *file;
    file = fopen(filename, "ab"); 

    if (file == NULL) {
        printf("Error opening file. Creating a new file.\n");
        file = fopen(filename, "wb"); 
    }
        int len;
        len = strlen(info->first_name) + 1;
        fwrite(&len, sizeof(int), 1, file);
        fwrite(info->first_name, sizeof(char), len, file);

        len = strlen(info->patronimic) + 1;
        fwrite(&len, sizeof(int), 1, file);
        fwrite(info->patronimic, sizeof(char), len, file);

        len = strlen(info->surname) + 1;
        fwrite(&len, sizeof(int), 1, file);
        fwrite(info->surname, sizeof(char), len, file);

        fwrite(&(info->date_of_birth), sizeof(date), 1, file);
        fwrite(&(info->date_of_birth.year), sizeof(int), 1, file);
        fwrite(&(info->date_of_birth.month), sizeof(int), 1, file);
        fwrite(&(info->date_of_birth.day), sizeof(int), 1, file);

        len = strlen(info->group) + 1;
        fwrite(&len, sizeof(int), 1, file);
        fwrite(info->group, sizeof(char), len, file);

        fwrite(&(info->mark), sizeof(float), 1, file);
        fwrite(&(info->id), sizeof(int), 1, file);

        fwrite(&(info->gender), sizeof(gender), 1, file);
    fclose(file);
}

// Function to add student in the tree
Node *add_student(Node *root, char file_name[]) {
    student_info *s1 = data_input();
    root = insert(root, s1);
    Node *found = search(root, s1->id);
    int place = find_place(found, s1->id);
    write_to_binary_file(&(found->key[place]), file_name);
    return root;
}


// Find student and print out data 
void find_student(Node *node, int id) {
    Node *found = search(node, id);
    if (found) {
        int index = index_det(found, id);

        printf("Student is found:\n");
        printf("%s %s %s\n", found->key[index].surname, found->key[index].first_name, found->key[index].patronimic);
        printf("Student's id: %d\n", found->key[index].id);
        printf("Date of birth: %d %d %d\n", found->key[index].date_of_birth.day, found->key[index].date_of_birth.month, found->key[index].date_of_birth.year);
        printf("Group: %s\n", found->key[index].group);
        printf("Rating score: %.2f\n", found->key[index].mark);
        printf("Gender: %s\n", (found->key[index].gender == MALE ? "Male" : "Female"));
    }
    else printf("Not found!\n");
}


// Read each field from data.bin file
Node *read_from_binary_file(Node *root, char filename[]) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("No file. Creating...\n");
        return NULL;
    }
    student_info *current = NULL;
    int len;
    while (fread(&len, sizeof(int), 1, file) == 1) {
        current = malloc(sizeof(student_info));
        if (current == NULL) {
            printf("Memory allocation failed!\n");
            break;
        }

        current->first_name = malloc(sizeof(char) * len);
        fread(current->first_name, sizeof(char), len, file);
        
        fread(&len, sizeof(int), 1, file);
        current->patronimic = malloc(sizeof(char) * len);
        fread(current->patronimic, sizeof(char), len, file);

        fread(&len, sizeof(int), 1, file);
        current->surname = malloc(sizeof(char) * len);
        fread(current->surname, sizeof(char), len, file);

        fread(&(current->date_of_birth), sizeof(date), 1, file);
        fread(&(current->date_of_birth.year), sizeof(int), 1, file);
        fread(&(current->date_of_birth.month), sizeof(int), 1, file);
        fread(&(current->date_of_birth.day), sizeof(int), 1, file);

        fread(&len, sizeof(int), 1, file);
        current->group = malloc(sizeof(char) * len);
        fread(current->group, sizeof(char), len, file);

        fread(&(current->mark), sizeof(float), 1, file);
        fread(&(current->id), sizeof(int), 1, file);
        id_list[id_list_len] = current->id;
        id_list_len++;

        fread(&(current->gender), sizeof(gender), 1, file);
        root = insert(root, current);
    }
    fclose(file);
    return root;
}


// Rewrite data to save current tree, because deletion may be
void recursiveREwrite(Node *node, int depth, char filename[]) {
    if (node == NULL) return;
    for (int i = 0; i < node->size; i++) {
        write_to_binary_file(&(node->key[i]), filename);
    }

    if (is_leaf(node)) {
        return;
    }

    if (node->size == 1) {
        recursiveREwrite(node->first, depth + 1, filename);
        recursiveREwrite(node->second, depth + 1, filename);

    } else if (node->size == 2) {
        recursiveREwrite(node->first, depth + 1, filename);
        recursiveREwrite(node->second, depth + 1, filename);
        recursiveREwrite(node->third, depth + 1, filename);

    } else if (node->size == 3) {
        recursiveREwrite(node->first, depth + 1, filename);
        recursiveREwrite(node->second, depth + 1, filename);
        recursiveREwrite(node->third, depth + 1, filename);
        recursiveREwrite(node->fourth, depth + 1, filename);
    }
}


// Print out all found students by criterion
void PrintAllFound(Node *node, int mode, char *keyword, int depth, int *found) {
    if (node == NULL) return;
    for (int i = 0; i < node->size; i++) {
        int match;
        if (mode == 2) match = strcmp(keyword, node->key[i].first_name);
        else if (mode == 3) match = strcmp(keyword, node->key[i].surname);
        else if (mode == 4) match = strcmp(keyword, node->key[i].group);
        if (match == 0) {
            (*found)++;
            printf("Student is found:\n");
            printf("%s %s %s\n", node->key[i].surname, node->key[i].first_name, node->key[i].patronimic);
            printf("Student's id: %d\n", node->key[i].id);
            printf("Date of birth: %d %d %d\n", node->key[i].date_of_birth.day, node->key[i].date_of_birth.month, node->key[i].date_of_birth.year);
            printf("Group: %s\n", node->key[i].group);
            printf("Rating score: %.2f\n", node->key[i].mark);
            printf("Gender: %s\n", (node->key[i].gender == MALE ? "Male" : "Female"));
            printf("\n");
        }
    }

    if (is_leaf(node)) {
        return;
    }

    if (node->size == 1) {
        PrintAllFound(node->first, mode, keyword, depth + 1, found);
        PrintAllFound(node->second, mode, keyword, depth + 1, found);
    } else if (node->size == 2) {
        PrintAllFound(node->first, mode, keyword, depth + 1, found);
        PrintAllFound(node->second, mode, keyword, depth + 1, found);
        PrintAllFound(node->third, mode, keyword, depth + 1, found);
    } else if (node->size == 3) {
        PrintAllFound(node->first, mode, keyword, depth + 1, found);
        PrintAllFound(node->second, mode, keyword, depth + 1, found);
        PrintAllFound(node->third, mode, keyword, depth + 1, found);
        PrintAllFound(node->fourth, mode, keyword, depth + 1, found);
    }
}


int main() {
    int last_id = 0;
    char file_name[] = "data.bin";
    Node *root = NULL;
    FILE *data_file;
    char task_number[50];
    char id_num[50];
    int i;
    int got_number;
    root = read_from_binary_file(root, file_name);
    if (root == NULL) {
        data_file = fopen(file_name, "ab");
        fclose(data_file);
        root = NULL;
        root = read_from_binary_file(root, file_name);
    }
    printf("Student record keeping program\n");
    while (true) {
        printf("Please, select an option\n");
        printf("1) Add student\n"
            "2) Find student\n"
            "3) Delete student\n"
            "4) Print tree (id only)\n"
            "6) Exit\n");
        printf("Enter the option number: ");

        got_number = int_input();
        if (got_number == -1000)
        printf("Incorrect input!!!!!!!\n");
        else {
        if (got_number == 1) {
            root = add_student(root, file_name);
        }
        else if (got_number == 2) {
            int mode;
            while (true) {
                printf("1) Search by id - 1\n"
                       "2) Search by name - 2\n"
                       "3) Search by surname - 3\n"
                       "4) Search by group - 4\n");
                       
                printf("Enter the option number: ");
                mode = int_input();
                if (mode != -1000 && (mode == 1 || mode == 2 || mode == 3 || mode == 4)) break;
                else printf("Invalid input!\n");
            }
            if (mode == 1) {
                int id;
                while (true) {
                    printf("Enter the student's id: ");
                    id = int_input();
                    if (id > 0) break;
                }
                printf("\n");
                find_student(root, id);
            }
            else if(mode != 1) {
                int found = 0;
                char *keyword;
                printf("Enter keyword: ");
                char_input(&keyword);
                int lvl = getLevelCount(root);
                PrintAllFound(root, mode, keyword, lvl, &found);
                if (found == 0) printf("Not found!!!\n");
                free(keyword);
            }
        }
        else if (got_number == 3) {
            int id_to_del;
            while (true) {
                printf("Enter the student's id: ");
                id_to_del = int_input();
                if (id_to_del > 0) break;
            }
            root = delete_func(root, id_to_del);
        }
        else if (got_number == 4) {
            int depth = getLevelCount(root);
            recursivePrint(root, depth);
        }

        else if (got_number == 6) {
            FILE *file;
            file = fopen(file_name, "wb");
            fclose(file);
            int level = getLevelCount(root);
            recursiveREwrite(root, level, file_name);
            printf("Thanks for using my program!\n"
                   "Author: Latko Nazar\n");
            break;
        } 
        else
            printf("Incorrect input!\n");
        }
    printf("\n");
  }
  return 0;
}