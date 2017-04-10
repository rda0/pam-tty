all: pam_tty.so

multilib: pam_tty_32.so pam_tty_64.so

pam_tty.so:
	gcc -v -fPIC -DPIC -shared -rdynamic -o pam_tty.so pam_tty.c

pam_tty_32.so:
	gcc -v -m32 -fPIC -DPIC -shared -rdynamic -o pam_tty_32.so pam_tty.c

pam_tty_64.so:
	gcc -v -m64 -fPIC -DPIC -shared -rdynamic -o pam_tty_64.so pam_tty.c

clean:
	if [ -e pam_tty.so ]; then rm pam_tty.so; fi
	if [ -e pam_tty_32.so ]; then rm pam_tty_32.so; fi
	if [ -e pam_tty_64.so ]; then rm pam_tty_64.so; fi

install:
	if [ -e pam_tty.so ]; then \
	  if [ ! -e "/lib/security" ]; then mkdir /lib/security; fi; \
	  cp pam_tty.so /lib/security/pam_tty.so; \
	fi
	if [ -e pam_tty_32.so ]; then \
	  if [ ! -e "/lib/security" ]; then mkdir /lib/security; fi; \
	  cp pam_tty_32.so /lib/security/pam_tty.so; \
	fi
	if [ -e pam_tty_64.so ]; then \
	  if [ ! -e "/lib64/security" ]; then mkdir /lib64/security; fi; \
	  cp pam_tty_64.so /lib64/security/pam_tty.so; \
	fi

uninstall:
	if [ -e /lib/security/pam_tty.so ]; then rm /lib/security/pam_tty.so; fi
	if [ -e /lib64/security/pam_tty.so ]; then rm /lib64/security/pam_tty.so; fi
