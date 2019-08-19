#!bin/bash
make
mkdir -p /opt/resign/bin/
mv -f resign /opt/resign/bin/
make clean
ln -fs /opt/resign/bin/resign /usr/local/bin/resign
echo "install succeed resign."