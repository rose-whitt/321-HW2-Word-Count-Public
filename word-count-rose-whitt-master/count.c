/*
 * COMP 321 Project 2: Word Count
 *
 * This program counts the characters, words, and lines in one or more files,
 * depending on the command-line arguments.
 * 
 * Rose Whitt; rew9
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "csapp.h"

/* 
 * This is a structure that is used to track the character,
 * word, and line counts in a given file.
 */
struct counts {
	int   char_count;
	int   word_count;
	int   line_count;
};

// This is a structure that tracks file name and count values.
struct file_count {
	char 	*file_name;	// Reference to the file's name.
	struct 	counts count;	// Information about the file's counts.
};


// A linked list node
struct node {
    struct file_count data;	// Information about the file.
    struct node* next;		// Reference to the next node.
    struct node* prev;		// Reference to the previous node.
};
	
static void	app_error_fmt(const char *fmt, ...);
static int	do_count(char *input_files[], const int nfiles,
		    const bool char_flag, const bool word_flag,
		    const bool line_flag, const bool test_flag);
static void	print_counts(FILE *fp, struct counts *cnts, const char *name,
		    const bool char_flag, const bool word_flag,
		    const bool line_flag);

/*
 * Requires:
 *   The first argument, "fmt", is a printf-style format string, and all
 *   subsequent arguments must match the types of arguments indicated in the
 *   format string.
 *
 * Effects:
 *   Prints a formatted error message to stderr using the supplied
 *   format string and arguments.  The message is prepended with
 *   the string "ERROR: " and a newline is added at the end.
 */
static void
app_error_fmt(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "ERROR: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

/*
 * Requires:
 *   The "fp" argument must be a valid FILE pointer.
 *   The "cnts" argument must not be NULL and must point to an allocated
 *   structure of type "struct counts".
 *   The "name" argument must not be NULL and must point to an allocated
 *   string.
 *
 * Effects:
 *   Prints the character, word, and line counts for a particular file
 *   as directed by the flags.
 */
static void
print_counts(FILE *fp, struct counts *cnts, const char *name,
    const bool char_flag, const bool word_flag, const bool line_flag)
{

	if (line_flag)
		fprintf(fp, "%8d", cnts->line_count);
	if (word_flag)
		fprintf(fp, "%8d", cnts->word_count);
	if (char_flag)
		fprintf(fp, "%8d", cnts->char_count);
	fprintf(fp, " %s\n", name);
}


/*
 * Requires:
 *	The "head" argument must not be NULL and must point to an
 *	allocated structure of type "struct node**".
 *	The "new_data" argument must not be NULL and must point to
 *	an allocated structure of type "struct file_count".
 *
 * Effects:
 *	Given a reference to the head of a list, creates
 *	a new node from the inputted file_count structure
 *	and appends this new node at the end.
 */
static void
append(struct node** head, struct file_count new_data)
{
	/* Allocate malloc memory for the new node. */
        struct node* new_node = (struct node*) malloc(sizeof(struct node));
        
        /* Create a 'last' node pointing to the head for later traversal. */
        struct node *last = *head;
        
        new_node->data = new_data;	// Assign the data to the new node.
                          
        /* 3. This new node is going to be the last node, so make next of
                                        it as NULL*/
        /* 
         * Since the new node is appended to the end,
         * make its next pointer be null.
         */
        new_node->next = NULL;
        
        /* If the Linked List is empty, the new node becomes the head node. */
        if (*head == NULL) {
		*head = new_node;
       		return;			// Terminate the function.
        }
        
        /*
         * If the Linked List is not empty, traverse until you reach the last node.
         */
        while (last->next != NULL) {
        	last = last->next;	// Update the last node as you go.
	}
	
	new_node->prev = last;		// Change the prev of the new node.
	
	last->next = new_node;		// Change the next of the new node.
	
	return;		// Terminate the function.
}                              


/*
 * Requires:
 *	The "head" argument must not be NULL and must point to an
 *	allocated structure of type "struct node*".
 *
 * Effects:
 *	Prints out the results in ASCIIbetical order based on filename.
 */
static void
print_sort (struct node* head, const bool char_flag, const bool word_flag, const bool line_flag)
{
	struct node* current;	// Declare a 'current' node for iteration.
	
	while (head != NULL) {	// Iterate until no nodes are left.
		/* 
		 * Create a 'min' node with an initial
		 *  reference to the head node.
		 */
		struct node* min = head;
		current = head;	// Assign the head node to the current node.
		
		// First, find the minimum (lowest ASCII value) file name.
		while (current != NULL) {
			/*
			 * If not in asciibetical order,
			 * make the current node the new min node.
			 */
			if (strcmp(min->data.file_name, current->data.file_name) > 0) {
				min = current;
			}
			/* Update current to continue iterating through.
			 * When current's next is NULL, current is the tail.
			 */
			current = current->next;
		}
		
		
		// Print the information of the lowest ASCIIbetical file.
		print_counts(stdout, &min->data.count, min->data.file_name
			, char_flag, word_flag, line_flag);
		
		
		
		// Store the references to min's next and prev nodes.
		struct node* prev = min->prev;
		struct node* next = min->next;
		
		/*
		 * Remove the current minimum node from the Linked List.
		 */
		
		// Make 'head' NULL when min is the only node in the list.
		if ((min == head) && (next == NULL)) {
			head = NULL;	// This terminates the sorting.
		} else if (min == head) {
			// This occurs when the min is the head.
			head = next;	// Set the head to be the next node.
			head->prev = NULL;
		} else if (next == NULL) {
			// This occurs when min is the last (aka tail) node.
			prev->next = NULL;
		} else {
			// This occurs when min is somewhere in the middle.
			prev->next = next;
			next->prev = prev;
		}
		
		Free(min);	// free min from memory
	}

}

                              
/*                         
 * Requires:
 *   The "input_files" argument is an array of strings with "nfiles" valid
 *   strings.
 *
 * Effects: 
 *   Prints to stdout the counts for each file in input_files in
 *   alphabetical order followed by the total counts.  The
 *   "char/word/line_flag" arguments indicate which counts should be
 *   printed on a file-by-file basis.  The total count will include all
 *   three counts regardless of the flags.  An error message is printed
 *   to stderr for each file that cannot be opened.
 *
 *   Returns 0 if every file in input_files was successfully opened and
 *   processed.  If, however, an error occurred opening or processing any
 *   of the files, a small positive integer is returned.
 *
 *   The behavior is undefined when "test_flag" is true.
 */
static int
do_count(char *input_files[], const int nfiles, const bool char_flag,
    const bool word_flag, const bool line_flag, const bool test_flag)
{
	
	int e = 0;	// This is the error value.
	
	// Make an instance of the counts struct to store the totals.
	struct counts count_total = {0, 0, 0};
	
	
	struct node* head = NULL; // Initialize the head node to be NULL.
	

	for (int i = 0; i < nfiles; i++) {	// Iterate over each file.
		// Open the current file for reading purposes.
		FILE *cur_file = fopen(input_files[i], "r");
		
		// Catch the error when the file cannot be opened.
		if (cur_file == NULL) {
			app_error_fmt("cannot open file \'%s\'", input_files[i]);
			e = 1;		// Set the error value to 1.
			// Continue to the next file after reporting the error.
			continue;
		}
		
		
		/*
		 * Create an instance of the file_count structure for the
		 * current file.
		 */
		struct file_count cur_file_count;
		cur_file_count.file_name = input_files[i];
		cur_file_count.count.char_count = 0;
		cur_file_count.count.word_count = 0;
		cur_file_count.count.line_count = 0;
		
		// This is for testing the cannot read file error. (IGNORE).
		if (test_flag) {
			close(fileno(cur_file));
		}
		
		int prev_c = 32;	// Make prev_c initially a space.
		int c;		// Create the character for the while loop.
		
		/*
		 * Iterate over each character in the file until
		 * the end of the file is reached.
		 */
		while ((c = fgetc(cur_file)) != EOF) {
			
			// Update the total and current file character count.
			count_total.char_count += 1;
			cur_file_count.count.char_count += 1;
												
			/*
			 * This checks if we have begun a new word, meaning
			 * we should update the word counts to account for
			 * the previous word. A word is detected not just
			 * when the character is not a space, but when the
			 * character is not a space and the previous
			 * character was a space.
			 */
			if (!isspace(c) && isspace(prev_c)) {
				count_total.word_count += 1;
				cur_file_count.count.word_count += 1;
			
			}
			
			/*
			 * Update the line counts when the current character
			 * is the newline character.
			 */
			if (c == '\n') {
				count_total.line_count += 1;
				cur_file_count.count.line_count += 1;
			}
			
			/*
			 * Set the previous character to your current
			 * character for the next iteration.
			 */
			prev_c = c;
		}
		
		// Catch the error when the file cannot be read.
		if (!feof(cur_file)) {
			app_error_fmt("cannot read file \'%s\'", input_files[i]);
			e = 1;
		}
		
		/*
		 * Append your current file's node to the Linked List.
		 */
		append(&head, cur_file_count);
		
		
	}
	
	// Print the counts in ASCIIbetical order.
	print_sort(head, char_flag, word_flag, line_flag);
	
	
	/*
	 * If a flag is false, we still want to print it,
	 * but its count value should be zero!!!
	 */
	if (!char_flag) {
		count_total.char_count = 0;
	}
	if (!word_flag) {
		count_total.word_count = 0;
	}
	if (!line_flag) {
		count_total.line_count = 0;
	}
	
	// Print the total counts.
	print_counts(stdout, &count_total, "total", true, true, true);
	
	// Return the error value.
	return (e);
}

/*
 * Requires:
 *   Nothing.
 *
 * Effects:
 *   Parses command line arguments and invokes do_count with the appropriate
 *   arguments based on the command line.
 */
int 
main(int argc, char **argv)
{
	int c;			// Option character
	extern int optind;	// Option index

	// Abort flag: Was there an error on the command line? 
	bool abort_flag = false;

	// Option flags: Were these options on the command line?
	bool char_flag = false;
	bool line_flag = false;
	bool test_flag = false;
	bool word_flag = false;

	// Process the command line arguments.
	while ((c = getopt(argc, argv, "cltw")) != -1) {
		switch (c) {
		case 'c':
			// Count characters.
			if (char_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				char_flag = true;
			}
			break;
		case 'l':
			// Count lines.
			if (line_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				line_flag = true;
			}
			break;
		case 't':
			// Enable test flag.
			if (test_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				test_flag = true;
			}
			break;
		case 'w':
			// Count words.
			if (word_flag) {
				// A flag can only appear once.
				abort_flag = true;
			} else {
				word_flag = true;
			}
			break;
		case '?':
			// An error character was returned by getopt().
			abort_flag = true;
			break;
		}
	}
	if (abort_flag || optind == argc) {
		/*
		 * In this case, use fprintf instead of app_error_fmt because
		 * error messages describing how to run a program typically
		 * begin with "usage: " not "ERROR: ".
		 */
		fprintf(stderr,
		    "usage: %s [-c] [-l] [-t] [-w] <input filenames>\n",
		    argv[0]);
		return (1);	// Indicate an error.
	}

	// Process the input files, and return the result.
	return (do_count(&argv[optind], argc - optind, char_flag, word_flag,
	    line_flag, test_flag));
}
