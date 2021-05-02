//*********************
// Exercise  3
//
// Name: Matanel Pataki
//
// ID: 205951023
//
// A program that generates Twitter tweets.
//*********************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//values:
#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define ARG_NUM_NO_LIMIT 4
#define ARG_NUM_WITH_LIMIT 5
#define BASE_TEN 10

//messages:
#define ERR_MSG_USAGE "Usage: <seed>,<num_of_tweets>,<file>,<o-num_of_words>\n"
#define ERR_MSG_BAD_FILE "Error: Invalid file.\n"
#define TWEET_INDEX "Tweet %d: "
#define ERR_MSG_ALLOCATION "Allocation failure: Exiting."

//STRUCTS:

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list;
  int *next_word_idx_array;
  int num_of_occurrences;
  int prob_list_size;
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  int num_of_occurrences;
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
/*************************************/

/**
 * checking memory allocation. Exiting the program if there's a problem.
 * @param ptr a pointer to a heep address.
 */
void check_alloc (void *ptr)
{
  if (!ptr)
    {
      printf (ERR_MSG_ALLOCATION);
      exit (1);
    }
}

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add (LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc (sizeof (Node));
  check_alloc (new_node);
  if (new_node == NULL)
    {
      exit (1);
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

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number (int max_number)
{
  return rand () % max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word (LinkList *dictionary)
{
  if (!dictionary)
    {
      return NULL;
    }
  int rand_num = get_random_number (dictionary->size + 1);
  WordStruct *word = NULL;
  Node *traveller = dictionary->first;
  int counter = 0;
  //run through the dictionary, getting random word to return:
  while (traveller)
    {
      if (counter == rand_num)
        {
          word = traveller->data;
          if (word->prob_list)
            {
              return word;
            }
        }
      traveller = traveller->next;
      counter++;
    }
  return NULL;
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency.
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word (WordStruct *word_struct_ptr)
{
  if (!word_struct_ptr->next_word_idx_array)
    {
      return NULL;
    }
  int rand_num = get_random_number (word_struct_ptr->num_of_occurrences);
  return word_struct_ptr->prob_list[word_struct_ptr->next_word_idx_array
  [rand_num]].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence (LinkList *dictionary)
{
  WordStruct *first_word = get_first_random_word (dictionary);
  while (!first_word)
    {
      first_word = get_first_random_word (dictionary);
    }
  printf ("%s", first_word->word);
  int words_counter = 1;
  WordStruct *next_word = NULL;
  next_word = get_next_random_word (first_word);
  do
    {
      printf (" %s", next_word->word);
      words_counter++;
      next_word = get_next_random_word (next_word);
    }
  while (next_word
         && words_counter < MAX_WORDS_IN_SENTENCE_GENERATION - 1);
  printf ("\n");
  return words_counter;
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
  if (!first_word || !second_word)
    {
      return 1;
    }
  int i = 0;
  for (; i < first_word->prob_list_size; ++i)
    {
      if (!strcmp (first_word->prob_list[i].word_struct_ptr->word,
                   second_word->word))
        {
          break;
        }
    }
  if (i != first_word->prob_list_size)
    {
      ++(first_word->prob_list[i].num_of_occurrences);
      return 0;
    }
  WordProbability *tmp = realloc (first_word->prob_list,
                                  sizeof (WordProbability)
                                  * (first_word->prob_list_size + 1));
  check_alloc (tmp);
  first_word->prob_list = tmp;
  first_word->prob_list[first_word->prob_list_size++] =
      (WordProbability) {second_word, 1};
  return 1;
}

/**
 * checking if a word is in the dictionary according to the letters given.
 * @param letters char *.
 * @param dictionary the current linked list dictionary.
 * @return a pointer to the word if already exists, Null otherwise.
 */
WordStruct *is_in_dictionary (char *letters, LinkList *dictionary)
{
  Node *p = dictionary->first;
  for (int i = 0; i < dictionary->size; i++)
    {
      if (!strcmp (p->data->word, letters))
        {
          return p->data;
        }
      p = p->next;
    }
  return NULL;
}

bool not_end_of_sentence (const WordStruct *pre_word)
{ return pre_word->word[strlen (pre_word->word) - 1] != '.'; }

/**
 * creating a new word to add to dictionary
 * @param dictionary the current dictionary.
 * @param cur_letters the current letters of the word.
 * @param pre_word the previous word.
 * @param cur_word the current word.
 * @return the new word.
 */
WordStruct *create_new_word (LinkList *dictionary, const char *cur_letters,
                             WordStruct *pre_word, WordStruct *cur_word)
{
  cur_word = malloc (sizeof (WordStruct));
  check_alloc (cur_word);
  char *word_letters = malloc (MAX_WORD_LENGTH);
  check_alloc (word_letters);
  strcpy (word_letters, cur_letters);
  cur_word->word = word_letters;
  cur_word->prob_list = NULL;
  cur_word->prob_list_size = 0;
  cur_word->num_of_occurrences = 1;
  if (pre_word && not_end_of_sentence (pre_word))
    {
      add_word_to_probability_list (pre_word, cur_word);
    }
  add (dictionary, cur_word);
  return cur_word;
}
/**
 * get the next letters from the file.
 * @param tweet the current line to read from
 * @param cur_letters the cur_letters
 * @param got_new_line indicator if it is a new line
 * @return the new current letters.
 */
char *get_next_letters (char *tweet, char *cur_letters, int *got_new_line)
{
  if (*got_new_line)
    {
      cur_letters = strtok (tweet, " \n");
      *got_new_line = 0;
    }
  else
    {
      cur_letters = strtok (NULL, " \n");
    }
  return cur_letters;
}

/**
 * fill an array according to the occurrences of the word.
 * will later get a random index from that array witch will return a random
 * word.
 * @param word the word to create the index array for.
 */
void fill_next_word_idx_array (WordStruct *word)
{
  if (word->prob_list_size == 0)
    { //in case this is a word with .
      word->next_word_idx_array = NULL;
      return;
    }
  word->next_word_idx_array = malloc (sizeof (int) * word->num_of_occurrences);
  check_alloc (word->next_word_idx_array);
  int counter = 0;
  for (int i = 0; i < word->prob_list_size; ++i)
    {
      for (int j = 0; j < word->prob_list[i].num_of_occurrences; ++j)
        {
          word->next_word_idx_array[counter++] = i;
        }
    }
}

/**
 * updating the current word
 * @param pre_word the previous word.
 * @param cur_word  the current word.
 */
void update_word (WordStruct *pre_word, WordStruct *cur_word)
{
  if (pre_word && not_end_of_sentence (pre_word))
    {
      add_word_to_probability_list (pre_word, cur_word);
    }
  cur_word->num_of_occurrences++;
}

/**
 * create and fill for each word in the dictionary its index array.
 * @param dictionary the current dictionary.
 */
void fill_dictionary_indexes_arrays (LinkList *dictionary)
{
  Node *cur_node = dictionary->first;
  while (cur_node)
    {
      fill_next_word_idx_array (cur_node->data);
      cur_node = cur_node->next;
    }
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
  char *cur_letters = "";
  int counter = 0;
  WordStruct *pre_word = NULL;
  WordStruct *cur_word = NULL;
  int got_new_line = 0;

  while ((counter != words_to_read) && fgets (tweet, MAX_SENTENCE_LENGTH, fp))
    {
      got_new_line = 1;
      while ((counter != words_to_read) &&
             (cur_letters = get_next_letters (tweet, cur_letters,
                                              &got_new_line)))
        {
          pre_word = cur_word;
          //checking if the word is already on the dictionary:
          cur_word = is_in_dictionary (cur_letters, dictionary);
          if (cur_word)
            {
              update_word (pre_word, cur_word);
              counter++;
              continue;
            }
          //creating new word to add to dictionary:
          cur_word = create_new_word (dictionary, cur_letters,
                                      pre_word, cur_word);
          counter++;
        }
    }
  fill_dictionary_indexes_arrays (dictionary);

}

/**
 * freeing the string of each word, its prob-list and the word struct itself.
 * @param word_ptr the current word to free.
 */
void free_word_struct (WordStruct *word_ptr)
{
  free (word_ptr->word);
  free (word_ptr->next_word_idx_array);
  free (word_ptr->prob_list);
  free (word_ptr);
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary (LinkList *dictionary)
{
  Node *del = dictionary->first;
  Node *tmp = NULL;
  while (del)
    {
      tmp = del->next;
      free_word_struct (del->data);
      free (del);
      del = tmp;
    }
  free (dictionary);
}

/**
 * checking user input
 * @param argc num of arguments
 * @param argv arguments array
 * @param words_to_read the amount of words to read from file.
 * @return 1 when error, 0 otherwise.
 */
int check_user_input (int argc, char *const *argv, int *words_to_read)
{
  if (argc == ARG_NUM_NO_LIMIT)
    {
      *words_to_read = -1;
    }
  else if (argc == ARG_NUM_WITH_LIMIT)
    {
      *words_to_read = (int) strtol (argv[4], NULL, BASE_TEN);
    }
  else
    {
      printf (ERR_MSG_USAGE);
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
/**
 * initializing variables.
 * @param argc num of arguments
 * @param argv arguments array
 * @param num_of_tweets number of tweets to generate.
 * @param fp_corpus a pointer to the file.
 * @return 1 when error, 0 otherwise.
 */
int initialize_variables (char *const *argv, unsigned int *seed,
                          int *num_of_tweets, FILE **fp_corpus)
{
  (*seed) = (unsigned int) strtol (argv[1], NULL, BASE_TEN);
  (*num_of_tweets) = (int) strtol (argv[2], NULL, BASE_TEN);
  (*fp_corpus) = fopen (argv[3], "r");
  if ((*fp_corpus) == NULL)  // check success of opening file
    {
      printf (ERR_MSG_BAD_FILE);
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

/**
 * generating the amount of tweets asked by  the user.
 * @param seed for creating the random tweets.
 * @param num_of_tweets number of tweets to generate.
 * @param dictionary the current dictionary to generate from.
 */
void generate_all_tweets (unsigned int seed, int num_of_tweets,
                          LinkList *dictionary)
{
  srand (seed);
  for (int i = 0; i < num_of_tweets; i++)
    {
      printf (TWEET_INDEX, i + 1);
      generate_sentence (dictionary);
    }
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
  //declare variables:
  unsigned int seed = 0;
  int num_of_tweets = 0;
  int words_to_read = 0;
  FILE *fp_corpus = NULL;
  int return_value = EXIT_SUCCESS;

  //check user's input:
  return_value = check_user_input (argc, argv, &words_to_read);
  if (return_value == EXIT_FAILURE)
    {
      return return_value;
    }
  //if user's input is ok, initialize variables:
  return_value = initialize_variables (argv, &seed,
                                       &num_of_tweets, &fp_corpus);
  if (return_value == EXIT_FAILURE)
    {
      return return_value;
    }

  //declaring and allocating memory for new dictionary:
  LinkList *dictionary = malloc (sizeof (LinkList));
  check_alloc (dictionary);

  //filling dictionary according to given file:
  fill_dictionary (fp_corpus, words_to_read, dictionary);

  //generating tweets according to user's input:
  generate_all_tweets (seed, num_of_tweets, dictionary);

  //freeing dictionary, and closing file:
  free_dictionary (dictionary);
  fclose (fp_corpus);
  dictionary = NULL;

  return EXIT_SUCCESS;
}