#!/usr/bin/env zsh
touch test.txt
i=0;
rm test.txt > /dev/null;
touch text.txt;
while test $i -lt 20; do 
    echo "====== Game $i ======" >> test.txt;
    rm log_$i.txt > /dev/null;
    touch log_$i.txt;
    ./rungame.sh DraculaView.* HunterView.* GameView.* hunter.c dracula.c Queue.* Item.h Makefile | tee log_$i.txt | tail -2 >> test.txt;
    i=`expr $i + 1`;
    echo "" >> test.txt;
done