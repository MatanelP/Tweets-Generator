//*********************
// Exercise  3
//
// Name: Matanel Pataki
//
// ID: 205951023
//
// A program that generates Twitter twits.
//*********************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list;
  int prob_list_size;
  //... Add your own fields here
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  int num_of_occurences;
  //... Add your own fields here
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
  WordStruct *data;
  struct Node *next;
} Node;

typedef struct LinkList {
  Node *first;
  Node *last;
  int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add (LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc (sizeof (Node));
  if (new_node == NULL)
    {
      return 1;
    }
  *new_node = (Node) {data, NULL};

  if (link_list->first == NULL)
    {
      link_list->first = new_node;
      link_list->last = new_node;
    }
  else
    {
      link_list->last->next = new_node;
      link_list->last = new_node;
    }

  link_list->size++;
  return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number (int max_number)
{
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word (LinkList *dictionary)
{
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word (WordStruct *word_struct_ptr)
{
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence (LinkList *dictionary)
{
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list (WordStruct *first_word,
                                  WordStruct *second_word)
{
  WordProbability *current = first_word->prob_list;
  for (size_t i = 0; i < first_word->prob_list_size; i++)
    {
      if (!strcmp (current->word_struct_ptr->word, second_word->word))
        {
          current->num_of_occurences++;
          return 0;
        }
      current++;
    }
  WordProbability *tmp = realloc (first_word->prob_list,
                                  sizeof (WordProbability)
                                  * (first_word->prob_list_size + 1));
  if (!tmp) //todo: how to exit the program if its null?
    {
      free (first_word->prob_list);
      first_word->prob_list = NULL;
      //todo: how to exit the progrm if its null?
    }
  first_word->prob_list = tmp;
  first_word->prob_list_size++;
  current->word_struct_ptr = second_word;
  current->num_of_occurences = 1;
//  if (!strcmp (first_word->word, "is"))
//    {
//      printf ("%s\n%d\n",current->word_struct_ptr->word,first_word->prob_list_size);
//    }
  return 1;
}

WordStruct *is_in_dictionary (char *letters, LinkList *p_list)
{
  Node *p = p_list->first;
  for (size_t i = 0; i < p_list->size; i++)
    {
      if (!strcmp (p->data->word, letters))
        {
          return p->data;
        }
      p = p->next;
    }
  return NULL;
}
/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary (FILE *fp, int words_to_read, LinkList *dictionary)
{
  char tweet[MAX_SENTENCE_LENGTH];
  fgets (tweet, MAX_SENTENCE_LENGTH, fp);
  char *word_letters = strtok (tweet, " ");
  WordStruct *last_word = NULL;
  for (size_t i = 0; i < words_to_read; i++)
    {
      WordStruct *word = is_in_dictionary (word_letters, dictionary); //gets a pointer to the already existing word in dictionary, null otherwise.
      if (word)
        {
          add_word_to_probability_list (last_word, word);
          last_word = word;
          word_letters = strtok (NULL, " ");
          continue;
        }
      word = malloc (sizeof (WordStruct)); //todo: global variable in case malloc is failing.
      WordProbability *word_probability = malloc (0);

      word->word = word_letters;
      word->prob_list = word_probability; //todo: should point to null if its a final word in a sentence.
      word->prob_list_size = 0;

      add (dictionary, word);
      if (last_word)
        {
          add_word_to_probability_list (last_word, word);
        }
      last_word = word;

      word_letters = strtok (NULL, " ");
    }
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary (LinkList *dictionary)
{
}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main (int argc, char *argv[])
{
  int words_to_read = 0;
  if (argc == 4)
    {
      words_to_read = -1;
    }
  else if (argc == 5)
    {
      words_to_read = (int) strtol (argv[4], NULL, 10);
    }
  else
    {
      printf ("Usage: <seed>,<number of sentences>,<file>,<optional - number of words>\n");
      return EXIT_FAILURE;
    }



//  srand (argv[1]);

  FILE *fp_corpus = fopen (argv[3], "r");
  if (fp_corpus == NULL)  // check success of opening file
    {
      printf ("Error: Invalid file.\n");
      return EXIT_FAILURE;
    }

  LinkList *dictionary = malloc (sizeof (Node) * words_to_read);
  fill_dictionary (fp_corpus, words_to_read, dictionary);

  return 0;
}