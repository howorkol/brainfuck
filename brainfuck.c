/*
 * Lee Howorko
 * Jan 3 2015
 * Brainfuck interpreter.
 * http://en.wikipedia.org/wiki/Brainfuck
 */

#include <stdio.h>
#include <stdlib.h>

struct instruction {
  char command;
  struct instruction * jump_target;
};

int push(struct instruction * dp);
struct instruction * pop();
int compile(FILE * fp);
int execute();

unsigned int program_max_size = 256;
unsigned int data_max_size = 256;
unsigned int stack_max_size = 16;
unsigned int stack_curr_size = 0;

struct instruction * instructions = NULL;
struct instruction ** stack = NULL;

int main(int argc, char ** argv) {
  FILE * fp;
  int result;

  if ((argc != 2) || ((fp = fopen(argv[1], "r")) == NULL)) {
    fprintf(stderr, "Usage: %s <filename.bf>\n", argv[0]);
    return 1;
  }

  stack = malloc(stack_max_size * sizeof(struct instruction *));
  instructions = malloc(program_max_size * sizeof(struct instruction));

  result = compile(fp);
  fclose(fp);
  if (result == 1) { fprintf(stderr, "Compile Error\n"); return 1; }
  result = execute();
  if (result == 1) { fprintf(stderr, "Run Error\n"); return 1; }

  free(stack);
  free(instructions);
  return 0;
}

int push(struct instruction * dp) {
  if (stack_curr_size == stack_max_size) 
    if ((stack = realloc(stack, stack_max_size += 16)) == NULL) return 1;
  stack[stack_curr_size++] = dp;
  return 0;
}

struct instruction * pop() {
  if (stack_curr_size == 0) return NULL;
  return stack[--stack_curr_size];
}

int compile(FILE * fp) {
  struct instruction * ip = instructions;
  struct instruction * jump_ptr;
  char c;  

  while ((c = fgetc(fp)) != EOF) {
    switch (c) {
      case '>': case '<': case '+': case '-': case '.': case ',': break;
      case '[': if (push(ip) == 1) return 1; break;
      case ']':
        if ((jump_ptr = pop()) == NULL) return 1;
        ip->jump_target = jump_ptr;
        jump_ptr->jump_target = ip;
        break;
      default: continue;
    }

    ip->command = c;
    if (++ip >= &instructions[program_max_size]) {
      instructions = realloc(instructions, program_max_size += 256); 
    }
  }

  if (stack_curr_size != 0) return 1;
  return 0;
}

int execute() {
  struct instruction * ip;
  unsigned short * data = malloc(data_max_size * sizeof(unsigned short));
  unsigned short * dp = data;

  for (ip = instructions; ip->command != 0; ++ip) {
    switch (ip->command) {
      case '>': 
        if (++dp >= &data[data_max_size]) 
          if ((data = realloc(data, data_max_size += 256)) == NULL) return 1;
        break;
      case '<': if (--dp < data) return 1; break;
      case '+': ++*dp; break;
      case '-': --*dp; break;
      case '.': putchar(*dp); break;
      case ',': *dp = getchar(); break;
      case '[': if (*dp == 0) ip = ip->jump_target; break; 
      case ']': if (*dp != 0) ip = ip->jump_target; break;
    }
  }
 
  free(data); 
  return 0;
}
