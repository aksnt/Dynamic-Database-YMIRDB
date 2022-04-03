/**
 * comp2017 - assignment 2
 * <Akshil Nathu>
 * <anat9934>
 */


//Library headers
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

//Main headers
#include "ymirdb.h"
#include "commands.h"
#include "helpers.h"
#include "utility.h"

// Global Variables
int num_words = 0; //Keeps count of the number of words in the input
char **input = NULL; //Stores the input as char**
entry *current_entries = NULL; //Doubly LinkedList of the current entries
entry *current_entries_head = NULL; //Stores the head of the current entries
snapshot *stored_snapshots = NULL; //Doubly LinkedList of the stored snapshots


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
            if (!validate_snapshot(2))
                continue;
            char res = drop(atoi2(input[1]));
            if (res == 0)
                printf("no such snapshot\n");
            else
                printf("ok\n");

            printf("\n");

        } else if (strcasecmp(input[0], "CHECKOUT") == 0) {
            checkout();
            printf("\n");

        } else if (strcasecmp(input[0], "SNAPSHOT") == 0) {
            snapsh();
            printf("\n");

        } else if (strcasecmp(input[0], "ROLLBACK") == 0) {
            rollback();
            printf("\n");

        } else if (strcasecmp(input[0], "HELP") == 0) {
            help();

        } else if (strcasecmp(input[0], "BYE") == 0) {
            // Free memory allocated for input
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


//Main commands and helpers below. Defined in commands.h and helpers.h 

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

void del_snapshots(snapshot *cur) {
    if (!cur)
        return;

    del_snapshots(cur->next);
    if (cur->entries_head)
        del_entries(cur->entries, cur->entries, 0, cur);
    free(cur);
}

void bye() {
    if (current_entries_head != NULL)
        del_entries(current_entries, current_entries, 1, NULL);

    del_snapshots(stored_snapshots);
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
        current->forward = (entry **)
            realloc(current->forward, sizeof(entry *) * current->forward_size);
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
            // current->forward = (entry **)realloc(current->forward, sizeof(entry *) * current->forward_size);
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
        current->backward = (entry **)
            realloc(current->backward, sizeof(entry *) * current->backward_size);
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
            // current->backward = (entry **)realloc(current->backward, sizeof(entry *) * current->backward_size);
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

entry *deep_copy_entries(entry *entries) {
    if (!entries)
        return NULL;

    entry *cursor = entries;
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
        mem->next = NULL;
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

    cursor = entries;
    entry *copied_cursor = copied_head;
    while (cursor) {
        copied_cursor->values =
            (element *)malloc(sizeof(element) * cursor->length);
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

        copied_cursor->forward =
            (entry **)malloc(sizeof(entry) * cursor->forward_size);
        for (int i = 0; i < cursor->forward_size; i++) {
            (copied_cursor->forward)[i] = get_entry_by_key(
                (cursor->forward)[i]->key,
                copied_head);
        }

        copied_cursor->backward =
            (entry **)malloc(sizeof(entry) * cursor->backward_size);
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
            printf("no such key\n");
            return 0;
        }

        if (strcmp(input[i], input[1]) == 0) {
            printf("not permitted\n");
            return 0;
        }
    }
    return 1;
}

void list_keys() {
    if (!current_entries_head) {
        printf("no keys\n\n");
        return;
    }

    entry *cursor = current_entries;
    while (cursor) {
        printf("%s\n", cursor->key);
        cursor = cursor->next;
    }
    printf("\n");
}

void list_entries() {
    if (!current_entries_head) {
        printf("no entries\n\n");
        return;
    }
    entry *cursor = current_entries;
    while (cursor) {
        printf("%s ", cursor->key);
        print_elements(cursor);
        cursor = cursor->next;
    }
    printf("\n");
}

void list_snapshots() {
    if (!stored_snapshots) {
        printf("no snapshots\n\n");
        return;
    }

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

    entry *receiver = NULL;
    if (current_entries_head)
        receiver = get_entry_by_key(input[1], current_entries);

    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!validate_key())
        return;

    print_elements(receiver);
}

void del_entry(entry *receiver, entry *entries, char is_current_entries, snapshot *snapshot) {
    if (!(receiver->prev)) {
        if (receiver->next) {
            receiver->next->prev = NULL;
        }

        entries = receiver->next;
        if (is_current_entries)
            current_entries_head = entries;
        else if (snapshot)
            snapshot->entries_head = entries;

    } else if (!(receiver->next)) {
        receiver->prev->next = NULL;
    } else {  // free b in eg1
        entry *p = receiver->prev;
        entry *n = receiver->next;
        p->next = n;
        n->prev = p;
    }

    if (receiver->prev == NULL && receiver->next == NULL) {
        if (is_current_entries)
            current_entries_head = NULL;
        else if (snapshot)
            snapshot->entries_head = NULL;
    }

    // Removes the backward reference for the entry being deleted
    for (int i = 0; i < receiver->forward_size; i++)
        remove_backward((receiver->forward)[i], receiver);

    free(receiver->forward);
    free(receiver->backward);
    free(receiver->values);
    free(receiver);
}

void del_entries(entry *cursor, entry *entries, char is_current_entries, snapshot *snapshot) {
    if (cursor == NULL) return;

    // do smt before recrusive function
    del_entries(cursor->next, entries, is_current_entries, snapshot);
    // do smt after recrusive function
    del_entry(cursor, entries, is_current_entries, snapshot);
}

char del(entry *entries) {
    if (!validate_input(2))
        return 1;

    entry *receiver = NULL;
    if (current_entries_head)
        receiver = get_entry_by_key(input[1], current_entries);

    if (!receiver) {
        return 2;
    }

    if (receiver->backward_size > 0) {
        return 3;
    }

    del_entry(receiver, current_entries, 1, NULL);
    if (current_entries_head)
        current_entries = current_entries_head;
    return 0;
}

void purge() {
    if (!validate_input(2))
        return;

    if (!validate_key())
        return;   

    if (current_entries_head)
        del_entries(current_entries, current_entries, 1, NULL);

    if (current_entries_head)
        current_entries = current_entries_head;

    // Calls on del() to remove all entries in a snapshot
    snapshot *current = stored_snapshots;
    while (current) {
        entry *cursor = get_entry_by_key(input[1], current->entries);
        if (cursor)
            del_entries(current->entries, current->entries, 0, current);

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
    entry *position = NULL;
    if (current_entries_head)
        position = get_entry_by_key(input[1], current_entries);
    char is_new_entry = 0;

    // Realloc memory based on the new entry for a key
    if (position) {
        position->values =
            (element *)realloc(position->values, sizeof(element) * (num_words - 2));

        // Key doesn't exist, allocate memory for new entry
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

    // Assign values for entry along with its type
    for (int i = 0; i < num_words - 2; ++i) {
        if (is_integer(input[i + 2])) {
            (position->values)[i].type = INTEGER;
            (position->values)[i].value = atoi2(input[i + 2]);
        } else {
            (position->values)[i].type = ENTRY;
            (position->values)[i].entry =
                get_entry_by_key(input[i + 2], current_entries);
            position->is_simple = 0;

            add_forward(position, (position->values)[i].entry);
            add_backward((position->values)[i].entry, position);
        }
    }

    // Update the length of the entry
    position->length = num_words - 2;

    // Insert the new node at the head of the list whether it is new or not
    if (current_entries_head == NULL) {
        current_entries = position;
        current_entries->next = NULL;
        current_entries->prev = NULL;
        current_entries_head = current_entries;
    } else if (is_new_entry) {
        current_entries->prev = position;
        position->next = current_entries;
        current_entries = position;
        current_entries_head = current_entries;
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

    // Realloc memory based on the number of input being pushed
    position->length += num_words - 2;
    position->values =
        (element *)realloc(position->values, sizeof(element) * position->length);

    // PUSH a 5 6
    // [1 2 3 @ @]
    // [1 2 @ @ 3]
    // [1 @ @ 2 3]
    // [ @ @ 1 2 3]
    // [5 6 1 2 3]

    // shift values of the existing entry
    for (int i = position->length - (num_words - 2) - 1; i >= 0; i--) {
        element tmp = (position->values)[i];
        (position->values)[i] = (position->values)[i + (num_words - 2)];
        (position->values)[i + (num_words - 2)] = tmp;
    }

    // adds forward and backward position
    position->is_simple = 1;
    for (int i = 0, j = num_words - 1; i < (num_words - 2); i++, --j) {
        if (is_integer(input[i + 2])) {
            (position->values)[i].type = INTEGER;
            (position->values)[i].value = atoi2(input[j]);
        } else {
            (position->values)[i].type = ENTRY;
            (position->values)[i].entry =
                get_entry_by_key(input[j], current_entries);
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

    // Realloc memory based on the additional input
    position->length += num_words - 2;
    position->values =
        (element *)realloc(position->values, sizeof(element) * position->length);

    // Update the values in the entry
    position->is_simple = 1;
    for (int i = 0; i < num_words - 2; ++i) {
        int index = position->length - (num_words - 2) + i;
        if (is_integer(input[i + 2])) {
            (position->values)[index].type = INTEGER;
            (position->values)[index].value = atoi2(input[i + 2]);
        } else {
            (position->values)[index].type = ENTRY;
            (position->values)[index].entry =
                get_entry_by_key(input[i + 2], current_entries);
            position->is_simple = 0;

            add_forward(position, (position->values)[index].entry);
            add_backward((position->values)[index].entry, position);
        }
    }

    printf("ok\n");
}

char drop(int id) {
    snapshot *current = get_snap_by_id(id);
    if (!current) {
        return 0;
    }

    // If we are dropping a snapshot at the head of the list
    if (!(current->prev)) {
        if (current->next)
            current->next->prev = NULL;

        stored_snapshots = current->next;

        // If we are dropping a snapshot at the tail of the list
    } else if (!(current->next)) {
        current->prev->next = NULL;

        // If we are dropping a snapshot in the middle (all other cases)
    } else {
        snapshot *p = current->prev;
        snapshot *n = current->next;
        p->next = n;
        n->prev = p;
    }

    // Entries in that snapshot are deleted
    if (current->entries_head)
        del_entries(current->entries, current->entries, 0, current);
    // Free *current
    free(current);
    return 1;
}

void rollback() {
    if (!validate_snapshot(2)) {
        return;
    }

    snapshot *position = get_snap_by_id(atoi2(input[1]));
    if (!position) {
        printf("no such snapshot\n");
        return;
    }

    // Entries are completely deleted in the current list
    if (current_entries_head)
        del_entries(current_entries, current_entries, 1, NULL);

    snapshot *cursor = stored_snapshots;
    snapshot *temp;

    // Based on the snapshot ID, its entries are deep copied to the current entries
    if (position->entries_head) {
        current_entries = deep_copy_entries(position->entries);
    } else
        current_entries = NULL;

    current_entries_head = current_entries;

    // Deleting newer snapshots
    while (cursor != position) {
        temp = cursor->next;
        drop(cursor->id);
        cursor = temp;
    }

    printf("ok\n");
}

void checkout() {
    if (!validate_snapshot(2))
        return;

    snapshot *position = get_snap_by_id(atoi2(input[1]));
    if (!position) {
        printf("no such snapshot\n");
        return;
    }

    // Deletes entries and copies over those from the snapshot ID in input[1]
    del_entries(current_entries, current_entries, 0, position);

    current_entries = deep_copy_entries(position->entries);
    current_entries_head = current_entries;
    printf("ok\n");
}

void snapsh() {
    snapshot *new_snap = malloc(sizeof(snapshot));

    // If it is the first snapshot, begin counting ID from 1
    if (!stored_snapshots) {
        new_snap->id = 1;
        stored_snapshots = new_snap;
        new_snap->next = NULL;
        new_snap->prev = NULL;
        // Otherwise, find the position to insert the new snapshot using max_id
    } else {
        snapshot *cursor = stored_snapshots;
        int max_id = 0;
        while (cursor) {
            if (cursor->id > max_id) {
                max_id = cursor->id;
            }
            cursor = cursor->next;
        }

        // Insert the new snapshot at the head of list in all cases
        new_snap->id = max_id + 1;
        new_snap->next = stored_snapshots;
        new_snap->prev = NULL;
        stored_snapshots->prev = new_snap;
        stored_snapshots = new_snap;
    }

    // Deep copy the current state into this snapshot
    new_snap->entries = deep_copy_entries(current_entries);
    new_snap->entries_head = new_snap->entries;

    printf("saved as snapshot %d\n", new_snap->id);
}

void pick() {
    if (!validate_input(2))
        return;

    int index = atoi2(input[2]);
    entry *receiver = get_entry_by_key(input[1], current_entries);

    if (!receiver) {
        printf("no such key\n");
        return;
    }

    if (index > receiver->length || index <= 0) {
        printf("index out of range\n");
        return;
    }

    // PICK is indexed from 1, while receiver->values are from 0 thus index - 1
    index--;
    element pick = (receiver->values)[index];

    // Print based on the item type
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

    // Print the value
    index--;
    element pluck = (position->values)[index];
    if (pluck.type == INTEGER) {
        printf("%d\n", pluck.value);
    } else {
        printf("%s\n", (pluck.entry)->key);
    }

    // Removing value and shifting array
    for (int i = index; i < position->length - 1; ++i) {
        (position->values)[i] = (position->values)[i + 1];

        remove_forward(position, (position->values)[i].entry);
        remove_backward((position->values)[i].entry, position);
    }
    // Realloc memory based on new length
    position->length--;
    position->values =
        (element *)realloc(position->values, sizeof(element) * position->length);
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

    position->length--;
    position->values =
        (element *)realloc(position->values, sizeof(element) * position->length);
}

int sum_helper(entry *receiver) {
    int sum = 0;
    for (int i = 0; i < receiver->length; ++i) {
        element e = (receiver->values)[i];

        // If it is a value, add it to current sum
        if (e.type == INTEGER) {
            sum += e.value;
            // Otherwise, it is a key, recusrively call sum_helper to add to total sum
        } else {
            sum += sum_helper(e.entry);
        }
    }

    // Returns the total sum
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

    // Calls on helper function to recursively sum
    int answer = sum_helper(receiver);
    printf("%d\n", answer);
}

int min_helper(entry *receiver) {
    element e = (receiver->values)[0];

    // Keeps track of the current minimum of current entry
    int min;
    // Keeps track of the minimum of a key within the current entry
    int min2;

    for (int i = 0; i < receiver->length; ++i) {
        e = (receiver->values)[i];

        // Stores minimum if it is a value
        if (e.type == INTEGER) {
            if (e.value < min)
                min = e.value;
                
            // If it is a key, recursively store the minimum in min2
        } else {
            min2 = min_helper(e.entry);
        }
        // Compare the minimums in both cases to find the true minimum
        
    }
    if (min2 < min)
         min = min2;

    // Return the minimum of the entire entry
    
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
    // Sets the current max to the minimum value of entry based on min_helper
    int max = min_helper(receiver);   // stores max of the current entry
    int max2 = min_helper(receiver);  // stores max of the entry if it contains a key

    for (int i = 0; i < receiver->length; ++i) {
        element e = (receiver->values)[i];
        if (e.type == INTEGER) {
            if (e.value > max)
                max = e.value;
        } else {
            max2 = max_helper(e.entry);
        }
    }

    // Compare and return true max, then return it
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

        // Counts the length of the entry if it is an integer
        if (e.type == INTEGER) {
            len += 1;
            // If it is a key, recusrively calls to count the length of that entry
        } else {
            len += len_helper(e.entry);
        }
    }

    // Returns total length
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
        printf("not permitted on general entries\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    // Utilises tmp to reverse entries
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
        printf("not permitted on general entries\n");
        return;
    }

    if (!receiver->length) {
        printf("nil\n");
        return;
    }

    // Allocate memory for an array that will calculate repeated values
    int *flags = (int *)malloc(sizeof(int) * receiver->length);
    flags[0] = 0;

    // stores number of non-repeated values
    int count = 1;

    for (int i = 1; i < receiver->length; ++i) {
        // If NEXT entry is equal to its previous, it is repeated thus flag as 1
        if ((receiver->values)[i].value == (receiver->values)[i - 1].value) {
            flags[i] = 1;
            // Otherwise, simply flag as 0 and increase the count
        } else {
            flags[i] = 0;
            count++;
        }
    }

    // Allocate memory for an array storing unique values
    element *unique = (element *)malloc(count * sizeof(element));
    for (int i = 0, j = 0; i < receiver->length; ++i) {
        // Only store values marked with a 0 as they are unique, increment j
        if (flags[i] == 0) {
            unique[j++] = (receiver->values)[i];
        }
    }

    // Realloc memory for the values of the entry based on the count
    receiver->values = realloc(receiver->values, count * sizeof(element));
    for (int i = 0; i < count; i++) {
        (receiver->values)[i] = unique[i];
    }
    receiver->length = count;

    // Free unique and flags as they are no longer needed
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

    char is_simple = 1;
    for (int i = 0; i < receiver->length; ++i) {
        element e = (receiver->values)[i];
        if (e.type == ENTRY) 
            is_simple = 0;
    }
    
    if (is_simple) {
        printf("simple\n");
    } else {
        printf("general\n");
    }
}

int print_cmp(const void *a, const void *b) {
    // Dereference the double pointer of entry to obtain value
    const char *a1 = *((char **)a);
    const char *a2 = *((char **)b);
    // Compare these values to print in alphabetical order
    return strcmp(a1, a2);
}

void get_forward(entry *receiver, char **result, int *result_size) {
    for (int i = 0; i < receiver->forward_size; ++i) {
        (*result_size)++;
        result = realloc(result, sizeof(char *) * (*result_size));
        result[*result_size - 1] = (receiver->forward)[i]->key;
    }

    for (int i = 0; i < receiver->forward_size; ++i) {
        get_forward((receiver->forward)[i], result, result_size);
    }
}

void forward() {
    if (!validate_input(2))
        return;

    entry *receiver = NULL;
    if (current_entries_head)
        receiver = get_entry_by_key(input[1], current_entries);

    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!receiver->forward_size) {
        printf("nil\n");
        return;
    }

    char **result = malloc(0);
    int result_size = 0;
    get_forward(receiver, result, &result_size);

    qsort(result, result_size, sizeof(char *), print_cmp);
    for (int i = 0; i < result_size; i++) {
        printf("%s", result[i]);
        if (i == result_size - 1)
            printf("\n");
        else
            printf(" ");
    }

    free(result);
}

void get_backward(entry *receiver, char **result, int *result_size) {
    for (int i = 0; i < receiver->backward_size; ++i) {
        (*result_size)++;
        result = realloc(result, sizeof(char *) * (*result_size));
        result[*result_size - 1] = (receiver->backward)[i]->key;
    }

    for (int i = 0; i < receiver->backward_size; ++i) {
        get_backward((receiver->backward)[i], result, result_size);
    }
}

void backward() {
    if (!validate_input(2))
        return;

    entry *receiver = NULL;
    if (current_entries_head)
        receiver = get_entry_by_key(input[1], current_entries);

    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!receiver->backward_size) {
        printf("nil\n");
        return;
    }
    char **result = malloc(0);
    int result_size = 0;
    get_backward(receiver, result, &result_size);

    qsort(result, result_size, sizeof(char *), print_cmp);
    for (int i = 0; i < result_size; i++) {
        printf("%s", result[i]);
        if (i == result_size - 1)
            printf("\n");
        else
            printf(" ");
    }

    free(result);
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
        printf("not permitted on general entries\n");
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

// Utility functions below. Defined in utility.h

int num_words_in_line(char *line) {
    int len_line = strlen(line);
    int num_words = 0;

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

// Code adapted from week 2 tutorial by Jiahao Chen
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