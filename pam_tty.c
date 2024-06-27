/* File:        pam_tty.c
 * Author:      Sven Mäder <maeder@phys.ethz.ch>, ETH Zurich, ISG D-PHYS
 * Date:        2017-04-11
 *
 * Description: PAM module to check if the login occurs via a specific tty
 *              It will return PAM_SUCCESS if the first characters of the
 *              tty used for login matches one of the strings supplied as
 *              value of argument `tty`.
 *              Example: tty=/dev/ttyS will match all logins via a serial
 *                       console like /dev/ttyS0, /dev/ttyS1, etc.
 *
 * Arguments:   The following module arguments are supported
 *
 *   debug      (Optional) Enables debugging output to syslog
 *
 *   tty=<tty>  (Required) Specifies the string(s) to match against the tty
 *                         Sepatate multiple values by comma `,`
 *                         Examples:  tty=/dev/ttyS0
 *                                    tty=/dev/ttyS0,/dev/hvc
 *
 * Copyright 2017 Sven Mäder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include <security/pam_modules.h>
#include <security/pam_ext.h>
//#include <security/pam_appl.h>

/* Check if argument was supplied */
static unsigned int has_argument(const char* argument, int argc, const char** argv) {
   int len = strlen(argument);
   int i;

   if (argument != NULL && argc > 0 && argv != NULL) {
       for (i = 0; i < argc; i++) {
           if (argv[i] != NULL) {
               if (strncmp(argument, argv[i], len) == 0) {
                   return 1;
               }
           }
       }
   }
   return 0;
}

/* Return values supplied with an argument */
static const char* get_value(const char* key, int argc, const char** argv) {
   int len = strlen(key);
   int i;

   if (key != NULL && argc > 0 && argv != NULL) {
       for (i = 0; i < argc; i++) {
           if (argv[i] != NULL) {
               if (strncmp(key, argv[i], len) == 0 && argv[i][len] == '=') {
                   return argv[i] + len + 1;
               }
           }
       }
   }
   return NULL;
}

/* Split a string by the delimiter and return a list of strings */
static char** get_values(char* list, const char* delimiter) {
    char **values = NULL;
    char *ptr;
    int n_delimiter = 0, i;

    /* first split of string */
    ptr = strtok(list, delimiter);
    /* continue to split string and append tokens to 'values' */
    while (ptr) {
        values = realloc(values, sizeof(char*) * ++n_delimiter);
        if (values == NULL) {
            /* memory allocation failed */
            return NULL;
            //exit(-1);
        }
        values[n_delimiter-1] = ptr;
        ptr = strtok(NULL, delimiter);
    }
    /* realloc one extra element for the last NULL */
    values = realloc(values, sizeof(char*) * (n_delimiter+1));
    values[n_delimiter] = 0;
    return values;
}

/* Returns PAM_SUCCESS if login occurs via a specific tty */
int pam_tty(pam_handle_t *pamh, int argc, const char **argv) {
    int pgi_ret, i;
    unsigned int debug = 0;
    char *tty;
    const char *arg_tty_key = "tty";
    char *arg_tty;
    char **arg_tty_values;

    /* Check if debug mode is enabled using argument `debug` */
    debug = has_argument("debug", argc, argv);

    /* Get the actual tty used for the login */
    if (PAM_SUCCESS != (pgi_ret = pam_get_item(pamh, PAM_TTY, (const void **)&tty))) {
        pam_syslog(pamh, LOG_ERR, "unable to obtain tty");
        return(PAM_IGNORE);
    } else {
        if (debug)
            pam_syslog(pamh, LOG_DEBUG, "successfully obtained tty: %s", tty);
    }

    if (tty == NULL) {
        if (debug)
            pam_syslog(pamh, LOG_DEBUG, "tty is NULL");
        return(PAM_IGNORE);
    }

    /* Get (list of) tty to match from module arguments (`tty=<list_of_ttys>`) */
    arg_tty = (char *)get_value(arg_tty_key, argc, argv);
    if(arg_tty == NULL) {
        pam_syslog(pamh, LOG_ERR, "missing argment: %s", arg_tty_key);
        return(PAM_IGNORE);
    } else if(strcmp(arg_tty, "") == 0) {
        pam_syslog(pamh, LOG_ERR, "missing argment value: %s", arg_tty_key);
        return(PAM_IGNORE);
    }

    /* Split string (with list of ttys) into stringtable */
    arg_tty_values = get_values(arg_tty, ",");
    if(arg_tty_values == NULL) {
        pam_syslog(pamh, LOG_ERR, "memory allocation failed");
        return(PAM_IGNORE);
    }

    /* Check if any string matches against login tty, return PAM_SUCCESS on first match */
    for(i = 0; arg_tty_values[i] != NULL; i++) {
        if(strncmp(tty, arg_tty_values[i], strlen(arg_tty_values[i])) == 0) {
            if (debug)
                pam_syslog(pamh, LOG_DEBUG, "successfully matched tty: %s", arg_tty_values[i]);
            free(arg_tty_values); /* free the memory allocated */
            return(PAM_SUCCESS);
        }
    }

    if (debug)
        pam_syslog(pamh, LOG_DEBUG, "failed to match tty");
    free(arg_tty_values); /* free the memory allocated */
    return(PAM_IGNORE);
}

/* PAM entry point for session creation */
int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return(pam_tty(pamh, argc, argv));
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
    return(pam_tty(pamh, argc, argv));
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
