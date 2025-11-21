#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);

extern void build_halt_test(Seq_T instructions);
extern void build_LV_and_output_test(Seq_T instructions);
extern void build_io_test(Seq_T instructions);
extern void build_special_io_test(Seq_T instructions);
extern void build_arithmetic_test(Seq_T stream);
extern void build_divide_truncate_test(Seq_T stream);
extern void build_divide_0_test(Seq_T stream);
extern void build_nand_test(Seq_T stream);
extern void build_cmov_zero_test(Seq_T stream);
extern void build_cmov_non_zero_test(Seq_T stream);
extern void build_load_program_test(Seq_T stream);
extern void build_map_unmap_test(Seq_T stream);
extern void build_sstore_sload_test(Seq_T stream);
extern void build_load_segment_test(Seq_T stream);


/* The array `tests` contains all unit tests for the lab. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        { "halt",         NULL, "", build_halt_test },
        { "build_LV_and_output",        NULL, "Hello40\n", build_LV_and_output_test },
        { "build_io_test",        "Hello40\n", "Hello40\n", build_io_test },
        { "build_special_io_test",    NULL, NULL, build_special_io_test },
        { "build_arithmetic_test",        NULL, "cat\n", build_arithmetic_test },
        { "build_divide_truncate_test",    NULL, "!\n", build_divide_truncate_test },
        { "build_nand_test",  "5", "5", build_nand_test },
        { "build_cmov_zero_test",  NULL, "a", build_cmov_zero_test },
        { "build_cmov_non_zero_test",  NULL, "b", build_cmov_non_zero_test },
        { "build_load_progam_test",  NULL, "X", build_load_program_test },
        { "build_map_unmap_test",  NULL, "1\n2\n1\n", build_map_unmap_test },
        { "build_sstore_sload_test",  NULL, "a\n", build_sstore_sload_test },
        { "build_load_segment_test",  NULL, "", build_load_segment_test },
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("um_tests/%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("input_tests/%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("output_tests/%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}