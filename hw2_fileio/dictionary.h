/*
 * dictionary.h
 *
 * This file declares functions and constants for a dictionary of
 * words and definitions. There there can be multiple definitions
 * for a word. A word is limited to a length of MAX_WORD characters
 * including the terminating '\0' character, and a definition is
 * limited to MAX_DEF characters including the terminating '\0'
 * character. The dictionary is limited to MAX_ENTRIES entries.
 *
 *  @since 2019-05-09
 *  @author Philip Gust
 */

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <stdbool.h>

/** Maximum word length */
#define MAX_WORD 64

/** Maximum definition length */
#define MAX_DEF 20000

/** Maximum entries in dictionary */
#define MAX_ENTRIES 5000

/**
 * Return the number of entries in the dictionary.
 * @return the number of dictionary entries
 */
int getDictionarySize();

/**
 * Get dictionary word.
 *
 * @param entry the index of the entry
 * @param def the definition for the entry
 * @return true if entry found
 */
bool getDictionaryWord(int entry, char word[]);

/**
 * Get dictionary definition.
 *
 * @param entry the index of the entry
 * @param def the definition for the entry
 * @return true if entry found
 */
bool getDictionaryDefinition(int entry, char def[]);

/**
 * Find dictionary entry for a word. If word ends
 * with wildcard (*), finds any matching word.
 *
 * @param word the word to match
 * @param start_entry the starting entry
 * @return entry index or -1 if not found
 */
int getDictionaryEntry(const char word[], int start_entry);

/**
 * Put definition entry for name. Assumes unique entry name.
 *
 * @param word the entry name
 * @param def the entry definition
 * @return index of new entry or -1 if dictionary is full
 */
int putDictionaryEntry(const char word[], const char def[]);

#endif /* DICTIONARY_H_ */
