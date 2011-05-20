#!/bin/sh

if [ $# -lt 2 ]; then
  echo "usage: test.sh <host> <port>"
  exit
fi

# create random 10mb test file
echo "Creating random 10mb test file ... "
dd if=/dev/urandom of=./test.in bs=10485760 count=1 > /dev/null 2>&1

echo "Starting server ... "
# start server
./echoserver-poll $2 > /dev/null 2>&1 &

echo "Starting test ... "
# synchronous test.
md5 test.in | awk '{print $4}'
nc $1 $2 < test.in | md5 &
nc $1 $2 < test.in | md5 &
nc $1 $2 < test.in | md5 &
wait %2
wait %3
wait %4

echo "If the hashes are the same, great success"
killall echoserver-poll
