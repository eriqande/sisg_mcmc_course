# Additional Structure Fun

## Introduction
Here we do a few maneuvers to further convince ourself that the "Model with prior population information" should only be used when you have very strong reasons to believe that the different areas you have sampled truly represent different subpopulations and that given that you wish to infer migrants.  It is not intended to assess how well you can partition individuals into the different subpopulations.  The reason for this is that the MIGPRIOR is quite a strong prior.  It says that *a priori* you expect that with probability 1 minus MIGPRIOR each individual in a designated subpopulation has all of its ancestry from that subpopulation.  As a thought exercise, think about what would happen if you have sampled individuals from 5 populations, you set MIGPRIOR to 0.01, but then you don't have any genetic data on the individuals...

well, in that case the likelihood does not alter the prior and structure would tell you that each individual in each population, on average had 99% of its ancestry from that subpopulation.  It should be clear that at this point you would not want to jump up and raise your arms and say, "Victory! I have found population structure!!"

Here we will analyze some simple simulated data to drive this point home.

## Some Simulated Data
I am going to simulate some data: 18 microsats on 150 individuals---50 from each of 3 sampling locations.  The kicker is that the allele freqs in all the sampling locations will be identical (i.e. they are all drawn from one big, random-mating population, but I will label the first 50 "1" and the second 50 "2" and the final 50 "3".  Since we will later look at how structure handles this situation with the LOCPRIOR model we will make both a PopData and a Location column for them.  To do this I use my little command line program called ''nookie'' and then post-process the results (you don't have to do this, though I show the command line I used).
```sh
# draw allele freqs at 18 loci from the dirichlet dsn
/PriorPopAndLocPrior/--% (echo 18; for((i=1;i<=18;i++)); do echo 12; cl_rand -d 2 2 2 2 2 2 2 2 2 2 2 2; done;) > 18LocFreqs.txt

# run nookie to make 150 genotypes randomly drawn from those allele freqs
# and post process the output to make three populations/sampling locations of
# 50 individuals each
/PriorPopAndLocPrior/--% nookie -a 18LocFreqs.txt -f 150 1 | awk 'BEGIN {n=0} /Indiv_Fem/ {print "Samp_"n+1,"    "int(n/50)+1,"   "int(n/50)+1"   ",$0; n++}' | sed 's/Indiv_.*_0//g;' > NoStructure.txt 
```

The resulting files looks like the following, which you should copy and paste into a file names `NoStructure.txt` on your computer.
```

Samp_1     1    1          4 2   8 9   6 7   7 6   3 2   8 5   9 0   9 10   2 5   1 0   6 8   10 9   3 3   0 5   7 2   3 5   3 8   1 5
Samp_2     1    1          11 2   11 8   8 1   4 5   3 1   4 10   10 9   3 4   8 1   6 7   4 5   1 10   1 11   4 2   11 11   1 4   6 6   7 2
Samp_3     1    1          1 9   7 11   3 10   5 2   5 11   11 4   11 11   4 4   0 3   8 6   11 8   11 1   6 10   2 2   7 5   1 9   8 3   9 3
Samp_4     1    1          11 8   11 1   3 3   2 2   6 2   8 9   11 3   9 4   10 2   3 6   4 6   2 7   7 2   2 5   4 3   5 2   1 6   4 4
Samp_5     1    1          4 5   11 0   9 6   6 0   0 9   4 1   6 11   3 4   8 8   7 6   6 11   8 8   2 4   5 1   6 4   7 1   3 4   4 9
Samp_6     1    1          3 11   11 10   6 3   6 0   5 8   9 6   9 11   11 9   2 8   1 4   6 0   8 11   7 6   9 2   2 7   5 2   11 4   11 3
Samp_7     1    1          10 9   4 1   4 9   6 10   10 11   4 10   2 9   2 9   1 8   6 0   4 6   11 1   2 4   1 11   11 11   1 5   4 5   9 4
Samp_8     1    1          1 4   4 11   1 11   4 5   8 7   11 1   3 10   5 4   0 8   1 8   10 6   11 11   4 4   2 4   5 6   3 9   11 8   9 7
Samp_9     1    1          5 8   11 10   8 8   5 4   0 6   11 8   3 3   4 9   9 5   3 9   0 11   4 4   3 7   1 2   1 7   1 2   11 5   7 2
Samp_10     1    1          1 2   10 7   9 0   5 4   4 6   3 7   3 6   9 2   9 2   8 6   3 5   1 3   6 6   4 1   0 11   5 5   1 3   9 9
Samp_11     1    1          4 3   11 10   0 3   0 10   7 4   11 10   3 11   4 7   5 9   4 0   5 1   11 5   8 8   5 4   8 11   1 1   1 6   4 11
Samp_12     1    1          2 8   3 11   8 4   9 0   3 4   4 4   2 11   8 3   0 9   11 1   11 4   7 4   6 0   11 5   5 0   1 1   2 6   11 11
Samp_13     1    1          5 9   8 8   0 6   4 0   6 1   8 4   6 2   3 9   9 2   7 0   6 4   8 4   0 6   2 11   11 11   1 10   4 2   9 3
Samp_14     1    1          10 4   10 8   0 1   6 5   4 8   7 5   9 11   4 10   5 10   6 0   6 4   7 8   8 11   11 7   8 5   7 10   6 9   5 9
Samp_15     1    1          8 10   8 11   9 6   4 10   5 3   11 11   2 5   4 4   11 4   2 0   7 4   4 4   2 3   7 2   6 11   8 1   2 11   7 4
Samp_16     1    1          3 2   5 8   9 3   5 0   1 2   1 0   2 11   6 3   8 3   0 6   1 11   7 1   10 3   1 9   6 11   0 1   4 11   11 3
Samp_17     1    1          8 9   2 2   6 7   6 7   9 0   4 4   1 3   4 11   10 8   9 6   6 2   11 7   9 4   1 8   11 11   11 7   5 8   3 10
Samp_18     1    1          11 5   2 3   10 2   1 4   6 10   10 11   3 9   11 3   3 10   2 1   4 1   7 1   6 11   4 5   5 11   1 5   4 3   4 10
Samp_19     1    1          11 2   8 2   3 0   0 5   4 8   3 10   11 9   2 7   1 2   6 3   8 5   1 4   0 4   3 4   5 11   7 1   7 10   3 9
Samp_20     1    1          10 3   9 7   6 2   2 0   0 7   4 5   9 11   4 11   0 10   3 2   0 4   8 7   9 0   9 0   11 11   1 2   11 11   11 11
Samp_21     1    1          9 9   11 8   3 3   6 4   1 6   10 3   6 6   2 7   0 8   8 2   4 7   11 11   1 4   2 2   11 11   3 1   3 2   4 4
Samp_22     1    1          3 11   11 0   11 6   5 11   11 7   4 8   2 4   7 3   10 5   9 7   4 8   7 3   2 5   5 11   11 2   1 5   11 11   9 11
Samp_23     1    1          9 10   3 11   0 6   5 4   5 9   11 0   4 8   2 4   11 3   1 0   9 11   7 3   8 11   1 5   8 7   10 10   5 3   3 0
Samp_24     1    1          6 8   8 2   0 8   5 6   2 5   8 4   3 11   10 8   2 0   0 2   9 4   7 1   0 8   1 2   6 5   1 11   6 6   5 7
Samp_25     1    1          10 8   8 9   6 4   9 7   9 1   4 6   4 11   3 5   2 2   10 8   8 0   7 7   6 6   3 5   7 11   4 2   11 5   11 4
Samp_26     1    1          3 7   11 8   0 3   5 0   11 8   8 3   1 6   1 3   9 0   10 2   11 1   11 6   6 4   5 4   11 11   1 5   0 11   2 9
Samp_27     1    1          3 3   2 8   4 6   7 5   6 6   5 8   1 6   2 4   2 3   1 0   11 11   7 3   2 2   2 5   11 6   4 4   5 0   3 4
Samp_28     1    1          4 6   8 11   2 4   0 5   6 7   3 8   2 7   7 9   4 10   4 11   11 9   6 11   3 6   8 2   6 11   1 3   5 3   3 4
Samp_29     1    1          2 9   11 7   1 3   6 0   6 8   11 7   10 3   6 3   2 2   6 7   0 1   11 6   2 0   11 5   11 8   4 1   0 7   3 10
Samp_30     1    1          7 0   2 3   8 3   0 7   11 11   11 4   7 2   8 4   8 2   8 2   3 4   7 7   6 6   3 2   7 6   5 3   5 1   7 6
Samp_31     1    1          4 7   11 3   2 3   7 3   8 0   11 11   3 6   8 0   11 2   1 6   3 11   4 10   2 6   3 4   11 7   0 1   5 10   3 3
Samp_32     1    1          5 7   1 1   8 5   5 5   7 6   4 6   11 9   2 4   7 11   0 6   5 4   7 11   2 11   6 2   0 4   9 0   11 8   9 5
Samp_33     1    1          9 1   1 2   6 6   4 0   7 8   11 11   9 0   6 11   8 5   8 6   11 1   11 5   3 11   0 11   11 5   2 5   10 5   7 11
Samp_34     1    1          10 8   11 11   6 4   9 10   3 5   11 8   2 10   2 3   2 8   4 1   6 0   1 1   6 11   4 0   8 11   7 7   11 1   11 4
Samp_35     1    1          5 9   6 7   2 2   7 4   4 8   7 11   4 4   8 11   8 2   7 6   3 11   7 11   6 2   4 5   8 5   5 2   9 8   10 11
Samp_36     1    1          3 9   7 7   11 6   1 2   8 6   9 11   3 5   9 11   1 5   6 2   6 10   7 7   5 5   5 5   11 11   7 10   11 6   11 10
Samp_37     1    1          3 4   7 3   9 9   0 0   2 5   5 6   1 9   9 4   0 11   6 2   1 9   1 4   11 7   1 2   4 11   5 5   11 11   4 3
Samp_38     1    1          10 8   2 1   4 9   0 6   4 6   4 10   9 1   2 4   11 0   0 1   1 6   1 4   9 3   4 8   11 1   10 1   11 1   3 10
Samp_39     1    1          4 5   2 3   3 4   4 7   5 3   11 11   1 1   4 2   11 2   8 7   11 4   8 11   2 2   4 2   5 11   1 7   9 6   4 7
Samp_40     1    1          4 7   0 1   6 0   4 5   2 10   9 5   6 3   2 4   5 2   2 0   3 1   6 11   3 4   11 0   8 2   5 1   6 1   0 7
Samp_41     1    1          4 11   7 8   0 2   7 6   0 7   5 1   2 4   4 7   8 2   0 10   1 6   1 10   9 5   11 8   0 11   11 7   11 6   4 9
Samp_42     1    1          2 6   8 3   6 9   4 0   11 11   7 2   6 11   2 8   9 0   6 0   3 6   7 4   4 6   1 0   7 5   5 1   11 11   9 4
Samp_43     1    1          6 9   8 4   0 11   0 0   0 9   1 8   9 11   9 4   11 11   7 6   10 10   1 8   5 2   4 7   5 11   2 9   5 10   3 4
Samp_44     1    1          2 8   10 7   9 2   4 0   11 6   11 6   3 2   5 8   8 0   2 5   3 11   4 11   2 10   2 5   0 7   1 7   0 5   9 9
Samp_45     1    1          5 7   1 7   3 0   9 0   0 7   5 2   7 6   3 1   2 0   11 1   4 9   3 11   6 11   8 2   11 2   1 1   11 7   10 4
Samp_46     1    1          3 2   8 11   8 8   5 6   1 6   11 8   1 6   11 5   0 10   8 8   6 3   3 11   3 7   2 0   8 5   9 3   7 8   11 2
Samp_47     1    1          11 11   7 0   9 0   7 6   11 11   7 4   9 3   2 6   11 10   6 0   6 6   7 1   2 1   0 2   0 2   1 10   8 4   11 3
Samp_48     1    1          10 10   3 11   0 1   4 0   6 5   8 11   3 11   4 4   0 5   6 11   5 1   0 4   4 11   8 0   11 11   5 7   3 2   6 7
Samp_49     1    1          0 8   11 7   3 8   7 1   11 9   7 4   1 6   3 4   2 8   11 8   0 8   7 0   4 5   2 8   7 0   10 2   0 6   11 10
Samp_50     1    1          4 9   9 6   3 3   10 4   0 3   3 0   6 11   1 2   5 7   3 2   1 8   11 4   11 6   5 1   8 5   1 9   3 11   4 9
Samp_51     2    2          4 5   8 3   2 2   0 6   6 4   9 7   7 6   4 11   2 0   3 7   11 11   8 1   8 7   2 11   11 8   1 9   4 7   4 3
Samp_52     2    2          10 10   8 9   7 10   9 6   11 11   10 7   10 11   9 4   1 10   2 2   7 6   10 7   11 2   0 10   8 11   0 9   11 10   5 4
Samp_53     2    2          4 9   2 8   3 5   0 6   1 5   8 11   3 4   10 1   6 9   2 5   8 10   1 8   6 0   5 5   7 2   7 7   11 6   9 11
Samp_54     2    2          11 3   3 8   6 8   7 0   7 4   8 7   9 9   11 5   2 2   3 0   1 9   3 7   2 6   5 11   4 11   1 9   0 2   9 3
Samp_55     2    2          2 9   9 7   4 0   7 6   4 11   7 3   11 9   2 4   0 5   1 7   9 4   3 8   3 2   1 1   11 4   5 3   5 5   3 5
Samp_56     2    2          2 8   8 9   3 3   1 6   11 4   4 7   5 1   10 6   5 2   0 6   0 11   1 7   4 10   2 2   5 5   1 9   9 5   2 11
Samp_57     2    2          9 3   6 7   4 3   6 5   7 2   4 6   10 8   9 8   11 5   1 7   11 9   11 8   4 4   4 0   5 11   5 3   10 10   10 10
Samp_58     2    2          10 5   2 10   2 7   4 9   11 8   2 6   4 2   9 6   11 2   8 6   0 11   11 5   6 11   2 8   11 0   5 1   6 0   3 9
Samp_59     2    2          3 3   1 11   9 7   1 5   0 6   10 10   4 3   4 5   5 2   0 7   11 1   7 10   6 5   7 4   11 5   1 1   7 9   10 3
Samp_60     2    2          11 2   4 8   10 6   5 0   11 11   5 10   11 8   4 4   0 3   2 8   1 10   8 5   2 11   1 0   11 7   7 0   11 9   10 9
Samp_61     2    2          10 11   1 10   7 9   4 9   9 7   11 11   11 7   4 10   11 5   1 7   11 6   3 10   8 11   5 1   7 3   4 11   6 2   4 3
Samp_62     2    2          7 5   11 1   0 6   5 7   11 6   11 11   2 9   11 2   11 11   11 9   6 1   4 7   11 3   2 4   7 11   2 1   6 11   4 11
Samp_63     2    2          3 2   5 7   0 8   5 9   6 10   4 10   2 3   5 5   5 8   6 0   8 10   3 1   0 5   0 11   11 6   4 2   6 8   4 10
Samp_64     2    2          4 2   4 4   3 0   7 10   6 11   5 1   10 2   2 4   8 5   9 8   6 3   11 8   7 10   8 6   5 7   4 1   4 0   11 5
Samp_65     2    2          10 5   11 9   8 3   0 4   3 4   1 10   6 7   11 2   0 5   1 6   7 5   4 3   2 6   1 1   0 0   1 5   10 4   10 9
Samp_66     2    2          10 9   4 11   6 8   4 7   11 11   7 1   3 9   3 4   10 8   2 3   10 7   1 5   7 7   1 11   11 0   3 3   3 3   1 9
Samp_67     2    2          5 8   8 11   2 9   6 7   8 9   5 5   2 9   8 4   0 10   9 0   4 8   5 11   11 11   0 11   11 5   9 3   9 6   11 7
Samp_68     2    2          9 5   8 0   1 6   6 7   8 11   2 10   8 9   3 5   1 2   3 0   10 6   4 11   0 7   3 5   11 1   2 1   6 6   3 11
Samp_69     2    2          8 4   6 9   1 3   4 9   7 11   0 11   1 9   1 8   2 9   2 4   11 1   5 11   5 2   2 0   11 2   3 5   11 2   4 11
Samp_70     2    2          9 5   11 1   5 5   1 9   6 7   5 4   9 9   11 5   2 10   7 7   4 1   4 5   6 4   2 0   5 11   9 5   6 5   9 7
Samp_71     2    2          3 9   8 0   9 2   6 4   6 4   5 10   10 11   1 3   5 3   3 8   6 10   11 7   2 3   5 2   7 7   2 4   5 9   3 5
Samp_72     2    2          7 10   6 2   4 5   6 9   5 4   10 3   3 6   1 7   11 0   11 6   0 3   8 7   4 2   5 0   0 7   7 3   1 4   8 9
Samp_73     2    2          2 4   0 11   2 1   6 0   9 3   5 5   6 11   4 11   8 0   7 9   11 0   4 1   6 2   6 5   11 11   2 4   3 5   2 11
Samp_74     2    2          1 7   11 0   6 10   4 4   3 6   0 6   9 10   4 4   9 8   3 1   9 1   11 7   11 11   11 1   9 2   9 3   6 6   11 1
Samp_75     2    2          7 5   8 1   1 3   2 1   11 10   1 0   6 3   4 1   2 8   6 6   6 9   11 3   6 6   0 11   11 0   1 3   11 11   10 11
Samp_76     2    2          10 3   11 1   0 9   9 6   6 11   6 9   7 6   7 4   2 11   0 1   8 1   5 11   10 6   8 0   11 8   5 2   0 11   1 4
Samp_77     2    2          10 7   3 9   4 4   7 4   0 3   7 8   2 2   11 3   5 2   2 9   6 6   3 8   2 10   5 4   4 4   3 9   9 2   3 9
Samp_78     2    2          7 5   11 11   6 10   5 11   6 9   10 8   8 6   5 1   0 1   2 0   10 5   4 11   6 4   5 5   7 1   9 0   8 7   3 9
Samp_79     2    2          5 3   8 8   3 1   11 5   11 6   4 5   9 6   3 6   0 2   1 0   2 1   3 4   4 2   2 4   2 5   1 5   5 6   3 10
Samp_80     2    2          11 9   11 4   8 6   0 8   1 6   4 0   4 9   4 8   0 9   3 8   6 11   4 7   8 2   2 1   5 4   8 3   9 11   11 10
Samp_81     2    2          6 9   11 8   3 1   4 5   7 0   7 10   1 0   10 3   6 2   5 8   4 5   7 9   11 2   0 3   11 4   1 9   6 10   4 11
Samp_82     2    2          7 6   1 4   5 9   4 10   8 9   8 4   3 3   3 9   10 2   9 3   5 3   5 7   4 11   4 4   5 8   7 3   6 10   3 0
Samp_83     2    2          5 8   2 3   6 3   6 4   9 3   6 8   6 5   11 8   7 3   2 3   4 4   5 7   2 11   0 6   11 11   2 4   11 10   9 7
Samp_84     2    2          2 0   8 8   6 9   9 4   8 6   11 4   3 3   7 3   10 8   3 8   10 11   10 11   11 8   2 3   5 0   0 4   4 3   4 3
Samp_85     2    2          3 1   8 2   2 6   7 9   8 11   0 6   2 6   8 4   11 4   1 8   4 9   7 11   5 6   0 6   11 11   11 5   9 4   0 11
Samp_86     2    2          5 11   9 2   6 2   7 0   0 7   4 2   4 9   2 4   5 4   7 0   8 2   1 5   11 3   8 11   0 8   9 0   2 4   4 9
Samp_87     2    2          2 5   8 11   10 2   5 3   6 11   5 6   9 11   4 7   3 5   0 4   6 11   11 11   4 6   11 2   1 8   4 1   11 3   4 2
Samp_88     2    2          1 7   0 7   2 2   4 8   6 4   6 6   11 4   9 3   10 2   3 7   4 8   5 10   1 2   1 4   1 11   5 3   5 5   10 0
Samp_89     2    2          9 1   3 8   2 9   6 9   11 2   4 6   9 11   2 3   11 5   6 1   10 3   7 11   3 2   2 5   8 11   0 1   3 5   2 11
Samp_90     2    2          10 9   11 10   6 3   0 4   7 6   5 6   2 9   4 3   3 4   11 11   1 0   1 11   3 5   1 2   11 4   9 1   6 6   7 8
Samp_91     2    2          1 11   10 11   6 4   6 4   11 6   7 1   9 10   2 7   0 8   6 8   10 6   7 5   3 2   8 8   4 11   1 5   8 4   10 3
Samp_92     2    2          1 9   8 0   3 8   5 9   9 0   8 4   6 9   7 9   4 5   3 9   9 9   11 8   2 6   1 2   11 8   7 11   2 6   3 4
Samp_93     2    2          1 10   3 2   1 1   4 6   2 9   7 8   11 3   0 2   0 9   4 7   4 4   4 1   2 6   1 2   0 9   0 9   10 4   3 3
Samp_94     2    2          5 9   9 11   6 2   4 7   4 11   7 10   9 9   3 4   10 5   5 9   10 9   3 7   6 10   1 9   5 11   9 10   9 11   9 10
Samp_95     2    2          5 8   0 8   2 1   7 10   6 8   8 10   9 8   6 10   2 8   7 0   4 11   7 4   1 7   2 0   11 11   1 10   6 7   10 9
Samp_96     2    2          8 11   3 9   8 6   0 6   6 6   1 4   11 5   4 3   2 0   0 7   1 0   1 3   2 2   2 4   4 5   10 3   11 10   8 7
Samp_97     2    2          2 9   8 7   9 1   0 9   11 11   8 8   6 3   7 0   8 5   1 6   9 0   3 7   0 3   1 9   11 11   1 2   11 7   0 10
Samp_98     2    2          2 11   6 4   6 3   2 9   9 11   7 4   11 11   5 2   0 8   1 6   8 10   11 7   0 7   6 8   0 5   1 1   5 11   4 10
Samp_99     2    2          8 8   0 6   5 6   5 0   6 4   7 7   1 1   6 1   0 3   4 6   10 4   7 3   7 5   6 1   0 11   1 1   10 7   3 9
Samp_100     2    2          10 4   4 1   10 3   11 5   4 11   4 3   4 6   10 10   5 8   6 1   6 4   2 5   9 6   1 4   7 11   9 11   8 9   1 1
Samp_101     3    3          9 4   8 1   7 8   7 4   3 2   11 1   6 7   6 1   9 10   6 0   4 4   4 10   0 0   9 5   7 3   4 7   11 1   7 3
Samp_102     3    3          6 5   8 11   7 9   0 0   9 5   11 4   0 11   9 4   5 10   6 6   0 3   11 11   4 9   0 0   0 5   1 2   8 11   5 11
Samp_103     3    3          9 4   8 11   2 4   9 5   8 4   7 10   3 3   11 9   11 0   2 0   0 9   4 1   8 2   0 7   7 0   1 3   9 2   5 3
Samp_104     3    3          5 9   7 2   2 6   0 9   2 8   5 4   11 11   11 4   4 5   6 6   11 1   8 8   1 6   11 2   3 3   10 4   6 11   4 4
Samp_105     3    3          5 11   11 2   0 4   2 3   7 6   8 1   1 10   2 5   8 10   0 6   3 11   11 11   2 7   8 5   0 11   7 1   8 2   7 11
Samp_106     3    3          9 9   11 3   9 4   10 2   8 2   6 11   11 11   9 4   0 8   2 6   2 0   10 10   6 3   2 9   8 11   9 7   9 0   9 9
Samp_107     3    3          2 3   2 4   8 9   5 0   1 8   10 7   2 2   4 5   10 0   6 1   4 6   11 4   2 7   5 6   11 3   9 4   6 6   3 3
Samp_108     3    3          5 2   2 11   8 6   6 0   4 10   4 5   6 6   2 2   11 10   5 6   9 6   7 7   6 2   5 4   7 11   1 8   1 11   9 1
Samp_109     3    3          5 7   6 0   6 3   4 0   0 2   11 8   4 3   1 9   2 5   7 0   1 2   11 10   6 4   4 0   11 11   1 7   10 7   3 3
Samp_110     3    3          9 4   2 11   3 2   8 5   0 11   10 7   11 9   8 10   8 5   3 10   4 6   11 4   11 7   9 4   4 8   3 1   10 5   3 3
Samp_111     3    3          11 3   2 1   4 6   2 0   9 6   10 5   9 6   8 3   8 1   4 6   9 6   4 10   2 11   7 1   11 11   5 0   11 0   0 11
Samp_112     3    3          9 7   4 3   8 2   0 2   10 3   7 8   8 6   2 1   10 2   0 1   11 9   10 5   5 5   8 1   11 1   9 4   5 8   4 3
Samp_113     3    3          9 11   11 4   2 3   9 5   11 8   4 11   6 11   4 8   2 5   5 3   10 1   11 0   2 2   3 6   5 4   3 1   4 7   3 4
Samp_114     3    3          4 10   0 6   4 6   0 10   6 3   7 11   11 9   9 10   8 5   7 7   10 10   7 7   5 4   1 7   5 11   5 5   11 3   3 11
Samp_115     3    3          10 11   0 11   11 0   10 0   11 11   4 10   0 9   7 5   0 9   0 0   8 4   11 1   0 11   2 4   11 10   4 3   10 2   9 4
Samp_116     3    3          5 10   1 11   7 1   4 5   11 1   6 11   4 10   9 2   5 5   8 1   1 4   11 7   6 11   11 2   11 5   7 5   0 9   11 4
Samp_117     3    3          4 2   1 3   9 6   0 4   9 6   7 3   3 9   5 4   10 11   8 1   4 8   4 1   3 4   2 11   0 5   3 1   1 2   7 7
Samp_118     3    3          2 3   4 8   4 7   0 5   0 1   9 11   4 11   2 4   8 8   11 3   0 0   8 11   2 0   1 8   7 0   7 5   1 5   10 11
Samp_119     3    3          7 7   9 7   3 3   5 8   0 2   7 1   11 0   11 5   9 11   4 2   11 11   4 7   11 3   7 6   8 10   10 7   11 5   11 3
Samp_120     3    3          4 5   11 8   0 8   7 0   11 0   11 4   11 9   4 4   5 2   2 3   11 8   10 4   11 9   11 1   2 7   11 11   5 6   6 7
Samp_121     3    3          2 8   2 6   4 0   7 6   0 11   4 7   1 7   6 7   5 0   2 0   8 5   8 11   2 6   7 4   11 11   7 1   5 5   11 7
Samp_122     3    3          4 3   11 0   6 8   0 9   8 8   4 1   7 11   2 10   5 2   7 10   5 0   3 4   11 3   0 1   11 11   7 0   10 8   2 2
Samp_123     3    3          11 7   11 10   3 6   7 5   5 3   4 8   3 6   9 3   2 10   0 1   1 11   11 11   5 6   2 1   8 6   10 4   5 6   3 4
Samp_124     3    3          8 3   8 11   6 10   0 4   11 0   0 2   2 6   2 4   2 2   8 1   4 1   4 11   11 3   4 0   5 5   7 3   5 6   4 6
Samp_125     3    3          5 10   8 11   2 3   7 6   11 2   8 5   4 6   9 7   9 8   6 3   2 9   2 1   6 7   4 2   11 7   0 0   10 5   11 11
Samp_126     3    3          11 2   8 0   9 0   7 0   7 7   11 4   3 6   9 8   9 10   1 6   8 6   7 5   7 6   9 8   6 11   1 2   11 5   3 5
Samp_127     3    3          5 6   8 3   8 0   7 5   11 11   8 7   5 7   2 1   2 11   6 6   11 10   3 8   2 0   2 6   8 10   4 9   11 5   10 10
Samp_128     3    3          10 5   1 1   2 3   7 3   6 0   7 11   9 3   2 2   8 9   8 0   8 9   2 11   11 6   1 5   7 11   6 4   6 5   7 9
Samp_129     3    3          9 10   2 5   2 6   0 10   11 0   7 11   5 9   7 11   5 8   6 0   9 9   11 8   2 2   3 11   9 11   10 9   9 11   4 3
Samp_130     3    3          10 9   8 8   4 0   10 9   6 11   5 7   0 6   9 3   5 2   11 1   6 8   4 7   2 3   11 2   4 9   10 9   6 7   5 9
Samp_131     3    3          11 8   10 10   1 2   5 5   6 8   4 3   11 9   11 5   5 11   8 2   4 4   5 11   7 2   8 11   11 4   0 1   10 6   9 9
Samp_132     3    3          1 3   7 2   6 3   6 0   6 4   4 4   11 10   3 5   5 2   7 1   9 11   7 4   7 11   6 5   11 11   1 4   6 0   11 10
Samp_133     3    3          8 1   2 8   1 2   5 3   11 6   7 4   4 11   2 1   9 5   2 6   6 11   6 7   2 9   11 11   0 11   5 9   2 9   4 1
Samp_134     3    3          10 3   11 3   4 2   5 2   6 4   4 10   9 8   7 1   5 2   2 8   11 10   2 7   3 11   2 0   4 3   2 4   1 11   11 9
Samp_135     3    3          4 11   2 11   6 6   6 8   11 11   10 1   3 7   2 5   10 3   8 0   4 9   11 3   4 7   2 5   11 0   1 5   5 4   11 3
Samp_136     3    3          9 11   2 3   0 3   6 0   11 6   4 10   3 3   7 2   2 10   6 2   6 11   3 1   3 3   4 1   7 4   3 5   11 11   9 4
Samp_137     3    3          7 2   2 2   2 9   0 4   1 8   7 4   4 11   2 2   5 0   3 1   9 11   10 9   5 2   5 11   11 11   0 1   6 5   4 4
Samp_138     3    3          2 10   6 0   8 3   0 4   11 6   10 8   9 9   1 3   10 11   3 7   5 3   11 1   0 11   1 4   11 7   9 1   0 11   7 7
Samp_139     3    3          9 11   10 1   6 5   6 6   11 11   11 11   3 8   5 4   8 10   0 9   11 11   11 5   2 4   5 11   0 8   11 9   5 5   11 5
Samp_140     3    3          7 7   3 8   6 4   4 5   6 1   2 8   7 3   1 8   0 2   11 3   5 11   11 11   8 2   1 4   11 11   1 5   5 4   11 11
Samp_141     3    3          9 11   3 2   5 3   4 4   7 6   7 8   11 6   4 5   10 2   6 4   0 3   7 7   11 2   1 1   7 11   1 2   5 5   3 9
Samp_142     3    3          9 1   4 9   1 8   9 5   9 0   4 0   1 9   7 2   0 11   6 8   8 8   10 7   6 7   11 9   11 11   9 4   4 11   3 3
Samp_143     3    3          4 9   9 9   6 1   0 0   6 6   8 8   1 11   0 1   5 0   9 7   8 11   8 4   2 6   5 4   11 11   7 1   0 5   10 9
Samp_144     3    3          6 1   4 10   2 8   9 0   5 1   4 10   3 11   2 7   1 10   3 6   7 10   10 8   7 8   4 8   11 11   9 9   10 5   9 1
Samp_145     3    3          10 3   3 10   0 3   0 0   3 3   8 5   9 3   11 4   9 5   6 3   11 4   1 11   2 11   2 2   8 0   1 1   11 9   11 4
Samp_146     3    3          3 3   6 8   3 4   2 10   4 3   3 10   11 11   9 11   8 8   2 0   8 8   5 4   8 11   5 5   8 3   7 1   4 3   10 4
Samp_147     3    3          3 2   7 0   7 1   0 9   2 9   10 5   11 9   7 3   5 5   9 2   10 6   7 4   11 5   2 3   7 11   5 4   6 2   9 1
Samp_148     3    3          3 9   11 1   10 8   2 7   11 0   10 4   0 5   3 10   7 2   0 0   1 4   11 2   5 5   2 9   11 11   9 1   11 11   5 11
Samp_149     3    3          4 3   8 3   4 10   4 0   6 4   4 2   2 9   4 3   11 3   2 1   6 4   1 5   11 7   5 0   5 11   1 1   2 6   9 9
Samp_150     3    3          7 2   10 7   6 2   0 0   6 6   8 2   11 1   7 10   8 2   3 6   10 9   11 1   10 4   1 4   0 4   7 1   6 6   3 3
```

Note that alleles are named starting at 0 and there is no missing data.  So, when you run this in structure, you should set the missing data indicator to -9 and all will be fine.  (i.e. don't set the missing data value to 0)

## Getting It Into Structure
Make a new structure project.  Then include `NoStructure.txt`.  Note that it has 150 individuals and 18 loci.  There is no row of locus names.  Tell structure that the file includes:
  -Individual ID column
  -Putative pop origin column
  -A sampling location info column
(The latter two columns are identical, we just want both in there since we will do two different types of analyses here.)

## A Prior Pop Info Run
Make a parameter set called PriorPop with these (or similar) attributes:
```
====================  Parameter Set: PriorPop  ====================     


                                    Running Length          

                     Length of Burnin Period: 5000
                     Number of MCMC Reps after Burnin: 15000


                                    Ancestry Model Info          

                   Use Prior Population Information to Assist Clustering           
                     * GENSBACK  = 2
                     * MIGRPRIOR = 0.01
                     * For Individuals without population information data, use    
                          Admixture Model            
                                ** Infer Alpha
                                ** Initial Value of ALPHA (Dirichlet Parameter for Degree of Admixture):  1.0
                                ** Use Same Alpha for all Populations
                                ** Use a Uniform Prior for Alpha
                                  *** Maximum Value for Alpha: 10.0
                                  *** SD of Proposal for Updating Alpha: 0.025


                                    Frequency Model Info   

                     Allele Frequencies are Correlated among Pops
                       * Assume Different Values of Fst for Different Subpopulations
                       * Prior Mean of Fst for Pops: 0.01
                       * Prior SD   of Fst for Pops: 0.05
                       * Use Constant Lambda (Allele Frequencies Parameter) 
                       * Value of Lambda: 1.0


                                    Advanced Options   

                      Estimate the Probability of the Data Under the Model 
                      Frequency of Metropolis update for Q: 10

```
Then run that dude, setting the assumed number of subpopulations to K=3.  When it is done, make a bar plot and understand why this result does not actually indicate that you have found an enormous amount of population structure.

## Some Runs with the LOCPRIOR model
The recently released method of Hubisz et al. 2009 is now a more appropriate and available method for
incorporating sampling location into a structure analysis.  In the case of the admixture analysis, each
sampling location has its own set of alpha parameters which help to determine (in a sense they are the
"prior" parameters for) the Q vectors you expect to see in each sampling locale.

Do a series of runs at K=1, K=2, and K=3 using the Location prior model in structure and see if using
that model encourages structure to spuriously identify population structure.

