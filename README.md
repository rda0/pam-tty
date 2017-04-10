# pam_tty
PAM module to check if the login occurs via a specific tty like a serial console.

## Prerequisites

Get the required PAM headers:

```sh
apt source pam
mv pam-1.1.8 pam
```

## Clone

Clone this repo:

```sh
git clone https://github.com/rda0/pam_tty.git
```

## Build

```sh
cd pam_tty
```

```sh
gcc -fPIC -DPIC -shared -rdynamic -I../pam/libpam/include -o pam_tty.so pam_tty.c
```

Or, if you are running a multilib system, you will need to compile the PAM module for every architecture your system has a `libpam` for, for example for `Linux/x86_64` and `Linux/i386`:

```sh
gcc -m32 -fPIC -DPIC -shared -rdynamic -I../pam/libpam/include -o pam_tty_32.so pam_tty.c
gcc -m64 -fPIC -DPIC -shared -rdynamic -I../pam/libpam/include -o pam_tty_64.so pam_tty.c
```

## Install

Copy the PAM module to `/lib/security`:

```sh
mkdir /lib/security
cp pam_tty.so /lib/security/pam_tty.so
chmod 755 /lib/security/pam_tty.so
chown root:root /lib/security/pam_tty.so
```

Or, on a multilib system:

```sh
cp pam_tty_32.so /lib/security/pam_tty.so
cp pam_tty_64.so /lib64/security/pam_tty.so
chown root:root /lib/security/pam_tty.so /lib64/security/pam_tty.so
chmod 755 /lib/security/pam_tty.so /lib64/security/pam_tty.so
```

Enable the module in the PAM config (remove `debug` parameter for production):

```sh
# pam_tty.so will return PAM_SUCCESS if the tty is matched
auth [success=1 default=ignore] pam_tty.so debug
# and skip the next module (here a two factor authentication)
auth required pam_u2f.so cue nouserok authfile=/etc/security/u2f_keys
```
