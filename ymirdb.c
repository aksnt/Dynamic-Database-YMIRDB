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

// HELPERS
void print_elements(entry *receiver);
int num_words_in_line(char *line);
char **get_words(char *line, int *num_words);
char is_integer(char *str);
int atoi2(char *str);

// General functions
void bye();
void help();
void command_invalid();
void list_keys();
void list_entries();
void list_snapshots();

// Main Commands
void get();
void del();

void set();
void push();
void append();
void pick();
void pluck();
void pop();

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
            if (strcasecmp(input[1], "KEYS") == 0) {
                list_keys();
				printf("\n");
			}
            else if (strcasecmp(input[1], "ENTRIES") == 0) {
                list_entries();
				printf("\n");
			}
            // else if (strcasecmp(input[1], "SNAPSHOTS") == 0)
            // list_snapshots();
            else
                printf("Invalid command\n");
        } else if (strcasecmp(input[0], "SET") == 0) {
            set();
			printf("\n");

        } else if (strcasecmp(input[0], "GET") == 0) {
            get();
			printf("\n");

        } else if (strcasecmp(input[0], "DEL") == 0) {
            del();
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

        } else if (strcasecmp(input[0], "SNAPSHOT") == 0) {
        } else if (strcasecmp(input[0], "ROLLBACK") == 0) {
        } else if (strcasecmp(input[0], "HELP") == 0) {
            help();

        } else if (strcasecmp(input[0], "BYE") == 0) {
            for (int i = 0; i < num_words; ++i)
                free(input[i]);

            free(input);
            bye();
            break;
        } else {
            command_invalid();
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

void bye() {
    free_entries(current_entries);
    free(stored_snapshots);
    printf("bye\n");
}

/**
 * @brief Get the entry by key
 *
 * @param key
 * @return struct entry* the entry that has the matched key
 */
entry *get_entry_by_key(char *key) {
    entry *cursor = current_entries;
    while (cursor != NULL) {
        // Do some thing with the cursor here
        if (strcmp(key, cursor->key) == 0)
            return cursor;

        cursor = cursor->next;
    }

    // Key doesn't exist
    return NULL;
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

char validate_key() {
    for (int i = 2; i < num_words; ++i) {
        if (is_integer(input[i]))
            continue;

        if (!get_entry_by_key(input[i])) {
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
        current->forward = (entry **)realloc(current->forward, current->forward_size);
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
            current->forward = (entry **)realloc(current->forward, current->forward_size);
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
        current->backward = (entry **)realloc(current->backward, current->backward_size);
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
            current->backward = (entry **)realloc(current->backward, current->backward_size);
            return;
        }
    }
}

void set() {
    if (!validate_input(3))
        return;

    if (!validate_key())
        return;

    // Key valid. Store position of key here if found in database
    entry *position = get_entry_by_key(input[1]);
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
            (position->values)[i].entry = get_entry_by_key(input[i + 2]);
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

void get() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1]);
    if (!receiver) {
        printf("Key not found\n");
        return;
    }
    if (!validate_key())
        return;

    print_elements(receiver);
}

void del() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1]);
    if (!receiver) {
        printf("no such key\n");
        return;
    }

    if (receiver->backward_size > 0) {
        printf("not permitted\n");
        return;
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
}

void append() {
    if (!validate_input(3))
        return;

    entry *position = get_entry_by_key(input[1]);
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
            (position->values)[index].entry = get_entry_by_key(input[i + 2]);
            position->is_simple = 0;

            add_forward(position, (position->values)[index].entry);
            add_backward((position->values)[index].entry, position);
        }
    }

    printf("ok\n");
}

void push() {
    if (!validate_input(3))
        return;

    entry *position = get_entry_by_key(input[1]);
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
            (position->values)[i].entry = get_entry_by_key(input[i + 2]);
            position->is_simple = 0;

            add_forward(position, (position->values)[i].entry);
            add_backward((position->values)[i].entry, position);
        }
    }

    printf("ok\n");
}

void pick() {
    if (!validate_input(2))
        return;

    int index = atoi2(input[2]);
    entry *receiver = get_entry_by_key(input[1]);

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

    entry *position = get_entry_by_key(input[1]);
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

    entry *position = get_entry_by_key(input[1]);
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

        remove_forward(position, (position->values)[i].entry);
        remove_backward((position->values)[i].entry, position);
    } 
    
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

    entry *receiver = get_entry_by_key(input[1]);
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

    entry *receiver = get_entry_by_key(input[1]);
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

    entry *receiver = get_entry_by_key(input[1]);
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

    entry *receiver = get_entry_by_key(input[1]);
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

    entry *receiver = get_entry_by_key(input[1]);
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

    entry *receiver = get_entry_by_key(input[1]);
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
    // 2 ways
    // 1st: O(nm)
    // 2nd: 1 2 2 2 3 4 4
    //////: 0 0 1 1 0 0 1 O(n)
    //////: 1 2 3 4     O(n) => O(n)

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

    entry *receiver = get_entry_by_key(input[1]);
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

void print_backward(entry *receiver) {
    for (int i = 0; i < receiver->backward_size; ++i) {
        receiver = (receiver->backward)[i];
        if (receiver->is_simple) {
            if (i == receiver->backward_size - 1) {
                printf("%s\n", receiver->key);
            }
            printf("%s, ", receiver->key);

        } else {
            print_backward(receiver);
        }
    }
}

void print_forward(entry *receiver) {
    // add to an array?

    for (int i = 0; i < receiver->forward_size; ++i) {
        entry *future = (receiver->forward)[i];
        if (future->is_simple) {
            if (i == receiver->forward_size) {
                printf("%s\n", future->key);
            } else {
                printf("%s, ", future->key);
            }
        } else {
            print_forward(future);
        }
    }
}

void forward() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1]);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!receiver->forward_size) {
        printf("nil\n");
        return;
    }
    print_forward(receiver);
}

void backward() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1]);
    if (!receiver) {
        printf("no such key\n");
        return;
    }
    if (!receiver->backward_size) {
        printf("nil\n");
        return;
    }
    print_backward(receiver);
}

int compare(const void *a, const void *b) {
    const element *num_1 = (element*) a;
    const element *num_2 = (element*) b;
    return  num_1->value - num_2->value;
}

void sort() {
    if (!validate_input(2))
        return;

    entry *receiver = get_entry_by_key(input[1]);
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

    qsort(receiver->values, receiver->length, sizeof(entry), compare);
    printf("ok\n");
}

void command_invalid() {
    printf("Invalid operation, try HELP.\n");
}

void help() {
    printf("%s\n", HELP);
}

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
