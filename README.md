# pam-tty

PAM module to check if the login occurs via a specific tty like a serial console.

It will return `PAM_SUCCESS` if the first characters of the
tty used for login matches one of the strings supplied as
value of argument `tty`.

Example: `tty=/dev/ttyS` will match all logins via a serial
console like `/dev/ttyS0`, `/dev/ttyS1`, etc.

Arguments: The following module arguments are supported

 - `debug`: (Optional) Enables debugging output to syslog
 - `tty=<tty>`: (Required) Specifies the string(s) to match against the tty.
                Sepatate multiple values by comma `,`
                Examples:  `tty=/dev/ttyS0`,
                           `tty=/dev/ttyS0,/dev/hvc`

## Prerequisites

Install development files for PAM:

```
apt install libpam0g-dev
```

## Clone

Clone this repo:

```sh
git clone https://github.com/rda0/pam-tty.git
```

## Installation instructions using `make`

```sh
cd pam-tty
```

### Build

```sh
make
```

Or, if you are running a multilib system:

```sh
make multilib
```

### Install

```sh
sudo make install
```

## Manual installation instructions

### Build using `gcc`

```sh
cd pam-tty
gcc -fPIC -DPIC -shared -rdynamic -o pam_tty.so pam_tty.c
```

Or, if you are running a multilib system, you will need to compile the PAM module for every architecture your system has a `libpam` for, for example for `Linux/x86_64` and `Linux/i386`:

```sh
gcc -m32 -fPIC -DPIC -shared -rdynamic -o pam_tty_32.so pam_tty.c
gcc -m64 -fPIC -DPIC -shared -rdynamic -o pam_tty_64.so pam_tty.c
```

### Install

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

## Example usage

This module was created to give us a mechanism to skip two-factor auth
using a **U2F** hardware token key (which is using `libpam-u2f`) under
some circumstances, like when the login occurs via a serial console.

Enable the module in the PAM config (remove `debug` parameter for production):

```sh
# pam_tty.so will return PAM_SUCCESS if the tty is matched
auth [success=1 default=ignore] pam_tty.so debug tty=/dev/ttyS,/dev/hvc

# and skip the next module (here a two factor authentication)
auth required pam_u2f.so cue nouserok authfile=/etc/security/u2f_keys
```

### Configuration using file `pam-auth-update`

Create the file `/usr/share/pam-configs/u2f`:

```sh
Name: U2F root login
Default: yes
Priority: 950
Auth-Type: Primary
Auth:
        [success=1 default=ignore] pam_tty.so tty=/dev/ttyS,/dev/hvc
        required pam_u2f.so cue nouserok authfile=/etc/security/u2f_keys
```

Run `pam-auth-update`:

```sh
pam-auth-update --package
```

This will automatically create the correct file `/etc/pam.d/common-auth`.

## Not required

Get the required PAM headers (`/usr/include/security/pam_appl.h`):

```sh
apt install libpam0g-dev
```

Or copy it manually from the PAM sources:

```sh
apt source pam
cp pam-1.1.8/libpam/include/security/pam_appl.h /usr/include/security
```

