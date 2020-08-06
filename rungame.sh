#!/usr/bin/env zsh
#
# COMP2521 20T2 -- 
# COMP2521 19T0 -- shim for admtools Autotest; Fury of Dracula dryrun
#
# 2017-12-08	Jashank Jeremy <{jashankj,z5017851}@cse.unsw.edu.au>
# 2018-12-03	Jashank Jeremy <{jashankj,z5017851}@cse.unsw.edu.au>

RED='\033[0;31m'
NC='\033[0m'

# Apparently the global zsh config doesn't set up a sensible $PATH...?
path=(
	/home/cs2521/bin
	/bin  /usr/bin  /usr/local/bin
	/sbin /usr/sbin /usr/local/sbin
)

__get_prog_name__=$(basename "$0")
TMPDIR=${TMPDIR:-/tmp}
WORKDIR=$TMPDIR/dryrun.$$

WEBROOT=/web/cs2521/20T2
ASSIGNS=$WEBROOT/ass
DRACHOME=$ASSIGNS/ass2

########################################################################
main()
{
	sh $DRACHOME/hunt/dryrun/dryrun.sh "$@"
	if [ $? -ne 0 ]
	then
		echo "${RED}Dryrun failed - can't proceed.${NC}"
		exit 1
	fi
	
	umask 077

	[ "X$1" = "X" ] && usage
	apply_limits
	create_workdir
	hunt_cp_student_files "$@"
	hunt_cp_supplied_files
	hunt_do_compile
	hunt_do_round
	cleanup_workdir
}

usage()
{
	echo "usage: $0 Makefile dracula.c hunter.c ..."
	exit 64 # EX_USAGE
}

########################################################################
# Apply ulimits.
#
# -S sets the "soft" limit.
apply_limits()
{
	local block=512
	local MB=1024
	local MiB=1048576

	ulimit -Sn 256				# open files
	ulimit -St 60				# cpu time (seconds)
	ulimit -Su 1000				# max user processes

	# ASan doesn't like these at all; turn them off for now.
	#ulimit -Sm $((1024 * $MB))		# max memory size (kBytes)
	#ulimit -Sd $((100 * $MB))		# data seg size (kBytes)
	#ulimit -Ss $((8 * $MB))			# stack size (kBytes)
	#ulimit -Sv $((1024 * $MB))		# virtual memory (kBytes)

	ulimit -Sc $((4 * $MiB / $block))	# core file size (blocks)
	ulimit -Sf $((4 * $MiB / $block))	# file size (blocks)
}

########################################################################
errx()
{
	case "$1" in
	EX_USAGE)	exit_code=64 ;;
	EX_DATAERR)	exit_code=65 ;;
	EX_NOINPUT)	exit_code=66 ;;
	EX_NOUSER)	exit_code=67 ;;
	EX_NOHOST)	exit_code=68 ;;
	EX_UNAVAILABLE)	exit_code=69 ;;
	EX_SOFTWARE)	exit_code=70 ;;
	EX_OSERR)	exit_code=71 ;;
	EX_OSFILE)	exit_code=72 ;;
	EX_CANTCREAT)	exit_code=73 ;;
	EX_IOERR)	exit_code=74 ;;
	EX_TEMPFAIL)	exit_code=75 ;;
	EX_PROTOCOL)	exit_code=76 ;;
	EX_NOPERM)	exit_code=77 ;;
	EX_CONFIG)	exit_code=78 ;;
	*)		exit_code=1  ;;
	esac
	[ $exit_code -ne 1 ] && shift

	echo "${__get_prog_name__}: $@"
	exit ${exit_code}
}

########################################################################
#
create_workdir()
{
	mkdir -p $WORKDIR || \
		errx EX_CANTCREAT "couldn't create '$WORKDIR'"
}

require_file()
{
	if [ ! -f $WORKDIR/"$1" ]
	then
		errx EX_NOINPUT "$1: No such file or directory"
	fi
}

replace_file_from()
{
	install -m 444 $DRACHOME/$2/$1 $WORKDIR
}

fetch_file_from()
{
	[ ! -e $WORKDIR/"$1" ] && replace_file_from $1 $2
}

# inject a file
replace_hunt_file() { replace_file_from $1 hunt/dryrun/files }

# inject a file if it's not already there
fetch_hunt_file() { fetch_file_from $1 hunt/dryrun/files }

cleanup_workdir()
{
	rm -rf $WORKDIR
}
trap cleanup_workdir EXIT

########################################################################
# hunt check code

hunt_cp_student_files()
{
	# sanity-check files coming in
	for file in "$@"
	do
		[ ! -e "${file}" ] && \
			errx EX_NOINPUT "${file}: No such file or directory"
		[ ! -f "${file}" ] && \
			errx EX_NOINPUT "${file}: Not a file"

		cp "${file}" $WORKDIR
	done

	cd $WORKDIR
	require_file Makefile
	require_file dracula.c
	require_file hunter.c
	require_file GameView.c
	require_file GameView.h
	require_file DraculaView.c
	require_file DraculaView.h
	require_file HunterView.c
	require_file HunterView.h
}

hunt_cp_supplied_files()
{
	# grab the header files.
	replace_hunt_file Game.h
	replace_hunt_file hunter.h
	replace_hunt_file dracula.h
	replace_hunt_file tournament/player.c

	# fetch the supplied view
	view=hunt/dryrun/files
	fetch_file_from Map.c $view
	fetch_file_from Map.h $view
	fetch_file_from Places.c $view
	fetch_file_from Places.h $view
}

hunt_do_compile()
{
	cd $WORKDIR

	{
		echo '/^LIBS/a'
		echo 'LIBS+=-ldl -ljansson'
		echo '.'
		echo 'wq'
	} | ed Makefile > /dev/null

	if ! make dracula
	then
		errx EX_SOFTWARE "couldn't compile dracula; can't proceed."
	fi

	if ! make hunter
	then
		errx EX_SOFTWARE "couldn't compile hunter; can't proceed."
	fi
}

hunt_do_round()
{
	env SEGFAULT_SIGNALS='abrt segv fpe' \
	~cs2521ass/bin/node ~cs2521ass/20T2.dracula/game_runner/runGame.js \
	./hunter ./hunter ./hunter ./hunter ./dracula
}

########################################################################

main "$@"

