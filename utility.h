#ifndef UTILITY_H
#define UTILITY_H


/**
 * @brief Calculates the number of words in the input while ignoring spaces/tabs/etc
 * 
 * @param line 
 * @return int num_words
 */
int num_words_in_line(char* line);

/**
 * @brief Function that is used to get the words in the input based on num_words_in_line()
 * 
 * @param line 
 * @param num_words 
 * @return char** words
 */
char** get_words(char* line, int* num_words);


/**
 * @brief Check if a char is an integer (was unsure if the provided function was allowed)
 * 
 * @param str 
 * @return char 1 if it is an integer, 0 otherwise
 */
char is_integer(char* str);

/**
 * @brief Built-in atoi was giving errors (VSCODE), wrote another to avoid errors
 * 
 * @param str 
 * @return int result 
 */
int atoi2(char* str);

/**
 * @brief Validates input ensuring num_words by user is valid, key is an alphabet
 * and that max key limit has not been reached
 * 
 * @param valid_num_words 
 * @return char 1 if valid, 0 otherwise
 */
char validate_input(int valid_num_words);

/**
 * @brief Validates snapshot input by ensuring input does not exceed num_words and
 * the id provided is not an alphabet
 * 
 * @param valid_num_words 
 * @return char 1 if valid, 0 otherwise
 */
char validate_snapshot(int valid_num_words);

/**
 * @brief Validates key by ensuring the key exists in current state, and 
 * input is not equal to the key
 * 
 * @return char 
 */
char validate_key();

/**
 * @brief Used for qsort to compare two double pointers
 * 
 * @param a 
 * @param b 
 * @return sorted values in ascending order
 */
int print_cmp(const void *a, const void *b);

/**
 * @brief Used for qsort to compare two values
 * 
 * @param a 
 * @param b 
 * @return int in ascending order
 */
int compare(const void *a, const void *b);

#endif