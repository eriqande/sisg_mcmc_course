

# SISG_MCMC_COURSE

This repository includes the LaTeX source code and the image files
to produce the lectures used by Eric C. Anderson and Matthew Stephens
in their course "Markov Chain Monte Carlo for Statistical Genetics" taught
at the Summer Institute in Statistical Genetics (SISG).  

These lecture notes were originally written by Eric C. Anderson and Mike
Denham in the spring of 2004 for the SISG that was taught that year
at North Carolina State University.  The pair taught the same course
in 2005.  After that, Matthew Stephens took over Mike Denham's part of the
course, adding lectures and rewriting significant portions of others.

We are grateful to Mike Denham for providing the style files for formatting
the lectures. These files begin with "mcd".

This repository was initialized on GitHub in 2014.  It does not include the
revision history over the previous 5 years (when it was under version control with
subversion).

## GETTING AND COMPILING THE LECTURES

Each lecture or section of the of the course is compiled as its own LaTeX
document, and the PDFs are catenated afterward.  The script `./lectures/PDFLatexThem.sh`
runs LaTeX twice on each lecture/section of the course.  The script
`./lectures/CatenatePDFs.sh` catenates the PDFs together.  Note that this 
relies on a Mac-specific script.

Here are the basic directions:
```sh
# get the repository:
git clone ...

# change into the lectures directory
cd ./sisg_mcmc_course/lectures

# typeset it all
./PDFLatexThem.sh

# catenate the lectures into a single PDF document, named whatever
# you want it to be (below named "all-lectures.pdf")
./CatenatePDFs.sh all-lectures.pdf
```
On the last step I get some warnings about Type 1 fonts, but I don't think that it is a 
problem.

## LICENSE

Copyright (C)  2014  Eric C. Anderson and Matthew Stephens with contributions
from Mike Denham.

Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3
or any later version published by the Free Software Foundation;
with no Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.
A copy of the license is included in file LICENSE.md.

Some of the style files are, where indicated, under the 
LaTeX Project Public License.

