#!/usr/bin/env zsh
touch test.txt
i=1;
rm test.txt > /dev/null;
while test $i -le 100; do 
    echo "====== Game $i ======" >> test.txt;
    rm log_$i.txt 2> /dev/null;
    touch log_$i.txt;
    ./rungame.sh DraculaView.* HunterView.* GameView.* hunter.c dracula.c Queue.* Item.h Makefile | tee log_$i.txt | tail -2 >> test.txt;
    if egrep "dis" log_$i.txt > /dev/null; then
        echo "Game $i has a diaqualification!"
    fi
    i=`expr $i + 1`;
    echo "" >> test.txt;
done