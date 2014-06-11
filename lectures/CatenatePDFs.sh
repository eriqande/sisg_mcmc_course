

# put default values here
# VAR=default



function usage {
      echo Syntax:
      echo "  $(basename $0)  NameOfOutputPDF_File"
      echo
}

if [ $# -eq 0 ]; then
    usage;
    exit 1;
fi;

while getopts ":h" opt; do
    case $opt in
	h    ) 
	    usage
	    exit 1
	    ;;
	#m    )  VAR=$OPTARG;
	#    ;;
	\?   )
	    usage
	    exit  1
	    ;;
    esac
done

shift $((OPTIND-1));


# uncomment to test for right number of required args
if [ $# -ne 1 ]; then
    usage;
    exit 1;
fi
OUTFILE=$1


FILES=" Overview.pdf Session1.pdf Session2.pdf Session3.pdf Session4.pdf Session5.pdf \
 Session6.pdf Session7.pdf Session8.pdf Session9.pdf Session10.pdf References.pdf "


pdftk $FILES cat output $OUTFILE



