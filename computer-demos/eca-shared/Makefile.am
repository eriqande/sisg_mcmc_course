ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

# with all the lame versions of eca_opt running around, I will just make separate
# taget archives for each one!
# targets
noinst_LIBRARIES = libecashared.a libgfmcmc.a libecaopt.a libecaopt2.a libecaopt2j5.a libecaopt3.a 


# here are the headers, which we don't install, but need in the distro
noinst_HEADERS = include/ECA_MemAlloc.h \
	 include/ECA_Opt.h \
	 include/ECA_Opt2.h \
	 include/ECA_Opt2_July5_2007.h \
	 include/ECA_Opt3.h \
	 include/ECA_ReadData.h \
	 include/ECA_print.h \
	 include/ECA_utilities.h \
	 include/GFMCMC.h \
	 include/GFMCMC_DrawingUtilities.h \
	 include/GFMCMC_Shortcuts.h \
	 include/MCTypesEtc.h \
	 include/MathStatRand.h \
	 include/ranlib.h

# sources for each target
libecashared_a_SOURCES = src/ECA_MemAlloc.c \
	 src/ECA_ReadData.c \
	 src/ECA_print.c \
	 src/ECA_utilities.c \
	 src/MCTypesEtc.c \
	 src/MathStatRand.c \
	 src/com.c \
	 src/linpack.c \
	 src/ranlib.c

libgfmcmc_a_SOURCES = src/GFMCMC.c src/GFMCMC_DrawingUtilities.c

libecaopt_a_SOURCES = src/ECA_Opt.c

libecaopt2_a_SOURCES = src/ECA_Opt2.c

libecaopt2j5_a_SOURCES = src/ECA_Opt2_July5_2007.c

libecaopt3_a_SOURCES = src/ECA_Opt3.c



# cflags for each target
libecashared_a_CFLAGS =  -Iinclude 

# apple mavericks complains about GLUT stuff.  Turn it off with -Wno-deprecated 
# (of course we will miss other things we might want to hear about...)
libgfmcmc_a_CFLAGS = $(GL_CFLAGS) $(GLU_CFLAGS) $(GLUT_CFLAGS) -Iinclude -Wno-deprecated

libecaopt_a_CFLAGS = -Iinclude

libecaopt2_a_CFLAGS = -Iinclude

libecaopt2j5_a_CFLAGS = -Iinclude

libecaopt3_a_CFLAGS = -Iinclude



