#ifndef COMMANDS_H
#define COMMANDS_H

#include "ymirdb.h"


// General commands
/**
 * @brief Bye command that prints "bye"
 * 
 */
void bye();

/**
 * @brief Help command that prints the "help" string found in ymirdb.h
 * 
 */
void help();

/**
 * @brief Handles the case where command inputted is not supported
 * 
 */
void command_invalid();

/**
 * @brief Command that lists all the keys in the current state
 * 
 */
void list_keys();

/**
 * @brief Command that lists all entries in the current state
 * 
 */
void list_entries();

/**
 * @brief Command that lists all snapshots in the current state
 * 
 */
void list_snapshots();

// Main Commands

/**
 * @brief Get function that takes input and prints the value at that key
 * 
 */
void get();

/**
 * @brief Command that deletes an entry at the key
 * 
 * @param entries (takes in current entries)
 * @return char 1 invalid input, 2 if no such key, 3 if not permitted  
 */
char del(entry *entries);

/**
 * @brief Purges an entry based on input key from current state and snapshots
 * 
 */
void purge();

/**
 * @brief Command that sets or "stores" an entry in the current state
 * 
 */
void set();

/**
 * @brief Command that pushes values to the front of a given key
 * 
 */
void push();

/**
 * @brief Command that appends values to the back of a given key
 * 
 */
void append();

/**
 * @brief Command that displays a given entry at the index inputted
 * 
 */
void pick();

/**
 * @brief Removes a value at the index inputted from a given entry
 * 
 */
void pluck();

/**
 * @brief Removes the first value from a given key and prints to stdout
 * 
 */
void pop();

/**
 * @brief Command that deletes a snapshot from the current snapshots
 * 
 * @return char 0 if snapshot does not exist, 1 otherwise
 */
char drop();

/**
 * @brief Command that restores the current state to an earlier snapshot and removes
 *        newer snapshots
 * 
 */
void rollback();

/**
 * @brief Command that restores the current state to an earlier snapshots
 * 
 */
void checkout();

/**
 * @brief Command which snapshots the current state and inserts new snap at head
 * 
 */
void snapsh();  // snapshot command

/**
 * @brief Prints out the minimum value from a given entry
 * 
 */
void min();

/**
 * @brief Prints out the maximum value of a givbn entry
 * 
 */
void max();

/**
 * @brief Prints out the total sum of a given entry
 * 
 */
void sum();

/**
 * @brief Prints out the total length of a given entry
 * 
 */
void len();

/**
 * @brief Reverses an entry only if it is simple
 * 
 */
void rev();

/**
 * @brief Deletes all adjacent values in an entry if it is simple
 * 
 */
void uniq();

/**
 * @brief Sorts an entry in ascending order if it is simple
 * 
 */
void sort();

/**
 * @brief Displays all forward references of a given entry
 * 
 */
void forward();

/**
 * @brief Displays all backward references of a given entry
 * 
 */
void backward();

/**
 * @brief Prints out the type of entry a given entry is (simple or general)
 * 
 */
void type();

#endif // COMMANDS_H