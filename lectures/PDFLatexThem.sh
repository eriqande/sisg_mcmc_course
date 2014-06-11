

# we can latex them in whatever order
TEXS="Overview.tex    Session10.tex  Session4.tex  Session7.tex \
References.tex  Session2.tex   Session5.tex  Session8.tex  \
Session1.tex    Session3.tex   Session6.tex  Session9.tex"




for T in $TEXS; do
    echo "********** LATEXING:  $T   *************";
    ((pdflatex $T) && (pdflatex $T)) || echo "FAILED TO LATEX $T" 
done;
