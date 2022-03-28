/**
 * comp2017 - assignment 2
 * <your name>
 * <your unikey>
 */

#include "ymirdb.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


// Global Variables
int num_words = 0;
char **input = NULL;
entry *current_entries = NULL;
snapshot *stored_snapshots = NULL;

//Utility functions
int num_words_in_line(char *line);
char **get_words(char *line, int *num_words);
char is_integer(char *str);
int atoi2(char *str) ;

// General commands
void bye();
void help();
void command_invalid();
void list_keys();
void list_entries();
void list_snapshots();

// Main Commands
void get();
char del(entry *entries);
void purge();

void set();
void push();
void append();

void pick();
void pluck();
void pop();

void drop();

void checkout();
void snapsh(); //snapshot command

void min();
void max();
void sum();
void len();

void rev();
void uniq();
void sort();

void forward();
void backward();
void type();

int main(void) {
    char line[MAX_LINE];

    while (true) {
        printf("> ");

        if (NULL == fgets(line, MAX_LINE, stdin)) {
            printf("bye\n");
            return 0;
        }

        num_words = 0;
        input = get_words(line, &num_words);

        if (strcasecmp(input[0], "LIST") == 0) {
            if (strcasecmp(input[1], "KEYS") == 0) 
                list_keys();
            else if (strcasecmp(input[1], "ENTRIES") == 0)
                list_entries();
            else if (strcasecmp(input[1], "SNAPSHOTS") == 0)
                list_snapshots();
            else
                printf("Invalid command\n\n");

        } else if (strcasecmp(input[0], "SET") == 0) {
            set();
            printf("\n");

        } else if (strcasecmp(input[0], "GET") == 0) {
            get();
            printf("\n");

        } else if (strcasecmp(input[0], "DEL") == 0) {
            char res = del(current_entries);
            if (!res)
                printf("ok\n\n");
            else if (res == 2)
                printf("no such key\n\n");
            else if (res == 3)
                printf("not permitted\n\n");

        } else if (strcasecmp(input[0], "PURGE") == 0) {
            purge();
            printf("\n");

        } else if (strcasecmp(input[0], "PUSH") == 0) {
            push();
            printf("\n");

        } else if (strcasecmp(input[0], "APPEND") == 0) {
            append();
            printf("\n");

        } else if (strcasecmp(input[0], "PICK") == 0) {
            pick();
            printf("\n");

        } else if (strcasecmp(input[0], "PLUCK") == 0) {
            pluck();
            printf("\n");

        } else if (strcasecmp(input[0], "POP") == 0) {
            pop();
            printf("\n");

        } else if (strcasecmp(input[0], "SUM") == 0) {
            sum();
            printf("\n");

        } else if (strcasecmp(input[0], "MIN") == 0) {
            min();
            printf("\n");

        } else if (strcasecmp(input[0], "MAX") == 0) {
            max();
            printf("\n");

        } else if (strcasecmp(input[0], "LEN") == 0) {
            len();
            printf("\n");

        } else if (strcasecmp(input[0], "REV") == 0) {
            rev();
            printf("\n");

        } else if (strcasecmp(input[0], "UNIQ") == 0) {
            uniq();
            printf("\n");

        } else if (strcasecmp(input[0], "TYPE") == 0) {
            type();
            printf("\n");

        } else if (strcasecmp(input[0], "SORT") == 0) {
            sort();
            printf("\n");

        } else if (strcasecmp(input[0], "FORWARD") == 0) {
            forward();
            printf("\n");

        } else if (strcasecmp(input[0], "BACKWARD") == 0) {
            backward();
            printf("\n");

        } else if (strcasecmp(input[0], "DROP") == 0) {
            drop();
            printf("\n");

        } else if (strcasecmp(input[0], "CHECKOUT") == 0) {
            checkout();
            printf("\n");

        } else if (strcasecmp(input[0], "SNAPSHOT") == 0) {
            snapsh();
            printf("\n");

        } else if (strcasecmp(input[0], "ROLLBACK") == 0) {
        } else if (strcasecmp(input[0], "CHECKOUT") == 0) {
        } else if (strcasecmp(input[0], "DROP") == 0) {
        } else if (strcasecmp(input[0], "HELP") == 0) {
            help();
            printf("\n");

        } else if (strcasecmp(input[0], "BYE") == 0) {
            //Free memory allocated for input
            for (int i = 0; i < num_words; ++i)
                free(input[i]);

            free(input);
            bye();
            break;
        } else {
            command_invalid();
            printf("\n");
        }

        for (int i = 0; i < num_words; ++i)
            free(input[i]);

        free(input);
    }

    return 0;
}

void free_entries(entry *cursor) {
    if (!cursor)
        return;

    free_entries(cursor->next);
    free(cursor->forward);
    free(cursor->backward);
    free(cursor->values);
    free(cursor);
}

void print_elements(entry *receiver) {
    printf("[");

    for (int i = 0; i < receiver->length; ++i) {
        element e = (receiver->values)[i];
        if (e.type == INTEGER) {
            printf("%d", e.value);
        } else {
            printf("%s", (e.entry)->key);
        }

        if (i < receiver->length - 1)
            printf(" ");
    }

    printf("]\n");
}

void bye() {
    free_entries(current_entries);
    free(stored_snapshots);
    printf("bye\n");
}

void help() {
    printf("%s\n", HELP);
}

void add_forward(entry *current, entry *forward) {
    char is_added = 1;
    for (int j = 0; j < current->forward_size; ++j) {
        if (forward == (current->forward)[j]) {
            is_added = 0;
            break;
        }
    }

    if (is_added) {
        (current->forward_size)++;
        current->forward = (entry **)realloc(current->forward, sizeof(entry *) * current->forward_size);
        (current->forward)[current->forward_size - 1] = forward;
    }
}

void remove_forward(entry *current, entry *forward) {
    for (int j = 0; j < current->forward_size; ++j) {
        if (forward == (current->forward)[j]) {
            for (int k = j; k < current->forward_size - 1; k++) {
                entry *tmp = (current->forward)[k];
                (current->forward)[k] = (current->forward)[k + 1];
                (current->forward)[k + 1] = tmp;
            }

            (current->forward_size)--;
            current->forward = (entry **)realloc(current->forward, sizeof(entry *) * current->forward_size);
            return;
        }
    }
}

void add_backward(entry *current, entry *backward) {
    char is_added = 1;
    for (int j = 0; j < current->backward_size; ++j) {
        if (backward == (current->backward)[j]) {
            is_added = 0;
            break;
        }
    }

    if (is_added) {
        (current->backward_size)++;
        current->backward = (entry **)realloc(current->backward, sizeof(entry *) * current->backward_size);
        (current->backward)[current->backward_size - 1] = backward;
    }
}

void remove_backward(entry *current, entry *backward) {
    for (int j = 0; j < current->backward_size; ++j) {
        if (backward == (current->backward)[j]) {
            for (int k = j; k < current->backward_size - 1; k++) {
                entry *tmp = (current->backward)[k];
                (current->backward)[k] = (current->backward)[k + 1];
                (current->backward)[k + 1] = tmp;
            }

            (current->backward_size)--;
            current->backward = (entry **)realloc(current->backward, sizeof(entry *) * current->backward_size);
            return;
        }
    }
}

entry *get_entry_by_key(char *key, entry *entries) {
    entry *cursor = entries;
    while (cursor != NULL) {
        if (strcmp(key, cursor->key) == 0)
            return cursor;

        cursor = cursor->next;
    }

    // Key doesn't exist
    return NULL;
}

snapshot *get_snap_by_id(int id) {
    snapshot *snap = stored_snapshots;
    while (snap) {
        if (id == snap->id)
            return snap;

        snap = snap->next;
    }

    // Snap doesn't exist
    return NULL;
}

entry *deep_copy_current_snapshots() {
    if (!stored_snapshots)
        return NULL;

    snapshot *position = get_snap_by_id(atoi2(input[1]));
    entry *cursor = position->entries;
    entry *prev_copied_cursor = NULL;
    entry *copied_head = NULL;

    // [entry1] <-> [entry2] <-> [entry3]
    // [copy01] <-> [copy02]     [copy03]

    while (cursor) {
        entry *mem = (entry *)malloc(sizeof(entry));
        if (!copied_head) {
            copied_head = mem;
        }

        mem->prev = prev_copied_cursor;
        if (prev_copied_cursor)
            prev_copied_cursor->next = mem;

        prev_copied_cursor = mem;

        strcpy(mem->key, cursor->key);
        mem->is_simple = cursor->is_simple;
        mem->length = cursor->length;
        mem->forward_size = cursor->forward_size;
        mem->backward_size = cursor->backward_size;

        cursor = cursor->next;
    }

    cursor = position->entries;
    entry *copied_cursor = copied_head;
    while (cursor) {
        copied_cursor->values = (element *)malloc(sizeof(element) * cursor->length);
        for (int i = 0; i < cursor->length; i++) {
            if ((cursor->values)[i].type == INTEGER)
                (copied_cursor->values)[i] = (cursor->values)[i];
            else {
                (copied_cursor->values)[i].type = ENTRY;
                (copied_cursor->values)[i].entry =
                    get_entry_by_key(
                        (cursor->values)[i].entry->key,
                        copied_head);
            }
        }

        copied_cursor->forward = (entry **)malloc(sizeof(entry) * cursor->forward_size);
        for (int i = 0; i < cursor->forward_size; i++) {
            (copied_cursor->forward)[i] = get_entry_by_key(
                (cursor->forward)[i]->key,
                copied_head);
        }

        copied_cursor->backward = (entry **)malloc(sizeof(entry) * cursor->backward_size);
        for (int i = 0; i < cursor->backward_size; i++) {
            (copied_cursor->backward)[i] = get_entry_by_key(
                (cursor->backward)[i]->key,
                copied_head);
        }

        cursor = cursor->next;
        copied_cursor = copied_cursor->next;
    }

    return copied_head;
}

entry *deep_copy_current_entries() {
    if (!current_entries)
        return NULL;

    entry *cursor = current_entries;
    entry *prev_copied_cursor = NULL;
    entry *copied_head = NULL;

    // [entry1] <-> [entry2] <-> [entry3]
    // [copy01] <-> [copy02]     [copy03]

    while (cursor) {
        entry *mem = (entry *)malloc(sizeof(entry));
        if (!copied_head) {
            copied_head = mem;
        }

        mem->prev = prev_copied_cursor;
        if (prev_copied_cursor)
            prev_copied_cursor->next = mem;

        prev_copied_cursor = mem;

        strcpy(mem->key, cursor->key);
        mem->is_simple = cursor->is_simple;
        mem->length = cursor->length;
        mem->forward_size = cursor->forward_size;
        mem->backward_size = cursor->backward_size;

        cursor = cursor->next;
    }

    cursor = current_entries;
    entry *copied_cursor = copied_head;
    while (cursor) {
        copied_cursor->values = (element *)malloc(sizeof(element) * cursor->length);
        for (int i = 0; i < cursor->length; i++) {
            if ((cursor->values)[i].type == INTEGER)
                (copied_cursor->values)[i] = (cursor->values)[i];
            else {
                (copied_cursor->values)[i].type = ENTRY;
                (copied_cursor->values)[i].entry =
                    get_entry_by_key(
                        (cursor->values)[i].entry->key,
                        copied_head);
            }
        }

        copied_cursor->forward = (entry **)malloc(sizeof(entry) * cursor->forward_size);
        for (int i = 0; i < cursor->forward_size; i++) {
            (copied_cursor->forward)[i] = get_entry_by_key(
                (cursor->forward)[i]->key,
                copied_head);
        }

        copied_cursor->backward = (entry **)malloc(sizeof(entry) * cursor->backward_size);
        for (int i = 0; i < cursor->backward_size; i++) {
            (copied_cursor->backward)[i] = get_entry_by_key(
                (cursor->backward)[i]->key,
                copied_head);
        }

        cursor = cursor->next;
        copied_cursor = copied_cursor->next;
    }

    return copied_head;
}

char validate_input(int valid_num_words) {
    if (num_words < valid_num_words) {
        printf("Error. Invalid Command\n");
        return 0;
    }

    if (strlen(input[1]) > MAX_KEY) {
        printf("Error. Max key reached.\n");
        return 0;
    }

    if (!isalpha(input[1][0])) {
        printf("Error. Key is not an alphabet\n");
        return 0;
    }

    return 1;
}

char validate_snapshot(int valid_num_words) {
    if (num_words < valid_num_words) {
        printf("Error. Invalid Command\n");
        return 0;
    }
    if (isalpha(input[1][0])) {
        printf("Error. ID cannot be an alphabet\n");
        return 0;
    }

    return 1;
}

char validate_key() {
    for (int i = 2; i < num_words; ++i) {
        if (is_integer(input[i]))
            continue;

        if (!get_entry_by_key(input[i], current_entries)) {
            printf("Error\n");
            return 0;
        }

        if (strcmp(input[i], input[1]) == 0) {
            printf("Error. Key cannot be the same\n");
            return 0;
        }
    }
    return 1;
}

void list_keys() {
    if (!current_entries)
        printf("no keys\n");

    entry *cursor = current_entries;
    while (cursor) {
        printf("%s\n", cursor->key);
        cursor = cursor->next;
    }
    printf("\n");
}

void list_entries() {
    if (!current_entries)
        printf("no entries\n");

    entry *cursor = current_entries;
    while (cursor) {
        printf("%s ", cursor->key);
        print_elements(cursor);
        cursor = cursor->next;
    }
    printf("\n");
}

void list_snapshots() {
    if (!stored_snapshots)
        printf("no snapshots\n");

    snapshot *snap = stored_snapshots;
    while (snap) {
        printf("%d", snap->id);
        if (snap->next)
            printf("\n");
        else
            printf("\n");
        snap = snap->next;
    }
    printf("\n");
}

void get() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!validate_key())
        return;

    print_elements(receiver);
}

char del(entry *entries) {
    if (!validate_input(2))
        return 1;

    entry *receiver = get_entry_by_key(input[1], entries);
    if (!receiver) {
        return 2;
    }

    if (receiver->backward_size > 0) {
        return 3;
    }

    // Eg1: a b c
    // free a in eg1
    if (!(receiver->prev)) {
        if (receiver->next) {
            receiver->next->prev = NULL;
        }

        current_entries = receiver->next;

    } else if (!(receiver->next)) {  // free c in eg1

        receiver->prev->next = NULL;
    } else {  // free b in eg1
        entry *p = receiver->prev;
        entry *n = receiver->next;
        p->next = n;
        n->prev = p;
    }

    // a -> b -> c
    // d -> c

    for (int i = 0; i < receiver->forward_size; i++)
        remove_backward((receiver->forward)[i], receiver);

    free(receiver->forward);
    free(receiver->backward);
    free(receiver->values);
    free(receiver);

    return 0;
}

void purge() {
    char res = del(current_entries);
    if (res == 2) {
        printf("no such key\n");
        return;
    } else if (res == 3) {
        printf("not permitted\n");
        return;
    }

    snapshot *current = stored_snapshots;
    while (current) {
        entry *cursor = get_entry_by_key(input[1], current->entries);
        if (cursor)
            del(current->entries);

        current = current->next;
    }

    printf("ok\n");
}

void set() {
    if (!validate_input(3))
        return;

    if (!validate_key())
        return;

    // Key valid. Store position of key here if found in database
    entry *position = get_entry_by_key(input[1], current_entries);
    char is_new_entry = 0;

    // realloc memory based on the entries
    if (position) {
        position->values = (element *)realloc(position->values, sizeof(element) * (num_words - 2));

        // key doesn't eixst, allocate memory for new entry
    } else {
        is_new_entry = 1;
        position = (entry *)malloc(sizeof(entry));
        strcpy(position->key, input[1]);
        position->values = (element *)malloc(sizeof(element) * (num_words - 2));
        position->next = NULL;
        position->prev = NULL;
        position->forward_size = 0;
        position->backward_size = 0;
        position->forward = (entry **)malloc(0);
        position->backward = (entry **)malloc(0);
    }

    position->is_simple = 1;

    // Assign values for entry
    for (int i = 0; i < num_words - 2; ++i) {
        if (is_integer(input[i + 2])) {
            (position->values)[i].type = INTEGER;
            (position->values)[i].value = atoi2(input[i + 2]);
        } else {
            (position->values)[i].type = ENTRY;
            (position->values)[i].entry = get_entry_by_key(input[i + 2], current_entries);
            position->is_simple = 0;

            add_forward(position, (position->values)[i].entry);
            add_backward((position->values)[i].entry, position);
        }
    }

    position->length = num_words - 2;

    if (current_entries == NULL) {
        current_entries = position;
        current_entries->next = NULL;
        current_entries->prev = NULL;
    } else if (is_new_entry) {
        current_entries->prev = position;
        position->next = current_entries;
        current_entries = position;
    }

    printf("ok\n");
}

void push() {
    if (!validate_input(3))
        return;

    entry *position = get_entry_by_key(input[1], current_entries);
    if (!position) {
        printf("no such key\n");
        return;
    }

    if (!validate_key())
        return;

    position->length += num_words - 2;
    position->values = (element *)realloc(position->values, sizeof(element) * position->length);

    // PUSH a 5 6
    // [1 2 3 @ @]
    // [1 2 @ @ 3]
    // [1 @ @ 2 3]
    // [ @ @ 1 2 3]
    // [5 6 1 2 3]

    for (int i = position->length - (num_words - 2) - 1; i >= 0; i--) {
        element tmp = (position->values)[i];
        (position->values)[i] = (position->values)[i + (num_words - 2)];
        (position->values)[i + (num_words - 2)] = tmp;
    }

    // adds forward and backward position
    position->is_simple = 1;
    for (int i = 0; i < num_words - 2; ++i) {
        if (is_integer(input[i + 2])) {
            (position->values)[i].type = INTEGER;
            (position->values)[i].value = atoi2(input[i + 2]);
        } else {
            (position->values)[i].type = ENTRY;
            (position->values)[i].entry = get_entry_by_key(input[i + 2], current_entries);
            position->is_simple = 0;

            add_forward(position, (position->values)[i].entry);
            add_backward((position->values)[i].entry, position);
        }
    }

    printf("ok\n");
}

void append() {
    if (!validate_input(3))
        return;

    entry *position = get_entry_by_key(input[1], current_entries);
    if (!position) {
        printf("no such key\n");
        return;
    }

    if (!validate_key())
        return;

    position->length += num_words - 2;
    position->values = (element *)realloc(position->values, sizeof(element) * position->length);
    position->is_simple = 1;
    for (int i = 0; i < num_words - 2; ++i) {
        int index = position->length - (num_words - 2) + i;
        if (is_integer(input[i + 2])) {
            (position->values)[index].type = INTEGER;
            (position->values)[index].value = atoi2(input[i + 2]);
        } else {
            (position->values)[index].type = ENTRY;
            (position->values)[index].entry = get_entry_by_key(input[i + 2], current_entries);
            position->is_simple = 0;

            add_forward(position, (position->values)[index].entry);
            add_backward((position->values)[index].entry, position);
        }
    }

    printf("ok\n");
}

void drop() {
    if (!validate_snapshot(2))
        return;

    snapshot *current = get_snap_by_id(atoi2(input[1]));
    if (!current) {
        printf("no such snapshot\n");
        return;
    }

    entry *entries = current->entries;
    if (!entries) {
        printf("no entries in snapshot\n");
        return;
    }

    if (!(current->prev)) {
        if (current->next)
            current->next->prev = NULL;

        stored_snapshots = current->next;

    } else if (!(current->next)) {
        current->prev->next = NULL;

    } else {
        snapshot *p = current->prev;
        snapshot *n = current->next;
        p->next = n;
        n->prev = p;
    }

    free_entries(current->entries);
    free(current);
    printf("ok\n");
}

void checkout() {
    if (num_words - 2 != 0) {
        printf("Invalid command\n");
        return;
    }

    if (!is_integer(input[1])) {
        printf("ID is not an integer\n");
        return;
    }

    snapshot *position = get_snap_by_id(atoi2(input[1]));
    if (!position) {
        printf("no such snapshot\n");
        return;
    }
    entry *pointer = current_entries;
    pointer = deep_copy_current_snapshots();
    current_entries = pointer;

    printf("ok\n");
}




void snapsh() {
    snapshot *new_snap = malloc(sizeof(snapshot));
    if (!stored_snapshots) {
        new_snap->id = 1;
        stored_snapshots = new_snap;
        new_snap->next = NULL;
        new_snap->prev = NULL;
    } else {
        snapshot *cursor = stored_snapshots;
        int max_id = 0;
        while (cursor) {
            if (cursor->id > max_id) {
                max_id = cursor->id;
            }
            cursor = cursor->next;
        }
        new_snap->id = max_id + 1;
        new_snap->next = stored_snapshots;
        new_snap->prev = NULL;
        stored_snapshots->prev = new_snap;
        stored_snapshots = new_snap;
    }

    new_snap->entries = deep_copy_current_entries();

    printf("saved as snapshot %d\n", new_snap->id);
}



void pick() {
    if (!validate_input(2))
        return;

    int index = atoi2(input[2]);
    entry *receiver = get_entry_by_key(input[1], current_entries);

    if (index > receiver->length || index <= 0) {
        printf("index out of range\n");
        return;
    }

    element pick = (receiver->values)[index - 1];
    if (pick.type == INTEGER) {
        printf("%d\n", pick.value);
    } else {
        printf("%s\n", (pick.entry)->key);
    }
}

void pluck() {
    if (!validate_input(3))
        return;

    int index = atoi2(input[2]);

    entry *position = get_entry_by_key(input[1], current_entries);
    if (!position) {
        printf("no such key\n");
        return;
    }

    if (index > position->length || index <= 0) {
        printf("index out of range\n");
        return;
    }

    // printing of the value
    index--;
    element pluck = (position->values)[index];
    if (pluck.type == INTEGER) {
        printf("%d\n", pluck.value);
    } else {
        printf("%s\n", (pluck.entry)->key);
    }

    // removing value and shifting array
    for (int i = index; i < position->length - 1; ++i) {
        (position->values)[i] = (position->values)[i + 1];

        remove_forward(position, (position->values)[i].entry);
        remove_backward((position->values)[i].entry, position);
    }
    // Realloc memory based on new length
    position->length--;
    position->values = (element *)realloc(position->values, sizeof(element) * position->length);
}

void pop() {
    if (!validate_input(2))
        return;

    entry *position = get_entry_by_key(input[1], current_entries);
    if (!position) {
        printf("no such key\n");
        return;
    }

    if (!position->length) {
        printf("nil\n");
        return;
    }

    element pop = (position->values)[0];
    // printing of the value
    if (pop.type == INTEGER) {
        printf("%d\n", pop.value);
    } else {
        printf("%s\n", (pop.entry)->key);
    }

    // removing value and shifting array
    for (int i = 0; i < position->length - 1; ++i) {
        (position->values)[i] = (position->values)[i + 1];
    }

    // if (position->backward_size) {
    //     remove_backward((position->values)[index].entry, position);
    // }

    position->length--;
    position->values = (element *)realloc(position->values, sizeof(element) * position->length);
}

int sum_helper(entry *receiver) {
    int sum = 0;
    for (int i = 0; i < receiver->length; ++i) {
        element e = (receiver->values)[i];
        if (e.type == INTEGER) {
            sum += e.value;
        } else {
            sum += sum_helper(e.entry);
        }
    }

    return sum;
}

void sum() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    int answer = sum_helper(receiver);
    printf("%d\n", answer);
}

int min_helper(entry *receiver) {
    element e = (receiver->values)[0];
    int min;
    int min2;

    for (int i = 0; i < receiver->length; ++i) {
        e = (receiver->values)[i];
        if (e.type == INTEGER) {
            if (e.value < min)
                min = e.value;
        } else {
            min2 = min_helper(e.entry);
        }
        if (min2 < min)
            min = min2;
    }

    return min;
}

void min() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    int answer = min_helper(receiver);
    printf("%d\n", answer);
}

int max_helper(entry *receiver) {
    int max = min_helper(receiver);
    int max2 = min_helper(receiver);

    for (int i = 0; i < receiver->length; ++i) {
        element e = (receiver->values)[i];
        if (e.type == INTEGER) {
            if (e.value > max)
                max = e.value;
        } else {
            max2 = max_helper(e.entry);
        }
    }
    if (max2 > max)
        max = max2;
    return max;
}

void max() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    int answer = max_helper(receiver);
    printf("%d\n", answer);
}

int len_helper(entry *receiver) {
    element e = (receiver->values)[0];
    int len = 0;

    for (int i = 0; i < receiver->length; ++i) {
        e = (receiver->values)[i];
        if (e.type == INTEGER) {
            len += 1;
        } else {
            len += len_helper(e.entry);
        }
    }

    return len;
}

void len() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    int answer = len_helper(receiver);
    printf("%d\n", answer);
}

void rev() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }

    if (!receiver->is_simple) {
        printf("Cannot perform on general entries\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    for (int i = 0, j = receiver->length - 1; i < j; ++i, --j) {
        element tmp;
        tmp = (receiver->values)[i];
        (receiver->values)[i] = (receiver->values)[j];
        (receiver->values)[j] = tmp;
    }
    printf("ok\n");
}

void uniq() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!receiver->is_simple) {
        printf("Cannot perform on general entries\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    int *flags = (int *)malloc(sizeof(int) * receiver->length);
    flags[0] = 0;
    int count = 1;  // number of non-repeated numbers
    for (int i = 1; i < receiver->length; ++i) {
        if ((receiver->values)[i].value == (receiver->values)[i - 1].value) {
            flags[i] = 1;
        } else {
            flags[i] = 0;
            count++;
        }
    }
    element *unique = (element *)malloc(count * sizeof(element));
    for (int i = 0, j = 0; i < receiver->length; ++i) {
        if (flags[i] == 0) {
            unique[j++] = (receiver->values)[i];
        }
    }
    receiver->values = realloc(receiver->values, count * sizeof(element));
    for (int i = 0; i < count; i++) {
        (receiver->values)[i] = unique[i];
    }
    receiver->length = count;
    free(unique);
    free(flags);
    printf("ok\n");
}

void type() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (receiver->is_simple) {
        printf("simple\n");
    } else {
        printf("general\n");
    }
}

int print_cmp(const void *a, const void *b) {
    const entry *a1 = *((entry **)a);
    const entry *a2 = *((entry **)b);
    return strcmp(a1->key, a2->key);
}

void print_forward(entry *receiver) {
    entry **p = (entry **)malloc(sizeof(entry *) * receiver->forward_size);
    for (int i = 0; i < receiver->forward_size; ++i) {
        p[i] = (receiver->forward)[i];
    }

    qsort(p, receiver->forward_size, sizeof(entry *), print_cmp);
    for (int i = 0; i < receiver->forward_size; ++i) {
        entry *future = p[i];
        if (i == receiver->forward_size - 1) {
            printf("%s\n", future->key);
        } else {
            printf("%s, ", future->key);
        }
    }
    free(p);
}

void forward() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (receiver->is_simple && !receiver->forward_size) {
        printf("nil\n");
        return;
    }

    print_forward(receiver);
}

void print_backward(entry *receiver) {
    entry **p = (entry **)malloc(sizeof(entry *) * receiver->backward_size);
    for (int i = 0; i < receiver->backward_size; ++i) {
        p[i] = (receiver->backward)[i];
    }

    qsort(p, receiver->backward_size, sizeof(entry *), print_cmp);
    for (int i = 0; i < receiver->backward_size; ++i) {
        entry *future = p[i];
        if (i == receiver->backward_size - 1) {
            printf("%s\n", future->key);
        } else {
            printf("%s, ", future->key);
        }
    }
    free(p);
}

void backward() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (receiver->is_simple && !receiver->backward_size) {
        printf("nil\n");
        return;
    }
    print_backward(receiver);
}

int compare(const void *a, const void *b) {
    const element *num_1 = (element *)a;
    const element *num_2 = (element *)b;
    return num_1->value - num_2->value;
}

void sort() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1], current_entries);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!receiver->is_simple) {
        printf("Cannot perform on general entries\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    qsort(receiver->values, receiver->length, sizeof(element), compare);
    printf("ok\n");
}

void command_invalid() {
    printf("Invalid operation, try HELP.\n");
}

//Utility functionns below

int num_words_in_line(char *line) {
    int len_line = strlen(line);
    int num_words = 0;
    //      Hello   world
    char is_last_char_space = 1;
    // for each word
    for (int line_i = 0; line_i < len_line - 1; line_i++) {
        if (line[line_i] == ' ' || line[line_i] == '\t')
            is_last_char_space = 1;
        else {
            if (is_last_char_space)
                num_words++;

            is_last_char_space = 0;
        }
    }
    return num_words;
}

char **get_words(char *line, int *num_words) {
    int len_line = strlen(line);
    if (len_line == 0) {
        return NULL;
    }

    // separated by spaces,   ' ', '\t'
    *num_words = num_words_in_line(line);
    char **words = (char **)malloc(*num_words * sizeof(char *));
    char is_last_char_space = 1;
    /// 01234567
    //      Hello   world
    int word_i = 0;
    int first = 0;
    int last = 0;

    // for each word
    for (int line_i = 0; line_i < len_line; line_i++) {
        if (line[line_i] == ' ' || line[line_i] == '\t' || line[line_i] == '\n') {
            if (!is_last_char_space) {
                last = line_i - 1;
                words[word_i] = (char *)malloc((last - first + 2) * sizeof(char));
                for (int i = first, j = 0; i <= last; i++, j++)
                    words[word_i][j] = line[i];

                words[word_i][last - first + 1] = '\0';
                word_i++;
            }

            is_last_char_space = 1;
        } else {
            if (is_last_char_space) {
                first = line_i;
            }

            is_last_char_space = 0;
        }
    }

    return words;
}

char is_integer(char *str) {
    int start_index = 0;
    if (str[0] == '-') {
        start_index = 1;
    }

    for (int i = start_index; i < strlen(str); ++i) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }

    return 1;
}
// Code adapted from week 2 tutorial by Jiahao Chen with a little change
int atoi2(char *str) {
    int is_negative = 1;
    int result = 0;
    int i = 0;
    int len = strlen(str);

    if (str[0] == '-') {
        is_negative = -1;
        i++;
    }

    for (; i < len; ++i) {
        result = result * 10 + str[i] - '0';
    }
    return result * is_negative;
}
