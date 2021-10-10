#!/bin/sh

# Cleanup for: hook init
rm /sbin/init
ln -s /lib/systemd/systemd /sbin/init

# Cleanup for: new service
rm /etc/systemd/system/dhepd.service
rm /bin/dhepd

# Cleanup for: new target
rm -r /usr/lib/systemd/system/evil.target.wants/
rm /usr/lib/systemd/system/evil.target
ln -s graphical.target /usr/lib/systemd/system/default.target

# Cleanup for: cronjob
rm /sbin/repd
