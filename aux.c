/**
 * Vaccination Management System - Auxiliary Functions
 * @brief: This file contains auxiliary functions for the vaccination:
 * - Memory management
 * - Date validation and comparison
 * - System initialization and cleanup
 * - Safety checks and error handling
 * @file: aux.c
 * @author: ist1114455 (Marta Santos)
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "project.h"

/** Checks for duplicate batch names in existing batches
 * @param num   number of batches
 * @param batches   array of Batch structures
 * @param batch_name   name of the batch
 * @return   1 if duplicate exists, 0 if name is unique
 */
int validate_dup_batch_name(int num, Batch *batches, char *batch_name) {
    for (int i = 0; i < num; i++) {
        if (strcmp(batches[i].batch_name, batch_name) == 0) {
            return 1;
        }
    }
    return 0;
}


/** Validates batch name length against the limit
 * @param batch_name   name of the batch
 * @return  1 if exceeds limit, 0 if within bounds
 */
int validate_batch_name_max(char *batch_name) {
    if (strlen(batch_name) > MAXBATCHNAME) {
        return 1;
    }
    return 0;
}


/** Checks if batch name contains only hexadecimal uppercase
characters (0-9, A-F)
 * @param batch_name   name of the batch
 * @return  1 if invalid characters found, 0 if valid
 */
int validate_batch_name_caract(char *batch_name) {
    for (int i = 0; *(batch_name +i) != '\0'; i++) {
        if (!((*(batch_name +i) >= '0' && *(batch_name +i) <= '9') ||
        (*(batch_name +i) >= 'A' && *(batch_name +i) <= 'F'))) {
            return 1;
        }
    }
    return 0;
}


/** Validates vaccine name for invalid characters and length
 * @param vacc_name   name of the vaccine
 * @return  1 if invalid, 0 if valid
 */
int validate_vacc_name(char *vacc_name) {
    for (int i = 0; *(vacc_name +i) != '\0'; i++) {
        if (*(vacc_name +i) == ' ' || *(vacc_name +i) == '\n' ||
        *(vacc_name +i) == '\t') {
            return 1;
        }
    }
    if (strlen(vacc_name) > MAXVACCNAME) {
        return 1;
    }
    return 0;
}


/** Checks if dose quantity is positive
 * @return  1 if invalid (≤0), 0 if valid
 */
int validate_doses(int doses) {
    if (doses < 1) {
        return 1;
    }
    return 0;
}


/** Validates date against current system date and calendar rules
 * @param date   date to validate
 * @return  1 if invalid, 0 if valid
 */
int validate_date(Date *date, Sys *sys) {
    Date current_date = sys->today;

    if (date->year < current_date.year ||
    (date->year == current_date.year && date->month < current_date.month) ||
    (date->year == current_date.year && date->month == current_date.month &&
    date->day < current_date.day)) {
        return 1;
    }
    /* definition of days per month (index 0 will not be used) */
    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (date->month < 1 || date->month > 12 || date->day < 1 ||
    date->day > days_in_month[date->month]) {
        return 1;
    }
    return 0;
}


/** Initializes batch slots with NULL/zero values
 * @param batches   array of Batch structures
 * @param start     starting index for initialization
 * @param end   ending index for initialization
 */
void set_batch_slots(Batch *batches, int start, int end) {
    for (int i = start; i < end; i++) {
        (*(batches + i)).batch_name = NULL;
        (*(batches + i)).vacc_name = NULL;
        (*(batches + i)).num_app = 0;
        (*(batches + i)).doses = 0;
    }
}


/** Validator for all batch input fields
 * @param sys   system structure
 * @param batch_name   name of the batch
 * @param vacc_name   name of the vaccine
 * @param exp_date   expiration date
 * @param doses   number of doses
 * @param idiom   language identifier
 * @details Checks capacity, date, duplicates, naming rules, and doses
 * @return 1 if any validation fails, 0 if all valid
 */
int validate_batch_inputs(Sys *sys, char *batch_name, char *vacc_name,
    Date *exp_date, int doses, int idiom) {

    if (sys->num_batch >= MAXBATCH) {
        puts(idiom == 0 ? E2MANYVACC : E2MANYVACCPT);
        return 1;
    }
    if (validate_date(exp_date, sys)) {
        puts(idiom == 0 ? EINVDATE : EINVDATEPT);
        return 1;
    }
    if (validate_dup_batch_name(sys->num_batch, sys->batches, batch_name)) {
        puts(idiom == 0 ? EDUPBATCH : EDUPBATCHPT);
        return 1;
    }
    if (validate_vacc_name(vacc_name)) {
        puts(idiom == 0 ? EINVNAME : EINVNAMEPT);
        return 1;
    }
    if (validate_batch_name_max(batch_name) ||
    validate_batch_name_caract(batch_name)) {
        puts(idiom == 0 ? EINVBATCH : EINVBATCHPT);
        return 1;
    }
    if (validate_doses(doses)) {
        puts(idiom == 0 ? EINVQUANT : EINVQUANTPT);
        return 1;
    }
    return 0;
}


/** Compares two dates chronologically
 * @param date1   first date
 * @param date2   second date
 * @return  negative if date1 is earlier, positive if date1 is later,
0 if dates are equal
 */
int ord_date(Date *date1, Date *date2) {
    if (date1->year != date2->year) {
        return date1->year - date2->year;
    }
    if (date1->month != date2->month) {
        return date1->month - date2->month;
    }
    return date1->day - date2->day;
}


/** Compares two batches for sorting purposes
 * @param a   first batch
 * @param b   second batch
 * @details Primary sort by expiration date, secondary by batch name
 * @return  negative if a comes first, positive if b comes first,
0 if equal
 */
int ord_batches(Batch *a, Batch *b) {
    int cmp = ord_date(&a->exp_date, &b->exp_date);
    if (cmp != 0) { /* if dates different */
        return cmp;
    }
    return strcmp(a->batch_name, b->batch_name);
}


/** Sorts batches array
 * @param batches   array of Batch structures
 * @param num_batches   number of batches
 * @details Orders by expiration date (chronologically),
then by batch name (alphabetical)
 */
void sort_batches(Batch *batches, int num_batches) {
    /* outer loop: control the number of passes through the array */
    for (int i = 0; i < num_batches - 1; i++) {
        /* inner loop: handles the comparisons and swaps */
        for (int j = 0; j < num_batches - i - 1; j++) {
            /* compare adjacent batches */
            if (ord_batches(&batches[j], &batches[j + 1]) > 0) {
                /* perform the swap using a temporary variable */
                Batch temp = batches[j];
                batches[j] = batches[j + 1];
                batches[j + 1] = temp;
            }
        }
    }
}


/** Prints batch information in required format
 * @param batch   batch structure
 * @details Format: <vaccine_name> <batch_name> <dd-mm-yy> <doses>
 <applications>
 */
void print_batch_info(const Batch *batch) {
    printf("%s %s %02d-%02d-%02d %d %d\n",
           batch->vacc_name,
           batch->batch_name,
           batch->exp_date.day,
           batch->exp_date.month,
           batch->exp_date.year,
           batch->doses,
           batch->num_app);
}


/** Extracts username from input command, handling both quoted and
unquoted names
 * @param input   input line
 * @param user_name   buffer to store extracted username
 */
void extract_user(char *input, char *user_name) {
    int i = 2, j = 0; /* start after command letter */

    /* case quoted (with spaces) */
    if (input[i] == '"') {
        while (input[++i] != '"') { /* skip first quote */
            user_name[j++] = input[i];
        }
        i++; /* skip final quote */
    /* handle unquoted username */
    } else {
        while (input[i] != ' ' && input[i] != '\0') {
            user_name[j++] = input[i++];
        }
    }

    /* end username and clean nline if present */
    user_name[j] = '\0';
    if (j > 0 && user_name[j - 1] == '\n') {
        user_name[j - 1] = '\0';
    }
}


/** Expands inocula storage capacity when needed
 * @param sys   system structure
 */
void expand_inocula_memory(Sys *sys) {
    if (sys->num_inocula >= sys->mem_capacity) {
        sys->mem_capacity = (sys->mem_capacity > 0) ?
        sys->mem_capacity * 2 : 10;

        sys->inocula = (Inocula *)realloc(sys->inocula,
            sizeof(Inocula) * sys->mem_capacity);
    }
}


/** Creates a new vaccination inoculation in the system
 * @param sys   system structure
 * @param batch   batch structure
 * @param user_name   name of the user
 * @param vacc_name   name of the vaccine
 * @param idiom   language identifier
 * @details Allocates memory for strings and validates allocations and
prints batch name required
 */
void create_inocula(Sys *sys, Batch *batch, char *user_name,
    char *vacc_name, int idiom) {

    /* allocate and store user/vaccine/batch names */
    sys->inocula[sys->num_inocula].user_name = strdup(user_name);
    sys->inocula[sys->num_inocula].vacc_name = strdup(vacc_name);
    sys->inocula[sys->num_inocula].batch_name = strdup(batch->batch_name);

    /* verify all allocations succeeded */
    check_allocation(sys->inocula[sys->num_inocula].user_name, idiom);
    check_allocation(sys->inocula[sys->num_inocula].vacc_name, idiom);
    check_allocation(sys->inocula[sys->num_inocula].batch_name, idiom);

    sys->inocula[sys->num_inocula].ap_date = sys->today;
    /* update counters */
    batch->num_app++;
    sys->num_inocula++;
    printf("%s\n", batch->batch_name);
}


/** Checks if user was already vaccinated with same vaccine today
 * @param sys   system structure
 * @param user_name   name of the user
 * @param vacc_name   name of the vaccine
 * @return  1 if duplicate found, 0 otherwise
 */
int is_already_vaccinated(Sys *sys, char *user_name, char *vacc_name) {
    for (int i = 0; i < sys->num_inocula; i++) {
        if (strcmp(sys->inocula[i].user_name, user_name) == 0 &&
            strcmp(sys->inocula[i].vacc_name, vacc_name) == 0 &&
            sys->today.year == sys->inocula[i].ap_date.year &&
            sys->today.month == sys->inocula[i].ap_date.month &&
            sys->today.day == sys->inocula[i].ap_date.day) {
            return 1;
        }
    }
    return 0;
}


/** Compares two inoculations by application date
 * @param a   first inoculation
 * @param b   second inoculation
 * @details Uses ord_date() for chronological comparison
 * @return  negative if a is earlier, positive if a is later,
0 if same application date
 */
int ord_inoculas(Inocula *a, Inocula *b) {
    int cmp = ord_date(&a->ap_date, &b->ap_date);
    return cmp;
}


/** Sorts inoculations by application date (ascending)
 * @param inocula   array of Inocula structures
 * @param num_inocula   number of inoculations
 */
void sort_inoculas(Inocula *inocula, int num_inocula) {
    /* outer loop: control the number of passes through the array */
    for (int i = 0; i < num_inocula - 1; i++) {
        /* inner loop: handles the comparisons and swaps */
        for (int j = 0; j < num_inocula - i - 1; j++) {
            if (ord_inoculas(&inocula[j], &inocula[j + 1]) > 0) {
                /* swap if out of order */
                Inocula temp = inocula[j];
                inocula[j] = inocula[j + 1];
                inocula[j + 1] = temp;
            }
        }
    }
}


/** Prints inoculation information in required format
 * @param inocula   inoculation structure
 * @details Format: <user_name> <batch_name> <DD-MM-YY>
 */
void print_inocula_info(const Inocula *inocula) {
    printf("%s %s %02d-%02d-%02d\n",
           inocula->user_name,
           inocula->batch_name,
           inocula->ap_date.day,
           inocula->ap_date.month,
           inocula->ap_date.year);
}


/** Checks if any inoculations exist for given user
 * @param sys   system structure
 * @param user_name   name of the user
 * @return  1 if user found, 0 if no recors exist
 */
int is_user_found(Sys *sys, char *user_name) {
    for (int i = 0; i < sys->num_inocula; i++) {
        if (strcmp(sys->inocula[i].user_name, user_name) == 0) {
            return 1;
        }
    }
    return 0;
}


/** Verifies if a batch exists in the system
 * @param sys   system structure
 * @param batch_name   name of the batch
 * @return  1 if batch exists, 0 if not found
 */
int is_batch_found(Sys *sys, char *batch_name) {
    for (int i = 0; i < sys->num_batch; i++) {
        if (strcmp(sys->batches[i].batch_name, batch_name) == 0) {
            return 1; /* batch exists */
        }
    }
    return 0;
}


/** Determines if an inoculation record should be deleted based on criteria
 * @param inocula   inoculation structure
 * @param user_name   name of the user
 * @param num_param   number of parameters provided
 * @param day   day of the date
 * @param month   month of the date
 * @param year   year of the date
 * @param batch_name   name of the batch
 * @details Checks user match plus optional date and batch filters
 * @return   1 if record should be deleted, 0 otherwise
 */
int delete_inocula(const Inocula *inocula, const char *user_name,
    int num_param, int day, int month, int year, const char *batch_name) {

    /* check user match */
    if (strcmp(inocula->user_name, user_name) != 0) {
        return 0; /* skip if wrong user */
    }

    /* check date and batch match if provided */
    int matches_date = (num_param < 3) ||
        (inocula->ap_date.year == year &&
         inocula->ap_date.month == month &&
         inocula->ap_date.day == day);

    /* check batch match if provided */
    int matches_batch = (num_param < 5) ||
        (strcmp(inocula->batch_name, batch_name) == 0);

    /* both filters must pass */
    return matches_date && matches_batch;
}


/** Checks if a date is in the future compared to system date
 * @param date   date to check
 * @param sys   system structure
 * @return   1 if future date, 0 if current or past date
 */
 int is_future_date(Date *date, Sys *sys) {
    Date current_date = sys->today;

    if (date->year > current_date.year) {
        return 1;
    }
    if (date->year == current_date.year && date->month > current_date.month) {
        return 1;
    }
    if (date->year == current_date.year && 
        date->month == current_date.month && 
        date->day > current_date.day) {
        return 1;
    }
    return 0;
}


/** Frees memory allocated for an inoculation record
 * @param inocula   inoculation structure
 */
void free_inocula(Inocula *inocula) {
    free(inocula->user_name);
    free(inocula->vacc_name);
    free(inocula->batch_name);
}


/** Initializes system with default values
 * @param sys   system structure
 */
void set_system(Sys *sys) {
    sys->mem_capacity = 10; /* initial capacity for batches/ inoculations */
    sys->num_batch = 0;
    sys->num_inocula = 0;

    /* set default system date */
    sys->today.day = 1;
    sys->today.month = 1;
    sys->today.year = 2025;

    /* initialize batch counters */
    for (int i = 0; i < sys->num_batch; i++) {
        sys->batches[i].num_app = 0;
        sys->batches[i].doses = 0;
    }
}


/** Releases all dynamically allocated system memory
 * @param sys   system structure
 */
void free_system(Sys *sys) {
    for (int i = 0; i < sys->num_batch; i++) {
        free(sys->batches[i].batch_name);
        free(sys->batches[i].vacc_name);
    }
    for (int i = 0; i < sys->num_inocula; i++) {
        free(sys->inocula[i].user_name);
        free(sys->inocula[i].vacc_name);
        free(sys->inocula[i].batch_name);
    }
    free(sys->batches);
    free(sys->inocula);
}


/** Verifies memory allocation success
 * @param ptr   pointer to allocated memory
 * @param idiom   language identifier
 */
void check_allocation(void *ptr, int idiom) {
    if (ptr == NULL) {
        puts(idiom == 0 ? ENOMEMORY : ENOMEMORYPT);
        exit(EXITNOMEM);
    }
}