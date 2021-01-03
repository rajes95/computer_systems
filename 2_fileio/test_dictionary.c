/*
 * test_dictionary.c
 *
 * This file adds unit tet suite for the dictionary functions.
 *
 *  @since 2019-05-09
 *  @author Philip Gust
 */

#include <stdbool.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "dictionary.h"

/**
 * Test empty dictionary
 */
static void testDictionaryEmpty(void) {
	int size = getDictionarySize();
	CU_ASSERT_EQUAL(size, 0);

	int entry = getDictionaryEntry("", 0);
	CU_ASSERT_EQUAL(entry, -1);
	entry = getDictionaryEntry("", MAX_ENTRIES);
	CU_ASSERT_EQUAL(entry, -1);
}

/**
 * Test dictionary entries.
 */
static void testDictionaryEntries(void) {
	char def[MAX_DEF];
	char word[MAX_WORD];
	char test_def[MAX_DEF];
	char test_word[MAX_WORD];
	bool result;
	int entry;

	// make 10 entries
	for (int i = 0; i < 10; i++) {
		sprintf(word, "%d", i);
		sprintf(def, "definition %d", i);
		entry = putDictionaryEntry(word, def);
		CU_ASSERT_EQUAL_FATAL(entry, i);
	}

	// make 10 more duplicate word entries
	for (int i = 10; i < 20; i++) {
		sprintf(word, "%d", i%10);
		sprintf(def, "definition %d", i);
		entry = putDictionaryEntry(word, def);
		CU_ASSERT_EQUAL_FATAL(entry, i);
	}

	// lookup words and definitions in dictionary
	for (int i = 0; i < 10; i++) {
		sprintf(test_word, "%d", i);

		// look up word
		entry = getDictionaryEntry(test_word, 0);
		CU_ASSERT_EQUAL_FATAL(entry, i);

		// test expected word
		result = getDictionaryWord(entry, word);
		CU_ASSERT_STRING_EQUAL(word, test_word);

		sprintf(test_def, "definition %d", i);

		// test expected def
		result = getDictionaryDefinition(entry, def);
		CU_ASSERT_STRING_EQUAL(def, test_def);

		// look up second occurrence of word
		entry = getDictionaryEntry(test_word, entry+1);
		CU_ASSERT_EQUAL_FATAL(entry, 10+i);

		// test expected word
		result = getDictionaryWord(entry, word);
		CU_ASSERT_STRING_EQUAL(word, test_word);

		sprintf(test_def, "definition %d", 10+i);

		// test expected second def
		result = getDictionaryDefinition(entry, def);
		CU_ASSERT_STRING_EQUAL(def, test_def);

		// look up third occurrence of word
		entry = getDictionaryEntry(word, entry+1);
		CU_ASSERT_EQUAL_FATAL(entry, -1);
	}
}

/**
 * Test full dictionary.
 */
static void testDictionaryCapacity(void) {
	char def[MAX_DEF];
	char word[MAX_WORD];
	int entry;

	// fill dictionary
	for (int i = getDictionarySize(); i < MAX_ENTRIES; i++) {
		sprintf(word, "%d", i);
		sprintf(def, "definition %d", i);
		entry = putDictionaryEntry(word, def);
		CU_ASSERT_EQUAL_FATAL(entry, i);
	}

	// make one more entry
	sprintf(word, "%d", MAX_ENTRIES);
	sprintf(def, "definition %d", MAX_ENTRIES);
	entry = putDictionaryEntry(word, def);
	CU_ASSERT_EQUAL(entry, -1);

	// check that dictionary is full
	int size = getDictionarySize();
	CU_ASSERT_EQUAL(size, MAX_ENTRIES);
}

/**
 *  Add a test suite for dictionary tests. Test framework
 *  must already be initialized.
 */
void test_dictionary(void) {
	// add a suite to the registry with no init or cleanup
	CU_pSuite pSuite = CU_add_suite("dictionary_tests", NULL, NULL);

	// add the tests to the suite
	CU_add_test(pSuite, "testDictionaryEmpty", testDictionaryEmpty);
	CU_add_test(pSuite, "testDictionaryEntries", testDictionaryEntries);
	CU_add_test(pSuite, "testDictionaryCapacity", testDictionaryCapacity);
}
