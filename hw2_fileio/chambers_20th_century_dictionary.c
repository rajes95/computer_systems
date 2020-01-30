/*
 * chambers_20th_century_dictionary.c
 *
 * This program demonstrates the use of various C buffered
 * stream I/O function to implement a dictionary of words
 * and definitions, and an interactive command interpreter
 * for retrieving entry words, definitions, and counts.
 *
 *  @since 2019-05-09
 *  @author Philip Gust
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "dictionary.h"
#include "test_dictionary.h"


/** Maximum length of input line */
#define MAX_LINE 256

/** Number of truncated definitions read */
int defs_truncated;

/** Number of words skipped because the were too long */
int words_skipped;

/** Maximum definition length read */
int max_def_len;


/**
 * Read a line of text from the dictionary file. Input Lines
 * are terminated by the newline sequence "\r\n". This function
 * normalizes the newline to '\n'.
 *
 * @param stream the input stream to read from
 * @param str the input string
 * @param size the maximum number of characters to read including '\0'
 * @return the length of string read including '\n' but not '\0'
 *   or -1 if a read error occurred
 */
int readLine(FILE* stream, char str[], int size) {
	if (fgets(str, size, stream) == NULL) {
		return -1;
	}

	// replace "\r\n" end of line sequence with "\n"
	int len = strlen(str);
	if ((len >= 2) && (str[len-2] == '\r') && (str[len-1] = '\n')) {
		str[len-2] = '\n';
		str[len-1] = '\0';
		len--;
	}
	return len;
}

/**
 * Read the definition into the def buffer up to size characters.
 * Subsequent lines are ignored.
 *
 * Chambers 20th Century Dictionary entries as read from the file
 * are of the form:
 *
 * SAKE, sak'e, _n._ a Japanese fermented liquor made from rice: a generic\r\n
 * name for all spirituous liquors.\r\n
 * \r\n
 *
 * The first line has the defined word followed by comma. Definitions
 * can have multiple lines. Entries are separated by an empty line.
 * Definitions returned have "\r\n" normalize to C style "\n" and no
 * blank line.
 *
 * @param file the input File
 * @param def the input buffer for the definition with initial text
 * @param size the maximum definition size including '\0' terminator
 * @return length of definition read; -1 if error
 */
int readDefinition(FILE* file, char def[], int size) {
	int deflen  = strlen(def); // current definition length

	while (!feof(file)) {
		char line[MAX_LINE];
		int linelen = readLine(file, line, MAX_LINE);
    	if (linelen < 0) {
    		return -1;
    	}

    	// end of definition if line is empty
		if ((line[0] == '\n') || (line[0] == '\0')) {
			break;
		}

		// add line to definition if would not exceed size
		if (deflen+linelen < size) {
			strcpy(def+deflen, line);
		}
		deflen += linelen;
    }

	// record size of longest definition read
	if (max_def_len < deflen) {
		max_def_len = deflen;
	}

	// count truncated definitions
	if (deflen > size) {
		defs_truncated++;
		deflen = strlen(def);
	}
    return deflen;
}

/**
 * Read definitions from input stream, starting with the first_word
 * and ending with the last_word in the dictionary and build a dictionary
 * with the words and their definitions.
 *
 * @param file the input FILE
 * @param first_word the first word whose definition is added to the map
 * @param last_word the last word whose definition is added to the map
 * @return number of entries added to dictionary;
 */
int readChambers_20th_CenturyDictionary(
	FILE* file, const char first_word[], const char last_word[]) {

	// line buffer
    char line[MAX_LINE];
    int count = 0;

    // find line with first word to process
	int wordlen = strlen(first_word);
	bool found = false;
    while (!found && (readLine(file, line, MAX_LINE) >= 0)) {
        if (strncmp(first_word, line, wordlen) == 0) {
        	found = (line[wordlen] == ',');
        }
    }

    if (found) {  // found first word
		char def[MAX_DEF];  // space for reading definitions
		do {
			// get definition of current word
			strcpy(def, line); // copy first line to definition
			int deflen = readDefinition(file, def, MAX_DEF);
			if (deflen < 0) {
				break;
			}

			// definition valid if it begins with word followed by ","
			int wordlen = 0;
			for (int i = 0; i < deflen; i++) {
				if (def[i] == ',') {
					wordlen = i;
					break;
				}
			}

			// process word if found
			if (wordlen > 0) {
				// done if found last word
				found = (strncmp(last_word, def, wordlen) == 0);

				// skip if word too long
				if (wordlen >= MAX_WORD) {
					words_skipped++;
					continue;
				}

				// extract the word being defined
				char word[MAX_WORD];
				strncpy(word, def, wordlen);
				word[wordlen] = '\0';  // terminate word string

				// add word and definition to the dictionary
				if (putDictionaryEntry(word, def) < 0) {
					break; // too many entries
				}
				count++;

			}
		} while (!found && (readLine(file, line, MAX_LINE) >= 0));
    }
    return count;
}

/**
 * Build dictionary from entries in Chambers's Twentieth Century Dictionary
 * (part 4 of 4: S-Z and supplements) hosted by Project Gutenberg at
 * http://www.gutenberg.org/cache/epub/38700/pg38700.txt .
 *
 * Interactive command interpreter recognizes commands #word[*] (print number
 * of matching words, =word[*] (print matching words), and ?word[*] (print
 * matching definitions). Word ending with wildcard indicates all words with
 * matching prefixes.
 *
 * @return true if dictionary successfully loaded, false otherwise
 */
bool runChambers_20th_CenturyDictionary(void) {
	// read piped output of curl
	// see http://stackoverflow.com/questions/26648857/can-fopen-be-used-to-open-the-url
    printf("\nOpening Chambers's Twentieth Century Dictionary (part 4 of 4: S-Z and supplements)\n");
	FILE* file = popen("curl -s 'http://www.gutenberg.org/cache/epub/38700/pg38700.txt'", "r");
	if (file == NULL) {
		printf("...Error opening dictionary\n");
		return false;
    }


	char first_word[] = "SAB";    // first definition to load
	char last_word[] = "SYZYGY";  // last definition to load
	printf("...Loading definitions from '%s' to '%s'\n", first_word, last_word);
	int n_entries = readChambers_20th_CenturyDictionary(file, first_word, last_word);
	pclose(file);

	printf("...Loaded %d definitions, skipped: %d, truncated: %d, longest: %d\n",
			n_entries, words_skipped, defs_truncated, max_def_len);
	if (n_entries == 0) {  // error if no definitions loaded
		return false;
	}

	char cmd[MAX_LINE] = "\n";
	int cmdlen = 1;
	do {
		cmd[cmdlen-1] = '\0';  // eliminate newline character

		if (strcmp("quit", cmd) == 0) {
			printf("Done.\n");
			break;
		}

		if ((cmd[0] == '#') || (cmd[0] == '=') || (cmd[0] == '?')) {
			char target_word[MAX_WORD];
			strcpy(target_word, cmd+1);  // skip command char

			int def_count = 0;
			int entry = getDictionaryEntry(target_word, 0);
			while (entry >= 0) {
				if (cmd[0] == '=') {         // print word
					char word[MAX_WORD];
					getDictionaryWord(entry, word);
					printf("\n%s", word);
				} else if (cmd[0] == '?') {  // print definition
					char def[MAX_DEF];
					getDictionaryDefinition(entry, def);
					printf("\n%s", def);
				}
				def_count++;
				entry = getDictionaryEntry(target_word, entry+1);
			}

  			if (cmd[0] == '#') {                  // print count
				printf("%d\n", def_count);
			} else if (def_count == 0) {          // no matches for word or def query
				printf("No definitions match '%s'\n", target_word);
			}
		} else {
			// unknown command: show command list
			printf("Commands: \n");
		    printf("  Count words: #<word> or #<prefix>*\n");
			printf("  List words: =<word> or =<prefix>*\n");
		    printf("  List definitions: ?<word> or ?<prefix>*\n");
			printf("  Quit: quit\n");
		}

		printf("\n> ");  // prompt
	} while ((cmdlen = readLine(stdin, cmd, MAX_LINE)) >= 0);

	pclose(file);
	return true;
}

/**
 * Set up unit test framework and run tests.
 *
 * @return CUE_SUCCESS if all tests successful
 */
CU_ErrorCode runTests(void) {
	// initialize the CUnit test registry -- only once per application
	CU_initialize_registry();

	// add dictionary test suite
	test_dictionary();

	// run all test suites using the basic interface
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

	// display information on failures that occurred
	CU_basic_show_failures(CU_get_failure_list());

	// Clean up registry and return status
	CU_cleanup_registry();
	CU_ErrorCode code = CU_get_error();
	return code;
}

/**
 * Run testChambers_20th_CenturyDictionary().
 *
 * @return EXIT_SUCCESS if dictionary loaded, EXIT_FAILURE if error
 */
int main(int argc, char** argv) {
	setbuf(stdout,NULL);

	// look for '-test' argument and run test (workaround for bug in Eclipse debugging)
	if ((argc == 2) && ((strcmp(argv[1], "-test") ==0) || (strcmp(argv[1], "\"-test\"") == 0))) {
		CU_ErrorCode code = runTests();
		return (code == CUE_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	if (!runChambers_20th_CenturyDictionary()) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
