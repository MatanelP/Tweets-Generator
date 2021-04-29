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

int mallocs = 0;

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list;
  int num_of_occurences;
  int prob_list_size;
  //... Add your own fields here
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  int num_of_occurences;
  struct WordProbability *next;
//  struct WordProbability *last;
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

/*************************************/

void check_alloc (void *ptr)
{
  mallocs++;
  if (!ptr)
    {
      printf ("Allocation failure: Exiting.");
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
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word (WordStruct *word_struct_ptr)
{
  int rand_num = get_random_number (word_struct_ptr->num_of_occurences);
  int counter = 0;
  int revise = 0;
  WordProbability *traveller = word_struct_ptr->prob_list;
  while (traveller)
    {
      if (counter == rand_num)
        {
          return traveller->word_struct_ptr;
        }
      if (traveller->num_of_occurences - revise > 1)
        {
          counter++;
          revise++;
          continue;
        }
      revise = 0;
      traveller = traveller->next;
      counter++;
    }
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
  printf ("%s ", first_word->word);
  WordStruct *next_word = get_next_random_word (first_word);
  int words_counter = 1;
  while (next_word->prob_list
         && words_counter < MAX_WORDS_IN_SENTENCE_GENERATION - 1)
    {
      printf ("%s ", next_word->word);
      words_counter++;
      next_word = get_next_random_word (next_word);
    }
  printf ("%s\n", next_word->word);
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
//todo: ask if you can implement the Word_Probability dynamic array as a linked list.
{
  if (!first_word || !second_word)
    {
      return 1;
    }
  if (!first_word->prob_list) //if prob_list is empty, start new one
    {
      WordProbability *head_word_prob = malloc (sizeof (WordProbability));
      check_alloc (head_word_prob);
      if (!head_word_prob)
        {
          exit (1);
        }

      first_word->prob_list = head_word_prob;
      first_word->prob_list_size = 1;
      head_word_prob->word_struct_ptr = second_word;
      head_word_prob->num_of_occurences = 1;
      head_word_prob->next = NULL;
//      first_word->prob_list->last = head_word_prob;
      return 1;
    }

  WordProbability *current = first_word->prob_list;
  // check if the second word is already in the first's prob_list.
  while (current)
    {
      if (!strcmp (current->word_struct_ptr->word, second_word->word))
        {
          current->num_of_occurences++;
          return 0;
        }
      current = current->next;

    }

  // if not then add the second word into the first's prob_list.
  //todo: do i even need it?
  WordProbability *tmp = realloc (first_word->prob_list,
                                  sizeof (WordProbability)
                                  * (first_word->prob_list_size + 1));

  check_alloc (tmp);
  first_word->prob_list = tmp;
  first_word->prob_list_size++;

  WordProbability *new_word_prob = malloc (sizeof (WordProbability));
  check_alloc (new_word_prob);
  new_word_prob->word_struct_ptr = second_word;
  new_word_prob->num_of_occurences = 1;
  new_word_prob->next = first_word->prob_list;
  first_word->prob_list = new_word_prob;

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
  char *cur_letters = "";
  size_t counter = 0;
  WordStruct *pre_word = NULL;
  WordStruct *cur_word = NULL;
  int got_new_line = 0;

  while (fgets (tweet, MAX_SENTENCE_LENGTH, fp))
    {
      got_new_line = 1;
      while (1)
        {
          pre_word = cur_word;
          if (got_new_line)
            {
              cur_letters = strtok (tweet, " \n");
              got_new_line = 0;
            }
          else
            {
              cur_letters = strtok (NULL, " \n");
            }
          //checking if got to end of line:
          if (!cur_letters)
            {
              break;
            }

          //checking if the word is already on the dictionary:
          cur_word = is_in_dictionary (cur_letters, dictionary);
          if (cur_word)
            {
              if (pre_word
                  && pre_word->word[strlen (pre_word->word) - 1] != '.')
                {
                  add_word_to_probability_list (pre_word, cur_word);
                }
              cur_word->num_of_occurences++;
              continue;
            }

          //creating new word to add to dictionary:
          cur_word = malloc (sizeof (WordStruct));
          check_alloc (cur_word);
          char *word_letters = malloc (MAX_WORD_LENGTH);
          check_alloc (word_letters);
          strcpy (word_letters, cur_letters);
//          free (cur_letters);
          cur_word->word = word_letters;
          cur_word->prob_list = NULL;
          cur_word->prob_list_size = 0;
          cur_word->num_of_occurences = 1;
          if (pre_word && pre_word->word[strlen (pre_word->word) - 1] != '.')
            {
              add_word_to_probability_list (pre_word, cur_word);
            }
          add (dictionary, cur_word);

          counter++;
          if (counter == words_to_read)
            {
              return;
            }

        }

    }
}

void free_prob_struck (struct WordProbability *prob_struck)
{
  WordProbability *del = prob_struck;
  WordProbability *tmp = NULL;
  while (del)
    {
      tmp = del->next;
      free (del);
      del = tmp;
    }
}
void free_word_struct (WordStruct *word_ptr)
{
  WordStruct *del = word_ptr;
  free (del->word);
  free_prob_struck (del->prob_list);
  free (del);
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

void print_dictionary (LinkList *dictionary)
{
//  printf ("dictionary size is: %i\n", dictionary->size);
  if (!dictionary)
    {
      return;
    }
  Node *traveller = dictionary->first;
  int counter = 1;
  while (traveller)
    {
      printf ("%d) word: %s - %d\n\tprob list - %d: \t", counter, traveller->data->word, traveller->data->num_of_occurences, traveller->data->prob_list_size);
      WordProbability *prob = traveller->data->prob_list;
      int prob_counter = 1;
      while (prob)
        {
          printf ("(%d)\t %s - %d\t", prob_counter, prob->word_struct_ptr->word, prob->num_of_occurences);
          prob = prob->next;
          prob_counter++;
        }
      printf ("\n");
      traveller = traveller->next;
      counter++;
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
  int words_to_read = 0;
  unsigned int seed = 0;
  int num_of_tweets = 0;
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
  seed = (unsigned int) strtol (argv[1], NULL, 10);
  num_of_tweets = (int) strtol (argv[2], NULL, 10);
  FILE *fp_corpus = fopen (argv[3], "r");
  if (fp_corpus == NULL)  // check success of opening file
    {
      printf ("Error: Invalid file.\n");
      return EXIT_FAILURE;
    }

  LinkList *dictionary = malloc (sizeof (LinkList));
  check_alloc (dictionary);

  fill_dictionary (fp_corpus, words_to_read, dictionary);

  srand (seed);
  for (int i = 0; i < num_of_tweets; i++)
    {
      printf ("Tweet %d: ", i + 1);
      generate_sentence (dictionary);
    }
  free_dictionary (dictionary);
  fclose (fp_corpus);
//  dictionary = NULL;

//  print_dictionary (dictionary);

  return 0;
}