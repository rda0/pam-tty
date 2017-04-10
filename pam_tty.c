/* 
 * Author: Sven Mäder <maeder@phys.ethz.ch>, ETH Zurich, ISG D-PHYS
 */

/* Define which PAM interfaces we provide */
  #define PAM_SM_ACCOUNT
  #define PAM_SM_AUTH
  #define PAM_SM_PASSWORD
  #define PAM_SM_SESSION

  /* Include PAM headers */
  #include <security/pam_appl.h>
  #include <security/pam_modules.h>
  #include <security/pam_ext.h>
  
  #include <syslog.h>
  #include <string.h>

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
      char *tty;
      
      if (PAM_SUCCESS != (pgi_ret = pam_get_item(pamh, PAM_TTY, (const void **)&tty))) {
          pam_syslog(pamh, LOG_ERR, "Unable to obtain the tty.");
      } else {
          pam_syslog(pamh, LOG_DEBUG, "Successfully obtained the tty.");
          pam_syslog(pamh, LOG_DEBUG, "The tty is: %s", tty);
          if((strncmp(tty, "/dev/ttyS", 9) == 0) || (strncmp(tty, "/dev/hvc", 8) == 0)) {
              pam_syslog(pamh, LOG_DEBUG, "Successfully matched tty.");
              return(PAM_SUCCESS);
          } else {
              pam_syslog(pamh, LOG_DEBUG, "Failed to match tty.");
          }              
      }
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
