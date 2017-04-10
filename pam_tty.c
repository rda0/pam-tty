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

static const char* get_arg(const char* key, int argc, const char** argv) {
   int len = strlen(key);
   int i;

   for (i = 0; i < argc; i++) {
       if (strncmp(key, argv[i], len) == 0 && argv[i][len] == '=') {
           return argv[i] + len + 1;
       }
   }
   return NULL;
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
    int pgi_ret;
    char* tty;
    char* arg_tty;
    char** res = NULL;
    char* p;
    int n_delim = 0, i;

    arg_tty = (char *)get_arg("tty", argc, argv);
    p = strtok(arg_tty, ",");
    /* split string and append tokens to 'res' */
    while (p) {
        res = realloc(res, sizeof(char*) * ++n_delim);

        if (res == NULL) {
            exit(-1); /* memory allocation failed */
        }

        res[n_delim-1] = p;
        p = strtok (NULL, ",");
    }

    /* realloc one extra element for the last NULL */
    res = realloc(res, sizeof(char*) * (n_delim+1));
    res[n_delim] = 0;

    if (PAM_SUCCESS != (pgi_ret = pam_get_item(pamh, PAM_TTY, (const void **)&tty))) {
        pam_syslog(pamh, LOG_ERR, "Unable to obtain the tty.");
    } else {
        pam_syslog(pamh, LOG_DEBUG, "Successfully obtained the tty.");
        pam_syslog(pamh, LOG_DEBUG, "The tty is: %s", tty);
        if(arg_tty != NULL) {
        for (i = 0; i < (n_delim+1); ++i) {
            if(strncmp(tty, res[i], strlen(res[i])) == 0) {
            //if(strncmp(tty, arg_tty, strlen(match_tty)) == 0) {
            //if((strncmp(tty, "/dev/ttyS", 9) == 0) || (strncmp(tty, "/dev/hvc", 8) == 0)) {
                pam_syslog(pamh, LOG_DEBUG, "Successfully matched tty.");
                free(res); /* free the memory allocated */
                return(PAM_SUCCESS);
            } else {
                pam_syslog(pamh, LOG_DEBUG, "Failed to match tty.");
            }
        }
        } else {
            pam_syslog(pamh, LOG_ERR, "Missing argment: tty");
        }              
    }
    free(res); /* free the memory allocated */
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
