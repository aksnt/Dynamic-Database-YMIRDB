#ifndef HELPER_H
#define HELPER_H

#include "ymirdb.h"

/**
 * @brief Function which is used for printing entries in get() or list_entries()
 * 
 * @param receiver 
 */
void print_elements(entry *receiver);

/**
 * @brief Recursively deletes and frees entries
 * 
 * @param cur 
 */
void del_snapshots(snapshot* cur);

/**
 * @brief Adds forward position to an entry
 * 
 * @param current
 * @param forward
 */
void add_forward(entry *current, entry *forward);


/**
 * @brief dds forward position to an entry
 * 
 * @param current
 * @param forward
 */
void remove_forward(entry *current, entry *forward);


/**
 * @brief Adds backward position to an entry
 * 
 * @param current 
 * @param backward 
 */
void add_backward(entry *current, entry *backward);

/**
 * @brief Removes forward position to an entry
 * 
 * @param current 
 * @param backward 
 */
void remove_backward(entry *current, entry *backward);


/**
 * @brief Returns the position of the entry in linkedlist based on input[1]
 * 
 * @param key 
 * @param entries 
 * @return entry* to the entry with the given key
 */
entry *get_entry_by_key(char *key, entry *entries);


/**
 * @brief Returns the position of the snapshot ID based on the input[1]
 * 
 * @param id 
 * @return snapshot* to the position of the snapshot in the stored snapshots
 */
snapshot *get_snap_by_id(int id);

/**
 * @brief Deep copies LinkedList to a new linkedlist
 * 
 * @param entries (current_entries)
 * @return entry* to the head of the new list
 */
entry *deep_copy_entries(entry *entries);

/**
 * @brief Helper to delete a single entry and free the memory
 * 
 * @param receiver 
 * @param entries 
 * @param is_current_entries 
 * @param snapshot 
 */
void del_entry(entry *receiver, entry *entries, char is_current_entries, snapshot *snapshot);


/**
 * @brief Helper that utilises del_entry(); to delete multiple entries with recursion
 * 
 * @param cursor 
 * @param entries 
 * @param is_current_entries 
 * @param snapshot 
 */
void del_entries(entry *cursor, entry *entries, char is_current_entries, snapshot *snapshot);

/**
 * @brief Helper function to recursively total the entry
 * 
 * @param receiver 
 * @return int sum of the entry
 */
int sum_helper(entry *receiver);

/**
 * @brief Helper function to recursively find the minimum of the entry
 * 
 * @param receiver 
 * @return int min of the entry
 */
int min_helper(entry *receiver);

/**
 * @brief Helper function to recursively find the maximum of the entry
 * 
 * @param receiver 
 * @return int max of the entry
 */
int max_helper(entry *receiver);

/**
 * @brief Helper function to recursively find the length the entry
 * 
 * @param receiver 
 * @return int len of the entry
 */
int len_helper(entry *receiver);

/**
 * @brief Get the forward references of a particular entry using recursion
 * 
 * @param receiver 
 * @param result 
 * @param result_size 
 */
void get_forward(entry *receiver, char **result, int *result_size);

/**
 * @brief Get the backward references of a particular entry using recursion
 * 
 * @param receiver 
 * @param result 
 * @param result_size 
 */
void get_backward(entry *receiver, char **result, int *result_size);

#endif