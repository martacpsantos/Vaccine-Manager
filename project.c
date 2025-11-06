/* iaed25 - ist1114455 - project */
/**
 * A program simulating a vaccination management system.
 * Handles batch registration, inoculation, and date management.
 * @file: project.c
 * @author: ist1114455 (Marta Santos)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "project.h"


/** Handles command 'c', adding a new batch to the system, printing it
 * @param sys system structure
 * @param input input line
 * @param idiom language identifier
 * @details Validates all input fields
 */
static void add_batch(Sys *sys, const char *input, int idiom) {
    /* variables to store batch info */
    char batch_name[MAXBATCHNAME +1];
    char vacc_name[MAXVACCNAME*10];
    Date exp_date;
    int doses;
    
    /* initialize batch slots */
    set_batch_slots(sys->batches, sys->num_batch, sys->mem_capacity);
    /* check if memory capacity needs to be increased */
    if (sys->num_batch >= sys->mem_capacity) {
        sys->mem_capacity = sys->mem_capacity ? sys->mem_capacity * 2 : 10;
        sys->batches = realloc(sys->batches, sizeof(Batch)*sys->mem_capacity);
        set_batch_slots(sys->batches, sys->num_batch, sys->mem_capacity);
    }

    sscanf(input, "c %s %d-%d-%d %d %s", batch_name,
        &exp_date.day, &exp_date.month, &exp_date.year,
        &doses, vacc_name);
    
    /* validation of the received data */
    if (validate_batch_inputs(sys, batch_name, vacc_name, &exp_date,
        doses, idiom)) {
        return;
    }

    /* dinamic duplication of the strings */
    sys->batches[sys->num_batch].batch_name = strdup(batch_name);
    sys->batches[sys->num_batch].vacc_name = strdup(vacc_name);
    /* store batch data */
    sys->batches[sys->num_batch].exp_date = exp_date;
    sys->batches[sys->num_batch].doses = doses;

    check_allocation(sys->batches[sys->num_batch].batch_name, idiom);
    check_allocation(sys->batches[sys->num_batch].vacc_name, idiom);

    sys->num_batch++; /* increment batch count */
    printf("%s\n", batch_name);
    return;
}


/** Handles command 'l' to list batches
 * @param sys   system structure
 * @param input     input line
 * @param idiom     language identifier
 * @details Lists all batches or specific ones based on user input
 */
static void list_batches(Sys *sys, char *input, int idiom) {
    /* skips 'l' and space to help extract vacc name */
    char *current = input + 2;
    sort_batches(sys->batches, sys->num_batch);

    if (*current == '\0') {
        for (int i = 0; i < sys->num_batch; i++) {
            print_batch_info(&sys->batches[i]);
        }
    }
    else {
        /* list specific batches */
        while (*current != '\0') {
            char vacc_name[MAXVACCNAME + 1];
            int len = 0; int found = 0;

            /* extract vacc name from input */
            while (*current != ' ' && *current != '\0') {
                vacc_name[len++] = *current++;
            }
            vacc_name[len] = '\0'; /* end string */

            /* check if the last character is a newline */
            if (vacc_name[len - 1] == '\n') vacc_name[len - 1] = '\0';
            while (*current == ' ') current++; /* skip spaces between names */

            for (int i = 0; i < sys->num_batch; i++) {
                if (strcmp(sys->batches[i].vacc_name, vacc_name) == 0) {
                    print_batch_info(&sys->batches[i]);
                    found = 1;
                }
            }
            if (!found) printf("%s: %s\n", vacc_name, idiom == 0 ?
                ENOSVACC : ENOSVACCPT);
        }
    }
}


/** Handles command 't' to update or display the system date
 * @param sys   system structure
 * @param input     input line
 * @param idiom     language identifier
 */
static void update_date(Sys *sys, char *input, int idiom) {
    char *current = input + 2; /* skip 't' and space */
    int day, month, year;

    /* no argument given - show current date */
    if (*current == '\0') {
        printf("%02d-%02d-%02d\n", sys->today.day,
            sys->today.month,
            sys->today.year);
        return;
    }
    else {
        /* attempt to advance time */
        sscanf(input, "t %02d-%02d-%d", &day, &month, &year);
        Date new_date = {day, month, year};
        if (validate_date(&new_date, sys)) {
            puts(idiom == 0 ? EINVDATE : EINVDATEPT);
            return;
        }
        sys->today = new_date; /* update */
        printf("%02d-%02d-%02d\n",
            sys->today.day,
            sys->today.month,
            sys->today.year);
        return;
    }
}


/** Handles command 'a' to administer a vaccine and prints its batch name
 * @param sys   system structure
 * @param input     input line
 * @param idiom     language identifier
 * @details Checks if the user has already been vaccinated with the same
vaccine today. If not, administers a vaccine dose having in consideration
that the batch with at least one dose available with an older expiration date
but still valid compared to the current date must be the chosen one
 */
static void vaccinate(Sys *sys, char *input, int idiom) {
    char user_name[BUFMAX];
    char vacc_name[MAXVACCNAME*10];

    extract_user(input, user_name);
    /* extract vacc name based on the existence of quotation marks before */
    sscanf(input + 2 + strlen(user_name) + (input[2] == '"' ? 3 : 1),
    "%s", vacc_name);
    sort_batches(sys->batches, sys->num_batch);

    /* check for duplicate vaccination */
    if (is_already_vaccinated(sys, user_name, vacc_name)) {
        puts(idiom == 0 ? EALRVACC : EALRVACCPT);
        return;
    }
    expand_inocula_memory(sys);

    /* find and use valid available batch */
    for (int i = 0; i < sys->num_batch; i++) {
        /* check for matching vacc with available doses */
        if (strcasecmp(sys->batches[i].vacc_name, vacc_name) == 0 &&
        sys->batches[i].doses > 0) {

            /* apply vaccination and reduce doses */
            sys->batches[i].doses--;
            create_inocula(sys, &sys->batches[i], user_name, vacc_name, idiom);
            return;
        }
    }
    /* no stock available if loop completes without match */
    puts(idiom == 0 ? ENOSTOCK : ENOSTOCKPT);
    return;
}


/** Handles command 'r' to remove the availability of a vaccine
 * @param sys   system structure
 * @param input     input line
 * @param idiom     language identifier
 * @details Handles both complete removal (if unused) and dose zeroing
 (if used), printing doses applied or error message, if batch can not be found
 */
static void delete_batch(Sys *sys, const char *input, int idiom) {
    char batch_name[MAXBATCHNAME + 1];
    sscanf(input, "r %s", batch_name);

    int found = 0; /* batch existence flag */

    /* search through all batches */
    for (int i = 0; i < sys->num_batch; i++) {
        if (strcmp(sys->batches[i].batch_name, batch_name) == 0) {
            found = 1;

            /* case in which batch has no applications - full removal */
            if (sys->batches[i].num_app == 0) {
                printf("0\n");
                /* free allocated memory */
                free(sys->batches[i].batch_name);
                free(sys->batches[i].vacc_name);
                /* moving elements */
                for (int j = i; j < sys->num_batch - 1; j++) {
                    sys->batches[j] = sys->batches[j + 1];
                }
                sys->num_batch--; /* decrement batch count */
                return;
            } else { /* case in which batch has applications */
                sys->batches[i].doses = 0; /* reset doses */
                printf("%d\n", sys->batches[i].num_app);
                return;
            }
        }
    }
    /* batch not found */
    if (!found) {
        printf("%s: %s\n", batch_name, idiom == 0 ? ENOSBATCH : ENOSBATCHPT);
    }
}


/** Handles command 'u' to list inoculations
 * @param sys   system structure
 * @param input     input line
 * @param idiom     language identifier
 * @details Lists vaccination inoculations either for all users or a
specific user, outputing inoculations in chronological order of application
 */
static void list_inoculas(Sys *sys, char *input, int idiom) {
    char user_name[BUFMAX];
    char *current = input + 1; /* skips 'u' and space in order
    to extract user */

    sort_inoculas(sys->inocula, sys->num_inocula);

    /* case in which no username is provided - list all inoculations */
    if (*current == '\0' || *current == '\n') {
        for (int j = 0; j < sys->num_inocula; j++) {
            print_inocula_info(&sys->inocula[j]);
        }
    }
    else { /* username is provided */
        int found = 0; /* user existence flag */
        extract_user(input, user_name);

        /* search for corresponding user's inoculations */
        for (int i = 0; i < sys->num_inocula; i++) {
            if (strcmp(sys->inocula[i].user_name, user_name) == 0) {
                /* print inoculation info */
                print_inocula_info(&sys->inocula[i]);
                found = 1;
            }
        }
        if (!found) { /* user not found - error message */
            printf("%s: %s\n", user_name, idiom == 0 ? ENOSUSER : ENOSUSERPT);
        }
    }
    return;
}


/** Handles command 'd' to delete vaccination records
 * @param sys   system structure
 * @param input     input line
 * @param idiom     language identifier
 * @details Deletes inoculation records based on user, date, and batch
 */
static void delete_registration(Sys *sys, const char *input, int idiom) {
    char user_name[MAXUSERNAME + 1]; char batch_name[MAXBATCHNAME + 1];
    int day, month, year;

    /* (1-5 possible parameters) */
    int num_param = sscanf(input, "d %s %d-%d-%d %s", user_name, &day, &month, &year, batch_name);

    if (!is_user_found(sys, user_name)) { /* checks if user exists */
        printf("%s: %s\n", user_name, idiom == 0 ? ENOSUSER : ENOSUSERPT);
        return;
    }
    if (num_param >= 4) { /* validate date if provided */
        Date ap_date = {day, month, year};
        if ((is_future_date(&ap_date, sys))) {
            puts(idiom == 0 ? EINVDATE : EINVDATEPT);
            return;
        }
    }
    /* validate batch if provided */
    if (num_param == 5 && !is_batch_found(sys, batch_name)) {
        printf("%s: %s\n", batch_name, idiom == 0 ? ENOSBATCH : ENOSBATCHPT);
        return;
    }
    
    int total_deleted = 0; int new_index = 0;

    /* filter and compact inocula array */
    for (int i = 0; i < sys->num_inocula; i++) {
        if (delete_inocula(&sys->inocula[i], user_name, num_param, day, month, year, batch_name)) {

            /* mark for deletion */
            free_inocula(&sys->inocula[i]); total_deleted++;
        } else if (new_index != i) {

            /* compact array by moving non-deleted elements */
            sys->inocula[new_index++] = sys->inocula[i];
        } else new_index++; /* advance index if no move needed */
    }

    /* update count and print results */
    sys->num_inocula = new_index; printf("%d\n", total_deleted);
}


/** Main program, manages the vaccination system
 * @return always returns 0
 */
int main (int argc, char *idiom[]) {
    char buf[BUFMAX]; /* input buffer for commands */
    Sys sys; /* main system structure */

    set_system(&sys);
    
    int idioma = 0; /* default to english (0) */

    /* portuguese idiom if 'pt' argument provided */
    if (argc > 1 && strcmp(idiom[1], "pt") == 0) {
        idioma = 1;
    }

    /* allocate initial memory for batches and inoculations */
    sys.batches = (Batch *)malloc(sizeof(Batch) * sys.mem_capacity);
    sys.inocula = (Inocula *)malloc(sys.mem_capacity * sizeof(Inocula));
    check_allocation(sys.batches, idioma);
    check_allocation(sys.inocula, idioma);

    /* main command processing loop */
    while (fgets(buf, BUFMAX, stdin)) {
        switch(buf[0]) {
            case 'c': add_batch(&sys, buf, idioma); break;
            case 'l': list_batches(&sys, buf, idioma); break;
            case 'a': vaccinate(&sys, buf, idioma); break;
            case 'r': delete_batch(&sys, buf, idioma); break;
            case 'u': list_inoculas(&sys, buf, idioma); break;
            case 't': update_date(&sys, buf, idioma); break;
            case 'd': delete_registration(&sys, buf, idioma); break;
            case 'q': free_system(&sys); /* clean memory */
            return 0;
            default: break;
        }
    }
    return 0;
}