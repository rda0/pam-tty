/* 
 * Author: Sven Mäder <maeder@phys.ethz.ch>, ETH Zurich, ISG D-PHYS
 */

/* Define which PAM interfaces we provide */
#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

/* Include headers */
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

/* Include PAM headers */
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>

static const char* get_value(const char* key, int argc, const char** argv) {
   int len = strlen(key);
   int i;

   for (i = 0; i < argc; i++) {
       if (strncmp(key, argv[i], len) == 0 && argv[i][len] == '=') {
           return argv[i] + len + 1;
       }
   }
   return NULL;
}

static char** get_values(char* list, const char* delimiter) {
    char **values = NULL;
    char *ptr;
    int n_delimiter = 0, i;

    ptr = strtok(list, delimiter);
    /* split string and append tokens to 'res' */
    while (ptr) {
        values = realloc(values, sizeof(char*) * ++n_delimiter);
        if (values == NULL) {
            exit(-1); /* memory allocation failed */
        }
        values[n_delimiter-1] = ptr;
        ptr = strtok(NULL, delimiter);
    }
    /* realloc one extra element for the last NULL */
    values = realloc(values, sizeof(char*) * (n_delimiter+1));
    values[n_delimiter] = 0;
    return values;
}

/* PAM entry point for session creation */
int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return(PAM_IGNORE);
}

/* PAM entry point for session cleanup */
int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return(PAM_IGNORE);
}

/* PAM entry point for accounting */
int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return(PAM_IGNORE);
}

/* PAM entry point for authentication verification */
int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int pgi_ret, i;
    char *tty;
    char *arg_tty;
    char **arg_tty_values;
//    char **res = NULL;
//    char *p;
//    int n_delim = 0, i;
//
    arg_tty = (char *)get_value("tty", argc, argv);
//    p = strtok(arg_tty, ",");
//    /* split string and append tokens to 'res' */
//    while (p) {
//        res = realloc(res, sizeof(char*) * ++n_delim);
//        if (res == NULL) {
//            exit(-1); /* memory allocation failed */
//        }
//        res[n_delim-1] = p;
//        p = strtok (NULL, ",");
//    }
//
//    /* realloc one extra element for the last NULL */
//    res = realloc(res, sizeof(char*) * (n_delim+1));
//    res[n_delim] = 0;
    arg_tty_values = get_values(arg_tty, ",");

    if (PAM_SUCCESS != (pgi_ret = pam_get_item(pamh, PAM_TTY, (const void **)&tty))) {
        pam_syslog(pamh, LOG_ERR, "Unable to obtain the tty.");
    } else {
        pam_syslog(pamh, LOG_DEBUG, "Successfully obtained the tty.");
        pam_syslog(pamh, LOG_DEBUG, "The tty is: %s", tty);
        if(arg_tty != NULL) {
        for(i = 0; arg_tty_values[i] != NULL; i++) {
        //for (i = 0; i < (n_delim+1); ++i) {
            if(strncmp(tty, arg_tty_values[i], strlen(arg_tty_values[i])) == 0) {
            //if(strncmp(tty, arg_tty, strlen(match_tty)) == 0) {
            //if((strncmp(tty, "/dev/ttyS", 9) == 0) || (strncmp(tty, "/dev/hvc", 8) == 0)) {
                pam_syslog(pamh, LOG_DEBUG, "Successfully matched tty.");
                free(arg_tty_values); /* free the memory allocated */
                return(PAM_SUCCESS);
            } else {
                pam_syslog(pamh, LOG_DEBUG, "Failed to match tty.");
            }
        }
        } else {
            pam_syslog(pamh, LOG_ERR, "Missing argment: tty");
        }              
    }
    free(arg_tty_values); /* free the memory allocated */
    return(PAM_IGNORE);
}

/*
   PAM entry point for setting user credentials (that is, to actually
   establish the authenticated user's credentials to the service provider)
 */
int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return(PAM_IGNORE);
}

/* PAM entry point for authentication token (password) changes */
int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return(PAM_IGNORE);
}
