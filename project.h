#ifndef PROJECT_H
#define PROJECT_H
/**
 * @brief: This file contains the data structures and functions' prototypes.
 * @file: project.h
 * @author: ist1114455 (Marta Santos)
 */

/* limits */
#define BUFMAX 65535        /**< max. len. of input line	*/
#define MAXBATCH 1000       /**< max. registered batches   */
#define MAXBATCHNAME 20     /**< max. len. of batch name	*/
#define MAXVACCNAME 50     /**< max. len. of vaccine name	*/
#define MAXUSERNAME 200     /**< max. len. of user name	*/

/* errors */
#define E2MANYVACC "too many vaccines"
#define EDUPBATCH "duplicate batch number"
#define EINVBATCH "invalid batch"
#define EINVNAME "invalid name"
#define EINVDATE "invalid date"
#define EINVQUANT "invalid quantity"
#define ENOSVACC "no such vaccine"
#define ENOSTOCK "no stock"
#define EALRVACC "already vaccinated"
#define ENOSBATCH "no such batch"
#define ENOSUSER "no such user"
#define ENOMEMORY "No memory"

/* erros */
#define E2MANYVACCPT "demasiadas vacinas"
#define EDUPBATCHPT "número de lote duplicado"
#define EINVBATCHPT "lote inválido"
#define EINVNAMEPT "nome inválido"
#define EINVDATEPT "data inválida"
#define EINVQUANTPT "quantidade inválida"
#define ENOSVACCPT "vacina inexistente"
#define ENOSTOCKPT "esgotado"
#define EALRVACCPT "já vacinado"
#define ENOSBATCHPT "lote inexistente"
#define ENOSUSERPT "utente inexistente"
#define ENOMEMORYPT "sem memória"

#define EXITNOMEM -1

/** represents a date in day-month-year format */
typedef struct {
    int day, month, year;
} Date;


/* represents a vaccine batch in the system */
typedef struct Batch {
    char *vacc_name;        /**< name of vaccine        */
    char *batch_name;       /**< name of batch      */
    Date exp_date;      /**< expiration date        */
    int doses;       /**< number of doses        */
    int num_app;        /**< number of applications   */
} Batch;


/* represents a single vaccination record */
typedef struct {
    char *user_name;        /**< name of user vaccinated */
    char *vacc_name;        /**< name of vaccine        */
    char *batch_name;       /**< name of batch      */
    Date ap_date;       /**< date of vaccination     */
} Inocula;


/* main system that holds all vaccination data and operational parameters */
typedef struct {
    int mem_capacity;       /**< inicial memory capacity for batches/inoculations */
    int num_batch;      /**< number of batches registered */
    int num_inocula;        /**< number of inoculations registered */
    Batch *batches;     /**< array of batches */
    Date today;      /**< current date */
    Inocula *inocula;   /**< array of inoculations */
} Sys;



/* validations */
int validate_dup_batch_name(int num, Batch *batches, char *batch_name);
int validate_batch_name_max(char *batch_name);
int validate_batch_name_caract(char *batch_name);
int validate_vacc_name(char *vacc_name);
int validate_doses(int doses);
int validate_date(Date *date, Sys *sys);
int validate_batch_inputs(Sys *sys, char *batch_name, char *vacc_name,
    Date *exp_date, int doses, int idiom);

int is_future_date(Date *date, Sys *sys);
int is_user_found(Sys *sys, char *user_name);
int is_batch_found(Sys *sys, char *batch_name);
int is_already_vaccinated(Sys *sys, char *user_name, char *vacc_name);


/* sorting batches/inoculations by date */
int ord_date(Date *a, Date *b);
int ord_batches(Batch *a, Batch *b);
void sort_batches(Batch *batches, int num_batch);

int ord_inoculas(Inocula *a, Inocula *b);
void sort_inoculas(Inocula *inocula, int num_inocula);


/* prints info */
void print_batch_info(const Batch *batch);
void print_inocula_info(const Inocula *inocula);


/* extracts user name from input */
void extract_user(char *input, char *user_name);


/* inoculation management */
int delete_inocula(const Inocula *inocula, const char *user_name,
    int num_param, int day, int month, int year, const char *batch_name);
void create_inocula(Sys *sys, Batch *batch, char *user_name,
    char *vacc_name, int idiom);


/* initializations and memory management */
void set_batch_slots(Batch *batches, int start, int end);
void set_system(Sys *sys);
void free_system(Sys *sys);

void expand_inocula_memory(Sys *sys);
void free_inocula(Inocula *inocula);

void check_allocation(void *ptr, int idiom);

#endif